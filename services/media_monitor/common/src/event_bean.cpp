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

#include "event_bean.h"
#include "log.h"
#include "monitor_error.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

constexpr int MAX_MAP_SIZE = 1000;

EventBean::EventBean() {}

EventBean::EventBean(const ModuleId &mId, const EventId &eId,
    const EventType &type)
    : moduleId_(mId), eventId_(eId), eventType_(type) {}

bool EventBean::WriteToParcel(MessageParcel &parcel)
{
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(moduleId_), false, "write moduleId failed");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(eventId_), false, "write eventId failed");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(eventType_), false, "write eventId failed");

    FALSE_RETURN_V_MSG_E(intMap_.size() < MAX_MAP_SIZE, false,
        "The size of intMap_ exceeds the maximum value");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(intMap_.size()), false, "write intMap.size() failed");
    for (auto &it : intMap_) {
        FALSE_RETURN_V_MSG_E(parcel.WriteString(it.first), false, "intMap failed to WriteString for key");
        FALSE_RETURN_V_MSG_E(parcel.WriteInt32(it.second), false, "intMap failed to WriteInt32 for value");
    }

    FALSE_RETURN_V_MSG_E(stringMap_.size() < MAX_MAP_SIZE, false,
        "The size of stringMap_ exceeds the maximum value");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(stringMap_.size()), false, "write stringMap.size() failed");
    for (auto &it : stringMap_) {
        FALSE_RETURN_V_MSG_E(parcel.WriteString(it.first), false, "stringMap failed to WriteString for key");
        FALSE_RETURN_V_MSG_E(parcel.WriteString(it.second), false, "stringMap failed to WriteString for value");
    }

    FALSE_RETURN_V_MSG_E(uint64Map_.size() < MAX_MAP_SIZE, false,
        "The size of uint64Map_ exceeds the maximum value");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(uint64Map_.size()), false, "write uint64Map.size() failed");
    for (auto &it : uint64Map_) {
        FALSE_RETURN_V_MSG_E(parcel.WriteString(it.first), false, "uint64Map failed to WriteString for key");
        FALSE_RETURN_V_MSG_E(parcel.WriteUint64(it.second), false, "uint64Map failed to WriteInt32 for value");
    }

    FALSE_RETURN_V_MSG_E(floatMap_.size() < MAX_MAP_SIZE, false,
        "The size of floatMap_ exceeds the maximum value");
    FALSE_RETURN_V_MSG_E(parcel.WriteInt32(floatMap_.size()), false, "write floatMap.size() failed");
    for (auto &it : floatMap_) {
        FALSE_RETURN_V_MSG_E(parcel.WriteString(it.first), false, "floatMap failed to WriteString for key");
        FALSE_RETURN_V_MSG_E(parcel.WriteFloat(it.second), false, "floatMap failed to WriteInt32 for value");
    }
    return true;
}

void EventBean::ReadFromParcel(MessageParcel &parcel)
{
    moduleId_ = static_cast<ModuleId>(parcel.ReadInt32());
    eventId_ = static_cast<EventId>(parcel.ReadInt32());
    eventType_ = static_cast<EventType>(parcel.ReadInt32());

    int32_t intMapSize = parcel.ReadInt32();
    FALSE_RETURN_MSG(intMapSize < MAX_MAP_SIZE,
        "The size of intMapSize exceeds the maximum value");
    for (int32_t index = 0; index < intMapSize; index++) {
        std::string key = parcel.ReadString();
        int32_t value = parcel.ReadInt32();
        Add(key, value);
    }

    int32_t stringMapSize = parcel.ReadInt32();
    FALSE_RETURN_MSG(stringMapSize < MAX_MAP_SIZE,
        "The size of stringMapSize exceeds the maximum value");
    for (int32_t index = 0; index < stringMapSize; index++) {
        std::string key = parcel.ReadString();
        std::string value = parcel.ReadString();
        Add(key, value);
    }

    int32_t uint64MapSize = parcel.ReadInt32();
    FALSE_RETURN_MSG(uint64MapSize < MAX_MAP_SIZE,
        "The size of uint64MapSize exceeds the maximum value");
    for (int32_t index = 0; index < uint64MapSize; index++) {
        std::string key = parcel.ReadString();
        uint64_t value = parcel.ReadUint64();
        Add(key, value);
    }

    int32_t floatMapSize = parcel.ReadInt32();
    FALSE_RETURN_MSG(floatMapSize < MAX_MAP_SIZE,
        "The size of floatMapSize exceeds the maximum value");
    for (int32_t index = 0; index < floatMapSize; index++) {
        std::string key = parcel.ReadString();
        float value = parcel.ReadFloat();
        Add(key, value);
    }
}

void EventBean::Add(const std::string &key, int32_t value)
{
    intMap_.emplace(key, value);
}

void EventBean::Add(const std::string &key, std::string value)
{
    stringMap_.emplace(key, value);
}

void EventBean::Add(const std::string &key, uint64_t value)
{
    uint64Map_.emplace(key, value);
}

void EventBean::Add(const std::string &key, float value)
{
    floatMap_.emplace(key, value);
}

std::map<std::string, int32_t> EventBean::GetIntMap()
{
    return intMap_;
}

std::map<std::string, std::string> EventBean::GetStringMap()
{
    return stringMap_;
}

std::map<std::string, uint64_t> EventBean::GetUint64Map()
{
    return uint64Map_;
}

std::map<std::string, float> EventBean::GetFloatMap()
{
    return floatMap_;
}

ModuleId EventBean::GetModuleId()
{
    return moduleId_;
}
EventId EventBean::GetEventId()
{
    return eventId_;
}

EventType EventBean::GetEventType()
{
    return eventType_;
}

void EventBean::SetModuleId(const ModuleId &mId)
{
    moduleId_ = mId;
}

void EventBean::SetEventId(const EventId &eId)
{
    eventId_ = eId;
}

void EventBean::SetEventType(const EventType &type)
{
    eventType_ = type;
}

int32_t EventBean::GetIntValue(const std::string &key)
{
    if (intMap_.find(key) != intMap_.end()) {
        return intMap_.find(key)->second;
    } else {
        return -1;
    }
}

std::string EventBean::GetStringValue(const std::string &key)
{
    if (stringMap_.find(key) != stringMap_.end()) {
        return stringMap_.find(key)->second;
    } else {
        return "";
    }
}

uint64_t EventBean::GetUint64Value(const std::string &key)
{
    if (uint64Map_.find(key) != uint64Map_.end()) {
        return uint64Map_.find(key)->second;
    } else {
        return 0;
    }
}

float EventBean::GetFloatValue(const std::string &key)
{
    if (floatMap_.find(key) != floatMap_.end()) {
        return floatMap_.find(key)->second;
    } else {
        return 0;
    }
}

void EventBean::UpdateIntMap(const std::string &key, int32_t value)
{
    if (intMap_.find(key) != intMap_.end()) {
        intMap_[key] = value;
    }
}

void EventBean::UpdateStringMap(const std::string &key, std::string value)
{
    if (stringMap_.find(key) != stringMap_.end()) {
        stringMap_[key] = value;
    }
}

void EventBean::UpdateUint64Map(const std::string &key, uint64_t value)
{
    if (uint64Map_.find(key) != uint64Map_.end()) {
        uint64Map_[key] = value;
    }
}

void EventBean::UpdateFloatMap(const std::string &key, float value)
{
    if (floatMap_.find(key) != floatMap_.end()) {
        floatMap_[key] = value;
    }
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS