/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "Mutex"

#include "osal/task/mutex.h"
#include "common/log.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer" };
}

namespace OHOS {
namespace Media {
Mutex::Mutex() : created_(true)
{
    int rtv = pthread_mutex_init(&nativeHandle_, nullptr);
    if (rtv != 0) {
        created_ = false;
        MEDIA_LOG_E("failed to init pthread mutex");
    }
}

Mutex::~Mutex()
{
    if (created_) {
        pthread_mutex_destroy(&nativeHandle_);
    }
}

void Mutex::lock()
{
    FALSE_RETURN_MSG(created_, "lock uninitialized pthread mutex!");
    pthread_mutex_lock(&nativeHandle_);
}

bool Mutex::try_lock()
{
    FALSE_RETURN_V_MSG_E(created_,
        false, "trylock uninitialized pthread mutex.");
    int ret = pthread_mutex_trylock(&nativeHandle_);
    FALSE_LOG_MSG(ret == 0, "trylock failed with ret = " PUBLIC_LOG_D32, ret);
    return ret == 0;
}

void Mutex::unlock()
{
    FALSE_RETURN_MSG(created_, "unlock uninitialized pthread mutex!");
    pthread_mutex_unlock(&nativeHandle_);
}

FairMutex::FairMutex() : Mutex()
{
    int rtv = pthread_mutex_init(&failLockHandle_, nullptr);
    if (rtv != 0) {
        created_ = false;
        MEDIA_LOG_E("failed to init FairMutex");
    } else {
        created_ = true;
    }
}

FairMutex::~FairMutex()
{
    if (created_) {
        pthread_mutex_destroy(&failLockHandle_);
    }
}

void FairMutex::lock()
{
    if (created_) {
        pthread_mutex_lock(&failLockHandle_);
        Mutex::lock();
        pthread_mutex_unlock(&failLockHandle_);
    } else {
        Mutex::lock();
    }
}
} // namespace Media
} // namespace OHOS