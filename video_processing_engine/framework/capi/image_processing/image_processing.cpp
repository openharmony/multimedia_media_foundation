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

#include "image_processing.h"

#include <functional>

#include "vpe_log.h"
#include "image_processing_loader.h"

const int32_t IMAGE_PROCESSING_TYPE_COLOR_SPACE_CONVERSION = 0x1;
const int32_t IMAGE_PROCESSING_TYPE_METADATA_GENERATION = 0x2;
const int32_t IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER = 0x4;
const char* IMAGE_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL = "QualityLevel";

namespace {
bool CallImageProcessingSupport(std::function<bool(ImageProcessingNdkLoader&)>&& operation)
{
    if (ImageProcessingNdkLoader::Get().IsValid()) {
        return operation(ImageProcessingNdkLoader::Get());
    }
    return false;
}

ImageProcessing_ErrorCode CallImageProcessing(
    std::function<ImageProcessing_ErrorCode(ImageProcessingNdkLoader&)>&& operation)
{
    if (ImageProcessingNdkLoader::Get().IsValid()) {
        return operation(ImageProcessingNdkLoader::Get());
    }
    return IMAGE_PROCESSING_ERROR_UNKNOWN;
}

ImageProcessing_ErrorCode CallImageProcessingWithLoad(
    std::function<ImageProcessing_ErrorCode(ImageProcessingNdkLoader&)>&& operation)
{
    if (ImageProcessingNdkLoader::Get().LoadLibrary()) {
        return operation(ImageProcessingNdkLoader::Get());
    }
    return IMAGE_PROCESSING_ERROR_UNKNOWN;
}

ImageProcessing_ErrorCode CallImageProcessingWithUnload(
    std::function<ImageProcessing_ErrorCode(ImageProcessingNdkLoader&)>&& operation)
{
    if (ImageProcessingNdkLoader::Get().IsValid()) {
        auto ret = operation(ImageProcessingNdkLoader::Get());
        ImageProcessingNdkLoader::Get().UnloadLibrary();
        return ret;
    }
    return IMAGE_PROCESSING_ERROR_UNKNOWN;
}
}

ImageProcessing_ErrorCode OH_ImageProcessing_InitializeEnvironment(void)
{
    return CallImageProcessingWithLoad([](ImageProcessingNdkLoader& loader) { return loader.InitializeEnvironment(); });
}

ImageProcessing_ErrorCode OH_ImageProcessing_DeinitializeEnvironment(void)
{
    return CallImageProcessingWithUnload([](ImageProcessingNdkLoader& loader) {
        return loader.DeinitializeEnvironment();
    });
}

bool OH_ImageProcessing_IsColorSpaceConversionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return CallImageProcessingSupport([sourceImageInfo, destinationImageInfo](ImageProcessingNdkLoader& loader) {
        return loader.IsColorSpaceConversionSupported(sourceImageInfo, destinationImageInfo);
    });
}

bool OH_ImageProcessing_IsCompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return CallImageProcessingSupport(
        [sourceImageInfo, sourceGainmapInfo, destinationImageInfo](ImageProcessingNdkLoader& loader) {
            return loader.IsCompositionSupported(sourceImageInfo, sourceGainmapInfo, destinationImageInfo);
        });
}

bool OH_ImageProcessing_IsDecompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo)
{
    return CallImageProcessingSupport(
        [sourceImageInfo, destinationImageInfo, destinationGainmapInfo](ImageProcessingNdkLoader& loader) {
            return loader.IsDecompositionSupported(sourceImageInfo, destinationImageInfo, destinationGainmapInfo);
        });
}

bool OH_ImageProcessing_IsMetadataGenerationSupported(const ImageProcessing_ColorSpaceInfo* sourceImageInfo)
{
    return CallImageProcessingSupport([sourceImageInfo](ImageProcessingNdkLoader& loader) {
        return loader.IsMetadataGenerationSupported(sourceImageInfo);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_Create(OH_ImageProcessing** imageProcessor, int type)
{
    return CallImageProcessingWithLoad([imageProcessor, type](ImageProcessingNdkLoader& loader) {
        return loader.Create(imageProcessor, type);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_Destroy(OH_ImageProcessing* imageProcessor)
{
    return CallImageProcessingWithUnload([imageProcessor](ImageProcessingNdkLoader& loader) {
        return loader.Destroy(imageProcessor);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_SetParameter(OH_ImageProcessing* imageProcessor,
    const OH_AVFormat* parameter)
{
    return CallImageProcessing([imageProcessor, parameter](ImageProcessingNdkLoader& loader) {
        return loader.SetParameter(imageProcessor, parameter);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_GetParameter(OH_ImageProcessing* imageProcessor, OH_AVFormat* parameter)
{
    return CallImageProcessing([imageProcessor, parameter](ImageProcessingNdkLoader& loader) {
        return loader.GetParameter(imageProcessor, parameter);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_ConvertColorSpace(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage)
{
    return CallImageProcessing([imageProcessor, sourceImage, destinationImage](ImageProcessingNdkLoader& loader) {
        return loader.ConvertColorSpace(imageProcessor, sourceImage, destinationImage);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_Compose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage)
{
    return CallImageProcessing(
        [imageProcessor, sourceImage, sourceGainmap, destinationImage](ImageProcessingNdkLoader& loader) {
            return loader.Compose(imageProcessor, sourceImage, sourceGainmap, destinationImage);
        });
}

ImageProcessing_ErrorCode OH_ImageProcessing_Decompose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap)
{
    return CallImageProcessing(
        [imageProcessor, sourceImage, destinationImage, destinationGainmap](ImageProcessingNdkLoader& loader) {
            return loader.Decompose(imageProcessor, sourceImage, destinationImage, destinationGainmap);
        });
}

ImageProcessing_ErrorCode OH_ImageProcessing_GenerateMetadata(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage)
{
    return CallImageProcessing([imageProcessor, sourceImage](ImageProcessingNdkLoader& loader) {
        return loader.GenerateMetadata(imageProcessor, sourceImage);
    });
}

ImageProcessing_ErrorCode OH_ImageProcessing_EnhanceDetail(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage)
{
    return CallImageProcessing([imageProcessor, sourceImage, destinationImage](ImageProcessingNdkLoader& loader) {
        return loader.EnhanceDetail(imageProcessor, sourceImage, destinationImage);
    });
}
