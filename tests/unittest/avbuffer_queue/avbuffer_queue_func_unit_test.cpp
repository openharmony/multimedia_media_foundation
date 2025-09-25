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
#include "status.h"
#include "buffer/avbuffer_queue.h"
#include "avbuffer_queue_impl.h"
#include "avbuffer_queue_producer_impl.h"
#include "avbuffer_queue_producer_proxy.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace AVBufferQueueFuncUT {
class AVBufferQueueInnerUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);

    std::shared_ptr<AVBufferQueueImpl> avBufferQueueImpl_;
};

class BrokerListener : public IBrokerListener {
public:
    explicit BrokerListener() {}

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }

    void OnBufferFilled(std::shared_ptr<AVBuffer> &avBuffer) override
    {
        ASSERT_NE(avBuffer, nullptr);
    }
};

class ConsumerListener : public IConsumerListener {
public:
    explicit ConsumerListener() {}

    void OnBufferAvailable() override {}
};

class ProducerListener : public IRemoteStub<IProducerListener> {
public:
    explicit ProducerListener() {}

    void OnBufferAvailable() override {}
};

void AVBufferQueueInnerUnitTest::SetUpTestCase(void) {}

void AVBufferQueueInnerUnitTest::TearDownTestCase(void) {}

void AVBufferQueueInnerUnitTest::SetUp(void)
{
    std::string name = "queue";
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(name);
}

void AVBufferQueueInnerUnitTest::TearDown(void)
{
    avBufferQueueImpl_ = nullptr;
}

