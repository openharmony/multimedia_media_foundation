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

#ifndef I_EVENT_BEAN_H
#define I_EVENT_BEAN_H

#include <map>
#include "media_monitor_info.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class EventBean {
public:
    EventBean();
    EventBean(const ModuleId &mId, const EventId &eId,
        const EventType &type);
    ~EventBean() = default;

    void Add(const std::string &key, int32_t value);
    void Add(const std::string &key, std::string value);
    void Add(const std::string &key, uint64_t value);
    void Add(const std::string &key, float value);

    std::map<std::string, int32_t> GetIntMap();
    std::map<std::string, std::string> GetStringMap();
    std::map<std::string, uint64_t> GetUint64Map();
    std::map<std::string, float> GetFloatMap();

    void UpdateIntMap(const std::string &key, int32_t value);
    void UpdateStringMap(const std::string &key, std::string value);
    void UpdateUint64Map(const std::string &key, uint64_t value);
    void UpdateFloatMap(const std::string &key, float value);

    int32_t GetIntValue(const std::string &key);
    std::string GetStringValue(const std::string &key);
    uint64_t GetUint64Value(const std::string &key);
    float GetFloatValue(const std::string &key);

    ModuleId GetModuleId();
    EventId GetEventId();
    EventType GetEventType();

    void SetModuleId(const ModuleId &mId);
    void SetEventId(const EventId &eId);
    void SetEventType(const EventType &type);

    bool WriteToParcel(MessageParcel &parcel);
    void ReadFromParcel(MessageParcel &parcel);

private:
    ModuleId moduleId_;
    EventId eventId_;
    EventType eventType_;
    std::map<std::string, int32_t> intMap_;
    std::map<std::string, std::string> stringMap_;
    std::map<std::string, uint64_t> uint64Map_;
    std::map<std::string, float> floatMap_;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_EVENT_BEAN_H