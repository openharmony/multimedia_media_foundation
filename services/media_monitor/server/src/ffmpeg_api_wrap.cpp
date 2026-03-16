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

#include "ffmpeg_api_wrap.h"
#include <string>
#include <dlfcn.h>
#include "log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

FFmpegApiWrap::FFmpegApiWrap()
{
}

FFmpegApiWrap::~FFmpegApiWrap()
{
    Close();
}

bool FFmpegApiWrap::Open()
{
    bool ret = false;
    std::string path = "libohosffmpeg.z.so";
    handler = dlopen(path.c_str(), RTLD_NOW);
    if (handler == nullptr) {
        return false;
    }
    ret = LoadFormatApi();
    FALSE_RETURN_V(ret == true, ret);
    ret = LoadCodecApi();
    FALSE_RETURN_V(ret == true, ret);
    ret = LoadUtilsApi();
    FALSE_RETURN_V(ret == true, ret);
    ret = LoadResampleApi();
    FALSE_RETURN_V(ret == true, ret);
    return ret;
}

bool FFmpegApiWrap::LoadFormatApi()
{
    foramtAllocOutputFunc = (FormatAllocOutputContextFunc)dlsym(handler, "avformat_alloc_output_context2");
    FALSE_RETURN_V_MSG_E(foramtAllocOutputFunc != nullptr, false, "load error");
    formatFreeContextFunc = (FormatFreeContextFunc)dlsym(handler, "avformat_free_context");
    FALSE_RETURN_V_MSG_E(formatFreeContextFunc != nullptr, false, "load error");
    formatNewStreamFunc = (FormatNewStreamFunc)dlsym(handler, "avformat_new_stream");
    FALSE_RETURN_V_MSG_E(formatNewStreamFunc != nullptr, false, "load error");
    formatFlushFunc = (FormatFlushFunc)dlsym(handler, "avformat_flush");
    FALSE_RETURN_V_MSG_E(formatFlushFunc != nullptr, false, "load error");
    formatWriteHeaderFunc = (FormatWriteHeaderFunc)dlsym(handler, "avformat_write_header");
    FALSE_RETURN_V_MSG_E(formatWriteHeaderFunc != nullptr, false, "load error");
    formatWriteTrailerFunc = (FormatWriteTrailerFunc)dlsym(handler, "av_write_trailer");
    FALSE_RETURN_V_MSG_E(formatWriteTrailerFunc != nullptr, false, "load error");
    ioOpenFunc = (IoOpenFunc)dlsym(handler, "avio_open2");
    FALSE_RETURN_V_MSG_E(ioOpenFunc != nullptr, false, "load error");
    ioFlushFunc = (IoFlushFunc)dlsym(handler, "avio_flush");
    FALSE_RETURN_V_MSG_E(ioFlushFunc != nullptr, false, "load error");
    ioCloseFunc = (IoCloseFunc)dlsym(handler, "avio_close");
    FALSE_RETURN_V_MSG_E(ioCloseFunc != nullptr, false, "load error");
    writeFrameFunc = (FormatWriteFrameFunc)dlsym(handler, "av_interleaved_write_frame");
    FALSE_RETURN_V_MSG_E(writeFrameFunc != nullptr, false, "load error");
    return true;
}

bool FFmpegApiWrap::LoadCodecApi()
{
    codecFindFunc = (CodecFindEncoderFunc)dlsym(handler, "avcodec_find_encoder");
    FALSE_RETURN_V_MSG_E(codecFindFunc != nullptr, false, "load error");
    codecAllocFunc = (CodecAllocContextFunc)dlsym(handler, "avcodec_alloc_context3");
    FALSE_RETURN_V_MSG_E(codecAllocFunc != nullptr, false, "load error");
    codecFreeContextFunc = (CodecFreeContextFunc)dlsym(handler, "avcodec_free_context");
    FALSE_RETURN_V_MSG_E(codecFreeContextFunc != nullptr, false, "load error");
    codecOpenFunc = (CodecOpenFunc)dlsym(handler, "avcodec_open2");
    FALSE_RETURN_V_MSG_E(codecOpenFunc != nullptr, false, "load error");
    codecParamFromContextFunc = (CodecParamFromContextFunc)dlsym(handler, "avcodec_parameters_from_context");
    FALSE_RETURN_V_MSG_E(codecParamFromContextFunc != nullptr, false, "load error");
    sendFrameFunc = (CodecSendFrameFunc)dlsym(handler, "avcodec_send_frame");
    FALSE_RETURN_V_MSG_E(sendFrameFunc != nullptr, false, "load error");
    recvPacketFunc = (CodecRecvPacketFunc)dlsym(handler, "avcodec_receive_packet");
    FALSE_RETURN_V_MSG_E(recvPacketFunc != nullptr, false, "load error");
    packetAllocFunc = (PacketAllocFunc)dlsym(handler, "av_packet_alloc");
    FALSE_RETURN_V_MSG_E(packetAllocFunc != nullptr, false, "load error");
    packetFreeFunc = (PacketFreeFunc)dlsym(handler, "av_packet_free");
    FALSE_RETURN_V_MSG_E(packetFreeFunc != nullptr, false, "load error");
    packetUnrefFunc = (PacketUnrefFunc)dlsym(handler, "av_packet_unref");
    FALSE_RETURN_V_MSG_E(packetUnrefFunc != nullptr, false, "load error");
    return true;
}

bool FFmpegApiWrap::LoadUtilsApi()
{
    frameAllocFunc = (FrameAllocFunc)dlsym(handler, "av_frame_alloc");
    FALSE_RETURN_V_MSG_E(frameAllocFunc != nullptr, false, "load error");
    frameGetBufferFunc = (FrameGetBufferFunc)dlsym(handler, "av_frame_get_buffer");
    FALSE_RETURN_V_MSG_E(frameGetBufferFunc != nullptr, false, "load error");
    frameFreeFunc = (FrameFreeFunc)dlsym(handler, "av_frame_free");
    FALSE_RETURN_V_MSG_E(frameFreeFunc != nullptr, false, "load error");
    getChannelLayoutFromMaskFunc = (GetChannelLayoutFromMaskFunc)dlsym(handler, "av_channel_layout_from_mask");
    FALSE_RETURN_V_MSG_E(getChannelLayoutFromMaskFunc != nullptr, false, "load error");
    getChannelLayoutDefaultFunc = (GetChannelLayoutDefaultFunc)dlsym(handler, "av_channel_layout_default");
    FALSE_RETURN_V_MSG_E(getChannelLayoutDefaultFunc != nullptr, false, "load error");
    getChannelLayoutCopyFunc = (GetChannelLayoutCopyFunc)dlsym(handler, "av_channel_layout_copy");
    FALSE_RETURN_V_MSG_E(getChannelLayoutCopyFunc != nullptr, false, "load error");
    getBytesPerSampleFunc = (GetBytesPerSampleFunc)dlsym(handler, "av_get_bytes_per_sample");
    FALSE_RETURN_V_MSG_E(getBytesPerSampleFunc != nullptr, false, "load error");
    sampleFmtIsPlannarFunc = (SampleFmtIsPlannarFunc)dlsym(handler, "av_sample_fmt_is_planar");
    FALSE_RETURN_V_MSG_E(sampleFmtIsPlannarFunc != nullptr, false, "load error");
    return true;
}

bool FFmpegApiWrap::LoadResampleApi()
{
    swrSetOpts2Func = (SwrSetOpts2Func)dlsym(handler, "swr_alloc_set_opts2");
    FALSE_RETURN_V_MSG_E(swrSetOpts2Func != nullptr, false, "load error");
    swrAllocFunc = (SwrAllocFunc)dlsym(handler, "swr_alloc");
    FALSE_RETURN_V_MSG_E(swrAllocFunc != nullptr, false, "load error");
    swrInitFunc = (SwrInitFunc)dlsym(handler, "swr_init");
    FALSE_RETURN_V_MSG_E(swrInitFunc != nullptr, false, "load error");
    swrFreeFunc = (SwrFreeFunc)dlsym(handler, "swr_free");
    FALSE_RETURN_V_MSG_E(swrFreeFunc != nullptr, false, "load error");
    swrConvertFunc = (SwrConvertFunc)dlsym(handler, "swr_convert");
    FALSE_RETURN_V_MSG_E(swrConvertFunc != nullptr, false, "load error");
    return true;
}

void FFmpegApiWrap::Close()
{
    if (handler != nullptr) {
        dlclose(handler);
        handler = nullptr;
    }
}

AVCodec *FFmpegApiWrap::CodecFindEncoder(AVCodecID id)
{
    AVCodec *codec = nullptr;
    if (codecFindFunc != nullptr) {
        codec = codecFindFunc(id);
    }
    return codec;
}

AVCodecContext *FFmpegApiWrap::CodecAllocContext(const AVCodec *codec)
{
    AVCodecContext *context = nullptr;
    if (codecAllocFunc != nullptr) {
        context = codecAllocFunc(codec);
    }
    return context;
}

int FFmpegApiWrap::CodecOpen(AVCodecContext *avctx,
    const AVCodec *codec, AVDictionary **options)
{
    int ret = -1;
    if (codecOpenFunc != nullptr) {
        ret = codecOpenFunc(avctx, codec, options);
    }
    return ret;
}

int FFmpegApiWrap::FormatAllocOutputContext(AVFormatContext **ctx,
    const AVOutputFormat *oformat, const char *formatName, const char *filename)
{
    int ret = -1;
    if (foramtAllocOutputFunc != nullptr) {
        ret = foramtAllocOutputFunc(ctx, oformat, formatName, filename);
    }
    return ret;
}

AVStream *FFmpegApiWrap::FormatNewStream(AVFormatContext *s, const AVCodec *c)
{
    AVStream *stream = nullptr;
    if (formatNewStreamFunc != nullptr) {
        stream = formatNewStreamFunc(s, c);
    }
    return stream;
}

int FFmpegApiWrap::CodecParamFromContext(AVCodecParameters *par, const AVCodecContext *codec)
{
    int ret = -1;
    if (codecParamFromContextFunc != nullptr) {
        ret = codecParamFromContextFunc(par, codec);
    }
    return ret;
}

int FFmpegApiWrap::IoOpen(AVIOContext **s, const char *url, int flags,
    const AVIOInterruptCB *intCb, AVDictionary **options)
{
    int ret = -1;
    if (ioOpenFunc != nullptr) {
        ret = ioOpenFunc(s, url, flags, intCb, options);
    }
    return ret;
}

void FFmpegApiWrap::IoFlush(AVIOContext *s)
{
    if (ioFlushFunc != nullptr) {
        ioFlushFunc(s);
    }
}

int FFmpegApiWrap::IoClose(AVIOContext *s)
{
    int ret = -1;
    if (ioCloseFunc != nullptr) {
        ret = ioCloseFunc(s);
    }
    return ret;
}

int FFmpegApiWrap::FormatWriteHeader(AVFormatContext *s, AVDictionary **options)
{
    int ret = -1;
    if (formatWriteHeaderFunc != nullptr) {
        ret = formatWriteHeaderFunc(s, options);
    }
    return ret;
}

int FFmpegApiWrap::FormatWriteTrailer(AVFormatContext *s)
{
    int ret = -1;
    if (formatWriteTrailerFunc != nullptr) {
        ret = formatWriteTrailerFunc(s);
    }
    return ret;
}

AVPacket *FFmpegApiWrap::PacketAlloc()
{
    AVPacket *pkt = nullptr;
    if (packetAllocFunc != nullptr) {
        pkt = packetAllocFunc();
    }
    return pkt;
}

AVFrame *FFmpegApiWrap::FrameAlloc()
{
    AVFrame *frame = nullptr;
    if (frameAllocFunc != nullptr) {
        frame = frameAllocFunc();
    }
    return frame;
}

int FFmpegApiWrap::FrameGetBuffer(AVFrame *frame, int align)
{
    int ret = -1;
    if (frameGetBufferFunc != nullptr) {
        ret = frameGetBufferFunc(frame, align);
    }
    return ret;
}

int FFmpegApiWrap::CodecSendFrame(AVCodecContext *avctx, const AVFrame *frame)
{
    int ret = -1;
    if (sendFrameFunc != nullptr) {
        ret = sendFrameFunc(avctx, frame);
    }
    return ret;
}

int FFmpegApiWrap::CodecRecvPacket(AVCodecContext *avctx, AVPacket *avpkt)
{
    int ret = -1;
    if (recvPacketFunc != nullptr) {
        ret = recvPacketFunc(avctx, avpkt);
    }
    return ret;
}

int FFmpegApiWrap::FormatWriteFrame(AVFormatContext *s, AVPacket *pkt)
{
    int ret = -1;
    if (writeFrameFunc != nullptr) {
        ret = writeFrameFunc(s, pkt);
    }
    return ret;
}

int FFmpegApiWrap::FormatFlush(AVFormatContext *s)
{
    int ret = -1;
    if (formatFlushFunc != nullptr) {
        ret = formatFlushFunc(s);
    }
    return ret;
}

void FFmpegApiWrap::FormatFreeContext(AVFormatContext *s)
{
    if (formatFreeContextFunc != nullptr) {
        formatFreeContextFunc(s);
    }
}

void FFmpegApiWrap::CodecFreeContext(AVCodecContext **avctx)
{
    if (codecFreeContextFunc != nullptr) {
        codecFreeContextFunc(avctx);
    }
}

void FFmpegApiWrap::FrameFree(AVFrame **frame)
{
    if (frameFreeFunc != nullptr) {
        frameFreeFunc(frame);
    }
}

void FFmpegApiWrap::PacketFree(AVPacket **pkt)
{
    if (packetFreeFunc != nullptr) {
        packetFreeFunc(pkt);
    }
}

void FFmpegApiWrap::PacketUnref(AVPacket *pkt)
{
    if (packetUnrefFunc != nullptr) {
        packetUnrefFunc(pkt);
    }
}

int64_t FFmpegApiWrap::GetChannelLayoutFromMask(AVChannelLayout *channel_layout, uint64_t mask)
{
    int ret = -1;
    if (getChannelLayoutFromMaskFunc != nullptr) {
        ret = getChannelLayoutFromMaskFunc(channel_layout, mask);
    }
    return ret;
}

int64_t FFmpegApiWrap::GetChannelLayoutDefault(AVChannelLayout *ch_layout, int nb_channels)
{
    int ret = -1;
    if (getChannelLayoutDefaultFunc != nullptr) {
        ret = getChannelLayoutDefaultFunc(ch_layout, nb_channels);
    }
    return ret;
}

int64_t FFmpegApiWrap::GetChannelLayoutCopy(AVChannelLayout *dst, const AVChannelLayout *src)
{
    int ret = -1;
    if (getChannelLayoutCopyFunc != nullptr) {
        ret = getChannelLayoutCopyFunc(dst, src);
    }
    return ret;
}

int FFmpegApiWrap::GetBytesPerSample(AVSampleFormat sampleFmt)
{
    int ret = -1;
    if (getBytesPerSampleFunc != nullptr) {
        ret = getBytesPerSampleFunc(sampleFmt);
    }
    return ret;
}

int FFmpegApiWrap::SampleFmtIsPlannar(AVSampleFormat sampleFmt)
{
    int ret = -1;
    if (sampleFmtIsPlannarFunc != nullptr) {
        ret = sampleFmtIsPlannarFunc(sampleFmt);
    }
    return ret;
}

struct SwrContext *FFmpegApiWrap::SwrAlloc()
{
    struct SwrContext *s = nullptr;
    if (swrAllocFunc) {
        s = swrAllocFunc();
    }
    return s;
}

int FFmpegApiWrap::SwrSetOpts2(struct SwrContext **ps,
    const AVChannelLayout *out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
    const AVChannelLayout *in_ch_layout, enum AVSampleFormat in_sample_fmt, int in_sample_rate,
    int log_offset, void *log_ctx)
{
    int ret = -1;
    if (swrSetOpts2Func != nullptr) {
        ret = swrSetOpts2Func(ps, out_ch_layout, out_sample_fmt, out_sample_rate,
            in_ch_layout, in_sample_fmt, in_sample_rate, log_offset, log_ctx);
    }
    return ret;
}

int FFmpegApiWrap::SwrInit(struct SwrContext *s)
{
    int ret = -1;
    if (swrInitFunc != nullptr) {
        ret = swrInitFunc(s);
    }
    return ret;
}

void FFmpegApiWrap::SwrFree(struct SwrContext **s)
{
    if (swrFreeFunc) {
        swrFreeFunc(s);
    }
}

int FFmpegApiWrap::SwrConvert(struct SwrContext *s,
    uint8_t **out, int outCount, const uint8_t **in, int inCount)
{
    int ret = -1;
    if (swrConvertFunc != nullptr) {
        ret = swrConvertFunc(s, out, outCount, in, inCount);
    }
    return ret;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS