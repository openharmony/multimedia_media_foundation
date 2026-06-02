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

#include <gtest/gtest.h>
#include <cstring>
#include <cmath>
#include <vector>
#include "common/pcm_convert.h"
#include "meta/audio_types.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::Media::Plugins;

namespace OHOS {
namespace Media {

class PcmConvertUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(void) {}
    void TearDown(void) {}
};

/**
 * @tc.name: GetPcmBytesPerSample_U8
 * @tc.desc: Test GetPcmBytesPerSample returns 1 for SAMPLE_U8
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_U8, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_U8), 1);
}

/**
 * @tc.name: GetPcmBytesPerSample_S16LE
 * @tc.desc: Test GetPcmBytesPerSample returns 2 for SAMPLE_S16LE
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_S16LE, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_S16LE), 2);
}

/**
 * @tc.name: GetPcmBytesPerSample_S24LE
 * @tc.desc: Test GetPcmBytesPerSample returns 3 for SAMPLE_S24LE
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_S24LE, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_S24LE), 3);
}

/**
 * @tc.name: GetPcmBytesPerSample_S32LE
 * @tc.desc: Test GetPcmBytesPerSample returns 4 for SAMPLE_S32LE
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_S32LE, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_S32LE), 4);
}

/**
 * @tc.name: GetPcmBytesPerSample_F32LE
 * @tc.desc: Test GetPcmBytesPerSample returns 4 for SAMPLE_F32LE
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_F32LE, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_F32LE), 4);
}

/**
 * @tc.name: GetPcmBytesPerSample_InvalidFormat
 * @tc.desc: Test GetPcmBytesPerSample returns 0 for unsupported formats
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmBytesPerSample_InvalidFormat, TestSize.Level1)
{
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_U8P), 0);
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_S16P), 0);
    EXPECT_EQ(GetPcmBytesPerSample(SAMPLE_S16BE), 0);
    EXPECT_EQ(GetPcmBytesPerSample(INVALID_WIDTH), 0);
}

/**
 * @tc.name: GetPcmConvertOutputSize_Valid
 * @tc.desc: Test GetPcmConvertOutputSize returns correct size
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmConvertOutputSize_Valid, TestSize.Level1)
{
    size_t sampleCount = 1024;
    EXPECT_EQ(GetPcmConvertOutputSize(sampleCount, SAMPLE_U8), 1024u);
    EXPECT_EQ(GetPcmConvertOutputSize(sampleCount, SAMPLE_S16LE), 2048u);
    EXPECT_EQ(GetPcmConvertOutputSize(sampleCount, SAMPLE_S24LE), 3072u);
    EXPECT_EQ(GetPcmConvertOutputSize(sampleCount, SAMPLE_S32LE), 4096u);
    EXPECT_EQ(GetPcmConvertOutputSize(sampleCount, SAMPLE_F32LE), 4096u);
}

/**
 * @tc.name: GetPcmConvertOutputSize_Invalid
 * @tc.desc: Test GetPcmConvertOutputSize returns 0 for invalid format
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, GetPcmConvertOutputSize_Invalid, TestSize.Level1)
{
    EXPECT_EQ(GetPcmConvertOutputSize(1024, SAMPLE_U8P), 0u);
    EXPECT_EQ(GetPcmConvertOutputSize(1024, INVALID_WIDTH), 0u);
}

/**
 * @tc.name: ConvertPcm_NullInput
 * @tc.desc: Test ConvertPcmSampleFormat with null input pointer
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_NullInput, TestSize.Level1)
{
    uint8_t output[100] = {0};
    EXPECT_EQ(ConvertPcmSampleFormat(nullptr, 10, SAMPLE_U8, SAMPLE_S16LE, output),
        Status::ERROR_NULL_POINTER);
}

/**
 * @tc.name: ConvertPcm_NullOutput
 * @tc.desc: Test ConvertPcmSampleFormat with null output pointer
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_NullOutput, TestSize.Level1)
{
    uint8_t input[100] = {0};
    EXPECT_EQ(ConvertPcmSampleFormat(input, 10, SAMPLE_U8, SAMPLE_S16LE, nullptr),
        Status::ERROR_NULL_POINTER);
}

/**
 * @tc.name: ConvertPcm_ZeroSamples
 * @tc.desc: Test ConvertPcmSampleFormat with zero sample count returns OK
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_ZeroSamples, TestSize.Level1)
{
    uint8_t input[100] = {0};
    uint8_t output[100] = {0};
    EXPECT_EQ(ConvertPcmSampleFormat(input, 0, SAMPLE_U8, SAMPLE_S16LE, output),
        Status::OK);
}

/**
 * @tc.name: ConvertPcm_UnsupportedFormat
 * @tc.desc: Test ConvertPcmSampleFormat with unsupported format
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_UnsupportedFormat, TestSize.Level1)
{
    uint8_t input[100] = {0};
    uint8_t output[100] = {0};
    EXPECT_EQ(ConvertPcmSampleFormat(input, 10, SAMPLE_U8P, SAMPLE_S16LE, output),
        Status::ERROR_UNSUPPORTED_FORMAT);
    EXPECT_EQ(ConvertPcmSampleFormat(input, 10, SAMPLE_U8, SAMPLE_S16P, output),
        Status::ERROR_UNSUPPORTED_FORMAT);
}

/**
 * @tc.name: ConvertPcm_SameFormat
 * @tc.desc: Test ConvertPcmSampleFormat with same input and output format
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_SameFormat, TestSize.Level1)
{
    uint8_t input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t output[6] = {0};
    Status status = ConvertPcmSampleFormat(input, 6, SAMPLE_U8, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);
    EXPECT_EQ(std::memcmp(input, output, 6), 0);
}

/**
 * @tc.name: ConvertPcm_U8_To_S16LE
 * @tc.desc: Test U8 to S16LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_U8_To_S16LE, TestSize.Level1)
{
    uint8_t input[] = {0, 128, 255};
    uint8_t output[6] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_U8, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_NEAR(outSamples[0], -32768, 1);
    EXPECT_NEAR(outSamples[1], 0, 1);
    EXPECT_NEAR(outSamples[2], 32512, 128);
}

/**
 * @tc.name: ConvertPcm_U8_To_S32LE
 * @tc.desc: Test U8 to S32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_U8_To_S32LE, TestSize.Level1)
{
    uint8_t input[] = {0, 128, 255};
    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_U8, SAMPLE_S32LE, output);
    EXPECT_EQ(status, Status::OK);

    int32_t* outSamples = reinterpret_cast<int32_t*>(output);
    EXPECT_NEAR(outSamples[0], -2147483648, 1);
    EXPECT_NEAR(outSamples[1], 0, 1);
    EXPECT_NEAR(outSamples[2], 2130706432, 1);
}

/**
 * @tc.name: ConvertPcm_U8_To_F32LE
 * @tc.desc: Test U8 to F32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_U8_To_F32LE, TestSize.Level1)
{
    uint8_t input[] = {0, 128, 255};
    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_U8, SAMPLE_F32LE, output);
    EXPECT_EQ(status, Status::OK);

    float* outSamples = reinterpret_cast<float*>(output);
    EXPECT_NEAR(outSamples[0], -1.0f, 0.01f);
    EXPECT_NEAR(outSamples[1], 0.0f, 0.01f);
    EXPECT_NEAR(outSamples[2], 127.0f / 128.0f, 0.01f);
}

/**
 * @tc.name: ConvertPcm_S16LE_To_U8
 * @tc.desc: Test S16LE to U8 conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S16LE_To_U8, TestSize.Level1)
{
    int16_t input[] = {-32768, 0, 32767};
    uint8_t output[3] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S16LE, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);

    EXPECT_NEAR(output[0], 0, 1);
    EXPECT_NEAR(output[1], 128, 1);
    EXPECT_NEAR(output[2], 255, 1);
}

/**
 * @tc.name: ConvertPcm_S16LE_To_S32LE
 * @tc.desc: Test S16LE to S32LE conversion (left shift 16 bits)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S16LE_To_S32LE, TestSize.Level1)
{
    int16_t input[] = {-32768, 0, 16384};
    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S16LE, SAMPLE_S32LE, output);
    EXPECT_EQ(status, Status::OK);

    int32_t* outSamples = reinterpret_cast<int32_t*>(output);
    EXPECT_EQ(outSamples[0], -2147483648);
    EXPECT_EQ(outSamples[1], 0);
    EXPECT_EQ(outSamples[2], 1073741824);
}

/**
 * @tc.name: ConvertPcm_S16LE_To_F32LE
 * @tc.desc: Test S16LE to F32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S16LE_To_F32LE, TestSize.Level1)
{
    int16_t input[] = {-32768, 0, 32767};
    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S16LE, SAMPLE_F32LE, output);
    EXPECT_EQ(status, Status::OK);

    float* outSamples = reinterpret_cast<float*>(output);
    EXPECT_NEAR(outSamples[0], -1.0f, 0.0001f);
    EXPECT_NEAR(outSamples[1], 0.0f, 0.0001f);
    EXPECT_NEAR(outSamples[2], 0.99997f, 0.0001f);
}

/**
 * @tc.name: ConvertPcm_S32LE_To_S16LE
 * @tc.desc: Test S32LE to S16LE conversion (right shift 16 bits)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S32LE_To_S16LE, TestSize.Level1)
{
    int32_t input[] = {-2147483648, 0, 2147483647};
    uint8_t output[6] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S32LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_NEAR(outSamples[0], -32768, 1);
    EXPECT_NEAR(outSamples[1], 0, 1);
    EXPECT_NEAR(outSamples[2], 32767, 1);
}

/**
 * @tc.name: ConvertPcm_S32LE_To_F32LE
 * @tc.desc: Test S32LE to F32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S32LE_To_F32LE, TestSize.Level1)
{
    int32_t input[] = {-2147483648, 0, 2147483647};
    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S32LE, SAMPLE_F32LE, output);
    EXPECT_EQ(status, Status::OK);

    float* outSamples = reinterpret_cast<float*>(output);
    EXPECT_NEAR(outSamples[0], -1.0f, 0.0001f);
    EXPECT_NEAR(outSamples[1], 0.0f, 0.0001f);
    EXPECT_NEAR(outSamples[2], 1.0f, 0.0001f);
}

/**
 * @tc.name: ConvertPcm_F32LE_To_S16LE
 * @tc.desc: Test F32LE to S16LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_F32LE_To_S16LE, TestSize.Level1)
{
    float input[] = {-1.0f, 0.0f, 0.5f, 1.0f};
    uint8_t output[8] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 4,
        SAMPLE_F32LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_EQ(outSamples[0], -32768);
    EXPECT_EQ(outSamples[1], 0);
    EXPECT_NEAR(outSamples[2], 16384, 1);
    EXPECT_EQ(outSamples[3], 32767);
}

/**
 * @tc.name: ConvertPcm_F32LE_To_S32LE
 * @tc.desc: Test F32LE to S32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_F32LE_To_S32LE, TestSize.Level1)
{
    float input[] = {-1.0f, 0.0f, 0.5f, 1.0f};
    uint8_t output[16] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 4,
        SAMPLE_F32LE, SAMPLE_S32LE, output);
    EXPECT_EQ(status, Status::OK);

    int32_t* outSamples = reinterpret_cast<int32_t*>(output);
    EXPECT_NEAR(outSamples[0], -2147483648, 1);
    EXPECT_EQ(outSamples[1], 0);
    EXPECT_NEAR(outSamples[2], 1073741824, 1);
    EXPECT_NEAR(outSamples[3], 2147483647, 1);
}

/**
 * @tc.name: ConvertPcm_F32LE_To_U8
 * @tc.desc: Test F32LE to U8 conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_F32LE_To_U8, TestSize.Level1)
{
    float input[] = {-1.0f, 0.0f, 1.0f};
    uint8_t output[3] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_F32LE, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);

    EXPECT_NEAR(output[0], 0, 1);
    EXPECT_NEAR(output[1], 128, 1);
    EXPECT_NEAR(output[2], 255, 1);
}

/**
 * @tc.name: ConvertPcm_F32LE_Clamp
 * @tc.desc: Test F32LE to integer conversion clamps out-of-range values
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_F32LE_Clamp, TestSize.Level1)
{
    float input[] = {-2.0f, 2.0f};
    uint8_t output[4] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 2,
        SAMPLE_F32LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_EQ(outSamples[0], -32768);
    EXPECT_EQ(outSamples[1], 32767);
}

/**
 * @tc.name: ConvertPcm_U8_To_S24LE
 * @tc.desc: Test U8 to S24LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_U8_To_S24LE, TestSize.Level1)
{
    uint8_t input[] = {0, 128, 255};
    uint8_t output[9] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_U8, SAMPLE_S24LE, output);
    EXPECT_EQ(status, Status::OK);

    auto ReadS24 = [](const uint8_t* p) -> int32_t {
        uint32_t val = static_cast<uint32_t>(p[0]) |
                       (static_cast<uint32_t>(p[1]) << 8) |
                       (static_cast<uint32_t>(p[2]) << 16);
        if (val & 0x00800000u) {
            val |= 0xFF000000u;
        }
        return static_cast<int32_t>(val);
    };

    EXPECT_NEAR(ReadS24(output), -8388608, 1);
    EXPECT_NEAR(ReadS24(output + 3), 0, 1);
    EXPECT_NEAR(ReadS24(output + 6), 8323072, 1);
}

/**
 * @tc.name: ConvertPcm_S24LE_To_S16LE
 * @tc.desc: Test S24LE to S16LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S24LE_To_S16LE, TestSize.Level1)
{
    uint8_t input[9];
    auto WriteS24 = [](uint8_t* p, int32_t val) {
        uint32_t uval = static_cast<uint32_t>(val);
        p[0] = static_cast<uint8_t>(uval & 0xFFu);
        p[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);
        p[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);
    };
    WriteS24(input, -8388608);
    WriteS24(input + 3, 0);
    WriteS24(input + 6, 8388607);

    uint8_t output[6] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_S24LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_NEAR(outSamples[0], -32768, 1);
    EXPECT_NEAR(outSamples[1], 0, 1);
    EXPECT_NEAR(outSamples[2], 32767, 1);
}

/**
 * @tc.name: ConvertPcm_S24LE_To_S32LE
 * @tc.desc: Test S24LE to S32LE conversion (left shift 8 bits)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S24LE_To_S32LE, TestSize.Level1)
{
    uint8_t input[9];
    auto WriteS24 = [](uint8_t* p, int32_t val) {
        uint32_t uval = static_cast<uint32_t>(val);
        p[0] = static_cast<uint8_t>(uval & 0xFFu);
        p[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);
        p[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);
    };
    WriteS24(input, -8388608);
    WriteS24(input + 3, 0);
    WriteS24(input + 6, 8388607);

    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_S24LE, SAMPLE_S32LE, output);
    EXPECT_EQ(status, Status::OK);

    int32_t* outSamples = reinterpret_cast<int32_t*>(output);
    EXPECT_EQ(outSamples[0], -2147483648);
    EXPECT_EQ(outSamples[1], 0);
    EXPECT_EQ(outSamples[2], 2147483392);
}

/**
 * @tc.name: ConvertPcm_S24LE_To_F32LE
 * @tc.desc: Test S24LE to F32LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S24LE_To_F32LE, TestSize.Level1)
{
    uint8_t input[9];
    auto WriteS24 = [](uint8_t* p, int32_t val) {
        uint32_t uval = static_cast<uint32_t>(val);
        p[0] = static_cast<uint8_t>(uval & 0xFFu);
        p[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);
        p[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);
    };
    WriteS24(input, -8388608);
    WriteS24(input + 3, 0);
    WriteS24(input + 6, 8388607);

    uint8_t output[12] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_S24LE, SAMPLE_F32LE, output);
    EXPECT_EQ(status, Status::OK);

    float* outSamples = reinterpret_cast<float*>(output);
    EXPECT_NEAR(outSamples[0], -1.0f, 0.0001f);
    EXPECT_NEAR(outSamples[1], 0.0f, 0.0001f);
    EXPECT_NEAR(outSamples[2], 1.0f, 0.0001f);
}

/**
 * @tc.name: ConvertPcm_S32LE_To_S24LE
 * @tc.desc: Test S32LE to S24LE conversion (right shift 8 bits)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S32LE_To_S24LE, TestSize.Level1)
{
    int32_t input[] = {-2147483648, 0, 2147483647};
    uint8_t output[9] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S32LE, SAMPLE_S24LE, output);
    EXPECT_EQ(status, Status::OK);

    auto ReadS24 = [](const uint8_t* p) -> int32_t {
        uint32_t val = static_cast<uint32_t>(p[0]) |
                       (static_cast<uint32_t>(p[1]) << 8) |
                       (static_cast<uint32_t>(p[2]) << 16);
        if (val & 0x00800000u) {
            val |= 0xFF000000u;
        }
        return static_cast<int32_t>(val);
    };

    EXPECT_NEAR(ReadS24(output), -8388608, 1);
    EXPECT_NEAR(ReadS24(output + 3), 0, 1);
    EXPECT_NEAR(ReadS24(output + 6), 8388607, 1);
}

/**
 * @tc.name: ConvertPcm_S16LE_To_S24LE
 * @tc.desc: Test S16LE to S24LE conversion (left shift 8 bits)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S16LE_To_S24LE, TestSize.Level1)
{
    int16_t input[] = {-32768, 0, 32767};
    uint8_t output[9] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S16LE, SAMPLE_S24LE, output);
    EXPECT_EQ(status, Status::OK);

    auto ReadS24 = [](const uint8_t* p) -> int32_t {
        uint32_t val = static_cast<uint32_t>(p[0]) |
                       (static_cast<uint32_t>(p[1]) << 8) |
                       (static_cast<uint32_t>(p[2]) << 16);
        if (val & 0x00800000u) {
            val |= 0xFF000000u;
        }
        return static_cast<int32_t>(val);
    };

    EXPECT_EQ(ReadS24(output), -8388608);
    EXPECT_EQ(ReadS24(output + 3), 0);
    EXPECT_EQ(ReadS24(output + 6), 8388352);
}

/**
 * @tc.name: ConvertPcm_F32LE_To_S24LE
 * @tc.desc: Test F32LE to S24LE conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_F32LE_To_S24LE, TestSize.Level1)
{
    float input[] = {-1.0f, 0.0f, 1.0f};
    uint8_t output[9] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_F32LE, SAMPLE_S24LE, output);
    EXPECT_EQ(status, Status::OK);

    auto ReadS24 = [](const uint8_t* p) -> int32_t {
        uint32_t val = static_cast<uint32_t>(p[0]) |
                       (static_cast<uint32_t>(p[1]) << 8) |
                       (static_cast<uint32_t>(p[2]) << 16);
        if (val & 0x00800000u) {
            val |= 0xFF000000u;
        }
        return static_cast<int32_t>(val);
    };

    EXPECT_NEAR(ReadS24(output), -8388608, 1);
    EXPECT_NEAR(ReadS24(output + 3), 0, 1);
    EXPECT_NEAR(ReadS24(output + 6), 8388607, 1);
}

/**
 * @tc.name: ConvertPcm_MultiChannel
 * @tc.desc: Test conversion with multi-channel interleaved data (stereo)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_MultiChannel, TestSize.Level1)
{
    int16_t input[] = {-1000, 500, 20000, -30000};
    uint8_t output[8] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 4,
        SAMPLE_S16LE, SAMPLE_F32LE, output);
    EXPECT_EQ(status, Status::OK);

    float* outSamples = reinterpret_cast<float*>(output);
    EXPECT_NEAR(outSamples[0], -1000.0f / 32768.0f, 0.001f);
    EXPECT_NEAR(outSamples[1], 500.0f / 32768.0f, 0.001f);
    EXPECT_NEAR(outSamples[2], 20000.0f / 32768.0f, 0.001f);
    EXPECT_NEAR(outSamples[3], -30000.0f / 32768.0f, 0.001f);
}

/**
 * @tc.name: ConvertPcm_LargeBuffer
 * @tc.desc: Test conversion with a larger buffer (1024 samples)
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_LargeBuffer, TestSize.Level1)
{
    constexpr size_t sampleCount = 1024;
    std::vector<int16_t> input(sampleCount);
    for (size_t i = 0; i < sampleCount; ++i) {
        input[i] = static_cast<int16_t>(i - 512);
    }

    std::vector<uint8_t> output(sampleCount * 4);
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input.data()),
        sampleCount, SAMPLE_S16LE, SAMPLE_S32LE, output.data());
    EXPECT_EQ(status, Status::OK);

    int32_t* outSamples = reinterpret_cast<int32_t*>(output.data());
    for (size_t i = 0; i < sampleCount; ++i) {
        EXPECT_EQ(outSamples[i], static_cast<int32_t>(input[i]) * 65536);
    }
}

/**
 * @tc.name: ConvertPcm_S24LE_To_U8
 * @tc.desc: Test S24LE to U8 conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S24LE_To_U8, TestSize.Level1)
{
    uint8_t input[9];
    auto WriteS24 = [](uint8_t* p, int32_t val) {
        uint32_t uval = static_cast<uint32_t>(val);
        p[0] = static_cast<uint8_t>(uval & 0xFFu);
        p[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);
        p[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);
    };
    WriteS24(input, -8388608);
    WriteS24(input + 3, 0);
    WriteS24(input + 6, 8388607);

    uint8_t output[3] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_S24LE, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);

    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 128);
    EXPECT_EQ(output[2], 255);
}

/**
 * @tc.name: ConvertPcm_S32LE_To_U8
 * @tc.desc: Test S32LE to U8 conversion
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S32LE_To_U8, TestSize.Level1)
{
    int32_t input[] = {-2147483648, 0, 2147483647};
    uint8_t output[3] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), 3,
        SAMPLE_S32LE, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);

    EXPECT_EQ(output[0], 0);
    EXPECT_EQ(output[1], 128);
    EXPECT_EQ(output[2], 255);
}

/**
 * @tc.name: ConvertPcm_U8_RoundTrip
 * @tc.desc: Test U8 to F32LE and back to U8 round-trip preserves values
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_U8_RoundTrip, TestSize.Level1)
{
    uint8_t input[] = {0, 1, 127, 128, 129, 254, 255};
    constexpr size_t count = sizeof(input) / sizeof(input[0]);

    uint8_t floatBuf[count * 4] = {0};
    Status status = ConvertPcmSampleFormat(input, count, SAMPLE_U8, SAMPLE_F32LE, floatBuf);
    EXPECT_EQ(status, Status::OK);

    uint8_t output[count] = {0};
    status = ConvertPcmSampleFormat(floatBuf, count, SAMPLE_F32LE, SAMPLE_U8, output);
    EXPECT_EQ(status, Status::OK);

    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(output[i], input[i]);
    }
}

/**
 * @tc.name: ConvertPcm_S16LE_RoundTrip
 * @tc.desc: Test S16LE to F32LE and back to S16LE round-trip preserves values
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S16LE_RoundTrip, TestSize.Level1)
{
    int16_t input[] = {-32768, -1, 0, 1, 32767};
    constexpr size_t count = sizeof(input) / sizeof(input[0]);

    uint8_t floatBuf[count * 4] = {0};
    Status status = ConvertPcmSampleFormat(reinterpret_cast<uint8_t*>(input), count,
        SAMPLE_S16LE, SAMPLE_F32LE, floatBuf);
    EXPECT_EQ(status, Status::OK);

    uint8_t output[count * 2] = {0};
    status = ConvertPcmSampleFormat(floatBuf, count, SAMPLE_F32LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(outSamples[i], input[i]);
    }
}

/**
 * @tc.name: ConvertPcm_S24LE_NegativeValues
 * @tc.desc: Test S24LE conversion with negative intermediate values
 * @tc.type: FUNC
 */
HWTEST_F(PcmConvertUnitTest, ConvertPcm_S24LE_NegativeValues, TestSize.Level1)
{
    uint8_t input[9];
    auto WriteS24 = [](uint8_t* p, int32_t val) {
        uint32_t uval = static_cast<uint32_t>(val);
        p[0] = static_cast<uint8_t>(uval & 0xFFu);
        p[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);
        p[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);
    };
    WriteS24(input, -1);         // negative one
    WriteS24(input + 3, -256);   // -256
    WriteS24(input + 6, -8388608); // min S24

    // S24 to S16: right shift 8 bits
    uint8_t output[6] = {0};
    Status status = ConvertPcmSampleFormat(input, 3, SAMPLE_S24LE, SAMPLE_S16LE, output);
    EXPECT_EQ(status, Status::OK);

    int16_t* outSamples = reinterpret_cast<int16_t*>(output);
    EXPECT_EQ(outSamples[0], -1);      // -1 >> 8 = -1
    EXPECT_EQ(outSamples[1], -1);      // -256 >> 8 = -1
    EXPECT_EQ(outSamples[2], -32768);  // -8388608 >> 8 = -32768
}

} // namespace Media
} // namespace OHOS
