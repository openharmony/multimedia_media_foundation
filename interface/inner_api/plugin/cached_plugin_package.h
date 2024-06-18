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

#ifndef HISTREAMER_CACHED_PLUGIN_PACKAGE_H
#define HISTREAMER_CACHED_PLUGIN_PACKAGE_H

#include <vector>
#include "plugin_package.h"
#include "plugin_base.h"
#include "plugin_list.h"
#include "osal/task/mutex.h"
#include "osal/task/autolock.h"

namespace OHOS {
namespace Media {
namespace Plugins {

class CachedPluginPackage {
public:
    std::shared_ptr<PluginBase> CreatePlugin(PluginDescription pluginDescription);
    std::shared_ptr<PluginDefBase> GetPluginDef(PluginDescription pluginDescription);

private:
    std::vector<std::shared_ptr<PluginPackage>> pluginPackageList_;
    Mutex pluginMutex_;
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_PACKAGE_H
