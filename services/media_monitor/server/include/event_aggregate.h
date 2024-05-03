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

#ifndef EVENT_AGGREGATE_H
#define EVENT_AGGREGATE_H

#include <mutex>
#include "event_bean.h"
#include "audio_memo.h"
#include "media_monitor_policy.h"
#include "audio_info.h"

using OHOS::AudioStandard::AudioStreamPolicyServiceDiedCallback;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class EventAggregate {
public:
    static EventAggregate& GetEventAggregate()
    {
        static EventAggregate eventAggregate;
        return eventAggregate;
    }

    EventAggregate();
    ~EventAggregate();

    void WriteEvent(std::shared_ptr<EventBean> &bean);

private:
    void UpdateAggregateEventList(std::shared_ptr<EventBean> &bean);
    void HandleDeviceChangeEvent(std::shared_ptr<EventBean> &bean);
    void HandleStreamChangeEvent(std::shared_ptr<EventBean> &bean);
    void HandleVolumeChange(std::shared_ptr<EventBean> &bean);
    void HandleBackgroundSilentPlayback(std::shared_ptr<EventBean> &bean);

    AudioMemo& audioMemo_;
    MediaMonitorPolicy& mediaMonitorPolicy_;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // EVENT_AGGREGATE_H