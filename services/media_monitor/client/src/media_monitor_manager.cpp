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
#include "media_monitor_base.h"
#include "media_monitor_client.h"
#include "media_monitor_death_recipient.h"

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
    sptr<IMediaMonitor> gamp = GetMediaMonitorProxy();
    if (gamp == nullptr) {
        MEDIA_LOG_E("gamp is nullptr.");
        return;
    }
    gamp->WriteLogMsg(bean);
}

void MediaMonitorManager::GetAudioRouteMsg(std::map<PerferredType, shared_ptr<MonitorDeviceInfo>> &perferredDevices)
{
    MEDIA_LOG_D("Get audio route devices");
    sptr<IMediaMonitor> gamp = GetMediaMonitorProxy();
    if (gamp == nullptr) {
        MEDIA_LOG_E("gamp is nullptr.");
        return;
    }
    gamp->GetAudioRouteMsg(perferredDevices);
}

void MediaMonitorManager::WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size)
{
    if (!dumpEnable_) {
        MEDIA_LOG_D("dump status error return");
        return;
    }
    if (versionType_ == COMMERCIAL_VERSION) {
        return;
    }
    int duration = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - dumpStartTime_;
    if (duration > MAX_DUMP_TIME) {
        MEDIA_LOG_I("dump duration > 90 return");
        dumpEnable_ = false;
        dumpStartTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return;
    }
    FALSE_RETURN_MSG(ptr != nullptr, "inbuffer is nullptr.");
    sptr<IMediaMonitor> gamp = GetMediaMonitorProxy();
    FALSE_RETURN_MSG(gamp != nullptr, "gamp is nullptr.");
    std::shared_ptr<AVBuffer> bufferPtr = nullptr;
    int32_t ret = gamp->GetInputBuffer(bufferPtr, size);
    FALSE_RETURN_MSG(ret == SUCCESS, "get buffer failed.");
    FALSE_RETURN_MSG(bufferPtr != nullptr, "get buffer is nullptr.");
    FALSE_RETURN_MSG(bufferPtr->memory_ != nullptr, "get memory is nullptr.");
    if (bufferPtr->memory_->GetCapacity() > 0) {
        ret = bufferPtr->memory_->Write(static_cast<uint8_t*>(ptr), size, 0);
        MEDIA_LOG_D("write pcm buffer size  %{public}d", bufferPtr->memory_->GetSize());
        FALSE_RETURN_MSG(ret > 0, "write to buffer failed");
    } else {
        MEDIA_LOG_E("create avbuffer failed");
    }
    ret = gamp->WriteAudioBuffer(fileName, bufferPtr);
}

int32_t MediaMonitorManager::SetMediaParameters(const std::vector<std::pair<std::string, std::string>>& kvpairs)
{
    if (versionType_ == COMMERCIAL_VERSION) {
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

    MEDIA_LOG_D("set dump media param, %{public}d %{public}s", dumpEnable_, dumpType_.c_str());
    sptr<IMediaMonitor> gamp = GetMediaMonitorProxy();
    if (gamp == nullptr) {
        MEDIA_LOG_E("gamp is nullptr.");
        return ERROR;
    }

    int32_t ret = gamp->SetMediaParameters(dumpType, dumpEnable);
    if (ret != SUCCESS) {
        MEDIA_LOG_E("set dump media param failed");
    }

    return ret;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS