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

#ifndef HISTREAMER_UTSOURCETEST2_H
#define HISTREAMER_UTSOURCETEST2_H

#include "plugin/interface/source_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
class UtSourceTest2 : public SourcePlugin {
public:
    explicit UtSourceTest2(std::string name) : SourcePlugin(std::move(name))
    {
    }

    ~UtSourceTest2() override = default;

    Status SetSource(std::string &uri, std::shared_ptr<std::map<std::string, ValueType>> params) override;

    Status Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen) override;

    Status GetSize(size_t &size) override;

    bool IsSeekable() override;

    Status SeekTo(uint64_t offset) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(const std::shared_ptr<Callback> &cb) override;

    static bool available;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_UTSOURCETEST2_H