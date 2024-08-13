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
#include "include/core/SkYUVAPixmaps.h"

#include "algorithm_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class Skia {
public:
    static AlgoErrorCode Process(const sptr<SurfaceBuffer>& input, sptr<SurfaceBuffer>& output);

private:
    Skia() = default;
    virtual ~Skia() = default;
    Skia(const Skia&) = delete;
    Skia& operator=(const Skia&) = delete;
    Skia(Skia&&) = delete;
    Skia& operator=(Skia&&) = delete;
};
} // VideoProcessingEngine
} // Media
} // OHOS

#endif // SKIA_IMPL_H