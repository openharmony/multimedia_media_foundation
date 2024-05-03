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

#include "media_monitor_policy.h"
#include "log.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void MediaMonitorPolicy::WriteEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write event");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    if (bean->GetEventType() == BEHAVIOR_EVENT) {
        WriteBehaviorEvent(eventId, bean);
    } else if (bean->GetEventType() == FAULT_EVENT) {
        WriteFaultEvent(eventId, bean);
    } else {
        WriteAggregationEvent(eventId, bean);
    }
}

void MediaMonitorPolicy::WriteBehaviorEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write behavior event");
    switch (eventId) {
        case DEVICE_CHANGE:
            mediaEventBaseWriter_.WriteDeviceChange(bean);
            break;
        case HEADSET_CHANGE:
            mediaEventBaseWriter_.WriteHeasetChange(bean);
            break;
        case STREAM_CHANGE:
            mediaEventBaseWriter_.WriteStreamChange(bean);
            break;
        case VOLUME_CHANGE:
            mediaEventBaseWriter_.WriteVolumeChange(bean);
            break;
        case AUDIO_ROUTE_CHANGE:
            mediaEventBaseWriter_.WriteAudioRouteChange(bean);
            break;
        case AUDIO_PIPE_CHANGE:
            mediaEventBaseWriter_.WriteAudioPipeChange(bean);
            break;
        case AUDIO_FOCUS_MIGRATE:
            mediaEventBaseWriter_.WriteFocusMigrate(bean);
            break;
        case SET_FORCE_USE_AUDIO_DEVICE:
            mediaEventBaseWriter_.WriteSetForceDevice(bean);
            break;
        default:
            break;
    }
}

void MediaMonitorPolicy::WriteFaultEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write fault event");
    switch (eventId) {
        case LOAD_CONFIG_ERROR:
            mediaEventBaseWriter_.WriteLoadConfigError(bean);
            break;
        case AUDIO_SERVICE_STARTUP_ERROR:
            mediaEventBaseWriter_.WriteAudioStartupError(bean);
            break;
        case AUDIO_STREAM_EXHAUSTED_STATS:
            mediaEventBaseWriter_.WriteStreamExhastedError(bean);
            break;
        default:
            break;
    }
}

void MediaMonitorPolicy::WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write aggregation event");
    switch (eventId) {
        case AUDIO_STREAM_CREATE_ERROR_STATS:
            mediaEventBaseWriter_.WriteStreamCreateError(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            mediaEventBaseWriter_.WriteBackgoundSilentPlayback(bean);
            break;
        case AUDIO_STREAM_UTILIZATION_STATS:
            mediaEventBaseWriter_.WriteStreamStatistic(bean);
            break;
        case AUDIO_DEVICE_UTILIZATION_STATS:
             mediaEventBaseWriter_.WriteDeviceStatistic(bean);
            break;
        case BT_UTILIZATION_STATS:
            mediaEventBaseWriter_.WriteBtUsageStatistic(bean);
            break;
        case PERFORMANCE_UNDER_OVERRUN_STATS:
            mediaEventBaseWriter_.WriteUnderrunStatistic(bean);
            break;
        case PLAYBACK_VOLUME_STATS:
            mediaEventBaseWriter_.WritePlaybackVolume(bean);
            break;
        case MUTED_CAPTURE_STATS:
            mediaEventBaseWriter_.WriteMutedCapture(bean);
            break;
        default:
            break;
    }
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS