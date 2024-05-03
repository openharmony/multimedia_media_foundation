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

#include "hisysevent.h"
#include "log.h"
#include "media_event_base_writer.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

static constexpr int INITAL_VALUE = 1;

void MediaEventBaseWriter::WriteHeasetChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write heaset change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "HEADSET_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISCONNECT", bean->GetIntValue("ISCONNECT"),
        "HASMIC", bean->GetIntValue("HASMIC"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"));
}

void MediaEventBaseWriter::WriteVolumeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write volume change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "VOLUME_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "APP_UID", bean->GetIntValue("APP_UID"),
        "APP_PID", bean->GetIntValue("APP_PID"),
        "STREAMTYPE", bean->GetIntValue("STREAMTYPE"),
        "VOLUME", bean->GetFloatValue("VOLUME"),
        "SYSVOLUME", bean->GetIntValue("SYSVOLUME"),
        "VOLUMEFACTOR", bean->GetFloatValue("VOLUMEFACTOR"),
        "POWERVOLUMEFACTOR", bean->GetFloatValue("POWERVOLUMEFACTOR"));
}

void MediaEventBaseWriter::WriteStreamChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "STREAM_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "UID", bean->GetIntValue("UID"),
        "PID", bean->GetIntValue("PID"),
        "TRANSACTIONID", bean->GetUint64Value("TRANSACTIONID"),
        "STREAMTYPE", bean->GetIntValue("STREAMTYPE"),
        "STATE", bean->GetIntValue("STATE"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"),
        "NETWORKID", bean->GetStringValue("NETWORKID"));
}

void MediaEventBaseWriter::WriteDeviceChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write device Change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "DEVICE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ISOUTPUT", bean->GetIntValue("ISOUTPUT"),
        "STREAMID", bean->GetIntValue("STREAMID"),
        "STREAMTYPE", bean->GetIntValue("STREAMTYPE"),
        "DEVICETYPE", bean->GetIntValue("DEVICETYPE"),
        "NETWORKID", bean->GetStringValue("NETWORKID"),
        "ADDRESS", bean->GetStringValue("ADDRESS"),
        "DEVICE_DETAILED_CATEGORY", bean->GetStringValue("DEVICE_NAME"));
}

void MediaEventBaseWriter::WriteLoadConfigError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write load config error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "LOAD_CONFIG_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "CATEGORY", static_cast<uint16_t>(bean->GetIntValue("CATEGORY")));
}

void MediaEventBaseWriter::WriteAudioStartupError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio startup error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_SERVICE_STARTUP_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "SERVICE_ID", static_cast<uint>(bean->GetIntValue("SERVICE_ID")),
        "ERROR_CODE", static_cast<uint32_t>(bean->GetIntValue("ERROR_CODE")));
}

void MediaEventBaseWriter::WriteStreamExhastedError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream exhasted error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_SERVER_STARTUP_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "DUBIOUS_APP", bean->GetStringValue("DUBIOUS_APP"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
}

void MediaEventBaseWriter::WriteStreamCreateError(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream create error");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_STREAM_CREATE_ERROR_STATS",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "ERROR_CODE", bean->GetIntValue("ERROR_CODE"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
}

void MediaEventBaseWriter::WriteBackgoundSilentPlayback(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write backgound silent playback");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "BACKGROUND_SILENT_PLAYBACK",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "APP_VERSION_CODE", static_cast<uint32_t>(bean->GetIntValue("APP_VERSION_CODE")),
        "TIMES", INITAL_VALUE);
}

void MediaEventBaseWriter::WriteSetForceDevice(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write set force use device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "SET_FORCE_USE_AUDIO_DEVICE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")));
}

void MediaEventBaseWriter::WriteRouteChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write route change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "ROUTE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "REASON", bean->GetIntValue("REASON"),
        "TIMESTAMP", bean->GetUint64Value("TIMESTAMP"),
        "PREVIOUS_DEVICE", bean->GetIntValue("PREVIOUS_DEVICE"),
        "CURRENT_DEVICE", bean->GetIntValue("CURRENT_DEVICE"),
        "APP_NAME", bean->GetStringValue("APP_NAME"));
}

void MediaEventBaseWriter::WriteDeviceStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write device statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_DEVICE_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetIntValue("DURATION")));
}

void MediaEventBaseWriter::WriteBtUsageStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write bt usage statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "BT_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "BT_TYPE", static_cast<uint>(bean->GetIntValue("BT_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetIntValue("DURATION")));
}

void MediaEventBaseWriter::WriteStreamStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write stream statistic duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_STREAM_UTILIZATION_STATS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "PIPE_TYPE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "SAMPLE_RATE", static_cast<uint>(bean->GetIntValue("SAMPLE_RATE")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "DURATION", static_cast<uint32_t>(bean->GetIntValue("DURATION")));
}

void MediaEventBaseWriter::WriteUnderrunStatistic(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write underrun statistic times");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PERFORMANCE_UNDERRUN_STATISTIC",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "PIPE_TYPE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE")),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "TIMES", static_cast<uint32_t>(bean->GetIntValue("TIMES")));
}

void MediaEventBaseWriter::WriteAudioPipeChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio pipe change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PERFORMANCE_UNDERRUN_STATISTIC",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "IS_PLAYBACK", static_cast<bool>(bean->GetIntValue("IS_PLAYBACK")),
        "PIPE_TYPE_BEFORE_CHANGE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE_BEFORE_CHANGE")),
        "PIPE_TYPE_AFTER_CHANGE", static_cast<uint>(bean->GetIntValue("PIPE_TYPE_AFTER_CHANGE")),
        "REASON", static_cast<uint>(bean->GetIntValue("REASON")));
}

void MediaEventBaseWriter::WriteAudioRouteChange(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write audio route Change");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_ROUTE_CHANGE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "TIMESTAMP", bean->GetUint64Value("TIMESTAMP"),
        "DEVICE_TYPE_BEFORE_CHANGE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE_BEFORE_CHANGE")),
        "DEVICE_TYPE_AFTER_CHANGE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE_AFTER_CHANGE")),
        "REASON", static_cast<uint>(bean->GetIntValue("REASON")));
}

void MediaEventBaseWriter::WriteFocusMigrate(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write focus migrate");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "AUDIO_FOCUS_MIGRATE",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "DEVICE_DESC", bean->GetStringValue("DEVICE_DESC"),
        "APP_NAME", bean->GetStringValue("APP_NAME"),
        "MIGRATE_DIRECTION", static_cast<uint>(bean->GetIntValue("MIGRATE_DIRECTION")));
}

void MediaEventBaseWriter::WriteMutedCapture(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write muted capture duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "MUTED_CAPTURE_STATS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetIntValue("DURATION")));
}

void MediaEventBaseWriter::WritePlaybackVolume(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write playback volume duration");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::AUDIO, "PLAYBACK_VOLUME_STATS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "LEVEL", static_cast<uint>(bean->GetIntValue("LEVEL")),
        "STREAM_TYPE", static_cast<uint>(bean->GetIntValue("STREAM_TYPE")),
        "DEVICE_TYPE", static_cast<uint>(bean->GetIntValue("DEVICE_TYPE")),
        "DURATION", static_cast<uint32_t>(bean->GetIntValue("DURATION")));
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS