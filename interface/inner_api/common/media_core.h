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

#ifndef MEDIA_CORE_H
#define MEDIA_CORE_H

#include <map>
#include <string_view>
#include "errors.h"
namespace OHOS {
namespace Media {
using MSErrCode = ErrCode;

// bit 28~21 is subsys, bit 20~16 is Module. bit 15~0 is code
constexpr MSErrCode MS_MODULE = 0X01000;
constexpr MSErrCode MS_ERR_OFFSET = ErrCodeOffset(SUBSYS_MULTIMEDIA, MS_MODULE);
enum MediaServiceErrCode : ErrCode {
    MSERR_OK                = ERR_OK,
    MSERR_NO_MEMORY         = MS_ERR_OFFSET + ENOMEM,   // no memory
    MSERR_INVALID_OPERATION = MS_ERR_OFFSET + ENOSYS,   // opertation not be permitted
    MSERR_INVALID_VAL       = MS_ERR_OFFSET + EINVAL,   // invalid argument
    MSERR_UNKNOWN           = MS_ERR_OFFSET + 0x200,    // unkown error.
    MSERR_MANDATORY_PARAMETER_UNSPECIFIED,              // mandatory parameters are left unspecified
    MSERR_INCORRECT_PARAMETER_TYPE,                     // Incorrect parameter types
    MSERR_PARAMETER_VERIFICATION_FAILED,                // Parameter verification failed
    MSERR_SERVICE_DIED,                                 // media service died
    MSERR_CREATE_REC_ENGINE_FAILED,                     // create recorder engine failed.
    MSERR_CREATE_PLAYER_ENGINE_FAILED,                  // create player engine failed.
    MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,        // create avmetadatahelper engine failed.
    MSERR_CREATE_AVCODEC_ENGINE_FAILED,                 // create avcodec engine failed.
    MSERR_INVALID_STATE,                                // the state is not support this operation.
    MSERR_UNSUPPORT,                                    // unsupport interface.
    MSERR_UNSUPPORT_AUD_SRC_TYPE,                       // unsupport audio source type.
    MSERR_UNSUPPORT_AUD_SAMPLE_RATE,                    // unsupport audio sample rate.
    MSERR_UNSUPPORT_AUD_CHANNEL_NUM,                    // unsupport audio channel.
    MSERR_UNSUPPORT_AUD_ENC_TYPE,                       // unsupport audio encoder type.
    MSERR_UNSUPPORT_AUD_PARAMS,                         // unsupport audio params(other params).
    MSERR_UNSUPPORT_VID_SRC_TYPE,                       // unsupport video source type.
    MSERR_UNSUPPORT_VID_ENC_TYPE,                       // unsupport video encoder type.
    MSERR_UNSUPPORT_VID_PARAMS,                         // unsupport video params(other params).
    MSERR_UNSUPPORT_CONTAINER_TYPE,                     // unsupport container format type.
    MSERR_UNSUPPORT_PROTOCOL_TYPE,                      // unsupport protocol type.
    MSERR_UNSUPPORT_VID_DEC_TYPE,                       // unsupport video decoder type.
    MSERR_UNSUPPORT_AUD_DEC_TYPE,                       // unsupport audio decoder type.
    MSERR_UNSUPPORT_STREAM,                             // internal data stream error.
    MSERR_UNSUPPORT_FILE,                               // this appears to be a text file.
    MSERR_UNSUPPORT_SOURCE,                             // unsupport source type.
    MSERR_AUD_RENDER_FAILED,                            // audio render failed.
    MSERR_AUD_ENC_FAILED,                               // audio encode failed.
    MSERR_VID_ENC_FAILED,                               // video encode failed.
    MSERR_AUD_DEC_FAILED,                               // audio decode failed.
    MSERR_VID_DEC_FAILED,                               // video decode failed.
    MSERR_MUXER_FAILED,                                 // stream avmuxer failed.
    MSERR_DEMUXER_FAILED,                               // stream demuxer or parser failed.
    MSERR_OPEN_FILE_FAILED,                             // open file failed.
    MSERR_FILE_ACCESS_FAILED,                           // read or write file failed.
    MSERR_START_FAILED,                                 // audio/video start failed.
    MSERR_PAUSE_FAILED,                                 // audio/video pause failed.
    MSERR_STOP_FAILED,                                  // audio/video stop failed.
    MSERR_SEEK_FAILED,                                  // audio/video seek failed.
    MSERR_NETWORK_TIMEOUT,                              // network timeout.
    MSERR_NOT_FIND_CONTAINER,                           // not find a demuxer.
    MSERR_DATA_SOURCE_IO_ERROR,                         // media data source IO failed.
    MSERR_DATA_SOURCE_OBTAIN_MEM_ERROR,                 // media data source get mem failed.
    MSERR_DATA_SOURCE_ERROR_UNKNOWN,                    // media data source error unknow.
    MSERR_AUD_INTERRUPT,                                // audio interrupted.
    MSERR_USER_NO_PERMISSION,                           // user permission denied (AccessToken).
    MSERR_DRM_VERIFICATION_FAILED,                      // DRM verification failed
    MSERR_UNSUPPORT_WATER_MARK,                         // unsupported watermark
    MSERR_DEMUXER_BUFFER_NO_MEMORY,                     // demuxer cache data reached its limit
    MSERR_IO_CANNOT_FIND_HOST = MS_ERR_OFFSET + 0x400,  // IO can not find host
    MSERR_IO_CONNECTION_TIMEOUT,                        // IO connection timeout
    MSERR_IO_NETWORK_ABNORMAL,                          // IO network abnormal.
    MSERR_IO_NETWORK_UNAVAILABLE,                       // IO network unavailable.
    MSERR_IO_NO_PERMISSION,                             // IO no permission.
    MSERR_IO_NETWORK_ACCESS_DENIED,                     // IO request denied.
    MSERR_IO_RESOURE_NOT_FOUND,                         // IO resource not found.
    MSERR_IO_SSL_CLIENT_CERT_NEEDED,                    // IO SSL client cert needed.
    MSERR_IO_SSL_CONNECT_FAIL,                          // IO SSL connect fail.
    MSERR_IO_SSL_SERVER_CERT_UNTRUSTED,                 // IO SSL server cert untrusted.
    MSERR_IO_UNSUPPORTTED_REQUEST,                      // IO unsupported request.
    MSERR_IO_DATA_ABNORMAL,                             // IO data abnormal.
    MSERR_IO_AUDIO_DEVICE_ERROR,                        // audio device error.
    MSERR_IO_VIDEO_DEVICE_ERROR,                        // video device error.
    MSERR_EXTEND_START      = MS_ERR_OFFSET + 0xF000,   // extend err start.
};

// media api error code
enum MediaServiceExtErrCode : int32_t {
    MSERR_EXT_OK = 0,
    MSERR_EXT_NO_MEMORY = 1,           // no memory.
    MSERR_EXT_OPERATE_NOT_PERMIT = 2,  // opertation not be permitted.
    MSERR_EXT_INVALID_VAL = 3,         // invalid argument.
    MSERR_EXT_IO = 4,                  // IO error.
    MSERR_EXT_TIMEOUT = 5,             // network timeout.
    MSERR_EXT_UNKNOWN = 6,             // unknown error.
    MSERR_EXT_SERVICE_DIED = 7,        // media service died.
    MSERR_EXT_INVALID_STATE = 8,       // the state is not support this operation.
    MSERR_EXT_UNSUPPORT = 9,           // unsupport interface.
    MSERR_EXT_EXTEND_START = 100,      // extend err start.
};

// media api9 error code
enum MediaServiceExtErrCodeAPI9 : int32_t {
    MSERR_EXT_API9_OK = 0,                          // use for determine error
    MSERR_EXT_API9_NO_PERMISSION = 201,             // permission denied (AccessToken).
    MSERR_EXT_API9_PERMISSION_DENIED = 202,         // permission denied (system API).
    MSERR_EXT_API9_INVALID_PARAMETER = 401,         // invalid parameter.
    MSERR_EXT_API9_UNSUPPORT_CAPABILITY = 801,      // unsupport api.
    MSERR_EXT_API9_NO_MEMORY = 5400101,             // no memory.
    MSERR_EXT_API9_OPERATE_NOT_PERMIT = 5400102,    // opertation not be permitted.
    MSERR_EXT_API9_IO = 5400103,                    // IO error.
    MSERR_EXT_API9_TIMEOUT = 5400104,               // opertate timeout.
    MSERR_EXT_API9_SERVICE_DIED = 5400105,          // media service died.
    MSERR_EXT_API9_UNSUPPORT_FORMAT = 5400106,      // unsupport format.
    MSERR_EXT_API9_AUDIO_INTERRUPTED = 5400107,     // audio interrupted.
    MSERR_EXT_API14_IO_CANNOT_FIND_HOST = 5411001,              // IO can not find host
    MSERR_EXT_API14_IO_CONNECTION_TIMEOUT = 5411002,            // IO connection timeout
    MSERR_EXT_API14_IO_NETWORK_ABNORMAL = 5411003,              // IO network abnormal.
    MSERR_EXT_API14_IO_NETWORK_UNAVAILABLE = 5411004,           // IO network unavailable.
    MSERR_EXT_API14_IO_NO_PERMISSION = 5411005,                 // IO no permission.
    MSERR_EXT_API14_IO_NETWORK_ACCESS_DENIED = 5411006,         // IO request denied.
    MSERR_EXT_API14_IO_RESOURE_NOT_FOUND = 5411007,             // IO resource not found.
    MSERR_EXT_API14_IO_SSL_CLIENT_CERT_NEEDED = 5411008,        // IO SSL client cert needed.
    MSERR_EXT_API14_IO_SSL_CONNECT_FAIL = 5411009,              // IO SSL connect fail.
    MSERR_EXT_API14_IO_SSL_SERVER_CERT_UNTRUSTED = 5411010,     // IO SSL server cert untrusted.
    MSERR_EXT_API14_IO_UNSUPPORTTED_REQUEST = 5411011,          // IO unsupported request.
    MSERR_EXT_API14_IO_DATA_ABNORMAL = 5411012,                 // IO data abnormal.
    MSERR_EXT_API14_IO_AUDIO_DEVICE_ERROR = 5411020,            // audio device error.
    MSERR_EXT_API14_IO_VIDEO_DEVICE_ERROR = 5411030,            // video device error.
};

/**
 *  Media type
 */
enum MediaType : int32_t {
    /**
     * track is audio.
     */
    MEDIA_TYPE_AUD = 0,
    /**
     * track is video.
     */
    MEDIA_TYPE_VID = 1,
    /**
     * track is subtitle.
     */
    MEDIA_TYPE_SUBTITLE = 2,
    /**
     * track is unknown
     */
    MEDIA_TYPE_MAX_COUNT = 3,
};

/**
 * Enumerates the state change reason.
 *
 */
enum StateChangeReason {
    /**
     * audio/video state change by user
     */
    USER = 1,
    /**
     * audio/video state change by system
     */
    BACKGROUND = 2,
};

enum BufferingInfoType : int32_t {
    /* begin to b buffering */
    BUFFERING_START = 1,
    /* end to buffering */
    BUFFERING_END = 2,
    /* buffering percent */
    BUFFERING_PERCENT = 3,
    /* cached duration in milliseconds */
    CACHED_DURATION = 4,
};

enum PlayerSeekMode : int32_t {
    /* sync to keyframes after the time point. */
    SEEK_NEXT_SYNC = 0,
    /* sync to keyframes before the time point. */
    SEEK_PREVIOUS_SYNC,
    /* sync to closest keyframes. */
    SEEK_CLOSEST_SYNC,
    /* seek to frames closest the time point. */
    SEEK_CLOSEST,
    /* seek continously. */
    SEEK_CONTINOUS,
};

enum PlayerSwitchMode : int32_t {
    /* no seek. */
    SWITCH_SMOOTH = 0,
    /* switch to keyframes before the time point. */
    SWITCH_SEGMENT,
    /* switch to frames closest the time point. */
    SWITCH_CLOSEST,
};

/**
 * @brief Enumerates the container format types.
 */
class ContainerFormatType {
public:
    static constexpr std::string_view CFT_MPEG_4A = "m4a";
    static constexpr std::string_view CFT_MPEG_4 = "mp4";
};

/**
 * @brief the struct of geolocation
 *
 * @param latitude float: latitude in degrees. Its value must be in the range [-90, 90].
 * @param longitude float: longitude in degrees. Its value must be in the range [-180, 180].
 */
struct Location {
    int32_t latitude = 0;
    int32_t longitude = 0;
};

struct AVFileDescriptor {
    int32_t fd = 0;
    int64_t offset = 0;
    int64_t length = -1;
};

struct FrameLayerInfo {
    bool isDiscardable = false;
    uint32_t gopId = 0;
    int32_t layer = -1;
};

struct GopLayerInfo {
    uint32_t gopSize = 0;
    uint32_t layerCount = 0;
    std::map<uint8_t, uint32_t> layerFrameNum;
};

} // namespace Media
} // namespace OHOS
#endif // MEDIA_CORE_H
