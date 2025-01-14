/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_FILTER_BASE_H
#define HISTREAMER_PIPELINE_CORE_FILTER_BASE_H
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include "meta/meta.h"
#include "buffer/avbuffer_queue_producer.h"
#include "common/event.h"
#include "common/interrupt_monitor.h"
#include "osal/task/mutex.h"
#include "osal/task/condition_variable.h"
#include "osal/task/task.h"

namespace OHOS {
namespace Media {
namespace Pipeline {

class Filter;

enum class FilterType {
    FILTERTYPE_SOURCE,
    FILTERTYPE_DEMUXER,
    FILTERTYPE_AENC,
    FILTERTYPE_ADEC,
    FILTERTYPE_VENC,
    FILTERTYPE_VDEC,
    FILTERTYPE_MUXER,
    FILTERTYPE_VIDEODEC,
    FILTERTYPE_ASINK,
    FILTERTYPE_FSINK,
    FILTERTYPE_SSINK,
    AUDIO_CAPTURE,
    AUDIO_DATA_SOURCE,
    VIDEO_CAPTURE,
    FILTERTYPE_VIDRESIZE,
    FILTERTYPE_SEI,
    TIMED_METADATA,
    FILTERTYPE_MAX,
};

enum class StreamType {
    STREAMTYPE_PACKED,
    STREAMTYPE_ENCODED_AUDIO,
    STREAMTYPE_ENCODED_VIDEO,
    STREAMTYPE_RAW_AUDIO,
    STREAMTYPE_RAW_VIDEO,
    STREAMTYPE_SUBTITLE,
    STREAMTYPE_MAX,
};

enum class FilterState {
    CREATED,     // Filter created
    INITIALIZED, // Init called
    PREPARING,   // Prepare called
    READY,       // Ready Event reported
    RUNNING,     // Start called
    PAUSED,      // Pause called
    STOPPED,     // Stop called
    RELEASED,    // Release called
    ERROR,       // State fail
};

enum class FilterCallBackCommand {
    NEXT_FILTER_NEEDED,
    NEXT_FILTER_REMOVED,
    NEXT_FILTER_UPDATE,
    FILTER_CALLBACK_COMMAND_MAX,
};

class EventReceiver {
public:
    virtual ~EventReceiver() = default;
    virtual void OnEvent(const Event& event) = 0;
    virtual void OnDfxEvent(const DfxEvent& event)
    {
        (void)event;
    }
    virtual void NotifyRelease() {}
};

class FilterCallback {
public:
    virtual ~FilterCallback() = default;
    virtual Status OnCallback(const std::shared_ptr<Filter>& filter, FilterCallBackCommand cmd, StreamType outType) = 0;
    virtual void NotifyRelease() {}
};

class FilterLinkCallback {
public:
    virtual ~FilterLinkCallback() = default;
    virtual void OnLinkedResult(const sptr<AVBufferQueueProducer>& queue, std::shared_ptr<Meta>& meta) = 0;
    virtual void OnUnlinkedResult(std::shared_ptr<Meta>& meta) = 0;
    virtual void OnUpdatedResult(std::shared_ptr<Meta>& meta) = 0;
};

class Filter {
public:
    explicit Filter(std::string name, FilterType type, bool asyncMode = false);

    virtual ~Filter();

    virtual void Init(const std::shared_ptr<EventReceiver>& receiver, const std::shared_ptr<FilterCallback>& callback);

    // override this func if you need to use InterruptMonitor instance in your class
    virtual void Init(const std::shared_ptr<EventReceiver>& receiver, const std::shared_ptr<FilterCallback>& callback,
                      const std::shared_ptr<InterruptMonitor>& monitor);

    virtual void LinkPipeLine(const std::string &groupId) final;

    virtual Status Prepare() final;

    virtual Status Start() final;

    virtual Status Pause() final;

    virtual Status PauseDragging() final;

    virtual Status PauseAudioAlign() final;

    virtual Status Resume() final;

    virtual Status ResumeDragging() final;

    virtual Status ResumeAudioAlign() final;

    virtual Status Stop() final;

    virtual Status Flush() final;

    virtual Status Release() final;

    virtual Status Preroll() final;

    virtual Status WaitPrerollDone(bool render) final;

    virtual void StartFilterTask() final;

    virtual void PauseFilterTask() final;

    virtual Status SetPlayRange(int64_t start, int64_t end) final;

    virtual Status ProcessInputBuffer(int sendArg = 0, int64_t delayUs = 0) final;

    virtual Status ProcessOutputBuffer(int sendArg = 0, int64_t delayUs = 0, bool byIdx = false, uint32_t idx = 0,
        int64_t renderTime = -1) final;

    virtual Status WaitAllState(FilterState state) final;

    virtual Status DoInitAfterLink();

    virtual Status DoPrepare();

    virtual Status DoStart();

    virtual Status DoPause();

    virtual Status DoPauseDragging();

    virtual Status DoPauseAudioAlign();

    virtual Status DoResume();

    virtual Status DoResumeDragging();

    virtual Status DoResumeAudioAlign();

    virtual Status DoStop();

    virtual Status DoFlush();

    virtual Status DoRelease();

    virtual Status DoPreroll();

    virtual Status DoWaitPrerollDone(bool render);

    virtual Status DoSetPlayRange(int64_t start, int64_t end);

    virtual Status DoProcessInputBuffer(int recvArg, bool dropFrame);

    virtual Status DoProcessOutputBuffer(int recvArg, bool dropFrame, bool byIdx, uint32_t idx, int64_t renderTime);

    virtual void SetParameter(const std::shared_ptr<Meta>& meta);

    virtual void GetParameter(std::shared_ptr<Meta>& meta);

    virtual Status LinkNext(const std::shared_ptr<Filter>& nextFilter, StreamType outType);

    virtual Status UpdateNext(const std::shared_ptr<Filter>& nextFilter, StreamType outType);

    virtual Status UnLinkNext(const std::shared_ptr<Filter>& nextFilter, StreamType outType);

    FilterType GetFilterType();

    virtual Status OnLinked(StreamType inType, const std::shared_ptr<Meta>& meta,
                            const std::shared_ptr<FilterLinkCallback>& callback);

    virtual Status OnUpdated(StreamType inType, const std::shared_ptr<Meta>& meta,
                             const std::shared_ptr<FilterLinkCallback>& callback);

    virtual Status OnUnLinked(StreamType inType, const std::shared_ptr<FilterLinkCallback>& callback);

    virtual Status ClearAllNextFilters();

    virtual Status SetMuted(bool isMuted)
    {
        (void)isMuted;
        return Status::OK;
    }
protected:
    std::string name_;

    std::shared_ptr<Meta> meta_;

    FilterType filterType_;

    std::vector<StreamType> supportedInStreams_;
    std::vector<StreamType> supportedOutStreams_;

    std::map<StreamType, std::vector<std::shared_ptr<Filter>>> nextFiltersMap_;

    std::shared_ptr<EventReceiver> receiver_;

    std::shared_ptr<FilterCallback> callback_;

    std::shared_ptr<InterruptMonitor> interruptMonitor_;

    std::map<StreamType, std::vector<std::shared_ptr<FilterLinkCallback>>> linkCallbackMaps_;

    std::string groupId_;

private:
    void ChangeState(FilterState state);

    void SetErrCode(Status errCode);

    Status GetErrCode();

    Status PrepareDone();

    Status StartDone();

    Status PauseDone();

    Status ResumeDone();

    Status StopDone();

    Status ReleaseDone();

    OHOS::Media::Mutex stateMutex_{};

    OHOS::Media::ConditionVariable cond_{};

    FilterState curState_{FilterState::CREATED};

    Status errCode_ = Status::OK;

    int64_t jobIdx_ = 0;

    int64_t processIdx_ = 0;

    int64_t jobIdxBase_ = 0;

    std::unique_ptr<Task> filterTask_;

    bool isAsyncMode_;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif
