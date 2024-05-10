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

#define HST_LOG_TAG "Filter"

#include "filter/filter.h"
#include "osal/utils/util.h"
#include "common/log.h"
#include <algorithm>

namespace OHOS {
namespace Media {
namespace Pipeline {
Filter::Filter(std::string name, FilterType type, bool isAsyncMode)
    : name_(std::move(name)), filterType_(type), isAsyncMode_(isAsyncMode)
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

void Filter::LinkPipeLine(const std::string& groupId)
{
    groupId_ = groupId;
    if (isAsyncMode_) {
        TaskType taskType;
        switch (filterType_) {
            case FilterType::FILTERTYPE_VENC:
            case FilterType::FILTERTYPE_VDEC:
            case FilterType::VIDEO_CAPTURE:
                taskType = TaskType::VIDEO;
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
        filterTask_->SubmitJobOnce([this] {
           DoInitAfterLink();
           ChangeState(FilterState::INITIALIZED);
        });
    } else {
        DoInitAfterLink();
        ChangeState(FilterState::INITIALIZED);
    }
}

Status Filter::Prepare()
{
    MEDIA_LOG_I("Filter::Prepare %{public}s, prevState:%{public}d", name_.c_str(), curState_);
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
    MEDIA_LOG_I("Filter::Prepare enter %{public}s", name_.c_str());
    // next filters maybe added in DoPrepare, so we must DoPrepare first
    Status ret = DoPrepare();
    SetErrCode(ret);
    if (ret != Status::OK) {
        return ret;
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Prepare();
        }
    }
    ChangeState(FilterState::READY);
    return ret;
}

Status Filter::PrepareFrame(bool renderFirstFrame)
{
    MEDIA_LOG_I("Filter::PrepareFrame %{public}s", name_.c_str());
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto rtv = filter->PrepareFrame(renderFirstFrame);
            if (rtv != Status::OK) {
                return rtv;
            }
        }
    }
    auto ret = DoPrepareFrame(renderFirstFrame);
    return ret;
}

Status Filter::WaitPrepareFrame()
{
    MEDIA_LOG_I("Filter::WaitPrepareFrame %{public}s", name_.c_str());
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            auto ret = filter->WaitPrepareFrame();
            if (ret != Status::OK) {
                return ret;
            }
        }
    }
    return Status::OK;
}

Status Filter::Start()
{
    MEDIA_LOG_I("Filter::Start %{public}s, prevState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this] {
            if (StartDone() == Status::OK) {
                filterTask_->Start();
            }
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
    MEDIA_LOG_E("Filter::Start enter %{public}s", name_.c_str());
    Status ret = DoStart();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RUNNING : FilterState::ERROR);
    return ret;
}

Status Filter::Pause()
{
    MEDIA_LOG_I("Filter::Pause %{public}s, prevState:%{public}d", name_.c_str(), curState_);
    // In offload case, we need pause to interrupt audio_sink_plugin write function,  so do not use filterTask_
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

Status Filter::PauseDone()
{
    MEDIA_LOG_I("Filter::Pause enter %{public}s", name_.c_str());
    Status ret = DoPause();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::PAUSED : FilterState::ERROR);
    return ret;
}

Status Filter::Resume()
{
    MEDIA_LOG_I("Filter::Resume %{public}s, prevState:%{public}d", name_.c_str(), curState_);
    if (filterTask_) {
        filterTask_->SubmitJobOnce([this]() {
            if (ResumeDone() == Status::OK) {
                filterTask_->Start();
            }
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
    MEDIA_LOG_I("Filter::Resume enter %{public}s", name_.c_str());
    Status ret = DoResume();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RUNNING : FilterState::ERROR);
    return ret;
}

Status Filter::Stop()
{
    MEDIA_LOG_I("Filter::Stop %{public}s, prevState:%{public}d", name_.c_str(), curState_);
    // In offload case, we need stop to interrupt audio_sink_plugin write function,  so do not use filterTask_
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
    MEDIA_LOG_I("Filter::Stop enter %{public}s", name_.c_str());
    Status ret = DoStop();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::STOPPED : FilterState::ERROR);
    return ret;
}

Status Filter::Flush()
{
    MEDIA_LOG_I("Filter::Flush %{public}s, prevState:%{public}d", name_.c_str(), curState_);
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
    MEDIA_LOG_I("Filter::Release  %{public}s, prevState:%{public}d", name_.c_str(), curState_);
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
    MEDIA_LOG_I("Filter::Release enter %{public}s", name_.c_str());
    Status ret = DoRelease();
    SetErrCode(ret);
    ChangeState(ret == Status::OK ? FilterState::RELEASED : FilterState::ERROR);
    nextFiltersMap_.clear();
    return ret;
}

Status Filter::ProcessInputBuffer(int sendArg, int64_t delayUs)
{
    MEDIA_LOG_D("Filter::ProcessInputBuffer  %{public}s", name_.c_str());
    if (filterTask_) {
        jobIdx_++;
        filterTask_->SubmitJob([this, sendArg]() {
            processIdx_++;
            DoProcessInputBuffer(sendArg, processIdx_ <= jobIdxBase_);  // drop frame after flush
        }, delayUs, 0);
    } else {
        Task::SleepInTask(delayUs / 1000); // 1000 convert to ms
        DoProcessInputBuffer(sendArg, false);
    }
    return Status::OK;
}

Status Filter::ProcessOutputBuffer(int sendArg, int64_t delayUs)
{
    MEDIA_LOG_D("Filter::ProcessOutputBuffer  %{public}s", name_.c_str());
    if (filterTask_) {
        jobIdx_++;
        filterTask_->SubmitJob([this, sendArg]() {
            processIdx_++;
            DoProcessOutputBuffer(sendArg, processIdx_<= jobIdxBase_); // drop frame after flush
        }, delayUs, 0);
    } else {
        Task::SleepInTask(delayUs / 1000); // 1000 convert to ms
        DoProcessOutputBuffer(sendArg, false);
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

Status Filter::DoPrepareFrame(bool renderFirstFrame)
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

Status Filter::DoResume()
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

Status Filter::DoProcessInputBuffer(int recvArg, bool dropFrame)
{
    return Status::OK;
}

Status Filter::DoProcessOutputBuffer(int recvArg, bool dropFrame)
{
    return Status::OK;
}

// should only call in this cpp
void Filter::ChangeState(FilterState state)
{
    MEDIA_LOG_I("Filter::ChangeState to %{public}d. %{public}s", state, name_.c_str());
    AutoLock lock(stateMutex_);
    curState_ = state;
    cond_.NotifyOne();
}

Status Filter::WaitAllState(FilterState state)
{
    AutoLock lock(stateMutex_);
    if (curState_ != state) {
        cond_.WaitFor(lock, 30000, [this, state] { // 30000 ms timeout
            return curState_ == state || curState_ == FilterState::ERROR;
        });
        if (curState_ != state) {
            MEDIA_LOG_E("Filter(%{public}s) wait state %{public}d fail, curState %{public}d",
                name_.c_str(), state, curState_);
            return GetErrCode();
        }
    }

    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            if (filter->WaitAllState(state) != Status::OK) {
                return filter->GetErrCode();
            }
        }
    }
    return Status::OK;
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
