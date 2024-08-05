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

#ifndef VIDEO_PROCESSING_LOADER_H
#define VIDEO_PROCESSING_LOADER_H

#include <atomic>
#include <functional>
#include <mutex>
#include <string>

#include "video_processing_capi_interface.h"

class VideoProcessingNdkLoader {
public:
    static VideoProcessingNdkLoader& Get();

    bool LoadLibrary();
    void UnloadLibrary();
    bool IsValid();

    // Interface
    VideoProcessing_ErrorCode InitializeEnvironment();
    VideoProcessing_ErrorCode DeinitializeEnvironment();
    bool IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
        const VideoProcessing_ColorSpaceInfo* destinationVideoInfo);
    bool IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo);
    VideoProcessing_ErrorCode Create(OH_VideoProcessing** videoProcessor, int type);
    VideoProcessing_ErrorCode Destroy(OH_VideoProcessing* videoProcessor);
    VideoProcessing_ErrorCode RegisterCallback(OH_VideoProcessing* videoProcessor,
        const VideoProcessing_Callback* callback, void* userData);
    VideoProcessing_ErrorCode SetSurface(OH_VideoProcessing* videoProcessor, const OHNativeWindow* window);
    VideoProcessing_ErrorCode GetSurface(OH_VideoProcessing* videoProcessor, OHNativeWindow** window);
    VideoProcessing_ErrorCode SetParameter(OH_VideoProcessing* videoProcessor, const OH_AVFormat* parameter);
    VideoProcessing_ErrorCode GetParameter(OH_VideoProcessing* videoProcessor, OH_AVFormat* parameter);
    VideoProcessing_ErrorCode Start(OH_VideoProcessing* videoProcessor);
    VideoProcessing_ErrorCode Stop(OH_VideoProcessing* videoProcessor);
    VideoProcessing_ErrorCode RenderOutputBuffer(OH_VideoProcessing* videoProcessor, uint32_t index);

    // Callback
    VideoProcessing_ErrorCode Create(VideoProcessing_Callback** callback);
    VideoProcessing_ErrorCode Destroy(VideoProcessing_Callback* callback);
    VideoProcessing_ErrorCode BindOnError(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnError onError);
    VideoProcessing_ErrorCode BindOnState(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnState onState);
    VideoProcessing_ErrorCode BindOnNewOutputBuffer(VideoProcessing_Callback* callback,
        OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer);

private:
    using createNdkFunc = IVideoProcessingNdk* (*)();
    using destroyNdkFunc = void (*)(IVideoProcessingNdk* obj);

    VideoProcessingNdkLoader() = default;
    virtual ~VideoProcessingNdkLoader() = default;
    VideoProcessingNdkLoader(const VideoProcessingNdkLoader&) = delete;
    VideoProcessingNdkLoader& operator=(const VideoProcessingNdkLoader&) = delete;
    VideoProcessingNdkLoader(VideoProcessingNdkLoader&&) = delete;
    VideoProcessingNdkLoader& operator=(VideoProcessingNdkLoader&&) = delete;

    bool LoadLibraryLocked();
    void UnloadLibraryLocked();
    bool OpenLibraryLocked(const std::string& path);
    bool LoadInterfaceLocked(IVideoProcessingNdk*& interface, destroyNdkFunc& destroyFunc,
        const std::string& createFuncName, const std::string& destroyFuncName, const std::string& path);

    bool CallSupportNdk(std::function<bool(IVideoProcessingNdk*)>&& operation);
    VideoProcessing_ErrorCode CallNdk(std::function<VideoProcessing_ErrorCode(IVideoProcessingNdk*)>&& operation);

    std::mutex lock_{};
    // Guarded by lock_ begin
    std::atomic<bool> isValid_{};
    uint32_t refCount_{0};
    IVideoProcessingNdk* videoProcessing_{};
    destroyNdkFunc destroyVideoProcessingFunc_{};
    void* mLibHandle{};
    // Guarded by lock_ end
};

#endif // VIDEO_PROCESSING_LOADER_H
