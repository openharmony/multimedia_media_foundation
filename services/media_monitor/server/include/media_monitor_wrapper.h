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
#ifndef MEDIA_MONITOR_WRAPPER_H
#define MEDIA_MONITOR_WRAPPER_H

#include <dlfcn.h>
#include "monitor_error.h"
#include "event_bean.h"
#include "log.h"

namespace OHOS {
namespace Media {
namespace MediaMonitor {
struct BundleInfo {
    std::string appName;
    int32_t versionCode;
};

typedef MediaMonitorErr GetBundleInfoFromUid(int32_t appUid, BundleInfo *bundleInfoRes);

class MediaMonitorWrapper {
public:
    MediaMonitorWrapper();
    ~MediaMonitorWrapper();
    MediaMonitorErr GetBundleInfo(int32_t appUid, BundleInfo *bundleInfo);
private:
    void* soHandler_;
    GetBundleInfoFromUid *getBundleInfoFromUid_ = nullptr;
};

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
#endif // MEDIA_MONITOR_WRAPPER_H
