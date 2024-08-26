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

#ifndef DETAIL_ENHANCER_H
#define DETAIL_ENHANCER_H

#include "refbase.h"
#include "surface_buffer.h"

#include "algorithm_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class DetailEnhancer {
public:
    enum Level {
        LEVEL_NONE = 0,
        LEVEL_LOW,
        LEVEL_MEDIUM,
        LEVEL_HIGH,
    };

    DetailEnhancer() = default;
    virtual ~DetailEnhancer() = default;
    DetailEnhancer(const DetailEnhancer&) = delete;
    DetailEnhancer& operator=(const DetailEnhancer&) = delete;
    DetailEnhancer(DetailEnhancer&&) = delete;
    DetailEnhancer& operator=(DetailEnhancer&&) = delete;

    AlgoErrorCode Initialize();
    AlgoErrorCode Deinitialize();
    AlgoErrorCode SetParameter(const Level& level);
    AlgoErrorCode GetParameter(Level& level);
    AlgoErrorCode Process(const sptr<SurfaceBuffer>& sourceImage, sptr<SurfaceBuffer>& destinationImage);

private:
    mutable std::mutex lock_{};
    // Guarded by lock_ begin
    bool isInitialized_{false};
    Level level_{LEVEL_LOW};
    // Guarded by lock_ end
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // DETAIL_ENHANCER_H
