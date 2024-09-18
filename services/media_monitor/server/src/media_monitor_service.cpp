/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "media_monitor_service.h"
#include <filesystem>
#include <sys/stat.h>
#include "system_ability_definition.h"
#include "parameters.h"
#include "log.h"
#include "monitor_error.h"
#include "media_audio_encoder.h"
#include "media_monitor_death_recipient.h"
#include "iservice_registry.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaMonitorService"};
}

using namespace std;

namespace OHOS {
namespace Media {
namespace MediaMonitor {
constexpr int32_t WAIT_DUMP_TIMEOUT_S = 1;
REGISTER_SYSTEM_ABILITY_BY_ID(MediaMonitorService, MEDIA_MONITOR_SERVICE_ID, true)

MediaMonitorService::MediaMonitorService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate),
    eventAggregate_(EventAggregate::GetEventAggregate()),
    audioMemo_(AudioMemo::GetAudioMemo())
{
        MEDIA_LOG_I("MediaMonitorService constructor");
}

void MediaMonitorService::OnDump()
{
    MEDIA_LOG_I("OnDump");
}

int32_t MediaMonitorService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    MEDIA_LOG_I("MediaMonitorService Dump");
    std::string dumpString = "------------------MediaMonitor------------------\n";

    eventAggregate_.WriteInfo(fd, dumpString);
    audioMemo_.WriteInfo(fd, dumpString);
    write(fd, dumpString.c_str(), dumpString.size());
    return SUCCESS;
}

void MediaMonitorService::OnStart()
{
    MEDIA_LOG_I("OnStart");
    bool res = Publish(this);
    if (!res) {
        MEDIA_LOG_I("publish sa err");
        return;
    }
    signal_ = std::make_shared<MessageSignal>();
    signal_->isRunning_.store(true);
    messageLoopThread_ = make_unique<thread>(&MediaMonitorService::MessageLoopFunc, this);
    pthread_setname_np(messageLoopThread_->native_handle(), "MMLoopThread");
    versionType_ = OHOS::system::GetParameter("const.logsystem.versiontype", COMMERCIAL_VERSION);
    MEDIA_LOG_I("MediaMonitorService get version type %{public}s", versionType_.c_str());
}

void MediaMonitorService::OnStop()
{
    MEDIA_LOG_I("OnStop");
    isExit_ = true;
    signal_->isRunning_.store(false);
    DumpThreadExit();
}

void MediaMonitorService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    MEDIA_LOG_I("systemAbilityId:%{public}d", systemAbilityId);
}

void MediaMonitorService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    MEDIA_LOG_I("OnRemoveSystemAbility systemAbilityId:%{public}d removed", systemAbilityId);
}

void MediaMonitorService::WriteLogMsg(std::shared_ptr<EventBean> &bean)
{
    MEDIA_LOG_D("Write event");
    if (bean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    AddMessageToQueue(bean);
}

void MediaMonitorService::MessageLoopFunc()
{
    MEDIA_LOG_D("MediaMonitorService MessageLoopFunc");
    if (signal_ == nullptr) {
        MEDIA_LOG_E("MediaMonitorService MessageLoopFunc signal_ is nullptr");
        return;
    }
    while (!isExit_) {
        if (!signal_->isRunning_.load()) {
            break;
        }
        std::shared_ptr<EventBean> msg;
        {
            unique_lock<mutex> lock(signal_->messageMutex_);
            signal_->messageCond_.wait(lock, [this]() { return signal_->messageQueue_.size() > 0; });
            if (!signal_->isRunning_.load()) {
                break;
            }
            msg = signal_->messageQueue_.front();
            if (msg == nullptr) {
                signal_->messageQueue_.pop();
                continue;
            }
            signal_->messageQueue_.pop();
        }
        GetMessageFromQueue(msg);
    }
}

void MediaMonitorService::GetMessageFromQueue(std::shared_ptr<EventBean> &message)
{
    if (message == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return;
    }
    eventAggregate_.WriteEvent(message);
}

void MediaMonitorService::AddMessageToQueue(std::shared_ptr<EventBean> &message)
{
    MEDIA_LOG_D("MediaMonitorService AddMessageToQueue");
    if (signal_ == nullptr) {
        MEDIA_LOG_E("signal_ is null");
        return;
    }
    unique_lock<mutex> lock(signal_->messageMutex_);
    if (!signal_->isRunning_.load()) {
        MEDIA_LOG_E("!isRunning_");
        return;
    }
    signal_->messageQueue_.push(message);
    signal_->messageCond_.notify_all();
}

int32_t MediaMonitorService::GetAudioRouteMsg(std::map<PerferredType,
    std::shared_ptr<MonitorDeviceInfo>> &perferredDevices)
{
    MEDIA_LOG_D("MediaMonitorService GetAudioRouteMsg");
    return audioMemo_.GetAudioRouteMsg(perferredDevices);
}

void MediaMonitorService::AudioEncodeDump()
{
    MEDIA_LOG_I("encode pcm start");
    std::shared_ptr<MediaAudioEncoder> encoder = std::make_shared<MediaAudioEncoder>();
    encoder->EncodePcmFiles(fileFloader_);
    encoder = nullptr;
}

bool MediaMonitorService::IsNeedDump()
{
    MEDIA_LOG_D("dumpType_:%{public}s, dumpEnable:%{public}d", dumpType_.c_str(), dumpEnable_);
    if (versionType_ != BETA_VERSION) {
        return false;
    }
    if (dumpType_ == DEFAULT_DUMP_TYPE || dumpType_ == BETA_DUMP_TYPE) {
        return dumpEnable_;
    }
    return false;
}

int32_t MediaMonitorService::WriteAudioBuffer(const std::string &fileName, std::shared_ptr<AVBuffer> &buffer)
{
    if (!IsNeedDump()) {
        AudioBufferRelease(buffer);
        return ERR_ILLEGAL_STATE;
    }
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERROR, "input buffer nullptr");
    if (buffer->memory_ == nullptr) {
        MEDIA_LOG_E("WriteAudioBuffer get buffer failed");
        return ERROR;
    }
    AddBufferToQueue(fileName, buffer);
    return SUCCESS;
}

int32_t MediaMonitorService::GetInputBuffer(std::shared_ptr<AVBuffer> &buffer, int32_t size)
{
    if (!IsNeedDump()) {
        return ERR_ILLEGAL_STATE;
    }

    unique_lock<mutex> lock(bufferMutex_);
    if (audioBufferCache_) {
        audioBufferCache_->RequestBuffer(buffer, size);
    }
    return SUCCESS;
}

int32_t MediaMonitorService::InputBufferFilled(std::string fileName, uint64_t bufferId)
{
    std::shared_ptr<AVBuffer> buffer = nullptr;
    if (audioBufferCache_) {
        audioBufferCache_->GetBufferById(buffer, bufferId);
    }
    if (buffer == nullptr || buffer->memory_ == nullptr) {
        return ERROR;
    }
    AddBufferToQueue(fileName, buffer);
    return SUCCESS;
}

int32_t MediaMonitorService::SetMediaParameters(const std::string& dumpType, const std::string& dumpEnable)
{
    MEDIA_LOG_D("SetMediaParameters dumpEnable: %{public}s", dumpEnable.c_str());
    unique_lock<mutex> lock(paramMutex_);
    if (dumpType != DEFAULT_DUMP_TYPE && dumpType != BETA_DUMP_TYPE) {
        MEDIA_LOG_E("dumpType:%{public}s isvaild", dumpType.c_str());
        return ERROR;
    }

    bool isDumpEnable = (dumpEnable == "true") ? true : false;
    if (dumpEnable_ == isDumpEnable) {
        MEDIA_LOG_D("set dumpType is repeate, current enable:%{public}d", dumpEnable_);
        return ERROR;
    }

    dumpType_ = dumpType;
    dumpEnable_ = isDumpEnable;
    fileFloader_ = (dumpType_ == BETA_DUMP_TYPE) ? BETA_DUMP_DIR : DEFAULT_DUMP_DIR;
    dumpThreadTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (dumpEnable_) {
        DumpThreadStart();
    } else {
        DumpThreadStop();
    }
    return SUCCESS;
}

void MediaMonitorService::DumpThreadStart()
{
    MEDIA_LOG_I("DumpThreadStart enter");
    DumpFileClear();
    isDumpExit_ = false;
    if (dumpSignal_ != nullptr) {
        dumpSignal_->isRunning_.store(false);
    }
    if (dumpLoopThread_ && dumpLoopThread_->joinable()) {
        dumpLoopThread_->join();
    }
    dumpSignal_ = std::make_shared<DumpSignal>();
    audioBufferCache_ = std::make_shared<AudioBufferCache>();
    dumpSignal_->isRunning_.store(true);
    dumpLoopThread_ = std::make_unique<thread>(&MediaMonitorService::DumpLoopFunc, this);
    pthread_setname_np(dumpLoopThread_->native_handle(), "MDLoopThread");
}

void MediaMonitorService::DumpThreadStop()
{
    MEDIA_LOG_I("DumpThreadStop enter");
    dumpEnable_ = false;
    DumpBufferClear();
    dumpSignal_->isRunning_.store(false);
    if (dumpType_ == BETA_DUMP_TYPE) {
        HistoryFilesHandle();
        AudioEncodeDump();
    }
    if (audioBufferCache_) {
        audioBufferCache_->Clear();
        audioBufferCache_ = nullptr;
    }
    dumpType_ = DEFAULT_DUMP_TYPE;
    fileFloader_ = DEFAULT_DUMP_DIR;
}

void MediaMonitorService::DumpThreadExit()
{
    MEDIA_LOG_I("DumpThreadExit enter");
    dumpEnable_ = false;
    isDumpExit_ = true;
    dumpSignal_->isRunning_.store(false);
}

void MediaMonitorService::DumpLoopFunc()
{
    MEDIA_LOG_I("DumpLoopFunc enter");
    FALSE_RETURN_MSG(dumpSignal_ != nullptr, "signal is nullptr");
    while (!isDumpExit_) {
        if (!dumpSignal_->isRunning_.load()) {
            MEDIA_LOG_I("DumpLoopFunc running exit");
            break;
        }
        std::queue<std::pair<std::string, std::shared_ptr<AVBuffer>>> tmpBufferQue;
        {
            unique_lock<mutex> lock(dumpSignal_->dumpMutex_);
            dumpSignal_->dumpCond_.wait_for(lock, std::chrono::seconds(WAIT_DUMP_TIMEOUT_S), [this]() {
                return dumpSignal_->dumpQueue_.size() > 0;
            });
            tmpBufferQue.swap(dumpSignal_->dumpQueue_);
        }

        DumpBufferWrite(tmpBufferQue);
        int duration = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - dumpThreadTime_;
        if (!dumpEnable_ || (dumpType_ == BETA_DUMP_TYPE && duration >= MAX_DUMP_TIME)) {
            MEDIA_LOG_D("dump duration %{public}d", duration);
            DumpThreadStop();
        }
    }
}

void MediaMonitorService::DumpBufferWrite(std::queue<std::pair<std::string, std::shared_ptr<AVBuffer>>> &bufferQueue)
{
    std::pair<std::string, std::shared_ptr<AVBuffer>> dumpData;
    while (!bufferQueue.empty()) {
        dumpData = bufferQueue.front();
        bufferQueue.pop();
        if (dumpEnable_) {
            WriteBufferFromQueue(dumpData.first, dumpData.second);
        }
        AudioBufferRelease(dumpData.second);
    }
}

void MediaMonitorService::AddBufferToQueue(const std::string &fileName, std::shared_ptr<AVBuffer> &buffer)
{
    MEDIA_LOG_D("AddBufferToQueue enter");
    FALSE_RETURN_MSG(dumpSignal_ != nullptr, "signal is nullptr");
    unique_lock<mutex> lock(dumpSignal_->dumpMutex_);
    if (!dumpSignal_->isRunning_.load()) {
        MEDIA_LOG_E("not running");
        return;
    }
    dumpSignal_->dumpQueue_.push(std::make_pair(fileName, buffer));
    dumpSignal_->dumpCond_.notify_all();
}

void MediaMonitorService::WriteBufferFromQueue(const std::string &fileName, std::shared_ptr<AVBuffer> &buffer)
{
    MEDIA_LOG_D("WriteBufferFromQueue enter");
    FALSE_RETURN_MSG(buffer != nullptr, "buffer is nullptr");
    std::string realFilePath = fileFloader_ + fileName;
    FILE *dumpFile = fopen(realFilePath.c_str(), "a");
    FALSE_RETURN_MSG(dumpFile != nullptr, "pcm file %{public}s open fail, error %{public}s",
        realFilePath.c_str(), strerror(errno));
    if (fseek(dumpFile, 0, SEEK_END)) {
        return;
    }
    int filelen = ftell(dumpFile);
    if (filelen >= FILE_MAX_SIZE && dumpType_ == BETA_DUMP_TYPE) {
        (void)fclose(dumpFile);
        (void)DeleteHistoryFile(realFilePath);
        dumpFile = fopen(realFilePath.c_str(), "a");
        FALSE_RETURN_MSG(dumpFile != nullptr, "reopen file failed");
    }
    size_t bufferSize = static_cast<size_t>(buffer->memory_->GetSize());
    size_t writeResult = fwrite(buffer->memory_->GetAddr(), 1, bufferSize, dumpFile);
    FALSE_RETURN_MSG(writeResult == bufferSize, "write file failed");
    (void)fclose(dumpFile);
    dumpFile = nullptr;
}

void MediaMonitorService::DumpFileClear()
{
    for (const auto& elem : std::filesystem::directory_iterator(BETA_DUMP_DIR)) {
        (void)DeleteHistoryFile(elem.path().string());
    }
}

void MediaMonitorService::DumpBufferClear()
{
    dumpSignal_->dumpMutex_.lock();
    while (!dumpSignal_->dumpQueue_.empty()) {
        auto& dumpData = dumpSignal_->dumpQueue_.front();
        AudioBufferRelease(dumpData.second);
        dumpSignal_->dumpQueue_.pop();
    }
    dumpSignal_->dumpMutex_.unlock();
}

void MediaMonitorService::AudioBufferRelease(std::shared_ptr<AVBuffer> &buffer)
{
    FALSE_RETURN_MSG(buffer != nullptr, "buffer is nullptr");
    if (audioBufferCache_) {
        audioBufferCache_->ReleaseBuffer(buffer);
    }
}

void MediaMonitorService::HistoryFilesHandle()
{
    MEDIA_LOG_D("HistoryFilesHandle enter");
    if (dumpType_ != BETA_DUMP_TYPE) {
        return;
    }
    std::vector<std::filesystem::path> mediaFileVecs;
    for (const auto& elem : std::filesystem::directory_iterator(fileFloader_)) {
        if (std::filesystem::is_regular_file(elem.status())) {
            mediaFileVecs.emplace_back(elem.path());
        }
    }
    MEDIA_LOG_D("HistoryFilesHandle dumpType %{public}s size: %{public}d",
        dumpType_.c_str(), (int)mediaFileVecs.size());
    std::sort(mediaFileVecs.begin(), mediaFileVecs.end(),
        [](const filesystem::path& file1, const filesystem::path& file2) {
            return filesystem::file_time_type(filesystem::last_write_time(file1)) <
            filesystem::file_time_type(filesystem::last_write_time(file2));
    });
    while (static_cast<int>(mediaFileVecs.size()) > MAX_FILE_COUNT) {
        std::string delFilePath = (mediaFileVecs.front()).string();
        if (DeleteHistoryFile(delFilePath) && !mediaFileVecs.empty()) {
            mediaFileVecs.erase(mediaFileVecs.begin());
        }
    }
    MEDIA_LOG_I("HistoryFilesHandle exit");
}

bool MediaMonitorService::DeleteHistoryFile(const std::string &filePath)
{
    MEDIA_LOG_I("DeleteHistoryFile enter");
    if (!IsRealPath(filePath)) {
        return false;
    }
    (void)chmod(filePath.c_str(), FILE_MODE);
    if (remove(filePath.c_str()) != 0) {
        MEDIA_LOG_E("remove file %{public}s failed ", filePath.c_str());
        return false;
    }
    return true;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS