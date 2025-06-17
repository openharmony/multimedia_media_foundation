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
#ifndef HISTREAMER_PLUGIN_HDI_CODEC_MANAGER_H
#define HISTREAMER_PLUGIN_HDI_CODEC_MANAGER_H
#include "codec_manager.h"
#include <unordered_map>
#include "codec_omx_ext.h"
#include "v4_0/icodec_component_manager.h"
#include "v4_0/codec_types.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace CodecAdapter {
namespace CodecHDI = OHOS::HDI::Codec::V4_0;

class HdiCodecManager : public CodecManager {
public:
    static HdiCodecManager& GetInstance();
    ~HdiCodecManager() override;

    int32_t CreateComponent(const Plugin::Any& component, uint32_t& id, const std::string& name,
                                 const Plugin::Any& appData, const Plugin::Any& callbacks) override;
    int32_t DestroyComponent(const Plugin::Any& component, uint32_t id) override;

    Status RegisterCodecPlugins(const std::shared_ptr<OHOS::Media::Plugin::Register>& reg) override;
    Status UnRegisterCodecPlugins() override;
private:
    HdiCodecManager();
    void Init();
    void Reset();
    void AddHdiCap(const CodecHDI::CodecCompCapability& hdiCap);
    void InitCaps();
    static std::vector<VideoPixelFormat> GetCodecFormats(const CodecHDI::CodecVideoPortCap& port);
    static std::string GetCodecMime(const CodecHDI::AvCodecRole& role);
    static PluginType GetCodecType(const CodecHDI::CodecType& hdiType);

    sptr<CodecHDI::ICodecComponentManager> mgr_ {nullptr};
    std::vector<CodecCapability> codecCapabilitys_;
};
} // namespace CodecAdapter
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_HDI_CODEC_MANAGER_H
#endif