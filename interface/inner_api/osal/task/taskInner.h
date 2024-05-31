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

#ifndef HISTREAMER_FOUNDATION_OSAL_TASK_INNER_H
#define HISTREAMER_FOUNDATION_OSAL_TASK_INNER_H

#include <atomic>
#include <functional>
#include <string>
#include <list>
#include <map>
#include "osal/task/condition_variable.h"
#include "osal/task/mutex.h"
#include "osal/task/autolock.h"
#include "osal/task/pipeline_threadpool.h"

#ifdef MEDIA_FOUNDATION_FFRT
    #include "ffrt.h"
#else
    #include <map>
#endif


namespace OHOS {
namespace Media {

class TaskInner : public std::enable_shared_from_this<TaskInner> {
public:
    explicit TaskInner(const std::string& name, const std::string& groupId, TaskType type,
        TaskPriority priority, bool singleLoop);

    virtual ~TaskInner();

    virtual void Init();

    virtual void DeInit();

    virtual void Start();

    virtual void Stop();

    virtual void StopAsync();

    virtual void Pause();

    virtual void PauseAsync();

    virtual void RegisterJob(const std::function<int64_t()>& job);

    virtual void SubmitJobOnce(const std::function<void()>& job, int64_t delay, bool wait);
    
    virtual void SubmitJob(const std::function<void()>& job, int64_t delay, bool wait);

    virtual bool IsTaskRunning() { return runningState_ == RunningState::STARTED; }

    void SetEnableStateChangeLog(bool enable) { isStateLogEnabled_ = enable; }

    int64_t NextJobUs();

    void HandleJob();

    static void SleepInTask(unsigned ms);

private:
    enum class RunningState : int {
        STARTED,
        PAUSING,
        PAUSED,
        STOPPING,
        STOPPED,
    };

    const std::string name_;
    std::atomic<RunningState> runningState_{RunningState::PAUSED};
    std::atomic<bool> jobState_{false};
    std::function<int64_t()> job_;
    bool singleLoop_ = false;
    int64_t topProcessUs_ {-1};
    bool topIsJob_;
    std::shared_ptr<PipeLineThread> pipelineThread_;
    std::atomic<bool> isStateLogEnabled_{true};
#ifdef MEDIA_FOUNDATION_FFRT
    void DoJob(const std::function<void()>& job);
    std::shared_ptr<ffrt::queue> jobQueue_;
    Mutex stateMutex_;
    ConditionVariable syncCond_;
    ffrt::recursive_mutex jobMutex_;
#else
    void UpdateTop();

    int64_t InsertJob(const std::function<void()>& job, int64_t delayUs, bool inJobQueue);

    Mutex stateMutex_{};
    FairMutex jobMutex_{};
    Mutex replyMtx_{};
    ConditionVariable syncCond_{};
    ConditionVariable replyCond_{};
    std::map<int64_t, std::function<void()>> msgQueue_;  // msg will be sorted by timeUs
    std::map<int64_t, std::function<void()>> jobQueue_;  // msg will be sorted by timeUs
#endif
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_TASK_H

