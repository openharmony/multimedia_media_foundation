/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "common/log.h"
#include "meta/any.h"
#include <map>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "Any" };
}

namespace {
using namespace OHOS::Media;
using BaseTypesMap = std::map<std::string, AnyValueType>;

const BaseTypesMap &GetBaseTypesMap()
{
    static const BaseTypesMap baseTypeMap([]() {
        BaseTypesMap typeMap;
        Any defaultBool = (bool)true;
        typeMap[std::string(defaultBool.TypeName())] = AnyValueType::BOOL;
        Any defaultInt32 = (int32_t)0;
        typeMap[std::string(defaultInt32.TypeName())] = AnyValueType::INT32_T;
        Any defaultInt64 = (int64_t)0;
        typeMap[std::string(defaultInt64.TypeName())] = AnyValueType::INT64_T;
        Any defaultFoalt = (float)0.0;
        typeMap[std::string(defaultFoalt.TypeName())] = AnyValueType::FLOAT;
        Any defaultDouble = (double)0.0;
        typeMap[std::string(defaultDouble.TypeName())] = AnyValueType::DOUBLE;
        Any defaultString = std::string();
        typeMap[std::string(defaultString.TypeName())] = AnyValueType::STRING;
        Any defaultVecUint8 = std::vector<uint8_t>();
        typeMap[std::string(defaultVecUint8.TypeName())] = AnyValueType::VECTOR_UINT8;
        Any defaultVecInt32 = std::vector<int32_t>();
        typeMap[std::string(defaultVecInt32.TypeName())] = AnyValueType::VECTOR_INT32;
        return typeMap;
    }());
    return baseTypeMap;
}
} // namespace

namespace OHOS {
namespace Media {
bool Any::BaseTypesToParcel(const Any *operand, MessageParcel &parcel) noexcept
{
    auto iter = GetBaseTypesMap().find(std::string(operand->TypeName()));
    if (iter == GetBaseTypesMap().end()) {
        parcel.WriteInt32(static_cast<int32_t>(AnyValueType::INVALID_TYPE));
        return false;
    }
    bool ret = parcel.WriteInt32(static_cast<int32_t>(iter->second));
    switch (iter->second) {
        case AnyValueType::BOOL:
            ret = ret && parcel.WriteBool(*AnyCast<bool>(operand));
            break;
        case AnyValueType::INT32_T:
            ret = ret && parcel.WriteInt32(*AnyCast<int32_t>(operand));
            break;
        case AnyValueType::INT64_T:
            ret = ret && parcel.WriteInt64(*AnyCast<int64_t>(operand));
            break;
        case AnyValueType::FLOAT:
            ret = ret && parcel.WriteFloat(*AnyCast<float>(operand));
            break;
        case AnyValueType::DOUBLE:
            ret = ret && parcel.WriteDouble(*AnyCast<double>(operand));
            break;
        case AnyValueType::STRING:
            ret = ret && parcel.WriteString(*AnyCast<std::string>(operand));
            break;
        case AnyValueType::VECTOR_UINT8:
            ret = ret && parcel.WriteUInt8Vector(*AnyCast<std::vector<uint8_t>>(operand));
            break;
        case AnyValueType::VECTOR_INT32:
            ret = ret && parcel.WriteInt32Vector(*AnyCast<std::vector<int32_t>>(operand));
            break;
        default: {
            parcel.WriteInt32(static_cast<int32_t>(AnyValueType::INVALID_TYPE));
            return false;
        }
    }
    return ret;
}

enum class StatusCodeFromParcel {
    SUCCESS = 0,
    ENUM_RETRY = 1,
    NO_RETRY = 2,
};

static void BaseTypesVectorUint8(Any *operand, MessageParcel &parcel)
{
    std::vector<uint8_t> val;
    (void)parcel.ReadUInt8Vector(&val);
    Any tmp(val);
    operand->Swap(tmp);
}

static void BaseTypesVectorInt32(Any *operand, MessageParcel &parcel)
{
    std::vector<int32_t> val;
    (void)parcel.ReadInt32Vector(&val);
    Any tmp(val);
    operand->Swap(tmp);
}

// returnValue : 0 -- success; 1 -- retry for enum type; 2 -- failed no retry
int Any::BaseTypesFromParcel(Any *operand, MessageParcel &parcel) noexcept
{
    AnyValueType type = static_cast<AnyValueType>(parcel.ReadInt32());
    switch (type) {
        case AnyValueType::BOOL: {
            Any tmp(parcel.ReadBool());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::INT32_T: {
            Any tmp(parcel.ReadInt32());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::INT64_T: {
            Any tmp(parcel.ReadInt64());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::FLOAT: {
            Any tmp(parcel.ReadFloat());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::DOUBLE: {
            Any tmp(parcel.ReadDouble());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::STRING: {
            Any tmp(parcel.ReadString());
            operand->Swap(tmp);
            break;
        }
        case AnyValueType::VECTOR_UINT8: {
            BaseTypesVectorUint8(operand, parcel);
            break;
        }
        case AnyValueType::VECTOR_INT32: {
            BaseTypesVectorInt32(operand, parcel);
            break;
        }
        case AnyValueType::INVALID_TYPE:
            return static_cast<int>(StatusCodeFromParcel::ENUM_RETRY);
        default:
            return static_cast<int>(StatusCodeFromParcel::NO_RETRY);
    }
    return static_cast<int>(StatusCodeFromParcel::SUCCESS);
}

/**
 * Get TypeName From function info.
 * Extract the Type name out of Function Info
 * @param functionInfo Function Info
 * @return Name of Type T ,Such as <b>bool int float double std::vector<unsigned char></b> etc.
 * @example In windows with MEDIA_NO_OHOS define,
 * FunctionInfo will be like <br>
 * static constexpr std::string_view OHOS::Media::Any::GetTypeName()
 * [with T = <b>bool</b>; std::string_view = std::basic_string_view<char>] <br>
 * with MEDIA_OHOS define, FunctionInfo will be like <br>
 * static std::string_view OHOS::Media::Any::GetTypeName() [T = <b>std::vector<unsigned char></b>]  <br>
 * For EnumType , FunctionInfo will be like <br>
 * static std::string_view OHOS::Media::Any::GetTypeName() [T = <b>OHOS::Media::Plugins::VideoEncodeBitrateMode</b>]
 */
std::string_view Any::GetTypeNameFromFunctionInfo(const char* functionInfo) noexcept
{
    std::string_view stringInfo = functionInfo;
    std::string_view retType = "Unknown";
    size_t beginIndex = stringInfo.find_first_of('=');
    if (beginIndex == std::string::npos) {
        MEDIA_LOG_E("GetTypeNameFromFunctionInfo failed. Function: " PUBLIC_LOG_S, stringInfo.data());
        return retType;
    } else {
        beginIndex += 2; // 2 表示右移两位
    }
#ifdef MEDIA_OHOS
    size_t endIndex = stringInfo.find_last_of(']');
#else
    size_t endIndex = stringInfo.find_last_of(';');
#endif
    FALSE_RETURN_V_MSG_E(endIndex != std::string::npos,
        retType, "GetTypeNameFromFunctionInfo find Type failed. Function: " PUBLIC_LOG_S, stringInfo.data());
    std::string_view typeNameRet(functionInfo + beginIndex, endIndex - beginIndex);
    return typeNameRet;
}
} // namespace Media
} // namespace OHOS