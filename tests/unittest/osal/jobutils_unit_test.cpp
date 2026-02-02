/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <atomic>
#include "osal/task/jobutils.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
class JobUtilsUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void) {};
    void TearDown(void) {};
};

/**
 * @tc.name: SubmitJobOnceAsyncAndWait
 * @tc.desc: Submit a job asynchronously and wait for its completion.
 * @tc.type: FUNC
 */
HWTEST_F(JobUtilsUnitTest, SubmitJobOnceAsyncAndWait, TestSize.Level1)
{
    std::atomic<int> counter{0};
    auto job = [&counter]() {
        counter.fetch_add(1, std::memory_order_relaxed);
    };

    JobHandle handle = SubmitJobOnceAsync(job);
    WaitForFinish(handle);

    EXPECT_EQ(counter.load(std::memory_order_relaxed), 1);
}

/**
 * @tc.name: WaitForFinish_NullHandle
 * @tc.desc: Call WaitForFinish with a default-initialized handle to ensure no crash.
 * @tc.type: FUNC
 */
HWTEST_F(JobUtilsUnitTest, WaitForFinish_NullHandle, TestSize.Level1)
{
    JobHandle nullHandle {};
    WaitForFinish(nullHandle);
    std::atomic<int> counter{0};
    auto job = [&counter]() {
        counter.fetch_add(1, std::memory_order_relaxed);
    };
    JobHandle handle = SubmitJobOnceAsync(job);
    WaitForFinish(handle);
    EXPECT_EQ(counter.load(std::memory_order_relaxed), 1);
}
} // namespace Media
} // namespace OHOS
