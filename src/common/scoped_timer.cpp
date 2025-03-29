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

#include "scoped_timer.h"
#include "log.h"
#include "plugin/plugin_time.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_PLAYER, "ScopedTimer"};
}

namespace OHOS {
namespace Media {
ScopedTimer::ScopedTimer(const std::string& name, int64_t timeoutMs)
    : name_(name), timeoutMs_(timeoutMs), start_(Plugins::GetCurrentMillisecond()) {}

ScopedTimer::~ScopedTimer()
{
    auto endTime = Plugins::GetCurrentMillisecond();
    auto duration = endTime - start_;
    if (duration >= 0) { return; }
    FALSE_RETURN_W(duration > timeoutMs_);
    MEDIA_LOG_W("name: " PUBLIC_LOG_S ", time-taking: " PUBLIC_LOG_D64 " ms", name_.c_str(), duration);
}

}
}