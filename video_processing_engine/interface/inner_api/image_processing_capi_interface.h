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

#ifndef IMAGE_PROCESSING_CAPI_INTERFACE_H
#define IMAGE_PROCESSING_CAPI_INTERFACE_H

#include "image_processing_types.h"

class IImageProcessingNdk {
public:
    virtual ImageProcessing_ErrorCode InitializeEnvironment() = 0;
    virtual ImageProcessing_ErrorCode DeinitializeEnvironment() = 0;
    virtual bool IsColorSpaceConversionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo) = 0;
    virtual bool IsCompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo) = 0;
    virtual bool IsDecompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo) = 0;
    virtual bool IsMetadataGenerationSupported(const ImageProcessing_ColorSpaceInfo* sourceImageInfo) = 0;
    virtual ImageProcessing_ErrorCode Create(OH_ImageProcessing** imageProcessor, int32_t type) = 0;
    virtual ImageProcessing_ErrorCode Destroy(OH_ImageProcessing* imageProcessor) = 0;
    virtual ImageProcessing_ErrorCode SetParameter(OH_ImageProcessing* imageProcessor,
        const OH_AVFormat* parameter) = 0;
    virtual ImageProcessing_ErrorCode GetParameter(OH_ImageProcessing* imageProcessor, OH_AVFormat* parameter) = 0;
    virtual ImageProcessing_ErrorCode ConvertColorSpace(OH_ImageProcessing* imageProcessor,
        OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage) = 0;
    virtual ImageProcessing_ErrorCode Compose(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage) = 0;
    virtual ImageProcessing_ErrorCode Decompose(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap) = 0;
    virtual ImageProcessing_ErrorCode GenerateMetadata(OH_ImageProcessing* imageProcessor,
        OH_PixelmapNative* sourceImage) = 0;
    virtual ImageProcessing_ErrorCode EnhanceDetail(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* destinationImage) = 0;

protected:
    IImageProcessingNdk() = default;
    virtual ~IImageProcessingNdk() = default;
    IImageProcessingNdk(const IImageProcessingNdk&) = delete;
    IImageProcessingNdk& operator=(const IImageProcessingNdk&) = delete;
    IImageProcessingNdk(IImageProcessingNdk&&) = delete;
    IImageProcessingNdk& operator=(IImageProcessingNdk&&) = delete;
};

extern "C" IImageProcessingNdk* CreateImageProcessingNdk();
extern "C" void DestroyImageProcessingNdk(IImageProcessingNdk* obj);

#endif // IMAGE_PROCESSING_CAPI_INTERFACE_H
