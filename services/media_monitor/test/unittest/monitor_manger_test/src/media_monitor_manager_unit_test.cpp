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

#include "parameters.h"
#include "media_monitor_manager.h"
#include "event_bean.h"
#include "monitor_utils.h"
#include "monitor_error.h"
#include "media_monitor_manager_unit_test.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace Media {
namespace MediaMonitor {

void MediaMonitorManagerUnitTest::SetUpTestCase(void) {}
void MediaMonitorManagerUnitTest::TearDownTestCase(void) {}
void MediaMonitorManagerUnitTest::SetUp(void) {}
void MediaMonitorManagerUnitTest::TearDown(void) {}


HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_001, TestSize.Level0)
{
    int32_t isHasMic = 1;
    int32_t isConnected = 1;
    int32_t deviceType = 2;
    uint64_t duration = TimeUtils::GetCurSec();
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(
        AUDIO, HEADSET_CHANGE, BEHAVIOR_EVENT);
        bean->Add("HASMIC", isHasMic);
        bean->Add("ISCONNECT", isConnected);
        bean->Add("DEVICETYPE", deviceType);
        bean->Add("START_TIME", duration);
    MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetIntValue("HASMIC"), isHasMic);
    EXPECT_EQ(bean->GetIntValue("ISCONNECT"), isConnected);
    EXPECT_EQ(bean->GetIntValue("DEVICETYPE"), deviceType);
    EXPECT_EQ(bean->GetUint64Value("START_TIME"), duration);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_002, TestSize.Level0)
{
    ModuleId moduleId = AUDIO;
    EventId eventId = LOAD_CONFIG_ERROR;
    EventType eventType = FAULT_EVENT;

    int32_t isHasMic = 1;
    int32_t isConnected = 1;
    int32_t deviceType = 2;
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(
        AUDIO, eventId, eventType);
        bean->Add("HASMIC", isHasMic);
        bean->Add("ISCONNECT", isConnected);
        bean->Add("DEVICETYPE", deviceType);
    MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetModuleId(), moduleId);
    EXPECT_EQ(bean->GetEventId(), eventId);
    EXPECT_EQ(bean->GetEventType(), eventType);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_003, TestSize.Level0)
{
    ModuleId moduleId = AUDIO;
    EventId eventId = LOAD_CONFIG_ERROR;
    EventType eventType = FAULT_EVENT;

    int32_t isHasMic1 = 1;
    int32_t isConnected1 = 1;
    int32_t deviceType1 = 2;

    int32_t isHasMic2 = 1;
    int32_t isConnected2 = 1;
    int32_t deviceType2 = 2;
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(
        moduleId, eventId, eventType);
    bean->Add("HASMIC", isHasMic1);
    bean->Add("ISCONNECT", isConnected1);
    bean->Add("DEVICETYPE", deviceType1);

    bean->UpdateIntMap("HASMIC", isHasMic2);
    bean->UpdateIntMap("ISCONNECT", isConnected2);
    bean->UpdateIntMap("DEVICETYPE", deviceType2);

    MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetIntValue("HASMIC"), isHasMic2);
    EXPECT_EQ(bean->GetIntValue("ISCONNECT"), isConnected2);
    EXPECT_EQ(bean->GetIntValue("DEVICETYPE"), deviceType2);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteAudioBuffer_001, TestSize.Level0)
{
    std::string filename = "/data/log/audiodump/unit_test_48000_1_1.pcm";
    uint8_t *buffer = nullptr;
    MediaMonitorManager::GetInstance().WriteAudioBuffer(filename, static_cast<void *>(buffer), 0);
    EXPECT_EQ(buffer, nullptr);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_GetAudioAppStateMsg_001, TestSize.Level0)
{
    int32_t selfUid = getuid();
    setuid(1041);
    //ADD
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(
        AUDIO, APP_BACKGROUND_STATE, BEHAVIOR_EVENT);
    bean->Add("PID", 1);
    bean->Add("IS_FREEZE", 1);
    bean->Add("IS_BACK", 1);
    bean->Add("HAS_SESSION", 1);
    bean->Add("HAS_BACK_TASK", 1);
    bean->Add("IS_BINDER", 1);
    bean->Add("IS_ADD", 1);
    MediaMonitorManager::GetInstance().WriteLogMsg(bean);
    usleep(100000); // 100ms

    std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> appStateMap;
    MediaMonitorManager::GetInstance().GetAudioAppStateMsg(appStateMap);
    auto it = appStateMap.find(1);
    EXPECT_NE(it, appStateMap.end());

    std::shared_ptr<EventBean> bean1 = std::make_shared<EventBean>(
        AUDIO, APP_BACKGROUND_STATE, BEHAVIOR_EVENT);
    bean1->Add("PID", 1);
    bean1->Add("IS_FREEZE", 0);
    bean1->Add("IS_BACK", 0);
    bean1->Add("HAS_SESSION", 0);
    bean1->Add("HAS_BACK_TASK", 0);
    bean1->Add("IS_BINDER", 0);
    bean1->Add("IS_ADD", 0);
    MediaMonitorManager::GetInstance().WriteLogMsg(bean1);
    usleep(100000); // 100ms

    std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> appStateMap1;
    MediaMonitorManager::GetInstance().GetAudioAppStateMsg(appStateMap1);
    it = appStateMap1.find(1);
    EXPECT_EQ(it, appStateMap1.end());
    setuid(selfUid);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_005, TestSize.Level0)
{
    ModuleId moduleId = AUDIO;
    EventId eventId = STREAM_MOVE_EXCEPTION;
    EventType eventType = FAULT_EVENT;

    int32_t clientId = 1000;
    uint32_t sessionId = 123456;
    std::string srcName = "testSrc";
    std::string desName = "testDes";
    uint32_t streamType = 1;
    std::string error = "test error";
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(moduleId, eventId, eventType);
    bean->Add("CLIENT_UID", clientId);
    bean->Add("SESSION_ID", static_cast<int32_t>(sessionId));
    bean->Add("CURRENT_NAME", srcName);
    bean->Add("DES_NAME", desName);
    bean->Add("STREAM_TYPE", static_cast<int32_t>(streamType));
    bean->Add("ERROR_DESCRIPTION", error);
    Media::MediaMonitor::MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetModuleId(), moduleId);
    EXPECT_EQ(bean->GetEventId(), eventId);
    EXPECT_EQ(bean->GetEventType(), eventType);

    std::shared_ptr<EventBean> bean1 = std::make_shared<EventBean>(moduleId, eventId, eventType);
    bean1->Add("CLIENT_UID_U", clientId);
    bean1->Add("SESSION_ID_U", static_cast<int32_t>(sessionId));
    bean1->Add("CURRENT_NAME_U", srcName);
    bean1->Add("DES_NAME_U", desName);
    bean1->Add("STREAM_TYPE_U", static_cast<int32_t>(streamType));
    bean1->Add("ERROR_DESCRIPTION_U", error);
    Media::MediaMonitor::MediaMonitorManager::GetInstance().WriteLogMsg(bean1);

    EXPECT_EQ(bean1->GetModuleId(), moduleId);
    EXPECT_EQ(bean1->GetEventId(), eventId);
    EXPECT_EQ(bean1->GetEventType(), eventType);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_006, TestSize.Level0)
{
    ModuleId moduleId = AUDIO;
    EventId eventId = HPAE_MESSAGE_QUEUE_EXCEPTION;
    EventType eventType = FAULT_EVENT;

    int32_t msg_type = 1000;
    std::string func_name = "unit_test_send_request";
    std::string error = "test error";
    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(moduleId, eventId, eventType);
    bean->Add("MSG_TYPE", msg_type);
    bean->Add("MSG_FUNC_NAME", func_name);
    bean->Add("MSG_ERROR_DESCRIPTION", error);
    Media::MediaMonitor::MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetModuleId(), moduleId);
    EXPECT_EQ(bean->GetEventId(), eventId);
    EXPECT_EQ(bean->GetEventType(), eventType);

    std::shared_ptr<EventBean> bean1 = std::make_shared<EventBean>(moduleId, eventId, eventType);
    bean1->Add("MSG_TYPE_U", msg_type);
    bean1->Add("MSG_FUNC_NAME_U", func_name);
    bean1->Add("MSG_ERROR_DESCRIPTION_U", error);
    Media::MediaMonitor::MediaMonitorManager::GetInstance().WriteLogMsg(bean1);

    EXPECT_EQ(bean1->GetModuleId(), moduleId);
    EXPECT_EQ(bean1->GetEventId(), eventId);
    EXPECT_EQ(bean1->GetEventType(), eventType);
}

HWTEST(MediaMonitorManagerUnitTest, Monitor_Manager_WriteLogMsg_007, TestSize.Level1)
{
    ModuleId moduleId = AUDIO;
    EventId eventId = LOOPBACK_EXCEPTION;
    EventType eventType = FAULT_EVENT;

    int32_t appUid = 1000; // 1000 for test appuid
    std::string appName = "testName";
    int32_t renderDeviceType = 1;
    int32_t captureDeviceType = 1;
    int32_t errScope = 1;
    int32_t errType = 1;

    std::shared_ptr<EventBean> bean = std::make_shared<EventBean>(moduleId, eventId, eventType);
    bean->Add("APP_UID", appUid);
    bean->Add("APP_NAME", appName);
    bean->Add("RENDER_DEVICE_TYPE", renderDeviceType);
    bean->Add("CAPTURE_DEVICE_TYPE", captureDeviceType);
    bean->Add("ERROR_SCOPE", errScope);
    bean->Add("ERROR_TYPE", errType);
    Media::MediaMonitor::MediaMonitorManager::GetInstance().WriteLogMsg(bean);

    EXPECT_EQ(bean->GetModuleId(), moduleId);
    EXPECT_EQ(bean->GetEventId(), eventId);
    EXPECT_EQ(bean->GetEventType(), eventType);
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS