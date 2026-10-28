# 架构设计及约束

> **全局性**架构设计原则及约束说明

## 设计原则

| 原则 | 描述 | 理由 |
|------|------|------|
| 三层架构 | 场景封装层（Scene）→ Pipeline 框架层 → 插件层（Plugin） | 职责分离，每层可独立演进，新增格式只需注册插件 |
| Pipeline Filter 链 | 数据流通过 Filter 链式传递，每个 Filter 职责单一 | 解耦数据处理环节，新增/替换 Filter 不影响链路其它部分 |
| 插件化 Sniff 机制 | Source/Demuxer 插件通过 Sniff 嗅探打分自动选择 | 支持多种媒体格式，新格式只需注册插件无需修改框架 |
| 插件动态加载 | 通过 dlopen/dlsym 动态加载 .z.so 插件包 | 按需加载，减少内存占用；支持运行时扩展 |
| 无 RTTI 设计 | -fno-rtti 编译，使用 Any 替代 typeid/dynamic_cast | 减少 binary 体积，适配嵌入式/轻量设备 |
| 多平台适配 | config.gni 自动适配 mini/small/standard 三档设备 | 一套代码覆盖从 liteos_m 到标准 Linux 的全量设备 |
| 状态机驱动 | HiPlayer/HiRecorder 使用 StateMachine 管理生命周期 | 状态转换有明确约束，防止非法操作 |
| InnerAPI 契约 | interface/inner_api 定义公共契约，src/engine 为实现 | 接口与实现分离，上层仅依赖 InnerAPI 头文件 |
| v1/v2 双版本 | v1（Port 模型）服务 lite 设备，v2（LinkNext 模型）服务 standard 设备 | 兼容历史代码的同时支持新架构演进 |

## 逻辑架构

```
┌─────────────────────────────────────────────────────────────┐
│              场景封装层（Scene Layer）                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐   │
│  │ HiPlayerImpl │  │HiRecorderImpl│  │ HstEngineFactory │   │
│  │ (StateMachine)│  │(StateMachine)│  │ (Score/Create)   │   │
│  └──────┬───────┘  └──────┬───────┘  └──────────────────┘   │
├─────────┼──────────────────┼─────────────────────────────────┤
│              Pipeline 框架层（Pipeline Layer）                │
│  ┌──────────┐   ┌───────────┐   ┌──────────┐  ┌──────────┐  │
│  │MediaSrc  │──→│ Demuxer   │──→│AudioDec  │→│AudioSink │  │
│  │Filter    │──→│ Filter    │──→│VideoDec  │→│VideoSink │  │
│  └──────────┘   └───────────┘   └──────────┘  └──────────┘  │
│  FilterFactory · Pipeline · MediaSyncManager · Port(v1)/LinkNext(v2) │
├─────────────────────────────────────────────────────────────┤
│              插件层（Plugin Layer）                           │
│  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌──────────┐  │
│  │Source插件   │ │Demuxer插件 │ │Codec插件    │ │Sink插件   │  │
│  │(File/HTTP/ │ │(ffmpeg/    │ │(ffmpeg/     │ │(Audio/   │  │
│  │ HLS/Stream)│ │ AAC/WAV/   │ │ codec_      │ │ Surface/ │  │
│  │            │ │ MiniMP4)   │ │  adapter)   │ │ File)    │  │
│  └────────────┘ └────────────┘ └────────────┘ └──────────┘  │
│  PluginManager/V2 · PluginPackage · PluginList · Sniff       │
├─────────────────────────────────────────────────────────────┤
│              基础设施层（Foundation）                         │
│  AVBuffer · AVBufferQueue · AVMemory · Meta · Format · Any   │
│  OSAL(Task/Mutex/FileSystem) · MediaMonitor(SA 3013)         │
└─────────────────────────────────────────────────────────────┘
```

## 架构约束

> **全局性**的架构约束说明，需要包含 what/why/how

- **三层依赖单向**
  - **what**：场景层 → Pipeline 层 → 插件层；依赖方向不可逆向
  - **why**：逆向依赖会导致循环依赖，破坏层次边界
  - **how**：engine/scene 依赖 engine/pipeline 和 engine/plugin；engine/pipeline 不依赖 engine/scene

- **InnerAPI 是契约边界**
  - **what**：interface/inner_api 定义公共契约头文件，src/ 和 engine/ 为实现，上层仅依赖 inner_api
  - **why**：直接依赖实现头文件会导致耦合，无法独立演进
  - **how**：inner_api 头文件放 interface/inner_api/，实现放 src/ 和 engine/

- **Pipeline 统一控制**
  - **what**：生命周期操作（Prepare/Start/Stop/Flush）必须通过 Pipeline 统一下发，严禁直接操作特定 Filter
  - **why**：直接操作 Filter 会绕过状态同步，导致数据流不一致
  - **how**：所有操作通过 Pipeline::Prepare/Start/Stop 下发到全部 Filter

- **插件通过 PluginManager 创建**
  - **what**：严禁绕过 PluginManager/PluginManagerV2 直接实例化插件
  - **why**：绕过管理器会跳过 Sniff 选择和能力匹配
  - **how**：通过 PluginManager::CreateSourcePlugin/CreateDemuxerPlugin 等方法创建

- **无 RTTI 约束**
  - **what**：严禁使用 typeid/dynamic_cast，使用 Any 和 ReinterpretPointerCast
  - **why**：-fno-rtti 编译选项下 RTTI 不可用，使用会导致编译错误
  - **how**：类型安全用 Any/AnyCast，指针转换用 ReinterpretPointerCast<T>

- **Filter 注册必须通过工厂**
  - **what**：新增 Filter 必须通过 FilterFactory 注册（AutoRegisterFilter 模板），严禁直接 new
  - **why**：直接实例化会跳过工厂管理，无法被 Pipeline 动态创建
  - **how**：使用 AutoRegisterFilter<T> 静态注册，FilterFactory::CreateFilter 创建

- **状态机保护**
  - **what**：HiPlayer/HiRecorder 所有操作必须经过 StateMachine 校验
  - **why**：未校验的操作可能导致未定义行为，如未 Prepare 就 Start
  - **how**：StateMachine::SendEvent 校验操作请求合法性，非法操作请求返回错误

- **多平台条件编译**
  - **what**：平台相关代码必须用 config.gni 的 media_foundation_enable_* 标志控制
  - **why**：mini/small/standard 设备能力差异大，硬编码会导致编译失败
  - **how**：config.gni 按平台设置默认值，代码中用 #ifdef 条件编译
