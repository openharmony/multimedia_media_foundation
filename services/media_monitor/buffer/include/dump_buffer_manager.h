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

#ifndef DUMP_BUFFER_MANAGER_H
#define DUMP_BUFFER_MANAGER_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DumpBuffer DumpBuffer;

DumpBuffer *DumpBufferNew(void);

DumpBuffer *DumpBufferCreate(int32_t capacity);

void DumpBufferDestroy(DumpBuffer *buffer);

uint8_t *DumpBufferGetAddr(DumpBuffer *buffer);

int32_t DumpBufferGetCapacity(DumpBuffer *buffer);

int32_t DumpBufferGetSize(DumpBuffer *buffer);

bool DumpBufferSetSize(DumpBuffer *buffer, int32_t size);

int32_t DumpBufferWrite(DumpBuffer *buffer, const uint8_t *in, int32_t writeSize);

uint64_t DumpBufferGetUniqueId(DumpBuffer *buffer);

bool DumpBufferReadFromParcel(DumpBuffer *buffer, void *parcel);

bool DumpBufferWriteToParcel(DumpBuffer *buffer, void *parcel);
#ifdef __cplusplus
}
#endif
#endif // DUMP_BUFFER_MANAGER_H