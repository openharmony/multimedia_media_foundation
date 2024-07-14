/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "plugin/cached_plugin_package.h"

#include "common/log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer" };
}

namespace OHOS {
namespace Media {
namespace Plugins {
std::shared_ptr<PluginBase> CachedPluginPackage::CreatePlugin(PluginDescription pluginDescription)
{
    MEDIA_LOG_I("CreatePlugin " PUBLIC_LOG_S, pluginDescription.pluginName.c_str());
    std::shared_ptr<PluginDefBase> pluginDef = GetPluginDef(pluginDescription);
    if (pluginDef != nullptr) {
        auto plugin = pluginDef->GetCreator()(pluginDef->name);
        if (!plugin) {
            return {};
        }
        return std::shared_ptr<PluginBase>(plugin);
    } else {
        MEDIA_LOG_I("pluginDef is nullptr");
        return nullptr;
    }
}

std::shared_ptr<PluginDefBase> CachedPluginPackage::GetPluginDef(PluginDescription pluginDescription)
{
    AutoLock lock(pluginMutex_);
    std::vector<std::shared_ptr<PluginPackage>>::iterator itPluginPackage;
    for (itPluginPackage = pluginPackageList_.begin();
        itPluginPackage != pluginPackageList_.end(); itPluginPackage++) {
        if (*itPluginPackage == nullptr) {
            return nullptr;
        }
        std::vector<std::shared_ptr<PluginDefBase>> pluginDefList = (*itPluginPackage)->GetAllPlugins();
        std::vector<std::shared_ptr<PluginDefBase>>::iterator itPluginDef;
        for (itPluginDef = pluginDefList.begin(); itPluginDef != pluginDefList.end(); itPluginDef++) {
            if (*itPluginDef == nullptr) {
                return nullptr;
            }
            if (strcmp((*itPluginDef)->name.c_str(), pluginDescription.pluginName.c_str()) == 0) {
                return (*itPluginDef);
            }
        }
    }
    std::shared_ptr<PluginPackage> pluginPackage = std::make_shared<PluginPackage>();
    bool ret = pluginPackage->LoadPluginPackage(pluginDescription.packageName);
    if (!ret) {
        return nullptr;
    }
    pluginPackageList_.push_back(pluginPackage);
    std::vector<std::shared_ptr<PluginDefBase>> pluginDefList = pluginPackage->GetAllPlugins();
    std::vector<std::shared_ptr<PluginDefBase>>::iterator itPluginDef;
    for (itPluginDef = pluginDefList.begin(); itPluginDef != pluginDefList.end(); itPluginDef++) {
        if (*itPluginDef == nullptr) {
            return nullptr;
        }
        if (strcmp((*itPluginDef)->name.c_str(), pluginDescription.pluginName.c_str()) == 0) {
            return (*itPluginDef);
        }
    }
    return nullptr;
}

} // namespace Plugins
} // namespace Media
} // namespace OHOS