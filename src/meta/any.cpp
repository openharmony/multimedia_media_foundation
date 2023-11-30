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
#include "meta/any.h"

namespace OHOS {
namespace Media {
bool Any::BaseTypesToParcel(const Any *operand, MessageParcel &parcel) noexcept
{
    bool ret = true;
    const auto &type = operand->TypeName();
    ret &= parcel.WriteString(std::string(type));
    if (Any::IsSameTypeWith<int32_t>(*operand)) {
        ret &= parcel.WriteInt32(*AnyCast<int32_t>(operand));
    } else if (Any::IsSameTypeWith<int64_t>(*operand)) {
        ret &= parcel.WriteInt64(*AnyCast<int64_t>(operand));
    } else if (Any::IsSameTypeWith<float>(*operand)) {
        ret &= parcel.WriteFloat(*AnyCast<float>(operand));
    } else if (Any::IsSameTypeWith<double>(*operand)) {
        ret &= parcel.WriteDouble(*AnyCast<double>(operand));
    } else if (Any::IsSameTypeWith<std::string>(*operand)) {
        ret &= parcel.WriteString(*AnyCast<std::string>(operand));
    } else if (Any::IsSameTypeWith<std::vector<uint8_t>>(*operand)) {
        ret &= parcel.WriteUInt8Vector(*AnyCast<std::vector<uint8_t>>(operand));
    } else {
        return false;
    }
    return ret;
}
// returnValue : 0 -- success; 1 -- retry enum; 2 -- failed no retry
int Any::BaseTypesFromParcel(Any *operand, MessageParcel &parcel) noexcept
{
    std::string type = parcel.ReadString();
    if (MakeAny<int32_t>().SameTypeWith(std::string_view(type))) {
        Any tmp(parcel.ReadInt32());
        operand->Swap(tmp);
        return 0;
    } else if (MakeAny<int64_t>().SameTypeWith(std::string_view(type))) {
        Any tmp(parcel.ReadInt64());
        operand->Swap(tmp);
        return 0;
    } else if (MakeAny<float>().SameTypeWith(std::string_view(type))) {
        Any tmp(parcel.ReadFloat());
        operand->Swap(tmp);
        return 0;
    } else if (MakeAny<double>().SameTypeWith(std::string_view(type))) {
        Any tmp(parcel.ReadDouble());
        operand->Swap(tmp);
        return 0;
    } else if (MakeAny<std::string>().SameTypeWith(std::string_view(type))) {
        Any tmp(parcel.ReadString());
        operand->Swap(tmp);
        return 0;
    } else if (MakeAny<std::vector<uint8_t>>().SameTypeWith(std::string_view(type))) {
        std::vector<uint8_t> val;
        (void)parcel.ReadUInt8Vector(&val);
        Any tmp(val);
        operand->Swap(tmp);
        return 0;
    }
    return 1;
}
} // namespace Media
} // namespace OHOS