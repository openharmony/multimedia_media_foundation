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

#ifndef MEDIA_MONITOR_PROXY_H
#define MEDIA_MONITOR_PROXY_H

#include "iremote_proxy.h"
#include "media_monitor_base.h"
#include "media_monitor_ipc_interface_code.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorClient : public IRemoteProxy<IMediaMonitor> {
public:
    explicit MediaMonitorClient(const sptr<IRemoteObject> &impl);
    virtual ~MediaMonitorClient() = default;

    void WriteLogMsg(std::shared_ptr<EventBean> &bean)  override;

    int32_t GetAudioRouteMsg(std::map<PerferredType,
        std::shared_ptr<MonitorDeviceInfo>> &perferredDevices) override;

    int32_t WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size) override;

    int32_t GetInputBuffer(std::shared_ptr<DumpBuffer> &buffer, int32_t size) override;

    int32_t InputBufferFilled(const std::string &fileName, uint64_t bufferId, int32_t size) override;

    int32_t SetMediaParameters(const std::string &dumpType, const std::string &dumpEnable) override;

    int32_t GetPcmDumpStatus(int32_t &dumpEnable) override;

    int32_t ErasePreferredDeviceByType(const PerferredType preferredType) override;
private:
    int32_t LoadDumpBufferWrap(const std::string &dumpEnable);
    static inline BrokerDelegator<MediaMonitorClient> delegator_;
    std::shared_ptr<DumpBufferWrap> dumpBufferWrap_ = nullptr;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_PROXY_H