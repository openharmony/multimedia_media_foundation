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

#ifndef VIDEO_PROCESSING_IMPL_H
#define VIDEO_PROCESSING_IMPL_H

#include <memory>

#include "video_processing_native.h"

struct OH_VideoProcessing {
public:
    static VideoProcessing_ErrorCode Create(OH_VideoProcessing** instance, int type);
    static VideoProcessing_ErrorCode Destroy(OH_VideoProcessing* instance);

    std::shared_ptr<OHOS::Media::VideoProcessingEngine::VideoProcessingNative> GetObj();

private:
    OH_VideoProcessing(int type);
    ~OH_VideoProcessing();

    std::shared_ptr<OHOS::Media::VideoProcessingEngine::VideoProcessingNative> videoProcessing_{};
};

#endif // VIDEO_PROCESSING_IMPL_H
