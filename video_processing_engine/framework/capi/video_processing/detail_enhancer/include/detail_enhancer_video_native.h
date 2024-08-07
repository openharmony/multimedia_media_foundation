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

#ifndef DETAIL_ENHANCER_VIDEO_NATIVE_H
#define DETAIL_ENHANCER_VIDEO_NATIVE_H

#include <atomic>
#include <functional>
#include <mutex>

#include "detail_enhancer.h"
#include "video_processing_native.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
/**
 * Detail enhancer interface implementation.
 */
class DetailEnhancerVideoNative : public VideoProcessingNative {
public:
    explicit DetailEnhancerVideoNative(OH_VideoProcessing* context) : VideoProcessingNative(context) {};
    virtual ~DetailEnhancerVideoNative() = default;
    DetailEnhancerVideoNative(const DetailEnhancerVideoNative&) = delete;
    DetailEnhancerVideoNative& operator=(const DetailEnhancerVideoNative&) = delete;
    DetailEnhancerVideoNative(DetailEnhancerVideoNative&&) = delete;
    DetailEnhancerVideoNative& operator=(DetailEnhancerVideoNative&&) = delete;

protected:
    VideoProcessing_ErrorCode InitializeInner() final;
    VideoProcessing_ErrorCode DeinitializeInner() final;
    VideoProcessing_ErrorCode SetParameter(const OHOS::Media::Format& parameter) final;
    VideoProcessing_ErrorCode GetParameter(OHOS::Media::Format& parameter) final;
    VideoProcessing_ErrorCode SetProducerSurface(const OHNativeWindow& window, BufferRequestConfig& requestCfg) final;
    VideoProcessing_ErrorCode Process(const sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage) final;
    void UpdateRequestCfg(const sptr<SurfaceBuffer>& consumerBuffer, BufferRequestConfig& requestCfg) final;

private:
    int NDKLevelToInner(int level) const;

    DetailEnhancer detailEnhancer_{};
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // DETAIL_ENHANCER_VIDEO_NATIVE_H