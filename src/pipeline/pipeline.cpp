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

#define HST_LOG_TAG "Pipeline"

#include <queue>
#include <stack>
#include "pipeline/pipeline.h"
#include "osal/task/autolock.h"
#include "osal/task/jobutils.h"
#include "common/log.h"
#include "osal/utils/hitrace_utils.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
static std::atomic<uint16_t> pipeLineId = 0;

int32_t Pipeline::GetNextPipelineId()
{
    return pipeLineId++;
}

Pipeline::~Pipeline()
{
}

void Pipeline::Init(const std::shared_ptr<EventReceiver>& receiver, const std::shared_ptr<FilterCallback>& callback,
    const std::string& groupId)
{
    MEDIA_LOG_I("Pipeline::Init");
    eventReceiver_ = receiver;
    filterCallback_ = callback;
    groupId_ = groupId;
}

Status Pipeline::Prepare()
{
    MEDIA_LOG_I("Prepare enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->Prepare();
            if (ret != Status::OK) {
                return;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->WaitAllState(FilterState::READY);
            if (ret != Status::OK) {
                return;
            }
        }
    });
    MEDIA_LOG_I("Prepare done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::PrepareFrame(bool renderFirstFrame)
{
    MEDIA_LOG_I("PrepareFrame enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            auto rtv = (*it)->PrepareFrame(renderFirstFrame);
            if (rtv != Status::OK) {
                ret = rtv;
                return;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            Status waitRet = (*it)->WaitPrepareFrame();
            if (waitRet != Status::OK) {
                ret = waitRet;
                return;
            }
        }
    });
    MEDIA_LOG_I("PrepareFrame done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::Start()
{
    MEDIA_LOG_I("Start enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->Start();
            if (ret != Status::OK) {
                return;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->WaitAllState(FilterState::RUNNING);
            if (ret != Status::OK) {
                return;
            }
        }
    });
    MEDIA_LOG_I("Start done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::Pause()
{
    MEDIA_LOG_I("Pause enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            auto rtv = (*it)->Pause();
            if (rtv != Status::OK) {
                ret = rtv;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            auto rtv = (*it)->WaitAllState(FilterState::PAUSED);
            if (rtv != Status::OK) {
                ret = rtv;
            }
        }
    });
    MEDIA_LOG_I("Pause done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::Resume()
{
    MEDIA_LOG_I("Resume enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->Resume();
            if (ret != Status::OK) {
                return;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            ret = (*it)->WaitAllState(FilterState::RUNNING);
            if (ret != Status::OK) {
                return;
            }
        }
    });
    MEDIA_LOG_I("Resume done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::Stop()
{
    MEDIA_LOG_I("Stop enter.");
    Status ret = Status::OK;
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            if (*it == nullptr) {
                MEDIA_LOG_E("Pipeline error: " PUBLIC_LOG_ZU, filters_.size());
                continue;
            }
            auto rtv = (*it)->Stop();
            if (rtv != Status::OK) {
                ret = rtv;
            }
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            auto rtv = (*it)->WaitAllState(FilterState::STOPPED);
            if (rtv != Status::OK) {
                ret = rtv;
            }
        }
        filters_.clear();
    });
    MEDIA_LOG_I("Stop done ret = %{public}d", ret);
    return ret;
}

Status Pipeline::Flush()
{
    MEDIA_LOG_I("Flush enter.");
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            (*it)->Flush();
        }
    });
    MEDIA_LOG_I("Flush end.");
    return Status::OK;
}

Status Pipeline::Release()
{
    MEDIA_LOG_I("Release enter.");
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            (*it)->Release();
        }
        for (auto it = filters_.begin(); it != filters_.end(); ++it) {
            (*it)->WaitAllState(FilterState::RELEASED);
        }
        filters_.clear();
    });
    MEDIA_LOG_I("Stop done.");
    return Status::OK;
}

Status Pipeline::AddHeadFilters(std::vector<std::shared_ptr<Filter>> filtersIn)
{
    std::vector<std::shared_ptr<Filter>> filtersToAdd;
    for (auto& filterIn : filtersIn) {
        bool matched = false;
        for (const auto& filter : filters_) {
            if (filterIn == filter) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            filtersToAdd.push_back(filterIn);
            filterIn->LinkPipeLine(groupId_);
        }
    }
    if (filtersToAdd.empty()) {
        MEDIA_LOG_I("filter already exists");
        return Status::OK;
    }
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        this->filters_.insert(this->filters_.end(), filtersToAdd.begin(), filtersToAdd.end());
    });
    return Status::OK;
}

Status Pipeline::RemoveHeadFilter(const std::shared_ptr<Filter>& filter)
{
    SubmitJobOnce([&] {
        AutoLock lock(mutex_);
        auto it = std::find_if(filters_.begin(), filters_.end(),
                               [&filter](const std::shared_ptr<Filter>& filterPtr) { return filterPtr == filter; });
        if (it != filters_.end()) {
            filters_.erase(it);
        }
        return Status::OK;
    });
    return Status::OK;
}

Status Pipeline::LinkFilters(const std::shared_ptr<Filter> &preFilter,
                             const std::vector<std::shared_ptr<Filter>> &nextFilters,
                             StreamType type)
{
    for (auto nextFilter : nextFilters) {
        auto ret = preFilter->LinkNext(nextFilter, type);
        nextFilter->LinkPipeLine(groupId_);
        FALSE_RETURN_V(ret == Status::OK, ret);
    }
    return Status::OK;
}

Status Pipeline::UpdateFilters(const std::shared_ptr<Filter> &preFilter,
                               const std::vector<std::shared_ptr<Filter>> &nextFilters,
                               StreamType type)
{
    for (auto nextFilter : nextFilters) {
        preFilter->UpdateNext(nextFilter, type);
    }
    return Status::OK;
}

Status Pipeline::UnLinkFilters(const std::shared_ptr<Filter> &preFilter,
                               const std::vector<std::shared_ptr<Filter>> &nextFilters,
                               StreamType type)
{
    for (auto nextFilter : nextFilters) {
        preFilter->UnLinkNext(nextFilter, type);
    }
    return Status::OK;
}

void Pipeline::OnEvent(const Event& event)
{
}

} // namespace Pipeline
} // namespace Media
} // namespace OHOS
