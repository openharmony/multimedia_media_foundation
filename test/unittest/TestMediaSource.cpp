/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "../../interface/inner_api/common/media_source.h"

using namespace testing::ext;

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugins;

namespace {
constexpr int32_t INVALID_WIDTH = -1;
constexpr int32_t INVALID_HEIGHT = -1;
constexpr int32_t FULL_HD_WIDTH = 1920;
constexpr int32_t FULL_HD_HEIGHT = 1080;
constexpr int32_t UHD_WIDTH = 3840;
constexpr int32_t UHD_HEIGHT = 2160;
constexpr int32_t OVERFLOW_EDGE = 65536;
}

HWTEST(TestMediaSource, get_min_video_resolution_should_cover_all_branches, TestSize.Level1)
{
    TrackSelectionFilter filter;

    filter.minVideoResolution = {INVALID_WIDTH, FULL_HD_HEIGHT};
    EXPECT_EQ(filter.GetMinVideoResolution(), 0U);

    filter.minVideoResolution = {FULL_HD_WIDTH, INVALID_HEIGHT};
    EXPECT_EQ(filter.GetMinVideoResolution(), 0U);

    filter.minVideoResolution = {OVERFLOW_EDGE, OVERFLOW_EDGE};
    EXPECT_EQ(filter.GetMinVideoResolution(), UINT32_MAX);

    filter.minVideoResolution = {FULL_HD_WIDTH, FULL_HD_HEIGHT};
    EXPECT_EQ(filter.GetMinVideoResolution(), static_cast<uint32_t>(FULL_HD_WIDTH) * FULL_HD_HEIGHT);
}

HWTEST(TestMediaSource, get_max_video_resolution_should_cover_all_branches, TestSize.Level1)
{
    TrackSelectionFilter filter;

    filter.maxVideoResolution = {INVALID_WIDTH, FULL_HD_HEIGHT};
    EXPECT_EQ(filter.GetMaxVideoResolution(), 0U);

    filter.maxVideoResolution = {FULL_HD_WIDTH, INVALID_HEIGHT};
    EXPECT_EQ(filter.GetMaxVideoResolution(), 0U);

    filter.maxVideoResolution = {OVERFLOW_EDGE, OVERFLOW_EDGE};
    EXPECT_EQ(filter.GetMaxVideoResolution(), UINT32_MAX);

    filter.maxVideoResolution = {UHD_WIDTH, UHD_HEIGHT};
    EXPECT_EQ(filter.GetMaxVideoResolution(), static_cast<uint32_t>(UHD_WIDTH) * UHD_HEIGHT);
}

HWTEST(TestMediaSource, set_get_track_select_filter_round_trip, TestSize.Level1)
{
    TrackSelectionFilter filter;
    filter.maxVideoBitrate = 5000000;
    filter.minVideoBitrate = 500000;
    filter.maxVideoFrameRate = 60;
    filter.minVideoFrameRate = 24;
    filter.maxVideoResolution = {1920, 1080};
    filter.minVideoResolution = {640, 480};
    filter.preferredVideoMimeTypes = {"video/avc", "video/hevc"};
    filter.maxAudioBitrate = 320000;
    filter.minAudioBitrate = 48000;
    filter.maxAudioChannels = 2;
    filter.preferredAudioMimeTypes = {"audio/mp4a-latm"};
    filter.preferredAudioLanguages = {"eng", "chi"};
    filter.preferredSubtitleLanguages = {"chi"};

    MediaSource source("https://example.com/test.m3u8");
    source.SetTrackSelectFilter(filter);
    auto result = source.GetTrackSelectFilter();

    EXPECT_EQ(result.maxVideoBitrate, 5000000);
    EXPECT_EQ(result.minVideoBitrate, 500000);
    EXPECT_EQ(result.maxVideoFrameRate, 60);
    EXPECT_EQ(result.minVideoFrameRate, 24);
    EXPECT_EQ(result.maxVideoResolution.first, 1920);
    EXPECT_EQ(result.maxVideoResolution.second, 1080);
    EXPECT_EQ(result.minVideoResolution.first, 640);
    EXPECT_EQ(result.minVideoResolution.second, 480);
    EXPECT_EQ(result.preferredVideoMimeTypes.size(), 2);
    EXPECT_EQ(result.preferredVideoMimeTypes[0], "video/avc");
    EXPECT_EQ(result.preferredVideoMimeTypes[1], "video/hevc");
    EXPECT_EQ(result.maxAudioBitrate, 320000);
    EXPECT_EQ(result.minAudioBitrate, 48000);
    EXPECT_EQ(result.maxAudioChannels, 2);
    EXPECT_EQ(result.preferredAudioMimeTypes.size(), 1);
    EXPECT_EQ(result.preferredAudioMimeTypes[0], "audio/mp4a-latm");
    EXPECT_EQ(result.preferredAudioLanguages.size(), 2);
    EXPECT_EQ(result.preferredAudioLanguages[0], "eng");
    EXPECT_EQ(result.preferredAudioLanguages[1], "chi");
    EXPECT_EQ(result.preferredSubtitleLanguages.size(), 1);
    EXPECT_EQ(result.preferredSubtitleLanguages[0], "chi");
}

HWTEST(TestMediaSource, get_track_select_filter_default_all_unset, TestSize.Level1)
{
    MediaSource source("https://example.com/test.m3u8");
    auto filter = source.GetTrackSelectFilter();

    EXPECT_EQ(filter.maxVideoBitrate, -1);
    EXPECT_EQ(filter.minVideoBitrate, -1);
    EXPECT_EQ(filter.maxVideoFrameRate, -1);
    EXPECT_EQ(filter.minVideoFrameRate, -1);
    EXPECT_EQ(filter.maxVideoResolution.first, -1);
    EXPECT_EQ(filter.maxVideoResolution.second, -1);
    EXPECT_EQ(filter.minVideoResolution.first, -1);
    EXPECT_EQ(filter.minVideoResolution.second, -1);
    EXPECT_TRUE(filter.preferredVideoMimeTypes.empty());
    EXPECT_EQ(filter.maxAudioBitrate, -1);
    EXPECT_EQ(filter.minAudioBitrate, -1);
    EXPECT_EQ(filter.maxAudioChannels, -1);
    EXPECT_TRUE(filter.preferredAudioMimeTypes.empty());
    EXPECT_TRUE(filter.preferredAudioLanguages.empty());
    EXPECT_TRUE(filter.preferredSubtitleLanguages.empty());
}

HWTEST(TestMediaSource, set_get_retrieve_key_round_trip, TestSize.Level1)
{
    MediaSource source("https://example.com/test.m3u8");
    source.SetRetrieveKey("mykey123");
    EXPECT_EQ(source.GetRetrieveKey(), "mykey123");
}

HWTEST(TestMediaSource, get_retrieve_key_default_empty, TestSize.Level1)
{
    MediaSource source("https://example.com/test.m3u8");
    EXPECT_EQ(source.GetRetrieveKey(), "");
}
} // namespace Test
} // namespace Media
} // namespace OHOS