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

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorStub"};
}

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
    MEDIA_LOG_I("HandleGetAudioRouteMsg in");
    std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> preferredDevices;

    int32_t ret = GetAudioRouteMsg(preferredDevices);

    int32_t mapSize = static_cast<int32_t>(preferredDevices.size());
    reply.WriteInt32(mapSize);
    for (auto &it : preferredDevices) {
        reply.WriteInt32(static_cast<int32_t>(it.first));
        reply.WriteInt32(it.second->deviceType_);
        reply.WriteString(it.second->deviceName_);
        reply.WriteString(it.second->address_);
        reply.WriteInt32(it.second->deviceCategory_);
        reply.WriteInt32(it.second->usageOrSourceType_);
    }
    reply.WriteInt32(ret);
    MEDIA_LOG_I("MediaMonitorStub::HandleGetAudioRouteMsg out");
}

void MediaMonitorStub::HandleGetAudioExcludedDevicesMsg(MessageParcel &data, MessageParcel &reply)
{
    MEDIA_LOG_I("HandleGetAudioExcludedDevicesMsg in");
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>> excludedDevices;
    int32_t ret = GetAudioExcludedDevicesMsg(excludedDevices);
    int32_t mapSize = static_cast<int32_t>(excludedDevices.size());
    reply.WriteInt32(mapSize);
    for (auto &it : excludedDevices) {
        reply.WriteInt32(static_cast<int32_t>(it.first));
        reply.WriteInt32(static_cast<int32_t>(it.second.size()));
        for (auto &device : it.second) {
            reply.WriteInt32(device->deviceType_);
            reply.WriteInt32(device->deviceCategory_);
            reply.WriteInt32(device->usageOrSourceType_);
            reply.WriteInt32(device->audioDeviceUsage_);
            reply.WriteString(device->deviceName_);
            reply.WriteString(device->address_);
            reply.WriteString(device->networkId_);
        }
    }
    reply.WriteInt32(ret);
    MEDIA_LOG_I("HandleGetAudioExcludedDevicesMsg out");
}

void MediaMonitorStub::HandleSetMediaParams(MessageParcel &data, MessageParcel &reply)
{
    std::string dumpType;
    std::string dumpEnable;
    if (data.ReadString(dumpType) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleSetMediaParams read type failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    if (data.ReadString(dumpEnable) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleSetMediaParams read type failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    int32_t ret = SetMediaParameters(dumpType, dumpEnable);
    reply.WriteInt32(ret);
}

void MediaMonitorStub::HandleGetInputBuffer(MessageParcel &data, MessageParcel &reply)
{
    int32_t size;
    std::shared_ptr<DumpBuffer> buffer = nullptr;
    if (data.ReadInt32(size) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleGetInputBuffer get size failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    int32_t ret = GetInputBuffer(buffer, size);
    if (buffer == nullptr) {
        MEDIA_LOG_E("MediaMonitorStub::HandleGetInputBuffer failed");
        reply.WriteInt32(ret);
        return;
    }
    if (dumpBufferWrap_ == nullptr) {
        reply.WriteInt32(ERR_OPERATION_FAILED);
        return;
    }
    void *replyPtr = reinterpret_cast<void *>(&reply);
    if (dumpBufferWrap_->WriteToParcel(buffer.get(), replyPtr) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleGetInputBuffer write data failed");
        reply.WriteInt32(ERR_OPERATION_FAILED);
        return;
    }
    reply.WriteInt32(ret);
}

void MediaMonitorStub::HandleInputBufferFilled(MessageParcel &data, MessageParcel &reply)
{
    std::string fileName;
    uint64_t bufferId = 0;
    int32_t size = 0;
    if (data.ReadString(fileName) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleInputBufferFilled read name failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    if (data.ReadUint64(bufferId) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleInputBufferFilled read id failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }

    if (data.ReadInt32(size) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleInputBufferFilled read size failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    int32_t ret = InputBufferFilled(fileName, bufferId, size);
    reply.WriteInt32(ret);
}

void MediaMonitorStub::HandleGetPcmDumpStatus(MessageParcel &data, MessageParcel &reply)
{
    int32_t status = 0;
    int32_t ret = GetPcmDumpStatus(status);
    reply.WriteInt32(status);
    reply.WriteInt32(ret);
}

void MediaMonitorStub::HandleErasePreferredDeviceByType(MessageParcel &data, MessageParcel &reply)
{
    int32_t preferredType;
    if (data.ReadInt32(preferredType) == false) {
        MEDIA_LOG_E("MediaMonitorStub::HandleErasePreferredDeviceByType read type failed");
        reply.WriteInt32(ERR_INVALID_PARAM);
        return;
    }
    int32_t ret = ErasePreferredDeviceByType(static_cast<PreferredType>(preferredType));
    reply.WriteInt32(ret);
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS