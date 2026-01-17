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
#ifndef DUMP_BUFFER_DEFINE_H
#define DUMP_BUFFER_DEFINE_H

#include "avbuffer_queue.h"
#include "parcel.h"
#include "message_parcel.h"

struct DumpBuffer : public OHOS::Parcelable {
    DumpBuffer();
    explicit DumpBuffer(const std::shared_ptr<OHOS::Media::AVBuffer> &buffer);
    virtual ~DumpBuffer();
    std::shared_ptr<OHOS::Media::AVBuffer> buffer_;

    bool Marshalling(OHOS::Parcel &parcel) const;
    static DumpBuffer *Unmarshalling(OHOS::Parcel &data);
};

#endif // DUMP_BUFFER_DEFINE_H