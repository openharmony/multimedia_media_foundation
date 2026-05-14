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

#ifndef MEDIA_FOUNDATION_PCM_CONVERT_H
#define MEDIA_FOUNDATION_PCM_CONVERT_H

#include <cstdint>
#include <cstddef>
#include "common/status.h"
#include "meta/audio_types.h"

namespace OHOS {
namespace Media {

/**
 * @brief Get the number of bytes per sample for a given PCM format.
 *
 * Supports interleaved formats: SAMPLE_U8, SAMPLE_S16LE, SAMPLE_S24LE, SAMPLE_S32LE, SAMPLE_F32LE.
 * @param format The audio sample format.
 * @return Bytes per sample, or 0 if the format is invalid or unsupported.
 */
int32_t GetPcmBytesPerSample(Plugins::AudioSampleFormat format);

/**
 * @brief Calculate the required output buffer size for PCM sample format conversion.
 *
 * @param sampleCount Number of audio samples (not bytes).
 * @param outputFormat Target PCM sample format.
 * @return Required output buffer size in bytes, or 0 if parameters are invalid.
 */
size_t GetPcmConvertOutputSize(size_t sampleCount, Plugins::AudioSampleFormat outputFormat);

/**
 * @brief Convert PCM data from one sample format to another.
 *
 * Supports conversion between SAMPLE_U8, SAMPLE_S16LE, SAMPLE_S24LE, SAMPLE_S32LE, and SAMPLE_F32LE.
 * The caller is responsible for allocating the output buffer with sufficient size.
 *
 * @param input Pointer to input PCM data. Must not be null.
 * @param sampleCount Number of audio samples to convert (not bytes).
 * @param inputFormat Source PCM sample format.
 * @param outputFormat Target PCM sample format.
 * @param output Pointer to output buffer. Must not be null and must have sufficient size.
 * @return Status::OK on success, or an error code on failure.
 *         Returns ERROR_NULL_POINTER if input/output is null.
 *         Returns ERROR_INVALID_PARAMETER if formats are invalid.
 *         Returns ERROR_UNSUPPORTED_FORMAT if the conversion pair is not supported.
 */
Status ConvertPcmSampleFormat(const uint8_t* input, size_t sampleCount,
    Plugins::AudioSampleFormat inputFormat, Plugins::AudioSampleFormat outputFormat, uint8_t* output);

} // namespace Media
} // namespace OHOS

#endif // MEDIA_FOUNDATION_PCM_CONVERT_H
