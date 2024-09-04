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

#include "detail_enhancer.h"

#ifdef SKIA_ENABLE
#include "skia_impl.h"
#endif
#include "vpe_log.h"

using namespace OHOS;
using namespace OHOS::Media::VideoProcessingEngine;

AlgoErrorCode DetailEnhancer::Initialize()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (isInitialized_) {
        VPE_LOGW("Already init!");
        return ALGO_ERROR_OPERATION_NOT_PERMITTED;
    }

    isInitialized_ = true;
    return ALGO_SUCCESS;
}

AlgoErrorCode DetailEnhancer::Deinitialize()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (!isInitialized_) {
        VPE_LOGW("Already deinit!");
        return ALGO_ERROR_OPERATION_NOT_PERMITTED;
    }

    isInitialized_ = false;
    return ALGO_SUCCESS;
}

AlgoErrorCode DetailEnhancer::SetParameter(const Level& level)
{
    std::lock_guard<std::mutex> lock(lock_);
    level_ = level;
    return ALGO_SUCCESS;
}

AlgoErrorCode DetailEnhancer::GetParameter(Level& level)
{
    std::lock_guard<std::mutex> lock(lock_);
    level = level_;
    return ALGO_SUCCESS;
}

AlgoErrorCode DetailEnhancer::Process(const sptr<SurfaceBuffer>& sourceImage,
    sptr<SurfaceBuffer>& destinationImage)
{
#ifdef SKIA_ENABLE
    return Skia::Process(sourceImage, destinationImage);
#else
    return ALGO_ERROR_PROCESS_FAILED;
#endif
}
