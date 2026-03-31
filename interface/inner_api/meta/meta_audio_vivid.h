/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef MEDIA_FOUNDATION_META_AUDIO_VIVID_H
#define MEDIA_FOUNDATION_META_AUDIO_VIVID_H

#include <cstdint>
#include "meta/meta.h"
#include "meta/format.h"

namespace OHOS {
namespace Media {

class AudioMetaBuilderBase {
public:
    explicit AudioMetaBuilderBase() = default;
    virtual ~AudioMetaBuilderBase() = default;
    virtual int32_t Init(const std::shared_ptr<Meta> &meta) = 0;
    virtual int32_t UpdateObjectPosCartesian(int32_t objectIndex, float x, float y, float z) = 0;
    virtual int32_t UpdateObjectPosPolar(int32_t objectIndex, float azimuth, float elevation, float distance) = 0;
    virtual int32_t UpdateObjectGain(int32_t objectIndex, float gain) = 0;
    virtual int32_t GetMetaLen(bool withStaticMeta, int32_t &len) = 0;
    virtual int32_t GetMeta(bool withStaticMeta, uint8_t *buffer, int32_t len) = 0;
};

}
}

#endif