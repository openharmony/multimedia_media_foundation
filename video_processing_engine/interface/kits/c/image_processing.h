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
 * @brief Provide APIs for image quality processing.
 *
 * @since 13
 */

/**
 * @file image_processing.h
 *
 * @brief Declare image processing functions.
 *
 * Provides SDR content processing for images, including color space conversion, metadata generation
 * and image scaling.
 *
 * @library libimage_processing.so
 * @syscap SystemCapability.Multimedia.VideoProcessingEngine
 * @kit ImageKit
 * @since 13
 */

#ifndef VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_H
#define VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>
#include "image_processing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize global environment for image processing.
 *
 * This function is optional. \n
 * Typically, this function is called once when the host process is started to initialize the global environment for
 * image processing, which can reduce the time of {@link OH_ImageProcessing_Create}. \n
 * To deinitialize global environment, call {@link OH_ImageProcessing_DeinitializeEnvironment}.
 *
 * @return {@link IMAGE_PROCESSING_SUCCESS} if initialization is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INITIALIZE_FAILED} if initialization is failed. \n
 * You can check if the device GPU is working properly.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_InitializeEnvironment(void);

/**
 * @brief Deinitialize global environment for image processing.
 *
 * This function is required if {@link OH_ImageProcessing_InitializeEnvironment} is called. Typically, this
 * function is called when the host process is about to exit to deinitialize the global environment, which is
 * initialized by calling {@link OH_ImageProcessing_InitializeEnvironment}. \n
 * If there is some image processing instance existing, this function should not be called. \n
 * If the {@link OH_ImageProcessing_InitializeEnvironment} is not called, this function should not be called.
 *
 * @return {@link IMAGE_PROCESSING_SUCCESS} if deinitialization is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_OPERATION_NOT_PERMITTED} if some image processing instance is not destroyed or
 * {@link OH_ImageProcessing_InitializeEnvironment} is not called. \n
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_DeinitializeEnvironment(void);

/**
 * @brief Query whether the image color space conversion is supported.
 *
 * @param sourceImageInfo Input image color space information pointer.
 * @param destinationImageInfo Output image color space information pointer.
 * @return <b>true</b> if the color space conversion is supported. \n
 * <b>false</b> if the the color space conversion is unsupported.
 * @since 13
 */
bool OH_ImageProcessing_IsColorSpaceConversionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo);

/**
 * @brief Query whether the image composition is supported.
 *
 * @param sourceImageInfo Input image color space information pointer.
 * @param sourceGainmapInfo Input gainmap color space information pointer.
 * @param destinationImageInfo Output image color space information pointer.
 * @return <b>true</b> if the image composition is supported. \n
 * <b>false</b> if the image composition is unsupported.
 * @since 13
 */
bool OH_ImageProcessing_IsCompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* sourceGainmapInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo);

/**
 * @brief Query whether the image decomposition is supported.
 *
 * @param sourceImageInfo Input image color space information pointer.
 * @param destinationImageInfo Output image color space information pointer.
 * @param destinationGainmapInfo Output gainmap information pointer.
 * @return <b>true</b> if the image decomposition is supported. \n
 * <b>false</b> if the image decomposition is unsupported.
 * @since 13
 */
bool OH_ImageProcessing_IsDecompositionSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationImageInfo,
    const ImageProcessing_ColorSpaceInfo* destinationGainmapInfo);

/**
 * @brief Query whether the image metadata generation is supported.
 *
 * @param sourceImageInfo Input image color space information pointer.
 * @return <b>true</b> if the image metadata generation is supported.. \n
 * <b>false</b> if the image metadata generation is unsupported.
 * @since 13
 */
bool OH_ImageProcessing_IsMetadataGenerationSupported(
    const ImageProcessing_ColorSpaceInfo* sourceImageInfo);
    
/**
 * @brief Create an image processing instance.
 *
 * @param imageProcessor Output parameter. The *imageProcessor points to a new image processing object.
 * The *imageProcessor must be null before passed in.
 * @param type Use IMAGE_PROCESSING_TYPE_XXX to specify the processing type. The processing type of the instance can not
 * be changed.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if creating an image processing successfully. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the type is not supported. For example, if metadata
 * generation is not supported by vendor, it returns unsupported processing. \n
 * {@link IMAGE_PROCESSING_ERROR_CREATE_FAILED} if failed to create an image processing. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or <b>*</b>instance is <b>not</b> null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if type is invalid. \n
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_Create(OH_ImageProcessing** imageProcessor, int32_t type);

/**
 * @brief Destroy the image processing instance.
 *
 * @param imageProcessor An image processing instance pointer. It is recommended setting the
 * instance pointer to null after the instance is destroyed.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if the instance is destroyed successfully. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_Destroy(OH_ImageProcessing* imageProcessor);

/**
 * @brief Set parameter for image processing.
 *
 * Add parameter identified by the specified parameter key.
 *
 * @param imageProcessor An image processing instance pointer.
 * @param parameter The parameter for image processing.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if setting parameter is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the parameter is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of the parameter is invalid. For example, the parameter
 * contains unsupported parameter key or value. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_SetParameter(OH_ImageProcessing* imageProcessor,
    const OH_AVFormat* parameter);

/**
 * @brief Get parameter of image processing.
 *
 * Get parameter identified by the specified parameter key.
 *
 * @param imageProcessor An image processing instance pointer.
 * @param parameter The parameter used by the image processing instance.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if getting parameter is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the parameter is null. \n
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_GetParameter(OH_ImageProcessing* imageProcessor,
    OH_AVFormat* parameter);

/**
 * @brief Conversion between single-layer images.
 *
 * The function generate the destinationImage from sourceImage. It include the colorspace conversion from
 * HDR image to SDR image, SDR image to HDR image, SDR image to SDR image and HDR image to HDR image.
 *
 * @param imageProcessor An image processing instance pointer. The instance should be created with
 * type {@link IMAGE_PROCESSING_TYPE_COLOR_SPACE_CONVERSION}.
 * @param sourceImage Input image pointer.
 * @param destinationImage Output image pointer.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if processing image is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the image is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of image is invalid. For example, the color space
 * of the image is unsupported. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the processing is not supported. \n
 * {@link IMAGE_PROCESSING_ERROR_PROCESS_FAILED} if processing error occurs. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_ConvertColorSpace(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage);

/**
 * @brief Composition from dual-layer HDR images to single-layer HDR images.
 *
 * The function generate the destinationImage from sourceImage and sourceGainmap.
 *
 * @param imageProcessor An image processing instance pointer. The instance should be created with
 * type {@link IMAGE_PROCESSING_TYPE_COMPOSITION}.
 * @param sourceImage Input image pointer.
 * @param sourceGainmap Input gainmap pointer.
 * @param destinationImage Output image pointer.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if processing image is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the image is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of image is invalid. For example, the color space
 * of the image is unsupported. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the processing is not supported. \n
 * {@link IMAGE_PROCESSING_ERROR_PROCESS_FAILED} if processing error occurs. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_Compose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* sourceGainmap, OH_PixelmapNative* destinationImage);

/**
 * @brief Decomposition from single-layer HDR images to dual-layer HDR images.
 *
 * The function generate the destinationImage and destinationGainmap from sourceImage.
 *
 * @param imageProcessor An image processing instance pointer. The instance should be created with
 * type {@link IMAGE_PROCESSING_TYPE_DECOMPOSITION}.
 * @param sourceImage Input image pointer.
 * @param destinationImage Output image pointer.
 * @param destinationGainmap Output gainmap pointer.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if processing image is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the image is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of image is invalid. For example, the color space
 * of the image is unsupported. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the processing is not supported. \n
 * {@link IMAGE_PROCESSING_ERROR_PROCESS_FAILED} if processing error occurs. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_Decompose(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage, OH_PixelmapNative* destinationGainmap);

/**
 * @brief Metadata Generation for HDR images.
 *
 * The function generate metadata for the sourceImage.
 *
 * @param imageProcessor An image processing instance pointer. The instance should be created with
 * type {@link IMAGE_PROCESSING_TYPE_METADATA_GENERATION}.
 * @param sourceImage Input image pointer.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if processing image is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the image is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of image is invalid. For example, the color space
 * of the image is unsupported. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the processing is not supported. \n
 * {@link IMAGE_PROCESSING_ERROR_PROCESS_FAILED} if processing error occurs. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_GenerateMetadata(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage);

/**
 * @brief Clarity enhancement for images.
 *
 * The function generate the destinationImage from sourceImage with necessary scaling operation according to the size
 * preset in the sourceImage and destinationImage. Different levels of scaling methonds are provided to balance
 * performance and image quality.
 *
 * @param imageProcessor An image processing instance pointer. The instance should be created with
 * type {@link IMAGE_PROCESSING_TYPE_DETAIL_ENHANCER}.
 * @param sourceImage Input image pointer.
 * @param destinationImage Output image pointer.
 * @return {@link IMAGE_PROCESSING_SUCCESS} if processing image is successful. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_INSTANCE} if instance is null or not an image processing instance. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_PARAMETER} if the image is null. \n
 * {@link IMAGE_PROCESSING_ERROR_INVALID_VALUE} if some property of image is invalid. For example, the color space
 * of the image is unsupported. \n
 * {@link IMAGE_PROCESSING_ERROR_UNSUPPORTED_PROCESSING} if the processing is not supported. \n
 * {@link IMAGE_PROCESSING_ERROR_PROCESS_FAILED} if processing error occurs. \n
 * {@link IMAGE_PROCESSING_ERROR_NO_MEMORY} if memory allocation failed.
 * @since 13
 */
ImageProcessing_ErrorCode OH_ImageProcessing_EnhanceDetail(OH_ImageProcessing* imageProcessor,
    OH_PixelmapNative* sourceImage, OH_PixelmapNative* destinationImage);
#ifdef __cplusplus
}
#endif

#endif // VIDEO_PROCESSING_ENGINE_C_API_IMAGE_PROCESSING_H
/** @} */
