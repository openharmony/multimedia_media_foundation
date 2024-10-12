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

#include "media_monitor_wrapper.h"
#include "system_ability.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN_FOUNDATION, "HiStreamer"};
}

namespace OHOS {
namespace Media {
namespace MediaMonitor {
static std::string WRAPPER_DL_PATH = "/sys/lib64/libmedia_monitor_wrapper.z.so";
const char *GET_BUNDLE_INFO_FROM_UID = "GetBundleInfoFromUid";

MediaMonitorWrapper::MediaMonitorWrapper()
{
    auto soHandler_ = ::dlopen(WRAPPER_DL_PATH.c_str(), RTLD_NOW);
    if (!soHandler_) {
        MEDIA_LOG_E("dlopen failed due to " PUBLIC_LOG_S, ::dlerror());
        return;
    }
    getBundleInfoFromUid_ =
        reinterpret_cast<GetBundleInfoFromUid *>(::dlsym(soHandler_, "GetBundleInfoFromUid"));
    if (!getBundleInfoFromUid_) {
        MEDIA_LOG_E("dlsym failed.check so has this function." PUBLIC_LOG_S, ::dlerror());
    }
}

MediaMonitorWrapper::~MediaMonitorWrapper()
{
    if (soHandler_) {
        ::dlclose(soHandler_);
        soHandler_ = nullptr;
    }

}

MediaMonitorErr MediaMonitorWrapper::GetBundleInfo(int32_t appUid, BundleInfo *bundleInfo)
{
    if (!getBundleInfoFromUid_) {
        MEDIA_LOG_E("getBundleInfoFromUid_ failed.");
        return MediaMonitorErr::ERR_OPERATION_FAILED;
    }
    return getBundleInfoFromUid_(appUid, bundleInfo);
}

} // namespace MediaMonitor
} // namespace Media
} // namespace OHOS
