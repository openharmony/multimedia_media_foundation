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
    std::shared_ptr<Task> task1 = nullptr;
    std::shared_ptr<Task> task2 = nullptr;
    std::shared_ptr<Task> task3 = nullptr;
    std::shared_ptr<Task> task4 = nullptr;
    std::shared_ptr<Task> task5 = nullptr;
    std::shared_ptr<Task> task6 = nullptr;
    std::shared_ptr<Task> task7 = nullptr;
    std::shared_ptr<Task> task8 = nullptr;
    std::shared_ptr<Task> task9 = nullptr;
    std::shared_ptr<Task> task10 = nullptr;
    std::shared_ptr<Task> task01 = nullptr;
    std::shared_ptr<Task> task02 = nullptr;
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
    task1 = std::make_shared<Task>("workTask1");
    task2 = std::make_shared<Task>("workTask2");
    task3 = std::make_shared<Task>("workTask3");
    task4 = std::make_shared<Task>("workTask4");
    task5 = std::make_shared<Task>("workTask5");
    task6 = std::make_shared<Task>("workTask6");
    task7 = std::make_shared<Task>("workTask7");
    task8 = std::make_shared<Task>("workTask8");
    task9 = std::make_shared<Task>("workTask9");
    task10 = std::make_shared<Task>("workTask10");
    task01 = std::make_shared<Task>("workTask01");
    task02 = std::make_shared<Task>("workTask02");
    task03 = std::make_shared<Task>("workTask03","",TaskType::GLOBAL,TaskPriority::NORMAL,true);
    task04 = std::make_shared<Task>("workTask04","",TaskType::VIDEO,TaskPriority::NORMAL,true);
    task05 = std::make_shared<Task>("workTask05","",TaskType::AUDIO,TaskPriority::NORMAL,true);
    task06 = std::make_shared<Task>("workTask06","",TaskType::SUBTITLE,TaskPriority::NORMAL,true);
    task07 = std::make_shared<Task>("workTask07","",TaskType::GLOBAL,TaskPriority::LOW,true);
    task08 = std::make_shared<Task>("workTask08","",TaskType::VIDEO,TaskPriority::LOW,true);
    task09 = std::make_shared<Task>("workTask09","",TaskType::AUDIO,TaskPriority::LOW,true);
    task20 = std::make_shared<Task>("workTask20","",TaskType::SUBTITLE,TaskPriority::LOW,true);
    task21 = std::make_shared<Task>("workTask21","",TaskType::SINGLETON,TaskPriority::LOW,true);
    task22 = std::make_shared<Task>("workTask22","",TaskType::GLOBAL,TaskPriority::MIDDLE,true);
    task23 = std::make_shared<Task>("workTask23","",TaskType::VIDEO,TaskPriority::MIDDLE,true);
    task24 = std::make_shared<Task>("workTask24","",TaskType::AUDIO,TaskPriority::MIDDLE,true);
    task25 = std::make_shared<Task>("workTask25","",TaskType::SUBTITLE,TaskPriority::MIDDLE,true);
    task26 = std::make_shared<Task>("workTask26","",TaskType::SINGLETON,TaskPriority::MIDDLE,true);
    task27 = std::make_shared<Task>("workTask27","",TaskType::GLOBAL,TaskPriority::HIGH,true);
    task28 = std::make_shared<Task>("workTask28","",TaskType::VIDEO,TaskPriority::HIGH,true);
    task29 = std::make_shared<Task>("workTask29","",TaskType::AUDIO,TaskPriority::HIGH,true);
    task30 = std::make_shared<Task>("workTask30","",TaskType::SUBTITLE,TaskPriority::HIGH,true);
    task31 = std::make_shared<Task>("workTask31","",TaskType::SINGLETON,TaskPriority::HIGH,true);
    task32 = std::make_shared<Task>("workTask32","",TaskType::GLOBAL,TaskPriority::HIGHEST,true);
    task33 = std::make_shared<Task>("workTask33","",TaskType::VIDEO,TaskPriority::HIGHEST,true);
    task34 = std::make_shared<Task>("workTask34","",TaskType::AUDIO,TaskPriority::HIGHEST,true);
    task35 = std::make_shared<Task>("workTask35","",TaskType::SUBTITLE,TaskPriority::HIGHEST,true);
    task36 = std::make_shared<Task>("workTask36","",TaskType::SINGLETON,TaskPriority::HIGHEST,true);
    task37 = std::make_shared<Task>("workTask37","",TaskType::SINGLETON,TaskPriority::NORMAL,false);
    task38 = std::make_shared<Task>("workTask38","",TaskType::GLOBAL,TaskPriority::NORMAL,false);
    task39 = std::make_shared<Task>("workTask39","",TaskType::VIDEO,TaskPriority::NORMAL,false);
    task40 = std::make_shared<Task>("workTask40","",TaskType::AUDIO,TaskPriority::NORMAL,false);
    task41 = std::make_shared<Task>("workTask41","",TaskType::SUBTITLE,TaskPriority::NORMAL,false);
    task42 = std::make_shared<Task>("workTask42","",TaskType::GLOBAL,TaskPriority::LOW,false);
    task43 = std::make_shared<Task>("workTask43","",TaskType::VIDEO,TaskPriority::LOW,false);
    task44 = std::make_shared<Task>("workTask44","",TaskType::AUDIO,TaskPriority::LOW,false);
    task45 = std::make_shared<Task>("workTask45","",TaskType::SUBTITLE,TaskPriority::LOW,false);
    task46 = std::make_shared<Task>("workTask46","",TaskType::SINGLETON,TaskPriority::LOW,false);
    task47 = std::make_shared<Task>("workTask47","",TaskType::GLOBAL,TaskPriority::MIDDLE,false);
    task48 = std::make_shared<Task>("workTask48","",TaskType::VIDEO,TaskPriority::MIDDLE,false);
    task49 = std::make_shared<Task>("workTask49","",TaskType::AUDIO,TaskPriority::MIDDLE,false);
    task50 = std::make_shared<Task>("workTask50","",TaskType::SUBTITLE,TaskPriority::MIDDLE,false);
    task51 = std::make_shared<Task>("workTask51","",TaskType::SINGLETON,TaskPriority::MIDDLE,false);
    task52 = std::make_shared<Task>("workTask52","",TaskType::GLOBAL,TaskPriority::HIGH,false);
    task53 = std::make_shared<Task>("workTask53","",TaskType::VIDEO,TaskPriority::HIGH,false);
    task54 = std::make_shared<Task>("workTask54","",TaskType::AUDIO,TaskPriority::HIGH,false);
    task55 = std::make_shared<Task>("workTask55","",TaskType::SUBTITLE,TaskPriority::HIGH,false);
    task56 = std::make_shared<Task>("workTask56","",TaskType::SINGLETON,TaskPriority::HIGH,false);
    task57 = std::make_shared<Task>("workTask57","",TaskType::GLOBAL,TaskPriority::HIGHEST,false);
    task58 = std::make_shared<Task>("workTask58","",TaskType::VIDEO,TaskPriority::HIGHEST,false);
    task59 = std::make_shared<Task>("workTask59","",TaskType::AUDIO,TaskPriority::HIGHEST,false);
    task60 = std::make_shared<Task>("workTask60","",TaskType::SUBTITLE,TaskPriority::HIGHEST,false);
    task61 = std::make_shared<Task>("workTask61","",TaskType::SINGLETON,TaskPriority::HIGHEST,false);
}

void TaskInnerUnitTest::TearDown(void)
{
    task1 = nullptr;
    task1 = nullptr;
    task2 = nullptr;
    task3 = nullptr;
    task4 = nullptr;
    task5 = nullptr;
    task6 = nullptr;
    task7 = nullptr;
    task8 = nullptr;
    task9 = nullptr;
    task10 = nullptr;
    task01 = nullptr;
    task02 = nullptr;
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: TaskNotRunning
 * @tc.desc: TaskNotRunning
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskNotRunning, TestSize.Level1)
{
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
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
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
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task03->RegisterJob([]() {
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
    task03->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task04->RegisterJob([]() {
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
    task04->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task05->RegisterJob([]() {
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
    task05->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task06->RegisterJob([]() {
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
    task06->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task07->RegisterJob([]() {
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
    task07->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task08->RegisterJob([]() {
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
    task08->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task09->RegisterJob([]() {
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
    task09->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task20->RegisterJob([]() {
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
    task20->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task21->RegisterJob([]() {
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
    task21->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task22->RegisterJob([]() {
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
    task22->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task23->RegisterJob([]() {
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
    task23->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task24->RegisterJob([]() {
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
    task24->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task25->RegisterJob([]() {
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
    task25->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task26->RegisterJob([]() {
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
    task26->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task27->RegisterJob([]() {
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
    task27->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task28->RegisterJob([]() {
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
    task28->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task29->RegisterJob([]() {
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
    task29->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task30->RegisterJob([]() {
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
    task30->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task31->RegisterJob([]() {
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
    task31->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task32->RegisterJob([]() {
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
    task32->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task33->RegisterJob([]() {
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
    task33->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task34->RegisterJob([]() {
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
    task34->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task35->RegisterJob([]() {
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
    task35->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task36->RegisterJob([]() {
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
    task36->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task37->RegisterJob([]() {
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
    task37->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task38->RegisterJob([]() {
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
    task38->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task39->RegisterJob([]() {
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
    task39->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task40->RegisterJob([]() {
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
    task40->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task41->RegisterJob([]() {
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
    task41->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task42->RegisterJob([]() {
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
    task42->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task43->RegisterJob([]() {
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
    task43->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task44->RegisterJob([]() {
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
    task44->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task45->RegisterJob([]() {
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
    task45->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task46->RegisterJob([]() {
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
    task46->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task47->RegisterJob([]() {
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
    task47->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task48->RegisterJob([]() {
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
    task48->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task49->RegisterJob([]() {
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
    task49->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task50->RegisterJob([]() {
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
    task50->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task51->RegisterJob([]() {
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
    task51->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task52->RegisterJob([]() {
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
    task52->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task53->RegisterJob([]() {
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
    task53->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task54->RegisterJob([]() {
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
    task54->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task55->RegisterJob([]() {
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
    task55->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task56->RegisterJob([]() {
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
    task56->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task57->RegisterJob([]() {
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
    task57->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task58->RegisterJob([]() {
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
    task58->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task59->RegisterJob([]() {
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
    task59->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task60->RegisterJob([]() {
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
    task60->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}

/**
 * @tc.name: WaitFor_Failed
 * @tc.desc: WaitFor_Failed
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, WaitFor_Failed, TestSize.Level1)
{
    AutoLock lock(mutex_);
    task61->RegisterJob([]() {
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
    task61->Start();
    int timeoutMs = 100;
    isStop_.store(false);
    auto start = std::chrono::high_resolution_clock::now();
    auto rtv = cv.WaitFor(lock, timeoutMs, [this] { return isStop_.load(); });
    auto end = std::chrono::high_resolution_clock::now();
    int64_t diff = static_cast<int64_t>(static_cast<std::chrono::duration<double>>(end - start).count() * 1000);
    EXPECT_EQ(false, rtv);
    EXPECT_TRUE((std::abs(static_cast<int>(diff) - timeoutMs) < 20) || (diff < 5));
    MEDIA_LOG_I("Wait Time Diff: " PUBLIC_LOG_D64, diff);
}
} // namespace MetaFuncUT
} // namespace Media
} // namespace OHOS