/**
 * @tc.name: GetLocalProducerTest
 * @tc.desc: Test get local producer
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, GetLocalProducerTest, TestSize.Level1)
{
    auto producer1 = avBufferQueueImpl_->GetLocalProducer();
    ASSERT_NE(producer1, nullptr);

    auto producer2 = avBufferQueueImpl_->GetLocalProducer();
    ASSERT_EQ(producer1, producer2);
}

/**
 * @tc.name: GetLocalConsumerTest
 * @tc.desc: Test get local consumer
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, GetLocalConsumerTest, TestSize.Level1)
{
    auto consumer1 = avBufferQueueImpl_->GetLocalConsumer();
    ASSERT_NE(consumer1, nullptr);

    auto consumer2 = avBufferQueueImpl_->GetLocalConsumer();
    ASSERT_EQ(consumer1, consumer2);
}

/**
 * @tc.name: SetQueueSizeTest
 * @tc.desc: Test set queue size
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, SetQueueSizeTest, TestSize.Level1)
{
    uint32_t size = 10;
    ASSERT_EQ(avBufferQueueImpl_->SetQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);

    size = AVBUFFER_QUEUE_MAX_QUEUE_SIZE;
    ASSERT_EQ(avBufferQueueImpl_->SetQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);

    ASSERT_EQ(avBufferQueueImpl_->SetQueueSize(size + 1), Status::ERROR_INVALID_BUFFER_SIZE);

    size = 0;
    ASSERT_EQ(avBufferQueueImpl_->SetQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);
}

/**
 * @tc.name: SetLargerQueueSize
 * @tc.desc: Test set Larger queue size
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, SetLargerQueueSize, TestSize.Level1)
{
    uint32_t size = AVBUFFER_QUEUE_MAX_QUEUE_SIZE + 1;
    ASSERT_EQ(avBufferQueueImpl_->SetLargerQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);
 
    size = AVBUFFER_QUEUE_MAX_QUEUE_SIZE_FOR_LARGER;
    ASSERT_EQ(avBufferQueueImpl_->SetLargerQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);
 
    ASSERT_EQ(avBufferQueueImpl_->SetLargerQueueSize(size + 1), Status::ERROR_INVALID_BUFFER_SIZE);
 
    size = 0;
    ASSERT_EQ(avBufferQueueImpl_->SetLargerQueueSize(size), Status::OK);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), size);
}

/**
 * @tc.name: DeleteBuffersTest
 * @tc.desc: Test delete buffers
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, DeleteBuffersTest, TestSize.Level1)
{
    sptr<IConsumerListener> listener = new ConsumerListener();
    AVBufferConfig config;
    config.size = 10;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    avBufferQueueImpl_->SetConsumerListener(listener);
    std::shared_ptr<AVBuffer> buffer1 = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer1, config), Status::OK);
    buffer1->memory_->SetSize(config.size);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer1, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer1, true), Status::OK);
    config.capacity = 20;
    std::shared_ptr<AVBuffer> buffer2 = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer2, config), Status::OK);
    buffer2->memory_->SetSize(config.size);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer2, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer2, true), Status::OK);

    uint32_t count = 10;
    avBufferQueueImpl_->InsertFreeBufferInOrder(buffer1->GetUniqueId());
    avBufferQueueImpl_->InsertFreeBufferInOrder(buffer2->GetUniqueId());
    avBufferQueueImpl_->DeleteBuffers(count);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), 0);

    count = 0;
    avBufferQueueImpl_->DeleteBuffers(count);
    ASSERT_EQ(avBufferQueueImpl_->GetQueueSize(), 0);
}

/**
 * @tc.name: ChceckConfigTest
 * @tc.desc: Test check config
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ChceckConfigTest, TestSize.Level1)
{
    AVBufferConfig config;
    int32_t size = 10;

    // memoryType : UNKNOWN_MEMORY
    config.memoryType = MemoryType::UNKNOWN_MEMORY;
    EXPECT_EQ(avBufferQueueImpl_->CheckConfig(config), Status::ERROR_UNEXPECTED_MEMORY_TYPE);

    // memoryType_ : UNKNOWN_MEMORY, memoryType : !UNKNOWN_MEMORY
    config.memoryType = MemoryType::SHARED_MEMORY;
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(size, MemoryType::UNKNOWN_MEMORY, "test");
    EXPECT_EQ(avBufferQueueImpl_->CheckConfig(config), Status::OK);

    // memoryType_ : !UNKNOWN_MEMORY, memoryType : != memoryType_
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(size, MemoryType::SHARED_MEMORY, "test");
    EXPECT_EQ(avBufferQueueImpl_->CheckConfig(config), Status::ERROR_UNEXPECTED_MEMORY_TYPE);

    // memoryType_ : !UNKNOWN_MEMORY, memoryType : == memoryType_
    config.memoryType = MemoryType::SHARED_MEMORY;
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(size, MemoryType::SHARED_MEMORY, "test");
    EXPECT_EQ(avBufferQueueImpl_->CheckConfig(config), Status::OK);
}

/**
 * @tc.name: PushBufferTest_001
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_001, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);

    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_SIZE);
}

/**
 * @tc.name: PushBufferTest_002
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_002, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);

    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_STATE);
}

/**
 * @tc.name: PushBufferTest_003
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_003, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = -1;
    config.capacity = 0;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    ASSERT_NE(nullptr, buffer);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_SIZE);
}

/**
 * @tc.name: PushBufferTest_004
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_004, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    ASSERT_NE(nullptr, buffer);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    avBufferQueueImpl_->DeleteBuffers(1);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
}

/**
 * @tc.name: PushBufferTest_005
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_005, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    ASSERT_NE(nullptr, buffer);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    sptr<IBrokerListener> listener = new BrokerListener();
    avBufferQueueImpl_->SetBrokerListener(listener);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
}

/**
 * @tc.name: PushBufferTest_006
 * @tc.desc: Test push buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, PushBufferTest_006, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    ASSERT_NE(nullptr, buffer);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    sptr<IBrokerListener> listener = new BrokerListener();
    avBufferQueueImpl_->SetBrokerListener(listener);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, false), Status::OK);
}

/**
 * @tc.name: ReturnBufferTest
 * @tc.desc: Test return buffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ReturnBufferTest, TestSize.Level1)
{
    sptr<IConsumerListener> listener = new ConsumerListener();
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    avBufferQueueImpl_->SetConsumerListener(listener);
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    ASSERT_NE(nullptr, buffer);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    buffer->memory_->SetSize(config.size);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->AcquireBuffer(buffer), Status::OK);

    EXPECT_EQ(avBufferQueueImpl_->ReleaseBuffer(buffer->GetUniqueId()), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->ReturnBuffer(buffer, false), Status::ERROR_INVALID_BUFFER_STATE);

    buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    buffer->memory_->SetSize(config.size);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->ReturnBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_STATE);

    EXPECT_EQ(avBufferQueueImpl_->ReturnBuffer(2, true), Status::ERROR_INVALID_BUFFER_ID);

    buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(avBufferQueueImpl_->AllocBuffer(buffer, config), Status::OK);
    buffer->memory_->SetSize(config.size);
    EXPECT_EQ(avBufferQueueImpl_->RequestReuseBuffer(buffer, config), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
    avBufferQueueImpl_->DeleteBuffers(buffer->GetUniqueId());
    EXPECT_EQ(avBufferQueueImpl_->ReturnBuffer(buffer->GetUniqueId(), true), Status::ERROR_INVALID_BUFFER_ID);
}

/**
 * @tc.name: RemoveBrokerListenerTest
 * @tc.desc: Test RemoveBrokerListener interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, RemoveBrokerListenerTest, TestSize.Level1)
{
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(AVBUFFER_QUEUE_MAX_QUEUE_SIZE + 1,
        MemoryType::SHARED_MEMORY, "RemoveBrokerListenerTest");
    sptr<IBrokerListener> listener1 = new BrokerListener();
    sptr<IBrokerListener> listener2 = new BrokerListener();
    sptr<IBrokerListener> listener3 = new BrokerListener();
    avBufferQueueImpl_->SetBrokerListener(listener1);
    avBufferQueueImpl_->SetBrokerListener(listener2);
    avBufferQueueImpl_->SetBrokerListener(listener3);
    int32_t brokerListenersSize = avBufferQueueImpl_->brokerListeners_.size();
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->RemoveBrokerListener(listener3));
    EXPECT_EQ(brokerListenersSize - 1, avBufferQueueImpl_->brokerListeners_.size());
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->RemoveBrokerListener(listener1));
    EXPECT_EQ(brokerListenersSize - 1, avBufferQueueImpl_->brokerListeners_.size());
}

/**
 * @tc.name: ClearTest
 * @tc.desc: Test Clear interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ClearTest, TestSize.Level1)
{
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVAllocator> allocator = AVAllocatorFactory::CreateSharedAllocator(MemoryFlag::MEMORY_READ_WRITE);
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(allocator, 1, 1);
    ASSERT_NE(nullptr, buffer);
    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->AllocBuffer(buffer, config));
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->RequestReuseBuffer(buffer, config));
    buffer->memory_->SetSize(1);
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->PushBuffer(buffer, true));
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->Clear());
}

/**
 * @tc.name: ClearBufferIfTest
 * @tc.desc: Test ClearBufferIf interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ClearBufferIfTest, TestSize.Level1)
{
    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);
 
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
 
    constexpr uint32_t totalBufferCount = 10;
    for (uint32_t i = 0; i < totalBufferCount; i++) {
        std::shared_ptr<AVAllocator> allocator =
            AVAllocatorFactory::CreateSharedAllocator(MemoryFlag::MEMORY_READ_WRITE);
        std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(allocator, 1, 1);
        ASSERT_NE(nullptr, buffer);
        EXPECT_EQ(Status::OK, avBufferQueueImpl_->AllocBuffer(buffer, config));
        EXPECT_EQ(Status::OK, avBufferQueueImpl_->RequestReuseBuffer(buffer, config));
        buffer->memory_->SetSize(1);
        buffer->pts_ = static_cast<int64_t>(i);
        EXPECT_EQ(Status::OK, avBufferQueueImpl_->PushBuffer(buffer, true));
    }
    constexpr uint32_t startPts = 4;
    auto isNewerSample = [](const std::shared_ptr<AVBuffer>& buffer) {
        return (buffer != nullptr) && (buffer->pts_ >= startPts);
    };
    // clear pts remain pts from 0 ~ (startPts - 1 )
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->ClearBufferIf(isNewerSample));
    EXPECT_EQ(startPts, avBufferQueueImpl_->GetFilledBufferSize());
}

/**
 * @tc.name: GetFilledBufferSize_001
 * @tc.desc: Test GetFilledBufferSize interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, GetFilledBufferSize_001, TestSize.Level1)
{
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);
    EXPECT_EQ(avBufferQueueImpl_->SetQueueSize(2), Status::OK);

    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);

    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(buffer->GetConfig().size, 0);

    EXPECT_EQ(avBufferQueueImpl_->AttachBuffer(buffer, false), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->RequestBuffer(buffer, config, 0), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_SIZE);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->RequestBuffer(buffer, config, 0), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    buffer->memory_->SetSize(100);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 100);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 1);

    std::shared_ptr<AVBuffer> bufferConsumer;
    EXPECT_EQ(avBufferQueueImpl_->AcquireBuffer(bufferConsumer), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->AcquireBuffer(bufferConsumer), Status::ERROR_NO_DIRTY_BUFFER);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);
}

/**
 * @tc.name: RequestBufferWaitUs_001
 * @tc.desc: Test RequestBufferWaitUs interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, RequestBufferWaitUs_001, TestSize.Level1)
{
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);
    EXPECT_EQ(avBufferQueueImpl_->SetQueueSize(2), Status::OK);

    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);

    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(buffer->GetConfig().size, 0);

    EXPECT_EQ(avBufferQueueImpl_->AttachBuffer(buffer, false), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->RequestBufferWaitUs(buffer, config, 0), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_SIZE);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->RequestBufferWaitUs(buffer, config, 0), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 0);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    buffer->memory_->SetSize(100);
    EXPECT_EQ(avBufferQueueImpl_->PushBuffer(buffer, true), Status::OK);
    EXPECT_EQ(buffer->GetConfig().size, 100);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 1);

    std::shared_ptr<AVBuffer> bufferConsumer;
    EXPECT_EQ(avBufferQueueImpl_->AcquireBuffer(bufferConsumer), Status::OK);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);

    EXPECT_EQ(avBufferQueueImpl_->AcquireBuffer(bufferConsumer), Status::ERROR_NO_DIRTY_BUFFER);
    EXPECT_EQ(avBufferQueueImpl_->GetFilledBufferSize(), 0);
}

/**
 * @tc.name: ProducerProxyCreate_001
 * @tc.desc: Test ProducerProxyCreate interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyCreate_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = nullptr;
    auto result = AVBufferQueueProducerProxy::Create(object);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ProducerProxyCreate_002
 * @tc.desc: Test ProducerProxyCreate interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyCreate_002, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    auto result = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(result, nullptr);

}

/**
 * @tc.name: ProducerProxyGetQueueSize_001
 * @tc.desc: Test ProducerProxyGetQueueSize interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyGetQueueSize_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->GetQueueSize(), 0);
}

/**
 * @tc.name: ProducerProxySetQueueSize_001
 * @tc.desc: Test ProducerProxySetQueueSize interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxySetQueueSize_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->SetQueueSize(10), Status::OK);
}

/**
 * @tc.name: ProducerProxyRequestBuffer_001
 * @tc.desc: Test ProducerProxyRequestBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyRequestBuffer_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    EXPECT_EQ(avBufferQueueImpl_->SetQueueSize(2), Status::OK);

    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);

    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);

    EXPECT_EQ(avBufferQueueImpl_->AttachBuffer(buffer, false), Status::OK);
    EXPECT_EQ(proxy->RequestBuffer(buffer, config, 0), Status::OK);
}

/**
 * @tc.name: ProducerProxyPushBuffer_001
 * @tc.desc: Test ProducerProxyPushBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyPushBuffer_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    AVBufferConfig config;
    config.size = 1;
    config.capacity = 1;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVAllocator> allocator = AVAllocatorFactory::CreateSharedAllocator(MemoryFlag::MEMORY_READ_WRITE);
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(allocator, 1, 1);
    ASSERT_NE(nullptr, buffer);
    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->AllocBuffer(buffer, config));
    EXPECT_EQ(Status::OK, avBufferQueueImpl_->RequestReuseBuffer(buffer, config));
    buffer->memory_->SetSize(1);
    EXPECT_EQ(proxy->PushBuffer(buffer, true), Status::OK);
}

/**
 * @tc.name: ProducerProxyReturnBuffer_001
 * @tc.desc: Test ProducerProxyReturnBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyReturnBuffer_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    buffer->memory_->SetSize(config.size);
    EXPECT_EQ(proxy->ReturnBuffer(buffer, true), Status::ERROR_INVALID_BUFFER_ID);
}

/**
 * @tc.name: ProducerProxyAttachBuffer_001
 * @tc.desc: Test ProducerProxyAttachBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyAttachBuffer_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    EXPECT_EQ(avBufferQueueImpl_->SetQueueSize(2), Status::OK);
    sptr<IConsumerListener> listener = new ConsumerListener();
    avBufferQueueImpl_->SetConsumerListener(listener);
    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    EXPECT_EQ(proxy->AttachBuffer(buffer, false), Status::ERROR_UNEXPECTED_MEMORY_TYPE);
}

/**
 * @tc.name: ProducerProxyDetachBuffer_001
 * @tc.desc: Test ProducerProxyDetachBuffer interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyDetachBuffer_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    AVBufferConfig config;
    config.size = 100;
    config.capacity = 100;
    config.memoryType = MemoryType::VIRTUAL_MEMORY;
    std::shared_ptr<AVBuffer> buffer = AVBuffer::CreateAVBuffer(config);
    buffer->memory_->SetSize(config.size);
    EXPECT_EQ(proxy->DetachBuffer(buffer), Status::ERROR_INVALID_BUFFER_ID);
}

/**
 * @tc.name: ProducerProxySetBufferFilledListener_001
 * @tc.desc: Test ProducerProxySetBufferFilledListener interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxySetBufferFilledListener_001, TestSize.Level1)
{
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(AVBUFFER_QUEUE_MAX_QUEUE_SIZE + 1,
        MemoryType::SHARED_MEMORY, "RemoveBrokerListenerTest");
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    sptr<IBrokerListener> listener = new BrokerListener();
    EXPECT_EQ(proxy->SetBufferFilledListener(listener), Status::ERROR_IPC_SEND_REQUEST);
}

/**
 * @tc.name: ProducerProxyRemoveBufferFilledListener_001
 * @tc.desc: Test ProducerProxyRemoveBufferFilledListener interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyRemoveBufferFilledListener_001, TestSize.Level1)
{
    avBufferQueueImpl_ = std::make_shared<AVBufferQueueImpl>(AVBUFFER_QUEUE_MAX_QUEUE_SIZE + 1,
        MemoryType::SHARED_MEMORY, "RemoveBrokerListenerTest");
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    sptr<IBrokerListener> listener = new BrokerListener();
    proxy->SetBufferFilledListener(listener);
    EXPECT_EQ(proxy->RemoveBufferFilledListener(listener), Status::ERROR_IPC_SEND_REQUEST);
}

/**
 * @tc.name: ProducerProxySetBufferAvailableListener_001
 * @tc.desc: Test ProducerProxySetBufferAvailableListener interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxySetBufferAvailableListener_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    sptr<IProducerListener> listener = new ProducerListener();
    EXPECT_EQ(proxy->SetBufferAvailableListener(listener), Status::OK);
}

/**
 * @tc.name: ProducerProxyClear_001
 * @tc.desc: Test ProducerProxyClear interface
 * @tc.type: FUNC
 */
HWTEST_F(AVBufferQueueInnerUnitTest, ProducerProxyClear_001, TestSize.Level1)
{
    sptr<IRemoteObject> object = new AVBufferQueueProducerImpl(avBufferQueueImpl_);
    EXPECT_NE(object, nullptr);
    auto proxy = AVBufferQueueProducerProxy::Create(object);
    EXPECT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->Clear(), Status::OK);
}
} // namespace AVBufferQueueFuncUT
} // namespace Media
} // namespace OHOS
