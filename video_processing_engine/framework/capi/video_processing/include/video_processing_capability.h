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

#ifndef VIDEO_PROCESSING_CAPABILITY_H
#define VIDEO_PROCESSING_CAPABILITY_H

#include "video_processing_types.h"

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {
class VideoProcessingCapability {
public:
    static bool IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
        const VideoProcessing_ColorSpaceInfo* destinationVideoInfo);
    static bool IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo);

private:
    VideoProcessingCapability() = default;
    virtual ~VideoProcessingCapability() = default;
    VideoProcessingCapability(const VideoProcessingCapability&) = delete;
    VideoProcessingCapability& operator=(const VideoProcessingCapability&) = delete;
    VideoProcessingCapability(VideoProcessingCapability&&) = delete;
    VideoProcessingCapability& operator=(VideoProcessingCapability&&) = delete;
};
} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

#endif // VIDEO_PROCESSING_CAPABILITY_H