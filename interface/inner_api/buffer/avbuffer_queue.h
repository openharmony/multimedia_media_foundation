/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_AVBUFFER_QUEUE_H
#define HISTREAMER_FOUNDATION_AVBUFFER_QUEUE_H

#include "buffer/avbuffer_queue_define.h"
#include "buffer/avbuffer_queue_producer.h"
#include "buffer/avbuffer_queue_consumer.h"
#include "surface.h"

namespace OHOS {
namespace Media {

class AVBufferQueue {
public:
    static std::shared_ptr<AVBufferQueue> Create(uint32_t size, MemoryType type = MemoryType::UNKNOWN_MEMORY,
        const std::string& name = "", bool disableAlloc = false);
    static std::shared_ptr<AVBufferQueue> CreateAsSurfaceProducer(
            sptr<Surface>& surface, const std::string& name = "");

    AVBufferQueue() = default;
    virtual ~AVBufferQueue() = default;
    AVBufferQueue(const AVBufferQueue&) = delete;

    virtual std::shared_ptr<AVBufferQueueProducer> GetLocalProducer() = 0;
    virtual std::shared_ptr<AVBufferQueueConsumer> GetLocalConsumer() = 0;

    // 跨进程对象智能指针
    virtual sptr<AVBufferQueueProducer> GetProducer() = 0;
    virtual sptr<AVBufferQueueConsumer> GetConsumer() = 0;

    virtual sptr<Surface> GetSurfaceAsProducer() = 0;
    virtual sptr<Surface> GetSurfaceAsConsumer() = 0;

    virtual uint32_t GetQueueSize() = 0;
    virtual Status SetQueueSize(uint32_t size) = 0;
    // allow set larger size to AVBUFFER_QUEUE_MAX_QUEUE_SIZE_FOR_LARGER
    virtual Status SetLargerQueueSize(uint32_t size) = 0;
    virtual bool IsBufferInQueue(const std::shared_ptr<AVBuffer>& buffer) = 0;
    virtual Status Clear() = 0;
    virtual Status ClearBufferIf(std::function<bool(const std::shared_ptr<AVBuffer>&)> pred) = 0;
    virtual Status SetQueueSizeAndAttachBuffer(uint32_t size, std::shared_ptr<AVBuffer>& buffer, bool isFilled);
    virtual uint32_t GetFilledBufferSize()
    {
        return 0;
    }
};

} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_FOUNDATION_AVBUFFER_QUEUE_H
