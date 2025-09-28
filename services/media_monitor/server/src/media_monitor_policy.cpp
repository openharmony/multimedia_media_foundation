dd
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
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorPolicy"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

static constexpr int32_t INITIAL_VALUE = 1;
const int32_t MAX_PLAY_COUNT = 100;

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
        std::this_thread::sleep_for(std::chrono::minutes(systemTonePlaybackTime_));
        afterSleepTime_ = TimeUtils::GetCurSec();
        if (afterSleepTime_ - lastAudioTime_ >= aggregationSleepTime_) {
            HandleToHiSysEvent();
            lastAudioTime_ = TimeUtils::GetCurSec();
        }
        if (afterSleepTime_ - lastSystemTonePlaybackTime_ >= systemTonePlaybackSleepTime_) {
            HandleToSystemTonePlaybackEvent();
            lastSystemTonePlaybackTime_ = TimeUtils::GetCurSec();
        }
        if (afterSleepTime_ - lastVolumeApiInvokeTime_ >= volumeApiInvokeSleepTime_) {
            HandleToVolumeApiInvokeEvent();
            lastVolumeApiInvokeTime_ = TimeUtils::GetCurSec();
        }
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
    BundleInfo bundleInfo = {};
    switch (eventId) {
        case DEVICE_CHANGE:
            mediaEventBaseWriter_.WriteDeviceChange(bean);
            break;
        case HEADSET_CHANGE:
            mediaEventBaseWriter_.WriteHeasetChange(bean);
            break;
        case STREAM_CHANGE:
            setAppNameToEventVector("UID", bean);
            mediaEventBaseWriter_.WriteStreamChange(bean);
            break;
        case VOLUME_CHANGE:
            mediaEventBaseWriter_.WriteVolumeChange(bean);
            break;
        case AUDIO_ROUTE_CHANGE:
            mediaEventBaseWriter_.WriteAudioRouteChange(bean);
            break;
        case AUDIO_PIPE_CHANGE:
            setAppNameToEventVector("CLIENT_UID", bean);
            mediaEventBaseWriter_.WriteAudioPipeChange(bean);
            break;
        case AUDIO_FOCUS_MIGRATE:
            setAppNameToEventVector("CLIENT_UID", bean);
            mediaEventBaseWriter_.WriteFocusMigrate(bean);
            break;
        case SET_FORCE_USE_AUDIO_DEVICE:
            setAppNameToEventVector("CLIENT_UID", bean);
            mediaEventBaseWriter_.WriteSetForceDevice(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("APP_NAME", bundleInfo.appName);
            bean->Add("APP_VERSION_CODE", bundleInfo.versionCode);
            mediaEventBaseWriter_.WriteBGSilentPlayback(bean);
            break;
        case STREAM_STANDBY:
            mediaEventBaseWriter_.WriteStreamStandby(bean);
            break;
        case AI_VOICE_NOISE_SUPPRESSION:
            mediaEventBaseWriter_.WriteNoiseSuppression(bean);
            break;
        default:
            WriteBehaviorEventExpansion(eventId, bean);
            break;
    }
}

void MediaMonitorPolicy::WriteBehaviorEventExpansion(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write behavior event expansion");
    BundleInfo bundleInfo = {};
    switch (eventId) {
        case VOLUME_SUBSCRIBE:
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("SUBSCRIBE_KEY", bundleInfo.subscrbeKey);
            bean->Add("SUBSCRIBE_RESULT", bundleInfo.subscrbeResult);
            mediaEventBaseWriter_.WriteVolumeSubscribe(bean);
            break;
        case SMARTPA_STATUS:
            mediaEventBaseWriter_.WriteSmartPAStatus(bean);
            break;
        case EXCLUDE_OUTPUT_DEVICE:
            setAppNameToEventVector("CLIENT_UID", bean);
            mediaEventBaseWriter_.WriteExcludeOutputDevice(bean);
            break;
        case SYSTEM_TONE_PLAYBACK:
            systemTonePlayEventVector_.push_back(bean);
            TriggerSystemTonePlaybackEvent(bean);
            break;
        case APP_BACKGROUND_STATE:
            mediaEventBaseWriter_.WriteAppBackgroundState(bean);
            break;
        default:
            break;
    }
}

