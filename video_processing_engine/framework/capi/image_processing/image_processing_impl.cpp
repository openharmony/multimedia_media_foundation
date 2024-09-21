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

#include "image_processing_impl.h"

#include <functional>
#include <unordered_map>

#include "vpe_log.h"
// NOTE: Add VPE feature header files like below:
// VPE feature header files begin
#include "detail_enhancer_image_native.h"
// VPE feature header files end

using namespace OHOS::Media::VideoProcessingEngine;

namespace {
template <typename T>
std::shared_ptr<ImageProcessingNative> Create()
{
    return std::make_shared<T>();
}
// NOTE: Add VPE features like below:
// VPE features begin
const std::unordered_map<int, std::function<std::shared_ptr<ImageProcessingNative>()>> CREATORS = {
    { IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER, Create<DetailEnhancerImageNative> },
};
// VPE features end
}

ImageProcessing_ErrorCode OH_ImageProcessing::Create(OH_ImageProcessing** instance, int type)
{
    auto it = CREATORS.find(type);
    if (instance == nullptr || it == CREATORS.end()) {
        VPE_LOGE("instance is null or type is not supported!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    OH_ImageProcessing* imageProcessor = new(std::nothrow) OH_ImageProcessing(type);
    if (imageProcessor == nullptr) {
        VPE_LOGE("Out of memory!");
        return IMAGE_PROCESSING_ERROR_NO_MEMORY;
    }
    auto obj = imageProcessor->GetObj();
    if (obj == nullptr) {
        VPE_LOGE("Failed to create image processing object!");
        delete imageProcessor;
        return IMAGE_PROCESSING_ERROR_CREATE_FAILED;
    }
    auto errorCode = obj->Initialize();
    if (errorCode != IMAGE_PROCESSING_SUCCESS) {
        VPE_LOGE("Failed to initialize image processing object!");
        delete imageProcessor;
        return errorCode;
    }
    *instance = imageProcessor;
    return IMAGE_PROCESSING_SUCCESS;
}

ImageProcessing_ErrorCode OH_ImageProcessing::Destroy(OH_ImageProcessing* instance)
{
    if (instance == nullptr || instance->GetObj() == nullptr) {
        VPE_LOGE("instance is null or invalid!");
        return IMAGE_PROCESSING_ERROR_INVALID_PARAMETER;
    }

    auto result = instance->GetObj()->Deinitialize();
    delete instance;
    return result;
}

OH_ImageProcessing::OH_ImageProcessing(int type)
{
    auto it = CREATORS.find(type);
    if (it == CREATORS.end()) {
        VPE_LOGE("Unknown type:%{public}d!", type);
    } else {
        imageProcessing_ = it->second();
    }
}

OH_ImageProcessing::~OH_ImageProcessing()
{
    imageProcessing_ = nullptr;
}

std::shared_ptr<ImageProcessingNative> OH_ImageProcessing::GetObj()
{
    return imageProcessing_;
}
