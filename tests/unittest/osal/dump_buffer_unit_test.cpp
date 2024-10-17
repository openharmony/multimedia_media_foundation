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

#include <memory>
#include <iostream>
#include "gtest/gtest.h"
#include "osal/utils/dump_buffer.h"
#include "common/media_core.h"
#include "meta/meta.h"
#include "buffer/avbuffer.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace {
const std::string DUMP_PARAM = "w";
const std::string DUMP_FILE_NAME = "DumpBufferTest.es";
}

namespace OHOS {
namespace Media {
class DumpBufferTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void) {};
    void TearDown(void) {};
};

/**
 * @tc.name: DumpAVBufferToFile
 * @tc.desc: DumpAVBufferToFile
 * @tc.type: FUNC
 */
HWTEST_F(DumpBufferTest, DumpAVBufferToFile, TestSize.Level1)
{
    std::shared_ptr<Media::AVBuffer> outputBuffer = nullptr;
    outputBuffer = std::make_shared<AVBuffer>();
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
}
}
}