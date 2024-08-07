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

#ifndef VIDEO_PROCESSING_NATIVE_H
#define VIDEO_PROCESSING_NATIVE_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "meta/format.h"
#include "surface.h"

#include "algorithm_types.h"
#include "video_processing_callback_native.h"
#include "video_processing_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
/**
 * Native implementation for video processing.
 */
class VideoProcessingNative : public std::enable_shared_from_this<VideoProcessingNative> {
public:
    VideoProcessing_ErrorCode Initialize();
    VideoProcessing_ErrorCode Deinitialize();
    VideoProcessing_ErrorCode RegisterCallback(const VideoProcessing_Callback* callback, void* userData);
    VideoProcessing_ErrorCode SetSurface(const OHNativeWindow* window);
    VideoProcessing_ErrorCode GetSurface(OHNativeWindow** window);
    VideoProcessing_ErrorCode SetParameter(const OH_AVFormat* parameter);
    VideoProcessing_ErrorCode GetParameter(OH_AVFormat* parameter);
    VideoProcessing_ErrorCode Start();
    VideoProcessing_ErrorCode Stop();
    VideoProcessing_ErrorCode RenderOutputBuffer(uint32_t index);

protected:
    explicit VideoProcessingNative(OH_VideoProcessing* context) : context_(context) {};
    virtual ~VideoProcessingNative() = default;
    VideoProcessingNative(const VideoProcessingNative&) = delete;
    VideoProcessingNative& operator=(const VideoProcessingNative&) = delete;
    VideoProcessingNative(VideoProcessingNative&&) = delete;
    VideoProcessingNative& operator=(VideoProcessingNative&&) = delete;

    static VideoProcessing_ErrorCode AlgoErrorToNdk(AlgoErrorCode errorCode);

    virtual VideoProcessing_ErrorCode InitializeInner();
    virtual VideoProcessing_ErrorCode DeinitializeInner();
    virtual VideoProcessing_ErrorCode SetParameter(const OHOS::Media::Format& parameter);
    virtual VideoProcessing_ErrorCode GetParameter(OHOS::Media::Format& parameter);
    virtual bool IsProducerSurfaceValid(const OHNativeWindow& window);
    virtual VideoProcessing_ErrorCode SetProducerSurface(const OHNativeWindow& window, BufferRequestConfig& requestCfg);
    virtual VideoProcessing_ErrorCode Process(const sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage);
    virtual void UpdateRequestCfg(const sptr<SurfaceBuffer>& consumerBuffer, BufferRequestConfig& requestCfg);

private:
    enum class VPEState : int {
        IDLE = 0,
        RUNNING,
        STOPPING
    };

    struct SurfaceBufferInfo {
        sptr<SurfaceBuffer> buffer{};
        int64_t timestamp{};
    };

    class ConsumerListener : public IBufferConsumerListener {
    public:
        explicit ConsumerListener(const std::shared_ptr<VideoProcessingNative>& owner) : owner_(owner) {}
        virtual ~ConsumerListener() = default;
        ConsumerListener(const ConsumerListener&) = delete;
        ConsumerListener& operator=(const ConsumerListener&) = delete;
        ConsumerListener(ConsumerListener&&) = delete;
        ConsumerListener& operator=(ConsumerListener&&) = delete;

        void OnBufferAvailable() final;

    private:
        std::shared_ptr<VideoProcessingNative> owner_{};
    };

    void OnError(VideoProcessing_ErrorCode errorCode);
    void OnState(VideoProcessing_State state);
    void OnNewOutputBuffer(uint32_t index);

    GSError OnConsumerBufferAvailable();
    GSError OnProducerBufferReleased();

    VideoProcessing_ErrorCode RenderOutputBufferInner(uint32_t index);
    sptr<Surface> CreateConsumerSurface(OHNativeWindow*& window);
    bool RequestBuffer(GSError& errorCode);
    void PrepareBuffers();
    void ProcessBuffers();
    bool WaitCV(std::function<bool(void)>&& checker, std::unique_lock<std::mutex>& lock);
    void CheckStopping();

    VideoProcessing_ErrorCode ExecuteWhenIdle(std::function<VideoProcessing_ErrorCode(void)>&& operation,
        const std::string& errorMessage);
    VideoProcessing_ErrorCode ExecuteWhenNotIdle(std::function<VideoProcessing_ErrorCode(void)>&& operation,
        const std::string& errorMessage);
    VideoProcessing_ErrorCode ExecuteWhenRunning(std::function<VideoProcessing_ErrorCode(void)>&& operation,
        const std::string& errorMessage);
    VideoProcessing_ErrorCode ExecuteWithCheck(std::function<bool(void)>&& checker,
        std::function<VideoProcessing_ErrorCode(void)>&& operation, const std::string& errorMessage);
    void OnCallback(std::function<void(std::shared_ptr<VideoProcessingCallbackNative>&, void*)>&& task);

    OH_VideoProcessing* context_{};

    mutable std::mutex lock_{};
    // Guarded by lock_ begin
    bool isInitialized_{false};
    std::atomic<VPEState> state_{VPEState::IDLE};
    std::atomic<bool> isExit_{false};
    sptr<Surface> consumer_{};
    sptr<Surface> producer_{};
    std::shared_ptr<VideoProcessingCallbackNative> callback_{};
    void* userData_{};
    std::thread worker_{};
    // Guarded by lock_ end

    std::condition_variable cv_{};
    std::atomic<bool> isOnNewOutputBuffer_{};

    mutable std::mutex bufferLock_{};
    // Guarded by bufferLock_ begin
    bool isBufferQueueReady_{};
    BufferRequestConfig requestCfg_{};
    std::queue<SurfaceBufferInfo> producerBufferQueue_;
    std::queue<SurfaceBufferInfo> consumerBufferQueue_;
    std::queue<SurfaceBufferInfo> RenderBufferQueue_;
    // Guarded by bufferLock_ end
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // VIDEO_PROCESSING_NATIVE_H
