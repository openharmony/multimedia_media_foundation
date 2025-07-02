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

#include "filter_unittest.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
using namespace std;
using namespace testing;
using namespace testing::ext;

static const int32_t NUM_1 = 1;

void FilterUnitTest::SetUpTestCase(void) {}

void FilterUnitTest::TearDownTestCase(void) {}

void FilterUnitTest::SetUp(void)
{
    filter_ = std::make_shared<Filter>("Filter", FilterType::FILTERTYPE_SOURCE);
}

void FilterUnitTest::TearDown(void)
{
    filter_ = nullptr;
}

/**
 * @tc.name  : Test PrepareDone
 * @tc.number: PrepareDone_001
 * @tc.desc  : Test ret != Status::OK
 */
HWTEST_F(FilterUnitTest, PrepareDone_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    filter_->curState_ = FilterState::CREATED;
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto innerFilter = std::make_shared<Filter>("innerFilter", FilterType::FILTERTYPE_SOURCE);
    innerFilter->curState_ = FilterState::ERROR;
    std::vector<std::shared_ptr<Filter>> vec = { innerFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->PrepareDone();
    EXPECT_EQ(ret, Status::ERROR_INVALID_OPERATION);
}

/**
 * @tc.name  : Test Freeze
 * @tc.number: Freeze_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, Freeze_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->Freeze();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test UnFreeze
 * @tc.number: UnFreeze_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, UnFreeze_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->UnFreeze();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test PauseDragging
 * @tc.number: PauseDragging_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, PauseDragging_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->PauseDragging();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test PauseAudioAlign
 * @tc.number: PauseAudioAlign_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, PauseAudioAlign_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->PauseAudioAlign();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test ResumeDragging
 * @tc.number: ResumeDragging_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, ResumeDragging_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->ResumeDragging();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test ResumeAudioAlign
 * @tc.number: ResumeAudioAlign_001
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, ResumeAudioAlign_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->ResumeAudioAlign();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test Preroll
 * @tc.number: Preroll_001
 * @tc.desc  : Test ret != Status::OK
 */
HWTEST_F(FilterUnitTest, Preroll_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    auto ret = testFilter->Preroll();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test Preroll
 * @tc.number: Preroll_002
 * @tc.desc  : Test nextFiltersMap_ : ret != Status::OK
 */
HWTEST_F(FilterUnitTest, Preroll_002, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    auto ret = filter_->Preroll();
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test WaitPrerollDone
 * @tc.number: WaitPrerollDone_001
 * @tc.desc  : Test nextFiltersMap_ : curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, WaitPrerollDone_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    StreamType type = StreamType::STREAMTYPE_PACKED;
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    std::vector<std::shared_ptr<Filter>> vec = { testFilter };
    filter_->nextFiltersMap_[type] = vec;
    bool render = true;
    auto ret = filter_->WaitPrerollDone(render);
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test WaitPrerollDone
 * @tc.number: WaitPrerollDone_002
 * @tc.desc  : Test curRet != Status::OK
 */
HWTEST_F(FilterUnitTest, WaitPrerollDone_002, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    auto testFilter = std::make_shared<TestFilter>("testFilter", FilterType::FILTERTYPE_SOURCE);
    bool render = true;
    auto ret = testFilter->WaitPrerollDone(render);
    EXPECT_EQ(ret, Status::ERROR_UNKNOWN);
}

/**
 * @tc.name  : Test ProcessOutputBuffer
 * @tc.number: ProcessOutputBuffer_001
 * @tc.desc  : Test isAsyncMode_ == false && filterTask_ == nullptr
 */
HWTEST_F(FilterUnitTest, ProcessOutputBuffer_001, TestSize.Level0)
{
    ASSERT_NE(filter_, nullptr);
    EXPECT_FALSE(filter_->isAsyncMode_);
    auto ret = filter_->ProcessOutputBuffer(NUM_1, NUM_1, true, NUM_1, NUM_1);
    EXPECT_EQ(ret, Status::OK);
}

} // namespace Pipeline
} // namespace Media
} // namespace OHOS