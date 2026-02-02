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

#include <gtest/gtest.h>
#include <string>
#include "meta/format.h"
#include "meta/source_types.h"
#include "unittest_log.h"
#include <cstdlib>

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace AnyFuncUT {
class FormatInnerUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
};

void FormatInnerUnitTest::SetUpTestCase(void) {}

void FormatInnerUnitTest::TearDownTestCase(void) {}

void FormatInnerUnitTest::SetUp(void)
{
    std::cout << "[SetUp]: SetUp!!!, test: ";
    const ::testing::TestInfo *testInfo_ = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string testName = testInfo_->name();
    std::cout << testName << std::endl;
}

void FormatInnerUnitTest::TearDown(void)
{
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: Format_GetUintValue
 * @tc.desc: Format_GetUintValue
 * @tc.type: FUNC
 */
HWTEST_F(FormatInnerUnitTest, Format_GetUintValue, TestSize.Level1)
{
    Format format;
    uint32_t resultValue = 0;
    std::string_view key = "buffer_size";
    bool isSuccess = format.GetUintValue(key, resultValue);
    EXPECT_EQ(false, isSuccess);
}
} // namespace AnyFuncUT
} // namespace Media
} // namespace OHOS
