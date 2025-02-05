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

#ifndef PERFORMANCE_UTILS_H
#define PERFORMANCE_UTILS_H

#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <algorithm>

#include "media_core.h"
#include "plugin/plugin_time.h"

namespace OHOS {
namespace Media {
namespace {
constexpr size_t DEFAULT_CAPACITY = 30;
constexpr size_t MIN_SIZE = 1;
}
#define CALC_EXPR_TIME_MS(expr)                                                                               \
    ({                                                                                                        \
        int64_t defaultDuration = 1;                                                                          \
        auto start = std::chrono::high_resolution_clock::now();                                               \
        expr;                                                                                                 \
        auto end = std::chrono::high_resolution_clock::now();                                                 \
        int64_t duration =                                                                                    \
            static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()); \
        std::max(defaultDuration, duration);                                                                  \
    })

struct MainPerfData {
    int32_t max = 0;
    int32_t min = 0;
    int32_t avg = 0;
};

class PerfRecorder {
public:
    PerfRecorder() = default;

    PerfRecorder(int32_t capacity) : capacity_(std::max(static_cast<int32_t>(MIN_SIZE), capacity)) {}

    virtual bool Record(int32_t extra)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        int64_t time = Plugins::GetCurrentMillisecond();
        std::pair<int64_t, int32_t> pair = { time, extra };
        bool isFull = list_.size() >= capacity_;
        if (isFull) {
            list_.pop_front();
        }
        list_.push_back(pair);
        return isFull;
    }

    virtual void Reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        list_.clear();
    }

    virtual MainPerfData GetMainPerfData()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        int32_t total = 0;
        int32_t max = std::numeric_limits<int32_t>::min();
        int32_t min = std::numeric_limits<int32_t>::max();

        for (auto pair : list_) {
            total += pair.second;
            max = std::max(max, pair.second);
            min = std::min(min, pair.second);
        }
        int32_t avg = total / std::max(list_.size(), MIN_SIZE);

        return { .max = max, .min = min, .avg = avg };
    }

    static const bool FULL = true;

protected:
    std::mutex mutex_{};
    size_t capacity_{ DEFAULT_CAPACITY };
    std::list<std::pair<int64_t, int32_t>> list_ {};
};

}  // namespace Media
}  // namespace OHOS
#endif  // PERFORMANCE_UTILS_H
