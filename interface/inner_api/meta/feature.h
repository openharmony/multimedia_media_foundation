/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef MEDIA_FOUNDATION_FEATURE_H
#define MEDIA_FOUNDATION_FEATURE_H

namespace OHOS {
namespace Media {
namespace Plugins {
class Feature {
public:
    static constexpr const char VIDEO_ENCODER_TEMPORAL_LEVEL_SCALE[] =
        "feature_video_encoder_temporal_level_scale"; ///< The string for querying feature of video temporal
                                                      ///< level scale
    static constexpr const char VIDEO_ENCODER_LONG_TERM_REFERENCE[] =
        "feature_video_encoder_long_term_reference"; ///< The string for querying feature of long term
                                                     ///< reference
    static constexpr const char VIDEO_LOW_LATENCY[] =
        "feature_video_low_latency"; ///< The string for querying video low latency
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_FEATURE_H