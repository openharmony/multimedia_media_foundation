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

enum DeviceChangeEvent {
    FOR_DEVICE_EVENT = 0,
    FOR_CAPTURE_MUTE_EVENT,
    FOR_VOLUME_CHANGE_EVENT
};

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

    void WriteInfo(int32_t fd, std::string &dumpString);

private:
    void UpdateAggregateEventList(std::shared_ptr<EventBean> &bean);
    void HandleDeviceChangeEvent(std::shared_ptr<EventBean> &bean);
    void HandleStreamChangeEvent(std::shared_ptr<EventBean> &bean);

    void HandleStreamExhaustedErrorEvent(std::shared_ptr<EventBean> &bean);
    void HandleStreamCreateErrorEvent(std::shared_ptr<EventBean> &bean);
    void HandleBackgroundSilentPlayback(std::shared_ptr<EventBean> &bean);
    void HandleUnderrunStatistic(std::shared_ptr<EventBean> &bean);
    void HandleForceUseDevice(std::shared_ptr<EventBean> &bean);
    void HandleCaptureMutedStatusChange(std::shared_ptr<EventBean> &bean);
    void HandleVolumeChange(std::shared_ptr<EventBean> &bean);

    void HandleCaptureMuted(std::shared_ptr<EventBean> &bean);
    void HandleDeviceUsage(std::shared_ptr<EventBean> &bean);
    void HandleStreamUsage(std::shared_ptr<EventBean> &bean);
    void HandleStreamChangeForVolume(std::shared_ptr<EventBean> &bean);
    void HandlePipeChange(std::shared_ptr<EventBean> &bean);
    void HandleFocusMigrate(std::shared_ptr<EventBean> &bean);
    void HandleJankPlaybackEvent(std::shared_ptr<EventBean> &bean);

    void AddToDeviceUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime);
    void AddToStreamUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime);
    void AddToCaptureMuteUsage(std::shared_ptr<EventBean> &bean, uint64_t curruntTime);
    void AddToVolumeVector(std::shared_ptr<EventBean> &bean, uint64_t curruntTime);
    void AddToStreamPropertyVector(std::shared_ptr<EventBean> &bean, uint64_t curruntTime);

    void HandleDeviceChangeForDeviceUsage(std::shared_ptr<EventBean> &bean);
    void HandleDeviceChangeForCaptureMuted(std::shared_ptr<EventBean> &bean);
    void HandleDeviceChangeForVolume(std::shared_ptr<EventBean> &bean);
    void HandleDeviceChangeForDuration(const DeviceChangeEvent &event,
        std::shared_ptr<EventBean> &bean, std::shared_ptr<EventBean> &beanInVector);
    void HandleStreamPropertyStats(std::shared_ptr<EventBean> &bean);

    AudioMemo& audioMemo_;
    MediaMonitorPolicy& mediaMonitorPolicy_;

    std::vector<std::shared_ptr<EventBean>> deviceUsageVector_;
    std::vector<std::shared_ptr<EventBean>> streamUsageVector_;
    std::vector<std::shared_ptr<EventBean>> captureMutedVector_;
    std::vector<std::shared_ptr<EventBean>> volumeVector_;
    std::vector<std::shared_ptr<EventBean>> streamPropertyVector_;

    // Record volume for stream state 2->5->2
    int32_t systemVol_ = -1;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // EVENT_AGGREGATE_H