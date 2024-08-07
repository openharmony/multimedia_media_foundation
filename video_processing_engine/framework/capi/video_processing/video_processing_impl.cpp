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

#include "video_processing_impl.h"

#include <functional>
#include <unordered_map>

#include "vpe_log.h"
// NOTE: Add VPE feature header files like below:
// VPE feature header files begin
#include "detail_enhancer_video_native.h"
// VPE feature header files end

using namespace OHOS::Media::VideoProcessingEngine;

namespace {
template <typename T>
std::shared_ptr<VideoProcessingNative> Create(OH_VideoProcessing* context)
{
    return std::make_shared<T>(context);
}
// NOTE: Add VPE features like below:
// VPE features begin
const std::unordered_map<int, std::function<std::shared_ptr<VideoProcessingNative>(OH_VideoProcessing*)>> CREATORS = {
    { VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER, Create<DetailEnhancerVideoNative> },
};
// VPE features end
}

VideoProcessing_ErrorCode OH_VideoProcessing::Create(OH_VideoProcessing** instance, int type)
{
    auto it = CREATORS.find(type);
    if (instance == nullptr || it == CREATORS.end()) {
        VPE_LOGE("instance is null or type is not supported!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    OH_VideoProcessing* videoProcessor = new(std::nothrow) OH_VideoProcessing(type);
    if (videoProcessor == nullptr) {
        VPE_LOGE("Out of memory!");
        return VIDEO_PROCESSING_ERROR_NO_MEMORY;
    }
    auto obj = videoProcessor->GetObj();
    if (obj == nullptr) {
        VPE_LOGE("Failed to create video processing object!");
        delete videoProcessor;
        return VIDEO_PROCESSING_ERROR_CREATE_FAILED;
    }
    auto errorCode = obj->Initialize();
    if (errorCode != VIDEO_PROCESSING_SUCCESS) {
        VPE_LOGE("Failed to initialize video processing oject!");
        delete videoProcessor;
        return errorCode;
    }
    *instance = videoProcessor;
    return VIDEO_PROCESSING_SUCCESS;
}

VideoProcessing_ErrorCode OH_VideoProcessing::Destroy(OH_VideoProcessing* instance)
{
    if (instance == nullptr || instance->GetObj() == nullptr) {
        VPE_LOGE("instance is null or invalid!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    auto result = instance->GetObj()->Deinitialize();
    delete instance;
    return result;
}

OH_VideoProcessing::OH_VideoProcessing(int type)
{
    auto it = CREATORS.find(type);
    if (it == CREATORS.end()) {
        VPE_LOGE("Unknown type:%{public}d!", type);
    } else {
        videoProcessing_ = it->second(this);
    }
}

OH_VideoProcessing::~OH_VideoProcessing()
{
    videoProcessing_ = nullptr;
}

std::shared_ptr<VideoProcessingNative> OH_VideoProcessing::GetObj()
{
    return videoProcessing_;
}
