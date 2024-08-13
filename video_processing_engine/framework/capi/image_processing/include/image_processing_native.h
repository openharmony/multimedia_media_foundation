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

#ifndef IMAGE_PROCESSING_NATIVE_H
#define IMAGE_PROCESSING_NATIVE_H

#include <mutex>

#include "meta/format.h"
#include "surface_buffer.h"
#include "pixel_map.h"

#include "algorithm_types.h"
#include "image_processing_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
/**
 * Native implementation for image processing.
 */
class ImageProcessingNative : public std::enable_shared_from_this<ImageProcessingNative> {
public:
    ImageProcessing_ErrorCode Initialize();
    ImageProcessing_ErrorCode Deinitialize();
    ImageProcessing_ErrorCode SetParameter(const OH_AVFormat* parameter);
    ImageProcessing_ErrorCode GetParameter(OH_AVFormat* parameter);
    ImageProcessing_ErrorCode ConvertColorSpace(OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage);
    ImageProcessing_ErrorCode Compose(OH_PixelmapNative* sourceImage, OH_PixelmapNative* sourceGainmap,
        OH_PixelmapNative* destinationImage);
    ImageProcessing_ErrorCode Decompose(OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage,
        OH_PixelmapNative* destinationGainmap);
    ImageProcessing_ErrorCode GenerateMetadata(OH_PixelmapNative* sourceImage);
    ImageProcessing_ErrorCode EnhanceDetail(OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage);

protected:
    ImageProcessingNative() = default;
    virtual ~ImageProcessingNative() = default;
    ImageProcessingNative(const ImageProcessingNative&) = delete;
    ImageProcessingNative& operator=(const ImageProcessingNative&) = delete;
    ImageProcessingNative(ImageProcessingNative&&) = delete;
    ImageProcessingNative& operator=(ImageProcessingNative&&) = delete;

    static ImageProcessing_ErrorCode AlgoErrorToNdk(AlgoErrorCode errorCode);
    static ImageProcessing_ErrorCode GetSurfaceBufferFromPixelMap(const std::shared_ptr<PixelMap>& pixelMap,
        sptr<SurfaceBuffer>& surfaceBuffer);
    static ImageProcessing_ErrorCode GetSurfaceBufferFromPixelMapNoCopy(const std::shared_ptr<PixelMap>& pixelMap,
        sptr<SurfaceBuffer>& surfaceBuffer);
    static ImageProcessing_ErrorCode SetSurfaceBufferToPixelMap(const sptr<SurfaceBuffer>& surfaceBuffer,
        std::shared_ptr<PixelMap>& pixelMap);

    virtual ImageProcessing_ErrorCode InitializeInner();
    virtual ImageProcessing_ErrorCode DeinitializeInner();
    virtual ImageProcessing_ErrorCode SetParameter(const OHOS::Media::Format& parameter);
    virtual ImageProcessing_ErrorCode GetParameter(OHOS::Media::Format& parameter);
    virtual ImageProcessing_ErrorCode ConvertColorSpace(sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage);
    virtual ImageProcessing_ErrorCode Compose(sptr<SurfaceBuffer>& sourceImage, sptr<SurfaceBuffer>& sourceGainmap,
        sptr<SurfaceBuffer>& destinationImage);
    virtual ImageProcessing_ErrorCode Decompose(sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage, sptr<SurfaceBuffer>& destinationGainmap);
    virtual ImageProcessing_ErrorCode GenerateMetadata(sptr<SurfaceBuffer>& sourceImage);
    virtual ImageProcessing_ErrorCode EnhanceDetail(sptr<SurfaceBuffer>& sourceImage,
        sptr<SurfaceBuffer>& destinationImage);

private:
    static ImageProcessing_ErrorCode CreateSurfaceBufferFromPixelMap(const std::shared_ptr<PixelMap>& pixelMap,
        sptr<SurfaceBuffer>& surfaceBuffer);
    static ImageProcessing_ErrorCode ConvertPixelMapToSurfaceBuffer(const std::shared_ptr<PixelMap>& pixelMap,
        sptr<SurfaceBuffer>& surfaceBuffer);
    static ImageProcessing_ErrorCode CopyPixelMapToSurfaceBuffer(const std::shared_ptr<PixelMap>& pixelMap,
        sptr<SurfaceBuffer>& surfaceBuffer);
    static ImageProcessing_ErrorCode CopySurfaceBufferToPixelMap(const sptr<SurfaceBuffer>& surfaceBuffer,
        std::shared_ptr<PixelMap>& pixelMap);

    mutable std::mutex lock_{};
    // Guarded by lock_ begin
    bool isInitialized_{false};
    // Guarded by lock_ end
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // IMAGE_PROCESSING_NATIVE_H
