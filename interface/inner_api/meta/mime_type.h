/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef MEDIA_FOUNDATION_MIME_TYPE_H
#define MEDIA_FOUNDATION_MIME_TYPE_H

namespace OHOS {
namespace Media {
namespace Plugins {
class MimeType {
public:
    static constexpr const char VIDEO_H263[] = "video/h263";
    static constexpr const char VIDEO_AVC[] = "video/avc";
    static constexpr const char VIDEO_AV1[] = "video/av1";
    static constexpr const char VIDEO_MPEG1[] = "video/mpeg";
    static constexpr const char VIDEO_MPEG2[] = "video/mpeg2";
    static constexpr const char VIDEO_HEVC[] = "video/hevc";
    static constexpr const char VIDEO_MPEG4[] = "video/mp4v-es";
    static constexpr const char VIDEO_VP8[] = "video/x-vnd.on2.vp8";
    static constexpr const char VIDEO_VP9[] = "video/x-vnd.on2.vp9";
    static constexpr const char VIDEO_VVC[] = "video/vvc";
    static constexpr const char VIDEO_VC1[] = "video/vc1";
    static constexpr const char VIDEO_RV30[] = "video/rv30";
    static constexpr const char VIDEO_RV40[] = "video/rv40";
    static constexpr const char VIDEO_MJPEG[] = "video/mjpeg";
    static constexpr const char VIDEO_WMV3[] = "video/wmv3";
    static constexpr const char VIDEO_MSVIDEO1[] = "video/msvideo1";

    static constexpr const char AUDIO_AMR_NB[] = "audio/3gpp";
    static constexpr const char AUDIO_AMR_WB[] = "audio/amr-wb";
    static constexpr const char AUDIO_MPEG[] = "audio/mpeg";
    static constexpr const char AUDIO_AAC[] = "audio/mp4a-latm";
    static constexpr const char AUDIO_VORBIS[] = "audio/vorbis";
    static constexpr const char AUDIO_OPUS[] = "audio/opus";
    static constexpr const char AUDIO_FLAC[] = "audio/flac";
    static constexpr const char AUDIO_RAW[] = "audio/raw";
    static constexpr const char AUDIO_G711MU[] = "audio/g711mu";
    static constexpr const char AUDIO_G711A[] = "audio/g711a";
    static constexpr const char AUDIO_AVS3DA[] = "audio/av3a";
    static constexpr const char AUDIO_LBVC[] = "audio/lbvc";
    static constexpr const char AUDIO_APE[] = "audio/x-ape";
    static constexpr const char AUDIO_COOK[] = "audio/cook";
    static constexpr const char AUDIO_AC3[] = "audio/ac3";
    static constexpr const char AUDIO_EAC3[] = "audio/eac3";
    static constexpr const char AUDIO_GSM[] = "audio/gsm";
    static constexpr const char AUDIO_MIME_L2HC[] = "audio/l2hc";
    static constexpr const char AUDIO_GSM_MS[] = "audio/gsm_ms";
    static constexpr const char AUDIO_WMAV1[] = "audio/wmav1";
    static constexpr const char AUDIO_WMAV2[] = "audio/wmav2";
    static constexpr const char AUDIO_WMAPRO[] = "audio/wmapro";
    static constexpr const char AUDIO_ADPCM_MS[] = "audio/adpcm_ms";
    static constexpr const char AUDIO_ADPCM_IMA_QT[] = "audio/adpcm_ima_qt";
    static constexpr const char AUDIO_ADPCM_IMA_WAV[] = "audio/adpcm_ima_wav";
    static constexpr const char AUDIO_ADPCM_IMA_DK3[] = "audio/adpcm_ima_dk3";
    static constexpr const char AUDIO_ADPCM_IMA_DK4[] = "audio/adpcm_ima_dk4";
    static constexpr const char AUDIO_ADPCM_IMA_WS[] = "audio/adpcm_ima_ws";
    static constexpr const char AUDIO_ADPCM_IMA_SMJPEG[] = "audio/adpcm_ima_smjpeg";
    static constexpr const char AUDIO_ADPCM_IMA_DAT4[] = "audio/adpcm_ima_dat4";
    static constexpr const char AUDIO_ADPCM_MTAF[] = "audio/adpcm_mtaf";
    static constexpr const char AUDIO_ADPCM_ADX[] = "audio/adpcm_adx";
    static constexpr const char AUDIO_ADPCM_AFC[] = "audio/adpcm_afc";
    static constexpr const char AUDIO_ADPCM_AICA[] = "audio/adpcm_aica";
    static constexpr const char AUDIO_ADPCM_CT[] = "audio/adpcm_ct";
    static constexpr const char AUDIO_ADPCM_DTK[] = "audio/adpcm_dtk";
    static constexpr const char AUDIO_ADPCM_G722[] = "audio/adpcm_g722";
    static constexpr const char AUDIO_ADPCM_G726[] = "audio/adpcm_g726";
    static constexpr const char AUDIO_ADPCM_G726LE[] = "audio/adpcm_g726le";
    static constexpr const char AUDIO_ADPCM_IMA_AMV[] = "audio/adpcm_ima_amv";
    static constexpr const char AUDIO_ADPCM_IMA_APC[] = "audio/adpcm_ima_apc";
    static constexpr const char AUDIO_ADPCM_IMA_ISS[] = "audio/adpcm_ima_iss";
    static constexpr const char AUDIO_ADPCM_IMA_OKI[] = "audio/adpcm_ima_oki";
    static constexpr const char AUDIO_ADPCM_IMA_RAD[] = "audio/adpcm_ima_rad";
    static constexpr const char AUDIO_ADPCM_PSX[] = "audio/adpcm_psx";
    static constexpr const char AUDIO_ADPCM_SBPRO_2[] = "audio/adpcm_sbpro_2";
    static constexpr const char AUDIO_ADPCM_SBPRO_3[] = "audio/adpcm_sbpro_3";
    static constexpr const char AUDIO_ADPCM_SBPRO_4[] = "audio/adpcm_sbpro_4";
    static constexpr const char AUDIO_ADPCM_THP[] = "audio/adpcm_thp";
    static constexpr const char AUDIO_ADPCM_THP_LE[] = "audio/adpcm_thp_le";
    static constexpr const char AUDIO_ADPCM_XA[] = "audio/adpcm_xa";
    static constexpr const char AUDIO_ADPCM_YAMAHA[] = "audio/adpcm_yamaha";
    static constexpr const char AUDIO_WMV[] = "audio/x-ms-wma";
    static constexpr const char AUDIO_ALAC[] = "audio/alac";
    static constexpr const char AUDIO_ILBC[] = "audio/ilbc";
    static constexpr const char AUDIO_TRUEHD[] = "audio/truehd";

    static constexpr const char IMAGE_JPG[] = "image/jpeg";
    static constexpr const char IMAGE_PNG[] = "image/png";
    static constexpr const char IMAGE_BMP[] = "image/bmp";

    static constexpr const char MEDIA_MP4[] = "media/mp4";
    static constexpr const char MEDIA_M4A[] = "media/m4a";
    static constexpr const char MEDIA_AMR[] = "media/amr";
    static constexpr const char MEDIA_MP3[] = "media/mp3";
    static constexpr const char MEDIA_WAV[] = "media/wav";
    static constexpr const char MEDIA_AAC[] = "media/aac";
    static constexpr const char MEDIA_FLAC[] = "media/flac";

    static constexpr const char TEXT_SUBRIP[] = "application/x-subrip";
    static constexpr const char TEXT_WEBVTT[] = "text/vtt";
    static constexpr const char TEXT_LRC[] = "text/plain";
    static constexpr const char TEXT_SAMI[] = "application/x-sami";
    static constexpr const char TEXT_ASS[] = "text/x-ass";
    static constexpr const char TIMED_METADATA[] = "meta/timed-metadata";

    static constexpr const char INVALID_TYPE[] = "invalid";
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // MEDIA_FOUNDATION_MIME_TYPE_H
