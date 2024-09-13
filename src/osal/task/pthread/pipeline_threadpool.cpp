/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#define HST_LOG_TAG "Task"
#include "osal/task/task.h"
#include "osal/task/taskInner.h"
#include "osal/task/thread.h"
#include "osal/utils/util.h"
#include "cpp_ext/memory_ext.h"
#include "common/log.h"

#include <mutex>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_ONLY_PRERELEASE, LOG_DOMAIN_FOUNDATION, "HiStreamer" };
}

namespace OHOS {
namespace Media {
namespace {
    constexpr int64_t ADJUST_US = 500;
    constexpr int64_t US_PER_MS = 1000;
}

static ThreadPriority ConvertPriorityType(TaskPriority priority)
{
    switch (priority) {
        case TaskPriority::LOW:
            return ThreadPriority::LOW;
        case TaskPriority::NORMAL:
            return ThreadPriority::NORMAL;
        case TaskPriority::MIDDLE:
            return ThreadPriority::MIDDLE;
        case TaskPriority::HIGHEST:
            return ThreadPriority::HIGHEST;
        default:
            return ThreadPriority::HIGH;
    }
}

static std::string TaskTypeConvert(TaskType type)
{
    static const std::map<TaskType, std::string> table = {
        {TaskType::GLOBAL, "G"},
        {TaskType::VIDEO, "V"},
        {TaskType::AUDIO, "A"},
        {TaskType::SUBTITLE, "T"},
        {TaskType::SINGLETON, "S"},
    };
    auto it = table.find(type);
    if (it != table.end()) {
        return it->second;
    }
    return "";
}

static int64_t GetNowUs()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

PipeLineThreadPool& PipeLineThreadPool::GetInstance()
{
    static PipeLineThreadPool instance;
    return instance;
}

std::shared_ptr<PipeLineThread> PipeLineThreadPool::FindThread(const std::string &groupId,
    TaskType taskType, TaskPriority priority)
{
    AutoLock lock(mutex_);
    if (workerGroupMap.find(groupId) == workerGroupMap.end()) {
        workerGroupMap[groupId] = std::make_shared<std::list<std::shared_ptr<PipeLineThread>>>();
    }
    std::shared_ptr<std::list<std::shared_ptr<PipeLineThread>>> threadList = workerGroupMap[groupId];
    for (auto thread : *threadList.get()) {
        if (thread->type_ == taskType) {
            return thread;
        }
    }
    std::shared_ptr<PipeLineThread> newThread = std::make_shared<PipeLineThread>(groupId, taskType, priority);
    threadList->push_back(newThread);
    return newThread;
}

void PipeLineThreadPool::DestroyThread(cosnt std::string &groupId)
{
    MEDIA_LOG_I("DestroyThread groupId:" PUBLIC_LOG_S, groupId.c_str());
    std::shared_ptr<std::list<std::shared_ptr<PipeLineThread>>> threadList;
    {
        AutoLock lock(mutex_);
        if (workerGroupMap.find(groupId) == workerGroupMap.end()) {
            MEDIA_LOG_E("DestroyThread groupId not exist");
            return;
        }
        threadList = workerGroupMap[groupId];
        workerGroupMap.erase(groupId);
    }
    for (auto thread : *threadList.get()) {
        thread->Exit();
    }
}

PipeLineThread::PipeLineThread(std::string groupId, TaskType type, TaskPriority priority)
    : groupId_(groupId), type_(type)
{
    MEDIA_LOG_I("PipeLineThread groupId:" PUBLIC_LOG_S " type:%{public}d created call", groupId_.c_str(), type);
    loop_ = CppExt::make_unique<Thread>(ConvertPriorityType(priority));
    name_ = groupId_ + "_" + TaskTypeConvert(type);
    loop_->SetName(name_);
    threadExit_ = false;
    if (loop_->CreateThread([this] { Run(); })) {
        threadExit_ = false;
    } else {
        threadExit_ = true;
        loop_ = nullptr;
        MEDIA_LOG_E("PipeLineThread " PUBLIC_LOG_S " create failed", name_.c_str());
    }
}

PipeLineThread::~PipeLineThread()
{
    Exit();
}

void PipeLineThread::Exit()
{
    {
        AutoLock lock(mutex_);
        FALSE_RETURN_W(!threadExit_.load() && loop_);

        MEDIA_LOG_I("PipeLineThread " PUBLIC_LOG_S " exit", name_.c_str());
        threadExit_ = true;
        syncCond_.NotifyAll();

        // trigger to quit thread in current running thread, must not wait,
        // or else the current thread will be suspended and can not quit.
        if (IsRunningInSelf()) {
            return;
        }
    }
    // loop_ destroy will wait thread join
    loop_ = nullptr;
}

void PipeLineThread::Run()
{
    MEDIA_LOG_I("PipeLineThread " PUBLIC_LOG_S " run enter", name_.c_str());
    while (true) {
        std::shared_ptr<TaskInner> nextTask;
        {
            AutoLock lock(mutex_);
            if (threadExit_.load()) {
                break;
            }
            int64_t nextJobUs = INT64_MAX;
            for (auto task: taskList_) {
                int64_t taskJobUs = task->NextJobUs();
                if (taskJobUs == -1) {
                    continue;
                }
                if (taskJobUs < nextJobUs) {
                    nextJobUs = taskJobUs;
                    nextTask = task;
                }
            }
            if (nextTask == nullptr) {
                syncCond_.Wait(lock);
                continue;
            }
            int64_t nowUs = GetNowUs();
            if (nextJobUs > (nowUs + ADJUST_US)) {
                syncCond_.WaitFor(lock, (nextJobUs - nowUs + ADJUST_US) / US_PER_MS);
                continue;
            }
        }
        nextTask->HandleJob();
    }
}

void PipeLineThread::AddTask(std::shared_ptr<TaskInner> task)
{
    AutoLock lock(mutex_);
    taskList_.push_back(task);
}

void PipeLineThread::RemoveTask(std::shared_ptr<TaskInner> task)
{
    {
        AutoLock lock(mutex_);
        taskList_.remove(task);
        FALSE_LOG_MSG(!taskList_.empty(),
         "PipeLineThread " PUBLIC_LOG_S " remove all Task", name_.c_str());
    }
    if (type_ == TaskType::SINGLETON) {
        PipeLineThreadPool::GetInstance().DestroyThread(name_);
    }
}

void PipeLineThread::LockJobState()
{
    if (IsRunningInSelf()) {
        return;
    }
    mutex_.lock();
}

void PipeLineThread::UnLockJobState(bool notifyChange)
{
    if (IsRunningInSelf()) {
        return;
    }
    mutex_.unlock();
    if (notifyChange) {
        syncCond_.NotifyAll();
    }
}

bool PipeLineThread::IsRunningInSelf()
{
    return loop_ ? loop_->IsRunningInSelf() : false;
}
} // namespace Media
} // namespace OHOS
