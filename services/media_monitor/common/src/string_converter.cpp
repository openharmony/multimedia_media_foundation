/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "StringConverter"

#include "foundation/utils/string_converter.h"
#include <charconv>

namespace OHOS {
namespace Media {

template <typename T>
bool StringConverter(const std::string &str, T &result)
{
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    return ec == std::errc{} && ptr == str.data() + str.size();
}

template bool StringConverter(const std::string &str, int64_t &result);
template bool StringConverter(const std::string &str, int32_t &result);
template bool StringConverter(const std::string &str, uint64_t &result);
template bool StringConverter(const std::string &str, uint32_t &result);
template bool StringConverter(const std::string &str, uint8_t &result);

} // namespace Media
} // namespace OHOS
