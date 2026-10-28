# 业务背景与系统定位

> media_foundation（HiStreamer）在 OpenHarmony 多媒体子系统中的定位与核心约束。

## 系统定位

media_foundation 是 OpenHarmony 多媒体子系统的**媒体引擎基础层**，位于上层框架与底层硬件能力之间：

```
应用层 (ArkTS / C / C++ 应用)
    ↓
API 层 (AVPlayer / AVRecorder / AVScreenCapture / AVTranscoder)
    ↓
player_framework (standard) / media_lite (mini/small)  ← 上层框架
    ↓
media_foundation (HiStreamer)  ← 本组件
    ↓
原子能力层 (MediaDemuxer / MediaCodec / VideoDecoder / AudioRenderer)
    ↓
插件层 (ffmpeg Adapter / HDI Adapter / codec_adapter / Source / Sink)
    ↓
硬件驱动 (HDI / codec_server / Audio HDI / Surface)
```

## 进程模型

| 进程 | 包含模块 | 说明 |
|------|---------|------|
| 媒体服务进程 | HiPlayerImpl / HiRecorderImpl（被 player_framework 引擎调用） | 引擎在 MediaServer 进程内运行 |
| 编解码服务进程 | codec_adapter → codec_server | 硬件编解码通过 HDI 调用独立进程 |
| media_monitor 进程 | MediaMonitorService (SA 3013) | 独立监控服务进程 |

**关键约束**：HiStreamer 引擎本身不管理进程间通信（IPC 由上层 player_framework 负责），但在 AVBufferQueue 的 Producer/Consumer 跨进程场景下通过 Proxy/Stub 实现 IPC。

## 关键技术约束

- **编译选项**：-fno-rtti -fexceptions（禁用 RTTI，启用异常；定义 HST_ANY_WITH_NO_RTTI）
- **构建系统**：GN + Ninja，config.gni 按平台（mini/small/standard）自动适配
- **插件加载**：dlopen/dlsym 动态加载 .z.so，从 `/system/lib64/media/media_plugins/` 目录
- **许可证**：ffmpeg 受 LGPL 许可证约束，修改 ffmpeg Adapter 需评估许可证影响

## API 层次

```
┌─────────────────────────────────────────┐
│ C API (Native API)                        │  ← Native 开发者使用
│  OH_AVBuffer / OH_AVFormat / OH_AVMemory │
├─────────────────────────────────────────┤
│  Inner API                              │  ← 系统内部使用（player_framework 等）
│  AVBuffer / AVBufferQueue / Filter /     │
│  Pipeline / PluginManager / Meta / Task  │
├─────────────────────────────────────────┤
│  Histreamer Engine API                  │  ← 引擎接口
│  CreateHiPlayer() / HstEngineFactory     │
└─────────────────────────────────────────┘
```

## 知识关联

- [[architecture]] - 架构总览
- [[glossary]] - 术语速查
