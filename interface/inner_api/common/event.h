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

#ifndef HISTREAMER_PIPELINE_CORE_EVENT_H
#define HISTREAMER_PIPELINE_CORE_EVENT_H

#include <string>
#include <map>
#include "meta/any.h"

namespace OHOS {
namespace Media {
// 各个组件向Pipeline报告的事件类型
enum struct EventType : uint32_t {
    EVENT_READY = 0,
    EVENT_AUDIO_PROGRESS, // unit is HST_TIME_BASE
    EVENT_VIDEO_PROGRESS, // unit is HST_TIME_BASE
    EVENT_COMPLETE,
    EVENT_ERROR,
    EVENT_PLUGIN_ERROR,
    EVENT_PLUGIN_EVENT,
    EVENT_BUFFERING,
    EVENT_BUFFER_PROGRESS,
    EVENT_DECODER_ERROR,
    EVENT_RESOLUTION_CHANGE,
    EVENT_VIDEO_RENDERING_START,
    EVENT_IS_LIVE_STREAM,
    EVENT_DRM_INFO_UPDATED,
    EVENT_AUDIO_INTERRUPT,
    EVENT_AUDIO_STATE_CHANGE,
    EVENT_AUDIO_FIRST_FRAME,
    EVENT_AUDIO_DEVICE_CHANGE,
    EVENT_AUDIO_SERVICE_DIED,
    BUFFERING_START,
    BUFFERING_END,
    EVENT_CACHED_DURATION,
    EVENT_SOURCE_BITRATE_START,
    EVENT_SUBTITLE_TEXT_UPDATE,
    EVENT_AUDIO_TRACK_CHANGE,
    EVENT_VIDEO_TRACK_CHANGE,
    EVENT_SUBTITLE_TRACK_CHANGE,
    EVENT_SEI_INFO,
    EVENT_SUPER_RESOLUTION_CHANGED,
    EVENT_VIDEO_FIRST_FRAME,
    EVENT_FLV_AUTO_SELECT_BITRATE,
    EVENT_ANCHOR_UPDATE,
    EVENT_DATA_NEEDED,
    EVENT_FIRST_FRAME_READY,
    EVENT_RELEASE_VIDEO_DECODER,
    EVENT_VIDEO_NO_NEED_INIT,
    EVENT_DECODE_ERROR_FRAME,
    EVENT_VIDEO_DECODER_RESTART,
    EVENT_VIDEO_TARGET_ARRIVED,
};

// DFX events and infos reported from filters, modules, and plugins
enum struct DfxEventType : uint32_t {
    DFX_INFO_START = 0,
    DFX_INFO_PLAYER_VIDEO_LAG,
    DFX_INFO_PLAYER_AUDIO_LAG,
    DFX_INFO_PLAYER_STREAM_LAG,
    DFX_INFO_PLAYER_EOS_SEEK,
    DFX_INFO_PERF_REPORT,
    DFX_INFO_MEMORY_USAGE,
    DFX_EVENT_BUTT,
};

struct Event {
    std::string srcFilter;
    EventType type;
    Any param;
};

struct DfxEvent {
    std::string callerName;
    DfxEventType type;
    Any param;
};

const char* GetEventName(EventType type);
} // namespace Media
} // namespace OHOS
#endif
