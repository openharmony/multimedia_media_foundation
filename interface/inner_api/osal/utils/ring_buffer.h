/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_RING_BUFFER_H
#define HISTREAMER_RING_BUFFER_H

#include <atomic>
#include <memory>
#include "cpp_ext/memory_ext.h"
#include "common/log.h"
#include "osal/task/condition_variable.h"
#include "osal/task/mutex.h"
#include "osal/task/autolock.h"
#include "securec.h"

namespace OHOS {
namespace Media {
class RingBuffer {
public:
    explicit RingBuffer(size_t bufferSize) : bufferSize_(bufferSize)
    {
    }

    ~RingBuffer() = default;

    bool Init()
    {
        buffer_ = CppExt::make_unique<uint8_t[]>(bufferSize_);
        return buffer_ != nullptr;
    }

    size_t ReadBuffer(void* ptr, size_t readSize, int waitTimes = 0)
    {
        AutoLock lck(writeMutex_);
        if (!isActive_ || !isReadBlockingAllowed_) {
            return 0;
        }
        MEDIA_LOG_D("ReadBuffer in current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        auto available = tail_ - head_;
        while (waitTimes > 0 && available == 0) {
            MEDIA_LOG_DD("ReadBuffer wait , waitTimes is " PUBLIC_LOG_U64, waitTimes);
            writeCondition_.Wait(lck);
            if (!isActive_ || !isReadBlockingAllowed_) {
                return 0;
            }
            available = tail_ - head_;
            waitTimes--;
        }
        FALSE_RETURN_V(ptr != nullptr, 0);
        available = (available > readSize) ? readSize : available;
        size_t index = head_ % bufferSize_;
        if (index + available < bufferSize_) {
            (void)memcpy_s(ptr, available, buffer_.get() + index, available);
        } else {
            (void)memcpy_s(ptr, bufferSize_ - index, buffer_.get() + index, bufferSize_ - index);
            (void)memcpy_s(((uint8_t*)ptr) + (bufferSize_ - index), available - (bufferSize_ - index), buffer_.get(),
                           available - (bufferSize_ - index));
        }
        head_ += available;
        mediaOffset_ += available;
        MEDIA_LOG_DD("ReadBuffer finish available is " PUBLIC_LOG_ZU ", mediaOffset_ " PUBLIC_LOG_U64, available,
            mediaOffset_);
        writeCondition_.NotifyAll();
        MEDIA_LOG_D("ReadBuffer end current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        return available;
    }

    bool WriteBuffer(void* ptr, size_t writeSize)
    {
        AutoLock lck(writeMutex_);
        if (!isActive_) {
            return false;
        }
        MEDIA_LOG_D("WriteBuffer in current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        if (writeSize > SIZE_MAX - tail_) {
            MEDIA_LOG_W("WriteBuffer WriteSize overflow " PUBLIC_LOG_ZU ", tail " PUBLIC_LOG_ZU, writeSize, tail_);
            return false;
        }
        while (writeSize + tail_ > head_ + bufferSize_) {
            MEDIA_LOG_DD("WriteBuffer wait writeSize is " PUBLIC_LOG_U64, writeSize);
            writeCondition_.Wait(lck);
            if (!isActive_) {
                return false;
            }
        }
        FALSE_RETURN_V(ptr != nullptr, false);
        size_t index = tail_ % bufferSize_;
        if (index + writeSize < bufferSize_) {
            (void)memcpy_s(buffer_.get() + index, writeSize, ptr, writeSize);
        } else {
            (void)memcpy_s(buffer_.get() + index, bufferSize_ - index, ptr, bufferSize_ - index);
            (void)memcpy_s(buffer_.get(), writeSize - (bufferSize_ - index), ((uint8_t*)ptr) + bufferSize_ - index,
                           writeSize - (bufferSize_ - index));
        }
        tail_ += writeSize;
        writeCondition_.NotifyAll();
        MEDIA_LOG_D("WriteBuffer out current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        return true;
    }

    void SetActive(bool active, bool cleanData = true)
    {
        AutoLock lck(writeMutex_);
        isActive_ = active;
        if (!active) {
            if (cleanData) {
                head_ = 0;
                tail_ = 0;
            }
            writeCondition_.NotifyAll();
        }
    }

    void SetReadBlocking(bool isReadBlockingAllowed)
    {
        {
            AutoLock lck(writeMutex_);
            isReadBlockingAllowed_ = isReadBlockingAllowed;
        }
        writeCondition_.NotifyAll();
    }

    size_t GetSize()
    {
        return (tail_ - head_);
    }

    size_t GetFreeSize()
    {
        return bufferSize_ - GetSize();
    }

    inline size_t GetHead()
    {
        return head_;
    }

    inline size_t GetTail()
    {
        return tail_;
    }

    uint64_t GetMediaOffset()
    {
        return mediaOffset_;
    }

    void SetMediaOffset(uint64_t offset)
    {
        mediaOffset_ = offset;
    }

    void Clear()
    {
        AutoLock lck(writeMutex_);
        head_ = 0;
        tail_ = 0;
        writeCondition_.NotifyAll();
    }
	
    void SetTail(size_t newTail)
    {
        {
            AutoLock lck(writeMutex_);
            MEDIA_LOG_I("SetTail: current tail " PUBLIC_LOG_ZU ", to tail " PUBLIC_LOG_ZU, tail_, newTail);
            if (newTail >= 0 && newTail >= head_) {
                tail_ = newTail;
            }
        }
        MEDIA_LOG_I("SetTail in current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        writeCondition_.NotifyAll();
    }
	
    bool Seek(uint64_t offset)
    {
        AutoLock lck(writeMutex_);
        MEDIA_LOG_I("Seek: buffer size " PUBLIC_LOG_ZU ", offset " PUBLIC_LOG_U64
                    ", mediaOffset_ " PUBLIC_LOG_U64, GetSize(), offset, mediaOffset_);
        bool result = false;
        // case1: seek forward success without dropping data already downloaded
        if (offset >= mediaOffset_ && (offset - mediaOffset_ < GetSize())) {
            head_ += offset - mediaOffset_;
            mediaOffset_ = offset;
            result = true;
        } else if (offset < mediaOffset_ &&
            (mediaOffset_ - offset <= bufferSize_ - GetSize())) { // case2: seek backward
            size_t minPosition = tail_ > bufferSize_ ? tail_ - bufferSize_ : 0;
            size_t maxInterval = head_ - minPosition;
            size_t interval = static_cast<size_t>(mediaOffset_ - offset);
            // Seek backward success without dropping data already downloaded
            if (interval <= maxInterval) {
                MEDIA_LOG_I("Seek backward success, size:" PUBLIC_LOG_ZU ", head:" PUBLIC_LOG_ZU ", tail:" PUBLIC_LOG_ZU
                    ", minPosition:" PUBLIC_LOG_ZU ", maxInterval:" PUBLIC_LOG_ZU ", interval:" PUBLIC_LOG_ZU
                    ", target offset:" PUBLIC_LOG_U64 ", current offset:" PUBLIC_LOG_U64,
                    GetSize(), head_, tail_, minPosition, maxInterval, interval, offset, mediaOffset_);
                head_ -= interval;
                mediaOffset_ = offset;
                result = true;
            }
        }
        writeCondition_.NotifyAll();
        return result;
    }

    bool SetHead(size_t newHead)
    {
        bool result = false;
        {
            AutoLock lck(writeMutex_);
            MEDIA_LOG_I("SetHead: current head " PUBLIC_LOG_ZU ", to head " PUBLIC_LOG_ZU, head_, newHead);
            if (newHead >= head_ && newHead <= tail_) {
                mediaOffset_ += (newHead - head_);
                head_ = newHead;
                result = true;
            }
        }
        MEDIA_LOG_I("SetHead in current tail " PUBLIC_LOG_ZU ", head_ " PUBLIC_LOG_ZU, tail_, head_);
        writeCondition_.NotifyAll();
        return result;
    }
private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "RingBuffer" };
    const size_t bufferSize_;
    std::unique_ptr<uint8_t[]> buffer_;
    size_t head_ {0}; // head
    size_t tail_ {0}; // tail
    Mutex writeMutex_ {};
    ConditionVariable writeCondition_ {};
    bool isActive_ {true};
    uint64_t mediaOffset_ {0};
    bool isReadBlockingAllowed_ {true};
};
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_RING_BUFFER_H