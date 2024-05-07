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

#ifndef I_MEDIA_MONITOR_INFO_H
#define I_MEDIA_MONITOR_INFO_H

#include <string>
#include "system_ability_definition.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

enum ModuleId {
    AUDIO = 0,
    MEDIA = 1,
};

enum EventId {
    // FAULT_EVENT
    LOAD_CONFIG_ERROR = 0,
    AUDIO_SERVICE_STARTUP_ERROR = 1,
    AUDIO_STREAM_EXHAUSTED_STATS = 2,
    AUDIO_STREAM_CREATE_ERROR_STATS = 3,
    // AGGREGATION_EVENT
    BACKGROUND_SILENT_PLAYBACK = 4,
    AUDIO_STREAM_UTILIZATION_STATS = 5,
    AUDIO_DEVICE_UTILIZATION_STATS = 6,
    BT_UTILIZATION_STATS = 7,
    PERFORMANCE_UNDER_OVERRUN_STATS = 8,
    PLAYBACK_VOLUME_STATS = 9,
    MUTED_CAPTURE_STATS = 10,
    // BEHAVIOR_EVENT
    DEVICE_CHANGE = 11,
    HEADSET_CHANGE = 12,
    VOLUME_CHANGE = 13,
    STREAM_CHANGE = 14,
    AUDIO_ROUTE_CHANGE = 15,
    AUDIO_PIPE_CHANGE = 16,
    AUDIO_FOCUS_MIGRATE = 17,
    SET_FORCE_USE_AUDIO_DEVICE = 18,

    // just for capturer mute status change
    CAPTURE_MUTE_STATUS_CHANGE = 19,
};

enum EventType {
    FAULT_EVENT = 0,
    BEHAVIOR_EVENT = 1,
    FREQUENCY_AGGREGATION_EVENT = 2,
    DURATION_AGGREGATION_EVENT = 3,
};

struct MonitorDeviceInfo {
    int32_t deviceType_ = -1;
    std::string deviceName_ = "";
    std::string address_ = "";
    int32_t deviceCategory_ = -1;
};

enum RendererState {
    /** INVALID state */
    RENDERER_INVALID = -1,
    /** Create New Renderer instance */
    RENDERER_NEW,
    /** Reneder Prepared state */
    RENDERER_PREPARED,
    /** Rendere Running state */
    RENDERER_RUNNING,
    /** Renderer Stopped state */
    RENDERER_STOPPED,
    /** Renderer Released state */
    RENDERER_RELEASED,
    /** Renderer Paused state */
    RENDERER_PAUSED
};

enum CapturerState {
    /** Capturer INVALID state */
    CAPTURER_INVALID = -1,
    /** Create new capturer instance */
    CAPTURER_NEW,
    /** Capturer Prepared state */
    CAPTURER_PREPARED,
    /** Capturer Running state */
    CAPTURER_RUNNING,
    /** Capturer Stopped state */
    CAPTURER_STOPPED,
    /** Capturer Released state */
    CAPTURER_RELEASED,
    /** Capturer Paused state */
    CAPTURER_PAUSED
};

enum AudioConfigType {
    AUDIO_CONVERTER_CONFIG = 0,
    AUDIO_DEVICE_PRIVACY = 1,
    AUDIO_EFFECT_CONFIG = 2,
    AUDIO_INTERRUPT_POLICY_CONFIG = 3,
    AUDIO_STRATEGY_ROUTER = 4,
    AUDIO_TONE_DTMF_CONFIG = 5,
    AUDIO_USAGE_STRATEGY = 6,
    AUDIO_VOLUME_CONFIG = 7,
};

enum ServiceId {
    AUDIO_SERVER_ID = OHOS::AUDIO_DISTRIBUTED_SERVICE_ID,
    AUDIO_POLICY_SERVICE_ID = OHOS::AUDIO_POLICY_SERVICE_ID,
};

enum ServiceType {
    AUDIO_SERVER = 0,
    AUDIO_POLICY_SERVER = 1,
    AUDIO_INTERRUPT_SERVER = 2,
    AUDIO_POLICY_SERVICE = 3,
};

enum PerferredType {
    MEDIA_RENDER = 0,
    CALL_RENDER = 1,
    CALL_CAPTURE = 2,
    RING_RENDER = 3,
    RECORD_CAPTURE = 4,
    TONE_RENDER = 5,
};

enum FocusDirection {
    LOCAL = 0,
    ROMOTE = 1,
};

enum PipeChangeReason {
    DEVICE_CHANGE_FROM_FAST = 0,
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MEDIA_MONITOR_INFO_H