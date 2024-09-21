/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "video_processing_native.h"

#include <chrono>
#include <cinttypes>

#include "common/native_mfmagic.h"
#include "native_window.h"

#include "vpe_log.h"
#include "video_processing_callback_impl.h"

using namespace OHOS;
using namespace OHOS::Media::VideoProcessingEngine;

namespace {
constexpr uint32_t BUFFER_QUEUE_SIZE = 5;
constexpr int LIMITTED_TIME = 100;
const std::unordered_map<AlgoErrorCode, VideoProcessing_ErrorCode> ERROR_MAP = {
    { ALGO_SUCCESS,                         VIDEO_PROCESSING_SUCCESS },
    { ALGO_ERROR_INVALID_PARAMETER,         VIDEO_PROCESSING_ERROR_INVALID_PARAMETER },
    { ALGO_ERROR_UNKNOWN,                   VIDEO_PROCESSING_ERROR_UNKNOWN },
    { ALGO_ERROR_INITIALIZE_FAILED,         VIDEO_PROCESSING_ERROR_INITIALIZE_FAILED },
    { ALGO_ERROR_CREATE_FAILED,             VIDEO_PROCESSING_ERROR_CREATE_FAILED },
    { ALGO_ERROR_PROCESS_FAILED,            VIDEO_PROCESSING_ERROR_PROCESS_FAILED },
    { ALGO_ERROR_UNSUPPORTED_PROCESSING,    VIDEO_PROCESSING_ERROR_UNSUPPORTED_PROCESSING },
    { ALGO_ERROR_OPERATION_NOT_PERMITTED,   VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED },
    { ALGO_ERROR_NO_MEMORY,                 VIDEO_PROCESSING_ERROR_NO_MEMORY },
    { ALGO_ERROR_INVALID_INSTANCE,          VIDEO_PROCESSING_ERROR_INVALID_INSTANCE },
    { ALGO_ERROR_INVALID_VALUE,             VIDEO_PROCESSING_ERROR_INVALID_VALUE },
};
}

VideoProcessing_ErrorCode VideoProcessingNative::Initialize()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (isInitialized_) {
        VPE_LOGE("Already initialize!");
        return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
    }
    worker_ = std::thread([this]() {
        while (!isExit_.load()) {
            {
                std::unique_lock<std::mutex> lock(bufferLock_);
                if (!WaitCV([this] {
                    return isExit_.load() || (!producerBufferQueue_.empty() && !consumerBufferQueue_.empty());
                    }, lock)) {
                    CheckStopping();
                    continue;
                }

                if (isExit_.load()) {
                    VPE_LOGI("Video processing destroy.");
                    break;
                }
                if (producerBufferQueue_.empty() || consumerBufferQueue_.empty()) {
                    VPE_LOGD("Video processing spurious wakeup.");
                    continue;
                }
            }
            ProcessBuffers();
            CheckStopping();
        };
    });
    auto errorCode = InitializeInner();
    isInitialized_ = true;
    return errorCode;
}

VideoProcessing_ErrorCode VideoProcessingNative::Deinitialize()
{
    VideoProcessing_ErrorCode errorCode;
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (!isInitialized_) {
            VPE_LOGE("Already deinitialize!");
            return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
        }
        isInitialized_ = false;
        errorCode = DeinitializeInner();
        if (state_.load() == VPEState::RUNNING) {
            state_ = VPEState::STOPPING;
        }
        isExit_ = true;
    }
    cv_.notify_one();
    if (worker_.joinable()) {
        worker_.join();
    }
    CheckStopping();
    return errorCode;
}

VideoProcessing_ErrorCode VideoProcessingNative::RegisterCallback(const VideoProcessing_Callback* callback,
    void* userData)
{
    if (callback == nullptr || callback->GetObj() == nullptr || !callback->GetObj()->IsValid()) {
        VPE_LOGE("callback is null or invalid!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    return ExecuteWhenIdle([this, callback, userData]() {
        callback_ = callback->GetObj();
        userData_ = userData;
        return VIDEO_PROCESSING_SUCCESS;
        }, "Registration of callbacks during running is not allowed!");
}

VideoProcessing_ErrorCode VideoProcessingNative::SetSurface(const OHNativeWindow* window)
{
    if (window == nullptr || window->surface == nullptr || window->surface->IsConsumer() ||
        !IsProducerSurfaceValid(*window)) {
        VPE_LOGE("window is null or invalid!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    std::lock_guard<std::mutex> lock(lock_);
    auto surface = window->surface;
    GSError errorCode = surface->RegisterReleaseListener([this]([[maybe_unused]] sptr<SurfaceBuffer> &buffer) {
        return OnProducerBufferReleased();
    });
    if (errorCode != GSERROR_OK) {
        VPE_LOGE("window is null or invalid!");
        return VIDEO_PROCESSING_ERROR_PROCESS_FAILED;
    }
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);

    producer_ = surface;
    return SetProducerSurface(*window, requestCfg_);
}

VideoProcessing_ErrorCode VideoProcessingNative::GetSurface(OHNativeWindow** window)
{
    if (window == nullptr) {
        VPE_LOGE("window is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return ExecuteWhenIdle([this, window]() {
        if (consumer_ != nullptr) {
            VPE_LOGE("consumer surface is already created!");
            return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
        }

        consumer_ = CreateConsumerSurface(*window);
        if (consumer_ == nullptr) {
            VPE_LOGE("Failed to create consumer surface!");
            return VIDEO_PROCESSING_ERROR_CREATE_FAILED;
        }
        return VIDEO_PROCESSING_SUCCESS;
        }, "Getting a surface during running is not allowed!");
}

VideoProcessing_ErrorCode VideoProcessingNative::SetParameter(const OH_AVFormat* parameter)
{
    if (parameter == nullptr) {
        VPE_LOGE("parameter is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return SetParameter(parameter->format_);
}

VideoProcessing_ErrorCode VideoProcessingNative::GetParameter(OH_AVFormat* parameter)
{
    if (parameter == nullptr) {
        VPE_LOGE("parameter is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return GetParameter(parameter->format_);
}

VideoProcessing_ErrorCode VideoProcessingNative::Start()
{
    auto errorCode = ExecuteWhenIdle([this]() {
        if (consumer_ == nullptr || producer_ == nullptr || callback_ == nullptr) {
            VPE_LOGE("The consumer, producer surfaces and callback are not ready!");
            return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
        }
        callback_->LockModifiers();
        state_ = VPEState::RUNNING;
        return VIDEO_PROCESSING_SUCCESS;
    }, "Already start!");
    if (errorCode == VIDEO_PROCESSING_SUCCESS) {
        OnState(VIDEO_PROCESSING_STATE_RUNNING);
    }
    return errorCode;
}

VideoProcessing_ErrorCode VideoProcessingNative::Stop()
{
    auto errorCode = ExecuteWhenRunning([this]() {
        callback_->UnlockModifiers();

        std::lock_guard<std::mutex> lock(bufferLock_);
        if (consumerBufferQueue_.empty()) {
            state_ = VPEState::IDLE;
        } else {
            state_ = VPEState::STOPPING;
        }
        return VIDEO_PROCESSING_SUCCESS;
    }, "Already stop!");

    if (state_.load() == VPEState::IDLE) {
        OnState(VIDEO_PROCESSING_STATE_STOPPED);
    } else {
        cv_.notify_one();
    }
    return errorCode;
}

VideoProcessing_ErrorCode VideoProcessingNative::RenderOutputBuffer(uint32_t index)
{
    if (isOnNewOutputBuffer_.load()) {
        return RenderOutputBufferInner(index);
    }
    return ExecuteWhenNotIdle(
        [this, index]() { return RenderOutputBufferInner(index); },
        "RenderOutputBuffer must be called during running!");
}

VideoProcessing_ErrorCode VideoProcessingNative::AlgoErrorToNdk(AlgoErrorCode errorCode)
{
    auto it = ERROR_MAP.find(errorCode);
    if (it == ERROR_MAP.end()) {
        VPE_LOGE("Invalid error code:%{public}d", errorCode);
        return VIDEO_PROCESSING_ERROR_UNKNOWN;
    }
    return it->second;
}

VideoProcessing_ErrorCode VideoProcessingNative::InitializeInner()
{
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode VideoProcessingNative::DeinitializeInner()
{
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode VideoProcessingNative::SetParameter([[maybe_unused]] const OHOS::Media::Format& parameter)
{
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode VideoProcessingNative::GetParameter([[maybe_unused]] OHOS::Media::Format& parameter)
{
    return VIDEO_PROCESSING_SUCCESS;
}

bool VideoProcessingNative::IsProducerSurfaceValid([[maybe_unused]] const OHNativeWindow& window)
{
    return true;
}

VideoProcessing_ErrorCode VideoProcessingNative::SetProducerSurface([[maybe_unused]] const OHNativeWindow& window,
    [[maybe_unused]] BufferRequestConfig& requestCfg)
{
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode VideoProcessingNative::Process([[maybe_unused]] const sptr<SurfaceBuffer>& sourceImage,
    [[maybe_unused]] sptr<SurfaceBuffer>& destinationImage)
{
    return VIDEO_PROCESSING_SUCCESS;
}

void VideoProcessingNative::UpdateRequestCfg([[maybe_unused]] const sptr<SurfaceBuffer>& consumerBuffer,
    [[maybe_unused]] BufferRequestConfig& requestCfg)
{
}

void VideoProcessingNative::OnError(VideoProcessing_ErrorCode errorCode)
{
    OnCallback([this, errorCode](std::shared_ptr<VideoProcessingCallbackNative>& callback, void* userData) {
        callback->OnError(context_, errorCode, userData);
    });
}

void VideoProcessingNative::OnState(VideoProcessing_State state)
{
    OnCallback([this, state](std::shared_ptr<VideoProcessingCallbackNative>& callback, void* userData) {
        callback->OnState(context_, state, userData);
    });
}

void VideoProcessingNative::OnNewOutputBuffer(uint32_t index)
{
    OnCallback([this, index](std::shared_ptr<VideoProcessingCallbackNative>& callback, void* userData) {
        if (state_.load() == VPEState::IDLE) {
            return;
        }
        if (callback->HasOnNewOutputBuffer()) {
            isOnNewOutputBuffer_ = true;
            callback->OnNewOutputBuffer(context_, index, userData);
            isOnNewOutputBuffer_ = false;
        } else {
            RenderOutputBufferInner(index);
        }
    });
}

GSError VideoProcessingNative::OnConsumerBufferAvailable()
{
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (state_.load() != VPEState::RUNNING) {
            return GSERROR_INVALID_OPERATING;
        }

        SurfaceBufferInfo bufferInfo{};
        int releaseFence = -1;
        OHOS::Rect damage;
        GSError errorCode = consumer_->AcquireBuffer(bufferInfo.buffer, releaseFence, bufferInfo.timestamp, damage);
        if (errorCode != GSERROR_OK || bufferInfo.buffer == nullptr) {
            VPE_LOGE("Failed to acquire buffer!");
            return errorCode;
        }

        std::lock_guard<std::mutex> buflock(bufferLock_);
        if (!isBufferQueueReady_) {
            isBufferQueueReady_ = true;
            requestCfg_.usage |= bufferInfo.buffer->GetUsage();
            requestCfg_.timeout = 0;
            requestCfg_.strideAlignment = 32; // 32 内存对齐
            UpdateRequestCfg(bufferInfo.buffer, requestCfg_);
            PrepareBuffers();
        }
        consumerBufferQueue_.push(bufferInfo);
    }
    cv_.notify_one();
    return GSERROR_OK;
}

GSError VideoProcessingNative::OnProducerBufferReleased()
{
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (producer_ == nullptr) {
            VPE_LOGW("Producer surface is null!");
            return GSERROR_OK;
        }

        std::lock_guard<std::mutex> buflock(bufferLock_);
        GSError errorCode = GSERROR_OK;
        if (!RequestBuffer(errorCode)) {
            VPE_LOGE("Failed to request buffer!");
            return errorCode;
        }
    }
    if (state_.load() != VPEState::IDLE) {
        cv_.notify_one();
    }
    return GSERROR_OK;
}

VideoProcessing_ErrorCode VideoProcessingNative::RenderOutputBufferInner(uint32_t index)
{
    std::unique_lock<std::mutex> lock(bufferLock_);
    for (uint32_t i = 0; i < RenderBufferQueue_.size(); i++) {
        SurfaceBufferInfo bufferInfo = RenderBufferQueue_.front();
        RenderBufferQueue_.pop();
        if (bufferInfo.buffer->GetSeqNum() == index) {
            BufferFlushConfig flushcfg {
                { 0, 0, bufferInfo.buffer->GetWidth(), bufferInfo.buffer->GetHeight() },
                bufferInfo.timestamp
            };
            producer_->FlushBuffer(bufferInfo.buffer, -1, flushcfg);
            return VIDEO_PROCESSING_SUCCESS;
        }
    }
    VPE_LOGE("Invalid input: index=%{public}u!", index);
    return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
}

sptr<Surface> VideoProcessingNative::CreateConsumerSurface(OHNativeWindow*& window)
{
    sptr<Surface> consumer = Surface::CreateSurfaceAsConsumer("VideoProcessingSurface");
    if (consumer == nullptr) {
        VPE_LOGE("Failed to create consumer surface!");
        return nullptr;
    }
    sptr<IBufferConsumerListener> listener = new(std::nothrow) ConsumerListener(shared_from_this());
    if (listener == nullptr) {
        VPE_LOGE("Failed to create consumer surface listener!");
        return nullptr;
    }
    if (consumer->RegisterConsumerListener(listener) != GSERROR_OK) {
        VPE_LOGE("Failed to register consumer surface listener!");
        return nullptr;
    }

    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    if (surface == nullptr) {
        VPE_LOGE("Failed to create producer surface!");
        return nullptr;
    }
    consumer->SetQueueSize(BUFFER_QUEUE_SIZE);

    window = CreateNativeWindowFromSurface(&surface);
    if (window == nullptr) {
        VPE_LOGE("Failed to create the window!");
        return nullptr;
    }
    return consumer;
}

bool VideoProcessingNative::RequestBuffer(GSError& errorCode)
{
    SurfaceBufferInfo bufferInfo{};
    int releaseFence = -1;
    errorCode = producer_->RequestBuffer(bufferInfo.buffer, releaseFence, requestCfg_);
    if (errorCode != GSERROR_OK || bufferInfo.buffer == nullptr) {
        return false;
    }
    producerBufferQueue_.push(bufferInfo);
    return true;
}

void VideoProcessingNative::PrepareBuffers()
{
    for (uint32_t i = 0; i < producer_->GetQueueSize(); i++) {
        GSError errorCode;
        RequestBuffer(errorCode);
    }
}

void VideoProcessingNative::ProcessBuffers()
{
    sptr<Surface> consumer;
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (consumer_ == nullptr) {
            return;
        }
        consumer = consumer_;
    }
    while (state_.load() != VPEState::IDLE) {
        SurfaceBufferInfo srcBufferInfo;
        SurfaceBufferInfo dstBufferInfo;
        {
            std::lock_guard<std::mutex> bufferLock(bufferLock_);
            if (producerBufferQueue_.empty() || consumerBufferQueue_.empty()) {
                break;
            }
            srcBufferInfo = consumerBufferQueue_.front();
            dstBufferInfo = producerBufferQueue_.front();
            consumerBufferQueue_.pop();
            producerBufferQueue_.pop();
        }
        dstBufferInfo.timestamp = srcBufferInfo.timestamp;
        auto errorCode = Process(srcBufferInfo.buffer, dstBufferInfo.buffer);
        if (errorCode != VIDEO_PROCESSING_SUCCESS) {
            OnError(errorCode);
        }
        consumer->ReleaseBuffer(srcBufferInfo.buffer, -1);
        {
            std::lock_guard<std::mutex> bufferLock(bufferLock_);
            RenderBufferQueue_.push(dstBufferInfo);
        }
        // If this function is not registered, the output buffer is sent out as soon as the buffer is filled with
        // processed data without reporting.
        OnNewOutputBuffer(dstBufferInfo.buffer->GetSeqNum());
    }
}

bool VideoProcessingNative::WaitCV(std::function<bool(void)>&& checker, std::unique_lock<std::mutex>& lock)
{
    if (state_.load() == VPEState::STOPPING) {
        if (!cv_.wait_for(lock, std::chrono::milliseconds(LIMITTED_TIME), checker)) {
            VPE_LOGI("Video processing timeout.");
            return false;
        }
    } else {
        cv_.wait(lock, checker);
    }
    return true;
}

void VideoProcessingNative::CheckStopping()
{
    bool isStopped = false;
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (state_.load() == VPEState::STOPPING) {
            state_ = VPEState::IDLE;
            isStopped = true;
        }
    }
    if (isStopped) {
        OnState(VIDEO_PROCESSING_STATE_STOPPED);
    }
}

VideoProcessing_ErrorCode VideoProcessingNative::ExecuteWhenIdle(
    std::function<VideoProcessing_ErrorCode(void)>&& operation, const std::string& errorMessage)
{
    return ExecuteWithCheck([this] { return state_.load() != VPEState::IDLE; }, std::move(operation), errorMessage);
}

VideoProcessing_ErrorCode VideoProcessingNative::ExecuteWhenNotIdle(
    std::function<VideoProcessing_ErrorCode(void)>&& operation, const std::string& errorMessage)
{
    return ExecuteWithCheck([this] { return state_.load() == VPEState::IDLE; }, std::move(operation), errorMessage);
}

VideoProcessing_ErrorCode VideoProcessingNative::ExecuteWhenRunning(
    std::function<VideoProcessing_ErrorCode(void)>&& operation, const std::string& errorMessage)
{
    return ExecuteWithCheck([this] { return state_.load() != VPEState::RUNNING; }, std::move(operation), errorMessage);
}

VideoProcessing_ErrorCode VideoProcessingNative::ExecuteWithCheck(std::function<bool(void)>&& checker,
    std::function<VideoProcessing_ErrorCode(void)>&& operation, const std::string& errorMessage)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (checker()) {
        VPE_LOGW("%{public}s", errorMessage.c_str());
        return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
    }
    return operation();
}

void VideoProcessingNative::OnCallback(
    std::function<void(std::shared_ptr<VideoProcessingCallbackNative>&, void*)>&& task)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (callback_ == nullptr) {
        return;
    }
    task(callback_, userData_);
}

void VideoProcessingNative::ConsumerListener::OnBufferAvailable()
{
    if (owner_ == nullptr) {
        VPE_LOGE("Video processing is null!");
        return;
    }
    owner_->OnConsumerBufferAvailable();
}
