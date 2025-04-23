/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef MEDIA_FOUNDATION_VIDEO_TYPES_H
#define MEDIA_FOUNDATION_VIDEO_TYPES_H

#include <cstdint>  // NOLINT: used it

namespace OHOS {
namespace Media {
namespace Plugins {
enum class VideoScaleType {
    VIDEO_SCALE_TYPE_FIT,
    VIDEO_SCALE_TYPE_FIT_CROP,
};

/**
 * @enum Video Pixel Format.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct VideoPixelFormat : int32_t {
    UNKNOWN = -1,
    YUV420P = 0,        ///< planar YUV 4:2:0, 1 Cr & Cb sample per 2x2 Y samples
    YUVI420 = 1,        ///< planar YUV 4:2:0, 1 Cr & Cb sample per 2x2 Y samples
    NV12 = 2,           ///< semi-planar YUV 4:2:0, UVUV...
    NV21 = 3,           ///< semi-planar YUV 4:2:0, VUVU...
    SURFACE_FORMAT = 4, ///< format from surface
    RGBA = 5,           ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    YUV410P,            ///< planar YUV 4:1:0, 1 Cr & Cb sample per 4x4 Y samples
    YUV411P,            ///< planar YUV 4:1:1, 1 Cr & Cb sample per 4x1 Y samples
    YUYV422,            ///< packed YUV 4:2:2, Y0 Cb Y1 Cr
    YUV422P,            ///< planar YUV 4:2:2, 1 Cr & Cb sample per 2x1 Y samples
    YUV444P,            ///< planar YUV 4:4:4, 1 Cr & Cb sample per 1x1 Y samples
    ARGB,               ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    ABGR,               ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    BGRA,               ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    RGB24,              ///< packed RGB 8:8:8, RGBRGB...
    BGR24,              ///< packed RGB 8:8:8, BGRBGR...
    PAL8,               ///< 8 bit with AV_PIX_FMT_RGB32 palette
    GRAY8,              ///< Y
    MONOWHITE,          ///< Y, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    MONOBLACK,          ///< Y, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    YUVJ420P,           ///< planar YUV 4:2:0, 12bpp, full scale (JPEG)
    YUVJ422P,           ///< planar YUV 4:2:2, 16bpp, full scale (JPEG)
    YUVJ444P,           ///< planar YUV 4:4:4, 24bpp, full scale (JPEG)
};

/**
 * @enum Video H264/AVC profile.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct VideoH264Profile : int32_t {
    UNKNOWN,
    BASELINE,  ///< Baseline profile
    MAIN,      ///< Main profile
    EXTENDED,  ///< Extended profile
    HIGH,      ///< High profile
    HIGH10,    ///< High 10 profile
    HIGH422,   ///< High 4:2:2 profile
    HIGH444,   ///< High 4:4:4 profile
};

/**
 * @enum Video Bit Stream format.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct VideoBitStreamFormat : uint32_t {
    UNKNOWN,
    AVC1,  // H264 bit stream format
    HEVC,  // H265 bit stream format
    ANNEXB, // H264, H265 bit stream format
};

enum VideoEncodeBitrateMode : int32_t {
    CBR = 0, // constant bit rate mode.
    VBR = 1, // variable bit rate mode.
    CQ = 2, // constant quality mode.
    CRF = 3, // constant rate factor.
    SQR = 4, // stable quality rate control mode.
    CBR_VIDEOCALL = 10, // constant bit rate mode for video call or meeting scene.
};

enum class ColorPrimary : int32_t {
    BT709 = 1,
    UNSPECIFIED = 2,
    BT470_M = 4,
    BT601_625 = 5,
    BT601_525 = 6,
    SMPTE_ST240 = 7,
    GENERIC_FILM = 8,
    BT2020 = 9,
    SMPTE_ST428 = 10,
    P3DCI = 11,
    P3D65 = 12,
};

enum class TransferCharacteristic : int32_t {
    BT709 = 1,
    UNSPECIFIED = 2,
    GAMMA_2_2 = 4,
    GAMMA_2_8 = 5,
    BT601 = 6,
    SMPTE_ST240 = 7,
    LINEAR = 8,
    LOG = 9,
    LOG_SQRT = 10,
    IEC_61966_2_4 = 11,
    BT1361 = 12,
    IEC_61966_2_1 = 13,
    BT2020_10BIT = 14,
    BT2020_12BIT = 15,
    PQ = 16,
    SMPTE_ST428 = 17,
    HLG = 18,
};

enum class MatrixCoefficient : int32_t {
    IDENTITY = 0,
    BT709 = 1,
    UNSPECIFIED = 2,
    FCC = 4,
    BT601_625 = 5,
    BT601_525 = 6,
    SMPTE_ST240 = 7,
    YCGCO = 8,
    BT2020_NCL = 9,
    BT2020_CL = 10,
    SMPTE_ST2085 = 11,
    CHROMATICITY_NCL = 12,
    CHROMATICITY_CL = 13,
    ICTCP = 14,
};

enum class ChromaLocation {
    UNSPECIFIED = 0,
    LEFT = 1, ///< MPEG-2/4 4:2:0, H.264 default for 4:2:0
    CENTER = 2, ///< MPEG-1 4:2:0, JPEG 4:2:0, H.263 4:2:0
    TOPLEFT = 3, ///< ITU-R 601, SMPTE 274M 296M S314M(DV 4:1:1), mpeg2 4:2:2
    TOP = 4,
    BOTTOMLEFT = 5,
    BOTTOM = 6,
};

enum VideoRotation : int32_t {
    VIDEO_ROTATION_0 = 0,
    VIDEO_ROTATION_90 = 90,
    VIDEO_ROTATION_180 = 180,
    VIDEO_ROTATION_270 = 270,
};

enum VideoOrientationType : int32_t {
    ROTATE_NONE = 0,        /**< No rotation */
    ROTATE_90,              /**< Rotation by 90 degrees */
    ROTATE_180,             /**< Rotation by 180 degrees */
    ROTATE_270,             /**< Rotation by 270 degrees */
    FLIP_H,                 /**< Flip horizontally */
    FLIP_V,                 /**< Flip vertically */
    FLIP_H_ROT90,           /**< Flip horizontally and rotate 90 degrees */
    FLIP_V_ROT90,           /**< Flip vertically and rotate 90 degrees */
    FLIP_H_ROT180,          /**< Flip horizontally and rotate 180 degrees */
    FLIP_V_ROT180,          /**< Flip vertically and rotate 180 degrees */
    FLIP_H_ROT270,          /**< Flip horizontally and rotate 270 degrees */
    FLIP_V_ROT270           /**< Flip vertically and rotate 270 degrees */
};

enum AVCProfile : int32_t {
    AVC_PROFILE_BASELINE = 0,
    AVC_PROFILE_CONSTRAINED_BASELINE = 1,
    AVC_PROFILE_CONSTRAINED_HIGH = 2,
    AVC_PROFILE_EXTENDED = 3,
    AVC_PROFILE_HIGH = 4,
    AVC_PROFILE_HIGH_10 = 5,
    AVC_PROFILE_HIGH_422 = 6,
    AVC_PROFILE_HIGH_444 = 7,
    AVC_PROFILE_MAIN = 8,
};

enum HEVCProfile : int32_t {
    HEVC_PROFILE_MAIN = 0,
    HEVC_PROFILE_MAIN_10 = 1,
    HEVC_PROFILE_MAIN_STILL = 2,
    HEVC_PROFILE_MAIN_10_HDR10 = 3,
    HEVC_PROFILE_MAIN_10_HDR10_PLUS = 4,
    HEVC_PROFILE_UNKNOW = -1,
};

enum MPEG2Profile : int32_t {
    MPEG2_PROFILE_SIMPLE  = 0,
    MPEG2_PROFILE_MAIN    = 1,
    MPEG2_PROFILE_SNR     = 2,
    MPEG2_PROFILE_SPATIAL = 3,
    MPEG2_PROFILE_HIGH    = 4,
    MPEG2_PROFILE_422     = 5,
};

enum MPEG4Profile : int32_t {
    MPEG4_PROFILE_SIMPLE             = 0,
    MPEG4_PROFILE_SIMPLE_SCALABLE    = 1,
    MPEG4_PROFILE_CORE               = 2,
    MPEG4_PROFILE_MAIN               = 3,
    MPEG4_PROFILE_NBIT               = 4,
    MPEG4_PROFILE_HYBRID             = 5,
    MPEG4_PROFILE_BASIC_ANIMATED_TEXTURE = 6,
    MPEG4_PROFILE_SCALABLE_TEXTURE   = 7,
    MPEG4_PROFILE_SIMPLE_FA          = 8,
    MPEG4_PROFILE_ADVANCED_REAL_TIME_SIMPLE  = 9,
    MPEG4_PROFILE_CORE_SCALABLE      = 10,
    MPEG4_PROFILE_ADVANCED_CODING_EFFICIENCY = 11,
    MPEG4_PROFILE_ADVANCED_CORE      = 12,
    MPEG4_PROFILE_ADVANCED_SCALABLE_TEXTURE  = 13,
    MPEG4_PROFILE_SIMPLE_FBA         = 14,
    MPEG4_PROFILE_SIMPLE_STUDIO      = 15,
    MPEG4_PROFILE_CORE_STUDIO        = 16,
    MPEG4_PROFILE_ADVANCED_SIMPLE    = 17,
    MPEG4_PROFILE_FINE_GRANULARITY_SCALABLE  = 18,
};

enum H263Profile : int32_t {
    H263_PROFILE_BASELINE = 0,
    H263_PROFILE_H320_CODING_EFFICIENCY_VERSION2_BACKWARD_COMPATIBILITY = 1, // ffmpeg not support
    H263_PROFILE_VERSION_1_BACKWARD_COMPATIBILITY = 2,
    H263_PROFILE_VERSION_2_INTERACTIVE_AND_STREAMING_WIRELESS = 3, // ffmpeg not support
    H263_PROFILE_VERSION_3_INTERACTIVE_AND_STREAMING_WIRELESS = 4, // ffmpeg not support
    H263_PROFILE_CONVERSATIONAL_HIGH_COMPRESSION = 5, // ffmpeg not support
    H263_PROFILE_CONVERSATIONAL_INTERNET = 6, // ffmpeg not support
    H263_PROFILE_CONVERSATIONAL_PLUS_INTERLACE = 7, // ffmpeg not support
    H263_PROFILE_HIGH_LATENCY = 8 // ffmpeg not support
};

enum VP8Profile : int32_t {
    VP8_PROFILE_MAIN = 0,
};

enum AVCLevel : int32_t {
    AVC_LEVEL_1 = 0,
    AVC_LEVEL_1b = 1,
    AVC_LEVEL_11 = 2,
    AVC_LEVEL_12 = 3,
    AVC_LEVEL_13 = 4,
    AVC_LEVEL_2 = 5,
    AVC_LEVEL_21 = 6,
    AVC_LEVEL_22 = 7,
    AVC_LEVEL_3 = 8,
    AVC_LEVEL_31 = 9,
    AVC_LEVEL_32 = 10,
    AVC_LEVEL_4 = 11,
    AVC_LEVEL_41 = 12,
    AVC_LEVEL_42 = 13,
    AVC_LEVEL_5 = 14,
    AVC_LEVEL_51 = 15,
    AVC_LEVEL_52 = 16,
    AVC_LEVEL_6 = 17,
    AVC_LEVEL_61 = 18,
    AVC_LEVEL_62 = 19,
};

enum HEVCLevel : int32_t {
    HEVC_LEVEL_1 = 0,
    HEVC_LEVEL_2 = 1,
    HEVC_LEVEL_21 = 2,
    HEVC_LEVEL_3 = 3,
    HEVC_LEVEL_31 = 4,
    HEVC_LEVEL_4 = 5,
    HEVC_LEVEL_41 = 6,
    HEVC_LEVEL_5 = 7,
    HEVC_LEVEL_51 = 8,
    HEVC_LEVEL_52 = 9,
    HEVC_LEVEL_6 = 10,
    HEVC_LEVEL_61 = 11,
    HEVC_LEVEL_62 = 12,
    HEVC_LEVEL_UNKNOW = -1,
};

enum MPEG2Level : int32_t {
    MPEG2_LEVEL_LL = 0,
    MPEG2_LEVEL_ML = 1,
    MPEG2_LEVEL_H14 = 2,
    MPEG2_LEVEL_HL = 3,
};

enum MPEG4Level : int32_t {
    MPEG4_LEVEL_0  = 0,
    MPEG4_LEVEL_0B = 1,
    MPEG4_LEVEL_1  = 2,
    MPEG4_LEVEL_2  = 3,
    MPEG4_LEVEL_3  = 4,
    MPEG4_LEVEL_3B = 5,
    MPEG4_LEVEL_4  = 6,
    MPEG4_LEVEL_4A = 7,
    MPEG4_LEVEL_5  = 8,
    MPEG4_LEVEL_6  = 9,
};

enum H263Level : int32_t {
    H263_LEVEL_10 = 0,
    H263_LEVEL_20 = 1,
    H263_LEVEL_30 = 2,
    H263_LEVEL_40 = 3,
    H263_LEVEL_45 = 4,
    H263_LEVEL_50 = 5,
    H263_LEVEL_60 = 6,
    H263_LEVEL_70 = 7
};

/**
 * @enum Temporal group of picture reference mode.
 *
 * @since 5.0
 */
enum class TemporalGopReferenceMode : int32_t {
    ADJACENT_REFERENCE = 0,
    JUMP_REFERENCE = 1,
    UNIFORMLY_SCALED_REFERENCE = 2,
    UNKNOWN,
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_VIDEO_TYPES_H
