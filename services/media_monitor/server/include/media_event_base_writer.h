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

#ifndef MEDIA_EVENT_BASE_WRITER_H
#define MEDIA_EVENT_BASE_WRITER_H

#include "event_bean.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

struct DfxSystemTonePlaybackResult {
    std::vector<uint64_t> timeStamp{};
    std::vector<int32_t> systemSoundType{};
    std::vector<int32_t> clientUid{};
    std::vector<int32_t> deviceType{};
    std::vector<int32_t> errorCode{};
    std::vector<std::string> errorReason{};
    std::vector<bool> muteState{};
    std::vector<bool> muteHaptics{};
    std::vector<int32_t> ringMode{};
    std::vector<int32_t> streamType{};
    std::vector<bool> vibrationState{};
    std::vector<int64_t> volumeLevel{};
};

class MediaEventBaseWriter {
public:
    static MediaEventBaseWriter& GetMediaEventBaseWriter()
    {
        static MediaEventBaseWriter mediaEventBaseWriter;
        return mediaEventBaseWriter;
    }

    MediaEventBaseWriter() = default;
    ~MediaEventBaseWriter() = default;

    void WriteLoadConfigError(std::shared_ptr<EventBean> &bean);
    void WriteLoadEffectEngineError(std::shared_ptr<EventBean> &bean);
    void WriteAppWriteMute(std::shared_ptr<EventBean> &bean);
    void WriteHdiException(std::shared_ptr<EventBean> &bean);
    void WriteJankPlaybackError(std::shared_ptr<EventBean> &bean);
    void WriteAudioStartupError(std::shared_ptr<EventBean> &bean);
    void WriteStreamExhastedError(std::shared_ptr<EventBean> &bean);
    void WriteStreamCreateError(std::shared_ptr<EventBean> &bean);
    void WriteBackgoundSilentPlayback(std::shared_ptr<EventBean> &bean);
    void WriteBGSilentPlayback(std::shared_ptr<EventBean> &bean);
    void WriteUnderrunStatistic(std::shared_ptr<EventBean> &bean);
    void WriteHeasetChange(std::shared_ptr<EventBean> &bean);
    void WriteVolumeChange(std::shared_ptr<EventBean> &bean);
    void WriteStreamChange(std::shared_ptr<EventBean> &bean);
    void WriteStreamStandby(std::shared_ptr<EventBean> &bean);
    void WriteSmartPAStatus(std::shared_ptr<EventBean> &bean);
    void WriteNoiseSuppression(std::shared_ptr<EventBean> &bean);
    void WriteDeviceChange(std::shared_ptr<EventBean> &bean);
    void WriteSetForceDevice(std::shared_ptr<EventBean> &bean);
    void WriteDeviceStatistic(std::shared_ptr<EventBean> &bean);
    void WriteStreamStatistic(std::shared_ptr<EventBean> &bean);
    void WriteBtUsageStatistic(std::shared_ptr<EventBean> &bean);
    void WriteAudioPipeChange(std::shared_ptr<EventBean> &bean);
    void WriteAudioRouteChange(std::shared_ptr<EventBean> &bean);
    void WriteDbAccessException(std::shared_ptr<EventBean> &bean);
    void WriteDeviceChangeException(std::shared_ptr<EventBean> &bean);
    void WriteFocusMigrate(std::shared_ptr<EventBean> &bean);
    void WriteMutedCapture(std::shared_ptr<EventBean> &bean);
    void WritePlaybackVolume(std::shared_ptr<EventBean> &bean);
    void WriteStreamPropertyStatistic(std::shared_ptr<EventBean> &bean);
    void WriteVolumeSubscribe(std::shared_ptr<EventBean> &bean);
    void WriteCustomizedToneChange(std::shared_ptr<EventBean> &bean);
    void WriteExcludeOutputDevice(std::shared_ptr<EventBean> &bean);
    void WriteSystemTonePlayback(const std::unique_ptr<DfxSystemTonePlaybackResult> &result);
    void WriteStreamOccupancy(std::shared_ptr<EventBean> &bean);
    void WriteAudioRecordError(std::shared_ptr<EventBean> &bean);
    void WriteAppBackgroundState(std::shared_ptr<EventBean> &bean);
    void WriteMessageQueueException(std::shared_ptr<EventBean> &bean);
    void WriteStreamMoveException(std::shared_ptr<EventBean> &bean);
    void WriteVolumeApiInvoke(std::shared_ptr<EventBean> &bean);
    void WriteAppCallSession(std::shared_ptr<EventBean> &bean);
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_EVENT_BASE_WRITER_H