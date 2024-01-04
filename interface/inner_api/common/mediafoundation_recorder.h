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

#ifndef MEDIA_FOUNDATION_RECORDER_H
#define MEDIA_FOUNDATION_RECORDER_H

#include <string_view>

namespace OHOS {
namespace Media {
/**
 * @brief Enumerates the container format types.
 */
class ContainerFormatType {
public:
    static constexpr std::string_view CFT_MPEG_4A = "m4a";
    static constexpr std::string_view CFT_MPEG_4 = "mp4";
};
/**
 * @brief the struct of geolocation
 *
 * @param latitude float: latitude in degrees. Its value must be in the range [-90, 90].
 * @param longitude float: longitude in degrees. Its value must be in the range [-180, 180].
 */
struct Location {
    int32_t latitude = 0;
    int32_t longitude = 0;
};
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_RECORDER_H
