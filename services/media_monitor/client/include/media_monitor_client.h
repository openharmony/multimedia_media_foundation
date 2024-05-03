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

#ifndef MEDIA_MONITOR_PROXY_H
#define MEDIA_MONITOR_PROXY_H

#include "iremote_proxy.h"
#include "media_monitor_base.h"
#include "media_monitor_ipc_interface_code.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorClient : public IRemoteProxy<IMediaMonitor> {
public:
    explicit MediaMonitorClient(const sptr<IRemoteObject> &impl);
    virtual ~MediaMonitorClient() = default;

    void WriteLogMsg(std::shared_ptr<EventBean> &bean)  override;

    void GetAudioRouteMsg(std::map<PerferredType,
        std::shared_ptr<MonitorDeviceInfo>> &perferredDevices) override;

private:
    static inline BrokerDelegator<MediaMonitorClient> delegator_;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_PROXY_H