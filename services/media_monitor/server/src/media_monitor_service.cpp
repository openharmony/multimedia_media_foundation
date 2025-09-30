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
constexpr int32_t AUDIO_UID = 1041;
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

void MediaMonitorService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    MEDIA_LOG_I("systemAbilityId:%{public}d", systemAbilityId);
}

void MediaMonitorService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    MEDIA_LOG_I("OnRemoveSystemAbility systemAbilityId:%{public}d removed", systemAbilityId);
}

ErrCode MediaMonitorService::WriteLogMsg(const EventBean &bean)
{
    MEDIA_LOG_D("Write event");
    auto eventBean = std::make_shared<EventBean>(bean);
    if (eventBean == nullptr) {
        MEDIA_LOG_E("eventBean is nullptr");
        return ERROR;
    }
    AddMessageToQueue(eventBean);
    return SUCCESS;
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

ErrCode MediaMonitorService::GetAudioRouteMsg(std::unordered_map<int32_t,
    MonitorDeviceInfo> &preferredDevices, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    MEDIA_LOG_D("MediaMonitorService GetAudioRouteMsg");
    std::map<PreferredType, shared_ptr<MonitorDeviceInfo>> preferredDevicePtrs;
    funcResult = audioMemo_.GetAudioRouteMsg(preferredDevicePtrs);
    for (auto &devicePtr : preferredDevicePtrs) {
        preferredDevices[static_cast<int32_t>(devicePtr.first)] = *devicePtr.second;
    }
    return funcResult;
}

ErrCode MediaMonitorService::GetAudioExcludedDevicesMsg(std::unordered_map<int32_t,
    std::vector<MonitorDeviceInfo>> &excludedDevices, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    MEDIA_LOG_D("MediaMonitorService GetAudioExcludedDevicesMsg");
    std::map<AudioDeviceUsage, std::vector<std::shared_ptr<MonitorDeviceInfo>>> excludedDevicesPtr;
    funcResult = audioMemo_.GetAudioExcludedDevicesMsg(excludedDevicesPtr);
    for (auto &devicePtr : excludedDevicesPtr) {
        std::vector<MonitorDeviceInfo> deviceInfos;
        for (auto &deviceInfo : devicePtr.second) {
            deviceInfos.push_back(*deviceInfo);
        }
        excludedDevices[static_cast<int32_t>(devicePtr.first)] = deviceInfos;
    }
    return funcResult;
}

ErrCode MediaMonitorService::GetAudioAppStateMsg(std::unordered_map<int32_t, MonitorAppStateInfo> &appStateMapOut,
    int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    std::map<int32_t, std::shared_ptr<MonitorAppStateInfo>> appStateInner;
    funcResult = audioMemo_.GetAudioAppStateMsg(appStateInner);
    for (auto &appState : appStateInner) {
        appStateMapOut[static_cast<int32_t>(appState.first)] = *appState.second;
    }
    return funcResult;
}

ErrCode MediaMonitorService::GetDistributedDeviceInfo(std::vector<std::string> &deviceInfos, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    funcResult = audioMemo_.GetDistributedDeviceInfo(deviceInfos);
    return funcResult;
}

ErrCode MediaMonitorService::GetDistributedSceneInfo(std::string &sceneInfo, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    funcResult = audioMemo_.GetDistributedSceneInfo(sceneInfo);
    return funcResult;
}

void MediaMonitorService::AudioEncodeDump()
{
    MEDIA_LOG_I("encode pcm start");
    std::shared_ptr<MediaAudioEncoder> encoder = std::make_shared<MediaAudioEncoder>();
    encoder->EncodePcmFiles(fileFloader_);
    encoder = nullptr;
}

bool MediaMonitorService::VerifyIsAudio()
{
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    if (callingUid == AUDIO_UID) {
        return true;
    }
    return false;
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

ErrCode MediaMonitorService::WriteAudioBuffer(const std::string &fileName, uint64_t ptr, uint32_t size,
    int32_t &funcResult)
{
    funcResult = SUCCESS;
    return funcResult;
}

ErrCode MediaMonitorService::GetInputBuffer(DumpBuffer &buffer, int32_t size, int32_t &funcResult)
{
    if (versionType_ != BETA_VERSION) {
        funcResult = ERROR;
        return funcResult;
    }

    if (!IsNeedDump()) {
        funcResult = ERR_ILLEGAL_STATE;
        return funcResult;
    }

    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permissionn denied");
    unique_lock<mutex> lock(bufferMutex_);
    if (audioBufferCache_) {
        shared_ptr<DumpBuffer> dumpBuffer = std::make_shared<DumpBuffer>();
        audioBufferCache_->RequestBuffer(dumpBuffer, size);
        FALSE_UPDATE_RETURN_V_MSG_E(dumpBuffer != nullptr, funcResult, ERROR, "request buffer failed");
        buffer = *dumpBuffer;
    }
    funcResult = SUCCESS;
    return funcResult;
}

ErrCode MediaMonitorService::InputBufferFilled(const std::string &fileName, uint64_t bufferId, uint32_t size,
    int32_t &funcResult)
{
    if (versionType_ != BETA_VERSION) {
        funcResult = ERROR;
        return funcResult;
    }

    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permissionn denied");
    FALSE_UPDATE_RETURN_V_MSG_E(audioBufferCache_ != nullptr, funcResult, ERROR, "buffer cahce nullptr");
    std::shared_ptr<DumpBuffer> buffer = nullptr;
    audioBufferCache_->GetBufferById(buffer, bufferId);
    FALSE_RETURN_V_MSG_E(buffer != nullptr, ERROR, "get buffer falied");
    audioBufferCache_->SetBufferSize(buffer, size);
    AddBufferToQueue(fileName, buffer);
    funcResult = SUCCESS;
    return funcResult;
}

ErrCode MediaMonitorService::GetPcmDumpStatus(int32_t &dumpEnable, int32_t &funcResult)
{
    if (versionType_ != BETA_VERSION) {
        funcResult = ERROR;
        return funcResult;
    }

    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permissionn denied");
    dumpEnable = dumpEnable_ ? 1 : 0;
    funcResult = SUCCESS;
    return funcResult;
}

ErrCode MediaMonitorService::SetMediaParameters(const std::string &dumpType, const std::string &dumpEnable,
    int32_t &funcResult)
{
    if (versionType_ != BETA_VERSION) {
        funcResult = ERROR;
        return funcResult;
    }

    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permissionn denied");
    MEDIA_LOG_D("SetMediaParameters dumpEnable: %{public}s", dumpEnable.c_str());
    unique_lock<mutex> lock(paramMutex_);
    if (dumpType != DEFAULT_DUMP_TYPE && dumpType != BETA_DUMP_TYPE) {
        MEDIA_LOG_E("dumpType:%{public}s isvaild", dumpType.c_str());
        funcResult = ERROR;
        return funcResult;
    }

    bool isDumpEnable = (dumpEnable == "true") ? true : false;
    if (dumpEnable_ == isDumpEnable) {
        MEDIA_LOG_D("set dumpType is repeate, current enable:%{public}d", dumpEnable_);
        funcResult = ERROR;
        return funcResult;
    }

    dumpType_ = dumpType;
    dumpEnable_ = isDumpEnable;
    fileFloader_ = (dumpType_ == BETA_DUMP_TYPE) ? BETA_DUMP_DIR : DEFAULT_DUMP_DIR;
    dumpThreadTime_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    funcResult = DumpThreadProcess();
    return funcResult;
}

int32_t MediaMonitorService::DumpThreadProcess()
{
    if (dumpSignal_ != nullptr) {
        dumpSignal_->isRunning_.store(false);
    }
    if (dumpEnable_) {
        isDumpExit_ = false;
        if (dumpLoopThread_ && dumpLoopThread_->joinable()) {
            dumpLoopThread_->join();
        }
        dumpBufferWrap_ = std::make_shared<DumpBufferWrap>();
        if (!dumpBufferWrap_->Open()) {
            return ERROR;
        }
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
    audioBufferCache_ = std::make_shared<AudioBufferCache>(dumpBufferWrap_);
    dumpSignal_ = std::make_shared<DumpSignal>();
    dumpSignal_->isRunning_.store(true);
    dumpLoopThread_ = std::make_unique<thread>(&MediaMonitorService::DumpLoopFunc, this);
    pthread_setname_np(dumpLoopThread_->native_handle(), "MDLoopThread");
}

void MediaMonitorService::DumpThreadStop()
{
    MEDIA_LOG_I("DumpThreadStop enter");
    dumpSignal_->isRunning_.store(false);
    dumpEnable_ = false;
    DumpBufferClear();
    if (dumpType_ == BETA_DUMP_TYPE) {
        HistoryFilesHandle();
        AudioEncodeDump();
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
    FALSE_RETURN_MSG(dumpBufferWrap_ != nullptr, "buffer wrap is nullptr");
    while (!isDumpExit_) {
        if (!dumpSignal_->isRunning_.load()) {
            MEDIA_LOG_I("DumpLoopFunc running exit");
            break;
        }
        std::queue<std::pair<std::string, std::shared_ptr<DumpBuffer>>> tmpBufferQue;
        {
            unique_lock<mutex> lock(dumpSignal_->dumpMutex_);
            dumpSignal_->dumpCond_.wait_for(lock, std::chrono::seconds(WAIT_DUMP_TIMEOUT_S), [this]() {
                return dumpSignal_->dumpQueue_.size() > 0;
            });
            tmpBufferQue.swap(dumpSignal_->dumpQueue_);
        }

        DumpBufferWrite(tmpBufferQue);
        int duration = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - dumpThreadTime_;
        if (dumpType_ == BETA_DUMP_TYPE && duration >= MAX_DUMP_TIME) {
            MEDIA_LOG_I("dump duration %{public}d", duration);
            DumpThreadStop();
        }
    }
    if (audioBufferCache_ != nullptr) {
        audioBufferCache_->Clear();
        audioBufferCache_ = nullptr;
    }
    if (dumpBufferWrap_ != nullptr) {
        dumpBufferWrap_->Close();
        dumpBufferWrap_ = nullptr;
    }
}

void MediaMonitorService::DumpBufferWrite(std::queue<std::pair<std::string, std::shared_ptr<DumpBuffer>>> &bufferQueue)
{
    std::pair<std::string, std::shared_ptr<DumpBuffer>> dumpData;
    while (!bufferQueue.empty()) {
        dumpData = bufferQueue.front();
        bufferQueue.pop();
        if (dumpEnable_) {
            WriteBufferFromQueue(dumpData.first, dumpData.second);
        }
        AudioBufferRelease(dumpData.second);
    }
}

void MediaMonitorService::AddBufferToQueue(const std::string &fileName, std::shared_ptr<DumpBuffer> &buffer)
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

void MediaMonitorService::WriteBufferFromQueue(const std::string &fileName, std::shared_ptr<DumpBuffer> &buffer)
{
    MEDIA_LOG_D("WriteBufferFromQueue enter");
    FALSE_RETURN_MSG(buffer != nullptr, "buffer is nullptr");
    std::string realFilePath = fileFloader_ + fileName;
    FALSE_RETURN_MSG(IsRealPath(fileFloader_), "check path failed");
    FILE *dumpFile = fopen(realFilePath.c_str(), "a");
    FALSE_RETURN_MSG(dumpFile != nullptr, "pcm file open failed");
    if (fseek(dumpFile, 0, SEEK_END)) {
        (void)fclose(dumpFile);
        return;
    }
    int filelen = ftell(dumpFile);
    if (filelen >= FILE_MAX_SIZE && dumpType_ == BETA_DUMP_TYPE) {
        (void)fclose(dumpFile);
        (void)DeleteHistoryFile(realFilePath);
        dumpFile = fopen(realFilePath.c_str(), "a");
        FALSE_RETURN_MSG(dumpFile != nullptr, "reopen file failed");
    }
    size_t bufferSize = static_cast<size_t>(dumpBufferWrap_->GetSize(buffer.get()));
    uint8_t *bufferAddr = dumpBufferWrap_->GetAddr(buffer.get());
    if (bufferAddr == nullptr) {
        (void)fclose(dumpFile);
        dumpFile = nullptr;
        return;
    }
    (void)fwrite(bufferAddr, 1, bufferSize, dumpFile);
    (void)fclose(dumpFile);
    dumpFile = nullptr;
}

void MediaMonitorService::DumpFileClear()
{
    std::error_code errorCode;
    std::filesystem::directory_iterator iter(BETA_DUMP_DIR, errorCode);
    if (errorCode) {
        MEDIA_LOG_E("get file failed");
        return;
    }

    for (const auto &elem : iter) {
        (void)DeleteHistoryFile(elem.path().string());
    }
}

void MediaMonitorService::DumpBufferClear()
{
    dumpSignal_->dumpMutex_.lock();
    while (!dumpSignal_->dumpQueue_.empty()) {
        auto &dumpData = dumpSignal_->dumpQueue_.front();
        AudioBufferRelease(dumpData.second);
        dumpSignal_->dumpQueue_.pop();
    }
    dumpSignal_->dumpMutex_.unlock();
}

void MediaMonitorService::AudioBufferRelease(std::shared_ptr<DumpBuffer> &buffer)
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

    std::error_code errorCode;
    std::filesystem::directory_iterator iter(fileFloader_, errorCode);
    if (errorCode) {
        MEDIA_LOG_E("get file failed");
        return;
    }
    std::vector<std::filesystem::path> mediaFileVecs;
    for (const auto &elem : iter) {
        if (std::filesystem::is_regular_file(elem.status())) {
            mediaFileVecs.emplace_back(elem.path());
        }
    }
    MEDIA_LOG_D("HistoryFilesHandle dumpType %{public}s size: %{public}d",
        dumpType_.c_str(), (int)mediaFileVecs.size());
    std::sort(mediaFileVecs.begin(), mediaFileVecs.end(),
        [](const std::filesystem::path &file1, const std::filesystem::path &file2) {
            return std::filesystem::file_time_type(std::filesystem::last_write_time(file1)) <
            std::filesystem::file_time_type(std::filesystem::last_write_time(file2));
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
        MEDIA_LOG_E("remove file failed ");
        return false;
    }
    return true;
}

ErrCode MediaMonitorService::ErasePreferredDeviceByType(int32_t preferredType, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    MEDIA_LOG_D("ErasePreferredDeviceByType enter");
    funcResult = audioMemo_.ErasePreferredDeviceByType(static_cast<PreferredType>(preferredType));
    return funcResult;
}

ErrCode MediaMonitorService::GetCollaborativeDeviceState(
    std::unordered_map<std::string, uint32_t> &storedCollaborativeMap, int32_t &funcResult)
{
    FALSE_UPDATE_RETURN_V_MSG_E(VerifyIsAudio(), funcResult, ERROR, "client permission denied");
    MEDIA_LOG_D("MediaMonitorService GetCollaborativeDeviceState");
    // need to send unordered_map to idl
    std::map<std::string, uint32_t> addressToCollaborativeEnabledMap;
    funcResult = audioMemo_.GetCollaborativeDeviceState(addressToCollaborativeEnabledMap);
    for (auto &p: addressToCollaborativeEnabledMap) {
        storedCollaborativeMap[p.first] = p.second;
    }
    return funcResult;
}
} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS