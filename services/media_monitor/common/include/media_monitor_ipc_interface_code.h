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
#ifndef I_MEDIA_MONITOR_INTERFACE_H
#define I_MEDIA_MONITOR_INTERFACE_H
namespace OHOS {
namespace Media {
namespace MediaMonitor {

enum class MediaMonitorInterfaceCode {
    WRITE_LOG_MSG,
    GET_AUDIO_ROUTE_MSG,
    SET_MEDIA_PARAMS,
    GET_INPUT_BUFFER,
    INPUT_BUFFER_FILL,
    GET_DUMP_STATUS,
    ERASE_PREFERRED_DEVICE,
    MEDIA_MONITOR_CODE_MAX = ERASE_PREFERRED_DEVICE,
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MEDIA_MONITOR_INTERFACE_H