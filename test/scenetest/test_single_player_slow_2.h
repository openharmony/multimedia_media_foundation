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
#ifndef HISTREAMER_TEST_SINGLE_PLAYER_SLOW_2_H
#define HISTREAMER_TEST_SINGLE_PLAYER_SLOW_2_H

#include <chrono>
#include <thread>
#include "testngpp/testngpp.hpp"
#include "helper/test_player.hpp"
#include "test_single_player_slow_1.h"

using namespace OHOS::Media::Test;

// @fixture(tags=audio_play_slow)
FIXTURE(dataDrivenSinglePlayerTestSlow2)
{
    DATA_PROVIDER(myurls, 1,
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/00-200Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/01-300Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/02-400Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/03-500Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/04-600Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/05-700Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/06-800Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/07-900Hz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/08-1kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/09-2kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/1.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/10-3kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/100Hz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/11-4kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/12-5kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/12KHz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/13-6kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/14-7kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/15-8kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/150Hz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/15KHz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/16-9kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/17-10kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/171005793.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/18-12kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/180Hz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/19-16kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/1kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/1KHz-44.1K-sine_-6dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/1KHz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/20-20kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/200Hz-44.1K-sine_0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/20kHz.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/2kHz Stereo 0dB.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/a2002011001-e02-8kHz.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/add home address.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/add work address.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/ail_ext.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/ail_int.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/AM playing.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/AM radio.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/AM.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/blow face foot.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/blow face.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/blow foot.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/blow_windscreen.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/blow_windscreen_foot.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/bridgegril85595213.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Bye_Bye_Bye-Lovestoned-19466604.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/call back.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Call to 1669.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/callcenter.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/cancel.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains~1.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains~2.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains~3.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains~4.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chains~5.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Change frequency 97.5 FM.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Change to a route that avoids tolls..flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Change to an economical route.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Change to the nearest route.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Changed to AM.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Changed to FM.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Chinese148755.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/close the AC .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/close the roof .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Close the window on passenger side.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Close the window on the drive side.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/cool down the temperature .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Cover27487643.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/dash-runner-8bit.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/dash-runner-8bit_short.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/decrease the fan speed please.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/defrost_off.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/defrost_on.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/meet.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/out.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/out~1.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/resume music.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/sample_16kHz.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/sample_16kHz_2.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/sample_32kHz.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/sample_441.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Save as a favorite son.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Set play mode loop all---All swirls.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Set play mode shuffle---Play randomly.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/short_music.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/size up.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/sounds on please.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/Switch to fastest route.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/take me back to the previous .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/take me to settings.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/test_48000Hz_32bit_1Channel.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/test_file_acc.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/tune to 837 AM.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/turn down the light to the lowest.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/turn on the maximum fan speed.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/want to add this station to favorite .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/warmer on .flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/whitenoise.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/yes.flac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/FLAC/zoom out the map.flac")),

    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/01_Minimal_tone.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/01_Sherbet.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/02_Haze.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/02_Peaceful_dreaming.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/03_Bubbles.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/03_Twinkling_bell.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/04_Pathetique.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/04_Transparent_piano.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/05_Animato_Tone.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/05_One_fine_day.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/06_Beat_maker.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/06_Stepping_stones.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/07_DontPanic.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/07_Ringing_to_you.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/08_Vocalise.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/09_Circus_time.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/09_Postbox.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/1077205091002.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/10_Latino_jazz.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/10_Tinkerbell.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/11_CaffeineSnake.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/11_High_frequency.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/12_Sunshine.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/12_Tweeters.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/13_Oriental_express.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/13_Starry_night.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/14_A_beat_for_me.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/14_Pizzicato.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/15_On_my_mind.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/15_Pause_tone.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/16_Drawing_the_night.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/16_Heaven.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/17_Pixiedust.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/17_Pure_tone.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/18_A_cricket_chirps.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/18_TaDa.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/19_Basic_bell.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/19_Morning_breeze.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/20_Classic_bell.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/20_Cloud.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/21_Postman.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/22_On_time.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/a.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/A_toy_watch.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/BeatPlucker.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Big_Easy.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/BirdLoop.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Bollywood.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Breeze.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/camera_click.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/camera_click_short.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/CaribbeanIce.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Chains.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Charger_Connection.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/CurveBallBlend.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/DigitalPhone.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Eastern_Sky.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Effect_Tick.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/EtherShake.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Forest.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Funk_Yall.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Gimme_Mo_Town.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Good_Morning.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Growl.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Insert.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/KeypressDelete.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/KeypressReturn.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/KeypressSpacebar.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Light.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Lock.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/LoveFlute.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/MidEvilJaunt.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Mist.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Morning.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Noisey_One.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/No_Limits.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/PDD.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Rain.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Road_Trip.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Savannah.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Seville.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Steppin_Out.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Terminated.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TerribleTwos.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Third_Eye.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Ticktac.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Battery_caution.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Call_Disconnect.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Error.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Low_Battery.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Touch.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Unlock.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Unlock_Glass.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Unlock_Puzzle.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/TW_Waterdrop.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Unlock.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/VeryAlarmed.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Victory.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/VideoRecord.ogg")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/World.ogg")),

    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/00-200Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/01-300Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/02-400Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/03-500Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/04-600Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/05-700Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/06-800Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/07-900Hz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/08-1kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/09-2kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/1.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/10-3kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/100Hz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/11-4kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/12-5kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/12KHz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/13-6kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/14-7kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/15-8kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/150Hz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/15KHz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/16-9kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/17-10kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/171005793.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/18-12kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/180Hz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/19-16kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/1kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/1KHz-44.1K-sine_-6dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/1KHz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/20-20kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/200Hz-44.1K-sine_0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/2kHz Stereo 0dB.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/a2002011001-e02-8kHz.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/add home address.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/add work address.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/ail_ext.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/ail_int.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/AM playing.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/AM radio.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/AM.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/blow face foot.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/blow face.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/blow foot.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/blow_windscreen.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/blow_windscreen_foot.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/bridgegril85595213.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Bye_Bye_Bye-Lovestoned-19466604.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/call back.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Call to 1669.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/callcenter.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/cancel.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains~1.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains~2.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains~3.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains~4.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chains~5.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Change frequency 97.5 FM.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Change to a route that avoids tolls..m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Change to an economical route.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Change to the nearest route.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Changed to AM.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Changed to FM.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Chinese148755.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/close the AC .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/close the roof .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Close the window on passenger side.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Close the window on the drive side.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/cool down the temperature .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Cover27487643.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/dash-runner-8bit.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/decrease the fan speed please.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/defrost_off.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/defrost_on.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/meet.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/out.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/out~1.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/piano.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/resume music.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/sample_16kHz.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/sample_16kHz_2.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/sample_32kHz.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/sample_441.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Save as a favorite son.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Set play mode loop all---All swirls.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Set play mode shuffle---Play randomly.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/short_music.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/size up.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/sounds on please.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Switch to fastest route.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/take me back to the previous .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/take me to settings.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/test_48000Hz_32bit_1Channel.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/test_file_acc.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/tune to 837 AM.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/turn down the light to the lowest.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/turn on the maximum fan speed.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/want to add this station to favorite .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/warmer on .m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Whistling_Down_the_Road-1.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/Whistling_Down_the_Road.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/yes.m4a")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/M4A/zoom out the map.m4a")),
    DATA_GROUP(std::string("http://img.51miz.com/preview/sound/00/26/73/51miz-S267356-423D33372.mp3")),
    DATA_GROUP(std::string("https://img.51miz.com/preview/sound/00/26/73/51miz-S267356-423D33372.mp3")));

	// @test(data="myurls")
    PTEST((std::string url), Test single player play url music, and finished automatically)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    DATA_PROVIDER(specialFileUrls, 1,
    // TO FIX: ffmpeg wav demuxer cause crash  RESOURCE_DIR "/../../../../resource_ext/WAV/dash-runner-8bit.wav
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/AAC/AACLC_1channel_001.aac")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/APE/bridgegril85595213.ape")),
    DATA_GROUP(std::string(RESOURCE_DIR "/../../../../resource_ext/OGG/Chains.ogg")));

    // @test(data="specialFileUrls", tags=debug)
    PTEST((std::string url), Debug single player play url music, and finished automatically)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

#endif //HISTREAMER_TEST_SINGLE_PLAYER_SLOW_2_H