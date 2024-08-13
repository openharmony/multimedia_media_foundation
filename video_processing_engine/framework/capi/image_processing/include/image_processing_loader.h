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

#ifndef IMAGE_PROCESSING_LOADER_H
#define IMAGE_PROCESSING_LOADER_H

#include <atomic>
#include <functional>
#include <mutex>
#include <string>

#include "image_processing_capi_interface.h"

class ImageProcessingNdkLoader {
public:
    static ImageProcessingNdkLoader& Get();

    bool LoadLibrary();
    void UnloadLibrary();
    bool IsValid() const;

    // Interface
    ImageProcessing_ErrorCode InitializeEnvironment();
    ImageProcessing_ErrorCode DeinitializeEnvironment();
    bool IsColorSpaceConversionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo);
    bool IsCompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo);
    bool IsDecompositionSupported(
        const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
        const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo);
    bool IsMetadataGenerationSupported(const ImageProcessing_ColorSpaceInfo* sourceImageInfo);
    ImageProcessing_ErrorCode Create(OH_ImageProcessing** imageProcessor, int32_t type);
    ImageProcessing_ErrorCode Destroy(OH_ImageProcessing* imageProcessor);
    ImageProcessing_ErrorCode SetParameter(OH_ImageProcessing* imageProcessor, const OH_AVFormat* parameter);
    ImageProcessing_ErrorCode GetParameter(OH_ImageProcessing* imageProcessor, OH_AVFormat* parameter);
    ImageProcessing_ErrorCode ConvertColorSpace(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* destinationImage);
    ImageProcessing_ErrorCode Compose(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage);
    ImageProcessing_ErrorCode Decompose(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap);
    ImageProcessing_ErrorCode GenerateMetadata(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage);
    ImageProcessing_ErrorCode EnhanceDetail(OH_ImageProcessing* imageProcessor, OH_PixelmapNative* sourceImage,
        OH_PixelmapNative* destinationImage);

private:
    using createNdkFunc = IImageProcessingNdk* (*)();
    using destroyNdkFunc = void (*)(IImageProcessingNdk* obj);

    ImageProcessingNdkLoader() = default;
    virtual ~ImageProcessingNdkLoader() = default;
    ImageProcessingNdkLoader(const ImageProcessingNdkLoader&) = delete;
    ImageProcessingNdkLoader& operator=(const ImageProcessingNdkLoader&) = delete;
    ImageProcessingNdkLoader(ImageProcessingNdkLoader&&) = delete;
    ImageProcessingNdkLoader& operator=(ImageProcessingNdkLoader&&) = delete;

    bool LoadLibraryLocked();
    void UnloadLibraryLocked();
    bool OpenLibraryLocked(const std::string& path);
    bool LoadInterfaceLocked(IImageProcessingNdk*& interface, destroyNdkFunc& destroyFunc,
        const std::string& createFuncName, const std::string& destroyFuncName, const std::string& path);

    bool CallSupportNdk(std::function<bool(IImageProcessingNdk*)>&& operation);
    ImageProcessing_ErrorCode CallNdk(std::function<ImageProcessing_ErrorCode(IImageProcessingNdk*)>&& operation);

    std::mutex lock_{};
    // Guarded by lock_ begin
    std::atomic<bool> isValid_{};
    uint32_t refCount_{0};
    IImageProcessingNdk* imageProcessing_{};
    destroyNdkFunc destroyImageProcessingFunc_{};
    void* libHandle_{};
    // Guarded by lock_ end
};

#endif // IMAGE_PROCESSING_LOADER_H
