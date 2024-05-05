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
            iter->second->deviceCategory_ = bean->GetIntValue("CATEGORY");
            isInDeviceMap = true;
        }
    }
    if (isInDeviceMap) {
        std::shared_ptr<MonitorDeviceInfo> deviceInfo = std::make_shared<MonitorDeviceInfo>();
        deviceInfo->deviceType_ = bean->GetIntValue("DEVICE_TYPE");
        deviceInfo->deviceName_ = bean->GetIntValue("DEVICE_NAME");
        deviceInfo->address_ = bean->GetIntValue("ADDRESS");
        deviceInfo->deviceCategory_ = bean->GetIntValue("CATEGORY");
        perferredDevices_.emplace(perferredType, deviceInfo);
    }
}

void AudioMemo::GetAudioRouteMsg(std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> &perferredDevices)
{
    MEDIA_LOG_D("Begin get perferred device");
    std::lock_guard<std::mutex> lockEventMap(perferredDeviceMutex_);
    perferredDevices = perferredDevices_;
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
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS