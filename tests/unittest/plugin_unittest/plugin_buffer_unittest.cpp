/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "plugin_buffer_unittest.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::Media::Plugins;

const static uint8_t TEST_IN = 100;
constexpr size_t MEM_VALID_POSITION = 0;
constexpr size_t TEST_WRITE_SIZE = 100;
constexpr size_t TEST_READ_SIZE = 100;

/**
 * @tc.name  : Test Memory
 * @tc.number: Memory_001
 * @tc.desc  : Test Memory !allocMem
 */
HWTEST_F(PluginBufferUnitTest, Memory_001, TestSize.Level1)
{
    bool allocMem = false;
    Memory memory(capacity_, nullptr, align_, type_, allocMem);
    EXPECT_TRUE(memory.addr == nullptr);
}

/**
 * @tc.name  : Test Write
 * @tc.number: Write_001
 * @tc.desc  : Test Write position == MEM_INVALID_POSITION
 */
HWTEST_F(PluginBufferUnitTest, Write_001, TestSize.Level1)
{
    const uint8_t in = TEST_IN;
    size_t writeSize = TEST_WRITE_SIZE;
    size_t position = MEM_INVALID_POSITION;
    size_t result = memory_.Write(&in, writeSize, position);
    EXPECT_EQ(result, writeSize);
}

/**
 * @tc.name  : Test Read
 * @tc.number: Read_001
 * @tc.desc  : Test Read position != MEM_INVALID_POSITION
 */
HWTEST_F(PluginBufferUnitTest, Read_001, TestSize.Level1)
{
    size_t position = MEM_VALID_POSITION;
    const uint8_t in = TEST_IN;
    size_t writeSize = TEST_WRITE_SIZE;
    memory_.Write(&in, writeSize, position);
    uint8_t out[100] = {0};
    size_t readSize = TEST_READ_SIZE;
    size_t result = memory_.Read(out, readSize, position);
    EXPECT_EQ(result, readSize);
}

/**
 * @tc.name  : Test Read
 * @tc.number: Read_002
 * @tc.desc  : Test Read position == MEM_INVALID_POSITION
 */
HWTEST_F(PluginBufferUnitTest, Read_002, TestSize.Level1)
{
    size_t position = MEM_VALID_POSITION;
    const uint8_t in = TEST_IN;
    size_t writeSize = TEST_WRITE_SIZE;
    memory_.Write(&in, writeSize, position);
    uint8_t out[100] = {0};
    size_t readSize = TEST_READ_SIZE;
    position = MEM_INVALID_POSITION;
    size_t result = memory_.Read(out, readSize, position);
    EXPECT_EQ(result, readSize);
}

/**
 * @tc.name  : Test GetReadOnlyData
 * @tc.number: GetReadOnlyData_001
 * @tc.desc  : Test GetReadOnlyData position > capacity_
 */
HWTEST_F(PluginBufferUnitTest, GetReadOnlyData_001, TestSize.Level1)
{
    size_t position = capacity_ + 1;
    const uint8_t* data = memory_.GetReadOnlyData(position);
    EXPECT_EQ(data, nullptr);
}

/**
 * @tc.name  : Test GetWritableAddr
 * @tc.number: GetWritableAddr_001
 * @tc.desc  : Test GetWritableAddr position + estimatedWriteSize > capacity_
 */
HWTEST_F(PluginBufferUnitTest, GetWritableAddr_001, TestSize.Level1)
{
    size_t estimatedWriteSize = capacity_ + 1;
    size_t position = MEM_VALID_POSITION;
    uint8_t* writableAddr = memory_.GetWritableAddr(estimatedWriteSize, position);
    EXPECT_EQ(writableAddr, nullptr);
}

/**
 * @tc.name  : Test UpdateDataSize
 * @tc.number: UpdateDataSize_001
 * @tc.desc  : Test UpdateDataSize position + realWriteSize > capacity_
 */
HWTEST_F(PluginBufferUnitTest, UpdateDataSize_001, TestSize.Level1)
{
    size_t realWriteSize = capacity_ + 1;
    size_t position = MEM_VALID_POSITION;
    memory_.UpdateDataSize(realWriteSize, position);
    EXPECT_EQ(memory_.GetSize(), 0);
}