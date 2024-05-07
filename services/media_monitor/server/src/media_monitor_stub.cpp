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

#include "media_monitor_stub.h"
#include "media_monitor_ipc_interface_code.h"
#include "log.h"
#include "monitor_error.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

int32_t MediaMonitorStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    FALSE_RETURN_V_MSG_E(data.ReadInterfaceToken() == GetDescriptor(), -1, "ReadInterfaceToken failed");
    if (code <= static_cast<uint32_t>(MediaMonitorInterfaceCode ::MEDIA_MONITOR_CODE_MAX)) {
        (this->*handlers[code])(data, reply);
        return MediaMonitorErr::SUCCESS;
    }
    MEDIA_LOG_E("default case, need check MediaMonitorStub");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

void MediaMonitorStub::HandleWriteLogMsg(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<EventBean> eventBean = std::make_shared<EventBean>();
    eventBean->ReadFromParcel(data);
    WriteLogMsg(eventBean);
}

void MediaMonitorStub::HandleGetAudioRouteMsg(MessageParcel &data, MessageParcel &reply)
{
    std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> perferredDevices;

    GetAudioRouteMsg(perferredDevices);

    int32_t mapSize = perferredDevices.size();
    reply.WriteInt32(mapSize);
    for (auto &it : perferredDevices) {
        reply.WriteInt32(static_cast<int32_t>(it.first));
        reply.WriteInt32(it.second->deviceType_);
        reply.WriteString(it.second->deviceName_);
        reply.WriteString(it.second->address_);
        reply.WriteInt32(it.second->deviceCategory_);
    }
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS