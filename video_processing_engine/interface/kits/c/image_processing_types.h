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

/**
 * @addtogroup ImageProcessing
 * @{
 *
 * @brief Provide image processing including color space conversion and metadata generation.
 *
 * @since 12
 */

/**
 * @file image_processing_types.h
 *
 * @brief Type definitions for image processing.
 *
 * @library libimage_processing.so
 * @syscap SystemCapability.Multimedia.VideoProcessingEngine
 * @kit Image Kit
 * @since 12
 */

#ifndef VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_TYPES_H
#define VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the object for image processing.
 *
 * Define a null pointer of OH_ImageProcessing and call {@link OH_ImageProcessing_Create} to create an image processing
 * instance. The pointer should be null before creating instance.
 * User can create multiple image processing instances for different processing types.
 *
 * @since 12
 */
typedef struct OH_ImageProcessing OH_ImageProcessing;

/**
 * @brief Forward declaration of OH_PixelmapNative.
 *
 * @since 12
 */
typedef struct OH_PixelmapNative OH_PixelmapNative;

/**
 * @brief Forward declaration of OH_AVFormat.
 *
 * @since 12
 */
typedef struct OH_AVFormat OH_AVFormat;

/**
 * @brief Used to create an image processing instance for color space conversion.
 *
 * Color space conversion includes the conversion of single-layer HDR images to SDR images, as well as
 * the color space conversion of SDR images, and the conversion of SDR images to single-layer HDR images. Some
 * capabilities are supported by vendor. Use {@link OH_ImageProcessing_IsColorSpaceConversionSupported} to query if
 * the conversion is supported between single-layer images.
 *
 * @see OH_ImageProcessing_Create
 * @since 12
 */
extern const int32_t IMAGE_PROCESSING_TYPE_COLOR_SPACE_CONVERSION;

/**
 * @brief Used to create an image processing instance for HDR image composition.
 *
 * HDR image compose includes the conversion from dual-layer HDR images to single-layer HDR images. Some
 * capabilities are supported by vendor. Use {@link OH_ImageProcessing_IsCompositionSupported} to
 * query if the composition is supported from dual-layer HDR image to single-layer HDR image.
 *
 * @see OH_ImageProcessing_Create
 * @since 12
 */
extern const int32_t IMAGE_PROCESSING_TYPE_COMPOSITION;

/**
 * @brief Used to create an image processing instance for HDR image decomposition.
 *
 * HDR image decompose includes the conversion from single-layer HDR images to dual-layer HDR images. Some
 * capabilities are supported by vendor. Use {@link OH_ImageProcessing_IsDecompositionSupported} to
 * query if the decomposition is supported from single-layer image to dual-layer HDR image.
 *
 * @see OH_ImageProcessing_Create
 * @since 12
 */
extern const int32_t IMAGE_PROCESSING_TYPE_DECOMPOSITION;

/**
 * @brief Used to create an image processing instance for metadata generation.
 *
 * Generate HDR Vivid metadata for single-layer image. The capability is supported by vendor. If the capability is not
 * supported, {@link OH_ImageProcessing_Create} returns {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING}.
 *
 * @see OH_ImageProcessing_Create
 * @since 12
 */
extern const int32_t IMAGE_PROCESSING_TYPE_METADATA_GENERATION;

/**
 * @brief Used to create an image processing instance for detail enhancement.
 *
 * Scale or resize images with the specified quality or just enhance details for rendering an image without changing
 * its resolution.
 *
 * @see OH_ImageProcessing_Create
 * @since 12
 */
extern const int32_t IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER;

/**
 * @brief The key is used to specify the quality level for image detail enhancement.
 *
 * See {@link ImageDetailEnhancer_QualityLevel} for its value.
 * Use {@link OH_ImageProcessing_SetParameter} to set the quality level.
 * Use {@link OH_ImageProcessing_GetParameter} to get the current quality level.
 *
 * @see OH_VideoProcessing_SetParameter
 * @see OH_VideoProcessing_GetParameter
 * @since 12
 */
extern const char* IMAGE_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL;

/**
 * @brief The color space information is used for color space conversion capability query.
 *
 * @see OH_ImageProcessing_IsColorSpaceConversionSupported
 * @see OH_ImageProcessing_IsCompositionSupported
 * @see OH_ImageProcessing_IsDecompositionSupported
 * @since 12
 */
typedef struct ImageProcessing_ColorSpaceInfo {
    /** define metadata type */
    int32_t metadataType;
    /** define color space, {@link enum OH_NativeBuffer_ColorSpace} */
    int32_t colorSpace;
    /** define pixel format, {@link enum OH_NativeBuffer_Format} */
    int32_t pixelFormat;
} ImageProcessing_ColorSpaceInfo;

/**
 * @brief The quality level is used for detail enhancement.
 *
 * It is the value of the key parameter {@link IMAGE_DETAIL_ENHANCER_PARAMETER_KEY_QUALITY_LEVEL}.
 *
 * @see OH_ImageProcessing_SetParameter
 * @see OH_ImageProcessing_GetParameter
 * @since 12
 */
typedef enum ImageDetailEnhancer_QualityLevel {
    /** No detail enhancement */
    IMAGE_DETAIL_ENHANCER_QUALITY_LEVEL_NONE,
    /** A low level of detail enhancement quality but with a fast speed. It's the default level */
    IMAGE_DETAIL_ENHANCER_QUALITY_LEVEL_LOW,
    /** A medium level of detail enhancement quality. Its speed is between the low setting and high setting */
    IMAGE_DETAIL_ENHANCER_QUALITY_LEVEL_MEDIUM,
    /** A high level of detail enhancement quality but with a relatively slow speed */
    IMAGE_DETAIL_ENHANCER_QUALITY_LEVEL_HIGH,
} ImageDetailEnhancer_QualityLevel;

/**
 * @brief Image processing error code.
 *
 * @since 12
 */
typedef enum ImageProcessing_ErrorCode {
    /** Operation is successful */
    IMAGE_PROCESSING_SUCCESS,
    /** Parameter is invalid */
    IMAGE_PROCESSING_ERROR_INVALID_PARAMETER = 401,
    /** Some unknown error occurred */
    IMAGE_PROCESSING_ERROR_UNKNOWN = 29200001,
    /** Initialize global environment for image processing failed */
    IMAGE_PROCESSING_ERROR_INITIALIZE_FAILED,
    /** Create image processing instance failed */
    IMAGE_PROCESSING_ERROR_CREATE_FAILED,
    /** Process image failed */
    IMAGE_PROCESSING_ERROR_PROCESS_FAILED,
    /** Processing is not supported */
    IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING,
    /** Operation is not permitted */
    IMAGE_PROCESSING_ERROR_OPERATION_NOT_PERMITTED,
    /** Out of memory */
    IMAGE_PROCESSING_ERROR_NO_MEMORY,
    /** Image processing instance is invalid */
    IMAGE_PROCESSING_ERROR_INVALID_INSTANCE,
    /** Value is invalid. */
    IMAGE_PROCESSING_ERROR_INVALID_VALUE
} ImageProcessing_ErrorCode;

#ifdef __cplusplus
}
#endif

#endif // VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_TYPES_H
/** @} */
