/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 
#define HST_LOG_TAG "InterruptMonitor"
 
#include "common/interrupt_monitor.h"
#include <algorithm>
 
using namespace std;
 
namespace OHOS {
 
namespace Media {
void InterruptMonitor::SetInterruptState(bool isInterruptNeeded)
{
    interruptState_.store(isInterruptNeeded);
    NotifyInterrupt(isInterruptNeeded);
}
 
void InterruptMonitor::RegisterListener(const shared_ptr<InterruptListener> listener)
{
    vec_.push_back(std::weak_ptr<InterruptListener>(listener));
}
 
void InterruptMonitor::DeregisterListener(std::shared_ptr<InterruptListener> listener)
{
    if (listener == nullptr) {
        return;
    }
    for (auto it = vec_.begin(); it != vec_.end();) {
        if (it->lock() == listener) {
            it = vec_.erase(it);
        } else {
            ++it;
        }
    }
}
 
void InterruptMonitor::CleanUnusedListener()
{
    for (auto it = vec_.begin(); it != vec_.end();) {
        if (!(it->expired())) {
            it = vec_.erase(it);
        } else {
            ++it;
        }
    }
}
 
void InterruptMonitor::NotifyInterrupt(bool isInterruptNeeded)
{
    for (auto listener : vec_) {
        if (auto interruptListener = listener.lock()) {
            interruptListener->OnInterrupted(isInterruptNeeded);
        }
    }
}
} // namespace Media
 
} // namespace OHOS