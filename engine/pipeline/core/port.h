/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_PORT_H
#define HISTREAMER_PIPELINE_CORE_PORT_H

#include <memory>
#include <string>
#include <utility>
#include "foundation/cpp_ext/type_traits_ext.h"
#include "foundation/utils/constants.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/type_define.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class InfoTransfer;
enum class PortType { IN, OUT };

struct PortDesc {
    std::string name;
    bool isPcm;
};

struct PortInfo {
    PortType type;
    std::vector<PortDesc> ports;
};
enum class WorkMode { PUSH, PULL };

class Port {
public:
    Port(InfoTransfer *ownerFilter, std::string portName)
        : filter(ownerFilter), name(std::move(portName)) {}
    virtual ~Port() = default;
    const std::string& GetName();
    const InfoTransfer* GetOwnerFilter() const;
    virtual std::shared_ptr<Port> GetPeerPort();
    WorkMode GetWorkMode()
    {
        return workMode;
    }
    virtual ErrorCode Connect(const std::shared_ptr<Port>& port) = 0;
    virtual ErrorCode Disconnect() = 0;
    virtual ErrorCode Activate(const std::vector<WorkMode>& modes, WorkMode& outMode) = 0;
    virtual bool Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                           Plugin::Capability& negotiatedCap,
                           const Plugin::Meta& upstreamParams,
                           Plugin::Meta& downstreamParams) = 0;
    virtual bool Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta, Plugin::Meta& upstreamParams,
                           Plugin::Meta& downstreamParams) = 0;
    /**
     * push data
     * @param buffer pushed buffer
     * @param offset means the offset from the start of the stream between Filter.Start and Filter.Stop. -1 means ignore
     * this parameter.
     */
    virtual void PushData(const AVBufferPtr& buffer, int64_t offset) = 0;
    virtual ErrorCode PullData(uint64_t offset, size_t size, AVBufferPtr& data) = 0;

protected:
    InfoTransfer* filter;
    WorkMode workMode {WorkMode::PUSH};
    const std::string name;
};

class OutPort;

class InPort : public Port {
public:
    explicit InPort(InfoTransfer *filter, std::string name = PORT_NAME_DEFAULT)
        : Port(filter, std::move(name)) {}
    ~InPort() override = default;
    ErrorCode Connect(const std::shared_ptr<Port>& port) override;
    ErrorCode Disconnect() override;
    ErrorCode Activate(const std::vector<WorkMode>& modes, WorkMode& outMode) override;
    std::shared_ptr<Port> GetPeerPort() override;
    bool Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    bool Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta, Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    void PushData(const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode PullData(uint64_t offset, size_t size, AVBufferPtr& data) override;

private:
    std::weak_ptr<Port> prevPort;
};

class OutPort : public Port {
public:
    explicit OutPort(InfoTransfer *filter, std::string name = PORT_NAME_DEFAULT)
        : Port(filter, std::move(name)) {}
    ~OutPort() override = default;
    ErrorCode Connect(const std::shared_ptr<Port>& port) override;
    ErrorCode Disconnect() override;
    ErrorCode Activate(const std::vector<WorkMode>& modes, WorkMode& outMode) override;
    std::shared_ptr<Port> GetPeerPort() override;
    bool Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    bool Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta, Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    void PushData(const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode PullData(uint64_t offset, size_t size, AVBufferPtr& data) override;

private:
    bool InSamePipeline(const std::shared_ptr<Port>& port) const;

private:
    std::shared_ptr<Port> nextPort;
};

class EmptyInPort : public InPort {
public:
    static std::shared_ptr<InPort> GetInstance()
    {
        return port;
    }
    EmptyInPort() : InPort(nullptr, "emptyInPort") {}
    ~EmptyInPort() override = default;
    ErrorCode Connect(const std::shared_ptr<Port>& another) override;
    ErrorCode Activate(const std::vector<WorkMode>& modes, WorkMode& outMode) override;
    bool Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    bool Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta, Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    void PushData(const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode PullData(uint64_t offset, size_t size, AVBufferPtr& data) override;

private:
    static std::shared_ptr<InPort> port;
};

class EmptyOutPort : public OutPort {
public:
    static std::shared_ptr<OutPort> GetInstance()
    {
        return port;
    }
    EmptyOutPort() : OutPort(nullptr, "emptyOutPort") {}
    ~EmptyOutPort() override = default;
    ErrorCode Connect(const std::shared_ptr<Port>& another) override;
    ErrorCode Activate(const std::vector<WorkMode>& modes, WorkMode& outMode) override;
    bool Negotiate(const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    bool Configure(const std::shared_ptr<const Plugin::Meta>& upstreamMeta, Plugin::Meta& upstreamParams,
                   Plugin::Meta& downstreamParams) override;
    void PushData(const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode PullData(uint64_t offset, size_t size, AVBufferPtr& data) override;

private:
    static std::shared_ptr<OutPort> port;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_CORE_PORT_H
