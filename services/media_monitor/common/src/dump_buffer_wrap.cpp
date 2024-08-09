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

#include "dump_buffer_wrap.h"
#include <string>
#include <dlfcn.h>
#include "log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {

DumpBufferWrap::DumpBufferWrap()
{
}

DumpBufferWrap::~DumpBufferWrap()
{
    Close();
}

bool DumpBufferWrap::Open()
{
    std::string path = "libmedia_monitor_buffer.z.so";
    handler = dlopen(path.c_str(), RTLD_NOW);
    if (handler == nullptr) {
        return false;
    }

    newFunc = (BufferNewFunc)dlsym(handler, "DumpBufferNew");
    FALSE_RETURN_V_MSG_E(newFunc != nullptr, false, "load error");

    createFunc = (BufferCreateFunc)dlsym(handler, "DumpBufferCreate");
    FALSE_RETURN_V_MSG_E(createFunc != nullptr, false, "load error");

    destroyFunc = (BufferDestroyFunc)dlsym(handler, "DumpBufferDestroy");
    FALSE_RETURN_V_MSG_E(destroyFunc != nullptr, false, "load error");

    getAddrFunc = (BufferGetAddrFunc)dlsym(handler, "DumpBufferGetAddr");
    FALSE_RETURN_V_MSG_E(getAddrFunc != nullptr, false, "load error");

    getSizeFunc = (BufferGetSizeFunc)dlsym(handler, "DumpBufferGetSize");
    FALSE_RETURN_V_MSG_E(getSizeFunc != nullptr, false, "load error");

    setSizeFunc = (BufferSetSizeFunc)dlsym(handler, "DumpBufferSetSize");
    FALSE_RETURN_V_MSG_E(setSizeFunc != nullptr, false, "load error");

    getIdFunc = (BufferGetUniIdFunc)dlsym(handler, "DumpBufferGetUniqueId");
    FALSE_RETURN_V_MSG_E(getIdFunc != nullptr, false, "load error");

    getCapacityFunc = (BufferGetCapacityFunc)dlsym(handler, "DumpBufferGetCapacity");
    FALSE_RETURN_V_MSG_E(getCapacityFunc != nullptr, false, "load error");

    writeFunc = (BufferWriteFunc)dlsym(handler, "DumpBufferWrite");
    FALSE_RETURN_V_MSG_E(writeFunc != nullptr, false, "load error");

    readFromParcel = (BufferReadFromParcelFunc)dlsym(handler, "DumpBufferReadFromParcel");
    FALSE_RETURN_V_MSG_E(readFromParcel != nullptr, false, "load error");

    writeToParcel = (BufferWriteToParcelFunc)dlsym(handler, "DumpBufferWriteToParcel");
    FALSE_RETURN_V_MSG_E(writeToParcel != nullptr, false, "load error");
    return true;
}

void DumpBufferWrap::Close()
{
    if (handler != nullptr) {
        dlclose(handler);
        handler = nullptr;
    }
}

DumpBuffer *DumpBufferWrap::NewDumpBuffer()
{
    DumpBuffer* buffer = nullptr;
    if (newFunc != nullptr) {
        buffer = newFunc();
    }
    return buffer;
}

DumpBuffer *DumpBufferWrap::CreateDumpBuffer(int32_t capacity)
{
    DumpBuffer* buffer = nullptr;
    if (createFunc != nullptr) {
        buffer = createFunc(capacity);
    }
    return buffer;
}

void DumpBufferWrap::DestroyDumpBuffer(DumpBuffer *buffer)
{
    if (destroyFunc != nullptr) {
        destroyFunc(buffer);
    }
}

uint8_t *DumpBufferWrap::GetAddr(DumpBuffer *buffer)
{
    uint8_t *addr = nullptr;
    if (getAddrFunc != nullptr) {
        addr = getAddrFunc(buffer);
    }
    return addr;
}

int32_t DumpBufferWrap::GetCapacity(DumpBuffer *buffer)
{
    int32_t size = -1;
    if (getCapacityFunc != nullptr) {
        size = getCapacityFunc(buffer);
    }
    return size;
}

int32_t DumpBufferWrap::GetSize(DumpBuffer *buffer)
{
    int32_t size = -1;
    if (getSizeFunc != nullptr) {
        size = getSizeFunc(buffer);
    }
    return size;
}

int32_t DumpBufferWrap::Write(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize)
{
    int32_t size = -1;
    if (writeFunc != nullptr) {
        size = writeFunc(buffer, in, writeSize);
    }
    return size;
}

uint64_t DumpBufferWrap::GetUniqueId(DumpBuffer *buffer)
{
    uint64_t id = -1;
    if (getIdFunc != nullptr) {
        id = getIdFunc(buffer);
    }
    return id;
}

bool DumpBufferWrap::SetSize(DumpBuffer *buffer, int32_t size)
{
    bool ret = false;
    if (setSizeFunc != nullptr) {
        ret = setSizeFunc(buffer, size);
    }
    return ret;
}

bool DumpBufferWrap::ReadFromParcel(DumpBuffer *buffer, void *parcel)
{
    bool ret = false;
    if (readFromParcel != nullptr) {
        ret = readFromParcel(buffer, parcel);
    }
    return ret;
}

bool DumpBufferWrap::WriteToParcel(DumpBuffer *buffer, void *parcel)
{
    bool ret = false;
    if (writeToParcel != nullptr) {
        ret = writeToParcel(buffer, parcel);
    }
    return ret;
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS