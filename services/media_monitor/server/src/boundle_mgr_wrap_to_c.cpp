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

#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "media_monitor_wrapper.h"
#include "system_ability_definition.h"

using namespace OHOS::Media::MediaMonitor;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

extern "C" MediaMonitorErr GetBundleInfoFromUid(int32_t appUid, BundleInfo *bundleInfoRes)
{
    std::string bundleName {""};
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    auto systemAbilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    FALSE_RETURN_V_MSG_E(
        systemAbilityManager != nullptr, MediaMonitorErr::ERR_INVALID_OPERATION, "systemAbilityManager is nullptr");

    OHOS::sptr<OHOS::IRemoteObject> remoteObject
        = systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    FALSE_RETURN_V_MSG_E(
        remoteObject != nullptr, MediaMonitorErr::ERR_INVALID_OPERATION, "remoteObject is nullptr");

    OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgrProxy
        = OHOS::iface_cast<OHOS::AppExecFwk::IBundleMgr>(remoteObject);
    FALSE_RETURN_V_MSG_E(
        bundleMgrProxy != nullptr, MediaMonitorErr::ERR_INVALID_OPERATION, "bundleMgrProxy is nullptr");

    bundleMgrProxy->GetNameForUid(appUid, bundleName);

    bundleMgrProxy->GetBundleInfoV9(bundleName, OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT |
        OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION |
        OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_WITH_HASH_VALUE,
        bundleInfo,
        OHOS::AppExecFwk::Constants::ALL_USERID);
    bundleInfoRes->appName = bundleInfo.name;
    bundleInfoRes->versionCode = static_cast<int32_t> (bundleInfo.versionCode);

    return MediaMonitorErr::SUCCESS;
}
