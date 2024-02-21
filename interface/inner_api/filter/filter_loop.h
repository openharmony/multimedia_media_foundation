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
 *
 * Description: API of message queue thread
 */

#ifndef FILTER_LOOP_H
#define FILTER_LOOP_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <list>

namespace OHOS {
namespace Media {
namespace Pipeline {

using MsgId = uint64_t;
struct MsgInfo {
    int32_t what;
    int32_t arg;
    bool flushed;
};

class Filter;
class FilterLoop {
public:
    FilterLoop(std::string name, Filter* filter);
    virtual ~FilterLoop();
    void SendAsyncMsg(int32_t what, int arg, int64_t delayUs = 0, bool isDataMsg = false);

    static constexpr MsgId ASYNC_MSG_ID = 0;

    void InitAsync(int arg = 0, int64_t delayUs = 0);

    void PrepareAsync(int arg = 0, int64_t delayUs = 0);

    void StartAsync(int arg = 0, int64_t delayUs = 0);

    void ResumeAsync(int arg = 0, int64_t delayUs = 0);

    void PauseAsync(int arg = 0, int64_t delayUs = 0);

    void StopAsync(int arg = 0, int64_t delayUs = 0);

    void FlushAsync(int arg = 0, int64_t delayUs = 0);

    void ReleaseAsync(int arg = 0, int64_t delayUs = 0);

    void ProcessInputBufferAsync(int arg = 0, int64_t delayUs = 0);

    void ProcessOutBufferAsync(int arg = 0, int64_t delayUs = 0);

private:
    void MainLoop();
    uint32_t GenerateMsgId();
    void HandleMessage(const MsgInfo &info);
    using TimeUs = int64_t;
    static TimeUs GetNowUs();

private:
    std::unique_ptr<std::thread> loopThread_;
    std::mutex mutex_;
    std::string name_;
    bool threadNeedStop_ = false;
    bool dataQueueRunning_ = false;

    std::map<TimeUs, MsgInfo> commandQueue_;  // msg will be sorted by timeUs
    std::map<TimeUs, MsgInfo> dataQueue_;  // msg will be sorted by timeUs
    std::condition_variable threadCond_;
    Filter *filter_;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif
