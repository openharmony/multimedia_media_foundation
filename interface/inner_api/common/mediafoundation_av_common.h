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

#ifndef MEDIA_FOUNDATION_AV_COMMON_H
#define MEDIA_FOUNDATION_AV_COMMON_H

#include <string_view>

namespace OHOS {
namespace Media {
/**
 *  Media type
 */
enum MediaType : int32_t {
    /**
     * track is audio.
     */
    MEDIA_TYPE_AUD = 0,
    /**
     * track is video.
     */
    MEDIA_TYPE_VID = 1,
    /**
     * track is subtitle.
     */
    MEDIA_TYPE_SUBTITLE = 2,
};

/**
 * Enumerates the state change reason.
 *
 */
enum StateChangeReason {
    /**
     * audio/video state change by user
     */
    USER = 1,
    /**
     * audio/video state change by system
     */
    BACKGROUND = 2,
};
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_AV_COMMON_H
