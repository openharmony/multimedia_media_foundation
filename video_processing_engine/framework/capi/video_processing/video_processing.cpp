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

#include <atomic>
#include <functional>

#include "vpe_log.h"

#include "video_processing_callback_impl.h"
#include "video_processing_capability.h"
#include "video_processing_impl.h"
#include "video_processing_loader.h"

using namespace OHOS::Media::VideoProcessingEngine;

const int32_t VIDEO_PROCESSING_TYPE_COLOR_SPACE_CONVERSION = 0x1;
const int32_t VIDEO_PROCESSING_TYPE_METADATA_GENERATION = 0x2;
const int32_t VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER = 0x4;
const char* VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL = "QualityLevel";

namespace {
std::atomic<bool> g_isInit = false;

bool CallVideoProcessingSupport(std::function<bool(void)>&& operation,
    std::function<bool(VideoProcessingNdkLoader&)>&& operationLoader)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        return operationLoader(VideoProcessingNdkLoader::Get());
    }
    return operation();
}

template <typename T1, typename T2>
VideoProcessing_ErrorCode CallVideoProcessing(T2* instance,
    std::function<VideoProcessing_ErrorCode(std::shared_ptr<T1>&)>&& operation,
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operationLoader)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        return operationLoader(VideoProcessingNdkLoader::Get());
    }
    if (instance == nullptr) {
        VPE_LOGE("instance is null!");
        return VIDEO_PROCESSING_ERROR_INVALID_PARAMETER;
    }
    auto obj = instance->GetObj();
    if (obj == nullptr) {
        VPE_LOGE("instance is invalid!");
        return VIDEO_PROCESSING_ERROR_INVALID_INSTANCE;
    }
    return operation(obj);
}

VideoProcessing_ErrorCode CallVideoProcessingWithLoad(
    std::function<VideoProcessing_ErrorCode(void)>&& operation,
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operationLoader)
{
    if (VideoProcessingNdkLoader::Get().LoadLibrary()) {
        return operationLoader(VideoProcessingNdkLoader::Get());
    }
    return operation();
}

VideoProcessing_ErrorCode CallVideoProcessingWithUnload(
    std::function<VideoProcessing_ErrorCode(void)>&& operation,
    std::function<VideoProcessing_ErrorCode(VideoProcessingNdkLoader&)>&& operationLoader)
{
    if (VideoProcessingNdkLoader::Get().IsValid()) {
        auto ret = operationLoader(VideoProcessingNdkLoader::Get());
        if (ret == VIDEO_PROCESSING_SUCCESS) {
            VideoProcessingNdkLoader::Get().UnloadLibrary();
        }
        return ret;
    }
    return operation();
}
}

VideoProcessing_ErrorCode OH_VideoProcessing_InitializeEnvironment(void)
{
    return CallVideoProcessingWithLoad(
        []() {
            g_isInit = true;
            return VIDEO_PROCESSING_SUCCESS;
        },
        [](VideoProcessingNdkLoader& loader) { return loader.InitializeEnvironment(); });
}

VideoProcessing_ErrorCode OH_VideoProcessing_DeinitializeEnvironment(void)
{
    return CallVideoProcessingWithUnload(
        []() {
            if (!g_isInit.load()) {
                return VIDEO_PROCESSING_ERROR_OPERATION_NOT_PERMITTED;
            }
            g_isInit = false;
            return VIDEO_PROCESSING_SUCCESS;
        },
        [](VideoProcessingNdkLoader& loader) { return loader.DeinitializeEnvironment(); });
}

bool OH_VideoProcessing_IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
    const VideoProcessing_ColorSpaceInfo* destinationVideoInfo)
{
    return CallVideoProcessingSupport([sourceVideoInfo, destinationVideoInfo]() {
        return VideoProcessingCapability::IsColorSpaceConversionSupported(sourceVideoInfo, destinationVideoInfo);
        }, [sourceVideoInfo, destinationVideoInfo](VideoProcessingNdkLoader& loader) {
        return loader.IsColorSpaceConversionSupported(sourceVideoInfo, destinationVideoInfo);
    });
}

bool OH_VideoProcessing_IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo)
{
    return CallVideoProcessingSupport([sourceVideoInfo]() {
        return VideoProcessingCapability::IsMetadataGenerationSupported(sourceVideoInfo);
        }, [sourceVideoInfo](VideoProcessingNdkLoader& loader) {
        return loader.IsMetadataGenerationSupported(sourceVideoInfo);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Create(OH_VideoProcessing** videoProcessor, int type)
{
    return CallVideoProcessingWithLoad(
        [videoProcessor, type]() { return OH_VideoProcessing::Create(videoProcessor, type); },
        [videoProcessor, type](VideoProcessingNdkLoader& loader) { return loader.Create(videoProcessor, type); }
    );
}

VideoProcessing_ErrorCode OH_VideoProcessing_Destroy(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessingWithUnload([videoProcessor]() { return OH_VideoProcessing::Destroy(videoProcessor); },
        [videoProcessor](VideoProcessingNdkLoader& loader) { return loader.Destroy(videoProcessor); });
}

VideoProcessing_ErrorCode OH_VideoProcessing_RegisterCallback(OH_VideoProcessing* videoProcessor,
    const VideoProcessing_Callback* callback, void* userData)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [callback, userData](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->RegisterCallback(callback, userData);
    }, [videoProcessor, callback, userData](VideoProcessingNdkLoader& loader) {
        return loader.RegisterCallback(videoProcessor, callback, userData);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_SetSurface(OH_VideoProcessing* videoProcessor,
    const OHNativeWindow* window)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [window](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->SetSurface(window);
    }, [videoProcessor, window](VideoProcessingNdkLoader& loader) {
        return loader.SetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_GetSurface(OH_VideoProcessing* videoProcessor, OHNativeWindow** window)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [window](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->GetSurface(window);
    }, [videoProcessor, window](VideoProcessingNdkLoader& loader) {
        return loader.GetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_SetParameter(OH_VideoProcessing* videoProcessor,
    const OH_AVFormat* parameter)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [parameter](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->SetParameter(parameter);
    }, [videoProcessor, parameter](VideoProcessingNdkLoader& loader) {
        return loader.SetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_GetParameter(OH_VideoProcessing* videoProcessor, OH_AVFormat* parameter)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [parameter](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->GetParameter(parameter);
    }, [videoProcessor, parameter](VideoProcessingNdkLoader& loader) {
        return loader.GetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Start(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [](std::shared_ptr<VideoProcessingNative>& obj) {
        return obj->Start();
    }, [videoProcessor](VideoProcessingNdkLoader& loader) {
        return loader.Start(videoProcessor);
    });
}

VideoProcessing_ErrorCode OH_VideoProcessing_Stop(OH_VideoProcessing* videoProcessor)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [](std::shared_ptr<VideoProcessingNative>& obj) {
            return obj->Stop();
        }, [videoProcessor](VideoProcessingNdkLoader& loader) {
            return loader.Stop(videoProcessor);
        });
}

VideoProcessing_ErrorCode OH_VideoProcessing_RenderOutputBuffer(OH_VideoProcessing* videoProcessor, uint32_t index)
{
    return CallVideoProcessing<VideoProcessingNative, OH_VideoProcessing>(videoProcessor,
        [index](std::shared_ptr<VideoProcessingNative>& obj) {
            return obj->RenderOutputBuffer(index);
        }, [videoProcessor, index](VideoProcessingNdkLoader& loader) {
            return loader.RenderOutputBuffer(videoProcessor, index);
        });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_Create(VideoProcessing_Callback** callback)
{
    return CallVideoProcessingWithLoad([callback]() { return VideoProcessing_Callback::Create(callback); },
        [callback](VideoProcessingNdkLoader& loader) { return loader.Create(callback); });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_Destroy(VideoProcessing_Callback* callback)
{
    return CallVideoProcessingWithUnload([callback]() { return VideoProcessing_Callback::Destroy(callback); },
        [callback](VideoProcessingNdkLoader& loader) { return loader.Destroy(callback); });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnError(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnError onError)
{
    return CallVideoProcessing<VideoProcessingCallbackNative, VideoProcessing_Callback>(callback,
        [onError](std::shared_ptr<VideoProcessingCallbackNative>& obj) {
            return obj->BindOnError(onError);
        }, [callback, onError](VideoProcessingNdkLoader& loader) {
            return loader.BindOnError(callback, onError);
        });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnState(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnState onState)
{
    return CallVideoProcessing<VideoProcessingCallbackNative, VideoProcessing_Callback>(callback,
        [onState](std::shared_ptr<VideoProcessingCallbackNative>& obj) {
            return obj->BindOnState(onState);
        }, [callback, onState](VideoProcessingNdkLoader& loader) {
            return loader.BindOnState(callback, onState);
        });
}

VideoProcessing_ErrorCode OH_VideoProcessingCallback_BindOnNewOutputBuffer(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer)
{
    return CallVideoProcessing<VideoProcessingCallbackNative, VideoProcessing_Callback>(callback,
        [onNewOutputBuffer](std::shared_ptr<VideoProcessingCallbackNative>& obj) {
            return obj->BindOnNewOutputBuffer(onNewOutputBuffer);
        }, [callback, onNewOutputBuffer](VideoProcessingNdkLoader& loader) {
            return loader.BindOnNewOutputBuffer(callback, onNewOutputBuffer);
        });
}
