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

namespace OHOS {
namespace Media {

class DetailEnhancerImageNdkUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DetailEnhancerImageNdkUnitTest::SetUpTestCase(void)
{
}

void DetailEnhancerImageNdkUnitTest::TearDownTestCase(void)
{
}

void DetailEnhancerImageNdkUnitTest::SetUp(void)
{
}

void DetailEnhancerImageNdkUnitTest::TearDown(void)
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
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// initialize and de-initialize
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_02, TestSize.Level1)
{
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_InitializeEnvironment();
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    ret = OH_ImageProcessing_DeinitializeEnvironment();
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// de-initialize without //initialize and de-initialize
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_03, TestSize.Level1)
{
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_DeinitializeEnvironment();
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// create instance
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_04, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
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
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// destroy null instance
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_07, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Destroy(instance);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// set parameter normally
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_08, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, IMAGE_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL, 10);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_SetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// set parameter with null
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_09, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    OH_AVFormat* parameter = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_SetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// set parameter but instance is null
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_10, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_AVFormat* parameter = nullptr;
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_SetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// get parameter after set parameter
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_11, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(parameter, IMAGE_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL,
        IMAGE_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH);
    OH_ImageProcessing_SetParameter(instance, parameter)
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// get parameter but parameter is null
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_12, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_GetParameter(instance, parameter);
    EXPECT_EQ(ret, IMAGE_PROCESSING_SUCCESS);
}

// get parameter but instance is null set
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_13, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_AVFormat* parameter = OH_AVFormat_Create();
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// get parameter but parameter is not null
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_14, TestSize.Level1)
{
    OH_ImageProcessing* instance = nullptr;
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER);
    OH_AVFormat* parameter = OH_AVFormat_Create();
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_GetParameter(instance, parameter);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
}

// colorspaceconversion support check
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_15, TestSize.Level1)
{
    ImageProcessing_ColorSpaceInfo param {
        .metadataType = 0x1,
        .colorSpace = 0x2,
        .pixelFormat = 0x3,
    }
    bool ret = OH_ImageProcessing_IsColorSpaceConversionSupported(&param, &param);
    EXPECT_EQ(ret, false);
}

// conposition support check
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_16, TestSize.Level1)
{
    ImageProcessing_ColorSpaceInfo param {
        .metadataType = 0x1,
        .colorSpace = 0x2,
        .pixelFormat = 0x3,
    }
    bool ret = OH_ImageProcessing_IsCompositionSupported(&param, &param, &param);
    EXPECT_EQ(ret, false);
}

// deconposition support check
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_17, TestSize.Level1)
{
    ImageProcessing_ColorSpaceInfo param {
        .metadataType = 0x1,
        .colorSpace = 0x2,
        .pixelFormat = 0x3,
    }
    bool ret = OH_ImageProcessing_IsDecompositionSupported(&param, &param, &param);
    EXPECT_EQ(ret, false);
}

// metadata generation support check
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_18, TestSize.Level1)
{
    ImageProcessing_ColorSpaceInfo param {
        .metadataType = 0x1,
        .colorSpace = 0x2,
        .pixelFormat = 0x3,
    }
    bool ret = OH_ImageProcessing_IsMetadataGenerationSupported(&param);
    EXPECT_EQ(ret, false);
}

// convert color space
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_19, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_PixelmapNative* srcImg = nullptr;
    OH_PixelmapNative* dstImg = nullptr;
    CreateEmptyPixelmap(&srcImg, 720, 960, 4);
    CreateEmptyPixelmap(&dstImg, 2880, 3840, 4);
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPR_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_ConvertColorSpace(instance, srcImg, dstImg);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    OH_ImageProcessing_DeinitializeEnvironment();
}

// compose
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_20, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_PixelmapNative* srcImg = nullptr;
    OH_PixelmapNative* gainmap = nullptr;
    OH_PixelmapNative* dstImg = nullptr;
    CreateEmptyPixelmap(&srcImg, 720, 960, 4);
    CreateEmptyPixelmap(&gainmap, 720, 960, 4);
    CreateEmptyPixelmap(&dstImg, 720, 960, 4);
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPR_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Compose(instance, srcImg, gainmap, dstImg);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    OH_ImageProcessing_DeinitializeEnvironment();
}

// decompose
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_21, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_PixelmapNative* srcImg = nullptr;
    OH_PixelmapNative* gainmap = nullptr;
    OH_PixelmapNative* dstImg = nullptr;
    CreateEmptyPixelmap(&srcImg, 720, 960, 4);
    CreateEmptyPixelmap(&gainmap, 720, 960, 4);
    CreateEmptyPixelmap(&dstImg, 720, 960, 4);
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPR_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_Decompose(instance, srcImg, gainmap, dstImg);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    OH_ImageProcessing_DeinitializeEnvironment();
}

// generate metadata
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_22, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_PixelmapNative* srcImg = nullptr;
    CreateEmptyPixelmap(&srcImg, 720, 960, 4);
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPR_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_GenerateMetadata(instance, srcImg);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    OH_ImageProcessing_DeinitializeEnvironment();
}

// detail enhance
HWTEST_F(DetailEnhancerImageNdkUnitTest, vpeImageNdk_23, TestSize.Level1)
{
    OH_ImageProcessing_InitializeEnvironment();
    OH_ImageProcessing* instance = nullptr;
    OH_PixelmapNative* srcImg = nullptr;
    OH_PixelmapNative* dstImg = nullptr;
    CreateEmptyPixelmap(&srcImg, 720, 960, 4);
    CreateEmptyPixelmap(&dstImg, 2880, 3840, 4);
    OH_ImageProcessing_Create(&instance, IMAGE_PROCESSING_TYPR_DETAIL_ENHANCER);
    ImageProcessing_ErrorCode ret = OH_ImageProcessing_EnhanceDetail(instance, srcImg, dstImg);
    EXPECT_NE(ret, IMAGE_PROCESSING_SUCCESS);
    OH_ImageProcessing_DeinitializeEnvironment();
}

} // namespace Media
} // namespace OHOS
