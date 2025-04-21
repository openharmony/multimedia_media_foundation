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

#include "media_monitor_client.h"
#include "monitor_error.h"
#include "log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorClient"};
}

using namespace std;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

constexpr int MAX_MAP_SIZE = 1000;

MediaMonitorClient::MediaMonitorClient(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IMediaMonitor>(impl)
{
    MEDIA_LOG_I("MediaMonitorClient::MediaMonitorClient");
}

void MediaMonitorClient::WriteLogMsg(std::shared_ptr<EventBean> &bean)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    data.WriteInterfaceToken(GetDescriptor());
    bean->WriteToParcel(data);
    Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::WRITE_LOG_MSG), data, reply, option);
}

int32_t MediaMonitorClient::GetAudioRouteMsg(std::map<PreferredType,
    std::shared_ptr<MonitorDeviceInfo>> &preferredDevices)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::GET_AUDIO_ROUTE_MSG), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "get Audio Route failed");

    int32_t mapSize = reply.ReadInt32();
    if (mapSize > MAX_MAP_SIZE) {
        MEDIA_LOG_E("The size of mapSize exceeds the maximum value");
        return ERR_INVALID_OPERATION;
    }
    for (int32_t index = 0; index < mapSize; index++) {
        shared_ptr<MonitorDeviceInfo> deviceInfo = std::make_shared<MonitorDeviceInfo>();
        PreferredType preferredType = static_cast<PreferredType>(reply.ReadInt32());
        deviceInfo->deviceType_ = reply.ReadInt32();
        deviceInfo->deviceName_ = reply.ReadString();
        deviceInfo->address_ = reply.ReadString();
        deviceInfo->deviceCategory_ = reply.ReadInt32();
        deviceInfo->usageOrSourceType_ = reply.ReadInt32();
        preferredDevices.emplace(preferredType, deviceInfo);
    }
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size)
{
    std::shared_ptr<DumpBuffer> bufferPtr = nullptr;

    int32_t ret = GetInputBuffer(bufferPtr, size);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "get buffer failed.");
    FALSE_RETURN_V_MSG_E(bufferPtr != nullptr, ERROR, "get buffer is nullptr.");

    std::shared_ptr<DumpBufferWrap> tmpBufferWrap = dumpBufferWrap_;
    FALSE_RETURN_V_MSG_E(tmpBufferWrap != nullptr, ERROR, "buffer wrap is nullptr.");

    int32_t bufferCapacitySize = tmpBufferWrap->GetCapacity(bufferPtr.get());
    FALSE_RETURN_V_MSG_E(bufferCapacitySize > 0, ERROR, "get buffer capacity error");
    int32_t writeSize = tmpBufferWrap->Write(bufferPtr.get(), static_cast<uint8_t*>(ptr), size);
    FALSE_RETURN_V_MSG_E(writeSize > 0, ERROR, "write buffer error");

    uint64_t bufferId = tmpBufferWrap->GetUniqueId(bufferPtr.get());
    ret = InputBufferFilled(fileName, bufferId, writeSize);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "write buffer error %{public}d", ret);
    return SUCCESS;
}

int32_t MediaMonitorClient::GetInputBuffer(std::shared_ptr<DumpBuffer> &buffer, int32_t size)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(size);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::GET_INPUT_BUFFER), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "get pcm buffer failed %{public}d", error);

    std::shared_ptr<DumpBufferWrap> tmpBufferWrap = dumpBufferWrap_;
    FALSE_RETURN_V_MSG_E(tmpBufferWrap != nullptr, ERROR, "buffer wrap is nullptr.");

    DumpBuffer *bufferPtr = tmpBufferWrap->NewDumpBuffer();
    FALSE_RETURN_V_MSG_E(bufferPtr != nullptr, error, "new dump buffer error");

    buffer = std::shared_ptr<DumpBuffer>(bufferPtr, [tmpBufferWrap](DumpBuffer *ptr) {
        if (tmpBufferWrap != nullptr && ptr != nullptr) {
            tmpBufferWrap->DestroyDumpBuffer(ptr);
        }
    });

    void *replyPtr = reinterpret_cast<void *>(&reply);
    if (tmpBufferWrap->ReadFromParcel(buffer.get(), replyPtr) == false) {
        MEDIA_LOG_E("read data failed");
        return reply.ReadInt32();
    }
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::InputBufferFilled(const std::string &fileName, uint64_t bufferId, int32_t size)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteString(fileName);
    data.WriteUint64(bufferId);
    data.WriteInt32(size);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::INPUT_BUFFER_FILL), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "send request error %{public}d", error);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::SetMediaParameters(const std::string &dumpType, const std::string &dumpEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    FALSE_RETURN_V_MSG_E(LoadDumpBufferWrap(dumpEnable) == SUCCESS, ERROR, "load buffer wrap error");

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteString(dumpType);
    data.WriteString(dumpEnable);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::SET_MEDIA_PARAMS), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "set media param error %{public}d", error);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::GetPcmDumpStatus(int32_t &dumpEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::GET_DUMP_STATUS), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "get media param error %{public}d", error);
    dumpEnable = reply.ReadInt32();
    MEDIA_LOG_I("get media param %d", dumpEnable);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::ErasePreferredDeviceByType(const PreferredType preferredType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(static_cast<int32_t>(preferredType));
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::ERASE_PREFERRED_DEVICE), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "erase preferred device error %{public}d", error);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::LoadDumpBufferWrap(const std::string &dumpEnable)
{
    bool flag = (dumpEnable == "true") ? true : false;
    if (flag && dumpBufferWrap_ != nullptr) {
        return SUCCESS;
    }

    if (flag) {
        dumpBufferWrap_ = std::make_shared<DumpBufferWrap>();
        bool ret = dumpBufferWrap_->Open();
        if (!ret) {
            MEDIA_LOG_E("load dumpbuffer failed");
            dumpBufferWrap_ = nullptr;
            return ERROR;
        }
    } else {
        dumpBufferWrap_ = nullptr;
    }
    return SUCCESS;
}

int32_t MediaMonitorClient::GetAudioExcludedDevicesMsg(std::map<AudioDeviceUsage,
    std::vector<std::shared_ptr<MonitorDeviceInfo>>> &excludedDevices)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::GET_EXCLUDED_DEVICES_MSG), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "get excluded devices failed");

    int32_t mapSize = reply.ReadInt32();
    if (mapSize > MAX_MAP_SIZE) {
        MEDIA_LOG_E("The size of mapSize exceeds the maximum value");
        return ERR_INVALID_OPERATION;
    }
    for (int32_t index = 0; index < mapSize; index++) {
        AudioDeviceUsage deviceUsage = static_cast<AudioDeviceUsage>(reply.ReadInt32());
        int32_t size = reply.ReadInt32();
        FALSE_RETURN_V_MSG_E(size >= 0 && size <= MAX_MAP_SIZE, ERR_INVALID_OPERATION,
            "The size of vector is invalid");
        std::vector<std::shared_ptr<MonitorDeviceInfo>> deviceInfoVec;
        for (int32_t i = 0; i < size; i++) {
            shared_ptr<MonitorDeviceInfo> deviceInfo = make_shared<MonitorDeviceInfo>();
            deviceInfo->deviceType_ = reply.ReadInt32();
            deviceInfo->deviceCategory_ = reply.ReadInt32();
            deviceInfo->usageOrSourceType_ = reply.ReadInt32();
            deviceInfo->audioDeviceUsage_ = reply.ReadInt32();
            deviceInfo->deviceName_ = reply.ReadString();
            deviceInfo->address_ = reply.ReadString();
            deviceInfo->networkId_ = reply.ReadString();
            deviceInfoVec.push_back(deviceInfo);
        }
        excludedDevices.emplace(deviceUsage, deviceInfoVec);
    }
    return reply.ReadInt32();
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS