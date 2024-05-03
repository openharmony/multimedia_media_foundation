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
    void WriteAudioStartupError(std::shared_ptr<EventBean> &bean);
    void WriteStreamExhastedError(std::shared_ptr<EventBean> &bean);
    void WriteStreamCreateError(std::shared_ptr<EventBean> &bean);
    void WriteBackgoundSilentPlayback(std::shared_ptr<EventBean> &bean);
    void WriteUnderrunStatistic(std::shared_ptr<EventBean> &bean);
    void WriteHeasetChange(std::shared_ptr<EventBean> &bean);
    void WriteVolumeChange(std::shared_ptr<EventBean> &bean);
    void WriteStreamChange(std::shared_ptr<EventBean> &bean);
    void WriteDeviceChange(std::shared_ptr<EventBean> &bean);
    void WriteSetForceDevice(std::shared_ptr<EventBean> &bean);
    void WriteRouteChange(std::shared_ptr<EventBean> &bean);
    void WriteDeviceStatistic(std::shared_ptr<EventBean> &bean);
    void WriteStreamStatistic(std::shared_ptr<EventBean> &bean);
    void WriteBtUsageStatistic(std::shared_ptr<EventBean> &bean);
    void WriteAudioPipeChange(std::shared_ptr<EventBean> &bean);
    void WriteAudioRouteChange(std::shared_ptr<EventBean> &bean);
    void WriteFocusMigrate(std::shared_ptr<EventBean> &bean);
    void WriteMutedCapture(std::shared_ptr<EventBean> &bean);
    void WritePlaybackVolume(std::shared_ptr<EventBean> &bean);
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_EVENT_BASE_WRITER_H