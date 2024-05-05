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

#ifndef MEDIA_MONITOR_H
#define MEDIA_MONITOR_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "system_ability.h"
#include "media_monitor_stub.h"
#include "event_aggregate.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class EventAggregate;

struct MessageSignal {
    std::mutex messageMutex_;
    std::condition_variable messageCond_;
    std::queue<std::shared_ptr<EventBean>> messageQueue_;
    std::atomic<bool> isRunning_ = false;
};

class MediaMonitorService : public SystemAbility, public MediaMonitorStub {
    DECLARE_SYSTEM_ABILITY(MediaMonitorService);

public:
    DISALLOW_COPY_AND_MOVE(MediaMonitorService);
    explicit MediaMonitorService(int32_t systemAbilityId, bool runOnCreate = true);
    virtual ~MediaMonitorService() = default;
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

    void WriteLogMsg(std::shared_ptr<EventBean> &bean)  override;

    void GetAudioRouteMsg(std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> &perferredDevices) override;

private:
    MediaMonitorService();
    void MessageLoopFunc();
    void GetMessageFromQueue(std::shared_ptr<EventBean> &message);
    void AddMessageToQueue(std::shared_ptr<EventBean> &message);

    EventAggregate& eventAggregate_;
    AudioMemo& audioMemo_;
    std::unique_ptr<std::thread> messageLoopThread_ = nullptr;
    std::shared_ptr<MessageSignal> signal_ = nullptr;
    bool isExit_ = false;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_H