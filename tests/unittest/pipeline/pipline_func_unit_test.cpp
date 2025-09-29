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
#include "pipline_func_unit_test.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace PiplineFuncUT {

void PiplineUnitTest::SetUpTestCase(void)
{
    std::cout << "[SetUpTestCase]: SetUp!!!" << std::endl;
}

void PiplineUnitTest::TearDownTestCase(void)
{
    std::cout << "[TearDownTestCase]: over!!!" << std::endl;
}

void PiplineUnitTest::SetUp(void)
{
    std::cout << "[SetUp]: SetUp!!!" << std::endl;
    pipeline_ = std::make_shared<Pipeline::Pipeline>();
    ASSERT_NE(pipeline_, nullptr);
    testId = std::string("Test_") + std::to_string(Pipeline::Pipeline::GetNextPipelineId());
    pipeline_->Init(nullptr, nullptr, testId);
    filterOne_ = std::make_shared<TestFilter>("filterOne", Pipeline::FilterType::AUDIO_CAPTURE);
    filterTwo_ = std::make_shared<TestFilter>("filterTwo", Pipeline::FilterType::FILTERTYPE_AENC);
}

void PiplineUnitTest::TearDown(void)
{
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: Pipeline_Test_AddHeadFilters_0100
 * @tc.desc: Pipeline_Test_AddHeadFilters_0100
 * @tc.type: FUNC
 */
HWTEST_F(PiplineUnitTest, Pipeline_Test_AddHeadFilters_0100, TestSize.Level1)
{
    EXPECT_EQ(pipeline_->AddHeadFilters({filterOne_, filterTwo_}), Status::OK);
}

/**
 * @tc.name: Pipeline_Test_LinkFilters_0100
 * @tc.desc: Pipeline_Test_LinkFilters_0100
 * @tc.type: FUNC
 */
HWTEST_F(PiplineUnitTest, Pipeline_Test_LinkFilters_0100, TestSize.Level1)
{
    EXPECT_EQ(pipeline_->AddHeadFilters({filterOne_}), Status::OK);
    EXPECT_EQ(pipeline_->LinkFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
    EXPECT_EQ(pipeline_->UpdateFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
    EXPECT_EQ(pipeline_->UnLinkFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
}

/**
 * @tc.name: Pipeline_Test_pipline_0100
 * @tc.desc: Pipeline_Test_pipline_0100
 * @tc.type: FUNC
 */
HWTEST_F(PiplineUnitTest, Pipeline_Test_pipline_0100, TestSize.Level1)
{
    EXPECT_EQ(pipeline_->AddHeadFilters({filterOne_}), Status::OK);
    EXPECT_EQ(pipeline_->LinkFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
    EXPECT_EQ(pipeline_->UpdateFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
    EXPECT_EQ(pipeline_->UnLinkFilters(filterOne_, {filterTwo_},
        Pipeline::StreamType::STREAMTYPE_ENCODED_AUDIO), Status::OK);
    EXPECT_EQ(pipeline_->Prepare(), Status::OK);
    EXPECT_EQ(pipeline_->Start(), Status::OK);
    Event event;
    event.type = EventType::EVENT_VIDEO_TRACK_CHANGE;
    event.param = 0;
    pipeline_->OnEvent(event);
    EXPECT_EQ(pipeline_->Pause(), Status::OK);
    EXPECT_EQ(pipeline_->Resume(), Status::OK);
    EXPECT_EQ(pipeline_->Stop(), Status::OK);
    EXPECT_EQ(pipeline_->Flush(), Status::OK);
    EXPECT_EQ(pipeline_->RemoveHeadFilter(filterOne_), Status::OK);
    EXPECT_EQ(pipeline_->Release(), Status::OK);
}

} // namespace PiplineFuncUT
} // namespace Media
} // namespace OHOS
