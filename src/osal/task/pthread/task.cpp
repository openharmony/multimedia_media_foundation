/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "osal/task/pipeline_threadpool.h"
#include "osal/utils/util.h"
#include "cpp_ext/memory_ext.h"
#include "common/log.h"

#include <mutex>

namespace OHOS {
namespace Media {

static int32_t SINGLETON_TASK_ID = 0;

void Task::SleepInTask(unsigned ms)
{
    OSAL::SleepFor(ms);
}

void Task::SetEnableStateChangeLog(bool enable)
{
    if (taskInner_ != nullptr) {
        taskInner_->SetEnableStateChangeLog(enable);
    }
}

static int64_t GetNowUs()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

Task::Task(std::string name, std::string groupId, TaskType type, TaskPriority priority, bool singleLoop)
    : taskInner_(std::make_shared<TaskInner>(name, groupId, type, priority, singleLoop))
{
    taskInner_->Init();
}

Task::~Task()
{
    taskInner_->DeInit();
}

void Task::Start()
{
    taskInner_->Start();
}

void Task::Stop()
{
    taskInner_->Stop();
}

void Task::StopAsync()
{
    taskInner_->StopAsync();
}

void Task::Pause()
{
    taskInner_->Pause();
}

void Task::PauseAsync()
{
    taskInner_->PauseAsync();
}

void Task::RegisterJob(const std::function<int64_t()>& job)
{
    taskInner_->RegisterJob(job);
}

void Task::SubmitJobOnce(const std::function<void()>& job, int64_t delayUs, bool wait)
{
    taskInner_->SubmitJobOnce(job, delayUs, wait);
}

void Task::SubmitJob(const std::function<void()>& job, int64_t delayUs, bool wait)
{
    taskInner_->SubmitJob(job, delayUs, wait);
}

bool Task::IsTaskRunning()
{
    return taskInner_->IsTaskRunning();
}

TaskInner::TaskInner(std::string name, std::string groupId, TaskType type, TaskPriority priority, bool singleLoop)
    : name_(std::move(name)), runningState_(RunningState::PAUSED), singleLoop_(singleLoop)
{
    MEDIA_LOG_I("task " PUBLIC_LOG_S " groupId:" PUBLIC_LOG_S " type:%{public}d ctor called",
        name_.c_str(), groupId.c_str(), type);
    if (type == TaskType::SINGLETON) {
        std::string newName = name_ + std::to_string(++SINGLETON_TASK_ID);
        pipelineThread_ = PipeLineThreadPool::GetInstance().FindThread(newName, type, priority);
    } else if (type == TaskType::GLOBAL) {
        pipelineThread_ = PipeLineThreadPool::GetInstance().FindThread("OS_GLOBAL", type, TaskPriority::NORMAL);
    } else {
        pipelineThread_ = PipeLineThreadPool::GetInstance().FindThread(groupId, type, priority);
    }
}

void TaskInner::Init()
{
    MEDIA_LOG_I("task " PUBLIC_LOG_S " Init called", name_.c_str());
    pipelineThread_->AddTask(shared_from_this());
}

void TaskInner::DeInit()
{
    MEDIA_LOG_I("task " PUBLIC_LOG_S " DeInit called", name_.c_str());
    {
        AutoLock lock(stateMutex_);
        runningState_ = RunningState::STOPPED;
        AutoLock lock2(jobMutex_);
        topProcessUs_ = -1;
    }
    pipelineThread_->RemoveTask(shared_from_this());
    MEDIA_LOG_I("task " PUBLIC_LOG_S " DeInit done", name_.c_str());
}

TaskInner::~TaskInner()
{
    MEDIA_LOG_I("task " PUBLIC_LOG_S " dtor called", name_.c_str());
}

void TaskInner::Start()
{
    MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(), "task " PUBLIC_LOG_S " Start called", name_.c_str());
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    runningState_ = RunningState::STARTED;
    if (singleLoop_) {
        topProcessUs_ = GetNowUs();
    } else {
        UpdateTop();
    }
    pipelineThread_->UnLockJobState(true);
    MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(), "task " PUBLIC_LOG_S " Start done", name_.c_str());
}

void TaskInner::Stop()
{
    if (pipelineThread_->IsRunningInSelf()) {
        MEDIA_LOG_W("task " PUBLIC_LOG_S " Stop done in self task", name_.c_str());
        runningState_ = RunningState::STOPPED;
        topProcessUs_ = -1;
        return;
    }
    MEDIA_LOG_I("task " PUBLIC_LOG_S " Stop entered", name_.c_str());
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    if (runningState_.load() == RunningState::STOPPED) {
        pipelineThread_->UnLockJobState(false);
        return;
    }
    AutoLock lock2(jobMutex_);
    runningState_ = RunningState::STOPPED;
    topProcessUs_ = -1;
    pipelineThread_->UnLockJobState(true);
    MEDIA_LOG_I("task " PUBLIC_LOG_S " Stop done", name_.c_str());
}

void TaskInner::StopAsync()
{
    if (pipelineThread_->IsRunningInSelf()) {
        MEDIA_LOG_W("task " PUBLIC_LOG_S " Stop done in self task", name_.c_str());
        runningState_ = RunningState::STOPPED;
        topProcessUs_ = -1;
        return;
    }
    MEDIA_LOG_I("task " PUBLIC_LOG_S " StopAsync called", name_.c_str());
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    bool stateChanged = false;
    if (runningState_.load() != RunningState::STOPPED) {
        runningState_ = RunningState::STOPPED;
        topProcessUs_ = -1;
        stateChanged = true;
    }
    pipelineThread_->UnLockJobState(stateChanged);
}

void TaskInner::Pause()
{
    if (pipelineThread_->IsRunningInSelf()) {
        RunningState state = runningState_.load();
        if (state == RunningState::STARTED) {
            MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(),
                "task " PUBLIC_LOG_S " Pause done in self task", name_.c_str());
            runningState_ = RunningState::PAUSED;
            topProcessUs_ = -1;
            return;
        } else {
            MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(),
                "task " PUBLIC_LOG_S " Pause skip in self task, curret State: " PUBLIC_LOG_D32, name_.c_str(), state);
            return;
        }
    }
    MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(), "task " PUBLIC_LOG_S " Pause called", name_.c_str());
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    RunningState state = runningState_.load();
    if (state != RunningState::STARTED) {
        pipelineThread_->UnLockJobState(false);
        return;
    }
    AutoLock lock2(jobMutex_);
    runningState_ = RunningState::PAUSED;
    topProcessUs_ = -1;
    pipelineThread_->UnLockJobState(true);
    MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(), "task " PUBLIC_LOG_S " Pause done.", name_.c_str());
}

// There is no need to perform notification, as no call would wait for PAUSING state.
// If perform notification may cause unnecessasy running when the task is already in PAUSED state.
void TaskInner::PauseAsync()
{
    if (pipelineThread_->IsRunningInSelf()) {
        RunningState state = runningState_.load();
        if (state == RunningState::STARTED) {
            MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(),
                "task " PUBLIC_LOG_S " PauseAsync done in self task", name_.c_str());
            runningState_ = RunningState::PAUSED;
            topProcessUs_ = -1;
            return;
        } else {
            MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(),
                "task " PUBLIC_LOG_S " PauseAsync skip in self task, curretState:%{public}d", name_.c_str(), state);
            return;
        }
    }
    MEDIA_LOG_I_FALSE_D(isStateLogEnabled_.load(), "task " PUBLIC_LOG_S " PauseAsync called", name_.c_str());
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    bool stateChanged = false;
    if (runningState_.load() == RunningState::STARTED) {
        runningState_ = RunningState::PAUSED;
        topProcessUs_ = -1;
        stateChanged = true;
    }
    pipelineThread_->UnLockJobState(stateChanged);
}

void TaskInner::RegisterJob(const std::function<int64_t()>& job)
{
    MEDIA_LOG_I("task " PUBLIC_LOG_S " RegisterHandler called", name_.c_str());
    job_ = std::move(job);
}

void TaskInner::SubmitJobOnce(const std::function<void()>& job, int64_t delayUs, bool wait)
{
    MEDIA_LOG_D("task " PUBLIC_LOG_S " SubmitJobOnce called", name_.c_str());
    int64_t time = InsertJob(job, delayUs, false);
    if (wait) {
        AutoLock lock(replyMtx_);
        replyCond_.Wait(lock, [this, time] { return msgQueue_.find(time) == msgQueue_.end(); });
    }
}

void TaskInner::SubmitJob(const std::function<void()>& job, int64_t delayUs, bool wait)
{
    MEDIA_LOG_D("task " PUBLIC_LOG_S " SubmitJob called  delayUs:%{public}" PRId64, name_.c_str(), delayUs);
    int64_t time = InsertJob(job, delayUs, true);
    if (wait) {
        AutoLock lock(replyMtx_);
        replyCond_.Wait(lock, [this, time] { return jobQueue_.find(time) == jobQueue_.end(); });
    }
}

void TaskInner::UpdateTop()
{
    if (msgQueue_.empty() && ((runningState_.load() != RunningState::STARTED) || jobQueue_.empty())) {
        topProcessUs_ = -1;
        return;
    }
    if (msgQueue_.empty()) {
        topProcessUs_ = jobQueue_.begin()->first;
        topIsJob_ = true;
    } else if ((runningState_.load() != RunningState::STARTED) || jobQueue_.empty()) {
        topProcessUs_ = msgQueue_.begin()->first;
        topIsJob_ = false;
    } else {
        int64_t msgProcessTime = msgQueue_.begin()->first;
        int64_t jobProcessTime = jobQueue_.begin()->first;
        int64_t nowUs =  GetNowUs();
        if (msgProcessTime <= nowUs || msgProcessTime <= jobProcessTime) {
            topProcessUs_ = msgProcessTime;
            topIsJob_ = false;
        } else  {
            topProcessUs_ = jobProcessTime;
            topIsJob_ = true;
        }
    }
}

int64_t TaskInner::NextJobUs()
{
    AutoLock lock(stateMutex_);
    return topProcessUs_;
}

void TaskInner::HandleJob()
{
    if (singleLoop_) {
        stateMutex_.lock();
        if (runningState_.load() == RunningState::PAUSED || runningState_.load() == RunningState::STOPPED) {
            topProcessUs_ = -1;
            stateMutex_.unlock();
            return;
        }
        AutoLock lock(jobMutex_);
        stateMutex_.unlock();
        int64_t nextDelay = job_();
        topProcessUs_ = GetNowUs() + nextDelay;
    } else {
        std::function<void()> nextJob;
        stateMutex_.lock();
        if (topIsJob_) {
            nextJob = std::move(jobQueue_.begin()->second);
            jobQueue_.erase(jobQueue_.begin());
        } else {
            nextJob = std::move(msgQueue_.begin()->second);
            msgQueue_.erase(msgQueue_.begin());
        }
        {
            AutoLock lock(jobMutex_);
            stateMutex_.unlock();
            nextJob();
            replyCond_.NotifyAll();
        }
        AutoLock lock(stateMutex_);
        UpdateTop();
    }
}

int64_t TaskInner::InsertJob(const std::function<void()>& job, int64_t delayUs, bool inJobQueue)
{
    pipelineThread_->LockJobState();
    AutoLock lock(stateMutex_);
    int64_t nowUs = GetNowUs();
    if (delayUs < 0) {
        delayUs = 0;
    }
    int64_t processTime = nowUs + delayUs;
    if (inJobQueue) {
        if (jobQueue_.find(processTime) != jobQueue_.end()) {
            MEDIA_LOG_W("DUPLICATIVE jobQueue_ TIMESTAMP!!!");
            processTime++;
        }
        jobQueue_[processTime] = std::move(job);
    } else {
        if (msgQueue_.find(processTime) != msgQueue_.end()) {
            MEDIA_LOG_W("DUPLICATIVE msgQueue_ TIMESTAMP!!!");
            processTime++;
        }
        msgQueue_[processTime] = std::move(job);
    }
    int64_t lastProcessUs = topProcessUs_;
    if (processTime <= topProcessUs_ || topProcessUs_ == -1) {
        UpdateTop();
    }
    pipelineThread_->UnLockJobState(lastProcessUs != topProcessUs_);
    return processTime;
}
} // namespace Media
} // namespace OHOS
