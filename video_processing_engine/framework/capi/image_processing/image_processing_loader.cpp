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

#include "image_processing_loader.h"

namespace {
const std::string VPE_IMPL_LIBRARY_PATH = "libimage_processing_capi_impl.so";
}

ImageProcessingNdkLoader& ImageProcessingNdkLoader::Get()
{
    static ImageProcessingNdkLoader loader{};
    return loader;
}

bool ImageProcessingNdkLoader::LoadLibrary()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (refCount_ > 0) {
        refCount_++;
        return true;
    }

    if (!LoadLibraryLocked()) {
        return false;
    }
    refCount_++;
    return true;
}

void ImageProcessingNdkLoader::UnloadLibrary()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (refCount_ == 0) {
        VPE_LOGW("Unload too many times!");
        return;
    }

    refCount_--;
    if (refCount_ > 0) {
        return;
    }
    UnloadLibraryLocked();
}

bool ImageProcessingNdkLoader::IsValid()
{
    return isValid_.load();
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::InitializeEnvironment()
{
    return CallNdk([](IImageProcessingNdk* intfNdk) { return intfNdk->InitializeEnvironment(); });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::DeinitializeEnvironment()
{
    return CallNdk([](IImageProcessingNdk* intfNdk) { return intfNdk->DeinitializeEnvironment(); });
}

bool ImageProcessingNdkLoader::IsColorSpaceConversionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return CallSupportNdk([sourceImageInfo, destinationImageInfo](IImageProcessingNdk* intfNdk) {
        return intfNdk->IsColorSpaceConversionSupported(sourceImageInfo, destinationImageInfo);
    });
}

bool ImageProcessingNdkLoader::IsCompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo)
{
    return CallSupportNdk([sourceImageInfo, sourceGainmapInfo, destinationImageInfo](IImageProcessingNdk* intfNdk) {
        return intfNdk->IsCompositionSupported(sourceImageInfo, sourceGainmapInfo, destinationImageInfo);
    });
}

bool ImageProcessingNdkLoader::IsDecompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo)
{
    return CallSupportNdk(
        [sourceImageInfo, destinationImageInfo, destinationGainmapInfo](IImageProcessingNdk* intfNdk) {
            return intfNdk->IsDecompositionSupported(sourceImageInfo, destinationImageInfo, destinationGainmapInfo);
        });
}

bool ImageProcessingNdkLoader::IsMetadataGenerationSupported(const ImageProcessing_ColorSpaceInfo* sourceImageInfo)
{
    return CallSupportNdk([sourceImageInfo](IImageProcessingNdk* intfNdk) {
        return intfNdk->IsMetadataGenerationSupported(sourceImageInfo);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::Create(OH_ImageProcessing** imageProcessor, int32_t type)
{
    return CallNdk([imageProcessor, type](IImageProcessingNdk* intfNdk) {
        return intfNdk->Create(imageProcessor, type);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::Destroy(OH_ImageProcessing* imageProcessor)
{
    return CallNdk([imageProcessor](IImageProcessingNdk* intfNdk) { return intfNdk->Destroy(imageProcessor); });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::SetParameter(OH_ImageProcessing* imageProcessor,
    const OH_AVFormat* parameter)
{
    return CallNdk([imageProcessor, parameter](IImageProcessingNdk* intfNdk) {
        return intfNdk->SetParameter(imageProcessor, parameter);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::GetParameter(OH_ImageProcessing* imageProcessor,
    OH_AVFormat* parameter)
{
    return CallNdk([imageProcessor, parameter](IImageProcessingNdk* intfNdk) {
        return intfNdk->GetParameter(imageProcessor, parameter);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::ConvertColorSpace(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage)
{
    return CallNdk([imageProcessor, sourceImage, destinationImage](IImageProcessingNdk* intfNdk) {
        return intfNdk->ConvertColorSpace(imageProcessor, sourceImage, destinationImage);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::Compose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage)
{
    return CallNdk([imageProcessor, sourceImage, sourceGainmap, destinationImage](IImageProcessingNdk* intfNdk) {
        return intfNdk->Compose(imageProcessor, sourceImage, sourceGainmap, destinationImage);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::Decompose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap)
{
    return CallNdk([imageProcessor, sourceImage, destinationImage, destinationGainmap](IImageProcessingNdk* intfNdk) {
        return intfNdk->Decompose(imageProcessor, sourceImage, destinationImage, destinationGainmap);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::GenerateMetadata(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage)
{
    return CallNdk([imageProcessor, sourceImage](IImageProcessingNdk* intfNdk) {
        return intfNdk->GenerateMetadata(imageProcessor, sourceImage);
    });
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::EnhanceDetail(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage)
{
    return CallNdk([imageProcessor, sourceImage, destinationImage](IImageProcessingNdk* intfNdk) {
        return intfNdk->EnhanceDetail(imageProcessor, sourceImage, destinationImage);
    });
}

bool ImageProcessingNdkLoader::LoadLibraryLocked()
{
    VPE_LOGI("Load library...");
    const std::string& path = VPE_IMPL_LIBRARY_PATH;
    if (!OpenLibraryLocked(path)) {
        return false;
    }

    if (!LoadInterfaceLocked<IImageProcessingNdk*, destroyNdkFunc, createNdkFunc>(
        imageProcessing_, destroyImageProcessingFunc_,
        "CreateImageProcessingNdk", "DestroyImageProcessingNdk", path)) {
        UnloadLibraryLocked();
        return false;
    }

    isValid_ = true;
    VPE_LOGI("Load library successfully.");
    return true;
}

void ImageProcessingNdkLoader::UnloadLibraryLocked()
{
    isValid_ = false;
    if (destroyImageProcessingFunc_ != nullptr) {
        destroyImageProcessingFunc_(imageProcessing_);
        imageProcessing_ = nullptr;
        destroyImageProcessingFunc_ = nullptr;
    }

    if (mLibHandle != nullptr)  {
        dlclose(mLibHandle);
        mLibHandle = nullptr;
    }

    VPE_LOGI("Unload library.");
}

bool ImageProcessingNdkLoader::OpenLibraryLocked(const std::string& path)
{
    mLibHandle = dlopen(path.c_str(), RTLD_NOW);
    if (mLibHandle == nullptr) {
        VPE_LOGW("Can't open library %{public}s - %{public}s", path.c_str(), dlerror());
        return false;
    }
    return true;
}

bool ImageProcessingNdkLoader::CallSupportNdk(std::function<bool(IImageProcessingNdk*)>&& operation)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (imageProcessing_ == nullptr) {
        return false;
    }
    return operation(imageProcessing_);
}

ImageProcessing_ErrorCode ImageProcessingNdkLoader::CallNdk(
    std::function<ImageProcessing_ErrorCode(IImageProcessingNdk*)>&& operation)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (imageProcessing_ == nullptr) {
        return IMAGE_PROCESSING_ERROR_UNKNOWN;
    }
    return operation(imageProcessing_);
}