/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_AVSHAREDMEMORY_H
#define MOCK_AVSHAREDMEMORY_H

#include <memory>
#include <string>
#include "buffer/avsharedmemory.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace Media {
class MockAVSharedMemory : public AVSharedMemory {
public:
    virtual ~MockAVSharedMemory() = default;

    MOCK_CONST_METHOD0(GetBase, uint8_t *());
    MOCK_CONST_METHOD0(GetSize, int32_t());
    MOCK_CONST_METHOD0(GetFlags, uint32_t());
    MOCK_CONST_METHOD0(GetSharedMemoryID, uint64_t());
};
} // namespace Media
} // namespace OHOS
#endif // MOCK_AVSHAREDMEMORY_H