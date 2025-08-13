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
constexpr int32_t EXCLUDED = 0;
constexpr int32_t UNEXCLUDED = 1;
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void AudioMemo::UpdataRouteInfo(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Begin updata preferred device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    bool isInDeviceMap = false;
    PreferredType preferredType = GetPreferredType(bean);
    std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>>::iterator iter;
    std::lock_guard<std::mutex> lockEventMap(preferredDeviceMutex_);
    for (iter = preferredDevices_.begin(); iter != preferredDevices_.end(); iter++) {
        if (iter->first == preferredType) {
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
        preferredDevices_.emplace(preferredType, deviceInfo);
    }
}

int32_t AudioMemo::GetAudioRouteMsg(std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> &preferredDevices)
{
    MEDIA_LOG_D("Begin get preferred device");
    std::lock_guard<std::mutex> lockEventMap(preferredDeviceMutex_);
    preferredDevices = preferredDevices_;
    return SUCCESS;
}

PreferredType AudioMemo::GetPreferredType(std::shared_ptr<EventBean> &bean)
{
    if (bean->GetIntValue("IS_PLAYBACK")) {
        return GetPreferredRenderType(bean->GetIntValue("STREAM_TYPE"));
    } else {
        return GetPreferredCaptureType(bean->GetIntValue("AUDIO_SCENE"));
    }
}

PreferredType AudioMemo::GetPreferredRenderType(int32_t streamUsage)
{
    if (streamUsage == AudioStandard::STREAM_USAGE_VOICE_COMMUNICATION ||
        streamUsage == AudioStandard::STREAM_USAGE_VOICE_MODEM_COMMUNICATION ||
        streamUsage == AudioStandard::STREAM_USAGE_VIDEO_COMMUNICATION) {
        return CALL_RENDER;
    } else {
        return MEDIA_RENDER;
    }
}

PreferredType AudioMemo::GetPreferredCaptureType(int32_t audioScene)
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
        std::lock_guard<std::mutex> lockEventMap(preferredDeviceMutex_);
        if (preferredDevices_.size() == 0) {
            dumpString += "No preferred device set.\n";
            return;
        }
        dumpString += "Preferred Device\n";
        for (auto &it : preferredDevices_) {
            dumpString += "    preferred type: " + GetPreferredNameFromType(it.first) + "\n";
            dumpString += "    device type: " + std::to_string(it.second->deviceType_) + "\n";
            dumpString += "\n";
        }
    }
}

std::string AudioMemo::GetPreferredNameFromType(const PreferredType &type)
{
    std::string preferredName = "";
    switch (type) {
        case CALL_RENDER:
            preferredName = "call render";
            break;
        case MEDIA_RENDER:
            preferredName = "media render";
            break;
        case CALL_CAPTURE:
            preferredName = "call capture";
            break;
        case RECORD_CAPTURE:
            preferredName = "record capture";
            break;
        default:
            break;
    }
    return preferredName;
}

int32_t AudioMemo::ErasePreferredDeviceByType(const PreferredType &preferredType)
{
    MEDIA_LOG_D("Erase preferred device by type");
    std::lock_guard<std::mutex> lockEventMap(preferredDeviceMutex_);
    auto iter = preferredDevices_.find(preferredType);
    if (iter != preferredDevices_.end()) {
        preferredDevices_.erase(iter);
        return SUCCESS;
    }
    return ERROR;
}

void AudioMemo::UpdateExcludedDevice(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Begin updata excluded device");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    AudioDeviceUsage audioDeviceUsage = static_cast<AudioDeviceUsage>(bean->GetIntValue("AUDIO_DEVICE_USAGE"));
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>>::iterator iter;
    std::lock_guard<std::mutex> lockEventMap(excludedDeviceMutex_);
    std::shared_ptr<MonitorDeviceInfo> deviceInfo = std::make_shared<MonitorDeviceInfo>();
    deviceInfo->deviceType_ = bean->GetIntValue("DEVICE_TYPE");
    deviceInfo->deviceCategory_ = bean->GetIntValue("BT_TYPE");
    deviceInfo->usageOrSourceType_ = bean->GetIntValue("STREAM_TYPE");
    deviceInfo->audioDeviceUsage_ = bean->GetIntValue("AUDIO_DEVICE_USAGE");
    deviceInfo->deviceName_ = bean->GetStringValue("DEVICE_NAME");
    deviceInfo->address_ = bean->GetStringValue("ADDRESS");
    deviceInfo->networkId_ = bean->GetStringValue("NETWORKID");
    int32_t exclusionStatus = bean->GetIntValue("EXCLUSION_STATUS");
    UpdateExcludedDeviceInner(audioDeviceUsage, deviceInfo, exclusionStatus);
}

void AudioMemo::UpdateExcludedDeviceInner(AudioDeviceUsage audioDevUsage,
    std::shared_ptr<MonitorDeviceInfo> &deviceInfo, int32_t exclusionStatus)
{
    auto isPresent = [&deviceInfo](const std::shared_ptr<MonitorDeviceInfo> &device) {
        return device->deviceType_ == deviceInfo->deviceType_ &&
            device->address_ == deviceInfo->address_ &&
            device->networkId_ == deviceInfo->networkId_;
    };
    if (exclusionStatus == EXCLUDED) {
        auto it = find_if(excludedDevices_[audioDevUsage].begin(), excludedDevices_[audioDevUsage].end(),
            isPresent);
        if (it == excludedDevices_[audioDevUsage].end()) {
            excludedDevices_[audioDevUsage].push_back(deviceInfo);
        }
    } else if (exclusionStatus == UNEXCLUDED) {
        auto it = find_if(excludedDevices_[audioDevUsage].begin(), excludedDevices_[audioDevUsage].end(),
            isPresent);
        if (it != excludedDevices_[audioDevUsage].end()) {
            excludedDevices_[audioDevUsage].erase(it);
        }
    }
}

int32_t AudioMemo::GetAudioExcludedDevicesMsg(std::map<AudioDeviceUsage,
    std::vector<std::shared_ptr<MonitorDeviceInfo>>> &excludedDevices)
{
    MEDIA_LOG_I("Begin get excluded device");
    std::lock_guard<std::mutex> lockEventMap(excludedDeviceMutex_);
    excludedDevices = excludedDevices_;
    return SUCCESS;
}

void AudioMemo::UpdateCollaborativeDeviceState(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Begin update collaborative device state");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    std::string deviceAddress_ = bean->GetStringValue("ADDRESS");
    uint32_t state = bean->GetIntValue("COLLABORATIVE_STATE");
    std::lock_guard<std::mutex> lockEventMap(collaborativeMutex_);
    addressToCollaborativeEnabledMap_[deviceAddress_] = state;
}

int32_t AudioMemo::GetCollaborativeDeviceState(std::map<std::string, uint32_t> &addressToCollaborativeEnabledMap)
{
    MEDIA_LOG_D("Begin get collaborative device state");
    std::lock_guard<std::mutex> lockEventMap(collaborativeMutex_);
    addressToCollaborativeEnabledMap = addressToCollaborativeEnabledMap_;
    return SUCCESS;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS