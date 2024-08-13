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

#ifndef IMAGE_PROCESSING_CAPABILITY_H
#define IMAGE_PROCESSING_CAPABILITY_H

#include "image_processing_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class ImageProcessingCapability {
public:
    static bool IsColorSpaceConversionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo);
    static bool IsCompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo);
    static bool IsDecompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo);
    static bool IsMetadataGenerationSupported(const ImageProcessing_ColorSpaceInfo* sourceImageInfo);

private:
    ImageProcessingCapability() = default;
    virtual ~ImageProcessingCapability() = default;
    ImageProcessingCapability(const ImageProcessingCapability&) = delete;
    ImageProcessingCapability& operator=(const ImageProcessingCapability&) = delete;
    ImageProcessingCapability(ImageProcessingCapability&&) = delete;
    ImageProcessingCapability& operator=(ImageProcessingCapability&&) = delete;
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // IMAGE_PROCESSING_CAPABILITY_H
