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

#include <cstdio>
#include <cstring>
#include <sstream>
#include <dirent.h>
#include "common/log.h"
#include "osal/utils/dump_buffer.h"
#include "parameter.h"
#include "osal/filesystem/file_system.h"

namespace OHOS {
namespace Media {
std::map<std::string, FILE*> allDumpFileFds;
void DumpAVBufferToFile(const std::string para, const std::string& fileName, const std::shared_ptr<AVBuffer>& buffer)
{
    std::string dumpPara = para;
    if (dumpPara.empty()) {
        MEDIA_LOG_I("dumpPara is empty.");
        return;
    }
    if (buffer == nullptr) {
        MEDIA_LOG_I("buffer is nullptr.");
        return;
    }
    if (buffer->memory_ == nullptr) {
        MEDIA_LOG_I("memory is nullptr.");
        return;
    }
    size_t bufferSize = buffer->memory_->GetSize();
    FALSE_RETURN(bufferSize != 0);
    if (dumpPara != "w" && dumpPara != "a") {
        MEDIA_LOG_I("dumpPara is invalid.");
        return;
    }
    std::string mode = dumpPara + "b+";
    auto iter = allDumpFileFds.find(fileName);
    if (!FileSystem::IsExists(DUMP_FILE_DIR + fileName) || iter == allDumpFileFds.end()
        || allDumpFileFds.find(fileName)->second == nullptr) {
        allDumpFileFds.insert({fileName, std::fopen(std::string(DUMP_FILE_DIR + fileName).c_str(), mode.c_str())});
    }
    if (buffer->memory_->GetAddr() == nullptr) {
        MEDIA_LOG_I("dump buffer data is null.");
        return;
    }
    if (allDumpFileFds.find(fileName)->second == nullptr) {
        MEDIA_LOG_I("dump file is null.");
        return;
    }
    int ret = fwrite(reinterpret_cast<const char*>(buffer->memory_->GetAddr()),
        1, bufferSize, allDumpFileFds.find(fileName)->second);
    if (ret < 0) {
        MEDIA_LOG_I("dump is fail.");
    }
}
} // Media
} // OHOS