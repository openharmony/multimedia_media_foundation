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
 * @file native_averrors.h
 *
 * @brief Media framework error code.
 *
 * @kit AVCodecKit
 * @library libnative_media_core.so
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 9
 */

#ifndef NATIVE_AVERRORS_H
#define NATIVE_AVERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AV error code
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 9
 * @version 1.0
 */
typedef enum OH_AVErrCode {
    /**
     * the operation completed successfully.
     */
    AV_ERR_OK = 0,
    /**
     * no memory.
     */
    AV_ERR_NO_MEMORY = 1,
    /**
     * opertation not be permitted.
     */
    AV_ERR_OPERATE_NOT_PERMIT = 2,
    /**
     * invalid argument.
     */
    AV_ERR_INVALID_VAL = 3,
    /**
     * IO error.
     */
    AV_ERR_IO = 4,
    /**
     * network timeout.
     */
    AV_ERR_TIMEOUT = 5,
    /**
     * unknown error.
     */
    AV_ERR_UNKNOWN = 6,
    /**
     * media service died.
     */
    AV_ERR_SERVICE_DIED = 7,
    /**
     * the state is not support this operation.
     */
    AV_ERR_INVALID_STATE = 8,
    /**
     * unsupport interface.
     */
    AV_ERR_UNSUPPORT = 9,
    /**
     * @error input data error.
     * @since 12
     */
    AV_ERR_INPUT_DATA_ERROR = 10,
    /**
     * extend err start.
     */
    AV_ERR_EXTEND_START = 100,
    /**drm error base.
     * @since 12
     */
    AV_ERR_DRM_BASE = 200,
    /** drm decypt failed.
     * @since 12
     */
    AV_ERR_DRM_DECRYPT_FAILED = 201,
    /**
     * @error video error base.
     * @since 12
     */
    AV_ERR_VIDEO_BASE = 300,
    /**
     * @error video unsupported color space conversion.
     * @since 12
     */
    AV_ERR_VIDEO_UNSUPPORTED_COLOR_SPACE_CONVERSION = 301,
    /**
     * @error the address of server is incorrect, and IO can not find host.
     * @since 14
     */
    AV_ERR_IO_CANNOT_FIND_HOST = 5411001,
    /**
     * @error network connection timeout.
     * @since 14
     */
    AV_ERR_IO_CONNECTION_TIMEOUT = 5411002,
    /**
     * @error failed link due to abnormal network.
     * @since 14
     */
    AV_ERR_IO_NETWORK_ABNORMAL = 5411003,
    /**
     * @error failed link due to unavailable network.
     * @since 14
     */
    AV_ERR_IO_NETWORK_UNAVAILABLE = 5411004,
    /**
     * @error network permission dennied.
     * @since 14
     */
    AV_ERR_IO_NO_PERMISSION = 5411005,
    /**
     * @error the client request parameters are incorrect or exceed the processing capacity.
     * @since 14
     */
    AV_ERR_IO_NETWORK_ACCESS_DENIED = 5411006,
    /**
     * @error cannot find available network resources.
     * @since 14
     */
    AV_ERR_IO_RESOURCE_NOT_FOUND = 5411007,
    /**
     * @error the server fails to verify the client certificate because the certificate is not carried,
     *  the certificate is invalid, or the certificate is expired.
     * @since 14
     */
    AV_ERR_IO_SSL_CLIENT_CERT_NEEDED = 5411008,
    /**
     * @error the client fails to verify the server certificate because the certificate is not carried,
     *  the certificate is invalid, or the certificate is expired.
     * @since 14
     */
    AV_ERR_IO_SSL_CONNECT_FAIL = 5411009,
    /**
     * @error IO SSL server cert untrusted.
     * @since 14
     */
    AV_ERR_IO_SSL_SERVER_CERT_UNTRUSTED = 5411010,
    /**
     * @error unsupported request due to network protocols.
     * @since 14
     */
    AV_ERR_IO_UNSUPPORTED_REQUEST = 5411011,
} OH_AVErrCode;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AVERRORS_H
/** @} */
