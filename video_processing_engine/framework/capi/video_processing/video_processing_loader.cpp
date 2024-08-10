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

#include "video_processing_loader.h"

#include <dlfcn.h>

#include "vpe_log.h"

namespace {
const std::string VPE_IMPL_LIBRARY_PATH = "libvideo_processing_capi_impl.so";
}

VideoProcessingNdkLoader& VideoProcessingNdkLoader::Get()
{
    static VideoProcessingNdkLoader loader{};
    return loader;
}

bool VideoProcessingNdkLoader::LoadLibrary()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (refCount_ > 0) {
        refCount_++;
        return true;
    }

    if (!LoadLibraryLocked()) {
        return false;
    }
    refCount_++;
    return true;
}

void VideoProcessingNdkLoader::UnloadLibrary()
{
    std::lock_guard<std::mutex> lock(lock_);
    if (refCount_ == 0) {
        VPE_LOGW("Unload too many times!");
        return;
    }

    refCount_--;
    if (refCount_ > 0) {
        return;
    }
    UnloadLibraryLocked();
}

bool VideoProcessingNdkLoader::IsValid() const
{
    return isValid_.load();
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::InitializeEnvironment()
{
    return CallNdk([](IVideoProcessingNdk* intfNdk) { return intfNdk->InitializeEnvironment(); });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::DeinitializeEnvironment()
{
    return CallNdk([](IVideoProcessingNdk* intfNdk) { return intfNdk->DeinitializeEnvironment(); });
}

bool VideoProcessingNdkLoader::IsColorSpaceConversionSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo,
    const VideoProcessing_ColorSpaceInfo* destinationVideoInfo)
{
    return CallSupportNdk([sourceVideoInfo, destinationVideoInfo](IVideoProcessingNdk* intfNdk) {
        return intfNdk->IsColorSpaceConversionSupported(sourceVideoInfo, destinationVideoInfo);
    });
}

bool VideoProcessingNdkLoader::IsMetadataGenerationSupported(const VideoProcessing_ColorSpaceInfo* sourceVideoInfo)
{
    return CallSupportNdk([sourceVideoInfo](IVideoProcessingNdk* intfNdk) {
        return intfNdk->IsMetadataGenerationSupported(sourceVideoInfo);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Create(OH_VideoProcessing** videoProcessor, int type)
{
    return CallNdk([videoProcessor, type](IVideoProcessingNdk* intfNdk) {
        return intfNdk->Create(videoProcessor, type);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Destroy(OH_VideoProcessing* videoProcessor)
{
    return CallNdk([videoProcessor](IVideoProcessingNdk* intfNdk) { return intfNdk->Destroy(videoProcessor); });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::RegisterCallback(OH_VideoProcessing* videoProcessor,
    const VideoProcessing_Callback* callback, void* userData)
{
    return CallNdk([videoProcessor, callback, userData](IVideoProcessingNdk* intfNdk) {
        return intfNdk->RegisterCallback(videoProcessor, callback, userData);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::SetSurface(OH_VideoProcessing* videoProcessor,
    const OHNativeWindow* window)
{
    return CallNdk([videoProcessor, window](IVideoProcessingNdk* intfNdk) {
        return intfNdk->SetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::GetSurface(OH_VideoProcessing* videoProcessor,
    OHNativeWindow** window)
{
    return CallNdk([videoProcessor, window](IVideoProcessingNdk* intfNdk) {
        return intfNdk->GetSurface(videoProcessor, window);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::SetParameter(OH_VideoProcessing* videoProcessor,
    const OH_AVFormat* parameter)
{
    return CallNdk([videoProcessor, parameter](IVideoProcessingNdk* intfNdk) {
        return intfNdk->SetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::GetParameter(OH_VideoProcessing* videoProcessor,
    OH_AVFormat* parameter)
{
    return CallNdk([videoProcessor, parameter](IVideoProcessingNdk* intfNdk) {
        return intfNdk->GetParameter(videoProcessor, parameter);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Start(OH_VideoProcessing* videoProcessor)
{
    return CallNdk([videoProcessor](IVideoProcessingNdk* intfNdk) { return intfNdk->Start(videoProcessor); });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Stop(OH_VideoProcessing* videoProcessor)
{
    return CallNdk([videoProcessor](IVideoProcessingNdk* intfNdk) { return intfNdk->Stop(videoProcessor); });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::RenderOutputBuffer(OH_VideoProcessing* videoProcessor,
    uint32_t index)
{
    return CallNdk([videoProcessor, index](IVideoProcessingNdk* intfNdk) {
        return intfNdk->RenderOutputBuffer(videoProcessor, index);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Create(VideoProcessing_Callback** callback)
{
    return CallNdk([callback](IVideoProcessingNdk* intfNdk) {
        return intfNdk->Create(callback);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::Destroy(VideoProcessing_Callback* callback)
{
    return CallNdk([callback](IVideoProcessingNdk* intfNdk) {
        return intfNdk->Destroy(callback);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::BindOnError(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnError onError)
{
    return CallNdk([callback, onError](IVideoProcessingNdk* intfNdk) {
        return intfNdk->BindOnError(callback, onError);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::BindOnState(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnState onState)
{
    return CallNdk([callback, onState](IVideoProcessingNdk* intfNdk) {
        return intfNdk->BindOnState(callback, onState);
    });
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::BindOnNewOutputBuffer(VideoProcessing_Callback* callback,
    OH_VideoProcessingCallback_OnNewOutputBuffer onNewOutputBuffer)
{
    return CallNdk([callback, onNewOutputBuffer](IVideoProcessingNdk* intfNdk) {
        return intfNdk->BindOnNewOutputBuffer(callback, onNewOutputBuffer);
    });
}

bool VideoProcessingNdkLoader::LoadLibraryLocked()
{
    VPE_LOGI("Load library...");
    const std::string& path = VPE_IMPL_LIBRARY_PATH;
    if (!OpenLibraryLocked(path)) {
        return false;
    }

    if (!LoadInterfaceLocked(videoProcessing_, destroyVideoProcessingFunc_,
        "CreateVideoProcessingNdk", "DestroyVideoProcessingNdk", path)) {
        UnloadLibraryLocked();
        return false;
    }

    isValid_ = true;
    VPE_LOGI("Load library successfully.");
    return true;
}

void VideoProcessingNdkLoader::UnloadLibraryLocked()
{
    isValid_ = false;
    if (destroyVideoProcessingFunc_ != nullptr) {
        destroyVideoProcessingFunc_(videoProcessing_);
        videoProcessing_ = nullptr;
        destroyVideoProcessingFunc_ = nullptr;
    }

    if (libHandle_ != nullptr)  {
        dlclose(libHandle_);
        libHandle_ = nullptr;
    }

    VPE_LOGI("Unload library.");
}

bool VideoProcessingNdkLoader::OpenLibraryLocked(const std::string& path)
{
    libHandle_ = dlopen(path.c_str(), RTLD_NOW);
    if (libHandle_ == nullptr) {
        VPE_LOGW("Can't open library %{public}s - %{public}s", path.c_str(), dlerror());
        return false;
    }
    return true;
}

bool VideoProcessingNdkLoader::LoadInterfaceLocked(IVideoProcessingNdk*& interface, destroyNdkFunc& destroyFunc,
    const std::string& createFuncName, const std::string& destroyFuncName, const std::string& path)
{
    createNdkFunc createFunc = reinterpret_cast<createNdkFunc>(dlsym(libHandle_, createFuncName.c_str()));
    if (createFunc == nullptr) {
        VPE_LOGE("Failed to locate %{public}s in %{public}s - %{public}s",
            createFuncName.c_str(), path.c_str(), path.c_str());
        UnloadLibraryLocked();
        return false;
    }
    destroyFunc = reinterpret_cast<destroyNdkFunc>(dlsym(libHandle_, destroyFuncName.c_str()));
    if (destroyFunc == nullptr) {
        VPE_LOGE("Failed to locate %{public}s in %{public}s - %{public}s",
            destroyFuncName.c_str(), path.c_str(), path.c_str());
        UnloadLibraryLocked();
        return false;
    }
    interface = createFunc();
    if (interface == nullptr) {
        VPE_LOGW("Failed to create interface!");
        UnloadLibraryLocked();
        return false;
    }
    return true;
}

bool VideoProcessingNdkLoader::CallSupportNdk(std::function<bool(IVideoProcessingNdk*)>&& operation)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (videoProcessing_ == nullptr) {
        return false;
    }
    return operation(videoProcessing_);
}

VideoProcessing_ErrorCode VideoProcessingNdkLoader::CallNdk(
    std::function<VideoProcessing_ErrorCode(IVideoProcessingNdk*)>&& operation)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (videoProcessing_ == nullptr) {
        return VIDEO_PROCESSING_ERROR_UNKNOWN;
    }
    return operation(videoProcessing_);
}
