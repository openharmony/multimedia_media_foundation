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

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "DumpBuffer" };
constexpr size_t DUMP_DATA_UNIT = 1; // data unit is 1 byte
}

namespace OHOS {
namespace Media {
std::map<std::string, FILE*> allDumpFileFds;
void DumpAVBufferToFile(const std::string& para, const std::string& fileName, const std::shared_ptr<AVBuffer>& buffer)
{
    MEDIA_LOG_D("dump avbuffer to %{public}s", fileName.c_str());
    if (buffer == nullptr || buffer->memory_ == nullptr) {
        MEDIA_LOG_E("buffer or memory is nullptr.");
        return;
    }
    FALSE_RETURN_MSG((para == "w" || para == "a") && !fileName.empty(), "para or fileName is invalid.");
    size_t bufferSize = static_cast<size_t>(buffer->memory_->GetSize());
    FALSE_RETURN((bufferSize != 0) && (buffer->memory_->GetAddr() != nullptr));
    std::string mode = para + "b+";
    std::string filePath = DUMP_FILE_DIR + fileName;
    if (filePath == "") {
        return;
    }
    FILE* dumpFile = std::fopen(filePath.c_str(), mode.c_str());
    if (dumpFile == nullptr) {
        MEDIA_LOG_E("dump buffer to file failed.");
        return;
    }
    size_t ret =
        fwrite(reinterpret_cast<const char*>(buffer->memory_->GetAddr()), DUMP_DATA_UNIT, bufferSize, dumpFile);
    if (ret < 0) {
        MEDIA_LOG_I("dump is fail.");
    }
    std::fclose(dumpFile);
}
} // Media
} // OHOS