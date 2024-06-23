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
#ifndef PIPELINE_UNITTEST_H
#define PIPELINE_UNITTEST_H
#include <gtest/gtest.h>
#include "pipeline/pipeline.h"
#include "filter/filter.h"
namespace OHOS {
namespace Media {
namespace PiplineFuncUT {
class TestFilter : public Pipeline::Filter {
public:
    TestFilter(std::string name, Pipeline::FilterType type): Pipeline::Filter(std::move(name), type) {};
    ~TestFilter() override = default;
};
class PiplineUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
private:
    std::shared_ptr<Pipeline::Pipeline> pipeline_;
    std::shared_ptr<TestFilter> filterOne_;
    std::shared_ptr<TestFilter> filterTwo_;
    std::string testId;
};
} // namespace PiplineFuncUT
} // namespace Media
} // namespace OHOS
#endif // PIPELINE_UNITTEST_H