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

#ifndef SKIA_IMPL_H
#define SKIA_IMPL_H

#include "surface_buffer.h"
#include "SkYUVAPixmaps.h"

#include "algorithm_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class Skia {
public:
    static std::unique_ptr<Skia> Create();

    enum ImageFormatType {
        IMAGE_FORMAT_TYPE_UNKNOWN = 0,
        IMAGE_FORMAT_TYPE_RGB,
        IMAGE_FORMAT_TYPE_YUV,
    };

    Skia() = default;
    virtual ~Skia() = default;
    Skia(const Skia&) = delete;
    Skia& operator=(const Skia&) = delete;
    Skia(Skia&&) = delete;
    Skia& operator=(Skia&&) = delete;

    AlgoErrorCode Process(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output);

private:
    AlgoErrorCode RGBScale(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output);
    AlgoErrorCode YUVScale(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output);
    AlgoErrorCode PixmapScale(SkPixmap inputPixmap, SkPixmap outputPixmap, SkSamplingOptions options);
    SkColorType GetImageFormat(const sptr<SurfaceBuffer>& input);
    ImageFormatType GetImageType(const sptr<SurfaceBuffer>& input, const sptr<SurfaceBuffer>& output);
    SkColorType GetRGBImageFormat(const sptr<SurfaceBuffer>& surfaceBuffer);
    int CreateYUVPixmap(const sptr<SurfaceBuffer>& buffer, SkPixmap* pixmaps);
    AlgoErrorCode YUVPixmapScale(SkPixmap* inputPixmap, SkPixmap* outputPixmap, SkSamplingOptions opt, int numPlanes);
    int ConfigYUVFormat(const sptr<SurfaceBuffer>& buffer, SkYUVAInfo::PlaneConfig& planeConfig, size_t* rowbyte,
        unsigned char** pixmapAddr);
};
} // VideoProcessingEngine
} // Media
} // OHOS

#endif // SKIA_IMPL_H