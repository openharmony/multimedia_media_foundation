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

#include "detail_enhancer_video_native.h"

#include "native_window.h"

#include "vpe_log.h"

using namespace OHOS;
using namespace OHOS::Media::VideoProcessingEngine;

namespace {
const std::unordered_map<int, int> LEVEL_MAP = {
    { VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_NONE,     DetailEnhancer::LEVEL_NONE },
    { VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_LOW,      DetailEnhancer::LEVEL_LOW },
    { VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_MEDIUM,   DetailEnhancer::LEVEL_MEDIUM },
    { VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH,     DetailEnhancer::LEVEL_HIGH },
};
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::InitializeInner()
{
    return AlgoErrorToNdk(detailEnhancer_.Initialize());
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::DeinitializeInner()
{
    return AlgoErrorToNdk(detailEnhancer_.Deinitialize());
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::SetParameter(const OHOS::Media::Format& parameter)
{
    int level;
    if (!parameter.GetIntValue(VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, level)) {
        VPE_LOGE("No quality level is configured!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    int innerLevel = NDKLevelToInner(level);
    if (innerLevel == -1) {
        VPE_LOGE("Quality level(%{public}d) is invalid!", level);
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    return AlgoErrorToNdk(detailEnhancer_.SetParameter(static_cast<DetailEnhancer::Level>(innerLevel)));
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::GetParameter(OHOS::Media::Format& parameter)
{
    DetailEnhancer::Level level;
    auto errorCode = AlgoErrorToNdk(detailEnhancer_.GetParameter(level));
    if (errorCode != VIDEO_PROCESSING_SUCCESS) {
        return errorCode;
    }
    if (!parameter.PutIntValue(VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, level)) {
        VPE_LOGE("Failed to configure quality level!");
        return VIDEO_PROCESSING_ERROR_PROCESS_FAILED;
    }
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::SetProducerSurface(const OHNativeWindow& window,
    BufferRequestConfig& requestCfg)
{
    requestCfg.width = window.surface->GetRequestWidth();
    requestCfg.height = window.surface->GetRequestHeight();
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode DetailEnhancerVideoNative::Process(const sptr<SurfaceBuffer>& sourceImage,
    sptr<SurfaceBuffer>& destinationImage)
{
    return AlgoErrorToNdk(detailEnhancer_.Process(sourceImage, destinationImage));
}

void DetailEnhancerVideoNative::UpdateRequestCfg(const sptr<SurfaceBuffer>& consumerBuffer,
    BufferRequestConfig& requestCfg)
{
    if (requestCfg.width == 0 || requestCfg.height == 0) {
        requestCfg.width = consumerBuffer->GetWidth();
        requestCfg.height = consumerBuffer->GetHeight();
    }
    requestCfg.format = consumerBuffer->GetFormat();
}

int DetailEnhancerVideoNative::NDKLevelToInner(int level) const
{
    auto it = LEVEL_MAP.find(level);
    if (it == LEVEL_MAP.end()) {
        VPE_LOGE("Invalid input level:%{public}d", level);
        return -1;
    }
    return it->second;
}
