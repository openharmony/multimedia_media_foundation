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
#include <unistd.h>
#include <fstream>
#include <gtest/gtest.h>

#include "native_avformat.h"
#include "surface/window.h"
#include "external_window.h"

#include "video_processing.h"
#include "video_processing_impl.h"
#include "video_processing_loader.h"
#include "video_processing_native.h"
#include "video_processing_types.h"
#include "video_processing_callback_impl.h"
#include "video_processing_callback_native.h"
#include "video_sample.h"
#ifdef SKIA_ENABLE
#include "skia_impl.h"
#endif
#include "detail_enhancer_video_native.h"

constexpr int64_t NANOS_IN_SECOND = 1000000000L;
constexpr int64_t NANOS_IN_MICRO = 1000L;
constexpr uint32_t DEFAULT_WIDTH = 1920;
constexpr uint32_t DEFAULT_HEIGHT = 1080;
constexpr uint64_t NV12_FMT_INDEX = 24;
constexpr int CREATE_TYPE = 0x4;

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {

const VideoProcessing_ColorSpaceInfo SRC_INFO {
    .metadataType = OH_VIDEO_HDR_HDR10,
    .colorSpace = OH_COLORSPACE_BT2020_PQ_LIMIT,
    .pixelFormat = NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP,
};

const VideoProcessing_ColorSpaceInfo DST_INFO {
    .metadataType = OH_VIDEO_HDR_HLG,
    .colorSpace = OH_COLORSPACE_BT2020_PQ_LIMIT,
    .pixelFormat = NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP,
};

BufferRequestConfig bufferConfig1 = {
    .width = 0,
    .height = 0,
    .strideAlignment = 0x8,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888,
    .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    .timeout = 0,
};

BufferRequestConfig bufferConfig2 = {
    .width = 400,
    .height = 0,
    .strideAlignment = 0x8,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888,
    .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    .timeout = 0,
};

BufferRequestConfig bufferConfig4 = {
    .width = 0,
    .height = 400,
    .strideAlignment = 0x8,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888,
    .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    .timeout = 0,
};

BufferRequestConfig bufferConfig3 = {
    .width = 400,
    .height = 400,
    .strideAlignment = 0x8,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888,
    .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    .timeout = 0,
};

static std::condition_variable g_Cond;

class TestConsumerListener : public IBufferConsumerListener {
public:
    TestConsumerListener(sptr<Surface> cs, std::string_view name);
    ~TestConsumerListener();
    void OnBufferAvailable() override;
};

TestConsumerListener::TestConsumerListener(sptr<Surface> cs, std::string_view name) {}

TestConsumerListener::~TestConsumerListener() {}

void TestConsumerListener::OnBufferAvailable() {}

void OnErrorEmptyCallback(OH_VideoProcessing* handle, VideoProcessing_ErrorCode errorCode, void* userData)
{
}

void OnError(OH_VideoProcessing* handle, VideoProcessing_ErrorCode errorCode, void* userData)
{
}

void OnState(OH_VideoProcessing* instance, VideoProcessing_State state, void* userData)
{
}

void OnNewOutputBuffer(OH_VideoProcessing* instance, uint32_t index, void* userData)
{
}

class DetailEnhancerVideoNdkUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    uint32_t FlushSurf(OHNativeWindowBuffer* ohNativeWindowBuffer, OHNativeWindow* window);
};

void DetailEnhancerVideoNdkUnitTest::SetUpTestCase(void)
{
}

void DetailEnhancerVideoNdkUnitTest::TearDownTestCase(void)
{
}

void DetailEnhancerVideoNdkUnitTest::SetUp(void)
{
}

void DetailEnhancerVideoNdkUnitTest::TearDown(void)
{
}

int64_t GetSystemTime()
{
    struct timespec now;
    (void)clock_gettime(CLOCK_BOOTTIME, &now);
    int64_t nanoTime = reinterpret_cast<int64_t>(now.tv_sec) * NANOS_IN_SECOND + now.tv_nsec;
    return nanoTime / NANOS_IN_MICRO;
}

uint32_t DetailEnhancerVideoNdkUnitTest::FlushSurf(OHNativeWindowBuffer* ohNativeWindowBuffer, OHNativeWindow* window)
{
    struct Region region;
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0;
    rect->y = 0;
    rect->w = DEFAULT_WIDTH;
    rect->h = DEFAULT_HEIGHT;
    region.rects = rect;
    NativeWindowHandleOpt(window, SET_UI_TIMESTAMP, GetSystemTime());
    int32_t err = OH_NativeWindow_NativeWindowFlushBuffer(window, ohNativeWindowBuffer, -1, region);
    delete rect;
    if (err != 0) {
        cout << "FlushBuffer failed" << endl;
        return 1;
    }
    return 0;
}

// load test
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_00, TestSize.Level1)
{
    VideoProcessingNdkLoader& ndkLoader = VideoProcessingNdkLoader::Get();
    ndkLoader.LoadLibrary();
    ndkLoader.LoadLibrary();
    ndkLoader.InitializeEnvironment();
    ndkLoader.DeinitializeEnvironment();
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    ndkLoader.Create(&instance, CREATE_TYPE);
    ndkLoader.LoadLibraryLocked();
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    ndkLoader.RegisterCallback(instance, callback, nullptr);
    OHNativeWindow* window = nullptr;
    ndkLoader.GetSurface(nullptr, nullptr);
    ndkLoader.SetSurface(instance, window);
    ndkLoader.GetSurface(instance, &window);
    OH_AVFormat* parameter = nullptr;
    ndkLoader.SetParameter(instance, parameter);
    ndkLoader.GetParameter(instance, parameter);
    ndkLoader.Start(instance);
    ndkLoader.Stop(instance);
    ndkLoader.RenderOutputBuffer(instance, 0);
    ndkLoader.Create(&callback);
    ndkLoader.Destroy(callback);
    ndkLoader.Destroy(instance);
    ndkLoader.UnloadLibrary();
    ndkLoader.UnloadLibrary();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// initialize context with nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_01, TestSize.Level1)
{
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// initialize context with nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_01_1, TestSize.Level1)
{
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    ret = OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing::Destroy(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// create context without initialization
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_02, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_Destroy(instance);
}

// create context impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_02_1, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing::Destroy(instance);
}

// initialize context with wrong type impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_03_1, TestSize.Level1)
{
    int badCreateType = 0x1;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    ret = OH_VideoProcessing::Create(&instance, badCreateType);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing::Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// destroy context without create
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_05, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// destroy context without create impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_05_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing::Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// destroy context without create or initialize
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_06, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// destroy context without create or initialize impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_06_1, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing::Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// support check with null
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_07, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool res = OH_VideoProcessing_IsColorSpaceConversionSupported(nullptr, nullptr);
    ASSERT_FALSE(res);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// support check
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_08, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool res = OH_VideoProcessing_IsColorSpaceConversionSupported(&SRC_INFO, &DST_INFO);
    ASSERT_FALSE(res);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// support check 2
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_09, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool res = OH_VideoProcessing_IsColorSpaceConversionSupported(&SRC_INFO, nullptr);
    ASSERT_FALSE(res);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// support check 3
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_10, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool res = OH_VideoProcessing_IsColorSpaceConversionSupported(nullptr, &DST_INFO);
    ASSERT_FALSE(res);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// metagen check
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_11, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool ret = OH_VideoProcessing_IsMetadataGenerationSupported(nullptr);
    ASSERT_FALSE(ret);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// metagen check 2
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_12, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool ret = OH_VideoProcessing_IsMetadataGenerationSupported(&SRC_INFO);
    ASSERT_FALSE(ret);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// metagen check 3
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_13, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool ret = OH_VideoProcessing_IsMetadataGenerationSupported(&DST_INFO);
    ASSERT_FALSE(ret);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_14, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetParameter(instance, parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter to nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_14_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = nullptr;
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetParameter(parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to high
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_15, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetParameter(instance, parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to high impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_15_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to medium
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_16, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_MEDIUM);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetParameter(instance, parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to medium impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_16_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_MEDIUM);
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to low
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_17, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_LOW);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetParameter(instance, parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to low impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_17_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_LOW);
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to none
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_18, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_NONE);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetParameter(instance, parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter quality level to none impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_18_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_NONE);
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter to non-nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_19_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    VideoProcessing_ErrorCode ret = instance->GetObj()->GetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_20, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter to nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_20_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = nullptr;
    VideoProcessing_ErrorCode ret = instance->GetObj()->GetParameter(parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter after setting to high
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_21, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    OH_VideoProcessing_SetParameter(instance, parameterSetted);
    OH_AVFormat* parameterGot = OH_AVFormat_Create();
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_GetParameter(instance, parameterGot);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter after setting to high impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_21_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    instance->GetObj()->SetParameter(parameterSetted);
    OH_AVFormat* parameterGot = OH_AVFormat_Create();
    VideoProcessing_ErrorCode ret = instance->GetObj()->GetParameter(parameterGot);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set surface with surface from another instance
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_22, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    OH_VideoProcessing_GetSurface(instance, &window);
    OH_VideoProcessing_GetSurface(instance2, &window2);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetSurface(instance, window2);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set surface with surface from another instance impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_22_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_VideoProcessing::Create(&instance2, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    instance->GetObj()->GetSurface(&window);
    instance->GetObj()->GetSurface(&window2);
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetSurface(window2);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing::Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set surface with nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_23, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetSurface(instance, window);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set surface with nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_23_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    VideoProcessing_ErrorCode ret = instance->GetObj()->SetSurface(window);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get surface to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_24, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_GetSurface(instance, &window);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get surface to nullptr impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_24_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    VideoProcessing_ErrorCode ret = instance->GetObj()->GetSurface(&window);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_25, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback function impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_25_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = VideoProcessing_Callback::Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create and destroy callback function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_26, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Destroy(callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create and destroy callback function impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_26_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_Callback::Create(&callback);
    VideoProcessing_ErrorCode ret = VideoProcessing_Callback::Destroy(callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// destroy callback without create
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_27, TestSize.Level1)
{
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Destroy(callback);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// destroy callback without create impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_27_1, TestSize.Level1)
{
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = VideoProcessing_Callback::Destroy(callback);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// create callback function then register
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_28, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnError(callback, OnError);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnState(callback, OnState);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    ret = OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback function then register impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_28_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = VideoProcessing_Callback::Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnError(OnError);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnState(OnState);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnNewOutputBuffer(OnNewOutputBuffer);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    ret = instance->GetObj()->RegisterCallback(callback, &userData);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_Callback::Destroy(callback);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback but register null function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_29, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnError(callback, nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnState(callback, nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    ret = OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback but register null function impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_29_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = VideoProcessing_Callback::Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnError(nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnState(nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = callback->GetObj()->BindOnNewOutputBuffer(nullptr);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    ret = instance->GetObj()->RegisterCallback(callback, &userData);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_Callback::Destroy(callback);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// createa and destroy callback function with register
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_30, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    OH_VideoProcessingCallback_BindOnError(callback, OnError);
    OH_VideoProcessingCallback_BindOnState(callback, OnState);
    OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    auto userData = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_Destroy(callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// createa and destroy callback function with register impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_30_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_Callback::Create(&callback);
    callback->GetObj()->BindOnError(OnError);
    callback->GetObj()->BindOnState(OnState);
    callback->GetObj()->BindOnNewOutputBuffer(OnNewOutputBuffer);
    auto userData = nullptr;
    VideoProcessing_ErrorCode ret = instance->GetObj()->RegisterCallback(callback, &userData);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = VideoProcessing_Callback::Destroy(callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback and register but instance is nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_31, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Create(&callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnError(callback, OnError);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnState(callback, OnState);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback and register but callback is nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_32, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_BindOnError(callback, OnError);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnState(callback, OnState);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    ret = OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// start processing with flush surface then stop
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_33, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    OH_VideoProcessingCallback_BindOnError(callback, OnError);
    OH_VideoProcessingCallback_BindOnState(callback, OnState);
    OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    OH_VideoProcessing_SetParameter(instance, parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    OH_VideoProcessing_GetSurface(instance, &window);
    OH_VideoProcessing_GetSurface(instance2, &window2);
    OH_VideoProcessing_SetSurface(instance, window2);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessing_Start(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = OH_VideoProcessing_Stop(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// start processing with flush surface then stop impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_33_1, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_VideoProcessing::Create(&instance2, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_Callback::Create(&callback);
    callback->GetObj()->BindOnError(OnError);
    callback->GetObj()->BindOnState(OnState);
    callback->GetObj()->BindOnNewOutputBuffer(OnNewOutputBuffer);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    instance->GetObj()->RegisterCallback(callback, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    instance->GetObj()->SetParameter(parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    instance->GetObj()->GetSurface(&window);
    instance->GetObj()->GetSurface(&window2);
    instance->GetObj()->SetSurface(window2);
    VideoProcessing_ErrorCode ret =  instance->GetObj()->Start();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = instance->GetObj()->Stop();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_Callback::Destroy(callback);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing::Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// bind on null callback
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_34, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    OH_VideoProcessingCallback_BindOnError(callback, nullptr);
    OH_VideoProcessingCallback_BindOnState(callback, nullptr);
    OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, nullptr);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    OH_VideoProcessing_SetParameter(instance, parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    OH_VideoProcessing_GetSurface(instance, &window);
    OH_VideoProcessing_GetSurface(instance2, &window2);
    OH_VideoProcessing_SetSurface(instance, window2);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessing_Start(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = OH_VideoProcessing_Stop(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// bind on null callback impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_34_1, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_VideoProcessing::Create(&instance2, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_Callback::Create(&callback);
    callback->GetObj()->BindOnError(nullptr);
    callback->GetObj()->BindOnState(nullptr);
    callback->GetObj()->BindOnNewOutputBuffer(nullptr);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    instance->GetObj()->RegisterCallback(callback, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    instance->GetObj()->SetParameter(parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    instance->GetObj()->GetSurface(&window);
    instance->GetObj()->GetSurface(&window2);
    instance->GetObj()->SetSurface(window2);
    VideoProcessing_ErrorCode ret =  instance->GetObj()->Start();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = instance->GetObj()->Stop();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_Callback::Destroy(callback);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing::Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// register null callback
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_34_2, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    OH_VideoProcessing_RegisterCallback(instance, nullptr, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    OH_VideoProcessing_SetParameter(instance, parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    OH_VideoProcessing_GetSurface(instance, &window);
    OH_VideoProcessing_GetSurface(instance2, &window2);
    OH_VideoProcessing_SetSurface(instance, window2);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessing_Start(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = OH_VideoProcessing_Stop(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// register null callback impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_34_3, TestSize.Level1)
{
    OHNativeWindowBuffer *ohNativeWindowBuffer;
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    OH_VideoProcessing::Create(&instance2, CREATE_TYPE);
    auto userData = VIDEO_PROCESSING_STATE_STOPPED;
    instance->GetObj()->RegisterCallback(nullptr, &userData);
    OH_AVFormat* parameterSetted = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameterSetted, VIDEO_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    instance->GetObj()->SetParameter(parameterSetted);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    instance->GetObj()->GetSurface(&window);
    instance->GetObj()->GetSurface(&window2);
    instance->GetObj()->SetSurface(window2);
    VideoProcessing_ErrorCode ret =  instance->GetObj()->Start();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    ret = instance->GetObj()->Stop();
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing::Destroy(instance);
    OH_VideoProcessing::Destroy(instance2);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// call output buffer
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_35, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_RenderOutputBuffer(instance, 0);
    OH_VideoProcessing_Destroy(instance);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// call output buffer impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_35_1, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing::Create(&instance, CREATE_TYPE);
    instance->GetObj()->RenderOutputBuffer(0);
    OH_VideoProcessing::Destroy(instance);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// callback native
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_36, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    auto callback = std::make_shared<VideoProcessingEngine::VideoProcessingCallbackNative>();
    callback->OnError(instance, VIDEO_PROCESSING_SUCCESS, nullptr);
    callback->OnState(instance, VIDEO_PROCESSING_STATE_RUNNING, nullptr);
    callback->OnNewOutputBuffer(instance, 0, nullptr);
}

sptr<SurfaceBuffer> CreateSurfaceBuffer(uint32_t pixelFormat, int32_t width, int32_t height)
{
    auto buffer = SurfaceBuffer::Create();
    if (buffer == nullptr) {
        return nullptr;
    }
    if (width <= 0 || height <= 0) {
        return nullptr;
    }
    BufferRequestConfig inputCfg;
    inputCfg.width = width;
    inputCfg.height = height;
    inputCfg.strideAlignment = width;
    inputCfg.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    inputCfg.format = pixelFormat;
    inputCfg.timeout = 0;
    GSError err = buffer->Alloc(inputCfg);
    if (err != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}

// bind callback after start
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_37, TestSize.Level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Start(instance);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessingCallback_BindOnError(nullptr, OnErrorEmptyCallback);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// sample multi thread start
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_38, TestSize.Level1)
{
    std::unique_ptr<VideoSample> sample = std::make_unique<VideoSample>();
    sample->SetQualityLevel(VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    VideoProcessParam Param = {NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 480, 360,
                                NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 1280, 720};
    int32_t ret = sample->InitVideoSample(Param);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    sample->StartProcess();
    EXPECT_EQ(sample->WaitAndStopSample(), VIDEO_PROCESSING_SUCCESS);
}

// sample multi thread start Impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_39, TestSize.Level1)
{
    std::unique_ptr<VideoSample> sample = std::make_unique<VideoSample>();
    sample->SetImplLoader(true);
    sample->SetQualityLevel(VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    VideoProcessParam Param = {NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 480, 360,
                                NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 1280, 720};
    int32_t ret = sample->InitVideoSampleImpl(Param);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    sample->StartProcessImpl();
    EXPECT_EQ(sample->WaitAndStopSampleImpl(), VIDEO_PROCESSING_SUCCESS);
}

// set surface on running Impl
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_40, TestSize.Level1)
{
    std::unique_ptr<VideoSample> sample = std::make_unique<VideoSample>();
    sample->SetImplLoader(true);
    sample->SetQualityLevel(VIDEO_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    VideoProcessParam Param = {NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 480, 360,
                                NATIVEBUFFER_PIXEL_FMT_YCRCB_420_P, 1280, 720};
    int32_t ret = sample->InitVideoSampleImpl(Param);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    sample->StartProcessImpl();
    EXPECT_EQ(sample->SetSurfaceOnRunningImpl(), VIDEO_PROCESSING_SUCCESS);
    EXPECT_EQ(sample->WaitAndStopSampleImpl(), VIDEO_PROCESSING_SUCCESS);
}

HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_41, TestSize.Level1)
{
    DetailEnhancer enhancer;
    enhancer.Initialize();
    enhancer.Initialize();
    sptr<SurfaceBuffer> input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    sptr<SurfaceBuffer> output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    AlgoErrorCode ret = enhancer.Process(input, output);
    EXPECT_NE(ret, ALGO_SUCCESS);
    enhancer.Deinitialize();
    enhancer.Deinitialize();
}

#ifdef SKIA_ENABLE
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_42, TestSize.Level1)
{
    Media::VideoProcessingEngine::Skia skiaImpl;
    sptr<SurfaceBuffer> input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YUV_422_I, 1024, 1024);
    sptr<SurfaceBuffer> output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YUV_422_I, 1024, 1024);
    AlgoErrorCode ret = skiaImpl.Process(input, output);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YUV_422_I, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_NE(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_BGRA_8888, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_BGRA_8888, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_SP, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_SP, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_P, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCBCR_420_P, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
    input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_P, 1024, 1024);
    output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_YCRCB_420_P, 1024, 1024);
    ret = skiaImpl.Process(input, output);
    EXPECT_EQ(ret, ALGO_SUCCESS);
}
#endif

// native impl inner test
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_43, TestSize.Level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessingNative videoNative(instance);
    ret = videoNative.InitializeInner();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OHOS::Media::Format parameter;
    videoNative.SetSurface(nullptr);
    videoNative.GetSurface(nullptr);
    OH_VideoProcessing* instanceTmp = nullptr;
    OH_VideoProcessing::Create(&instanceTmp, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    struct Region region;
    struct Region::Rect *rect = new Region::Rect();
    rect->x = 0;
    rect->y = 0;
    rect->w = DEFAULT_WIDTH;
    rect->h = DEFAULT_HEIGHT;
    region.rects = rect;
    NativeWindowHandleOpt(window, SET_UI_TIMESTAMP, GetSystemTime());
    instanceTmp->GetObj()->GetSurface(&window);
    videoNative.SetSurface(window);
    videoNative.GetSurface(nullptr);
    ret = videoNative.SetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = videoNative.GetParameter(parameter);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    sptr<SurfaceBuffer> input = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    sptr<SurfaceBuffer> output = CreateSurfaceBuffer(OHOS::GRAPHIC_PIXEL_FMT_RGBA_8888, 1024, 1024);
    videoNative.ProcessBuffers();
    ret = videoNative.Process(input, output);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    videoNative.UpdateRequestCfg(input, bufferConfig1);
    videoNative.UpdateRequestCfg(input, bufferConfig2);
    videoNative.UpdateRequestCfg(input, bufferConfig3);
    videoNative.UpdateRequestCfg(input, bufferConfig4);
    videoNative.Deinitialize();
    videoNative.SetProducerSurface(*window, bufferConfig4);
    videoNative.OnError(VIDEO_PROCESSING_SUCCESS);
    videoNative.OnNewOutputBuffer(1);
    videoNative.OnProducerBufferReleased();
    videoNative.RenderOutputBufferInner(1);
    videoNative.ProcessBuffers();
    std::unique_lock<std::mutex> lock;
    videoNative.WaitCV([this] {return 1;}, lock);
}

HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_44, TestSize.Level1)
{
    VideoProcessingNdkLoader::Get().UnloadLibrary();
    VideoProcessingNdkLoader::Get().LoadLibrary();
    VideoProcessingNdkLoader::Get().UnloadLibrary();
    VideoProcessingNdkLoader::Get().LoadLibrary();
    bool ret = VideoProcessingNdkLoader::Get().LoadLibrary();
    EXPECT_EQ(ret, true);
    VideoProcessingNdkLoader::Get().UnloadLibrary();
    VideoProcessingNdkLoader::Get().UnloadLibrary();
    VideoProcessingNdkLoader::Get().UnloadLibrary();
}

}
} // namespace Media
} // namespace OHOS
