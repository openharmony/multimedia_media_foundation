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

#include "native_avbuffer.h"
#include <shared_mutex>
#include "common/log.h"
#include "common/native_mfmagic.h"
#include "meta/meta.h"
#include "native_window.h"

using namespace OHOS;
using namespace OHOS::Media;

OH_AVBuffer *OH_AVBuffer_Create(int32_t capacity)
{
    FALSE_RETURN_V_MSG_E(capacity > 0, nullptr, "capacity %{public}d is error!", capacity);
    auto allocator = AVAllocatorFactory::CreateSharedAllocator(MemoryFlag::MEMORY_READ_WRITE);
    FALSE_RETURN_V_MSG_E(allocator != nullptr, nullptr, "create allocator failed");

    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(allocator, capacity);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "create OH_AVBuffer failed");
    FALSE_RETURN_V_MSG_E(buffer->memory_ != nullptr, nullptr, "memory is nullptr");
    FALSE_RETURN_V_MSG_E(buffer->memory_->GetAddr() != nullptr, nullptr, "memory's addr is nullptr");

    struct OH_AVBuffer *buf = new (std::nothrow) OH_AVBuffer(buffer);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "failed to new OH_AVBuffer");
    buf->isUserCreated = true;
    return buf;
}

OH_AVErrCode OH_AVBuffer_Destroy(struct OH_AVBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, AV_ERR_INVALID_VAL, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->isUserCreated, AV_ERR_OPERATE_NOT_PERMIT, "input buffer is not user created!");
    delete buffer;
    return AV_ERR_OK;
}

OH_AVErrCode OH_AVBuffer_GetBufferAttr(OH_AVBuffer *buffer, OH_AVCodecBufferAttr *attr)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, AV_ERR_INVALID_VAL, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, AV_ERR_INVALID_VAL, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(attr != nullptr, AV_ERR_INVALID_VAL, "attr is nullptr!");
    attr->pts = buffer->buffer_->pts_;
    attr->flags = static_cast<uint32_t>(buffer->buffer_->flag_);
    if (buffer->buffer_->memory_ != nullptr) {
        attr->offset = buffer->buffer_->memory_->GetOffset();
        attr->size = buffer->buffer_->memory_->GetSize();
    } else {
        attr->offset = 0;
        attr->size = 0;
    }
    return AV_ERR_OK;
}

OH_AVErrCode OH_AVBuffer_SetBufferAttr(OH_AVBuffer *buffer, const OH_AVCodecBufferAttr *attr)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, AV_ERR_INVALID_VAL, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, AV_ERR_INVALID_VAL, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(attr != nullptr, AV_ERR_INVALID_VAL, "attr is nullptr!");
    buffer->buffer_->pts_ = attr->pts;
    buffer->buffer_->flag_ = attr->flags;

    if (buffer->buffer_->memory_ != nullptr) {
        Status ret = buffer->buffer_->memory_->SetSize(attr->size);
        FALSE_RETURN_V_MSG_E(ret == Status::OK, AV_ERR_INVALID_VAL, "size is invalid!");

        ret = buffer->buffer_->memory_->SetOffset(attr->offset);
        FALSE_RETURN_V_MSG_E(ret == Status::OK, AV_ERR_INVALID_VAL, "offset is invalid!");
    }
    return AV_ERR_OK;
}

OH_AVFormat *OH_AVBuffer_GetParameter(OH_AVBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, nullptr, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, nullptr, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->meta_ != nullptr, nullptr, "buffer's meta is nullptr!");

    OH_AVFormat *avFormat = OH_AVFormat_Create();
    if (!avFormat->format_.SetMeta(buffer->buffer_->meta_)) {
        MEDIA_LOG_E("set meta failed!");
        OH_AVFormat_Destroy(avFormat);
        avFormat = nullptr;
    }
    return avFormat;
}

OH_AVErrCode OH_AVBuffer_SetParameter(OH_AVBuffer *buffer, const OH_AVFormat *format)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, AV_ERR_INVALID_VAL, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, AV_ERR_INVALID_VAL, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(format != nullptr, AV_ERR_INVALID_VAL, "input format is nullptr!");
    FALSE_RETURN_V_MSG_E(format->magic_ == MFMagic::MFMAGIC_FORMAT, AV_ERR_INVALID_VAL, "magic error!");

    auto formatRef = const_cast<OH_AVFormat *>(format);
    std::shared_ptr<Meta> meta = formatRef->format_.GetMeta();
    FALSE_RETURN_V_MSG_E(meta != nullptr, AV_ERR_INVALID_VAL, "input meta is nullptr!!");

    *(buffer->buffer_->meta_) = *(meta);
    return AV_ERR_OK;
}

uint8_t *OH_AVBuffer_GetAddr(OH_AVBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, nullptr, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, nullptr, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, nullptr, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->GetAddr();
}

int32_t OH_AVBuffer_GetCapacity(OH_AVBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, -1, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, -1, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, -1, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, -1, "buffer's memory is nullptr!");
    return buffer->buffer_->memory_->GetCapacity();
}

OH_NativeBuffer *OH_AVBuffer_GetNativeBuffer(OH_AVBuffer *buffer)
{
    FALSE_RETURN_V_MSG_E(buffer != nullptr, nullptr, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->magic_ == MFMagic::MFMAGIC_AVBUFFER, nullptr, "magic error!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_ != nullptr, nullptr, "buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer->buffer_->memory_ != nullptr, nullptr, "buffer's memory is nullptr!");
    sptr<SurfaceBuffer> surfaceBuffer = buffer->buffer_->memory_->GetSurfaceBuffer();
    FALSE_RETURN_V_MSG_E(surfaceBuffer != nullptr, nullptr, "surfaceBuffer is nullptr!");
    surfaceBuffer->IncStrongRef(surfaceBuffer.GetRefPtr());
    return surfaceBuffer->SurfaceBufferToNativeBuffer();
}