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

#include <plugin/plugin_definition.h>
#include "gtest/gtest.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugins;
using namespace testing::ext;
HWTEST(PluginDefinitionTest, test_PluginDefBase, TestSize.Level1)
{
    PluginDefBase def;
    ASSERT_TRUE(def.GetExtensions().empty());
    ASSERT_TRUE(def.GetInCaps().empty());
    ASSERT_TRUE(def.GetOCaps().empty());
}
} // namespace Test
} // namespace Media
} // namespace OHOS