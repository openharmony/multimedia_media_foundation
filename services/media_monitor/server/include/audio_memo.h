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
    int32_t GetAudioRouteMsg(std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> &preferredDevices);

    void WriteInfo(int32_t fd, std::string &dumpString);

    int32_t ErasePreferredDeviceByType(const PreferredType &preferredType);

    void UpdateExcludedDevice(std::shared_ptr<EventBean> &bean);
    int32_t GetAudioExcludedDevicesMsg(std::map<AudioDeviceUsage,
        std::vector<std::shared_ptr<MonitorDeviceInfo>>> &excludedDevices);
    
    void UpdateCollaborativeDeviceState(std::shared_ptr<EventBean> &bean);
    int32_t GetCollaborativeDeviceState(std::map<std::string, uint32_t> &addressToCollaborativeEnabledMap);

    void UpdateAppBackgroundState(std::shared_ptr<EventBean> &bean);
    int32_t GetAudioAppStateMsg(std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> &appStateMap);

    void UpdateDistributedSceneInfo(std::shared_ptr<EventBean> &bean);
    int32_t GetDistributedSceneInfo(std::string &distributedSceneInfo);
    void UpdateDistributedDeviceInfo(std::shared_ptr<EventBean> &bean);
    int32_t GetDistributedDeviceInfo(std::vector<std::string> &distributedDeviceInfos);
private:
    PreferredType GetPreferredType(std::shared_ptr<EventBean> &bean);
    PreferredType GetPreferredRenderType(int32_t streamUsage);
    PreferredType GetPreferredCaptureType(int32_t AudioScene);

    std::string GetPreferredNameFromType(const PreferredType &type);

    void UpdateExcludedDeviceInner(AudioDeviceUsage audioDevUsage, std::shared_ptr<MonitorDeviceInfo> &deviceInfo,
        int32_t exclusionStatus);
    void UpdateAppBackgroundStateInner(int32_t pid, std::shared_ptr<MonitorAppStateInfo> appStateInfo, int32_t isAdd);

    std::mutex preferredDeviceMutex_;
    std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> preferredDevices_;

    std::mutex excludedDeviceMutex_;
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>> excludedDevices_;

    std::mutex collaborativeMutex_;
    std::map<std::string, uint32_t> addressToCollaborativeEnabledMap_;

    std::mutex appStateMutex_;
    std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> appStateMap_;

    std::mutex distributedInfoMutex_;
    std::string distributedSceneInfo_;
    std::map<std::string, std::string> distributedDeviceInfos_;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // AUDIO_MEMO_H