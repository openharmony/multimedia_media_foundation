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

#ifndef VIDEO_PROCESSING_CALLBACK_NATIVE_H
#define VIDEO_PROCESSING_CALLBACK_NATIVE_H

#include <atomic>
#include <functional>
#include <mutex>

#include "video_processing_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class VideoProcessingCallbackNative {
public:
    VideoProcessingCallbackNative() = default;
    virtual ~VideoProcessingCallbackNative() = default;
    VideoProcessingCallbackNative(const VideoProcessingCallbackNative&) = delete;
    VideoProcessingCallbackNative& operator=(const VideoProcessingCallbackNative&) = delete;
    VideoProcessingCallbackNative(VideoProcessingCallbackNative&&) = delete;
    VideoProcessingCallbackNative& operator=(VideoProcessingCallbackNative&&) = delete;

    VideoProcessing_ErrorCode BindOnError(OH_VideoProcessingCallback_OnError onError);
    VideoProcessing_ErrorCode BindOnState(OH_VideoProcessingCallback_OnState onState);
    VideoProcessing_ErrorCode BindOnNewOutputBuffer(OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer);

    bool IsValid() const;
    bool IsModifiable() const;
    void LockModifiers();
    void UnlockModifiers();
    bool HasOnNewOutputBuffer() const;

    void OnError(OH_VideoProcessing* instance, VideoProcessing_ErrorCode errorCode, void* userData) const;
    void OnState(OH_VideoProcessing* instance, VideoProcessing_State state, void* userData) const;
    void OnNewOutputBuffer(OH_VideoProcessing* instance, uint32_t index, void* userData) const;

protected:
    VideoProcessing_ErrorCode BindFunction(std::function<void()>&& functionBinder);

    mutable std::mutex lock_{};
    // Guarded by lock_ begin
    std::atomic<bool> isValid_{false};
    std::atomic<bool> isModifiable_{true};
    OH_VideoProcessingCallback_OnError onError_{};
    OH_VideoProcessingCallback_OnState onState_{};
    OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer_{};
    // Guarded by lock_ end
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // VIDEO_PROCESSING_CALLBACK_NATIVE_H
