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

#ifndef HISTREAMER_PLUGIN_CORE_SOURCE_H
#define HISTREAMER_PLUGIN_CORE_SOURCE_H

#include <cstddef>
#include <cstdint>
#include <memory>

#include "plugin/common/media_source.h"
#include "plugin/common/plugin_types.h"
#include "plugin/core/base.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct SourcePlugin;

class Source : public Base {
public:
    Source(const Source &) = delete;
    Source operator=(const Source &) = delete;
    ~Source() override = default;

    Status SetSource(std::shared_ptr<MediaSource> source);

    Status Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen);

    Status GetSize(uint64_t& size);

    Seekable GetSeekable();

    Status SeekTo(uint64_t offset);

private:
    friend class PluginManager;

    Source(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<SourcePlugin> plugin);

private:
    std::shared_ptr<SourcePlugin> source_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_SOURCE_H
