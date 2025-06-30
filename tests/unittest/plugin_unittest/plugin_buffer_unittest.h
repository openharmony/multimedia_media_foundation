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
#ifndef PLUGIN_BUFFER_UNITTEST_H
#define PLUGIN_BUFFER_UNITTEST_H
#include "gtest/gtest.h"
#include "plugin/plugin_buffer.h"

namespace OHOS {
namespace Media {
namespace Plugins {
class PluginBufferUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void) {};
    void TearDown(void) {};

public:
    size_t align_ = 0;
    bool allocMem = true;
    size_t capacity_ = 1024 * 1024; // 1MB
    MemoryType type_ = MemoryType::VIRTUAL_MEMORY;
    Memory memory_{capacity_, nullptr, align_, type_, allocMem};
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // PLUGIN_BUFFER_UNITTEST_H