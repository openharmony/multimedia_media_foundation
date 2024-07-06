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

void AvsharedmemorybaseUnitTest::SetUpTestCase(void)
{}

void AvsharedmemorybaseUnitTest::TearDownTestCase(void)
{}

void AvsharedmemorybaseUnitTest::SetUp(void)
{}

void AvsharedmemorybaseUnitTest::TearDown(void)
{}

/**
 * @tc.name: Test AVSharedMemoryBase::Read API
 * @tc.desc: Test AVSharedMemoryBase::Read interface, set readSize to 0.
 * @tc.type: FUNC
 */
HWTEST_F(AvsharedmemorybaseUnitTest, Read_Test_001, TestSize.Level0)
{
    std::shared_ptr<AVSharedMemoryBase> memory =
        std::make_shared<AVSharedMemoryBase>(1024, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test");
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
        std::make_shared<AVSharedMemoryBase>(1024, AVSharedMemory::Flags::FLAGS_READ_WRITE, "test");
    ASSERT_TRUE(memory != nullptr);
    int32_t ret = memory->Init();
    ASSERT_TRUE(ret == static_cast<int32_t>(Status::OK));

    // 2. Call the function to be tested
    uint8_t buffer[1024] = {0};
    ret = memory->Write(buffer, 1024);
    ASSERT_TRUE(ret == 1024);

    // 3. Verify the result
    uint8_t readBuffer[1024] = {0};
    ret = memory->Read(readBuffer, 1024);
    ASSERT_TRUE(ret == 1024);
    ASSERT_TRUE(memcmp(buffer, readBuffer, 1024) == 0);
}
}  // namespace AvsharedmemorybaseUnitTest
}  // namespace Media
}  // namespace OHOS