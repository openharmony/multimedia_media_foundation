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

#include "common/pcm_convert.h"
#include "securec.h"
#include <cstring>
#include <algorithm>
#include <cmath>

namespace OHOS {
namespace Media {

using Plugins::AudioSampleFormat;

namespace {

constexpr float F32_MAX_VAL = 1.0f;
constexpr float F32_MIN_VAL = -1.0f;

constexpr float U8_OFFSET = 128.0f;
constexpr float U8_SCALE = 128.0f;

constexpr float S16_SCALE = 32768.0f;
constexpr float S24_SCALE = 8388608.0f;
constexpr float S32_SCALE = 2147483648.0f;

inline int32_t ReadS24(const uint8_t* ptr)
{
    uint32_t val = static_cast<uint32_t>(ptr[0]) |
                   (static_cast<uint32_t>(ptr[1]) << 8) |   // 1 byte: 8 bits
                   (static_cast<uint32_t>(ptr[2]) << 16);   // 2 bytes: 16 bits
    if (val & 0x00800000u) {
        val |= 0xFF000000u;
    }
    return static_cast<int32_t>(val);
}

inline void WriteS24(uint8_t* ptr, int32_t val)
{
    uint32_t uval = static_cast<uint32_t>(val);
    ptr[0] = static_cast<uint8_t>(uval & 0xFFu);
    ptr[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);    // 1 byte: 8 bits
    ptr[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);   // 2 bytes: 16 bits
}

inline int16_t ReadS16(const uint8_t* ptr)
{
    return static_cast<int16_t>(static_cast<uint16_t>(ptr[0]) |
                                (static_cast<uint16_t>(ptr[1]) << 8)); // 8
}

inline void WriteS16(uint8_t* ptr, int16_t val)
{
    uint16_t uval = static_cast<uint16_t>(val);
    ptr[0] = static_cast<uint8_t>(uval & 0xFFu);
    ptr[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);    // 1 byte: 8 bits
}

inline int32_t ReadS32(const uint8_t* ptr)
{
    return static_cast<int32_t>(static_cast<uint32_t>(ptr[0]) |
                                (static_cast<uint32_t>(ptr[1]) << 8) | // 1 byte: 8bit
                                (static_cast<uint32_t>(ptr[2]) << 16) | // 2 byte: 16bit
                                (static_cast<uint32_t>(ptr[3]) << 24)); // 3 byte: 24bit
}

inline void WriteS32(uint8_t* ptr, int32_t val)
{
    uint32_t uval = static_cast<uint32_t>(val);
    ptr[0] = static_cast<uint8_t>(uval & 0xFFu);
    ptr[1] = static_cast<uint8_t>((uval >> 8) & 0xFFu);    // 1 byte: 8 bits
    ptr[2] = static_cast<uint8_t>((uval >> 16) & 0xFFu);   // 2 bytes: 16 bits
    ptr[3] = static_cast<uint8_t>((uval >> 24) & 0xFFu);   // 3 bytes: 24 bits
}

inline float ReadF32(const uint8_t* ptr)
{
    float val;
    if (memcpy_s(&val, sizeof(float), ptr, sizeof(float)) != EOK) {
        return 0;
    }
    return val;
}

inline void WriteF32(uint8_t* ptr, float val)
{
    if (memcpy_s(ptr, sizeof(float), &val, sizeof(float)) != EOK) {
        return;
    }
    return;
}

inline float U8ToFloat(uint8_t val)
{
    return (static_cast<float>(val) - U8_OFFSET) / U8_SCALE;
}

inline float S16ToFloat(int16_t val)
{
    return static_cast<float>(val) / S16_SCALE;
}

inline float S24ToFloat(int32_t val)
{
    return static_cast<float>(val) / S24_SCALE;
}

inline float S32ToFloat(int32_t val)
{
    return static_cast<float>(val) / S32_SCALE;
}

inline uint8_t FloatToU8(float val)
{
    val = std::clamp(val, F32_MIN_VAL, F32_MAX_VAL);
    int32_t result = static_cast<int32_t>(std::round(val * 128.0f + 128.0f)); // scale 128, same as U8ToFloat
    return static_cast<uint8_t>(std::clamp(result, 0, 255)); // 255 max U8
}

inline int16_t FloatToS16(float val)
{
    val = std::clamp(val, F32_MIN_VAL, F32_MAX_VAL);
    int32_t result = static_cast<int32_t>(std::round(val * 32768.0f)); // 32768
    return static_cast<int16_t>(std::clamp(result, -32768, 32767)); // -32768 ~ 32767
}

inline int32_t FloatToS24(float val)
{
    val = std::clamp(val, F32_MIN_VAL, F32_MAX_VAL);
    int32_t result = static_cast<int32_t>(std::round(val * 8388608.0f)); // 8388608.0f
    return std::clamp(result, -8388608, 8388607); // -8388608 ~ 8388607
}

inline int32_t FloatToS32(float val)
{
    val = std::clamp(val, F32_MIN_VAL, F32_MAX_VAL);
    int64_t result = static_cast<int64_t>(std::round(val * 2147483648.0)); // 2147483648.0
    // -2147483648 ~ 2147483647
    return static_cast<int32_t>(std::clamp(result,
        static_cast<int64_t>(-2147483648), // -2147483648 ~
        static_cast<int64_t>(2147483647))); // ~ 2147483647
}

template<int32_t Divisor>
inline int32_t FloorDiv(int32_t v)
{
    constexpr int32_t adjustment = Divisor - 1; // adjustment for floor division
    return v < 0 ? static_cast<int32_t>((static_cast<int64_t>(v) - adjustment) / Divisor) : v / Divisor;
}

bool IsSupportedInterleavedFormat(AudioSampleFormat format)
{
    switch (format) {
        case AudioSampleFormat::SAMPLE_U8: // fall-through
        case AudioSampleFormat::SAMPLE_S16LE: // fall-through
        case AudioSampleFormat::SAMPLE_S24LE: // fall-through
        case AudioSampleFormat::SAMPLE_S32LE: // fall-through
        case AudioSampleFormat::SAMPLE_F32LE:
            return true;
        default:
            return false;
    }
}

template<typename ReadFn, typename WriteFn, typename ConvertFn>
void ConvertSamples(const uint8_t* input, size_t sampleCount, uint8_t* output,
    int32_t inputBytes, int32_t outputBytes, ReadFn read, WriteFn write, ConvertFn convert)
{
    const uint8_t* inPtr = input;
    uint8_t* outPtr = output;
    for (size_t i = 0; i < sampleCount; ++i) {
        auto val = read(inPtr);
        auto converted = convert(val);
        write(outPtr, converted);
        inPtr += inputBytes;
        outPtr += outputBytes;
    }
}

Status ConvertFromU8(const uint8_t* input, size_t sampleCount,
    int32_t inputBytes, int32_t outputBytes, uint8_t* output, AudioSampleFormat outputFormat)
{
    switch (outputFormat) {
        case AudioSampleFormat::SAMPLE_S16LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return *p; },
                [](uint8_t* p, int16_t v) { WriteS16(p, v); },
                [](uint8_t v) { return FloatToS16(U8ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_S24LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return *p; },
                [](uint8_t* p, int32_t v) { WriteS24(p, v); },
                [](uint8_t v) { return FloatToS24(U8ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_S32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return *p; },
                [](uint8_t* p, int32_t v) { WriteS32(p, v); },
                [](uint8_t v) { return FloatToS32(U8ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_F32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return *p; },
                [](uint8_t* p, float v) { WriteF32(p, v); },
                [](uint8_t v) { return U8ToFloat(v); });
            break;
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

Status ConvertFromS16LE(const uint8_t* input, size_t sampleCount,
    int32_t inputBytes, int32_t outputBytes, uint8_t* output, AudioSampleFormat outputFormat)
{
    switch (outputFormat) {
        case AudioSampleFormat::SAMPLE_U8:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS16(p); },
                [](uint8_t* p, uint8_t v) { *p = v; },
                [](int16_t v) { return FloatToU8(S16ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_S24LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS16(p); },
                [](uint8_t* p, int32_t v) { WriteS24(p, v); },
                [](int16_t v) { return static_cast<int32_t>(v) * 256; }); // 256: S16 to S24 scale
            break;
        case AudioSampleFormat::SAMPLE_S32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS16(p); },
                [](uint8_t* p, int32_t v) { WriteS32(p, v); },
                [](int16_t v) { return static_cast<int32_t>(v) * 65536; }); // 65536: S16 to S32 scale
            break;
        case AudioSampleFormat::SAMPLE_F32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS16(p); },
                [](uint8_t* p, float v) { WriteF32(p, v); },
                [](int16_t v) { return S16ToFloat(v); });
            break;
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

Status ConvertFromS24LE(const uint8_t* input, size_t sampleCount,
    int32_t inputBytes, int32_t outputBytes, uint8_t* output, AudioSampleFormat outputFormat)
{
    switch (outputFormat) {
        case AudioSampleFormat::SAMPLE_U8:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS24(p); },
                [](uint8_t* p, uint8_t v) { *p = v; },
                [](int32_t v) { return FloatToU8(S24ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_S16LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS24(p); },
                [](uint8_t* p, int16_t v) { WriteS16(p, v); },
                [](int32_t v) { return static_cast<int16_t>(FloorDiv<256>(v)); }); // 256: S24 to S16 scale
            break;
        case AudioSampleFormat::SAMPLE_S32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS24(p); },
                [](uint8_t* p, int32_t v) { WriteS32(p, v); },
                [](int32_t v) { return v * 256; }); // 256: S24 to S32 scale
            break;
        case AudioSampleFormat::SAMPLE_F32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS24(p); },
                [](uint8_t* p, float v) { WriteF32(p, v); },
                [](int32_t v) { return S24ToFloat(v); });
            break;
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

Status ConvertFromS32LE(const uint8_t* input, size_t sampleCount,
    int32_t inputBytes, int32_t outputBytes, uint8_t* output, AudioSampleFormat outputFormat)
{
    switch (outputFormat) {
        case AudioSampleFormat::SAMPLE_U8:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS32(p); },
                [](uint8_t* p, uint8_t v) { *p = v; },
                [](int32_t v) { return FloatToU8(S32ToFloat(v)); });
            break;
        case AudioSampleFormat::SAMPLE_S16LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS32(p); },
                [](uint8_t* p, int16_t v) { WriteS16(p, v); },
                [](int32_t v) { return static_cast<int16_t>(FloorDiv<65536>(v)); }); // 65536: S32 to S16 scale
            break;
        case AudioSampleFormat::SAMPLE_S24LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS32(p); },
                [](uint8_t* p, int32_t v) { WriteS24(p, v); },
                [](int32_t v) { return FloorDiv<256>(v); }); // 256: S32 to S24 scale
            break;
        case AudioSampleFormat::SAMPLE_F32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadS32(p); },
                [](uint8_t* p, float v) { WriteF32(p, v); },
                [](int32_t v) { return S32ToFloat(v); });
            break;
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

Status ConvertFromF32LE(const uint8_t* input, size_t sampleCount,
    int32_t inputBytes, int32_t outputBytes, uint8_t* output, AudioSampleFormat outputFormat)
{
    switch (outputFormat) {
        case AudioSampleFormat::SAMPLE_U8:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadF32(p); },
                [](uint8_t* p, uint8_t v) { *p = v; },
                [](float v) { return FloatToU8(v); });
            break;
        case AudioSampleFormat::SAMPLE_S16LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadF32(p); },
                [](uint8_t* p, int16_t v) { WriteS16(p, v); },
                [](float v) { return FloatToS16(v); });
            break;
        case AudioSampleFormat::SAMPLE_S24LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadF32(p); },
                [](uint8_t* p, int32_t v) { WriteS24(p, v); },
                [](float v) { return FloatToS24(v); });
            break;
        case AudioSampleFormat::SAMPLE_S32LE:
            ConvertSamples(input, sampleCount, output, inputBytes, outputBytes,
                [](const uint8_t* p) { return ReadF32(p); },
                [](uint8_t* p, int32_t v) { WriteS32(p, v); },
                [](float v) { return FloatToS32(v); });
            break;
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    return Status::OK;
}

} // namespace

int32_t GetPcmBytesPerSample(AudioSampleFormat format)
{
    switch (format) {
        case AudioSampleFormat::SAMPLE_U8:
            return 1;
        case AudioSampleFormat::SAMPLE_S16LE:
            return 2; // 2 bytes
        case AudioSampleFormat::SAMPLE_S24LE:
            return 3; // 3 bytes
        case AudioSampleFormat::SAMPLE_S32LE: // fall-through
        case AudioSampleFormat::SAMPLE_F32LE:
            return 4; // 4 bytes
        default:
            return 0;
    }
}

size_t GetPcmConvertOutputSize(size_t sampleCount, AudioSampleFormat outputFormat)
{
    int32_t bytesPerSample = GetPcmBytesPerSample(outputFormat);
    if (bytesPerSample <= 0) {
        return 0;
    }
    return sampleCount * static_cast<size_t>(bytesPerSample);
}

Status ConvertPcmSampleFormat(const uint8_t* input, size_t sampleCount,
    AudioSampleFormat inputFormat, AudioSampleFormat outputFormat, uint8_t* output)
{
    if (input == nullptr || output == nullptr) {
        return Status::ERROR_NULL_POINTER;
    }
    if (sampleCount == 0) {
        return Status::OK;
    }
    if (!IsSupportedInterleavedFormat(inputFormat) || !IsSupportedInterleavedFormat(outputFormat)) {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }

    int32_t inputBytes = GetPcmBytesPerSample(inputFormat);
    int32_t outputBytes = GetPcmBytesPerSample(outputFormat);
    if (inputBytes <= 0 || outputBytes <= 0) {
        return Status::ERROR_INVALID_PARAMETER;
    }

    if (inputFormat == outputFormat) {
        if (memmove_s(output, sampleCount * static_cast<size_t>(inputBytes),
                      input, sampleCount * static_cast<size_t>(inputBytes)) != EOK) {
            return Status::ERROR_INVALID_PARAMETER;
        }
        return Status::OK;
    }

    switch (inputFormat) {
        case AudioSampleFormat::SAMPLE_U8:
            return ConvertFromU8(input, sampleCount, inputBytes, outputBytes, output, outputFormat);
        case AudioSampleFormat::SAMPLE_S16LE:
            return ConvertFromS16LE(input, sampleCount, inputBytes, outputBytes, output, outputFormat);
        case AudioSampleFormat::SAMPLE_S24LE:
            return ConvertFromS24LE(input, sampleCount, inputBytes, outputBytes, output, outputFormat);
        case AudioSampleFormat::SAMPLE_S32LE:
            return ConvertFromS32LE(input, sampleCount, inputBytes, outputBytes, output, outputFormat);
        case AudioSampleFormat::SAMPLE_F32LE:
            return ConvertFromF32LE(input, sampleCount, inputBytes, outputBytes, output, outputFormat);
        default:
            return Status::ERROR_UNSUPPORTED_FORMAT;
    }
}

} // namespace Media
} // namespace OHOS
