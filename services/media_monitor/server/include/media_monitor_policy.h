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

#ifndef MEDIA_MONITOR_POLICY_H
#define MEDIA_MONITOR_POLICY_H

#include <map>
#include "event_bean.h"
#include "media_event_base_writer.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class MediaMonitorPolicy {
public:
    static MediaMonitorPolicy& GetMediaMonitorPolicy()
    {
        static MediaMonitorPolicy mediaMonitorPolicy;
        return mediaMonitorPolicy;
    }

    MediaMonitorPolicy()
        : mediaEventBaseWriter_(MediaEventBaseWriter::GetMediaEventBaseWriter()) {}
    ~MediaMonitorPolicy() = default;

    void WriteEvent(EventId eventId, std::shared_ptr<EventBean> &bean);

    void WriteBehaviorEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteFaultEvent(EventId eventId, std::shared_ptr<EventBean> &bean);
    void WriteAggregationEvent(EventId eventId, std::shared_ptr<EventBean> &bean);

private:
    MediaEventBaseWriter& mediaEventBaseWriter_;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_POLICY_H