/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_MEDIA_SOURCE_H
#define HISTREAMER_PLUGIN_MEDIA_SOURCE_H

#include <map>
#include <memory>
#ifndef OHOS_LITE
#include "common/media_data_source.h"
#endif
#include "meta/media_types.h"

namespace OHOS {
namespace Media {
namespace Plugins {
/// End of Stream Buffer Flag
#define BUFFER_FLAG_EOS 0x00000001
/// Video Key Frame Flag
#define BUFFER_FLAG_KEY_FRAME 0x00000002
/**
 * @brief Unified enumerates media source types.
 *
 * @since 1.0
 * @version 1.0
 */
enum class SourceType : int32_t {
    /** Local file path or network address */
    SOURCE_TYPE_URI = 0,
    /** Local file descriptor */
    SOURCE_TYPE_FD,
    /** Stream data */
    SOURCE_TYPE_STREAM,
};

typedef struct PlayStrategy {
    uint32_t width {0};
    uint32_t height {0};
    uint32_t duration {0};
    bool preferHDR {false};
    std::string audioLanguage {""};
    std::string subtitleLanguage {""};
    double bufferDurationForPlaying {0};
} PlayStrategy;

struct PlayMediaStream {
    std::string url {""};
    uint32_t width {0};
    uint32_t height {0};
    uint32_t bitrate {0};
};

class AVMimeTypes {
public:
    static constexpr std::string_view APPLICATION_M3U8 = "application/m3u8";
};

typedef struct DownloadInfo {
    int32_t avgDownloadRate {0};
    int32_t avgDownloadSpeed {0};
    uint64_t totalDownLoadBits {0};
    bool isTimeOut {false};
} DownloadInfo;

typedef struct PlaybackInfo {
    std::string serverIpAddress {};
    int64_t averageDownloadRate = 0;
    int64_t downloadRate = 0;
    bool isDownloading = false;
    int64_t bufferDuration = 0;
} PlaybackInfo;

/**
 * IMediaSourceLoader::OPEN & IMediaSourceLoadingRequest::RespondData
 */
enum MediaSourceError : int32_t {
    /**
     * The callback interface times out and needs to be retried.
     */
    MEDIA_SOURCE_ERROR_RETRY = 0,
    /**
     * Used for internal exceptions
     */
    MEDIA_SOURCE_ERROR_IO = -1,
};

// preDonwload start
enum class LoadingRequestError : int32_t {
    /**
     * If reach the resource end, client should return.
     */
    LOADING_ERROR_SUCCESS = 0,
	
    /**
     * If resource not reay for access, client should return.
     */
    LOADING_ERROR_NOT_READY = 1,
	
    /**
     * If resource url not exit, client should return.
     */
    LOADING_ERROR_NO_RESOURCE  = 2,
	
    /**
     * If the uuid of resource handle invalid, client should return.
     */
    LOADING_ERROR_INVAID_HANDLE = 3,
	
    /**
     * If client has no right to request the resouce, client should return.
     */
    LOADING_ERROR_ACCESS_DENIED = 4,
	
    /**
     * If access time out, client should return by this error.
     */
    LOADING_ERROR_ACCESS_TIMEOUT = 5,
	
    /**
     * If authorization failed, client should return this error.
     */
    LOADING_ERROR_AUTHORIZE_FAILED = 6,
};

class IMediaSourceLoadingRequest {
public:
    virtual ~IMediaSourceLoadingRequest() = default;

    virtual int32_t RespondData(int64_t uuid, int64_t offset, const std::shared_ptr<AVSharedMemory> &mem) = 0;

    virtual int32_t RespondHeader(int64_t uuid, std::map<std::string, std::string> header, std::string redirectUrl) = 0;

    virtual int32_t FinishLoading(int64_t uuid, LoadingRequestError requestedError) = 0;

    virtual void Release() {};

private:
    std::string url_ {};
    std::map<std::string, std::string> header_ {};
};

class IMediaSourceLoader {
public:
    virtual ~IMediaSourceLoader() = default;

    virtual int32_t Init(std::shared_ptr<IMediaSourceLoadingRequest> &request) = 0;
    
    virtual int64_t Open(const std::string &url, const std::map<std::string, std::string> &header) = 0;

    virtual int32_t Read(int64_t uuid, int64_t requestedOffset, int64_t requestedLength) = 0;

    virtual int32_t Close(int64_t uuid) = 0;
};

using MediaStreamList = std::vector<std::shared_ptr<PlayMediaStream>>;
class MediaSource {
public:
    /// Construct an a specified URI.
    explicit MediaSource(std::string uri);

#ifndef OHOS_LITE
    explicit MediaSource(std::shared_ptr<IMediaDataSource> dataSrc);
#endif

    MediaSource(std::string uri, std::map<std::string, std::string> header);

    /// Destructor
    virtual ~MediaSource() = default;

    /// Obtains the source type.
    SourceType GetSourceType() const;

    /// Obtains the media source URI.
    const std::string &GetSourceUri() const;

    const std::map<std::string, std::string> &GetSourceHeader() const;

    void AddMediaStream(const std::shared_ptr<PlayMediaStream>& playMediaStream);
    MediaStreamList GetMediaStreamList() const;

    void SetPlayStrategy(const std::shared_ptr<PlayStrategy>& playStrategy);

    std::shared_ptr<PlayStrategy> GetPlayStrategy() const;

    void SetMimeType(const std::string& mimeType);

    std::string GetMimeType() const;

    void SetAppUid(int32_t appUid);

    int32_t GetAppUid();

    void SetSourceLoader(std::shared_ptr<IMediaSourceLoader> mediaSourceLoader);

    std::shared_ptr<IMediaSourceLoader> GetSourceLoader() const;

    //std::shared_ptr<DataConsumer> GetDataConsumer() const;
#ifndef OHOS_LITE
    std::shared_ptr<IMediaDataSource> GetDataSrc() const;
#endif
private:
    std::string uri_ {};
    std::string mimeType_ {};
    SourceType type_ {};
    std::map<std::string, std::string> header_ {};
    std::shared_ptr<PlayStrategy> playStrategy_ {};
    int32_t appUid_ {-1};
    //std::shared_ptr<DataConsumer> dataConsumer_ {};
#ifndef OHOS_LITE
    std::shared_ptr<IMediaDataSource> dataSrc_ {};
#endif
    std::shared_ptr<IMediaSourceLoader> sourceLoader_ {};
};
} // namespace Plugins
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_MEDIA_SOURCE_H