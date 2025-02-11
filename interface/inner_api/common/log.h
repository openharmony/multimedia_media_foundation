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

#ifndef HISTREAMER_FOUNDATION_LOG_H
#define HISTREAMER_FOUNDATION_LOG_H

#include <cinttypes>
#include <string>

#ifdef MEDIA_OHOS
#include "hilog/log.h"
#else
#include "log_adapter.h"
#endif

// If file name and line number is need, #define HST_DEBUG at the beginning of the cpp file.
#define HST_DEBUG

#ifdef MEDIA_OHOS
#undef LOG_DOMAIN_SYSTEM_PLAYER
#define LOG_DOMAIN_SYSTEM_PLAYER 0xD002B22
#undef LOG_DOMAIN_STREAM_SOURCE
#define LOG_DOMAIN_STREAM_SOURCE 0xD002B23
#undef LOG_DOMAIN_FOUNDATION
#define LOG_DOMAIN_FOUNDATION    0xD002B24
#undef LOG_DOMAIN_DEMUXER
#define LOG_DOMAIN_DEMUXER       0xD002B3A
#undef LOG_DOMAIN_MUXER
#define LOG_DOMAIN_MUXER         0xD002B3B
#undef LOG_DOMAIN_AUDIO
#define LOG_DOMAIN_AUDIO         0xD002B31
#undef LOG_DOMAIN_PLAYER
#define LOG_DOMAIN_PLAYER        0xD002B2B
#undef LOG_DOMAIN_RECORDER
#define LOG_DOMAIN_RECORDER      0xD002B2C
#undef LOG_DOMAIN_SCREENCAPTURE
#define LOG_DOMAIN_SCREENCAPTURE 0xD002B2E
#undef LOG_DOMAIN_HIPLAYER
#define LOG_DOMAIN_HIPLAYER      0xD002B2D
#undef LOG_DOMAIN_METADATA
#define LOG_DOMAIN_METADATA      0xD002B2C
#define PUBLIC_LOG "%{public}"
#else
#define PUBLIC_LOG "%"
#endif

#ifndef HST_LOG_TAG
#define HST_LOG_TAG "NULL"
#endif

#define PUBLIC_LOG_C PUBLIC_LOG "c"
#define PUBLIC_LOG_S PUBLIC_LOG "s"
#define PUBLIC_LOG_D8 PUBLIC_LOG PRId8
#define PUBLIC_LOG_D16 PUBLIC_LOG PRId16
#define PUBLIC_LOG_D32 PUBLIC_LOG PRId32
#define PUBLIC_LOG_D64 PUBLIC_LOG PRId64
#define PUBLIC_LOG_U8 PUBLIC_LOG PRIu8
#define PUBLIC_LOG_U16 PUBLIC_LOG PRIu16
#define PUBLIC_LOG_U32 PUBLIC_LOG PRIu32
#define PUBLIC_LOG_U64 PUBLIC_LOG PRIu64
#define PUBLIC_LOG_F PUBLIC_LOG "f"
#define PUBLIC_LOG_P PUBLIC_LOG "p"
#define PUBLIC_LOG_ZU PUBLIC_LOG "zu"

#undef LOG_TAG
#define LOG_TAG LABEL.tag
#undef LOG_DOMAIN
#define LOG_DOMAIN LABEL.domain
#undef LOG_TYPE
#define LOG_TYPE LABEL.type


#ifdef MEDIA_OHOS
#ifndef HST_DEBUG
#define HST_HILOG(op, fmt, args...)                              \
    do {                                                         \
        op(LOG_TYPE, PUBLIC_LOG_S ":" fmt, HST_LOG_TAG, ##args); \
    } while (0)
#else
#define HST_HILOG(op, fmt, args...)                                                                     \
    do {                                                                                                \
        op(LOG_TYPE, "(" PUBLIC_LOG_S "(), " PUBLIC_LOG_D32 "): " fmt, __FUNCTION__, __LINE__, ##args); \
    } while (0)
#define HST_HILOG_SHORT(op, fmt, args...)                           \
    do {                                                            \
        op(LOG_TYPE, "#" PUBLIC_LOG_D32 " " fmt, __LINE__, ##args); \
    } while (0)
#define HST_HILOG_NO_RELEASE(op, fmt, args...)                                                                     \
    do {                                                                                                           \
        op(LOG_ONLY_PRERELEASE, "(" PUBLIC_LOG_S "(), " PUBLIC_LOG_D32 "): " fmt, __FUNCTION__, __LINE__, ##args); \
    } while (0)

#define HST_HILOG_TAG(op, fmt, args...)                               \
    do {                                                              \
        op(LOG_TYPE, "[" PUBLIC_LOG_S "]:" fmt, HST_LOG_TAG, ##args); \
    } while (0)

#define HST_HILOG_WITH_LEVEL_JUDGE(op1, op2, con, fmt, args...)                                              \
    do {                                                                                                     \
        if (!con) {                                                                                          \
            op2(LOG_TYPE, "(" PUBLIC_LOG_S "(), " PUBLIC_LOG_D32 "): " fmt, __FUNCTION__, __LINE__, ##args); \
        } else {                                                                                             \
            op1(LOG_TYPE, "(" PUBLIC_LOG_S "(), " PUBLIC_LOG_D32 "): " fmt, __FUNCTION__, __LINE__, ##args); \
        }                                                                                                    \
    } while (0)
#endif

#define MEDIA_LOG_D(fmt, ...) HST_HILOG(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I(fmt, ...) HST_HILOG(HILOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_W(fmt, ...) HST_HILOG(HILOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_E(fmt, ...) HST_HILOG(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_F(fmt, ...) HST_HILOG(HILOG_FATAL, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I_NO_RELEASE(fmt, ...) HST_HILOG_NO_RELEASE(HILOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_W_NO_RELEASE(fmt, ...) HST_HILOG_NO_RELEASE(HILOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_E_NO_RELEASE(fmt, ...) HST_HILOG_NO_RELEASE(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_F_NO_RELEASE(fmt, ...) HST_HILOG_NO_RELEASE(HILOG_FATAL, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_D_SHORT(fmt, ...) HST_HILOG_SHORT(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I_SHORT(fmt, ...) HST_HILOG_SHORT(HILOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_W_SHORT(fmt, ...) HST_HILOG_SHORT(HILOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_E_SHORT(fmt, ...) HST_HILOG_SHORT(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_F_SHORT(fmt, ...) HST_HILOG_SHORT(HILOG_FATAL, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I_FALSE_D(con, fmt, ...) \
    HST_HILOG_WITH_LEVEL_JUDGE(HILOG_INFO, HILOG_DEBUG, con, fmt, ##__VA_ARGS__)

#define HST_HILOG_T_WITH_LEVEL_JUDGE(op1, op2, con, fmt, args...)           \
    do {                                                                    \
        if (!con) {                                                         \
            op2(LOG_TYPE, "[" PUBLIC_LOG_S "]:" fmt, HST_LOG_TAG, ##args);  \
        } else {                                                            \
            op1(LOG_TYPE, "[" PUBLIC_LOG_S "]:" fmt, HST_LOG_TAG, ##args);  \
        }                                                                   \
    } while (0)

#define MEDIA_LOG_D_T(fmt, ...) HST_HILOG_TAG(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I_T(fmt, ...) HST_HILOG_TAG(HILOG_INFO, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_W_T(fmt, ...) HST_HILOG_TAG(HILOG_WARN, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_E_T(fmt, ...) HST_HILOG_TAG(HILOG_ERROR, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_F_T(fmt, ...) HST_HILOG_TAG(HILOG_FATAL, fmt, ##__VA_ARGS__)
#define MEDIA_LOG_I_FALSE_D_T(con, fmt, ...)                                      \
    HST_HILOG_T_WITH_LEVEL_JUDGE(HILOG_INFO, HILOG_DEBUG, con, fmt, ##__VA_ARGS__)

#define MEDIA_LOG_LIMIT(op, frequency, fmt, ...)             \
    do {                                                     \
        static uint64_t currentTimes = 0;                    \
        if (currentTimes++ % ((uint32_t)(frequency)) == 0) { \
            op(fmt, ##__VA_ARGS__);                          \
        }                                                    \
    } while (0)

#define MEDIA_LOGE_LIMIT(frequency, fmt, ...) MEDIA_LOG_LIMIT(MEDIA_LOG_E, frequency, fmt, ##__VA_ARGS__)
#define MEDIA_LOGW_LIMIT(frequency, fmt, ...) MEDIA_LOG_LIMIT(MEDIA_LOG_W, frequency, fmt, ##__VA_ARGS__)
#define MEDIA_LOGI_LIMIT(frequency, fmt, ...) MEDIA_LOG_LIMIT(MEDIA_LOG_I, frequency, fmt, ##__VA_ARGS__)
#define MEDIA_LOGD_LIMIT(frequency, fmt, ...) MEDIA_LOG_LIMIT(MEDIA_LOG_D, frequency, fmt, ##__VA_ARGS__)
#endif

// Control the MEDIA_LOG_D.
// If MEDIA_LOG_D is needed, #define MEDIA_LOG_DEBUG 1 at the beginning of the cpp file.
#ifndef MEDIA_LOG_DEBUG
#define MEDIA_LOG_DEBUG 1
#endif

#if !MEDIA_LOG_DEBUG
#undef MEDIA_LOG_D
#define MEDIA_LOG_D(msg, ...) ((void)0)
#endif

// Control the debug detail logs MEDIA_LOG_DD.
// If MEDIA_LOG_DD is needed, #define MEDIA_LOG_DEBUG_DETAIL 1 at the beginning of the cpp file.
#ifndef MEDIA_LOG_DEBUG_DETAIL
#define MEDIA_LOG_DEBUG_DETAIL 0
#endif

#if !MEDIA_LOG_DEBUG_DETAIL
#undef MEDIA_LOG_DD
#define MEDIA_LOG_DD(msg, ...) ((void)0)
#else
#undef MEDIA_LOG_DD
#define MEDIA_LOG_DD MEDIA_LOG_D
#endif

#ifndef NOK_RETURN
#define NOK_RETURN(exec)                                                           \
    do {                                                                           \
        Status returnValue = (exec);                                               \
        if (returnValue != Status::OK) {                                           \
            MEDIA_LOG_E("NOK_RETURN on Status(" PUBLIC_LOG_D32 ").", returnValue); \
            return returnValue;                                                    \
        }                                                                          \
    } while (0)
#endif

#ifndef NOK_LOG
#define NOK_LOG(exec)                                                           \
    do {                                                                        \
        Status returnValue = (exec);                                            \
        if (returnValue != Status::OK) {                                        \
            MEDIA_LOG_E("NOK_LOG on Status(" PUBLIC_LOG_D32 ").", returnValue); \
        }                                                                       \
    } while (0)
#endif

// If exec not return zero, then record the error code, especially when call system C function.
#ifndef NZERO_LOG
#define NZERO_LOG(exec)                                                                          \
    do {                                                                                         \
        int returnValue = (exec);                                                                \
        if (returnValue != 0) {                                                                  \
            MEDIA_LOG_E("NZERO_LOG when call (" #exec "), return " PUBLIC_LOG_D32, returnValue); \
        }                                                                                        \
    } while (0)
#endif

#ifndef NZERO_RETURN
#define NZERO_RETURN(exec)                                                                          \
    do {                                                                                            \
        int returnValue = (exec);                                                                   \
        if (returnValue != 0) {                                                                     \
            MEDIA_LOG_E("NZERO_RETURN when call (" #exec "), return " PUBLIC_LOG_D32, returnValue); \
            return returnValue;                                                                     \
        }                                                                                           \
    } while (0)
#endif

#ifndef NZERO_RETURN_V
#define NZERO_RETURN_V(exec, ret)                                                                     \
    do {                                                                                              \
        int returnValue = (exec);                                                                     \
        if (returnValue != 0) {                                                                       \
            MEDIA_LOG_E("NZERO_RETURN_V when call (" #exec "), return " PUBLIC_LOG_D32, returnValue); \
            return ret;                                                                               \
        }                                                                                             \
    } while (0)
#endif

#ifndef FALSE_RETURN
#define FALSE_RETURN(exec)                                 \
    do {                                                   \
        bool returnValue = (exec);                         \
        if (!returnValue) {                                \
            MEDIA_LOG_E_NO_RELEASE("FALSE_RETURN " #exec); \
            return;                                        \
        }                                                  \
    } while (0)
#endif

#ifndef FALSE_RETURN_NOLOG
#define FALSE_RETURN_NOLOG(exec)                                 \
    do {                                                   \
        bool returnValue = (exec);                         \
        if (!returnValue) {                                \
            return;                                        \
        }                                                  \
    } while (0)
#endif

#ifndef FALSE_CONTINUE_NOLOG
#define FALSE_CONTINUE_NOLOG(exec)                                     \
    if (1) {                                                           \
        bool returnValue = (exec);                                     \
        if (!returnValue) {                                            \
            continue;                                                  \
        }                                                              \
    } else                                                             \
        void(0)
#endif

#ifndef FALSE_RETURN_W
#define FALSE_RETURN_W(exec)                    \
    do {                                        \
        bool returnValue = (exec);              \
        if (!returnValue) {                     \
            MEDIA_LOG_W("FALSE_RETURN " #exec); \
            return;                             \
        }                                       \
    } while (0)
#endif

#ifndef FALSE_RETURN_V
#define FALSE_RETURN_V(exec, ret)                            \
    do {                                                     \
        bool returnValue = (exec);                           \
        if (!returnValue) {                                  \
            MEDIA_LOG_E_NO_RELEASE("FALSE_RETURN_V " #exec); \
            return ret;                                      \
        }                                                    \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_NOLOG
#define FALSE_RETURN_V_NOLOG(exec, ret)                            \
    do {                                                     \
        bool returnValue = (exec);                           \
        if (!returnValue) {                                  \
            return ret;                                      \
        }                                                    \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_W
#define FALSE_RETURN_V_W(exec, ret)                 \
    do {                                            \
        bool returnValue = (exec);                  \
        if (!returnValue) {                         \
            MEDIA_LOG_W("FALSE_RETURN_V_W " #exec); \
            return ret;                             \
        }                                           \
    } while (0)
#endif

#ifndef FALSE_RETURN_MSG
#define FALSE_RETURN_MSG(exec, fmt, args...) \
    do {                                     \
        bool returnValue = (exec);           \
        if (!returnValue) {                  \
            MEDIA_LOG_E(fmt, ##args);        \
            return;                          \
        }                                    \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_MSG_IMPL
#define FALSE_RETURN_V_MSG_IMPL(loglevel, exec, ret, fmt, args...) \
    do {                                                           \
        bool returnValue = (exec);                                 \
        if (!returnValue) {                                        \
            loglevel(fmt, ##args);                                 \
            return ret;                                            \
        }                                                          \
    } while (0)
#endif

#ifndef FALSE_RETURN_V_MSG
#define FALSE_RETURN_V_MSG(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_E, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_RETURN_V_MSG_D
#define FALSE_RETURN_V_MSG_D(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_D, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_RETURN_V_MSG_W
#define FALSE_RETURN_V_MSG_W(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_W, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_RETURN_V_MSG_E
#define FALSE_RETURN_V_MSG_E(exec, ret, fmt, args...) FALSE_RETURN_V_MSG_IMPL(MEDIA_LOG_E, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_UPDATE_RETURN_V_MSG_IMPL
#define FALSE_UPDATE_RETURN_V_MSG_IMPL(loglevel, val, exec, ret, fmt, args...) \
    do {                                                                       \
        bool returnValue = (exec);                                             \
        if (!returnValue) {                                                    \
            val = ret;                                                         \
            loglevel(fmt, ##args);                                             \
            return ret;                                                        \
        }                                                                      \
    } while (0)
#endif

#ifndef FALSE_UPDATE_RETURN_V_MSG_E
#define FALSE_UPDATE_RETURN_V_MSG_E(exec, val, ret, fmt, args...) \
    FALSE_UPDATE_RETURN_V_MSG_IMPL(MEDIA_LOG_E, val, exec, ret, fmt, ##args)
#endif

#ifndef FALSE_LOG
#define FALSE_LOG(exec)                       \
    do {                                      \
        bool returnValue = (exec);            \
        if (!returnValue) {                   \
            MEDIA_LOG_E("FALSE_LOG: " #exec); \
        }                                     \
    } while (0)
#endif

#ifndef FALSE_LOG_MSG_IMPL
#define FALSE_LOG_MSG_IMPL(loglevel, exec, fmt, args...) \
    do {                                                 \
        bool returnValue = (exec);                       \
        if (!returnValue) {                              \
            loglevel(fmt, ##args);                       \
        }                                                \
    } while (0)
#endif

#ifndef FALSE_LOG_MSG
#define FALSE_LOG_MSG(exec, fmt, args...) FALSE_LOG_MSG_IMPL(MEDIA_LOG_E, exec, fmt, ##args)
#endif

#ifndef FALSE_LOG_MSG_W
#define FALSE_LOG_MSG_W(exec, fmt, args...) FALSE_LOG_MSG_IMPL(MEDIA_LOG_W, exec, fmt, ##args)
#endif

#define POINTER_MASK 0x00FFFFFF
#define FAKE_POINTER(addr) (POINTER_MASK & reinterpret_cast<uintptr_t>(addr))
#endif  // HISTREAMER_FOUNDATION_LOG_H