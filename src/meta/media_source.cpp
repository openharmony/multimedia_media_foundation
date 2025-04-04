/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include "common/media_source.h"
#include "common/log.h"
#include <type_traits>

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_DOMAIN_FOUNDATION, "MediaSource" };
}

namespace OHOS {
namespace Media {
namespace Plugins {
MediaSource::MediaSource(std::string uri)
    : uri_(std::move(uri)), type_(SourceType::SOURCE_TYPE_URI)
{
}

#ifndef OHOS_LITE
MediaSource::MediaSource(std::shared_ptr<IMediaDataSource> dataSrc)
    :type_(SourceType::SOURCE_TYPE_STREAM), dataSrc_(std::move(dataSrc))
{
    MEDIA_LOG_I("MediaSource ctor called dataSrc");
}
#endif

MediaSource::MediaSource(std::string uri, std::map<std::string, std::string> header)
    : uri_(std::move(uri)), header_(std::move(header))
{
    MEDIA_LOG_I("MediaSource ctor called header");
}

SourceType MediaSource::GetSourceType() const
{
    return type_;
}

const std::string &MediaSource::GetSourceUri() const
{
    return uri_;
}

const std::map<std::string, std::string> &MediaSource::GetSourceHeader() const
{
    return header_;
}

void MediaSource::SetPlayStrategy(const std::shared_ptr<PlayStrategy>& playStrategy)
{
    MEDIA_LOG_I("SetPlayStrategy width: %{public}d, height: %{public}d, duration: %{public}d, preferHDR: %{public}d",
        playStrategy->width, playStrategy->height, playStrategy->duration, playStrategy->preferHDR);
    playStrategy_ = playStrategy;
}

std::shared_ptr<PlayStrategy> MediaSource::GetPlayStrategy() const
{
    return playStrategy_;
}

void MediaSource::AddMediaStream(const std::shared_ptr<PlayMediaStream>& playMediaStream)
{
    playMediaStreamVec_.push_back(playMediaStream);
}
 
MediaStreamList MediaSource::GetMediaStreamList() const
{
    return playMediaStreamVec_;
}

void MediaSource::SetAppUid(int32_t appUid)
{
    appUid_ = appUid;
}

int32_t MediaSource::GetAppUid()
{
    return appUid_;
}

void MediaSource::SetMimeType(const std::string& mimeType)
{
    mimeType_ = mimeType;
}

std::string MediaSource::GetMimeType() const
{
    return mimeType_;
}

void MediaSource::SetSourceLoader(std::shared_ptr<IMediaSourceLoader> mediaSourceLoader)
{
    MEDIA_LOG_I("SetSourceLoader");
    sourceLoader_ = std::move(mediaSourceLoader);
}
 
std::shared_ptr<IMediaSourceLoader> MediaSource::GetSourceLoader() const
{
    MEDIA_LOG_I("GetSourceLoader");
    return sourceLoader_;
}
#ifndef OHOS_LITE
std::shared_ptr<IMediaDataSource> MediaSource::GetDataSrc() const
{
    return dataSrc_;
}
#endif
} // namespace Plugins
} // namespace Media
} // namespace OHOS