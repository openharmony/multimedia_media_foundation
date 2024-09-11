/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef VIDEO_SAMPLE
#define VIDEO_SAMPLE

#include <string>
#include <thread>
#include <memory>
#include <iostream>

#include "surface/window.h"
#include "native_buffer.h"
#include "iconsumer_surface.h"
#include "sync_fence.h"
#include "video_processing.h"
#include "video_processing_native.h"

typedef struct VideoProcessParam {
    OH_NativeBuffer_Format inFmt;
    int32_t inWidth;
    int32_t inHeight;
    OH_NativeBuffer_Format outFmt;
    int32_t outWidth;
    int32_t outHeight;
} VideoProcessParam;

namespace OHOS {

struct SurfaceBufferWrapper {
public:
    SurfaceBufferWrapper() = default;
    ~SurfaceBufferWrapper() = default;

    sptr<SurfaceBuffer> memory{nullptr};
    sptr<SyncFence> fence{nullptr};
    int64_t timestamp;
};

class VideoSample {
public:
    VideoSample();
    ~VideoSample();

    void OnBufferAvailable();
    void NotifyCv();
    void SetImplLoader(bool isImpl);
    void SetQualityLevel(VideoDetailEnhancer_QualityLevel level);
    void UpdateErrorCount();
    int32_t InitVideoSample(VideoProcessParam param);
    int32_t InitVideoSampleImpl(VideoProcessParam param);
    int32_t InputFunc();
    int32_t SetSurfaceOnRunningImpl();
    int32_t StartProcess();
    int32_t StartProcessImpl();
    int32_t WaitAndStopSample();
    int32_t WaitAndStopSampleImpl();

private:
    void SetInputWindowParam();

    VideoProcessing_Callback* callback_{};
    VideoProcessing_Callback* callbackImpl_{};
    std::unique_ptr<std::thread> inputLoop_{};
    std::mutex mutexListener_{};
    sptr<Surface> cs_{};
    int32_t errCount_{0};
    std::queue<sptr<SurfaceBuffer>> inputBufferAvilQue_{};
    bool isImpl_{false};
    OHNativeWindow* inWindow_{};
    OHNativeWindow* outWindow_{};
    VideoProcessParam param_{};
    struct Region::Rect* rect_{};
    struct Region region_{};
    OH_VideoProcessing* videoProcessor_{};
    OH_VideoProcessing* videoProcessorImpl_{};
    VideoDetailEnhancer_QualityLevel qualityLevel_{};
    std::mutex mutex_{};
    std::condition_variable cv_{};
};
} // namespace OHOS

#endif // VIDEO_SAMPLE
