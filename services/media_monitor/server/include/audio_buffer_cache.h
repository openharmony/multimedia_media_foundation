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

#ifndef AUDIO_BUFFER_CACHE_H
#define AUDIO_BUFFER_CACHE_H

#include <list>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

#include "dump_buffer_wrap.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

using AudioBuffer = DumpBuffer;
using AudioBufferElement = struct AudioBufferElement {
    int32_t size {0};
    std::shared_ptr<AudioBuffer> buffer {nullptr};
};

class AudioBufferCache {
public:
    AudioBufferCache(std::shared_ptr<DumpBufferWrap> wrap);
    ~AudioBufferCache();
    int32_t RequestBuffer(std::shared_ptr<AudioBuffer> &buffer, int32_t size);
    int32_t ReleaseBuffer(std::shared_ptr<AudioBuffer> &buffer);
    int32_t Clear();
    int32_t GetBufferById(std::shared_ptr<AudioBuffer> &buffer, uint64_t bufferId);
    int32_t SetBufferSize(std::shared_ptr<AudioBuffer> &buffer, int32_t size);
private:
    int32_t RequestCacheBuffer(std::shared_ptr<AudioBuffer> &buffer, int32_t size);
    int32_t AllocAudioBuffer(std::shared_ptr<AudioBuffer> &buffer, int32_t size);
    int32_t DeleteAudioBuffer(uint64_t bufferId, int32_t size);
    int32_t AllocBuffer(std::shared_ptr<AudioBuffer> &buffer, int32_t size);
    std::mutex mutex_;
    std::list<uint64_t> freeBufferList_;
    std::map<uint64_t, AudioBufferElement> bufferMap_;
    int32_t bufferSize_ = 0;
    std::shared_ptr<DumpBufferWrap> dumpBufferWrap_ = nullptr;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // AUDIO_BUFFER_CACHE_H