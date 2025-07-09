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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "avbuffer_queue_supplement_unit_test.h"
#include "status.h"
#include "buffer/avbuffer_queue.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace Media {
static const uint32_t NUM = -1;
static const uint32_t NUM_0 = 0;
static const uint32_t NUM_1 = 1;
static const uint32_t NUM_10 = 10;

void AVBufferQueueSupplementUnitTest::SetUpTestCase(void) {}

void AVBufferQueueSupplementUnitTest::TearDownTestCase(void) {}

void AVBufferQueueSupplementUnitTest::SetUp(void)
{
    name = "queue";
}

void AVBufferQueueSupplementUnitTest::TearDown(void) {}

/**
 * @tc.name: ClearBufferIfTest_001
 * @tc.desc: Test ClearBufferIf interface
 * @tc.type: cacheIt == cachedBufferMap_.end()
 *           cacheIt->second.state != AVBUFFER_STATE_PUSHED
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, ClearBufferIfTest, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);
    uint64_t uniqueId = NUM_1;
    avBufferQueueImpl->dirtyBufferList_.push_back(uniqueId);
    auto isNewerSample = [](const std::shared_ptr<AVBuffer>& buffer) {
        return buffer == nullptr;
    };
    EXPECT_EQ(Status::OK, avBufferQueueImpl->ClearBufferIf(isNewerSample));


    AVBufferConfig config;
    config.size = NUM;
    config.capacity = NUM_0;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;

    AVBufferElement ele = {
        .config = config,
        .state = AVBUFFER_STATE_RELEASED,
        .isDeleting = false,
        .buffer = nullptr,
    };
    avBufferQueueImpl->cachedBufferMap_[uniqueId] = ele;
    EXPECT_EQ(Status::OK, avBufferQueueImpl->ClearBufferIf(isNewerSample));
}

/**
 * @tc.name: ReturnBuffer
 * @tc.desc: Test ReturnBuffer interface
 * @tc.type: cachedBufferMap_[uniqueId].isDeleting
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, ReturnBuffer, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);

    uint64_t uniqueId = NUM_1;
    AVBufferConfig config;
    config.size = NUM_1;
    config.capacity = NUM_1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);

    AVBufferElement ele = {
        .config = config,
        .state = AVBUFFER_STATE_RELEASED,
        .isDeleting = true,
        .buffer = nullptr,
    };
    avBufferQueueImpl->cachedBufferMap_[uniqueId] = ele;
    EXPECT_EQ(avBufferQueueImpl->ReturnBuffer(uniqueId, true), Status::OK);
}

/**
 * @tc.name: ReleaseBuffer
 * @tc.desc: Test ReleaseBuffer interface
 * @tc.type: cachedBufferMap_[uniqueId].isDeleting
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, ReleaseBuffer, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);

    uint64_t uniqueId = NUM_1;
    AVBufferConfig config;
    config.size = NUM_1;
    config.capacity = NUM_1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);

    AVBufferElement ele = {
        .config = config,
        .state = AVBUFFER_STATE_ACQUIRED,
        .isDeleting = true,
        .buffer = nullptr,
    };
    avBufferQueueImpl->cachedBufferMap_[uniqueId] = ele;
    EXPECT_EQ(avBufferQueueImpl->ReleaseBuffer(uniqueId), Status::OK);
}

/**
 * @tc.name: PushBufferOnFilled
 * @tc.desc: Test PushBufferOnFilled interface
 * @tc.type: ret != Status::OK
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, PushBufferOnFilled, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);
    AVBufferConfig config;
    config.size = NUM_1;
    config.capacity = NUM_1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;

    EXPECT_EQ(avBufferQueueImpl->PushBufferOnFilled(NUM_1, false), Status::ERROR_INVALID_BUFFER_ID);
}

/**
 * @tc.name: SetQueueSizeBeforeAttachBufferLocked_001
 * @tc.desc: Test SetQueueSizeBeforeAttachBufferLocked interface
 * @tc.type: size <= size_
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, SetQueueSizeBeforeAttachBufferLocked_001, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);
    uint32_t size = NUM_0;
    avBufferQueueImpl->SetQueueSizeBeforeAttachBufferLocked(size);
    EXPECT_EQ(avBufferQueueImpl->size_, size);
}

/**
 * @tc.name: SetQueueSizeBeforeAttachBufferLocked_002
 * @tc.desc: Test SetQueueSizeBeforeAttachBufferLocked interface
 * @tc.type: disableAlloc_
 */
HWTEST_F(AVBufferQueueSupplementUnitTest, SetQueueSizeBeforeAttachBufferLocked_002, TestSize.Level1)
{
    auto avBufferQueueImpl = std::make_shared<AVBufferQueueImpl>(name);
    uint32_t size = NUM_10;
    avBufferQueueImpl->disableAlloc_ = false;
    avBufferQueueImpl->SetQueueSizeBeforeAttachBufferLocked(size);
    EXPECT_EQ(avBufferQueueImpl->size_, size);
}
} // namespace Media
} // namespace OHOS
