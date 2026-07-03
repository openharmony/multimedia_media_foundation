/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "media_monitor_dt_test.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstring>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "audio_dump_buffer.h"
#include "audio_info.h"
#include "audio_memo.h"
#include "audio_stream_info.h"
#include "dump_buffer_wrap.h"
#include "event_aggregate.h"
#include "event_bean.h"
#include "ffmpeg_api_wrap.h"
#include "media_audio_encoder.h"
#include "media_event_base_writer.h"
#include "media_monitor_manager.h"
#include "media_monitor_policy.h"
#include "media_monitor_service.h"
#include "media_monitor_wrapper.h"
#include "monitor_error.h"
#include "monitor_utils.h"
#include "securec.h"
#include "string_converter.h"

extern "C" OHOS::Media::MediaMonitor::MediaMonitorErr GetBundleInfoFromUid(int32_t appUid,
    OHOS::Media::MediaMonitor::BundleInfo *bundleInfoRes);

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace Media {
namespace MediaMonitor {
namespace {

constexpr int32_t TEST_CLIENT_UID = 1001;
constexpr uint64_t TEST_DURATION = 31;

// Stub return values
constexpr int32_t STUB_DUMP_CAPACITY = 64;
constexpr int32_t STUB_DUMP_SIZE = 32;
constexpr int32_t STUB_CODEC_OPEN_RET = 24;
constexpr int32_t STUB_FORMAT_ALLOC_RET = 31;
constexpr int32_t STUB_CODEC_PARAM_RET = 33;
constexpr int32_t STUB_IO_OPEN_RET = 34;
constexpr int32_t STUB_IO_CLOSE_RET = 35;
constexpr int32_t STUB_WRITE_HEADER_RET = 36;
constexpr int32_t STUB_WRITE_TRAILER_RET = 37;
constexpr int32_t STUB_FRAME_GET_BUF_RET = 43;
constexpr int32_t STUB_SEND_FRAME_RET = 44;
constexpr int32_t STUB_WRITE_FRAME_RET = 45;
constexpr int32_t STUB_FLUSH_RET = 46;
constexpr int32_t STUB_BYTES_PER_SAMPLE = 2;
constexpr int32_t STUB_CHANNEL_COUNT = 2;

// Bean test data values
constexpr int32_t TEST_PID = 100;
constexpr int32_t TEST_APP_PID = 200;
constexpr int32_t TEST_SAMPLE_RATE = 48000;
constexpr uint64_t TEST_TRANSACTION_ID = 123;
constexpr uint64_t TEST_TIME_STAMP = 123;
constexpr int32_t TEST_SESSION_PID = 10;
constexpr int32_t TEST_BACK_TASK_PID = 11;
constexpr int32_t TEST_STREAM_TYPE_PLAYBACK = 1;
constexpr int32_t TEST_STREAM_TYPE_CAPTURE = 2;
constexpr int32_t TEST_DEVICE_TYPE_PLAYBACK = 2;
constexpr int32_t TEST_DEVICE_TYPE_USAGE = 3;
constexpr int32_t TEST_BT_TYPE = 4;
constexpr int32_t TEST_PIPE_TYPE_CHANGE = 3;
constexpr int32_t TEST_PIPE_TYPE_USAGE = 5;
constexpr int32_t TEST_CHANNEL_LAYOUT = 3;
constexpr int32_t TEST_VOLUME_LEVEL = 4;
constexpr int32_t TEST_ERROR_CODE = 7;
constexpr int32_t TEST_LEVEL = 6;
constexpr int32_t TEST_RING_MODE = 2;
constexpr int32_t TEST_STREAM_TYPE_TONE = 3;
constexpr int32_t TEST_DM_DEVICE_TYPE = 2;
constexpr int32_t TEST_PARAM_VALUE = 5;
constexpr int32_t TEST_ACTIVE_PID = 2;
constexpr int32_t TEST_INCOMING_PID = 1;
constexpr int32_t TEST_MUTE_PLAY_DURATION = 2;

DumpBuffer *DumpBufferNewStub()
{
    return reinterpret_cast<DumpBuffer *>(0x10);
}

DumpBuffer *DumpBufferCreateStub(int32_t capacity)
{
    return capacity > 0 ? reinterpret_cast<DumpBuffer *>(0x11) : nullptr;
}

void DumpBufferDestroyStub(DumpBuffer *buffer)
{
    (void)buffer;
}

uint8_t *DumpBufferGetAddrStub(DumpBuffer *buffer)
{
    static uint8_t data = 0;
    return buffer == nullptr ? nullptr : &data;
}

int32_t DumpBufferGetCapacityStub(DumpBuffer *buffer)
{
    return buffer == nullptr ? -1 : STUB_DUMP_CAPACITY;
}

int32_t DumpBufferGetSizeStub(DumpBuffer *buffer)
{
    return buffer == nullptr ? -1 : STUB_DUMP_SIZE;
}

bool DumpBufferSetSizeStub(DumpBuffer *buffer, int32_t size)
{
    return buffer != nullptr && size >= 0;
}

uint64_t DumpBufferGetUniqueIdStub(DumpBuffer *buffer)
{
    return buffer == nullptr ? 0 : 0x1234;
}

int32_t DumpBufferWriteStub(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize)
{
    return buffer == nullptr || in == nullptr ? -1 : writeSize;
}

bool DumpBufferParcelStub(DumpBuffer *buffer, void *parcel)
{
    return buffer != nullptr && parcel != nullptr;
}

AVCodec *CodecFindEncoderStub(enum AVCodecID id)
{
    return id == AV_CODEC_ID_NONE ? nullptr : reinterpret_cast<AVCodec *>(0x21);
}

AVCodecContext *CodecAllocContextStub(const AVCodec *codec)
{
    return codec == nullptr ? nullptr : reinterpret_cast<AVCodecContext *>(0x22);
}

int CodecOpenStub(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)
{
    (void)options;
    return avctx == nullptr || codec == nullptr ? -1 : STUB_CODEC_OPEN_RET;
}

int FormatAllocOutputContextStub(AVFormatContext **ctx, const AVOutputFormat *oformat,
    const char *formatName, const char *filename)
{
    (void)oformat;
    (void)formatName;
    (void)filename;
    if (ctx != nullptr) {
        *ctx = reinterpret_cast<AVFormatContext *>(0x31);
    }
    return STUB_FORMAT_ALLOC_RET;
}

AVStream *FormatNewStreamStub(AVFormatContext *s, const AVCodec *c)
{
    (void)c;
    return s == nullptr ? nullptr : reinterpret_cast<AVStream *>(0x32);
}

int CodecParamFromContextStub(AVCodecParameters *par, const AVCodecContext *codec)
{
    return par == nullptr || codec == nullptr ? -1 : STUB_CODEC_PARAM_RET;
}

int IoOpenStub(AVIOContext **s, const char *url, int flags, const AVIOInterruptCB *intCb, AVDictionary **options)
{
    (void)flags;
    (void)intCb;
    (void)options;
    if (s != nullptr && url != nullptr) {
        *s = reinterpret_cast<AVIOContext *>(0x34);
        return STUB_IO_OPEN_RET;
    }
    return -1;
}

void IoFlushStub(AVIOContext *s)
{
    (void)s;
}

int IoCloseStub(AVIOContext *s)
{
    return s == nullptr ? -1 : STUB_IO_CLOSE_RET;
}

int FormatWriteHeaderStub(AVFormatContext *s, AVDictionary **options)
{
    (void)options;
    return s == nullptr ? -1 : STUB_WRITE_HEADER_RET;
}

int FormatWriteTrailerStub(AVFormatContext *s)
{
    return s == nullptr ? -1 : STUB_WRITE_TRAILER_RET;
}

AVPacket *PacketAllocStub()
{
    return reinterpret_cast<AVPacket *>(0x41);
}

AVFrame *FrameAllocStub()
{
    return reinterpret_cast<AVFrame *>(0x42);
}

int FrameGetBufferStub(AVFrame *frame, int align)
{
    (void)align;
    return frame == nullptr ? -1 : STUB_FRAME_GET_BUF_RET;
}

int CodecSendFrameStub(AVCodecContext *avctx, const AVFrame *frame)
{
    (void)frame;
    return avctx == nullptr ? -1 : STUB_SEND_FRAME_RET;
}

int CodecRecvPacketStub(AVCodecContext *avctx, AVPacket *avpkt)
{
    return avctx == nullptr || avpkt == nullptr ? -1 : AVERROR_EOF;
}

int FormatWriteFrameStub(AVFormatContext *s, AVPacket *pkt)
{
    return s == nullptr || pkt == nullptr ? -1 : STUB_WRITE_FRAME_RET;
}

int FormatFlushStub(AVFormatContext *s)
{
    return s == nullptr ? -1 : STUB_FLUSH_RET;
}

void FormatFreeContextStub(AVFormatContext *s)
{
    (void)s;
}

void CodecFreeContextStub(AVCodecContext **avctx)
{
    if (avctx != nullptr) {
        *avctx = nullptr;
    }
}

void FrameFreeStub(AVFrame **frame)
{
    if (frame != nullptr) {
        *frame = nullptr;
    }
}

void PacketFreeStub(AVPacket **pkt)
{
    if (pkt != nullptr) {
        *pkt = nullptr;
    }
}

void PacketUnrefStub(AVPacket *pkt)
{
    (void)pkt;
}

int64_t ChannelLayoutFromMaskStub(AVChannelLayout *channelLayout, uint64_t mask)
{
    if (channelLayout != nullptr) {
        channelLayout->u.mask = mask;
        channelLayout->nb_channels = STUB_CHANNEL_COUNT;
    }
    return 0;
}

int64_t ChannelLayoutDefaultStub(AVChannelLayout *channelLayout, int nbChannels)
{
    if (channelLayout != nullptr) {
        channelLayout->nb_channels = nbChannels;
    }
    return 0;
}

int64_t ChannelLayoutCopyStub(AVChannelLayout *dst, const AVChannelLayout *src)
{
    if (dst == nullptr || src == nullptr) {
        return -1;
    }
    *dst = *src;
    return 0;
}

int BytesPerSampleStub(enum AVSampleFormat sampleFmt)
{
    return sampleFmt == AV_SAMPLE_FMT_NONE ? -1 : STUB_BYTES_PER_SAMPLE;
}

int SampleFmtIsPlanarStub(enum AVSampleFormat sampleFmt)
{
    return sampleFmt == AV_SAMPLE_FMT_FLTP ? 1 : 0;
}

struct SwrContext *SwrAllocStub()
{
    return reinterpret_cast<SwrContext *>(0x51);
}

// Parameter count must match FFmpeg swr_set_opts2 API signature; cannot be reduced.
int SwrSetOpts2Stub(struct SwrContext **ps, const AVChannelLayout *outChLayout, // NOLINT(readability-function-size)
    enum AVSampleFormat outSampleFmt, int outSampleRate, const AVChannelLayout *inChLayout,
    enum AVSampleFormat inSampleFmt, int inSampleRate, int logOffset, void *logCtx)
{
    (void)outChLayout;
    (void)outSampleFmt;
    (void)outSampleRate;
    (void)inChLayout;
    (void)inSampleFmt;
    (void)inSampleRate;
    (void)logOffset;
    (void)logCtx;
    return ps == nullptr || *ps == nullptr ? -1 : 0;
}

int SwrInitStub(struct SwrContext *s)
{
    return s == nullptr ? -1 : 0;
}

void SwrFreeStub(struct SwrContext **s)
{
    if (s != nullptr) {
        *s = nullptr;
    }
}

int SwrConvertStub(struct SwrContext *s, uint8_t **out, int outCount, const uint8_t **in, int inCount)
{
    (void)out;
    (void)in;
    return s == nullptr ? -1 : std::min(outCount, inCount);
}

MediaMonitorErr GetBundleInfoSuccessStub(int32_t appUid, BundleInfo *bundleInfo)
{
    if (bundleInfo == nullptr) {
        return ERR_INVALID_PARAM;
    }
    bundleInfo->appName = "stub." + std::to_string(appUid);
    bundleInfo->versionCode = appUid;
    bundleInfo->subscribeKey = "key";
    bundleInfo->subscribeResult = SUCCESS;
    return SUCCESS;
}

std::shared_ptr<EventBean> MakeEvent(EventId eventId, EventType type = BEHAVIOR_EVENT)
{
    return std::make_shared<EventBean>(AUDIO, eventId, type);
}

void AddString(const std::shared_ptr<EventBean> &bean, const std::string &key, const std::string &value)
{
    ASSERT_NE(bean, nullptr);
    bean->Add(key, value);
}

void AddString(EventBean &bean, const std::string &key, const std::string &value)
{
    bean.Add(key, value);
}

std::shared_ptr<EventBean> MakeStreamChangeBean(int32_t state, int32_t isOutput, int32_t streamId = 1)
{
    auto bean = MakeEvent(STREAM_CHANGE, BEHAVIOR_EVENT);
    bean->Add("STATE", state);
    bean->Add("ISOUTPUT", isOutput);
    bean->Add("STREAMID", streamId);
    bean->Add("UID", TEST_CLIENT_UID);
    bean->Add("PID", TEST_PID);
    bean->Add("STREAM_TYPE", TEST_STREAM_TYPE_PLAYBACK);
    bean->Add("DEVICETYPE", TEST_DEVICE_TYPE_PLAYBACK);
    bean->Add("PIPE_TYPE", TEST_PIPE_TYPE_CHANGE);
    bean->Add("SAMPLE_RATE", TEST_SAMPLE_RATE);
    AddString(bean, "APP_NAME", "dt.app");
    bean->Add("EFFECT_CHAIN", 0);
    bean->Add("ENCODING_TYPE", 1);
    bean->Add("CHANNEL_LAYOUT", static_cast<uint64_t>(TEST_CHANNEL_LAYOUT));
    bean->Add("MUTED", 1);
    return bean;
}

std::shared_ptr<EventBean> MakeUsageBean(EventId eventId)
{
    auto bean = MakeEvent(eventId, FREQUENCY_AGGREGATION_EVENT);
    bean->Add("IS_PLAYBACK", 1);
    bean->Add("STREAMID", 1);
    bean->Add("UID", TEST_CLIENT_UID);
    bean->Add("PID", TEST_PID);
    bean->Add("STREAM_TYPE", TEST_STREAM_TYPE_CAPTURE);
    bean->Add("DEVICE_TYPE", TEST_DEVICE_TYPE_USAGE);
    bean->Add("BT_TYPE", TEST_BT_TYPE);
    bean->Add("PIPE_TYPE", TEST_PIPE_TYPE_USAGE);
    bean->Add("SAMPLE_RATE", TEST_SAMPLE_RATE);
    AddString(bean, "APP_NAME", "dt.app");
    bean->Add("EFFECT_CHAIN", 1);
    bean->Add("ENCODING_TYPE", 1);
    bean->Add("CHANNEL_LAYOUT", static_cast<uint64_t>(TEST_CHANNEL_LAYOUT));
    bean->Add("LEVEL", TEST_LEVEL);
    bean->Add("CLIENT_UID", TEST_CLIENT_UID);
    bean->Add("ERROR_CODE", TEST_ERROR_CODE);
    bean->Add("TIMES", 1);
    bean->Add("DURATION", TEST_DURATION);
    return bean;
}

void FillRichBeanDeviceAndSession(const std::shared_ptr<EventBean> &bean)
{
    bean->Add("HASMIC", 1);
    bean->Add("ISCONNECT", 1);
    bean->Add("DEVICETYPE", TEST_DEVICE_TYPE_PLAYBACK);
    bean->Add("ISOUTPUT", 1);
    bean->Add("APP_UID", TEST_CLIENT_UID);
    bean->Add("APP_PID", TEST_APP_PID);
    bean->Add("TRANSACTIONID", TEST_TRANSACTION_ID);
    bean->Add("STATE", AudioStandard::State::RUNNING);
    AddString(bean, "NETWORKID", "net");
    AddString(bean, "ADDRESS", "addr");
    AddString(bean, "DEVICE_NAME", "device");
    bean->Add("STATUS", 1);
    bean->Add("CATEGORY", 1);
    bean->Add("ENGINE_TYPE", 1);
    AddString(bean, "APP_BUNDLE_NAME", "bundle");
    bean->Add("STREAM_VOLUME", 1.0f);
    bean->Add("MUTE_STATE", 1);
    bean->Add("APP_BACKGROUND_STATE", 0);
    bean->Add("MUTE_PLAY_START_TIME", static_cast<uint64_t>(1));
    bean->Add("MUTE_PLAY_DURATION", TEST_MUTE_PLAY_DURATION);
    bean->Add("HDI_TYPE", 1);
    bean->Add("ERROR_CASE", 1);
    bean->Add("ERROR_MSG", 1);
    AddString(bean, "ERROR_DESCRIPTION", "error");
    bean->Add("REASON", 1);
    bean->Add("PERIOD_MS", 1);
    bean->Add("HDI_ADAPTER", 1);
    bean->Add("POSITION", 1);
    AddString(bean, "APP_NAMES", "apps");
    bean->Add("JANK_START_TIME", static_cast<uint64_t>(1));
    bean->Add("SERVICE_ID", 1);
    AddString(bean, "DUBIOUS_APP", "dubious");
    bean->Add("PIPE_TYPE_BEFORE_CHANGE", 1);
    bean->Add("PIPE_TYPE_AFTER_CHANGE", 2);
    bean->Add("DEVICE_TYPE_BEFORE_CHANGE", 1);
    bean->Add("DEVICE_TYPE_AFTER_CHANGE", 2);
    bean->Add("PRE_AUDIO_SCENE", 1);
    bean->Add("CUR_AUDIO_SCENE", 2);
}

void FillRichBeanRouteAndInterrupt(const std::shared_ptr<EventBean> &bean)
{
    AddString(bean, "DEVICE_LIST", "list");
    bean->Add("ROUTER_TYPE", 1);
    AddString(bean, "DB_TYPE", "db");
    bean->Add("CHANGE_REASON", 1);
    AddString(bean, "DEVICE_DESC", "desc");
    bean->Add("MIGRATE_DIRECTION", 1);
    bean->Add("ADD_REMOVE_OPERATION", 1);
    bean->Add("FILE_SIZE", static_cast<uint64_t>(1));
    bean->Add("RINGTONE_CATEGORY", 1);
    bean->Add("MEDIA_TYPE", 1);
    AddString(bean, "MIME_TYPE", "audio/pcm");
    bean->Add("TIMESTAMP", static_cast<uint64_t>(1));
    bean->Add("RESULT", 0);
    AddString(bean, "SUBSCRIBE_KEY", "key");
    bean->Add("SUBSCRIBE_RESULT", 0);
    bean->Add("SESSIONID", 1);
    bean->Add("STREAM_OR_SOURCE_TYPE", 1);
    bean->Add("START_TIME", static_cast<uint64_t>(1));
    bean->Add("UPLOAD_TIME", static_cast<uint64_t>(2));
    bean->Add("STANDBY_DURATION_S", 1);
    bean->Add("INCOMING_UID", TEST_CLIENT_UID);
    bean->Add("ACTIVE_UID", TEST_CLIENT_UID + 1);
    bean->Add("INCOMING_SOURCE", 1);
    bean->Add("INCOMING_PID", TEST_INCOMING_PID);
    AddString(bean, "INCOMING_PKG", "incoming");
    bean->Add("ACTIVE_SOURCE", 1);
    bean->Add("ACTIVE_PID", TEST_ACTIVE_PID);
    AddString(bean, "ACTIVE_PKG", "active");
    bean->Add("TYPE", 1);
    bean->Add("COUNT", 1);
    bean->Add("MSG_TYPE", 1);
    AddString(bean, "MSG_FUNC_NAME", "func");
    AddString(bean, "MSG_ERROR_DESCRIPTION", "message");
    bean->Add("SESSION_ID", 1);
    AddString(bean, "CURRENT_NAME", "cur");
    AddString(bean, "DES_NAME", "des");
    AddString(bean, "FUNC_NAME", "SetVolume");
    bean->Add("PARAM_VALUE", TEST_PARAM_VALUE);
    bean->Add("SYSTEMHAP_SET_FOCUSSTRATEGY", 1);
    AddString(bean, "ERROR_INFO", "info");
    bean->Add("INTERRUPT_HINTTYPE", 1);
    AddString(bean, "RENDERER_INFO", "renderer");
    AddString(bean, "SESSION_INFO", "session");
    bean->Add("WINDOW_STATE", 1);
    bean->Add("RENDERER_PLAY_TIMES", 1);
    AddString(bean, "CURR_APP_NAME", "curApp");
    AddString(bean, "CURR_RENDERER_INFO", "curRenderer");
    AddString(bean, "CURR_SESSION_INFO", "curSession");
    AddString(bean, "BUNDLENAME", "bundle");
    bean->Add("AUDIODIRECTION", 1);
    bean->Add("AUDIOSTREAM", 1);
    bean->Add("CALLFUNC", 1);
    bean->Add("ERROR_SCENE", 1);
    bean->Add("LOUD_VOLUME_TIMES", 1);
    bean->Add("SCENE_TYPE", LOUD_VOLUME_SCENE);
    AddString(bean, "MUTETYPE", "mute");
    AddString(bean, "ERROR_REASON", "reason");
    bean->Add("RENDER_DEVICE_TYPE", 1);
    bean->Add("CAPTURE_DEVICE_TYPE", 1);
    bean->Add("ERROR_SCOPE", 1);
    bean->Add("ERROR_TYPE", 1);
    bean->Add("FEATURE", 1);
}

void FillRichBeanSceneAndDistributed(const std::shared_ptr<EventBean> &bean)
{
    bean->Add("AUDIO_SCENE", AudioStandard::AUDIO_SCENE_PHONE_CALL);
    bean->Add("AUDIO_DEVICE_USAGE", MEDIA_OUTPUT_DEVICES);
    bean->Add("EXCLUSION_STATUS", 0);
    bean->Add("COLLABORATIVE_STATE", 1);
    bean->Add("PID", TEST_SESSION_PID);
    bean->Add("HAS_SESSION", 1);
    bean->Add("HAS_BACK_TASK", 1);
    bean->Add("IS_ADD", 1);
    AddString(bean, "SCENE_INFO", "scene");
    bean->Add("SERVICE_STATUS", 1);
    AddString(bean, "NETWORK_ID", "net");
    bean->Add("HDI_PIN", 1);
    AddString(bean, "ORIGINAL_INFO", "origin");
    bean->Add("DM_DEVICE_TYPE", TEST_DM_DEVICE_TYPE);
    bean->Add("ERROR_UID", TEST_CLIENT_UID);
}

std::shared_ptr<EventBean> MakeRichBean(EventId eventId, EventType type)
{
    auto bean = MakeUsageBean(eventId);
    bean->SetEventType(type);
    FillRichBeanDeviceAndSession(bean);
    FillRichBeanRouteAndInterrupt(bean);
    FillRichBeanSceneAndDistributed(bean);
    return bean;
}

void FillSystemToneBean(const std::shared_ptr<EventBean> &bean)
{
    bean->Add("TIME_STAMP", TEST_TIME_STAMP);
    bean->Add("SYSTEM_SOUND_TYPE", 1);
    bean->Add("CLIENT_UID", TEST_CLIENT_UID);
    bean->Add("DEVICE_TYPE", TEST_DEVICE_TYPE_PLAYBACK);
    bean->Add("ERROR_CODE", 0);
    AddString(bean, "ERROR_REASON", "none");
    bean->Add("MUTE_STATE", 0);
    bean->Add("MUTE_HAPTICS", 1);
    bean->Add("RING_MODE", TEST_RING_MODE);
    bean->Add("STREAM_TYPE", TEST_STREAM_TYPE_TONE);
    bean->Add("VIBRATION_STATE", 1);
    bean->Add("VOLUME_LEVEL", TEST_VOLUME_LEVEL);
}

void ResetPolicyState(MediaMonitorPolicy &policy)
{
    policy.startThread_.store(false);
    if (policy.timeThread_ != nullptr && policy.timeThread_->joinable()) {
        policy.timeThread_->detach();
    }
    policy.eventVector_.clear();
    policy.systemTonePlayEventVector_.clear();
    policy.karaokeFeatureEventVector_.clear();
    policy.cachedBundleInfoMap_.clear();
    policy.volumeApiInvokeRecordSet_.clear();
    while (!policy.volumeApiInvokeEventQueue_.empty()) {
        policy.volumeApiInvokeEventQueue_.pop();
    }
    policy.callSessionHapSet_.clear();
    policy.audioInterruptErrorSet_.clear();
    policy.suiteEngineNodeStatsMap_.clear();
    policy.loudVolumeTimes_.store(0);
    policy.systemTonePlayerCount_ = 0;
}

void ResetAudioMemoState(AudioMemo &memo)
{
    memo.preferredDevices_.clear();
    memo.excludedDevices_.clear();
    memo.addressToCollaborativeEnabledMap_.clear();
    memo.appSessionMap_.clear();
    memo.appBackTaskMap_.clear();
    memo.distributedSceneInfo_.clear();
    memo.distributedDeviceInfos_.clear();
    memo.dmDeviceInfos_.clear();
}

} // namespace

void MediaMonitorDtTest::SetUpTestCase(void) {}
void MediaMonitorDtTest::TearDownTestCase(void) {}

void MediaMonitorDtTest::SetUp(void)
{
    ResetPolicyState(MediaMonitorPolicy::GetMediaMonitorPolicy());
    ResetAudioMemoState(AudioMemo::GetAudioMemo());
}

void MediaMonitorDtTest::TearDown(void)
{
    ResetPolicyState(MediaMonitorPolicy::GetMediaMonitorPolicy());
    ResetAudioMemoState(AudioMemo::GetAudioMemo());
}

HWTEST(MediaMonitorDtTest, EventBean_MissingUpdateAndParcelBranch_001, TestSize.Level0)
{
    EventBean bean(AUDIO, LOAD_CONFIG_ERROR, FAULT_EVENT);
    EXPECT_EQ(bean.GetIntValue("missing"), -1);
    EXPECT_EQ(bean.GetStringValue("missing"), "UNKNOWN");
    EXPECT_EQ(bean.GetUint64Value("missing"), 0);
    EXPECT_FLOAT_EQ(bean.GetFloatValue("missing"), 0);

    bean.UpdateIntMap("missing", 1);
    bean.UpdateStringMap("missing", "value");
    bean.UpdateUint64Map("missing", 1);
    bean.UpdateFloatMap("missing", 1.0f);
    EXPECT_TRUE(bean.GetIntMap().empty());
    EXPECT_TRUE(bean.GetStringMap().empty());
    EXPECT_TRUE(bean.GetUint64Map().empty());
    EXPECT_TRUE(bean.GetFloatMap().empty());

    bean.Add("int", 1);
    AddString(bean, "string", "value");
    bean.Add("uint64", static_cast<uint64_t>(2));
    bean.Add("float", 3.0f);
    MessageParcel parcel;
    EXPECT_TRUE(bean.Marshalling(parcel));
    std::shared_ptr<EventBean> out(EventBean::Unmarshalling(parcel));
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetModuleId(), AUDIO);
    EXPECT_EQ(out->GetEventId(), LOAD_CONFIG_ERROR);
    EXPECT_EQ(out->GetEventType(), FAULT_EVENT);
    EXPECT_EQ(out->GetIntValue("int"), 1);
    EXPECT_EQ(out->GetStringValue("string"), "value");
    EXPECT_EQ(out->GetUint64Value("uint64"), 2);
    EXPECT_FLOAT_EQ(out->GetFloatValue("float"), 3.0f);
}

HWTEST(MediaMonitorDtTest, EventBean_ParcelMaxSizeBranch_001, TestSize.Level0)
{
    EventBean bean;
    for (int32_t i = 0; i < 1000; ++i) {
        bean.Add("key" + std::to_string(i), i);
    }
    MessageParcel parcel;
    EXPECT_FALSE(bean.Marshalling(parcel));

    MessageParcel badParcel;
    EXPECT_TRUE(badParcel.WriteInt32(AUDIO));
    EXPECT_TRUE(badParcel.WriteInt32(LOAD_CONFIG_ERROR));
    EXPECT_TRUE(badParcel.WriteInt32(FAULT_EVENT));
    EXPECT_TRUE(badParcel.WriteInt32(1000));
    EventBean readBean;
    readBean.ReadFromParcel(badParcel);
    EXPECT_TRUE(readBean.GetIntMap().empty());
}

HWTEST(MediaMonitorDtTest, StringConverter_ValidAndInvalid_001, TestSize.Level0)
{
    int32_t intValue = 0;
    uint32_t uintValue = 0;
    uint64_t uint64Value = 0;
    int64_t int64Value = 0;
    uint8_t uint8Value = 0;

    EXPECT_TRUE(OHOS::Media::StringConverter("123", intValue));
    EXPECT_EQ(intValue, 123);
    EXPECT_TRUE(OHOS::Media::StringConverter("456", uintValue));
    EXPECT_EQ(uintValue, 456U);
    EXPECT_TRUE(OHOS::Media::StringConverter("789", uint64Value));
    EXPECT_EQ(uint64Value, 789U);
    EXPECT_TRUE(OHOS::Media::StringConverter("-99", int64Value));
    EXPECT_EQ(int64Value, -99);
    EXPECT_TRUE(OHOS::Media::StringConverter("12", uint8Value));
    EXPECT_EQ(uint8Value, 12);
    EXPECT_FALSE(OHOS::Media::StringConverter("12x", intValue));
    EXPECT_FALSE(OHOS::Media::StringConverter("", uint64Value));
}

HWTEST(MediaMonitorDtTest, MonitorUtils_IsRealPathBranch_001, TestSize.Level0)
{
    EXPECT_TRUE(IsRealPath("/"));
    EXPECT_FALSE(IsRealPath("/path/not/exist/media_monitor_dt"));
    EXPECT_FALSE(IsRealPath(std::string(PATH_MAX + 1, 'a')));
    EXPECT_NE(TimeUtils::GetCurSec(), static_cast<uint64_t>(-1));
}

HWTEST(MediaMonitorDtTest, DumpBufferWrap_DefaultAndFuncPtrBranch_001, TestSize.Level0)
{
    DumpBufferWrap wrap;
    DumpBuffer *buffer = reinterpret_cast<DumpBuffer *>(0x100);
    uint8_t data = 1;
    int32_t parcel = 0;

    EXPECT_EQ(wrap.NewDumpBuffer(), nullptr);
    EXPECT_EQ(wrap.CreateDumpBuffer(10), nullptr);
    EXPECT_EQ(wrap.GetAddr(buffer), nullptr);
    EXPECT_EQ(wrap.GetCapacity(buffer), -1);
    EXPECT_EQ(wrap.GetSize(buffer), -1);
    EXPECT_EQ(wrap.Write(buffer, &data, 1), -1);
    EXPECT_EQ(wrap.GetUniqueId(buffer), static_cast<uint64_t>(-1));
    EXPECT_FALSE(wrap.SetSize(buffer, 1));
    EXPECT_FALSE(wrap.ReadFromParcel(buffer, &parcel));
    EXPECT_FALSE(wrap.WriteToParcel(buffer, &parcel));
    wrap.DestroyDumpBuffer(buffer);
    wrap.Close();

    wrap.newFunc = DumpBufferNewStub;
    wrap.createFunc = DumpBufferCreateStub;
    wrap.destroyFunc = DumpBufferDestroyStub;
    wrap.getAddrFunc = DumpBufferGetAddrStub;
    wrap.getCapacityFunc = DumpBufferGetCapacityStub;
    wrap.getSizeFunc = DumpBufferGetSizeStub;
    wrap.setSizeFunc = DumpBufferSetSizeStub;
    wrap.getIdFunc = DumpBufferGetUniqueIdStub;
    wrap.writeFunc = DumpBufferWriteStub;
    wrap.readFromParcel = DumpBufferParcelStub;
    wrap.writeToParcel = DumpBufferParcelStub;

    EXPECT_NE(wrap.NewDumpBuffer(), nullptr);
    EXPECT_NE(wrap.CreateDumpBuffer(10), nullptr);
    EXPECT_NE(wrap.GetAddr(buffer), nullptr);
    EXPECT_EQ(wrap.GetCapacity(buffer), STUB_DUMP_CAPACITY);
    EXPECT_EQ(wrap.GetSize(buffer), STUB_DUMP_SIZE);
    EXPECT_EQ(wrap.Write(buffer, &data, 1), 1);
    EXPECT_EQ(wrap.GetUniqueId(buffer), 0x1234);
    EXPECT_TRUE(wrap.SetSize(buffer, 1));
    EXPECT_TRUE(wrap.ReadFromParcel(buffer, &parcel));
    EXPECT_TRUE(wrap.WriteToParcel(buffer, &parcel));
    wrap.DestroyDumpBuffer(buffer);
}

HWTEST(MediaMonitorDtTest, FFmpegApiWrap_DefaultAndFuncPtrBranch_001, TestSize.Level0)
{
    FFmpegApiWrap wrap;
    AVFormatContext *formatContext = nullptr;
    AVIOContext *ioContext = nullptr;
    AVChannelLayout layout = {};
    SwrContext *swrContext = reinterpret_cast<SwrContext *>(0x51);

    EXPECT_EQ(wrap.CodecFindEncoder(AV_CODEC_ID_FLAC), nullptr);
    EXPECT_EQ(wrap.CodecAllocContext(nullptr), nullptr);
    EXPECT_EQ(wrap.CodecOpen(nullptr, nullptr, nullptr), -1);
    EXPECT_EQ(wrap.FormatAllocOutputContext(&formatContext, nullptr, nullptr, "a.flac"), -1);
    EXPECT_EQ(wrap.FormatNewStream(nullptr, nullptr), nullptr);
    EXPECT_EQ(wrap.CodecParamFromContext(nullptr, nullptr), -1);
    EXPECT_EQ(wrap.IoOpen(&ioContext, "a.flac", 0, nullptr, nullptr), -1);
    wrap.IoFlush(nullptr);
    EXPECT_EQ(wrap.IoClose(nullptr), -1);
    EXPECT_EQ(wrap.FormatWriteHeader(nullptr, nullptr), -1);
    EXPECT_EQ(wrap.FormatWriteTrailer(nullptr), -1);
    EXPECT_EQ(wrap.PacketAlloc(), nullptr);
    EXPECT_EQ(wrap.FrameAlloc(), nullptr);
    EXPECT_EQ(wrap.FrameGetBuffer(nullptr, 0), -1);
    EXPECT_EQ(wrap.CodecSendFrame(nullptr, nullptr), -1);
    EXPECT_EQ(wrap.CodecRecvPacket(nullptr, nullptr), -1);
    EXPECT_EQ(wrap.FormatWriteFrame(nullptr, nullptr), -1);
    EXPECT_EQ(wrap.FormatFlush(nullptr), -1);
    wrap.FormatFreeContext(nullptr);
    wrap.CodecFreeContext(nullptr);
    wrap.FrameFree(nullptr);
    wrap.PacketFree(nullptr);
    wrap.PacketUnref(nullptr);
    EXPECT_EQ(wrap.GetChannelLayoutFromMask(&layout, TEST_CHANNEL_LAYOUT), -1);
    EXPECT_EQ(wrap.GetChannelLayoutDefault(&layout, STUB_CHANNEL_COUNT), -1);
    EXPECT_EQ(wrap.GetChannelLayoutCopy(&layout, &layout), -1);
    EXPECT_EQ(wrap.GetBytesPerSample(AV_SAMPLE_FMT_S16), -1);
    EXPECT_EQ(wrap.SampleFmtIsPlannar(AV_SAMPLE_FMT_S16), -1);
    EXPECT_EQ(wrap.SwrAlloc(), nullptr);
    EXPECT_EQ(wrap.SwrSetOpts2(&swrContext, &layout, AV_SAMPLE_FMT_S16, TEST_SAMPLE_RATE,
        &layout, AV_SAMPLE_FMT_S16, TEST_SAMPLE_RATE, 0, nullptr), -1);
    EXPECT_EQ(wrap.SwrInit(swrContext), -1);
    wrap.SwrFree(&swrContext);
    EXPECT_EQ(wrap.SwrConvert(swrContext, nullptr, 0, nullptr, 0), -1);
}

void SetupFFmpegApiWrapFunctionPointers(FFmpegApiWrap &wrap)
{
    wrap.codecFindFunc = CodecFindEncoderStub;
    wrap.codecAllocFunc = CodecAllocContextStub;
    wrap.codecOpenFunc = CodecOpenStub;
    wrap.foramtAllocOutputFunc = FormatAllocOutputContextStub;
    wrap.formatNewStreamFunc = FormatNewStreamStub;
    wrap.codecParamFromContextFunc = CodecParamFromContextStub;
    wrap.ioOpenFunc = IoOpenStub;
    wrap.ioFlushFunc = IoFlushStub;
    wrap.ioCloseFunc = IoCloseStub;
    wrap.formatWriteHeaderFunc = FormatWriteHeaderStub;
    wrap.formatWriteTrailerFunc = FormatWriteTrailerStub;
    wrap.packetAllocFunc = PacketAllocStub;
    wrap.frameAllocFunc = FrameAllocStub;
    wrap.frameGetBufferFunc = FrameGetBufferStub;
    wrap.sendFrameFunc = CodecSendFrameStub;
    wrap.recvPacketFunc = CodecRecvPacketStub;
    wrap.writeFrameFunc = FormatWriteFrameStub;
    wrap.formatFlushFunc = FormatFlushStub;
    wrap.formatFreeContextFunc = FormatFreeContextStub;
    wrap.codecFreeContextFunc = CodecFreeContextStub;
    wrap.frameFreeFunc = FrameFreeStub;
    wrap.packetFreeFunc = PacketFreeStub;
    wrap.packetUnrefFunc = PacketUnrefStub;
    wrap.getChannelLayoutFromMaskFunc = ChannelLayoutFromMaskStub;
    wrap.getChannelLayoutDefaultFunc = ChannelLayoutDefaultStub;
    wrap.getChannelLayoutCopyFunc = ChannelLayoutCopyStub;
    wrap.getBytesPerSampleFunc = BytesPerSampleStub;
    wrap.sampleFmtIsPlannarFunc = SampleFmtIsPlanarStub;
    wrap.swrAllocFunc = SwrAllocStub;
    wrap.swrSetOpts2Func = SwrSetOpts2Stub;
    wrap.swrInitFunc = SwrInitStub;
    wrap.swrFreeFunc = SwrFreeStub;
    wrap.swrConvertFunc = SwrConvertStub;
}

void VerifyFFmpegApiWrapCodecAndFormat(FFmpegApiWrap &wrap, AVCodecContext *codecContext,
    AVCodec *codec, AVFormatContext *&formatContext, AVIOContext *&ioContext)
{
    EXPECT_NE(codec, nullptr);
    EXPECT_NE(codecContext, nullptr);
    EXPECT_EQ(wrap.CodecOpen(codecContext, codec, nullptr), STUB_CODEC_OPEN_RET);
    EXPECT_EQ(wrap.FormatAllocOutputContext(&formatContext, nullptr, nullptr, "a.flac"),
        STUB_FORMAT_ALLOC_RET);
    EXPECT_NE(formatContext, nullptr);
    EXPECT_NE(wrap.FormatNewStream(formatContext, codec), nullptr);
    EXPECT_EQ(wrap.CodecParamFromContext(reinterpret_cast<AVCodecParameters *>(0x33), codecContext),
        STUB_CODEC_PARAM_RET);
    EXPECT_EQ(wrap.IoOpen(&ioContext, "a.flac", 0, nullptr, nullptr), STUB_IO_OPEN_RET);
    wrap.IoFlush(ioContext);
    EXPECT_EQ(wrap.IoClose(ioContext), STUB_IO_CLOSE_RET);
    EXPECT_EQ(wrap.FormatWriteHeader(formatContext, nullptr), STUB_WRITE_HEADER_RET);
    EXPECT_EQ(wrap.FormatWriteTrailer(formatContext), STUB_WRITE_TRAILER_RET);
}

void VerifyFFmpegApiWrapFrameAndPacket(FFmpegApiWrap &wrap, AVFormatContext *formatContext,
    AVCodecContext *codecContext, AVPacket *&packet, AVFrame *&frame)
{
    EXPECT_NE(wrap.PacketAlloc(), nullptr);
    EXPECT_NE(wrap.FrameAlloc(), nullptr);
    EXPECT_EQ(wrap.FrameGetBuffer(frame, 0), STUB_FRAME_GET_BUF_RET);
    EXPECT_EQ(wrap.CodecSendFrame(codecContext, frame), STUB_SEND_FRAME_RET);
    EXPECT_EQ(wrap.CodecRecvPacket(codecContext, packet), AVERROR_EOF);
    EXPECT_EQ(wrap.FormatWriteFrame(formatContext, packet), STUB_WRITE_FRAME_RET);
    EXPECT_EQ(wrap.FormatFlush(formatContext), STUB_FLUSH_RET);
    wrap.FormatFreeContext(formatContext);
    wrap.CodecFreeContext(&codecContext);
    EXPECT_EQ(codecContext, nullptr);
    wrap.FrameFree(&frame);
    EXPECT_EQ(frame, nullptr);
    wrap.PacketFree(&packet);
    EXPECT_EQ(packet, nullptr);
    wrap.PacketUnref(packet);
}

void VerifyFFmpegApiWrapSwrAndLayout(FFmpegApiWrap &wrap, AVChannelLayout &layout, SwrContext *&swrContext)
{
    EXPECT_EQ(wrap.GetChannelLayoutFromMask(&layout, TEST_CHANNEL_LAYOUT), 0);
    EXPECT_EQ(wrap.GetChannelLayoutDefault(&layout, STUB_CHANNEL_COUNT), 0);
    EXPECT_EQ(wrap.GetChannelLayoutCopy(&layout, &layout), 0);
    EXPECT_EQ(wrap.GetBytesPerSample(AV_SAMPLE_FMT_S16), STUB_BYTES_PER_SAMPLE);
    EXPECT_EQ(wrap.SampleFmtIsPlannar(AV_SAMPLE_FMT_FLTP), 1);
    EXPECT_EQ(wrap.SwrSetOpts2(&swrContext, &layout, AV_SAMPLE_FMT_S16, TEST_SAMPLE_RATE,
        &layout, AV_SAMPLE_FMT_S16, TEST_SAMPLE_RATE, 0, nullptr), 0);
    EXPECT_EQ(wrap.SwrInit(swrContext), 0);
    EXPECT_EQ(wrap.SwrConvert(swrContext, nullptr, TEST_CHANNEL_LAYOUT, nullptr, STUB_BYTES_PER_SAMPLE),
        STUB_BYTES_PER_SAMPLE);
    wrap.SwrFree(&swrContext);
    EXPECT_EQ(swrContext, nullptr);
}

HWTEST(MediaMonitorDtTest, FFmpegApiWrap_FunctionPointersReturnStubValues_001, TestSize.Level0)
{
    FFmpegApiWrap wrap;
    SetupFFmpegApiWrapFunctionPointers(wrap);

    AVFormatContext *formatContext = nullptr;
    AVIOContext *ioContext = nullptr;
    AVChannelLayout layout = {};
    AVPacket *packet = reinterpret_cast<AVPacket *>(0x41);
    AVFrame *frame = reinterpret_cast<AVFrame *>(0x42);
    SwrContext *swrContext = wrap.SwrAlloc();
    AVCodec *codec = wrap.CodecFindEncoder(AV_CODEC_ID_FLAC);
    AVCodecContext *codecContext = wrap.CodecAllocContext(codec);

    VerifyFFmpegApiWrapCodecAndFormat(wrap, codecContext, codec, formatContext, ioContext);
    VerifyFFmpegApiWrapFrameAndPacket(wrap, formatContext, codecContext, packet, frame);
    VerifyFFmpegApiWrapSwrAndLayout(wrap, layout, swrContext);
}

HWTEST(MediaMonitorDtTest, MediaAudioEncoder_ParseAndBufferBranch_001, TestSize.Level0)
{
    MediaAudioEncoder encoder;
    AudioEncodeConfig config;
    std::string sampleRate = "48000";
    std::vector<std::string> supportList = {"44100", "48000"};
    EXPECT_TRUE(encoder.IsSupportAudioArgs(sampleRate, supportList));
    sampleRate = "12345";
    EXPECT_FALSE(encoder.IsSupportAudioArgs(sampleRate, supportList));

    EXPECT_EQ(encoder.ParseAudioArgs("dt_48000_2_1.pcm", config), SUCCESS);
    EXPECT_EQ(config.sampleRate, 48000U);
    EXPECT_EQ(config.channels, 2U);
    EXPECT_EQ(config.sampleFmt, SampleFormat::S16LE);
    EXPECT_EQ(encoder.ParseAudioArgs("dt_1.pcm", config), ERROR);
    EXPECT_EQ(encoder.ParseAudioArgs("dt_12345_2_1.pcm", config), ERROR);

    EXPECT_EQ(encoder.GetAudioConfig("/data/dt_48000_1_2.pcm", config), SUCCESS);
    EXPECT_EQ(encoder.fileName_, "/data/dt_48000_1_2.flac");
    EXPECT_EQ(encoder.srcSampleFormat_, SampleFormat::S24LE);

    EXPECT_EQ(encoder.PcmDataSize(), 4608U * 2U * 2U);
    EXPECT_FALSE(encoder.DeleteSrcFile("/path/not/exist/media_monitor_dt.pcm"));
}

HWTEST(MediaMonitorDtTest, MediaAudioEncoder_CopyAndPcmSizeBranch_001, TestSize.Level0)
{
    MediaAudioEncoder encoder;
    int32_t dst[2] = {0, 0};
    uint8_t src[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    encoder.CopyS24ToS32(nullptr, src, 2);
    encoder.CopyS24ToS32(dst, nullptr, 2);
    encoder.CopyS24ToS32(dst, src, 2);
    EXPECT_EQ(dst[0], 0x03020100);
    EXPECT_EQ(dst[1], 0x06050400);

    encoder.apiWrap_ = std::make_shared<FFmpegApiWrap>();
    encoder.apiWrap_->getBytesPerSampleFunc = BytesPerSampleStub;
    encoder.srcSampleFormat_ = SampleFormat::S16LE;
    AVCodecContext *context = new AVCodecContext();
    context->frame_size = 10;
    context->ch_layout.nb_channels = 2;
    encoder.audioCodecContext_ = std::shared_ptr<AVCodecContext>(context, [](AVCodecContext *ptr) {
        delete ptr;
    });
    EXPECT_EQ(encoder.PcmDataSize(), 40U);
    encoder.srcSampleFormat_ = SampleFormat::S24LE;
    EXPECT_EQ(encoder.PcmDataSize(), 60U);
}

HWTEST(MediaMonitorDtTest, MediaAudioEncoder_FrameAndWriteBranch_001, TestSize.Level0)
{
    MediaAudioEncoder encoder;
    uint8_t frameData[16] = {};
    uint8_t srcData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    AVFrame *frame = new AVFrame();
    frame->data[0] = frameData;
    frame->linesize[0] = sizeof(frameData);
    encoder.avFrame_ = std::shared_ptr<AVFrame>(frame, [](AVFrame *ptr) {
        delete ptr;
    });

    EXPECT_EQ(encoder.WritePcm(srcData, sizeof(srcData)), ERROR);
    encoder.isInit_ = true;
    EXPECT_EQ(encoder.WritePcm(nullptr, sizeof(srcData)), ERROR);
    encoder.srcSampleFormat_ = SampleFormat::S16LE;
    EXPECT_EQ(encoder.FillFrameFromBuffer(srcData, sizeof(srcData)), SUCCESS);
    EXPECT_EQ(memcmp(frameData, srcData, sizeof(srcData)), 0);
    EXPECT_EQ(encoder.FillFrameFromBuffer(srcData, sizeof(frameData) + 1), ERROR);
}

HWTEST(MediaMonitorDtTest, SampleConvert_InitConvertAndReleaseBranch_001, TestSize.Level0)
{
    ResamplePara param;
    param.channels = 2;
    param.sampleRate = TEST_SAMPLE_RATE;
    param.channelLayout = 3;
    param.srcFfFmt = AV_SAMPLE_FMT_FLTP;
    param.destFmt = AV_SAMPLE_FMT_S16;
    SampleConvert nullConvert(nullptr);
    EXPECT_EQ(nullConvert.Init(param), ERROR);
    EXPECT_EQ(nullConvert.Convert(nullptr, 0, nullptr), ERROR);

    auto apiWrap = std::make_shared<FFmpegApiWrap>();
    apiWrap->swrAllocFunc = SwrAllocStub;
    apiWrap->getChannelLayoutFromMaskFunc = ChannelLayoutFromMaskStub;
    apiWrap->getChannelLayoutDefaultFunc = ChannelLayoutDefaultStub;
    apiWrap->swrSetOpts2Func = SwrSetOpts2Stub;
    apiWrap->swrInitFunc = SwrInitStub;
    apiWrap->swrFreeFunc = SwrFreeStub;
    apiWrap->sampleFmtIsPlannarFunc = SampleFmtIsPlanarStub;
    apiWrap->swrConvertFunc = SwrConvertStub;

    SampleConvert convert(apiWrap);
    EXPECT_EQ(convert.Init(param), SUCCESS);
    uint8_t src[8] = {};
    uint8_t out[8] = {};
    uint8_t *extendedData[] = {out};
    AVFrame frame = {};
    frame.extended_data = extendedData;
    frame.nb_samples = 2;
    EXPECT_EQ(convert.Convert(src, sizeof(src), &frame), SUCCESS);
    convert.Release();
    EXPECT_EQ(convert.Convert(src, sizeof(src), &frame), ERROR);
}

std::shared_ptr<EventBean> MakeWriterTestBean()
{
    auto bean = MakeUsageBean(STREAM_UTILIZATION_STATS);
    bean->Add("HASMIC", 1);
    bean->Add("ISCONNECT", 1);
    bean->Add("DEVICETYPE", TEST_DEVICE_TYPE_PLAYBACK);
    bean->Add("VOLUME", 0.5f);
    bean->Add("VOLUMEFACTOR", 1.0f);
    bean->Add("POWERVOLUMEFACTOR", 1.0f);
    AddString(bean, "NETWORKID", "net");
    AddString(bean, "ADDRESS", "addr");
    AddString(bean, "DEVICE_NAME", "name");
    bean->Add("STATUS", 1);
    bean->Add("CATEGORY", 1);
    bean->Add("ENGINE_TYPE", 1);
    AddString(bean, "APP_BUNDLE_NAME", "bundle");
    bean->Add("STREAM_VOLUME", 1.0f);
    bean->Add("MUTE_STATE", 1);
    bean->Add("APP_BACKGROUND_STATE", 0);
    bean->Add("MUTE_PLAY_START_TIME", static_cast<uint64_t>(1));
    bean->Add("MUTE_PLAY_DURATION", TEST_MUTE_PLAY_DURATION);
    bean->Add("HDI_TYPE", 1);
    bean->Add("ERROR_CASE", 1);
    bean->Add("ERROR_MSG", 1);
    AddString(bean, "ERROR_DESCRIPTION", "error");
    bean->Add("REASON", 1);
    bean->Add("PERIOD_MS", 1);
    bean->Add("HDI_ADAPTER", 1);
    bean->Add("POSITION", 1);
    AddString(bean, "APP_NAMES", "apps");
    bean->Add("JANK_START_TIME", static_cast<uint64_t>(1));
    bean->Add("SERVICE_ID", 1);
    AddString(bean, "DUBIOUS_APP", "dubious");
    bean->Add("PIPE_TYPE_BEFORE_CHANGE", 1);
    bean->Add("PIPE_TYPE_AFTER_CHANGE", 2);
    bean->Add("DEVICE_TYPE_BEFORE_CHANGE", 1);
    bean->Add("DEVICE_TYPE_AFTER_CHANGE", 2);
    bean->Add("PRE_AUDIO_SCENE", 1);
    bean->Add("CUR_AUDIO_SCENE", 2);
    AddString(bean, "DEVICE_LIST", "list");
    bean->Add("ROUTER_TYPE", 1);
    AddString(bean, "DB_TYPE", "db");
    bean->Add("CHANGE_REASON", 1);
    AddString(bean, "DEVICE_DESC", "desc");
    bean->Add("MIGRATE_DIRECTION", 1);
    bean->Add("ADD_REMOVE_OPERATION", 1);
    bean->Add("FILE_SIZE", static_cast<uint64_t>(1));
    bean->Add("RINGTONE_CATEGORY", 1);
    bean->Add("MEDIA_TYPE", 1);
    AddString(bean, "MIME_TYPE", "audio/pcm");
    bean->Add("TIMESTAMP", static_cast<uint64_t>(1));
    bean->Add("RESULT", 0);
    AddString(bean, "SUBSCRIBE_KEY", "key");
    bean->Add("SUBSCRIBE_RESULT", 0);
    bean->Add("SESSIONID", 1);
    bean->Add("STREAM_OR_SOURCE_TYPE", 1);
    bean->Add("START_TIME", static_cast<uint64_t>(1));
    bean->Add("UPLOAD_TIME", static_cast<uint64_t>(2));
    bean->Add("STANDBY_DURATION_S", 1);
    bean->Add("INCOMING_SOURCE", 1);
    bean->Add("INCOMING_PID", TEST_INCOMING_PID);
    AddString(bean, "INCOMING_PKG", "incoming");
    bean->Add("ACTIVE_SOURCE", 1);
    bean->Add("ACTIVE_PID", TEST_ACTIVE_PID);
    AddString(bean, "ACTIVE_PKG", "active");
    bean->Add("TYPE", 1);
    bean->Add("COUNT", 1);
    bean->Add("MSG_TYPE", 1);
    AddString(bean, "MSG_FUNC_NAME", "func");
    AddString(bean, "MSG_ERROR_DESCRIPTION", "message");
    bean->Add("SESSION_ID", 1);
    AddString(bean, "CURRENT_NAME", "cur");
    AddString(bean, "DES_NAME", "des");
    AddString(bean, "FUNC_NAME", "SetVolume");
    bean->Add("PARAM_VALUE", TEST_PARAM_VALUE);
    bean->Add("SYSTEMHAP_SET_FOCUSSTRATEGY", 1);
    AddString(bean, "ERROR_INFO", "info");
    bean->Add("INTERRUPT_HINTTYPE", 1);
    AddString(bean, "RENDERER_INFO", "renderer");
    AddString(bean, "SESSION_INFO", "session");
    bean->Add("WINDOW_STATE", 1);
    bean->Add("RENDERER_PLAY_TIMES", 1);
    AddString(bean, "CURR_APP_NAME", "curApp");
    AddString(bean, "CURR_RENDERER_INFO", "curRenderer");
    AddString(bean, "CURR_SESSION_INFO", "curSession");
    AddString(bean, "BUNDLENAME", "bundle");
    bean->Add("AUDIODIRECTION", 1);
    bean->Add("AUDIOSTREAM", 1);
    bean->Add("CALLFUNC", 1);
    bean->Add("ERROR_SCENE", 1);
    bean->Add("LOUD_VOLUME_TIMES", 1);
    bean->Add("SCENE_TYPE", LOUD_VOLUME_SCENE);
    AddString(bean, "MUTETYPE", "mute");
    AddString(bean, "ERROR_REASON", "reason");
    bean->Add("APP_UID", 1);
    bean->Add("RENDER_DEVICE_TYPE", 1);
    bean->Add("CAPTURE_DEVICE_TYPE", 1);
    bean->Add("ERROR_SCOPE", 1);
    bean->Add("ERROR_TYPE", 1);
    bean->Add("FEATURE", 1);
    return bean;
}

void WriteAllEventsWithNull(MediaEventBaseWriter &writer)
{
    std::shared_ptr<EventBean> nullBean = nullptr;
    writer.WriteHeasetChange(nullBean);
    writer.WriteVolumeChange(nullBean);
    writer.WriteStreamChange(nullBean);
    writer.WriteStreamStandby(nullBean);
    writer.WriteSmartPAStatus(nullBean);
    writer.WriteDeviceChange(nullBean);
    writer.WriteNoiseSuppression(nullBean);
    writer.WriteLoadConfigError(nullBean);
    writer.WriteLoadEffectEngineError(nullBean);
    writer.WriteAppWriteMute(nullBean);
    writer.WriteHdiException(nullBean);
    writer.WriteJankPlaybackError(nullBean);
    writer.WriteAudioStartupError(nullBean);
    writer.WriteStreamExhastedError(nullBean);
    writer.WriteStreamCreateError(nullBean);
    writer.WriteBackgoundSilentPlayback(nullBean);
    writer.WriteBGSilentPlayback(nullBean);
    writer.WriteExcludeOutputDevice(nullBean);
    writer.WriteSetForceDevice(nullBean);
    writer.WriteDeviceStatistic(nullBean);
    writer.WriteBtUsageStatistic(nullBean);
    writer.WriteStreamStatistic(nullBean);
    writer.WriteStreamPropertyStatistic(nullBean);
    writer.WriteUnderrunStatistic(nullBean);
    writer.WriteAudioPipeChange(nullBean);
    writer.WriteAudioRouteChange(nullBean);
    writer.WriteDbAccessException(nullBean);
    writer.WriteDeviceChangeException(nullBean);
    writer.WriteFocusMigrate(nullBean);
    writer.WriteMutedCapture(nullBean);
    writer.WriteCustomizedToneChange(nullBean);
    writer.WritePlaybackVolume(nullBean);
    writer.WriteVolumeSubscribe(nullBean);
    writer.WriteStreamOccupancy(nullBean);
    writer.WriteAudioRecordError(nullBean);
    writer.WriteMessageQueueException(nullBean);
    writer.WriteStreamMoveException(nullBean);
    writer.WriteVolumeApiInvoke(nullBean);
    writer.WriteAppCallSession(nullBean);
    writer.WriteAudioInterruptErrorEvent(nullBean);
    writer.WriteAudioPlaybackErrorEvent(nullBean);
    writer.WriteAudioMainThreadEvent(nullBean);
    writer.WriteSuiteEngineException(nullBean);
    writer.WriteMuteBundleName(nullBean);
    writer.WriteTonePlaybackFailed(nullBean);
    writer.WriteAudioLoopbackException(nullBean);
    writer.WriteKaraokeFeatureStatistic(nullBean);
}

void WriteAllEventsWithValidBean(MediaEventBaseWriter &writer, const std::shared_ptr<EventBean> &bean)
{
    writer.WriteHeasetChange(bean);
    writer.WriteVolumeChange(bean);
    writer.WriteStreamChange(bean);
    writer.WriteStreamStandby(bean);
    writer.WriteSmartPAStatus(bean);
    writer.WriteDeviceChange(bean);
    writer.WriteNoiseSuppression(bean);
    writer.WriteLoadConfigError(bean);
    writer.WriteLoadEffectEngineError(bean);
    writer.WriteAppWriteMute(bean);
    writer.WriteHdiException(bean);
    writer.WriteJankPlaybackError(bean);
    bean->UpdateIntMap("REASON", 0);
    writer.WriteJankPlaybackError(bean);
    writer.WriteAudioStartupError(bean);
    writer.WriteStreamExhastedError(bean);
    writer.WriteStreamCreateError(bean);
    writer.WriteBackgoundSilentPlayback(bean);
    writer.WriteBGSilentPlayback(bean);
    writer.WriteExcludeOutputDevice(bean);
    writer.WriteSetForceDevice(bean);
    writer.WriteDeviceStatistic(bean);
    writer.WriteBtUsageStatistic(bean);
    writer.WriteStreamStatistic(bean);
    writer.WriteStreamPropertyStatistic(bean);
    writer.WriteUnderrunStatistic(bean);
    writer.WriteAudioPipeChange(bean);
    writer.WriteAudioRouteChange(bean);
    writer.WriteDbAccessException(bean);
    writer.WriteDeviceChangeException(bean);
    writer.WriteFocusMigrate(bean);
    writer.WriteMutedCapture(bean);
    writer.WriteCustomizedToneChange(bean);
    writer.WritePlaybackVolume(bean);
    writer.WriteVolumeSubscribe(bean);
    auto systemToneResult = std::make_unique<DfxSystemTonePlaybackResult>();
    writer.WriteSystemTonePlayback(systemToneResult);
    writer.WriteStreamOccupancy(bean);
    writer.WriteAudioRecordError(bean);
    writer.WriteMessageQueueException(bean);
    writer.WriteStreamMoveException(bean);
    writer.WriteVolumeApiInvoke(bean);
    writer.WriteAppCallSession(bean);
    writer.WriteAudioInterruptErrorEvent(bean);
    writer.WriteAudioPlaybackErrorEvent(bean);
    writer.WriteAudioMainThreadEvent(bean);
    SuiteEngineUtilizationStatsReportData data;
    data.nodeType = "node";
    data.appNameList.push_back("app");
    data.nodeCountList.push_back(1);
    writer.WriteSuiteEngineUtilizationStats(data);
    writer.WriteSuiteEngineException(bean);
    writer.WriteVolumeSettingStatistics(bean);
    writer.WriteMuteBundleName(bean);
    writer.WriteTonePlaybackFailed(bean);
    writer.WriteAudioLoopbackException(bean);
    writer.WriteKaraokeFeatureStatistic(bean);
}

HWTEST(MediaMonitorDtTest, MediaEventBaseWriter_NullAndValidBranches_001, TestSize.Level0)
{
    MediaEventBaseWriter &writer = MediaEventBaseWriter::GetMediaEventBaseWriter();
    auto bean = MakeWriterTestBean();
    WriteAllEventsWithNull(writer);
    WriteAllEventsWithValidBean(writer, bean);
    EXPECT_EQ(bean->GetStringValue("APP_NAME"), "dt.app");
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_WriteEventRoutingBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    std::shared_ptr<EventBean> nullBean = nullptr;
    policy.WriteEvent(HEADSET_CHANGE, nullBean);

    auto behavior = MakeEvent(HEADSET_CHANGE, BEHAVIOR_EVENT);
    behavior->Add("HASMIC", 1);
    behavior->Add("ISCONNECT", 1);
    behavior->Add("DEVICETYPE", 2);
    policy.WriteEvent(HEADSET_CHANGE, behavior);

    auto fault = MakeEvent(LOAD_CONFIG_ERROR, FAULT_EVENT);
    fault->Add("CATEGORY", 1);
    policy.WriteEvent(LOAD_CONFIG_ERROR, fault);

    auto aggregation = MakeUsageBean(STREAM_PROPERTY_STATS);
    policy.WriteEvent(STREAM_PROPERTY_STATS, aggregation);
    EXPECT_TRUE(policy.eventVector_.empty());
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_SwitchCaseExpansionBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    std::vector<EventId> behaviorEvents = {
        DEVICE_CHANGE, HEADSET_CHANGE, STREAM_CHANGE, VOLUME_CHANGE, AUDIO_ROUTE_CHANGE,
        AUDIO_PIPE_CHANGE, AUDIO_FOCUS_MIGRATE, SET_FORCE_USE_AUDIO_DEVICE,
        BACKGROUND_SILENT_PLAYBACK, STREAM_STANDBY, AI_VOICE_NOISE_SUPPRESSION,
        PROCESS_IN_MAINTHREAD, VOLUME_SUBSCRIBE, SMARTPA_STATUS, EXCLUDE_OUTPUT_DEVICE,
        SYSTEM_TONE_PLAYBACK, MUTE_BUNDLE_NAME, UNKNOW_EVENTID
    };
    for (auto eventId : behaviorEvents) {
        auto bean = MakeRichBean(eventId, BEHAVIOR_EVENT);
        FillSystemToneBean(bean);
        policy.WriteBehaviorEvent(eventId, bean);
    }

    std::vector<EventId> faultEvents = {
        LOAD_CONFIG_ERROR, AUDIO_SERVICE_STARTUP_ERROR, LOAD_EFFECT_ENGINE_ERROR, DB_ACCESS_EXCEPTION,
        DEVICE_CHANGE_EXCEPTION, APP_WRITE_MUTE, HDI_EXCEPTION, JANK_PLAYBACK, RECORD_ERROR,
        HPAE_MESSAGE_QUEUE_EXCEPTION, LOOPBACK_EXCEPTION, STREAM_MOVE_EXCEPTION, SUITE_ENGINE_EXCEPTION,
        TONE_PLAYBACK_FAILED, AUDIO_STREAM_EXHAUSTED_STATS, UNKNOW_EVENTID
    };
    for (auto eventId : faultEvents) {
        auto bean = MakeRichBean(eventId, FAULT_EVENT);
        policy.WriteFaultEvent(eventId, bean);
    }

    std::vector<EventId> aggregateEvents = {
        AUDIO_STREAM_CREATE_ERROR_STATS, BACKGROUND_SILENT_PLAYBACK, STREAM_UTILIZATION_STATS,
        STREAM_PROPERTY_STATS, AUDIO_DEVICE_UTILIZATION_STATS, BT_UTILIZATION_STATS,
        PERFORMANCE_UNDER_OVERRUN_STATS, PLAYBACK_VOLUME_STATS, MUTED_CAPTURE_STATS,
        STREAM_OCCUPANCY, ADD_REMOVE_CUSTOMIZED_TONE, UNKNOW_EVENTID
    };
    for (auto eventId : aggregateEvents) {
        auto bean = MakeRichBean(eventId, FREQUENCY_AGGREGATION_EVENT);
        policy.WriteAggregationEvent(eventId, bean);
    }

    EXPECT_FALSE(policy.systemTonePlayEventVector_.empty());
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_EventVectorAggregationBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    std::shared_ptr<EventBean> nullBean = nullptr;
    policy.HandDeviceUsageToEventVector(nullBean);
    policy.HandBtUsageToEventVector(nullBean);
    EXPECT_TRUE(policy.eventVector_.empty());

    auto deviceUsage = MakeUsageBean(AUDIO_DEVICE_UTILIZATION_STATS);
    policy.HandDeviceUsageToEventVector(deviceUsage);
    ASSERT_EQ(policy.eventVector_.size(), 1U);
    EXPECT_EQ(policy.eventVector_[0]->GetUint64Value("DURATION"), TEST_DURATION);
    auto sameDeviceUsage = MakeUsageBean(AUDIO_DEVICE_UTILIZATION_STATS);
    sameDeviceUsage->UpdateUint64Map("DURATION", 10);
    policy.HandDeviceUsageToEventVector(sameDeviceUsage);
    EXPECT_EQ(policy.eventVector_.size(), 1U);
    EXPECT_EQ(policy.eventVector_[0]->GetUint64Value("DURATION"), TEST_DURATION + 10);

    auto btUsage = MakeUsageBean(BT_UTILIZATION_STATS);
    policy.HandBtUsageToEventVector(btUsage);
    auto streamUsage = MakeUsageBean(STREAM_UTILIZATION_STATS);
    policy.HandStreamUsageToEventVector(streamUsage);
    auto streamProperty = MakeUsageBean(STREAM_PROPERTY_STATS);
    policy.HandStreamPropertyToEventVector(streamProperty);
    auto volume = MakeUsageBean(PLAYBACK_VOLUME_STATS);
    policy.HandleVolumeToEventVector(volume);
    auto captureMuted = MakeUsageBean(MUTED_CAPTURE_STATS);
    policy.HandleCaptureMutedToEventVector(captureMuted);
    auto createError = MakeUsageBean(AUDIO_STREAM_CREATE_ERROR_STATS);
    policy.HandleCreateErrorToEventVector(createError);
    policy.HandleCreateErrorToEventVector(createError);
    auto underrun = MakeUsageBean(PERFORMANCE_UNDER_OVERRUN_STATS);
    policy.HandleUnderrunToEventVector(underrun);
    policy.HandleUnderrunToEventVector(underrun);

    EXPECT_GT(policy.eventVector_.size(), 5U);
    policy.HandleToHiSysEvent();
    EXPECT_TRUE(policy.eventVector_.empty());
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_SilentSystemToneAndQueueBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    auto silent = MakeUsageBean(BACKGROUND_SILENT_PLAYBACK);
    policy.HandleSilentPlaybackToEventVector(silent);
    policy.HandleSilentPlaybackToEventVector(silent);
    ASSERT_EQ(policy.eventVector_.size(), 1U);
    EXPECT_EQ(policy.eventVector_[0]->GetIntValue("TIMES"), 2);

    auto tone = MakeEvent(SYSTEM_TONE_PLAYBACK, BEHAVIOR_EVENT);
    FillSystemToneBean(tone);
    policy.systemTonePlayerCount_ = 99;
    policy.TriggerSystemTonePlaybackEvent(tone);
    EXPECT_EQ(policy.systemTonePlayerCount_, 0);
    EXPECT_TRUE(policy.systemTonePlayEventVector_.empty());

    policy.systemTonePlayEventVector_.push_back(tone);
    auto result = std::make_unique<DfxSystemTonePlaybackResult>();
    policy.CollectDataToDfxResult(result.get());
    ASSERT_EQ(result->timeStamp.size(), 1U);
    EXPECT_EQ(result->clientUid[0], TEST_CLIENT_UID);
    policy.systemTonePlayEventVector_.push_back(nullptr);
    policy.CollectDataToDfxResult(nullptr);
    policy.HandleToSystemTonePlaybackEvent();
    EXPECT_TRUE(policy.systemTonePlayEventVector_.empty());

    std::shared_ptr<EventBean> nullBean = nullptr;
    policy.AddToVolumeApiInvokeQueue(nullBean);
    auto apiEvent = MakeEvent(VOLUME_API_INVOKE, FREQUENCY_AGGREGATION_EVENT);
    apiEvent->Add("CLIENT_UID", TEST_CLIENT_UID);
    AddString(apiEvent, "FUNC_NAME", "SetVolume");
    apiEvent->Add("PARAM_VALUE", 3);
    policy.volumeApiInvokeRecordSetSize_ = 1;
    policy.AddToVolumeApiInvokeQueue(apiEvent);
    policy.AddToVolumeApiInvokeQueue(apiEvent);
    auto apiEvent2 = MakeEvent(VOLUME_API_INVOKE, FREQUENCY_AGGREGATION_EVENT);
    apiEvent2->Add("CLIENT_UID", TEST_CLIENT_UID + 1);
    AddString(apiEvent2, "FUNC_NAME", "SetVolume");
    apiEvent2->Add("PARAM_VALUE", 4);
    policy.AddToVolumeApiInvokeQueue(apiEvent2);
    EXPECT_GE(policy.volumeApiInvokeRecordSet_.size(), 1U);
    policy.volumeApiInvokeOnceEvent_ = 1;
    policy.HandleToVolumeApiInvokeEvent();
    EXPECT_FALSE(policy.volumeApiInvokeEventQueue_.empty());
}

void VerifySuiteEngineNodeStats(MediaMonitorPolicy &policy)
{
    auto suite = MakeEvent(SUITE_ENGINE_UTILIZATION_STATS, FREQUENCY_AGGREGATION_EVENT);
    suite->Add("CLIENT_UID", TEST_CLIENT_UID);
    AddString(suite, "AUDIO_NODE_TYPE", "node");
    suite->Add("AUDIO_NODE_COUNT", 1);
    suite->Add("RT_MODE_RENDER_COUNT", 2);
    suite->Add("RT_MODE_RTF_OVER_BASE_COUNT", -1);
    suite->Add("RT_MODE_RTF_OVER_110BASE_COUNT", 3);
    suite->Add("RT_MODE_RTF_OVER_120BASE_COUNT", 4);
    suite->Add("RT_MODE_RTF_OVER_100_COUNT", 5);
    suite->Add("EDIT_MODE_RENDER_COUNT", 6);
    suite->Add("EDIT_MODE_RTF_OVER_BASE_COUNT", 7);
    suite->Add("EDIT_MODE_RTF_OVER_110BASE_COUNT", 8);
    suite->Add("EDIT_MODE_RTF_OVER_120BASE_COUNT", 9);
    suite->Add("EDIT_MODE_RTF_OVER_100_COUNT", 10);
    policy.AddToSuiteEngineNodeStatsMap(suite);
    policy.AddToSuiteEngineNodeStatsMap(suite);
    ASSERT_EQ(policy.suiteEngineNodeStatsMap_.size(), 1U);
    auto &stat = policy.suiteEngineNodeStatsMap_.begin()->second.begin()->second;
    EXPECT_EQ(stat.nodeCount, 2U);
    EXPECT_EQ(stat.rtfOverBaselineCnt, 0U);
    policy.HandleToSuiteEngineUtilizationStatsEvent();
    EXPECT_TRUE(policy.suiteEngineNodeStatsMap_.empty());
}

void VerifyVolumeSettingAndKaraoke(MediaMonitorPolicy &policy)
{
    auto volumeSetting = MakeEvent(VOLUME_SETTING_STATISTICS, FREQUENCY_AGGREGATION_EVENT);
    volumeSetting->Add("SCENE_TYPE", LOUD_VOLUME_SCENE);
    policy.HandleVolumeSettingStatistics(volumeSetting);
    EXPECT_EQ(policy.loudVolumeTimes_.load(), 1);
    auto otherScene = MakeEvent(VOLUME_SETTING_STATISTICS, FREQUENCY_AGGREGATION_EVENT);
    otherScene->Add("SCENE_TYPE", 10);
    policy.HandleVolumeSettingStatistics(otherScene);
    EXPECT_EQ(policy.loudVolumeTimes_.load(), 1);
    policy.HandleToVolumeSettingStatisticsEvent();
    EXPECT_EQ(policy.loudVolumeTimes_.load(), 0);
    policy.HandleToLoudVolumeSceneEvent(volumeSetting);

    auto karaoke = MakeEvent(KARAOKE_FEATURE_UTILIZATION, FREQUENCY_AGGREGATION_EVENT);
    karaoke->Add("UID", TEST_CLIENT_UID);
    karaoke->Add("TYPE", 1);
    karaoke->Add("FEATURE", 2);
    karaoke->Add("DEVICE_TYPE", 3);
    karaoke->Add("DURATION", static_cast<uint64_t>(4));
    policy.AddToKaraokeFeatureVector(karaoke);
    policy.AddToKaraokeFeatureVector(karaoke);
    ASSERT_EQ(policy.karaokeFeatureEventVector_.size(), 1U);
    EXPECT_EQ(policy.karaokeFeatureEventVector_[0]->GetUint64Value("DURATION"), 8);
    policy.karaokeFeatureEventVector_.push_back(nullptr);
    policy.HandleToKaraokeFeatureEvent();
    EXPECT_TRUE(policy.karaokeFeatureEventVector_.empty());
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_CallInterruptSuiteVolumeAndKaraokeBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    std::shared_ptr<EventBean> nullBean = nullptr;
    policy.AddToCallSessionQueue(nullBean);
    policy.AddAudioInterruptErrorEvent(nullBean);
    policy.AddAudioPlaybackErrorEvent(nullBean);
    policy.AddToKaraokeFeatureVector(nullBean);

    auto call = MakeEvent(HAP_CALL_AUDIO_SESSION, FREQUENCY_AGGREGATION_EVENT);
    call->Add("CLIENT_UID", TEST_CLIENT_UID);
    call->Add("SYSTEMHAP_SET_FOCUSSTRATEGY", 1);
    policy.callSessionHapSetSize_ = 1;
    policy.AddToCallSessionQueue(call);
    policy.AddToCallSessionQueue(call);
    EXPECT_EQ(policy.callSessionHapSet_.size(), 1U);

    auto interrupt = MakeEvent(INTERRUPT_ERROR, FREQUENCY_AGGREGATION_EVENT);
    AddString(interrupt, "APP_NAME", "app");
    AddString(interrupt, "ERROR_INFO", "err");
    policy.audioInterruptErrorSetSize_ = 1;
    policy.AddAudioInterruptErrorEvent(interrupt);
    policy.AddAudioInterruptErrorEvent(interrupt);
    EXPECT_EQ(policy.audioInterruptErrorSet_.size(), 1U);

    auto playback = MakeEvent(AUDIO_PLAYBACK_ERROR, FREQUENCY_AGGREGATION_EVENT);
    AddString(playback, "APP_NAME", "app");
    playback->Add("STREAM_TYPE", 1);
    playback->Add("TYPE", 1);
    policy.AddAudioPlaybackErrorEvent(playback);

    VerifySuiteEngineNodeStats(policy);
    VerifyVolumeSettingAndKaraoke(policy);
}

HWTEST(MediaMonitorDtTest, MediaMonitorPolicy_BundleCacheAndWriteInfoBranch_001, TestSize.Level0)
{
    auto &policy = MediaMonitorPolicy::GetMediaMonitorPolicy();
    BundleInfo cached;
    cached.appName = "cached.app";
    cached.versionCode = 123;
    policy.cachedBundleInfoMap_[TEST_CLIENT_UID] = cached;
    BundleInfo out = policy.GetBundleInfo(TEST_CLIENT_UID);
    EXPECT_EQ(out.appName, "cached.app");

    BundleInfo fallback = policy.GetBundleInfo(TEST_CLIENT_UID + 1);
    EXPECT_EQ(fallback.appName, "uid:" + std::to_string(TEST_CLIENT_UID + 1));

    std::string dump;
    policy.WriteInfo(-1, dump);
    EXPECT_TRUE(dump.empty());
    policy.WriteInfo(1, dump);
    EXPECT_NE(dump.find("Counting of eventVector entries"), std::string::npos);
}

void VerifyUnifiedFaultCodeRecords(EventAggregate &aggregate)
{
    for (int32_t i = 0; i < 12; ++i) {
        auto fault = MakeEvent(UNIFIED_FAULT_CODE, FAULT_EVENT);
        fault->Add("ERROR_UID", i);
        fault->Add("ERROR_CODE", 0x1000 + i);
        fault->Add("ERROR_REASON", "reason" + std::to_string(i));
        aggregate.WriteEvent(fault);
    }
    auto records = aggregate.GetUnifiedFaultCodeRecords();
    EXPECT_EQ(records.size(), 10U);
    EXPECT_EQ(records.front()->GetIntValue("ERROR_UID"), 2);

    std::string dump;
    aggregate.WriteInfo(-1, dump);
    EXPECT_TRUE(dump.empty());
    aggregate.WriteInfo(1, dump);
    EXPECT_FALSE(dump.empty());
}

HWTEST(MediaMonitorDtTest, EventAggregate_RoutingStateAndFaultRecordBranch_001, TestSize.Level0)
{
    auto &aggregate = EventAggregate::GetEventAggregate();
    aggregate.deviceUsageVector_.clear();
    aggregate.streamUsageVector_.clear();
    aggregate.captureMutedVector_.clear();
    aggregate.volumeVector_.clear();
    aggregate.streamPropertyVector_.clear();
    aggregate.unifiedFaultCodeVector_.clear();

    std::shared_ptr<EventBean> nullBean = nullptr;
    aggregate.WriteEvent(nullBean);

    auto running = MakeStreamChangeBean(AudioStandard::State::RUNNING, 1);
    aggregate.WriteEvent(running);
    EXPECT_EQ(aggregate.deviceUsageVector_.size(), 1U);
    EXPECT_EQ(aggregate.streamUsageVector_.size(), 1U);
    EXPECT_EQ(aggregate.volumeVector_.size(), 1U);
    EXPECT_EQ(aggregate.streamPropertyVector_.size(), 1U);
    aggregate.WriteEvent(running);
    EXPECT_EQ(aggregate.deviceUsageVector_.size(), 1U);

    uint64_t oldTime = TimeUtils::GetCurSec() - TEST_DURATION;
    aggregate.deviceUsageVector_[0]->UpdateUint64Map("START_TIME", oldTime);
    aggregate.streamUsageVector_[0]->UpdateUint64Map("START_TIME", oldTime);
    aggregate.volumeVector_[0]->UpdateUint64Map("START_TIME", oldTime);
    aggregate.streamPropertyVector_[0]->UpdateUint64Map("START_TIME", oldTime);
    auto stopped = MakeStreamChangeBean(AudioStandard::State::STOPPED, 1);
    aggregate.WriteEvent(stopped);
    EXPECT_TRUE(aggregate.deviceUsageVector_.empty());
    EXPECT_TRUE(aggregate.streamUsageVector_.empty());
    EXPECT_TRUE(aggregate.volumeVector_.empty());
    EXPECT_TRUE(aggregate.streamPropertyVector_.empty());

    auto captureRunning = MakeStreamChangeBean(AudioStandard::State::RUNNING, 0, 2);
    aggregate.WriteEvent(captureRunning);
    EXPECT_EQ(aggregate.captureMutedVector_.size(), 1U);
    aggregate.captureMutedVector_[0]->Add("MUTED", 1);
    aggregate.captureMutedVector_[0]->UpdateUint64Map("START_TIME", oldTime);
    auto captureStopped = MakeStreamChangeBean(AudioStandard::State::STOPPED, 0, 2);
    aggregate.WriteEvent(captureStopped);
    EXPECT_TRUE(aggregate.captureMutedVector_.empty());

    VerifyUnifiedFaultCodeRecords(aggregate);
}

HWTEST(MediaMonitorDtTest, EventAggregate_SwitchCaseExpansionBranch_001, TestSize.Level0)
{
    auto &aggregate = EventAggregate::GetEventAggregate();
    std::vector<EventId> directEvents = {
        HEADSET_CHANGE, AUDIO_ROUTE_CHANGE, LOAD_CONFIG_ERROR, APP_WRITE_MUTE, HDI_EXCEPTION,
        AUDIO_SERVICE_STARTUP_ERROR, STREAM_STANDBY, SMARTPA_STATUS, DB_ACCESS_EXCEPTION,
        DEVICE_CHANGE_EXCEPTION, AI_VOICE_NOISE_SUPPRESSION, LOAD_EFFECT_ENGINE_ERROR,
        SYSTEM_TONE_PLAYBACK, ADD_REMOVE_CUSTOMIZED_TONE, RECORD_ERROR, STREAM_OCCUPANCY,
        HPAE_MESSAGE_QUEUE_EXCEPTION, STREAM_MOVE_EXCEPTION, PROCESS_IN_MAINTHREAD,
        SUITE_ENGINE_EXCEPTION, MUTE_BUNDLE_NAME, AUDIO_STREAM_CREATE_ERROR_STATS,
        TONE_PLAYBACK_FAILED, LOOPBACK_EXCEPTION, AUDIO_STREAM_EXHAUSTED_STATS
    };
    for (auto eventId : directEvents) {
        auto bean = MakeRichBean(eventId, eventId <= AUDIO_STREAM_CREATE_ERROR_STATS ? FAULT_EVENT : BEHAVIOR_EVENT);
        FillSystemToneBean(bean);
        aggregate.WriteEvent(bean);
    }

    std::vector<EventId> aggregateEvents = {
        BACKGROUND_SILENT_PLAYBACK, PERFORMANCE_UNDER_OVERRUN_STATS, SET_FORCE_USE_AUDIO_DEVICE,
        CAPTURE_MUTE_STATUS_CHANGE, VOLUME_CHANGE, AUDIO_PIPE_CHANGE, AUDIO_FOCUS_MIGRATE,
        JANK_PLAYBACK, EXCLUDE_OUTPUT_DEVICE, SET_DEVICE_COLLABORATIVE_STATE, APP_SESSION_STATE,
        APP_BACKTASK_STATE, VOLUME_API_INVOKE, HAP_CALL_AUDIO_SESSION, DISTRIBUTED_DEVICE_INFO,
        DISTRIBUTED_SCENE_INFO, DM_DEVICE_INFO, SUITE_ENGINE_UTILIZATION_STATS,
        VOLUME_SETTING_STATISTICS, INTERRUPT_ERROR, AUDIO_PLAYBACK_ERROR, KARAOKE_FEATURE_UTILIZATION,
        UNKNOW_EVENTID
    };
    for (auto eventId : aggregateEvents) {
        auto bean = MakeRichBean(eventId, BEHAVIOR_EVENT);
        aggregate.WriteEvent(bean);
    }

    std::unordered_map<int32_t, bool> sessionMap;
    AudioMemo::GetAudioMemo().GetAudioAppSessionMsg(sessionMap);
    EXPECT_FALSE(sessionMap.empty());
    std::string sceneInfo;
    AudioMemo::GetAudioMemo().GetDistributedSceneInfo(sceneInfo);
    EXPECT_EQ(sceneInfo, "scene");
}

HWTEST(MediaMonitorDtTest, EventAggregate_DeviceVolumeAndStateBranch_001, TestSize.Level0)
{
    auto &aggregate = EventAggregate::GetEventAggregate();
    uint64_t oldTime = TimeUtils::GetCurSec() - TEST_DURATION;
    aggregate.deviceUsageVector_.clear();
    aggregate.captureMutedVector_.clear();
    aggregate.volumeVector_.clear();

    auto deviceUsage = MakeUsageBean(AUDIO_DEVICE_UTILIZATION_STATS);
    deviceUsage->Add("START_TIME", oldTime);
    aggregate.deviceUsageVector_.push_back(deviceUsage);
    auto deviceChange = MakeEvent(DEVICE_CHANGE, BEHAVIOR_EVENT);
    deviceChange->Add("ISOUTPUT", 1);
    deviceChange->Add("STREAMID", 1);
    deviceChange->Add("DEVICETYPE", 4);
    aggregate.HandleDeviceChangeForDeviceUsage(deviceChange);
    EXPECT_EQ(aggregate.deviceUsageVector_[0]->GetIntValue("DEVICE_TYPE"), 4);

    auto captureMuted = MakeUsageBean(MUTED_CAPTURE_STATS);
    captureMuted->Add("START_TIME", oldTime);
    captureMuted->Add("MUTED", 1);
    aggregate.captureMutedVector_.push_back(captureMuted);
    auto captureDeviceChange = MakeEvent(DEVICE_CHANGE, BEHAVIOR_EVENT);
    captureDeviceChange->Add("ISOUTPUT", 0);
    captureDeviceChange->Add("STREAMID", 1);
    captureDeviceChange->Add("DEVICETYPE", 5);
    aggregate.HandleDeviceChangeForCaptureMuted(captureDeviceChange);
    EXPECT_EQ(aggregate.captureMutedVector_[0]->GetIntValue("DEVICE_TYPE"), 5);

    auto volume = MakeUsageBean(PLAYBACK_VOLUME_STATS);
    volume->Add("START_TIME", oldTime);
    aggregate.volumeVector_.push_back(volume);
    auto volumeChange = MakeEvent(VOLUME_CHANGE, BEHAVIOR_EVENT);
    volumeChange->Add("ISOUTPUT", 1);
    volumeChange->Add("STREAMID", 1);
    volumeChange->Add("SYSVOLUME", 9);
    aggregate.HandleVolumeChange(volumeChange);
    EXPECT_EQ(aggregate.systemVol_, 9);

    auto mutedOff = MakeStreamChangeBean(AudioStandard::State::RUNNING, 0, 3);
    mutedOff->UpdateIntMap("MUTED", 0);
    aggregate.HandleCaptureMutedStatusChange(mutedOff);
    EXPECT_TRUE(aggregate.captureMutedVector_.size() <= 1U);

    auto unknown = MakeEvent(UNKNOW_EVENTID, BEHAVIOR_EVENT);
    aggregate.WriteEvent(unknown);
    aggregate.UpdateAggregateStateEventList(unknown);
    EXPECT_EQ(unknown->GetEventId(), UNKNOW_EVENTID);
}

void VerifyExcludedDeviceBranch(AudioMemo &memo)
{
    auto excluded = MakeEvent(EXCLUDE_OUTPUT_DEVICE, BEHAVIOR_EVENT);
    excluded->Add("AUDIO_DEVICE_USAGE", MEDIA_OUTPUT_DEVICES);
    excluded->Add("DEVICE_TYPE", 1);
    excluded->Add("BT_TYPE", 1);
    excluded->Add("STREAM_TYPE", 1);
    AddString(excluded, "DEVICE_NAME", "device");
    AddString(excluded, "ADDRESS", "addr");
    AddString(excluded, "NETWORKID", "net");
    excluded->Add("EXCLUSION_STATUS", 0);
    memo.UpdateExcludedDevice(excluded);
    memo.UpdateExcludedDevice(excluded);
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>> excludedDevices;
    EXPECT_EQ(memo.GetAudioExcludedDevicesMsg(excludedDevices), SUCCESS);
    ASSERT_EQ(excludedDevices[MEDIA_OUTPUT_DEVICES].size(), 1U);
    excluded->UpdateIntMap("EXCLUSION_STATUS", 1);
    memo.UpdateExcludedDevice(excluded);
    excludedDevices.clear();
    memo.GetAudioExcludedDevicesMsg(excludedDevices);
    EXPECT_TRUE(excludedDevices[MEDIA_OUTPUT_DEVICES].empty());
}

HWTEST(MediaMonitorDtTest, AudioMemo_RouteExcludedAndStateBranch_001, TestSize.Level0)
{
    auto &memo = AudioMemo::GetAudioMemo();
    std::shared_ptr<EventBean> nullBean = nullptr;
    memo.UpdataRouteInfo(nullBean);
    memo.UpdateExcludedDevice(nullBean);
    memo.UpdateCollaborativeDeviceState(nullBean);
    memo.UpdateAppSessionState(nullBean);
    memo.UpdateAppBackTaskState(nullBean);
    memo.UpdateDistributedSceneInfo(nullBean);
    memo.UpdateDistributedDeviceInfo(nullBean);
    memo.UpdateDmDeviceInfo(nullBean);

    auto route = MakeEvent(SET_FORCE_USE_AUDIO_DEVICE, BEHAVIOR_EVENT);
    route->Add("IS_PLAYBACK", 1);
    route->Add("STREAM_TYPE", AudioStandard::STREAM_USAGE_VOICE_COMMUNICATION);
    route->Add("DEVICE_TYPE", 2);
    route->Add("DEVICE_NAME", 3);
    route->Add("ADDRESS", 4);
    route->Add("BT_TYPE", 5);
    memo.UpdataRouteInfo(route);
    route->UpdateIntMap("DEVICE_TYPE", 6);
    memo.UpdataRouteInfo(route);
    std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> preferredDevices;
    EXPECT_EQ(memo.GetAudioRouteMsg(preferredDevices), SUCCESS);
    ASSERT_EQ(preferredDevices.size(), 1U);
    EXPECT_EQ(preferredDevices.begin()->first, CALL_RENDER);
    EXPECT_EQ(preferredDevices.begin()->second->deviceType_, 6);
    EXPECT_EQ(memo.ErasePreferredDeviceByType(CALL_RENDER), SUCCESS);
    EXPECT_EQ(memo.ErasePreferredDeviceByType(CALL_RENDER), ERROR);

    VerifyExcludedDeviceBranch(memo);

    auto collaborative = MakeEvent(SET_DEVICE_COLLABORATIVE_STATE, BEHAVIOR_EVENT);
    AddString(collaborative, "ADDRESS", "addr");
    collaborative->Add("COLLABORATIVE_STATE", 1);
    memo.UpdateCollaborativeDeviceState(collaborative);
    std::map<std::string, uint32_t> collaborativeMap;
    EXPECT_EQ(memo.GetCollaborativeDeviceState(collaborativeMap), SUCCESS);
    EXPECT_EQ(collaborativeMap["addr"], 1U);
}

void VerifyDistributedAndDmDeviceInfo(AudioMemo &memo)
{
    auto distributed = MakeEvent(DISTRIBUTED_DEVICE_INFO, BEHAVIOR_EVENT);
    distributed->Add("SERVICE_STATUS", 1);
    distributed->Add("IS_ADD", 1);
    AddString(distributed, "NETWORK_ID", "net");
    distributed->Add("HDI_PIN", 1);
    AddString(distributed, "ORIGINAL_INFO", "origin");
    memo.UpdateDistributedDeviceInfo(distributed);
    std::vector<std::string> distributedInfos;
    memo.GetDistributedDeviceInfo(distributedInfos);
    ASSERT_EQ(distributedInfos.size(), 1U);
    EXPECT_EQ(distributedInfos[0], "origin");
    distributed->UpdateIntMap("IS_ADD", 0);
    memo.UpdateDistributedDeviceInfo(distributed);
    distributed->UpdateIntMap("SERVICE_STATUS", 2);
    memo.UpdateDistributedDeviceInfo(distributed);
    distributedInfos.clear();
    memo.GetDistributedDeviceInfo(distributedInfos);
    EXPECT_TRUE(distributedInfos.empty());

    auto dm = MakeEvent(DM_DEVICE_INFO, BEHAVIOR_EVENT);
    dm->Add("IS_ADD", 1);
    AddString(dm, "DEVICE_NAME", "dev");
    AddString(dm, "NETWORK_ID", "net");
    dm->Add("DM_DEVICE_TYPE", TEST_DM_DEVICE_TYPE);
    memo.UpdateDmDeviceInfo(dm);
    std::vector<MonitorDmDeviceInfo> dmInfos;
    memo.GetDmDeviceInfo(dmInfos);
    ASSERT_EQ(dmInfos.size(), 1U);
    EXPECT_EQ(dmInfos[0].deviceName_, "dev");
    dm->UpdateIntMap("IS_ADD", 0);
    memo.UpdateDmDeviceInfo(dm);

    std::string dump;
    memo.WriteInfo(-1, dump);
    EXPECT_TRUE(dump.empty());
    memo.WriteInfo(1, dump);
    EXPECT_NE(dump.find("No preferred device set"), std::string::npos);
}

HWTEST(MediaMonitorDtTest, AudioMemo_AppDistributedDmAndWriteInfoBranch_001, TestSize.Level0)
{
    auto &memo = AudioMemo::GetAudioMemo();
    auto session = MakeEvent(APP_SESSION_STATE, BEHAVIOR_EVENT);
    session->Add("PID", TEST_SESSION_PID);
    session->Add("HAS_SESSION", 1);
    session->Add("IS_ADD", 1);
    memo.UpdateAppSessionState(session);
    std::unordered_map<int32_t, bool> sessionMap;
    memo.GetAudioAppSessionMsg(sessionMap);
    EXPECT_TRUE(sessionMap[TEST_SESSION_PID]);
    session->UpdateIntMap("IS_ADD", 0);
    memo.UpdateAppSessionState(session);
    sessionMap.clear();
    memo.GetAudioAppSessionMsg(sessionMap);
    EXPECT_TRUE(sessionMap.empty());

    auto backTask = MakeEvent(APP_BACKTASK_STATE, BEHAVIOR_EVENT);
    backTask->Add("PID", TEST_BACK_TASK_PID);
    backTask->Add("HAS_BACK_TASK", 1);
    backTask->Add("IS_ADD", 1);
    memo.UpdateAppBackTaskState(backTask);
    std::unordered_map<int32_t, bool> backTaskMap;
    memo.GetAudioAppBackTaskMsg(backTaskMap);
    EXPECT_TRUE(backTaskMap[TEST_BACK_TASK_PID]);
    backTask->UpdateIntMap("IS_ADD", 0);
    memo.UpdateAppBackTaskState(backTask);

    auto scene = MakeEvent(DISTRIBUTED_SCENE_INFO, BEHAVIOR_EVENT);
    AddString(scene, "SCENE_INFO", "scene");
    memo.UpdateDistributedSceneInfo(scene);
    std::string sceneInfo;
    memo.GetDistributedSceneInfo(sceneInfo);
    EXPECT_EQ(sceneInfo, "scene");

    VerifyDistributedAndDmDeviceInfo(memo);
}

HWTEST(MediaMonitorDtTest, MediaMonitorService_PermissionParameterAndQueueBranch_001, TestSize.Level0)
{
    MediaMonitorService service(MEDIA_MONITOR_SERVICE_ID, false);
    EXPECT_FALSE(service.IsNeedDump());
    service.versionType_ = BETA_VERSION;
    service.dumpType_ = "INVALID";
    service.dumpEnable_ = true;
    EXPECT_FALSE(service.IsNeedDump());
    service.dumpType_ = DEFAULT_DUMP_TYPE;
    EXPECT_TRUE(service.IsNeedDump());

    int32_t funcResult = SUCCESS;
    int32_t dumpEnable = 0;
    service.versionType_ = COMMERCIAL_VERSION;
    EXPECT_EQ(service.GetPcmDumpStatus(dumpEnable, funcResult), ERROR);
    EXPECT_EQ(service.SetMediaParameters(DEFAULT_DUMP_TYPE, "true", funcResult), ERROR);

    service.versionType_ = BETA_VERSION;
    service.dumpEnable_ = false;
    AudioDumpBuffer buffer;
    uint8_t raw[4] = {1, 2, 3, 4};
    buffer.size = sizeof(raw);
    ASSERT_EQ(buffer.RawDataCpy(raw), 0);
    EXPECT_EQ(service.WriteAudioBuffer("dt.pcm", buffer, funcResult), ERR_ILLEGAL_STATE);

    service.dumpSignal_ = std::make_shared<DumpSignal>();
    service.dumpSignal_->isRunning_.store(true);
    service.dumpEnable_ = true;
    service.dumpType_ = DEFAULT_DUMP_TYPE;
    bool hasAudioPermission = service.VerifyIsAudio();
    EXPECT_EQ(service.WriteAudioBuffer("dt.pcm", buffer, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(funcResult, hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetPcmDumpStatus(dumpEnable, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    if (hasAudioPermission) {
        EXPECT_EQ(dumpEnable, 1);
    }
    EXPECT_EQ(service.SetMediaParameters("INVALID", "true", funcResult), ERROR);
    service.dumpEnable_ = false;
    EXPECT_EQ(service.SetMediaParameters(DEFAULT_DUMP_TYPE, "false", funcResult), ERROR);
}

HWTEST(MediaMonitorDtTest, MediaMonitorService_MessageDumpAndFileBranch_001, TestSize.Level0)
{
    MediaMonitorService service(MEDIA_MONITOR_SERVICE_ID, false);
    std::shared_ptr<EventBean> nullBean = nullptr;
    service.GetMessageFromQueue(nullBean);
    service.MessageLoopFunc();
    service.AddMessageToQueue(nullBean);

    service.signal_ = std::make_shared<MessageSignal>();
    service.signal_->isRunning_.store(false);
    service.AddMessageToQueue(nullBean);
    service.signal_->isRunning_.store(true);
    auto bean = MakeEvent(HEADSET_CHANGE, BEHAVIOR_EVENT);
    bean->Add("HASMIC", 1);
    bean->Add("ISCONNECT", 1);
    bean->Add("DEVICETYPE", 2);
    service.AddMessageToQueue(bean);
    EXPECT_EQ(service.signal_->messageQueue_.size(), 1U);
    service.GetMessageFromQueue(bean);

    service.dumpSignal_ = std::make_shared<DumpSignal>();
    service.dumpSignal_->isRunning_.store(false);
    std::shared_ptr<AudioDumpBuffer> dumpBuffer = std::make_shared<AudioDumpBuffer>();
    service.AddBufferToQueue("dt.pcm", dumpBuffer);
    service.dumpSignal_->isRunning_.store(true);
    service.AddBufferToQueue("dt.pcm", dumpBuffer);
    EXPECT_EQ(service.dumpSignal_->dumpQueue_.size(), 1U);
    service.DumpBufferClear();
    EXPECT_TRUE(service.dumpSignal_->dumpQueue_.empty());

    std::queue<std::pair<std::string, std::shared_ptr<AudioDumpBuffer>>> writeQueue;
    writeQueue.push(std::make_pair("dt.pcm", dumpBuffer));
    service.dumpEnable_ = false;
    service.DumpBufferWrite(writeQueue);
    EXPECT_TRUE(writeQueue.empty());
    std::shared_ptr<AudioDumpBuffer> nullBuffer = nullptr;
    service.WriteBufferFromQueue("dt.pcm", nullBuffer);
    dumpBuffer->size = 1;
    service.WriteBufferFromQueue("dt.pcm", dumpBuffer);
    EXPECT_FALSE(service.DeleteHistoryFile("/path/not/exist/media_monitor_dt.pcm"));

    service.dumpType_ = DEFAULT_DUMP_TYPE;
    service.HistoryFilesHandle();
    service.isDumpExit_ = true;
    service.DumpLoopFunc();
    service.DumpThreadStop();
    service.DumpThreadExit();
}

HWTEST(MediaMonitorDtTest, MediaMonitorService_RecordDumpAndMemoBridgeBranch_001, TestSize.Level0)
{
    MediaMonitorService service(MEDIA_MONITOR_SERVICE_ID, false);
    int32_t funcResult = ERROR;
    auto &aggregate = EventAggregate::GetEventAggregate();
    aggregate.unifiedFaultCodeVector_.push_back(nullptr);
    auto fault = MakeEvent(UNIFIED_FAULT_CODE, FAULT_EVENT);
    fault->Add("ERROR_UID", 10);
    fault->Add("ERROR_CODE", 0x1234);
    AddString(fault, "ERROR_REASON", "reason");
    fault->Add("TIMESTAMP", static_cast<uint64_t>(99));
    aggregate.unifiedFaultCodeVector_.push_back(fault);
    std::vector<std::string> faultRecords;
    EXPECT_EQ(service.GetUnifiedFaultCodeRecords(faultRecords, funcResult), SUCCESS);
    ASSERT_EQ(faultRecords.size(), 1U);
    EXPECT_NE(faultRecords[0].find("uid:10"), std::string::npos);
    EXPECT_NE(faultRecords[0].find("errorCode:0x00001234"), std::string::npos);

    EXPECT_EQ(service.Dump(-1, {}), SUCCESS);
    EXPECT_EQ(service.Dump(-1, {u"-dk"}), -1);

    std::unordered_map<int32_t, MonitorDeviceInfo> preferredDevices;
    std::unordered_map<int32_t, std::vector<MonitorDeviceInfo>> excludedDevices;
    std::unordered_map<int32_t, bool> sessionMap;
    std::unordered_map<int32_t, bool> backTaskMap;
    std::unordered_map<std::string, uint32_t> collaborativeMap;
    std::vector<std::string> deviceInfos;
    std::string sceneInfo;
    std::vector<MonitorDmDeviceInfo> dmInfos;
    bool hasAudioPermission = service.VerifyIsAudio();
    EXPECT_EQ(service.GetAudioRouteMsg(preferredDevices, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetAudioExcludedDevicesMsg(excludedDevices, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetAudioAppSessionMsg(sessionMap, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetAudioAppBackTaskMsg(backTaskMap, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetDistributedDeviceInfo(deviceInfos, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetDistributedSceneInfo(sceneInfo, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.GetDmDeviceInfo(dmInfos, funcResult), hasAudioPermission ? SUCCESS : ERROR);
    EXPECT_EQ(service.ErasePreferredDeviceByType(CALL_RENDER, funcResult), ERROR);
    EXPECT_EQ(service.GetCollaborativeDeviceState(collaborativeMap, funcResult), hasAudioPermission ? SUCCESS : ERROR);
}

HWTEST(MediaMonitorDtTest, MediaMonitorManager_CallbackParameterAndProxyNullBranch_001, TestSize.Level0)
{
    MediaMonitorManager &manager = MediaMonitorManager::GetInstance();
    MediaMonitorManager::HiviewUeEnableChangeCallback("persist.hiviewdfx.hiview.ue.enable", "true", nullptr);
    EXPECT_TRUE(MediaMonitorManager::ShouldWriteLogEvent(AI_VOICE_NOISE_SUPPRESSION));
    MediaMonitorManager::HiviewUeEnableChangeCallback("persist.hiviewdfx.hiview.ue.enable", "false", nullptr);
    EXPECT_FALSE(MediaMonitorManager::ShouldWriteLogEvent(AI_VOICE_NOISE_SUPPRESSION));
    EXPECT_TRUE(MediaMonitorManager::ShouldWriteLogEvent(HEADSET_CHANGE));
    MediaMonitorManager::HiviewUeEnableChangeCallback("persist.hiviewdfx.hiview.ue.enable", nullptr, nullptr);
    MediaMonitorManager::MediaMonitorDied(0);

    auto bean = MakeEvent(HEADSET_CHANGE, BEHAVIOR_EVENT);
    manager.WriteLogMsg(bean);
    std::map<PreferredType, std::shared_ptr<MonitorDeviceInfo>> preferredDevices;
    manager.GetAudioRouteMsg(preferredDevices);
    EXPECT_TRUE(preferredDevices.empty());
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>> excludedDevices;
    manager.GetAudioExcludedDevicesMsg(excludedDevices);
    EXPECT_TRUE(excludedDevices.empty());
    std::map<int32_t, bool> sessionMap;
    manager.GetAudioAppSessionMsg(sessionMap);
    EXPECT_TRUE(sessionMap.empty());
    std::map<int32_t, bool> backTaskMap;
    manager.GetAudioAppBackTaskMsg(backTaskMap);
    EXPECT_TRUE(backTaskMap.empty());
    std::map<std::string, uint32_t> collaborativeMap;
    manager.GetCollaborativeDeviceState(collaborativeMap);
    EXPECT_TRUE(collaborativeMap.empty());
    std::vector<std::string> deviceInfos;
    manager.GetDistributedDeviceInfo(deviceInfos);
    EXPECT_TRUE(deviceInfos.empty());
    std::string sceneInfo = "old";
    manager.GetDistributedSceneInfo(sceneInfo);
    EXPECT_EQ(sceneInfo, "old");
    std::vector<MonitorDmDeviceInfo> dmInfos;
    manager.GetDmDeviceInfo(dmInfos);
    EXPECT_TRUE(dmInfos.empty());
    std::vector<std::string> faultRecords;
    manager.GetUnifiedFaultCodeRecords(faultRecords);
    EXPECT_TRUE(faultRecords.empty());
    EXPECT_EQ(manager.ErasePreferredDeviceByType(CALL_RENDER), ERROR);
}

HWTEST(MediaMonitorDtTest, MediaMonitorManager_MediaParameterBranch_001, TestSize.Level0)
{
    MediaMonitorManager &manager = MediaMonitorManager::GetInstance();
    std::vector<std::pair<std::string, std::string>> result;
    manager.versionType_ = COMMERCIAL_VERSION;
    EXPECT_EQ(manager.GetMediaParameters({DEFAULT_DUMP_TYPE}, result), ERROR);
    EXPECT_EQ(manager.SetMediaParameters({{DEFAULT_DUMP_TYPE, "true"}}), ERROR);

    manager.versionType_ = BETA_VERSION;
    manager.dumpEnable_ = false;
    EXPECT_EQ(manager.GetMediaParameters({"INVALID"}, result), ERROR);
    result.clear();
    EXPECT_EQ(manager.GetMediaParameters({DEFAULT_DUMP_TYPE}, result), SUCCESS);
    ASSERT_EQ(result.size(), 1U);
    EXPECT_EQ(result[0].second, "false");

    EXPECT_EQ(manager.SetMediaParameters({{"INVALID", "true"}}), ERROR);
    manager.dumpEnable_ = true;
    manager.dumpStartTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    EXPECT_EQ(manager.SetMediaParameters({{DEFAULT_DUMP_TYPE, "true"}}), ERROR);
    manager.dumpStartTime_ -= 91;
    EXPECT_EQ(manager.SetMediaParameters({{DEFAULT_DUMP_TYPE, "false"}}), ERROR);

    uint8_t data[4] = {};
    manager.dumpEnable_ = false;
    manager.WriteAudioBuffer("dt.pcm", data, sizeof(data));
    manager.dumpEnable_ = true;
    manager.versionType_ = COMMERCIAL_VERSION;
    manager.WriteAudioBuffer("dt.pcm", data, sizeof(data));
    manager.versionType_ = BETA_VERSION;
    manager.dumpStartTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - 91;
    manager.WriteAudioBuffer("dt.pcm", data, sizeof(data));
    EXPECT_FALSE(manager.dumpEnable_);
}

HWTEST(MediaMonitorDtTest, MediaMonitorWrapper_AndBundleCBranch_001, TestSize.Level0)
{
    BundleInfo bundleInfo;
    MediaMonitorWrapper wrapper;
    MediaMonitorErr ret = wrapper.GetBundleInfo(TEST_CLIENT_UID, &bundleInfo);
    EXPECT_TRUE(ret == SUCCESS || ret == ERR_OPERATION_FAILED || ret == ERR_INVALID_OPERATION);

    wrapper.getBundleInfoFromUid_ = GetBundleInfoSuccessStub;
    EXPECT_EQ(wrapper.GetBundleInfo(TEST_CLIENT_UID, &bundleInfo), SUCCESS);
    EXPECT_EQ(bundleInfo.appName, "stub." + std::to_string(TEST_CLIENT_UID));

    BundleInfo cBundleInfo;
    ret = ::GetBundleInfoFromUid(TEST_CLIENT_UID, &cBundleInfo);
    EXPECT_TRUE(ret == SUCCESS || ret == ERR_INVALID_OPERATION);
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
