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

#ifndef MEDIA_MONITOR_POLICY_H
#define MEDIA_MONITOR_POLICY_H

#include <thread>
#include <chrono>
#include <atomic>
#include <map>
#include <vector>
#include "event_bean.h"
#include "monitor_utils.h"
#include "media_event_base_writer.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorPolicy {
public:
    static MediaMonitorPolicy& GetMediaMonitorPolicy()
    {
        static MediaMonitorPolicy mediaMonitorPolicy;
        return mediaMonitorPolicy;
    }

    MediaMonitorPolicy();
    ~MediaMonitorPolicy();

    void WriteEvent(EventId eventId, std::shared_ptr<EventBean> &bean);

    void WriteBehaviorEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteFaultEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean);

    void HandDeviceUsageToEventVector(std::shared_ptr<EventBean> &deviceUsage);
    void HandStreamUsageToEventVector(std::shared_ptr<EventBean> &streamUsage);
    void HandBtUsageToEventVector(std::shared_ptr<EventBean> &btUsage);
    void AddToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleExhaustedToEventVector(const std::string &appName);
    void HandleCreateErrorToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleSilentPlaybackToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleUnderrunToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleCaptureMutedToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleVolumeToEventVector(std::shared_ptr<EventBean> &bean);

    void WhetherToHiSysEvent();
    void WriteInfo(int32_t fd, std::string &dumpString);

private:
    static constexpr int32_t DEFAULT_AGGREGATION_FREQUENCY = 1000;
    static constexpr int32_t DEFAULT_AGGREGATION_TIME = 24 * 60;

    void ReadParameter();
    void StartTimeThread();
    void StopTimeThread();
    void TimeFunc();
    void HandleToHiSysEvent();

    uint64_t curruntTime_ = 0;
    std::unique_ptr<std::thread> timeThread_ = nullptr;
    std::atomic_bool startThread_ = true;

    MediaEventBaseWriter& mediaEventBaseWriter_;

    std::mutex eventVectorMutex_;
    std::vector<std::shared_ptr<EventBean>> eventVector_;

    int32_t aggregationFrequency_ = DEFAULT_AGGREGATION_FREQUENCY;
    int32_t aggregationTime_ = DEFAULT_AGGREGATION_TIME;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_POLICY_H