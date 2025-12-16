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

#include "dump_buffer_define.h"
#include "log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer" };
}


DumpBuffer::DumpBuffer()
{
    buffer_ = std::make_shared<OHOS::Media::AVBuffer>();
}

DumpBuffer::DumpBuffer(const std::shared_ptr<OHOS::Media::AVBuffer> &buffer)
    : buffer_(buffer)
{
}

DumpBuffer::~DumpBuffer()
{
    buffer_ = nullptr;
}

bool DumpBuffer::Marshalling(OHOS::Parcel &parcel) const
{
    OHOS::MessageParcel *parcelIn = static_cast<OHOS::MessageParcel*>(&parcel);
    FALSE_RETURN_V(buffer_ != nullptr, false);
    bool ret = buffer_->WriteToMessageParcel(*parcelIn);
    return ret;
}

DumpBuffer *DumpBuffer::Unmarshalling(OHOS::Parcel &data)
{
    std::shared_ptr<OHOS::Media::AVBuffer> buffer = OHOS::Media::AVBuffer::CreateAVBuffer();
    FALSE_RETURN_V(buffer != nullptr, nullptr);
    struct DumpBuffer *dumpBuffer = new (std::nothrow) DumpBuffer(buffer);
    FALSE_RETURN_V(dumpBuffer != nullptr, nullptr);

    OHOS::MessageParcel *parcelIn = static_cast<OHOS::MessageParcel*>(&data);
    dumpBuffer->buffer_->ReadFromMessageParcel(*parcelIn);
    return dumpBuffer;
}