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

#ifndef HISTREAMER_PLUGIN_MANAGER_V_H
#define HISTREAMER_PLUGIN_MANAGER_V_H

#include <vector>
#include "cached_plugin_package.h"

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
    std::shared_ptr<PluginBase> CreatePluginByMime(PluginType pluginType, std::string mime);
    std::shared_ptr<PluginBase> CreatePluginByName(std::string name);
    std::string SnifferPlugin(PluginType pluginType, std::shared_ptr<DataSource> dataSource);

private:
    PluginManagerV2();
    std::shared_ptr<CachedPluginPackage> cachedPluginPackage_;
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_PACKAGE_H
