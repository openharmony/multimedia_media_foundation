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

#include "event_aggregate.h"
#include "log.h"
#include "media_monitor_info.h"
#include "monitor_utils.h"
#include "audio_system_manager.h"
#include "audio_device_info.h"

#include "iservice_registry.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "EventAggregate"};
}

using OHOS::AudioStandard::AudioSystemManager;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

static constexpr int32_t NEED_INCREASE_FREQUENCY = 30;
static constexpr int32_t UNINITIALIZED = -1;

EventAggregate::EventAggregate()
    :audioMemo_(AudioMemo::GetAudioMemo()),
    mediaMonitorPolicy_(MediaMonitorPolicy::GetMediaMonitorPolicy())
{
    MEDIA_LOG_D("EventAggregate Constructor");
}

EventAggregate::~EventAggregate()
{
    MEDIA_LOG_D("EventAggregate Destructor");
}

void EventAggregate::WriteEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("WriteEvent enter");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }

    EventId eventId = bean->GetEventId();
    switch (eventId) {
        case HEADSET_CHANGE:
        case AUDIO_ROUTE_CHANGE:
        case LOAD_CONFIG_ERROR:
        case AUDIO_SERVICE_STARTUP_ERROR:
        case STREAM_STANDBY:
        case SMARTPA_STATUS:
        case AI_VOICE_NOISE_SUPPRESSION:
            mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
            break;
        case LOAD_EFFECT_ENGINE_ERROR:
            mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
            break;
        default:
            UpdateAggregateEventList(bean);
            break;
    }
}

void EventAggregate::UpdateAggregateEventList(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Update Aggregate Event List");
    EventId eventId = bean->GetEventId();
    switch (eventId) {
        case DEVICE_CHANGE:
            HandleDeviceChangeEvent(bean);
            break;
        case STREAM_CHANGE:
            HandleStreamChangeEvent(bean);
            break;
        case AUDIO_STREAM_EXHAUSTED_STATS:
            HandleStreamExhaustedErrorEvent(bean);
            break;
        case AUDIO_STREAM_CREATE_ERROR_STATS:
            HandleStreamCreateErrorEvent(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            HandleBackgroundSilentPlayback(bean);
            break;
        case PERFORMANCE_UNDER_OVERRUN_STATS:
            HandleUnderrunStatistic(bean);
            break;
        case SET_FORCE_USE_AUDIO_DEVICE:
            HandleForceUseDevice(bean);
            break;
        case CAPTURE_MUTE_STATUS_CHANGE:
            HandleCaptureMutedStatusChange(bean);
            break;
        case VOLUME_CHANGE:
            HandleVolumeChange(bean);
            break;
        case AUDIO_PIPE_CHANGE:
            HandlePipeChange(bean);
            break;
        case AUDIO_FOCUS_MIGRATE:
            HandleFocusMigrate(bean);
            break;
        case JANK_PLAYBACK:
            HandleJankPlaybackEvent(bean);
            break;
        case EXCLUDE_OUTPUT_DEVICE:
            HandleExcludedOutputDevices(bean);
            break;
        default:
            break;
    }
}

void EventAggregate::HandleDeviceChangeEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Begin handle device change event");
    HandleDeviceChangeForDeviceUsage(bean);
    HandleDeviceChangeForCaptureMuted(bean);
    HandleDeviceChangeForVolume(bean);
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleDeviceChangeForDeviceUsage(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle device change for device event aggregate.");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &deviceUsageBean) {
        if (bean->GetIntValue("ISOUTPUT") == deviceUsageBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("STREAMID") == deviceUsageBean->GetIntValue("STREAMID")) {
            MEDIA_LOG_D("Find the existing device usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(deviceUsageVector_.begin(), deviceUsageVector_.end(), isExist);
    if (it != deviceUsageVector_.end()) {
        bean->Add("STREAM_TYPE", (*it)->GetIntValue("STREAM_TYPE"));
        HandleDeviceChangeForDuration(FOR_DEVICE_EVENT, bean, *it);
    }
}

void EventAggregate::HandleDeviceChangeForCaptureMuted(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle device change for capture muted event aggregate");
    if (bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_W("HandleDeviceChangeForCaptureMuted is playback");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &captureMutedBean) {
        if (bean->GetIntValue("STREAMID") == captureMutedBean->GetIntValue("STREAMID")) {
            MEDIA_LOG_D("Find the existing capture muted");
            return true;
        }
        return false;
    };
    auto it = std::find_if(captureMutedVector_.begin(), captureMutedVector_.end(), isExist);
    if (it != captureMutedVector_.end() && (*it)->GetIntValue("MUTED")) {
        HandleDeviceChangeForDuration(FOR_CAPTURE_MUTE_EVENT, bean, *it);
    }
}

void EventAggregate::HandleDeviceChangeForVolume(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle device change for volume event aggregate");
    if (!bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_D("HandleDeviceChangeForVolume is not playback");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &volumeBean) {
        if (bean->GetIntValue("STREAMID") == volumeBean->GetIntValue("STREAMID")) {
            MEDIA_LOG_D("Find the existing volume");
            return true;
        }
        return false;
    };
    auto it = std::find_if(volumeVector_.begin(), volumeVector_.end(), isExist);
    if (it != volumeVector_.end()) {
        HandleDeviceChangeForDuration(FOR_VOLUME_CHANGE_EVENT, bean, *it);
    }
}

void EventAggregate::HandleDeviceChangeForDuration(const DeviceChangeEvent &event,
    std::shared_ptr<EventBean> &bean, std::shared_ptr<EventBean> &beanInVector)
{
    if (bean->GetIntValue("DEVICETYPE") != beanInVector->GetIntValue("DEVICE_TYPE")) {
        uint64_t duration = TimeUtils::GetCurSec() - beanInVector->GetUint64Value("START_TIME");
        if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            beanInVector->Add("DURATION", duration);
            if (event == FOR_DEVICE_EVENT) {
                mediaMonitorPolicy_.HandDeviceUsageToEventVector(beanInVector);
            } else if (event == FOR_CAPTURE_MUTE_EVENT) {
                mediaMonitorPolicy_.HandleCaptureMutedToEventVector(beanInVector);
            } else if (event == FOR_VOLUME_CHANGE_EVENT) {
                mediaMonitorPolicy_.HandleVolumeToEventVector(beanInVector);
            }
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        beanInVector->UpdateIntMap("DEVICE_TYPE", bean->GetIntValue("DEVICETYPE"));
        beanInVector->UpdateUint64Map("START_TIME", TimeUtils::GetCurSec());
    }
}

void EventAggregate::HandleStreamChangeEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream change event");
    if (bean->GetIntValue("STATE") == AudioStandard::State::RUNNING) {
        MEDIA_LOG_D("Stream State RUNNING");
        uint64_t curruntTime = TimeUtils::GetCurSec();
        AddToDeviceUsage(bean, curruntTime);
        AddToStreamUsage(bean, curruntTime);
        AddToStreamPropertyVector(bean, curruntTime);
        AddToCaptureMuteUsage(bean, curruntTime);
        AddToVolumeVector(bean, curruntTime);
    } else if (bean->GetIntValue("STATE") == AudioStandard::State::STOPPED ||
                bean->GetIntValue("STATE") == AudioStandard::State::PAUSED ||
                bean->GetIntValue("STATE") == AudioStandard::State::RELEASED) {
        MEDIA_LOG_D("Stream State STOPPED/PAUSED/RELEASED");
        HandleDeviceUsage(bean);
        HandleStreamUsage(bean);
        HandleCaptureMuted(bean);
        HandleStreamChangeForVolume(bean);
        HandleStreamPropertyStats(bean);
    }
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::AddToDeviceUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime)
{
    MEDIA_LOG_D("Add to device usage from stream change event");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (bean->GetIntValue("ISOUTPUT") == eventBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("STREAMID") == eventBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("UID") == eventBean->GetIntValue("UID") &&
            bean->GetIntValue("PID") == eventBean->GetIntValue("PID") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("STATE") == eventBean->GetIntValue("STATE") &&
            bean->GetIntValue("DEVICETYPE") == eventBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing device usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(deviceUsageVector_.begin(), deviceUsageVector_.end(), isExist);
    if (it != deviceUsageVector_.end()) {
        MEDIA_LOG_D("The current device already exists, do not add it again");
        return;
    }
    std::shared_ptr<EventBean> deviceUsageBean = std::make_shared<EventBean>();
    int32_t deviceType = bean->GetIntValue("DEVICETYPE");
    deviceUsageBean->Add("IS_PLAYBACK", bean->GetIntValue("ISOUTPUT"));
    deviceUsageBean->Add("STREAMID", bean->GetIntValue("STREAMID"));
    deviceUsageBean->Add("UID", bean->GetIntValue("UID"));
    deviceUsageBean->Add("PID", bean->GetIntValue("PID"));
    deviceUsageBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
    deviceUsageBean->Add("STATE", bean->GetIntValue("STATE"));
    deviceUsageBean->Add("DEVICE_TYPE", deviceType);
    deviceUsageBean->Add("START_TIME", curruntTime);
    deviceUsageVector_.push_back(deviceUsageBean);
    if (deviceType == AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO ||
        deviceType == AudioStandard::DEVICE_TYPE_BLUETOOTH_A2DP) {
        deviceUsageBean->Add("BT_TYPE", bean->GetIntValue("BT_TYPE"));
    }
}

void EventAggregate::AddToStreamUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime)
{
    MEDIA_LOG_D("Add to stream usage from stream change event");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (bean->GetIntValue("STREAMID") == eventBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("UID") == eventBean->GetIntValue("UID") &&
            bean->GetIntValue("PID") == eventBean->GetIntValue("PID") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICETYPE") == eventBean->GetIntValue("DEVICE_TYPE") &&
            bean->GetIntValue("ISOUTPUT") == eventBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("PIPE_TYPE") == eventBean->GetIntValue("PIPE_TYPE") &&
            bean->GetIntValue("SAMPLE_RATE") == eventBean->GetIntValue("SAMPLE_RATE")) {
            MEDIA_LOG_D("Find the existing stream usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(streamUsageVector_.begin(), streamUsageVector_.end(), isExist);
    if (it != streamUsageVector_.end()) {
        MEDIA_LOG_D("The current stream already exists, do not add it again");
        return;
    }
    std::shared_ptr<EventBean> streamUsageBean = std::make_shared<EventBean>();
    streamUsageBean->Add("STREAMID", bean->GetIntValue("STREAMID"));
    streamUsageBean->Add("UID", bean->GetIntValue("UID"));
    streamUsageBean->Add("PID", bean->GetIntValue("PID"));
    streamUsageBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
    streamUsageBean->Add("DEVICE_TYPE", bean->GetIntValue("DEVICETYPE"));
    streamUsageBean->Add("IS_PLAYBACK", bean->GetIntValue("ISOUTPUT"));
    streamUsageBean->Add("PIPE_TYPE", bean->GetIntValue("PIPE_TYPE"));
    streamUsageBean->Add("SAMPLE_RATE", bean->GetIntValue("SAMPLE_RATE"));
    streamUsageBean->Add("APP_NAME", bean->GetStringValue("APP_NAME"));
    streamUsageBean->Add("STATE", bean->GetIntValue("STATE"));
    streamUsageBean->Add("EFFECT_CHAIN", bean->GetIntValue("EFFECT_CHAIN"));
    streamUsageBean->Add("START_TIME", curruntTime);
    streamUsageVector_.push_back(streamUsageBean);
}

void EventAggregate::AddToStreamPropertyVector(std::shared_ptr<EventBean> &bean, uint64_t curruntTime)
{
    MEDIA_LOG_D("Add to stream prorerty vector from stream change event");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (bean->GetUint64Value("CHANNEL_LAYOUT") == eventBean->GetUint64Value("CHANNEL_LAYOUT") &&
            bean->GetIntValue("UID") == eventBean->GetIntValue("UID") &&
            bean->GetIntValue("ENCODING_TYPE") == eventBean->GetIntValue("ENCODING_TYPE") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("ISOUTPUT") == eventBean->GetIntValue("IS_PLAYBACK")) {
            MEDIA_LOG_D("Find the existing stream property");
            return true;
        }
        return false;
    };
    auto it = std::find_if(streamPropertyVector_.begin(), streamPropertyVector_.end(), isExist);
    if (it != streamPropertyVector_.end()) {
        MEDIA_LOG_D("The current stream property already exists, do not add it again");
        return;
    }
    std::shared_ptr<EventBean> streamPropertyBean = std::make_shared<EventBean>();
    streamPropertyBean->Add("ENCODING_TYPE", bean->GetIntValue("ENCODING_TYPE"));
    streamPropertyBean->Add("UID", bean->GetIntValue("UID"));
    streamPropertyBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
    streamPropertyBean->Add("IS_PLAYBACK", bean->GetIntValue("ISOUTPUT"));
    streamPropertyBean->Add("CHANNEL_LAYOUT", bean->GetUint64Value("CHANNEL_LAYOUT"));
    streamPropertyBean->Add("APP_NAME", bean->GetStringValue("APP_NAME"));
    streamPropertyBean->Add("STATE", bean->GetIntValue("STATE"));
    streamPropertyBean->Add("START_TIME", curruntTime);
    streamPropertyVector_.push_back(streamPropertyBean);
}

void EventAggregate::AddToCaptureMuteUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime)
{
    MEDIA_LOG_D("Add to capture mute usage from stream change event");
    if (bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_D("AddToCaptureMuteUsage is playback");
        return;
    }
    if (!bean->GetIntValue("MUTED")) {
        MEDIA_LOG_D("AddToCaptureMuteUsage is not muted");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (bean->GetIntValue("STREAMID") == eventBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICETYPE") == eventBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing capture muted usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(captureMutedVector_.begin(), captureMutedVector_.end(), isExist);
    if (it != captureMutedVector_.end()) {
        MEDIA_LOG_D("The current capture already exists, do not add it again");
        return;
    }
    std::shared_ptr<EventBean> captureMutedBean = std::make_shared<EventBean>();
    captureMutedBean->Add("STREAMID", bean->GetIntValue("STREAMID"));
    captureMutedBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
    captureMutedBean->Add("DEVICE_TYPE", bean->GetIntValue("DEVICETYPE"));
    captureMutedBean->Add("START_TIME", curruntTime);
    captureMutedVector_.push_back(captureMutedBean);
}

void EventAggregate::AddToVolumeVector(std::shared_ptr<EventBean> &bean, uint64_t curruntTime)
{
    MEDIA_LOG_D("Add to volume vector from stream change event");
    if (!bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_D("EventAggregate AddToVolumeVector is not playback");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &volumeBean) {
        if (bean->GetIntValue("STREAMID") == volumeBean->GetIntValue("STREAMID")) {
            MEDIA_LOG_D("Find the existing capture volume vector");
            return true;
        }
        return false;
    };
    auto it = std::find_if(volumeVector_.begin(), volumeVector_.end(), isExist);
    if (it != volumeVector_.end()) {
        MEDIA_LOG_D("The current volume already exists, do not add it again");
        return;
    }
    std::shared_ptr<EventBean> volumeBean = std::make_shared<EventBean>();
    volumeBean->Add("STREAMID", bean->GetIntValue("STREAMID"));
    volumeBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
    volumeBean->Add("DEVICE_TYPE", bean->GetIntValue("DEVICETYPE"));
    volumeBean->Add("LEVEL", systemVol_);
    volumeBean->Add("START_TIME", TimeUtils::GetCurSec());
    volumeVector_.push_back(volumeBean);
}

void EventAggregate::HandleDeviceUsage(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle device usage");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &deviceUsageBean) {
        if (bean->GetIntValue("STREAMID") == deviceUsageBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("UID") == deviceUsageBean->GetIntValue("UID") &&
            bean->GetIntValue("PID") == deviceUsageBean->GetIntValue("PID") &&
            bean->GetIntValue("STREAM_TYPE") == deviceUsageBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICETYPE") == deviceUsageBean->GetIntValue("DEVICE_TYPE") &&
            bean->GetIntValue("ISOUTPUT") == deviceUsageBean->GetIntValue("IS_PLAYBACK")) {
            MEDIA_LOG_D("Find the existing device usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(deviceUsageVector_.begin(), deviceUsageVector_.end(), isExist);
    if (it != deviceUsageVector_.end()) {
        uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
        int32_t deviceType = (*it)->GetIntValue("DEVICE_TYPE");
        if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            (*it)->Add("DURATION", duration);
            mediaMonitorPolicy_.HandDeviceUsageToEventVector(*it);
            if (deviceType == AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO ||
                deviceType == AudioStandard::DEVICE_TYPE_BLUETOOTH_A2DP) {
                mediaMonitorPolicy_.HandBtUsageToEventVector(*it);
            }
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        deviceUsageVector_.erase(it);
    }
}

void EventAggregate::HandleStreamUsage(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream usage");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &streamUsageBean) {
        if (bean->GetIntValue("STREAMID") == streamUsageBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("UID") == streamUsageBean->GetIntValue("UID") &&
            bean->GetIntValue("PID") == streamUsageBean->GetIntValue("PID") &&
            bean->GetIntValue("STREAM_TYPE") == streamUsageBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("ISOUTPUT") == streamUsageBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("PIPE_TYPE") == streamUsageBean->GetIntValue("PIPE_TYPE") &&
            bean->GetIntValue("SAMPLE_RATE") == streamUsageBean->GetIntValue("SAMPLE_RATE")) {
            MEDIA_LOG_D("Find the existing stream usage");
            return true;
        }
        return false;
    };
    auto it = std::find_if(streamUsageVector_.begin(), streamUsageVector_.end(), isExist);
    if (it != streamUsageVector_.end()) {
        uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
        if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            (*it)->Add("DURATION", duration);
            mediaMonitorPolicy_.HandStreamUsageToEventVector(*it);
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        streamUsageVector_.erase(it);
    }
}

void EventAggregate::HandleStreamPropertyStats(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream property stats");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &streamPropertyBean) {
        if (bean->GetUint64Value("CHANNEL_LAYOUT") == streamPropertyBean->GetUint64Value("CHANNEL_LAYOUT") &&
            bean->GetIntValue("UID") == streamPropertyBean->GetIntValue("UID") &&
            bean->GetIntValue("ENCODING_TYPE") == streamPropertyBean->GetIntValue("ENCODING_TYPE") &&
            bean->GetIntValue("STREAM_TYPE") == streamPropertyBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("ISOUTPUT") == streamPropertyBean->GetIntValue("IS_PLAYBACK")) {
            MEDIA_LOG_D("Find the existing stream property");
            return true;
        }
        return false;
    };
    auto it = std::find_if(streamPropertyVector_.begin(), streamPropertyVector_.end(), isExist);
    if (it != streamPropertyVector_.end()) {
        uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
        if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            (*it)->Add("DURATION", duration);
            mediaMonitorPolicy_.HandStreamPropertyToEventVector(*it);
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        streamPropertyVector_.erase(it);
    }
}

void EventAggregate::HandleCaptureMuted(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle capture muted");
    if (bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_D("HandleCaptureMuted is playback");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &captureMutedBean) {
        if (bean->GetIntValue("STREAMID") == captureMutedBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("STREAM_TYPE") == captureMutedBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICETYPE") == captureMutedBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing capture muted");
            return true;
        }
        return false;
    };
    auto it = std::find_if(captureMutedVector_.begin(), captureMutedVector_.end(), isExist);
    if (it != captureMutedVector_.end()) {
        uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
        if ((*it)->GetIntValue("MUTED") && duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            (*it)->Add("DURATION", duration);
            mediaMonitorPolicy_.HandleCaptureMutedToEventVector(*it);
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        captureMutedVector_.erase(it);
    }
}

void EventAggregate::HandleStreamChangeForVolume(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream Change for volume");
    auto isExist = [&bean](const std::shared_ptr<EventBean> &volumeBean) {
        if (bean->GetIntValue("ISOUTPUT") &&
            bean->GetIntValue("STREAMID") == volumeBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("STREAM_TYPE") == volumeBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICETYPE") == volumeBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing volume vector");
            return true;
        }
        return false;
    };

    auto it = std::find_if(volumeVector_.begin(), volumeVector_.end(), isExist);
    if (it != volumeVector_.end()) {
        uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
        if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY)) {
            (*it)->Add("DURATION", duration);
            mediaMonitorPolicy_.HandleVolumeToEventVector(*it);
            mediaMonitorPolicy_.WhetherToHiSysEvent();
        }
        volumeVector_.erase(it);
    }
}

void EventAggregate::HandleCaptureMutedStatusChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle capture muted status change");
    if (!bean->GetIntValue("MUTED")) {
        HandleCaptureMuted(bean);
    } else {
        uint64_t curruntTime = TimeUtils::GetCurSec();
        AddToCaptureMuteUsage(bean, curruntTime);
    }
}

void EventAggregate::HandleVolumeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle volume change");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);

    if (!bean->GetIntValue("ISOUTPUT")) {
        MEDIA_LOG_D("EventAggregate HandleVolumeChange is not playback");
        return;
    }
    auto isExist = [&bean](const std::shared_ptr<EventBean> &volumeBean) {
        if (bean->GetIntValue("STREAMID") == volumeBean->GetIntValue("STREAMID") &&
            bean->GetIntValue("SYSVOLUME") != volumeBean->GetIntValue("LEVEL")) {
            MEDIA_LOG_D("Find the existing volume vector");
            return true;
        }
        return false;
    };
    auto it = std::find_if(volumeVector_.begin(), volumeVector_.end(), isExist);
    if (it != volumeVector_.end()) {
        if ((*it)->GetUint64Value("START_TIME") >= 0) {
            uint64_t duration = TimeUtils::GetCurSec() - (*it)->GetUint64Value("START_TIME");
            if (duration > 0 && (static_cast<int64_t>(duration) > NEED_INCREASE_FREQUENCY) &&
                (*it)->GetIntValue("LEVEL") != UNINITIALIZED) {
                (*it)->Add("DURATION", duration);
                mediaMonitorPolicy_.HandleVolumeToEventVector(*it);
                mediaMonitorPolicy_.WhetherToHiSysEvent();
            }
            (*it)->UpdateIntMap("LEVEL", bean->GetIntValue("SYSVOLUME"));
        }
    }
    // Record volume for stream state 2->5->2
    systemVol_ = bean->GetIntValue("SYSVOLUME");
}

void EventAggregate::HandlePipeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle pipe change");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleStreamExhaustedErrorEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream exhausted error event");
    mediaMonitorPolicy_.HandleExhaustedToEventVector(bean);
    mediaMonitorPolicy_.WhetherToHiSysEvent();
}

void EventAggregate::HandleStreamCreateErrorEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream create error event");
    mediaMonitorPolicy_.HandleCreateErrorToEventVector(bean);
    mediaMonitorPolicy_.WhetherToHiSysEvent();
}

void EventAggregate::HandleBackgroundSilentPlayback(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle background silent playback");
    mediaMonitorPolicy_.HandleSilentPlaybackToEventVector(bean);
    mediaMonitorPolicy_.WhetherToHiSysEvent();
    bean->SetEventType(Media::MediaMonitor::BEHAVIOR_EVENT); // report behavior event BG_SILENT_PLAYBACK
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleUnderrunStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle underrun statistic");
    mediaMonitorPolicy_.HandleUnderrunToEventVector(bean);
    mediaMonitorPolicy_.WhetherToHiSysEvent();
}

void EventAggregate::HandleForceUseDevice(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle force use device");
    audioMemo_.UpdataRouteInfo(bean);
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleFocusMigrate(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle focus use migrate");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleJankPlaybackEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle jank playback event");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleExcludedOutputDevices(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle exclude output devices");
    audioMemo_.UpdateExcludedDevice(bean);
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::WriteInfo(int32_t fd, std::string &dumpString)
{
    if (fd != -1) {
        mediaMonitorPolicy_.WriteInfo(fd, dumpString);
    }
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS