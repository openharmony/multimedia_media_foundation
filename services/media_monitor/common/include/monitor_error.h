/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MONITOR_ERRORS_H
#define MONITOR_ERRORS_H

#include <cstdint>

namespace OHOS {
namespace Media {
namespace MediaMonitor {

enum MediaMonitorErr : int32_t {
    /** Success */
    SUCCESS = 0,
    /** Fail */
    ERROR = -1,
    /** Status error */
    ERR_ILLEGAL_STATE = -2,
    /** Invalid parameter */
    ERR_INVALID_PARAM = -3,
    /** Early media preparation */
    ERR_EARLY_PREPARE = -4,
    /** Invalid operation */
    ERR_INVALID_OPERATION = -5,
    /** error operation failed */
    ERR_OPERATION_FAILED = -6,
    /** Buffer reading failed */
    ERR_READ_BUFFER = -7,
    /** Buffer writing failed */
    ERR_WRITE_BUFFER = -8,
    /**  Device not started */
    ERR_NOT_STARTED = -9,
    /**  Invalid Device handle */
    ERR_INVALID_HANDLE = -10,
    /**  unsupported operation */
    ERR_NOT_SUPPORTED = -11,
    /**  unsupported device */
    ERR_DEVICE_NOT_SUPPORTED = -12,
    /**  write operation failed */
    ERR_WRITE_FAILED = -13,
    /**  read operation failed */
    ERR_READ_FAILED = -14,
    /**  device init failed */
    ERR_DEVICE_INIT = -15,
    /** Invalid data size that has been read */
    ERR_INVALID_READ = -16,
    /** Invalid data size that has been written */
    ERR_INVALID_WRITE = -17,
    /** set invalid index < 0 */
    ERR_INVALID_INDEX = -18,
    /** focus request denied */
    ERR_FOCUS_DENIED = -19,
    /** incorrect render/capture mode */
    ERR_INCORRECT_MODE = -20,
    /** permission denied */
    ERR_PERMISSION_DENIED = -21,
    /** Memory alloc failed */
    ERR_MEMORY_ALLOC_FAILED = -22,
    /** microphone is disabled by EDM */
    ERR_MICROPHONE_DISABLED_BY_EDM = -23,
    /** system permission denied */
    ERR_SYSTEM_PERMISSION_DENIED = -24,
    /** Unknown error */
    BASE_MONITOR_ERR_OFFSET = -200,
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif  // MONITOR_ERRORS_H
