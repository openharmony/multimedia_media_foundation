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

#include "media_monitor_manager.h"
#include "event_bean.h"
#include "event_bean_unit_test.h"
#include "media_monitor_info.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void EventBeanUnitTest::SetUpTestCase(void) {}
void EventBeanUnitTest::TearDownTestCase(void) {}
void EventBeanUnitTest::SetUp(void) {}
void EventBeanUnitTest::TearDown(void) {}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetModuleId_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    ModuleId moduleId = AUDIO;
    bean->SetModuleId(moduleId);
    EXPECT_EQ(bean->GetModuleId(), moduleId);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetModuleId_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    ModuleId moduleId1 = AUDIO;
    ModuleId moduleId2 = MEDIA;
    bean->SetModuleId(moduleId1);
    bean->SetModuleId(moduleId2);
    EXPECT_EQ(bean->GetModuleId(), moduleId2);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetModuleId_003, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    ModuleId moduleId1 = AUDIO;
    ModuleId moduleId2 = MEDIA;
    bean->SetModuleId(moduleId2);
    bean->SetModuleId(moduleId1);
    EXPECT_EQ(bean->GetModuleId(), moduleId1);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventId_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventId eventId = LOAD_CONFIG_ERROR;
    bean->SetEventId(eventId);
    EXPECT_EQ(bean->GetEventId(), eventId);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventId_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventId eventId1 = LOAD_CONFIG_ERROR;
    EventId eventId2 = AUDIO_SERVICE_STARTUP_ERROR;
    bean->SetEventId(eventId1);
    bean->SetEventId(eventId2);
    EXPECT_EQ(bean->GetEventId(), eventId2);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventId_003, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventId eventId1 = LOAD_CONFIG_ERROR;
    EventId eventId2 = AUDIO_SERVICE_STARTUP_ERROR;
    bean->SetEventId(eventId2);
    bean->SetEventId(eventId1);
    EXPECT_EQ(bean->GetEventId(), eventId1);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventType_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventType eventType = FAULT_EVENT;
    bean->SetEventType(eventType);
    EXPECT_EQ(bean->GetEventType(), eventType);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventType_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventType eventType1 = FAULT_EVENT;
    EventType eventType2 = BEHAVIOR_EVENT;
    bean->SetEventType(eventType1);
    bean->SetEventType(eventType2);
    EXPECT_EQ(bean->GetEventType(), eventType2);
}

HWTEST(EventBeanUnitTest, Event_Bean_SetAndGetEventType_003, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    EventType eventType1 = FAULT_EVENT;
    EventType eventType2 = BEHAVIOR_EVENT;
    bean->SetEventType(eventType2);
    bean->SetEventType(eventType1);
    EXPECT_EQ(bean->GetEventType(), eventType1);
}

HWTEST(EventBeanUnitTest, Event_Bean_AddAndGetValue_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "DEVICETYPE";
    int32_t value = 2;
    bean->Add(key, value);
    EXPECT_EQ(bean->GetIntValue(key), value);
}

HWTEST(EventBeanUnitTest, Event_Bean_AddAndGetValue_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "APP_NAME";
    std::string value = "com.sample.unittest";
    bean->Add(key, value);
    EXPECT_EQ(bean->GetStringValue(key), value);
}

HWTEST(EventBeanUnitTest, Event_Bean_AddAndGetValue_003, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "TRANSACTIONID";
    uint64_t value = 4153092512;
    bean->Add(key, value);
    EXPECT_EQ(bean->GetUint64Value(key), value);
}

HWTEST(EventBeanUnitTest, Event_Bean_AddAndGetValue_004, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "VOLUME";
    float value = 0.12;
    bean->Add(key, value);
    EXPECT_EQ(bean->GetFloatValue(key), value);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateIntMap_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "DEVICETYPE";
    int32_t firstValue = 2;
    int32_t secondValue = 1;

    bean->Add(key, firstValue);
    bean->UpdateIntMap(key, secondValue);
    EXPECT_EQ(bean->GetIntValue(key), secondValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateIntMap_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "DEVICETYPE";
    int32_t firstValue = 2;
    int32_t secondValue = 1;

    bean->Add(key, firstValue);
    bean->UpdateIntMap(key, secondValue);
    EXPECT_NE(bean->GetIntValue(key), firstValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateStringMap_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "APP_NAME";
    std::string firstValue = "com.sample.unittest1";
    std::string secondValue = "com.sample.unittest2";

    bean->Add(key, firstValue);
    bean->UpdateStringMap(key, secondValue);
    EXPECT_EQ(bean->GetStringValue(key), secondValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateStringMap_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "APP_NAME";
    std::string firstValue = "com.sample.unittest1";
    std::string secondValue = "com.sample.unittest2";

    bean->Add(key, firstValue);
    bean->UpdateStringMap(key, secondValue);
    EXPECT_NE(bean->GetStringValue(key), firstValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateUint64Value_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "TRANSACTIONID";
    uint64_t firstValue = 4153092512;
    uint64_t secondValue = 1000000000;

    bean->Add(key, firstValue);
    bean->UpdateUint64Map(key, secondValue);
    EXPECT_EQ(bean->GetUint64Value(key), secondValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateUint64Value_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "TRANSACTIONID";
    uint64_t firstValue = 4153092512;
    uint64_t secondValue = 1000000000;

    bean->Add(key, firstValue);
    bean->UpdateUint64Map(key, secondValue);
    EXPECT_NE(bean->GetUint64Value(key), firstValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateFloatMap_001, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "VOLUME";
    float firstValue = 0.12;
    float secondValue = 0.24;

    bean->Add(key, firstValue);
    bean->UpdateFloatMap(key, secondValue);
    EXPECT_EQ(bean->GetFloatValue(key), secondValue);
}

HWTEST(EventBeanUnitTest, Event_Bean_UpdateFloatMap_002, TestSize.Level0)
{
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>();
    std::string key = "VOLUME";
    float firstValue = 0.12;
    float secondValue = 0.24;

    bean->Add(key, firstValue);
    bean->UpdateFloatMap(key, secondValue);
    EXPECT_NE(bean->GetFloatValue(key), firstValue);
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS