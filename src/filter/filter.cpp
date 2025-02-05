/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "Filter"
#define MEDIA_PIPELINE

#include "filter/filter.h"
#include "osal/utils/util.h"
#include "common/log.h"
#include <algorithm>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "Filter" };
constexpr uint32_t THREAD_PRIORITY_41 = 7;
}

namespace OHOS {
namespace Media {
namespace Pipeline {
Filter::Filter(std::string name, FilterType type, bool isAyncMode)
    : name_(std::move(name)), filterType_(type), isAsyncMode_(isAyncMode)
{
}

Filter::~Filter()
{
    nextFiltersMap_.clear();
}

void Filter::Init(const std::shared_ptr<EventReceiver>& receiver, const std::shared_ptr<FilterCallback>& callback)
{
    receiver_ = receiver;
    callback_ = callback;
}

void Filter::Init(const std::shared_ptr<EventReceiver>& receiver,
                  const std::shared_ptr<FilterCallback>& callback, const std::shared_ptr<InterruptMonitor>& monitor)
{
    Init(receiver, callback);
    interruptMonitor_ = monitor;
}

void Filter::LinkPipeLine(const std::string &groupId, bool needTurbo)
{
    groupId_ = groupId;
    MEDIA_LOG_I("Filter %{public}s LinkPipeLine:%{public}s, isAsyncMode_:%{public}d",
        name_.c_str(), groupId.c_str(), isAsyncMode_);
    if (isAsyncMode_) {
        TaskType taskType;
        switch (filterType_) {
            case FilterType::FILTERTYPE_VENC:
            case FilterType::FILTERTYPE_VDEC:
            case FilterType::VIDEO_CAPTURE:
                taskType = TaskType::SINGLETON;
                break;
            case FilterType::FILTERTYPE_ASINK:
            case FilterType::AUDIO_CAPTURE:
                taskType = TaskType::AUDIO;
                break;
            default:
                taskType = TaskType::SINGLETON;
                break;
        }
        filterTask_ = std::make_unique<Task>(name_, groupId_, taskType, TaskPriority::HIGH, false);
        if (needTurbo) {
            filterTask_->UpdateThreadPriority(THREAD_PRIORITY_41, "media_service");
        }
        filterTask_->SubmitJobOnce([this] {
            Status ret = DoInitAfterLink();
            SetErrCode(ret);
            ChangeState(ret == Status::OK ? FilterState::INITIALIZED : FilterState::ERROR);
        });
    } else {
        Status ret = DoInitAfterLink();
        SetErrCode(ret);
        ChangeState(ret == Status::OK ? FilterState::INITIALIZED : FilterState::ERROR);
    }
}

Status Filter::Prepare()
{
    MEDIA_LOG_D("Prepare %{public}s, pState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this] {
            PrepareDone();
        });
    } else {
        return PrepareDone();
    }
    return Status::OK;
}

Status Filter::PrepareDone()
{
    MEDIA_LOG_I("Prepare enter %{public}s", name_.c_str());
    if (curState_ == FilterState::ERROR) {
        // if DoInitAfterLink error, do not prepare.
        return Status::ERROR_INVALID_OPERATION;
    }
    // next filters maybe added in DoPrepare, so we must DoPrepare first
    Status ret = DoPrepare();
    SetErrCode(ret);
    if (ret != Status::OK) {
        ChangeState(FilterState::ERROR);
        return ret;
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto ret = filter->Prepare();
            if (ret != Status::OK) {
                return ret;
            }
        }
    }
    ChangeState(FilterState::READY);
    return ret;
}

Status Filter::Start()
{
    MEDIA_LOG_D("Start %{public}s, pState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this] {
            StartDone();
            filterTask_->Start();
        });
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Start();
            }
        }
    } else {
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Start();
            }
        }
        return StartDone();
    }
    return Status::OK;
}

Status Filter::StartDone()
{
    MEDIA_LOG_I("Start in %{public}s", name_.c_str());
    Status ret = DoStart();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RUNNING : FilterState::ERROR);
    return ret;
}

Status Filter::Pause()
{
    MEDIA_LOG_D("Pause %{public}s, pState:%{public}d", name_.c_str(), curState_);
    // In offload case, we need pause to interrupt audio_sink_plugin write function,  so do not use asyncmode
    auto ret = PauseDone();
    if (filterTask_) {
        filterTask_->Pause();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Pause();
        }
    }
    return ret;
}

Status Filter::PauseDragging()
{
    MEDIA_LOG_D("PauseDragging %{public}s, pState:%{public}d", name_.c_str(), curState_);
    auto ret = DoPauseDragging();
    if (filterTask_) {
        filterTask_->Pause();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto curRet = filter->PauseDragging();
            if (curRet != Status::OK) {
                ret = curRet;
            }
        }
    }
    return ret;
}

Status Filter::PauseAudioAlign()
{
    MEDIA_LOG_D("PauseAudioAlign %{public}s, pState:%{public}d", name_.c_str(), curState_);
    auto ret = DoPauseAudioAlign();
    if (filterTask_) {
        filterTask_->Pause();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto curRet = filter->PauseAudioAlign();
            if (curRet != Status::OK) {
                ret = curRet;
            }
        }
    }
    return ret;
}

