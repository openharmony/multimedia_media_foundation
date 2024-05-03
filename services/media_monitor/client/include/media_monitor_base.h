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

#ifndef I_MEDIA_MONITOR_BASE_H
#define I_MEDIA_MONITOR_BASE_H

#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "event_bean.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class AudioDeviceDescriptor;
class IMediaMonitor : public IRemoteBroker {
public:
    /**
     * Sets WriteLogMsg.
     *
     * @param bean EventBean
     */
    virtual void WriteLogMsg(std::shared_ptr<EventBean> &bean)  = 0;

    /**
     * Sets GetAudioRouteMsg.
     *
     * @param perferredDevices MonitorDeviceInfo map
     */
    virtual void GetAudioRouteMsg(std::map<PerferredType,
        std::shared_ptr<MonitorDeviceInfo>> &perferredDevices) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IMediaMonitor");
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MEDIA_MONITOR_BASE_H