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

#include "audio_memo.h"
#include "log.h"
#include "audio_stream_info.h"
#include "audio_source_type.h"
#include "audio_info.h"
#include "monitor_error.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "AudioMemo"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void AudioMemo::UpdataRouteInfo(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Begin updata perferred device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    bool isInDeviceMap = false;
    PerferredType perferredType = GetPerferredType(bean);
    std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>>::iterator iter;
    std::lock_guard<std::mutex> lockEventMap(perferredDeviceMutex_);
    for (iter = perferredDevices_.begin(); iter != perferredDevices_.end(); iter++) {
        if (iter->first == perferredType) {
            iter->second->deviceType_ = bean->GetIntValue("DEVICE_TYPE");
            iter->second->deviceName_ = bean->GetIntValue("DEVICE_NAME");
            iter->second->address_ = bean->GetIntValue("ADDRESS");
            iter->second->deviceCategory_ = bean->GetIntValue("BT_TYPE");
            iter->second->usageOrSourceType_ = bean->GetIntValue("STREAM_TYPE");
            isInDeviceMap = true;
        }
    }
    if (!isInDeviceMap) {
        std::shared_ptr<MonitorDeviceInfo> deviceInfo = std::make_shared<MonitorDeviceInfo>();
        deviceInfo->deviceType_ = bean->GetIntValue("DEVICE_TYPE");
        deviceInfo->deviceName_ = bean->GetIntValue("DEVICE_NAME");
        deviceInfo->address_ = bean->GetIntValue("ADDRESS");
        deviceInfo->deviceCategory_ = bean->GetIntValue("BT_TYPE");
        deviceInfo->usageOrSourceType_ = bean->GetIntValue("STREAM_TYPE");
        perferredDevices_.emplace(perferredType, deviceInfo);
    }
}

int32_t AudioMemo::GetAudioRouteMsg(std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> &perferredDevices)
{
    MEDIA_LOG_D("Begin get perferred device");
    std::lock_guard<std::mutex> lockEventMap(perferredDeviceMutex_);
    perferredDevices = perferredDevices_;
    return SUCCESS;
}

PerferredType AudioMemo::GetPerferredType(std::shared_ptr<EventBean> &bean)
{
    if (bean->GetIntValue("IS_PLAYBACK")) {
        return GetPerferredRenderType(bean->GetIntValue("STREAM_TYPE"));
    } else {
        return GetPerferredCaptureType(bean->GetIntValue("AUDIO_SCENE"));
    }
}

PerferredType AudioMemo::GetPerferredRenderType(int32_t streamUsage)
{
    if (streamUsage == AudioStandard::STREAM_USAGE_VOICE_COMMUNICATION ||
        streamUsage == AudioStandard::STREAM_USAGE_VOICE_MODEM_COMMUNICATION ||
        streamUsage == AudioStandard::STREAM_USAGE_VIDEO_COMMUNICATION) {
        return CALL_RENDER;
    } else {
        return MEDIA_RENDER;
    }
}

PerferredType AudioMemo::GetPerferredCaptureType(int32_t audioScene)
{
    if (audioScene == AudioStandard::AUDIO_SCENE_PHONE_CALL ||
        audioScene == AudioStandard::AUDIO_SCENE_PHONE_CHAT ||
        audioScene == AudioStandard::SOURCE_TYPE_VOICE_COMMUNICATION) {
        return CALL_CAPTURE;
    } else {
        return RECORD_CAPTURE;
    }
}

void AudioMemo::WriteInfo(int32_t fd, std::string &dumpString)
{
    if (fd != -1) {
        std::lock_guard<std::mutex> lockEventMap(perferredDeviceMutex_);
        if (perferredDevices_.size() == 0) {
            dumpString += "No preferred device set.\n";
            return;
        }
        dumpString += "Perferred Device\n";
        for (auto &it : perferredDevices_) {
            dumpString += "    perferred type: " + GetPerferredNameFromType(it.first) + "\n";
            dumpString += "    device type: " + std::to_string(it.second->deviceType_) + "\n";
            dumpString += "\n";
        }
    }
}

std::string AudioMemo::GetPerferredNameFromType(const PerferredType &type)
{
    std::string perferredName = "";
    switch (type) {
        case CALL_RENDER:
            perferredName = "call render";
            break;
        case MEDIA_RENDER:
            perferredName = "media render";
            break;
        case CALL_CAPTURE:
            perferredName = "call capture";
            break;
        case RECORD_CAPTURE:
            perferredName = "record capture";
            break;
        default:
            break;
    }
    return perferredName;
}

int32_t AudioMemo::ErasePreferredDeviceByType(const PerferredType &preferredType)
{
    MEDIA_LOG_D("Erase preferred device by type");
    std::lock_guard<std::mutex> lockEventMap(perferredDeviceMutex_);
    auto iter = perferredDevices_.find(preferredType);
    if (iter != perferredDevices_.end()) {
        perferredDevices_.erase(iter);
        return SUCCESS;
    }
    return ERROR;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS