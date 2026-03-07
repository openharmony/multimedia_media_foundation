/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "audio_vivid_unit_test.h"
#include "native_audio_vivid.h"
#include "native_avformat.h"
#include "meta/meta_key.h"
#include <dlfcn.h>
#include <gtest/gtest.h>

using namespace OHOS;
using namespace OHOS::Media;
using namespace testing::ext;

namespace {

constexpr int32_t OBJECT_NUMBER = 2;
constexpr int32_t OBJECT_INDEX = 0;
constexpr float GAIN_VALUE = 1.5f;
constexpr float CARTESIAN_X = 0.5f;
constexpr float CARTESIAN_Y = 0.3f;
constexpr float CARTESIAN_Z = 0.2f;
constexpr float POLAR_AZIMUTH = 45.0f;
constexpr float POLAR_ELEVATION = 30.0f;
constexpr float POLAR_DISTANCE = 0.8f;
constexpr int64_t LAYOUT_5_1_2 = 206158431759;
constexpr int64_t SOUNDBED_LAYOUT = 3;

constexpr const char* AUDIO_VIVID_LIB_NAME = "libAudioVividMetaBuilder.z.so";

bool IsAudioVividLibAvailable()
{
    void* handle = ::dlopen(AUDIO_VIVID_LIB_NAME, RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr) {
        return false;
    }
    ::dlclose(handle);
    return true;
}

} // namespace

void AudioVividUnitTest::SetUpTestCase(void) {}

void AudioVividUnitTest::TearDownTestCase(void) {}

void AudioVividUnitTest::SetUp(void) {}

void AudioVividUnitTest::TearDown(void) {}

/**
 * @tc.name: AudioVivid_Create_001
 * @tc.desc: Create builder with null builder pointer
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_001, TestSize.Level1)
{
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(nullptr, testFormat);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);

    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_002
 * @tc.desc: Create builder with null format pointer
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_002, TestSize.Level1)
{
    OH_AudioVividMetaBuilder *builder = nullptr;

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, nullptr);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_Create_003
 * @tc.desc: Create builder when libAudioVividMetaBuilder.z.so does not exist
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_003, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_004
 * @tc.desc: Create builder with MONO signal format
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_004, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_005
 * @tc.desc: Create builder with STEREO signal format
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_005, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_STEREO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_006
 * @tc.desc: Create builder with MC signal format (must have AUDIO_CHANNEL_LAYOUT)
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_006, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MC));
    OH_AVFormat_SetLongValue(testFormat, Tag::AUDIO_CHANNEL_LAYOUT, LAYOUT_5_1_2);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_007
 * @tc.desc: Create builder with MIX signal format (must have AUDIO_OBJECT_NUMBER)
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_007, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Create_008
 * @tc.desc: Create builder with MIX signal format with soundbed
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Create_008, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);
    OH_AVFormat_SetLongValue(testFormat, Tag::AUDIO_SOUNDBED_LAYOUT, SOUNDBED_LAYOUT);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        if (builder != nullptr) {
            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_UpdateObjectPos_001
 * @tc.desc: Update object position with null builder
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectPos_001, TestSize.Level1)
{
    OH_AudioObjectPosition pos;
    pos.isCartesian = true;
    pos.pos.cartesian.x = CARTESIAN_X;
    pos.pos.cartesian.y = CARTESIAN_Y;
    pos.pos.cartesian.z = CARTESIAN_Z;

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_UpdateObjectPos(nullptr, OBJECT_INDEX, pos);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_UpdateObjectPos_002
 * @tc.desc: Update object position with Cartesian coordinates in MIX mode
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectPos_002, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            OH_AudioObjectPosition pos;
            pos.isCartesian = true;
            pos.pos.cartesian.x = CARTESIAN_X;
            pos.pos.cartesian.y = CARTESIAN_Y;
            pos.pos.cartesian.z = CARTESIAN_Z;

            ret = OH_AudioVividMetaBuilder_UpdateObjectPos(builder, OBJECT_INDEX, pos);
            EXPECT_EQ(AV_ERR_OK, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_UpdateObjectPos_003
 * @tc.desc: Update object position with polar coordinates in MIX mode
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectPos_003, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            OH_AudioObjectPosition pos;
            pos.isCartesian = false;
            pos.pos.polar.azimuth = POLAR_AZIMUTH;
            pos.pos.polar.elevation = POLAR_ELEVATION;
            pos.pos.polar.distance = POLAR_DISTANCE;

            ret = OH_AudioVividMetaBuilder_UpdateObjectPos(builder, OBJECT_INDEX, pos);
            EXPECT_EQ(AV_ERR_OK, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_UpdateObjectPos_004
 * @tc.desc: Update object position in non-MIX mode (should fail)
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectPos_004, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MC));
    OH_AVFormat_SetLongValue(testFormat, Tag::AUDIO_CHANNEL_LAYOUT, LAYOUT_5_1_2);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            OH_AudioObjectPosition pos;
            pos.isCartesian = true;
            pos.pos.cartesian.x = CARTESIAN_X;
            pos.pos.cartesian.y = CARTESIAN_Y;
            pos.pos.cartesian.z = CARTESIAN_Z;

            ret = OH_AudioVividMetaBuilder_UpdateObjectPos(builder, OBJECT_INDEX, pos);
            EXPECT_EQ(AV_ERR_INVALID_VAL, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_UpdateObjectGain_001
 * @tc.desc: Update object gain with null builder
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectGain_001, TestSize.Level1)
{
    OH_AVErrCode ret = OH_AudioVividMetaBuilder_UpdateObjectGain(nullptr, OBJECT_INDEX, GAIN_VALUE);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_UpdateObjectGain_002
 * @tc.desc: Update object gain in MIX mode
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectGain_002, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            ret = OH_AudioVividMetaBuilder_UpdateObjectGain(builder, OBJECT_INDEX, GAIN_VALUE);
            EXPECT_EQ(AV_ERR_OK, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_UpdateObjectGain_003
 * @tc.desc: Update object gain in non-MIX mode (should fail)
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_UpdateObjectGain_003, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MC));
    OH_AVFormat_SetLongValue(testFormat, Tag::AUDIO_CHANNEL_LAYOUT, LAYOUT_5_1_2);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            ret = OH_AudioVividMetaBuilder_UpdateObjectGain(builder, OBJECT_INDEX, GAIN_VALUE);
            EXPECT_EQ(AV_ERR_INVALID_VAL, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_GetMetaLen_001
 * @tc.desc: Get metadata length with null builder
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMetaLen_001, TestSize.Level1)
{
    int32_t len = 0;
    OH_AVErrCode ret = OH_AudioVividMetaBuilder_GetMetaLen(nullptr, true, &len);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_GetMetaLen_002
 * @tc.desc: Get metadata length with null len pointer
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMetaLen_002, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            ret = OH_AudioVividMetaBuilder_GetMetaLen(builder, true, nullptr);
            EXPECT_EQ(AV_ERR_INVALID_VAL, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_GetMetaLen_003
 * @tc.desc: Get metadata length with static metadata in MIX mode
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMetaLen_003, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            int32_t len = 0;
            ret = OH_AudioVividMetaBuilder_GetMetaLen(builder, true, &len);
            EXPECT_EQ(AV_ERR_OK, ret);
            EXPECT_GT(len, 0);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_GetMetaLen_004
 * @tc.desc: Get metadata length without static metadata (dynamic only)
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMetaLen_004, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            int32_t len = 0;
            ret = OH_AudioVividMetaBuilder_GetMetaLen(builder, false, &len);
            EXPECT_EQ(AV_ERR_OK, ret);
            EXPECT_GE(len, 0);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_GetMeta_001
 * @tc.desc: Get metadata with null builder
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMeta_001, TestSize.Level1)
{
    uint8_t buffer[1024] = {0};
    OH_AVErrCode ret = OH_AudioVividMetaBuilder_GetMeta(nullptr, true, buffer, sizeof(buffer));
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_GetMeta_002
 * @tc.desc: Get metadata with null buffer
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMeta_002, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            ret = OH_AudioVividMetaBuilder_GetMeta(builder, true, nullptr, 1024);
            EXPECT_EQ(AV_ERR_INVALID_VAL, ret);

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_GetMeta_003
 * @tc.desc: Get metadata with valid buffer
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_GetMeta_003, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            int32_t len = 0;
            ret = OH_AudioVividMetaBuilder_GetMetaLen(builder, true, &len);
            EXPECT_EQ(AV_ERR_OK, ret);

            if (len > 0) {
                std::vector<uint8_t> buffer(len);
                ret = OH_AudioVividMetaBuilder_GetMeta(builder, true, buffer.data(), len);
                EXPECT_EQ(AV_ERR_OK, ret);
            }

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_Destroy_001
 * @tc.desc: Destroy builder with null builder
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Destroy_001, TestSize.Level1)
{
    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Destroy(nullptr);
    EXPECT_EQ(AV_ERR_INVALID_VAL, ret);
}

/**
 * @tc.name: AudioVivid_Destroy_002
 * @tc.desc: Destroy builder successfully
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_Destroy_002, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO));

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            ret = OH_AudioVividMetaBuilder_Destroy(builder);
            EXPECT_EQ(AV_ERR_OK, ret);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_FullWorkflow_001
 * @tc.desc: Full workflow test in MIX mode: create, update position, update gain, get meta, destroy
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_FullWorkflow_001, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            OH_AudioObjectPosition pos;
            pos.isCartesian = true;
            pos.pos.cartesian.x = CARTESIAN_X;
            pos.pos.cartesian.y = CARTESIAN_Y;
            pos.pos.cartesian.z = CARTESIAN_Z;

            ret = OH_AudioVividMetaBuilder_UpdateObjectPos(builder, OBJECT_INDEX, pos);
            EXPECT_EQ(AV_ERR_OK, ret);

            ret = OH_AudioVividMetaBuilder_UpdateObjectGain(builder, OBJECT_INDEX, GAIN_VALUE);
            EXPECT_EQ(AV_ERR_OK, ret);

            int32_t len = 0;
            ret = OH_AudioVividMetaBuilder_GetMetaLen(builder, true, &len);
            EXPECT_EQ(AV_ERR_OK, ret);

            if (len > 0) {
                std::vector<uint8_t> buffer(len);
                ret = OH_AudioVividMetaBuilder_GetMeta(builder, true, buffer.data(), len);
                EXPECT_EQ(AV_ERR_OK, ret);
            }

            ret = OH_AudioVividMetaBuilder_Destroy(builder);
            EXPECT_EQ(AV_ERR_OK, ret);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}

/**
 * @tc.name: AudioVivid_MultipleObjects_001
 * @tc.desc: Test updating multiple audio objects in MIX mode
 * @tc.type: FUNC
 */
HWTEST_F(AudioVividUnitTest, AudioVivid_MultipleObjects_001, TestSize.Level1)
{
    bool libAvailable = IsAudioVividLibAvailable();
    
    OH_AudioVividMetaBuilder *builder = nullptr;
    OH_AVFormat *testFormat = OH_AVFormat_Create();
    ASSERT_NE(nullptr, testFormat);
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_VIVID_SIGNAL_FORMAT,
        static_cast<int32_t>(OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX));
    OH_AVFormat_SetIntValue(testFormat, Tag::AUDIO_OBJECT_NUMBER, OBJECT_NUMBER);

    OH_AVErrCode ret = OH_AudioVividMetaBuilder_Create(&builder, testFormat);
    
    if (libAvailable) {
        EXPECT_EQ(AV_ERR_OK, ret);
        EXPECT_NE(nullptr, builder);
        
        if (builder != nullptr) {
            for (int32_t i = 0; i < OBJECT_NUMBER; i++) {
                OH_AudioObjectPosition pos;
                pos.isCartesian = true;
                pos.pos.cartesian.x = static_cast<float>(i) * 0.3f;
                pos.pos.cartesian.y = static_cast<float>(i) * 0.2f;
                pos.pos.cartesian.z = static_cast<float>(i) * 0.1f;

                ret = OH_AudioVividMetaBuilder_UpdateObjectPos(builder, i, pos);
                EXPECT_EQ(AV_ERR_OK, ret);

                ret = OH_AudioVividMetaBuilder_UpdateObjectGain(builder, i, GAIN_VALUE + static_cast<float>(i) * 0.5f);
                EXPECT_EQ(AV_ERR_OK, ret);
            }

            OH_AudioVividMetaBuilder_Destroy(builder);
        }
    } else {
        EXPECT_EQ(AV_ERR_UNSUPPORT, ret);
        EXPECT_EQ(nullptr, builder);
    }
    
    OH_AVFormat_Destroy(testFormat);
}
