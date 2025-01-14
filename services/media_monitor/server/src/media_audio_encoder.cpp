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
#include <sys/stat.h>
#include "log.h"
#include "securec.h"
#include "monitor_error.h"
#include "monitor_utils.h"
#include "string_converter.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

const int MAX_AUDIO_ARGS = 3;
const int ARGS_SAMPLERATE_POS = 3;
const int ARGS_CHANNEL_POS = 2;
const int ARGS_SAMPLEFORMAT_POS = 1;
const uint32_t DEFAULT_SAMPLERATE = 48000;
const uint32_t DEFAULT_CHANNELS = 1;

const int S24LE_SAMPLESIZE = 3;
const int S24LE_BYTE_SHIFT_8 = 8;
const int S24LE_BYTE_SHIFT_16 = 16;
const int S24LE_BYTE_SHIFT_24 = 24;
const int S24LE_BYTE_INDEX_0 = 0;
const int S24LE_BYTE_INDEX_1 = 1;
const int S24LE_BYTE_INDEX_2 = 2;

const mode_t MODE = 0775;
const std::string PCM_FILE = ".pcm";
const std::string FLAC_FILE = ".flac";
const std::string BLUETOOTCH_FILE = "bluetooth";
const uint8_t BLUETOOTH_SAMPLE_FORMAT_OFFSET = 1;
// samples(flac: 4608) * formatSize(f32: 4) * channles(6)
constexpr size_t MAX_BUFFER_LEN = 4608 * 4 * 6;
// samples(flac: 4608) * formatSize(s16: 2) * channles(2)
constexpr size_t DEFALUT_BUFFER_LEN = 4608 * 2 * 2;

static std::map<SampleFormat, AVSampleFormat> AudioSampleMap = {
    {SampleFormat::U8, AV_SAMPLE_FMT_U8},
    {SampleFormat::S16LE, AV_SAMPLE_FMT_S16},
    {SampleFormat::S24LE, AV_SAMPLE_FMT_S32},
    {SampleFormat::S32LE, AV_SAMPLE_FMT_S32},
    {SampleFormat::F32LE, AV_SAMPLE_FMT_FLT}
};
// encoder support sample rate
const std::vector<std::string> SupportedSampleRates = {
    "8000", "11025", "12000", "16000", "22050", "24000", "32000",
    "44100", "48000", "64000", "88200", "96000", "192000"
};
// encoder support audio channels
const std::vector<std::string> SupportedChannels = {"1", "2", "3", "4", "5", "6"};
// encoder support sample format 1 S16le, 2 S24le, 3 S32le, 4 F32le
const std::vector<std::string> SupportedSampleFormats = {"1", "2", "3", "4"};

SampleConvert::SampleConvert(std::shared_ptr<FFmpegApiWrap> apiWrap)
    : apiWrap_(apiWrap)
{
}

SampleConvert::~SampleConvert()
{
    Release();
}

int32_t SampleConvert::Init(const ResamplePara &param)
{
    resamplePara_ = param;
    if (apiWrap_ == nullptr) {
        MEDIA_LOG_E("api load failed");
        return ERROR;
    }
    auto swrContext = apiWrap_->SwrAlloc();
    if (swrContext == nullptr) {
        MEDIA_LOG_E("cannot allocate swr context");
        return ERROR;
    }
    swrContext = apiWrap_->SwrSetOpts(swrContext,
        resamplePara_.channelLayout, resamplePara_.destFmt, resamplePara_.sampleRate,
        resamplePara_.channelLayout, resamplePara_.srcFfFmt, resamplePara_.sampleRate,
        0, nullptr);
    if (apiWrap_->SwrInit(swrContext) != 0) {
        MEDIA_LOG_E("swr init error");
        return ERROR;
    }
    swrCtx_ = std::shared_ptr<SwrContext>(swrContext, [this](SwrContext* ptr) {
        apiWrap_->SwrFree(&ptr);
    });
    isInit_ = true;
    return SUCCESS;
}

int32_t SampleConvert::Convert(const uint8_t *src, size_t size, AVFrame *dstFrame)
{
    FALSE_RETURN_V_MSG_E(isInit_ == true, ERROR, "init error convert failed");
    FALSE_RETURN_V_MSG_E(dstFrame != nullptr, ERROR, "dst frame is nullptr");
    size_t lineSize = size / resamplePara_.channels;
    std::vector<const uint8_t*> tmpInput(resamplePara_.channels);
    tmpInput[0] = src;
    if (apiWrap_->SampleFmtIsPlannar(resamplePara_.srcFfFmt)) {
        for (size_t i = 1; i < tmpInput.size(); ++i) {
            tmpInput[i] = tmpInput[i-1] + lineSize;
        }
    }
    auto res = apiWrap_->SwrConvert(swrCtx_.get(),
        dstFrame->extended_data, dstFrame->nb_samples,
        tmpInput.data(), dstFrame->nb_samples);
    if (res < 0) {
        MEDIA_LOG_E("resample input failed");
        return ERROR;
    }
    return SUCCESS;
}

void SampleConvert::Release()
{
    swrCtx_ = nullptr;
    isInit_ = false;
}

int32_t MediaAudioEncoder::EncodePcmFiles(const std::string &fileDir)
{
    int32_t status = SUCCESS;
    apiWrap_ = std::make_shared<FFmpegApiWrap>();
    if (!apiWrap_->Open()) {
        apiWrap_->Close();
        MEDIA_LOG_E("load encoder api failed");
        apiWrap_ = nullptr;
        return ERROR;
    }

    std::error_code errorCode;
    std::filesystem::directory_iterator iter(fileDir, errorCode);
    if (errorCode) {
        MEDIA_LOG_E("get file failed");
        return ERROR;
    }
    for (const auto &elem : iter) {
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

    apiWrap_->Close();
    apiWrap_ = nullptr;
    return SUCCESS;
}

int32_t MediaAudioEncoder::EncodePcmToFlac(const std::string &in)
{
    int32_t status = SUCCESS;
    FALSE_RETURN_V_MSG_E(IsRealPath(in), ERROR, "check path failed");
    status = Init(in);
    if (status != SUCCESS) {
        Release();
        return ERROR;
    }
    size_t bufferLen = PcmDataSize();
    if (bufferLen <= 0 || bufferLen > MAX_BUFFER_LEN) {
        Release();
        return ERROR;
    }
    uint8_t *buffer = reinterpret_cast<uint8_t *>(malloc(bufferLen));
    if (buffer == nullptr) {
        Release();
        return ERROR;
    }
    FILE *pcmFile = fopen(in.c_str(), "rb");
    if (pcmFile == nullptr) {
        free(buffer);
        Release();
        return ERROR;
    }
    while (!feof(pcmFile)) {
        errno_t err = memset_s(static_cast<void *>(buffer), bufferLen, 0, bufferLen);
        if (err != EOK) {
            status = ERROR;
            break;
        }
        size_t bytesToWrite = fread(buffer, 1, bufferLen, pcmFile);
        if (bytesToWrite <= 0) {
            status = ERROR;
            break;
        }
        status = WritePcm(buffer, bufferLen);
        if (status != SUCCESS) {
            break;
        }
    }
    (void)fclose(pcmFile);
    free(buffer);
    Release();
    return status;
}

int32_t MediaAudioEncoder::Init(const std::string &inputFile)
{
    FALSE_RETURN_V_MSG_E(apiWrap_ != nullptr, ERROR, "load wrap api failed");
    int32_t ret = SUCCESS;
    AudioEncodeConfig config;
    ret = GetAudioConfig(inputFile, config);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "get audio config failed");
    ret = InitAudioEncode(config);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init encoder failed");
    ret = InitMux();
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init muxer failed");
    ret = InitFrame();
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init frame failed");
    ret = InitPacket();
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init packet failed");
    ret = InitSampleConvert();
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ret, "init convert failed");
    isInit_ = true;
    return ret;
}

bool MediaAudioEncoder::IsSupportAudioArgs(std::string &audioArg, const std::vector<std::string> &supportList)
{
    for (auto &arg : supportList) {
        if (audioArg == arg) {
            return true;
        }
    }
    return false;
}

int32_t MediaAudioEncoder::ParseAudioArgs(const std::string &fileName, AudioEncodeConfig &config)
{
    std::string sampleRate;
    std::string channel;
    std::string sampleFormat;

    std::vector<std::string> res;
    std::string nameStr = fileName.substr(0, fileName.rfind("."));
    std::istringstream iss(nameStr);
    std::string str;
    while (std::getline(iss, str, '_')) {
        res.push_back(str);
    }
    FALSE_RETURN_V_MSG_E(res.size() >= MAX_AUDIO_ARGS, ERROR,
        "parse args error, %{public}s", fileName.c_str());
    // xxx_sampleRate_channel_sampleFormat.flac
    sampleRate = res[res.size() - ARGS_SAMPLERATE_POS];
    channel = res[res.size() - ARGS_CHANNEL_POS];
    sampleFormat = res[res.size() - ARGS_SAMPLEFORMAT_POS];
    if (IsSupportAudioArgs(sampleRate, SupportedSampleRates) &&
        IsSupportAudioArgs(channel, SupportedChannels) &&
        IsSupportAudioArgs(sampleFormat, SupportedSampleFormats)) {
        FALSE_RETURN_V_MSG_E(StringConverter(sampleRate, config.sampleRate), ERROR,
            "parse sampleRate error, %{public}s", sampleRate.c_str());
        FALSE_RETURN_V_MSG_E(StringConverter(channel, config.channels), ERROR,
            "parse channel error, %{public}s", channel.c_str());
        uint8_t sampleFormatValue = 0;
        FALSE_RETURN_V_MSG_E(StringConverter(sampleFormat, sampleFormatValue), ERROR,
            "parse sampleFormat error, %{public}s", sampleFormat.c_str());
        config.sampleFmt = static_cast<SampleFormat>(sampleFormatValue);
        MEDIA_LOG_I("parser success %{public}s %{public}s %{public}s",
            sampleRate.c_str(), channel.c_str(), sampleFormat.c_str());
    } else {
        MEDIA_LOG_I("parser error filename: %{public}s", fileName.c_str());
        return ERROR;
    }
    if (fileName.find(BLUETOOTCH_FILE) != std::string::npos) {
        uint8_t sampleFormatValue = 0;
        FALSE_RETURN_V_MSG_E(StringConverter(sampleFormat, sampleFormatValue), ERROR,
            "parse sampleFormat error, %{public}s", sampleFormat.c_str());
        config.sampleFmt = static_cast<SampleFormat>(sampleFormatValue - BLUETOOTH_SAMPLE_FORMAT_OFFSET);
    }
    return SUCCESS;
}

int32_t MediaAudioEncoder::GetAudioConfig(const std::string &inFullName, AudioEncodeConfig &config)
{
    fileName_ = inFullName.substr(0, inFullName.length() - PCM_FILE.length()) + FLAC_FILE;
    std::string filename = std::filesystem::path(fileName_).filename().string();
    config.bitRate = 0;
    config.sampleRate = DEFAULT_SAMPLERATE;
    config.channels = DEFAULT_CHANNELS;
    config.sampleFmt = S16LE;
    config.audioCodecId = AV_CODEC_ID_FLAC;
    int32_t ret = ParseAudioArgs(filename, config);
    FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "parse args error");
    srcSampleFormat_ = config.sampleFmt;
    return ret;
}

int32_t MediaAudioEncoder::InitMux()
{
    int32_t ret = SUCCESS;
    AVFormatContext *formatCtx = nullptr;
    ret = apiWrap_->FormatAllocOutputContext(&formatCtx, nullptr, nullptr, fileName_.c_str());
    if (formatCtx == nullptr) {
        MEDIA_LOG_E("could not deduce output format from file extension %{public}s", fileName_.c_str());
        return ERROR;
    }
    formatContext_ = std::shared_ptr<AVFormatContext>(formatCtx, [this](AVFormatContext* ptr) {
        apiWrap_->FormatFreeContext(ptr);
    });
    if (audioCodecContext_ != nullptr) {
        AVStream *audioStream = nullptr;
        audioStream = apiWrap_->FormatNewStream(formatContext_.get(), nullptr);
        FALSE_RETURN_V_MSG_E(audioStream != nullptr, ERROR, "new audio stream error");
        ret = apiWrap_->CodecParamFromContext(audioStream->codecpar, audioCodecContext_.get());
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not copy the stream parameters");
        audioStream->codecpar->codec_tag = 0;
    }
    if (videoCodecContext_ != nullptr) {
        AVStream *videoStream = nullptr;
        videoStream = apiWrap_->FormatNewStream(formatContext_.get(), nullptr);
        FALSE_RETURN_V_MSG_E(videoStream != nullptr, ERROR, "new video stream error");
        ret = apiWrap_->CodecParamFromContext(videoStream->codecpar, videoCodecContext_.get());
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not copy the stream parameters");
        videoStream->codecpar->codec_tag = 0;
    }
    unsigned int formatCtxFlag = static_cast<unsigned int>(formatContext_->oformat->flags);
    if (!(formatCtxFlag & AVFMT_NOFILE)) {
        ret = apiWrap_->IoOpen(&formatContext_->pb, fileName_.c_str(), AVIO_FLAG_WRITE,
            &formatContext_->interrupt_callback, nullptr);
        FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "open fail %{public}s", fileName_.c_str());
    }
    ret = apiWrap_->FormatWriteHeader(formatContext_.get(), nullptr);
    if (ret < 0) {
        MEDIA_LOG_E("error occurred when write header: %{public}d", ret);
        return ERROR;
    }
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitAudioEncode(const AudioEncodeConfig &audioConfig)
{
    int32_t ret = SUCCESS;
    const AVCodec *codec = nullptr;
    codec = apiWrap_->CodecFindEncoder(audioConfig.audioCodecId);
    FALSE_RETURN_V_MSG_E(codec != nullptr, ERROR, "find audio codec failed");
    AVCodecContext *context = nullptr;
    context = apiWrap_->CodecAllocContext(codec);
    FALSE_RETURN_V_MSG_E(context != nullptr, ERROR, "alloc audio encode context failed");

    audioCodecContext_ = std::shared_ptr<AVCodecContext>(context, [this](AVCodecContext* ptr) {
        apiWrap_->CodecFreeContext(&ptr);
    });

    audioCodecContext_->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_S16;
    if (srcSampleFormat_ > SampleFormat::S16LE && audioConfig.audioCodecId == AV_CODEC_ID_FLAC) {
        audioCodecContext_->sample_fmt = AV_SAMPLE_FMT_S32;
    }
    audioCodecContext_->bit_rate = audioConfig.bitRate;
    audioCodecContext_->sample_rate = audioConfig.sampleRate;
    audioCodecContext_->channels = audioConfig.channels;
    audioCodecContext_->channel_layout = static_cast<uint64_t>(apiWrap_->GetChannelLayout(audioConfig.channels));

    unsigned int codecCtxFlag = static_cast<unsigned int>(audioCodecContext_->flags);
    codecCtxFlag |= AV_CODEC_FLAG_GLOBAL_HEADER;
    audioCodecContext_->flags = static_cast<int>(codecCtxFlag);
    ret = apiWrap_->CodecOpen(audioCodecContext_.get(), codec, nullptr);
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "could not open audio codec %{public}d", ret);
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitFrame()
{
    AVFrame* frame = apiWrap_->FrameAlloc();
    FALSE_RETURN_V_MSG_E(frame != nullptr, ERROR, "alloc frame failed");
    avFrame_ = std::shared_ptr<AVFrame>(frame, [this](AVFrame* frame) {
        apiWrap_->FrameFree(&frame);
    });

    avFrame_->format = audioCodecContext_->sample_fmt;
    avFrame_->nb_samples = audioCodecContext_->frame_size;
    avFrame_->channel_layout = audioCodecContext_->channel_layout;
    avFrame_->sample_rate = audioCodecContext_->sample_rate;
    avFrame_->channels = audioCodecContext_->channels;
    int32_t ret = apiWrap_->FrameGetBuffer(avFrame_.get(), 0);
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "get frame buffer failed %{public}d", ret);
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitPacket()
{
    AVPacket *packet = apiWrap_->PacketAlloc();
    FALSE_RETURN_V_MSG_E(packet != nullptr, ERROR, "alloc packet failed");
    avPacket_ = std::shared_ptr<AVPacket>(packet, [this](AVPacket *packet) {
        apiWrap_->PacketFree(&packet);
    });
    return SUCCESS;
}

int32_t MediaAudioEncoder::InitSampleConvert()
{
    FALSE_RETURN_V_MSG_E(avFrame_ != nullptr, ERROR, "frame error.");
    if (srcSampleFormat_ > SampleFormat::U8 && srcSampleFormat_ < SampleFormat::F32LE) {
        MEDIA_LOG_I("in sample format no need convert %{public}d", srcSampleFormat_);
        sampleConvert_ = nullptr;
        return SUCCESS;
    }

    sampleConvert_ = std::make_shared<SampleConvert>(apiWrap_);
    ResamplePara param {
            avFrame_->channels,
            avFrame_->sample_rate,
            avFrame_->channel_layout,
            AudioSampleMap[srcSampleFormat_],
            (AVSampleFormat)avFrame_->format,
    };
    int32_t ret = sampleConvert_->Init(param);
    return ret;
}

void MediaAudioEncoder::CopyS24ToS32(int32_t *dst, const uint8_t *src, size_t count)
{
    if (dst == nullptr || src == nullptr) {
        return;
    }

    dst += count;
    src += count * S24LE_SAMPLESIZE;
    for (; count > 0; --count) {
        src -= S24LE_SAMPLESIZE;
        *--dst = (int32_t)((src[S24LE_BYTE_INDEX_0] << S24LE_BYTE_SHIFT_8) |
            (src[S24LE_BYTE_INDEX_1] << S24LE_BYTE_SHIFT_16) |
            (src[S24LE_BYTE_INDEX_2] << S24LE_BYTE_SHIFT_24));
    }
}

int32_t MediaAudioEncoder::FillFrameFromBuffer(const uint8_t *buffer, size_t size)
{
    FALSE_RETURN_V_MSG_E(avFrame_->linesize[0] >= static_cast<int>(size), ERROR, "frame size error");
    if (srcSampleFormat_ == SampleFormat::S24LE) {
        size_t count = size / S24LE_SAMPLESIZE;
        CopyS24ToS32(reinterpret_cast<int32_t*>(avFrame_->data[0]), buffer, count);
    } else {
        errno_t err = memcpy_s(avFrame_->data[0], avFrame_->linesize[0], buffer, size);
        FALSE_RETURN_V_MSG_E(err == EOK, ERROR, "memcpy error");
    }
    return SUCCESS;
}

int32_t MediaAudioEncoder::WritePcm(const uint8_t *buffer, size_t size)
{
    int32_t ret = SUCCESS;
    FALSE_RETURN_V_MSG_E(isInit_ == true, ERROR, "init error");
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERROR, "buffer nullptr");
    FALSE_RETURN_V_MSG_E(avFrame_ != nullptr, ERROR, "frame nullptr");
    if (sampleConvert_ != nullptr) {
        ret = sampleConvert_->Convert(buffer, size, avFrame_.get());
        FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "resample frame error");
    } else {
        ret = FillFrameFromBuffer(buffer, size);
        FALSE_RETURN_V_MSG_E(ret == SUCCESS, ERROR, "fill frame error");
    }
    ret = WriteFrame();
    return ret;
}

int32_t MediaAudioEncoder::WriteFrame()
{
    int32_t ret = SUCCESS;
    FALSE_RETURN_V_MSG_E(avFrame_ != nullptr, ERROR, "frame nullptr");
    FALSE_RETURN_V_MSG_E(avPacket_ != nullptr, ERROR, "packet nullptr");
    ret = apiWrap_->CodecSendFrame(audioCodecContext_.get(), avFrame_.get());
    FALSE_RETURN_V_MSG_E(ret >= 0, ERROR, "send frame failed %{public}d", ret);
    while (true) {
        ret = apiWrap_->CodecRecvPacket(audioCodecContext_.get(), avPacket_.get());
        if (ret == 0) {
            ret = apiWrap_->FormatWriteFrame(formatContext_.get(), avPacket_.get());
            apiWrap_->PacketUnref(avPacket_.get());
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
    avPacket_ = nullptr;
    avFrame_ = nullptr;
    audioCodecContext_ = nullptr;
    videoCodecContext_ = nullptr;
    formatContext_ = nullptr;
    sampleConvert_ = nullptr;
}

void MediaAudioEncoder::Release()
{
    FALSE_RETURN_MSG(apiWrap_ != nullptr, "load api error");
    if (formatContext_ == nullptr) {
        ResetEncoderCtx();
        isInit_ = false;
        return;
    }
    if (isInit_) {
        apiWrap_->FormatWriteTrailer(formatContext_.get());
        apiWrap_->IoFlush(formatContext_->pb);
        unsigned int formatCtxFlag = static_cast<unsigned int>(formatContext_->oformat->flags);
        if (!(formatCtxFlag & AVFMT_NOFILE)) {
            (void)apiWrap_->IoClose(formatContext_->pb);
            formatContext_->pb = nullptr;
        }
        (void)apiWrap_->FormatFlush(formatContext_.get());
    }
    ResetEncoderCtx();
    isInit_ = false;
    return;
}

size_t MediaAudioEncoder::PcmDataSize()
{
    size_t size = DEFALUT_BUFFER_LEN;
    if (audioCodecContext_ == nullptr) {
        return size;
    }

    int bytesPerSample = 0;
    if (srcSampleFormat_ == SampleFormat::S24LE) {
        bytesPerSample = S24LE_SAMPLESIZE;
    } else {
        bytesPerSample = apiWrap_->GetBytesPerSample(AudioSampleMap[srcSampleFormat_]);
    }

    size = static_cast<size_t>(audioCodecContext_->frame_size
        * audioCodecContext_->channels
        * bytesPerSample);
    return size;
}

bool MediaAudioEncoder::DeleteSrcFile(const std::string &filePath)
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
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS