/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <plugin/plugin_base.h>
#include "gtest/gtest.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugins;
using namespace testing::ext;
class TestCallbackImpl : public Callback {
public:
    bool isTest = false;
    void OnEvent(const PluginEvent &event) override
    {
        (void)event;
    }
    void SetSelectBitRateFlag(bool flag, uint32_t desBitRate) override
    {
        isTest = true;
        (void)flag;
        (void)desBitRate;
    }
    bool CanAutoSelectBitRate() override
    {
        return false;
    }
};

/**
 * @tc.name: testPluginBase_001
 * @tc.desc: testPluginBase_001
 * @tc.type: FUNC
 */
HWTEST(PluginBaseTest, testPluginBase_001, TestSize.Level1)
{
    PluginBase def("testPluginBase_001");
    TestCallbackImpl cb {};
    EXPECT_EQ(Status::OK, def.SetCallback(&cb));
}

/**
 * @tc.name: testCallback_001
 * @tc.desc: testCallback_001
 * @tc.type: FUNC
 */
HWTEST(PluginBaseTest, testCallback_001, TestSize.Level1)
{
    TestCallbackImpl cb {};
    uint32_t desBitRate = 0;
    cb.SetSelectBitRateFlag(true, desBitRate);
    EXPECT_EQ(true, cb.isTest);
}

/**
 * @tc.name: testCallback_002
 * @tc.desc: testCallback_002
 * @tc.type: FUNC
 */
HWTEST(PluginBaseTest, testCallback_002, TestSize.Level1)
{
    TestCallbackImpl cb {};
    EXPECT_EQ(false, cb.CanAutoSelectBitRate());
}
} // namespace Test
} // namespace Media
} // namespace OHOS