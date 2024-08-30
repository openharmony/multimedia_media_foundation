/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <chrono>
#include <fstream>
#include <unistd.h>

#include "sync_fence.h"
#include "native_avformat.h"

#include "video_processing_impl.h"
#include "video_processing_callback_impl.h"
#include "video_processing_callback_native.h"
#include "video_sample.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;
using OHOS::Surface;
using std::mutex;
using OHOS::SurfaceBuffer;
using OHOS::IBufferConsumerListener;

namespace {
static std::mutex g_Mutex;
static std::condition_variable g_Cord;
constexpr std::chrono::seconds STOP_TIMEOUT(10);
static uint32_t kQueueBufferSize = 5;
}

class TestConsumerListener : public IBufferConsumerListener {
public:
    explicit TestConsumerListener(VideoSample *sample) : sample_(sample) {};
    ~TestConsumerListener() {}
    void OnBufferAvailable() override
    {
    }
private:
    VideoSample *sample_ = nullptr;
};

static void OnError(OH_VideoProcessing* videoProcessor, VideoProcessing_ErrorCode error, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    sample->errCount++;
}

static void OnState(OH_VideoProcessing* videoProcessor, VideoProcessing_State state, void* userData)
{
}

static void OnNewOutputBufferCall(OH_VideoProcessing* videoProcessor, uint32_t index, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_RenderOutputBuffer(videoProcessor, index);
    if (ret != VIDEO_PROCESSING_SUCCESS) {
        sample->errCount++;
    }
}

static void OnNewOutputBufferCallImpl(OH_VideoProcessing* videoProcessor, uint32_t index, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    VideoProcessing_ErrorCode ret = videoProcessor->GetObj()->RenderOutputBuffer(index);
    if (ret != VIDEO_PROCESSING_SUCCESS) {
        sample->errCount++;
    }
}

VideoSample::VideoSample()
{
}

VideoSample::~VideoSample()
{
    if (callback || callbackImpl) {
        if (!isImpl) {
            OH_VideoProcessingCallback_Destroy(callback);
            callback = nullptr;
        } else {
            VideoProcessing_Callback::Destroy(callbackImpl);
        }
    }
    if (rect) {
        delete rect;
        rect = nullptr;
    }
    if (!isImpl) {
        OH_VideoProcessing_Destroy(videoProcessor);
    } else {
        OH_VideoProcessing::Destroy(videoProcessorImpl);
    }
    OH_VideoProcessing_DeinitializeEnvironment();
}

int32_t VideoSample::InitVideoSample(VideoProcessParam param)
{
    OH_VideoProcessing_InitializeEnvironment();
    param_ = param;
    OH_VideoProcessing_Create(&videoProcessor, VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER);
    cs = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new TestConsumerListener(this);
    cs->RegisterConsumerListener(listener);
    auto p = cs->GetProducer();
    ps = Surface::CreateSurfaceAsProducer(p);
    outWindow = CreateNativeWindowFromSurface(&ps);
    cs->SetQueueSize(kQueueBufferSize);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_BUFFER_GEOMETRY, param_.out_width, param_.out_height);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_FORMAT, param_.outFmt);
    OH_VideoProcessing_SetSurface(videoProcessor, outWindow);
    OH_VideoProcessing_GetSurface(videoProcessor, &inWindow);
    SetInputWindowParam();
    OH_VideoProcessingCallback_Create(&callback);
    OH_VideoProcessingCallback_BindOnError(callback, OnError);
    OH_VideoProcessingCallback_BindOnState(callback, OnState);
    OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBufferCall);
    OH_VideoProcessing_RegisterCallback(videoProcessor, callback, this);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, qualityLevel_);
    OH_VideoProcessing_SetParameter(videoProcessor, parameter);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::InitVideoSampleImpl(VideoProcessParam param)
{
    param_ = param;
    OH_VideoProcessing::Create(&videoProcessorImpl, VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER);
    cs = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new TestConsumerListener(this);
    cs->RegisterConsumerListener(listener);
    auto p = cs->GetProducer();
    ps = Surface::CreateSurfaceAsProducer(p);
    outWindow = CreateNativeWindowFromSurface(&ps);
    cs->SetQueueSize(kQueueBufferSize);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_BUFFER_GEOMETRY, param_.out_width, param_.out_height);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow, SET_FORMAT, param_.outFmt);
    videoProcessorImpl->GetObj()->SetSurface(outWindow);
    videoProcessorImpl->GetObj()->GetSurface(&inWindow);
    SetInputWindowParam();
    VideoProcessing_Callback::Create(&callbackImpl);
    callbackImpl->GetObj()->BindOnError(OnError);
    callbackImpl->GetObj()->BindOnState(OnState);
    callbackImpl->GetObj()->BindOnNewOutputBuffer(OnNewOutputBufferCallImpl);
    videoProcessorImpl->GetObj()->RegisterCallback(callbackImpl, this);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, qualityLevel_);
    videoProcessorImpl->GetObj()->SetParameter(parameter);
    return VIDEO_PROCESSING_SUCCESS;
}

void VideoSample::SetInputWindowParam()
{
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow, SET_BUFFER_GEOMETRY, param_.in_width, param_.in_height);
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow, SET_FORMAT, param_.inFmt);

    rect = new Region::Rect();
    rect->x = 0;
    rect->y = 0;
    rect->w = param_.in_width;
    rect->h = param_.in_height;
    region.rects = rect;
}

int32_t VideoSample::InputFunc()
{
    for (int32_t i = 0; i < inputFrameNumber; i++) {
        int fenceFd = -1;
        OHNativeWindowBuffer *ohNativeWindowBuffer;
        OH_NativeWindow_NativeWindowRequestBuffer(inWindow, &ohNativeWindowBuffer, &fenceFd);
        if (fenceFd > 0) {
            close(fenceFd);
        }
        OH_NativeBuffer *nativeBuffer = nullptr;
        OH_NativeBuffer_FromNativeWindowBuffer(ohNativeWindowBuffer, &nativeBuffer);
        OH_NativeWindow_NativeWindowFlushBuffer(inWindow, ohNativeWindowBuffer, -1, region);
        usleep(30000); // 30000: sleep for a while
    }
    return 0;
}

int32_t VideoSample::StartProcess()
{
    OH_VideoProcessing_Start(videoProcessor);
    inputLoop_ = make_unique<thread>(&VideoSample::InputFunc, this);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::StartProcessImpl()
{
    videoProcessorImpl->GetObj()->Start();
    inputLoop_ = make_unique<thread>(&VideoSample::InputFunc, this);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::WaitAndStopSample()
{
    inputLoop_->join();
    int32_t ret = OH_VideoProcessing_Stop(videoProcessor);
    unique_lock<mutex> lock(g_Mutex);
    if (g_Cord.wait_for(lock, STOP_TIMEOUT) == std::cv_status::timeout) {
        std::cout << "waiting stop state timeout" << std::endl;
    }
    return ret;
}

int32_t VideoSample::WaitAndStopSampleImpl()
{
    inputLoop_->join();
    int32_t ret = videoProcessorImpl->GetObj()->Stop();
    unique_lock<mutex> lock(g_Mutex);
    if (g_Cord.wait_for(lock, STOP_TIMEOUT) == std::cv_status::timeout) {
        std::cout << "waiting stop state timeout" << std::endl;
    }
    return ret;
}

int32_t VideoSample::SetSurfaceOnRunningImpl()
{
    OH_VideoProcessing* videoProcessing2 = nullptr;
    OH_VideoProcessing::Create(&videoProcessing2, VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER);
    OHNativeWindow* window2 = nullptr;
    videoProcessing2->GetObj()->GetSurface(&window2);
    int32_t ret = videoProcessorImpl->GetObj()->SetSurface(window2);
    return ret;
}