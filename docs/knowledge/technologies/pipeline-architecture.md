# Pipeline 架构

> v1/v2 双版本 Pipeline 设计决策与关键约束

## v1/v2 双版本架构

HiStreamer 存在两套并行 Pipeline 架构，代码完全独立，不互相依赖：

| 维度 | v1 (Legacy) | v2 (inner_api) |
|------|-------------|----------------|
| 路径 | `engine/pipeline/core/` + `engine/include/pipeline/core/` | `interface/inner_api/pipeline/` + `src/pipeline/` + `src/filter/` |
| 数据模型 | Port(Push/Pull) 模型 | LinkNext + AVBufferQueue 模型 |
| Filter 基类 | FilterBase（继承 Filter + Plugin::CallbackWrap） | Filter（Template Method：public final → virtual Do*） |
| 能力协商 | Port::Negotiate() | OnLinked() 回调 |
| 插件管理 | PluginManager（Sniff 打分） | PluginManagerV2（PluginList 描述匹配 + PluginPackage 动态加载） |
| 使用场景 | media_lite（mini/small 设备） | player_framework（standard 设备） |

## Pipeline 统一控制

所有 Filter 生命周期操作通过 Pipeline 统一下发，Filter 间通过 AVBufferQueue 传递数据：

```
播放: MediaSource → Demuxer → AudioDecoder → AudioSink
                                → VideoDecoder → VideoSink
录制: AudioCapture → AudioEncoder → Muxer → OutputSink
      VideoCapture → VideoEncoder ↗
```

## 关键约束

- **禁止** v1 Port 模型与 v2 LinkNext 模型混用
- **禁止**绕过 FilterFactory 直接 new Filter（使用 AutoRegisterFilter 静态注册）
- **禁止**直接操作 Pipeline 内特定 Filter 的生命周期方法（通过 Pipeline 统一下发）
- **禁止** Filter Do* 方法阻塞（耗时操作投递 Task 异步执行）
- **禁止** v2 代码依赖 engine/ 内部头文件；v1 代码依赖 interface/inner_api 头文件

## 知识关联

- [[evolution]] — v1→v2 架构演进
- [[plugin-architecture]] — 插件创建机制
- [[buffer-and-memory]] — AVBufferQueue 数据传递
