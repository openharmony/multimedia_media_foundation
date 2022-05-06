/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_COMMON_SOURCE_TAG_H
#define HISTREAMER_PLUGIN_COMMON_SOURCE_TAG_H

#include <cstdint> // NOLINT: using uint32_t

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @enum source input type.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct SrcInputType : uint32_t {
    UNKNOWN,         ///< Unknown audio input type
    AUD_MIC,         ///< Audio source input type of Microphone PCM data
    AUD_ES,          ///< Audio source input type of Raw encoded data
    VID_SURFACE_YUV, ///< Video source input type of YUV video data
    VID_SURFACE_RGB, ///< Video source input type of RGB video data
    VID_SURFACE_ES,  ///< Video source input type of Raw encoded data
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_COMMON_SOURCE_TAG_H