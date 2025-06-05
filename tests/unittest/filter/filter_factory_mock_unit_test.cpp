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
 
#include "mock_filter.h"
#include <gtest/gtest.h>
#include "filter/filter_factory.h"
 
 
using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::Media::Pipeline;
 
namespace OHOS {
namespace Media {
namespace FilterUnitTest {
class FilterFactoryMockTest : public testing::Test {
public:
    // SetUpTestCase: Called before all test cases
    static void SetUpTestCase(void) { }
    // TearDownTestCase: Called after all test case
    static void TearDownTestCase(void) { }
    // SetUp: Called before each test cases
    void SetUp(void)
    {
        FilterFactory::Instance().generators.clear();
    }
    // SetUp: Called before each test cases
    void TearDown(void)
    {
        FilterFactory::Instance().generators.clear();
    }
};
 
std::shared_ptr<Filter> GenerateOneFilter(const std::string &name, const FilterType type)
{
    return std::make_shared<MockFilterForTest>(name, type);
}
 
HWTEST_F(FilterFactoryMockTest, Test_RegisterFilter_Case1, TestSize.Level1)
{
    auto &factory = FilterFactory::Instance();
    AutoRegisterFilter<MockFilterForTest> reg1("TestSourceFilterA", FilterType::FILTERTYPE_SOURCE);
    EXPECT_EQ(factory.generators.size(), 1); // expected only one generator
    AutoRegisterFilter<MockFilterForTest> reg2("TestSourceFilterB", FilterType::FILTERTYPE_DEMUXER);
    EXPECT_EQ(factory.generators.size(), 2); // expected two generators
    AutoRegisterFilter<MockFilterForTest> reg3("TestSourceFilterA", FilterType::FILTERTYPE_SOURCE);
    EXPECT_EQ(factory.generators.size(), 2); // expected two generators
    ASSERT_TRUE(factory.generators.find(FilterType::FILTERTYPE_SOURCE) != factory.generators.end());
    ASSERT_TRUE(factory.generators[FilterType::FILTERTYPE_SOURCE] == nullptr);
}
 
HWTEST_F(FilterFactoryMockTest, Test_RegisterFilter_Case2, TestSize.Level1)
{
    auto &factory = FilterFactory::Instance();
    AutoRegisterFilter<MockFilterForTest> reg1("TestSourceFilterA", FilterType::FILTERTYPE_SOURCE);
    EXPECT_EQ(factory.generators.size(), 1); // expected only one generator
    AutoRegisterFilter<MockFilterForTest> reg2("TestSourceFilterB", FilterType::FILTERTYPE_DEMUXER, GenerateOneFilter);
    EXPECT_EQ(factory.generators.size(), 2); // expected two generators
    AutoRegisterFilter<MockFilterForTest> reg3("TestSourceFilterA", FilterType::FILTERTYPE_SOURCE, GenerateOneFilter);
    EXPECT_EQ(factory.generators.size(), 2); // expected two generators
    ASSERT_TRUE(factory.generators.find(FilterType::FILTERTYPE_SOURCE) != factory.generators.end());
    ASSERT_TRUE(factory.generators[FilterType::FILTERTYPE_SOURCE] != nullptr);
}
 
HWTEST_F(FilterFactoryMockTest, Test_CreateFilter, TestSize.Level1)
{
    auto &factory = FilterFactory::Instance();
    AutoRegisterFilter<MockFilterForTest> reg1("TestSourceFilterA", FilterType::FILTERTYPE_SOURCE, GenerateOneFilter);
    auto filter1 = factory.CreateFilter<MockFilterForTest>("filter1", FilterType::FILTERTYPE_SOURCE);
    EXPECT_FALSE(filter1 == nullptr);
    auto filter2 = factory.CreateFilter<MockFilterForTest>("filter2", FilterType::FILTERTYPE_DEMUXER);
    EXPECT_TRUE(filter2 == nullptr);
}
} // namespace FilterUnitTest
} // namespace Media
} // namespace OHOS