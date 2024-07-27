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

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace Media {
namespace MediaMonitor {

using AudioEncodeConfig = struct AudioEncodeConfig {
    int64_t bitRate {0};
    int32_t sampleRate {0};
    uint64_t channelLayout {0};
    AVCodecID audioCodecId {AV_CODEC_ID_NONE};
    AVSampleFormat sampleFmt {AV_SAMPLE_FMT_NONE};
};

using VideoEncodeConfig = struct VideoEncodeConfig {
    int64_t bitRate {0};
    AVCodecID videoCodecId {AV_CODEC_ID_NONE};
};

using ResamplePara = struct ResamplePara {
    uint32_t channels {2}; // 2: STEREO
    uint32_t sampleRate {0};
    uint64_t channelLayout {0};
    AVSampleFormat srcFfFmt {AV_SAMPLE_FMT_NONE};
    AVSampleFormat destFmt {AV_SAMPLE_FMT_S16};
};

class SampleConvert {
public:
    int32_t Init(const ResamplePara& param);
    int32_t Convert(const uint8_t* src, size_t size, std::shared_ptr<AVFrame>& frame);
private:
    bool isInit_ = false;
    std::shared_ptr<SwrContext> swrCtx_ = nullptr;
    ResamplePara resamplePara_;
};

class MediaAudioEncoder {
public:
    MediaAudioEncoder() {};
    ~MediaAudioEncoder() {};
    size_t PcmDataSize();
    int32_t EncodePcmFiles(const std::string& fileDir);
private:
    int32_t EncodePcmToFlac(const std::string& in);
    int32_t Init(const std::string& inputFile);
    int32_t WritePcm(uint8_t* buffer, size_t size);
    int32_t Release();
    bool IsSupportAudioArgs(std::string& audioArg, const std::vector<std::string>& supportList);
    int32_t ParseAudioArgs(std::string& filename, AudioEncodeConfig& config);
    AudioEncodeConfig GetAudioConfig();
    int32_t InitMux(const std::string &url);
    int32_t InitAudioEncode(const AudioEncodeConfig& audioConfig);
    int32_t InitFrame(std::shared_ptr<AVFrame>& frame);
    int32_t InitPacket(std::shared_ptr<AVPacket>& packet);
    int32_t InitSampleConvert();
    int32_t WriteFrame(std::shared_ptr<AVFrame>& frame);
    void ResetEncoderCtx();
    bool DeleteSrcFile(const std::string& filePath);
    int32_t CheckFilePath(const std::string& inputFile);
    bool isInit_ = false;
    std::string url_;
    std::shared_ptr<AVFormatContext> formatContext_ = nullptr;
    std::shared_ptr<AVCodecContext> audioCodecContext_ = nullptr;
    std::shared_ptr<AVCodecContext> videoCodecContext_ = nullptr;
    std::shared_ptr<AVPacket> avPacket_ = nullptr;
    std::shared_ptr<AVFrame> avFrame_ = nullptr;
    AVSampleFormat srcSampleFormat_ = AV_SAMPLE_FMT_S16;
    std::shared_ptr<SampleConvert> sampleConvert_ = nullptr;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_AUDIO_ENCODER_H