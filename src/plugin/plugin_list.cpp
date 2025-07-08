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

#include "plugin/plugin_list.h"

#define DEFAULT_RANK 100

namespace OHOS {
namespace Media {
namespace Plugins {
PluginList::~PluginList()
{
    pluginDescriptionList_.clear();
}

PluginList &PluginList::GetInstance()
{
    static PluginList pluginList;
    return pluginList;
}

std::vector<PluginDescription> PluginList::GetAllPlugins()
{
    return pluginDescriptionList_;
}

std::vector<PluginDescription> PluginList::GetPluginsByCap(PluginType pluginType, std::string needCap)
{
    std::vector<PluginDescription> matchedPlugins;
    std::vector<PluginDescription>::iterator it;
    for (it = pluginDescriptionList_.begin(); it != pluginDescriptionList_.end(); it++) {
        PluginDescription temp = *it;
        if (temp.pluginType == pluginType &&
            strcmp(temp.cap.c_str(), needCap.c_str()) == 0) {
            matchedPlugins.push_back(temp);
        }
    }
    std::sort(matchedPlugins.begin(), matchedPlugins.end(),
        [](PluginDescription a, PluginDescription b) {
            return a.rank > b.rank;
        });
    return matchedPlugins;
}

PluginDescription PluginList::GetPluginByName(std::string name)
{
    std::vector<PluginDescription>::iterator it;
    PluginDescription bestMatchedPlugin;
    for (it = pluginDescriptionList_.begin(); it != pluginDescriptionList_.end(); it++) {
        PluginDescription temp = *it;
        if (strcmp((*it).pluginName.c_str(), name.c_str()) == 0) {
            bestMatchedPlugin = (*it);
        }
    }
    return bestMatchedPlugin;
}

std::vector<PluginDescription> PluginList::GetPluginsByType(PluginType pluginType)
{
    std::vector<PluginDescription> matchedPlugins;
    std::vector<PluginDescription>::iterator it;
    for (it = pluginDescriptionList_.begin(); it != pluginDescriptionList_.end(); it++) {
        PluginDescription temp = *it;
        if (temp.pluginType == pluginType) {
            matchedPlugins.push_back(temp);
        }
    }
    return matchedPlugins;
}

PluginList::PluginList()
{
    AddDataSourceStreamPlugins();
    AddFileFdSourcePlugins();
    AddFileSourcePlugins();
    AddHttpSourcePlugins();
    AddFFmpegDemuxerPlugins();
    AddFFmpegAudioDecodersPlugins();
    AddAudioVividDecodersPlugins();
    AddAudioVendorAacEncodersPlugin();
    AddG711muAudioDecoderPlugins();
    AddLbvcAudioDecoderPlugins();
    AddOpusAudioDecoderPlugins();
    AddRawAudioDecoderPlugins();
    AddAudioServerSinkPlugins();
    AddFFmpegAudioEncodersPlugins();
    AddG711muAudioEncoderPlugins();
    AddLbvcAudioEncoderPlugins();
    AddOpusAudioEncoderPlugins();
    AddAmrwbAudioEncoderPlugins();
    AddAmrnbAudioEncoderPlugins();
    AddMp3AudioEncoderPlugins();
    AddFFmpegMuxerPlugins();
    AddAc3AudioDecoderPlugins();
}

void PluginList::AddDataSourceStreamPlugins()
{
    PluginDescription dataSourceStreamPlugin;
    dataSourceStreamPlugin.pluginName = "DataStreamSource";
    dataSourceStreamPlugin.packageName = "DataStreamSource";
    dataSourceStreamPlugin.pluginType = PluginType::SOURCE;
    dataSourceStreamPlugin.cap = "stream";
    dataSourceStreamPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(dataSourceStreamPlugin);
}

void PluginList::AddFileFdSourcePlugins()
{
    PluginDescription fileFdSourcePlugin;
    fileFdSourcePlugin.pluginName = "FileFdSource";
    fileFdSourcePlugin.packageName = "FileFdSource";
    fileFdSourcePlugin.pluginType = PluginType::SOURCE;
    fileFdSourcePlugin.cap = "fd";
    fileFdSourcePlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(fileFdSourcePlugin);
}

void PluginList::AddFileSourcePlugins()
{
    PluginDescription fileSourcePlugin;
    fileSourcePlugin.pluginName = "FileSource";
    fileSourcePlugin.packageName = "FileSource";
    fileSourcePlugin.pluginType = PluginType::SOURCE;
    fileSourcePlugin.cap = "file";
    fileSourcePlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(fileSourcePlugin);
}

void PluginList::AddHttpSourcePlugins()
{
    PluginDescription httpSourcePlugin;
    httpSourcePlugin.pluginName = "HttpSource";
    httpSourcePlugin.packageName = "HttpSource";
    httpSourcePlugin.pluginType = PluginType::SOURCE;
    httpSourcePlugin.cap = "http";
    httpSourcePlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(httpSourcePlugin);

    PluginDescription httpsSourcePlugin;
    httpsSourcePlugin.pluginName = "HttpSource";
    httpsSourcePlugin.packageName = "HttpSource";
    httpsSourcePlugin.pluginType = PluginType::SOURCE;
    httpsSourcePlugin.cap = "https";
    httpsSourcePlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(httpsSourcePlugin);
}

void PluginList::AddAacDemuxerPlugin()
{
    PluginDescription aacDemuxerPlugin;
    aacDemuxerPlugin.pluginName = "avdemux_aac";
    aacDemuxerPlugin.packageName = "FFmpegDemuxer";
    aacDemuxerPlugin.pluginType = PluginType::DEMUXER;
    aacDemuxerPlugin.cap = "";
    aacDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(aacDemuxerPlugin);
}

void PluginList::AddAmrDemuxerPlugin()
{
    PluginDescription amrDemuxerPlugin;
    amrDemuxerPlugin.pluginName = "avdemux_amr";
    amrDemuxerPlugin.packageName = "FFmpegDemuxer";
    amrDemuxerPlugin.pluginType = PluginType::DEMUXER;
    amrDemuxerPlugin.cap = "";
    amrDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrDemuxerPlugin);
}

void PluginList::AddAmrnbDemuxerPlugin()
{
    PluginDescription amrnbDemuxerPlugin;
    amrnbDemuxerPlugin.pluginName = "avdemux_amrnb";
    amrnbDemuxerPlugin.packageName = "FFmpegDemuxer";
    amrnbDemuxerPlugin.pluginType = PluginType::DEMUXER;
    amrnbDemuxerPlugin.cap = "";
    amrnbDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrnbDemuxerPlugin);
}

void PluginList::AddAmrwbDemuxerPlugin()
{
    PluginDescription amrwbDemuxerPlugin;
    amrwbDemuxerPlugin.pluginName = "avdemux_amrwb";
    amrwbDemuxerPlugin.packageName = "FFmpegDemuxer";
    amrwbDemuxerPlugin.pluginType = PluginType::DEMUXER;
    amrwbDemuxerPlugin.cap = "";
    amrwbDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrwbDemuxerPlugin);
}

void PluginList::AddApeDemuxerPlugin()
{
    PluginDescription apeDemuxerPlugin;
    apeDemuxerPlugin.pluginName = "avdemux_ape";
    apeDemuxerPlugin.packageName = "FFmpegDemuxer";
    apeDemuxerPlugin.pluginType = PluginType::DEMUXER;
    apeDemuxerPlugin.cap = "";
    apeDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(apeDemuxerPlugin);
}

void PluginList::AddAsfDemuxerPlugin()
{
    PluginDescription asfDemuxerPlugin;
    asfDemuxerPlugin.pluginName = "avdemux_asf";
    asfDemuxerPlugin.packageName = "FFmpegDemuxer";
    asfDemuxerPlugin.pluginType = PluginType::DEMUXER;
    asfDemuxerPlugin.cap = "";
    asfDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(asfDemuxerPlugin);
}

void PluginList::AddAsfoDemuxerPlugin()
{
    PluginDescription asfoDemuxerPlugin;
    asfoDemuxerPlugin.pluginName = "avdemux_asf_o";
    asfoDemuxerPlugin.packageName = "FFmpegDemuxer";
    asfoDemuxerPlugin.pluginType = PluginType::DEMUXER;
    asfoDemuxerPlugin.cap = "";
    asfoDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(asfoDemuxerPlugin);
}

void PluginList::AddFlacDemuxerPlugin()
{
    PluginDescription flacDemuxerPlugin;
    flacDemuxerPlugin.pluginName = "avdemux_flac";
    flacDemuxerPlugin.packageName = "FFmpegDemuxer";
    flacDemuxerPlugin.pluginType = PluginType::DEMUXER;
    flacDemuxerPlugin.cap = "";
    flacDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(flacDemuxerPlugin);
}

void PluginList::AddFlvDemuxerPlugin()
{
    PluginDescription flvDemuxerPlugin;
    flvDemuxerPlugin.pluginName = "avdemux_flv";
    flvDemuxerPlugin.packageName = "FFmpegDemuxer";
    flvDemuxerPlugin.pluginType = PluginType::DEMUXER;
    flvDemuxerPlugin.cap = "";
    flvDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(flvDemuxerPlugin);
}

void PluginList::AddMatroskaDemuxerPlugin()
{
    PluginDescription matroskaDemuxerPlugin;
    matroskaDemuxerPlugin.pluginName = "avdemux_matroska,webm";
    matroskaDemuxerPlugin.packageName = "FFmpegDemuxer";
    matroskaDemuxerPlugin.pluginType = PluginType::DEMUXER;
    matroskaDemuxerPlugin.cap = "";
    matroskaDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(matroskaDemuxerPlugin);
}

void PluginList::AddMovDemuxerPlugin()
{
    PluginDescription movDemuxerPlugin;
    movDemuxerPlugin.pluginName = "avdemux_mov,mp4,m4a,3gp,3g2,mj2";
    movDemuxerPlugin.packageName = "FFmpegDemuxer";
    movDemuxerPlugin.pluginType = PluginType::DEMUXER;
    movDemuxerPlugin.cap = "";
    movDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(movDemuxerPlugin);
}

void PluginList::AddMp3DemuxerPlugin()
{
    PluginDescription mp3DemuxerPlugin;
    mp3DemuxerPlugin.pluginName = "avdemux_mp3";
    mp3DemuxerPlugin.packageName = "FFmpegDemuxer";
    mp3DemuxerPlugin.pluginType = PluginType::DEMUXER;
    mp3DemuxerPlugin.cap = "";
    mp3DemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mp3DemuxerPlugin);
}

void PluginList::AddMpegDemuxerPlugin()
{
    PluginDescription mpegDemuxerPlugin;
    mpegDemuxerPlugin.pluginName = "avdemux_mpeg";
    mpegDemuxerPlugin.packageName = "FFmpegDemuxer";
    mpegDemuxerPlugin.pluginType = PluginType::DEMUXER;
    mpegDemuxerPlugin.cap = "";
    mpegDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mpegDemuxerPlugin);
}

void PluginList::AddMpegtsDemuxerPlugin()
{
    PluginDescription mpegtsDemuxerPlugin;
    mpegtsDemuxerPlugin.pluginName = "avdemux_mpegts";
    mpegtsDemuxerPlugin.packageName = "FFmpegDemuxer";
    mpegtsDemuxerPlugin.pluginType = PluginType::DEMUXER;
    mpegtsDemuxerPlugin.cap = "";
    mpegtsDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mpegtsDemuxerPlugin);
}

void PluginList::AddAviDemuxerPlugin()
{
    PluginDescription aviDemuxerPlugin;
    aviDemuxerPlugin.pluginName = "avdemux_avi";
    aviDemuxerPlugin.packageName = "FFmpegDemuxer";
    aviDemuxerPlugin.pluginType = PluginType::DEMUXER;
    aviDemuxerPlugin.cap = "";
    aviDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(aviDemuxerPlugin);
}

void PluginList::AddSrtDemuxerPlugin()
{
    PluginDescription srtDemuxerPlugin;
    srtDemuxerPlugin.pluginName = "avdemux_srt";
    srtDemuxerPlugin.packageName = "FFmpegDemuxer";
    srtDemuxerPlugin.pluginType = PluginType::DEMUXER;
    srtDemuxerPlugin.cap = "";
    srtDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(srtDemuxerPlugin);
}

void PluginList::AddWebvttDemuxerPlugin()
{
    PluginDescription vttDemuxerPlugin;
    vttDemuxerPlugin.pluginName = "avdemux_webvtt";
    vttDemuxerPlugin.packageName = "FFmpegDemuxer";
    vttDemuxerPlugin.pluginType = PluginType::DEMUXER;
    vttDemuxerPlugin.cap = "";
    vttDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(vttDemuxerPlugin);
}

void PluginList::AddOggDemuxerPlugin()
{
    PluginDescription oggDemuxerPlugin;
    oggDemuxerPlugin.pluginName = "avdemux_ogg";
    oggDemuxerPlugin.packageName = "FFmpegDemuxer";
    oggDemuxerPlugin.pluginType = PluginType::DEMUXER;
    oggDemuxerPlugin.cap = "";
    oggDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(oggDemuxerPlugin);
}

void PluginList::AddWavDemuxerPlugin()
{
    PluginDescription wavDemuxerPlugin;
    wavDemuxerPlugin.pluginName = "avdemux_wav";
    wavDemuxerPlugin.packageName = "FFmpegDemuxer";
    wavDemuxerPlugin.pluginType = PluginType::DEMUXER;
    wavDemuxerPlugin.cap = "";
    wavDemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(wavDemuxerPlugin);
}

void PluginList::AddAc3DemuxerPlugin()
{
    PluginDescription ac3DemuxerPlugin;
    ac3DemuxerPlugin.pluginName = "avdemux_ac3";
    ac3DemuxerPlugin.packageName = "FFmpegDemuxer";
    ac3DemuxerPlugin.pluginType = PluginType::DEMUXER;
    ac3DemuxerPlugin.cap = "";
    ac3DemuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(ac3DemuxerPlugin);
}

void PluginList::AddFFmpegDemuxerPlugins()
{
    AddAacDemuxerPlugin();
    AddAmrDemuxerPlugin();
    AddAmrnbDemuxerPlugin();
    AddAmrwbDemuxerPlugin();
    AddApeDemuxerPlugin();
    AddAsfDemuxerPlugin();
    AddAsfoDemuxerPlugin();
    AddFlacDemuxerPlugin();
    AddFlvDemuxerPlugin();
    AddMatroskaDemuxerPlugin();
    AddMovDemuxerPlugin();
    AddMp3DemuxerPlugin();
    AddMpegDemuxerPlugin();
    AddMpegtsDemuxerPlugin();
    AddAviDemuxerPlugin();
    AddSrtDemuxerPlugin();
    AddWebvttDemuxerPlugin();
    AddOggDemuxerPlugin();
    AddWavDemuxerPlugin();
    AddAc3DemuxerPlugin();
}

void PluginList::AddMpegAudioDecoderPlugin()
{
    PluginDescription mpegAudioDecoderPlugin;
    mpegAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Mpeg";
    mpegAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    mpegAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    mpegAudioDecoderPlugin.cap = "audio/mpeg";
    mpegAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mpegAudioDecoderPlugin);
}

void PluginList::AddAacAudioDecoderPlugin()
{
    PluginDescription aacAudioDecoderPlugin;
    aacAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.AAC";
    aacAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    aacAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    aacAudioDecoderPlugin.cap = "audio/mp4a-latm";
    aacAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(aacAudioDecoderPlugin);
}

void PluginList::AddFFmpegAudioDecodersPlugins()
{
    AddMpegAudioDecoderPlugin();
    AddAacAudioDecoderPlugin();

    PluginDescription flacAudioDecoderPlugin;
    flacAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Flac";
    flacAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    flacAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    flacAudioDecoderPlugin.cap = "audio/flac";
    flacAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(flacAudioDecoderPlugin);

    PluginDescription vorbisAudioDecoderPlugin;
    vorbisAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Vorbis";
    vorbisAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    vorbisAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    vorbisAudioDecoderPlugin.cap = "audio/vorbis";
    vorbisAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(vorbisAudioDecoderPlugin);

    PluginDescription amrnbAudioDecoderPlugin;
    amrnbAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Amrnb";
    amrnbAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    amrnbAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    amrnbAudioDecoderPlugin.cap = "audio/3gpp";
    amrnbAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrnbAudioDecoderPlugin);

    PluginDescription amrwbAudioDecoderPlugin;
    amrwbAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Amrwb";
    amrwbAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    amrwbAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    amrwbAudioDecoderPlugin.cap = "audio/amr-wb";
    amrwbAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrwbAudioDecoderPlugin);

    PluginDescription apeAudioDecoderPlugin;
    apeAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Ape";
    apeAudioDecoderPlugin.packageName = "FFmpegAudioDecoders";
    apeAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    apeAudioDecoderPlugin.cap = "audio/x-ape";
    apeAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(apeAudioDecoderPlugin);
}

void PluginList::AddAudioVividDecodersPlugins()
{
    PluginDescription audioVividDecoderPlugin;
    audioVividDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Vivid";
    audioVividDecoderPlugin.packageName = "AudioVividDecoder";
    audioVividDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    audioVividDecoderPlugin.cap = "audio/av3a";
    audioVividDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(audioVividDecoderPlugin);
}

void PluginList::AddG711muAudioDecoderPlugins()
{
    PluginDescription g711muAudioDecoderPlugin;
    g711muAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.G711mu";
    g711muAudioDecoderPlugin.packageName = "G711muAudioDecoder";
    g711muAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    g711muAudioDecoderPlugin.cap = "audio/g711mu";
    g711muAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(g711muAudioDecoderPlugin);
}

void PluginList::AddLbvcAudioDecoderPlugins()
{
    PluginDescription lbvcAudioDecoderPlugin;
    lbvcAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.LBVC";
    lbvcAudioDecoderPlugin.packageName = "LbvcAudioDecoder";
    lbvcAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    lbvcAudioDecoderPlugin.cap = "audio/lbvc";
    lbvcAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(lbvcAudioDecoderPlugin);
}

void PluginList::AddOpusAudioDecoderPlugins()
{
    PluginDescription opusAudioDecoderPlugin;
    opusAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Opus";
    opusAudioDecoderPlugin.packageName = "OpusAudioDecoder";
    opusAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    opusAudioDecoderPlugin.cap = "audio/opus";
    opusAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(opusAudioDecoderPlugin);
}

void PluginList::AddAc3AudioDecoderPlugins()
{
    PluginDescription ac3AudioDecoderPlugin;
    ac3AudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.AC3";
    ac3AudioDecoderPlugin.packageName = "Ac3AudioDecoder";
    ac3AudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    ac3AudioDecoderPlugin.cap = "audio/ac3";
    ac3AudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(ac3AudioDecoderPlugin);
}

void PluginList::AddRawAudioDecoderPlugins()
{
    PluginDescription opusAudioDecoderPlugin;
    opusAudioDecoderPlugin.pluginName = "OH.Media.Codec.Decoder.Audio.Raw";
    opusAudioDecoderPlugin.packageName = "RawAudioDecoder";
    opusAudioDecoderPlugin.pluginType = PluginType::AUDIO_DECODER;
    opusAudioDecoderPlugin.cap = "audio/raw";
    opusAudioDecoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(opusAudioDecoderPlugin);
}

void PluginList::AddAudioServerSinkPlugins()
{
    PluginDescription audioServerSinkPlugin;
    audioServerSinkPlugin.pluginName = "AudioServerSink";
    audioServerSinkPlugin.packageName = "AudioServerSink";
    audioServerSinkPlugin.pluginType = PluginType::AUDIO_SINK;
    audioServerSinkPlugin.cap = "audio/raw";
    audioServerSinkPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(audioServerSinkPlugin);
}

void PluginList::AddFFmpegAudioEncodersPlugins()
{
    PluginDescription aacAudioEncoderPlugin;
    aacAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.AAC";
    aacAudioEncoderPlugin.packageName = "FFmpegAudioEncoders";
    aacAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    aacAudioEncoderPlugin.cap = "audio/mp4a-latm";
    aacAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(aacAudioEncoderPlugin);

    PluginDescription flacAudioEncoderPlugin;
    flacAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Flac";
    flacAudioEncoderPlugin.packageName = "FFmpegAudioEncoders";
    flacAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    flacAudioEncoderPlugin.cap = "audio/flac";
    flacAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(flacAudioEncoderPlugin);
}

void PluginList::AddAudioVendorAacEncodersPlugin()
{
    PluginDescription AacEncoderPlugin;
    AacEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Vendor.AAC";
    AacEncoderPlugin.packageName = "AudioVendorAacEncoder";
    AacEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    AacEncoderPlugin.cap = "audio/mp4a-latm";
    AacEncoderPlugin.rank = DEFAULT_RANK + 1; // larger than default aac plugin
    pluginDescriptionList_.push_back(AacEncoderPlugin);
}

void PluginList::AddG711muAudioEncoderPlugins()
{
    PluginDescription g711muAudioEncoderPlugin;
    g711muAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.G711mu";
    g711muAudioEncoderPlugin.packageName = "G711muAudioEncoder";
    g711muAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    g711muAudioEncoderPlugin.cap = "audio/g711mu";
    g711muAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(g711muAudioEncoderPlugin);
}

void PluginList::AddLbvcAudioEncoderPlugins()
{
    PluginDescription lbvcAudioEncoderPlugin;
    lbvcAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.LBVC";
    lbvcAudioEncoderPlugin.packageName = "LbvcAudioEncoder";
    lbvcAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    lbvcAudioEncoderPlugin.cap = "audio/lbvc";
    lbvcAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(lbvcAudioEncoderPlugin);
}

void PluginList::AddOpusAudioEncoderPlugins()
{
    PluginDescription opusAudioEncoderPlugin;
    opusAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Opus";
    opusAudioEncoderPlugin.packageName = "OpusAudioEncoder";
    opusAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    opusAudioEncoderPlugin.cap = "audio/opus";
    opusAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(opusAudioEncoderPlugin);
}

void PluginList::AddAmrwbAudioEncoderPlugins()
{
    PluginDescription amrwbAudioEncoderPlugin;
    amrwbAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Amrwb";
    amrwbAudioEncoderPlugin.packageName = "AmrWbAudioEncoder";
    amrwbAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    amrwbAudioEncoderPlugin.cap = "audio/amrwb";
    amrwbAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrwbAudioEncoderPlugin);
}
void PluginList::AddAmrnbAudioEncoderPlugins()
{
    PluginDescription amrnbAudioEncoderPlugin;
    amrnbAudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Amrnb";
    amrnbAudioEncoderPlugin.packageName = "AmrNbAudioEncoder";
    amrnbAudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    amrnbAudioEncoderPlugin.cap = "audio/amrnb";
    amrnbAudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrnbAudioEncoderPlugin);
}

void PluginList::AddMp3AudioEncoderPlugins()
{
    PluginDescription mp3AudioEncoderPlugin;
    mp3AudioEncoderPlugin.pluginName = "OH.Media.Codec.Encoder.Audio.Mp3";
    mp3AudioEncoderPlugin.packageName = "Mp3AudioEncoder";
    mp3AudioEncoderPlugin.pluginType = PluginType::AUDIO_ENCODER;
    mp3AudioEncoderPlugin.cap = "audio/mpeg";
    mp3AudioEncoderPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mp3AudioEncoderPlugin);
}

void PluginList::AddFFmpegMuxerPlugins()
{
    PluginDescription mp4MuxerPlugin;
    mp4MuxerPlugin.pluginName = "ffmpegMux_mp4";
    mp4MuxerPlugin.packageName = "FFmpegMuxer";
    mp4MuxerPlugin.pluginType = PluginType::MUXER;
    mp4MuxerPlugin.cap = "media/mp4";
    mp4MuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mp4MuxerPlugin);

    PluginDescription m4aMuxerPlugin;
    m4aMuxerPlugin.pluginName = "ffmpegMux_ipod";
    m4aMuxerPlugin.packageName = "FFmpegMuxer";
    m4aMuxerPlugin.pluginType = PluginType::MUXER;
    m4aMuxerPlugin.cap = "media/m4a";
    m4aMuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(m4aMuxerPlugin);

    PluginDescription amrMuxerPlugin;
    amrMuxerPlugin.pluginName = "ffmpegMux_amr";
    amrMuxerPlugin.packageName = "FFmpegMuxer";
    amrMuxerPlugin.pluginType = PluginType::MUXER;
    amrMuxerPlugin.cap = "media/amr";
    amrMuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(amrMuxerPlugin);

    PluginDescription mp3MuxerPlugin;
    mp3MuxerPlugin.pluginName = "ffmpegMux_mp3";
    mp3MuxerPlugin.packageName = "FFmpegMuxer";
    mp3MuxerPlugin.pluginType = PluginType::MUXER;
    mp3MuxerPlugin.cap = "media/mp3";
    mp3MuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(mp3MuxerPlugin);

    PluginDescription wavMuxerPlugin;
    wavMuxerPlugin.pluginName = "ffmpegMux_wav";
    wavMuxerPlugin.packageName = "FFmpegMuxer";
    wavMuxerPlugin.pluginType = PluginType::MUXER;
    wavMuxerPlugin.cap = "media/wav";
    wavMuxerPlugin.rank = DEFAULT_RANK;
    pluginDescriptionList_.push_back(wavMuxerPlugin);
}

} // namespace Plugins
} // namespace Media
} // namespace OHOS