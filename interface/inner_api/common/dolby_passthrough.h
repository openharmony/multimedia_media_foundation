/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef DOLBY_PASSTHROUGH_H
#define DOLBY_PASSTHROUGH_H

namespace OHOS {
namespace Media {
class IDolbyPassthrough {
public:
    virtual ~IDolbyPassthrough() = default;
    virtual bool IsAudioPass(const char* mime) = 0;
    virtual std::vector<std::string> GetList() = 0;
};
} // namespace Media
} // namespace OHOS
#endif // DOLBY_PASSTHROUGH_H