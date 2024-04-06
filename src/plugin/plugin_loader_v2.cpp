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
#include <dlfcn.h>
#include <dirent.h>

namespace OHOS {
namespace Media {
namespace Plugins {
const std::unordered_map<PluginType, std::string> PluginLoaderV2::PLUGIN_TYPE_PATH_MAP = {
    {PluginType::SOURCE, "source/"},
    {PluginType::DEMUXER, "demuxer/"},
    {PluginType::AUDIO_DECODER, "audio_decoder/"},
    {PluginType::AUDIO_ENCODER, "audio_encoder/"},
    {PluginType::AUDIO_SINK, "audio_sink/"},
    {PluginType::VIDEO_SINK, "video_sink/"},
    {PluginType::MUXER, "muxer/"},
};

const std::unordered_map<SubPluginType, std::string> PluginLoaderV2::SUB_PLUGIN_TYPE_PATH_MAP = {
    {SubPluginType::FILE_FD_SOURCE, "file_fd_source/"},
    {SubPluginType::FILE_SOURCE, "file_source/"},
    {SubPluginType::HTTP_SOURCE, "http_source/"},
    {SubPluginType::DATA_STREAM_SOURCE, "data_stream_source/"},
    {SubPluginType::FFMPEG_DEMUXER, "ffmpeg_demuxer/"},
    {SubPluginType::FFMPEG_AUDIO_DECODER, "ffmpeg_audio_decoder/"},
    {SubPluginType::G711MU_AUDIO_DECODER, "g711mu_audio_decoder/"},
    {SubPluginType::OPUS_AUDIO_DECODER, "opus_audio_decoder/"},
    {SubPluginType::AUDIO_VIVID_DECODER, "audio_vivid_decoder/"},
    {SubPluginType::DEFAULT_AUDIO_SINK, "default_audio_sink/"},
    {SubPluginType::FFMPEG_MUXER, "ffmpeg_muxer/"},
    {SubPluginType::FFMPEG_AUDIO_ENCODER, "ffmpeg_audio_encoder/"},
    {SubPluginType::G711MU_AUDIO_ENCODER, "g711mu_audio_encoder/"},
    {SubPluginType::OPUS_AUDIO_ENCODER, "opus_audio_encoder/"},
};

const std::string PluginLoaderV2::PLUGIN_64_ROOT_PATH = "/system/lib64/media/media_plugins/";

const std::string PluginLoaderV2::PLUGIN_32_ROOT_PATH = "/system/lib/media/media_plugins/";

const std::string PluginLoaderV2::REGISTER_PLUGIN_FUNC_STR = "RegisterPlugin";

const std::string PluginLoaderV2::GET_SCORE_FUNC_STR = "GetScore";

PluginLoaderV2::PluginLoaderV2(PluginType pluginType, SubPluginType subPluginType) : pluginLibHandle_(nullptr),
    plugin_(nullptr)
{
    InitPluginLibPath(pluginType, subPluginType);
}

void PluginLoaderV2::InitPluginLibPath(PluginType pluginType, SubPluginType subPluginType)
{
    std::string pluginPath;
    auto pluginPathEntry = PLUGIN_TYPE_PATH_MAP.find(pluginType);
    if (pluginPathEntry != PLUGIN_TYPE_PATH_MAP.end()) {
        pluginPath = pluginPathEntry->second;
    }

    std::string subPluginPath;
    auto subPluginPathEntry = SUB_PLUGIN_TYPE_PATH_MAP.find(subPluginType);
    if (subPluginPathEntry != SUB_PLUGIN_TYPE_PATH_MAP.end()) {
        subPluginPath = subPluginPathEntry->second;
    }

    pluginLibPath_ = pluginPath + subPluginPath;

    MEDIA_LOG_I("InitPluginLibPath path = %{public}s", pluginLibPath_.c_str());
}

std::shared_ptr<PluginBase> PluginLoaderV2::CreatePlugin(const std::unordered_map<std::string, std::string>& extra)
{
    extra_ = extra;

    if (pluginLibHandle_ == nullptr) {
        return ChooseBestPlugin();
    }

    return CreatePluginInternal(pluginLibHandle_);
}

std::shared_ptr<PluginBase> PluginLoaderV2::CreatePluginInternal(void* pluginLibHandle)
{
    RegisterPluginFunc registerPluginFunc = (RegisterPluginFunc)(::dlsym(pluginLibHandle,
        REGISTER_PLUGIN_FUNC_STR.c_str()));

    registerPluginFunc(shared_from_this());
    return plugin_;
}

void PluginLoaderV2::RegisterPlugin(std::shared_ptr<PluginBase> plugin)
{
    plugin_ = plugin;
}

std::shared_ptr<PluginBase> PluginLoaderV2::ChooseBestPlugin()
{
    std::string pluginLibAbsolutePath = PLUGIN_64_ROOT_PATH + pluginLibPath_;
    DIR* libDir = opendir(pluginLibAbsolutePath.c_str());
    if (libDir == nullptr) {
        MEDIA_LOG_W("looking for next dir, libDir is null on: %{public}s", pluginLibAbsolutePath.c_str());
        pluginLibAbsolutePath = PLUGIN_32_ROOT_PATH + pluginLibPath_;
        libDir = opendir(pluginLibAbsolutePath.c_str());
        if (libDir == nullptr) {
            MEDIA_LOG_E("ChooseBestPlugin failed, libDir is null on: %{public}s", pluginLibAbsolutePath.c_str());
            return nullptr;
        }
    }

    struct dirent* lib = nullptr;
    int maxScore = -1;
    while ((lib = readdir(libDir))) {
        if (lib->d_name[0] == '.') {
            continue;
        }
        std::string libName = pluginLibAbsolutePath + lib->d_name;
        MEDIA_LOG_I("ChooseBestPlugin plugin name %{public}s", libName.c_str());
        void* pluginHandle = ::dlopen(libName.c_str(), RTLD_NOW);
        auto plugin = CreatePluginInternal(pluginHandle);
        int score = plugin->GetScore();
        if (score > maxScore) {
            if (pluginLibHandle_ != nullptr) {
                ::dlclose(pluginLibHandle_);
            }
            maxScore = score;
            pluginLibHandle_ = pluginHandle;
            plugin_ = plugin;
        }
    }

    return plugin_;
}

std::unordered_map<std::string, std::string>& PluginLoaderV2::GetExtraInfo()
{
    return extra_;
}

}
}
}
