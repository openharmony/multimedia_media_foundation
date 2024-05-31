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

#ifndef HISTREAMER_FOUNDATION_OSAL_PIPELINETHREADPOOL_H
#define HISTREAMER_FOUNDATION_OSAL_PIPELINETHREADPOOL_H

#include <atomic>
#include <functional>
#include <string>
#include <list>
#include <map>
#include "osal/task/condition_variable.h"
#include "osal/task/mutex.h"
#include "osal/task/autolock.h"
#include "osal/task/thread.h"

namespace OHOS {
namespace Media {

class TaskInner;

class PipeLineThread {
public:
    PipeLineThread(std::string name, TaskType type, TaskPriority priority);
    ~PipeLineThread();
    void Run();
    void AddTask(std::shared_ptr<TaskInner> task);
    void RemoveTask(std::shared_ptr<TaskInner> task);
    void LockJobState();
    void UnLockJobState(bool notifyChange);
    void Exit();
    bool IsRunningInSelf();

    std::string name_;
    TaskType type_;
private:
    std::list<std::shared_ptr<TaskInner>> taskList_;
    std::unique_ptr<Thread> loop_;
    FairMutex mutex_;
    ConditionVariable syncCond_;
    std::atomic<bool> threadExiting_;
    std::atomic<bool> threadExit_;
};

class PipeLineThreadPool {
public:
    static PipeLineThreadPool &GetInstance();
    std::shared_ptr<PipeLineThread> FindThread(std::string groupId, TaskType taskType, TaskPriority priority);
    void DestroyThread(std::string groupId);
private:
    PipeLineThreadPool() = default;
    ~PipeLineThreadPool() = default;
    std::map<std::string, std::shared_ptr<std::list<std::shared_ptr<PipeLineThread>>>> workerGroupMap;
    Mutex mutex_;
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_PIPELINETHREADPOOL_H
