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
#include "common/avsharedmemorypool.h"
#include "mock/mock_avsharedmemory.h"

namespace OHOS {
namespace Media {
constexpr uint32_t INIT_MEM_CNT = 10;
constexpr int32_t MEM_SIZE = 1024;
constexpr uint32_t MAX_MEM_CNT = 100;
const int32_t MIN_MEM_SIZE = 1000;
const int32_t MAX_MEM_SIZE = 2 * 1024;
const int32_t TEST_SIZE = -1;
const std::string TEST_POOL_NAME = "acquireMemory";

class AVSharedMemoryPoolUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void)
    {
        pool_ = std::make_shared<AVSharedMemoryPool>(TEST_POOL_NAME);
    };
    void TearDown(void) {};

public:
    AVSharedMemoryPool::InitializeOption initOption_ {
        INIT_MEM_CNT,
        MEM_SIZE,
        MAX_MEM_CNT,
        AVSharedMemory::Flags::FLAGS_READ_WRITE,
        true,
        nullptr,
    };
    std::shared_ptr<AVSharedMemoryPool> pool_ = nullptr;
};
} // namespace Media
} // namespace OHOS
#endif // PLUGIN_BUFFER_UNITTEST_H