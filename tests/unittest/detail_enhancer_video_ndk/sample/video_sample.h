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

#ifndef VIDEO_SAMPLE_HF
#define VIDEO_SAMPLE_HF

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
    int32_t in_width;
    int32_t in_height;
    OH_NativeBuffer_Format outFmt;
    int32_t out_width;
    int32_t out_height;
}VideoProcessParam;

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
    int32_t inputFrameNumber = 10;
    VideoDetailEnhancer_QualityLevel qualityLevel_;
    bool isImpl = false;
    int32_t InputFunc();
    int32_t InitVideoSample(VideoProcessParam);
    int32_t InitVideoSampleImpl(VideoProcessParam);
    int32_t StartProcess();
    int32_t StartProcessImpl();
    int32_t WaitAndStopSample();
    int32_t WaitAndStopSampleImpl();
    int32_t SetSurfaceOnRunningImpl();
    int32_t errCount = 0;
private:
    void SetInputWindowParam();
    VideoProcessParam param_;
    OH_VideoProcessing* videoProcessor = nullptr;
    OH_VideoProcessing* videoProcessorImpl = nullptr;
    OHNativeWindow *inWindow = nullptr;
    OHNativeWindow *outWindow = nullptr;
    std::unique_ptr<std::ifstream> inFile;
    std::unique_ptr<std::thread> inputLoop_;
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_Callback* callbackImpl = nullptr;
    struct Region region;
    struct Region::Rect *rect;
    sptr<Surface> cs = nullptr;
    sptr<Surface> ps = nullptr;
};
} // namespace OHOS

#endif
