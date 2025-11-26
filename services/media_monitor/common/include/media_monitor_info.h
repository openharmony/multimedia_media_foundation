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
#include "parcel.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

enum ModuleId {
    UNKNOW_MODULEID = -1,
    AUDIO = 0,
    MEDIA = 1,
};

enum EventId {
    UNKNOW_EVENTID = -1,
    // FAULT_EVENT
    LOAD_CONFIG_ERROR = 0,
    LOAD_EFFECT_ENGINE_ERROR = 1,
    AUDIO_SERVICE_STARTUP_ERROR = 2,
    AUDIO_STREAM_EXHAUSTED_STATS = 3,
    AUDIO_STREAM_CREATE_ERROR_STATS = 4,
    // AGGREGATION_EVENT
    BACKGROUND_SILENT_PLAYBACK = 5,
    STREAM_UTILIZATION_STATS = 6,
    AUDIO_DEVICE_UTILIZATION_STATS = 7,
    BT_UTILIZATION_STATS = 8,
    PERFORMANCE_UNDER_OVERRUN_STATS = 9,
    PLAYBACK_VOLUME_STATS = 10,
    MUTED_CAPTURE_STATS = 11,
    STREAM_PROPERTY_STATS = 12,
    // BEHAVIOR_EVENT
    DEVICE_CHANGE = 13,
    HEADSET_CHANGE = 14,
    VOLUME_CHANGE = 15,
    STREAM_CHANGE = 16,
    AUDIO_ROUTE_CHANGE = 17,
    AUDIO_PIPE_CHANGE = 18,
    AUDIO_FOCUS_MIGRATE = 19,
    SET_FORCE_USE_AUDIO_DEVICE = 20,

    // just for capturer mute status change
    CAPTURE_MUTE_STATUS_CHANGE = 21,

    STREAM_STANDBY = 22,
    AI_VOICE_NOISE_SUPPRESSION = 23,
    VOLUME_SUBSCRIBE = 24,
    SMARTPA_STATUS = 25,
    JANK_PLAYBACK = 26,

    EXCLUDE_OUTPUT_DEVICE = 27,
    HAPTIC_PLAYER = 28,
    SYSTEM_TONE_PLAYBACK = 29,
    ADD_REMOVE_CUSTOMIZED_TONE = 30,
    RECORD_ERROR = 31,
    STREAM_OCCUPANCY = 32,
    APP_WRITE_MUTE = 33,
    HDI_EXCEPTION = 34,
    DB_ACCESS_EXCEPTION = 35,
    DEVICE_CHANGE_EXCEPTION = 36,

    // for storing collaborative service state
    SET_DEVICE_COLLABORATIVE_STATE = 37,
    APP_BACKGROUND_STATE = 38,
    HPAE_MESSAGE_QUEUE_EXCEPTION = 39,
    STREAM_MOVE_EXCEPTION = 40,

    VOLUME_API_INVOKE = 41,
    HAP_CALL_AUDIO_SESSION = 42,
    PROCESS_IN_MAINTHREAD = 43,
    DISTRIBUTED_DEVICE_INFO = 44,
    DISTRIBUTED_SCENE_INFO = 45,

    // for audio suite
    SUITE_ENGINE_UTILIZATION_STATS = 46,
    SUITE_ENGINE_EXCEPTION = 47,

    DM_DEVICE_INFO = 48,

    VOLUME_SETTING_STATISTICS = 51,

    MUTE_BUNDLE_NAME = 50,
};

enum VolumeStatisticsSceneType : uint8_t {
    LOUD_VOLUME_SCENE = 0,
};

enum EventType {
    UNKNOW_EVENTTYPE = -1,
    FAULT_EVENT = 0,
    BEHAVIOR_EVENT = 1,
    FREQUENCY_AGGREGATION_EVENT = 2,
    DURATION_AGGREGATION_EVENT = 3,
    HAPTIC_PLAYER_EVENT = 4,
    MESSAGE_ZONE_EVENT = 5,
};

struct MonitorDeviceInfo : public Parcelable {
    int32_t deviceType_ = -1;
    int32_t deviceCategory_ = -1;
    int32_t usageOrSourceType_ = -1;
    int32_t audioDeviceUsage_ = -1;
    std::string deviceName_ = "";
    std::string address_ = "";
    std::string networkId_ = "";

