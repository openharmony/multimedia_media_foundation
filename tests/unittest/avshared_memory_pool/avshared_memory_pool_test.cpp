/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "common/avsharedmemorypool.h"
#include "buffer/avsharedmemorybase.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace {
const std::string DUMP_PARAM = "w";
const std::string DUMP_FILE_NAME = "DumpBufferTest.es";
}

namespace OHOS {
namespace Media {
class AVSharedMemoryPoolTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void) {};
    void TearDown(void) {};
};

/**
 * @tc.name: ReleaseMemory
 * @tc.desc: ReleaseMemory
 * @tc.type: FUNC
 */
HWTEST_F(AVSharedMemoryPoolTest, ReleaseMemory, TestSize.Level1)
{
    int32_t size = 0;
    std::shared_ptr<AVSharedMemoryPool> pool = std::make_shared<AVSharedMemoryPool>("releaseMemory");
    AVSharedMemory *memory = nullptr;
    pool->ReleaseMemory(memory);
    bool result = pool->DoAcquireMemory(size, &memory);
    EXPECT_EQ(true, result);
}

/**
 * @tc.name: DoAcquireMemory
 * @tc.desc: DoAcquireMemory
 * @tc.type: FUNC
 */
HWTEST_F(AVSharedMemoryPoolTest, DoAcquireMemory, TestSize.Level1)
{
    int32_t size = 0;
    std::shared_ptr<AVSharedMemoryPool> pool = std::make_shared<AVSharedMemoryPool>("doAcquireMemory");
    AVSharedMemory *memory = nullptr;
    bool result = pool->DoAcquireMemory(size, &memory);
    EXPECT_EQ(true, result);
}

/**
 * @tc.name: CheckSize
 * @tc.desc: CheckSize
 * @tc.type: FUNC
 */
HWTEST_F(AVSharedMemoryPoolTest, CheckSize, TestSize.Level1)
{
    std::shared_ptr<AVSharedMemoryPool> pool = std::make_shared<AVSharedMemoryPool>("checkSize");
    EXPECT_EQ(false, pool->CheckSize(-2));
    EXPECT_EQ(false, pool->CheckSize(1));
    EXPECT_TRUE(pool->CheckSize(-1));
}

/**
 * @tc.name: AcquireMemory
 * @tc.desc: AcquireMemory
 * @tc.type: FUNC
 */
HWTEST_F(AVSharedMemoryPoolTest, AcquireMemory, TestSize.Level1)
{
    int32_t size = 0;
    bool blocking = true;
    std::shared_ptr<AVSharedMemoryPool> pool = std::make_shared<AVSharedMemoryPool>("acquireMemory");
    pool->SetNonBlocking(true);
    std::shared_ptr<AVSharedMemory> memory = pool->AcquireMemory(size, blocking);
    EXPECT_TRUE(memory == nullptr);
    pool->SetNonBlocking(false);
    std::shared_ptr<AVSharedMemory> memory1 = pool->AcquireMemory(size, blocking);
    EXPECT_TRUE(memory1 == nullptr);
}
} // namespace Media
} // namespace OHOS
