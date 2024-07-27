/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "media_audio_encoder.h"
#include <filesystem>
#include <sstream>
#include <map>
#include <vector>
#include <sys/stat.h>
#include "log.h"
#include "securec.h"
#include "monitor_error.h"
#include "monitor_utils.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

enum SampleFormat : uint8_t {
    U8 = 0,
    S16LE = 1,
    S24LE = 2,
    S32LE = 3,
    F32LE = 4,
};

static std::map<SampleFormat, AVSampleFormat> AudioSampleMap = {
    {SampleFormat::U8, AV_SAMPLE_FMT_U8},
    {SampleFormat::S16LE, AV_SAMPLE_FMT_S16},
    {SampleFormat::S24LE, AV_SAMPLE_FMT_S16},
    {SampleFormat::S32LE, AV_SAMPLE_FMT_S32},
    {SampleFormat::F32LE, AV_SAMPLE_FMT_FLT}
};

const std::vector<std::string> SupportedSampleRates = {"16000", "32000", "44100", "48000", "96000"};
const std::vector<std::string> SupportedChannels = {"1", "2"};
const std::vector<std::string> SupportedSampleFormats = {"1", "3", "4"};
const size_t MAX_AUDIO_ARGS = 3;
const size_t ARGS_SAMPLERATE_POS = 3;
const size_t ARGS_CHANNEL_POS = 2;
const size_t ARGS_SAMPLEFORMAT_POS = 1;
const uint32_t DEFAULT_SAMPLERATE = 48000;
const mode_t MODE = 0775;
const std::string PCM_FILE = ".pcm";
const std::string FLAC_FILE = ".flac";
constexpr size_t MAX_BUFFER_LEN = 4608 * 2 * 10;
constexpr size_t DEFALUT_BUFFER_LEN = 4608 * 2 * 2;

int32_t SampleConvert::Init(const ResamplePara& param)
{
    resamplePara_ = param;
    auto swrContext = swr_alloc();
    if (swrContext == nullptr) {
        MEDIA_LOG_E("cannot allocate swr context");
        return ERROR;
    }
    swrContext = swr_alloc_set_opts(swrContext,
                                    resamplePara_.channelLayout, resamplePara_.destFmt, resamplePara_.sampleRate,
                                    resamplePara_.channelLayout, resamplePara_.srcFfFmt, resamplePara_.sampleRate,
                                    0, nullptr);
    if (swr_init(swrContext) != 0) {
        MEDIA_LOG_E("swr init error");
        return ERROR;
    }
    swrCtx_ = std::shared_ptr<SwrContext>(swrContext, [](SwrContext *ptr) {
        if (ptr) {
            swr_free(&ptr);
        }
    });
    isInit_ = true;
    return SUCCESS;
}

int32_t SampleConvert::Convert(const uint8_t* src, size_t size, std::shared_ptr<AVFrame>& dstFrame)
{
    size_t lineSize = size / resamplePara_.channels;
    std::vector<const uint8_t*> tmpInput(resamplePara_.channels);
    tmpInput[0] = src;
    if (av_sample_fmt_is_planar(resamplePara_.srcFfFmt)) {
        for (size_t i = 1; i < tmpInput.size(); ++i) {
            tmpInput[i] = tmpInput[i-1] + lineSize;
        }
    }
    auto res = swr_convert(swrCtx_.get(),
        dstFrame->extended_data, dstFrame->nb_samples,
        tmpInput.data(), dstFrame->nb_samples);
    if (res < 0) {
        MEDIA_LOG_E("resample input failed");
        return ERROR;
    }
    return SUCCESS;
}

int32_t MediaAudioEncoder::EncodePcmFiles(const std::string& fileDir)
{
    int32_t status = SUCCESS;
    MEDIA_LOG_I("start encode dir: %{public}s pcm file", fileDir.c_str());
    status = CheckFilePath(fileDir);
    FALSE_RETURN_V_MSG_E(status == SUCCESS, status, "check path failed");
    for (const auto& elem : std::filesystem::directory_iterator(fileDir)) {
        if (std::filesystem::is_regular_file(elem.status())) {
            if (elem.path().extension() != PCM_FILE) {
                continue;
            }
            std::string in = elem.path();
            status = EncodePcmToFlac(in);
            if (status == SUCCESS) {
                DeleteSrcFile(in);
            }
        }
    }
    return SUCCESS;
}

int32_t MediaAudioEncoder::EncodePcmToFlac(const std::string& in)
{
    int32_t status = SUCCESS;
    status = Init(in);
    FALSE_RETURN_V_MSG_E(status == SUCCESS, status, "start encode flac failed");

    FILE* pcmFile = fopen(in.c_str(), "rb");
    FALSE_RETURN_V_MSG_E(pcmFile != nullptr, ERROR, "open pcm file failed");

    size_t bufferLen = PcmDataSize();
    if (bufferLen <= 0 || bufferLen > MAX_BUFFER_LEN) {
        return ERROR;
    }
    uint8_t* buffer = reinterpret_cast<uint8_t *>(malloc(bufferLen));
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERROR, "alloc buffer failed");

    while (!feof(pcmFile)) {
        errno_t err = memset_s(static_cast<void *>(buffer), bufferLen, 0, bufferLen);
        if (err != EOK) {
            MEDIA_LOG_E("memset error");
            status = ERROR;
            break;
        }
        size_t bytesToWrite = fread(buffer, 1, bufferLen, pcmFile);
        if (bytesToWrite <= 0) {
            MEDIA_LOG_E("read pcm bytes error");
            status = ERROR;
            break;
        }
        status = WritePcm(buffer, bufferLen);
        if (status != SUCCESS) {
            MEDIA_LOG_E("read pcm bytes error");
            break;
        }
    }

    status = Release();
    free(buffer);
    (void)fclose(pcmFile);
    FALSE_RETURN_V_MSG_E(status == SUCCESS, status, "encode flac release error");
    return SUCCESS;
}

int32_t MediaAudioEncoder::Init(const std::string& inputFile)
{
    int32_t ret = SUCCESS;
    ret = CheckFilePath(inputFile);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "check path failed");
    url_ = inputFile.substr(0, inputFile.length() - PCM_FILE.length()) + FLAC_FILE;
    AudioEncodeConfig config = GetAudioConfig();
    ret = InitAudioEncode(config);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init encoder failed");
    ret = InitMux(url_);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init muxer failed");
    ret = InitFrame(avFrame_);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init frame failed");
    ret = InitPacket(avPacket_);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init packet failed");
    ret = InitSampleConvert();
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init convert failed");
    return ret;
}

bool MediaAudioEncoder::IsSupportAudioArgs(std::string& audioArg, const std::vector<std::string>& supportList)
{
    for (auto& arg : supportList) {
        if (audioArg == arg) {
            return true;
        }
    }
    return false;
}

int32_t MediaAudioEncoder::ParseAudioArgs(std::string& filename, AudioEncodeConfig& config)
{
    std::string sampleRate;
    std::string channel;
    std::string sampleFormat;
    SampleFormat formatType = SampleFormat::S16LE;

    std::vector<std::string> res;
    std::string nameStr = filename.substr(0, filename.rfind("."));
    std::istringstream iss(nameStr);
    std::string str;
    while (std::getline(iss, str, '_')) {
        res.push_back(str);
    }
    FALSE_RETURN_V_MSG_E(res.size() >= MAX_AUDIO_ARGS, ERROR,
        "parse args error, %{public}s", filename.c_str());

    // xxx_sampleRate_channel_sampleFormat.flac
    sampleRate = res[res.size() - ARGS_SAMPLERATE_POS];
    channel = res[res.size() - ARGS_CHANNEL_POS];
    sampleFormat = res[res.size() - ARGS_SAMPLEFORMAT_POS];
    if (IsSupportAudioArgs(sampleRate, SupportedSampleRates) &&
        IsSupportAudioArgs(channel, SupportedChannels) &&
        IsSupportAudioArgs(sampleFormat, SupportedSampleFormats)) {
        config.sampleRate = std::stoi(sampleRate);
        config.channelLayout = av_get_default_channel_layout(std::stoi(channel));
        formatType = (SampleFormat)std::stoi(sampleFormat);
        config.sampleFmt = AudioSampleMap[formatType];
        MEDIA_LOG_I("parser success %{public}s %{public}s %{public}s",
            sampleRate.c_str(), channel.c_str(), sampleFormat.c_str());
        return SUCCESS;
    }
    return ERROR;
}

AudioEncodeConfig MediaAudioEncoder::GetAudioConfig()
{
    std::string filename = std::filesystem::path(url_).filename().string();
    AudioEncodeConfig config;
    config.sampleRate = DEFAULT_SAMPLERATE;
    config.bitRate = 0;
    config.channelLayout = (uint64_t)AV_CH_LAYOUT_MONO;
    config.sampleFmt = AV_SAMPLE_FMT_S16;
    config.audioCodecId = AV_CODEC_ID_FLAC;
    ParseAudioArgs(filename, config);
    srcSampleFormat_ = config.sampleFmt;
    return config;
}

int32_t MediaAudioEncoder::InitMux(const std::string &url)
{
    int32_t ret = SUCCESS;
    AVFormatContext* formatCtx = nullptr;
    ret = avformat_alloc_output_context2(&formatCtx, nullptr, nullptr, url.c_str());
    if (formatCtx == nullptr) {
        MEDIA_LOG_E("could not deduce output format from file extension %{public}s", url.c_str());
        return ERROR;
    }
    formatContext_ = std::shared_ptr<AVFormatContext>(formatCtx, [](AVFormatContext *ptr) {
        if (ptr) {
            avformat_free_context(ptr);
        }
    });
    if (audioCodecContext_ != nullptr) {
        AVStream* audioStream = avformat_new_stream(formatContext_.get(), nullptr);
        AVCodecParameters* audioCodecPar = nullptr;
        audioCodecPar = avcodec_parameters_alloc();
        FALSE_RETURN_V_MSG_E(audioCodecPar != nullptr, ERROR, "could not alloc parameters");
        avcodec_parameters_from_context(audioCodecPar, audioCodecContext_.get());
        ret = avcodec_parameters_copy(audioStream->codecpar, audioCodecPar);
        avcodec_parameters_free(&audioCodecPar);
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not copy the stream parameters");
        audioStream->codecpar->codec_tag = 0;
    }
    if (videoCodecContext_ != nullptr) {
        AVStream* videoStream = avformat_new_stream(formatContext_.get(), nullptr);
        AVCodecParameters* videoCodecPar = nullptr;
        videoCodecPar = avcodec_parameters_alloc();
        FALSE_RETURN_V_MSG_E(videoCodecPar != nullptr, ERROR, "could not alloc parameters");
        avcodec_parameters_from_context(videoCodecPar, videoCodecContext_.get());
        ret = avcodec_parameters_copy(videoStream->codecpar, videoCodecPar);
        avcodec_parameters_free(&videoCodecPar);
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not copy the stream parameters");
        videoStream->codecpar->codec_tag = 0;
    }
    AVDictionary *opt = nullptr;
    if (!(formatContext_->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open2(&formatContext_->pb, url.c_str(), AVIO_FLAG_WRITE,
            &formatContext_->interrupt_callback, nullptr);
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "open fail %{public}s", url.c_str());
    }
    ret = avformat_write_header(formatContext_.get(), &opt);
    if (ret < 0) {
        MEDIA_LOG_E("error occurred when write header: %{public}d", ret);
        Release();
        return ERROR;
    }
    isInit_ = true;
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitAudioEncode(const AudioEncodeConfig& audioConfig)
{
    int32_t ret = SUCCESS;
    const AVCodec* codec = nullptr;
    codec = avcodec_find_encoder(audioConfig.audioCodecId);
    FALSE_RETURN_V_MSG_E(codec != nullptr, ERROR, "find audio codec failed");
    AVCodecContext* context = nullptr;
    context = avcodec_alloc_context3(codec);
    FALSE_RETURN_V_MSG_E(context != nullptr, ERROR, "alloc audio encode context failed");

    audioCodecContext_ = std::shared_ptr<AVCodecContext>(context, [](AVCodecContext* ptr) {
        if (ptr) {
            avcodec_free_context(&ptr);
        }
    });

    audioCodecContext_->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_S16;
    // sink to hal s32le
    if (srcSampleFormat_ == AV_SAMPLE_FMT_S32 && audioConfig.audioCodecId == AV_CODEC_ID_FLAC) {
        audioCodecContext_->sample_fmt = srcSampleFormat_;
    }
    audioCodecContext_->bit_rate = audioConfig.bitRate;
    audioCodecContext_->sample_rate = audioConfig.sampleRate;
    if (codec->supported_samplerates) {
        audioCodecContext_->sample_rate = codec->supported_samplerates[0];
        for (int i = 0; codec->supported_samplerates[i]; i++) {
            if (codec->supported_samplerates[i] == audioConfig.sampleRate) {
                audioCodecContext_->sample_rate = audioConfig.sampleRate;
                MEDIA_LOG_I("find supported sample rate matching config");
            }
        }
    }
    audioCodecContext_->channel_layout = audioConfig.channelLayout;
    if (codec->channel_layouts) {
        audioCodecContext_->channel_layout = codec->channel_layouts[0];
        for (int i = 0; codec->channel_layouts[i]; i++) {
            if (codec->channel_layouts[i] == audioConfig.channelLayout) {
                audioCodecContext_->channel_layout = audioConfig.channelLayout;
                MEDIA_LOG_I("find supported channel layout matching config");
            }
        }
    }
    audioCodecContext_->channels = av_get_channel_layout_nb_channels(audioCodecContext_->channel_layout);
    audioCodecContext_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    ret = avcodec_open2(audioCodecContext_.get(), codec, nullptr);
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not open audio codec %{public}d", ret);
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitFrame(std::shared_ptr<AVFrame>& frame)
{
    AVFrame* ffFrame = nullptr;
    ffFrame = av_frame_alloc();
    FALSE_RETURN_V_MSG_E(ffFrame != nullptr, ERROR, "init frame failed");
    frame = std::shared_ptr<AVFrame>(ffFrame, [](AVFrame *ptr) {
        if (ptr) {
            av_frame_free(&ptr);
        }
    });
    frame->format = audioCodecContext_->sample_fmt;
    frame->nb_samples = audioCodecContext_->frame_size;
    frame->channel_layout = audioCodecContext_->channel_layout;
    frame->sample_rate = audioCodecContext_->sample_rate;
    frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
    int32_t ret = av_frame_get_buffer(frame.get(), 0);
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "get frame buffer failed %{public}d", ret);
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitPacket(std::shared_ptr<AVPacket>& packet)
{
    AVPacket* ffPacket = nullptr;
    ffPacket = av_packet_alloc();
    FALSE_RETURN_V_MSG_E(ffPacket != nullptr, ERROR, "init packet failed");
    packet = std::shared_ptr<AVPacket>(ffPacket, [](AVPacket *ptr) {
        if (ptr) {
            av_packet_free(&ptr);
        }
    });
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitSampleConvert()
{
    FALSE_RETURN_V_MSG_E(avFrame_ != nullptr, ERROR, "frame error.");
    if (srcSampleFormat_ == AV_SAMPLE_FMT_S16 || srcSampleFormat_ == AV_SAMPLE_FMT_S32) {
        MEDIA_LOG_I("in sample format no need convert %{public}d", srcSampleFormat_);
        sampleConvert_ = nullptr;
        return SUCCESS;
    }

    sampleConvert_ = std::make_shared<SampleConvert>();
    ResamplePara param {
            avFrame_->channels,
            avFrame_->sample_rate,
            avFrame_->channel_layout,
            srcSampleFormat_,
            (AVSampleFormat)avFrame_->format,
    };
    int32_t ret = sampleConvert_->Init(param);
    return ret;
}

int32_t MediaAudioEncoder::WritePcm(uint8_t* buffer, size_t size)
{
    if (isInit_ == false) {
        MEDIA_LOG_E("encoder init state error");
        return ERROR;
    }
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERROR, "buffer nullptr");
    if (avFrame_ == nullptr) {
        return ERROR;
    }
    if (sampleConvert_) {
        sampleConvert_->Convert(buffer, size, avFrame_);
    } else {
        errno_t err = memcpy_s(avFrame_->data[0], avFrame_->linesize[0], buffer, size);
        FALSE_RETURN_V_MSG_E(err == EOK, ERROR, "memcpy error");
    }
    return WriteFrame(avFrame_);
}

int32_t MediaAudioEncoder::WriteFrame(std::shared_ptr<AVFrame>& frame)
{
    int32_t ret = SUCCESS;
    FALSE_RETURN_V_MSG_E(frame != nullptr, ERROR, "frame nullptr");
    FALSE_RETURN_V_MSG_E(avPacket_ != nullptr, ERROR, "packet nullptr");
    ret = avcodec_send_frame(audioCodecContext_.get(), frame.get());
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "send frame failed %{public}d", ret);
    while (true) {
        ret = avcodec_receive_packet(audioCodecContext_.get(), avPacket_.get());
        if (ret == 0) {
            ret = av_interleaved_write_frame(formatContext_.get(), avPacket_.get());
            av_packet_unref(avPacket_.get());
            FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "write packet error");
            continue;
        } else if ((ret == AVERROR(EAGAIN)) || (ret == AVERROR_EOF)) {
            ret = SUCCESS;
        } else {
            MEDIA_LOG_E("receive packet error");
        }
        break;
    }
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "write packet error");
    return SUCCESS;
}

void MediaAudioEncoder::ResetEncoderCtx()
{
    formatContext_ = nullptr;
    audioCodecContext_ = nullptr;
    videoCodecContext_ = nullptr;
    avPacket_ = nullptr;
    avFrame_ = nullptr;
    sampleConvert_ = nullptr;
}

int32_t MediaAudioEncoder::Release()
{
    if (formatContext_ == nullptr) {
        ResetEncoderCtx();
        isInit_ = false;
        return SUCCESS;
    }
    if (isInit_) {
        av_write_trailer(formatContext_.get());
    }
    if (!(formatContext_->oformat->flags & AVFMT_NOFILE)) {
        avio_close(formatContext_->pb);
    }
    avformat_flush(formatContext_.get());
    ResetEncoderCtx();
    isInit_ = false;
    return SUCCESS;
}

size_t MediaAudioEncoder::PcmDataSize()
{
    size_t size = DEFALUT_BUFFER_LEN;
    if (audioCodecContext_ != nullptr) {
        size = audioCodecContext_->frame_size
        * av_get_channel_layout_nb_channels(audioCodecContext_->channel_layout)
        * av_get_bytes_per_sample(srcSampleFormat_);
    }
    return size;
}

bool MediaAudioEncoder::DeleteSrcFile(const std::string& filePath)
{
    if (!IsRealPath(filePath)) {
        return false;
    }
    (void)chmod(filePath.c_str(), MODE);
    if (remove(filePath.c_str()) != 0) {
        MEDIA_LOG_E("remove file %{public}s failed ", filePath.c_str());
        return false;
    }
    return true;
}

int32_t MediaAudioEncoder::CheckFilePath(const std::string& inputFile)
{
    if (!IsRealPath(inputFile)) {
        MEDIA_LOG_E("invalid url");
        return ERROR;
    }
    return SUCCESS;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS