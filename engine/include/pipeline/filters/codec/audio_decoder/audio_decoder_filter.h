/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_FILTER_AUDIO_DECODER_H
#define HISTREAMER_PIPELINE_FILTER_AUDIO_DECODER_H

#include "pipeline/filters/codec/codec_filter_base.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class AudioDecoderFilter : public CodecFilterBase {
public:
    AudioDecoderFilter(const std::string& name, std::shared_ptr<CodecMode> codecMode);
    ~AudioDecoderFilter() override;

    ErrorCode Prepare() override;

    ErrorCode Stop() override;

    void FlushStart() override;

    void FlushEnd() override;

    bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta,
                   Plugin::Meta& upstreamParams, Plugin::Meta& downstreamParams) override;

    bool Negotiate(const std::string& inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;

    void OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input) override;

    void OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output) override;

private:
    uint32_t CalculateBufferSize(const std::shared_ptr<const Plugin::Meta>& meta) override;

    std::vector<Capability::Key> GetRequiredOutCapKeys() override;

    void UpdateParams(const std::shared_ptr<const Plugin::Meta>& upMeta,
                      std::shared_ptr<Plugin::Meta>& meta) override;
};
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_FILTER_AUDIO_DECODER_H
