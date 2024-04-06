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

#ifndef HISTREAMER_PLUGINS_LOADER_V2_H
#define HISTREAMER_PLUGINS_LOADER_V2_H

#include "plugin/plugin_base.h"
#include "plugin/plugin_definition.h"
#include "meta/media_types.h"
#include <unordered_map>
#include <stack>

#define REGISTER_PLUGIN                                                                                                \
extern "C" __attribute__((visibility ("default"))) void RegisterPlugin(                                                \
    const std::shared_ptr<OHOS::Media::Plugins::PluginLoaderV2> pluginLoader)

namespace OHOS {
namespace Media {
namespace Plugins {

template <typename T>
auto PrintType(T const value)-> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

class PluginLoaderV2 : public std::enable_shared_from_this<PluginLoaderV2> {
public:
    PluginLoaderV2(PluginType pluginType, SubPluginType subPluginType);

    ~PluginLoaderV2() = default;

    std::shared_ptr<PluginBase> CreatePlugin(const std::unordered_map<std::string, std::string>& extra);

    std::unordered_map<std::string, std::string>& GetExtraInfo();

    void RegisterPlugin(std::shared_ptr<PluginBase> plugin);

private:
    using RegisterPluginFunc = void (*)(const std::shared_ptr<PluginLoaderV2> pluginLoader);

    using GetScoreFunc = int (*)();

    static const std::string PLUGIN_32_ROOT_PATH;

    static const std::string PLUGIN_64_ROOT_PATH;

    static const std::string REGISTER_PLUGIN_FUNC_STR;

    static const std::string GET_SCORE_FUNC_STR;

    static const std::unordered_map<PluginType, std::string> PLUGIN_TYPE_PATH_MAP;

    static const std::unordered_map<SubPluginType, std::string> SUB_PLUGIN_TYPE_PATH_MAP;

    void* pluginLibHandle_;

    std::string pluginLibPath_;

    std::shared_ptr<PluginBase> plugin_;

    std::unordered_map<std::string, std::string> extra_;

    std::shared_ptr<PluginBase> CreatePluginInternal(void* pluginLibHandle);

    std::shared_ptr<PluginBase> ChooseBestPlugin();

    void* LoadPluginLib();

    void InitPluginLibPath(PluginType pluginType, SubPluginType subPluginType);
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGINS_LOADER_H
