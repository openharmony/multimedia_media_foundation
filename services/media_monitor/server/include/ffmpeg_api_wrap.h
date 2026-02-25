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

#ifndef FFMPEG_API_WRAP_H
#define FFMPEG_API_WRAP_H

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

using CodecFindEncoderFunc = AVCodec *(*)(enum AVCodecID id);
using CodecAllocContextFunc = AVCodecContext *(*)(const AVCodec *codec);
using CodecOpenFunc = int (*)(AVCodecContext *avctx,
        const AVCodec *codec, AVDictionary **options);
using FormatAllocOutputContextFunc = int (*)(AVFormatContext **ctx,
        const AVOutputFormat *oformat,
        const char *formatName,
        const char *filename);
using FormatNewStreamFunc = AVStream *(*)(AVFormatContext *s, const AVCodec *c);
using CodecParamFromContextFunc = int (*)(AVCodecParameters *par,
        const AVCodecContext *codec);
using IoOpenFunc = int (*)(AVIOContext **s, const char *url, int flags,
        const AVIOInterruptCB *intCb, AVDictionary **options);
using IoFlushFunc = void (*)(AVIOContext *s);
using IoCloseFunc = int (*)(AVIOContext *s);

using FormatWriteHeaderFunc = int (*)(AVFormatContext *s, AVDictionary **options);
using FormatWriteTrailerFunc = int (*)(AVFormatContext *s);

using PacketAllocFunc = AVPacket *(*)(void);
using FrameAllocFunc = AVFrame *(*)(void);
using FrameGetBufferFunc = int (*)(AVFrame *frame, int align);

using CodecSendFrameFunc = int (*)(AVCodecContext *avctx, const AVFrame *frame);
using CodecRecvPacketFunc = int (*)(AVCodecContext *avctx, AVPacket *avpkt);
using FormatWriteFrameFunc = int (*)(AVFormatContext *s, AVPacket *pkt);
using FormatFlushFunc = int (*)(AVFormatContext *s);

using FormatFreeContextFunc = void (*)(AVFormatContext *s);
using CodecFreeContextFunc = void(*)(AVCodecContext **avctx);

using FrameFreeFunc = void (*)(AVFrame **frame);
using PacketFreeFunc = void (*)(AVPacket **pkt);
using PacketUnrefFunc = void (*)(AVPacket *pkt);

using GetChannelLayoutFunc = int64_t (*)(int nbChannels);
using GetChannelLayoutDefaultFunc = int64_t (*)(AVChannelLayout *ch_layout, int nb_channels);
using GetChannelLayoutCopyFunc = int64_t (*)(AVChannelLayout *dst, const AVChannelLayout *src);
using GetBytesPerSampleFunc = int (*)(enum AVSampleFormat sampleFmt);
using SampleFmtIsPlannarFunc = int (*)(enum AVSampleFormat sampleFmt);

using SwrSetOptsFunc = struct SwrContext *(*)(struct SwrContext *s,
        int64_t outChLayout, enum AVSampleFormat outSampleFmt, int outSampleRate,
        int64_t inChLayout, enum AVSampleFormat inSampleFmt, int inSampleRate,
        int logOffset, void *logCtx);
using SwrInitFunc = int (*)(struct SwrContext *s);
using SwrAllocFunc = struct SwrContext *(*)();
using SwrFreeFunc = void (*)(struct SwrContext **s);
using SwrConvertFunc = int (*)(struct SwrContext *s,
    uint8_t **out, int outCount, const uint8_t **in, int inCount);

class FFmpegApiWrap {
public:
    FFmpegApiWrap();
    ~FFmpegApiWrap();
    bool Open();
    void Close();
    AVCodec *CodecFindEncoder(AVCodecID id);
    AVCodecContext *CodecAllocContext(const AVCodec *codec);
    int CodecOpen(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);

    int FormatAllocOutputContext(AVFormatContext **ctx,
        const AVOutputFormat *oformat, const char *formatName, const char *filename);
    AVStream *FormatNewStream(AVFormatContext *s, const AVCodec *c);
    int CodecParamFromContext(AVCodecParameters *par, const AVCodecContext *codec);
    int IoOpen(AVIOContext **s, const char *url, int flags,
        const AVIOInterruptCB *intCb, AVDictionary **options);
    void IoFlush(AVIOContext *s);
    int IoClose(AVIOContext *s);
    int FormatWriteHeader(AVFormatContext *s, AVDictionary **options);
    int FormatWriteTrailer(AVFormatContext *s);
    int CodecSendFrame(AVCodecContext *avctx, const AVFrame *frame);
    int CodecRecvPacket(AVCodecContext *avctx, AVPacket *avpkt);
    int FormatWriteFrame(AVFormatContext *s, AVPacket *pkt);
    int FormatFlush(AVFormatContext *s);
    void FormatFreeContext(AVFormatContext *s);
    void CodecFreeContext(AVCodecContext **avctx);

    AVPacket *PacketAlloc();
    AVFrame *FrameAlloc();
    int FrameGetBuffer(AVFrame *frame, int align);
    void FrameFree(AVFrame **frame);
    void PacketFree(AVPacket **pkt);
    void PacketUnref(AVPacket *pkt);

    int64_t GetChannelLayout(int nbChannels);
    int64_t GetChannelLayoutDefault(AVChannelLayout *ch_layout, int nb_channels);
    int64_t GetChannelLayoutCopy(AVChannelLayout *dst, const AVChannelLayout *src);
    int GetBytesPerSample(AVSampleFormat sampleFmt);
    int SampleFmtIsPlannar(AVSampleFormat sampleFmt);
    struct SwrContext *SwrAlloc();
    struct SwrContext *SwrSetOpts(struct SwrContext *s,
        int64_t outChLayout, AVSampleFormat outSampleFmt, int outSampleRate,
        int64_t inChLayout, AVSampleFormat inSampleFmt, int inSampleRate,
        int logOffset, void *logCtx);
    int SwrInit(struct SwrContext *s);
    void SwrFree(struct SwrContext **s);
    int SwrConvert(struct SwrContext *s,
        uint8_t **out, int outCount, const uint8_t **in, int inCount);
private:
    bool LoadFormatApi();
    bool LoadCodecApi();
    bool LoadUtilsApi();
    bool LoadResampleApi();
    void *handler = nullptr;
    FormatAllocOutputContextFunc foramtAllocOutputFunc = nullptr;
    FormatNewStreamFunc formatNewStreamFunc = nullptr;
    FormatWriteHeaderFunc formatWriteHeaderFunc = nullptr;
    FormatWriteTrailerFunc formatWriteTrailerFunc = nullptr;
    FormatWriteFrameFunc writeFrameFunc = nullptr;
    FormatFlushFunc formatFlushFunc = nullptr;
    FormatFreeContextFunc formatFreeContextFunc = nullptr;
    IoOpenFunc ioOpenFunc = nullptr;
    IoFlushFunc ioFlushFunc = nullptr;
    IoCloseFunc ioCloseFunc = nullptr;

    CodecFindEncoderFunc codecFindFunc = nullptr;
    CodecAllocContextFunc codecAllocFunc = nullptr;
    CodecOpenFunc codecOpenFunc = nullptr;
    CodecParamFromContextFunc codecParamFromContextFunc = nullptr;
    CodecSendFrameFunc sendFrameFunc = nullptr;
    CodecRecvPacketFunc recvPacketFunc = nullptr;
    CodecFreeContextFunc codecFreeContextFunc = nullptr;

    PacketAllocFunc packetAllocFunc = nullptr;
    PacketFreeFunc packetFreeFunc = nullptr;
    PacketUnrefFunc packetUnrefFunc = nullptr;

    FrameAllocFunc frameAllocFunc = nullptr;
    FrameGetBufferFunc frameGetBufferFunc = nullptr;
    FrameFreeFunc frameFreeFunc = nullptr;
    GetChannelLayoutFunc getChannelLayoutFunc = nullptr;
    GetChannelLayoutDefaultFunc getChannelLayoutDefaultFunc = nullptr;
    GetChannelLayoutCopyFunc getChannelLayoutCopyFunc = nullptr;
    GetBytesPerSampleFunc getBytesPerSampleFunc = nullptr;
    SampleFmtIsPlannarFunc sampleFmtIsPlannarFunc = nullptr;

    SwrSetOptsFunc swrSetOptsFunc = nullptr;
    SwrAllocFunc swrAllocFunc = nullptr;
    SwrFreeFunc swrFreeFunc = nullptr;
    SwrInitFunc swrInitFunc = nullptr;
    SwrConvertFunc swrConvertFunc = nullptr;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // FFMPEG_API_WRAP_H