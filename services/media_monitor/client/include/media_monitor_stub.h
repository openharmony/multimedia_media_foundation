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

#ifndef I_MEDIA_MONITOR_STUB_H
#define I_MEDIA_MONITOR_STUB_H

#include "media_monitor_base.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {
class MediaMonitorStub : public IRemoteStub<IMediaMonitor> {
public:
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    void HandleWriteLogMsg(MessageParcel &data, MessageParcel &reply);
    void HandleGetAudioRouteMsg(MessageParcel &data, MessageParcel &reply);
    using HandlerFunc = void (MediaMonitorStub::*)(MessageParcel &data, MessageParcel &reply);
    static inline HandlerFunc handlers[] = {
        &MediaMonitorStub::HandleWriteLogMsg,
        &MediaMonitorStub::HandleGetAudioRouteMsg,
    };
    static constexpr size_t handlersNums = sizeof(handlers) / sizeof(HandlerFunc);
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MEDIA_MONITOR_STUB_H