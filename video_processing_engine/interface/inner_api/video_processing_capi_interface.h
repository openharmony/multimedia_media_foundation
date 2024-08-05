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

#ifndef VIDEO_PROCESSING_CAPI_INTERFACE_H
#define VIDEO_PROCESSING_CAPI_INTERFACE_H

#include "video_processing_types.h"

class IVideoProcessingNdk {
public:
    virtual VideoProcessing_ErrorCode InitializeEnvironment() = 0;
    virtual VideoProcessing_ErrorCode DeinitializeEnvironment() = 0;
    virtual bool IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
        const VideoProcessing_ColorSpaceInfo* destinationVideoInfo) = 0;
    virtual bool IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo) = 0;
    virtual VideoProcessing_ErrorCode Create(OH_VideoProcessing** videoProcessor, int type) = 0;
    virtual VideoProcessing_ErrorCode Destroy(OH_VideoProcessing* videoProcessor) = 0;
    virtual VideoProcessing_ErrorCode RegisterCallback(OH_VideoProcessing* videoProcessor,
        const VideoProcessing_Callback* callback, void* userData) = 0;
    virtual VideoProcessing_ErrorCode SetSurface(OH_VideoProcessing* videoProcessor, const OHNativeWindow* window) = 0;
    virtual VideoProcessing_ErrorCode GetSurface(OH_VideoProcessing* videoProcessor, OHNativeWindow** window) = 0;
    virtual VideoProcessing_ErrorCode SetParameter(OH_VideoProcessing* videoProcessor,
        const OH_AVFormat* parameter) = 0;
    virtual VideoProcessing_ErrorCode GetParameter(OH_VideoProcessing* videoProcessor, OH_AVFormat* parameter) = 0;
    virtual VideoProcessing_ErrorCode Start(OH_VideoProcessing* videoProcessor) = 0;
    virtual VideoProcessing_ErrorCode Stop(OH_VideoProcessing* videoProcessor) = 0;
    virtual VideoProcessing_ErrorCode RenderOutputBuffer(OH_VideoProcessing* videoProcessor, uint32_t index) = 0;

    virtual VideoProcessing_ErrorCode Create(VideoProcessing_Callback** callback) = 0;
    virtual VideoProcessing_ErrorCode Destroy(VideoProcessing_Callback* callback) = 0;
    virtual VideoProcessing_ErrorCode BindOnError(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnError onError) = 0;
    virtual VideoProcessing_ErrorCode BindOnState(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnState onState) = 0;
    virtual VideoProcessing_ErrorCode BindOnNewOutputBuffer(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer) = 0;

protected:
    IVideoProcessingNdk() = default;
    virtual ~IVideoProcessingNdk() = default;
    IVideoProcessingNdk(const IVideoProcessingNdk&) = delete;
    IVideoProcessingNdk& operator=(const IVideoProcessingNdk&) = delete;
    IVideoProcessingNdk(IVideoProcessingNdk&&) = delete;
    IVideoProcessingNdk& operator=(IVideoProcessingNdk&&) = delete;
};

extern "C" IVideoProcessingNdk* CreateVideoProcessingNdk();
extern "C" void DestroyVideoProcessingNdk(IVideoProcessingNdk* obj);

#endif // VIDEO_PROCESSING_CAPI_INTERFACE_H
