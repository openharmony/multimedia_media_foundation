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

#include "common/native_mfmagic.h"

using namespace OHOS::Media;

OH_AVFormat::OH_AVFormat() : MFObjectMagic(MFMagic::MFMAGIC_FORMAT) {}

OH_AVFormat::OH_AVFormat(const Format &fmt) : MFObjectMagic(MFMagic::MFMAGIC_FORMAT), format_(fmt) {}

OH_AVFormat::~OH_AVFormat()
{
    if (outString_ != nullptr) {
        free(outString_);
        outString_ = nullptr;
    }
    if (dumpInfo_ != nullptr) {
        free(dumpInfo_);
        dumpInfo_ = nullptr;
    }
}

OH_AVMemory::OH_AVMemory(const std::shared_ptr<AVSharedMemory> &mem)
    : MFObjectMagic(MFMagic::MFMAGIC_SHARED_MEMORY), memory_(mem)
{
}

OH_AVMemory::~OH_AVMemory() {}

bool OH_AVMemory::IsEqualMemory(const std::shared_ptr<AVSharedMemory> &mem)
{
    return (mem == memory_) ? true : false;
}

OH_AVBuffer::OH_AVBuffer(const std::shared_ptr<OHOS::Media::AVBuffer> &buffer)
    : MFObjectMagic(MFMagic::MFMAGIC_AVBUFFER), buffer_(buffer)
{
}

bool OH_AVBuffer::IsEqualBuffer(const std::shared_ptr<OHOS::Media::AVBuffer> &buffer)
{
    return (buffer == buffer_);
}
