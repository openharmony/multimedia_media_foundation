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

#ifndef AVBUFFER_H
#define AVBUFFER_H
 
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "buffer/avallocator.h"
#include "common/status.h"

namespace OHOS {
namespace Media {
class AVBuffer {
public:
    AVBuffer() = default;
    virtual ~AVBuffer() = default;
    MOCK_METHOD(AVBufferConfig&, GetConfig, (), ());
    MOCK_METHOD(uint64_t, GetUniqueId, (), ());
    MOCK_METHOD(bool, WriteToMessageParcel, (MessageParcel& parcel), ());
    MOCK_METHOD(bool, ReadFromMessageParcel, (MessageParcel& parcel, bool isSurfaceBuffer), ());
    static std::shared_ptr<AVBuffer> CreateAVBuffer(const AVBufferConfig &config)
    {
        auto avBuffer = std::make_shared<AVBuffer>();
        avBuffer->config_ = config;
        return avBuffer;
    }
    static std::shared_ptr<AVBuffer> CreateAVBuffer(std::shared_ptr<AVAllocator> allocator, int32_t capacity = 0,
                                                    int32_t align = 0)
    {
        (void)allocator;
        (void)capacity;
        (void)align;
        return std::make_shared<AVBuffer>();
    }
    static std::shared_ptr<AVBuffer> CreateAVBuffer(uint8_t *ptr, int32_t capacity, int32_t size = 0)
    {
        (void)ptr;
        (void)capacity;
        (void)size;
        return std::make_shared<AVBuffer>();
    }
    static std::shared_ptr<AVBuffer> CreateAVBuffer(sptr<SurfaceBuffer> surfaceBuffer)
    {
        (void)surfaceBuffer;
        return std::make_shared<AVBuffer>();
    }
protected:
    using MetaData = std::vector<uint8_t>;
    int64_t pts_;
    int64_t dts_;
    int64_t duration_;
    uint32_t flag_;
    std::shared_ptr<Meta> meta_;
    std::shared_ptr<AVMemory> memory_;
    AVBufferConfig config_;
    MOCK_METHOD(Status, Init, (std::shared_ptr<AVAllocator> allocator, int32_t capacity, int32_t align), ());
    MOCK_METHOD(Status, Init, (uint8_t* ptr, int32_t capacity, int32_t size), ());
    MOCK_METHOD(Status, Init, (sptr<SurfaceBuffer> surfaceBuffer), ());
};
}
}

#endif // AVBUFFER_H