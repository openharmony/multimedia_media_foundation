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

#ifdef VIDEO_SUPPORT
#define SHOW_FRAME_RATE 0
#define HST_LOG_TAG "VideoSinkFilter"

#include "pipeline/filters/sink/video_sink/video_sink_filter.h"
#include "foundation/log.h"
#include "foundation/osal/utils/util.h"
#include "foundation/utils/steady_clock.h"
#include "pipeline/factory/filter_factory.h"
#include "pipeline/filters/common/plugin_settings.h"
#include "pipeline/filters/common/plugin_utils.h"
#include "plugin/common/plugin_time.h"
#ifndef OHOS_LITE
#include "plugin/common/surface_allocator.h"
#endif

namespace OHOS {
namespace Media {
namespace Pipeline {
namespace {
    const uint32_t VSINK_DEFAULT_BUFFER_NUM = 8;
    const uint32_t DEFAULT_FRAME_RATE = 30;
}
static AutoRegisterFilter<VideoSinkFilter> g_registerFilterHelper("builtin.player.videosink");

VideoSinkFilter::VideoSinkFilter(const std::string& name) : MediaSynchronousSink(name)
{
    refreshTime_ = 0;
    syncerPriority_ = IMediaSynchronizer::VIDEO_SINK;
    MEDIA_LOG_I("VideoSinkFilter ctor called...");
}

VideoSinkFilter::~VideoSinkFilter()
{
    MEDIA_LOG_D("VideoSinkFilter deCtor.");
    if (renderTask_) {
        renderTask_->Stop();
    }
    if (frameRateTask_) {
        frameRateTask_->Stop();
    }
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
    if (inBufQueue_ != nullptr) {
        inBufQueue_->SetActive(false);
        inBufQueue_.reset();
    }
}

void VideoSinkFilter::Init(EventReceiver* receiver, FilterCallback* callback)
{
    MediaSynchronousSink::Init(receiver, callback);
    outPorts_.clear();
    if (inBufQueue_ == nullptr) {
        inBufQueue_ = std::make_shared<BlockingQueue<AVBufferPtr>>("VideoSinkInBufQue", VSINK_DEFAULT_BUFFER_NUM);
    }
    if (renderTask_ == nullptr) {
        renderTask_ = std::make_shared<OHOS::Media::OSAL::Task>("VideoSinkRenderThread");
        renderTask_->RegisterHandler([this] { RenderFrame(); });
    }
#if(SHOW_FRAME_RATE)
    if (frameRateTask_ == nullptr) {
        frameRateTask_ = std::make_shared<OHOS::Media::OSAL::Task>("CalcFrameRateThread");
        frameRateTask_->RegisterHandler([this] { CalcFrameRate(); });
    }
#endif
}

ErrorCode VideoSinkFilter::SetParameter(int32_t key, const Plugin::Any& value)
{
    Tag tag = Tag::INVALID;
    if (!TranslateIntoParameter(key, tag)) {
        MEDIA_LOG_I("SetParameter key " PUBLIC_LOG_D32 " is out of boundary", key);
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    switch (tag) {
        case Tag::VIDEO_SCALE_TYPE:
            FALSE_RETURN_V_MSG_E(Plugin::Any::IsSameTypeWith<Plugin::VideoScaleType>(value),
                                 ErrorCode::ERROR_INVALID_PARAMETER_TYPE,
                                 "VIDEO_SCALE_TYPE type should be Plugin::VideoScaleType");
            videoScaleType_ = Plugin::AnyCast<Plugin::VideoScaleType>(value);
            if (plugin_) {
                (void)plugin_->SetParameter(Tag::VIDEO_SCALE_TYPE, videoScaleType_);
            }
            break;
        default:
            break;
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoSinkFilter::GetParameter(int32_t key, Plugin::Any& value)
{
    if (state_.load() == FilterState::CREATED) {
        return ErrorCode::ERROR_AGAIN;
    }
    Tag tag = Tag::INVALID;
    if (!TranslateIntoParameter(key, tag)) {
        MEDIA_LOG_I("GetParameter key " PUBLIC_LOG_D32 "is out of boundary", key);
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    RETURN_AGAIN_IF_NULL(plugin_);
    return TranslatePluginStatus(plugin_->GetParameter(tag, value));
}

void VideoSinkFilter::HandleNegotiateParams(const Plugin::Meta& upstreamParams, Plugin::Meta& downstreamParams)
{
#ifndef OHOS_LITE
    MEDIA_LOG_I("Enter set surface buffer");
    Plugin::Tag tag = Plugin::Tag::VIDEO_MAX_SURFACE_NUM;
    auto ite = upstreamParams.Find(tag);
    if (ite != std::end(upstreamParams)) {
        if (Plugin::Any::IsSameTypeWith<uint32_t>(ite->second)) {
            auto ret = plugin_->SetParameter(tag, Plugin::AnyCast<uint32_t>(ite->second));
            if (ret != Plugin::Status::OK) {
                MEDIA_LOG_W("Set max surface num to plugin fail");
            }
        }
    }
    auto pluginAllocator = plugin_->GetAllocator();
    if (pluginAllocator != nullptr && pluginAllocator->GetMemoryType() == Plugin::MemoryType::SURFACE_BUFFER) {
        MEDIA_LOG_D("plugin provide surface allocator");
        auto allocator = Plugin::ReinterpretPointerCast<Plugin::SurfaceAllocator>(pluginAllocator);
        downstreamParams.Set<Tag::BUFFER_ALLOCATOR>(allocator);
    }
#endif
}

bool VideoSinkFilter::CreateVideoSinkPlugin(const std::shared_ptr<Plugin::PluginInfo>& selectedPluginInfo)
{
    if (plugin_ != nullptr) {
        if (pluginInfo_ != nullptr && pluginInfo_->name == selectedPluginInfo->name) {
            if (plugin_->Reset() == Plugin::Status::OK) {
                return true;
            }
            MEDIA_LOG_W("reuse previous plugin " PUBLIC_LOG_S " failed, will create new plugin",
                        pluginInfo_->name.c_str());
        }
        plugin_->Deinit();
    }
    plugin_ = Plugin::PluginManager::Instance().CreateVideoSinkPlugin(selectedPluginInfo->name);
    if (plugin_ == nullptr) {
        MEDIA_LOG_E("cannot create plugin " PUBLIC_LOG_S, selectedPluginInfo->name.c_str());
        return false;
    }
#ifndef OHOS_LITE
    (void)plugin_->SetParameter(Tag::VIDEO_SCALE_TYPE, videoScaleType_);
    if (surface_ != nullptr) {
        auto ret = TranslatePluginStatus(plugin_->SetParameter(Tag::VIDEO_SURFACE, surface_));
        if (ret != ErrorCode::SUCCESS) {
            MEDIA_LOG_W("Set surface to plugin fail, ret: " PUBLIC_LOG_U32, ret);
            return false;
        }
    }
#endif
    auto err = TranslatePluginStatus(plugin_->Init());
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("plugin " PUBLIC_LOG_S " init error", selectedPluginInfo->name.c_str());
        return false;
    }
    pluginInfo_ = selectedPluginInfo;
    return true;
}

bool VideoSinkFilter::Negotiate(const std::string& inPort,
                                const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                Plugin::Capability& negotiatedCap,
                                const Plugin::Meta& upstreamParams,
                                Plugin::Meta& downstreamParams)
{
    PROFILE_BEGIN("video sink negotiate start");
    if (state_ != FilterState::PREPARING) {
        MEDIA_LOG_W("Video sink filter is not in preparing when negotiate");
        return false;
    }
    auto candidatePlugins = FindAvailablePlugins(*upstreamCap, Plugin::PluginType::VIDEO_SINK);
    if (candidatePlugins.empty()) {
        MEDIA_LOG_E("no available video sink plugin");
        return false;
    }
    // always use first one
    std::shared_ptr<Plugin::PluginInfo> selectedPluginInfo = candidatePlugins[0].first;
    MEDIA_LOG_I("select plugin " PUBLIC_LOG_S, selectedPluginInfo->name.c_str());
    for (const auto& onCap : selectedPluginInfo->inCaps) {
        if (onCap.keys.count(CapabilityID::VIDEO_PIXEL_FORMAT) == 0) {
            MEDIA_LOG_E("each in caps of sink must contains valid video pixel format");
            return false;
        }
    }
    negotiatedCap = candidatePlugins[0].second;
    if (!CreateVideoSinkPlugin(selectedPluginInfo)) {
        return false;
    }
    PROFILE_END("video sink negotiate end");
    MEDIA_LOG_D("video sink negotiate success");
    return true;
}

bool VideoSinkFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta,
                                Plugin::Meta& upstreamParams, Plugin::Meta& downstreamParams)
{
    PROFILE_BEGIN("video sink configure start");
    if (plugin_ == nullptr || pluginInfo_ == nullptr) {
        MEDIA_LOG_E("cannot configure decoder when no plugin available");
        return false;
    }
    auto err = ConfigurePluginToStartNoLocked(upstreamMeta);
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("sink configure error");
        OnEvent(Event{name_, EventType::EVENT_ERROR, {err}});
        return false;
    }
    if (!upstreamMeta->Get<Plugin::Tag::VIDEO_FRAME_RATE>(frameRate_)) {
        MEDIA_LOG_I("frame rate is not found");
    }
    if (frameRate_ == 0) {
        frameRate_ = DEFAULT_FRAME_RATE;
    }
    waitPrerolledTimeout_ = 1000 / frameRate_; // 1s = 1000ms
    UpdateMediaTimeRange(*upstreamMeta);
    HandleNegotiateParams(upstreamParams, downstreamParams);
    state_ = FilterState::READY;
    OnEvent(Event{name_, EventType::EVENT_READY, {}});
    MEDIA_LOG_I("video sink send EVENT_READY");
    PROFILE_END("video sink configure end");
    return true;
}

ErrorCode VideoSinkFilter::ConfigurePluginParams(const std::shared_ptr<const Plugin::Meta>& meta)
{
    auto err = ErrorCode::SUCCESS;
    uint32_t width;
    if (meta->Get<Plugin::Tag::VIDEO_WIDTH>(width)) {
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::VIDEO_WIDTH, width));
        FAIL_RETURN_MSG(err, "Set plugin width fail");
    } else {
        MEDIA_LOG_W("Get VIDEO_WIDTH failed.");
    }
    uint32_t height;
    if (meta->Get<Plugin::Tag::VIDEO_HEIGHT>(height)) {
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::VIDEO_HEIGHT, height));
        FAIL_RETURN_MSG(err, "Set plugin height fail");
    } else {
        MEDIA_LOG_W("Get VIDEO_HEIGHT fail");
    }
    Plugin::VideoPixelFormat pixelFormat;
    if (meta->Get<Plugin::Tag::VIDEO_PIXEL_FORMAT>(pixelFormat)) {
        err = TranslatePluginStatus(plugin_->SetParameter(Tag::VIDEO_PIXEL_FORMAT, pixelFormat));
        FAIL_RETURN_MSG(err, "Set plugin pixel format fail");
    } else {
        MEDIA_LOG_W("Get VIDEO_PIXEL_FORMAT fail");
    }
    MEDIA_LOG_D("width: " PUBLIC_LOG_U32 ", height: " PUBLIC_LOG_U32 ", pixelFormat: " PUBLIC_LOG_U32,
                width, height, pixelFormat);
    return err;
}

ErrorCode VideoSinkFilter::ConfigurePluginToStartNoLocked(const std::shared_ptr<const Plugin::Meta>& meta)
{
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Init()), "Init plugin error");
    plugin_->SetCallback(this);
    FAIL_RETURN_MSG(ConfigurePluginParams(meta), "Configure plugin params fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Prepare()), "Prepare plugin fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Start()), "Start plugin fail");
    return ErrorCode::SUCCESS;
}

void VideoSinkFilter::RenderFrame()
{
    MEDIA_LOG_DD("RenderFrame called, inBufQue size: " PUBLIC_LOG_ZU, inBufQueue_->Size());
    auto frameBuffer = inBufQueue_->Pop();
    if (frameBuffer == nullptr) {
        MEDIA_LOG_D("Video sink find nullptr in esBufferQ");
        return;
    }
    auto err = WriteToPluginRefTimeSync(frameBuffer);
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("Write to plugin fail: " PUBLIC_LOG_U32 ", buf.pts: " PUBLIC_LOG_D64, err, frameBuffer->pts);
        return;
    }
    if (frameBuffer->flag & BUFFER_FLAG_EOS) {
        Event event{
            .srcFilter = name_,
            .type = EventType::EVENT_COMPLETE,
        };
        OnEvent(event);
        MEDIA_LOG_D("Video sink render frame send event_complete end.");
        renderTask_->PauseAsync();
    }
    MEDIA_LOG_DD("RenderFrame success");
}

