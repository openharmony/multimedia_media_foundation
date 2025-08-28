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
#include "parameters.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "monitor_error.h"
#include "media_monitor_proxy.h"
#include "media_monitor_death_recipient.h"
#include "dump_buffer_wrap.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorManager"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

using namespace std;

static mutex g_mmProxyMutex;
static sptr<IMediaMonitor> g_mmProxy = nullptr;
static std::shared_ptr<DumpBufferWrap> g_dumpBufferWrap = nullptr;
constexpr int MAX_DUMP_TIME = 90;

MediaMonitorManager::MediaMonitorManager()
{
    versionType_ = OHOS::system::GetParameter("const.logsystem.versiontype", COMMERCIAL_VERSION);
    MEDIA_LOG_I("version type:%{public}s", versionType_.c_str());
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

void MediaMonitorManager::GetAudioAppStateMsg(std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> &appStateMap)
{
    MEDIA_LOG_D("Get audio app state msg");
    sptr<IMediaMonitor> proxy = GetMediaMonitorProxy();
    FALSE_RETURN_MSG(proxy != nullptr, "proxy is nullptr");
    int32_t ret;
    std::unordered_map<int32_t, MonitorAppStateInfo> appStateInfos;
    proxy->GetAudioAppStateMsg(appStateInfos, ret);
    FALSE_RETURN_MSG(ret == SUCCESS, "GetAudioAppStateMsg with error %{public}d", ret);
    for (auto &appStateInfo : appStateInfos) {
        std::shared_ptr<MonitorAppStateInfo> infoPtr = std::make_shared<MonitorAppStateInfo>(appStateInfo.second);
        appStateMap.emplace(appStateInfo.first, infoPtr);
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
    std::shared_ptr<DumpBuffer> bufferPtr = std::make_shared<DumpBuffer>();
    proxy->GetInputBuffer(*bufferPtr, size, ret);
    FALSE_RETURN_MSG(ret == SUCCESS, "get buffer failed.");
    FALSE_RETURN_MSG(bufferPtr != nullptr, "get buffer is nullptr.");

    std::shared_ptr<DumpBufferWrap> tmpBufferWrap = g_dumpBufferWrap;
    FALSE_RETURN_MSG(tmpBufferWrap != nullptr, "buffer wrap is nullptr.");

    int32_t bufferCapacitySize = tmpBufferWrap->GetCapacity(bufferPtr.get());
    FALSE_RETURN_MSG(bufferCapacitySize > 0, "get buffer capacity error");
    int32_t writeSize = tmpBufferWrap->Write(bufferPtr.get(), static_cast<uint8_t*>(ptr), size);
    FALSE_RETURN_MSG(writeSize > 0, "write buffer error");

    uint64_t bufferId = tmpBufferWrap->GetUniqueId(bufferPtr.get());
    proxy->InputBufferFilled(fileName, bufferId, writeSize, ret);
    FALSE_RETURN_MSG(ret == SUCCESS, "write buffer error %{public}d", ret);
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

    FALSE_RETURN_V_MSG_E(LoadDumpBufferWrap(dumpEnable) == SUCCESS, ERROR, "load buffer wrap error");

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

int32_t MediaMonitorManager::LoadDumpBufferWrap(const std::string &dumpEnable)
{
    bool flag = (dumpEnable == "true") ? true : false;
    if (flag && g_dumpBufferWrap != nullptr) {
        return SUCCESS;
    }

    if (flag) {
        g_dumpBufferWrap = std::make_shared<DumpBufferWrap>();
        bool ret = g_dumpBufferWrap->Open();
        if (!ret) {
            MEDIA_LOG_E("load dumpbuffer failed");
            g_dumpBufferWrap = nullptr;
            return ERROR;
        }
    } else {
        g_dumpBufferWrap = nullptr;
    }
    return SUCCESS;
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
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS