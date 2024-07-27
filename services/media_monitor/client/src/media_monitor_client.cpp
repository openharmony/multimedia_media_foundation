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
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
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

int32_t MediaMonitorClient::GetAudioRouteMsg(std::map<PerferredType,
    std::shared_ptr<MonitorDeviceInfo>> &perferredDevices)
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
        PerferredType perferredType = static_cast<PerferredType>(reply.ReadInt32());
        deviceInfo->deviceType_ = reply.ReadInt32();
        deviceInfo->deviceName_ = reply.ReadString();
        deviceInfo->address_ = reply.ReadString();
        deviceInfo->deviceCategory_ = reply.ReadInt32();
        deviceInfo->usageOrSourceType_ = reply.ReadInt32();
        perferredDevices.emplace(perferredType, deviceInfo);
    }
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::WriteAudioBuffer(const std::string &fileName, std::shared_ptr<AVBuffer> &buffer)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERR_INVALID_DATA, "input buffer nullptr");
    data.WriteInterfaceToken(GetDescriptor());
    buffer->WriteToMessageParcel(data);
    data.WriteString(fileName);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::WRITE_AUDIO_BUFFER), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "send pcm buffer failed %{public}d", error);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::GetInputBuffer(std::shared_ptr<AVBuffer> &buffer, int32_t size)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(size);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::GET_INPUT_BUFFER), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "get pcm buffer failed %{public}d", error);

    buffer = AVBuffer::CreateAVBuffer();
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERR_INVALID_DATA, "create buffer failed");
    if (buffer->ReadFromMessageParcel(reply) == false) {
        MEDIA_LOG_E("read data failed");
        return reply.ReadInt32();
    }
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERR_INVALID_DATA, "get input buffer failed");
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::InputBufferFilled(std::string fileName, uint64_t bufferId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteString(fileName);
    data.WriteUint64(bufferId);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::INPUT_BUFFER_FILL), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "send request error %{public}d", error);
    return reply.ReadInt32();
}

int32_t MediaMonitorClient::SetMediaParameters(const std::string& dumpType, const std::string& dumpEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteInterfaceToken(GetDescriptor());
    data.WriteString(dumpType);
    data.WriteString(dumpEnable);
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(MediaMonitorInterfaceCode::SET_MEDIA_PARAMS), data, reply, option);
    FALSE_RETURN_V_MSG_E(error == ERR_NONE, error, "set media param error %{public}d", error);
    return reply.ReadInt32();
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS