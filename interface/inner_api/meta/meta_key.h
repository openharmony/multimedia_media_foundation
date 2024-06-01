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

#ifndef MEDIA_FOUNDATION_META_KEY_H
#define MEDIA_FOUNDATION_META_KEY_H

#include "meta/any.h"

namespace OHOS {
namespace Media {
class Tag {
public:
    /* -------------------- regular tag -------------------- */
    static constexpr const char REGULAR_TRACK_ID[] = "track_index"; ///< track id
    static constexpr const char REQUIRED_IN_BUFFER_CNT[] =
        "max_input_buffer_count"; ///< required buffer count of plugin; read only tag
    static constexpr const char REQUIRED_IN_BUFFER_SIZE[] =
        "max_input_size"; ///< required buffer size of plugin; read only tag
    static constexpr const char REQUIRED_OUT_BUFFER_CNT[] =
        "max_output_buffer_count"; ///< required buffer count of plugin; read only tag
    static constexpr const char REQUIRED_OUT_BUFFER_SIZE[] =
        "regular.required.out.buffer.size"; ///< required buffer size of plugin; read only tag
    static constexpr const char BUFFER_ALLOCATOR[] =
        "regular.buffer.allocator";                                          ///< Allocator, allocator to alloc buffers
    static constexpr const char BUFFERING_SIZE[] = "regular.buffering.size"; ///< download buffer size
    static constexpr const char WATERLINE_HIGH[] = "regular.waterline.high"; ///< high waterline
    static constexpr const char WATERLINE_LOW[] = "regular.waterline.low";   ///< low waterline
    static constexpr const char SRC_INPUT_TYPE[] = "regular.src.input.type"; ///< SrcInputType
    static constexpr const char APP_TOKEN_ID[] = "regular.app.token.id";     ///< app token id
    static constexpr const char APP_FULL_TOKEN_ID[] = "regular.app.full.token.id"; ///< app full token id
    static constexpr const char APP_UID[] = "regular.app.uid";                     ///< app uid
    static constexpr const char APP_PID[] = "regular.app.pid";                     ///< app pid
    static constexpr const char AUDIO_RENDER_INFO[] =
        "regular.audio.render.info"; ///< AudioRenderInfo, audio render info
    static constexpr const char AUDIO_INTERRUPT_MODE[] =
        "regular.audio.interrupt.mode";                            ///< AudioInterruptMode, audio interrupt mode
    static constexpr const char VIDEO_SCALE_TYPE[] = "scale_type"; ///< VideoScaleType, video scale type
    static constexpr const char INPUT_MEMORY_TYPE[] = "regular.input.memory.type";   ///< MemoryType
    static constexpr const char OUTPUT_MEMORY_TYPE[] = "regular.output.memory.type"; ///< MemoryType
    static constexpr const char PROCESS_NAME[] = "process_name";                     ///< string, process name
    static constexpr const char AUDIO_RENDER_SET_FLAG[] =
        "regular.audio.render.set.flag"; ///< bool, audio render set flag
    /* -------------------- media tag -------------------- */
    static constexpr const char MIME_TYPE[] = "codec_mime";            ///< @see MimeType
    static constexpr const char MEDIA_CODEC_NAME[] = "codec_name";     ///< codec name
    static constexpr const char MEDIA_TITLE[] = "title";               ///< title
    static constexpr const char MEDIA_ARTIST[] = "artist";             ///< artist
    static constexpr const char MEDIA_LYRICIST[] = "media.lyricist";   ///< lyricist
    static constexpr const char MEDIA_ALBUM[] = "album";               ///< album
    static constexpr const char MEDIA_ALBUM_ARTIST[] = "album_artist"; ///< album artist
    static constexpr const char MEDIA_DATE[] = "date";                 ///< media date, format：YYYY-MM-DD
    static constexpr const char MEDIA_COMMENT[] = "comment";           ///< comment
    static constexpr const char MEDIA_GENRE[] = "genre";               ///< genre
    static constexpr const char MEDIA_COPYRIGHT[] = "copyright";       ///< copyright
    static constexpr const char MEDIA_LANGUAGE[] = "language";         ///< language
    static constexpr const char MEDIA_DESCRIPTION[] = "description";   ///< description
    static constexpr const char MEDIA_LYRICS[] = "lyrics";             ///< lyrics
    static constexpr const char MEDIA_AUTHOR[] = "author";             ///< authoe
    static constexpr const char MEDIA_COMPOSER[] = "composer";         ///< composer
    static constexpr const char MEDIA_CREATION_TIME[] =
        "creation_time"; ///< creation time, string as YYYY-MM-DD HH:MM:SS.XXX or now
    static constexpr const char MEDIA_LATITUDE[] = "latitude";         ///< latitude, float
    static constexpr const char MEDIA_LONGITUDE[] = "longitude";       ///< longitude, float
    static constexpr const char MEDIA_DURATION[] = "duration";         ///< duration based on {@link HST_TIME_BASE}
    static constexpr const char MEDIA_FILE_SIZE[] = "media.file.size"; ///< file size
    static constexpr const char MEDIA_BITRATE[] = "bitrate";           ///< bite rate
    static constexpr const char MEDIA_FILE_URI[] = "media.file.uri";   ///< file uri
    static constexpr const char MEDIA_CODEC_CONFIG[] =
        "codec_config"; ///< codec config. e.g. AudioSpecificConfig for mp4
    static constexpr const char MEDIA_CODEC_MODE[] = "media.codec.mode"; ///< codec mode.
    static constexpr const char MEDIA_POSITION[] = "media.position";     ///< The byte position within media stream/file
    static constexpr const char MEDIA_START_TIME[] = "media.start.time"; ///< The start time of one track
    static constexpr const char MEDIA_SEEKABLE[] = "media.seekable"; ///< enum Seekable: Seekable status of the media
    static constexpr const char MEDIA_PLAYBACK_SPEED[] = "media.playback.speed"; ///< double, playback speed
    static constexpr const char MEDIA_TYPE[] =
        "track_type"; ///< enum MediaType: Audio Video Subtitle... int32_t, see {link @MediaTrackType}
    static constexpr const char MEDIA_TRACK_COUNT[] = "track_count";           ///< track count in file
    static constexpr const char MEDIA_FILE_TYPE[] = "file_type";               ///< @see FileType, track type
    static constexpr const char MEDIA_STREAM_TYPE[] = "media.stream.type";     ///< stream type of track data
    static constexpr const char MEDIA_HAS_VIDEO[] = "has_video";               ///< has video track in file
    static constexpr const char MEDIA_HAS_AUDIO[] = "has_audio";               ///< has audio track in file
    static constexpr const char MEDIA_HAS_SUBTITLE[] = "has_subtitle";         ///< has subtitle track in file
    static constexpr const char MEDIA_COVER[] = "cover";                       ///< cover in file
    static constexpr const char MEDIA_PROTOCOL_TYPE[] = "media.protocol.type"; ///< Source protocol type
    static constexpr const char MEDIA_PROFILE[] = "codec_profile";             ///< codec profile, Compatible 4.0
    static constexpr const char MEDIA_LEVEL[] = "codec_level";                 ///< codec level, Compatible 4.0
    static constexpr const char MEDIA_TIME_STAMP[] = "timeStamp";              ///< time stamp
    static constexpr const char MEDIA_END_OF_STREAM[] = "endOfStream";         ///< end of stream

    /* -------------------- audio universal tag -------------------- */
    static constexpr const char AUDIO_CHANNEL_COUNT[] = "channel_count"; ///< audio channel count
    static constexpr const char AUDIO_CHANNEL_LAYOUT[] =
        "channel_layout";                                            ///< @see AudioChannelLayout, stream channel layout
    static constexpr const char AUDIO_SAMPLE_RATE[] = "sample_rate"; ///< sample rate
    static constexpr const char AUDIO_SAMPLE_FORMAT[] = "audio_sample_format";        ///< @see AudioSampleFormat
    static constexpr const char AUDIO_SAMPLE_PER_FRAME[] = "audio_samples_per_frame"; ///< sample per frame
    static constexpr const char AUDIO_OUTPUT_CHANNELS[] = "audio.output.channels";    ///< sink output channel num
    static constexpr const char AUDIO_OUTPUT_CHANNEL_LAYOUT[] =
        "audio.output.channel.layout"; ///< @see AudioChannelLayout, sink output channel layout
    static constexpr const char AUDIO_COMPRESSION_LEVEL[] = "audio_compression_level";         ///< compression level
    static constexpr const char AUDIO_MAX_INPUT_SIZE[] = "audio.max.input.size";         ///< max input size
    static constexpr const char AUDIO_MAX_OUTPUT_SIZE[] = "audio.max.output.size";       ///< max output size
    static constexpr const char AUDIO_BITS_PER_CODED_SAMPLE[] = "bits_per_coded_sample"; ///< bits per coded sample

    /* -------------------- audio specific tag -------------------- */
    static constexpr const char AUDIO_MPEG_VERSION[] = "audio.mpeg.version"; ///< mpeg version
    static constexpr const char AUDIO_MPEG_LAYER[] = "audio.mpeg.layer";     ///< mpeg layer

    static constexpr const char AUDIO_AAC_PROFILE[] = "audio.aac.profile";             ///< @see AudioAacProfile
    static constexpr const char AUDIO_AAC_LEVEL[] = "audio.aac.level";                 ///< acc level
    static constexpr const char AUDIO_AAC_STREAM_FORMAT[] = "audio.aac.stream.format"; ///< @see AudioAacStreamFormat
    static constexpr const char AUDIO_AAC_IS_ADTS[] = "aac_is_adts";                   ///< acc format is adts
    static constexpr const char AUDIO_VIVID_METADATA[] = "audio.vivid.metadata";       ///< audio vivid metadata
    static constexpr const char AUDIO_OBJECT_NUMBER[] = "audio.object.number";         ///< audio object number
    static constexpr const char AUDIO_AAC_SBR[] = "sbr";                               ///< Key for aac sbr
    static constexpr const char AUDIO_FLAC_COMPLIANCE_LEVEL[] = "compliance_level";    ///< Key for compliance level
    static constexpr const char AUDIO_VORBIS_IDENTIFICATION_HEADER[] =
        "identification_header";                                              ///< Key for vorbis identification header
    static constexpr const char AUDIO_VORBIS_SETUP_HEADER[] = "setup_header"; ///< Key for vorbis setup header
    static constexpr const char OH_MD_KEY_AUDIO_OBJECT_NUMBER[] =
        "audio_object_number_key"; ///< Key for audio object number
    static constexpr const char OH_MD_KEY_AUDIO_VIVID_METADATA[] =
        "audio_vivid_metadata_key"; ///< Key for audio vivid metadata

    /* -------------------- video universal tag -------------------- */
    static constexpr const char VIDEO_WIDTH[] = "width";                               ///< video width
    static constexpr const char VIDEO_HEIGHT[] = "height";                             ///< video height
    static constexpr const char VIDEO_PIXEL_FORMAT[] = "pixel_format";                 ///< @see VideoPixelFormat
    static constexpr const char VIDEO_FRAME_RATE[] = "frame_rate";                     ///< video frame rate
    static constexpr const char VIDEO_SURFACE[] = "video.surface";                     ///< @see class Surface
    static constexpr const char VIDEO_MAX_SURFACE_NUM[] = "video.max.surface_num";     ///< max video surface num
    static constexpr const char VIDEO_CAPTURE_RATE[] = "capture_rate";                 ///< double, video capture rate
    static constexpr const char VIDEO_BIT_STREAM_FORMAT[] = "video.bit.stream.format"; ///< @see VideoBitStreamFormat
    static constexpr const char VIDEO_ROTATION[] = "rotation_angle";                   ///< @see VideoRotation
    static constexpr const char VIDEO_COLOR_PRIMARIES[] = "color_primaries";           ///< @see ColorPrimary
    static constexpr const char VIDEO_COLOR_TRC[] = "transfer_characteristics";        ///< @see TransferCharacteristic
    static constexpr const char VIDEO_COLOR_MATRIX_COEFF[] = "matrix_coefficients";    ///< @see MatrixCoefficient
    static constexpr const char VIDEO_COLOR_RANGE[] = "range_flag";                    ///< bool, video color range
    static constexpr const char VIDEO_IS_HDR_VIVID[] = "video_is_hdr_vivid";           ///< bool, video is hdr vivid
    static constexpr const char VIDEO_STRIDE[] = "stride";                             ///< int32_t, video stride
    static constexpr const char VIDEO_DISPLAY_WIDTH[] = "display_width";            ///< int32_t, video display width
    static constexpr const char VIDEO_DISPLAY_HEIGHT[] = "display_height";          ///< int32_t, video display height
    static constexpr const char VIDEO_PIC_WIDTH[] = "video_picture_width";          ///< int32_t, video picture width
    static constexpr const char VIDEO_PIC_HEIGHT[] = "video_picture_height";        ///< int32_t, video picture height
    static constexpr const char VIDEO_FRAME_RATE_ADAPTIVE_MODE[] =
        "frame_rate_adaptive_mode";                            ///< bool, video is framerate adaptive mode
    static constexpr const char VIDEO_DELAY[] = "video_delay"; ///< video delay
    static constexpr const char VIDEO_I_FRAME_INTERVAL[] = "i_frame_interval"; ///< Key for the interval of key frame.
    static constexpr const char VIDEO_REQUEST_I_FRAME[] =
        "req_i_frame"; ///<  Key for the request a I-Frame immediately.
    static constexpr const char VIDEO_ENCODE_BITRATE_MODE[] =
        "video_encode_bitrate_mode"; ///<  Key for video encode bitrate mode, see {link @VideoEncodeBitrateMode}
    static constexpr const char VIDEO_ENCODE_QUALITY[] =
        "quality"; ///<  key for the desired encoding quality,  this key is only supported for encoders that are
                   ///<  configured in constant quality mode
    static constexpr const char VIDEO_ENCODER_ENABLE_TEMPORAL_SCALABILITY[] =
        "video_encoder_enable_temporal_scalability"; ///< bool, key for enable the temporal scalability mode,
                                                     ///< true is enabled, false otherwise. The default value is false.
                                                     ///< This is an optional key that applies only to video encoder. It
                                                     ///< is used in configure.
    static constexpr const char VIDEO_ENCODER_TEMPORAL_GOP_SIZE[] =
        "video_encoder_temporal_gop_size"; ///< int32_t, key for describing the temporal group of picture size, It takes
                                           ///< effect only when temporal level scale is enable. This is an optional key
                                           ///< that applies only to video encoder. It is used in configure.
    static constexpr const char VIDEO_ENCODER_TEMPORAL_GOP_REFERENCE_MODE[] =
        "video_encoder_temporal_gop_reference_mode"; ///< int32_t, key for describing the reference mode in temporal
                                                     ///< group of picture, see enum {@link TemporalGopReferenceMode}.It
                                                     ///< takes effect only when temporal level sacle is enabled. This
                                                     ///< is an optional key that applies only to video encoder. It is
                                                     ///< used in configure.
    static constexpr const char VIDEO_ENCODER_LTR_FRAME_COUNT[] =
        "video_encoder_ltr_frame_count"; ///< int32_t, key for describing the count of used long-term reference frames,
                                       ///< must be within the supported range. To get supported range, you should query
                                       ///< wthether the capability is supported. This is an optional key that applies
                                       ///< only to video encoder. It is used in configure.
    static constexpr const char VIDEO_ENCODER_PER_FRAME_MARK_LTR[] =
        "video_encoder_per_frame_mark_ltr"; ///< bool, key for describing mark this frame as a long term reference
                                            ///< frame, true is mark, false otherwise. It takes effect only when the
                                            ///< count of used long term reference frames is configured. This is an
                                            ///< optional key that applies only to video encoder input loop. It takes
                                            ///< effect immediately.
    static constexpr const char VIDEO_ENCODER_PER_FRAME_USE_LTR[] =
        "video_encoder_per_frame_use_ltr"; ///< int32_t, key for describing the long term reference frame poc referenced
                                           ///< by this frame. This is an optional key that applies only to video
                                           ///< encoder input loop. It takes effect immediately.
    static constexpr const char VIDEO_PER_FRAME_IS_LTR[] =
        "video_per_frame_is_ltr"; ///< bool, key for indicating this frame is a long-term reference frame, true is LTR,
                                  ///< false otherwise. This is an optional key that applies only to video encoder
                                  ///< output loop. It indicates the attribute of the frame.
    static constexpr const char VIDEO_PER_FRAME_POC[] =
        "video_per_frame_poc"; ///< int32_t, key for describing the frame poc. This is an optional key that applies only
                               ///< to video encoder output loop. It indicates the attribute of the frame.
    static constexpr const char VIDEO_CROP_TOP[] =
        "video_crop_top"; ///< int32_t, describing the top-coordinate (y) of the crop rectangle. This is the top-most
                          ///< row included in the crop frame, where row indices start at 0.
    static constexpr const char VIDEO_CROP_BOTTOM[] =
        "video_crop_bottom"; ///< int32_t, describing the bottom-coordinate (y) of the crop rectangle. This is the
                             ///< bottom-most row included in the crop frame, where row indices start at 0.
    static constexpr const char VIDEO_CROP_LEFT[] =
        "video_crop_left"; ///<  int32_t, describing the left-coordinate (x) of the crop rectangle. This is the
                           ///<  left-most column included in the crop frame, where column indices start at 0.
    static constexpr const char VIDEO_CROP_RIGHT[] =
        "video_crop_right"; ///<  int32_t, describing the right-coordinate (x) of the crop rectangle. This is the
                            ///<  right-most column included in the crop frame, where column indices start at 0.
    static constexpr const char VIDEO_SLICE_HEIGHT[] =
        "video_slice_height"; ///< int32_t, describing the plane height of a multi-planar (YUV) video buffer
                              ///< layout. Slice height (or plane height/vertical stride) is the number of rows that
                              ///< must be skipped to get from the top of the Y plane to the top of the U plane in the
                              ///< buffer. In essence the offset of the U plane is sliceHeight * stride. The height
                              ///< of the U/V planes can be calculated based on the color format, though it is generally
                              ///< undefined and depends on the device and release.
    static constexpr const char VIDEO_ENABLE_LOW_LATENCY[] =
        "video_enable_low_latency"; ///< bool, key to enable the low latency mode, true is enabled, false
                                    ///< otherwise. If enabled, the video encoder or video decoder doesn't hold input
                                    ///< and output data more than required by the codec standards. This is an optional
                                    ///< key that applies only to video encoder or video decoder. It is used in
                                    ///< configure.
    static constexpr const char VIDEO_ENCODER_QP_MAX[] =
        "video_encoder_qp_max"; ///< int32_t, key for describing the maximum quantization parameter allowed for video
                                ///< encoder. It is used in configure/setparameter or takes effect immediately with the
                                ///< frame.
    static constexpr const char VIDEO_ENCODER_QP_MIN[] =
        "video_encoder_qp_min"; ///< int32_t, key for describing the minimum quantization parameter allowed for video
                                ///< encoder. It is used in configure/setparameter or takes effect immediately with the
                                ///< frame.
    static constexpr const char VIDEO_ENCODER_ENABLE_SURFACE_INPUT_CALLBACK[] =
        "video_encoder_enable_surface_input_callback"; ///< bool, the associated value is an bool (true or false): true
                                                       ///< is enabled, false is closed.
    static constexpr const char VIDEO_DECODER_RATE_UPPER_LIMIT[] =
        "video_decoder_rate_upper_limit"; ///< int32_t, key for upper rate limit of video decoder performance.
    static constexpr const char VIDEO_BUFFER_CAN_DROP[] =
        "video_buffer_can_drop"; ///< bool, key to describe that encoded video buffer can be dropped or not befor
                                 ///< sent to decoder in video playing.

    /* -------------------- video specific tag -------------------- */
    static constexpr const char VIDEO_H264_PROFILE[] = "video.h264.profile"; ///< @see VideoH264Profile
    static constexpr const char VIDEO_H264_LEVEL[] = "video.h264.level";     ///< h264 level
    static constexpr const char VIDEO_H265_PROFILE[] = "video.h265.profile"; ///< @see HEVCProfile
    static constexpr const char VIDEO_H265_LEVEL[] = "video.h265.level";     ///< @see HEVCLevel
    static constexpr const char VIDEO_CHROMA_LOCATION[] = "chroma_location"; ///< @see ChromaLocation

    /* -------------------- user specific tag -------------------- */
    static constexpr const char USER_FRAME_PTS[] = "user.frame.pts"; ///< The user frame pts
    static constexpr const char USER_TIME_SYNC_RESULT[] =
        "user.time.sync.result"; ///< std::string : The time sync result
    static constexpr const char USER_AV_SYNC_GROUP_INFO[] =
        "user.av.sync.group.info"; ///< std::string : The av sync group info
    static constexpr const char USER_SHARED_MEMORY_FD[] =
        "user.shared.memory.fd";                                               ///< std::string : The shared memory fd
    static constexpr const char USER_PUSH_DATA_TIME[] = "user.push.data.time"; ///< The user push data time

    /* -------------------- drm tag -------------------- */
    static constexpr const char DRM_CENC_INFO[] = "drm_cenc_info"; ///< drm cenc info
    static constexpr const char DRM_APP_NAME[] = "APP_NAME"; ///< drm app name
    static constexpr const char DRM_INSTANCE_ID[] = "INSTANCE_ID"; ///< drm instance id
    static constexpr const char DRM_DECRYPT_AVG_SIZE[] = "DECRYPT_AVG_SIZE"; ///< drm decrypt avg size
    static constexpr const char DRM_DECRYPT_AVG_DURATION[] = "DECRYPT_AVG_DURATION"; ///< drm decrypt avg duration
    static constexpr const char DRM_DECRYPT_MAX_SIZE[] = "DECRYPT_MAX_SIZE"; ///< drm decrypt max size
    static constexpr const char DRM_DECRYPT_MAX_DURATION[] = "DECRYPT_MAX_DURATION"; ///< drm decrypt max duration
    static constexpr const char DRM_DECRYPT_TIMES[] = "DECRYPT_TIMES"; ///< drm decrypt times
    static constexpr const char DRM_ERROR_CODE[] = "ERROR_CODE"; ///< drm error code
    static constexpr const char DRM_ERROR_MESG[] = "ERROR_MESG"; ///< drm error message

    /* -------------------- avcapability tag -------------------- */
    static constexpr const char FEATURE_PROPERTY_VIDEO_ENCODER_MAX_LTR_FRAME_COUNT[] =
        "feature_property_video_encoder_max_ltr_frame_count"; ///< int32_t, the key for querying the maximum long
                                                            ///< term reference count
    
    /* -------------------- AVCodec tag -------------------- */
    // Pid is int32_t and process name is string, to describe AVCodec's forward caller info.
    // For example, camera recording, forward caller is camera.
    static constexpr const char AV_CODEC_FORWARD_CALLER_PID[]           = "av_codec_forward_caller_pid";
    static constexpr const char AV_CODEC_FORWARD_CALLER_UID[]           = "av_codec_forward_caller_uid";
    static constexpr const char AV_CODEC_FORWARD_CALLER_PROCESS_NAME[]  = "av_codec_forward_caller_process_name";

    // Pid is int32_t and process name is string, to describe AVCodec's caller info.
    // For example, camera recording, caller is media_service.
    static constexpr const char AV_CODEC_CALLER_PID[]                   = "av_codec_caller_pid";
    static constexpr const char AV_CODEC_CALLER_UID[]                   = "av_codec_caller_uid";
    static constexpr const char AV_CODEC_CALLER_PROCESS_NAME[]          = "av_codec_caller_process_name";
/* -------------------- screen captrue tag -------------------- */
    static constexpr const char SCREEN_CAPTURE_ERR_CODE[] = "errCode";
    static constexpr const char SCREEN_CAPTURE_ERR_MSG[] = "errMsg";
    static constexpr const char SCREEN_CAPTURE_DURATION[] = "captureDuration";
    static constexpr const char SCREEN_CAPTURE_AV_TYPE[] = "avType";
    static constexpr const char SCREEN_CAPTURE_DATA_TYPE[] = "dataType";
    static constexpr const char SCREEN_CAPTURE_USER_AGREE[] = "userAgree";
    static constexpr const char SCREEN_CAPTURE_REQURE_MIC[] = "requireMic";
    static constexpr const char SCREEN_CAPTURE_ENABLE_MIC[] = "enableMic";
    static constexpr const char SCREEN_CAPTURE_VIDEO_RESOLUTION[] = "videoResolution";
    static constexpr const char SCREEN_CAPTURE_STOP_REASON[] = "stopReason";
    static constexpr const char SCREEN_CAPTURE_START_LATENCY[] = "startLatency";

/* -------------------- recorder tag -------------------- */
    static constexpr const char RECORDER_ERR_CODE[] = "errCode";
    static constexpr const char RECORDER_ERR_MSG[] = "errMsg";
    static constexpr const char RECORDER_DURATION[] = "recordDuration";
    static constexpr const char RECORDER_CONTAINER_MIME[] = "containerMime";
    static constexpr const char RECORDER_VIDEO_MIME[] = "videoMime";
    static constexpr const char RECORDER_VIDEO_RESOLUTION[] = "videoResolution";
    static constexpr const char RECORDER_VIDEO_BITRATE[] = "videoBitrate";
    static constexpr const char RECORDER_HDR_TYPE[] = "hdrType";
    static constexpr const char RECORDER_AUDIO_MIME[] = "audioMime";
    static constexpr const char RECORDER_AUDIO_SAMPLE_RATE[] = "audioSampleRate";
    static constexpr const char RECORDER_AUDIO_CHANNEL_COUNT[] = "audioChannelCount";
    static constexpr const char RECORDER_AUDIO_BITRATE[] = "audioBitrate";
    static constexpr const char RECORDER_START_LATENCY[] = "startLatency";
};

using TagTypeCharSeq = const char *;
using TagType = std::string;
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_META_KEY_H
