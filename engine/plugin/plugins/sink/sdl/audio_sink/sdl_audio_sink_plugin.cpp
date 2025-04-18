/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "SdlAudioSinkPlugin"

#include "sdl_audio_sink_plugin.h"
#include <functional>
#include "foundation/cpp_ext/memory_ext.h"
#include "foundation/log.h"
#include "foundation/osal/utils/util.h"
#include "foundation/pre_defines.h"
#include "foundation/utils/constants.h"
#include "plugin/common/plugin_attr_desc.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"

namespace {
using namespace OHOS::Media::Plugin;
using namespace Sdl;
constexpr int MAX_AUDIO_FRAME_SIZE = 192000;
constexpr uint32_t DEFAULT_OUTPUT_CHANNELS = 2;
constexpr AudioChannelLayout DEFAULT_OUTPUT_CHANNEL_LAYOUT = AudioChannelLayout::STEREO;
std::function<void(void*, uint8_t*, int)> g_audioCallback;

void RegisterAudioCallback(std::function<void(void*, uint8_t*, int)> callback)
{
    g_audioCallback = std::move(callback);
}
void SDLAudioCallback(void* userdata, uint8_t* stream, int len) // NOLINT: void*
{
    g_audioCallback(userdata, stream, len);
}
AVSampleFormat TranslateFormat(AudioSampleFormat format)
{
    switch (format) {
        case AudioSampleFormat::U8:
            return AV_SAMPLE_FMT_U8;
        case AudioSampleFormat::U8P:
            return AV_SAMPLE_FMT_U8P;
        case AudioSampleFormat::F32:
            return AV_SAMPLE_FMT_FLT;
        case AudioSampleFormat::F32P:
            return AV_SAMPLE_FMT_FLTP;
        case AudioSampleFormat::F64:
            return AV_SAMPLE_FMT_DBL;
        case AudioSampleFormat::F64P:
            return AV_SAMPLE_FMT_DBLP;
        case AudioSampleFormat::S32:
            return AV_SAMPLE_FMT_S32;
        case AudioSampleFormat::S32P:
            return AV_SAMPLE_FMT_S32P;
        case AudioSampleFormat::S16:
            return AV_SAMPLE_FMT_S16;
        case AudioSampleFormat::S16P:
            return AV_SAMPLE_FMT_S16P;
        default:
            return AV_SAMPLE_FMT_NONE;
    }
}

bool IsPlanes(AudioSampleFormat format)
{
    switch (format) {
        case AudioSampleFormat::F32P:
        case AudioSampleFormat::F64P:
        case AudioSampleFormat::S32P:
        case AudioSampleFormat::S16P:
            return true;
        default:
            return false;
    }
}

std::shared_ptr<AudioSinkPlugin> AudioSinkPluginCreator(const std::string& name)
{
    return std::make_shared<SdlAudioSinkPlugin>(name);
}

const Status SdlAudioRegister(const std::shared_ptr<Register>& reg)
{
    AudioSinkPluginDef definition;
    definition.name = "sdl_audio_sink";
    definition.rank = 100; // 100
    Capability cap(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
    cap.AppendDiscreteKeys<AudioSampleFormat>(
        Capability::Key::AUDIO_SAMPLE_FORMAT,
        {AudioSampleFormat::U8, AudioSampleFormat::U8P, AudioSampleFormat::S8, AudioSampleFormat::S8P,
         AudioSampleFormat::U16, AudioSampleFormat::U16P, AudioSampleFormat::S16, AudioSampleFormat::S16P,
         AudioSampleFormat::U32, AudioSampleFormat::U32P, AudioSampleFormat::S32, AudioSampleFormat::S32P,
         AudioSampleFormat::F32, AudioSampleFormat::F32P, AudioSampleFormat::F64, AudioSampleFormat::F64P});
    definition.inCaps.emplace_back(cap);
    definition.creator = AudioSinkPluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(SdlAudioSink, LicenseType::LGPL, SdlAudioRegister, [] {});
} // namespace

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Sdl {
SdlAudioSinkPlugin::SdlAudioSinkPlugin(std::string name)
    : AudioSinkPlugin(std::move(name)),
      volume_(SDL_MIX_MAXVOLUME)
{
}
Status SdlAudioSinkPlugin::Init()
{
    std::weak_ptr<SdlAudioSinkPlugin> weakPtr(shared_from_this());
    RegisterAudioCallback([weakPtr](void* userdata, uint8_t* stream, int len) {
        auto ptr = weakPtr.lock();
        if (ptr) {
            ptr->AudioCallback(userdata, stream, len);
        }
    });
    return Status::OK;
}

Status SdlAudioSinkPlugin::Deinit()
{
    if (resample_) {
        resample_.reset();
    }
    return Status::OK;
}

Status SdlAudioSinkPlugin::Prepare()
{
    MEDIA_LOG_I("SDL SINK Prepare enter...");
    reSrcFfFmt_ = TranslateFormat(audioFormat_);
    srcFrameSize_ = av_samples_get_buffer_size(nullptr, channels_, samplesPerFrame_, reSrcFfFmt_, 1);
    rb = CppExt::make_unique<RingBuffer>(srcFrameSize_ * 10); // 最大缓存10帧
    rb->Init();
    if (reSrcFfFmt_ != reFfDestFmt_) {
        needResample_ = true;
    }
    wantedSpec_.freq = sampleRate_;
    wantedSpec_.format = AUDIO_S16SYS;
    wantedSpec_.channels = channels_;
    wantedSpec_.samples = samplesPerFrame_;
    wantedSpec_.silence = 0;
    wantedSpec_.callback = SDLAudioCallback;
    if (SDL_OpenAudio(&wantedSpec_, nullptr) < 0) {
        MEDIA_LOG_E("sdl cannot open audio with error: " PUBLIC_LOG_S, SDL_GetError());
        return Status::ERROR_UNKNOWN;
    }
    if (bitsPerSample_ == 8 || bitsPerSample_ == 24) { // 8 24
        needResample_ = true;
    }
    if (needResample_) {
        resample_ = std::make_shared<Ffmpeg::Resample>();
        Ffmpeg::ResamplePara resamplePara {
            channels_,
            sampleRate_,
            bitsPerSample_,
            static_cast<int64_t>(channelLayout_),
            reSrcFfFmt_,
            samplesPerFrame_,
            reFfDestFmt_,
        };
        FALSE_RETURN_V_MSG(resample_->Init(resamplePara) == Status::OK, Status::ERROR_UNKNOWN, "Resample init error");
    }
    return Status::OK;
}

Status SdlAudioSinkPlugin::Reset()
{
    if (resample_) {
        resample_.reset();
    }
    return Status::OK;
}

Status SdlAudioSinkPlugin::Start()
{
    MEDIA_LOG_I("SDL SINK start...");
    SDL_PauseAudio(0);
    rb->SetActive(true);
    return Status::OK;
}

Status SdlAudioSinkPlugin::Stop()
{
    MEDIA_LOG_I("SDL SINK Stop...");
    rb->SetActive(false);
    SDL_PauseAudio(1);
    Flush();
    SDL_CloseAudio();
    MEDIA_LOG_I("SDL SINK Stop end");
    return Status::OK;
}

Status SdlAudioSinkPlugin::GetParameter(Tag tag, ValueType& value)
{
    switch (tag) {
        case Tag::AUDIO_OUTPUT_CHANNELS: {
            value = DEFAULT_OUTPUT_CHANNELS;
            MEDIA_LOG_D("Get outputChannels: " PUBLIC_LOG_U32, DEFAULT_OUTPUT_CHANNELS);
            break;
        }
        case Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT: {
            value = DEFAULT_OUTPUT_CHANNEL_LAYOUT;
            MEDIA_LOG_D("Get outputChannelLayout: " PUBLIC_LOG_U64, DEFAULT_OUTPUT_CHANNEL_LAYOUT);
            break;
        }
        default: {
            MEDIA_LOG_W("receive one parameter with unconcern key: " PUBLIC_LOG_S, Tag2String(tag));
            break;
        }
    }
    return Status::OK;
}

Status SdlAudioSinkPlugin::SetParameter(Tag tag, const ValueType& value)
{
#define RETURN_ERROR_IF_CHECK_ERROR(typenames)                                                                         \
    if (!Plugin::Any::IsSameTypeWith<typenames>(value)) {                                                              \
        return Status::ERROR_MISMATCHED_TYPE;                                                                          \
    }
    MEDIA_LOG_I("SetParameter entered, key: " PUBLIC_LOG_S, Tag2String(tag));
    switch (tag) {
        case Tag::AUDIO_OUTPUT_CHANNELS: {
            RETURN_ERROR_IF_CHECK_ERROR(uint32_t);
            channels_ = Plugin::AnyCast<uint32_t>(value);
            if (channels_ > DEFAULT_OUTPUT_CHANNELS) {
                channels_ = DEFAULT_OUTPUT_CHANNELS;
            }
            MEDIA_LOG_D("Set outputChannels: " PUBLIC_LOG_U32, channels_);
            break;
        }
        case Tag::AUDIO_SAMPLE_RATE:
            RETURN_ERROR_IF_CHECK_ERROR(uint32_t);
            sampleRate_ = Plugin::AnyCast<uint32_t>(value);
            break;
        case Tag::AUDIO_SAMPLE_PER_FRAME:
            RETURN_ERROR_IF_CHECK_ERROR(uint32_t);
            samplesPerFrame_ = Plugin::AnyCast<uint32_t>(value);
            break;
        case Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT: {
            RETURN_ERROR_IF_CHECK_ERROR(AudioChannelLayout);
            auto chanLayout = Plugin::AnyCast<AudioChannelLayout>(value);
            if (chanLayout == AudioChannelLayout::MONO) {
                channelLayout_ = AV_CH_LAYOUT_MONO;
            } else if (chanLayout == AudioChannelLayout::STEREO) {
                channelLayout_ = AV_CH_LAYOUT_STEREO;
            }
            MEDIA_LOG_D("Set outputChannelLayout: " PUBLIC_LOG_U64, channelLayout_);
            break;
        }
        case Tag::AUDIO_SAMPLE_FORMAT:
            RETURN_ERROR_IF_CHECK_ERROR(AudioSampleFormat);
            audioFormat_ = Plugin::AnyCast<AudioSampleFormat>(value);
            break;
        case Tag::BITS_PER_CODED_SAMPLE:
            RETURN_ERROR_IF_CHECK_ERROR(uint32_t);
            bitsPerSample_ = Plugin::AnyCast<uint32_t>(value);
            break;
        default: {
            MEDIA_LOG_W("receive one parameter with unconcern key: " PUBLIC_LOG_S, Tag2String(tag));
            break;
        }
    }
    return Status::OK;
}

std::shared_ptr<Allocator> SdlAudioSinkPlugin::GetAllocator()
{
    return nullptr;
}

Status SdlAudioSinkPlugin::SetCallback(Callback* cb)
{
    UNUSED_VARIABLE(cb);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::GetMute(bool& mute)
{
    UNUSED_VARIABLE(mute);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::SetMute(bool mute)
{
    UNUSED_VARIABLE(mute);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::GetVolume(float& volume)
{
    UNUSED_VARIABLE(volume);
    volume = volume_ * 1.0 / SDL_MIX_MAXVOLUME;
    return Status::OK;
}

Status SdlAudioSinkPlugin::SetVolume(float volume)
{
    UNUSED_VARIABLE(volume);
    volume_ = static_cast<int>(volume * SDL_MIX_MAXVOLUME);
    return Status::OK;
}

Status SdlAudioSinkPlugin::GetSpeed(float& speed)
{
    UNUSED_VARIABLE(speed);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::SetSpeed(float speed)
{
    UNUSED_VARIABLE(speed);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::Pause()
{
    MEDIA_LOG_I("Paused");
    SDL_PauseAudio(1);
    return Status::OK;
}

Status SdlAudioSinkPlugin::Resume()
{
    MEDIA_LOG_I("Resume");
    rb->SetActive(true);
    SDL_PauseAudio(0);
    return Status::OK;
}

Status SdlAudioSinkPlugin::GetLatency(uint64_t& nanoSec)
{
    nanoSec = 100 * HST_MSECOND; // 100ms
    return Status::OK;
}

Status SdlAudioSinkPlugin::GetFrameSize(size_t& size)
{
    UNUSED_VARIABLE(size);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::GetFrameCount(uint32_t& count)
{
    UNUSED_VARIABLE(count);
    return Status::ERROR_UNIMPLEMENTED;
}

Status SdlAudioSinkPlugin::Write(const std::shared_ptr<Buffer>& buffer)
{
    MEDIA_LOG_DD("SdlSink Write begin");
    FALSE_RETURN_V_MSG_W(buffer != nullptr && !buffer->IsEmpty(), Status::OK, "Receive empty buffer.");
    auto mem = buffer->GetMemory();
    auto srcBuffer = mem->GetReadOnlyData();
    auto destBuffer = const_cast<uint8_t*>(srcBuffer);
    auto srcLength = mem->GetSize();
    auto destLength = srcLength;
    if (needResample_ && resample_) {
        FALSE_LOG(resample_->Convert(srcBuffer, srcLength, destBuffer, destLength) == Status::OK);
    }
    MEDIA_LOG_DD("SdlSink Write before ring buffer");
    rb->WriteBuffer(destBuffer, destLength);
    MEDIA_LOG_DD("SdlSink Write end");
    return Status::OK;
}

Status SdlAudioSinkPlugin::Flush()
{
    SDL_ClearQueuedAudio(1);
    rb->SetActive(false);
    return Status::OK;
}

Status SdlAudioSinkPlugin::Drain()
{
    MEDIA_LOG_I("Drain begin");
    while (rb->GetSize()) {
        OSAL::SleepFor(10);  // 10
    }
    MEDIA_LOG_I("Drain end");
    return Status::OK;
}

void SdlAudioSinkPlugin::AudioCallback(void* userdata, uint8_t* stream, int len) // NOLINT: void*
{
    UNUSED_VARIABLE(userdata);
    MEDIA_LOG_DD("sdl audio callback begin");
    if (mixCache_.capacity() < len) {
        mixCache_.reserve(len);
    }
    auto realLen = rb->ReadBuffer(mixCache_.data(), len);
    if (realLen == 0) {
        MEDIA_LOG_DD("sdl audio callback end with 0");
        return;
    }
    SDL_memset(stream, 0, len);
    SDL_MixAudio(stream, mixCache_.data(), realLen, volume_);
    SDL_PauseAudio(0);
    MEDIA_LOG_DD("sdl audio callback end with " PUBLIC_LOG_ZU, realLen);
}
} // namespace Sdl
} // namespace Plugin
} // namespace Media
} // namespace OHOS