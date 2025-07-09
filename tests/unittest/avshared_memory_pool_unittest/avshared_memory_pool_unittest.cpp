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

#include "avshared_memory_pool_unittest.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;

/**
 * @tc.name  : Test CheckSize
 * @tc.number: CheckSize_001
 * @tc.desc  : Test ReleaseMemory notifier_ != nullptr
 *             Test ReleaseMemory *iter != memory
 *             Test CheckSize !option_.enableFixedSize && size == -1
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, CheckSize_001, TestSize.Level1)
{
    // Test ReleaseMemory notifier_ != nullptr
    auto notify = [](){};
    AVSharedMemory *memory = new (std::nothrow) MockAVSharedMemory();
    pool_->busyList_.push_back(memory);
    pool_->notifier_ = notify;
    pool_->ReleaseMemory(memory);

    // Test ReleaseMemory *iter != memory
    MockAVSharedMemory *memory1 = new (std::nothrow) MockAVSharedMemory();
    MockAVSharedMemory *memory2 = new (std::nothrow) MockAVSharedMemory();
    pool_->busyList_.push_back(memory1);
    pool_->busyList_.push_back(memory2);
    pool_->ReleaseMemory(memory2);
    pool_->ReleaseMemory(memory1);

    // Test CheckSize !option_.enableFixedSize && size == -1
    initOption_.enableFixedSize = false;
    pool_->option_ = initOption_;
    int32_t size = TEST_SIZE;
    bool result = pool_->CheckSize(size);
    EXPECT_EQ(false, result);
    pool_->Reset();
}

/**
 * @tc.name  : Test DoAcquireMemory
 * @tc.number: DoAcquireMemory_001
 * @tc.desc  : Test DoAcquireMemory (*iter)->GetSize() >= size
 *             Test DoAcquireMemory result != nullptr
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, DoAcquireMemory_001, TestSize.Level1)
{
    pool_->Init(initOption_);
    AVSharedMemory *memory = nullptr;
    bool result = pool_->DoAcquireMemory(MIN_MEM_SIZE, &memory);
    EXPECT_TRUE(nullptr != memory);
    delete memory;
    EXPECT_EQ(true, result);
}

/**
 * @tc.name  : Test DoAcquireMemory
 * @tc.number: DoAcquireMemory_002
 * @tc.desc  : Test DoAcquireMemory (*iter)->GetSize() < minIdleSize
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, DoAcquireMemory_002, TestSize.Level1)
{
    pool_->Init(initOption_);
    AVSharedMemory *memory = nullptr;
    bool result = pool_->DoAcquireMemory(MAX_MEM_SIZE, &memory);
    delete memory;
    EXPECT_EQ(true, result);
}

/**
 * @tc.name  : Test DoAcquireMemory
 * @tc.number: DoAcquireMemory_003
 * @tc.desc  : Test DoAcquireMemory (*iter)->GetSize() > minIdleSize
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, DoAcquireMemory_003, TestSize.Level1)
{
    MockAVSharedMemory *mockMemory = new (std::nothrow) MockAVSharedMemory();
    int32_t maxSize = std::numeric_limits<int32_t>::max();
    EXPECT_CALL(*(mockMemory), GetSize()).WillRepeatedly(Return(maxSize));
    pool_->idleList_.push_back(mockMemory);
    AVSharedMemory *memory = nullptr;
    bool result = pool_->DoAcquireMemory(MEM_SIZE, &memory);
    delete memory;
    EXPECT_EQ(true, result);
}

/**
 * @tc.name  : Test DoAcquireMemory
 * @tc.number: DoAcquireMemory_004
 * @tc.desc  : Test DoAcquireMemory !option_.enableFixedSize && minSizeIdleMem != idleList_.end()
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, DoAcquireMemory_004, TestSize.Level1)
{
    initOption_.enableFixedSize = false;
    pool_->Init(initOption_);
    AVSharedMemory *memory = nullptr;
    bool result = pool_->DoAcquireMemory(MAX_MEM_SIZE, &memory);
    delete memory;
    EXPECT_EQ(true, result);
}

/**
 * @tc.name  : Test AcquireMemory
 * @tc.number: AcquireMemory_001
 * @tc.desc  : Test AcquireMemory !option_.enableFixedSize
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, AcquireMemory_001, TestSize.Level1)
{
    initOption_.enableFixedSize = false;
    pool_->Init(initOption_);
    auto memory = pool_->AcquireMemory(MEM_SIZE, true);
    EXPECT_TRUE(memory != nullptr);
}

/**
 * @tc.name  : Test AcquireMemory
 * @tc.number: AcquireMemory_002
 * @tc.desc  : Test AcquireMemory !blocking || forceNonBlocking_
 */
HWTEST_F(AVSharedMemoryPoolUnitTest, AcquireMemory_002, TestSize.Level1)
{
    initOption_.enableFixedSize = false;
    pool_->Init(initOption_);
    auto memory1 = pool_->AcquireMemory(MEM_SIZE, false);
    EXPECT_TRUE(memory1 != nullptr);
    auto memory2 = pool_->AcquireMemory(MEM_SIZE, true);
    EXPECT_TRUE(memory2 != nullptr);

    pool_->SetNonBlocking(true);
    auto memory3 = pool_->AcquireMemory(MEM_SIZE, false);
    EXPECT_TRUE(memory3 != nullptr);
    auto memory4 = pool_->AcquireMemory(MEM_SIZE, true);
    EXPECT_TRUE(memory4 != nullptr);
}