ErrorCode VideoSinkFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    MEDIA_LOG_DD("video sink push data started, state_: " PUBLIC_LOG_D32, state_.load());
    if (isFlushing_ || state_.load() == FilterState::INITIALIZED) {
        MEDIA_LOG_I("video sink is flushing ignore this buffer");
        return ErrorCode::SUCCESS;
    }
    if (state_.load() != FilterState::RUNNING) {
        pushThreadIsBlocking_ = true;
        OSAL::ScopedLock lock(mutex_);
        startWorkingCondition_.Wait(lock, [this] {
            return state_ == FilterState::RUNNING || state_ == FilterState::INITIALIZED || isFlushing_;
        });
        pushThreadIsBlocking_ = false;
    }
    if (isFlushing_ || state_.load() == FilterState::INITIALIZED) {
        MEDIA_LOG_I("PushData return due to: isFlushing_ = " PUBLIC_LOG_D32 ", state_ = " PUBLIC_LOG_D32,
                    isFlushing_.load(), static_cast<int>(state_.load()));
        return ErrorCode::SUCCESS;
    }
    inBufQueue_->Push(buffer);
    MEDIA_LOG_DD("video sink push data end, buffer->pts: " PUBLIC_LOG_D64, buffer->pts);
    return ErrorCode::SUCCESS;
}

ErrorCode VideoSinkFilter::Start()
{
    MEDIA_LOG_DD("start called");
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED) {
        MEDIA_LOG_W("sink is not ready when start, state_: " PUBLIC_LOG_D32, state_.load());
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    inBufQueue_->SetActive(true);
    renderTask_->Start();
    auto err = FilterBase::Start();
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("Video sink filter start error");
        return err;
    }
    plugin_->Start();
    if (pushThreadIsBlocking_.load()) {
        startWorkingCondition_.NotifyOne();
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoSinkFilter::Stop()
{
    MEDIA_LOG_I("VideoSinkFilter stop called.");
    FAIL_RETURN_MSG(FilterBase::Stop(), "Video sink stop fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Stop()), "Stop plugin fail");
    if (pushThreadIsBlocking_.load()) {
        startWorkingCondition_.NotifyOne();
    }
    inBufQueue_->SetActive(false);
    renderTask_->Stop();
    if (frameRateTask_) {
        frameRateTask_->Stop();
    }
    return ErrorCode::SUCCESS;
}

ErrorCode VideoSinkFilter::Pause()
{
    MEDIA_LOG_D("Video sink filter pause start");
    if (state_ != FilterState::READY && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("video sink cannot pause when not working");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    FAIL_RETURN_MSG(FilterBase::Pause(), "Video sink pause fail");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Pause()), "Pause plugin fail");
    inBufQueue_->SetActive(false);
    renderTask_->Pause();
    if (frameRateTask_) {
        frameRateTask_->Pause();
    }
    MEDIA_LOG_D("Video sink filter pause end");
    return ErrorCode::SUCCESS;
}

ErrorCode VideoSinkFilter::Resume()
{
    MEDIA_LOG_D("Video sink filter resume");
    // only worked when state_ is paused
    if (state_ == FilterState::PAUSED) {
        forceRenderNextFrame_ = true;
        state_ = FilterState::RUNNING;
        if (pushThreadIsBlocking_) {
            startWorkingCondition_.NotifyOne();
        }
        auto err = TranslatePluginStatus(plugin_->Resume());
        if (err != ErrorCode::SUCCESS) {
            return err;
        }
        inBufQueue_->SetActive(true);
        renderTask_->Start();
        if (frameRateTask_) {
            frameRateTask_->Start();
        }
        renderFrameCnt_ = 0;
        discardFrameCnt_ = 0;
    }
    return ErrorCode::SUCCESS;
}

void VideoSinkFilter::FlushStart()
{
    MEDIA_LOG_D("FlushStart entered");
    isFlushing_ = true;
    if (pushThreadIsBlocking_) {
        startWorkingCondition_.NotifyOne();
    }
    if (inBufQueue_) {
        inBufQueue_->SetActive(false);
    }
    renderTask_->Pause();
    auto err = TranslatePluginStatus(plugin_->Flush());
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("Video sink filter flush failed.");
    }
}

void VideoSinkFilter::FlushEnd()
{
    MEDIA_LOG_D("FlushEnd entered");
    isFlushing_ = false;
    if (inBufQueue_) {
        inBufQueue_->SetActive(true);
    }
    renderTask_->Start();
    ResetSyncInfo();
    renderFrameCnt_ = 0;
    discardFrameCnt_ = 0;
}

#ifndef OHOS_LITE
ErrorCode VideoSinkFilter::SetVideoSurface(sptr<Surface> surface)
{
    if (!surface) {
        MEDIA_LOG_W("surface is null");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (plugin_) {
        auto ret = TranslatePluginStatus(plugin_->SetParameter(Tag::VIDEO_SURFACE, surface));
        if (ret != ErrorCode::SUCCESS) {
            MEDIA_LOG_W("Set surface to plugin fail");
            return ret;
        }
    } else {
        surface_ = surface;
    }
    MEDIA_LOG_D("SetVideoSurface success");
    return ErrorCode::SUCCESS;
}
#endif

bool VideoSinkFilter::CheckBufferLatenessMayWait(AVBufferPtr buffer)
{
    auto syncCenter = syncCenter_.lock();
    if (!syncCenter) {
        return false;
    }
    auto ct4Buffer = syncCenter->GetClockTime(buffer->pts);
    if (ct4Buffer != HST_TIME_NONE) {
        bool tooLate = false;
        auto nowCt = syncCenter->GetClockTimeNow();
        uint64_t latency = 0;
        plugin_->GetLatency(latency);
        auto diff = nowCt + (int64_t) latency - ct4Buffer;
        // diff < 0 or 0 < diff < 40ms(25Hz) render it
        if (diff < 0) {
            // buffer is early
            auto waitTimeMs = Plugin::HstTime2Ms(0 - diff);
            MEDIA_LOG_DD("buffer is eary, sleep for " PUBLIC_LOG_D64 " ms", waitTimeMs);
            OSAL::SleepFor(waitTimeMs);
        } else if (diff > 0 && Plugin::HstTime2Ms(diff) > 40) { // > 40ms
            // buffer is late
            tooLate = true;
            MEDIA_LOG_DD("buffer is too late");
        }
        // buffer is too late and is not key frame drop it
        if (tooLate && (buffer->flag & BUFFER_FLAG_KEY_FRAME) == 0) {
            return true;
        }
    }
    return false;
}

ErrorCode VideoSinkFilter::DoSyncWrite(const AVBufferPtr& buffer)
{
    bool shouldDrop = false;
    bool render = true;
    if ((buffer->flag & BUFFER_FLAG_EOS) == 0) {
        if (isFirstFrame_) {
            int64_t nowCt = 0;
            auto syncCenter = syncCenter_.lock();
            if (syncCenter) {
                nowCt = syncCenter->GetClockTimeNow();
            }
            uint64_t latency = 0;
            if (plugin_->GetLatency(latency) != Plugin::Status::OK) {
                MEDIA_LOG_I("failed to get latency, treat as 0");
            }
            if (syncCenter) {
                render = syncCenter->UpdateTimeAnchor(nowCt + latency, buffer->pts, this);
            }
            isFirstFrame_ = false;
            OnEvent(Event{name_, EventType::EVENT_VIDEO_RENDERING_START, {}});
            if (frameRateTask_) {
                frameRateTask_->Start();
            }
        } else {
            shouldDrop = CheckBufferLatenessMayWait(buffer);
        }
        if (forceRenderNextFrame_) {
            shouldDrop = false;
            forceRenderNextFrame_ = false;
        }
    }
    if (shouldDrop) {
        discardFrameCnt_++;
        MEDIA_LOG_DD("drop buffer with pts " PUBLIC_LOG_D64 " due to too late", buffer->pts);
        return ErrorCode::SUCCESS;
    } else if (!render) {
        discardFrameCnt_++;
        MEDIA_LOG_DD("drop buffer with pts " PUBLIC_LOG_D64 " due to seek not need to render", buffer->pts);
        return ErrorCode::SUCCESS;
    } else {
        renderFrameCnt_++;
        return TranslatePluginStatus(plugin_->Write(buffer));
    }
}

void VideoSinkFilter::ResetSyncInfo()
{
    ResetPrerollReported();
    isFirstFrame_ = true;
}

void VideoSinkFilter::CalcFrameRate()
{
    OSAL::SleepFor(1000); // 1000ms
    MEDIA_LOG_I("Render fps: " PUBLIC_LOG_U64 ", discard frame count: " PUBLIC_LOG_U64,
                renderFrameCnt_.load(), discardFrameCnt_.load());
    renderFrameCnt_ = 0;
    discardFrameCnt_ = 0;
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif