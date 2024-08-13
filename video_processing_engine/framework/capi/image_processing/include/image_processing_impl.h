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

#ifndef IMAGE_PROCESSING_IMPL_H
#define IMAGE_PROCESSING_IMPL_H

#include <memory>

#include "image_processing_native.h"

struct OH_ImageProcessing {
public:
    static ImageProcessing_ErrorCode Create(OH_ImageProcessing** instance, int type);
    static ImageProcessing_ErrorCode Destroy(OH_ImageProcessing* instance);

    std::shared_ptr<OHOS::Media::VideoProcessingEngine::ImageProcessingNative> GetObj();

private:
    OH_ImageProcessing(int type);
    ~OH_ImageProcessing();

    std::shared_ptr<OHOS::Media::VideoProcessingEngine::ImageProcessingNative> imageProcessing_{};
};

#endif // IMAGE_PROCESSING_IMPL_H
