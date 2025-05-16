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

#ifndef FDSAN_FD_H
#define FDSAN_FD_H

#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

namespace OHOS {
namespace Media {

class FdsanFd {
public:
    FdsanFd() = default;
    explicit FdsanFd(int32_t fd);
    ~FdsanFd();
    FdsanFd(const FdsanFd &copy) = delete;
    FdsanFd(FdsanFd &&move);
    FdsanFd &operator=(const FdsanFd &copy) = delete;
    FdsanFd &operator=(FdsanFd &&move);

    int32_t Get() const;
    void Reset(int32_t newFd = INVALID_FD);

private:
    static void ExchangeTag(int32_t fd, uint64_t oldTag, uint64_t newTag);
    static int32_t Close(int32_t fd, uint64_t tag);
    // Use the address of object as the file tag
    uint64_t Tag();
    int32_t fd_ = INVALID_FD;
    static constexpr int32_t INVALID_FD = -1;
};
}  // namespace Media
}  // namespace OHOS
#endif  // FDSAN_FD_H