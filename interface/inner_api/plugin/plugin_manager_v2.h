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

#ifndef HISTREAMER_PLUGINS_MANAGER_V2_H
#define HISTREAMER_PLUGINS_MANAGER_V2_H


#include <unordered_map>
#include "plugin_info.h"
#include "generic_plugin.h"
#include "plugin_loader_v2.h"

namespace OHOS {
namespace Media {
namespace Plugins {
class PluginManagerV2 {
public:
    PluginManagerV2(const PluginManagerV2&) = delete;
    PluginManagerV2 operator=(const PluginManagerV2&) = delete;
    ~PluginManagerV2() = default;
    static PluginManagerV2& Instance()
    {
        static PluginManagerV2 impl;
        return impl;
    }
   
    std::shared_ptr<PluginBase> CreatePlugin(PluginType pluginType, SubPluginType subPluginType,
        const std::unordered_map<std::string, std::string> extra = {});
   
private:
    PluginManagerV2() = default;

    std::unordered_map<SubPluginType, std::shared_ptr<PluginLoaderV2>> pluginLoaderMap_;
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGINS_MANAGER_V2_H
