/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "meta/meta.h"
#include <functional>
#include "common/log.h"
#include "meta.h"

/**
 * Steps of Adding New Tag
 *
 * 1. In meta_key.h, Add a Tag.
 * 2. In meta.h, Register Tag key Value mapping.
 *    Example: DEFINE_INSERT_GET_FUNC(tagCharSeq == Tag::TAGNAME, TAGTYPE, AnyValueType::VALUETYPE)
 * 3. In meta.cpp, Register default value to g_metadataDefaultValueMap ({Tag::TAGNAME, defaultTAGTYPE}).
 * 4. In order to support Enum/Bool Value Getter Setter from AVFormat,
 *    In meta.cpp, Register Tag key getter setter function mapping.
 *    Example: DEFINE_METADATA_SETTER_GETTER_FUNC(SrcTAGNAME, int32_t/int64_t)
 *    For Int32/Int64 Type, update g_metadataGetterSetterMap/g_metadataGetterSetterInt64Map.
 *    For Bool Type, update g_metadataBoolVector.
 * 5. Update meta_func_unit_test.cpp to add the testcase of new added Tag Type.
 *
 * Theory:
 * App --> AVFormat(ndk) --> Meta --> Parcel(ipc) --> Meta
 * AVFormat only support: int, int64(Long), float, double, string, buffer
 * Parcel only support: bool, int8, int16, int32, int64, uint8, uint16, uint32, uint64, float, double, pointer, buffer
 * Meta (based on any) support : all types in theory.
 *
 * Attention: Use AVFormat with Meta, with or without ipc, be care of the difference of supported types.
 * Currently, The ToParcel/FromParcel function(In Any.h) supports single value convert to/from parcel.
 * you can use meta's helper functions to handle the key and the correct value type:
 *    GetDefaultAnyValue: get the specified key's default value, It can get the value type.
 *    SetMetaData/GetMetaData: AVFormat use them to set/get enum/bool/int values,
 *    It can convert the integer to/from enum/bool automatically.
 **/
namespace OHOS {
namespace Media {
using namespace Plugins;

#define DEFINE_METADATA_SETTER_GETTER_FUNC(EnumTypeName, ExtTypeName)                       \
static bool Set##EnumTypeName(Meta& meta, const TagType& tag, ExtTypeName& value)           \
{                                                                                           \
    if (__is_enum(EnumTypeName)) {                                                          \
        meta.SetData(tag, EnumTypeName(value));                                             \
    } else {                                                                                \
        meta.SetData(tag, value);                                                           \
    }                                                                                       \
    return true;                                                                            \
}                                                                                           \
                                                                                            \
static bool Get##EnumTypeName(const Meta& meta, const TagType& tag, ExtTypeName& value)     \
{                                                                                           \
    EnumTypeName tmpValue;                                                                  \
    if (meta.GetData(tag, tmpValue)) {                                                      \
        value = static_cast<ExtTypeName>(tmpValue);                                         \
        return true;                                                                        \
    }                                                                                       \
    return false;                                                                           \
}

DEFINE_METADATA_SETTER_GETTER_FUNC(SrcInputType, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(AudioSampleFormat, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(VideoPixelFormat, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(MediaType, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(VideoH264Profile, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(VideoRotation, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(ColorPrimary, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(TransferCharacteristic, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(MatrixCoefficient, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(HEVCProfile, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(HEVCLevel, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(ChromaLocation, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(FileType, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(VideoEncodeBitrateMode, int32_t)
DEFINE_METADATA_SETTER_GETTER_FUNC(TemporalGopReferenceMode, int32_t)

DEFINE_METADATA_SETTER_GETTER_FUNC(AudioChannelLayout, int64_t)

#define  DEFINE_METADATA_SETTER_GETTER(tag, EnumType) {tag, std::make_pair(Set##EnumType, Get##EnumType)}

using  MetaSetterFunction = std::function<bool(Meta&, const TagType&, int32_t&)>;
using  MetaGetterFunction = std::function<bool(const Meta&, const TagType&, int32_t&)>;

static std::map<TagType, std::pair<MetaSetterFunction, MetaGetterFunction>> g_metadataGetterSetterMap = {
    DEFINE_METADATA_SETTER_GETTER(Tag::SRC_INPUT_TYPE, SrcInputType),
    DEFINE_METADATA_SETTER_GETTER(Tag::AUDIO_SAMPLE_FORMAT, AudioSampleFormat),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_PIXEL_FORMAT, VideoPixelFormat),
    DEFINE_METADATA_SETTER_GETTER(Tag::MEDIA_TYPE, MediaType),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_H264_PROFILE, VideoH264Profile),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_ROTATION, VideoRotation),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_COLOR_PRIMARIES, ColorPrimary),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_COLOR_TRC, TransferCharacteristic),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_COLOR_MATRIX_COEFF, MatrixCoefficient),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_H265_PROFILE, HEVCProfile),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_H265_LEVEL, HEVCLevel),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_CHROMA_LOCATION, ChromaLocation),
    DEFINE_METADATA_SETTER_GETTER(Tag::MEDIA_FILE_TYPE, FileType),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_ENCODE_BITRATE_MODE, VideoEncodeBitrateMode),
    DEFINE_METADATA_SETTER_GETTER(Tag::VIDEO_ENCODER_TEMPORAL_GOP_REFERENCE_MODE, TemporalGopReferenceMode),
};

using  MetaSetterInt64Function = std::function<bool(Meta&, const TagType&, int64_t&)>;
using  MetaGetterInt64Function = std::function<bool(const Meta&, const TagType&, int64_t&)>;
static std::map<TagType, std::pair<MetaSetterInt64Function, MetaGetterInt64Function>> g_metadataGetterSetterInt64Map = {
        DEFINE_METADATA_SETTER_GETTER(Tag::AUDIO_CHANNEL_LAYOUT, AudioChannelLayout),
        DEFINE_METADATA_SETTER_GETTER(Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, AudioChannelLayout)
};

static std::vector<TagType> g_metadataBoolVector = {
    Tag::VIDEO_COLOR_RANGE,
    Tag::VIDEO_FRAME_RATE_ADAPTIVE_MODE,
    Tag::VIDEO_REQUEST_I_FRAME,
    Tag::VIDEO_IS_HDR_VIVID,
    Tag::MEDIA_HAS_VIDEO,
    Tag::MEDIA_HAS_AUDIO,
    Tag::MEDIA_HAS_SUBTITLE,
    Tag::MEDIA_END_OF_STREAM
};

bool SetMetaData(Meta& meta, const TagType& tag, int32_t value)
{
    auto iter = g_metadataGetterSetterMap.find(tag);
    if (iter == g_metadataGetterSetterMap.end()) {
        if (std::find(g_metadataBoolVector.begin(), g_metadataBoolVector.end(), tag) != g_metadataBoolVector.end()) {
            meta.SetData(tag, value != 0 ? true : false);
            return true;
        }
        meta.SetData(tag, value);
        return true;
    }
    return iter->second.first(meta, tag, value);
}

bool GetMetaData(const Meta& meta, const TagType& tag, int32_t& value)
{
    auto iter = g_metadataGetterSetterMap.find(tag);
    if (iter == g_metadataGetterSetterMap.end()) {
        if (std::find(g_metadataBoolVector.begin(), g_metadataBoolVector.end(), tag) != g_metadataBoolVector.end()) {
            bool valueBool = false;
            FALSE_RETURN_V(meta.GetData(tag, valueBool), false);
            value = valueBool ? 1 : 0;
            return true;
        }
        return meta.GetData(tag, value);
    }
    return iter->second.second(meta, tag, value);
}

bool SetMetaData(Meta& meta, const TagType& tag, int64_t value)
{
    auto iter = g_metadataGetterSetterInt64Map.find(tag);
    if (iter == g_metadataGetterSetterInt64Map.end()) {
        meta.SetData(tag, value);
        return true;
    }
    return iter->second.first(meta, tag, value);
}

bool GetMetaData(const Meta& meta, const TagType& tag, int64_t& value)
{
    auto iter = g_metadataGetterSetterInt64Map.find(tag);
    if (iter == g_metadataGetterSetterInt64Map.end()) {
        return meta.GetData(tag, value);
    }
    return iter->second.second(meta, tag, value);
}

bool IsIntEnum(const TagType &tag)
{
    return (g_metadataGetterSetterMap.find(tag) != g_metadataGetterSetterMap.end());
}

bool IsLongEnum(const TagType &tag)
{
    return g_metadataGetterSetterInt64Map.find(tag) != g_metadataGetterSetterInt64Map.end();
}

static Any defaultString = std::string();
static Any defaultInt8 = (int8_t)0;
static Any defaultUInt8 = (uint8_t)0;
static Any defaultInt32 = (int32_t)0;
static Any defaultUInt32 = (uint32_t)0;
static Any defaultInt64 = (int64_t)0;
static Any defaultUInt64 = (uint64_t)0;
static Any defaultFloat = 0.0f;
static Any defaultDouble = (double)0.0;
static Any defaultBool = (bool) false;
static Any defaultSrcInputType = SrcInputType::UNKNOWN;
static Any defaultAudioSampleFormat = AudioSampleFormat::INVALID_WIDTH;
static Any defaultVideoPixelFormat = VideoPixelFormat::UNKNOWN;
static Any defaultMediaType = MediaType::UNKNOWN;
static Any defaultVideoH264Profile = VideoH264Profile::UNKNOWN;
static Any defaultVideoRotation = VideoRotation::VIDEO_ROTATION_0;
static Any defaultColorPrimary = ColorPrimary::BT2020;
static Any defaultTransferCharacteristic = TransferCharacteristic::BT1361;
static Any defaultMatrixCoefficient = MatrixCoefficient::BT2020_CL;
static Any defaultHEVCProfile = HEVCProfile::HEVC_PROFILE_UNKNOW;
static Any defaultHEVCLevel = HEVCLevel::HEVC_LEVEL_UNKNOW;
static Any defaultChromaLocation = ChromaLocation::BOTTOM;
static Any defaultFileType = FileType::UNKNOW;
static Any defaultVideoEncodeBitrateMode = VideoEncodeBitrateMode::CBR;
static Any defaultTemporalGopReferenceMode = TemporalGopReferenceMode::ADJACENT_REFERENCE;

static Any defaultAudioChannelLayout = AudioChannelLayout::UNKNOWN;
static Any defaultAudioAacProfile = AudioAacProfile::ELD;
static Any defaultAudioAacStreamFormat = AudioAacStreamFormat::ADIF;
static Any defaultVectorUInt8 = std::vector<uint8_t>();
static Any defaultVectorUInt32 = std::vector<uint32_t>();
static Any defaultVectorVideoBitStreamFormat = std::vector<VideoBitStreamFormat>();
static std::map<TagType, const Any &> g_metadataDefaultValueMap = {
    {Tag::SRC_INPUT_TYPE, defaultSrcInputType},
    {Tag::AUDIO_SAMPLE_FORMAT, defaultAudioSampleFormat},
    {Tag::VIDEO_PIXEL_FORMAT, defaultVideoPixelFormat},
    {Tag::MEDIA_TYPE, defaultMediaType},
    {Tag::VIDEO_H264_PROFILE, defaultVideoH264Profile},
    {Tag::VIDEO_ROTATION, defaultVideoRotation},
    {Tag::VIDEO_COLOR_PRIMARIES, defaultColorPrimary},
    {Tag::VIDEO_COLOR_TRC, defaultTransferCharacteristic},
    {Tag::VIDEO_COLOR_MATRIX_COEFF, defaultMatrixCoefficient},
    {Tag::VIDEO_H265_PROFILE, defaultHEVCProfile},
    {Tag::VIDEO_H265_LEVEL, defaultHEVCLevel},
    {Tag::VIDEO_CHROMA_LOCATION, defaultChromaLocation},
    {Tag::MEDIA_FILE_TYPE, defaultFileType},
    {Tag::VIDEO_ENCODE_BITRATE_MODE, defaultVideoEncodeBitrateMode},
    {Tag::VIDEO_ENCODER_TEMPORAL_GOP_REFERENCE_MODE, defaultTemporalGopReferenceMode},
    // Int8
    {Tag::RECORDER_HDR_TYPE, defaultInt8},
    // Uint_8
    {Tag::SCREEN_CAPTURE_AV_TYPE, defaultUInt8},
    {Tag::SCREEN_CAPTURE_DATA_TYPE, defaultUInt8},
    {Tag::SCREEN_CAPTURE_STOP_REASON, defaultUInt8},
    // Int32
    {Tag::APP_UID, defaultInt32},
    {Tag::APP_PID, defaultInt32},
    {Tag::APP_TOKEN_ID, defaultInt32},
    {Tag::REQUIRED_IN_BUFFER_CNT, defaultInt32},
    {Tag::REQUIRED_IN_BUFFER_SIZE, defaultInt32},
    {Tag::REQUIRED_OUT_BUFFER_CNT, defaultInt32},
    {Tag::REQUIRED_OUT_BUFFER_SIZE, defaultInt32},
    {Tag::BUFFERING_SIZE, defaultInt32},
    {Tag::WATERLINE_HIGH, defaultInt32},
    {Tag::WATERLINE_LOW, defaultInt32},
    {Tag::AUDIO_CHANNEL_COUNT, defaultInt32},
    {Tag::AUDIO_SAMPLE_RATE, defaultInt32},
    {Tag::AUDIO_SAMPLE_PER_FRAME, defaultInt32},
    {Tag::AUDIO_OUTPUT_CHANNELS, defaultInt32},
    {Tag::AUDIO_MPEG_VERSION, defaultInt32},
    {Tag::AUDIO_MPEG_LAYER, defaultInt32},
    {Tag::AUDIO_AAC_LEVEL, defaultInt32},
    {Tag::AUDIO_OBJECT_NUMBER, defaultInt32},
    {Tag::AUDIO_MAX_INPUT_SIZE, defaultInt32},
    {Tag::AUDIO_MAX_OUTPUT_SIZE, defaultInt32},
    {Tag::VIDEO_WIDTH, defaultInt32},
    {Tag::VIDEO_HEIGHT, defaultInt32},
    {Tag::VIDEO_DELAY, defaultInt32},
    {Tag::VIDEO_MAX_SURFACE_NUM, defaultInt32},
    {Tag::VIDEO_H264_LEVEL, defaultInt32},
    {Tag::MEDIA_TRACK_COUNT, defaultInt32},
    {Tag::AUDIO_AAC_IS_ADTS, defaultInt32},
    {Tag::AUDIO_COMPRESSION_LEVEL, defaultInt32},
    {Tag::AUDIO_BITS_PER_CODED_SAMPLE, defaultInt32},
    {Tag::REGULAR_TRACK_ID, defaultInt32},
    {Tag::VIDEO_SCALE_TYPE, defaultInt32},
    {Tag::VIDEO_I_FRAME_INTERVAL, defaultInt32},
    {Tag::MEDIA_PROFILE, defaultInt32},
    {Tag::VIDEO_ENCODE_QUALITY, defaultInt32},
    {Tag::AUDIO_AAC_SBR, defaultInt32},
    {Tag::AUDIO_FLAC_COMPLIANCE_LEVEL, defaultInt32},
    {Tag::MEDIA_LEVEL, defaultInt32},
    {Tag::VIDEO_STRIDE, defaultInt32},
    {Tag::VIDEO_DISPLAY_WIDTH, defaultInt32},
    {Tag::VIDEO_DISPLAY_HEIGHT, defaultInt32},
    {Tag::VIDEO_PIC_WIDTH, defaultInt32},
    {Tag::VIDEO_PIC_HEIGHT, defaultInt32},
    {Tag::VIDEO_ENCODER_TEMPORAL_GOP_SIZE, defaultInt32},
    {Tag::VIDEO_ENCODER_LTR_FRAME_COUNT, defaultInt32},
    {Tag::VIDEO_PER_FRAME_POC, defaultInt32},
    {Tag::VIDEO_ENCODER_PER_FRAME_USE_LTR, defaultInt32},
    {Tag::VIDEO_CROP_TOP, defaultInt32},
    {Tag::VIDEO_CROP_BOTTOM, defaultInt32},
    {Tag::VIDEO_CROP_LEFT, defaultInt32},
    {Tag::VIDEO_CROP_RIGHT, defaultInt32},
    {Tag::VIDEO_SLICE_HEIGHT, defaultInt32},
    {Tag::VIDEO_ENCODER_QP_MAX, defaultInt32},
    {Tag::VIDEO_ENCODER_QP_MIN, defaultInt32},
    {Tag::FEATURE_PROPERTY_VIDEO_ENCODER_MAX_LTR_FRAME_COUNT, defaultInt32},
    {Tag::OH_MD_KEY_AUDIO_OBJECT_NUMBER, defaultInt32},
    {Tag::AV_CODEC_CALLER_PID, defaultInt32},
    {Tag::AV_CODEC_CALLER_UID, defaultInt32},
    {Tag::AV_CODEC_FORWARD_CALLER_PID, defaultInt32},
    {Tag::AV_CODEC_FORWARD_CALLER_UID, defaultInt32},
    {Tag::VIDEO_DECODER_RATE_UPPER_LIMIT, defaultInt32},
    {Tag::SCREEN_CAPTURE_ERR_CODE, defaultInt32},
    {Tag::SCREEN_CAPTURE_DURATION, defaultInt32},
    {Tag::SCREEN_CAPTURE_START_LATENCY, defaultInt32},
    {Tag::DRM_ERROR_CODE, defaultInt32},
    {Tag::RECORDER_ERR_CODE, defaultInt32},
    {Tag::RECORDER_DURATION, defaultInt32},
    {Tag::RECORDER_VIDEO_BITRATE, defaultInt32},
    {Tag::RECORDER_AUDIO_SAMPLE_RATE, defaultInt32},
    {Tag::RECORDER_AUDIO_CHANNEL_COUNT, defaultInt32},
    {Tag::RECORDER_AUDIO_BITRATE, defaultInt32},
    {Tag::RECORDER_START_LATENCY, defaultInt32},
    // String
    {Tag::MIME_TYPE, defaultString},
    {Tag::MEDIA_FILE_URI, defaultString},
    {Tag::MEDIA_TITLE, defaultString},
    {Tag::MEDIA_ARTIST, defaultString},
    {Tag::MEDIA_LYRICIST, defaultString},
    {Tag::MEDIA_ALBUM, defaultString},
    {Tag::MEDIA_ALBUM_ARTIST, defaultString},
    {Tag::MEDIA_DATE, defaultString},
    {Tag::MEDIA_COMMENT, defaultString},
    {Tag::MEDIA_GENRE, defaultString},
    {Tag::MEDIA_COPYRIGHT, defaultString},
    {Tag::MEDIA_LANGUAGE, defaultString},
    {Tag::MEDIA_DESCRIPTION, defaultString},
    {Tag::USER_TIME_SYNC_RESULT, defaultString},
    {Tag::USER_AV_SYNC_GROUP_INFO, defaultString},
    {Tag::USER_SHARED_MEMORY_FD, defaultString},
    {Tag::MEDIA_AUTHOR, defaultString},
    {Tag::MEDIA_COMPOSER, defaultString},
    {Tag::MEDIA_LYRICS, defaultString},
    {Tag::MEDIA_CODEC_NAME, defaultString},
    {Tag::PROCESS_NAME, defaultString},
    {Tag::MEDIA_CREATION_TIME, defaultString},
    {Tag::AV_CODEC_CALLER_PROCESS_NAME, defaultString},
    {Tag::AV_CODEC_FORWARD_CALLER_PROCESS_NAME, defaultString},
    {Tag::SCREEN_CAPTURE_ERR_MSG, defaultString},
    {Tag::SCREEN_CAPTURE_VIDEO_RESOLUTION, defaultString},
    {Tag::DRM_APP_NAME, defaultString},
    {Tag::DRM_INSTANCE_ID, defaultString},
    {Tag::DRM_ERROR_MESG, defaultString},
    {Tag::RECORDER_ERR_MSG, defaultString},
    {Tag::RECORDER_CONTAINER_MIME, defaultString},
    {Tag::RECORDER_VIDEO_MIME, defaultString},
    {Tag::RECORDER_VIDEO_RESOLUTION, defaultString},
    {Tag::RECORDER_AUDIO_MIME, defaultString},
    // Float
    {Tag::MEDIA_LATITUDE, defaultFloat},
    {Tag::MEDIA_LONGITUDE, defaultFloat},
    // Double
    {Tag::VIDEO_CAPTURE_RATE, defaultDouble},
    {Tag::VIDEO_FRAME_RATE, defaultDouble},
    // Bool
    {Tag::VIDEO_COLOR_RANGE, defaultBool},
    {Tag::VIDEO_REQUEST_I_FRAME, defaultBool},
    {Tag::VIDEO_IS_HDR_VIVID, defaultBool},
    {Tag::MEDIA_HAS_VIDEO, defaultBool},
    {Tag::MEDIA_HAS_AUDIO, defaultBool},
    {Tag::MEDIA_HAS_SUBTITLE, defaultBool},
    {Tag::MEDIA_END_OF_STREAM, defaultBool},
    {Tag::VIDEO_FRAME_RATE_ADAPTIVE_MODE, defaultBool},
    {Tag::VIDEO_ENCODER_ENABLE_TEMPORAL_SCALABILITY, defaultBool},
    {Tag::VIDEO_ENCODER_PER_FRAME_MARK_LTR, defaultBool},
    {Tag::VIDEO_PER_FRAME_IS_LTR, defaultBool},
    {Tag::VIDEO_ENABLE_LOW_LATENCY, defaultBool},
    {Tag::VIDEO_ENCODER_ENABLE_SURFACE_INPUT_CALLBACK, defaultBool},
    {Tag::VIDEO_BUFFER_CAN_DROP, defaultBool},
    {Tag::AUDIO_RENDER_SET_FLAG, defaultBool},
    {Tag::SCREEN_CAPTURE_USER_AGREE, defaultBool},
    {Tag::SCREEN_CAPTURE_REQURE_MIC, defaultBool},
    {Tag::SCREEN_CAPTURE_ENABLE_MIC, defaultBool},
    // Int64
    {Tag::MEDIA_FILE_SIZE, defaultUInt64},
    {Tag::MEDIA_POSITION, defaultUInt64},
    {Tag::APP_FULL_TOKEN_ID, defaultInt64},
    {Tag::MEDIA_DURATION, defaultInt64},
    {Tag::MEDIA_BITRATE, defaultInt64},
    {Tag::MEDIA_START_TIME, defaultInt64},
    {Tag::USER_FRAME_PTS, defaultInt64},
    {Tag::USER_PUSH_DATA_TIME, defaultInt64},
    {Tag::MEDIA_TIME_STAMP, defaultInt64},
    // AudioChannelLayout UINT64_T
    {Tag::AUDIO_CHANNEL_LAYOUT, defaultAudioChannelLayout},
    {Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, defaultAudioChannelLayout},
    // AudioAacProfile UInt8
    {Tag::AUDIO_AAC_PROFILE, defaultAudioAacProfile},
    // AudioAacStreamFormat UInt8
    {Tag::AUDIO_AAC_STREAM_FORMAT, defaultAudioAacStreamFormat},
    // vector<uint8_t>
    {Tag::MEDIA_CODEC_CONFIG, defaultVectorUInt8},
    {Tag::AUDIO_VIVID_METADATA, defaultVectorUInt8},
    {Tag::MEDIA_COVER, defaultVectorUInt8},
    {Tag::AUDIO_VORBIS_IDENTIFICATION_HEADER, defaultVectorUInt8},
    {Tag::AUDIO_VORBIS_SETUP_HEADER, defaultVectorUInt8},
    {Tag::OH_MD_KEY_AUDIO_VIVID_METADATA, defaultVectorUInt8},
    // vector<Plugins::VideoBitStreamFormat>
    {Tag::VIDEO_BIT_STREAM_FORMAT, defaultVectorVideoBitStreamFormat},
    // vector<uint8_t>
    {Tag::DRM_CENC_INFO, defaultVectorUInt8},
    // Uint32
    {Tag::DRM_DECRYPT_AVG_SIZE, defaultUInt32},
    {Tag::DRM_DECRYPT_AVG_DURATION, defaultUInt32},
    {Tag::DRM_DECRYPT_MAX_SIZE, defaultUInt32},
    {Tag::DRM_DECRYPT_MAX_DURATION, defaultUInt32},
    {Tag::DRM_DECRYPT_TIMES, defaultUInt32}
};

static std::map<AnyValueType, const Any &> g_ValueTypeDefaultValueMap = {
    {AnyValueType::INVALID_TYPE, defaultString},
    {AnyValueType::BOOL, defaultBool},
    {AnyValueType::UINT8_T, defaultUInt8},
    {AnyValueType::INT32_T, defaultInt32},
    {AnyValueType::UINT32_T, defaultUInt32},
    {AnyValueType::INT64_T, defaultInt64},
    {AnyValueType::UINT64_T, defaultUInt64},
    {AnyValueType::FLOAT, defaultFloat},
    {AnyValueType::DOUBLE, defaultDouble},
    {AnyValueType::VECTOR_UINT8, defaultVectorUInt8},
    {AnyValueType::VECTOR_UINT32, defaultVectorUInt32},
    {AnyValueType::STRING, defaultString},
};

Any GetDefaultAnyValue(const TagType& tag)
{
    auto iter = g_metadataDefaultValueMap.find(tag);
    if (iter == g_metadataDefaultValueMap.end()) {
        return defaultString; //Default String type
    }
    return iter->second;
}

std::optional<Any> GetDefaultAnyValueOpt(const TagType &tag)
{
    auto iter = g_metadataDefaultValueMap.find(tag);
    if (iter == g_metadataDefaultValueMap.end()) {
        return std::nullopt;
    }
    return iter->second;
}

Any GetDefaultAnyValue(const TagType &tag, AnyValueType type)
{
    auto iter = g_metadataDefaultValueMap.find(tag);
    if (iter == g_metadataDefaultValueMap.end()) {
        auto typeIter = g_ValueTypeDefaultValueMap.find(type);
        if (typeIter != g_ValueTypeDefaultValueMap.end()) {
            return typeIter->second;
        } else {
            return defaultString; //Default String type
        }
    }
    return iter->second;
}

AnyValueType Meta::GetValueType(const TagType& key) const
{
    auto iter = map_.find(key);
    if (iter != map_.end()) {
        if (Any::IsSameTypeWith<int32_t>(iter->second)) {
            return AnyValueType::INT32_T;
        } else if (Any::IsSameTypeWith<bool>(iter->second)) {
            return AnyValueType::BOOL;
        } else if (Any::IsSameTypeWith<int64_t>(iter->second)) {
            return AnyValueType::INT64_T;
        } else if (Any::IsSameTypeWith<float>(iter->second)) {
            return AnyValueType::FLOAT;
        } else if (Any::IsSameTypeWith<double>(iter->second)) {
            return AnyValueType::DOUBLE;
        } else if (Any::IsSameTypeWith<std::vector<uint8_t>>(iter->second)) {
            return AnyValueType::VECTOR_UINT8;
        } else if (Any::IsSameTypeWith<std::string>(iter->second)) {
            return AnyValueType::STRING;
        } else {
            auto iter = g_metadataGetterSetterInt64Map.find(key);
            if (iter == g_metadataGetterSetterInt64Map.end()) {
                return AnyValueType::INT32_T;
            } else {
                return AnyValueType::INT64_T;
            }
        }
    }
    return AnyValueType::INVALID_TYPE;
}

bool Meta::ToParcel(MessageParcel &parcel) const
{
    MessageParcel metaParcel;
    int32_t metaSize = 0;
    bool ret = true;
    for (auto iter = begin(); iter != end(); ++iter) {
        ++metaSize;
        ret &= metaParcel.WriteString(iter->first);
        ret &= metaParcel.WriteInt32(static_cast<int32_t>(GetValueType(iter->first)));
        ret &= iter->second.ToParcel(metaParcel);
        if (!ret) {
            MEDIA_LOG_E("fail to Marshalling Key: " PUBLIC_LOG_S, iter->first.c_str());
            return false;
        }
    }
    if (ret) {
        ret = ret && parcel.WriteInt32(metaSize);
        if (metaSize != 0) {
            ret = ret && parcel.Append(metaParcel);
        }
    }
    return ret;
}

bool Meta::FromParcel(MessageParcel &parcel)
{
    map_.clear();
    int32_t size = parcel.ReadInt32();
    if (size < 0 || static_cast<size_t>(size) > parcel.GetRawDataCapacity()) {
        MEDIA_LOG_E("fail to Unmarshalling size: %{public}d", size);
        return false;
    }

    for (int32_t index = 0; index < size; index++) {
        std::string key = parcel.ReadString();
        AnyValueType type = static_cast<AnyValueType>(parcel.ReadInt32());
        Any value = GetDefaultAnyValue(key, type); //Init Default Value
        if (value.FromParcel(parcel)) {
            map_[key] = value;
        } else {
            MEDIA_LOG_E("fail to Unmarshalling Key: %{public}s", key.c_str());
            return false;
        }
    }
    return true;
}
}
} // namespace OHOS