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

#include "log.h"
#include "media_event_base_writer.h"

#ifdef MONITOR_ENABLE_HISYSEVENT
#include "hisysevent.h"
#endif

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaEventBaseWriter"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void MediaEventBaseWriter::WriteHeasetChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write heaset change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "HEADSET_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISCONNECT", bean->GetIntValue("ISCONNECT"),
        "HASMIC", bean->GetIntValue("HASMIC"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"));
#endif
}

void MediaEventBaseWriter::WriteVolumeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write volume change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "VOLUME_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "APP_UID", bean->GetIntValue("APP_UID"),
        "APP_PID", bean->GetIntValue("APP_PID"),
        "STREAMTYPE", bean->GetIntValue("STREAM_TYPE"),
        "VOLUME", bean->GetFloatValue("VOLUME"),
        "SYSVOLUME", bean->GetIntValue("SYSVOLUME"),
        "VOLUMEFACTOR", bean->GetFloatValue("VOLUMEFACTOR"),
        "POWERVOLUMEFACTOR", bean->GetFloatValue("POWERVOLUMEFACTOR"));
#endif
}

void MediaEventBaseWriter::WriteStreamChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "UID", bean->GetIntValue("UID"),
        "PID", bean->GetIntValue("PID"),
        "TRANSACTIONID", bean->GetUint64Value("TRANSACTIONID"),
        "STREAMTYPE", bean->GetIntValue("STREAM_TYPE"),
        "STATE", bean->GetIntValue("STATE"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"),
        "NETWORKID", bean->GetStringValue("NETWORKID"));
    if (ret) {
        MEDIA_LOG_E("write event fail: STREAM_CHANGE, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteStreamStandby(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream standby");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_STANDBY",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "STREAMID", bean->GetIntValue("STREAMID"),
        "STANDBY", bean->GetIntValue("STANDBY"));
    if (ret) {
        MEDIA_LOG_E("write event fail: STREAM_STANDBY, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteSmartPAStatus(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write smartPA status");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SMARTPA_STATUS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "STATUS", bean->GetIntValue("STATUS"));
    if (ret) {
        MEDIA_LOG_E("write event fail: SMARTPA_STATUS, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteDeviceChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write device Change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "DEVICE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "STREAMTYPE", bean->GetIntValue("STREAM_TYPE"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"),
        "NETWORKID", bean->GetStringValue("NETWORKID"),
        "ADDRESS", bean->GetStringValue("ADDRESS"),
        "DEVICE_DETAILED_CATEGORY", bean->GetStringValue("DEVICE_NAME"));
    if (ret) {
        MEDIA_LOG_E("write event fail: DEVICE_CHANGE, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteNoiseSuppression(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write noise suppression");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AI_VOICE_NOISE_SUPPRESSION",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_VALUE", bean->GetIntValue("CURRENT_VALUE"));
    if (ret) {
        MEDIA_LOG_E("write event fail: AI_VOICE_NOISE_SUPPRESSION, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteLoadConfigError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write load config error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "LOAD_CONFIG_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "CATEGORY", static_cast<uint16_t>(bean->GetIntValue("CATEGORY")));
#endif
}

void MediaEventBaseWriter::WriteLoadEffectEngineError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write load effect engine error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "LOAD_EFFECT_ENGINE_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "ENGINE_TYPE", static_cast<uint16_t>(bean->GetIntValue("ENGINE_TYPE")));
#endif
}

void MediaEventBaseWriter::WriteAppWriteMute(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("app write mute");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "APP_WRITE_MUTE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_BUNDLE_NAME", bean->GetStringValue("APP_BUNDLE_NAME"),
        "STREAM_TYPE", static_cast<uint8_t>(bean->GetIntValue("STREAM_TYPE")),
        "SESSION_ID", static_cast<uint32_t>(bean->GetIntValue("SESSION_ID")),
        "STREAM_VOLUME", bean->GetFloatValue("STREAM_VOLUME"),
        "MUTE_STATE", static_cast<uint8_t>(bean->GetIntValue("MUTE_STATE")),
        "APP_BACKGROUND_STATE", static_cast<uint8_t>(bean->GetIntValue("APP_BACKGROUND_STATE")),
        "MUTE_PLAY_START_TIME", bean->GetUint64Value("MUTE_PLAY_START_TIME"),
        "MUTE_PLAY_DURATION", static_cast<uint32_t>(bean->GetIntValue("MUTE_PLAY_DURATION")));
#endif
}

void MediaEventBaseWriter::WriteHdiException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write Hdi Exception error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "HDI_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "HDI_TYPE", static_cast<uint8_t>(bean->GetIntValue("HDI_TYPE")),
        "ERROR_CASE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CASE")),
        "ERROR_MSG", bean->GetIntValue("ERROR_MSG"),
        "ERROR_DESCRIPTION", bean->GetStringValue("ERROR_DESCRIPTION"));
#endif
}

void MediaEventBaseWriter::WriteJankPlaybackError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write jank playback error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    int32_t reason = bean->GetIntValue("REASON");
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "JANK_PLAYBACK",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "REASON", static_cast<uint8_t>(reason),
        "PERIOD_MS", static_cast<uint32_t>(bean->GetIntValue("PERIOD_MS")),
        "PIPE_TYPE", static_cast<uint8_t>(bean->GetIntValue("PIPE_TYPE")),
        "HDI_ADAPTER", static_cast<uint8_t>(bean->GetIntValue("HDI_ADAPTER")),
        "POSITION", static_cast<uint8_t>(bean->GetIntValue("POSITION")),
        "APP_NAME", (reason == 0 ? bean->GetStringValue("APP_NAMES") : bean->GetStringValue("APP_NAME")),
        "JANK_START_TIME", static_cast<int64_t>(bean->GetUint64Value("JANK_START_TIME")));
#endif
}

void MediaEventBaseWriter::WriteAudioStartupError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio startup error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_SERVICE_STARTUP_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "SERVICE_ID", static_cast<uint>(bean->GetIntValue("SERVICE_ID")),
        "ERROR_CODE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CODE")));
#endif
}

void MediaEventBaseWriter::WriteStreamExhastedError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream exhasted error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_STREAM_EXHAUSTED_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "DUBIOUS_APP", bean->GetStringValue("DUBIOUS_APP"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
#endif
}

void MediaEventBaseWriter::WriteStreamCreateError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream create error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_STREAM_CREATE_ERROR_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "ERROR_CODE", bean->GetIntValue("ERROR_CODE"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
#endif
}

void MediaEventBaseWriter::WriteBackgoundSilentPlayback(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write backgound silent playback");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "BACKGROUND_SILENT_PLAYBACK",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "APP_VERSION_CODE", static_cast<uint32_t>(bean->GetIntValue("APP_VERSION_CODE")),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
#endif
}

