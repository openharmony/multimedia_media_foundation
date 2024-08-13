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

#include "image_processing_capability.h"

#include "vpe_log.h"

using namespace OHOS::Media::VideoProcessingEngine;

bool ImageProcessingCapability::IsColorSpaceConversionSupported(
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return false;
}

bool ImageProcessingCapability::IsCompositionSupported(
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return false;
}

bool ImageProcessingCapability::IsDecompositionSupported(
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo)
{
    return false;
}

bool ImageProcessingCapability::IsMetadataGenerationSupported(
    [[maybe_unused]] const ImageProcessing_ColorSpaceInfo* sourceImageInfo)
{
    return false;
}
