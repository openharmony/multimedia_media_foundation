# 关键流程

> 播放/录制跨文件调用链，展示 StateMachine → Pipeline → Plugin 协作

## 播放核心流程

```
SetSource:  IPlayerEngine → HiPlayerImpl → StateMachine(SET_SOURCE) → Pipeline::AddHeadFilters
Prepare:    StateMachine(PREPARE) → Pipeline::Prepare → SourcePlugin::SetSource → TypeFinder 嗅探
            → PluginManager::CreateDemuxerPlugin → DemuxerPlugin::GetMediaInfo
            → 动态 LinkNext: Demuxer→Decoder→Sink
Play:       StateMachine(PLAY) → Pipeline::Start → 所有 Filter::Start → MediaSyncManager::Resume
Seek:       StateMachine(SEEK) → Pipeline::Flush → MediaSyncManager::Seek → DemuxerPlugin::SeekTo → Pipeline::Resume
Stop:       StateMachine(STOP) → Pipeline::Stop → MediaSyncManager::Pause
Reset:      StateMachine(RESET) → Pipeline::Release → 释放插件/缓冲/线程
```

## 录制核心流程

```
SetSource:  IRecorderEngine → HiRecorderImpl → StateMachine(SET_SOURCE) → Pipeline::AddHeadFilters(Capture)
Prepare:    StateMachine(PREPARE) → 构建 Pipeline: Capture→Encoder→Muxer→OutputSink → Pipeline::Prepare
GetSurface: VideoCaptureFilter::GetInputSurface → 返回 Surface 给摄像头
Start:      StateMachine(PLAY) → Pipeline::Start → MuxerPlugin::WriteHeader
Stop:       StateMachine(STOP) → Pipeline::Stop → MuxerPlugin::WriteTrailer → 刷新文件
```

## 数据流（播放）

```
SourcePlugin::Read → MediaSourceFilter →[AVBufferQueue]→ DemuxerFilter
  → DemuxerPlugin::ReadFrame →[AVBufferQueue]→ DecoderFilter
  → CodecPlugin::QueueInputBuffer → DataCallback::OnOutputBufferDone
  →[AVBufferQueue]→ SinkFilter → AudioSinkPlugin::Write / Surface 渲染
  → MediaSyncManager 同步控制
```

## 关键约束

- 所有操作必须经过 StateMachine 校验，非法操作请求返回错误
- 生命周期操作通过 Pipeline 统一下发，不可直接操作 Filter
- Seek 时必须 Flush 全链路后重新填充缓冲

## 知识关联

- [[scene-and-state-machine]] — 状态机设计
- [[pipeline-architecture]] — Pipeline 架构
