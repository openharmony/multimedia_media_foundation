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
#include <iostream>
#include <sstream>
#include "common/status.h"
#include "securec.h"
#define HST_LOG_TAG "Task"
#include "osal/task/task.h"
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

    std::shared_ptr<Task> task = nullptr;
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
    task = nullptr;
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: TaskPrintWhile
 * @tc.desc: TaskPrintWhile
 * @tc.type: FUNC
 */
HWTEST_F(TaskInnerUnitTest, TaskPrintWhile, TestSize.Level1)
{
    task = std::make_shared<Task>(std::string("P1Line"));
    task->RegisterJob([]() {
        bool runingState =true;
        int count = 0;
        while (runingState) {
            count++;
            MEDIA_LOG_I("task job TaskPrintWhile running at " PUBLIC_LOG_U32, count);
            sleep(1);
            if (count > 30) {
                runingState = false;
            }
        }
    });
    task->Start();
    sleep(10);
    task->Pause();
    sleep(10);
    task->Start();
    task->Stop();
}
} // namespace MetaFuncUT
} // namespace Media
} // namespace OHOS
