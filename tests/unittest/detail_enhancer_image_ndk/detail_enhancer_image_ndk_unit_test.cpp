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

#include <gtest/gtest.h>

#include "image_processing.h"
#include "image_processing_types.h"
#include "native_avformat.h"
#include "pixelmap_native.h"

using namespace std;
using namespace testing::ext;

namespace OHOS{
namespace Media{

class DetailEnhancerVideoNdkUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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

void CreateEmptyPixelmap(OH_PixelmapNative** pixelMap, int32_t width, int32_t height, int format)
{
    OH_Pixelmap_InitializationOptions* options = nullptr;
    (void)OH_PixelmapInitializationOptions_Create(&options);
    (void)OH_PixelmapInitializationOptions_SetWidth(options, width);
    (void)OH_PixelmapInitializationOptions_SetHeight(options, height);
    (void)OH_PixelmapInitializationOptions_SetPixelFormat(options, format);
    (void)OH_PixelmapNative_CreateEmptyPixelmap(options, pixelMap);
}

//initialize environment
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_01, TestSize.Level1)
{
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_InitializeEnvironment();
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}

// initialize and de-initialize
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_02, TestSize.Level1)
{
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_InitializeEnvironment();
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
    ret = OH_ImageProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}

// de-initialize without //initialize and de-initialize
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_03, TestSize.Level1)
{
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_DeinitializeEnvironment();
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}

// create instance
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_04, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}

// create instance with wrong type
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_05, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Create(&instance, 11);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// destroy instance
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_06, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Destroy(instance);
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}



}
}