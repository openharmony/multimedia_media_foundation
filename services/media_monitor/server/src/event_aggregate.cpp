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

using OHOS::AudioStandard::AudioSystemManager;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

EventAggregate::EventAggregate()
    :audioMemo_(AudioMemo::GetAudioMemo()),
    mediaMonitorPolicy_(MediaMonitorPolicy::GetMediaMonitorPolicy())
{
    MEDIA_LOG_I("EventAggregate::EventAggregate");
}

EventAggregate::~EventAggregate()
{
    MEDIA_LOG_I("EventAggregate::~EventAggregate");
}

void EventAggregate::WriteEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write event");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }

    EventId eventId = bean->GetEventId();
    switch (eventId) {
        case HEADSET_CHANGE:
            mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
            break;
        default:
            UpdateAggregateEventList(bean);
            break;
    }
}

void EventAggregate::UpdateAggregateEventList(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Classify and process events");
    EventId eventId = bean->GetEventId();
    switch (eventId) {
        case DEVICE_CHANGE:
            HandleDeviceChangeEvent(bean);
            break;
        case STREAM_CHANGE:
            HandleStreamChangeEvent(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            HandleBackgroundSilentPlayback(bean);
            break;
        case VOLUME_CHANGE:
            HandleVolumeChange(bean);
        default:
            break;
    }
}

void EventAggregate::HandleDeviceChangeEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle device change event");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleStreamChangeEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle stream change event");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleVolumeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle volume change");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}

void EventAggregate::HandleBackgroundSilentPlayback(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Handle background silent playback");
    mediaMonitorPolicy_.WriteEvent(bean->GetEventId(), bean);
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS