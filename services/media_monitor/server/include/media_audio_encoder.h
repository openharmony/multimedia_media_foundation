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

#ifndef MEDIA_AUDIO_ENCODER_H
#define MEDIA_AUDIO_ENCODER_H

#include <string>
#include <memory>
#include <vector>
#include "ffmpeg_api_wrap.h"

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

using AudioEncodeConfig = struct AudioEncodeConfig {
    int64_t bitRate {0};
    uint32_t sampleRate {0};
    uint32_t channels {1};
    SampleFormat sampleFmt {S16LE};
    AVCodecID audioCodecId {AV_CODEC_ID_NONE};
};

using VideoEncodeConfig = struct VideoEncodeConfig {
    int64_t bitRate {0};
    AVCodecID videoCodecId {AV_CODEC_ID_NONE};
};

using ResamplePara = struct ResamplePara {
    uint32_t channels {1};
    uint32_t sampleRate {0};
    uint64_t channelLayout {0};
    AVSampleFormat srcFfFmt {AV_SAMPLE_FMT_NONE};
    AVSampleFormat destFmt {AV_SAMPLE_FMT_S16};
};

class SampleConvert {
public:
    SampleConvert(std::shared_ptr<FFmpegApiWrap> apiWrap);
    ~SampleConvert();
    int32_t Init(const ResamplePara &param);
    int32_t Convert(const uint8_t *src, size_t size, AVFrame *frame);
    void Release();
private:
    bool isInit_ = false;
    ResamplePara resamplePara_;
    std::shared_ptr<SwrContext> swrCtx_ = nullptr;
    std::shared_ptr<FFmpegApiWrap> apiWrap_ = nullptr;
};

class MediaAudioEncoder {
public:
    MediaAudioEncoder() {};
    ~MediaAudioEncoder() {};
    size_t PcmDataSize();
    int32_t EncodePcmFiles(const std::string &fileDir);
private:
    int32_t EncodePcmToFlac(const std::string &in);
    int32_t Init(const std::string &inputFile);
    int32_t WritePcm(const uint8_t *buffer, size_t size);
    void Release();
    bool IsSupportAudioArgs(std::string &audioArg, const std::vector<std::string> &supportList);
    int32_t ParseAudioArgs(const std::string &fileName, AudioEncodeConfig &config);
    int32_t GetAudioConfig(const std::string &inFullName, AudioEncodeConfig &config);
    int32_t InitMux();
    int32_t InitAudioEncode(const AudioEncodeConfig &audioConfig);
    int32_t InitFrame();
    int32_t InitPacket();
    int32_t InitSampleConvert();
    int32_t FillFrameFromBuffer(const uint8_t *buffer, size_t size);
    int32_t WriteFrame();
    void CopyS24ToS32(int32_t *dst, const uint8_t *src, size_t count);
    void ResetEncoderCtx();
    bool DeleteSrcFile(const std::string &filePath);
    bool isInit_ = false;
    std::string fileName_;
    SampleFormat srcSampleFormat_ = SampleFormat::S16LE;
    std::shared_ptr<AVFormatContext> formatContext_ = nullptr;
    std::shared_ptr<AVCodecContext> audioCodecContext_ = nullptr;
    std::shared_ptr<AVCodecContext> videoCodecContext_ = nullptr;
    std::shared_ptr<AVPacket> avPacket_ = nullptr;
    std::shared_ptr<AVFrame> avFrame_ = nullptr;
    std::shared_ptr<SampleConvert> sampleConvert_ = nullptr;
    std::shared_ptr<FFmpegApiWrap> apiWrap_ = nullptr;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_AUDIO_ENCODER_H