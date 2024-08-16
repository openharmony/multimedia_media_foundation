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

#include "dump_buffer_manager.h"
#include "dump_buffer_define.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer" };
}

using namespace OHOS;
using namespace OHOS::Media;

DumpBuffer *DumpBufferNew(void)
{
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer();
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "new AVBuffer failed");
    struct DumpBuffer *buf = new (std::nothrow) DumpBuffer(buffer);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "failed to new DumpBuffer");
    return buf;
}

DumpBuffer *DumpBufferCreate(int32_t capacity)
{
    FALSE_RETURN_V_MSG_E(capacity > 0, nullptr, "capacity %{public}d is error!", capacity);
    AVBufferConfig avBufferConfig;
    avBufferConfig.size = capacity;
    avBufferConfig.memoryType = MemoryType::SHARED_MEMORY;
    avBufferConfig.memoryFlag = MemoryFlag::MEMORY_READ_WRITE;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(avBufferConfig);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "create AVBuffer failed");
    FALSE_RETURN_V_MSG_E(buffer->memory_ != nullptr, nullptr, "memory is nullptr");
    FALSE_RETURN_V_MSG_E(buffer->memory_->GetAddr() != nullptr, nullptr, "memory's addr is nullptr");

    struct DumpBuffer *buf = new (std::nothrow) DumpBuffer(buffer);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "failed to new DumpBuffer");
    return buf;
}

void DumpBufferDestroy(struct DumpBuffer *buffer)
{
    FALSE_RETURN_MSG(buffer != nullptr, "input buffer is nullptr!");
    delete buffer;
    return;
}

uint8_t *DumpBufferGetAddr(DumpBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, nullptr, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, nullptr, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->GetAddr();
}

int32_t DumpBufferGetCapacity(DumpBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, -1, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, -1, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, -1, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->GetCapacity();
}

int32_t DumpBufferGetSize(DumpBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, -1, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, -1, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, -1, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->GetSize();
}

bool DumpBufferSetSize(DumpBuffer *buffer, int32_t size)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, false, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, false, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, false, "buffer's memory is nullptr!");
    buffer->buffer_->memory_->SetSize(size);
    return true;
}

int32_t DumpBufferWrite(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, -1, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, -1, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, -1, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->Write(in, writeSize, 0);
}

uint64_t DumpBufferGetUniqueId(DumpBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, 0, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, 0, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, 0, "buffer's memory is nullptr!");
    return buffer->buffer_->GetUniqueId();
}

bool DumpBufferReadFromParcel(DumpBuffer *buffer, void *parcel)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, false, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(parcel != nullptr, false, "input parcel is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, false, "buffer is nullptr!");
    MessageParcel *parcelIn = static_cast<MessageParcel *>(parcel);
    bool ret = buffer->buffer_->ReadFromMessageParcel(*parcelIn);
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, false, "buffer's memory is nullptr!");
    return ret;
}

bool DumpBufferWriteToParcel(DumpBuffer *buffer, void *parcel)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, false, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(parcel != nullptr, false, "input parcel is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, false, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, false, "buffer's memory is nullptr!");
    MessageParcel *parcelIn = static_cast<MessageParcel *>(parcel);
    bool ret = buffer->buffer_->WriteToMessageParcel(*parcelIn);
    return ret;
}

#ifdef __cplusplus
}
#endif