/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_OSAL_TASK_H
#define HISTREAMER_FOUNDATION_OSAL_TASK_H

#include <functional>
#include <string>

namespace OHOS {
namespace Media {

enum class TaskPriority : int {
    LOW,
    NORMAL,
    MIDDLE,
    HIGH,
    HIGHEST,
};

enum class TaskType : int {
    GLOBAL,
    VIDEO,
    AUDIO,
    SINGLETON,
};

class TaskInner;

class Task {
public:
    explicit Task(const std::string& name, const std::string& groupId = "", TaskType type = TaskType::SINGLETON,
        TaskPriority priority = TaskPriority::NORMAL, bool singleLoop = true);

    virtual ~Task();

    virtual void Start();

    virtual void Stop();

    virtual void StopAsync();

    virtual void Pause();

    virtual void PauseAsync();

    virtual void RegisterJob(const std::function<int64_t()>& job);

    virtual void SubmitJobOnce(const std::function<void()>& job, int64_t delayUs = 0, bool wait = false);

    virtual void SubmitJob(const std::function<void()>& job, int64_t delayUs = 0, bool wait = false);

    virtual bool IsTaskRunning();

    static void SleepInTask(unsigned ms);

    void SetEnableStateChangeLog(bool enable);
private:
    std::shared_ptr<TaskInner> taskInner_;
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_TASK_H

