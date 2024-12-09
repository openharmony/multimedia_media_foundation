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
 * @file native_avbuffer.h
 *
 * @brief Declared the function interface of media data structure AVBuffer.
 * 
 * @library libnative_media_core.so
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 11
 */

#ifndef NATIVE_AVBUFFER_H
#define NATIVE_AVBUFFER_H

#include <stdint.h>
#include <stdio.h>
#include "native_avbuffer_info.h"
#include "native_averrors.h"
#include "native_avformat.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct OH_AVBuffer OH_AVBuffer;
typedef struct OH_NativeBuffer OH_NativeBuffer;

/**
 * @brief Create an OH_AVBuffer instance, It should be noted that the OH_AVBuffer instance pointed
 * to by the return value * needs to be released by {@link OH_AVBuffer_Destroy}.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param capacity the buffer's capacity, bytes
 * @return Returns a pointer to an OH_AVBuffer instance if the execution is successful, otherwise returns NULL.
 * Possible failure causes:
 * 1. capacity <= 0;
 * 2. internal error occurred, the system has no resources.
 * @since 11
 */
OH_AVBuffer *OH_AVBuffer_Create(int32_t capacity);

/**
 * @brief Clear the internal resources of the buffer and destroy the buffer instance.
 * The same buffer can not be destroyed repeatedly.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @return Function result code.
 *         {@link AV_ERR_OK} if the execution is successful.
 *         {@link AV_ERR_INVALID_VAL} if input buffer is NULL or structure verification failed of the buffer.
 *         {@link AV_ERR_OPERATE_NOT_PERMIT} if input buffer is not user created.
 * @since 11
 */
OH_AVErrCode OH_AVBuffer_Destroy(OH_AVBuffer *buffer);

/**
 * @brief Get the buffer's attribute, such as pts, size, offset, and flags.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @param attr Encapsulate OH_AVCodecBufferAttr structure instance pointer, please refer to
 * {@link OH_AVCodecBufferAttr}
 * @return Function result code.
 *         {@link AV_ERR_OK} if the execution is successful.
 *         {@link AV_ERR_INVALID_VAL}
 *         1. input buffer or attr is NULL;
 *         2. structure verification failed of the buffer.
 * @since 11
 */
OH_AVErrCode OH_AVBuffer_GetBufferAttr(OH_AVBuffer *buffer, OH_AVCodecBufferAttr *attr);

/**
 * @brief Set the buffer's attribute, such as pts, size, offset, and flags.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @param attr Encapsulate OH_AVCodecBufferAttr structure instance pointer, please refer to
 * {@link OH_AVCodecBufferAttr}
 * @return Function result code.
 *         {@link AV_ERR_OK} if the execution is successful.
 *         {@link AV_ERR_INVALID_VAL}
 *         1. input buffer or attr is NULL;
 *         2. structure verification failed of the buffer;
 *         3. the size or offset of input buffer's memory is invalid.
 * @since 11
 */
OH_AVErrCode OH_AVBuffer_SetBufferAttr(OH_AVBuffer *buffer, const OH_AVCodecBufferAttr *attr);

/**
 * @brief Get parameters except basic attributes, the information is carried in OH_AVFormat.
 * It should be noted that the OH_AVFormat instance pointed to
 * by the return value * needs to be released by {@link OH_AVFormat_Destroy}.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @return Returns Encapsulate OH_AVFormat structure instance pointer if the execution is successful,
 * otherwise returns NULL. Possible failure causes:
 * 1. input buffer is NULL;
 * 2. structure verification failed of the buffer;
 * 3. buffer's meta is NULL.
 * @since 11
 */
OH_AVFormat *OH_AVBuffer_GetParameter(OH_AVBuffer *buffer);

/**
 * @brief Set parameters except basic attributes, the information is carried in OH_AVFormat.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @param format Encapsulate OH_AVFormat structure instance pointer
 * @return Function result code.
 *         {@link AV_ERR_OK} if the execution is successful.
 *         {@link AV_ERR_INVALID_VAL}
 *         1. input buffer or format is NULL;
 *         2. structure verification failed of the buffer;
 *         3. input meta is NULL.
 * @since 11
 */
OH_AVErrCode OH_AVBuffer_SetParameter(OH_AVBuffer *buffer, const OH_AVFormat *format);

/**
 * @brief Get the buffer's virtual address.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @return the buffer's virtual address if the buffer is valid, otherwise NULL
 * Possible failure causes:
 * 1. input buffer is NULL;
 * 2. structure verification failed of the OH_AVBuffer;
 * 3. internal error has occurred.
 * @since 11
 */
uint8_t *OH_AVBuffer_GetAddr(OH_AVBuffer *buffer);

/**
 * @brief Get the buffer's capacity(byte).
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @return The buffer's capacity if the buffer is valid, otherwise -1
 * Possible failure causes:
 * 1. input buffer is NULL;
 * 2. structure verification failed of the OH_AVBuffer;
 * 3. internal error has occurred.
 * @since 11
 */
int32_t OH_AVBuffer_GetCapacity(OH_AVBuffer *buffer);

/**
 * @brief Get the OH_NativeBuffer instance pointer. It should be noted that the OH_AVBuffer
 * instance pointed to by the return value * needs to be released by {@link OH_NativeBuffer_Unreference}.
 * @syscap SystemCapability.Multimedia.Media.Core
 * @param buffer Encapsulate OH_AVBuffer structure instance pointer
 * @return Returns Encapsulate OH_NativeBuffer structure instance pointer is successful, otherwise returns NULL
 * Possible failure causes:
 * 1. input buffer is NULL;
 * 2. structure verification failed of the OH_AVBuffer;
 * 3. internal error has occurred.
 * @since 11
 */
OH_NativeBuffer *OH_AVBuffer_GetNativeBuffer(OH_AVBuffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AVBUFFER_H
/** @} */