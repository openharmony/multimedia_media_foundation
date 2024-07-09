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
#include "av_hardware_memory.h"
#include "gtest/gtest.h"
#include "osal/utils/dump_buffer.h"
#include "common/media_core.h"
#include "meta/meta.h"
#include "buffer/avbuffer.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace {
std::string DUMP_PARAM = "w";
std::string DUMP_FILE_NAME = "DumpBufferTest.es";
const int DUMP_SIZE = 1024;
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

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_002, TestSize.Level1)
{
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = nullptr;
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
}

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_003, TestSize.Level1)
{
    DUMP_PARAM = "invalid";
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = std::shared_ptr<AVMemory>(new AVHardwareMemory());
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
    DUMP_PARAM = "w";
}

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_004, TestSize.Level1)
{
    DUMP_FILE_NAME = "";
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = std::shared_ptr<AVMemory>(new AVHardwareMemory());
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
    DUMP_FILE_NAME = "DumpBufferTest.es";
}

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_005, TestSize.Level1)
{
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = std::shared_ptr<AVMemory>(new AVHardwareMemory());
    outputBuffer->memory_->SetSize(0);
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
}

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_006, TestSize.Level1)
{
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = std::shared_ptr<AVMemory>(new AVHardwareMemory());
    outputBuffer->memory_->SetSize(DUMP_SIZE);
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
}

HWTEST_F(DumpBufferTest, DumpAVBufferToFile_007, TestSize.Level1)
{
    std::shared_ptr<Media::AVBuffer> outputBuffer = std::make_shared<AVBuffer>();
    outputBuffer->memory_ = std::shared_ptr<AVMemory>(new AVHardwareMemory());
    outputBuffer->memory_->SetSize(DUMP_SIZE);
    outputBuffer->memory_->base_ = new uint8_t[DUMP_SIZE];
    DumpAVBufferToFile(DUMP_PARAM, DUMP_FILE_NAME, outputBuffer);
}

}
}