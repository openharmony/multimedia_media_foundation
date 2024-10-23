/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#ifndef HISTREAMER_NETWORK_TYPES_H
#define HISTREAMER_NETWORK_TYPES_H

#include <string>
#include <map>
#include "common/status.h"
#include "plugin/plugin_event.h"

namespace OHOS {
namespace Media {
namespace Plugins {
namespace HttpPlugin {
using RxBody = size_t(*)(void* buffer, size_t size, size_t nitems, void* userParam);
using RxHeader = size_t(*)(void* buffer, size_t size, size_t nitems, void* userParam);
using RequestCompletedFunc = std::function<void(const Status&)>;
using HandleResponseCbFunc = std::function<void(
    const NetworkClientErrorCode clientCode, const NetworkServerErrorCode serverCode, const Status ret)>;

struct RequestInfo {
    std::string url;
    std::map<std::string, std::string> httpHeader;
    int32_t timeoutMs{-1};
};
} // namespace HttpPlugin
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_NETWORK_TYPES_H
