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
#include <queue>
#include <unordered_set>
#include "event_bean.h"
#include "monitor_utils.h"
#include "media_event_base_writer.h"
#include "media_monitor_wrapper.h"

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
    void WriteBehaviorEventExpansion(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteFaultEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteFaultEventExpansion(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteAggregationEventExpansion(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteSystemTonePlaybackEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void TriggerSystemTonePlaybackEvent(std::shared_ptr<EventBean> &bean);
    void CollectDataToDfxResult(DfxSystemTonePlaybackResult *result);

    void HandDeviceUsageToEventVector(std::shared_ptr<EventBean> &deviceUsage);
    void HandStreamUsageToEventVector(std::shared_ptr<EventBean> &streamUsage);
    void HandBtUsageToEventVector(std::shared_ptr<EventBean> &btUsage);
    void AddToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleExhaustedToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleCreateErrorToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleSilentPlaybackToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleUnderrunToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleCaptureMutedToEventVector(std::shared_ptr<EventBean> &bean);
    void HandleVolumeToEventVector(std::shared_ptr<EventBean> &bean);
    void HandStreamPropertyToEventVector(std::shared_ptr<EventBean> &streamProperty);
    void TriggerSystemTonePlaybackTimeEvent(std::shared_ptr<EventBean> &bean);
    void AddToVolumeApiInvokeQueue(std::shared_ptr<EventBean> &bean);
    void AddToCallSessionQueue(std::shared_ptr<EventBean> &bean);
    void AddAudioInterruptErrorEvent(std::shared_ptr<EventBean> &bean);
    void AddToSuiteEngineNodeStatsMap(std::shared_ptr<EventBean> &bean);
    void HandleVolumeSettingStatistics(std::shared_ptr<EventBean> &bean);
    void AddLoudVolumeTimes();

    void WhetherToHiSysEvent();
    void WriteInfo(int32_t fd, std::string &dumpString);

private:
    static constexpr int32_t DEFAULT_DAILY_TIME_SECEND = 2 * 60 * 60;
    static constexpr int32_t DEFAULT_AGGREGATION_FREQUENCY = 1000;
    static constexpr int32_t DEFAULT_AGGREGATION_TIME = 24 * 60;
    static constexpr int32_t DEFAULT_TONE_PLAYBACK_TIME = 2 * 60;
    static constexpr int32_t DEFAULT_VOLUME_API_INVOKE_TIME = 24 * 60;
    static constexpr int32_t DEFAULT_SUITE_STATS_TIME = 24 * 60;
    static constexpr uint64_t DEFAULT_AGGREGATION_TIME_SECEND = 24 * 60 * 60;
    static constexpr uint64_t DEFAULT_TONE_PLAYBACK_TIME_SECEND = 2 * 60 * 60;
    static constexpr uint64_t DEFAULT_VOLUME_API_INVOKE_TIME_SECEND = 24 * 60 * 60;
    static constexpr uint64_t DEFAULT_SUITE_STATS_TIME_SECEND = 24 * 60 * 60;

    static constexpr int32_t DEFAULT_VOLUME_API_INVOKE_COUNT_ONCE = 20;

    void ReadParameter();
    void StartTimeThread();
    void StopTimeThread();
    void TimeFunc();
    void HandleToHiSysEvent();
    void HandleToSystemTonePlaybackEvent();
    void setAppNameToEventVector(const std::string key, std::shared_ptr<EventBean> &bean);
    BundleInfo GetBundleInfo(int32_t appUid);
    void SetBundleNameToEvent(const std::string key, std::shared_ptr<EventBean> &bean,
        const std::string &bundleNameKey);
    void HandleToVolumeApiInvokeEvent();
    void HandleToSuiteEngineUtilizationStatsEvent();
    void HandleToVolumeSettingStatisticsEvent();
    void HandleToLoudVolumeSceneEvent(std::shared_ptr<EventBean> &bean);

    uint64_t lastTimeDefaultDaily_ = 0;
    uint64_t curruntTime_ = 0;
    uint64_t lastAudioTime_ = 0;
    uint64_t afterSleepTime_ = 0;
    uint64_t lastSystemTonePlaybackTime_ = 0;
    uint64_t lastVolumeApiInvokeTime_ = 0;
    uint64_t lastSuiteStatsTime_ = 0;
    std::unique_ptr<std::thread> timeThread_ = nullptr;
    std::atomic_bool startThread_ = true;

    int32_t volumeApiInvokeOnceEvent_ = DEFAULT_VOLUME_API_INVOKE_COUNT_ONCE;

    MediaEventBaseWriter& mediaEventBaseWriter_;

    std::mutex eventVectorMutex_;
    std::vector<std::shared_ptr<EventBean>> eventVector_;
    std::map<int32_t, BundleInfo> cachedBundleInfoMap_;
    std::vector<std::shared_ptr<EventBean>> systemTonePlayEventVector_;
    std::queue<std::shared_ptr<EventBean>> volumeApiInvokeEventQueue_;
    std::unordered_set<std::string> volumeApiInvokeRecordSet_;

    std::mutex suiteStatsEventMutex_;
    std::unordered_map<std::string, std::unordered_map<std::string, SuiteEngineNodeStatCounts>>
        suiteEngineNodeStatsMap_;

    int32_t volumeApiInvokeRecordSetSize_ = 20 * 30 * 12;
    std::mutex volumeApiInvokeMutex_;
    std::atomic<int32_t> loudVolumeTimes_ = 0;

    uint64_t defaultDailySleepTime_ = DEFAULT_DAILY_TIME_SECEND;
    int32_t systemTonePlayerCount_ = 0;
    int32_t aggregationFrequency_ = DEFAULT_AGGREGATION_FREQUENCY;
    int32_t aggregationTime_ = DEFAULT_AGGREGATION_TIME;
    int32_t systemTonePlaybackTime_ = DEFAULT_TONE_PLAYBACK_TIME;
    uint64_t systemTonePlaybackSleepTime_ = DEFAULT_TONE_PLAYBACK_TIME_SECEND;
    uint64_t aggregationSleepTime_ = DEFAULT_AGGREGATION_TIME_SECEND;
    int32_t volumeApiInvokeTime_ = DEFAULT_VOLUME_API_INVOKE_TIME;
    uint64_t volumeApiInvokeSleepTime_ = DEFAULT_VOLUME_API_INVOKE_TIME_SECEND;
    int32_t suiteStatsTime_ = DEFAULT_SUITE_STATS_TIME;
    uint64_t suiteStatsSleepTime_ = DEFAULT_SUITE_STATS_TIME_SECEND;

    std::mutex callSessionMutex_;
    std::unordered_set<std::string> callSessionHapSet_;
    uint32_t callSessionHapSetSize_ = 100 * 100;

    std::mutex audioInterruptErrorMutex_;
    std::unordered_set<std::string> audioInterruptErrorSet_;
    uint32_t audioInterruptErrorSetSize_ = 100 * 100;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_POLICY_H