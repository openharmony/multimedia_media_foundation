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

#include "plugin/plugin_manager_v2.h"
#include "common/log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "PluginManagerV2" };
}

namespace OHOS {
namespace Media {
namespace Plugins {
PluginManagerV2::PluginManagerV2()
{
    MEDIA_LOG_I("PluginManagerV2");
    cachedPluginPackage_ = std::make_shared<CachedPluginPackage>();
}

std::shared_ptr<PluginBase> PluginManagerV2::CreatePluginByMime(PluginType pluginType, std::string mime)
{
    MEDIA_LOG_I("CreatePluginByMime pluginType: " PUBLIC_LOG_D32 " mime: " PUBLIC_LOG_S, pluginType, mime.c_str());
    std::vector<PluginDescription> pluginDescriptions = PluginList::GetInstance().GetPluginsByCap(pluginType, mime);
    for (auto desc: pluginDescriptions) {
        std::shared_ptr<PluginBase> plugin = cachedPluginPackage_->CreatePlugin(desc);
        if (plugin != nullptr) {
            return plugin;
        }
    }
    return nullptr;
}

std::shared_ptr<PluginBase> PluginManagerV2::CreatePluginByName(std::string name)
{
    MEDIA_LOG_I("CreatePluginByName pluginName: " PUBLIC_LOG_S, name.c_str());
    PluginDescription pluginDescription = PluginList::GetInstance().GetPluginByName(name);
    return cachedPluginPackage_->CreatePlugin(pluginDescription);
}

std::string PluginManagerV2::SnifferPlugin(PluginType pluginType, std::shared_ptr<DataSource> dataSource)
{
    MEDIA_LOG_I("SnifferPlugin pluginType: " PUBLIC_LOG_D32, pluginType);
    std::vector<PluginDescription> matchedPluginsDescriptions =
        PluginList::GetInstance().GetPluginsByType(pluginType);
    int maxProb = 0;
    std::vector<PluginDescription>::iterator it;
    PluginDescription bestMatchedPlugin;
    for (it = matchedPluginsDescriptions.begin(); it != matchedPluginsDescriptions.end(); it++) {
        std::shared_ptr<PluginDefBase> pluginDef = cachedPluginPackage_->GetPluginDef(*it);
        if (pluginDef != nullptr) {
            auto prob = pluginDef->GetSniffer()(pluginDef->name, dataSource);
            if (prob > maxProb) {
                maxProb = prob;
                bestMatchedPlugin = (*it);
            }
        }
    }
    return bestMatchedPlugin.pluginName;
}
} // namespace Plugins
} // namespace Media
} // namespace OHOS