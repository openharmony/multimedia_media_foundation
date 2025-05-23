/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGINS_EVENT_H
#define HISTREAMER_PLUGINS_EVENT_H

#include <cstdint> // NOLINT: using int32_t in this file
#include <string>
#include "meta/any.h"

namespace OHOS {
namespace Media {
namespace Plugins {
enum struct PluginEventType : int32_t {
    CLIENT_ERROR,
    SERVER_ERROR,
    OTHER_ERROR,
    BELOW_LOW_WATERLINE,
    ABOVE_LOW_WATERLINE,
    AUDIO_INTERRUPT,
    AUDIO_STATE_CHANGE,
    EVENT_CHANNEL_OPENED,
    EVENT_CHANNEL_OPEN_FAIL,
    EVENT_CHANNEL_CLOSED,
    SOURCE_DRM_INFO_UPDATE,
    BUFFERING_START,
    BUFFERING_PERCENT,
    BUFFERING_END,
    CACHED_DURATION,
    VIDEO_SIZE_CHANGE,
    SOURCE_BITRATE_START,
    EVENT_BUFFER_PROGRESS,
    INITIAL_BUFFER_SUCCESS,
    DASH_SEEK_READY,
    HLS_SEEK_READY,
    AUDIO_OUTPUT_FORMAT_CHANGED,
    FLV_AUTO_SELECT_BITRATE,
};

enum class NetworkClientErrorCode : int32_t {
    ERROR_OK,
    ERROR_TIME_OUT,
    ERROR_NOT_RETRY,
    ERROR_UNKNOWN,
};

using NetworkServerErrorCode = int;

struct PluginEvent {
    PluginEventType type;
    Any param;
    std::string description;
};

enum PluginDfxEventType : int32_t {
    EVENT_START = 0,

    PERF_START = 200,
    PERF_SOURCE,
    PERF_DEMUXER,
    PERF_VIDEO_DECODER,
    PERF_AUDIO_DECODER,
    PERF_VIDEO_SINK,
    PERF_AUDIO_SINK,
    PERF_VIDEO_RENDERER,
    PERF_AUDIO_RENDERER,
    PERF_MAX,

    EVENT_MAX,
};

struct PluginDfxEvent {
    PluginDfxEventType type;
    Any param;
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGINS_TYPES_H
