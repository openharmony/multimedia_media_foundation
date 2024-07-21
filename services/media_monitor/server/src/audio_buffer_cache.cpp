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

#include "audio_buffer_cache.h"
#include "log.h"
#include "monitor_error.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {
int32_t AudioBufferCache::RequestBuffer(std::shared_ptr<AudioBuffer>& buffer, int32_t size)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto status = RequestCacheBuffer(buffer, size);
    if (status == SUCCESS && buffer != nullptr) {
        return SUCCESS;
    }
    status = AllocAudioBuffer(buffer, size);
    if (status != SUCCESS) {
        return status;
    }
    bufferSize_ += 1;
    return SUCCESS;
}

int32_t AudioBufferCache::ReleaseBuffer(std::shared_ptr<AudioBuffer>& buffer)
{
    if (buffer == nullptr) {
        MEDIA_LOG_E("release buffer error");
        return ERROR;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto bufferId = buffer->GetUniqueId();
    for (auto it = freeBufferList_.begin(); it != freeBufferList_.end(); it++) {
        if (*it == bufferId) {
            return SUCCESS;
        }
    }
    if (bufferMap_.find(bufferId) == bufferMap_.end()) {
        buffer = nullptr;
        return ERROR;
    }

    freeBufferList_.emplace_back(bufferId);
    return SUCCESS;
}

int32_t AudioBufferCache::Clear()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (freeBufferList_.size() != bufferMap_.size()) {
        MEDIA_LOG_E("exist buffer not free");
    }

    for (auto it = freeBufferList_.begin(); it != freeBufferList_.end(); it++) {
        bufferMap_[*it].buffer = nullptr;
    }
    freeBufferList_.clear();
    bufferMap_.clear();
    bufferSize_ = 0;
    return SUCCESS;
}

int32_t AudioBufferCache::GetBufferById(std::shared_ptr<AudioBuffer>& buffer, uint64_t bufferId)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = bufferMap_.find(bufferId);
    if (it != bufferMap_.end()) {
        buffer = bufferMap_[bufferId].buffer;
        return SUCCESS;
    }
    return ERROR;
}

int32_t AudioBufferCache::RequestCacheBuffer(std::shared_ptr<AudioBuffer>& buffer, int32_t size)
{
    for (auto it = freeBufferList_.begin(); it != freeBufferList_.end(); it++) {
        if (size <= bufferMap_[*it].size) {
            buffer = bufferMap_[*it].buffer;
            freeBufferList_.erase(it);
            return SUCCESS;
        }
    }
    if (freeBufferList_.empty()) {
        buffer = nullptr;
        return ERROR;
    }
    int32_t status = DeleteAudioBuffer(freeBufferList_.front(), size);
    freeBufferList_.pop_front();
    if (status != SUCCESS) {
        return status;
    }
    status = AllocAudioBuffer(buffer, size);
    return status;
}

int32_t AudioBufferCache::AllocBuffer(std::shared_ptr<AudioBuffer>& buffer, int32_t size)
{
    AVBufferConfig avBufferConfig;
    avBufferConfig.size = size;
    avBufferConfig.memoryType = MemoryType::SHARED_MEMORY;
    avBufferConfig.memoryFlag = MemoryFlag::MEMORY_READ_WRITE;

    buffer = AVBuffer::CreateAVBuffer(avBufferConfig);
    if (buffer == nullptr) {
        return ERROR;
    }
    return SUCCESS;
}

int32_t AudioBufferCache::AllocAudioBuffer(std::shared_ptr<AudioBuffer>& buffer, int32_t size)
{
    int32_t status = AllocBuffer(buffer, size);
    if (status != SUCCESS || buffer == nullptr) {
        MEDIA_LOG_E("alloc buffer error");
        return ERROR;
    }
    AudioBufferElement ele = {
        .size = size,
        .buffer = buffer
    };
    bufferMap_[buffer->GetUniqueId()] = ele;
    return SUCCESS;
}

int32_t AudioBufferCache::DeleteAudioBuffer(uint64_t bufferId, int32_t size)
{
    auto it = bufferMap_.find(bufferId);
    if (it != bufferMap_.end()) {
        bufferMap_.erase(it);
        return SUCCESS;
    }
    return ERROR;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS