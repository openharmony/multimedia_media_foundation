/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Core
 * @{
 *
 * @brief The AudioVivid module provides builder for audio vivid meta data.
 *
 * @since 26.0.0
 */

/**
 * @file native_audio_vivid.h
 *
 * @brief Declared functions and enumerations related to Audio Vivid.
 *
 * @kit AVCodecKit
 * @library libnative_media_core.so
 * @syscap SystemCapability.Multimedia.Media.Core
 * @since 26.0.0
 */

#ifndef NATIVE_AUDIO_VIVID_H
#define NATIVE_AUDIO_VIVID_H

#include "native_avformat.h"
#include "native_averrors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the signal format for Audio Vivid encoder.
 *
 * @since 26.0.0
 */
typedef enum OH_AudioVividSignalFormat {
    /**
     * Audio Vivid signal format is mono, encoder accept 1 channel data and mark channel layout {@link CH_LAYOUT_MONO}.
     * @since 26.0.0
     */
    OH_AUDIO_VIVID_SIGNAL_FORMAT_MONO = 0,
    /**
     * Audio Vivid signal format is stereo, encoder accept 2 channels data and mark channel
     * layout {@link CH_LAYOUT_STEREO}.
     * @since 26.0.0
     */
    OH_AUDIO_VIVID_SIGNAL_FORMAT_STEREO = 1,
    /**
     * Audio Vivid signal format is multiple channels, encoder supports {@link CH_LAYOUT_5POINT1},
     * {@link CH_LAYOUT_5POINT1POINT2}, {@link CH_LAYOUT_5POINT1POINT4}, {@link CH_LAYOUT_7POINT1},
     * {@link CH_LAYOUT_7POINT1_POINT2}, {@link CH_LAYOUT_7POINT1_POINT4}.
     * @since 26.0.0
     */
    OH_AUDIO_VIVID_SIGNAL_FORMAT_MC = 2,
    /**
     * Audio Vivid signal format is mix, contain soundbed and object sounds. Soundbed supports {@link CH_LAYOUT_MONO},
     * {@link CH_LAYOUT_STEREO}, {@link CH_LAYOUT_5POINT1}, {@link CH_LAYOUT_5POINT1POINT2},
     * {@link CH_LAYOUT_5POINT1POINT4}, {@link CH_LAYOUT_7POINT1},
     * {@link CH_LAYOUT_7POINT1_POINT2}, {@link CH_LAYOUT_7POINT1_POINT4}.
     * @since 26.0.0
     */
    OH_AUDIO_VIVID_SIGNAL_FORMAT_MIX = 4,
} OH_AudioVividSignalFormat;

/**
 * @brief Represents a position in Cartesian coordinates.
 *
 * Cartesian coordinates use x, y, and z axes to define a postion in three-dimensional space.
 *
 * @since 26.0.0
 */
typedef struct OH_CartesianPosition {
    /**
     * Normalized X coordinate of an object's left/right dimension when object is in the Cartesian coordinate system.
     * Value range is [-1.0, 1.0].
     * @since 26.0.0
     */
    float x;
    /**
     * Normalized Y coordinate of an object's front/back dimension when object is in the Cartesian coordinate system.
     * Value range is [-1.0, 1.0].
     * @since 26.0.0
     */
    float y;
    /**
     * Normalized Z coordinate of an object's upper/lower dimension when object is in the Cartesian coordinate system.
     * Value range is [-1.0, 1.0].
     * @since 26.0.0
     */
    float z;
} OH_CartesianPosition;

/**
 * @brief Represents a position in polar (spherical) coordinates.
 *
 * Polar coordinates use azimuth, elevation, and distance to define a position in three-dimensional space.
 * @since 26.0.0
 */
typedef struct OH_PolarPosition {
    /**
     * Indicates the azimuth angle of the object's location when the polar coordinate system is used.
     * Value range is [-180.0, 180.0], where 0.0 is front, 90.0 is right, -90.0 is left.
     * @since 26.0.0
     */
    float azimuth;
    /**
     * Indicates the elevation angle of the object's location when the polar coordinate system is used.
     * Value range is [-90.0, 90.0], where 0.0 is horizontal, 90.0 is up, -90.0 is down.
     * @since 26.0.0
     */
    float elevation;
    /**
     * Normalized distance of an object's location when an object is placed in the polar coordinate system.
     * Value range is [0.0, 1.0].
     * @since 26.0.0
     */
    float distance;
} OH_PolarPosition;

/**
 * @brief Represents the position of an audio object in three-dimensional space.
 *
 * The position can be expressed in either Cartesian or polar coordinates.
 *
 * @since 26.0.0
 */
typedef struct OH_AudioObjectPosition {
    /**
     * Indicates whether the position is in Cartesian coordinates.
     * true: use pos.cartesian, false: use pos.polar.
     * @since 26.0.0
     */
    bool isCartesian;
    /**
     * @brief Union containing the position data in either Cartesian or polar coordinates.
     *
     * @since 26.0.0
     */
    union {
        /**
         * Represents position by Cartesian coordinates.
         * @since 26.0.0
         */
        OH_CartesianPosition cartesian;
        /**
         * Represents position by polar coordinates.
         * @since 26.0.0
         */
        OH_PolarPosition polar;
    } pos;
} OH_AudioObjectPosition;

/**
 * @brief Forward declaration of OH_AudioVividMetaBuilder.
 *
 * @since 26.0.0
 */
typedef struct OH_AudioVividMetaBuilderStruct OH_AudioVividMetaBuilder;

/**
 * @brief Creates an Audio Vivid metadata builder.
 *
 * @param builder Pointer to retrieve an OH_AudioVividMetaBuilder instance pointer.
 * @param format Pointer to the OH_AVFormat containing audio format information.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @note **Lifecycle Management:**
 *      The instance created by this function must be manually released by calling
 *      {@link OH_AudioVividMetaBuilder_Destroy} when it is no longer needed to
 *      prevent memory leaks.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_Create(OH_AudioVividMetaBuilder **builder, const OH_AVFormat *format);

/**
 * @brief Updates the position of an audio object in with Audio Vivid signal format {@link OH_VIVID_SIGNAL_FORMAT_MIX}
 *
 * In this signal format, the PCM channels are arranged as: soundbed channels followed by object channels in order.
 * The object channels are matched with objectIndex starting from 0.
 *
 * @param builder Pointer to the OH_AudioVividMetaBuilder.
 * @param objectIndex Index of the audio object to update, starting from 0.
 * @param pos The new position for the audio object.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_UpdateObjectPos(OH_AudioVividMetaBuilder *builder,
    int32_t objectIndex, OH_AudioObjectPosition pos);

/**
 * @brief Updates the rendering gain of an audio object.
 *
 * @param builder Pointer to the OH_AudioVividMetaBuilder.
 * @param objectIndex Index of the audio object to update, starting from 0.
 * @param gain The object rendering gain value, range is [0.0, 6.0]. This is optional; if not set, no gain is applied.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_UpdateObjectGain(OH_AudioVividMetaBuilder *builder,
    int32_t objectIndex, float gain);

/**
 * @brief Gets the length of the metadata.
 *
 * @param builder Pointer to the OH_AudioVividMetaBuilder.
 * @param withStaticMeta If set to true, the output len will include static metadata;
 * if false, only include dynamic metadata.
 * @param len Pointer to receive the metadata length in bytes.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_GetMetaLen(const OH_AudioVividMetaBuilder *builder, bool withStaticMeta,
    int32_t *len);

/**
 * @brief Gets the metadata data.
 *
 * @param builder Pointer to the OH_AudioVividMetaBuilder.
 * @param withStaticMeta If set to true, the output buffer will include static metadata;
 * if false, only include dynamic metadata.
 * @param buffer Pointer to the buffer to receive the metadata data.
 * @param len Length of the buffer in bytes.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_GetMeta(const OH_AudioVividMetaBuilder *builder, bool withStaticMeta,
    uint8_t *buffer, int32_t len);

/**
 * @brief Destroys the Audio Vivid metadata builder and releases resources.
 *
 * @param builder Pointer to the OH_AudioVividMetaBuilder to be destroyed.
 * @return Returns AV_ERR_OK if successful, otherwise returns an error code.
 * @since 26.0.0
 */
OH_AVErrCode OH_AudioVividMetaBuilder_Destroy(OH_AudioVividMetaBuilder *builder);

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AUDIO_VIVID_H

/** @} */