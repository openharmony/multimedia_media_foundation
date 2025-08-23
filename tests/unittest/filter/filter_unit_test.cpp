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
#include "filter/filter.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::Media::Pipeline;

namespace OHOS {
namespace Media {
namespace FilterUnitTest {
class FilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    const int32_t sleepTime_ = 1;
};

class FilterLinkCallbackTest : public FilterLinkCallback {
public:
    FilterLinkCallbackTest() = default;

    ~FilterLinkCallbackTest() = default;

    void OnLinkedResult(const sptr<AVBufferQueueProducer>& queue, std::shared_ptr<Meta>& meta) override
    {
        (void)queue;
        (void)meta;
    }

    void OnUnlinkedResult(std::shared_ptr<Meta>& meta) override
    {
        (void)meta;
    }

    void OnUpdatedResult(std::shared_ptr<Meta>& meta) override
    {
        (void)meta;
    }
};

class FilterCallbackTest : public FilterCallback {
public:
    FilterCallbackTest() = default;

    ~FilterCallbackTest() = default;

    Status OnCallback(const std::shared_ptr<Filter>& filter, FilterCallBackCommand cmd, StreamType outType) override
    {
        (void)filter;
        (void)cmd;
        (void)outType;
        return Status::OK;
    }
};

class EventReceiverTest : public EventReceiver {
public:
    EventReceiverTest() = default;

    ~EventReceiverTest() = default;