void MediaMonitorPolicy::TriggerSystemTonePlaybackEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Trigger system tone playback event . ");

    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    systemTonePlayerCount_++;
    MEDIA_LOG_I("systemTonePlayerCount_ . = %{public}d", systemTonePlayerCount_);
    if (systemTonePlayerCount_ >= MAX_PLAY_COUNT) {
        auto dfxResult = std::make_unique<DfxSystemTonePlaybackResult>();
        CollectDataToDfxResult(dfxResult.get());

        mediaEventBaseWriter_.WriteSystemTonePlayback(std::move(dfxResult));
        systemTonePlayerCount_ = 0;
        lastSystemTonePlaybackTime_ = TimeUtils::GetCurSec();
        systemTonePlayEventVector_.clear();
    }
}

void MediaMonitorPolicy::TriggerSystemTonePlaybackTimeEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("TriggerSystemTonePlaybackTimeEvent . ");
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto dfxResult = std::make_unique<DfxSystemTonePlaybackResult>();
    CollectDataToDfxResult(dfxResult.get());
    mediaEventBaseWriter_.WriteSystemTonePlayback(std::move(dfxResult));
    systemTonePlayerCount_ = 0;
    systemTonePlayEventVector_.clear();
}

 void MediaMonitorPolicy::CollectDataToDfxResult(DfxSystemTonePlaybackResult *result)
{
    MEDIA_LOG_D("Collect data to dfx result .");
    for (const auto& bean : systemTonePlayEventVector_) {
        if (bean == nullptr || result == nullptr) {
            MEDIA_LOG_E("The incoming data is invalid or data is nullptr.");
            return;
        }
        result->timeStamp.push_back(bean->GetUint64Value("TIME_STAMP"));
        result->systemSoundType.push_back(bean->GetIntValue("SYSTEM_SOUND_TYPE"));
        result->clientUid.push_back(bean->GetIntValue("CLIENT_UID"));
        result->deviceType.push_back(bean->GetIntValue("DEVICE_TYPE"));
        result->errorCode.push_back(bean->GetIntValue("ERROR_CODE"));
        result->errorReason.push_back(bean->GetStringValue("ERROR_REASON"));
        result->muteState.push_back(bean->GetIntValue("MUTE_STATE"));
        result->muteHaptics.push_back(bean->GetIntValue("MUTE_HAPTICS"));
        result->ringMode.push_back(bean->GetIntValue("RING_MODE"));
        result->streamType.push_back(bean->GetIntValue("STREAM_TYPE"));
        result->vibrationState.push_back(bean->GetIntValue("VIBRATION_STATE"));
        result->volumeLevel.push_back(bean->GetIntValue("VOLUME_LEVEL"));
    }
}

void MediaMonitorPolicy::setAppNameToEventVector(const std::string key, std::shared_ptr<EventBean> &bean)
{
    BundleInfo bundleInfo = GetBundleInfo(bean->GetIntValue(key));
    bean->Add("APP_NAME", bundleInfo.appName);
}

void MediaMonitorPolicy::SetBundleNameToEvent(const std::string key, std::shared_ptr<EventBean> &bean,
    const std::string &bundleNameKey)
{
    FALSE_RETURN_MSG(bean != nullptr, "eventBean is nullptr");
    BundleInfo bundleInfo = GetBundleInfo(bean->GetIntValue(key));
    bean->Add(bundleNameKey, bundleInfo.appName);
}

BundleInfo MediaMonitorPolicy::GetBundleInfo(int32_t appUid)
{
    auto cachedBundleInfo = cachedBundleInfoMap_.find(appUid);
    if (cachedBundleInfo != cachedBundleInfoMap_.end()) {
        return cachedBundleInfo->second;
    }
    MediaMonitorWrapper mediaMonitorWrapper;
    BundleInfo bundleInfo;
    bundleInfo.appName = "uid:" + std::to_string(appUid); // if no name found, use uid as default
    MediaMonitorErr err = mediaMonitorWrapper.GetBundleInfo(appUid, &bundleInfo);
    if (err != MediaMonitorErr::SUCCESS) {
        return bundleInfo;
    }
    cachedBundleInfoMap_.insert(std::make_pair(appUid, bundleInfo));
    return bundleInfo;
}

void MediaMonitorPolicy::WriteFaultEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write fault event");
    BundleInfo bundleInfo;
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
        case DB_ACCESS_EXCEPTION:
            mediaEventBaseWriter_.WriteDbAccessException(bean);
            break;
        case DEVICE_CHANGE_EXCEPTION:
            mediaEventBaseWriter_.WriteDeviceChangeException(bean);
            break;
        case APP_WRITE_MUTE:
            bundleInfo = GetBundleInfo(bean->GetIntValue("UID"));
            bean->Add("APP_BUNDLE_NAME", bundleInfo.appName);
            MEDIA_LOG_I("Handle mute event of app:" PUBLIC_LOG_S, bundleInfo.appName.c_str());
            mediaEventBaseWriter_.WriteAppWriteMute(bean);
            break;
        case HDI_EXCEPTION:
            mediaEventBaseWriter_.WriteHdiException(bean);
            break;
        case JANK_PLAYBACK:
            // update bundle name
            bundleInfo = GetBundleInfo(bean->GetIntValue("UID"));
            bean->Add("APP_NAME", bundleInfo.appName);
            MEDIA_LOG_I("Handle jank event of app:" PUBLIC_LOG_S, bundleInfo.appName.c_str());
            mediaEventBaseWriter_.WriteJankPlaybackError(bean);
            break;
        case RECORD_ERROR:
            SetBundleNameToEvent("INCOMING_UID", bean, "INCOMING_PKG");
            SetBundleNameToEvent("ACTIVE_UID", bean, "ACTIVE_PKG");
            mediaEventBaseWriter_.WriteAudioRecordError(bean);
            break;
        case HPAE_MESSAGE_QUEUE_EXCEPTION:
            mediaEventBaseWriter_.WriteMessageQueueException(bean);
            break;
        case STREAM_MOVE_EXCEPTION:
            mediaEventBaseWriter_.WriteStreamMoveException(bean);
            break;
        default:
            break;
    }
}

void MediaMonitorPolicy::WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    BundleInfo bundleInfo = {};
    switch (eventId) {
        case AUDIO_STREAM_EXHAUSTED_STATS:
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("DUBIOUS_APP", bundleInfo.appName);
            mediaEventBaseWriter_.WriteStreamExhastedError(bean);
            break;
        case AUDIO_STREAM_CREATE_ERROR_STATS:
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("APP_NAME", bundleInfo.appName);
            bean->Add("APP_VERSION_CODE", bundleInfo.versionCode);
            mediaEventBaseWriter_.WriteStreamCreateError(bean);
            break;
        case BACKGROUND_SILENT_PLAYBACK:
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("APP_NAME", bundleInfo.appName);
            bean->Add("APP_VERSION_CODE", bundleInfo.versionCode);
            mediaEventBaseWriter_.WriteBackgoundSilentPlayback(bean);
            break;
        case STREAM_UTILIZATION_STATS:
            // update bundle name
            bundleInfo = GetBundleInfo(bean->GetIntValue("UID"));
            bean->UpdateStringMap("APP_NAME", bundleInfo.appName);
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
            bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
            bean->Add("APP_NAME", bundleInfo.appName);
            mediaEventBaseWriter_.WriteUnderrunStatistic(bean);
            break;
        case PLAYBACK_VOLUME_STATS:
            mediaEventBaseWriter_.WritePlaybackVolume(bean);
            break;
        case MUTED_CAPTURE_STATS:
            mediaEventBaseWriter_.WriteMutedCapture(bean);
            break;
        default:
            WriteAggregationEventExpansion(eventId, bean);
            break;
    }
}

void MediaMonitorPolicy::WriteAggregationEventExpansion(EventId eventId, std::shared_ptr<EventBean> &bean)
{
    switch (eventId) {
        case STREAM_OCCUPANCY:
            SetBundleNameToEvent("UID", bean, "PKGNAME");
            mediaEventBaseWriter_.WriteStreamOccupancy(bean);
            break;
        case ADD_REMOVE_CUSTOMIZED_TONE:
            mediaEventBaseWriter_.WriteCustomizedToneChange(bean);
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
        eventBean->Add("UID", streamUsage->GetIntValue("UID"));
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

void MediaMonitorPolicy::HandleExhaustedToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle exhausted to event map");
    int32_t uid = bean->GetIntValue("CLIENT_UID");
    int32_t appStreamNum = bean->GetIntValue("TIMES");
    bool isInEventMap = false;
    auto isExist = [&uid](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == AUDIO_STREAM_EXHAUSTED_STATS &&
            uid == eventBean->GetIntValue("CLIENT_UID")) {
            MEDIA_LOG_I("Find the existing CLIENT_UID");
            return true;
        }
        return false;
    };
    std::lock_guard<std::mutex> lockEventVector(eventVectorMutex_);
    auto it = std::find_if(eventVector_.begin(), eventVector_.end(), isExist);
    if (it != eventVector_.end()) {
        int32_t streamNum = (*it)->GetIntValue("TIMES");
        if (appStreamNum > streamNum) {
            (*it)->UpdateIntMap("TIMES", appStreamNum);
        }
        isInEventMap = true;
    }
    if (!isInEventMap) {
        std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>(ModuleId::AUDIO,
            EventId::AUDIO_STREAM_EXHAUSTED_STATS, EventType::FREQUENCY_AGGREGATION_EVENT);
        eventBean->Add("CLIENT_UID", uid);
        eventBean->Add("TIMES", appStreamNum);
        AddToEventVector(eventBean);
    }
}

void MediaMonitorPolicy::HandleCreateErrorToEventVector(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Handle create error to event vector");
    bool isInEventMap = false;
    auto isExist = [&bean](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == AUDIO_STREAM_CREATE_ERROR_STATS &&
            bean->GetIntValue("CLIENT_UID") == eventBean->GetIntValue("CLIENT_UID") &&
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
        eventBean->Add("CLIENT_UID", bean->GetIntValue("CLIENT_UID"));
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
    BundleInfo bundleInfo = GetBundleInfo(bean->GetIntValue("CLIENT_UID"));
    auto isExist = [&bundleInfo](const std::shared_ptr<EventBean> &eventBean) {
        if (eventBean->GetEventId() == BACKGROUND_SILENT_PLAYBACK &&
            bundleInfo.appName == eventBean->GetStringValue("APP_NAME") &&
            bundleInfo.versionCode == eventBean->GetIntValue("APP_VERSION_CODE")) {
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
        eventBean->Add("APP_NAME", bundleInfo.appName);
        eventBean->Add("APP_VERSION_CODE", bundleInfo.versionCode);
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
            bean->GetIntValue("CLIENT_UID") == eventBean->GetIntValue("CLIENT_UID") &&
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
        eventBean->Add("CLIENT_UID", bean->GetIntValue("CLIENT_UID"));
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

void MediaMonitorPolicy::HandleToSystemTonePlaybackEvent()
{
    MEDIA_LOG_D("Handle to systemTone playback Event");
    for (auto &desc : systemTonePlayEventVector_) {
        if (desc == nullptr) {
            continue;
        }
        TriggerSystemTonePlaybackTimeEvent(desc);
    }
}

void MediaMonitorPolicy::AddToVolumeApiInvokeQueue(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("add to volume api invoke queue");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    setAppNameToEventVector("CLIENT_UID", bean);
    std::string key = bean->GetStringValue("APP_NAME") +
        bean->GetStringValue("FUNC_NAME") +
        std::to_string(bean->GetIntValue("PARAM_VALUE"));

    std::lock_guard<std::mutex> lock(volumeApiInvokeMutex_);
    if (volumeApiInvokeRecordSet_.find(key) == volumeApiInvokeRecordSet_.end() &&
        volumeApiInvokeRecordSet_.size() <= volumeApiInvokeRecordSetSize_) {
        volumeApiInvokeRecordSet_.emplace(key);
        volumeApiInvokeEventQueue_.push(bean);
    }
    if (volumeApiInvokeRecordSet_.size() > volumeApiInvokeRecordSetSize_) {
        MEDIA_LOG_D("volume api invoke record is full");
        return;
    }
}

void MediaMonitorPolicy::HandleToVolumeApiInvokeEvent()
{
    MEDIA_LOG_D("Handle to volume api invoke event");
    int32_t eventCount = 0;
    std::lock_guard<std::mutex> lock(volumeApiInvokeMutex_);
    while (eventCount < volumeApiInvokeOnceEvent_ && !volumeApiInvokeEventQueue_.empty()) {
        auto event = volumeApiInvokeEventQueue_.front();
        volumeApiInvokeEventQueue_.pop();
        eventCount++;
        mediaEventBaseWriter_.WriteVolumeApiInvoke(event);
    }
}

void MediaMonitorPolicy::AddToCallSessionQueue(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("add call audio session event");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    setAppNameToEventVector("CLIENT_UID", bean);
    std::string key = bean->GetStringValue("APP_NAME") +
        std::to_string(bean->GetIntValue("SYSTEMHAP_SET_FOCUSSTRATEGY"));

    std::lock_guard<std::mutex> lock(callSessionMutex_);
    if (callSessionHapSet_.find(key) == callSessionHapSet_.end() &&
        callSessionHapSet_.size() <= callSessionHapSetSize_) {
        callSessionHapSet_.emplace(key);
        mediaEventBaseWriter_.WriteAppCallSession(bean);
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