Status Filter::PauseDone()
{
    MEDIA_LOG_I("Pause in %{public}s", name_.c_str());
    Status ret = DoPause();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::PAUSED : FilterState::ERROR);
    return ret;
}

Status Filter::Resume()
{
    MEDIA_LOG_D("Resume %{public}s, pState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this]() {
            ResumeDone();
            filterTask_->Start();
        });
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Resume();
            }
        }
    } else {
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Resume();
            }
        }
        return ResumeDone();
    }
    return Status::OK;
}

Status Filter::ResumeDone()
{
    MEDIA_LOG_I("Resume in %{public}s", name_.c_str());
    Status ret = DoResume();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RUNNING : FilterState::ERROR);
    return ret;
}

Status Filter::ResumeDragging()
{
    MEDIA_LOG_D("ResumeDragging %{public}s, pState:%{public}d", name_.c_str(), curState_);
    auto ret = Status::OK;
    ret = DoResumeDragging();
    if (filterTask_) {
        filterTask_->Start();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto curRet = filter->ResumeDragging();
            if (curRet != Status::OK) {
                ret = curRet;
            }
        }
    }
    return ret;
}

Status Filter::ResumeAudioAlign()
{
    MEDIA_LOG_D("ResumeAudioAlign %{public}s, pState:%{public}d", name_.c_str(), curState_);
    auto ret = Status::OK;
    ret = DoResumeAudioAlign();
    if (filterTask_) {
        filterTask_->Start();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto curRet = filter->ResumeAudioAlign();
            if (curRet != Status::OK) {
                ret = curRet;
            }
        }
    }
    return ret;
}

Status Filter::Stop()
{
    MEDIA_LOG_D("Stop %{public}s, pState:%{public}d", name_.c_str(), curState_);
    // In offload case, we need stop to interrupt audio_sink_plugin write function,  so do not use asyncmode
    auto ret = StopDone();
    if (filterTask_) {
        filterTask_->Stop();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Stop();
        }
    }
    return ret;
}

Status Filter::StopDone()
{
    MEDIA_LOG_I("Stop in %{public}s", name_.c_str());
    Status ret = DoStop();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::STOPPED : FilterState::ERROR);
    return ret;
}

Status Filter::Flush()
{
    MEDIA_LOG_D("Flush %{public}s, pState:%{public}d", name_.c_str(), curState_);
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Flush();
        }
    }
    jobIdxBase_ = jobIdx_;
    return DoFlush();
}

Status Filter::Release()
{
    MEDIA_LOG_D("Release %{public}s, pState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this]() {
            ReleaseDone();
        });
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Release();
            }
        }
    } else {
        for (auto iter : nextFiltersMap_) {
            for (auto filter : iter.second) {
                filter->Release();
            }
        }
        return ReleaseDone();
    }
    return Status::OK;
}

Status Filter::ReleaseDone()
{
    MEDIA_LOG_I("Release in %{public}s", name_.c_str());
    Status ret = DoRelease();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RELEASED : FilterState::ERROR);
    return ret;
}

Status Filter::ClearAllNextFilters()
{
    nextFiltersMap_.clear();
    return Status::OK;
}

Status Filter::SetPlayRange(int64_t start, int64_t end)
{
    MEDIA_LOG_D("SetPlayRange %{public}s, pState:%{public}ld", name_.c_str(), curState_);
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->SetPlayRange(start, end);
        }
    }
    return DoSetPlayRange(start, end);
}

Status Filter::Preroll()
{
    Status ret = DoPreroll();
    if (ret != Status::OK) {
        return ret;
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            ret = filter->Preroll();
            if (ret != Status::OK) {
                return ret;
            }
        }
    }
    return Status::OK;
}

Status Filter::WaitPrerollDone(bool render)
{
    Status ret = Status::OK;
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto curRet = filter->WaitPrerollDone(render);
            if (curRet != Status::OK) {
                ret = curRet;
            }
        }
    }
    auto curRet = DoWaitPrerollDone(render);
    if (curRet != Status::OK) {
        ret = curRet;
    }
    return ret;
}

void Filter::StartFilterTask()
{
    if (filterTask_) {
        filterTask_->Start();
    }
}

void Filter::PauseFilterTask()
{
    if (filterTask_) {
        filterTask_->Pause();
    }
}

Status Filter::ProcessInputBuffer(int sendArg, int64_t delayUs)
{
    MEDIA_LOG_D("Filter::ProcessInputBuffer  %{public}s", name_.c_str());
    FALSE_RETURN_V_MSG(!isAsyncMode_ || filterTask_, Status::ERROR_INVALID_OPERATION, "no filterTask in async mode");
    if (filterTask_) {
        jobIdx_++;
        filterTask_->SubmitJob([this, sendArg]() {
            processIdx_++;
            DoProcessInputBuffer(sendArg, processIdx_ <= jobIdxBase_);  // drop frame after flush
            }, delayUs, false);
    } else {
        Task::SleepInTask(delayUs / 1000); // 1000 convert to ms
        DoProcessInputBuffer(sendArg, false);
    }
    return Status::OK;
}

Status Filter::ProcessOutputBuffer(int sendArg, int64_t delayUs, bool byIdx, uint32_t idx, int64_t renderTime)
{
    MEDIA_LOG_D("Filter::ProcessOutputBuffer  %{public}s", name_.c_str());
    FALSE_RETURN_V_MSG(!isAsyncMode_ || filterTask_, Status::ERROR_INVALID_OPERATION, "no filterTask in async mode");
    if (filterTask_) {
        jobIdx_++;
        int64_t processIdx = jobIdx_;
        filterTask_->SubmitJob([this, sendArg, processIdx, byIdx, idx, renderTime]() {
            processIdx_++;
            // drop frame after flush
            DoProcessOutputBuffer(sendArg, processIdx <= jobIdxBase_, byIdx, idx, renderTime);
            }, delayUs, false);
    } else {
        Task::SleepInTask(delayUs / 1000); // 1000 convert to ms
        DoProcessOutputBuffer(sendArg, false, false, idx, renderTime);
    }
    return Status::OK;
}

Status Filter::DoInitAfterLink()
{
    MEDIA_LOG_I("Filter::DoInitAfterLink");
    return Status::OK;
}

Status Filter::DoPrepare()
{
    return Status::OK;
}

Status Filter::DoStart()
{
    return Status::OK;
}

Status Filter::DoPause()
{
    return Status::OK;
}

Status Filter::DoPauseDragging()
{
    return Status::OK;
}

Status Filter::DoPauseAudioAlign()
{
    return Status::OK;
}

Status Filter::DoResume()
{
    return Status::OK;
}

Status Filter::DoResumeDragging()
{
    return Status::OK;
}

Status Filter::DoResumeAudioAlign()
{
    return Status::OK;
}

Status Filter::DoStop()
{
    return Status::OK;
}

Status Filter::DoFlush()
{
    return Status::OK;
}

Status Filter::DoRelease()
{
    return Status::OK;
}

Status Filter::DoPreroll()
{
    return Status::OK;
}

Status Filter::DoWaitPrerollDone(bool render)
{
    return Status::OK;
}

Status Filter::DoSetPlayRange(int64_t start, int64_t end)
{
    return Status::OK;
}

Status Filter::DoProcessInputBuffer(int recvArg, bool dropFrame)
{
    return Status::OK;
}

Status Filter::DoProcessOutputBuffer(int recvArg, bool dropFrame, bool byIdx, uint32_t idx, int64_t renderTimee)
{
    return Status::OK;
}

void Filter::ChangeState(FilterState state)
{
    AutoLock lock(stateMutex_);
    curState_ = curState_ == FilterState::ERROR ? FilterState::ERROR : state;
    MEDIA_LOG_I("%{public}s > %{public}d", name_.c_str(), curState_);
    cond_.NotifyOne();
}

Status Filter::WaitAllState(FilterState state)
{
    AutoLock lock(stateMutex_);
    MEDIA_LOG_I("%{public}s wait %{public}d", name_.c_str(), state);
    if (curState_ != state) {
        bool result = cond_.WaitFor(lock, 30000, [this, state] { // 30000 ms timeout
            return curState_ == state || curState_ == FilterState::ERROR;
        });
        if (!result) {
            SetErrCode(Status::ERROR_TIMED_OUT);
            return Status::ERROR_TIMED_OUT;
        }
        if (curState_ != state) {
            MEDIA_LOG_E("Filter(%{public}s) wait state %{public}d fail, curState %{public}d",
                name_.c_str(), state, curState_);
            return GetErrCode();
        }
    }

    Status res = Status::OK;
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            if (filter->WaitAllState(state) != Status::OK) {
                res = filter->GetErrCode();
            }
        }
    }
    return res;
}

void Filter::SetErrCode(Status errCode)
{
    errCode_ = errCode;
}

Status Filter::GetErrCode()
{
    return errCode_;
}

void Filter::SetParameter(const std::shared_ptr<Meta>& meta)
{
    meta_ = meta;
}

void Filter::GetParameter(std::shared_ptr<Meta>& meta)
{
    meta = meta_;
}

Status Filter::LinkNext(const std::shared_ptr<Filter>&, StreamType)
{
    return Status::OK;
}

Status Filter::UpdateNext(const std::shared_ptr<Filter>&, StreamType)
{
    return Status::OK;
}

Status Filter::UnLinkNext(const std::shared_ptr<Filter>&, StreamType)
{
    return Status::OK;
}

FilterType Filter::GetFilterType()
{
    return filterType_;
};

Status Filter::OnLinked(StreamType, const std::shared_ptr<Meta>&, const std::shared_ptr<FilterLinkCallback>&)
{
    return Status::OK;
};

Status Filter::OnUpdated(StreamType, const std::shared_ptr<Meta>&, const std::shared_ptr<FilterLinkCallback>&)
{
    return Status::OK;
}

Status Filter::OnUnLinked(StreamType, const std::shared_ptr<FilterLinkCallback>&)
{
    return Status::OK;
}

} // namespace Pipeline
} // namespace Media
} // namespace OHOS
