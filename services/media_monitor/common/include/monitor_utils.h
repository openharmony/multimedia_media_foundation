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

#ifndef I_MONITOR_UTILS_H
#define I_MONITOR_UTILS_H
#include <string>

namespace OHOS {
namespace Media {
namespace MediaMonitor {

const std::string COMMERCIAL_VERSION = "commercial";
const std::string BETA_VERSION = "beta";
const std::string DEFAULT_DUMP_TYPE = "R_AND_D";
const std::string BETA_DUMP_TYPE = "BETA";

bool IsRealPath(const std::string& inputPath);

class TimeUtils {
public:
    static uint64_t GetCurSec();
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS

#endif // I_MONITOR_UTILS_H