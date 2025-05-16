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

#include "common/fdsan_fd.h"

namespace OHOS {
namespace Media {

FdsanFd::FdsanFd(int32_t fd)
{
    Reset(fd);
}

FdsanFd::~FdsanFd()
{
    Reset();
}

FdsanFd::FdsanFd(FdsanFd &&move)
{
    *this = std::move(move);
}

FdsanFd &FdsanFd::operator=(FdsanFd &&move)
{
    if (this == &move) {
        return *this;
    }
    Reset();
    if (move.fd_ != INVALID_FD) {
        fd_ = move.fd_;
        move.fd_ = INVALID_FD;
        // Acquire ownership of the presumably unowned fd.
        ExchangeTag(fd_, move.Tag(), Tag());
    }
    return *this;
}

int32_t FdsanFd::Get() const
{
    return fd_;
}

void FdsanFd::Reset(int32_t newFd)
{
    if (fd_ != INVALID_FD) {
        Close(fd_, Tag());
        fd_ = INVALID_FD;
    }
    if (newFd != INVALID_FD) {
        fd_ = newFd;
        // Acquire ownership of the presumably unowned fd.
        ExchangeTag(fd_, 0, Tag());
    }
}

// Use the address of object as the file tag
uint64_t FdsanFd::Tag()
{
    return reinterpret_cast<uint64_t>(this);
}

void FdsanFd::ExchangeTag(int32_t fd, uint64_t oldTag, uint64_t newTag)
{
#if !defined(CROSS_PLATFORM)
    if (&fdsan_exchange_owner_tag) {
        fdsan_exchange_owner_tag(fd, oldTag, newTag);
    }
#endif
}

int32_t FdsanFd::Close(int32_t fd, uint64_t tag)
{
#if !defined(CROSS_PLATFORM)
    if (&fdsan_close_with_tag) {
        return fdsan_close_with_tag(fd, tag);
    }
#else
    return close(fd);
#endif
}

}  // namespace Media
}  // namespace OHOS