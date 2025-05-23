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

#include <climits>
#include <ctime>
#include "log.h"
#include "monitor_utils.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MonitorUtils"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

static constexpr int NANOSECOND_TO_SECOND = 1000000000;

bool IsRealPath(const std::string& inputPath)
{
    if (inputPath.length() > PATH_MAX) {
        MEDIA_LOG_E("invalid path");
        return false;
    }

    char path[PATH_MAX] = {0};
    auto realPath = realpath(inputPath.c_str(), path);
    FALSE_RETURN_V(realPath != nullptr, false);
    return true;
}

uint64_t TimeUtils::GetCurSec()
{
    uint64_t result = -1; // -1 for bad result.
    struct timespec time;
    int ret = clock_gettime(CLOCK_MONOTONIC, &time);
    FALSE_RETURN_V_MSG_E(ret >= 0, result, "GetCurSec fail, result:%{public}d", ret);
    MEDIA_LOG_D("GetCurSec success");
    result = static_cast<uint64_t>((time.tv_sec) + (time.tv_nsec / NANOSECOND_TO_SECOND));
    return result;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS