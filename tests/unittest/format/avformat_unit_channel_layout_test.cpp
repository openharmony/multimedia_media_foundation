/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include "avformat_mock.h"
#include "meta/format.h"
#include "meta/meta.h"
#include "common/status.h"
#include "native_audio_channel_layout.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;
namespace OHOS {
namespace Media {
namespace Plugins {
std::vector<OH_AudioChannelLayout> nativeChanneLayouts = {CH_LAYOUT_UNKNOWN,
                                                          CH_LAYOUT_MONO,
                                                          CH_LAYOUT_STEREO,
                                                          CH_LAYOUT_STEREO_DOWNMIX,
                                                          CH_LAYOUT_2POINT1,
                                                          CH_LAYOUT_3POINT0,
                                                          CH_LAYOUT_SURROUND,
                                                          CH_LAYOUT_3POINT1,
                                                          CH_LAYOUT_4POINT0,
                                                          CH_LAYOUT_QUAD_SIDE,
                                                          CH_LAYOUT_QUAD,
                                                          CH_LAYOUT_2POINT0POINT2,
                                                          CH_LAYOUT_AMB_ORDER1_ACN_N3D,
                                                          CH_LAYOUT_AMB_ORDER1_ACN_SN3D,
                                                          CH_LAYOUT_AMB_ORDER1_FUMA,
                                                          CH_LAYOUT_4POINT1,
                                                          CH_LAYOUT_5POINT0,
                                                          CH_LAYOUT_5POINT0_BACK,
                                                          CH_LAYOUT_2POINT1POINT2,
                                                          CH_LAYOUT_3POINT0POINT2,
                                                          CH_LAYOUT_5POINT1,
                                                          CH_LAYOUT_5POINT1_BACK,
                                                          CH_LAYOUT_6POINT0,
                                                          CH_LAYOUT_3POINT1POINT2,
                                                          CH_LAYOUT_6POINT0_FRONT,
                                                          CH_LAYOUT_HEXAGONAL,
                                                          CH_LAYOUT_6POINT1,
                                                          CH_LAYOUT_6POINT1_BACK,
                                                          CH_LAYOUT_6POINT1_FRONT,
                                                          CH_LAYOUT_7POINT0,
                                                          CH_LAYOUT_7POINT0_FRONT,
                                                          CH_LAYOUT_7POINT1,
                                                          CH_LAYOUT_OCTAGONAL,
                                                          CH_LAYOUT_5POINT1POINT2,
                                                          CH_LAYOUT_7POINT1_WIDE,
                                                          CH_LAYOUT_7POINT1_WIDE_BACK,
                                                          CH_LAYOUT_AMB_ORDER2_ACN_N3D,
                                                          CH_LAYOUT_AMB_ORDER2_ACN_SN3D,
                                                          CH_LAYOUT_AMB_ORDER2_FUMA,
                                                          CH_LAYOUT_5POINT1POINT4,
                                                          CH_LAYOUT_7POINT1POINT2,
                                                          CH_LAYOUT_7POINT1POINT4,
                                                          CH_LAYOUT_10POINT2,
                                                          CH_LAYOUT_9POINT1POINT4,
                                                          CH_LAYOUT_9POINT1POINT6,
                                                          CH_LAYOUT_HEXADECAGONAL,
                                                          CH_LAYOUT_AMB_ORDER3_ACN_N3D,
                                                          CH_LAYOUT_AMB_ORDER3_ACN_SN3D,
                                                          CH_LAYOUT_AMB_ORDER3_FUMA,
                                                          CH_LAYOUT_22POINT2};

std::vector<AudioChannelLayout> innerChanneLayouts = {
    UNKNOWN,
    MONO,
    STEREO,
    STEREO_DOWNMIX,
    CH_2POINT1,
    CH_2_1, /**CH_LAYOUT_3POINT0 in native_audio_channel_layout*/
    SURROUND,
    CH_3POINT1,
    CH_4POINT0,
    CH_2_2, /**CH_LAYOUT_QUAD_SIDE in native_audio_channel_layout*/
    QUAD,
    CH_2POINT0POINT2,
    HOA_ORDER1_ACN_N3D,
    HOA_ORDER1_ACN_SN3D,
    HOA_ORDER1_FUMA,
    CH_4POINT1,
    CH_5POINT0,
    CH_5POINT0_BACK,
    CH_2POINT1POINT2,
    CH_3POINT0POINT2,
    CH_5POINT1,
    CH_5POINT1_BACK,
    CH_6POINT0,
    CH_3POINT1POINT2,
    CH_6POINT0_FRONT,
    HEXAGONAL,
    CH_6POINT1,
    CH_6POINT1_BACK,
    CH_6POINT1_FRONT,
    CH_7POINT0,
    CH_7POINT0_FRONT,
    CH_7POINT1,
    OCTAGONAL,
    CH_5POINT1POINT2,
    CH_7POINT1_WIDE,
    CH_7POINT1_WIDE_BACK,
    HOA_ORDER2_ACN_N3D,
    HOA_ORDER2_ACN_SN3D,
    HOA_ORDER2_FUMA,
    CH_5POINT1POINT4,
    CH_7POINT1POINT2,
    CH_7POINT1POINT4,
    CH_10POINT2,
    CH_9POINT1POINT4,
    CH_9POINT1POINT6,
    HEXADECAGONAL,
    HOA_ORDER3_ACN_N3D,
    HOA_ORDER3_ACN_SN3D,
    HOA_ORDER3_FUMA,
    CH_22POINT2,
};

class AVFormatChannelayoutUnitTest : public testing::Test {
public:
    // SetUpTestCase: Called before all test cases
    static void SetUpTestCase(void);
    // TearDownTestCase: Called after all test case
    static void TearDownTestCase(void);
    // SetUp: Called before each test cases
    void SetUp(void);
    // TearDown: Called after each test cases
    void TearDown(void);

protected:
    std::shared_ptr<FormatMock> format_ = nullptr;
};

void AVFormatChannelayoutUnitTest::SetUpTestCase(void) {}

void AVFormatChannelayoutUnitTest::TearDownTestCase(void) {}

void AVFormatChannelayoutUnitTest::SetUp(void)
{
    format_ = FormatMockFactory::CreateFormat();
    ASSERT_NE(nullptr, format_);
}

void AVFormatChannelayoutUnitTest::TearDown(void)
{
    if (format_ != nullptr) {
        format_->Destroy();
    }
}

/**
 * @tc.name: channel_layout_001
 * @tc.desc: native channel layout match inner channel layout
 * @tc.type: FUNC
 * @tc.require: issueI5OX06 issueI5P8N0
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_001, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    int32_t innerSize = innerChanneLayouts.size();
    EXPECT_EQ(size, innerSize);
    for (int32_t i = 0; i < size; ++i) {
        EXPECT_EQ(static_cast<int64_t>(nativeChanneLayouts[i]), static_cast<int64_t>(innerChanneLayouts[i]));
    }
}

/**
 * @tc.name: channel_layout_002
 * @tc.desc: format put and get value
 * @tc.type: FUNC
 * @tc.require: issueI5OX06 issueI5P8N0
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_002, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    int64_t getLongValue = 0;
    for (int32_t i = 0; i < size; ++i) {
        EXPECT_TRUE(format_->PutLongValue(Tag::AUDIO_CHANNEL_LAYOUT, nativeChanneLayouts[i]));
        EXPECT_TRUE(format_->GetLongValue(Tag::AUDIO_CHANNEL_LAYOUT, getLongValue));
        EXPECT_EQ(getLongValue, static_cast<int64_t>(innerChanneLayouts[i]));
    }
}

/**
 * @tc.name: channel_layout_003
 * @tc.desc:
 *     1. set format;
 *     2. dmpinfo;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_003, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        std::shared_ptr<Format> format = std::make_shared<Format>();
        EXPECT_TRUE(format_->PutLongValue(Tag::AUDIO_CHANNEL_LAYOUT, nativeChanneLayouts[i]));
        std::string dumpInfo = format_->DumpInfo();
        std::cout << "dumpInfo: [" << dumpInfo << "]\n";
        std::stringstream dumpStream;
        dumpStream.str("");
        dumpStream << Tag::AUDIO_CHANNEL_LAYOUT << " = " << nativeChanneLayouts[i];
        EXPECT_NE(dumpInfo.find(dumpStream.str()), string::npos) << "dumpStream: [" << dumpStream.str() << "]\n"
                                                                 << "dumpInfo: [" << dumpInfo << "]\n";
    }
}

#ifndef AVFORMAT_CAPI_UNIT_TEST
/**
 * @tc.name: channel_layout_004
 * @tc.desc:
 *     1. set format;
 *     2. meta trans by parcel;
 *     3. dmpinfo;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_004, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        std::shared_ptr<Meta> meta = std::make_shared<Meta>();
        std::shared_ptr<Format> format = std::make_shared<Format>();
        meta->SetData(Tag::AUDIO_CHANNEL_LAYOUT, innerChanneLayouts[i]);
        format->SetMeta(meta);
        std::string dumpInfo = format->Stringify();
        std::cout << "dumpInfo: [" << dumpInfo << "]\n";
        std::stringstream dumpStream;
        auto checkFunc = [&dumpStream, &dumpInfo, i]() {
            dumpStream.str("");
            dumpStream << Tag::AUDIO_CHANNEL_LAYOUT << " = " << static_cast<int64_t>(nativeChanneLayouts[i]);
            EXPECT_NE(dumpInfo.find(dumpStream.str()), string::npos) << "dumpStream: [" << dumpStream.str() << "]\n";
        };
        std::cout << "before trans by parcel:\n";
        checkFunc();
        MessageParcel parcel;
        meta = format->GetMeta();
        ASSERT_TRUE(meta->ToParcel(parcel));
        ASSERT_TRUE(meta->FromParcel(parcel));

        format = std::make_shared<Format>();
        format->SetMeta(std::move(meta));
        dumpInfo = format->Stringify();
        std::cout << "after trans by parcel:\n";
        checkFunc();
    }
}

/**
 * @tc.name: channel_layout_005
 * @tc.desc:
 *     1. set values to meta;
 *     2. get formatMap;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_005, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        MessageParcel parcel;
        std::shared_ptr<Meta> meta = std::make_shared<Meta>();
        std::shared_ptr<Format> format = std::make_shared<Format>();
        meta->SetData(Tag::AUDIO_CHANNEL_LAYOUT, innerChanneLayouts[i]);
        format->SetMeta(meta);
        Format::FormatDataMap formatMap = format->GetFormatMap();
        auto iter = formatMap.find(Tag::AUDIO_CHANNEL_LAYOUT);
        ASSERT_NE(iter, formatMap.end());
        EXPECT_EQ(iter->second.type, FORMAT_TYPE_INT64);
        EXPECT_EQ(iter->second.val.int64Val, static_cast<int64_t>(innerChanneLayouts[i]));
    }
}

/**
 * @tc.name: channel_layout_006
 * @tc.desc:
 *     1. set values to meta;
 *     2. meta trans by parcel;
 *     3. get formatMap;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_006, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        MessageParcel parcel;
        std::shared_ptr<Meta> meta = std::make_shared<Meta>();
        std::shared_ptr<Format> format = std::make_shared<Format>();
        meta->SetData(Tag::AUDIO_CHANNEL_LAYOUT, innerChanneLayouts[i]);
        format->SetMeta(meta);
        meta = format->GetMeta();
        ASSERT_TRUE(meta->ToParcel(parcel));
        ASSERT_TRUE(meta->FromParcel(parcel));

        format = std::make_shared<Format>();
        format->SetMeta(std::move(meta));
        Format::FormatDataMap formatMap = format->GetFormatMap();
        auto iter = formatMap.find(Tag::AUDIO_CHANNEL_LAYOUT);
        ASSERT_NE(iter, formatMap.end());
        EXPECT_EQ(iter->second.type, FORMAT_TYPE_INT64);
        EXPECT_EQ(iter->second.val.int64Val, static_cast<int64_t>(innerChanneLayouts[i]));
    }
}

/**
 * @tc.name: channel_layout_007
 * @tc.desc:
 *     1. set values to meta;
 *     2. get format value type;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_007, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        MessageParcel parcel;
        std::shared_ptr<Meta> meta = std::make_shared<Meta>();
        std::shared_ptr<Format> format = std::make_shared<Format>();
        meta->SetData(Tag::AUDIO_CHANNEL_LAYOUT, innerChanneLayouts[i]);
        format->SetMeta(std::move(meta));
        EXPECT_EQ(format->GetValueType(Tag::AUDIO_CHANNEL_LAYOUT), FORMAT_TYPE_INT64);
    }
}

/**
 * @tc.name: channel_layout_008
 * @tc.desc:
 *     1. set values to meta;
 *     2. meta trans by parcel;
 *     3. get format value type;
 * @tc.type: FUNC
 * @tc.require: issueI5OWXY issueI5OXCD
 */
HWTEST_F(AVFormatChannelayoutUnitTest, channel_layout_008, TestSize.Level1)
{
    int32_t size = nativeChanneLayouts.size();
    for (int32_t i = 0; i < size; ++i) {
        MessageParcel parcel;
        std::shared_ptr<Meta> meta = std::make_shared<Meta>();
        std::shared_ptr<Format> format = std::make_shared<Format>();
        meta->SetData(Tag::AUDIO_CHANNEL_LAYOUT, innerChanneLayouts[i]);
        format->SetMeta(meta);
        meta = format->GetMeta();
        ASSERT_TRUE(meta->ToParcel(parcel));
        ASSERT_TRUE(meta->FromParcel(parcel));

        format = std::make_shared<Format>();
        format->SetMeta(std::move(meta));
        EXPECT_EQ(format->GetValueType(Tag::AUDIO_CHANNEL_LAYOUT), FORMAT_TYPE_INT64);
    }
}

#endif
} // namespace Plugins
} // namespace Media
} // namespace OHOS
