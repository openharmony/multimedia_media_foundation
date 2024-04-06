/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "PluginLoaderV2"

#include "common/log.h"
#include "plugin/plugin_loader_v2.h"
#include "plugin/plugin_manager_v2.h"

namespace OHOS {
namespace Media {
namespace Plugins {
std::shared_ptr<PluginBase> PluginManagerV2::CreatePlugin(PluginType pluginType, SubPluginType subPluginType,
    const std::unordered_map<std::string, std::string> extra)
{
    std::shared_ptr<PluginLoaderV2> pluginLoader;
    auto pluginLoaderEntry = pluginLoaderMap_.find(subPluginType);
    if (pluginLoaderEntry == pluginLoaderMap_.end()) {
        pluginLoader = std::make_shared<PluginLoaderV2>(pluginType, subPluginType);
        pluginLoaderMap_[subPluginType] = pluginLoader;
    } else {
        pluginLoader = pluginLoaderEntry->second;
    }

    return pluginLoader->CreatePlugin(extra);
}
} // namespace Plugins
} // namespace Media
} // namespace OHOS