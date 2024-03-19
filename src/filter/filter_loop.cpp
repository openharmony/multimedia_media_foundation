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
 *
 * Description: implementation of message queue thread
 */

#include "common/log.h"
#include "filter/filter_loop.h"
#include "filter/filter.h"
#include <chrono>
#include <cinttypes>

using namespace std;

namespace OHOS {
namespace Media {
namespace Pipeline {
FilterLoop::FilterLoop(string name, Filter* filter) : name_(name)
{
    loopThread_ = make_unique<thread>(&FilterLoop::MainLoop, this);
    filter_ = filter;
}

FilterLoop::~FilterLoop()
{
    MEDIA_LOG_I("dtor called");
    threadNeedStop_ = true;
    threadCond_.notify_all();
    if (loopThread_ != nullptr && loopThread_->joinable()) {
        loopThread_->join();
        loopThread_ = nullptr;
    }
    MEDIA_LOG_I("dtor end");
}

void FilterLoop::InitAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::INIT, arg, delayUs);
}

void FilterLoop::PrepareAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::PREPARE, arg, delayUs);
}

void FilterLoop::StartAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::START, arg, delayUs);
}

void FilterLoop::ResumeAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::RESUME, arg, delayUs);
}

void FilterLoop::PauseAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::PAUSE, arg, delayUs);
}

void FilterLoop::StopAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::STOP, arg, delayUs);
}

void FilterLoop::FlushAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::FLUSH, arg, delayUs);
}

void FilterLoop::ReleaseAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::RELEASE, arg, delayUs);
}

void FilterLoop::ProcessInputBufferAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::PROCESS_INPUT_BUFFER, arg, delayUs, true);
}

void FilterLoop::ProcessOutBufferAsync(int arg, int64_t delayUs)
{
    SendAsyncMsg(FilterPlaybackCommand::PROCESS_OUTPUT_BUFFER, arg, delayUs, true);
}

void FilterLoop::SendAsyncMsg(int32_t what, int arg, int64_t delayUs, bool isDataMsg)
{
    lock_guard<mutex> lock(mutex_);
    if (threadNeedStop_) {
        return;
    }
    TimeUs nowUs = GetNowUs();
    if (delayUs < 0) {
        delayUs = 0;
    }
    TimeUs msgProcessTime = (delayUs > INT64_MAX - nowUs) ? INT64_MAX : (nowUs + delayUs);
    if (isDataMsg) {
        if (dataQueue_.find(msgProcessTime) != dataQueue_.end()) {
            MEDIA_LOG_W("DUPLICATIVE dataQueue_ TIMESTAMP!!!");
            msgProcessTime++;
        }
        dataQueue_[msgProcessTime] = MsgInfo {what, arg, false};
        MEDIA_LOG_D("Handle data what %{public}d at Time %{public}" PRId64, what, msgProcessTime);
    } else {
        if (commandQueue_.find(msgProcessTime) != commandQueue_.end()) {
            MEDIA_LOG_W("DUPLICATIVE commandQueue_ TIMESTAMP!!!");
            msgProcessTime++;
        }
        if (what == FilterPlaybackCommand::FLUSH) {
            // flush the message in dataqueue
            for (auto msg = dataQueue_.begin(); msg != dataQueue_.end(); msg++) {
                msg->second.flushed = true;
            }
        }
        commandQueue_[msgProcessTime] = MsgInfo {what, arg, false};
        MEDIA_LOG_D("Handle command what %{public}d at Time %{public}" PRId64, what, msgProcessTime);
    }
    threadCond_.notify_all();
}

void FilterLoop::MainLoop()
{
    MEDIA_LOG_I("MainLoop begin");
    constexpr uint32_t nameSizeMax = 15;
    pthread_setname_np(pthread_self(), name_.substr(0, nameSizeMax).c_str());
    while (true) {
        MsgInfo info;
        {
            unique_lock<mutex> lock(mutex_);
            threadCond_.wait(lock, [this] {
                return threadNeedStop_ || !commandQueue_.empty() || (!dataQueue_.empty() && dataQueueRunning_);
            });
            if (threadNeedStop_) {
                MEDIA_LOG_I("stopped, remain %{public}zu command %{public}zu data unprocessed",
                    commandQueue_.size(), dataQueue_.size());
                break;
            }
            TimeUs nowUs = GetNowUs();
            TimeUs processUs;
            bool processCommand = true;
            if (commandQueue_.empty()) {
                processUs = dataQueue_.begin()->first;
                processCommand = false;
            } else if (dataQueue_.empty() || !dataQueueRunning_) {
                processUs = commandQueue_.begin()->first;
                processCommand = true;
            } else {
                TimeUs commandTime = commandQueue_.begin()->first;
                MsgInfo commandInfo = commandQueue_.begin()->second;
                TimeUs dataTime = dataQueue_.begin()->first;
                MsgInfo dataInfo = dataQueue_.begin()->second;
                // pause and stop command should be handle top priority
                if (commandTime <= nowUs && (commandInfo.what == FilterPlaybackCommand::PAUSE
                    || commandInfo.what == FilterPlaybackCommand::RELEASE)) {
                    processUs = commandTime;
                    processCommand = true;
                } else if (commandTime < dataTime) {
                    // handle flush message before flush command
                    if (commandInfo.what == FilterPlaybackCommand::FLUSH && dataInfo.flushed) {
                        processUs = nowUs;
                        processCommand = false;
                    } else {
                        processUs = commandTime;
                        processCommand = true;
                    }
                } else {
                    processUs = dataInfo.flushed ? nowUs : dataTime;
                    processCommand = false;
                }
            }
            MEDIA_LOG_D("Check message what:%{public}d at processUs:%{public}" PRId64 " nowUs:%{public}" PRId64,
                processCommand ? commandQueue_.begin()->second.what : dataQueue_.begin()->second.what, processUs,
                nowUs);
            if (processUs > nowUs) {
                threadCond_.wait_for(lock, chrono::microseconds(processUs - nowUs));
                continue;
            }
            if (processCommand) {
                info = commandQueue_.begin()->second;
                commandQueue_.erase(commandQueue_.begin());
            } else {
                info = dataQueue_.begin()->second;
                dataQueue_.erase(dataQueue_.begin());
            }
        }
        HandleMessage(info);
    }
    MEDIA_LOG_I("MainLoop exit");
}

void FilterLoop::HandleMessage(const MsgInfo &info)
{
    switch (info.what) {
        case Pipeline::FilterPlaybackCommand::INIT: {
            auto doInitRet = filter_->DoInit();
            if (doInitRet == Status::OK) {
                filter_->ChangeState(FilterState::INITIALIZED);
            } else {
                filter_->SetErrCode(doInitRet);
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::PREPARE: {
            auto doPrepareRet = filter_->DoPrepare();
            if (doPrepareRet == Status::OK) {
                filter_->ChangeState(FilterState::READY);
            } else {
                filter_->SetErrCode(doPrepareRet);
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::START: {
            if (filter_->DoStart() == Status::OK) {
                filter_->ChangeState(FilterState::RUNNING);
                dataQueueRunning_ = true;
            } else {
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::PAUSE: {
            if (filter_->DoPause() == Status::OK) {
                filter_->ChangeState(FilterState::PAUSED);
                dataQueueRunning_ = false;
            } else {
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::RESUME: {
            if (filter_->DoResume() == Status::OK) {
                filter_->ChangeState(FilterState::RUNNING);
                dataQueueRunning_ = true;
            } else {
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::STOP:{
            if (filter_->DoStop() == Status::OK) {
                filter_->ChangeState(FilterState::STOPPED);
            } else {
                filter_->ChangeState(FilterState::ERROR);
            }
            break;
        }
        case Pipeline::FilterPlaybackCommand::FLUSH: {
            filter_->DoFlush();
            filter_->ChangeState(FilterState::FLUSHED);
            break;
        }
        case Pipeline::FilterPlaybackCommand::RELEASE: {
            filter_->DoRelease();
            filter_->ChangeState(FilterState::RELEASED);
            threadNeedStop_ = true;
            break;
        }
        case Pipeline::FilterPlaybackCommand::PROCESS_INPUT_BUFFER:
            filter_->DoProcessInputBuffer(info.arg, info.flushed);
            break;
        case Pipeline::FilterPlaybackCommand::PROCESS_OUTPUT_BUFFER: {
            filter_->DoProcessOutputBuffer(info.arg, info.flushed);
            break;
        }
        default:
            break;
    }
}

FilterLoop::TimeUs FilterLoop::GetNowUs()
{
    auto now = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
}

} // namespace Pipeline
} // namespace Media
} // namespace OHOS
