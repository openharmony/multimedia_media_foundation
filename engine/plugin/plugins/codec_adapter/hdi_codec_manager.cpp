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
#if defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "HdiCodecManager"

#include "hdi_codec_manager.h"
#include "foundation/utils/constants.h"
#include "foundation/log.h"
#include "hdf_base.h"
#include "hdi_codec_adapter.h"
#include "plugin/common/plugin_caps_builder.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace CodecAdapter {
using namespace CodecHDI;

HdiCodecManager& HdiCodecManager::GetInstance()
{
    static HdiCodecManager impl;
    return impl;
}

HdiCodecManager::HdiCodecManager()
{
    Init();
}

HdiCodecManager::~HdiCodecManager()
{
    Reset();
}

int32_t HdiCodecManager::CreateComponent(const Plugin::Any& component, uint32_t& id, const std::string& name,
                                         const Plugin::Any& appData, const Plugin::Any& callbacks)
{
    MEDIA_LOG_I("CreateComponent Start");
    if (!mgr_) {
        Init();
        FALSE_RETURN_V_MSG(mgr_ != nullptr, HDF_FAILURE, "mgr is nullptr");
    }
    sptr<CodecHDI::ICodecComponent> codecComponent = Plugin::AnyCast<sptr<CodecHDI::ICodecComponent>>(component);
    FALSE_RETURN_V_MSG(codecComponent != nullptr, HDF_FAILURE, "component is nullptr");
    return mgr_->CreateComponent(codecComponent, id, const_cast<char *>(name.c_str()),
        Plugin::AnyCast<int64_t>(appData), Plugin::AnyCast<sptr<CodecHDI::ICodecCallback>>(callbacks));
}

int32_t HdiCodecManager::DestroyComponent(const Plugin::Any& component, uint32_t id)
{
    MEDIA_LOG_I("DestroyComponent Start");
    FALSE_RETURN_V_MSG(mgr_ != nullptr, HDF_FAILURE, "mgr_ is nullptr");
    (void)component;
    return mgr_->DestroyComponent(id);
}

Status HdiCodecManager::RegisterCodecPlugins(const std::shared_ptr<OHOS::Media::Plugin::Register>& reg)
{
    MEDIA_LOG_I("RegisterCodecPlugins Start");
    std::string packageName = "HdiCodecAdapter";
    if (!mgr_) {
        MEDIA_LOG_E("Codec package " PUBLIC_LOG_S " has no valid component manager", packageName.c_str());
        return Status::ERROR_INVALID_DATA;
    }
    InitCaps();
    for (auto codecCapability : codecCapabilitys_) {
        if (codecCapability.pluginType != PluginType::VIDEO_DECODER &&
            codecCapability.pluginType != PluginType::VIDEO_ENCODER) {
            MEDIA_LOG_W("Plugin does not belong to video codec, pluginType: " PUBLIC_LOG_D32,
                        codecCapability.pluginType);
            continue;
        }
        CodecPluginDef def;
        def.rank = 100; // 100 default rank
        def.codecMode = CodecMode::HARDWARE;
        def.pluginType = codecCapability.pluginType;
        def.name = packageName + "." + codecCapability.name;
        def.inCaps = codecCapability.inCaps;
        def.outCaps = codecCapability.outCaps;
        auto pluginMime = codecCapability.pluginMime;
        def.creator = [pluginMime] (const std::string& name) -> std::shared_ptr<CodecPlugin> {
            return std::make_shared<HdiCodecAdapter>(name, pluginMime);
        };
        if (reg->AddPlugin(def) != Status::OK) {
            MEDIA_LOG_E("Add plugin " PUBLIC_LOG_S " failed", def.name.c_str());
        }
        MEDIA_LOG_DD("pluginType: " PUBLIC_LOG_D32 ", pluginName: " PUBLIC_LOG_S, def.pluginType, def.name.c_str());
    }
    return Status::OK;
}

Status HdiCodecManager::UnRegisterCodecPlugins()
{
    MEDIA_LOG_I("UnRegisterCodecPlugins Start");
    return Status::OK;
}

void HdiCodecManager::Init()
{
    MEDIA_LOG_I("Init Start");
    mgr_ = CodecHDI::ICodecComponentManager::Get(false);
}

void HdiCodecManager::Reset()
{
    MEDIA_LOG_I("Reset Start");
    mgr_ = nullptr;
}

void HdiCodecManager::AddHdiCap(const CodecHDI::CodecCompCapability& hdiCap)
{
    MEDIA_LOG_DD("AddHdiCap Start");
    auto pluginType = GetCodecType(hdiCap.type);
    if (pluginType == PluginType::VIDEO_DECODER || pluginType == PluginType::VIDEO_ENCODER) {
        CodecCapability codecCapability;
        CapabilityBuilder incapBuilder;
        CapabilityBuilder outcapBuilder;
        auto mime = GetCodecMime(hdiCap.role);
        if (mime == "video/unknown") {
            return;
        }
        if (pluginType == PluginType::VIDEO_DECODER) {
            incapBuilder.SetMime(mime);
            if (mime == MEDIA_MIME_VIDEO_H264 || mime == MEDIA_MIME_VIDEO_H265) {
                incapBuilder.SetVideoBitStreamFormatList({VideoBitStreamFormat::ANNEXB});
            }
            outcapBuilder.SetMime(MEDIA_MIME_VIDEO_RAW);
            outcapBuilder.SetVideoPixelFormatList(GetCodecFormats(hdiCap.port.video));
        } else {
            incapBuilder.SetMime(MEDIA_MIME_VIDEO_RAW);
            incapBuilder.SetVideoPixelFormatList(GetCodecFormats(hdiCap.port.video));
            outcapBuilder.SetMime(mime);
        }
        codecCapability.inCaps.push_back(incapBuilder.Build());
        codecCapability.outCaps.push_back(outcapBuilder.Build());
        codecCapability.pluginType = pluginType;
        codecCapability.pluginMime = mime;
        codecCapability.name = hdiCap.compName;
        codecCapabilitys_.push_back(codecCapability);
    }
}

void HdiCodecManager::InitCaps()
{
    MEDIA_LOG_I("InitCaps Start");
    int32_t compCnt = 0;
    int32_t ret = mgr_->GetComponentNum(compCnt);
    if (ret != HDF_SUCCESS || compCnt <= 0) {
        MEDIA_LOG_E("failed to query component number, ret=%d", ret);
        return;
    }
    std::vector<CodecCompCapability> capList(compCnt);
    ret = mgr_->GetComponentCapabilityList(capList, compCnt);
    FALSE_RETURN_MSG(ret == HDF_SUCCESS, "GetComponentCapabilityList fail");
    for (auto& cap : capList) {
        AddHdiCap(cap);
    }
}

std::vector<VideoPixelFormat> HdiCodecManager::GetCodecFormats(const CodecHDI::CodecVideoPortCap& port)
{
    std::vector<VideoPixelFormat> formats;
    for (int32_t fmt : port.supportPixFmts) {
        switch (fmt) {
            case GRAPHIC_PIXEL_FMT_YCBCR_420_SP:
                formats.push_back(VideoPixelFormat::NV12);
                break;
            case GRAPHIC_PIXEL_FMT_YCRCB_420_SP:
                formats.push_back(VideoPixelFormat::NV21);
                break;
            case GRAPHIC_PIXEL_FMT_YCBCR_420_P:
                formats.push_back(VideoPixelFormat::YUV420P);
                break;
            case GRAPHIC_PIXEL_FMT_RGBA_8888:
                formats.push_back(VideoPixelFormat::RGBA);
                break;
            case GRAPHIC_PIXEL_FMT_BGRA_8888:
                formats.push_back(VideoPixelFormat::BGRA);
                break;
            default:
                MEDIA_LOG_W("Unknown Format" PUBLIC_LOG_D32, fmt);
        }
    }
    return formats;
}

PluginType HdiCodecManager::GetCodecType(const CodecHDI::CodecType& hdiType)
{
    switch (hdiType) {
        case VIDEO_DECODER:
            return PluginType::VIDEO_DECODER;
        case VIDEO_ENCODER:
            return PluginType::VIDEO_ENCODER;
        case AUDIO_DECODER:
            return PluginType::AUDIO_DECODER;
        case AUDIO_ENCODER:
            return PluginType::AUDIO_ENCODER;
        default:
            return PluginType::INVALID_TYPE;
    }
}

std::string HdiCodecManager::GetCodecMime(const CodecHDI::AvCodecRole& role)
{
    switch (role) {
        case MEDIA_ROLETYPE_VIDEO_AVC:
            return MEDIA_MIME_VIDEO_H264;
        case MEDIA_ROLETYPE_VIDEO_HEVC:
            return MEDIA_MIME_VIDEO_H265;
        default:
            return "video/unknown";
    }
}
} // namespace CodecAdapter
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif