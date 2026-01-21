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

#include "avformat_unit_test.h"

#include <fcntl.h>
#include "native_mfmagic.h"
#include "gtest/gtest.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

namespace {
constexpr int32_t INT_VALUE = 124;
constexpr int64_t LONG_VALUE = 12435;
constexpr double DOUBLE_VALUE = 666.625;
const std::string STRING_VALUE = "STRING_VALUE";

#define INT_TESTKEY Tag::APP_PID
#define LONG_TESTKEY Tag::MEDIA_DURATION
#define DOUBLE_TESTKEY Tag::VIDEO_CAPTURE_RATE
#define STRING_TESTKEY Tag::MEDIA_FILE_URI
} // namespace

void NativeAVFormatUnitTest::SetUpTestCase(void) {}

void NativeAVFormatUnitTest::TearDownTestCase(void) {}

void NativeAVFormatUnitTest::SetUp(void) {}

void NativeAVFormatUnitTest::TearDown(void) {}

/**
 * @tc.name: OH_AVFormat_GetKeyCount_0100
 * @tc.desc: Populate OH_AVFormat meta and verify GetKeyCount returns a non-zero number of keys.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKeyCount_0100, TestSize.Level2)
{
    OH_AVFormat* format = OH_AVFormat_Create();
    EXPECT_NE(nullptr, format);
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    meta->SetData(INT_TESTKEY, INT_VALUE);
    meta->SetData(LONG_TESTKEY, LONG_VALUE);
    meta->SetData(DOUBLE_TESTKEY, DOUBLE_VALUE);
    meta->SetData(STRING_TESTKEY, STRING_VALUE);

    format->format_.SetMeta(meta);
    uint32_t ret = OH_AVFormat_GetKeyCount(format);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: OH_AVFormat_GetKeyCount_0200
 * @tc.desc: Call GetKeyCount with a null format.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKeyCount_0200, TestSize.Level2)
{
    OH_AVFormat* format = nullptr;
    uint32_t ret = OH_AVFormat_GetKeyCount(format);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: OH_AVFormat_GetKey_0100
 * @tc.desc: Populate OH_AVFormat meta and get key by valid index.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKey_0100, TestSize.Level2)
{
    OH_AVFormat* format = OH_AVFormat_Create();
    EXPECT_NE(nullptr, format);
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    meta->SetData(INT_TESTKEY, INT_VALUE);
    meta->SetData(LONG_TESTKEY, LONG_VALUE);
    meta->SetData(DOUBLE_TESTKEY, DOUBLE_VALUE);
    meta->SetData(STRING_TESTKEY, STRING_VALUE);

    format->format_.SetMeta(meta);
    int index = 0;
    const char *key = "";
    bool ret = OH_AVFormat_GetKey(format, index, &key);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: OH_AVFormat_GetKey_0200
 * @tc.desc: Call GetKey with a null format.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKey_0200, TestSize.Level2)
{
    OH_AVFormat* format = nullptr;
    int index = 0;
    const char *key = "";
    bool ret = OH_AVFormat_GetKey(format, index, &key);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: OH_AVFormat_GetKey_0300
 * @tc.desc: Call GetKey with invalid indices (negative or out-of-range).
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKey_0300, TestSize.Level2)
{
    OH_AVFormat* format = OH_AVFormat_Create();
    EXPECT_NE(nullptr, format);
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    meta->SetData(INT_TESTKEY, INT_VALUE);
    meta->SetData(LONG_TESTKEY, LONG_VALUE);
    meta->SetData(DOUBLE_TESTKEY, DOUBLE_VALUE);
    meta->SetData(STRING_TESTKEY, STRING_VALUE);

    format->format_.SetMeta(meta);
    int index = -1;
    const char *key = "";
    bool ret = OH_AVFormat_GetKey(format, index, &key);
    EXPECT_EQ(ret, false);
    index = 100;
    ret = OH_AVFormat_GetKey(format, index, &key);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: OH_AVFormat_GetKey_0400
 * @tc.desc: Populate OH_AVFormat meta with valid outString and get key by valid index.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAVFormatUnitTest, OH_AVFormat_GetKey_0400, TestSize.Level2)
{
    OH_AVFormat* format = OH_AVFormat_Create();
    EXPECT_NE(nullptr, format);
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    meta->SetData(INT_TESTKEY, INT_VALUE);
    meta->SetData(LONG_TESTKEY, LONG_VALUE);
    meta->SetData(DOUBLE_TESTKEY, DOUBLE_VALUE);
    meta->SetData(STRING_TESTKEY, STRING_VALUE);

    format->format_.SetMeta(meta);
    char *outString = new char[10];
    format->outString_ = outString;
    int index = 0;
    const char *key = "";
    bool ret = OH_AVFormat_GetKey(format, index, &key);
    EXPECT_EQ(ret, true);
}
