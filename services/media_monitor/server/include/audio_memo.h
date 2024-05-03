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

#ifndef AUDIO_MEMO_H
#define AUDIO_MEMO_H

#include <map>
#include <mutex>
#include "media_monitor_info.h"
#include "event_bean.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class AudioMemo {
public:
    static AudioMemo& GetAudioMemo()
    {
        static AudioMemo audioMemo;
        return audioMemo;
    }

    AudioMemo() = default;
    ~AudioMemo() = default;

    void UpdataRouteInfo(std::shared_ptr<EventBean> &bean);
    void GetAudioRouteMsg(std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> &perferredDevices);
private:
    PerferredType GetPerferredType(std::shared_ptr<EventBean> &bean);
    PerferredType GetPerferredRenderType(int32_t streamUsage);
    PerferredType GetPerferredCaptureType(int32_t AudioScene);
    std::mutex perferredDeviceMutex_;
    std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> perferredDevices_;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // AUDIO_MEMO_H