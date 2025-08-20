/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "event_bean.h"
#include "iremote_proxy.h"
#include "parcel.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace EventBeanUT {
class EventBeanInnerUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    std::shared_ptr<MediaMonitor::EventBean> eventBean_;
    const std::string strKey = "invalidKey";
    std::string strValue = "invalidValue";
    int32_t int32Value_ = -1;
    uint64_t uint64Value_ = -1;
    float floatValue_ = -1.0f;
    std::map<std::string, std::int32_t> int32Map_;
    std::map<std::string, std::string> stringMap_;
    std::map<std::string, uint64_t> uint64Map_;
    std::map<std::string, float> floatMap_;
};

void EventBeanInnerUnitTest::SetUpTestCase(void) {}

void EventBeanInnerUnitTest::TearDownTestCase(void) {}

void EventBeanInnerUnitTest::SetUp(void)
{
    eventBean_ = std::make_shared<MediaMonitor::EventBean>();
}

void EventBeanInnerUnitTest::TearDown(void)
{
    eventBean_ = nullptr;
}

/**
 * @tc.name: GetIntValue_001
 * @tc.desc: Test GetIntValue interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetIntValue_001, TestSize.Level1)
{
    EXPECT_EQ(-1, eventBean_->GetIntValue(strKey));
}

/**
 * @tc.name: GetStringValue_001
 * @tc.desc: Test GetStringValue interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetStringValue_001, TestSize.Level1)
{
    EXPECT_EQ("UNKNOWN", eventBean_->GetStringValue(strKey));
}

/**
 * @tc.name: GetUint64Value_001
 * @tc.desc: Test GetUint64Value interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetUint64Value_001, TestSize.Level1)
{
    EXPECT_EQ(0, eventBean_->GetUint64Value(strKey));
}

/**
 * @tc.name: GetFloatValue_001
 * @tc.desc: Test GetFloatValue interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetFloatValue_001, TestSize.Level1)
{
    EXPECT_EQ(0, eventBean_->GetFloatValue(strKey));
}

/**
 * @tc.name: UpdateIntMap_001
 * @tc.desc: Test UpdateIntMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, UpdateIntMap_001, TestSize.Level1)
{
    eventBean_->UpdateIntMap(strKey, 0);
    EXPECT_EQ(-1, eventBean_->GetIntValue(strKey));
}

/**
 * @tc.name: UpdateStringMap_001
 * @tc.desc: Test UpdateStringMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, UpdateStringMap_001, TestSize.Level1)
{
    eventBean_->UpdateStringMap(strKey, strValue);
    EXPECT_EQ("UNKNOWN", eventBean_->GetStringValue(strKey));
}

/**
 * @tc.name: UpdateUint64Map_001
 * @tc.desc: Test UpdateUint64Map interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, UpdateUint64Map_001, TestSize.Level1)
{
    eventBean_->UpdateUint64Map(strKey, 1);
    EXPECT_EQ(0, eventBean_->GetUint64Value(strKey));
}

/**
 * @tc.name: UpdateFloatMap_001
 * @tc.desc: Test UpdateFloatMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, UpdateFloatMap_001, TestSize.Level1)
{
    eventBean_->UpdateFloatMap(strKey, 1);
    EXPECT_EQ(0, eventBean_->GetFloatValue(strKey));
}

/**
 * @tc.name: GetIntMap_001
 * @tc.desc: Test GetIntMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetIntMap_001, TestSize.Level1)
{
    eventBean_->Add(strKey, int32Value_);
    int32Map_ = eventBean_->GetIntMap();
    EXPECT_EQ(int32Value_, int32Map_[strKey]);
    int32Map_.clear();
}

/**
 * @tc.name: GetStringMap_001
 * @tc.desc: Test GetStringMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetStringMap_001, TestSize.Level1)
{
    eventBean_->Add(strKey, strValue);
    stringMap_ = eventBean_->GetStringMap();
    EXPECT_EQ(strValue, stringMap_[strKey]);
    stringMap_.clear();
}

/**
 * @tc.name: GetUint64Map_001
 * @tc.desc: Test GetUint64Map interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetUint64Map_001, TestSize.Level1)
{
    eventBean_->Add(strKey, uint64Value_);
    uint64Map_ = eventBean_->GetUint64Map();
    EXPECT_EQ(uint64Value_, uint64Map_[strKey]);
    uint64Map_.clear();
}

/**
 * @tc.name: GetFloatMap_001
 * @tc.desc: Test GetFloatMap interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, GetFloatMap_001, TestSize.Level1)
{
    eventBean_->Add(strKey, floatValue_);
    floatMap_ = eventBean_->GetFloatMap();
    EXPECT_EQ(floatValue_, floatMap_[strKey]);
    floatMap_.clear();
}

/**
 * @tc.name: ReadFromParcel_001
 * @tc.desc: Test ReadFromParcel interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, ReadFromParcel_001, TestSize.Level1)
{
    MessageParcel parcel;
    eventBean_->ReadFromParcel(parcel);
    EXPECT_NE(nullptr, eventBean_);
}

/**
 * @tc.name: Unmarshalling_001
 * @tc.desc: Test Unmarshalling interface
 * @tc.type: FUNC
 */
HWTEST_F(EventBeanInnerUnitTest, Unmarshalling_001, TestSize.Level1)
{
    Parcel data;
    MediaMonitor::EventBean *eventBean = eventBean_->Unmarshalling(data);
    EXPECT_NE(nullptr, eventBean);
    delete eventBean;
}
} // namespace EventBeanUT
} // namespace Media
} // namespace OHOS
