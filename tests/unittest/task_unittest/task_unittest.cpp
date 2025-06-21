/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "task_unittest.h"
namespace OHOS {
namespace Media {
using namespace testing::ext;
const uint32_t NUM_TEST = 0;

void TaskUnitTest::SetUpTestCase(void) {}
void TaskUnitTest::TearDownTestCase(void) {}
void TaskUnitTest::SetUp(void) {}
void TaskUnitTest::TearDown(void) {}

/**
 * @tc.name  : Test SetEnableStateChangeLog UpdateThreadPriority API
 * @tc.number: SetEnableStateChangeLog_001
 * @tc.desc  : Test SetEnableStateChangeLog all
 *             Test UpdateThreadPriority taskInner_ == nullptr
 */
HWTEST_F(TaskUnitTest, SetEnableStateChangeLog_001, TestSize.Level0)
{
    // Test UpdateThreadPriority
    auto taskPtr = std::make_shared<Task>("testName");
    ASSERT_NE(taskPtr, nullptr);
    taskPtr->taskInner_ = nullptr;
    uint32_t newPriority = NUM_TEST;
    std::string strBundleName = "testName";
    taskPtr->UpdateThreadPriority(newPriority, strBundleName);

    // Test SetEnableStateChangeLog
    taskPtr->taskInner_ =
        std::make_shared<TaskInner>("testName2", "", TaskType::SINGLETON, TaskPriority::NORMAL, true);
    taskPtr = std::make_shared<Task>("testName2");
    ASSERT_NE(taskPtr, nullptr);
    bool isSupport = true;
    taskPtr->SetEnableStateChangeLog(isSupport);
    EXPECT_EQ(taskPtr->taskInner_->isStateLogEnabled_, isSupport);
}
} // namespace Media
} // namespace OHOS