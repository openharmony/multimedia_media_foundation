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
#include "parameter.h"
#include "parameters.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

static constexpr int32_t INITIAL_VALUE = 1;

MediaMonitorPolicy::MediaMonitorPolicy()
    : mediaEventBaseWriter_(MediaEventBaseWriter::GetMediaEventBaseWriter())
{
    MEDIA_LOG_D("MediaMonitorPolicy Constructor");
    ReadParameter();
    StartTimeThread();
}

MediaMonitorPolicy::~MediaMonitorPolicy()
{
    MEDIA_LOG_D("MediaMonitorPolicy Destructor");
    StopTimeThread();
}

void MediaMonitorPolicy::ReadParameter()
{
    const int32_t length = 6;
    char aggregationFrequency[length] = {0};
    char aggregationTime[length] = {0};
    int32_t ret = GetParameter("persist.multimedia.mediafoundation.aggregationfrequency", "1000",
        aggregationFrequency, sizeof(aggregationFrequency) - 1);
    if (ret > 0) {
        aggregationFrequency_ = atoi(aggregationFrequency);
        MEDIA_LOG_I("Get aggregationFrequency_ success %{public}d", aggregationFrequency_);
    } else {
        MEDIA_LOG_E("Get aggregationFrequency_ failed %{public}d", ret);
    }

    ret = GetParameter("persist.multimedia.mediafoundation.aggregationtime", "1440",
        aggregationTime, sizeof(aggregationTime) - 1);
    if (ret > 0) {
        aggregationTime_ = atoi(aggregationTime);
        MEDIA_LOG_I("Get aggregationTime_ success %{public}d", aggregationTime_);
    } else {
        MEDIA_LOG_E("Get aggregationTime_ failed %{public}d", ret);
    }
}

void MediaMonitorPolicy::TimeFunc()
{
    while (startThread_.load(std::memory_order_acquire)) {
        curruntTime_ = TimeUtils::GetCurSec();
        std::this_thread::sleep_for(std::chrono::minutes(aggregationTime_));
        HandleToHiSysEvent();
    }
}

void MediaMonitorPolicy::StartTimeThread()
{
    timeThread_ = std::make_unique<std::thread>(&MediaMonitorPolicy::TimeFunc, this);
    pthread_setname_np(timeThread_->native_handle(), "DFXTiming");
}

void MediaMonitorPolicy::StopTimeThread()
{
    startThread_.store(false, std::memory_order_release);
}

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
        case BACKGROUND_SILENT_PLAYBACK:
            mediaEventBaseWriter_.WriteBGSilentPlayback(bean);
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
        case LOAD_EFFECT_ENGINE_ERROR:
            mediaEventBaseWriter_.WriteLoadEffectEngineError(bean);
            break;
        default:
            break;
    }
}

void MediaMonitorPolicy::WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write aggregation event");
    switch (eventId) {
        case AUDIO_STREAM_EXHAUSTED_STATS:
            mediaEventBaseWriter_.WriteStreamExhastedError(bean);
            break;
        case AUDIO_STREAM_CREATE_ERROR_STATS:
            mediaEventBaseWriter_.WriteStreamCreateError(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            mediaEventBaseWriter_.WriteBackgoundSilentPlayback(bean);
            break;
        case STREAM_UTILIZATION_STATS:
            mediaEventBaseWriter_.WriteStreamStatistic(bean);
            break;
        case STREAM_PROPERTY_STATS:
            mediaEventBaseWriter_.WriteStreamPropertyStatistic(bean);
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

void MediaMonitorPolicy::HandDeviceUsageToEventVector(std::shared_ptr<EventBean> &deviceUsage)
{
    MEDIA_LOG_I("Handle device usage to event vector");
    if (deviceUsage == nullptr) {
        MEDIA_LOG_E("MediaMonitorPolicy HandDeviceUsageToEventVector deviceUsage is nullpr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&deviceUsage](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == AUDIO_DEVICE_UTILIZATION_STATS &&
            deviceUsage->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK") &&
            deviceUsage->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            deviceUsage->GetIntValue("DEVICE_TYPE") == eventBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing device usage");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + deviceUsage->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::AUDIO_DEVICE_UTILIZATION_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("STREAM_TYPE", deviceUsage->GetIntValue("STREAM_TYPE"));
        eventBean->Add("DEVICE_TYPE", deviceUsage->GetIntValue("DEVICE_TYPE"));
        eventBean->Add("IS_PLAYBACK", deviceUsage->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("DURATION", deviceUsage->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandBtUsageToEventVector(std::shared_ptr<EventBean> &btUsage)
{
    MEDIA_LOG_I("Handle bt usage to event vector");
    if (btUsage == nullptr) {
        MEDIA_LOG_I("MediaMonitorPolicy HandBtUsageToEventVector btUsage is nullpr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&btUsage](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == BT_UTILIZATION_STATS &&
            btUsage->GetIntValue("BT_TYPE") == eventBean->GetIntValue("BT_TYPE") &&
            btUsage->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            btUsage->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK")) {
            MEDIA_LOG_I("Find the existing bt device usage");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + btUsage->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::BT_UTILIZATION_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("BT_TYPE", btUsage->GetIntValue("BT_TYPE"));
        eventBean->Add("IS_PLAYBACK", btUsage->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("STREAM_TYPE", btUsage->GetIntValue("STREAM_TYPE"));
        eventBean->Add("DURATION", btUsage->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandStreamUsageToEventVector(std::shared_ptr<EventBean> &streamUsage)
{
    MEDIA_LOG_I("Handle stream usage to event vector");
    if (streamUsage == nullptr) {
        MEDIA_LOG_E("MediaMonitorPolicy HandStreamUsageToEventVector streamUsage is nullpr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&streamUsage](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == STREAM_UTILIZATION_STATS &&
            streamUsage->GetIntValue("PIPE_TYPE") == eventBean->GetIntValue("PIPE_TYPE") &&
            streamUsage->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            streamUsage->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK") &&
            streamUsage->GetStringValue("APP_NAME") == eventBean->GetStringValue("APP_NAME") &&
            streamUsage->GetIntValue("SAMPLE_RATE") == eventBean->GetIntValue("SAMPLE_RATE")) {
            MEDIA_LOG_I("Find the existing stream usage");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + streamUsage->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }
    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::STREAM_UTILIZATION_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("PIPE_TYPE", streamUsage->GetIntValue("PIPE_TYPE"));
        eventBean->Add("IS_PLAYBACK", streamUsage->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("STREAM_TYPE", streamUsage->GetIntValue("STREAM_TYPE"));
        eventBean->Add("SAMPLE_RATE", streamUsage->GetIntValue("SAMPLE_RATE"));
        eventBean->Add("APP_NAME", streamUsage->GetStringValue("APP_NAME"));
        eventBean->Add("EFFECT_CHAIN", streamUsage->GetIntValue("EFFECT_CHAIN"));
        eventBean->Add("DURATION", streamUsage->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandStreamPropertyToEventVector(std::shared_ptr<EventBean> &streamProperty)
{
    MEDIA_LOG_I("Handle stream property to event vector");
    if (streamProperty == nullptr) {
        MEDIA_LOG_E("MediaMonitorPolicy HandStreamPropertyToEventVector streamProperty is nullptr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&streamProperty](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == STREAM_UTILIZATION_STATS &&
            streamProperty->GetIntValue("UID") == eventBean->GetIntValue("UID") &&
            streamProperty->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            streamProperty->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK") &&
            streamProperty->GetStringValue("APP_NAME") == eventBean->GetStringValue("APP_NAME") &&
            streamProperty->GetIntValue("CHANNEL_LAYOUT") ==
            static_cast<int64_t>(eventBean->GetUint64Value("CHANNEL_LAYOUT")) &&
            streamProperty->GetIntValue("ENCODING_TYPE") == eventBean->GetIntValue("ENCODING_TYPE")) {
            MEDIA_LOG_I("Find the existing stream property");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + streamProperty->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::STREAM_PROPERTY_STATS, EventType::DURATION_AGGREGATION_EVENT);
        
        eventBean->Add("IS_PLAYBACK", streamProperty->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("STREAM_TYPE", streamProperty->GetIntValue("STREAM_TYPE"));
        eventBean->Add("APP_NAME", streamProperty->GetStringValue("APP_NAME"));
        eventBean->Add("ENCODING_TYPE", streamProperty->GetIntValue("ENCODING_TYPE"));
        eventBean->Add("CHANNEL_LAYOUT", streamProperty->GetUint64Value("CHANNEL_LAYOUT"));
        eventBean->Add("DURATION", streamProperty->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleVolumeToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle volume to event vector");
    if (bean == nullptr) {
        MEDIA_LOG_E("MediaMonitorPolicy HandleVolumeToEventVector bean is nullpr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == PLAYBACK_VOLUME_STATS &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("LEVEL") == eventBean->GetIntValue("LEVEL") &&
            bean->GetIntValue("DEVICE_TYPE") == eventBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_D("Find the existing volume usage");
            return true;
        }
        return false;
    };

    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + bean->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::PLAYBACK_VOLUME_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
        eventBean->Add("LEVEL", bean->GetIntValue("LEVEL"));
        eventBean->Add("DEVICE_TYPE", bean->GetIntValue("DEVICE_TYPE"));
        eventBean->Add("DURATION", bean->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleCaptureMutedToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle capture muted to event vector");
    if (bean == nullptr) {
        MEDIA_LOG_E("MediaMonitorPolicy HandleCaptureMutedToEventVector bean is nullpr");
        return;
    }
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == MUTED_CAPTURE_STATS &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("DEVICE_TYPE") == eventBean->GetIntValue("DEVICE_TYPE")) {
            MEDIA_LOG_I("Find the existing capture muted");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        uint64_t duration = (*it)->GetUint64Value("DURATION") + bean->GetUint64Value("DURATION");
        (*it)->UpdateUint64Map("DURATION", duration);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::MUTED_CAPTURE_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
        eventBean->Add("DEVICE_TYPE", bean->GetIntValue("DEVICE_TYPE"));
        eventBean->Add("DURATION", bean->GetUint64Value("DURATION"));
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleExhaustedToEventVector(const std::string &appName)
{
    MEDIA_LOG_I("Handle exhausted to event map");
    bool isInEventMap = false;
    auto isExist = [&appName](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == AUDIO_STREAM_EXHAUSTED_STATS &&
            appName == eventBean->GetStringValue("DUBIOUS_APP")) {
            MEDIA_LOG_I("Find the existing DUBIOUS_APP");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        int32_t num = (*it)->GetIntValue("TIMES");
        (*it)->UpdateIntMap("TIMES", ++num);
        isInEventMap = true;
    }
    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::AUDIO_STREAM_EXHAUSTED_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("DUBIOUS_APP", appName);
        eventBean->Add("TIMES", INITIAL_VALUE);
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleCreateErrorToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle create error to event vector");
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == AUDIO_STREAM_CREATE_ERROR_STATS &&
            bean->GetStringValue("APP_NAME") == eventBean->GetStringValue("APP_NAME") &&
            bean->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE") &&
            bean->GetIntValue("ERROR_CODE") == eventBean->GetIntValue("ERROR_CODE")) {
            MEDIA_LOG_I("Find the existing create error app");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        int32_t num = (*it)->GetIntValue("TIMES");
        (*it)->UpdateIntMap("TIMES", ++num);
        isInEventMap = true;
    }

    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::AUDIO_STREAM_CREATE_ERROR_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("APP_NAME", bean->GetStringValue("APP_NAME"));
        eventBean->Add("IS_PLAYBACK", bean->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
        eventBean->Add("ERROR_CODE", bean->GetIntValue("ERROR_CODE"));
        eventBean->Add("TIMES", INITIAL_VALUE);
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleSilentPlaybackToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle silent playback to event vector");
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == BACKGROUND_SILENT_PLAYBACK &&
            bean->GetStringValue("APP_NAME") == eventBean->GetStringValue("APP_NAME") &&
            bean->GetIntValue("APP_VERSION_CODE") == eventBean->GetIntValue("APP_VERSION_CODE")) {
            MEDIA_LOG_I("Find the existing silent playback app");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        int32_t num = (*it)->GetIntValue("TIMES");
        (*it)->UpdateIntMap("TIMES", ++num);
        isInEventMap = true;
    }
    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::BACKGROUND_SILENT_PLAYBACK, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("APP_NAME", bean->GetStringValue("APP_NAME"));
        eventBean->Add("APP_VERSION_CODE", bean->GetIntValue("APP_VERSION_CODE"));
        eventBean->Add("TIMES", INITIAL_VALUE);
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleUnderrunToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle underrun to event vector");
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == PERFORMANCE_UNDER_OVERRUN_STATS &&
            bean->GetStringValue("APP_NAME") == eventBean->GetStringValue("APP_NAME") &&
            bean->GetIntValue("IS_PLAYBACK") == eventBean->GetIntValue("IS_PLAYBACK") &&
            bean->GetIntValue("PIPE_TYPE") == eventBean->GetIntValue("PIPE_TYPE") &&
            bean->GetIntValue("STREAM_TYPE") == eventBean->GetIntValue("STREAM_TYPE")) {
            MEDIA_LOG_I("Find the existing underrun app");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        int32_t num = (*it)->GetIntValue("TIMES");
        (*it)->UpdateIntMap("TIMES", ++num);
        isInEventMap = true;
    }
    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::PERFORMANCE_UNDER_OVERRUN_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("APP_NAME", bean->GetStringValue("APP_NAME"));
        eventBean->Add("IS_PLAYBACK", bean->GetIntValue("IS_PLAYBACK"));
        eventBean->Add("PIPE_TYPE", bean->GetIntValue("PIPE_TYPE"));
        eventBean->Add("STREAM_TYPE", bean->GetIntValue("STREAM_TYPE"));
        eventBean->Add("TIMES", INITIAL_VALUE);
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::AddToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Add to event vector");
    eventVector_.push_back(bean);
}

void MediaMonitorPolicy::WhetherToHiSysEvent()
{
    MEDIA_LOG_D("eventVector size %{public}zu", eventVector_.size());
    if (eventVector_.size() >= static_cast<uint32_t>(aggregationFrequency_)) {
        std::unique_ptr<std::thread>  writeEventThread = std::make_unique<std::thread>(
            &MediaMonitorPolicy::HandleToHiSysEvent, this);
        pthread_setname_np(writeEventThread->native_handle(), "ToHiSysEvent");
        writeEventThread->detach();
    }
}

void MediaMonitorPolicy::HandleToHiSysEvent()
{
    MEDIA_LOG_D("Handle to hiSysEvent");
    std::vector<std::shared_ptr<EventBean>> eventVector;
    {
        std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
        eventVector = eventVector_;
        eventVector_.clear();
    }
    for (auto &desc : eventVector) {
        if (desc == nullptr) {
            continue;
        }
        WriteEvent(desc->GetEventId(), desc);
    }
}

void MediaMonitorPolicy::WriteInfo(int32_t fd, std::string &dumpString)
{
    if (fd != -1) {
        int32_t remainderSize = aggregationFrequency_ - static_cast<int32_t>(eventVector_.size());
        int64_t elapsedTime = static_cast<int64_t>((TimeUtils::GetCurSec() - curruntTime_)) / 60;
        int64_t oneDay = aggregationTime_;
        dumpString += "Counting of eventVector entries:";
        dumpString += "\n";
        dumpString += "    The current number of eventVector: " + std::to_string(eventVector_.size());
        dumpString += "\n";
        dumpString += "    Remaining number of starting refreshes: " + std::to_string(remainderSize);
        dumpString += "\n";
        dumpString += "\n";
        dumpString += "Time Count:";
        dumpString += "\n";
        dumpString += "    Time elapsed:" + std::to_string(elapsedTime) + "min";
        dumpString += "\n";
        dumpString += "    Remaining refresh time:" + std::to_string(oneDay - elapsedTime) + "min";
        dumpString += "\n";
        dumpString += "\n";
    }
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS