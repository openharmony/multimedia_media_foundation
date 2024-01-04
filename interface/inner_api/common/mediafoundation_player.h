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

#ifndef MEDIA_FOUNDATION_PLAYER_H
#define MEDIA_FOUNDATION_PLAYER_H

#include <string_view>

namespace OHOS {
namespace Media {
enum BufferingInfoType : int32_t {
    /* begin to b buffering */
    BUFFERING_START = 1,
    /* end to buffering */
    BUFFERING_END = 2,
    /* buffering percent */
    BUFFERING_PERCENT = 3,
    /* cached duration in milliseconds */
    CACHED_DURATION = 4,
};

enum PlayerSeekMode : int32_t {
    /* sync to keyframes after the time point. */
    SEEK_NEXT_SYNC = 0,
    /* sync to keyframes before the time point. */
    SEEK_PREVIOUS_SYNC,
    /* sync to closest keyframes. */
    SEEK_CLOSEST_SYNC,
    /* seek to frames closest the time point. */
    SEEK_CLOSEST,
};
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_PLAYER_H
