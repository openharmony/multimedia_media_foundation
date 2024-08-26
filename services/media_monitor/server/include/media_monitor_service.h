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

#ifndef MEDIA_MONITOR_H
#define MEDIA_MONITOR_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "ipc_skeleton.h"
#include "iremote_stub.h"
#include "system_ability.h"
#include "media_monitor_stub.h"
#include "event_aggregate.h"
#include "audio_buffer_cache.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {

class EventAggregate;

struct MessageSignal {
    std::mutex messageMutex_;
    std::condition_variable messageCond_;
    std::queue<std::shared_ptr<EventBean>> messageQueue_;
    std::atomic<bool> isRunning_ = false;
};

struct DumpSignal {
    std::mutex dumpMutex_;
    std::condition_variable dumpCond_;
    std::queue<std::pair<std::string, std::shared_ptr<DumpBuffer>>> dumpQueue_;
    std::atomic<bool> isRunning_ = false;
};

const mode_t FILE_MODE = 0775;
const int FILE_MAX_SIZE = 20480000 * 8;
const int MAX_FILE_COUNT = 20;
const int MAX_DUMP_TIME = 90;

const std::string DEFAULT_DUMP_DIR = "/data/audio_debug/";
const std::string BETA_DUMP_DIR = "/data/log/audiodump/";

class MediaMonitorService : public SystemAbility, public MediaMonitorStub {
    DECLARE_SYSTEM_ABILITY(MediaMonitorService);

public:
    DISALLOW_COPY_AND_MOVE(MediaMonitorService);
    explicit MediaMonitorService(int32_t systemAbilityId, bool runOnCreate = true);
    virtual ~MediaMonitorService() = default;
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

    void WriteLogMsg(std::shared_ptr<EventBean> &bean)  override;

    int32_t GetAudioRouteMsg(std::map<PerferredType, std::shared_ptr<MonitorDeviceInfo>> &perferredDevices) override;

    int32_t SetMediaParameters(const std::string &dumpType, const std::string &dumpEnable) override;

    int32_t WriteAudioBuffer(const std::string &fileName, void *ptr, size_t size) override;

    int32_t GetInputBuffer(std::shared_ptr<DumpBuffer> &buffer, int32_t size) override;

    int32_t InputBufferFilled(const std::string &fileName, uint64_t bufferId, int32_t size) override;
private:
    MediaMonitorService();
    void MessageLoopFunc();
    void GetMessageFromQueue(std::shared_ptr<EventBean> &message);
    void AddMessageToQueue(std::shared_ptr<EventBean> &message);
    void AudioEncodeDump();

    EventAggregate& eventAggregate_;
    AudioMemo& audioMemo_;
    std::unique_ptr<std::thread> messageLoopThread_ = nullptr;
    std::shared_ptr<MessageSignal> signal_ = nullptr;
    bool isExit_ = false;

    bool VerifyIsAudio();
    bool IsNeedDump();
    int32_t DumpThreadProcess();
    void DumpThreadStart();
    void DumpThreadStop();
    void DumpThreadExit();
    void DumpLoopFunc();
    void DumpFileClear();
    void DumpBufferClear();
    void HistoryFilesHandle();
    void AudioBufferRelease(std::shared_ptr<DumpBuffer> &buffer);
    bool DeleteHistoryFile(const std::string &filePath);
    void DumpBufferWrite(std::queue<std::pair<std::string, std::shared_ptr<DumpBuffer>>> &bufferQueue);
    void AddBufferToQueue(const std::string &fileName, std::shared_ptr<DumpBuffer> &buffer);
    void WriteBufferFromQueue(const std::string &fileName, std::shared_ptr<DumpBuffer> &buffer);
    bool isDumpExit_ = false;
    bool dumpEnable_ = false;
    std::mutex paramMutex_;
    std::mutex bufferMutex_;
    std::string dumpType_ = DEFAULT_DUMP_TYPE;
    std::string versionType_ = COMMERCIAL_VERSION;
    std::string fileFloader_ = DEFAULT_DUMP_DIR;
    std::unique_ptr<std::thread> dumpLoopThread_ = nullptr;
    std::shared_ptr<DumpSignal> dumpSignal_ = nullptr;
    std::shared_ptr<AudioBufferCache> audioBufferCache_ = nullptr;
    std::time_t dumpThreadTime_ = 0;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_H