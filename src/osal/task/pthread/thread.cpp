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

#define HST_LOG_TAG "Thread"

#include "osal/task/thread.h"
#include "common/log.h"
#include "osal/task/autolock.h"

namespace OHOS {
namespace Media {
Thread::Thread(ThreadPriority priority) noexcept : id_(), name_(), priority_(priority), state_()
{
}

Thread::Thread(Thread&& other) noexcept
{
    *this = std::move(other);
}

Thread& Thread::operator=(Thread&& other) noexcept
{
    if (this != &other) {
        AutoLock lock(mutex_);
        id_ = other.id_;
        name_ = std::move(other.name_);
        priority_ = other.priority_;
        state_ = std::move(other.state_);
    }
    return *this;
}

Thread::~Thread() noexcept
{
    if (isExistThread_.load()) {
        pthread_join(id_, nullptr);
    }
}

bool Thread::HasThread() const noexcept
{
    AutoLock lock(mutex_);
    return state_ != nullptr;
}

void Thread::SetName(const std::string& name)
{
    name_ = name;
}

bool Thread::CreateThread(const std::function<void()>& func)
{
    {
        AutoLock lock(mutex_);
        state_ = std::make_unique<State>();
        state_->func = func;
        state_->name = name_;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#ifdef OHOS_LITE
    // Only OHOS_LITE can set inheritsched and schedpolicy.
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
#endif
    struct sched_param sched = {static_cast<int>(priority_)};
    pthread_attr_setschedparam(&attr, &sched);
#if defined(THREAD_STACK_SIZE) and THREAD_STACK_SIZE > 0
    pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE);
    MEDIA_LOG_I("thread stack size set to " PUBLIC_LOG_D32, THREAD_STACK_SIZE);
#endif
    int rtv = pthread_create(&id_, &attr, Thread::Run, this);
    if (rtv == 0) {
        MEDIA_LOG_I("thread " PUBLIC_LOG_S " create success", name_.c_str());
        isExistThread_.store(true);
        SetNameInternal();
    } else {
        AutoLock lock(mutex_);
        if (state_ != nullptr) {
            state_.reset();
        }
        MEDIA_LOG_E("thread create failed, name: " PUBLIC_LOG_S ", rtv: " PUBLIC_LOG_D32, name_.c_str(), rtv);
    }
    return rtv == 0;
}

bool Thread::IsRunningInSelf()
{
    pthread_t tid = pthread_self();
    AutoLock lock(mutex_);
    return tid == id_;
}

void Thread::SetNameInternal()
{
    AutoLock lock(mutex_);
    if (state_ && !name_.empty()) {
        constexpr int threadNameMaxSize = 15;
        if (name_.size() > threadNameMaxSize) {
            MEDIA_LOG_W("task name " PUBLIC_LOG_S " exceed max size: " PUBLIC_LOG_D32,
                        name_.c_str(), threadNameMaxSize);
            name_ = name_.substr(0, threadNameMaxSize);
        }
        pthread_setname_np(id_, name_.c_str());
    }
}

void* Thread::Run(void* arg) // NOLINT: void*
{
    std::function<void()> func;
    std::string name;
    {
        auto currentThread = static_cast<Thread *>(arg);
        AutoLock lock(currentThread->mutex_);
        auto state = currentThread->state_.get();
        if (state == nullptr) {
            return nullptr;
        }
        func = state->func;
        name = state->name;
    }
    func();
    {
        auto currentThread = static_cast<Thread *>(arg);
        AutoLock lock(currentThread->mutex_);
        currentThread->state_ = nullptr;
    }
    MEDIA_LOG_W("Thread " PUBLIC_LOG_S " exited...", name.c_str());
    return nullptr;
}
} // namespace Media
} // namespace OHOS
