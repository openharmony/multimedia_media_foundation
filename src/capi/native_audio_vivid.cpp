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

#include "native_audio_vivid.h"
#include "meta/meta_audio_vivid.h"
#include "meta/format.h"
#include "meta/meta.h"
#include "common/native_mfmagic.h"
#include "common/log.h"
#include <dlfcn.h>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "NativeAudioVivid" };
}

using namespace OHOS;
using namespace OHOS::Media;

using CreateBuilderFunc = AudioMetaBuilderBase *(*)();
using DestroyBuilderFunc = void (*)(AudioMetaBuilderBase *);

class AudioVividMetaManager {
public:
    AudioVividMetaManager() {}
    ~AudioVividMetaManager()
    {
        if (builder_ && destroyFunc_) {
            destroyFunc_(builder_);
            builder_ = nullptr;
        }
        if (library_) {
            ::dlclose(library_);
            library_ = nullptr;
        }
    }

    OH_AVErrCode Init(const Format &format)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        OH_AVErrCode loadRet = LoadAndCreateBuilder();
        if (loadRet != AV_ERR_OK) {
            return loadRet;
        }
        auto meta = const_cast<Format &>(format).GetMeta();
        int32_t ret = builder_->Init(meta);
        if (ret != 0) {
            MEDIA_LOG_E("audio vivid builder init fail");
            CleanupAfterInitFail();
            return AV_ERR_INVALID_VAL;
        }
        MEDIA_LOG_I("audio vivid builder init");
        return AV_ERR_OK;
    }

    OH_AVErrCode InitEmpty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        OH_AVErrCode loadRet = LoadAndCreateBuilder();
        if (loadRet != AV_ERR_OK) {
            return loadRet;
        }
        if (builder_->InitEmpty() != 0) {
            MEDIA_LOG_E("audio vivid builder InitEmpty fail");
            CleanupAfterInitFail();
            return AV_ERR_UNSUPPORT;
        }
        MEDIA_LOG_I("audio vivid builder init empty");
        return AV_ERR_OK;
    }

    OH_AVErrCode UpdateBaseMeta(const uint8_t *buffer, int32_t len)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->UpdateBaseMeta(buffer, len) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode AddObject(int32_t &objectIndex)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->AddObject(objectIndex) != 0) {
            return AV_ERR_UNKNOWN;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode RemoveObject(int32_t objectIndex)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->RemoveObject(objectIndex) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode UpdateObjPosCartesian(int32_t objId, float x, float y, float z)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->UpdateObjectPosCartesian(objId, x, y, z) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode UpdateObjPosPolar(int32_t objId, float azimuth, float elevation, float distance)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->UpdateObjectPosPolar(objId, azimuth, elevation, distance) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode UpdateObjGain(int32_t objId, float gain)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->UpdateObjectGain(objId, gain) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode GetLen(bool withStaticMeta, int32_t &len) const
    {
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->GetMetaLen(withStaticMeta, len) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

    OH_AVErrCode GetMeta(bool withStaticMeta, uint8_t *buffer, int32_t len) const
    {
        FALSE_RETURN_V_MSG_E(builder_ != nullptr, AV_ERR_INVALID_STATE, "builder not init!");
        if (builder_->GetMeta(withStaticMeta, buffer, len) != 0) {
            return AV_ERR_INVALID_VAL;
        }
        return AV_ERR_OK;
    }

private:
    OH_AVErrCode LoadAndCreateBuilder()
    {
        auto ptr = ::dlopen("libAudioVividMetaBuilder.z.so", RTLD_NOW | RTLD_LOCAL);
        FALSE_RETURN_V_MSG_E(ptr != nullptr, AV_ERR_UNSUPPORT, "lib not found, not support audio vivid metadata");

        CreateBuilderFunc createFunc =
            reinterpret_cast<CreateBuilderFunc>(::dlsym(ptr, "CreateAudioVividMetaBuilder"));
        DestroyBuilderFunc destroyFunc =
            reinterpret_cast<DestroyBuilderFunc>(::dlsym(ptr, "DestroyAudioVividMetaBuilder"));
        if (!createFunc || !destroyFunc) {
            ::dlclose(ptr);
            MEDIA_LOG_E("audio vivid meta load func failed!");
            return AV_ERR_UNKNOWN;
        }
        builder_ = createFunc();
        if (!builder_) {
            MEDIA_LOG_E("audio vivid builder nullptr");
            ::dlclose(ptr);
            return AV_ERR_UNKNOWN;
        }
        library_ = ptr;
        destroyFunc_ = destroyFunc;
        return AV_ERR_OK;
    }

    void CleanupAfterInitFail()
    {
        if (builder_ && destroyFunc_) {
            destroyFunc_(builder_);
            builder_ = nullptr;
        }
        if (library_) {
            ::dlclose(library_);
            library_ = nullptr;
        }
    }

    AudioMetaBuilderBase *builder_ = nullptr;
    void *library_ = nullptr;
    DestroyBuilderFunc destroyFunc_ = nullptr;
    std::mutex mutex_;
};

OH_AVErrCode OH_AudioVividMetaBuilder_Create(OH_AudioVividMetaBuilder **builder, const OH_AVFormat *format)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    FALSE_RETURN_V_MSG_E(format != nullptr, AV_ERR_INVALID_VAL, "input format is nullptr!");
    FALSE_RETURN_V_MSG_E(format->magic_ == MFMagic::MFMAGIC_FORMAT, AV_ERR_INVALID_VAL, "format magic error!");

    AudioVividMetaManager *innerBuilder = new(std::nothrow) AudioVividMetaManager();
    FALSE_RETURN_V_MSG_E(innerBuilder != nullptr, AV_ERR_UNKNOWN, "create inner builder error!");
    OH_AVErrCode ret = innerBuilder->Init(format->format_);
    if (ret != AV_ERR_OK) {
        delete innerBuilder;
        return ret;
    }
    *builder = reinterpret_cast<OH_AudioVividMetaBuilder *>(innerBuilder);
    return AV_ERR_OK;
}

OH_AVErrCode OH_AudioVividMetaBuilder_UpdateObjectPos(OH_AudioVividMetaBuilder *builder,
    int32_t objectIndex, OH_AudioObjectPosition pos)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    if (pos.isCartesian) {
        return innerBuilder->UpdateObjPosCartesian(objectIndex, pos.pos.cartesian.x,
            pos.pos.cartesian.y, pos.pos.cartesian.z);
    } else {
        return innerBuilder->UpdateObjPosPolar(objectIndex, pos.pos.polar.azimuth,
            pos.pos.polar.elevation, pos.pos.polar.distance);
    }
}

OH_AVErrCode OH_AudioVividMetaBuilder_UpdateObjectGain(OH_AudioVividMetaBuilder *builder,
    int32_t objectIndex, float gain)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    return innerBuilder->UpdateObjGain(objectIndex, gain);
}

OH_AVErrCode OH_AudioVividMetaBuilder_GetMetaLen(const OH_AudioVividMetaBuilder *builder, bool withStaticMeta,
    int32_t *len)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    FALSE_RETURN_V_MSG_E(len != nullptr, AV_ERR_INVALID_VAL, "input len is nullptr!");
    const AudioVividMetaManager *innerBuilder = reinterpret_cast<const AudioVividMetaManager *>(builder);
    int32_t outLen = 0;
    int32_t ret = innerBuilder->GetLen(withStaticMeta, outLen);
    FALSE_RETURN_V_MSG_E(ret == 0, AV_ERR_INVALID_VAL, "get len error!");
    *len = outLen;
    return AV_ERR_OK;
}

OH_AVErrCode OH_AudioVividMetaBuilder_GetMeta(const OH_AudioVividMetaBuilder *builder, bool withStaticMeta,
    uint8_t *buffer, int32_t len)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");

    const AudioVividMetaManager *innerBuilder = reinterpret_cast<const AudioVividMetaManager *>(builder);
    return innerBuilder->GetMeta(withStaticMeta, buffer, len);
}


OH_AVErrCode OH_AudioVividMetaBuilder_Destroy(OH_AudioVividMetaBuilder *builder)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    delete innerBuilder;
    return AV_ERR_OK;
}

OH_AVErrCode OH_AudioVividMetaBuilder_CreateEmptyBuilder(OH_AudioVividMetaBuilder **builder)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    AudioVividMetaManager *innerBuilder = new(std::nothrow) AudioVividMetaManager();
    FALSE_RETURN_V_MSG_E(innerBuilder != nullptr, AV_ERR_UNKNOWN, "create inner builder error!");
    OH_AVErrCode ret = innerBuilder->InitEmpty();
    if (ret != AV_ERR_OK) {
        delete innerBuilder;
        return ret;
    }
    *builder = reinterpret_cast<OH_AudioVividMetaBuilder *>(innerBuilder);
    return AV_ERR_OK;
}

OH_AVErrCode OH_AudioVividMetaBuilder_UpdateBaseMeta(OH_AudioVividMetaBuilder *builder, const uint8_t *buffer,
    int32_t len)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    FALSE_RETURN_V_MSG_E(buffer != nullptr, AV_ERR_INVALID_VAL, "input buffer is nullptr!");
    FALSE_RETURN_V_MSG_E(len > 0, AV_ERR_INVALID_VAL, "input len is invalid!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    return innerBuilder->UpdateBaseMeta(buffer, len);
}

OH_AVErrCode OH_AudioVividMetaBuilder_AddObject(OH_AudioVividMetaBuilder *builder, int32_t *objectIndex)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    FALSE_RETURN_V_MSG_E(objectIndex != nullptr, AV_ERR_INVALID_VAL, "input objectIndex is nullptr!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    int32_t idx = 0;
    OH_AVErrCode ret = innerBuilder->AddObject(idx);
    if (ret == AV_ERR_OK) {
        *objectIndex = idx;
    }
    return ret;
}

OH_AVErrCode OH_AudioVividMetaBuilder_RemoveObject(OH_AudioVividMetaBuilder *builder, int32_t objectIndex)
{
    FALSE_RETURN_V_MSG_E(builder != nullptr, AV_ERR_INVALID_VAL, "input builder is nullptr!");
    AudioVividMetaManager *innerBuilder = reinterpret_cast<AudioVividMetaManager *>(builder);
    return innerBuilder->RemoveObject(objectIndex);
}
