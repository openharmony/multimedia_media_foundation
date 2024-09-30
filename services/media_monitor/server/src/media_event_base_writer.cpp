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
        "REASON", static_cast<uint>(bean->GetIntValue("REASON")));
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
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS