/*
 *
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

#define HST_LOG_TAG "FFmpegDemuxerPlugin"

// Modify FFMPEG_LOG_DEBUG_ENABLE to 1 to show Ffmpeg log.
#define FFMPEG_LOG_DEBUG_ENABLE 0

#include "ffmpeg_demuxer_plugin.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
#include "ffmpeg_track_meta.h"
#include "foundation/cpp_ext/memory_ext.h"
#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "plugin/core/plugin_manager.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 78, 0) and LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(58, 64, 100)
#if LIBAVFORMAT_VERSION_INT != AV_VERSION_INT(58, 76, 100)
#include "libavformat/internal.h"
#endif
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
namespace {
std::map<std::string, std::shared_ptr<AVInputFormat>> g_pluginInputFormat;

std::map<AVMediaType, MediaType> g_MediaTypeMap = {
    {AVMEDIA_TYPE_AUDIO, MediaType::AUDIO},
    {AVMEDIA_TYPE_VIDEO, MediaType::VIDEO},
    {AVMEDIA_TYPE_DATA, MediaType::DATA},
    {AVMEDIA_TYPE_ATTACHMENT, MediaType::ATTACHMENT},
    {AVMEDIA_TYPE_UNKNOWN, MediaType::UNKNOWN},
    {AVMEDIA_TYPE_SUBTITLE, MediaType::SUBTITLE}
};

static const std::map<SeekMode, int32_t> seekModeToFfmpegSeekFlags = {
    { SeekMode::SEEK_PREVIOUS_SYNC, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD },
    { SeekMode::SEEK_NEXT_SYNC, AVSEEK_FLAG_FRAME },
    { SeekMode::SEEK_CLOSEST_SYNC, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_ANY },
    { SeekMode::SEEK_CLOSEST, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_ANY }
};
int Sniff(const std::string& pluginName, std::shared_ptr<DataSource> dataSource);

Status RegisterPlugins(const std::shared_ptr<Register>& reg);
static void FfmpegLogInit();
} // namespace

void* FFmpegDemuxerPlugin::DemuxerPluginAllocator::Alloc(size_t size)
{
    if (size == 0) {
        return nullptr;
    }
    return static_cast<void*>(new (std::nothrow) uint8_t[size]);
}

void FFmpegDemuxerPlugin::DemuxerPluginAllocator::Free(void* ptr) // NOLINT: void*
{
    if (ptr) {
        auto data = static_cast<uint8_t*>(ptr);
        delete[] data;
    }
}

FFmpegDemuxerPlugin::FFmpegDemuxerPlugin(std::string name)
    : DemuxerPlugin(std::move(name)),
      ioContext_(),
      callback_(nullptr),
      pluginImpl_(nullptr),
      formatContext_(nullptr),
      allocator_(std::make_shared<DemuxerPluginAllocator>()),
      mediaInfo_(nullptr),
      selectedTrackIds_()
{
    MEDIA_LOG_I("ctor called, plugin name: " PUBLIC_LOG_S, pluginName_.c_str());
}

FFmpegDemuxerPlugin::~FFmpegDemuxerPlugin()
{
    MEDIA_LOG_D("dtor called.");
    pluginImpl_ = nullptr;
}

Status FFmpegDemuxerPlugin::Init()
{
    MEDIA_LOG_D("Init called.");
    Reset();
    FfmpegLogInit();
    pluginImpl_ = g_pluginInputFormat[pluginName_];

    return pluginImpl_ ? Status::OK : Status::ERROR_UNSUPPORTED_FORMAT;
}

Status FFmpegDemuxerPlugin::Deinit()
{
    avbsfContext_.reset();
    vdBitStreamFormat_ = VideoBitStreamFormat{VideoBitStreamFormat::UNKNOWN};
    return Status::OK;
}

Status FFmpegDemuxerPlugin::Prepare()
{
    InitAVFormatContext();
    if (formatContext_ == nullptr) {
        MEDIA_LOG_E("prepare failed due to formatContext init error");
        return Status::ERROR_UNKNOWN;
    }
    return Status::OK;
}

Status FFmpegDemuxerPlugin::Reset()
{
    mediaInfo_.reset();
    ioContext_.offset = 0;
    ioContext_.eos = false;
    selectedTrackIds_.clear();
    avbsfContext_.reset();
    vdBitStreamFormat_ = VideoBitStreamFormat{VideoBitStreamFormat::UNKNOWN};
    return Status::OK;
}

/**
 * GetParameter no need supported by demuxer
 * @return return ERROR_UNIMPLEMENTED always.
 */
Status FFmpegDemuxerPlugin::GetParameter(Tag tag, ValueType& value)
{
    switch (tag) {
        case Tag::MEDIA_PLAYBACK_SPEED:
            value = playbackSpeed_;
            break;
        default:
            break;
    }
    return Status::OK;
}

/**
 * SetParameter no need supported by demuxer
 * @return return ERROR_UNIMPLEMENTED always.
 */
Status FFmpegDemuxerPlugin::SetParameter(Tag tag, const ValueType& value)
{
    switch (tag) {
        case Tag::MEDIA_PLAYBACK_SPEED:
            playbackSpeed_ = Plugin::AnyCast<double>(value);
            break;
        case Tag::VIDEO_BIT_STREAM_FORMAT:
            vdBitStreamFormat_ = Plugin::AnyCast<VideoBitStreamFormat>(value);
            break;
        default:
            break;
    }
    return Status::OK;
}

std::shared_ptr<Allocator> FFmpegDemuxerPlugin::GetAllocator()
{
    return allocator_;
}

Status FFmpegDemuxerPlugin::SetCallback(Callback* cb)
{
    callback_ = cb;
    return Status::OK;
}

Status FFmpegDemuxerPlugin::SetDataSource(const std::shared_ptr<DataSource>& source)
{
    ioContext_.dataSource = source;
    seekable_ = source->GetSeekable();
    return Status::OK;
}

Status FFmpegDemuxerPlugin::GetMediaInfo(MediaInfo& mediaInfo)
{
    if (!mediaInfo_ && !ParseMediaData()) {
        return Status::ERROR_WRONG_STATE;
    }
    mediaInfo = *mediaInfo_;
    return Status::OK;
}

size_t FFmpegDemuxerPlugin::GetTrackCount()
{
    size_t trackCnt = 0;
    if (mediaInfo_) {
        trackCnt = mediaInfo_->tracks.size();
    }
    return trackCnt;
}

Status FFmpegDemuxerPlugin::SelectTrack(int32_t trackId)
{
    if (!mediaInfo_) {
        MEDIA_LOG_E("SelectTrack called before GetMediaInfo()...");
        return Status::ERROR_WRONG_STATE;
    }
    if (trackId < 0 || trackId >= static_cast<int32_t>(mediaInfo_->tracks.size())) {
        MEDIA_LOG_E("SelectTrack called with invalid trackId: " PUBLIC_LOG_D32 ", number of tracks: " PUBLIC_LOG
                    "d", trackId, static_cast<int>(mediaInfo_->tracks.size()));
        return Status::ERROR_INVALID_PARAMETER;
    }
    OSAL::ScopedLock lock(mutex_);
    auto it = std::find_if(selectedTrackIds_.begin(), selectedTrackIds_.end(),
                           [trackId](int32_t streamId) { return trackId == streamId; });
    if (it == selectedTrackIds_.end()) {
        selectedTrackIds_.push_back(trackId);
    }
    return Status::OK;
}

Status FFmpegDemuxerPlugin::UnselectTrack(int32_t trackId)
{
    OSAL::ScopedLock lock(mutex_);
    auto it = std::find_if(selectedTrackIds_.begin(), selectedTrackIds_.end(),
                           [trackId](int32_t streamId) { return trackId == streamId; });
    if (it != selectedTrackIds_.end()) {
        selectedTrackIds_.erase(it);
    }
    return Status::OK;
}

Status FFmpegDemuxerPlugin::GetSelectedTracks(std::vector<int32_t>& trackIds)
{
    OSAL::ScopedLock lock(mutex_);
    trackIds = selectedTrackIds_;
    return Status::OK;
}

bool FFmpegDemuxerPlugin::ConvertAVPacketToFrameInfo(const AVStream& avStream, AVPacket& pkt, Buffer& frameInfo)
{
    frameInfo.trackID = static_cast<uint32_t>(pkt.stream_index);
    int64_t pts = (pkt.pts > 0) ? pkt.pts : 0;
    frameInfo.pts = ConvertTimeFromFFmpeg(pts, avStream.time_base);
    frameInfo.dts = static_cast<uint32_t>(pkt.dts);
    frameInfo.duration = ConvertTimeFromFFmpeg(pkt.duration, avStream.time_base);
    frameInfo.GetBufferMeta()->SetMeta(Tag::MEDIA_POSITION, static_cast<uint32_t>(pkt.pos));

    int frameSize = 0;
    if (avStream.codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        frameSize = pkt.size;
    } else if (avStream.codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (avStream.codecpar->codec_id == AV_CODEC_ID_RAWVIDEO) {
            MEDIA_LOG_W("unsupport raw video");
            return false;
        }
        if (vdBitStreamFormat_ == VideoBitStreamFormat::ANNEXB) {
            if (!avbsfContext_) {
                InitConvertContext(avStream);
            }
            if (avbsfContext_) {
                ConvertAvcOrHevcToAnnexb(pkt);
            }
        }
        frameSize = pkt.size;
        frameInfo.ChangeBufferMetaType(BufferMetaType::VIDEO);
    } else {
        MEDIA_LOG_W("unsupported codec type: " PUBLIC_LOG_D32, static_cast<int32_t>(avStream.codecpar->codec_type));
        return false;
    }
    auto data = frameInfo.AllocMemory(allocator_, frameSize);
    if (data) {
        size_t writeSize = data->Write(pkt.data, frameSize);
        FALSE_LOG_MSG(writeSize == static_cast<size_t>(frameSize), "Copy data failed.");
    }
    return data != nullptr;
}

void FFmpegDemuxerPlugin::InitConvertContext(const AVStream& avStream)
{
    const AVBitStreamFilter* avBitStreamFilter {nullptr};
    char codeTag[AV_FOURCC_MAX_STRING_SIZE] {0};
    av_fourcc_make_string(codeTag, avStream.codecpar->codec_tag);
    if (strncmp(codeTag, "avc1", strlen("avc1")) == 0) {
        avBitStreamFilter = av_bsf_get_by_name("h264_mp4toannexb");
    } else if (strncmp(codeTag, "hevc", strlen("hevc")) == 0) {
        avBitStreamFilter = av_bsf_get_by_name("hevc_mp4toannexb");
    }
    if (avBitStreamFilter && !avbsfContext_) {
        AVBSFContext* avbsfContext {nullptr};
        (void)av_bsf_alloc(avBitStreamFilter, &avbsfContext);
        (void)avcodec_parameters_copy(avbsfContext->par_in, avStream.codecpar);
        av_bsf_init(avbsfContext);
        avbsfContext_ = std::shared_ptr<AVBSFContext>(avbsfContext, [](AVBSFContext* ptr) {
            if (ptr) {
                av_bsf_free(&ptr);
            }
        });
    }
    FALSE_LOG_MSG(avbsfContext_ != nullptr, "the av bit stream convert can't support, codec_tag: " PUBLIC_LOG_S,
        codeTag);
}

void FFmpegDemuxerPlugin::ConvertAvcOrHevcToAnnexb(AVPacket& pkt)
{
    (void)av_bsf_send_packet(avbsfContext_.get(), &pkt);
    (void)av_packet_unref(&pkt);
    (void)av_bsf_receive_packet(avbsfContext_.get(), &pkt);
}

Status FFmpegDemuxerPlugin::ReadFrame(Buffer& info, int32_t timeOutMs)
{
    (void)timeOutMs;
    AVPacket pkt;
    int res = 0;
    do {
        res = av_read_frame(formatContext_.get(), &pkt);
    } while (res >= 0 && !selectedTrackIds_.empty() && !IsSelectedTrack(pkt.stream_index));
    Status result = Status::ERROR_UNKNOWN;
    if (res == 0 && ConvertAVPacketToFrameInfo(*(formatContext_->streams[pkt.stream_index]), pkt, info)) {
        result = Status::OK;
    } else {
        MEDIA_LOG_W("ReadFrame failed, rtv = " PUBLIC_LOG_S, AVStrError(res).c_str());
    }
    av_packet_unref(&pkt);
    return (res != AVERROR_EOF) ? result : Status::END_OF_STREAM;
}

/**
 * SeekTo seek operation
 * @param trackId  -1 for unspecified, >= 0 for specific trackid
 * @param seekTime
 * @param mode
 * @param realSeekTime
 * @return operation result.
 */
Status FFmpegDemuxerPlugin::SeekTo(int32_t trackId, int64_t seekTime, SeekMode mode, int64_t& realSeekTime)
{
    if (trackId == -1) {
        trackId = av_find_default_stream_index(formatContext_.get());
    }
    if (trackId < 0 || trackId >= static_cast<int32_t>(formatContext_->nb_streams)) {
        MEDIA_LOG_E("SeekTo called with invalid trackid = " PUBLIC_LOG_D32 ", nb_streams = " PUBLIC_LOG_D32 ".",
                    trackId, formatContext_->nb_streams);
        return Status::ERROR_INVALID_PARAMETER;
    }
    FALSE_RETURN_V_MSG_E(seekModeToFfmpegSeekFlags.count(mode), Status::ERROR_INVALID_PARAMETER,
                         "Unsupported seek mode: " PUBLIC_LOG_U32, static_cast<uint32_t>(mode));
    int flags = seekModeToFfmpegSeekFlags.at(mode);
    auto avStream = formatContext_->streams[trackId];
    int64_t ffTime = ConvertTimeToFFmpeg(seekTime, avStream->time_base);
    if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (Plugin::HstTime2Ms(ConvertTimeFromFFmpeg(avStream->duration, avStream->time_base) - seekTime) <= 100 // 100
            && mode == SeekMode::SEEK_NEXT_SYNC) {
            flags = seekModeToFfmpegSeekFlags.at(SeekMode::SEEK_PREVIOUS_SYNC);
        }
        int keyFrameIdx = av_index_search_timestamp(avStream, ffTime, flags);
        MEDIA_LOG_I("SeekTo " PUBLIC_LOG_D64 "ns, ffTime: " PUBLIC_LOG_D64 ", key frame index: "
                    PUBLIC_LOG_D32, realSeekTime, ffTime, keyFrameIdx);
        if (keyFrameIdx < 0) {
            keyFrameIdx = av_index_search_timestamp(avStream, ffTime,
                                                    seekModeToFfmpegSeekFlags.at(SeekMode::SEEK_PREVIOUS_SYNC));
        }
        if (keyFrameIdx >= 0) {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(58, 78, 0)
            ffTime = avformat_index_get_entry(avStream, keyFrameIdx)->timestamp;
#elif LIBAVFORMAT_VERSION_INT == AV_VERSION_INT(58, 76, 100)
            ffTime = avStream->index_entries[keyFrameIdx].timestamp;
#elif LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(58, 64, 100)
            ffTime = avStream->internal->index_entries[keyFrameIdx].timestamp;
#else
            ffTime = avStream->index_entries[keyFrameIdx].timestamp;
#endif
            if (ffTime < 0) {
                ffTime = 0;
            }
        }
    }
    realSeekTime = ConvertTimeFromFFmpeg(ffTime, avStream->time_base);
    MEDIA_LOG_I("SeekTo " PUBLIC_LOG_U64 " / " PUBLIC_LOG_D64, ffTime, realSeekTime);
    auto rtv = av_seek_frame(formatContext_.get(), trackId, ffTime, flags);
    MEDIA_LOG_I("Av seek finished, return value : " PUBLIC_LOG_D32, rtv);
    return (rtv >= 0) ? Status::OK : Status::ERROR_UNKNOWN;
}

void FFmpegDemuxerPlugin::InitAVFormatContext()
{
    AVFormatContext* formatContext = avformat_alloc_context();
    if (formatContext == nullptr) {
        MEDIA_LOG_E("InitAVFormatContext  failed.");
        return;
    }
    formatContext->pb = AllocAVIOContext(AVIO_FLAG_READ);
    formatContext->flags = static_cast<uint32_t>(formatContext->flags) | static_cast<uint32_t>(AVFMT_FLAG_CUSTOM_IO);
    int ret = avformat_open_input(&formatContext, nullptr, pluginImpl_.get(), nullptr);
    if (ret == 0) {
        formatContext_ = std::shared_ptr<AVFormatContext>(formatContext, [](AVFormatContext* ptr) {
            if (ptr) {
                auto ctx = ptr->pb;
                if (ctx) {
                    av_freep(&ctx->buffer);
                    av_free(ctx);
                }
                avformat_close_input(&ptr);
            }
        });
    } else {
        MEDIA_LOG_E("avformat_open_input using plugin " PUBLIC_LOG_S " failed with return = " PUBLIC_LOG_S,
                    pluginImpl_->name, AVStrError(ret).c_str());
    }
}

std::shared_ptr<AVCodecContext> FFmpegDemuxerPlugin::InitCodecContext(const AVStream& avStream)
{
    auto codecContext = std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(nullptr), [](AVCodecContext* p) {
        if (p) {
            avcodec_free_context(&p);
        }
    });
    if (codecContext == nullptr) {
        MEDIA_LOG_E("cannot create ffmpeg codecContext");
        return nullptr;
    }
    int ret = avcodec_parameters_to_context(codecContext.get(), avStream.codecpar);
    if (ret < 0) {
        MEDIA_LOG_E("avcodec_parameters_to_context failed with return = " PUBLIC_LOG_S, AVStrError(ret).c_str());
        return nullptr;
    }
    codecContext->workaround_bugs = static_cast<uint32_t>(codecContext->workaround_bugs) | FF_BUG_AUTODETECT;
    codecContext->err_recognition = 1;
    return codecContext;
}

AVIOContext* FFmpegDemuxerPlugin::AllocAVIOContext(int flags)
{
    constexpr int bufferSize = 4096;
    auto buffer = static_cast<unsigned char*>(av_malloc(bufferSize));
    if (buffer == nullptr) {
        MEDIA_LOG_E("AllocAVIOContext failed to av_malloc...");
        return nullptr;
    }
    AVIOContext* avioContext = avio_alloc_context(buffer, bufferSize, flags, static_cast<void*>(&ioContext_),
                                                  AVReadPacket, AVWritePacket, AVSeek);
    if (avioContext == nullptr) {
        MEDIA_LOG_E("AllocAVIOContext failed to avio_alloc_context...");
        av_free(buffer);
        return nullptr;
    }
    MEDIA_LOG_D("seekable_ is " PUBLIC_LOG_D32, static_cast<int32_t>(seekable_));
    avioContext->seekable = (seekable_ == Seekable::SEEKABLE) ? AVIO_SEEKABLE_NORMAL : 0;
    if (!(static_cast<uint32_t>(flags) & static_cast<uint32_t>(AVIO_FLAG_WRITE))) {
        avioContext->buf_ptr = avioContext->buf_end;
        avioContext->write_flag = 0;
    }
    return avioContext;
}

bool FFmpegDemuxerPlugin::IsSelectedTrack(int32_t trackId)
{
    return std::any_of(selectedTrackIds_.begin(), selectedTrackIds_.end(),
                       [trackId](int32_t id) { return id == trackId; });
}

void FFmpegDemuxerPlugin::SaveFileInfoToMetaInfo(Meta& meta)
{
    meta.Clear();
    AVDictionaryEntry* tag = nullptr;
    while ((tag = av_dict_get(formatContext_->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        InsertMediaTag(meta, tag);
    }
    int64_t nanoSec = formatContext_->duration * (HST_SECOND / AV_TIME_BASE);
    meta.Set<Tag::MEDIA_DURATION>(nanoSec);
}

bool FFmpegDemuxerPlugin::ParseMediaData()
{
    auto formatContext = formatContext_.get();
    // retrieve stream information
    auto ret = avformat_find_stream_info(formatContext, nullptr);
    FALSE_RETURN_V_MSG_E(ret >= 0, false, "avformat_find_stream_info fail : " PUBLIC_LOG_S, AVStrError(ret).c_str());
    av_dump_format(formatContext, 0, nullptr, false);

    CppExt::make_unique<MediaInfo>().swap(mediaInfo_);
    size_t streamCnt = formatContext_->nb_streams;
    mediaInfo_->general.Clear();
    mediaInfo_->tracks.reserve(streamCnt);
    for (size_t i = 0; i < streamCnt; ++i) {
        auto& avStream = *formatContext_->streams[i];
        auto codecContext = InitCodecContext(avStream);
        if (!codecContext) {
            continue;
        }
        Meta track;
        if (avStream.codecpar->codec_type == AVMEDIA_TYPE_VIDEO
            && avStream.codecpar->codec_id != AV_CODEC_ID_RAWVIDEO) {
            if (!codecContext->width ||!codecContext->height) {
                continue;
            }
            if (avStream.codecpar->codec_id == AV_CODEC_ID_H264) {
                track.Set<Tag::VIDEO_BIT_STREAM_FORMAT>(
                    std::vector<VideoBitStreamFormat>{VideoBitStreamFormat::AVC1, VideoBitStreamFormat::ANNEXB});
            } else if (avStream.codecpar->codec_id == AV_CODEC_ID_H265) {
                track.Set<Tag::VIDEO_BIT_STREAM_FORMAT>(
                    std::vector<VideoBitStreamFormat>{VideoBitStreamFormat::HEVC, VideoBitStreamFormat::ANNEXB});
            }
        }
        ConvertAVStreamToMetaInfo(avStream, formatContext_, codecContext, track);
        if (g_MediaTypeMap.find(avStream.codecpar->codec_type) != g_MediaTypeMap.end()) {
            track.Set<Tag::MEDIA_TYPE>(g_MediaTypeMap[avStream.codecpar->codec_type]);
        } else {
            MEDIA_LOG_E("media type not found!");
        }
        mediaInfo_->tracks.push_back(std::move(track));
    }
    SaveFileInfoToMetaInfo(mediaInfo_->general);
    return true;
}

// ffmpeg provide buf, we write data
int FFmpegDemuxerPlugin::AVReadPacket(void* opaque, uint8_t* buf, int bufSize) // NOLINT
{
    int rtv = -1;
    auto ioContext = static_cast<IOContext*>(opaque);
    if (ioContext && ioContext->dataSource) {
        auto buffer = std::make_shared<Buffer>();
        auto bufData = buffer->WrapMemory(buf, bufSize, 0);
        auto result = ioContext->dataSource->ReadAt(ioContext->offset, buffer, static_cast<size_t>(bufSize));
        MEDIA_LOG_DD("AVReadPacket read data size = " PUBLIC_LOG_D32, static_cast<int>(bufData->GetSize()));
        if (result == Status::OK) {
            auto bufferMem = buffer->GetMemory();
            if (bufferMem == nullptr) {
                MEDIA_LOG_E("AVReadPacket buffer GetMemory nullptr");
                return -1;
            }
            ioContext->offset += static_cast<int64_t>(bufferMem->GetSize());
            rtv = static_cast<int>(bufferMem->GetSize());
        } else if (result == Status::END_OF_STREAM) {
            ioContext->eos = true;
            rtv = AVERROR_EOF;
        } else {
            MEDIA_LOG_E("AVReadPacket failed with rtv = " PUBLIC_LOG_D32, static_cast<int>(result));
        }
    }
    return rtv;
}

/**
 * write packet unimplemented.
 * @return 0
 */
int FFmpegDemuxerPlugin::AVWritePacket(void* opaque, uint8_t* buf, int bufSize) // NOLINT: intentionally using void*
{
    (void)opaque;
    (void)buf;
    (void)bufSize;
    return 0;
}

int64_t FFmpegDemuxerPlugin::AVSeek(void* opaque, int64_t offset, int whence) // NOLINT: void*
{
    auto ioContext = static_cast<IOContext*>(opaque);
    uint64_t newPos = 0;
    switch (whence) {
        case SEEK_SET:
            newPos = static_cast<uint64_t>(offset);
            ioContext->offset = newPos;
            MEDIA_LOG_DD("AVSeek whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64 ", newPos = " PUBLIC_LOG
                         PRIu64, whence, offset, newPos);
            break;
        case SEEK_CUR:
            newPos = ioContext->offset + offset;
            MEDIA_LOG_DD("AVSeek whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64 ", newPos = " PUBLIC_LOG
                         PRIu64, whence, offset, newPos);
            break;
        case SEEK_END:
        case AVSEEK_SIZE: {
            uint64_t mediaDataSize = 0;
            if (ioContext->dataSource->GetSize(mediaDataSize) == Status::OK) {
                newPos = mediaDataSize + offset;
                MEDIA_LOG_I("AVSeek seek end whence: " PUBLIC_LOG_D32 ", pos = " PUBLIC_LOG_D64,
                            whence, offset);
            }
            break;
        }
        default:
            MEDIA_LOG_E("AVSeek unexpected whence: " PUBLIC_LOG_D32, whence);
            break;
    }
    if (whence != AVSEEK_SIZE) {
        ioContext->offset = newPos;
    }
    MEDIA_LOG_DD("current offset: " PUBLIC_LOG_D64 ", new pos: " PUBLIC_LOG_U64,
                 ioContext->offset, newPos);
    return newPos;
}

namespace {
int Sniff(const std::string& pluginName, std::shared_ptr<DataSource> dataSource)
{
    if (pluginName.empty() || !dataSource) {
        MEDIA_LOG_E("Sniff failed due to empty plugin name or dataSource invalid.");
        return 0;
    }
    auto plugin = g_pluginInputFormat[pluginName];
    if (!plugin || !plugin->read_probe) {
        MEDIA_LOG_DD("Sniff failed due to invalid plugin for " PUBLIC_LOG_S ".", pluginName.c_str());
        return 0;
    }
    size_t bufferSize = 4096;
    uint64_t fileSize = 0;
    if (dataSource->GetSize(fileSize) == Status::OK) {
        bufferSize = (bufferSize < fileSize) ? bufferSize : fileSize;
    }
    std::vector<uint8_t> buff(bufferSize);
    auto bufferInfo = std::make_shared<Buffer>();
    auto bufData = bufferInfo->WrapMemory(buff.data(), bufferSize, bufferSize);
    int confidence = 0;
    if (bufData && dataSource->ReadAt(0, bufferInfo, bufferSize) == Status::OK) {
        auto bufferInfoMem = bufferInfo->GetMemory();
        if (bufferInfoMem == nullptr) {
            MEDIA_LOG_E("Sniff failed due to bufferInfo GetMemory nullptr");
            return 0;
        }
        AVProbeData probeData{"", buff.data(), static_cast<int>(bufferInfoMem->GetSize()), ""};
        confidence = plugin->read_probe(&probeData);
    }
    if (confidence > 0) {
        MEDIA_LOG_I("Sniff: plugin pluginName = " PUBLIC_LOG_S ", probability = " PUBLIC_LOG_D32 " / 100 ...",
                    plugin->name, confidence);
    }
    return confidence;
}

bool IsInputFormatSupported(const char* name)
{
    if (!strcmp(name, "audio_device") || !strncmp(name, "image", 5) ||                 // 5
        !strcmp(name, "mjpeg") || !strcmp(name, "redir") || !strncmp(name, "u8", 2) || // 2
        !strncmp(name, "u16", 3) || !strncmp(name, "u24", 3) ||                        // 3
        !strncmp(name, "u32", 3) ||                                                    // 3
        !strncmp(name, "s8", 2) || !strncmp(name, "s16", 3) ||                         // 2 3
        !strncmp(name, "s24", 3) ||                                                    // 3
        !strncmp(name, "s32", 3) || !strncmp(name, "f32", 3) ||                        // 3
        !strncmp(name, "f64", 3) ||                                                    // 3
        !strcmp(name, "mulaw") || !strcmp(name, "alaw")) {
        return false;
    }

    /* no network demuxers */
    if (!strcmp(name, "sdp") || !strcmp(name, "rtsp") || !strcmp(name, "applehttp")) {
        return false;
    }
    return true;
}

Status RegisterPlugins(const std::shared_ptr<Register>& reg)
{
    MEDIA_LOG_D("RegisterPlugins called.");
    if (!reg) {
        MEDIA_LOG_E("RegisterPlugins failed due to null pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }
    const AVInputFormat* plugin = nullptr;
    void* i = nullptr;
    while ((plugin = av_demuxer_iterate(&i))) {
        MEDIA_LOG_DD("Attempting to handle libav demuxer plugin " PUBLIC_LOG_S " [" PUBLIC_LOG_S "]",
                     plugin->name, plugin->long_name);
        /* no emulators */
        if (plugin->long_name != nullptr) {
            if (!strncmp(plugin->long_name, "pcm ", 4)) { // 4
                continue;
            }
        }

        if (!IsInputFormatSupported(plugin->name)) {
            continue;
        }

        std::string pluginName = "avdemux_" + std::string(plugin->name);
        ReplaceDelimiter(".,|-<> ", '_', pluginName);

        DemuxerPluginDef regInfo;
        regInfo.name = pluginName;
        regInfo.description = "adapter for ffmpeg demuxer plugin";
        regInfo.rank = 100; // 100
        SplitString(plugin->extensions, ',').swap(regInfo.extensions);
        g_pluginInputFormat[pluginName] =
            std::shared_ptr<AVInputFormat>(const_cast<AVInputFormat*>(plugin), [](void*) {});
        regInfo.creator = [](const std::string& name) -> std::shared_ptr<DemuxerPlugin> {
            return std::make_shared<FFmpegDemuxerPlugin>(name);
        };
        regInfo.sniffer = Sniff;
        auto rtv = reg->AddPlugin(regInfo);
        if (rtv != Status::OK) {
            MEDIA_LOG_E("RegisterPlugins AddPlugin failed with return " PUBLIC_LOG_D32, static_cast<int>(rtv));
        }
    }
    return Status::OK;
}

#if FFMPEG_LOG_DEBUG_ENABLE
#ifdef MEDIA_OHOS
void FfmpegLogPrint(void* avcl, int level, const char* fmt, va_list vl)
{
    (void)avcl;
    switch (level) {
        case AV_LOG_INFO:
        case AV_LOG_DEBUG:
            HILOG_DEBUG(LOG_CORE, fmt, vl);
            break;
        case AV_LOG_WARNING:
            HILOG_WARN(LOG_CORE, fmt, vl);
            break;
        case AV_LOG_ERROR:
            HILOG_ERROR(LOG_CORE, fmt, vl);
            break;
        case AV_LOG_FATAL:
            HILOG_FATAL(LOG_CORE, fmt, vl);
            break;
    }
}
#else
void FfmpegLogPrint(void* avcl, int level, const char* fmt, va_list vl)
{
    (void)avcl;
    char buf[500] = {0};  // 500
    (void)vsnprintf_s(buf, sizeof(buf), fmt, vl);
    switch (level) {
        case AV_LOG_WARNING:
            MEDIA_LOG_W("Ffmpeg Message %d %s", level, buf);
            break;
        case AV_LOG_ERROR:
        case AV_LOG_FATAL:
            MEDIA_LOG_E("Ffmpeg Message %d %s", level, buf);
            break;
        case AV_LOG_INFO:
        case AV_LOG_DEBUG:
            MEDIA_LOG_DD("Ffmpeg Message %d %s", level, buf);
            break;
        default:
            break;
    }
}
#endif
#endif

static void FfmpegLogInit()
{
#if FFMPEG_LOG_DEBUG_ENABLE
    av_log_set_callback(FfmpegLogPrint);
#endif
}
} // namespace

PLUGIN_DEFINITION(FFmpegDemuxer, LicenseType::LGPL, RegisterPlugins, [] { g_pluginInputFormat.clear(); });
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
