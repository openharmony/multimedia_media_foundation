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
} // namespace Test
} // namespace Media
} // namespace OHOS