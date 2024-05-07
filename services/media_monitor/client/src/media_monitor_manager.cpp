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

#include "log.h"
#include "media_monitor_manager.h"
#include "monitor_error.h"
#include "media_monitor_client.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "media_monitor_death_recipient.h"


namespace OHOS {
namespace Media {
namespace MediaMonitor {

using namespace std;

static mutex g_mmProxyMutex;
static sptr<IMediaMonitor> g_mmProxy = nullptr;

static const sptr<IMediaMonitor> GetMediaMonitorProxy()
{
    MEDIA_LOG_D("Start to get media monitor manager proxy.");
    lock_guard<mutex> lock(g_mmProxyMutex);

    if (g_mmProxy == nullptr) {
        auto smmgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        FALSE_RETURN_V_MSG_E(smmgr != nullptr, nullptr, "smmgr init failed.");

        sptr<IRemoteObject> object = smmgr->GetSystemAbility(MEDIA_MONITOR_SERVICE_ID);
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
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS