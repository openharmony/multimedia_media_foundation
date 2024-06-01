/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <string>
#include "meta/meta.h"
#include "unittest_log.h"
#include <cstdlib>
#include <string>

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace MetaFuncUT {
class MetaInnerUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    std::shared_ptr<Meta> metaIn = nullptr;
    std::shared_ptr<Meta> metaOut = nullptr;
    std::shared_ptr<MessageParcel> parcel = nullptr;
};

void MetaInnerUnitTest::SetUpTestCase(void) {}

void MetaInnerUnitTest::TearDownTestCase(void) {}

void MetaInnerUnitTest::SetUp(void)
{
    std::cout << "[SetUp]: SetUp!!!, test: ";
    const ::testing::TestInfo *testInfo_ = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string testName = testInfo_->name();
    std::cout << testName << std::endl;
    parcel = std::make_shared<MessageParcel>();
    metaIn = std::make_shared<Meta>();
    metaOut = std::make_shared<Meta>();
}

void MetaInnerUnitTest::TearDown(void)
{
    metaIn->Clear();
    metaOut->Clear();
    parcel = nullptr;
    std::cout << "[TearDown]: over!!!" << std::endl;
}

/**
 * @tc.name: SetGet_PlainStringKey_Int
 * @tc.desc: SetGet_PlainStringKey_Int
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_PlainStringKey_Int, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    SetMetaData(*metaIn, "width", valueIn);
    GetMetaData(*metaIn, "width", valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Int32
 * @tc.desc: SetGet_Int32
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Int32, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    metaIn->Set<Tag::APP_PID>(valueIn);
    metaIn->Get<Tag::APP_PID>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_null
 * @tc.desc: SetGet_null
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_null, TestSize.Level1)
{
    bool valueOut = false;
    bool valueIn = true;
    bool ret = metaIn->Set<Tag::VIDEO_COLOR_RANGE>(nullptr);
    EXPECT_EQ(ret, false);
    ret &= metaIn->Set<Tag::MEDIA_TITLE>(nullptr);
    EXPECT_EQ(ret, false);
    ret &= metaIn->Set<Tag::USER_PUSH_DATA_TIME>(nullptr);
    EXPECT_EQ(ret, false);
    ret &= metaIn->Set<Tag::VIDEO_CROP_LEFT>(nullptr);
    EXPECT_EQ(ret, false);
    ret &= metaIn->Set<Tag::VIDEO_BIT_STREAM_FORMAT>(nullptr);
    EXPECT_EQ(ret, false);
    ret &= metaIn->Set<Tag::MEDIA_FILE_TYPE>(nullptr);
    metaIn->Get<Tag::VIDEO_COLOR_RANGE>(valueOut);
    EXPECT_NE(valueOut, valueIn);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SetGet_Bool
 * @tc.desc: SetGet_Bool
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Bool, TestSize.Level1)
{
    bool valueOut = false;
    bool valueIn = true;
    metaIn->Set<Tag::VIDEO_COLOR_RANGE>(valueIn);
    metaIn->Get<Tag::VIDEO_COLOR_RANGE>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Double
 * @tc.desc: SetGet_Double
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Double, TestSize.Level1)
{
    double valueOut = 0;
    double valueIn = 1.59261111;
    metaIn->Set<Tag::VIDEO_FRAME_RATE>(valueIn);
    metaIn->Get<Tag::VIDEO_FRAME_RATE>(valueOut);
    ASSERT_DOUBLE_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Long
 * @tc.desc: SetGet_Long
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Long, TestSize.Level1)
{
    int64_t valueOut = 0;
    int64_t valueIn = 17592186044673;
    metaIn->Set<Tag::MEDIA_DURATION>(valueIn);
    metaIn->Get<Tag::MEDIA_DURATION>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_String
 * @tc.desc: SetGet_String
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_String, TestSize.Level1)
{
    std::string valueOut = "";
    std::string valueIn = "STRING_TESTVALUE";
    metaIn->Set<Tag::MEDIA_FILE_URI>(valueIn);
    metaIn->Get<Tag::MEDIA_FILE_URI>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Data_Int32
 * @tc.desc: SetGet_Data_Int32
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Data_Int32, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    metaIn->SetData(Tag::APP_PID, valueIn);
    metaIn->GetData(Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Data_Int32_PlainInput
 * @tc.desc: SetGet_Data_Int32_PlainInput
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Data_Int32_PlainInput, TestSize.Level1)
{
    int32_t valueOut = 0;
    std::shared_ptr<Meta> meta_ = std::make_shared<Meta>();
    metaIn->SetData(Tag::APP_PID, 111);
    metaIn->GetData(Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, 111);
}

/**
 * @tc.name: SetGet_Data_String_PlainInput
 * @tc.desc: SetGet_Data_String_PlainInput
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Data_String_PlainInput, TestSize.Level1)
{
    std::string valueOut = "";
    metaIn->SetData(Tag::MEDIA_LYRICS, "Test Input");
    metaIn->GetData(Tag::MEDIA_LYRICS, valueOut);
    EXPECT_EQ(valueOut, "Test Input");
}

/**
 * @tc.name: SetGet_Data_Int32_Using_Parcel
 * @tc.desc: SetGet_Data_Int32_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Int32_Using_Parcel, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    metaIn->Set<Tag::APP_PID>(valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->Get<Tag::APP_PID>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_Data_Int32_Using_Parcel
 * @tc.desc: SetGet_Data_Int32_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Data_Int32_Using_Parcel, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    metaIn->SetData(Tag::APP_PID, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->GetData(Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_MetaData_Int32
 * @tc.desc: SetGet_MetaData_Int32
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Int32, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    SetMetaData(*metaIn, Tag::APP_PID, valueIn);
    GetMetaData(*metaIn, Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_MetaData_Int32_PlainInput
 * @tc.desc: SetGet_MetaData_Int32_PlainInput
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Int32_PlainInput, TestSize.Level1)
{
    int32_t valueOut = 0;
    SetMetaData(*metaIn, Tag::APP_PID, 141);
    GetMetaData(*metaIn, Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, 141);
}

/**
 * @tc.name: SetGet_MetaData_Enum_As_Int32
 * @tc.desc: SetGet_MetaData_Enum_As_Int32
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Enum_As_Int32, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = static_cast<int32_t>(Plugins::VideoRotation::VIDEO_ROTATION_90);
    SetMetaData(*metaIn, Tag::VIDEO_ROTATION, valueIn);
    GetMetaData(*metaIn, Tag::VIDEO_ROTATION, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_MetaData_Int32_Using_Parcel
 * @tc.desc: SetGet_MetaData_Int32_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Int32_Using_Parcel, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    SetMetaData(*metaIn, Tag::APP_PID, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->Get<Tag::APP_PID>(valueOut);
    EXPECT_EQ(valueOut, valueIn);
    valueOut = 0;
    GetMetaData(*metaOut, Tag::APP_PID, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_MetaData_Enum_As_Int32_Using_Parcel
 * @tc.desc: SetGet_MetaData_Enum_As_Int32_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Enum_As_Int32_Using_Parcel, TestSize.Level1)
{
    Plugins::VideoRotation valueOut;
    int32_t valueIn = static_cast<int32_t>(Plugins::VideoRotation::VIDEO_ROTATION_90);
    SetMetaData(*metaIn, Tag::VIDEO_ROTATION, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->Get<Tag::VIDEO_ROTATION>(valueOut);
    EXPECT_EQ(static_cast<int32_t>(valueOut), valueIn);
    int32_t valueIntOut = 0;
    GetMetaData(*metaOut, Tag::VIDEO_ROTATION, valueIntOut);
    EXPECT_EQ(valueIntOut, valueIn);
}

/**
 * @tc.name: SetGet_MetaData_Enum_As_Int64_Using_Parcel
 * @tc.desc: SetGet_MetaData_Enum_As_Int64_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_Enum_As_Int64_Using_Parcel, TestSize.Level1)
{
    Plugins::AudioChannelLayout valueOut;
    int64_t valueIn = static_cast<int64_t>(Plugins::AudioChannelLayout::HOA_ORDER1_FUMA);
    SetMetaData(*metaIn, Tag::AUDIO_CHANNEL_LAYOUT, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->Get<Tag::AUDIO_CHANNEL_LAYOUT>(valueOut);
    EXPECT_EQ(static_cast<int64_t>(valueOut), valueIn);
    int64_t valueInt64Out = 0;
    GetMetaData(*metaOut, Tag::AUDIO_CHANNEL_LAYOUT, valueInt64Out);
    EXPECT_EQ(valueOut, valueIn);
}

map<TagType, int32_t> testInt32Data = {
    // Int32 or Int32 Enum
    {Tag::APP_UID, 11},
    {Tag::APP_PID, 22},
    {Tag::APP_TOKEN_ID, 33},
    {Tag::REQUIRED_IN_BUFFER_CNT, 44},
    {Tag::REQUIRED_IN_BUFFER_SIZE, 11},
    {Tag::REQUIRED_OUT_BUFFER_CNT, 11},
    {Tag::REQUIRED_OUT_BUFFER_SIZE, 11},
    {Tag::BUFFERING_SIZE, 11},
    {Tag::WATERLINE_HIGH, 11},
    {Tag::WATERLINE_LOW, 11},
    {Tag::AUDIO_CHANNEL_COUNT, 11},
    {Tag::AUDIO_SAMPLE_RATE, 11},
    {Tag::AUDIO_SAMPLE_PER_FRAME, 22},
    {Tag::AUDIO_OUTPUT_CHANNELS, 33},
    {Tag::AUDIO_MPEG_VERSION, 11},
    {Tag::AUDIO_MPEG_LAYER, 123},
    {Tag::AUDIO_AAC_LEVEL, 12},
    {Tag::AUDIO_MAX_INPUT_SIZE, 21},
    {Tag::AUDIO_MAX_OUTPUT_SIZE, 32},
    {Tag::VIDEO_WIDTH, 12},
    {Tag::VIDEO_HEIGHT, 31},
    {Tag::VIDEO_DELAY, 54},
    {Tag::VIDEO_MAX_SURFACE_NUM, 45},
    {Tag::VIDEO_H264_LEVEL, 12},
    {Tag::AUDIO_AAC_IS_ADTS, 33},
    {Tag::AUDIO_COMPRESSION_LEVEL, 44},
    {Tag::AUDIO_BITS_PER_CODED_SAMPLE, 44},
    {Tag::MEDIA_TRACK_COUNT, 11},
    {Tag::REGULAR_TRACK_ID, 13},
    {Tag::VIDEO_SCALE_TYPE, 14},
    {Tag::VIDEO_I_FRAME_INTERVAL, 11},
    {Tag::MEDIA_PROFILE, 13},
    {Tag::VIDEO_ENCODE_QUALITY, 112},
    {Tag::AUDIO_AAC_SBR, 111},
    {Tag::AUDIO_OBJECT_NUMBER, 113},
    {Tag::AUDIO_FLAC_COMPLIANCE_LEVEL, 13},
    {Tag::MEDIA_LEVEL, 14},
    {Tag::VIDEO_STRIDE, 17},
    {Tag::VIDEO_DISPLAY_WIDTH, 19},
    {Tag::VIDEO_DISPLAY_HEIGHT, 23},
    {Tag::VIDEO_DISPLAY_WIDTH, 24},
    {Tag::VIDEO_DISPLAY_HEIGHT, 25},
    {Tag::VIDEO_ENCODER_TEMPORAL_GOP_SIZE, 11},
    {Tag::VIDEO_ENCODER_LTR_FRAME_COUNT, 12},
    {Tag::VIDEO_PER_FRAME_POC, 13},
    {Tag::VIDEO_ENCODER_PER_FRAME_USE_LTR, 14},
    {Tag::VIDEO_CROP_TOP, 15},
    {Tag::VIDEO_CROP_BOTTOM, 16},
    {Tag::VIDEO_CROP_LEFT, 17},
    {Tag::VIDEO_CROP_RIGHT, 18},
    {Tag::VIDEO_SLICE_HEIGHT, 19},
    {Tag::VIDEO_ENCODER_QP_MAX, 20},
    {Tag::VIDEO_ENCODER_QP_MIN, 21},
    {Tag::FEATURE_PROPERTY_VIDEO_ENCODER_MAX_LTR_FRAME_COUNT, 22},
    {Tag::VIDEO_DECODER_RATE_UPPER_LIMIT, 120},
    {Tag::SCREEN_CAPTURE_ERR_CODE, 3},
    {Tag::SCREEN_CAPTURE_DURATION, 5},
    {Tag::SCREEN_CAPTURE_START_LATENCY, 7},
    {Tag::DRM_ERROR_CODE, 28},
    {Tag::RECORDER_ERR_CODE, 6},
    {Tag::RECORDER_DURATION, 8},
    {Tag::RECORDER_VIDEO_BITRATE, 9},
    {Tag::RECORDER_AUDIO_SAMPLE_RATE, 4},
    {Tag::RECORDER_AUDIO_CHANNEL_COUNT, 3},
    {Tag::RECORDER_AUDIO_BITRATE, 6},
    {Tag::RECORDER_START_LATENCY, 7},
    {Tag::SRC_INPUT_TYPE, static_cast<int32_t>(Plugins::SrcInputType::AUD_ES)},
    {Tag::AUDIO_SAMPLE_FORMAT, static_cast<int32_t>(Plugins::AudioSampleFormat::SAMPLE_S16LE)},
    {Tag::VIDEO_PIXEL_FORMAT, static_cast<int32_t>(Plugins::VideoPixelFormat::YUV411P)},
    {Tag::MEDIA_TYPE, static_cast<int32_t>(Plugins::MediaType::AUDIO)},
    {Tag::VIDEO_H264_PROFILE, static_cast<int32_t>(Plugins::VideoH264Profile::BASELINE)},
    {Tag::VIDEO_ROTATION, static_cast<int32_t>(Plugins::VideoRotation::VIDEO_ROTATION_90)},
    {Tag::VIDEO_COLOR_PRIMARIES, static_cast<int32_t>(Plugins::ColorPrimary::BT2020)},
    {Tag::VIDEO_COLOR_TRC, static_cast<int32_t>(Plugins::TransferCharacteristic::BT1361)},
    {Tag::VIDEO_COLOR_MATRIX_COEFF, static_cast<int32_t>(Plugins::MatrixCoefficient::BT2020_CL)},
    {Tag::VIDEO_H265_PROFILE, static_cast<int32_t>(Plugins::HEVCProfile::HEVC_PROFILE_MAIN_10_HDR10)},
    {Tag::VIDEO_H265_LEVEL, static_cast<int32_t>(Plugins::HEVCLevel::HEVC_LEVEL_41)},
    {Tag::VIDEO_CHROMA_LOCATION, static_cast<int32_t>(Plugins::ChromaLocation::BOTTOM)},
    {Tag::MEDIA_FILE_TYPE, static_cast<int32_t>(Plugins::FileType::AMR)},
    {Tag::VIDEO_ENCODE_BITRATE_MODE, static_cast<int32_t>(Plugins::VideoEncodeBitrateMode::CBR)},
    {Tag::VIDEO_ENCODER_TEMPORAL_GOP_REFERENCE_MODE,
     static_cast<int32_t>(Plugins::TemporalGopReferenceMode::JUMP_REFERENCE)},
    // UINT8_T
    {Tag::AUDIO_AAC_PROFILE, static_cast<int32_t>(Plugins::AudioAacProfile::ELD)},
    {Tag::AUDIO_AAC_STREAM_FORMAT, static_cast<int32_t>(Plugins::AudioAacStreamFormat::ADIF)}};

    map<TagType, bool> testBoolData = {
    // Bool
    {Tag::SCREEN_CAPTURE_USER_AGREE, true},
    {Tag::SCREEN_CAPTURE_REQURE_MIC, false},
    {Tag::SCREEN_CAPTURE_ENABLE_MIC, true},
    {Tag::VIDEO_COLOR_RANGE, true},
    {Tag::VIDEO_REQUEST_I_FRAME, false},
    {Tag::MEDIA_HAS_VIDEO, true},
    {Tag::MEDIA_HAS_AUDIO, false},
    {Tag::MEDIA_END_OF_STREAM, true},
    {Tag::VIDEO_IS_HDR_VIVID, true},
    {Tag::VIDEO_FRAME_RATE_ADAPTIVE_MODE, true},
    {Tag::VIDEO_ENCODER_ENABLE_TEMPORAL_SCALABILITY, true},
    {Tag::VIDEO_ENCODER_PER_FRAME_MARK_LTR, true},
    {Tag::VIDEO_PER_FRAME_IS_LTR, true},
    {Tag::VIDEO_ENABLE_LOW_LATENCY, true},
    {Tag::VIDEO_ENCODER_ENABLE_SURFACE_INPUT_CALLBACK, true},
    {Tag::AUDIO_RENDER_SET_FLAG, true},
    {Tag::VIDEO_BUFFER_CAN_DROP, true}};


/**
 * @tc.name: SetGet_MetaData_All_As_Bool_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_Bool_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Bool_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testBoolData) {
        int32_t valueIn = item.second;
        SetMetaData(*metaIn, item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testBoolData) {
        int32_t valueIn = item.second;
        int32_t valueOut = 0;
        ASSERT_TRUE(GetMetaData(*metaOut, item.first, valueOut));
        std::cout <<  item.first << " , " << valueOut << " , " << valueIn << std::endl;
        EXPECT_EQ(valueOut, valueIn);
    }
}

/**
 * @tc.name: SetGet_MetaData_All_As_Int32_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_Int32_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Int32_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testInt32Data) {
        int32_t valueIn = item.second;
        SetMetaData(*metaIn, item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testInt32Data) {
        int32_t valueIn = item.second;
        int32_t valueOut = 0;
        ASSERT_TRUE(GetMetaData(*metaOut, item.first, valueOut));
        std::cout <<  item.first << " , " << valueOut << " , " << valueIn << std::endl;
        EXPECT_EQ(valueOut, valueIn);
    }
}

map<TagType, int64_t> testInt64Data = {
    // Int64 or Int64 Enum
    {Tag::APP_FULL_TOKEN_ID, 1234567890001},
    {Tag::MEDIA_DURATION, 1234567890002},
    {Tag::MEDIA_BITRATE, 1234567890003},
    {Tag::MEDIA_START_TIME, 1234567890004},
    {Tag::USER_FRAME_PTS, 1234567890005},
    {Tag::USER_PUSH_DATA_TIME, 1234567890006},
    {Tag::MEDIA_FILE_SIZE, 1234567890007},
    {Tag::MEDIA_POSITION, 1234567890008},
    {Tag::MEDIA_TIME_STAMP, 1234567890009},
    {Tag::AUDIO_CHANNEL_LAYOUT, static_cast<int64_t>(Plugins::AudioChannelLayout::CH_10POINT2)},
    {Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, static_cast<int64_t>(Plugins::AudioChannelLayout::HOA_ORDER3_FUMA)}
};

/**
 * @tc.name: SetGet_MetaData_All_As_Int64_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_Int64_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Int64_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testInt64Data) {
        int64_t valueIn = item.second;
        SetMetaData(*metaIn, item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testInt64Data) {
        int64_t valueIn = item.second;
        int64_t valueOut = 0;
        GetMetaData(*metaOut, item.first, valueOut);
        std::cout <<  item.first << " , " << valueOut << " , " << valueIn << std::endl;
        EXPECT_EQ(valueOut, valueIn);
    }
}

map<TagType, float> testFloatData = {
    // Float
    {Tag::MEDIA_LATITUDE, 1.01f},
    {Tag::MEDIA_LONGITUDE, 1.02f}
};

/**
 * @tc.name: SetGet_MetaData_All_As_Float_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_Float_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Float_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testFloatData) {
        float valueIn = item.second;
        metaIn->SetData(item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testFloatData) {
        float valueIn = item.second;
        float valueOut = 0.0f;
        metaOut->GetData(item.first, valueOut);
        ASSERT_FLOAT_EQ(valueOut, valueIn);
    }
}

map<TagType, double> testDoubleData = {
    // Double
    {Tag::VIDEO_FRAME_RATE, 1.01},
    {Tag::VIDEO_CAPTURE_RATE, 1.02}
};

/**
 * @tc.name: SetGet_MetaData_All_As_Double_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_Double_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Double_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testDoubleData) {
        double valueIn = item.second;
        metaIn->SetData(item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testDoubleData) {
        double valueIn = item.second;
        double valueOut = 0.0;
        metaOut->GetData(item.first, valueOut);
        ASSERT_DOUBLE_EQ(valueOut, valueIn);
    }
}

map<TagType, std::string> testStringData = {
    // String
    {Tag::MIME_TYPE, "String MIME_TYPE"},
    {Tag::MEDIA_FILE_URI, "String MEDIA_FILE_URI"},
    {Tag::MEDIA_TITLE, "String MEDIA_TITLE"},
    {Tag::MEDIA_ARTIST, "String MEDIA_ARTIST"},
    {Tag::MEDIA_LYRICIST, "String MEDIA_LYRICIST"},
    {Tag::MEDIA_ALBUM, "String MEDIA_ALBUM"},
    {Tag::MEDIA_ALBUM_ARTIST, "String MEDIA_ALBUM_ARTIST"},
    {Tag::MEDIA_DATE, "String MEDIA_DATE"},
    {Tag::MEDIA_COMMENT, "String MEDIA_COMMENT"},
    {Tag::MEDIA_GENRE, "String MEDIA_GENRE"},
    {Tag::MEDIA_COPYRIGHT, "String MEDIA_COPYRIGHT"},
    {Tag::MEDIA_LANGUAGE, "String MEDIA_LANGUAGE"},
    {Tag::MEDIA_DESCRIPTION, "String MEDIA_DESCRIPTION"},
    {Tag::USER_TIME_SYNC_RESULT, "String USER_TIME_SYNC_RESULT"},
    {Tag::USER_AV_SYNC_GROUP_INFO, "String USER_AV_SYNC_GROUP_INFO"},
    {Tag::USER_SHARED_MEMORY_FD, "String USER_SHARED_MEMORY_FD"},
    {Tag::MEDIA_AUTHOR, "String MEDIA_AUTHOR"},
    {Tag::MEDIA_COMPOSER, "String MEDIA_COMPOSER"},
    {Tag::MEDIA_LYRICS, "String MEDIA_LYRICS"},
    {Tag::MEDIA_CODEC_NAME, "String MEDIA_CODEC_NAME"},
    {Tag::PROCESS_NAME, "String PROCESS_NAME"},
    {Tag::MEDIA_CREATION_TIME, "String MEDIA_CREATION_TIME"},
    {Tag::SCREEN_CAPTURE_ERR_MSG, "String SCREEN_CAPTURE_ERR_MSG"},
    {Tag::SCREEN_CAPTURE_VIDEO_RESOLUTION, "String SCREEN_CAPTURE_VIDEO_RESOLUTION"},
    {Tag::DRM_APP_NAME, "String DRM_APP_NAME"},
    {Tag::DRM_INSTANCE_ID, "String DRM_INSTANCE_ID"},
    {Tag::DRM_ERROR_MESG, "String DRM_ERROR_MESG"},
    {Tag::RECORDER_ERR_MSG, "String RECORDER_ERR_MSG"},
    {Tag::RECORDER_CONTAINER_MIME, "String RECORDER_CONTAINER_MIME"},
    {Tag::RECORDER_VIDEO_MIME, "String RECORDER_VIDEO_MIME"},
    {Tag::RECORDER_VIDEO_RESOLUTION, "String RECORDER_VIDEO_RESOLUTION"},
    {Tag::RECORDER_AUDIO_MIME, "String RECORDER_AUDIO_MIME"},
};

/**
 * @tc.name: SetGet_MetaData_All_As_String_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_String_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_String_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testStringData) {
        std::string valueIn = item.second;
        metaIn->SetData(item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testStringData) {
        std::string valueIn = item.second;
        std::string valueOut = "String Value";
        metaOut->GetData(item.first, valueOut);
        std::cout <<  item.first << " , " << valueOut << " , " << valueIn << std::endl;
        EXPECT_EQ(valueOut, valueIn);
    }
}

std::vector<uint8_t> vectorUint8MediaCodec{1, 2, 3};
std::vector<uint8_t> vectorUint8MediaCover{1, 2, 3, 4};
map<TagType, std::vector<uint8_t>> testVetcorInt8Data = {
    // vector<uint8_t>
    {Tag::MEDIA_CODEC_CONFIG, vectorUint8MediaCodec},
    {Tag::MEDIA_COVER, vectorUint8MediaCover},
    {Tag::AUDIO_VORBIS_IDENTIFICATION_HEADER, vectorUint8MediaCover},
    {Tag::AUDIO_VORBIS_SETUP_HEADER, vectorUint8MediaCover},
    {Tag::AUDIO_VIVID_METADATA, vectorUint8MediaCover},
};

/**
 * @tc.name: SetGet_MetaData_All_As_VectorUint8_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_VectorUint8_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_VectorUint8_Using_ParcelPackage, TestSize.Level1)
{
    for (auto item : testVetcorInt8Data) {
        std::vector<uint8_t> valueIn = item.second;
        metaIn->SetData(item.first, valueIn);
    }
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    for (auto item : testVetcorInt8Data) {
        std::vector<uint8_t> valueIn = item.second;
        std::vector<uint8_t> valueOut;
        metaOut->GetData(item.first, valueOut);
        EXPECT_EQ(valueOut, valueIn);
    }
}

void PrepareInMeta(std::shared_ptr<Meta>& inMeta)
{
    for (auto item : testVetcorInt8Data) {
        std::vector<uint8_t> valueInVecInt8 = item.second;
        inMeta->SetData(item.first, valueInVecInt8);
    }
    for (auto item : testFloatData) {
        float valueInFloat = item.second;
        inMeta->SetData(item.first, valueInFloat);
    }
    for (auto item : testDoubleData) {
        double valueInDouble = item.second;
        inMeta->SetData(item.first, valueInDouble);
    }
    for (auto item : testStringData) {
        std::string valueInStr = item.second;
        inMeta->SetData(item.first, valueInStr);
    }
    for (auto item : testInt32Data) {
        int32_t valueInInt32 = item.second;
        SetMetaData(*inMeta, item.first, valueInInt32);
    }
}

void CheckOutMeta(const std::shared_ptr<Meta>& outMeta)
{
    for (auto item : testInt32Data) {
        int32_t valueInInt32 = item.second;
        int32_t valueOutInt32 = 0;
        GetMetaData(*outMeta, item.first, valueOutInt32);
        EXPECT_EQ(valueOutInt32, valueInInt32);
    }
    for (auto item : testStringData) {
        std::string valueInStr = item.second;
        std::string valueOutStr = "String Value";
        outMeta->GetData(item.first, valueOutStr);
        EXPECT_EQ(valueOutStr, valueInStr);
    }
    for (auto item : testFloatData) {
        float valueInFloat = item.second;
        float valueOutFloat = 0.0f;
        outMeta->GetData(item.first, valueOutFloat);
        ASSERT_FLOAT_EQ(valueOutFloat, valueInFloat);
    }
    for (auto item : testDoubleData) {
        double valueInDouble = item.second;
        double valueOutDouble = 0.0;
        outMeta->GetData(item.first, valueOutDouble);
        ASSERT_DOUBLE_EQ(valueOutDouble, valueInDouble);
    }
    for (auto item : testVetcorInt8Data) {
        std::vector<uint8_t> valueInVecInt8 = item.second;
        std::vector<uint8_t> valueOutVecInt8;
        outMeta->GetData(item.first, valueOutVecInt8);
        EXPECT_EQ(valueOutVecInt8, valueInVecInt8);
    }
}

/**
 * @tc.name: SetGet_MetaData_All_As_VectorUint8_Using_ParcelPackage
 * @tc.desc: SetGet_MetaData_All_As_VectorUint8_Using_ParcelPackage
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Mix_Using_ParcelPackage, TestSize.Level1)
{
    PrepareInMeta(metaIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    CheckOutMeta(metaOut);
}

/**
 * @tc.name: SetGet_MetaData_All_As_Mix_Using_AssignCopy
 * @tc.desc: SetGet_MetaData_All_As_Mix_Using_AssignCopy
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Mix_Using_AssignCopy, TestSize.Level1)
{
    PrepareInMeta(metaIn);
    Meta metaCopy = std::move(*metaIn);
    for (auto item : testInt32Data) {
        int32_t valueOutInt32 = 0;
        ASSERT_FALSE(GetMetaData(*metaIn, item.first, valueOutInt32));
    }
    CheckOutMeta(make_shared<Meta>(metaCopy));
}

/**
 * @tc.name: SetGet_MetaData_All_As_Mix_Using_SwapCopy
 * @tc.desc: SetGet_MetaData_All_As_Mix_Using_SwapCopy
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_MetaData_All_As_Mix_Using_SwapCopy, TestSize.Level1)
{
    PrepareInMeta(metaIn);
    Meta metaCopy(std::move(*metaIn));
    for (auto item : testInt32Data) {
        int32_t valueOutInt32 = 0;
        ASSERT_FALSE(GetMetaData(*metaIn, item.first, valueOutInt32));
    }
    CheckOutMeta(make_shared<Meta>(metaCopy));
}

/**
 * @tc.name: SetGet_Data_DrmStruct_Using_Parcel
 * @tc.desc: SetGet_Data_DrmStruct_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_Data_DrmStruct_Using_Parcel, TestSize.Level1)
{
    uint8_t iv[META_DRM_IV_SIZE] = {
        0x3d, 0x5e, 0x6d, 0x35, 0x9b, 0x9a, 0x41, 0xe8, 0xb8, 0x43, 0xdd, 0x3c, 0x6e, 0x72, 0xc4, 0x2c
    };
    uint8_t keyId[META_DRM_KEY_ID_SIZE] = {
        0x3a, 0x5e, 0x6d, 0x35, 0x9b, 0x9a, 0x41, 0xe8, 0xb8, 0x43, 0xdd, 0x3c, 0x6e, 0x72, 0xc4, 0x2a
    };
    Plugins::MetaDrmCencInfo *drmCencInfoIn = (Plugins::MetaDrmCencInfo *)malloc(sizeof(Plugins::MetaDrmCencInfo));
    drmCencInfoIn->algo = Plugins::MetaDrmCencAlgorithm::META_DRM_ALG_CENC_SM4_CBC;
    drmCencInfoIn->keyIdLen = META_DRM_KEY_ID_SIZE;
    auto error = memcpy_s(drmCencInfoIn->keyId, sizeof(drmCencInfoIn->keyId), keyId, META_DRM_KEY_ID_SIZE);
    ASSERT_EQ(error, EOK);
    drmCencInfoIn->ivLen = META_DRM_IV_SIZE;
    error = memcpy_s(drmCencInfoIn->iv, sizeof(drmCencInfoIn->iv), iv, META_DRM_IV_SIZE);
    ASSERT_EQ(error, EOK);
    drmCencInfoIn->encryptBlocks = 0;
    drmCencInfoIn->skipBlocks = 0;
    drmCencInfoIn->firstEncryptOffset = 0;
    drmCencInfoIn->subSampleNum = 1;
    for (uint32_t i = 0; i < drmCencInfoIn->subSampleNum; i++) {
        drmCencInfoIn->subSamples[i].clearHeaderLen = 0x10;
        drmCencInfoIn->subSamples[i].payLoadLen = 0;
    }
    std::vector<uint8_t> drmCencVecIn((uint8_t *)drmCencInfoIn,
        ((uint8_t *)drmCencInfoIn) + sizeof(Plugins::MetaDrmCencInfo));
    std::shared_ptr<Meta> metaIn = nullptr;
    std::shared_ptr<Meta> metaOut = nullptr;
    std::shared_ptr<MessageParcel> parcel = nullptr;
    parcel = std::make_shared<MessageParcel>();
    metaIn = std::make_shared<Meta>();
    metaOut = std::make_shared<Meta>();
    metaIn->SetData(Tag::DRM_CENC_INFO, drmCencVecIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));

    std::vector<uint8_t> drmCencVecOut;
    metaOut->GetData(Tag::DRM_CENC_INFO, drmCencVecOut);
    Plugins::MetaDrmCencInfo *drmCencInfoOut = reinterpret_cast<Plugins::MetaDrmCencInfo *>(&drmCencVecOut[0]);

    EXPECT_EQ(drmCencInfoIn->keyIdLen, drmCencInfoOut->keyIdLen);
    free(drmCencInfoIn);
}

/**
 * @tc.name: SetGet_self_Data_String_Using_Parcel
 * @tc.desc: SetGet_self_Data_String_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_self_Data_String_Using_Parcel, TestSize.Level1)
{
    std::string valueOut = "123";
    std::string valueIn = "123abc";
    std::string key = "myself";
    metaIn->SetData(key, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: SetGet_self_Data_float_Using_Parcel
 * @tc.desc: SetGet_self_Data_float_Using_Parcel
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, SetGet_self_Data_float_Using_Parcel, TestSize.Level1)
{
    float valueOut = 0.0f;
    float valueIn = 1.01f;
    std::string key = "myself";
    metaIn->SetData(key, valueIn);
    ASSERT_TRUE(metaIn->ToParcel(*parcel));
    ASSERT_TRUE(metaOut->FromParcel(*parcel));
    metaOut->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_Boolean
 * @tc.desc: GetValueType_Data_Boolean
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_Boolean, TestSize.Level1)
{
    bool valueOut = true;
    bool valueIn = false;
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::BOOL);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_Int32
 * @tc.desc: GetValueType_Data_Int32
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_Int32, TestSize.Level1)
{
    int32_t valueOut = 0;
    int32_t valueIn = 141;
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::INT32_T);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_Int64
 * @tc.desc: GetValueType_Data_Int64
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_Int64, TestSize.Level1)
{
    int64_t valueOut = 0;
    int64_t valueIn = 141;
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::INT64_T);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_FLOAT
 * @tc.desc: GetValueType_Data_FLOAT
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_FLOAT, TestSize.Level1)
{
    float valueOut = 0;
    float valueIn = 1.01;
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::FLOAT);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_DOUBLE
 * @tc.desc: GetValueType_Data_DOUBLE
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_DOUBLE, TestSize.Level1)
{
    double valueOut = 0;
    double valueIn = 1.01;
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::DOUBLE);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}

/**
 * @tc.name: GetValueType_Data_VECTOR_UINT8
 * @tc.desc: GetValueType_Data_VECTOR_UINT8
 * @tc.type: FUNC
 */
HWTEST_F(MetaInnerUnitTest, GetValueType_Data_VECTOR_UINT8, TestSize.Level1)
{
    vector<uint8_t> valueOut{1, 2, 3};
    vector<uint8_t> valueIn{1, 2, 3, 4};
    std::string key = "test";
    metaIn->SetData(key, valueIn);
    AnyValueType type = metaIn->GetValueType(key);
    ASSERT_TRUE(type == AnyValueType::VECTOR_UINT8);
    metaIn->GetData(key, valueOut);
    EXPECT_EQ(valueOut, valueIn);
}
} // namespace MetaFuncUT
} // namespace Media
} // namespace OHOS