    bool Marshalling(Parcel &parcel) const override
    {
        return parcel.WriteInt32(deviceType_) &&
            parcel.WriteInt32(deviceCategory_) &&
            parcel.WriteInt32(usageOrSourceType_) &&
            parcel.WriteInt32(audioDeviceUsage_) &&
            parcel.WriteString(deviceName_) &&
            parcel.WriteString(address_) &&
            parcel.WriteString(networkId_);
    }

    static MonitorDeviceInfo *Unmarshalling(Parcel &data)
    {
        MonitorDeviceInfo *monitorDeviceInfo = new (std::nothrow) MonitorDeviceInfo();
        if (monitorDeviceInfo == nullptr) {
            return nullptr;
        }
        data.ReadInt32(monitorDeviceInfo->deviceType_);
        data.ReadInt32(monitorDeviceInfo->deviceCategory_);
        data.ReadInt32(monitorDeviceInfo->usageOrSourceType_);
        data.ReadInt32(monitorDeviceInfo->audioDeviceUsage_);
        data.ReadString(monitorDeviceInfo->deviceName_);
        data.ReadString(monitorDeviceInfo->address_);
        data.ReadString(monitorDeviceInfo->networkId_);
        return monitorDeviceInfo;
    }
};

struct MonitorAppStateInfo : public Parcelable {
    int32_t isFreeze_ = -1;
    int32_t isBack_ = -1;
    int32_t hasSession_ = -1;
    int32_t hasBackTask_ = -1;
    int32_t isBinder_ = -1;

    bool Marshalling(Parcel &parcel) const override
    {
        return parcel.WriteInt32(isFreeze_) &&
            parcel.WriteInt32(isBack_) &&
            parcel.WriteInt32(hasSession_) &&
            parcel.WriteInt32(hasBackTask_) &&
            parcel.WriteInt32(isBinder_);
    }

    static MonitorAppStateInfo *Unmarshalling(Parcel &data)
    {
        MonitorAppStateInfo *monitorAppStateInfo = new (std::nothrow) MonitorAppStateInfo();
        if (monitorAppStateInfo == nullptr) {
            return nullptr;
        }
        data.ReadInt32(monitorAppStateInfo->isFreeze_);
        data.ReadInt32(monitorAppStateInfo->isBack_);
        data.ReadInt32(monitorAppStateInfo->hasSession_);
        data.ReadInt32(monitorAppStateInfo->hasBackTask_);
        data.ReadInt32(monitorAppStateInfo->isBinder_);
        return monitorAppStateInfo;
    }
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

enum PreferredType {
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

enum EffectEngineType {
    AUDIO_EFFECT_PROCESS_ENGINE = 0,
    AUDIO_CONVERTER_ENGINE = 1,
};

enum AudioDeviceUsage {
    MEDIA_OUTPUT_DEVICES = 1,
    MEDIA_INPUT_DEVICES = 2,
    ALL_MEDIA_DEVICES = 3,
    CALL_OUTPUT_DEVICES = 4,
    CALL_INPUT_DEVICES = 8,
    ALL_CALL_DEVICES = 12,
    D_ALL_DEVICES = 15,
};

struct MonitorDmDeviceInfo : public Parcelable {
    std::string deviceName_;
    std::string networkId_;
    uint16_t dmDeviceType_{0};

    bool Marshalling(Parcel &parcel) const override
    {
        return parcel.WriteString(deviceName_) &&
            parcel.WriteString(networkId_) &&
            parcel.WriteUint16(dmDeviceType_);
    }

    static MonitorDmDeviceInfo *Unmarshalling(Parcel &data)
    {
        MonitorDmDeviceInfo *monitorDmDeviceInfo = new (std::nothrow) MonitorDmDeviceInfo();
        if (monitorDmDeviceInfo == nullptr) {
            return nullptr;
        }
        data.ReadString(monitorDmDeviceInfo->deviceName_);
        data.ReadString(monitorDmDeviceInfo->networkId_);
        data.ReadUint16(monitorDmDeviceInfo->dmDeviceType_);
        return monitorDmDeviceInfo;
    }
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MEDIA_MONITOR_INFO_H