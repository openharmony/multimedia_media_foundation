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

#include "video_processing.h"

#include <functional>

#include "vpe_log.h"
#include "video_processing_loader.h"

const int32_t VIDEO_PROCESSING_TYPE_COLOR_SPACE_CONVERSION = 0x1;
const int32_t VIDEO_PROCESSING_TYPE_METADATA_GENERATION = 0x2;
const int32_t VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER = 0x4;
const char* VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL = "QualityLevel";

namespace {
bool CallVideoProcessingSupport(std::function<bool(VideoProcessingNdkLoader&)>&& operation)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        return operation(VideoProcessingNdkLoader::Get());
    }
    return false;
}

VideoProcessing_ErrorCode CallVideoProcessing(
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operation)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        return operation(VideoProcessingNdkLoader::Get());
    }
    return VIDEO_PROCESSING_ERROR_UNKNOWN;
}

VideoProcessing_ErrorCode CallVideoProcessingWithLoad(
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operation)
{
    if (VideoProcessingNdkLoader::Get().LoadLibrary()) {
        return operation(VideoProcessingNdkLoader::Get());
    }
    return VIDEO_PROCESSING_ERROR_UNKNOWN;
}

VideoProcessing_ErrorCode CallVideoProcessingWithUnload(
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operation)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        auto ret = operation(VideoProcessingNdkLoader::Get());
        VideoProcessingNdkLoader::Get().UnloadLibrary();
        return ret;
    }
    return VIDEO_PROCESSING_ERROR_UNKNOWN;
}
}

VideoProcessing_ErrorCode OH_VideoProcessing_InitializeEnvironment(void)
{
    return CallVideoProcessingWithLoad([](VideoProcessingNdkLoader& loader) { return loader.InitializeEnvironment(); });
}

VideoProcessing_ErrorCode OH_VideoProcessing_DeinitializeEnvironment(void)
{
    return CallVideoProcessingWithUnload([](VideoProcessingNdkLoader& loader) {
        return loader.DeinitializeEnvironment();
    });
}

bool OH_VideoProcessing_IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
    const VideoProcessing_ColorSpaceInfo* destinationVideoInfo)
{
    return CallVideoProcessingSupport([sourceVideoInfo, destinationVideoInfo](VideoProcessingNdkLoader& loader) {
        return loader.IsColorSpaceConversionSupported(sourceVideoInfo, destinationVideoInfo);
    });
}

bool OH_VideoProcessing_IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo)
{
    return CallVideoProcessingSupport([sourceVideoInfo](VideoProcessingNdkLoader& loader) {
        return loader.IsMetadataGenerationSupported(sourceVideoInfo);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Create(OH_VideoProcessing** videoProcessor, int type)
{
    return CallVideoProcessingWithLoad([videoProcessor, type](VideoProcessingNdkLoader& loader) {
        return loader.Create(videoProcessor, type);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Destroy(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessingWithUnload([videoProcessor](VideoProcessingNdkLoader& loader) {
        return loader.Destroy(videoProcessor);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_RegisterCallback(OH_VideoProcessing* videoProcessor,
    const VideoProcessing_Callback* callback, void* userData)
{
    return CallVideoProcessing([videoProcessor, callback, userData](VideoProcessingNdkLoader& loader) {
        return loader.RegisterCallback(videoProcessor, callback, userData);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_SetSurface(OH_VideoProcessing* videoProcessor,
    const OHNativeWindow* window)
{
    return CallVideoProcessing([videoProcessor, window](VideoProcessingNdkLoader& loader) {
        return loader.SetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_GetSurface(OH_VideoProcessing* videoProcessor, OHNativeWindow** window)
{
    return CallVideoProcessing([videoProcessor, window](VideoProcessingNdkLoader& loader) {
        return loader.GetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_SetParameter(OH_VideoProcessing* videoProcessor,
    const OH_AVFormat* parameter)
{
    return CallVideoProcessing([videoProcessor, parameter](VideoProcessingNdkLoader& loader) {
        return loader.SetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_GetParameter(OH_VideoProcessing* videoProcessor, OH_AVFormat* parameter)
{
    return CallVideoProcessing([videoProcessor, parameter](VideoProcessingNdkLoader& loader) {
        return loader.GetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Start(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessing([videoProcessor](VideoProcessingNdkLoader& loader) {
        return loader.Start(videoProcessor);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Stop(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessing([videoProcessor](VideoProcessingNdkLoader& loader) {
        return loader.Stop(videoProcessor);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_RenderOutputBuffer(OH_VideoProcessing* videoProcessor, uint32_t index)
{
    return CallVideoProcessing([videoProcessor, &index](VideoProcessingNdkLoader& loader) {
        return loader.RenderOutputBuffer(videoProcessor, index);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_Create(VideoProcessing_Callback** callback)
{
    return CallVideoProcessingWithLoad([callback](VideoProcessingNdkLoader& loader) {
        return loader.Create(callback);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_Destroy(VideoProcessing_Callback* callback)
{
    return CallVideoProcessingWithUnload([callback](VideoProcessingNdkLoader& loader) {
        return loader.Destroy(callback);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnError(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnError onError)
{
    return CallVideoProcessing([callback, onError](VideoProcessingNdkLoader& loader) {
        return loader.BindOnError(callback, onError);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnState(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnState onState)
{
    return CallVideoProcessing([callback, onState](VideoProcessingNdkLoader& loader) {
        return loader.BindOnState(callback, onState);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnNewOutputBuffer(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer)
{
    return CallVideoProcessing([callback, onNewOutputBuffer](VideoProcessingNdkLoader& loader) {
        return loader.BindOnNewOutputBuffer(callback, onNewOutputBuffer);
    });
}
