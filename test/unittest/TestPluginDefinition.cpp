/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "plugin/plugin_definition.h"
#include "gtest/gtest.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugins;
using namespace testing::ext;

class MockDataSource : public DataSource {
public:
    Status ReadAt(int64_t offset, std::shared_ptr<Buffer>& buffer, size_t expectedLen) override
    {
        (void)offset;
        (void)buffer;
        (void)expectedLen;
        return Status::OK;
    }

    Status GetSize(uint64_t& size) override
    {
        size = 0;
        return Status::OK;
    }

    Seekable GetSeekable() override
    {
        return Seekable::SEEKABLE;
    }

    int32_t GetStreamID() override
    {
        return 0;
    }

    bool IsDash() override
    {
        return false;
    }

    uint64_t GetSniffSize() override
    {
        return sniffSize_;
    }

    void SetSniffSize(uint64_t sniffSize) override
    {
        sniffSize_ = sniffSize;
    }

private:
    uint64_t sniffSize_ = 0;
};

HWTEST(PluginDefinitionTest, test_PluginDefBase, TestSize.Level1)
{
    PluginDefBase def;
    ASSERT_TRUE(def.GetExtensions().empty());
    ASSERT_TRUE(def.GetInCaps().empty());
    ASSERT_TRUE(def.GetOutCaps().empty());
}

HWTEST(PluginDefinitionTest, test_DataSource_GetSniffSize_Default, TestSize.Level1)
{
    MockDataSource dataSource;
    uint64_t size = dataSource.GetSniffSize();
    ASSERT_EQ(size, 0);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSizeAndGetSniffSize, TestSize.Level1)
{
    MockDataSource dataSource;
    uint64_t testSize = 1024;
    dataSource.SetSniffSize(testSize);
    uint64_t size = dataSource.GetSniffSize();
    ASSERT_EQ(size, testSize);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSize_Zero, TestSize.Level1)
{
    MockDataSource dataSource;
    dataSource.SetSniffSize(0);
    uint64_t size = dataSource.GetSniffSize();
    ASSERT_EQ(size, 0);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSize_Max, TestSize.Level1)
{
    MockDataSource dataSource;
    uint64_t maxSize = UINT64_MAX;
    dataSource.SetSniffSize(maxSize);
    uint64_t size = dataSource.GetSniffSize();
    ASSERT_EQ(size, maxSize);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSize_MultipleTimes, TestSize.Level1)
{
    MockDataSource dataSource;
    dataSource.SetSniffSize(100);
    ASSERT_EQ(dataSource.GetSniffSize(), 100);
    dataSource.SetSniffSize(200);
    ASSERT_EQ(dataSource.GetSniffSize(), 200);
    dataSource.SetSniffSize(300);
    ASSERT_EQ(dataSource.GetSniffSize(), 300);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSize_SmallValue, TestSize.Level1)
{
    MockDataSource dataSource;
    dataSource.SetSniffSize(1);
    ASSERT_EQ(dataSource.GetSniffSize(), 1);
}

HWTEST(PluginDefinitionTest, test_DataSource_SetSniffSize_LargeValue, TestSize.Level1)
{
    MockDataSource dataSource;
    uint64_t largeValue = 1024 * 1024 * 1024;
    dataSource.SetSniffSize(largeValue);
    ASSERT_EQ(dataSource.GetSniffSize(), largeValue);
}
} // namespace Test
} // namespace Media
} // namespace OHOS
