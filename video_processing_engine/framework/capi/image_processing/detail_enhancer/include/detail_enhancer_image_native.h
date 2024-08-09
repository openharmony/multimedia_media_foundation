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

#ifndef DETAIL_ENHANCER_IMAGE_NATIVE_H
#define DETAIL_ENHANCER_IMAGE_NATIVE_H

#include "detail_enhancer.h"
#include "image_processing_native.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class DetailEnhancerImageNative : public ImageProcessingNative {
public:
    DetailEnhancerImageNative() = default;
    virtual ~DetailEnhancerImageNative() = default;
    DetailEnhancerImageNative(const DetailEnhancerImageNative&) = delete;
    DetailEnhancerImageNative& operator=(const DetailEnhancerImageNative&) = delete;
    DetailEnhancerImageNative(DetailEnhancerImageNative&&) = delete;
    DetailEnhancerImageNative& operator=(DetailEnhancerImageNative&&) = delete;

protected:
    ImageProcessing_ErrorCode InitializeInner() final;
    ImageProcessing_ErrorCode DeinitializeInner() final;
    ImageProcessing_ErrorCode SetParameter(const OHOS::Media::Format& parameter) final;
    ImageProcessing_ErrorCode GetParameter(OHOS::Media::Format& parameter) final;
    ImageProcessing_ErrorCode EnhanceDetail(sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage) final;

private:
    int NDKLevelToInner(int level) const;

    DetailEnhancer detailEnhancer_{};
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // DETAIL_ENHANCER_IMAGE_NATIVE_H
