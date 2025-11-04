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

#ifndef ST_MEDIA_MONITOR_MANAGER_H
#define ST_MEDIA_MONITOR_MANAGER_H

#include "event_bean.h"
#include "monitor_utils.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorManager {
public:
    static MediaMonitorManager& GetInstance();

    void WriteLogMsg(std::shared_ptr<EventBean> &bean);

    void GetAudioRouteMsg(std::map<PreferredType,
        std::shared_ptr<MonitorDeviceInfo>> &preferredDevices);

    void WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size);

    int32_t GetMediaParameters(const std::vector<std::string> &subKeys,
        std::vector<std::pair<std::string, std::string>> &result);

    int32_t SetMediaParameters(const std::vector<std::pair<std::string, std::string>> &kvpairs);

    static void MediaMonitorDied(pid_t pid);

    int32_t ErasePreferredDeviceByType(const PreferredType preferredType);

    void GetAudioExcludedDevicesMsg(std::map<AudioDeviceUsage,
        std::vector<std::shared_ptr<MonitorDeviceInfo>>> &excludedDevices);
    
    void GetCollaborativeDeviceState(std::map<std::string, uint32_t> &addressToCollaborativeEnabledMap);

    void GetAudioAppStateMsg(std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> &appStateMap);

    void GetDistributedDeviceInfo(std::vector<std::string> &deviceInfos);

    void GetDistributedSceneInfo(std::string &sceneInfo);

private:
    MediaMonitorManager();
    ~MediaMonitorManager() {}
    bool dumpEnable_ = false;
    std::string dumpType_ = DEFAULT_DUMP_TYPE;
    std::string versionType_ = COMMERCIAL_VERSION;
    std::time_t dumpStartTime_ = 0;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // ST_MEDIA_MONITOR_MANAGER_H