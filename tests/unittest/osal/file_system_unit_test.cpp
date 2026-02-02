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

#include "gtest/gtest.h"
#include "osal/filesystem/file_system.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace {
const std::string DUMP_PARAM = "w";
const std::string DUMP_FILE_NAME = "DumpBufferTest.es";
}

namespace OHOS {
namespace Media {
static const int32_t NUM_VALID = -1;
static const int32_t NUM_VALIDOONEHUNDRED = -100;
class FileSystemUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp(void) {};
    void TearDown(void) {};
};

/**
 * @tc.name: CreateFiles
 * @tc.desc: CreateFiles
 * @tc.type: FUNC
 */
HWTEST_F(FileSystemUnitTest, CreateFiles, TestSize.Level1)
{
    std::shared_ptr<FileSystem> fileSystem = std::make_shared<FileSystem>();
    EXPECT_TRUE(fileSystem->MakeMultipleDir("/data/test/makeMulti/dir"));
    EXPECT_TRUE(fileSystem->MakeDir("/data/test/makedir"));
    EXPECT_TRUE(fileSystem->IsDirectory("/data/test/makedir"));
    EXPECT_TRUE(fileSystem->IsExists("/data/test/makedir"));
    EXPECT_EQ(false, fileSystem->IsRegularFile("/data/test/makedir"));
    fileSystem->ClearFileContent("/data/test/makeMulti/dir");
    fileSystem->RemoveFilesInDir("/data/test/makedir");
    EXPECT_TRUE(fileSystem->IsExists("/data/test/makedir"));
}

/**
 * @tc.name: IsSocketFile
 * @tc.desc: Simple coverage for IsSocketFile with invalid fd.
 * @tc.type: FUNC
 */
HWTEST_F(FileSystemUnitTest, IsSocketFile, TestSize.Level1)
{
    std::shared_ptr<FileSystem> fileSystem = std::make_shared<FileSystem>();
    int32_t invalidFd1 = NUM_VALID;
    int32_t invalidFd2 = NUM_VALIDOONEHUNDRED;
    EXPECT_FALSE(fileSystem->IsSocketFile(invalidFd1));
    EXPECT_FALSE(fileSystem->IsSocketFile(invalidFd2));
}
}
}