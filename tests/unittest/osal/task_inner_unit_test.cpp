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
#define HST_LOG_TAG "Task"
#include "osal/task/task.h"
#include "common/log.h"
#include "osal/task/pipeline_threadpool.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace TaskInnerFuncUT {
class TaskInnerFuncUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    Mutex mutex_;
    std::atomic<bool> isStop_{false};
    std::string modifyMsg_ = "";
    std::string groupId_ = "";
};

void TaskInnerFuncUnitTest::SetUpTestCase(void) {}

void TaskInnerFuncUnitTest::TearDownTestCase(void) {}

void TaskInnerFuncUnitTest::SetUp(void)
{
    std::cout << "[SetUp]: SetUp!!!, test: ";
    const ::testing::TestInfo *testInfo_ = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string testName = testInfo_->name();
    std::cout << testName << std::endl;
}

void TaskInnerFuncUnitTest::TearDown(void)
{
    PipeLineThreadPool::GetInstance().DestroyThread(groupId_);
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: Pause_Pause_Stop_Stop
 * @tc.desc: Pause_Pause_Stop_Stop
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerFuncUnitTest, Pause_Pause_Stop_Stop, TestSize.Level1)
{
    std::shared_ptr<Task> task = std::make_shared<Task>("workTask");
    AutoLock lock(mutex_);
    task->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task->Start();
    sleep(1);
    task->Pause();
    sleep(1);
    task->Pause();
    sleep(1);
    task->PauseAsync();
    task->Stop();
    task->Stop();
    ASSERT_EQ(false, task->IsTaskRunning());
}

/**
 * @tc.name: UpdateTop_Empty_MsgQueue
 * @tc.desc: UpdateTop No SubmitJob
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerFuncUnitTest, UpdateTop_Empty_MsgQueue, TestSize.Level1)
{
    std::shared_ptr<Task> task = std::make_shared<Task>("workTask", groupId_, TaskType::SINGLETON,
        TaskPriority::NORMAL, false);
    AutoLock lock(mutex_);
    task->RegisterJob([]() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    });
    task->Start();
    sleep(1);
    ASSERT_EQ(true, task->IsTaskRunning());
}

/**
 * @tc.name: Stop_SubmitJob_UpdateTop
 * @tc.desc: UpdateTop jobQueue no empty, state is not start
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerFuncUnitTest, Stop_SubmitJob_UpdateTop, TestSize.Level1)
{
    std::shared_ptr<Task> task = std::make_shared<Task>("workTask", groupId_, TaskType::SINGLETON,
        TaskPriority::NORMAL, false);
    std::function<int64_t()> job = []() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    };
    AutoLock lock(mutex_);
    task->RegisterJob(job);
    task->SubmitJob(job, -1, false);
    task->Start();
    task->Stop();
    sleep(1);
    ASSERT_EQ(false, task->IsTaskRunning());
}

/**
 * @tc.name: Stop_SubmitJobOnce_UpdateTop
 * @tc.desc: UpdateTop jobQueue no empty, state is not start
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerFuncUnitTest, Stop_SubmitJobOnce_UpdateTop, TestSize.Level1)
{
    std::shared_ptr<Task> task = std::make_shared<Task>("workTask", groupId_, TaskType::SINGLETON,
        TaskPriority::NORMAL, false);
    std::function<int64_t()> job = []() {
        bool runningState =true;
        int count = 0;
        while (runningState) {
            count++;
            sleep(1);
            if (count > 10){ //10 second
                runningState = false;
            }
        }
        return 0;
    };
    AutoLock lock(mutex_);
    task->RegisterJob(job);
    task->SubmitJobOnce(job, -1, false);
    task->Start();
    task->Stop();
    sleep(1);
    ASSERT_EQ(false, task->IsTaskRunning());
}
} // namespace TaskInnerFuncUT
} // namespace Media
} // namespace OHOS