/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_OSAL_AUTO_SPIN_LOCK_H
#define HISTREAMER_FOUNDATION_OSAL_AUTO_SPIN_LOCK_H

#include <atomic>

namespace OHOS {
namespace Media {

    class SpinLock {
    public:
        bool test_and_set(std::memory_order order)
        {
            return spinLock_.test_and_set(order);
        }

        void clear(std::memory_order order)
        {
            spinLock_.clear(order);
        }

    private:
        std::atomic_flag spinLock_ = ATOMIC_FLAG_INIT;
    };

    class AutoSpinLock {
    public:
        explicit AutoSpinLock(SpinLock& spinLock)
            : spinLock_(spinLock)
        {
            while (spinLock_.test_and_set(std::memory_order_acquire));
        }

        AutoSpinLock(const AutoSpinLock&) = delete;

        AutoSpinLock& operator=(const AutoSpinLock&) = delete;

        ~AutoSpinLock()
        {
            spinLock_.clear(std::memory_order_release);
        }

    private:
        SpinLock& spinLock_;
    };

} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_FOUNDATION_OSAL_AUTO_SPIN_LOCK_H
