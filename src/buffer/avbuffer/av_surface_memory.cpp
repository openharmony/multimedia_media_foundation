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

#include "av_surface_memory.h"
#include <unistd.h>
#include "av_surface_allocator.h"
#include "common/log.h"
#include "common/status.h"
#include "message_parcel.h"
#include "scope_guard.h"
#include "surface_buffer.h"
#include "surface_type.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "AVSurfaceMemory" };
}

namespace OHOS {
namespace Media {
std::shared_ptr<AVAllocator> AVAllocatorFactory::CreateSurfaceAllocator(const BufferRequestConfig &config)
{
    auto allocator = std::shared_ptr<AVSurfaceAllocator>(new AVSurfaceAllocator());
    FALSE_RETURN_V_MSG_E(allocator != nullptr, nullptr, "Create AVSurfaceAllocator failed, no memory");
    allocator->config_ = config;
    return allocator;
}

AVSurfaceAllocator::AVSurfaceAllocator()
{
    config_ = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0x0,
        .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
}

void *AVSurfaceAllocator::Alloc(int32_t capacity)
{
    (void)capacity;
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    FALSE_RETURN_V_MSG_E(surfaceBuffer != nullptr, nullptr, "No memory for new SurfaceBuffer!");
    GSError ret = surfaceBuffer->Alloc(config_);
    FALSE_RETURN_V_MSG_E(ret == GSERROR_OK, nullptr, "Surface Buffer Alloc failed, %{public}s",
                         GSErrorStr(ret).c_str());
    surfaceBuffer->IncStrongRef(surfaceBuffer.GetRefPtr());
    return static_cast<void *>(surfaceBuffer.GetRefPtr());
}

bool AVSurfaceAllocator::Free(void *ptr)
{
    FALSE_RETURN_V_MSG_E(ptr != nullptr, false, "ptr is nullptr");

    sptr<SurfaceBuffer> surfaceBuffer = sptr<SurfaceBuffer>(static_cast<SurfaceBuffer *>(ptr));
    surfaceBuffer->DecStrongRef(surfaceBuffer.GetRefPtr());

    MEDIA_LOG_DD("GetSptrRefCount:%{public}d", surfaceBuffer->GetSptrRefCount());
    surfaceBuffer = nullptr;
    return true;
}

MemoryType AVSurfaceAllocator::GetMemoryType()
{
    return MemoryType::SURFACE_MEMORY;
}

AVSurfaceMemory::AVSurfaceMemory() : isFirstFlag_(true) {}

AVSurfaceMemory::~AVSurfaceMemory()
{
    if (base_ != nullptr) {
        base_ = nullptr;
    }
    if (allocator_ != nullptr) {
        bool ret = allocator_->Free(surfaceBuffer_.GetRefPtr());
        FALSE_RETURN_MSG(ret, "Free memory failed, instance: 0x%{public}06" PRIXPTR, FAKE_POINTER(this));
    }
    surfaceBuffer_ = nullptr;
}

Status AVSurfaceMemory::Init()
{
    surfaceBuffer_ = sptr<SurfaceBuffer>(static_cast<SurfaceBuffer *>(allocator_->Alloc(0)));
    FALSE_RETURN_V_MSG_E(surfaceBuffer_ != nullptr, Status::ERROR_NO_MEMORY, "surfaceBuffer_ alloc failed");
    capacity_ = static_cast<int32_t>(surfaceBuffer_->GetSize());

    return Status::OK;
}

Status AVSurfaceMemory::Init(MessageParcel &parcel)
{
    (void)parcel.ReadUint64();
    return InitSurfaceBuffer(parcel);
}

Status AVSurfaceMemory::InitSurfaceBuffer(MessageParcel &parcel)
{
    surfaceBuffer_ = SurfaceBuffer::Create();
    FALSE_RETURN_V_MSG_E(surfaceBuffer_ != nullptr, Status::ERROR_NO_MEMORY, "No memory for new SurfaceBuffer!");

    GSError ret = surfaceBuffer_->ReadFromMessageParcel(parcel);
    FALSE_RETURN_V_MSG_E(ret == GSERROR_OK, Status::ERROR_INVALID_OPERATION, "Read message parcel failed! %{public}s",
                         GSErrorStr(ret).c_str());
    capacity_ = static_cast<int32_t>(surfaceBuffer_->GetSize());
    return Status::OK;
}

Status AVSurfaceMemory::InitSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer)
{
    surfaceBuffer_ = surfaceBuffer;
    capacity_ = static_cast<int32_t>(surfaceBuffer_->GetSize());
    size_ = capacity_;
    return Status::OK;
}

bool AVSurfaceMemory::WriteToMessageParcel(MessageParcel &parcel)
{
#ifdef MEDIA_OHOS
    MessageParcel bufferParcel;
    GSError gsRet = surfaceBuffer_->WriteToMessageParcel(bufferParcel);
    FALSE_RETURN_V_MSG_E(gsRet == GSERROR_OK, false, "Write message parcel failed!, %{public}s",
                         GSErrorStr(gsRet).c_str());
    size_t size = bufferParcel.GetDataSize();
    return parcel.WriteUint64(static_cast<uint64_t>(size)) && parcel.Append(bufferParcel);
#else
    return true;
#endif
}

bool AVSurfaceMemory::ReadFromMessageParcel(MessageParcel &parcel)
{
#ifdef MEDIA_OHOS
    uint64_t size = 0;
    bool ret = parcel.ReadUint64(size);
    parcel.SkipBytes(static_cast<size_t>(size));
    return ret;
#else
    return true;
#endif
}

uint8_t *AVSurfaceMemory::GetAddr()
{
    if (isFirstFlag_) {
        base_ = reinterpret_cast<uint8_t *>(surfaceBuffer_->GetVirAddr());
        isFirstFlag_ = false;
    }
    return base_;
}

MemoryType AVSurfaceMemory::GetMemoryType()
{
    return MemoryType::SURFACE_MEMORY;
}

int32_t AVSurfaceMemory::GetFileDescriptor()
{
    return surfaceBuffer_->GetFileDescriptor();
}

sptr<SurfaceBuffer> AVSurfaceMemory::GetSurfaceBuffer()
{
    return surfaceBuffer_;
}
} // namespace Media
} // namespace OHOS
