/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef VPE_LOG_H
#define VPE_LOG_H

#include <cinttypes>
#include <string>

#include "hilog/log.h"

#undef LOG_DOMAIN_FOUNDATION
#define LOG_DOMAIN_FOUNDATION    0xD002B3F

#undef LOG_TAG
#define LOG_TAG "VPE_CAPI"

#define VPE_LOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define VPE_LOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define VPE_LOGW(fmt, ...) HILOG_WARN(LOG_CORE, fmt, ##__VA_ARGS__)
#define VPE_LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
#define VPE_LOGF(fmt, ...) HILOG_FATAL(LOG_CORE, fmt, ##__VA_ARGS__)

#endif  // VPE_LOG_H