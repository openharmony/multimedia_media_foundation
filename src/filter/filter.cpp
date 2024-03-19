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
Filter::Filter(std::string name, FilterType type)
    : name_(std::move(name)), filterType_(std::move(type))
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

void Filter::ActiveAsyncMode()
{
    filterLoop_ = std::make_shared<FilterLoop>(name_, this);
}

Status Filter::Prepare()
{
    MEDIA_LOG_I("Filter::Prepare %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->PrepareAsync();
    } else {
        DoPrepare();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Prepare();
        }
    }
    if (!filterLoop_) {
        ChangeState(FilterState::READY);
    }
    return Status::OK;
}

Status Filter::Start()
{
    MEDIA_LOG_I("Filter::Start %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->StartAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Start();
        }
    }
    if (!filterLoop_) {
        DoStart();
        ChangeState(FilterState::RUNNING);
    }
    return Status::OK;
}

Status Filter::Pause()
{
    MEDIA_LOG_I("Filter::Pause %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->PauseAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Pause();
        }
    }
    if (!filterLoop_) {
        DoPause();
        ChangeState(FilterState::PAUSED);
    }
    return Status::OK;
}

Status Filter::Resume()
{
    MEDIA_LOG_I("Filter::Resume %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->ResumeAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Resume();
        }
    }
    if (!filterLoop_) {
        DoResume();
        ChangeState(FilterState::RUNNING);
    }
    return Status::OK;
}

Status Filter::Stop()
{
    MEDIA_LOG_I("Filter::Stop %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->StopAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Stop();
        }
    }
    if (!filterLoop_) {
        DoStop();
        ChangeState(FilterState::STOPPED);
    }
    return Status::OK;
}

Status Filter::Flush()
{
    MEDIA_LOG_I("Filter::Flush %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->FlushAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Flush();
        }
    }
    if (!filterLoop_) {
        DoFlush();
        ChangeState(FilterState::FLUSHED);
    }
    return Status::OK;
}

Status Filter::Release()
{
    MEDIA_LOG_I("Filter::Release  %{public}s", name_.c_str());
    if (filterLoop_) {
        filterLoop_->ReleaseAsync();
    }
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            filter->Release();
        }
    }
    if (!filterLoop_) {
        DoRelease();
        ChangeState(FilterState::RELEASED);
    }
    return Status::OK;
}

Status Filter::ProcessInputBuffer(int arg, int64_t delayUs)
{
    if (filterLoop_) {
        filterLoop_->ProcessInputBufferAsync(arg, delayUs);
    } else {
        OSAL::SleepFor(delayUs / 1000); // us to ms
        DoProcessInputBuffer(arg, false);
    }
    return Status::OK;
}

Status Filter::ProcessOutputBuffer(int arg, int64_t delayUs)
{
    if (filterLoop_) {
        filterLoop_->ProcessOutBufferAsync(arg, delayUs);
    } else {
        OSAL::SleepFor(delayUs / 1000); // us to ms
        DoProcessOutputBuffer(arg, false);
    }
    return Status::OK;
}

Status Filter::DoInit()
{
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

Status Filter::DoProcessInputBuffer(int arg, bool dropped)
{
    return Status::OK;
}

Status Filter::DoProcessOutputBuffer(int arg, bool dropped)
{
    return Status::OK;
}

// should only call in message queue;
void Filter::ChangeState(FilterState state)
{
    MEDIA_LOG_I("Filter::ChangeState to %{public}d. %{public}s", state, name_.c_str());
    AutoLock lock(stateMutex_);
    curState_ = state;
    cond_.NotifyOne();
}

bool Filter::WaitAllState(FilterState state)
{
    MEDIA_LOG_D("Filter::WaitAllState to %{public}d. %{public}s", state, name_.c_str());
    for (auto iter : nextFiltersMap_) {
        for (auto filter : iter.second) {
            if (!filter->WaitAllState(state)) {
                return false;
            }
        }
    }
    if (filterLoop_) {
        AutoLock lock(stateMutex_);
        bool res = cond_.WaitFor(lock, 1000, [this, state]() {
             return curState_ == state;
        }); // At most wait 1000ms
        MEDIA_LOG_D("Filter::WaitAllState res %{public}d. %{public}s", res, name_.c_str());
        return res;
    }
    return true;
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
    if (filterLoop_) {
        filterLoop_->InitAsync();
    } else {
        DoInit();
        ChangeState(FilterState::INITIALIZED);
    }
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
