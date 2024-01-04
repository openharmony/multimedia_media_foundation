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

#ifndef MEDIA_FOUNDATION_ERRORS_H
#define MEDIA_FOUNDATION_ERRORS_H

#include <string_view>

namespace OHOS {
namespace Media {
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
};
} // namespace MediaFoundation
} // namespace OHOS
#endif // MEDIA_FOUNDATION_ERRORS_H
