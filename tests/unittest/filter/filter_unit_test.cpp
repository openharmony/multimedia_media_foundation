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
#include "filter/filter.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::Media::Pipeline;

namespace OHOS {
namespace Media {
namespace FilterUnitTest {
class FilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
};

void FilterUnitTest::SetUpTestCase(void) {}

void FilterUnitTest::TearDownTestCase(void) {}

void FilterUnitTest::SetUp(void) {}

void FilterUnitTest::TearDown(void) {}

/**
 * @tc.name: LinkPipline_001
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_VENC
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_002
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_VDEC
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VDEC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_003
 * @tc.desc: Test LinkPipline interface, set filtertype to VIDEO_CAPTURE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::VIDEO_CAPTURE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_004
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_ASINK
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_004, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_ASINK, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_005
 * @tc.desc: Test LinkPipline interface, set filtertype to AUDIO_CAPTURE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_005, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::AUDIO_CAPTURE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_006
 * @tc.desc: Test LinkPipline interface, set filtertype to AUDIO_DATA_SOURCE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_006, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::AUDIO_DATA_SOURCE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_007
 * @tc.desc: Test LinkPipline interface, set asyncMode to false
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_007, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: Prepare_001
 * @tc.desc: Test Prepare interface, set asyncMode to false
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
    filter->Prepare();
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: Prepare_002
 * @tc.desc: Test Prepare interface, set asyncMode to true
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->Prepare();
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: Prepare_003
 * @tc.desc: Test Prepare interface, set state is error
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->ChangeState(FilterState::ERROR);
    EXPECT_EQ(Status::ERROR_INVALID_OPERATION, filter->Prepare());
}

/**
 * @tc.name: PrepareDone_001
 * @tc.desc: Test PrepareDone interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, PrepareDone_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->Prepare();
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: PrepareFrame_001
 * @tc.desc: Test PrepareFrame interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, PrepareFrame_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    EXPECT_EQ(Status::OK, filter->PrepareFrame(true));
}

/**
 * @tc.name: PrepareFrame_002
 * @tc.desc: Test PrepareFrame interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, PrepareFrame_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->PrepareFrame(true);
    filter2->PrepareFrame(true);
    EXPECT_EQ(Status::OK, filter->PrepareFrame(true));
}

/**
 * @tc.name: WaitPrepareFrame_001
 * @tc.desc: Test WaitPrepareFrame interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitPrepareFrame_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    EXPECT_EQ(Status::OK, filter->WaitPrepareFrame());
}

/**
 * @tc.name: Release_001
 * @tc.desc: Test Release interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Release_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->Start());
    EXPECT_EQ(Status::OK, filter->Pause());
    EXPECT_EQ(Status::OK, filter->Resume());
    EXPECT_EQ(Status::OK, filter->Stop());
    EXPECT_EQ(Status::OK, filter->Release());
}

/**
 * @tc.name: InputOutputBuffer_001
 * @tc.desc: Test ProcessInputBuffer and ProcessInOutputBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, InputOutputBuffer_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->ProcessInputBuffer(0, 0));
    EXPECT_EQ(Status::OK, filter->ProcessOutputBuffer(0, 0));
}

/**
 * @tc.name: WaitAllState_001
 * @tc.desc: Test WaitAllState interface, wait PREPARING error
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->WaitAllState(FilterState::PREPARING));
    filter->ChangeState(FilterState::ERROR);
}

/**
 * @tc.name: WaitAllState_002
 * @tc.desc: Test WaitAllState interface, wait INITIALIZED timeout
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->ChangeState(FilterState::ERROR);
    EXPECT_NE(Status::OK, filter->WaitAllState(FilterState::INITIALIZED));
}

/**
 * @tc.name: WaitAllState_003
 * @tc.desc: Test WaitAllState interface, wait INITIALIZED success
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->WaitAllState(FilterState::INITIALIZED));
}
} // namespace FilterUnitTest
} // namespace Media
} // namespace OHOS