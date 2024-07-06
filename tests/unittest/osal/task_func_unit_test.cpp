/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <string>
#include "unittest_log.h"
#include <cstdlib>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>
#include <sstream>
#include "common/status.h"
#include "securec.h"
#define HST_LOG_TAG "Task"
#include "osal/task/task.h"
#include "osal/task/condition_variable.h"
#include "cpp_ext/memory_ext.h"
#include "common/log.h"
#include "osal/task/pipeline_threadpool.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace MetaFuncUT {
class TaskInnerUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    ConditionVariable cv;
    Mutex mutex_;
    std::atomic<bool> isStop_{false};
    std::string modifyMsg_ = "";
};

void TaskInnerUnitTest::SetUpTestCase(void) {}

void TaskInnerUnitTest::TearDownTestCase(void) {}

void TaskInnerUnitTest::SetUp(void)
{
    std::cout << "[SetUp]: SetUp!!!, test: ";
    const ::testing::TestInfo *testInfo_ = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string testName = testInfo_->name();
    std::cout << testName << std::endl;
}

void TaskInnerUnitTest::TearDown(void)
{
    PipeLineThreadPool::GetInstance().DestroyThread("");
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: TaskNotRunning
 * @tc.desc: TaskNotRunning
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskNotRunning, TestSize.Level1)
{
    std::shared_ptr<Task> task1 = std::make_shared<Task>("workTask1");
    bool rtv = task1->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}