    void OnEvent(const Event& event) override
    {
        (void)event;
    }
};

void FilterUnitTest::SetUpTestCase(void) {}

void FilterUnitTest::TearDownTestCase(void) {}

void FilterUnitTest::SetUp(void) {}

void FilterUnitTest::TearDown(void) {}

/**
 * @tc.name: LinkPipline_001
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_VENC
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_002
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_VDEC
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VDEC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_003
 * @tc.desc: Test LinkPipline interface, set filtertype to VIDEO_CAPTURE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::VIDEO_CAPTURE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_004
 * @tc.desc: Test LinkPipline interface, set filtertype to FILTERTYPE_ASINK
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_004, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_ASINK, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_005
 * @tc.desc: Test LinkPipline interface, set filtertype to AUDIO_CAPTURE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_005, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::AUDIO_CAPTURE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_006
 * @tc.desc: Test LinkPipline interface, set filtertype to AUDIO_DATA_SOURCE
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_006, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::AUDIO_DATA_SOURCE, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: LinkPipline_007
 * @tc.desc: Test LinkPipline interface, set asyncMode to false
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, LinkPipline_007, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
}

/**
 * @tc.name: Prepare_001
 * @tc.desc: Test Prepare interface, set asyncMode to false
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
    filter->Prepare();
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: Prepare_002
 * @tc.desc: Test Prepare interface, set asyncMode to true
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->Prepare();
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: Prepare_003
 * @tc.desc: Test Prepare interface, set state is error
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Prepare_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->ChangeState(FilterState::ERROR);
    filter->Prepare();
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::ERROR, filter->curState_);
}

/**
 * @tc.name: PrepareDone_001
 * @tc.desc: Test PrepareDone interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, PrepareDone_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->Prepare();
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: PrepareDone_002
 * @tc.desc: Test PrepareDone interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, PrepareDone_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    filter2->LinkPipeLine("");
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->Prepare();
    sleep(sleepTime_);
    EXPECT_EQ(FilterState::READY, filter->curState_);
}

/**
 * @tc.name: Release_001
 * @tc.desc: Test Release interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Release_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->Start());
    EXPECT_EQ(Status::OK, filter->Pause());
    EXPECT_EQ(Status::OK, filter->Resume());
    EXPECT_EQ(Status::OK, filter->Stop());
    EXPECT_EQ(Status::OK, filter->Release());
}

/**
 * @tc.name: Release_002
 * @tc.desc: Test Release interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Release_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Start());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Pause());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Resume());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Stop());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Release());
    sleep(sleepTime_);
}

/**
 * @tc.name: InputOutputBuffer_001
 * @tc.desc: Test ProcessInputBuffer and ProcessInOutputBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, InputOutputBuffer_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->ProcessInputBuffer(0, 0));
    EXPECT_EQ(Status::OK, filter->ProcessOutputBuffer(0, 0));
}

/**
 * @tc.name: WaitAllState_001
 * @tc.desc: Test WaitAllState interface, wait PREPARING error
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_NE(Status::OK, filter->WaitAllState(FilterState::PREPARING));
}

/**
 * @tc.name: WaitAllState_002
 * @tc.desc: Test WaitAllState interface, wait INITIALIZED timeout
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    EXPECT_NE(Status::OK, filter->WaitAllState(FilterState::INITIALIZED));
}

/**
 * @tc.name: WaitAllState_003
 * @tc.desc: Test WaitAllState interface, wait INITIALIZED success
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_003, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    filter2->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->WaitAllState(FilterState::INITIALIZED));
}

/**
 * @tc.name: WaitAllState_004
 * @tc.desc: Test WaitAllState interface, isAsyncMode_ is false
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, WaitAllState_004, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    std::shared_ptr<Filter> filter2 = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter2->Init(nullptr, nullptr);
    filter->nextFiltersMap_[StreamType::STREAMTYPE_PACKED].push_back(filter2);
    filter->LinkPipeLine("");
    filter2->LinkPipeLine("");
    EXPECT_EQ(FilterState::INITIALIZED, filter->curState_);
    EXPECT_EQ(Status::OK, filter->WaitAllState(FilterState::INITIALIZED));
}


/**
 * @tc.name: ReleaseOnMuted_001
 * @tc.desc: Test ReleaseOnMuted interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, ReleaseOnMuted_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->Start());
    EXPECT_EQ(Status::OK, filter->Pause());
    EXPECT_EQ(Status::OK, filter->Resume());
    EXPECT_EQ(Status::OK, filter->ReleaseOnMuted(true));
}

/**
 * @tc.name: ReleaseOnMuted_002
 * @tc.desc: Test ReleaseOnMuted interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, ReleaseOnMuted_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Start());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Pause());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Resume());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->ReleaseOnMuted(true));
    sleep(sleepTime_);
}

/**
 * @tc.name: ReInitAndStart_001
 * @tc.desc: Test ReInitAndStart interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, ReInitAndStart_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, false);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    EXPECT_EQ(Status::OK, filter->Start());
    EXPECT_EQ(Status::OK, filter->Pause());
    EXPECT_EQ(Status::OK, filter->Resume());
    EXPECT_EQ(Status::OK, filter->ReleaseOnMuted(true));
    EXPECT_EQ(Status::OK, filter->ReInitAndStart());
}

/**
 * @tc.name: ReInitAndStart_002
 * @tc.desc: Test ReInitAndStart interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, ReInitAndStart_002, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    filter->Init(nullptr, nullptr);
    filter->LinkPipeLine("");
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Start());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Pause());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->Resume());
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->ReleaseOnMuted(true));
    sleep(sleepTime_);
    EXPECT_EQ(Status::OK, filter->ReInitAndStart());
    sleep(sleepTime_);
}

/**
 * @tc.name: SetPlayRange_001
 * @tc.desc: Test SetPlayRange interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, SetPlayRange_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->SetPlayRange(0, 0));
}

/**
 * @tc.name: DoSetPlayRange_001
 * @tc.desc: Test DoSetPlayRange interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, DoSetPlayRange_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->DoSetPlayRange(0, 0));
}

/**
 * @tc.name: Flush_001
 * @tc.desc: Test Flush interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, Flush_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->Flush());
}

/**
 * @tc.name: DoFlush_001
 * @tc.desc: Test DoFlush interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, DoFlush_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->DoFlush());
}

/**
 * @tc.name: HandleFormatChange_001
 * @tc.desc: Test HandleFormatChange interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, HandleFormatChange_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    EXPECT_EQ(Status::OK, filter->HandleFormatChange(meta));
}

/**
 * @tc.name: DoSetPerfRecEnabled_001
 * @tc.desc: Test DoSetPerfRecEnabled interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, DoSetPerfRecEnabled_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->DoSetPerfRecEnabled(true));
}

/**
 * @tc.name: DoProcessInputBuffer_001
 * @tc.desc: Test DoProcessInputBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, DoProcessInputBuffer_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->DoProcessInputBuffer(0, true));
}

/**
 * @tc.name: SetParameter_001
 * @tc.desc: Test SetParameter interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, SetParameter_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Meta> inMeta = std::make_shared<Meta>();
    std::shared_ptr<Meta> outMeta = std::make_shared<Meta>();
    filter->SetParameter(inMeta);
    filter->GetParameter(outMeta);
    EXPECT_EQ(inMeta, outMeta);
}

/**
 * @tc.name: GetParameter_001
 * @tc.desc: Test GetParameter interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, GetParameter_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    std::shared_ptr<Meta> inMeta = std::make_shared<Meta>();
    std::shared_ptr<Meta> outMeta = std::make_shared<Meta>();
    filter->SetParameter(inMeta);
    filter->GetParameter(outMeta);
    EXPECT_EQ(inMeta, outMeta);
}

/**
 * @tc.name: OnUpdated_001
 * @tc.desc: Test OnUpdated interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, OnUpdated_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    StreamType streamType = StreamType::STREAMTYPE_PACKED;
    std::shared_ptr<Meta> meta = std::make_shared<Meta>();
    std::shared_ptr<FilterLinkCallback> cb = std::make_shared<FilterLinkCallbackTest>();
    EXPECT_EQ(Status::OK, filter->OnUpdated(streamType, meta, cb));
}

/**
 * @tc.name: OnUnLinked_001
 * @tc.desc: Test OnUnLinked interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, OnUnLinked_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    StreamType streamType = StreamType::STREAMTYPE_PACKED;
    std::shared_ptr<FilterLinkCallback> cb = std::make_shared<FilterLinkCallbackTest>();
    EXPECT_EQ(Status::OK, filter->OnUnLinked(streamType, cb));
}

/**
 * @tc.name: SetMuted_001
 * @tc.desc: Test SetMuted interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, SetMuted_001, TestSize.Level1)
{
    std::shared_ptr<Filter> filter = std::make_shared<Filter>("testFilter", FilterType::FILTERTYPE_VENC, true);
    EXPECT_EQ(Status::OK, filter->SetMuted(true));
}

/**
 * @tc.name: NotifyRelease_001
 * @tc.desc: Test NotifyRelease interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, NotifyRelease_001, TestSize.Level1)
{
    std::shared_ptr<FilterCallback> filterCb = std::make_shared<FilterCallbackTest>();
    filterCb->NotifyRelease();
    EXPECT_NE(nullptr, filterCb);
}

/**
 * @tc.name: OnDfxEvent_001
 * @tc.desc: Test OnDfxEvent interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, OnDfxEvent_001, TestSize.Level1)
{
    std::shared_ptr<EventReceiver> eventReceiver = std::make_shared<EventReceiverTest>();
    DfxEvent event;
    eventReceiver->OnDfxEvent(event);
    EXPECT_NE(nullptr, eventReceiver);
}

/**
 * @tc.name: NotifyRelease_002
 * @tc.desc: Test NotifyRelease interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, NotifyRelease_002, TestSize.Level1)
{
    std::shared_ptr<EventReceiver> eventReceiver = std::make_shared<EventReceiverTest>();
    eventReceiver->NotifyRelease();
    EXPECT_NE(nullptr, eventReceiver);
}

/**
 * @tc.name: OnMemoryUsageEvent_001
 * @tc.desc: Test OnMemoryUsageEvent interface
 * @tc.type: FUNC
 */
HWTEST_F(FilterUnitTest, OnMemoryUsageEvent_001, TestSize.Level1)
{
    std::shared_ptr<EventReceiver> eventReceiver = std::make_shared<EventReceiverTest>();
    DfxEvent event;
    eventReceiver->OnMemoryUsageEvent(event);
    EXPECT_NE(nullptr, eventReceiver);
}
} // namespace FilterUnitTest
} // namespace Media
} // namespace OHOS