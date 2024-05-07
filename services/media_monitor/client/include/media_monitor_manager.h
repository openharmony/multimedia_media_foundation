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

#include "media_monitor_base.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorManager {
public:
    static MediaMonitorManager& GetInstance()
    {
        static MediaMonitorManager monitorManager;
        return monitorManager;
    }

    void WriteLogMsg(std::shared_ptr<EventBean> &bean);

    void GetAudioRouteMsg(std::map<PerferredType,
        std::shared_ptr<MonitorDeviceInfo>> &perferredDevices);

    static void MediaMonitorDied(pid_t pid);

private:
    MediaMonitorManager() {}
    ~MediaMonitorManager() {}
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // ST_MEDIA_MONITOR_MANAGER_H