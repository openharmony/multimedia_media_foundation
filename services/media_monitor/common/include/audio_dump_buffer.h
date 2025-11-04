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

#ifndef AUDIO_DUMP_BUFFER_H
#define AUDIO_DUMP_BUFFER_H
#include "parcel.h"
#include "message_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

constexpr int32_t MAX_RAW_DATA_SIZE = 1024 * 1024;
struct AudioDumpBuffer : public OHOS::Parcelable {
    uint32_t size = 0;
    const void *data = nullptr;
    AudioDumpBuffer() {};
    virtual ~AudioDumpBuffer()
    {
        if (data != nullptr) {
            delete[] static_cast<uint8_t*>(const_cast<void*>(data));
            data = nullptr;
        }
    }

    int32_t RawDataCpy(const void *readData)
    {
        if (size == 0 || size > MAX_RAW_DATA_SIZE || readData == nullptr) {
            return -1;
        }
        uint8_t *buffer = new (std::nothrow) uint8_t[size];
        if (buffer == nullptr) {
            return -1;
        }

        auto ret = memcpy_s(buffer, size, readData, size);
        if (ret != 0) {
            delete [] buffer;
            return -1;
        }
        if (data != nullptr) {
            delete[] static_cast<uint8_t*>(const_cast<void*>(data));
            data = nullptr;
        }

        data = reinterpret_cast<void*>(buffer);
        return 0;
    }

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        OHOS::MessageParcel *parcelIn = static_cast<OHOS::MessageParcel*>(&parcel);
        if (data == nullptr) {
            return false;
        }

        return parcelIn->WriteRawData(data, size);
    }
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // AUDIO_DUMP_BUFFER_H