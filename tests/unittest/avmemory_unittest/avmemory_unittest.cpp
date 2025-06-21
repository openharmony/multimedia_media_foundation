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

#include "avmemory_unittest.h"
#include "surface_buffer.h"
namespace OHOS {
namespace Media {
using namespace testing;
using namespace testing::ext;

void AVMemoryUnitTest::SetUpTestCase(void) {}
void AVMemoryUnitTest::TearDownTestCase(void) {}
void AVMemoryUnitTest::SetUp(void) {}
void AVMemoryUnitTest::TearDown(void) {}

/**
 * @tc.name  : Test CreateAVMemory API
 * @tc.number: CreateAVMemory_001
 * @tc.desc  : Test case default
 */
HWTEST_F(AVMemoryUnitTest, CreateAVMemory_001, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    MemoryType type = MemoryType::UNKNOWN_MEMORY;
    std::shared_ptr<MockAVAllocator> mockAllocator = std::make_shared<MockAVAllocator>();
    EXPECT_CALL(*(mockAllocator), GetMemoryType()).WillRepeatedly(Return(type));
    std::shared_ptr<AVAllocator> allocator = mockAllocator;
    auto ret = AVMemory::CreateAVMemory(allocator);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name  : Test CreateAVMemory API
 * @tc.number: CreateAVMemory_002
 * @tc.desc  : Test case MemoryType::VIRTUAL_MEMORY
 */
HWTEST_F(AVMemoryUnitTest, CreateAVMemory_002, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    MessageParcel parcel;
    parcel.writable_ = true;
    auto ret = AVMemory::CreateAVMemory(parcel, false);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name  : Test CreateAVMemory API
 * @tc.number: CreateAVMemory_003
 * @tc.desc  : Test case default
 */
HWTEST_F(AVMemoryUnitTest, CreateAVMemory_003, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    MessageParcel parcel;
    parcel.writable_ = false;
    auto ret = AVMemory::CreateAVMemory(parcel, false);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name  : Test Init API
 * @tc.number: Init_001
 * @tc.desc  : Test all
 */
HWTEST_F(AVMemoryUnitTest, Init_001, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    auto ret = avMemory->Init();
    EXPECT_EQ(ret, Status::ERROR_UNIMPLEMENTED);

    MessageParcel parcel;
    ret = avMemory->Init(parcel);
    EXPECT_EQ(ret, Status::ERROR_UNIMPLEMENTED);
}

/**
 * @tc.name  : Test InitSurfaceBuffer API
 * @tc.number: InitSurfaceBuffer_001
 * @tc.desc  : Test all
 */
HWTEST_F(AVMemoryUnitTest, InitSurfaceBuffer_001, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    sptr<OHOS::SurfaceBuffer> surfaceBuffer;
    auto ret = avMemory->InitSurfaceBuffer(surfaceBuffer);
    EXPECT_EQ(ret, Status::ERROR_UNIMPLEMENTED);

    MessageParcel parcel;
    ret = avMemory->InitSurfaceBuffer(parcel);
    EXPECT_EQ(ret, Status::ERROR_UNIMPLEMENTED);
}

/**
 * @tc.name  : Test ReadFromMessageParcel API
 * @tc.number: ReadFromMessageParcel_001
 * @tc.desc  : Test all
 */
HWTEST_F(AVMemoryUnitTest, ReadFromMessageParcel_001, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    MessageParcel parcel;
    auto ret = avMemory->ReadFromMessageParcel(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name  : Test WriteToMessageParcel API
 * @tc.number: WriteToMessageParcel_001
 * @tc.desc  : Test all
 */
HWTEST_F(AVMemoryUnitTest, WriteToMessageParcel_001, TestSize.Level0)
{
    auto avMemory = std::make_shared<AVMemory>();
    ASSERT_NE(avMemory, nullptr);
    MessageParcel parcel;
    auto ret = avMemory->WriteToMessageParcel(parcel);
    EXPECT_EQ(ret, false);
}
} // namespace Media
} // namespace OHOS