void MediaEventBaseWriter::WriteBGSilentPlayback(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write bg silent playback");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "BG_SILENT_PLAYBACK",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "APP_VERSION_CODE", static_cast<uint32_t>(bean->GetIntValue("APP_VERSION_CODE")));
    if (ret) {
        MEDIA_LOG_E("write event fail: BG_SILENT_PLAYBACK, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteExcludeOutputDevice(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write exclude output device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "EXCLUDE_OUTPUT_DEVICE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "EXCLUSION_STATUS", static_cast<uint>(bean->GetIntValue("EXCLUSION_STATUS")),
        "AUDIO_DEVICE_USAGE", static_cast<uint>(bean->GetIntValue("AUDIO_DEVICE_USAGE")),
        "DEVICE_TYPE", bean->GetIntValue("DEVICE_TYPE"));
#endif
}

void MediaEventBaseWriter::WriteSetForceDevice(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write set force use device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SET_FORCE_USE_AUDIO_DEVICE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")));
#endif
}

void MediaEventBaseWriter::WriteDeviceStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write device statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_DEVICE_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteBtUsageStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write bt usage statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "BT_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "BT_TYPE", static_cast<uint>(bean->GetIntValue("BT_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteStreamStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "PIPE_TYPE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "SAMPLE_RATE", static_cast<uint>(bean->GetIntValue("SAMPLE_RATE")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "EFFECT_CHAIN", static_cast<uint>(bean->GetIntValue("EFFECT_CHAIN")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteStreamPropertyStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream property statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_PROPERTY_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "ENCODING_TYPE", static_cast<uint>(bean->GetIntValue("ENCODING_TYPE")),
        "CHANNEL_LAYOUT", static_cast<uint64_t>(bean->GetUint64Value("CHANNEL_LAYOUT")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteUnderrunStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write underrun statistic times");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PERFORMANCE_UNDER_OVERRUN_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "PIPE_TYPE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
#endif
}

void MediaEventBaseWriter::WriteAudioPipeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio pipe change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_PIPE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "PIPE_TYPE_BEFORE_CHANGE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE_BEFORE_CHANGE")),
        "PIPE_TYPE_AFTER_CHANGE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE_AFTER_CHANGE")),
        "REASON", static_cast<uint>(bean->GetIntValue("REASON")));
#endif
}

void MediaEventBaseWriter::WriteAudioRouteChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio route Change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_ROUTE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "TIMESTAMP", bean->GetUint64Value("TIMESTAMP"),
        "DEVICE_TYPE_BEFORE_CHANGE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE_BEFORE_CHANGE")),
        "DEVICE_TYPE_AFTER_CHANGE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE_AFTER_CHANGE")),
        "REASON", static_cast<uint>(bean->GetIntValue("REASON")),
        "PRE_AUDIO_SCENE", static_cast<int8_t>(bean->GetIntValue("PRE_AUDIO_SCENE")),
        "CUR_AUDIO_SCENE", static_cast<int8_t>(bean->GetIntValue("CUR_AUDIO_SCENE")),
        "DEVICE_LIST", bean->GetStringValue("DEVICE_LIST"),
        "ROUTER_TYPE", static_cast<uint8_t>(bean->GetIntValue("ROUTER_TYPE")));
#endif
}

void MediaEventBaseWriter::WriteDbAccessException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write db access exception");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "DB_ACCESS_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "DB_TYPE", bean->GetStringValue("DB_TYPE"),
        "ERROR_CASE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CASE")),
        "ERROR_MSG", bean->GetIntValue("ERROR_MSG"),
        "ERROR_DESCRIPTION", bean->GetStringValue("ERROR_DESCRIPTION"));
#endif
}

void MediaEventBaseWriter::WriteDeviceChangeException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write device change exception");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "DEVICE_CHANGE_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "CHANGE_REASON", static_cast<uint8_t>(bean->GetIntValue("CHANGE_REASON")),
        "DEVICE_TYPE", bean->GetIntValue("DEVICE_TYPE"),
        "ERROR_CASE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CASE")),
        "ERROR_MSG", bean->GetIntValue("ERROR_MSG"),
        "ERROR_DESCRIPTION", bean->GetStringValue("ERROR_DESCRIPTION"));
#endif
}

void MediaEventBaseWriter::WriteFocusMigrate(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write focus migrate");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_FOCUS_MIGRATE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "DEVICE_DESC", bean->GetStringValue("DEVICE_DESC"),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "MIGRATE_DIRECTION", static_cast<uint>(bean->GetIntValue("MIGRATE_DIRECTION")));
#endif
}

