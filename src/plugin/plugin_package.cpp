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

#include "plugin/plugin_package.h"

#include <dlfcn.h>
#include "common/log.h"

namespace OHOS {
namespace Media {
namespace Plugins {
bool PluginPackage::LoadPluginPackage(std::string packageName)
{
    packageName_ = packageName;
    static std::string fileSeparator = "/";
    static std::string soPrefix = "libmedia_plugin_";
    static std::string soSuffix = ".z.so";
    std::string libPath = HST_PLUGIN_PATH + fileSeparator + soPrefix + packageName + soSuffix;
    auto libPathStr = libPath.c_str();
    soHandler_ = ::dlopen(libPathStr, RTLD_NOW);
    if (soHandler_ == nullptr) {
        MEDIA_LOG_E("dlopen failed due to " PUBLIC_LOG_S, ::dlerror());
        return false;
    }
    std::string registerFuncName = "register_" + packageName;
    std::string unregisterFuncName = "unregister_" + packageName;
    RegisterFunc registerFunc = nullptr;
    UnregisterFunc unregisterFunc = nullptr;
    MEDIA_LOG_I("dlopen registerFuncName " PUBLIC_LOG_S
        ", unregisterFuncName: " PUBLIC_LOG_S, registerFuncName.c_str(), unregisterFuncName.c_str());

    registerFunc = (RegisterFunc)(::dlsym(soHandler_, registerFuncName.c_str()));
    unregisterFunc = (UnregisterFunc)(::dlsym(soHandler_, unregisterFuncName.c_str()));

    if (registerFunc && unregisterFunc) {
        registerFunc(shared_from_this());
        return true;
    } else {
        return false;
    }
}

bool PluginPackage::UnLoadPluginPackage()
{
    (void)soHandler_;
    soHandler_ = nullptr;
    return true;
}

Status PluginPackage::AddPlugin(const PluginDefBase& def)
{
    MEDIA_LOG_I("AddPlugin " PUBLIC_LOG_S, def.name.c_str());
    std::shared_ptr<PluginDefBase> pluginDef = std::make_shared<PluginDefBase>();
    pluginDef->pluginType = def.pluginType;
    pluginDef->name = def.name;
    pluginDef->rank = def.rank;
    pluginDef->SetCreator(def.GetCreator());
    pluginDef->SetSniffer(def.GetSniffer());
    pluginDefList_.push_back(pluginDef);
    return Status::OK;
}

std::vector<std::shared_ptr<PluginDefBase>> PluginPackage::GetAllPlugins()
{
    return pluginDefList_;
}

} // namespace Plugins
} // namespace Media
} // namespace OHOS