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

#include "video_processing_callback_native.h"

#include "vpe_log.h"

using namespace OHOS::Media::VideoProcessingEngine;

VideoProcessing_ErrorCode VideoProcessingCallbackNative::BindOnError(OH_VideoProcessingCallback_OnError onError)
{
    if (onError == nullptr) {
        VPE_LOGE("onError is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return BindFunction([this, onError]() { onError_ = onError; });
}

VideoProcessing_ErrorCode VideoProcessingCallbackNative::BindOnState(OH_VideoProcessingCallback_OnState onState)
{
    if (onState == nullptr) {
        VPE_LOGE("onState is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return BindFunction([this, onState]() { onState_ = onState; });
}

VideoProcessing_ErrorCode VideoProcessingCallbackNative::BindOnNewOutputBuffer(
    OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer)
{
    if (onNewOutputBuffer == nullptr) {
        VPE_LOGE("onNewOutputBuffer is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return BindFunction([this, onNewOutputBuffer]() { onNewOutputBuffer_ = onNewOutputBuffer; });
}

bool VideoProcessingCallbackNative::IsValid() const
{
    return isValid_.load();
}

bool VideoProcessingCallbackNative::IsModifiable() const
{
    return isModifiable_.load();
}

void VideoProcessingCallbackNative::LockModifiers()
{
    std::lock_guard<std::mutex> lock(lock_);
    isModifiable_ = false;
}

void VideoProcessingCallbackNative::UnlockModifiers()
{
    std::lock_guard<std::mutex> lock(lock_);
    isModifiable_ = true;
}

bool VideoProcessingCallbackNative::HasOnNewOutputBuffer() const
{
    std::lock_guard<std::mutex> lock(lock_);
    return onNewOutputBuffer_ != nullptr;
}

void VideoProcessingCallbackNative::OnError(OH_VideoProcessing* instance, VideoProcessing_ErrorCode errorCode,
    void* userData) const
{
    if (onError_ == nullptr) {
        VPE_LOGD("onError_ is null!");
        return;
    }
    onError_(instance, errorCode, userData);
}

void VideoProcessingCallbackNative::OnState(OH_VideoProcessing* instance, VideoProcessing_State state,
    void* userData) const
{
    if (onState_ == nullptr) {
        VPE_LOGD("onState_ is null!");
        return;
    }
    onState_(instance, state, userData);
}

void VideoProcessingCallbackNative::OnNewOutputBuffer(OH_VideoProcessing* instance, uint32_t index,
    void* userData) const
{
    if (onNewOutputBuffer_ == nullptr) {
        VPE_LOGD("onNewOutputBuffer_ is null!");
        return;
    }
    onNewOutputBuffer_(instance, index, userData);
}

VideoProcessing_ErrorCode VideoProcessingCallbackNative::BindFunction(std::function<void()>&& functionBinder)
{
    if (!isModifiable_.load()) {
        return VIDEO_PROCESSING_ERROR_PROCESS_FAILED;
    }
    std::lock_guard<std::mutex> lock(lock_);
    functionBinder();
    isValid_ = true;
    return VIDEO_PROCESSING_SUCCESS;
}
