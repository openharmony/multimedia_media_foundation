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
#include "buffer/avsharedmemory.h"
#include "buffer/avsharedmemorybase.h"
#include "common/status.h"

constexpr int SIZE = 15;

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace AvsharedmemorybaseUnitTest {
class AvsharedmemorybaseUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
};

void AvsharedmemorybaseUnitTest::SetUpTestCase(void) {}

void AvsharedmemorybaseUnitTest::TearDownTestCase(void) {}

void AvsharedmemorybaseUnitTest::SetUp(void) {}

void AvsharedmemorybaseUnitTest::TearDown(void) {}

/**
 * @tc.name: Test AVSharedMemoryBase::Read API
 * @tc.desc: Test AVSharedMemoryBase::Read interface, set readSize to 0.
 * @tc.type: FUNC
 */
HWTEST_F(AvsharedmemorybaseUnitTest, Read_Test_001, TestSize.Level0)
{
    std::shared_ptr<AVSharedMemoryBase> memory =
        std::make_shared<AVSharedMemoryBase>(SIZE, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test");
    EXPECT_EQ(memory->Read(nullptr, 0, 0), 0);
}

/**
 * @tc.name: Test Read API
 * @tc.desc: Test Read interface, read data from memory.
 * @tc.type: FUNC
 */
HWTEST_F(AvsharedmemorybaseUnitTest, Read, TestSize.Level0)
{
    // 1. Set up the test environment
    std::shared_ptr<AVSharedMemoryBase> memory =
        std::make_shared<AVSharedMemoryBase>(SIZE, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test");
    ASSERT_TRUE(memory != nullptr);
    int32_t ret = memory->Init();
    ASSERT_TRUE(ret == static_cast<int32_t>(Status::OK));

    // 2. Call the function to be tested
    uint8_t buffer[SIZE] = {0};
    ret = memory->Write(buffer, SIZE);
    ASSERT_TRUE(ret == SIZE);

    // 3. Verify the result
    uint8_t readBuffer[SIZE] = {0};
    ret = memory->Read(readBuffer, SIZE);
    ASSERT_TRUE(ret == SIZE);
    ASSERT_TRUE(memcmp(buffer, readBuffer, SIZE) == 0);
}

/**
 * @tc.name: Test GetUsedSize API
 * @tc.desc: Verify GetUsedSize before/after Write and ClearUsedSize.
 * @tc.type: FUNC
 */
HWTEST_F(AvsharedmemorybaseUnitTest, GetUsedSize, TestSize.Level0)
{
    std::shared_ptr<AVSharedMemoryBase> memory =
        std::make_shared<AVSharedMemoryBase>(SIZE, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test");
    ASSERT_TRUE(memory != nullptr);

    int32_t ret = memory->Init();
    ASSERT_TRUE(ret == static_cast<int32_t>(Status::OK));

    EXPECT_EQ(memory->GetUsedSize(), 0);

    uint8_t buffer[SIZE] = {0};
    ret = memory->Write(buffer, SIZE);
    ASSERT_TRUE(ret == SIZE);
    EXPECT_EQ(memory->GetUsedSize(), SIZE);

    memory->ClearUsedSize();
    EXPECT_EQ(memory->GetUsedSize(), 0);
}

/**
 * @tc.name: Test GetSharedMemoryID API
 * @tc.desc: Verify different AVSharedMemoryBase instances have different IDs.
 * @tc.type: FUNC
 */
HWTEST_F(AvsharedmemorybaseUnitTest, GetSharedMemoryID, TestSize.Level0)
{
    std::shared_ptr<AVSharedMemoryBase> memory1 =
        std::make_shared<AVSharedMemoryBase>(SIZE, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test1");
    ASSERT_TRUE(memory1 != nullptr);

    int32_t ret = memory1->Init();
    ASSERT_TRUE(ret == static_cast<int32_t>(Status::OK));
    uint64_t id1 = memory1->GetSharedMemoryID();

    std::shared_ptr<AVSharedMemoryBase> memory2 =
        std::make_shared<AVSharedMemoryBase>(SIZE, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test2");
    ASSERT_TRUE(memory2 != nullptr);

    ret = memory2->Init();
    ASSERT_TRUE(ret == static_cast<int32_t>(Status::OK));
    uint64_t id2 = memory2->GetSharedMemoryID();

    EXPECT_NE(id1, id2);
}
}  // namespace AvsharedmemorybaseUnitTest
}  // namespace Media
}  // namespace OHOS