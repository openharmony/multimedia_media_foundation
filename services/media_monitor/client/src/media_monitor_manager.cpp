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

#include "media_monitor_manager.h"
#include "log.h"
#include "parameter.h"
#include "parameters.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "monitor_error.h"
#include "media_monitor_proxy.h"
#include "media_monitor_death_recipient.h"
#include "audio_dump_buffer.h"
#include <atomic>
#include <cstring>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorManager"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

using namespace std;

static mutex g_mmProxyMutex;
static sptr<IMediaMonitor> g_mmProxy = nullptr;
constexpr int MAX_DUMP_TIME = 90;

std::atomic_bool MediaMonitorManager::hiviewUeEnable_{false};

MediaMonitorManager::MediaMonitorManager()
{
    versionType_ = OHOS::system::GetParameter("const.logsystem.versiontype", COMMERCIAL_VERSION);
    MEDIA_LOG_I("version type:%{public}s", versionType_.c_str());
    WatchHiviewUeEnableParameter();
}

MediaMonitorManager& MediaMonitorManager::GetInstance()
{
    static MediaMonitorManager monitorManager;
    return monitorManager;
}

static const sptr<IMediaMonitor> GetMediaMonitorProxy()
{
    MEDIA_LOG_D("Start to get media monitor manager proxy.");
    lock_guard<mutex> lock(g_mmProxyMutex);

    if (g_mmProxy == nullptr) {
        auto smmgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        FALSE_RETURN_V_MSG_E(smmgr != nullptr, nullptr, "smmgr init failed.");

        sptr<IRemoteObject> object = smmgr->CheckSystemAbility(MEDIA_MONITOR_SERVICE_ID);
        FALSE_RETURN_V_MSG_E(object != nullptr, nullptr, "Object is NULL.");

        g_mmProxy = iface_cast<IMediaMonitor>(object);
        FALSE_RETURN_V_MSG_E(g_mmProxy != nullptr, nullptr, "Init g_mmProxy is NULL.");

        MEDIA_LOG_I("Init g_mmProxy is assigned.");
        pid_t pid = 0;
        sptr<MediaMonitorDeathRecipient> deathRecipient = new(std::nothrow) MediaMonitorDeathRecipient(pid);
        if (deathRecipient != nullptr) {
            deathRecipient->SetNotifyCb(std::bind(&MediaMonitorManager::MediaMonitorDied,
                std::placeholders::_1));
            MEDIA_LOG_I("Register media monitor death recipient");
            bool result = object->AddDeathRecipient(deathRecipient);
            if (!result) {
                MEDIA_LOG_E("failed to add deathRecipient");
            }
        }
    }
    return g_mmProxy;
}

void MediaMonitorManager::HiviewUeEnableChangeCallback(const char *key, const char *value, void *context)
{
    FALSE_RETURN_MSG(value != nullptr, "HiviewUeEnableChangeCallback: value is null");
    bool enable = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0);
    hiviewUeEnable_.store(enable, std::memory_order_release);
    MEDIA_LOG_I("HiviewUeEnable changed to %{public}d", enable);
}

void MediaMonitorManager::WatchHiviewUeEnableParameter()
{
    // Initialize with current value
    hiviewUeEnable_.store(OHOS::system::GetBoolParameter("persist.hiviewdfx.hiview.ue.enable", false),
        std::memory_order_release);

    // Register parameter change callback
    int32_t ret = WatchParameter("persist.hiviewdfx.hiview.ue.enable",
        HiviewUeEnableChangeCallback, this);
    FALSE_RETURN_MSG(ret == SUCCESS, "WatchHiviewUeEnableParameter failed!");
    MEDIA_LOG_I("WatchHiviewUeEnableParameter success, current value: %{public}d",
        hiviewUeEnable_.load(std::memory_order_relaxed));
}

void MediaMonitorManager::MediaMonitorDied(pid_t pid)
{
    MEDIA_LOG_I("media monitor died");
    lock_guard<mutex> lock(g_mmProxyMutex);
    if (g_mmProxy == nullptr) {
        MEDIA_LOG_I("media monitor has already died!");
        return;
    }
    g_mmProxy = nullptr;
}

void MediaMonitorManager::WriteLogMsg(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write event to media monitor");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    FALSE_RETURN_MSG(hiviewUeEnable_.load(std::memory_order_relaxed),
        "persist.hiviewdfx.hiview.ue.enable is false, skip write log msg");
    proxy->WriteLogMsg(*bean);
}

void MediaMonitorManager::GetAudioRouteMsg(std::map<PreferredType, shared_ptr<MonitorDeviceInfo>> &preferredDevices)
{
    MEDIA_LOG_D("Get audio route devices");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }

    int32_t ret;
    std::unordered_map<int32_t, MonitorDeviceInfo> deviceInfos;
    proxy->GetAudioRouteMsg(deviceInfos, ret);
    for (auto &deviceInfo : deviceInfos) {
        PreferredType preferredType = static_cast<PreferredType>(deviceInfo.first);
        shared_ptr<MonitorDeviceInfo> info = std::make_shared<MonitorDeviceInfo>(deviceInfo.second);
        preferredDevices.emplace(preferredType, info);
    }
}

void MediaMonitorManager::GetAudioExcludedDevicesMsg(std::map<AudioDeviceUsage,
    std::vector<std::shared_ptr<MonitorDeviceInfo>>> &excludedDevices)
{
    MEDIA_LOG_D("Get audio excluded devices");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    int32_t ret;
    std::unordered_map<int32_t, std::vector<MonitorDeviceInfo>> deviceInfos;
    proxy->GetAudioExcludedDevicesMsg(deviceInfos, ret);
    for (auto &deviceInfo : deviceInfos) {
        AudioDeviceUsage deviceUsage = static_cast<AudioDeviceUsage>(deviceInfo.first);
        std::vector<std::shared_ptr<MonitorDeviceInfo>> infoList;
        for (auto &info : deviceInfo.second) {
            std::shared_ptr<MonitorDeviceInfo> infoPtr = std::make_shared<MonitorDeviceInfo>(info);
            infoList.emplace_back(infoPtr);
        }
        excludedDevices.emplace(deviceUsage, infoList);
    }
}

void MediaMonitorManager::GetAudioAppSessionMsg(std::map<int32_t, bool> &avSessionMap)
{
    MEDIA_LOG_D("Get audio app session msg");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    FALSE_RETURN_MSG(proxy != nullptr, "proxy is nullptr");
    int32_t ret;
    std::unordered_map<int32_t, bool> sessionInfos;
    proxy->GetAudioAppSessionMsg(sessionInfos, ret);
    FALSE_RETURN_MSG(ret == SUCCESS, "GetAudioAppSessionMsg with error %{public}d", ret);
    for (auto &sessionInfo : sessionInfos) {
        avSessionMap.emplace(sessionInfo.first, sessionInfo.second);
    }
}

void MediaMonitorManager::GetAudioAppBackTaskMsg(std::map<int32_t, bool> &backTaskMap)
{
    MEDIA_LOG_D("Get audio app back task msg");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    FALSE_RETURN_MSG(proxy != nullptr, "proxy is nullptr");
    int32_t ret;
    std::unordered_map<int32_t, bool> backTaskInfos;
    proxy->GetAudioAppBackTaskMsg(backTaskInfos, ret);
    FALSE_RETURN_MSG(ret == SUCCESS, "GetAudioAppBackTaskMsg with error %{public}d", ret);
    for (auto &backTaskInfo : backTaskInfos) {
        backTaskMap.emplace(backTaskInfo.first, backTaskInfo.second);
    }
}

void MediaMonitorManager::WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size)
{
    if (!dumpEnable_) {
        MEDIA_LOG_D("dump status error return");
        return;
    }

    if (versionType_ != BETA_VERSION) {
        return;
    }

    int duration = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - dumpStartTime_;
    if (duration > MAX_DUMP_TIME) {
        MEDIA_LOG_I("dump duration > 90 return");
        dumpEnable_ = false;
        dumpStartTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return;
    }

    FALSE_RETURN_MSG(ptr != nullptr, "in data is empty");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    FALSE_RETURN_MSG(proxy != nullptr, "proxy is nullptr");

    int32_t ret;
    AudioDumpBuffer buffer;
    buffer.size = size;
    buffer.RawDataCpy(ptr);
    proxy->WriteAudioBuffer(fileName, buffer, ret);
    MEDIA_LOG_D("write audio buffer ret %{public}d", ret);
}

int32_t MediaMonitorManager::GetMediaParameters(const std::vector<std::string> &subKeys,
    std::vector<std::pair<std::string, std::string>> &result)
{
    if (versionType_ != BETA_VERSION) {
        MEDIA_LOG_E("version type is commercial");
        return ERROR;
    }

    bool match = false;
    std::string matchKey;
    std::string value = "false";
    for (auto it = subKeys.begin(); it != subKeys.end(); it++) {
        if (*it == BETA_DUMP_TYPE || *it == DEFAULT_DUMP_TYPE) {
            match = true;
            matchKey = *it;
            break;
        }
    }
    FALSE_RETURN_V_MSG_E(match, ERROR, "get media param invalid param");

    if (dumpEnable_ == false) {
        result.emplace_back(std::make_pair(matchKey, value));
        MEDIA_LOG_I("get media param: close");
        return SUCCESS;
    }

    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return ERROR;
    }

    int32_t status = 0;
    int32_t ret;
    proxy->GetPcmDumpStatus(status, ret);
    if (ret != SUCCESS) {
        MEDIA_LOG_E("get dump media param failed");
        return ERROR;
    }
    if (status > 0) {
        value = "true";
    }

    result.emplace_back(std::make_pair(matchKey, value));
    MEDIA_LOG_I("get media param: %{public}s", value.c_str());
    return SUCCESS;
}

int32_t MediaMonitorManager::SetMediaParameters(const std::vector<std::pair<std::string, std::string>> &kvpairs)
{
    if (versionType_ != BETA_VERSION) {
        MEDIA_LOG_E("version type is commercial");
        return ERROR;
    }
    std::string dumpType;
    std::string dumpEnable;
    for (auto it = kvpairs.begin(); it != kvpairs.end(); ++it) {
        if (it->first == DEFAULT_DUMP_TYPE || it->first == BETA_DUMP_TYPE) {
            dumpType = it->first;
            dumpEnable = it->second;
            break;
        }
        return ERROR;
    }

    if (dumpEnable_ && dumpEnable == "true") {
        int duration = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - dumpStartTime_;
        if (duration < MAX_DUMP_TIME) {
            MEDIA_LOG_E("set dump media param failed, already dumping");
            return ERROR;
        }
    }
    dumpType_ = dumpType;
    dumpEnable_ = (dumpEnable == "true") ? true : false;
    if (dumpEnable_) {
        dumpStartTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    MEDIA_LOG_I("set dump media param, %{public}d %{public}s", dumpEnable_, dumpType_.c_str());
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return ERROR;
    }

    int32_t ret;
    proxy->SetMediaParameters(dumpType, dumpEnable, ret);
    if (ret != SUCCESS) {
        MEDIA_LOG_E("set dump media param failed");
    }
    return ret;
}

int32_t MediaMonitorManager::ErasePreferredDeviceByType(const PreferredType preferredType)
{
    MEDIA_LOG_D("Erase preferred device by type");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return ERROR;
    }
    int32_t ret;
    proxy->ErasePreferredDeviceByType(preferredType, ret);
    return ret;
}

void MediaMonitorManager::GetCollaborativeDeviceState(
    std::map<std::string, uint32_t> &addressToCollaborativeEnabledMap)
{
    MEDIA_LOG_D("Get audio route devices");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    int32_t ret;
    std::unordered_map<std::string, uint32_t> storedCollaborativeMap;
    // idl only accepts unorderedmap!
    proxy->GetCollaborativeDeviceState(storedCollaborativeMap, ret);
    for (auto &p: storedCollaborativeMap) {
        addressToCollaborativeEnabledMap[p.first] = p.second;
    }
}

void MediaMonitorManager::GetDistributedDeviceInfo(std::vector<std::string> &deviceInfos)
{
    MEDIA_LOG_D("Get distributed device info");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    int32_t ret;
    proxy->GetDistributedDeviceInfo(deviceInfos, ret);
}

void MediaMonitorManager::GetDistributedSceneInfo(std::string &sceneInfo)
{
    MEDIA_LOG_D("Get distributed scene info");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    int32_t ret;
    proxy->GetDistributedSceneInfo(sceneInfo, ret);
}

void MediaMonitorManager::GetDmDeviceInfo(std::vector<MonitorDmDeviceInfo> &dmDeviceInfos)
{
    MEDIA_LOG_D("Get dm device info");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    if (proxy == nullptr) {
        MEDIA_LOG_E("proxy is nullptr.");
        return;
    }
    int32_t ret;
    proxy->GetDmDeviceInfo(dmDeviceInfos, ret);
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS