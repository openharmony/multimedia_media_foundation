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

#include "image_processing_native.h"

#include <cinttypes>

#include "common/native_mfmagic.h"
#include "pixelmap_native_impl.h"

#include "vpe_log.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace OHOS::Media::VideoProcessingEngine;

namespace {
const std::unordered_map<AlgoErrorCode, ImageProcessing_ErrorCode> ERROR_MAP = {
    { ALGO_SUCCESS,                         IMAGE_PROCESSING_SUCCESS },
    { ALGO_ERROR_INVALID_PARAMETER,         IMAGE_PROCESSING_ERROR_INVALID_PARAMETER },
    { ALGO_ERROR_UNKNOWN,                   IMAGE_PROCESSING_ERROR_UNKNOWN },
    { ALGO_ERROR_INITIALIZE_FAILED,         IMAGE_PROCESSING_ERROR_INITIALIZE_FAILED },
    { ALGO_ERROR_CREATE_FAILED,             IMAGE_PROCESSING_ERROR_CREATE_FAILED },
    { ALGO_ERROR_PROCESS_FAILED,            IMAGE_PROCESSING_ERROR_PROCESS_FAILED },
    { ALGO_ERROR_UNSUPPORTED_PROCESSING,    IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING },
    { ALGO_ERROR_OPERATION_NOT_PERMITTED,   IMAGE_PROCESSING_ERROR_OPERATION_NOT_PERMITTED },
    { ALGO_ERROR_NO_MEMORY,                 IMAGE_PROCESSING_ERROR_NO_MEMORY },
    { ALGO_ERROR_INVALID_INSTANCE,          IMAGE_PROCESSING_ERROR_INVALID_INSTANCE },
    { ALGO_ERROR_INVALID_VALUE,             IMAGE_PROCESSING_ERROR_INVALID_VALUE },
};

const std::unordered_map<OHOS::Media::PixelFormat, GraphicPixelFormat> FORMAT_MAP = {
    { OHOS::Media::PixelFormat::RGBA_8888, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888 },
    { OHOS::Media::PixelFormat::BGRA_8888, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BGRA_8888 },
};

float GetBytesPerPixel(OHOS::Media::PixelFormat format)
{
    float bytes;
    switch (format) {
        case OHOS::Media::PixelFormat::RGBA_8888:
        case OHOS::Media::PixelFormat::BGRA_8888:
            bytes = 4.0f; // 4.0 size bytes
            break;
        default:
            bytes = 3.0f; // 3.0 size bytes
            break;
    }
    return bytes;
}
}

ImageProcessing_ErrorCode ImageProcessingNative::Initialize()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (isInitialized_) {
        VPE_LOGE("Already initialize!");
        return IMAGE_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
    }
    auto errorCode = InitializeInner();
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    isInitialized_ = true;
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::Deinitialize()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (!isInitialized_) {
        VPE_LOGE("Already deinitialize!");
        return IMAGE_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
    }
    auto errorCode = DeinitializeInner();
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    isInitialized_ = false;
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::SetParameter(const OH_AVFormat* parameter)
{
    if (parameter == nullptr) {
        VPE_LOGE("parameter is null!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return SetParameter(parameter->format_);
}

ImageProcessing_ErrorCode ImageProcessingNative::GetParameter(OH_AVFormat* parameter)
{
    if (parameter == nullptr) {
        VPE_LOGE("parameter is null!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return GetParameter(parameter->format_);
}

ImageProcessing_ErrorCode ImageProcessingNative::ConvertColorSpace(OH_PixelmapNative* sourceImage,
    OH_PixelmapNative* destinationImage)
{
    if (sourceImage == nullptr || sourceImage->GetInnerPixelmap() == nullptr ||
        destinationImage == nullptr || destinationImage->GetInnerPixelmap() == nullptr) {
        VPE_LOGE("sourceImage or destinationImage is null or empty!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    sptr<SurfaceBuffer> srcBuffer = nullptr;
    auto errorCode = GetSurfaceBufferFromPixelMap(sourceImage->GetInnerPixelmap(), srcBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> dstBuffer = nullptr;
    std::shared_ptr<PixelMap> dstImage = destinationImage->GetInnerPixelmap();
    errorCode = GetSurfaceBufferFromPixelMapNoCopy(dstImage, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    errorCode = ConvertColorSpace(srcBuffer, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return SetSurfaceBufferToPixelMap(dstBuffer, dstImage);
}

ImageProcessing_ErrorCode ImageProcessingNative::Compose(OH_PixelmapNative* sourceImage,
    OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage)
{
    if (sourceImage == nullptr || sourceImage->GetInnerPixelmap() == nullptr ||
        sourceGainmap == nullptr || sourceGainmap->GetInnerPixelmap() == nullptr ||
        destinationImage == nullptr || destinationImage->GetInnerPixelmap() == nullptr) {
            VPE_LOGE("sourceImage, sourceGainmap or destinationImage is null or empty!");
            return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    sptr<SurfaceBuffer> srcBuffer = nullptr;
    auto errorCode = GetSurfaceBufferFromPixelMap(sourceImage->GetInnerPixelmap(), srcBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> srcGainmap = nullptr;
    errorCode = GetSurfaceBufferFromPixelMap(sourceGainmap->GetInnerPixelmap(), srcGainmap);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> dstBuffer = nullptr;
    std::shared_ptr<PixelMap> dstImage = destinationImage->GetInnerPixelmap();
    errorCode = GetSurfaceBufferFromPixelMapNoCopy(dstImage, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    errorCode = Compose(srcBuffer, srcGainmap, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return SetSurfaceBufferToPixelMap(dstBuffer, dstImage);
}

ImageProcessing_ErrorCode ImageProcessingNative::Decompose(OH_PixelmapNative* sourceImage,
    OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap)
{
    if (sourceImage == nullptr || sourceImage->GetInnerPixelmap() == nullptr ||
        destinationImage == nullptr || destinationImage->GetInnerPixelmap() == nullptr ||
        destinationGainmap == nullptr || destinationGainmap->GetInnerPixelmap() == nullptr) {
            VPE_LOGE("sourceImage, destinationImage or destinationGainmap is null or empty!");
            return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    sptr<SurfaceBuffer> srcBuffer = nullptr;
    auto errorCode = GetSurfaceBufferFromPixelMap(sourceImage->GetInnerPixelmap(), srcBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> dstBuffer = nullptr;
    std::shared_ptr<PixelMap> dstImage = destinationImage->GetInnerPixelmap();
    errorCode = GetSurfaceBufferFromPixelMapNoCopy(dstImage, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> dstGainmap = nullptr;
    std::shared_ptr<PixelMap> dstImageGainmap = destinationGainmap->GetInnerPixelmap();
    errorCode = GetSurfaceBufferFromPixelMapNoCopy(dstImageGainmap, dstGainmap);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    errorCode = Decompose(srcBuffer, dstBuffer, dstGainmap);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    errorCode = SetSurfaceBufferToPixelMap(dstBuffer, dstImage);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return SetSurfaceBufferToPixelMap(dstGainmap, dstImageGainmap);
}

ImageProcessing_ErrorCode ImageProcessingNative::GenerateMetadata(OH_PixelmapNative* sourceImage)
{
    if (sourceImage == nullptr || sourceImage->GetInnerPixelmap() == nullptr) {
            VPE_LOGE("sourceImage is null or empty!");
            return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    sptr<SurfaceBuffer> srcBuffer = nullptr;
    auto errorCode = GetSurfaceBufferFromPixelMap(sourceImage->GetInnerPixelmap(), srcBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return GenerateMetadata(srcBuffer); // Need to copy metadata to PixelMap
}

ImageProcessing_ErrorCode ImageProcessingNative::EnhanceDetail(OH_PixelmapNative* sourceImage,
    OH_PixelmapNative* destinationImage)
{
    if (sourceImage == nullptr || sourceImage->GetInnerPixelmap() == nullptr ||
        destinationImage == nullptr || destinationImage->GetInnerPixelmap() == nullptr) {
        VPE_LOGE("sourceImage or destinationImage is null or empty!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    sptr<SurfaceBuffer> srcBuffer = nullptr;
    auto errorCode = GetSurfaceBufferFromPixelMap(sourceImage->GetInnerPixelmap(), srcBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    sptr<SurfaceBuffer> dstBuffer = nullptr;
    std::shared_ptr<PixelMap> dstImage = destinationImage->GetInnerPixelmap();
    errorCode = GetSurfaceBufferFromPixelMapNoCopy(dstImage, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    errorCode = EnhanceDetail(srcBuffer, dstBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return SetSurfaceBufferToPixelMap(dstBuffer, dstImage);
}

ImageProcessing_ErrorCode ImageProcessingNative::AlgoErrorToNdk(AlgoErrorCode errorCode)
{
    auto it = ERROR_MAP.find(errorCode);
    if (it == ERROR_MAP.end()) {
        VPE_LOGE("Invalid error code:%{public}d", errorCode);
        return IMAGE_PROCESSING_ERROR_UNKNOWN;
    }
    return it->second;
}

ImageProcessing_ErrorCode ImageProcessingNative::GetSurfaceBufferFromPixelMap(const std::shared_ptr<PixelMap>& pixelMap,
    sptr<SurfaceBuffer>& surfaceBuffer)
{
    if (pixelMap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
        return IMAGE_PROCESSING_SUCCESS;
    }
    auto errorCode = CreateSurfaceBufferFromPixelMap(pixelMap, surfaceBuffer);
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        return errorCode;
    }
    return CopyPixelMapToSurfaceBuffer(pixelMap, surfaceBuffer);
}

ImageProcessing_ErrorCode ImageProcessingNative::GetSurfaceBufferFromPixelMapNoCopy(
    const std::shared_ptr<PixelMap>& pixelMap, sptr<SurfaceBuffer>& surfaceBuffer)
{
    if (pixelMap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
        return IMAGE_PROCESSING_SUCCESS;
    }
    return CreateSurfaceBufferFromPixelMap(pixelMap, surfaceBuffer);
}

ImageProcessing_ErrorCode ImageProcessingNative::SetSurfaceBufferToPixelMap(const sptr<SurfaceBuffer>& surfaceBuffer,
    std::shared_ptr<PixelMap>& pixelMap)
{
    if (pixelMap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        return IMAGE_PROCESSING_SUCCESS;
    }
    return CopySurfaceBufferToPixelMap(surfaceBuffer, pixelMap);
}

ImageProcessing_ErrorCode ImageProcessingNative::InitializeInner()
{
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::DeinitializeInner()
{
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::SetParameter([[maybe_unused]] const OHOS::Media::Format& parameter)
{
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::GetParameter([[maybe_unused]] OHOS::Media::Format& parameter)
{
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::ConvertColorSpace([[maybe_unused]] sptr<SurfaceBuffer>& sourceImage,
    [[maybe_unused]] sptr<SurfaceBuffer>& destinationImage)
{
    return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
}

ImageProcessing_ErrorCode ImageProcessingNative::Compose([[maybe_unused]] sptr<SurfaceBuffer>& sourceImage,
    [[maybe_unused]] sptr<SurfaceBuffer>& sourceGainmap, [[maybe_unused]] sptr<SurfaceBuffer>& destinationImage)
{
    return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
}

ImageProcessing_ErrorCode ImageProcessingNative::Decompose([[maybe_unused]] sptr<SurfaceBuffer>& sourceImage,
    [[maybe_unused]] sptr<SurfaceBuffer>& destinationImage, [[maybe_unused]] sptr<SurfaceBuffer>& destinationGainmap)
{
    return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
}

ImageProcessing_ErrorCode ImageProcessingNative::GenerateMetadata([[maybe_unused]] sptr<SurfaceBuffer>& sourceImage)
{
    return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
}

ImageProcessing_ErrorCode ImageProcessingNative::EnhanceDetail([[maybe_unused]] sptr<SurfaceBuffer>& sourceImage,
    [[maybe_unused]] sptr<SurfaceBuffer>& destinationImage)
{
    return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
}

ImageProcessing_ErrorCode ImageProcessingNative::CreateSurfaceBufferFromPixelMap(
    const std::shared_ptr<PixelMap>& pixelMap, sptr<SurfaceBuffer>& surfaceBuffer)
{
    surfaceBuffer = SurfaceBuffer::Create();
    if (surfaceBuffer == nullptr) {
        VPE_LOGE("Failed to create SurfaceBuffer!");
        return IMAGE_PROCESSING_ERROR_CREATE_FAILED;
    }
    return ConvertPixelMapToSurfaceBuffer(pixelMap, surfaceBuffer);
}

ImageProcessing_ErrorCode ImageProcessingNative::ConvertPixelMapToSurfaceBuffer(
    const std::shared_ptr<PixelMap>& pixelMap, sptr<SurfaceBuffer>& surfaceBuffer)
{
    auto it = FORMAT_MAP.find(pixelMap->GetPixelFormat());
    if (it == FORMAT_MAP.end()) {
        VPE_LOGE("Unsupported format: %{public}d", pixelMap->GetPixelFormat());
        return IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING;
    }
    BufferRequestConfig requestCfg = {};
    requestCfg.width = pixelMap->GetWidth();
    requestCfg.height = pixelMap->GetHeight();
    requestCfg.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    requestCfg.strideAlignment = requestCfg.width;
    requestCfg.format = it->second;
    requestCfg.timeout = 0;
    requestCfg.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    requestCfg.transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (surfaceBuffer->Alloc(requestCfg) != GSERROR_OK) {
        VPE_LOGE("Failed to allocate buffer for image!");
        return IMAGE_PROCESSING_ERROR_NO_MEMORY;
    }
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::CopyPixelMapToSurfaceBuffer(
    const std::shared_ptr<PixelMap>& pixelMap, sptr<SurfaceBuffer>& surfaceBuffer)
{
    int32_t rowSize = pixelMap->GetWidth() * GetBytesPerPixel(pixelMap->GetPixelFormat());
    for (int i = 0; i < pixelMap->GetHeight(); i++) {
        if (memcpy_s(static_cast<uint8_t*>(surfaceBuffer->GetVirAddr()) + i * surfaceBuffer->GetStride(),
            rowSize, pixelMap->GetPixels() + i * pixelMap->GetRowStride(), rowSize) != EOK) {
            VPE_LOGE("Failed to copy image buffer!");
            return IMAGE_PROCESSING_ERROR_PROCESS_FAILED;
            }
    }
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode ImageProcessingNative::CopySurfaceBufferToPixelMap(const sptr<SurfaceBuffer>& surfaceBuffer,
    std::shared_ptr<PixelMap>& pixelMap)
{
    int rowSize = std::min(surfaceBuffer->GetStride(), pixelMap->GetRowStride());
    for (int i = 0; i < surfaceBuffer->GetHeight(); ++i) {
        if (memcpy_s(static_cast<uint8_t*>(pixelMap->GetWritablePixels()) + i * pixelMap->GetRowStride(), rowSize,
            static_cast<uint8_t*>(surfaceBuffer->GetVirAddr()) + i * surfaceBuffer->GetStride(), rowSize) != EOK) {
            VPE_LOGE("Failed to copy image buffer!");
            return IMAGE_PROCESSING_ERROR_PROCESS_FAILED;
        }
    }
    return IMAGE_PROCESSING_SUCCESS;
}
