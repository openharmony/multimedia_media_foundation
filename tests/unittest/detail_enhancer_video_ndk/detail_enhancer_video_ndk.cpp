/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "native_avformat.h"
#include "surface/window.h"
#include "external_window.h"

#include "video_processing.h"
#include "video_processing_types.h"

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
    return nanoTime / NANOS_IN_MICRO
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
    int32_t err = OH_NativeWindowFlushBuffer(window, ohNativeWindowBuffer, -1, region);
    delete rect;
    if (err != 0) {
        cout << "FlushBuffer failed" << endl;
        return 1;
    }
    return 0;
}

// initialize context with nillptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_01, TestSize.level1)
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

// create context without initialization
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_02, TestSize.level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_Destroy(instance);
}

// initialize context with wrong type
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_03, TestSize.level1)
{
    int badCreateType = 0x1;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    ret = OH_VideoProcessing_Create(&instance, badCreateType);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
}

// repeated context initialization
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_04, TestSize.level1)
{
    VideoProcessing_ErrorCode ret1 = OH_VideoProcessing_InitializeEnvironment();
    EXPECT_EQ(ret1, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret2 = OH_VideoProcessing_InitializeEnvironment();
    EXPECT_EQ(ret2, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret3 = OH_VideoProcessing_InitializeEnvironment();
    EXPECT_EQ(ret3, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing* instance1 = nullptr;
    VideoProcessing_ErrorCode ret4 = OH_VideoProcessing_Create(&instance1, CREATE_TYPE);
    EXPECT_EQ(ret4, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret5 = OH_VideoProcessing_Destroy(instance1);
    EXPECT_EQ(ret5, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing* instance2 = nullptr;
    VideoProcessing_ErrorCode ret6 = OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    EXPECT_EQ(ret6, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret7 = OH_VideoProcessing_Destroy(instance2);
    EXPECT_EQ(ret7, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret8 = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret8, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret9 = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret9, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret10 = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret10, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret11 = OH_VideoProcessing_InitializeEnvironment();
    EXPECT_EQ(ret11, VIDEO_PROCESSING_SUCCESS);
    VideoProcessing_ErrorCode ret12 = OH_VideoProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret12, VIDEO_PROCESSING_SUCCESS);
}

// destroy context without create
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_05, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// destroy context without create or initialize
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_06, TestSize.level1)
{
    OH_VideoProcessing* instance = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_Destroy(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// support check with null
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_07, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    bool res = OH_VideoProcessing_IsColorSpaceConversionSupported(nullptr, nullptr)
    EXPECT_EQ(ret, false);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set parameter to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_08, TestSize.level1)
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

// set parameter quality level to high
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_09, TestSize.level1)
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

// set parameter quality level to medium
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_10, TestSize.level1)
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

// set parameter quality level to low
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_11, TestSize.level1)
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

// set parameter quality level to none
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_12, TestSize.level1)
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

// get parameter to non-nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_13, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_14, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_AVFormat* parameter = nullptr
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// get parameter after setting to high
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_15, TestSize.level1)
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

// set surface with surface from another instance
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_16, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing* instance2 = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_Create(&instance2, CREATE_TYPE);
    OHNativeWindow* window = nullptr;
    OHNativeWindow* window2 = nullptr;
    OH_VideoProcessing_GetSurface(instance, &window);
    OH_VideoProcessing_GetSurface(instance, &window2);
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_SetSurface(instance, window2);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// set surface with nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_17, TestSize.level1)
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

// get surface to nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_18, TestSize.level1)
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

// create callback function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_19, TestSize.level1)
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

// create and destroy callback function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_20, TestSize.level1)
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

// destroy callback without create
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_21, TestSize.level1)
{
    VideoProcessing_Callback* callback = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessingCallback_Destroy(callback);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
}

// create callback function then register
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_22, TestSize.level1)
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

// create callback but register null function
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_23, TestSize.level1)
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
    OH_VideoProcessing_DeinitializeEnvironment();
    OH_VideoProcessingCallback_Destroy(callback);
}

// createa and destroy callback function with register
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_24, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    VideoProcessing_Callback* callback = nullptr;
    OH_VideoProcessingCallback_Create(&callback);
    OH_VideoProcessingCallback_BindOnError(callback, OnError);
    OH_VideoProcessingCallback_BindOnState(callback, OnState);
    ret = OH_VideoProcessingCallback_BindOnNewOutputBuffer(callback, OnNewOutputBuffer);
    auto userData = nullptr;
    VideoProcessing_ErrorCode ret = OH_VideoProcessing_RegisterCallback(instance, callback, &userData);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret = OH_VideoProcessingCallback_Destroy(callback);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback and register but instance is nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_25, TestSize.level1)
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
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// create callback and register but callback is nullptr
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_26, TestSize.level1)
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
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_27, TestSize.level1)
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
    OH_VideoProcessing_GetSurface(instance, &window2);
    OH_VideoProcessing_SetSurface(instance, window2);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessing_Start(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    res = OH_VideoProcessing_Stop(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// start repeatedly
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_28, TestSize.level1)
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
    OH_VideoProcessing_GetSurface(instance, &window2);
    OH_VideoProcessing_SetSurface(instance, window2);
    VideoProcessing_ErrorCode ret =  OH_VideoProcessing_Start(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    ret =  OH_VideoProcessing_Start(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    ret =  OH_VideoProcessing_Start(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_FORMAT, NV12_FMT_INDEX);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    int fenceFd = -1;
    OH_NativeWindow_NativeWindowRequestBuffer(window, &ohNativeWindowBuffer, &fenceFd);
    FlushSurf(ohNativeWindowBuffer, window);
    res = OH_VideoProcessing_Stop(instance);
    EXPECT_EQ(ret, VIDEO_PROCESSING_SUCCESS);
    res = OH_VideoProcessing_Stop(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    res = OH_VideoProcessing_Stop(instance);
    EXPECT_NE(ret, VIDEO_PROCESSING_SUCCESS);
    OH_VideoProcessingCallback_Destroy(callback);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}

// call output buffer
HWTEST_F(DetailEnhancerVideoNdkUnitTest, vpeVideoNdk_29, TestSize.level1)
{
    OH_VideoProcessing_InitializeEnvironment();
    OH_VideoProcessing* instance = nullptr;
    OH_VideoProcessing_Create(&instance, CREATE_TYPE);
    OH_VideoProcessing_RenderOutputBuffer(instance, 0);
    OH_VideoProcessing_Destroy(instance);
    OH_VideoProcessing_DeinitializeEnvironment();
}
} // namespace Media
} // namespace OHOS
