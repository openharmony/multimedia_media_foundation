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
     * @the address of server is incorrect, and IO can not find host.
     * @since 14
     */
    AV_ERR_IO_CANNOT_FIND_HOST = 5411001,
    /**
     * @network connection timeout.
     * @since 14
     */
    AV_ERR_IO_CONNECTION_TIMEOUT = 5411002,
    /**
     * @failed link due to abnormal network.
     * @since 14
     */
    AV_ERR_IO_NETWORK_ABNORMAL = 5411003,
    /**
     * @failed link due to unavailable network.
     * @since 14
     */
    AV_ERR_IO_NETWORK_UNAVAILABLE = 5411004,
    /**
     * @network permission dennied.
     * @since 14
     */
    AV_ERR_IO_NO_PERMISSION = 5411005,
    /**
     * @the client request parameters are incorrect or exceed the processing capacity.
     * @since 14
     */
    AV_ERR_IO_NETWORK_ACCESS_DENIED = 5411006,
    /**
     * @cannot find available network resources.
     * @since 14
     */
    AV_ERR_IO_RESOURE_NOT_FOUND = 5411007,
    /**
     * @the server failS to verify the client certificate because the certificate is not carried,
     *  the certificate is invalid, or the certificate is expired.
     * @since 14
     */
    AV_ERR_IO_SSL_CLIENT_CERT_NEEDED = 5411008,
    /**
     * @the client fails to verify the server certificate because the certificate is not carried,
     *  the certificate is invalid, or the certificate is expired.
     * @since 14
     */
    AV_ERR_IO_SSL_CONNECT_FAIL = 5411009,
    /**
     * @IO SSL server cert untrusted.
     * @since 14
     */
    AV_ERR_IO_SSL_SERVER_CERT_UNTRUSTED = 5411010,
    /**
     * @unsupported request due to network protocols.
     * @since 14
     */
    AV_ERR_IO_UNSUPPORTTED_REQUEST = 5411011,
    /**
     * @file data is incorrect, and no specific data can be provided.
     * @since 14
     */
    AV_ERR_IO_DATA_ABNORMAL = 5411012,
    /**
     * @file is occupied by other processes and cannot be accessed.
     * @since 14
     */
    AV_ERR_IO_FILE_ACCESS_DENIED = 5411013,
    /**
     * @the file handle is incorrect.
     * @since 14
     */
    AV_ERR_IO_FILE_BAD_HANDLE = 5411014,
    /**
     * @IO file not found.
     * @since 14
     */
    AV_ERR_IO_FILE_NOT_FOUND = 5411015,
    /**
     * @IO file permission denied because API does not have permissions.
     * @since 14
     */
    AV_ERR_IO_FILE_PERMISSION_DENIED = 5411016,
    /**
     * @fail to decode the audio data.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEC_FAILED = 5411017,
    /**
     * @fail to init the audio decoder.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEC_INIT_FAILED = 5411018,
    /**
     * @the audio decoder is unavailable for handler incurrupt, or audio decoder subsystem abnormal.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEC_UNAVAILABLE = 5411019,
    /**
     * @audio device error.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEVICE_ERROR = 5411020,
    /**
     * @audio device state does not support the current operation.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEVICE_INVALID_STATE = 5411021,
    /**
     * @audio device timeout when calling audio render interface.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEVICE_TIMEOUT = 5411022,
    /**
     * @audio device unavailable, unable to create an audio device.
     * @since 14
     */
    AV_ERR_IO_AUDIO_DEVICE_UNAVAILABLE = 5411023,
    /**
     * @fail to encode the audio data.
     * @since 14
     */
    AV_ERR_IO_AUDIO_ENC_FAILED = 5411024,
    /**
     * @fail to init the audio encoder.
     * @since 14
     */
    AV_ERR_IO_AUDIO_ENC_INIT_FAILED = 5411025,
    /**
     * @audio encoder is unavailable for handler incurrupt, or audio encoder subsystem abnormal.
     * @since 14
     */
    AV_ERR_IO_AUDIO_ENC_UNAVAILABLE = 5411026,
    /**
     * @fail to decode the video data.
     * @since 14
     */
    AV_ERR_IO_VIDEO_DEC_FAILED = 5411027,
    /**
     * @fail to init the video decoder.
     * @since 14
     */
    AV_ERR_IO_VIDEO_DEC_INIT_FAILED = 5411028,
    /**
     * @video decoder is unavailable for handler incurrupt, or video decoder subsystem abnormal.
     * @since 14
     */
    AV_ERR_IO_VIDEO_DEC_UNAVAILABLE = 5411029,
    /**
     * @video device error.
     * @since 14
     */
    AV_ERR_IO_VIDEO_DEVICE_ERROR = 5411030,
    /**
     * @fail to encode the video data.
     * @since 14
     */
    AV_ERR_IO_VIDEO_ENC_FAILED = 5411031,
    /**
     * @fail to init the video encoder.
     * @since 14
     */
    AV_ERR_IO_VIDEO_ENC_INIT_FAILED = 5411032,
    /**
     * @video encoder is unavailable for handler incurrupt, or video encoder subsystem abnormal.
     * @since 14
     */
    AV_ERR_IO_VIDEO_ENC_UNAVAILABLE = 5411033,
} OH_AVErrCode;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AVERRORS_H
