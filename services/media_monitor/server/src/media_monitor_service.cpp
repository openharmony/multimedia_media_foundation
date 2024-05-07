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

#include "media_monitor_service.h"
#include "log.h"
#include "system_ability_definition.h"
#include "monitor_error.h"
#include "media_monitor_death_recipient.h"

#include "iservice_registry.h"

using namespace std;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

REGISTER_SYSTEM_ABILITY_BY_ID(MediaMonitorService, MEDIA_MONITOR_SERVICE_ID, true)

MediaMonitorService::MediaMonitorService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate),
    eventAggregate_(EventAggregate::GetEventAggregate()),
    audioMemo_(AudioMemo::GetAudioMemo())
{
        MEDIA_LOG_I("MediaMonitorService constructor");
}

void MediaMonitorService::OnDump()
{
    MEDIA_LOG_I("OnDump");
}

int32_t MediaMonitorService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    MEDIA_LOG_I("MediaMonitorService Dump");
    (void)fd;
    (void)args;
    return 0;
}

void MediaMonitorService::OnStart()
{
    MEDIA_LOG_I("OnStart");
    bool res = Publish(this);
    if (!res) {
        MEDIA_LOG_I("publish sa err");
        return;
    }
    signal_ = std::make_shared<MessageSignal>();
    signal_->isRunning_.store(true);
    messageLoopThread_ = make_unique<thread>(&MediaMonitorService::MessageLoopFunc, this);
    pthread_setname_np(messageLoopThread_->native_handle(), "MMLoopThread");
}

void MediaMonitorService::OnStop()
{
    MEDIA_LOG_I("OnStop");
    isExit_ = true;
    signal_->isRunning_.store(false);
}

void MediaMonitorService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    MEDIA_LOG_I("systemAbilityId:%{public}d", systemAbilityId);
}

void MediaMonitorService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    MEDIA_LOG_I("OnRemoveSystemAbility systemAbilityId:%{public}d removed", systemAbilityId);
}

void MediaMonitorService::WriteLogMsg(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write event");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    AddMessageToQueue(bean);
}

void MediaMonitorService::MessageLoopFunc()
{
    MEDIA_LOG_D("MediaMonitorService MessageLoopFunc");
    if (signal_ == nullptr) {
        MEDIA_LOG_E("MediaMonitorService MessageLoopFunc signal_ is nullptr");
        return;
    }
    while (!isExit_) {
        if (!signal_->isRunning_.load()) {
            break;
        }
        std::shared_ptr<EventBean> msg;
        unique_lock<mutex> lock(signal_->messageMutex_);
        {
            signal_->messageCond_.wait(lock, [this]() { return signal_->messageQueue_.size() > 0; });
            if (!signal_->isRunning_.load()) {
                break;
            }
            msg = signal_->messageQueue_.front();
            if (msg == nullptr) {
                signal_->messageQueue_.pop();
                continue;
            }
            signal_->messageQueue_.pop();
        }
        GetMessageFromQueue(msg);
    }
}

void MediaMonitorService::GetMessageFromQueue(std::shared_ptr<EventBean> &message)
{
    if (message == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    eventAggregate_.WriteEvent(message);
}

void MediaMonitorService::AddMessageToQueue(std::shared_ptr<EventBean> &message)
{
    MEDIA_LOG_D("MediaMonitorService AddMessageToQueue");
    if (signal_ == nullptr) {
        MEDIA_LOG_E("signal_ is null");
        return;
    }
    unique_lock<mutex> lock(signal_->messageMutex_);
    if (!signal_->isRunning_.load()) {
        MEDIA_LOG_E("!isRunning_");
        return;
    }
    signal_->messageQueue_.push(message);
    signal_->messageCond_.notify_all();
}

void MediaMonitorService::GetAudioRouteMsg(std::map<PerferredType,
    std::shared_ptr<MonitorDeviceInfo>> &perferredDevices)
{
    MEDIA_LOG_D("MediaMonitorService GetAudioRouteMsg");
    audioMemo_.GetAudioRouteMsg(perferredDevices);
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS