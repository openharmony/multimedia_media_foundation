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

#include "av_hardware_memory.h"
#include "av_shared_memory_ext.h"
#include "av_surface_memory.h"
#include "av_virtual_memory.h"
#include "buffer/avallocator.h"
#include "buffer/avbuffer.h"
#include "common/log.h"
#include "common/status.h"
#include "message_parcel.h"
#include "securec.h"
#include "surface_buffer.h"

namespace {
uint64_t GetUniqueId()
{
#ifdef MEDIA_OHOS
    using namespace std::chrono;
    static const uint64_t startTime =
        static_cast<uint64_t>(time_point_cast<seconds>(system_clock::now()).time_since_epoch().count());
    static const uint16_t processId = static_cast<uint16_t>(getpid());
#else
    static const uint64_t startTime = 0;
    static const uint16_t processId = 0;
#endif
    static std::atomic<uint32_t> bufferId = 0;
    if (bufferId == UINT32_MAX) {
        bufferId = 0;
    }
    union UniqueId {
        uint64_t startTime;    //  1--16, 16: time
        uint16_t processId[4]; // 17--32, 16: process id
        uint32_t bufferId[2];  // 33--64, 32: atomic val
    } uid = {.startTime = startTime};
    ++bufferId;
    uid.processId[1] = processId;
    uid.bufferId[1] = bufferId;
    return uid.startTime;
}

} // namespace
namespace OHOS {
namespace Media {
std::shared_ptr<AVMemory> AVMemory::CreateAVMemory(std::shared_ptr<AVAllocator> allocator, int32_t capacity,
                                                   int32_t align)
{
    MemoryType type = allocator->GetMemoryType();
    std::shared_ptr<AVMemory> mem = nullptr;
    switch (type) {
        case MemoryType::VIRTUAL_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVVirtualMemory());
            break;
        }
        case MemoryType::SURFACE_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVSurfaceMemory());
            break;
        }
        case MemoryType::SHARED_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVSharedMemoryExt());
            break;
        }
        case MemoryType::HARDWARE_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVHardwareMemory());
            break;
        }
        default:
            break;
    }
    FALSE_RETURN_V_MSG_E(mem != nullptr, nullptr, "Create AVMemory failed, no memory");

    mem->allocator_ = allocator;
    mem->capacity_ = capacity;
    mem->align_ = align;
    Status ret = mem->Init();
    FALSE_RETURN_V_MSG_E(ret == Status::OK, nullptr, "Init AVMemory failed, uid:" PUBLIC_LOG_U64, mem->uid_);
    return mem;
}

std::shared_ptr<AVMemory> AVMemory::CreateAVMemory(uint8_t *ptr, int32_t capacity, int32_t size)
{
    std::shared_ptr<AVMemory> mem = std::shared_ptr<AVMemory>(new AVVirtualMemory());
    FALSE_RETURN_V_MSG_E(mem != nullptr, nullptr, "Create AVVirtualMemory failed, no memory");
    mem->allocator_ = nullptr;
    mem->capacity_ = capacity;
    mem->size_ = size;
    mem->base_ = ptr;
    return mem;
}

std::shared_ptr<AVMemory> AVMemory::CreateAVMemory(MessageParcel &parcel, bool isSurfaceBuffer)
{
#ifdef MEDIA_OHOS
    if (isSurfaceBuffer) {
        auto mem = std::shared_ptr<AVMemory>(new AVSurfaceMemory());
        Status ret = mem->InitSurfaceBuffer(parcel);
        FALSE_RETURN_V_MSG_E(ret == Status::OK, nullptr, "Init AVSurfaceMemory failed");
        return mem;
    }
    MemoryType type = static_cast<MemoryType>(parcel.ReadUint8());
    std::shared_ptr<AVMemory> mem = nullptr;
    switch (type) {
        case MemoryType::VIRTUAL_MEMORY: {
            return nullptr;
        }
        case MemoryType::SURFACE_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVSurfaceMemory());
            break;
        }
        case MemoryType::SHARED_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVSharedMemoryExt());
            break;
        }
        case MemoryType::HARDWARE_MEMORY: {
            mem = std::shared_ptr<AVMemory>(new AVHardwareMemory());
            break;
        }
        default:
            break;
    }

    FALSE_RETURN_V_MSG_E(mem != nullptr, nullptr, "Create AVMemory failed, no memory");
    bool isReadParcel = mem->ReadCommonFromMessageParcel(parcel);
    FALSE_RETURN_V_MSG_E(isReadParcel == true, nullptr, "Read common memory info from parcel failed");

    Status ret = mem->Init(parcel);
    FALSE_RETURN_V_MSG_E(ret == Status::OK, nullptr, "Init AVMemory failed, uid:" PUBLIC_LOG_U64, mem->uid_);
    return mem;
#else
    return nullptr;
#endif
}

std::shared_ptr<AVMemory> AVMemory::CreateAVMemory(sptr<SurfaceBuffer> surfaceBuffer)
{
    auto mem = std::shared_ptr<AVMemory>(new AVSurfaceMemory());
    Status ret = mem->InitSurfaceBuffer(surfaceBuffer);
    FALSE_RETURN_V_MSG_E(ret == Status::OK, nullptr, "Init AVSurfaceMemory failed");
    return mem;
}

AVMemory::AVMemory() : align_(0), offset_(0), size_(0), base_(nullptr), uid_(GetUniqueId()), allocator_(nullptr)
{
    MEDIA_LOG_DD("enter ctor, instance:0x%{public}06" PRIXPTR ", uid:" PUBLIC_LOG_U64, FAKE_POINTER(this), uid_);
}

AVMemory::~AVMemory()
{
    MEDIA_LOG_DD("enter dtor, instance:0x%{public}06" PRIXPTR ", uid:" PUBLIC_LOG_U64, FAKE_POINTER(this), uid_);
}

Status AVMemory::Init()
{
    return Status::ERROR_UNIMPLEMENTED;
}

Status AVMemory::Init(MessageParcel &parcel)
{
    (void)parcel;
    return Status::ERROR_UNIMPLEMENTED;
}

Status AVMemory::InitSurfaceBuffer(MessageParcel &parcel)
{
    (void)parcel;
    return Status::ERROR_UNIMPLEMENTED;
}

Status AVMemory::InitSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer)
{
    (void)surfaceBuffer;
    return Status::ERROR_UNIMPLEMENTED;
}

bool AVMemory::ReadFromMessageParcel(MessageParcel &parcel)
{
    (void)parcel;
    return false;
}

bool AVMemory::WriteToMessageParcel(MessageParcel &parcel)
{
    (void)parcel;
    return false;
}

bool AVMemory::ReadCommonFromMessageParcel(MessageParcel &parcel)
{
#ifdef MEDIA_OHOS
    (void)parcel.ReadUint64();
    int32_t capacity = -1;
    int32_t align = -1;
    int32_t offset = -1;
    int32_t size = -1;

    bool ret = parcel.ReadInt32(capacity);
    FALSE_RETURN_V_MSG_E(ret && (capacity >= 0), false, "capacity is invalid");

    ret = parcel.ReadInt32(align);
    FALSE_RETURN_V_MSG_E(ret && (capacity >= align) && (align >= 0), false, "align is invalid");

    ret = parcel.ReadInt32(offset);
    FALSE_RETURN_V_MSG_E(ret && (capacity >= offset) && (offset >= 0), false, "offset is invalid");

    ret = parcel.ReadInt32(size);
    FALSE_RETURN_V_MSG_E(ret && (capacity >= size) && (size >= 0), false, "size is invalid");

    capacity_ = capacity;
    align_ = align;
    offset_ = offset;
    size_ = size;
    return true;
#else
    return true;
#endif
}

bool AVMemory::SkipCommonFromMessageParcel(MessageParcel &parcel)
{
#ifdef MEDIA_OHOS
    uint64_t skipSize = 0;
    bool ret = parcel.ReadUint64(skipSize);
    FALSE_RETURN_V_MSG_E(ret, false, "unknown parcel");
    parcel.SkipBytes(static_cast<size_t>(skipSize) - 2 * sizeof(int32_t)); // 2: the size of size_ and offset_

    int32_t size = -1;
    int32_t offset = -1;

    ret = parcel.ReadInt32(offset);
    FALSE_RETURN_V_MSG_E(ret && (capacity_ >= offset) && (offset >= 0), false, "offset is invalid");

    ret = parcel.ReadInt32(size);
    FALSE_RETURN_V_MSG_E(ret && (capacity_ >= size) && (size >= 0), false, "size is invalid");

    size_ = size;
    offset_ = offset;
    return true;
#else
    return true;
#endif
}

bool AVMemory::WriteCommonToMessageParcel(MessageParcel &parcel)
{
#ifdef MEDIA_OHOS
    bool ret = true;
    MessageParcel bufferParcel;
    ret = bufferParcel.WriteInt32(capacity_) && bufferParcel.WriteInt32(align_) && bufferParcel.WriteInt32(offset_) &&
          bufferParcel.WriteInt32(size_);

    size_t size = bufferParcel.GetDataSize();
    return ret && parcel.WriteUint64(static_cast<uint64_t>(size)) && parcel.Append(bufferParcel);
#endif
    return true;
}

MemoryType AVMemory::GetMemoryType()
{
    return MemoryType::VIRTUAL_MEMORY;
}

MemoryFlag AVMemory::GetMemoryFlag()
{
    return MemoryFlag::MEMORY_READ_WRITE;
}

int32_t AVMemory::GetCapacity()
{
    return capacity_;
}

int32_t AVMemory::GetSize()
{
    return size_;
}

Status AVMemory::SetSize(int32_t size)
{
    FALSE_RETURN_V_MSG_E((capacity_ >= size) && (size >= 0), Status::ERROR_INVALID_PARAMETER,
                         "size out of range, "
                         "current size:%{public}d , capacity:%{public}d",
                         size_, capacity_);
    size_ = size;
    return Status::OK;
}

int32_t AVMemory::GetOffset()
{
    return offset_;
}

Status AVMemory::SetOffset(int32_t offset)
{
    FALSE_RETURN_V_MSG_E((capacity_ >= offset) && (offset >= 0), Status::ERROR_INVALID_PARAMETER,
                         "offset out of range, "
                         "current offset:%{public}d , capacity:%{public}d",
                         offset_, capacity_);
    offset_ = offset;
    return Status::OK;
}

uint8_t *AVMemory::GetAddr()
{
    return base_;
}

int32_t AVMemory::GetFileDescriptor()
{
    return -1;
}

int32_t AVMemory::Write(const uint8_t *in, int32_t writeSize, int32_t position)
{
    FALSE_RETURN_V_MSG_E(in != nullptr, 0, "Input buffer is nullptr");
    FALSE_RETURN_V_MSG_E(writeSize > 0, 0, "Input writeSize:%{public}d is invalid", writeSize);
    FALSE_RETURN_V_MSG_E((GetMemoryFlag() & MemoryFlag::MEMORY_WRITE_ONLY) != 0, 0, "Lack write permission");
    uint8_t *addr = GetAddr();
    FALSE_RETURN_V_MSG_E(addr != nullptr, 0, "Base buffer is nullptr");
    int32_t start = 0;
    if (position <= INVALID_POSITION) {
        start = size_;
    } else {
        start = std::min(position, capacity_);
    }
    int32_t unusedSize = capacity_ - start;
    int32_t length = std::min(writeSize, unusedSize);
    FALSE_RETURN_V_MSG_E((length + start) <= capacity_, 0,
                         "Write out of bounds, length:%{public}d , start:%{public}d , capacity:%{public}d", length,
                         start, capacity_);
    uint8_t *dstPtr = addr + start;
    FALSE_RETURN_V_MSG_E(dstPtr != nullptr, 0, "Inner dstPtr is nullptr");
    auto error = memcpy_s(dstPtr, length, in, length);
    FALSE_RETURN_V_MSG_E(error == EOK, 0, "Inner memcpy_s failed, uid:" PUBLIC_LOG_U64 ", %{public}s", uid_,
                         strerror(error));
    size_ = start + length;
    return length;
}

int32_t AVMemory::Read(uint8_t *out, int32_t readSize, int32_t position)
{
    FALSE_RETURN_V_MSG_E(out != nullptr, 0, "Output buffer is nullptr");
    FALSE_RETURN_V_MSG_E(readSize > 0, 0, "Output readSize:%{public}d is invalid", readSize);
    FALSE_RETURN_V_MSG_E((GetMemoryFlag() & MemoryFlag::MEMORY_READ_ONLY) != 0, 0, "Lack read permission");
    uint8_t *addr = GetAddr();
    FALSE_RETURN_V_MSG_E(addr != nullptr, 0, "Base buffer is nullptr");
    int32_t start = 0;
    int32_t maxLength = size_;
    if (position > INVALID_POSITION) {
        start = std::min(position, size_);
        maxLength = size_ - start;
    }
    int32_t length = std::min(readSize, maxLength);
    FALSE_RETURN_V_MSG_E((length + start) <= capacity_, 0,
                         "Read out of bounds, length:%{public}d, start:%{public}d, capacity:%{public}d", length, start,
                         capacity_);
    uint8_t *srcPtr = addr + start;
    FALSE_RETURN_V_MSG_E(srcPtr != nullptr, 0, "Inner srcPtr is nullptr");
    auto error = memcpy_s(out, length, srcPtr, length);
    FALSE_RETURN_V_MSG_E(error == EOK, 0, "Inner memcpy_s failed, uid:%" PUBLIC_LOG_U64 ", %{public}s", uid_,
                         strerror(error));
    return length;
}

void AVMemory::Reset()
{
    size_ = 0;
}

sptr<SurfaceBuffer> AVMemory::GetSurfaceBuffer()
{
    return nullptr;
}
} // namespace Media
} // namespace OHOS