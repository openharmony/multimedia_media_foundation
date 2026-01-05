/*
 * Copyright (C) 2023-2025 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Core
 * @{
 *
 * @brief The Core module provides basic backbone capabilities for media frameworks,
 * including functions such as memory, error codes, and media data structures.
 *
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 9
 */

/**
 * @file native_avformat.h
 *
 * @brief Declared functions and enumerations related to OH_AVFormat.
 *
 * @kit AVCodecKit
 * @library libnative_media_core.so
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 9
 */

#ifndef NATIVE_AVFORMAT_H
#define NATIVE_AVFORMAT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OH_AVFormat OH_AVFormat;

/**
 * @brief Enumerates AVPixel Format.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 9
 * @version 1.0
 */
typedef enum OH_AVPixelFormat {
    /**
     * yuv 420 planar.
     */
    AV_PIXEL_FORMAT_YUVI420 = 1,
    /**
     *  NV12. yuv 420 semiplanar.
     */
    AV_PIXEL_FORMAT_NV12 = 2,
    /**
     *  NV21. yvu 420 semiplanar.
     */
    AV_PIXEL_FORMAT_NV21 = 3,
    /**
     * format from surface.
     */
    AV_PIXEL_FORMAT_SURFACE_FORMAT = 4,
    /**
     * RGBA8888
     */
    AV_PIXEL_FORMAT_RGBA = 5,
    /** RGBA1010102
     * since 20
     */
    AV_PIXEL_FORMAT_RGBA1010102 = 6,
} OH_AVPixelFormat;

/**
 * @briefCreate an OH_AVFormat handle pointer to read and write data
 * @syscap SystemCapability.Multimedia.Media.Core
 * @return Returns a pointer to an OH_AVFormat instance
 * @since 9
 * @version 1.0
 */
struct OH_AVFormat *OH_AVFormat_Create(void);

/**
 * @brief Create an audio OH_AVFormat handle pointer to read and write data.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param mimeType mime type
 * @param sampleRate sample rate
 * @param channelCount channel count
 * @return Returns a pointer to an OH_AVFormat instance if the execution is successful, otherwise NULL
 * Possible failure causes: 1. mimeType is NULL; 2. new format is NULL.
 * @since 10
 * @version 1.0
 */
struct OH_AVFormat *OH_AVFormat_CreateAudioFormat(const char *mimeType,
                                                  int32_t sampleRate,
                                                  int32_t channelCount);

/**
 * @brief Create an video OH_AVFormat handle pointer to read and write data.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param mimeType mime type
 * @param width width
 * @param height height
 * @return Returns a pointer to an OH_AVFormat instance if the execution is successful, otherwise NULL
 * Possible failure causes: 1. mimeType is NULL; 2. new format is NULL.
 * @since 10
 * @version 1.0
 */
struct OH_AVFormat *OH_AVFormat_CreateVideoFormat(const char *mimeType,
                                                  int32_t width,
                                                  int32_t height);

/**
 * @brief Destroy the OH_AVFormat instance, can not be destoryed repeatedly.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @return void
 * @since 9
 * @version 1.0
 */
void OH_AVFormat_Destroy(struct OH_AVFormat *format);

/**
 * @brief Copy OH_AVFormat handle resource.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param to OH_AVFormat handle pointer to receive data
 * @param from pointer to the OH_AVFormat handle of the copied data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input parameter is NULL;
 * 2. structure verification failed of the input OH_AVFormat.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_Copy(struct OH_AVFormat *to, struct OH_AVFormat *from);

/**
 * @brief Set a value of Int type to the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. value type corresponding to the key is incorrect.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetIntValue(struct OH_AVFormat *format, const char *key, int32_t value);

/**
 * @brief Write unsigned integer data to OH_AVFormat
 * @param {OH_AVFormat*} format pointer to an OH_AVFormat instance
 * @param {const char*} key key to write data
 * @param {uint32_t} value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes: 1. input format is nullptr. 2. input format's magic error.3. key is nullptr.
 * @since 23
 */
bool OH_AVFormat_SetUintValue(struct OH_AVFormat *format, const char *key, uint32_t value);

/**
 * @brief Set a value of Long type to the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. value type corresponding to the key is incorrect.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetLongValue(struct OH_AVFormat *format, const char *key, int64_t value);

/**
 * @brief Set a value of Float type to the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. value type corresponding to the key is incorrect.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetFloatValue(struct OH_AVFormat *format, const char *key, float value);

/**
 * @brief Set a value of Double type to the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. value type corresponding to the key is incorrect.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetDoubleValue(struct OH_AVFormat *format, const char *key, double value);

/**
 * @brief Set a value of String type to the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param value written data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input value is NULL;
 * 5. value type corresponding to the key is incorrect;
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetStringValue(struct OH_AVFormat *format, const char *key, const char *value);

/**
 * @brief Write a block of data of a specified length to OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param addr written data addr, the lifecycle is managed by the invoker
 * @param size written data length, range is(0, 1)MB
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input addr is NULL;
 * 5. size is 0 or exceeds the upper limit, which is 1MB;
 * 6. value type corresponding to the key is incorrect.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_SetBuffer(struct OH_AVFormat *format, const char *key, const uint8_t *addr, size_t size);

/**
 * @brief Write a list of int32_t data of a specified length to OH_AVFormat.
 *
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key key to write data
 * @param addr written data addr
 * @param size written data length
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL.
 * 2. input format's magic error.
 * 3. key is nullptr.
 * 4. addr is nullptr.
 * 5. size is zero.
 * @since 20
 */
bool OH_AVFormat_SetIntBuffer(struct OH_AVFormat *format, const char *key, const int32_t *addr, size_t size);

/**
 * @brief Get the Int value from the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key read key value
 * @param out read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input out is NULL;
 * 5. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetIntValue(struct OH_AVFormat *format, const char *key, int32_t *out);

/**
 * @brief Read unsigned integer data from OH_AVFormat
 * @param {OH_AVFormat*} format pointer to an OH_AVFormat instance
 * @param {const char*} key read key value
 * @param {uint32_t} out read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes: 1. input format is nullptr. 2. input format's magic error. 3. key is nullptr.
 * 4. out is nullptr.
 * @since 23
 */
bool OH_AVFormat_GetUintValue(struct OH_AVFormat *format, const char *key, uint32_t *out);

/**
 * @brief Get the Long value from the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key read key value
 * @param out read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input out is NULL;
 * 5. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetLongValue(struct OH_AVFormat *format, const char *key, int64_t *out);

/**
 * @brief Get the Float value from the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key read key value
 * @param out read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input out is NULL;
 * 5. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetFloatValue(struct OH_AVFormat *format, const char *key, float *out);

/**
 * @brief Get the Double value from the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key read key value
 * @param out read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input out is NULL;
 * 5. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetDoubleValue(struct OH_AVFormat *format, const char *key, double *out);

/**
 * @brief Get the String value from the key of OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key read key value
 * @param out The read string pointer, the data life cycle pointed to is updated with GetString,
 * and Format is destroyed. If the caller needs to hold it for a long time, it must copy the memory
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input out is NULL;
 * 5. the resources of out string generated by malloc is insufficient;
 * 6. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetStringValue(struct OH_AVFormat *format, const char *key, const char **out);

/**
 * @brief Read a block of data of specified length from OH_AVFormat
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key Key value for reading data
 * @param addr The life cycle is held by the format, with the destruction of the format,
 * if the caller needs to hold it for a long time, it must copy the memory
 * @param size Length of read data
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is NULL;
 * 2. structure verification failed of the input format;
 * 3. input key is NULL;
 * 4. input addr is NULL;
 * 5. input size is NULL;
 * 6. the obtained key does not exist or is not set.
 * @since 9
 * @version 1.0
 */
bool OH_AVFormat_GetBuffer(struct OH_AVFormat *format, const char *key, uint8_t **addr, size_t *size);

/**
 * @brief Read an array of int32_t values from an OH_AVFormat object.
 *
 * Note that the obtained buffer's lifetime bound to the OH_AVFormat object,
 * it's automatically invalidated when the format object is destroyed.\n
 * Applications must explicitly copy the data to newly allocated memory if
 * the data needs to outlive the OH_AVFormat instance.\n
 *
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @param key Data identifier key
 * @param addr Pointer to receive the data buffer reference
 * @param size Pointer to receive the element count
 * @return The return value is TRUE for success, FALSE for failure
 * Possible failure causes:
 * 1. input format is nullptr.
 * 2. input format's magic error.
 * 3. key is nullptr.
 * 4. addr is nullptr.
 * 5. size is nullptr.
 * @since 20
 */
bool OH_AVFormat_GetIntBuffer(struct OH_AVFormat *format, const char *key, int32_t **addr, size_t *size);

/**
 * @brief Return a string consisting of key and values contained in OH_AVFormat.
 * the max string that can be returned is 1024 bytes,
 * and the string pointer is released when the format is destroyed.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param format pointer to an OH_AVFormat instance
 * @return Returns a string consisting of key and data for success, NULL for failure
 * Possible failure causes: 1. input format is NULL; 2. system resources are insufficient.
 * @since 9
 * @version 1.0
 */
const char *OH_AVFormat_DumpInfo(struct OH_AVFormat *format);

/**
 * @brief Get the total number of keys contained in OH_AVFormat.
 * @param format Pointer to an OH_AVFormat instance
 * @return Returns the number of keys on success; returns 0 on failure
 * @details Possible failure causes:
 * 1. input format is NULL;
 * 2. system resources are insufficient.
 * @since 23
 */
uint32_t OH_AVFormat_GetKeyCount(OH_AVFormat *format);

/**
 * @brief Get the key name string by index from OH_AVFormat.
 * @param format Pointer to an OH_AVFormat instance
 * @param index Zero-based index of the key to query, range: [0, OH_AVFormat_GetKeyCount(format))
 * @param key Output pointer to receive the key name string; the lifecycle is bound to the format
 * @return Returns TRUE on success, FALSE on failure
 * @details Possible failure causes:
 * 1. input format is NULL;
 * 2. index is out of range;
 * 3. key is NULL;
 * 4. system resources are insufficient.
 * @since 23
 */
bool OH_AVFormat_GetKey(OH_AVFormat *format, uint32_t index, const char **key);

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AVFORMAT_H
/** @} */