void MediaEventBaseWriter::WriteMutedCapture(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write muted capture duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "MUTED_CAPTURE_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteCustomizedToneChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_I("Write customzed tone change subscription");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "ADD_REMOVE_CUSTOMIZED_TONE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "ADD_REMOVE_OPERATION", static_cast<bool>(bean->GetIntValue("ADD_REMOVE_OPERATION")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "FILE_SIZE", static_cast<uint32_t>(bean->GetUint64Value("FILE_SIZE")),
        "RINGTONE_CATEGORY", bean->GetIntValue("RINGTONE_CATEGORY"),
        "MEDIA_TYPE", static_cast<int8_t>(bean->GetIntValue("MEDIA_TYPE")),
        "MIME_TYPE", bean->GetStringValue("MIME_TYPE"),
        "TIMESTAMP", bean->GetUint64Value("TIMESTAMP"),
        "RESULT", static_cast<int8_t>(bean->GetIntValue("RESULT")));
    if (ret) {
        MEDIA_LOG_E("write event fail: ADD_REMOVE_CUSTOMIZED_TONE, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WritePlaybackVolume(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write playback volume duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PLAYBACK_VOLUME_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "LEVEL", static_cast<uint>(bean->GetIntValue("LEVEL")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetUint64Value("DURATION")));
#endif
}

void MediaEventBaseWriter::WriteVolumeSubscribe(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write volume subscription");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "VOLUME_SUBSCRIBE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SUBSCRIBE_KEY", bean->GetStringValue("SUBSCRIBE_KEY"),
        "SUBSCRIBE_RESULT", static_cast<uint32_t>(bean->GetIntValue("SUBSCRIBE_RESULT")));
    if (ret) {
        MEDIA_LOG_E("write event fail: VOLUME_SUBSCRIBE, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteSystemTonePlayback(const std::unique_ptr<DfxSystemTonePlaybackResult> &result)
{
#ifdef MONITOR_ENABLE_HISYSEVENT
    MEDIA_LOG_I("Write system tone playback MONITOR_ENABLE_HISYSEVENT");
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SYSTEM_TONE_PLAYBACK",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "TIME_STAMP", result->timeStamp,
        "SYSTEM_SOUND_TYPE", result->systemSoundType,
        "CLIENT_UID", result->clientUid,
        "DEVICE_TYPE", result->deviceType,
        "ERROR_CODE", result->errorCode,
        "ERROR_REASON", result->errorReason,
        "MUTE_STATE", result->muteState,
        "MUTE_HAPTICS", result->muteHaptics,
        "RING_MODE", result->ringMode,
        "STREAM_TYPE", result->streamType,
        "VIBRATION_STATE", result->vibrationState,
        "VOLUME_LEVEL", result->volumeLevel);
    MEDIA_LOG_I("Write system tone playback end");
#endif
}

void MediaEventBaseWriter::WriteStreamOccupancy(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream occupancy");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_OCCUPANCY",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "SESSIONID", bean->GetIntValue("SESSIONID"),
        "UID", bean->GetIntValue("UID"),
        "PKGNAME", bean->GetStringValue("PKGNAME"),
        "STREAM_OR_SOURCE_TYPE", bean->GetIntValue("STREAM_OR_SOURCE_TYPE"),
        "START_TIME", static_cast<int64_t>(bean->GetUint64Value("START_TIME")),
        "UPLOAD_TIME", static_cast<int64_t>(bean->GetUint64Value("UPLOAD_TIME")));
    if (ret) {
        MEDIA_LOG_E("write event fail: STREAM_OCCUPANCY, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteAudioRecordError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio record error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "RECORD_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "INCOMING_SOURCE", static_cast<uint8_t>(bean->GetIntValue("INCOMING_SOURCE")),
        "INCOMING_PID", bean->GetIntValue("INCOMING_PID"),
        "INCOMING_PKG", bean->GetStringValue("INCOMING_PKG"),
        "ACTIVE_SOURCE", static_cast<uint8_t>(bean->GetIntValue("ACTIVE_SOURCE")),
        "ACTIVE_PID", bean->GetIntValue("ACTIVE_PID"),
        "ACTIVE_PKG", bean->GetStringValue("ACTIVE_PKG"),
        "REASON", bean->GetIntValue("REASON"));
    if (ret) {
        MEDIA_LOG_E("write event fail: RECORD_ERROR, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteAppBackgroundState(std::shared_ptr<EventBean> &bean)
{
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "APP_BACKGROUND_STATE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR);
#endif
}

void MediaEventBaseWriter::WriteMessageQueueException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write message queue error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "HPAE_MESSAGE_QUEUE_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "MSG_TYPE", bean->GetIntValue("MSG_TYPE"),
        "MSG_FUNC_NAME", bean->GetStringValue("MSG_FUNC_NAME"),
        "MSG_ERROR_DESCRIPTION", bean->GetStringValue("MSG_ERROR_DESCRIPTION"));
    if (ret) {
        MEDIA_LOG_E("write event fail: HPAE_MESSAGE_QUEUE_EXCEPTION, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteStreamMoveException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream move error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    auto ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "MOVE_STREAM_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "CLIENT_UID", bean->GetIntValue("CLIENT_UID"),
        "SESSION_ID", static_cast<uint32_t>(bean->GetIntValue("SESSION_ID")),
        "CURRENT_NAME", bean->GetStringValue("CURRENT_NAME"),
        "DES_NAME", bean->GetStringValue("DES_NAME"),
        "STREAM_TYPE", static_cast<uint32_t>(bean->GetIntValue("STREAM_TYPE")),
        "ERROR_DESCRIPTION", bean->GetStringValue("ERROR_DESCRIPTION"));
    if (ret) {
        MEDIA_LOG_E("write event fail: MOVE_STREAM_EXCEPTION, ret = %{public}d", ret);
    }
#endif
}

void MediaEventBaseWriter::WriteVolumeApiInvoke(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write volume api invoke");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "VOLUME_API_INVOKE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "FUNC_NAME", bean->GetStringValue("FUNC_NAME"),
        "PARAM_VALUE", bean->GetIntValue("PARAM_VALUE"));
#endif
}

void MediaEventBaseWriter::WriteAppCallSession(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write call audio_session");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "HAP_CALL_AUDIO_SESSION",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "SYSTEMHAP_NAME", bean->GetStringValue("APP_NAME"),
        "SYSTEMHAP_SET_FOCUSSTRATEGY", bean->GetIntValue("SYSTEMHAP_SET_FOCUSSTRATEGY"));
#endif
}

void MediaEventBaseWriter::WriteAudioMainThreadEvent(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio process in main thread");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PROCESS_AUDIO_BY_MAINTHREAD",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "BUNDLENAME", bean->GetStringValue("BUNDLENAME"),
        "AUDIODIRECTION", bean->GetIntValue("AUDIODIRECTION"),
        "AUDIOSTREAM", bean->GetIntValue("AUDIOSTREAM"),
        "CALLFUNC", bean->GetIntValue("CALLFUNC"));
#endif
}

void MediaEventBaseWriter::WriteSuiteEngineUtilizationStats(
    std::shared_ptr<EventBean> &bean,
    std::vector<std> appNameList,
    std::vector<uint32_t> nodeCountList)
{
    MEDIA_LOG_D("audio suite engine utilization stats");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SUITE_ENGINE_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_NAME", appNameList,
        "AUDIO_NODE_TYPE", bean->GetStringValue("AUDIO_NODE_TYPE"),
        "AUDIO_NODE_COUNT", nodeCountList);
#endif
}
 
void MediaEventBaseWriter::WriteSuiteEngineException(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("audio suite engine exception");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
#ifdef MONITOR_ENABLE_HISYSEVENT
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SUITE_ENGINE_EXCEPTION",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "ERROR_SCENE", static_cast<uint32_t>(bean->GetIntValue("ERROR_SCENE")),
        "ERROR_CASE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CASE")),
        "ERROR_DESCRIPTION", bean->GetStringValue("ERROR_DESCRIPTION"));
#endif
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS