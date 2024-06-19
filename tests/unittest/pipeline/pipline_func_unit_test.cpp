/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
}

void PiplineUnitTest::TearDownTestCase(void)
{
}

void PiplineUnitTest::SetUp(void)
{
}

void PiplineUnitTest::TearDown(void)
{
}

/**
 * @tc.name: Pipeline_Test_0100
 * @tc.desc: Pipeline_Test_0100
 * @tc.type: FUNC
 */
HWTEST_F(PiplineFuncUT, Pipeline_Test_0100, TestSize.Level1)
{
    std::shared_ptr<Pipeline::Pipeline> pipeline_ = std::make_shared<Pipeline::Pipeline>();
    pipeline_->Init(nullptr, nullptr, 0);
    std::shared_ptr<TestFilter> testFilterOne = std::make_shared<TestFilter>();
    std::shared_ptr<TestFilter> testFilterTwo = std::make_shared<TestFilter>();
    pipeline_->AddHeadFilters({testFilterOne});
    pipeline_->LinkFilters(testFilterOne, {testFilterTwo}, StreamType::STREAMTYPE_ENCODED_AUDIO);
    pipeline_->UpdateFilters(testFilterOne, {testFilterTwo}, StreamType::STREAMTYPE_ENCODED_AUDIO);
    pipeline_->UnLinkFilters(testFilterOne, {testFilterTwo}, StreamType::STREAMTYPE_ENCODED_AUDIO);
    pipeline_->Prepare();
    pipeline_->Start();
    pipeline_->Pause();
    pipeline_->Resume();
    pipeline_->Stop();
    pipeline_->Flush();
    pipeline_->RemoveHeadFilter(filter);
    pipeline_->Release();
}
} // namespace PiplineFuncUT
} // namespace Media
} // namespace OHOS
