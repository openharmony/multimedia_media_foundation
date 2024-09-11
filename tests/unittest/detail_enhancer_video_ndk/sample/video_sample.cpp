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

#include "video_sample.h"

#include <chrono>
#include <unistd.h>

#include "native_avformat.h"
#include "sync_fence.h"

#include "video_processing_callback_impl.h"
#include "video_processing_callback_native.h"
#include "video_processing_impl.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;
using OHOS::Surface;
using std::mutex;
using OHOS::SurfaceBuffer;
using OHOS::IBufferConsumerListener;

namespace {
constexpr std::chrono::seconds STOP_TIMEOUT(10);
const int INPUT_FRAME_NUMBER = 10;
const int QUEUE_BUFFER_SIZE = 5;
}

class TestConsumerListener : public IBufferConsumerListener {
public:
    explicit TestConsumerListener(VideoSample *sample) : sample_(sample) {};
    ~TestConsumerListener() {}
    void OnBufferAvailable() override
    {
        sample_->OnBufferAvailable();
    }

private:
    VideoSample *sample_ = nullptr;
};

static void OnError(OH_VideoProcessing* videoProcessor, VideoProcessing_ErrorCode error, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    sample->UpdateErrorCount();
}

static void OnState(OH_VideoProcessing* videoProcessor, VideoProcessing_State state, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    if (state == VIDEO_PROCESSING_STATE_STOPPED) {
        sample->NotifyCv();
    }
}

static void OnNewOutputBufferCall(OH_VideoProcessing* videoProcessor, uint32_t index, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_RenderOutputBuffer(videoProcessor, index);
    if (ret != VIDEO_PROCESSING_SUCCESS) {
        sample->UpdateErrorCount();
    }
}

static void OnNewOutputBufferCallImpl(OH_VideoProcessing* videoProcessor, uint32_t index, void* userData)
{
    VideoSample* sample = reinterpret_cast<VideoSample*>(userData);
    VideoProcessing_ErrorCode ret = videoProcessor->GetObj()->RenderOutputBuffer(index);
    if (ret != VIDEO_PROCESSING_SUCCESS) {
        sample->UpdateErrorCount();
    }
}

VideoSample::VideoSample()
{
}

VideoSample::~VideoSample()
{
    if (callback_ || callbackImpl_) {
        if (!isImpl_) {
            OH_VideoProcessingCallback_Destroy(callback_);
            callback_ = nullptr;
        } else {
            VideoProcessing_Callback::Destroy(callbackImpl_);
            callbackImpl_ = nullptr;
        }
    }
    if (rect_) {
        delete rect_;
        rect_ = nullptr;
    }
    if (!isImpl_) {
        OH_VideoProcessing_Destroy(videoProcessor_);
    } else {
        OH_VideoProcessing::Destroy(videoProcessorImpl_);
    }
    if (cs_) {
        for (; !inputBufferAvilQue_.empty(); inputBufferAvilQue_.pop()) {
            cs_->ReleaseBuffer(inputBufferAvilQue_.front(), -1);
        }
        cs_->UnregisterConsumerListener();
    }
    cs_ = nullptr;
    OH_VideoProcessing_DeinitializeEnvironment();
}

int32_t VideoSample::InitVideoSample(VideoProcessParam param)
{
    OH_VideoProcessing_InitializeEnvironment();
    param_ = param;
    OH_VideoProcessing_Create(&videoProcessor_, VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER);
    cs_ = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new TestConsumerListener(this);
    cs_->RegisterConsumerListener(listener);
    auto p = cs_->GetProducer();
    sptr<Surface> ps = Surface::CreateSurfaceAsProducer(p);
    outWindow_ = CreateNativeWindowFromSurface(&ps);
    cs_->SetQueueSize(QUEUE_BUFFER_SIZE);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_BUFFER_GEOMETRY, param_.outWidth, param_.outHeight);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_FORMAT, param_.outFmt);
    OH_VideoProcessing_SetSurface(videoProcessor_, outWindow_);
    OH_VideoProcessing_GetSurface(videoProcessor_, &inWindow_);
    SetInputWindowParam();
    OH_VideoProcessingCallback_Create(&callback_);
    OH_VideoProcessingCallback_BindOnError(callback_, OnError);
    OH_VideoProcessingCallback_BindOnState(callback_, OnState);
    OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback_, OnNewOutputBufferCall);
    OH_VideoProcessing_RegisterCallback(videoProcessor_, callback_, this);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, qualityLevel_);
    OH_VideoProcessing_SetParameter(videoProcessor_, parameter);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::InitVideoSampleImpl(VideoProcessParam param)
{
    param_ = param;
    OH_VideoProcessing::Create(&videoProcessorImpl_, VIDEO_PROCESSING_TYPE_DETAIL_ENHANCER);
    cs_ = Surface::CreateSurfaceAsConsumer();
    sptr<IBufferConsumerListener> listener = new TestConsumerListener(this);
    cs_->RegisterConsumerListener(listener);
    auto p = cs_->GetProducer();
    sptr<Surface> ps = Surface::CreateSurfaceAsProducer(p);
    outWindow_ = CreateNativeWindowFromSurface(&ps);
    cs_->SetQueueSize(QUEUE_BUFFER_SIZE);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_BUFFER_GEOMETRY, param_.outWidth, param_.outHeight);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(outWindow_, SET_FORMAT, param_.outFmt);
    videoProcessorImpl_->GetObj()->SetSurface(outWindow_);
    videoProcessorImpl_->GetObj()->GetSurface(&inWindow_);
    SetInputWindowParam();
    VideoProcessing_Callback::Create(&callbackImpl_);
    callbackImpl_->GetObj()->BindOnError(OnError);
    callbackImpl_->GetObj()->BindOnState(OnState);
    callbackImpl_->GetObj()->BindOnNewOutputBuffer(OnNewOutputBufferCallImpl);
    videoProcessorImpl_->GetObj()->RegisterCallback(callbackImpl_, this);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, qualityLevel_);
    videoProcessorImpl_->GetObj()->SetParameter(parameter);
    return VIDEO_PROCESSING_SUCCESS;
}

void VideoSample::SetInputWindowParam()
{
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow_, SET_BUFFER_GEOMETRY, param_.inWidth, param_.inHeight);
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow_, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE |
        NATIVEBUFFER_USAGE_MEM_DMA | NATIVEBUFFER_USAGE_HW_RENDER);
    (void)OH_NativeWindow_NativeWindowHandleOpt(inWindow_, SET_FORMAT, param_.inFmt);

    rect_ = new Region::Rect();
    rect_->x = 0;
    rect_->y = 0;
    rect_->w = param_.inWidth;
    rect_->h = param_.inHeight;
    region_.rects = rect_;
}

int32_t VideoSample::InputFunc()
{
    for (int i = 0; i < INPUT_FRAME_NUMBER; i++) {
        int fenceFd = -1;
        OHNativeWindowBuffer *ohNativeWindowBuffer;
        OH_NativeWindow_NativeWindowRequestBuffer(inWindow_, &ohNativeWindowBuffer, &fenceFd);
        if (fenceFd > 0) {
            close(fenceFd);
        }
        OH_NativeBuffer *nativeBuffer = nullptr;
        OH_NativeBuffer_FromNativeWindowBuffer(ohNativeWindowBuffer, &nativeBuffer);
        OH_NativeWindow_NativeWindowFlushBuffer(inWindow_, ohNativeWindowBuffer, -1, region_);
    }
    return 0;
}

int32_t VideoSample::StartProcess()
{
    OH_VideoProcessing_Start(videoProcessor_);
    inputLoop_ = make_unique<thread>(&VideoSample::InputFunc, this);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::StartProcessImpl()
{
    videoProcessorImpl_->GetObj()->Start();
    inputLoop_ = make_unique<thread>(&VideoSample::InputFunc, this);
    return VIDEO_PROCESSING_SUCCESS;
}

int32_t VideoSample::WaitAndStopSample()
{
    inputLoop_->join();
    int32_t ret = OH_VideoProcessing_Stop(videoProcessor_);
    unique_lock<mutex> lock(mutex_);
    if (cv_.wait_for(lock, STOP_TIMEOUT) == std::cv_status::timeout) {
        std::cout << "waiting stop state timeout" << std::endl;
    }
    return ret;
}

int32_t VideoSample::WaitAndStopSampleImpl()
{
    inputLoop_->join();
    int32_t ret = videoProcessorImpl_->GetObj()->Stop();
    unique_lock<mutex> lock(mutex_);
    if (cv_.wait_for(lock, STOP_TIMEOUT) == std::cv_status::timeout) {
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
    int32_t ret = videoProcessorImpl_->GetObj()->SetSurface(window2);
    return ret;
}

void VideoSample::OnBufferAvailable()
{
    unique_lock<mutex> lock(mutexListener_);
    sptr<SurfaceBuffer> buffer;
    Rect damage = {};
    int32_t fence = -1;
    int64_t timestamp = 0;
    cs_->AcquireBuffer(buffer, fence, timestamp, damage);
    inputBufferAvilQue_.push(buffer);
    lock.unlock();
}

void VideoSample::UpdateErrorCount()
{
    errCount_++;
}

void VideoSample::SetQualityLevel(VideoDetailEnhancer_QualityLevel level)
{
    qualityLevel_ = level;
}

void VideoSample::SetImplLoader(bool isImpl)
{
    isImpl_ = isImpl;
}

void VideoSample::NotifyCv()
{
    cv_.notify_all();
}