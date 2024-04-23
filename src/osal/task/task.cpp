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

namespace OHOS {
namespace Media {

void Task::SleepInTask(unsigned ms)
{
    TaskInner::SleepInTask(ms);
}

void Task::SetEnableStateChangeLog(bool enable)
{
    if (taskInner_ != nullptr) {
        taskInner_->SetEnableStateChangeLog(enable);
    }
}

Task::Task(const std::string& name, const std::string& groupId, TaskType type, TaskPriority priority, bool singleLoop)
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
} // namespace Media
} // namespace OHOS
