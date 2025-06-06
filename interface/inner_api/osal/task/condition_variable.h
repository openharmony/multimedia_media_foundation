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

#ifndef HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H
#define HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H

#include <functional>
#include "osal/task/autolock.h"

namespace OHOS {
namespace Media {
class ConditionVariable {
public:
    ConditionVariable() noexcept;

    virtual ~ConditionVariable() noexcept;

    ConditionVariable(const ConditionVariable& other) = delete;

    ConditionVariable operator=(const ConditionVariable& other) = delete;

    void __attribute__((no_sanitize("cfi"))) NotifyOne() noexcept;

    void NotifyAll() noexcept;

    void Wait(AutoLock& lock) noexcept;

    void Wait(AutoLock& lock, std::function<bool()> pred) noexcept;

    bool WaitFor(AutoLock& lock, int64_t timeoutMs);

    bool WaitFor(AutoLock& lock, int64_t timeoutMs, std::function<bool()> pred);

private:
#ifdef MEDIA_FOUNDATION_FFRT
    ffrt::condition_variable cond_;
#else
    pthread_cond_t cond_;
#endif
    bool condInited_;
};
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_FOUNDATION_OSAL_CONDITION_VARIABLE_H
