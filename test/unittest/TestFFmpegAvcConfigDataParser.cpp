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

#include "gtest/gtest.h"
#include "plugin/plugins/ffmpeg_adapter/utils/avc_config_data_parser.h"

using namespace testing::ext;

namespace OHOS {
namespace Media {
namespace Test {
using namespace Plugin::Ffmpeg;

HWTEST(FFmpegAvcConfigDataParserTest, testParseConfigData, TestSize.Level1)
{
    const uint8_t* ptr1 = nullptr;
    auto parser1 = std::make_shared<AVCConfigDataParser>(ptr1, 0);
    bool res1 = parser1->ParseConfigData();
    ASSERT_FALSE(res1);

    uint8_t validConfig[] = {0x01, 0x42, 0x00, 0x1e, 0xff, 0xe1, 0x00, 0x0d, 0x67, 0x42, 0x00, 0x1e, 0x8d, 0x8d,
        0x40, 0x28, 0x02, 0xdd, 0x80, 0x88, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x03, 0x00, 0x78, 0x3c,
        0x18, 0x60, 0x01, 0x00, 0x05, 0x68, 0xeb, 0xec, 0xb2, 0x2c};
    auto parser2 = std::make_shared<AVCConfigDataParser>(validConfig, sizeof(validConfig));
    bool res2 = parser2->ParseConfigData();
    ASSERT_TRUE(res2);

    uint8_t invalidConfig1[] = {0x00, 0x42, 0x00, 0x1e, 0xff, 0xe1, 0x00, 0x0d};
    auto parser3 = std::make_shared<AVCConfigDataParser>(invalidConfig1, sizeof(invalidConfig1));
    bool res3 = parser3->ParseConfigData();
    ASSERT_TRUE(res3);

    uint8_t shortConfig[] = {0x01, 0x42, 0x00};
    auto parser4 = std::make_shared<AVCConfigDataParser>(shortConfig, sizeof(shortConfig));
    bool res4 = parser4->ParseConfigData();
    ASSERT_TRUE(res4);
}

HWTEST(FFmpegAvcConfigDataParserTest, testIsNeedAddFrameHeader, TestSize.Level1)
{
    uint8_t validConfig[] = {0x01, 0x42, 0x00, 0x1e, 0xff, 0xe1, 0x00, 0x0d, 0x67, 0x42, 0x00, 0x1e, 0x8d, 0x8d,
        0x40, 0x28, 0x02, 0xdd, 0x80, 0x88, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x03, 0x00, 0x78, 0x3c,
        0x18, 0x60, 0x01, 0x00, 0x05, 0x68, 0xeb, 0xec, 0xb2, 0x2c};
    auto parser1 = std::make_shared<AVCConfigDataParser>(validConfig, sizeof(validConfig));
    bool res1 = parser1->ParseConfigData();
    ASSERT_TRUE(res1);
    bool needHeader1 = parser1->IsNeedAddFrameHeader();
    ASSERT_FALSE(needHeader1);

    const uint8_t* ptr2 = nullptr;
    auto parser2 = std::make_shared<AVCConfigDataParser>(ptr2, 0);
    bool res2 = parser2->ParseConfigData();
    ASSERT_FALSE(res2);
    bool needHeader2 = parser2->IsNeedAddFrameHeader();
    ASSERT_TRUE(needHeader2);
}

HWTEST(FFmpegAvcConfigDataParserTest, testGetNewConfigData, TestSize.Level1)
{
    const uint8_t* ptr1 = nullptr;
    auto parser1 = std::make_shared<AVCConfigDataParser>(ptr1, 0);
    std::shared_ptr<uint8_t> newCfgData;
    size_t newCfgDataSize;
    bool res1 = parser1->GetNewConfigData(newCfgData, newCfgDataSize);
    ASSERT_FALSE(res1);

    uint8_t validConfig[] = {0x01, 0x42, 0x00, 0x1e, 0xff, 0xe1, 0x00, 0x0d, 0x67, 0x42, 0x00, 0x1e, 0x8d, 0x8d,
        0x40, 0x28, 0x02, 0xdd, 0x80, 0x88, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x03, 0x00, 0x78, 0x3c,
        0x18, 0x60, 0x01, 0x00, 0x05, 0x68, 0xeb, 0xec, 0xb2, 0x2c};
    auto parser2 = std::make_shared<AVCConfigDataParser>(validConfig, sizeof(validConfig));
    bool res2 = parser2->ParseConfigData();
    ASSERT_TRUE(res2);
    std::shared_ptr<uint8_t> data;
    size_t size;
    bool res3 = parser2->GetNewConfigData(data, size);
    ASSERT_TRUE(res3);
    ASSERT_NE(data, nullptr);
    ASSERT_GT(size, 0);
}

} // namespace Test
} // namespace Media
} // namespace OHOS