/**
 * @tc.name: TaskRegisterJobNotRunning
 * @tc.desc: TaskRegisterJobNotRunning
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobNotRunning, TestSize.Level1)
{
    std::shared_ptr<Task> task2 = std::make_shared<Task>("workTask2");
    task2->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobNotRunning running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    bool rtv = task2->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}


/**
 * @tc.name: TaskRegisterJobRun
 * @tc.desc: TaskRegisterJobRun
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobRun, TestSize.Level1)
{
    std::shared_ptr<Task> task3 = std::make_shared<Task>("workTask3");
    task3->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobRun running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task3->Start();
    sleep(1);
    bool rtv = task3->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskRegisterJobPause
 * @tc.desc: TaskRegisterJobPause
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobPause, TestSize.Level1)
{
    std::shared_ptr<Task> task4 = std::make_shared<Task>("workTask4");
    task4->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobPause running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task4->Start();
    task4->Pause();
    sleep(1);
    bool rtv = task4->IsTaskRunning();
    EXPECT_EQ(false, rtv);
    task4->Pause();
    sleep(1);
    rtv = task4->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}

/**
 * @tc.name: TaskJobPauseResume
 * @tc.desc: TaskJobPauseResume
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseResume, TestSize.Level1)
{
    std::shared_ptr<Task> task5 = std::make_shared<Task>("workTask5");
    task5->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseResume running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task5->Start();
    task5->Pause();
    sleep(1);
    bool rtv = task5->IsTaskRunning();
    EXPECT_EQ(false, rtv);
    task5->Start();
    sleep(1);
    rtv = task5->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}


/**
 * @tc.name: TaskRegisterJobPauseAsync
 * @tc.desc: TaskRegisterJobPauseAsync
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobPauseAsync, TestSize.Level1)
{
    std::shared_ptr<Task> task6 = std::make_shared<Task>("workTask6");
    task6->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobPauseAsync running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task6->Start();
    task6->PauseAsync();
    sleep(1);
    bool rtv = task6->IsTaskRunning();
    EXPECT_EQ(false, rtv);
    task6->PauseAsync();
    sleep(1);
    rtv = task6->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}


/**
 * @tc.name: TaskRegisterJobStopAsync
 * @tc.desc: TaskRegisterJobStopAsync
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobStopAsync, TestSize.Level1)
{
    std::shared_ptr<Task> task7 = std::make_shared<Task>("workTask7");
    task7->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobStopAsync running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task7->Start();
    sleep(1);
    task7->StopAsync();
    sleep(1);
    bool rtv = task7->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}

/**
 * @tc.name: TaskRegisterJobStop
 * @tc.desc: TaskRegisterJobStop
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobStop, TestSize.Level1)
{
    std::shared_ptr<Task> task8 = std::make_shared<Task>("workTask8");
    task8->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobStop running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task8->Start();
    sleep(2);
    task8->Stop();
    sleep(1);
    bool rtv = task8->IsTaskRunning();
    EXPECT_EQ(false, rtv);
}

/**
 * @tc.name: TaskRegisterJobStopResume
 * @tc.desc: TaskRegisterJobStopResume
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskRegisterJobStopResume, TestSize.Level1)
{
    std::shared_ptr<Task> task9 = std::make_shared<Task>("workTask9");
    task9->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskRegisterJobStopResume running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10) { //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task9->Start();
    sleep(2);
    task9->Stop();
    sleep(1);
    task9->Start();
    sleep(1);
    bool rtv = task9->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart
 * @tc.desc: TaskJobPauseStopResumeStart
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart, TestSize.Level1)
{
    std::shared_ptr<Task> task10 = std::make_shared<Task>("workTask10");
    task10->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task10->Start();
    sleep(3);
    task10->Pause();
    sleep(2);
    task10->Start();
    sleep(2);
    task10->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task10->Start();
    sleep(1);
    bool rtv = task10->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: WaitFor_Succ
 * @tc.desc: WaitFor_Succ
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Succ, TestSize.Level1)
{
    std::shared_ptr<Task> task01 = std::make_shared<Task>("workTask01");
    AutoLock lock(mutex_);
    task01->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task WaitFor_Succ running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task01->Start();
    int timeoutMs = 1000;
    isStop_.store(true);
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart03
 * @tc.desc: TaskJobPauseStopResumeStart03
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart03, TestSize.Level1)
{
    std::shared_ptr<Task> task03 = std::make_shared<Task>("workTask03",
     "", TaskType::GLOBAL, TaskPriority::NORMAL, true);
    task03->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task03->Start();
    sleep(3);
    task03->Pause();
    sleep(2);
    task03->Start();
    sleep(2);
    task03->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task03->Start();
    sleep(1);
    bool rtv = task03->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task03->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart04
 * @tc.desc: TaskJobPauseStopResumeStart04
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart04, TestSize.Level1)
{
    std::shared_ptr<Task> task04 = std::make_shared<Task>("workTask04",
     "", TaskType::VIDEO, TaskPriority::NORMAL, true);
    task04->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task04->Start();
    sleep(3);
    task04->Pause();
    sleep(2);
    task04->Start();
    sleep(2);
    task04->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task04->Start();
    sleep(1);
    bool rtv = task04->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task04->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart05
 * @tc.desc: TaskJobPauseStopResumeStart05
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart05, TestSize.Level1)
{
    std::shared_ptr<Task> task05 = std::make_shared<Task>("workTask05",
     "", TaskType::AUDIO, TaskPriority::NORMAL, true);
    task05->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task05->Start();
    sleep(3);
    task05->Pause();
    sleep(2);
    task05->Start();
    sleep(2);
    task05->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task05->Start();
    sleep(1);
    bool rtv = task05->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task05->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart06
 * @tc.desc: TaskJobPauseStopResumeStart06
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart06, TestSize.Level1)
{
    std::shared_ptr<Task> task06 = std::make_shared<Task>("workTask06",
     "", TaskType::SUBTITLE, TaskPriority::NORMAL, true);
    task06->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task06->Start();
    sleep(3);
    task06->Pause();
    sleep(2);
    task06->Start();
    sleep(2);
    task06->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task06->Start();
    sleep(1);
    bool rtv = task06->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task06->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart07
 * @tc.desc: TaskJobPauseStopResumeStart07
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart07, TestSize.Level1)
{
    std::shared_ptr<Task> task07 = std::make_shared<Task>("workTask07",
     "", TaskType::GLOBAL, TaskPriority::LOW, true);
    task07->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task07->Start();
    sleep(3);
    task07->Pause();
    sleep(2);
    task07->Start();
    sleep(2);
    task07->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task07->Start();
    sleep(1);
    bool rtv = task07->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task07->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart08
 * @tc.desc: TaskJobPauseStopResumeStart08
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart08, TestSize.Level1)
{
    std::shared_ptr<Task> task08 = std::make_shared<Task>("workTask08",
     "", TaskType::VIDEO, TaskPriority::LOW, true);
    task08->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task08->Start();
    sleep(3);
    task08->Pause();
    sleep(2);
    task08->Start();
    sleep(2);
    task08->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task08->Start();
    sleep(1);
    bool rtv = task08->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task08->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart09
 * @tc.desc: TaskJobPauseStopResumeStart09
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart09, TestSize.Level1)
{
    std::shared_ptr<Task> task09 = std::make_shared<Task>("workTask09",
     "", TaskType::AUDIO, TaskPriority::LOW, true);
    task09->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task09->Start();
    sleep(3);
    task09->Pause();
    sleep(2);
    task09->Start();
    sleep(2);
    task09->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task09->Start();
    sleep(1);
    bool rtv = task09->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task09->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart20
 * @tc.desc: TaskJobPauseStopResumeStart20
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart20, TestSize.Level1)
{
    std::shared_ptr<Task> task20 = std::make_shared<Task>("workTask20",
     "", TaskType::SUBTITLE, TaskPriority::LOW, true);
    task20->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task20->Start();
    sleep(3);
    task20->Pause();
    sleep(2);
    task20->Start();
    sleep(2);
    task20->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task20->Start();
    sleep(1);
    bool rtv = task20->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task20->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart21
 * @tc.desc: TaskJobPauseStopResumeStart21
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart21, TestSize.Level1)
{
    std::shared_ptr<Task> task21 = std::make_shared<Task>("workTask21",
     "", TaskType::SINGLETON, TaskPriority::LOW, true);
    task21->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task21->Start();
    sleep(3);
    task21->Pause();
    sleep(2);
    task21->Start();
    sleep(2);
    task21->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task21->Start();
    sleep(1);
    bool rtv = task21->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart22
 * @tc.desc: TaskJobPauseStopResumeStart22
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart22, TestSize.Level1)
{
    std::shared_ptr<Task> task22 = std::make_shared<Task>("workTask22",
     "", TaskType::GLOBAL, TaskPriority::MIDDLE, true);
    task22->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task22->Start();
    sleep(3);
    task22->Pause();
    sleep(2);
    task22->Start();
    sleep(2);
    task22->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task22->Start();
    sleep(1);
    bool rtv = task22->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task22->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart23
 * @tc.desc: TaskJobPauseStopResumeStart23
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart23, TestSize.Level1)
{
    std::shared_ptr<Task> task23 = std::make_shared<Task>("workTask23",
     "", TaskType::VIDEO, TaskPriority::MIDDLE, true);
    task23->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task23->Start();
    sleep(3);
    task23->Pause();
    sleep(2);
    task23->Start();
    sleep(2);
    task23->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task23->Start();
    sleep(1);
    bool rtv = task23->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task23->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart24
 * @tc.desc: TaskJobPauseStopResumeStart24
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart24, TestSize.Level1)
{
    std::shared_ptr<Task> task24 = std::make_shared<Task>("workTask24",
     "", TaskType::AUDIO, TaskPriority::MIDDLE, true);
    task24->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task24->Start();
    sleep(3);
    task24->Pause();
    sleep(2);
    task24->Start();
    sleep(2);
    task24->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task24->Start();
    sleep(1);
    bool rtv = task24->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task24->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart25
 * @tc.desc: TaskJobPauseStopResumeStart25
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart25, TestSize.Level1)
{
    std::shared_ptr<Task> task25 = std::make_shared<Task>("workTask25",
     "", TaskType::SUBTITLE, TaskPriority::MIDDLE, true);
    task25->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task25->Start();
    sleep(3);
    task25->Pause();
    sleep(2);
    task25->Start();
    sleep(2);
    task25->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task25->Start();
    sleep(1);
    bool rtv = task25->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task25->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart26
 * @tc.desc: TaskJobPauseStopResumeStart26
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart26, TestSize.Level1)
{
    std::shared_ptr<Task> task26 = std::make_shared<Task>("workTask26",
     "", TaskType::SINGLETON, TaskPriority::MIDDLE, true);
    task26->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task26->Start();
    sleep(3);
    task26->Pause();
    sleep(2);
    task26->Start();
    sleep(2);
    task26->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task26->Start();
    sleep(1);
    bool rtv = task26->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart27
 * @tc.desc: TaskJobPauseStopResumeStart27
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart27, TestSize.Level1)
{
    std::shared_ptr<Task> task27 = std::make_shared<Task>("workTask27",
     "", TaskType::GLOBAL, TaskPriority::HIGH, true);
    task27->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task27->Start();
    sleep(3);
    task27->Pause();
    sleep(2);
    task27->Start();
    sleep(2);
    task27->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task27->Start();
    sleep(1);
    bool rtv = task27->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task27->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart28
 * @tc.desc: TaskJobPauseStopResumeStart28
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart28, TestSize.Level1)
{
    std::shared_ptr<Task> task28 = std::make_shared<Task>("workTask28",
     "", TaskType::VIDEO, TaskPriority::HIGH, true);
    task28->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task28->Start();
    sleep(3);
    task28->Pause();
    sleep(2);
    task28->Start();
    sleep(2);
    task28->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task28->Start();
    sleep(1);
    bool rtv = task28->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task28->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart29
 * @tc.desc: TaskJobPauseStopResumeStart29
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart29, TestSize.Level1)
{
    std::shared_ptr<Task> task29 = std::make_shared<Task>("workTask29",
     "", TaskType::AUDIO, TaskPriority::HIGH, true);
    task29->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task29->Start();
    sleep(3);
    task29->Pause();
    sleep(2);
    task29->Start();
    sleep(2);
    task29->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task29->Start();
    sleep(1);
    bool rtv = task29->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task29->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart30
 * @tc.desc: TaskJobPauseStopResumeStart30
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart30, TestSize.Level1)
{
    std::shared_ptr<Task> task30 = std::make_shared<Task>("workTask30",
     "", TaskType::SUBTITLE, TaskPriority::HIGH, true);
    task30->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task30->Start();
    sleep(3);
    task30->Pause();
    sleep(2);
    task30->Start();
    sleep(2);
    task30->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task30->Start();
    sleep(1);
    bool rtv = task30->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task30->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart31
 * @tc.desc: TaskJobPauseStopResumeStart31
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart31, TestSize.Level1)
{
    std::shared_ptr<Task> task31 = std::make_shared<Task>("workTask31",
     "", TaskType::SINGLETON, TaskPriority::HIGH, true);
    task31->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task31->Start();
    sleep(3);
    task31->Pause();
    sleep(2);
    task31->Start();
    sleep(2);
    task31->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task31->Start();
    sleep(1);
    bool rtv = task31->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart32
 * @tc.desc: TaskJobPauseStopResumeStart32
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart32, TestSize.Level1)
{
    std::shared_ptr<Task> task32 = std::make_shared<Task>("workTask32",
     "", TaskType::GLOBAL, TaskPriority::HIGHEST, true);
    task32->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task32->Start();
    sleep(3);
    task32->Pause();
    sleep(2);
    task32->Start();
    sleep(2);
    task32->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task32->Start();
    sleep(1);
    bool rtv = task32->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task32->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart33
 * @tc.desc: TaskJobPauseStopResumeStart33
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart33, TestSize.Level1)
{
    std::shared_ptr<Task> task33 = std::make_shared<Task>("workTask33",
     "", TaskType::VIDEO, TaskPriority::HIGHEST, true);
    task33->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task33->Start();
    sleep(3);
    task33->Pause();
    sleep(2);
    task33->Start();
    sleep(2);
    task33->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task33->Start();
    sleep(1);
    bool rtv = task33->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task33->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart34
 * @tc.desc: TaskJobPauseStopResumeStart34
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart34, TestSize.Level1)
{
    std::shared_ptr<Task> task34 = std::make_shared<Task>("workTask34",
     "", TaskType::AUDIO, TaskPriority::HIGHEST, true);
    task34->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task34->Start();
    sleep(3);
    task34->Pause();
    sleep(2);
    task34->Start();
    sleep(2);
    task34->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task34->Start();
    sleep(1);
    bool rtv = task34->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task34->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart35
 * @tc.desc: TaskJobPauseStopResumeStart35
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart35, TestSize.Level1)
{
    std::shared_ptr<Task> task35 = std::make_shared<Task>("workTask35",
     "", TaskType::SUBTITLE, TaskPriority::HIGHEST, true);
    task35->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task35->Start();
    sleep(3);
    task35->Pause();
    sleep(2);
    task35->Start();
    sleep(2);
    task35->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task35->Start();
    sleep(1);
    bool rtv = task35->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task35->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart36
 * @tc.desc: TaskJobPauseStopResumeStart36
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart36, TestSize.Level1)
{
    std::shared_ptr<Task> task36 = std::make_shared<Task>("workTask36",
     "", TaskType::SINGLETON, TaskPriority::HIGHEST, true);
    task36->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task36->Start();
    sleep(3);
    task36->Pause();
    sleep(2);
    task36->Start();
    sleep(2);
    task36->Stop();
    sleep(1);
    EXPECT_EQ("middle", modifyMsg_);
    task36->Start();
    sleep(1);
    bool rtv = task36->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart37
 * @tc.desc: TaskJobPauseStopResumeStart37
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart37, TestSize.Level1)
{
    std::shared_ptr<Task> task37 = std::make_shared<Task>("workTask37",
     "", TaskType::SINGLETON, TaskPriority::NORMAL, false);
    task37->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task37->Start();
    sleep(3);
    task37->Pause();
    sleep(2);
    task37->Start();
    sleep(2);
    task37->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task37->Start();
    sleep(1);
    bool rtv = task37->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart38
 * @tc.desc: TaskJobPauseStopResumeStart38
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart38, TestSize.Level1)
{
    std::shared_ptr<Task> task38 = std::make_shared<Task>("workTask38",
     "", TaskType::GLOBAL, TaskPriority::NORMAL, false);
    task38->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task38->Start();
    sleep(3);
    task38->Pause();
    sleep(2);
    task38->Start();
    sleep(2);
    task38->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task38->Start();
    sleep(1);
    bool rtv = task38->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task38->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart39
 * @tc.desc: TaskJobPauseStopResumeStart39
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart39, TestSize.Level1)
{
    std::shared_ptr<Task> task39 = std::make_shared<Task>("workTask39",
     "", TaskType::VIDEO, TaskPriority::NORMAL, false);
    task39->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task39->Start();
    sleep(3);
    task39->Pause();
    sleep(2);
    task39->Start();
    sleep(2);
    task39->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task39->Start();
    sleep(1);
    bool rtv = task39->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task39->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart40
 * @tc.desc: TaskJobPauseStopResumeStart40
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart40, TestSize.Level1)
{
    std::shared_ptr<Task> task40 = std::make_shared<Task>("workTask40",
     "", TaskType::AUDIO, TaskPriority::NORMAL, false);
    task40->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task40->Start();
    sleep(3);
    task40->Pause();
    sleep(2);
    task40->Start();
    sleep(2);
    task40->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task40->Start();
    sleep(1);
    bool rtv = task40->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task40->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart41
 * @tc.desc: TaskJobPauseStopResumeStart41
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart41, TestSize.Level1)
{
    std::shared_ptr<Task> task41 = std::make_shared<Task>("workTask41",
     "", TaskType::SUBTITLE, TaskPriority::NORMAL, false);
    task41->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task41->Start();
    sleep(3);
    task41->Pause();
    sleep(2);
    task41->Start();
    sleep(2);
    task41->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task41->Start();
    sleep(1);
    bool rtv = task41->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task41->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart42
 * @tc.desc: TaskJobPauseStopResumeStart42
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart42, TestSize.Level1)
{
    std::shared_ptr<Task> task42 = std::make_shared<Task>("workTask42",
     "", TaskType::GLOBAL, TaskPriority::LOW, false);
    task42->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task42->Start();
    sleep(3);
    task42->Pause();
    sleep(2);
    task42->Start();
    sleep(2);
    task42->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task42->Start();
    sleep(1);
    bool rtv = task42->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task42->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart43
 * @tc.desc: TaskJobPauseStopResumeStart43
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart43, TestSize.Level1)
{
    std::shared_ptr<Task> task43 = std::make_shared<Task>("workTask43",
     "", TaskType::VIDEO, TaskPriority::LOW, false);
    task43->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task43->Start();
    sleep(3);
    task43->Pause();
    sleep(2);
    task43->Start();
    sleep(2);
    task43->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task43->Start();
    sleep(1);
    bool rtv = task43->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task43->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart44
 * @tc.desc: TaskJobPauseStopResumeStart44
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart44, TestSize.Level1)
{
    std::shared_ptr<Task> task44 = std::make_shared<Task>("workTask44",
     "", TaskType::AUDIO, TaskPriority::LOW, false);
    task44->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task44->Start();
    sleep(3);
    task44->Pause();
    sleep(2);
    task44->Start();
    sleep(2);
    task44->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task44->Start();
    sleep(1);
    bool rtv = task44->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task44->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart45
 * @tc.desc: TaskJobPauseStopResumeStart45
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart45, TestSize.Level1)
{
    std::shared_ptr<Task> task45 = std::make_shared<Task>("workTask45",
     "", TaskType::SUBTITLE, TaskPriority::LOW, false);
    task45->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task45->Start();
    sleep(3);
    task45->Pause();
    sleep(2);
    task45->Start();
    sleep(2);
    task45->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task45->Start();
    sleep(1);
    bool rtv = task45->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task45->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart46
 * @tc.desc: TaskJobPauseStopResumeStart46
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart46, TestSize.Level1)
{
    std::shared_ptr<Task> task46 = std::make_shared<Task>("workTask46",
     "", TaskType::SINGLETON, TaskPriority::LOW, false);
    task46->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task46->Start();
    sleep(3);
    task46->Pause();
    sleep(2);
    task46->Start();
    sleep(2);
    task46->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task46->Start();
    sleep(1);
    bool rtv = task46->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart47
 * @tc.desc: TaskJobPauseStopResumeStart47
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart47, TestSize.Level1)
{
    std::shared_ptr<Task> task47 = std::make_shared<Task>("workTask47",
     "", TaskType::GLOBAL, TaskPriority::MIDDLE, false);
    task47->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task47->Start();
    sleep(3);
    task47->Pause();
    sleep(2);
    task47->Start();
    sleep(2);
    task47->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task47->Start();
    sleep(1);
    bool rtv = task47->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task47->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart48
 * @tc.desc: TaskJobPauseStopResumeStart48
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart48, TestSize.Level1)
{
    std::shared_ptr<Task> task48 = std::make_shared<Task>("workTask48",
     "", TaskType::VIDEO, TaskPriority::MIDDLE, false);
    task48->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task48->Start();
    sleep(3);
    task48->Pause();
    sleep(2);
    task48->Start();
    sleep(2);
    task48->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task48->Start();
    sleep(1);
    bool rtv = task48->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task48->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart49
 * @tc.desc: TaskJobPauseStopResumeStart49
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart49, TestSize.Level1)
{
    std::shared_ptr<Task> task49 = std::make_shared<Task>("workTask49",
     "", TaskType::AUDIO, TaskPriority::MIDDLE, false);
    task49->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task49->Start();
    sleep(3);
    task49->Pause();
    sleep(2);
    task49->Start();
    sleep(2);
    task49->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task49->Start();
    sleep(1);
    bool rtv = task49->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task49->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart50
 * @tc.desc: TaskJobPauseStopResumeStart50
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart50, TestSize.Level1)
{
    std::shared_ptr<Task> task50 = std::make_shared<Task>("workTask50",
     "", TaskType::SUBTITLE, TaskPriority::MIDDLE, false);
    task50->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task50->Start();
    sleep(3);
    task50->Pause();
    sleep(2);
    task50->Start();
    sleep(2);
    task50->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task50->Start();
    sleep(1);
    bool rtv = task50->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task50->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart51
 * @tc.desc: TaskJobPauseStopResumeStart51
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart51, TestSize.Level1)
{
    std::shared_ptr<Task> task51 = std::make_shared<Task>("workTask51",
     "", TaskType::SINGLETON, TaskPriority::MIDDLE, false);
    task51->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task51->Start();
    sleep(3);
    task51->Pause();
    sleep(2);
    task51->Start();
    sleep(2);
    task51->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task51->Start();
    sleep(1);
    bool rtv = task51->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart52
 * @tc.desc: TaskJobPauseStopResumeStart52
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart52, TestSize.Level1)
{
    std::shared_ptr<Task> task52 = std::make_shared<Task>("workTask52",
     "", TaskType::GLOBAL, TaskPriority::HIGH, false);
    task52->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task52->Start();
    sleep(3);
    task52->Pause();
    sleep(2);
    task52->Start();
    sleep(2);
    task52->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task52->Start();
    sleep(1);
    bool rtv = task52->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task52->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart53
 * @tc.desc: TaskJobPauseStopResumeStart53
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart53, TestSize.Level1)
{
    std::shared_ptr<Task> task53 = std::make_shared<Task>("workTask53",
     "", TaskType::VIDEO, TaskPriority::HIGH, false);
    task53->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task53->Start();
    sleep(3);
    task53->Pause();
    sleep(2);
    task53->Start();
    sleep(2);
    task53->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task53->Start();
    sleep(1);
    bool rtv = task53->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task53->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart54
 * @tc.desc: TaskJobPauseStopResumeStart54
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart54, TestSize.Level1)
{
    std::shared_ptr<Task> task54 = std::make_shared<Task>("workTask54",
     "", TaskType::AUDIO, TaskPriority::HIGH, false);
    task54->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task54->Start();
    sleep(3);
    task54->Pause();
    sleep(2);
    task54->Start();
    sleep(2);
    task54->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task54->Start();
    sleep(1);
    bool rtv = task54->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task54->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart55
 * @tc.desc: TaskJobPauseStopResumeStart55
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart55, TestSize.Level1)
{
    std::shared_ptr<Task> task55 = std::make_shared<Task>("workTask55",
     "", TaskType::SUBTITLE, TaskPriority::HIGH, false);
    task55->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task55->Start();
    sleep(3);
    task55->Pause();
    sleep(2);
    task55->Start();
    sleep(2);
    task55->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task55->Start();
    sleep(1);
    bool rtv = task55->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task55->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart56
 * @tc.desc: TaskJobPauseStopResumeStart56
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart56, TestSize.Level1)
{
    std::shared_ptr<Task> task56 = std::make_shared<Task>("workTask56",
     "", TaskType::SINGLETON, TaskPriority::HIGH, false);
    task56->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task56->Start();
    sleep(3);
    task56->Pause();
    sleep(2);
    task56->Start();
    sleep(2);
    task56->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task56->Start();
    sleep(1);
    bool rtv = task56->IsTaskRunning();
    EXPECT_EQ(true, rtv);
}

/**
 * @tc.name: TaskJobPauseStopResumeStart57
 * @tc.desc: TaskJobPauseStopResumeStart57
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart57, TestSize.Level1)
{
    std::shared_ptr<Task> task57 = std::make_shared<Task>("workTask57",
     "", TaskType::GLOBAL, TaskPriority::HIGHEST, false);
    task57->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task57->Start();
    sleep(3);
    task57->Pause();
    sleep(2);
    task57->Start();
    sleep(2);
    task57->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task57->Start();
    sleep(1);
    bool rtv = task57->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task57->Stop();
}

/**
 * @tc.name: TaskJobPauseStopResumeStart58
 * @tc.desc: TaskJobPauseStopResumeStart58
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskJobPauseStopResumeStart58, TestSize.Level1)
{
    std::shared_ptr<Task> task58 = std::make_shared<Task>("workTask58",
     "", TaskType::VIDEO, TaskPriority::HIGHEST, false);
    task58->RegisterJob([this]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task TaskJobPauseStopResumeStart running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 5) { //5 second
                this->modifyMsg_ = "middle";
            }
            if (count > 20) { //20 second
                runningState = false;
            }
        }
        return 0;
    });
    task58->Start();
    sleep(3);
    task58->Pause();
    sleep(2);
    task58->Start();
    sleep(2);
    task58->Stop();
    sleep(1);
    EXPECT_EQ("", modifyMsg_);
    task58->Start();
    sleep(1);
    bool rtv = task58->IsTaskRunning();
    EXPECT_EQ(true, rtv);
    task58->Stop();
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    std::shared_ptr<Task> task02 = std::make_shared<Task>("workTask02");
    AutoLock lock(mutex_);
    task02->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            MEDIA_LOG_I("Task WaitFor_Failed running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task02->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_EQ(false, ((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5)));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}
} // namespace MetaFuncUT
} // namespace Media
} // namespace OHOS
