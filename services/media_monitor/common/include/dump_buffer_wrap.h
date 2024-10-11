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

#ifndef DUMP_BUFFER_WRAP_H
#define DUMP_BUFFER_WRAP_H

#include "dump_buffer_manager.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

using BufferNewFunc = DumpBuffer *(*)();
using BufferCreateFunc = DumpBuffer *(*)(int32_t capacity);
using BufferDestroyFunc = void (*)(DumpBuffer *buffer);
using BufferGetAddrFunc = uint8_t *(*)(DumpBuffer *buffer);
using BufferGetCapacityFunc = int32_t (*)(DumpBuffer *buffer);
using BufferGetSizeFunc = int32_t (*)(DumpBuffer *buffer);
using BufferSetSizeFunc = bool (*)(DumpBuffer *buffer, int32_t size);
using BufferGetUniIdFunc = uint64_t (*)(DumpBuffer *buffer);
using BufferWriteFunc = int32_t (*)(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize);
using BufferReadFromParcelFunc = bool (*)(DumpBuffer *buffer, void *parcel);
using BufferWriteToParcelFunc = bool (*)(DumpBuffer *buffer, void *parcel);

class DumpBufferWrap {
public:
    DumpBufferWrap();
    ~DumpBufferWrap();
    bool Open();
    void Close();
    DumpBuffer *NewDumpBuffer();
    DumpBuffer *CreateDumpBuffer(int32_t capacity);
    void DestroyDumpBuffer(DumpBuffer *buffer);
    uint8_t *GetAddr(DumpBuffer *buffer);
    int32_t GetCapacity(DumpBuffer *buffer);
    int32_t GetSize(DumpBuffer *buffer);
    int32_t Write(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize);
    uint64_t GetUniqueId(DumpBuffer *buffer);
    bool SetSize(DumpBuffer *buffer, int32_t size);
    bool ReadFromParcel(DumpBuffer *buffer, void *parcel);
    bool WriteToParcel(DumpBuffer *buffer, void *parcel);
private:
    void *handler = nullptr;
    BufferNewFunc newFunc = nullptr;
    BufferCreateFunc createFunc = nullptr;
    BufferDestroyFunc destroyFunc = nullptr;
    BufferGetAddrFunc getAddrFunc = nullptr;
    BufferSetSizeFunc setSizeFunc = nullptr;
    BufferGetSizeFunc getSizeFunc = nullptr;
    BufferGetUniIdFunc getIdFunc = nullptr;
    BufferWriteFunc writeFunc = nullptr;
    BufferGetCapacityFunc getCapacityFunc = nullptr;
    BufferReadFromParcelFunc readFromParcel = nullptr;
    BufferWriteToParcelFunc writeToParcel = nullptr;
};
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // DUMP_BUFFER_WRAP_H