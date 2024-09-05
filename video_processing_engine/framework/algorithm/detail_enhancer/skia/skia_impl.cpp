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

#include "skia_impl.h"

#include <array>
#include <chrono>
#include <dlfcn.h>

#include "vpe_log.h"

using namespace OHOS;
using namespace OHOS::Media::VideoProcessingEngine;

namespace {
enum ImageFormatType {
    IMAGE_FORMAT_TYPE_UNKNOWN = 0,
    IMAGE_FORMAT_TYPE_RGB,
    IMAGE_FORMAT_TYPE_YUV,
};

constexpr int CHANNEL_Y = 0;
constexpr int CHANNEL_UV1 = 1;
constexpr int CHANNEL_UV2 = 2;
constexpr int CHANNEL_NUM_NV12_NV21 = 2;
constexpr int CHANNEL_NUM_YU12_YV12 = 3;

ImageFormatType GetImageType(const sptr<SurfaceBuffer>& input, const sptr<SurfaceBuffer>& output)
{
    if (input->GetFormat() != output->GetFormat()) {
        VPE_LOGE("Different format for input and output!");
        return IMAGE_FORMAT_TYPE_UNKNOWN;
    }
    VPE_LOGD("Input format: %{public}d, output format: %{public}d.", input->GetFormat(), output->GetFormat());
    ImageFormatType imageType = IMAGE_FORMAT_TYPE_UNKNOWN;
    switch (input->GetFormat()) {
        case OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888:
        case OHOS::GRAPHIC_PIXEL_FMT_BGRA_8888:
            imageType = IMAGE_FORMAT_TYPE_RGB;
            break;
        case OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_SP: // NV12
        case OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_SP: // NV21
        case OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_P:  // YU12
        case OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_P:  // YV12
            imageType = IMAGE_FORMAT_TYPE_YUV;
            break;
        default:
            VPE_LOGE("Default unknown type.");
            break;
    }
    return imageType;
}

SkColorType GetRGBImageFormat(const sptr<SurfaceBuffer>& surfaceBuffer)
{
    SkColorType imageFormat;
    switch (surfaceBuffer->GetFormat()) {
        case OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888:
            imageFormat = kRGBA_8888_SkColorType;
            break;
        case OHOS::GRAPHIC_PIXEL_FMT_BGRA_8888:
            imageFormat = kBGRA_8888_SkColorType;
            break;
        default:
            imageFormat = kRGBA_8888_SkColorType;
            break;
    }
    return imageFormat;
}

AlgoErrorCode PixmapScale(const SkPixmap& inputPixmap, SkPixmap& outputPixmap, SkSamplingOptions options)
{
    if (!inputPixmap.scalePixels(outputPixmap, options)) {
        return ALGO_ERROR_PROCESS_FAILED;
    }
    return ALGO_SUCCESS;
}

AlgoErrorCode RGBScale(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output)
{
    if (input->GetWidth() <= 0 || input->GetHeight() <= 0 || output->GetWidth() <= 0 || output->GetHeight() <= 0) {
        VPE_LOGE("Invalid input or output size!");
        return ALGO_ERROR_INVALID_VALUE;
    }
    SkImageInfo inputInfo = SkImageInfo::Make(input->GetWidth(), input->GetHeight(), GetRGBImageFormat(input),
        kPremul_SkAlphaType);
    SkImageInfo outputInfo = SkImageInfo::Make(output->GetWidth(), output->GetHeight(), GetRGBImageFormat(output),
        kPremul_SkAlphaType);
    SkPixmap inputPixmap(inputInfo, input->GetVirAddr(), input->GetStride());
    SkPixmap outputPixmap(outputInfo, output->GetVirAddr(), output->GetStride());

    SkSamplingOptions scaleOption(SkFilterMode::kNearest);
    return PixmapScale(inputPixmap, outputPixmap, scaleOption);
}

int ConfigYUVFormat(const sptr<SurfaceBuffer>& buffer, SkYUVAInfo::PlaneConfig& planeConfig, size_t* rowbyte,
    unsigned char** pixmapAddr)
{
    int numPlanes;
    void* planesInfoPtr = nullptr;
    buffer->GetPlanesInfo(&planesInfoPtr);
    auto planesInfo = static_cast<OH_NativeBuffer_Planes*>(planesInfoPtr);
    switch (buffer->GetFormat()) {
        case OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_SP: // NV12
            planeConfig = SkYUVAInfo::PlaneConfig::kY_UV;
            rowbyte[CHANNEL_UV1] = planesInfo->planes[CHANNEL_UV1].columnStride;
            pixmapAddr[CHANNEL_UV1] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV1].offset);
            numPlanes = CHANNEL_NUM_NV12_NV21;
            break;
        case OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_SP: // NV21
            planeConfig = SkYUVAInfo::PlaneConfig::kY_VU;
            rowbyte[CHANNEL_UV1] = planesInfo->planes[CHANNEL_UV2].columnStride;
            pixmapAddr[CHANNEL_UV1] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV2].offset);
            numPlanes = CHANNEL_NUM_NV12_NV21;
            break;
        case OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_P:  // YU12
            planeConfig = SkYUVAInfo::PlaneConfig::kY_U_V;
            rowbyte[CHANNEL_UV1] = planesInfo->planes[CHANNEL_UV1].columnStride;
            rowbyte[CHANNEL_UV2] = planesInfo->planes[CHANNEL_UV2].columnStride;
            pixmapAddr[CHANNEL_UV1] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV1].offset);
            pixmapAddr[CHANNEL_UV2] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV2].offset);
            numPlanes = CHANNEL_NUM_YU12_YV12;
            break;
        case OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_P:  // YV12
            planeConfig = SkYUVAInfo::PlaneConfig::kY_V_U;
            rowbyte[CHANNEL_UV1] = planesInfo->planes[CHANNEL_UV2].columnStride;
            rowbyte[CHANNEL_UV2] = planesInfo->planes[CHANNEL_UV1].columnStride;
            pixmapAddr[CHANNEL_UV1] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV2].offset);
            pixmapAddr[CHANNEL_UV2] = pixmapAddr[CHANNEL_Y] +
                static_cast<unsigned int>(planesInfo->planes[CHANNEL_UV1].offset);
            numPlanes = CHANNEL_NUM_YU12_YV12;
            break;
        default:
            VPE_LOGD("YUVFormat: default NV12.");
            planeConfig = SkYUVAInfo::PlaneConfig::kY_UV;
            rowbyte[CHANNEL_UV1] = planesInfo->planes[CHANNEL_UV1].columnStride;
            numPlanes = CHANNEL_NUM_NV12_NV21;
            break;
    }
    return numPlanes;
}

int CreateYUVPixmap(const sptr<SurfaceBuffer>& buffer, std::array<SkPixmap, SkYUVAInfo::kMaxPlanes>& pixmaps)
{
    SkISize imageSize;
    imageSize.fWidth = buffer->GetWidth();
    if (imageSize.fWidth <= 0) {
        VPE_LOGE("Invalid width!");
        return 0;
    }
    imageSize.fHeight = buffer->GetHeight();
    if (imageSize.fHeight <= 0) {
        VPE_LOGE("Invalid height!");
        return 0;
    }
    SkYUVColorSpace yuvColorSpace = SkYUVColorSpace::kRec709_Full_SkYUVColorSpace;
    SkYUVAInfo::Subsampling subsampling = SkYUVAInfo::Subsampling::k420;
    SkYUVAInfo::PlaneConfig planeConfig = SkYUVAInfo::PlaneConfig::kY_UV;
    size_t rowbyte[SkYUVAInfo::kMaxPlanes];
    unsigned char* pixmapAddr[SkYUVAInfo::kMaxPlanes];

    void* planesInfoPtr = nullptr;
    buffer->GetPlanesInfo(&planesInfoPtr);
    auto planesInfo = static_cast<OH_NativeBuffer_Planes*>(planesInfoPtr);
    if (planesInfoPtr == nullptr) {
        VPE_LOGD("Planes info is nullptr, configure uv stride with general stride.");
        for (int i = 0; i < SkYUVAInfo::kMaxPlanes; i++) {
            rowbyte[i] = static_cast<size_t>(buffer->GetStride());
        }
    }

    rowbyte[CHANNEL_Y] = planesInfo->planes[CHANNEL_Y].columnStride;
    pixmapAddr[CHANNEL_Y] = static_cast<unsigned char*>(buffer->GetVirAddr());
    int numPlanes = ConfigYUVFormat(buffer, planeConfig, rowbyte, pixmapAddr);

    const SkYUVAInfo yuvInfo = SkYUVAInfo(imageSize, planeConfig, subsampling, yuvColorSpace);
    const SkYUVAPixmapInfo pixmapInfo = SkYUVAPixmapInfo(yuvInfo, SkYUVAPixmapInfo::DataType::kUnorm8, rowbyte);

    for (int i = 0; i < numPlanes; i++) {
        pixmaps[i].reset(pixmapInfo.planeInfo(i), pixmapAddr[i], rowbyte[i]);
    }
    return numPlanes;
}

AlgoErrorCode YUVPixmapScale(const std::array<SkPixmap, SkYUVAInfo::kMaxPlanes>& inputPixmap,
    std::array<SkPixmap, SkYUVAInfo::kMaxPlanes>& outputPixmap, SkSamplingOptions opt, int numPlanes)
{
    for (int i = 0; i < numPlanes; i++) {
        if (!inputPixmap[i].scalePixels(outputPixmap[i], opt)) {
            VPE_LOGE("YUV scale failed!");
            return ALGO_ERROR_PROCESS_FAILED;
        }
    }
    return ALGO_SUCCESS;
}

AlgoErrorCode YUVScale(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output)
{
    std::array<SkPixmap, SkYUVAInfo::kMaxPlanes> inputPixmap;
    std::array<SkPixmap, SkYUVAInfo::kMaxPlanes> outputPixmap;
    int numPlanesInput = CreateYUVPixmap(input, inputPixmap);
    int numPlanesOutput = CreateYUVPixmap(output, outputPixmap);
    if (numPlanesInput != numPlanesOutput || numPlanesInput * numPlanesOutput == 0) {
        VPE_LOGE("Wrong YUV settings!");
        return ALGO_ERROR_INVALID_VALUE;
    }
    SkSamplingOptions scaleOption(SkFilterMode::kNearest);
    return YUVPixmapScale(inputPixmap, outputPixmap, scaleOption, numPlanesInput);
}
}

AlgoErrorCode Skia::Process(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output)
{
    AlgoErrorCode errCode;
    ImageFormatType imageType = GetImageType(input, output);
    if (imageType == IMAGE_FORMAT_TYPE_RGB) {
        errCode = RGBScale(input, output);
    } else if (imageType == IMAGE_FORMAT_TYPE_YUV) {
        errCode = YUVScale(input, output);
    } else {
        VPE_LOGE("Unknown image format!");
        errCode = ALGO_ERROR_INVALID_VALUE;
    }
    return errCode;
}