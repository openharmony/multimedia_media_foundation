# AGENTS.md

This file provides guidance to CodeAgent when working with code in this repository.

## 路径基准

本仓对应 OpenHarmony `foundation/multimedia/media_foundation`。
本文中的 `docs/`、`engine/`、`src/`、`interface/`、`services/`、`test/`、`tests/` 均为本仓相对路径。
构建命令从 OpenHarmony 源码根目录执行；引用本仓 GN target 时使用
`//foundation/multimedia/media_foundation/...` 前缀。

## 项目定位

HiStreamer Media Foundation 是 OpenHarmony 多媒体子系统的**媒体引擎基础层**，
提供基于 Pipeline 的播放/录制媒体数据处理能力。向上对接 player_framework（standard 设备）
或 media_lite（mini/small 设备），内部采用三层架构（场景封装层 → Pipeline 框架层 → 插件层）。
优先按以下目录定位问题：

- `engine/scene/`：HiPlayer/HiRecorder 场景实现、StateMachine 状态机、HstEngineFactory 引擎工厂。
- `engine/pipeline/core/`：v1 Pipeline 框架（PipelineCore/FilterBase/Port 模型）。
- `engine/pipeline/factory/`：v1 FilterFactory 过滤器工厂。
- `engine/pipeline/filters/`：v1 Filter 节点实现（demux/codec/sink/source/muxer）。
- `engine/plugin/core/`：v1 插件框架（PluginManager/PluginRegister）。
- `engine/plugin/plugins/`：插件实现（ffmpeg_adapter/codec_adapter/hdi_adapter/source/demuxer/sink）。
- `engine/include/`：v1 公共头文件（pipeline/core、plugin、foundation）。
- `engine/foundation/`：OSAL 基础工具（osal、utils）。
- `src/pipeline/`：v2 Pipeline 实现（inner_api）。
- `src/filter/`：v2 Filter/FilterFactory 实现（Template Method 模式）。
- `src/buffer/`：AVBuffer/AVBufferQueue/AVMemory/Allocator 实现。
- `src/meta/`：Meta/Format/Any 类型系统实现。
- `src/capi/`：C API（Native API）实现（OH_AVBuffer/OH_AVFormat/OH_AVMemory）。
- `src/common/`：共享工具（share_memory/surface_memory/interrupt_monitor/fdsan_fd/pcm_convert）。
- `src/osal/`：OSAL 实现（pthread/FFRT、filesystem、utils）。
- `src/plugin/`：v2 插件管理（PluginManagerV2/PluginPackage/PluginList/CachedPluginPackage）。
- `interface/inner_api/`：InnerAPI 契约头文件（buffer/meta/filter/pipeline/plugin/osal/common/cpp_ext/network）。
- `interface/kits/c/`：C API 公共头文件（native_avbuffer/native_avformat/native_avmemory/native_averrors）。
- `interface/histreamer/`：引擎接口（CreateHiPlayer）。
- `services/media_monitor/`：MediaMonitor SA (3013) 服务（server/client/common/buffer）。
- `test/`：单元测试和场景测试（unittest/scenetest/fuzztest）。
- `tests/`：补充单元测试（avbuffer/format/meta/filter/pipeline/osal/event_bean）。
- `config.gni`：平台特性开关（media_foundation_enable_*）。
- `BUILD.gn`：编译输出配置（mini/small/standard）。

## 领域缩略索引

先按触发词确定方向，再到 `docs/knowledge/` 查详细知识文档，避免通读全部路由。

| 触发词 | 先看领域 |
| --- | --- |
| Pipeline、Filter、FilterBase、PipelineCore、FilterFactory、Port、LinkNext | Pipeline 架构 |
| PluginManager、PluginManagerV2、PluginPackage、PluginList、Sniff、dlopen、.z.so | 插件架构 |
| AVBuffer、AVBufferQueue、AVMemory、AVAllocator、MemoryType、Surface | Buffer 与内存 |
| Meta、Format、Any、Tag、AnyCast、OH_AVFormat、无 RTTI | Meta 与类型系统 |
| StateMachine、StateId、StateEvent、HiPlayer、HiRecorder、HstEngineFactory | 场景与状态机 |
| CodecMode、SyncMode、AsyncMode、CodecFilterBase、编解码 | 编解码模式 |
| MediaSyncManager、IMediaSyncCenter、AV同步、时间锚点 | 音视频同步 |
| Task、Mutex、Synchronizer、OSAL、FFRT、SteadyClock、BufferPool | OSAL 设计 |
| v1、v2、Legacy、inner_api、Port模型、LinkNext模型、演进 | 架构演进 |
| mini、small、standard、liteos、config.gni、平台适配 | 多平台适配 |
| MediaMonitor、EventBean、DumpBuffer、SA 3013、PCM转储 | MediaMonitor 架构 |
| OH_AVBuffer、OH_AVFormat、OH_AVMemory、C API、Native API、魔数 | C API 层 |
| ffmpeg、LGPL、codec_adapter、HDI、hcodec、硬件编解码 | 插件类型 |
| HiLog、HiTrace、Status、ErrorCode、EventType、DFX | 错误处理与 DFX |

## 典型工作流

1. 先判断改动场景，按"领域缩略索引"和"知识路由"读取对应知识文档、代码锚点。
2. 定位公开接口和内部实现边界；涉及 API 先看 `interface/inner_api/` 或 `interface/kits/c/`，再看 `src/` 或 `engine/` 实现。
3. 涉及播放/录制链路时，按 Source → Demuxer → Decoder/Encoder → Sink/Muxer 的顺序追踪数据流。
4. 小步修改，就近复用已有日志、错误码、智能指针、锁、Task 投递和测试夹具。
5. 先判断是否可定位 OpenHarmony 源码根；单仓环境取消编译环节，只做可执行静态验证。
6. 完整源码环境按"构建和验证"选择最近构建或测试目标。
7. 按"完成定义"输出结果。
8. 提交和 push 前，先完成"边界约束"中的检查。

## 技术栈

- **语言**：C++17（核心引擎），C（Native API 公共接口 OH_ 系列）
- **编译选项**：-fno-rtti -fexceptions（禁用 RTTI，启用异常；定义 HST_ANY_WITH_NO_RTTI）
- **构建系统**：GN + Ninja（OpenHarmony 标准）/ CMake（跨平台开发）
- **平台适配**：mini（liteos_m）/ small（liteos_a）/ standard（Linux），config.gni 自动检测
- **媒体引擎**：HiStreamer（Pipeline 架构）
- **插件机制**：dlopen 动态加载 .z.so、PluginManager/V2 注册发现
- **媒体处理**：ffmpeg Adapter（软解 LGPL）/ codec_adapter（硬解 HDI）/ HDI Adapter
- **任务调度**：pthread Task / FFRT（可选）
- **日志与打点**：HiLog（LOG_DOMAIN 0xD002B0A）、HiTraceMeter
- **目标系统**：OpenHarmony 系统

## 构建和验证

构建命令只从 OpenHarmony 源码根目录执行。
若当前路径或父级没有 `build.sh` 和 `prebuilts/build-tools/`，视为单仓环境，取消编译环节，
改做 `git diff --check`、`rg` 路径/符号核对、JSON/YAML 检查或 BUILD target 引用检查；
不要伪造构建结论。

```bash
# 构建主目标
./build.sh --product-name <product-name> --build-target histreamer --ccache
./build.sh --product-name <product-name> --build-target media_foundation --ccache

# 构建测试
./build.sh --product-name <product-name> --build-target histreamer_test
./build.sh --product-name <product-name> --build-target media_foundation_unit_test

# 静态检查（单仓环境 fallback）
git diff --check
rg -n "<changed-symbol>" engine/ src/ interface/  # 路径/符号核对
rg -n "#include" engine/ src/ interface/           # 头文件依赖检索
```

- `out/` 只代表已有产品输出；不能仅凭缺少 `out/` 判定为单仓。
- 产品名以已有 `out/`、当前任务说明、CI 或远程构建命令为准；`rk3568` 只是常见示例。
- 只改文档不构建，做路径、链接和空白检查即可。
- 代码改动优先构建最近 GN target；执行前确认 target 存在，并使用完整
  `//foundation/multimedia/media_foundation/...` 前缀。
- 构建失败先记录第一个真实编译错误；需要设备或 XTS 验证时，在最终回复说明缺口。
- macOS 本地通常只做静态检查；编译和设备测试使用 Linux CI、远程构建机或明确设备环境。

## 知识路由与快速入口

当前 `docs/knowledge/` 每个场景均有独立知识文档。
先按"领域缩略索引"确定方向，再查详细知识文档。

### 加载规范

- **启动时加载**：当前模块的 `glossary.md`（术语表）和 `business-context.md`（业务背景）
- **按需加载**：当前模块的 `architecture.md`（架构设计及约束）、`coding-standards.md`（编码规范）

### 术语查询流程

遇到不了解的术语词汇时：

1. 优先在当前模块的 **glossary.md** 中查询
2. 若 glossary 提供了 [[...]] 链接，按需加载对应技术文档
3. 若当前模块无该术语，直接阅读相关源码头文件

### 编辑前声明流程

编辑代码前，必须先声明以下信息：

1. **所属模块**：Pipeline / Plugin / Buffer / Meta / OSAL / Scene / CAPI / MediaMonitor / 跨模块
2. **任务类别**：Filter 开发 / 插件开发 / Pipeline 调整 / Buffer 变更 / Meta 变更 / OSAL 修改 / 状态机调整 / C API 变更 / DFX 增强 / Bug 修复 / 其它
3. **已读知识文档**：列出已阅读的对应模块 `docs/knowledge/` 下的相关文档
4. **发现的约束**：从已读文档中提取适用于本次修改的约束规则（引用项目宪法编号或编码铁律编号）

未完成声明前，不得开始编辑代码。

### 知识索引

知识目录：`docs/knowledge/`

```
docs/knowledge/
├── glossary.md                              # 术语表
├── business-context.md                      # 业务背景
├── architecture.md                          # 架构设计及约束
├── coding-standards.md                      # 编码规范
└── technologies/
    ├── pipeline-architecture.md             # Pipeline 架构（v1/v2 双版本）
    ├── plugin-architecture.md               # 插件架构（Sniff/动态加载）
    ├── buffer-and-memory.md                 # Buffer 与内存模型（MemoryType/Surface）
    ├── meta-and-format.md                   # Meta 与 Format 类型系统（Any/无 RTTI）
    ├── scene-and-state-machine.md           # 场景与状态机
    ├── multi-platform-adaptation.md         # 多平台适配（mini/small/standard）
    ├── evolution.md                         # v1→v2 架构演进
    └── flows.md                             # 关键流程（播放/录制链路）
```

| 场景 | 先读 | 加载时机 |
|------|------|---------|
| 理解业务术语 | glossary.md | 启动加载 |
| 理解系统定位与外部依赖 | business-context.md | 启动加载 |
| 模块设计 / 架构变更 | architecture.md | 按需加载 |
| 编码前必读 | coding-standards.md | 按需加载 |
| Pipeline 架构（v1/v2 双版本） | technologies/pipeline-architecture.md | 按需加载 |
| 插件架构（Sniff/动态加载） | technologies/plugin-architecture.md | 按需加载 |
| Buffer 与内存模型（MemoryType/Surface 跨进程） | technologies/buffer-and-memory.md | 按需加载 |
| Meta 与 Format 类型系统（Any/Tag/无 RTTI） | technologies/meta-and-format.md | 按需加载 |
| 场景与状态机（HiPlayer/HiRecorder/StateMachine） | technologies/scene-and-state-machine.md | 按需加载 |
| 多平台适配（mini/small/standard） | technologies/multi-platform-adaptation.md | 按需加载 |
| v1→v2 架构演进 | technologies/evolution.md | 按需加载 |
| 关键流程（播放/录制完整链路） | technologies/flows.md | 按需加载 |

### 路径→文档触发规则

修改以下目录时，必须先阅读对应知识文档：

| 修改路径 | 必读文档 | 原因 |
|----------|---------|------|
| `engine/scene/` | technologies/scene-and-state-machine.md | HiPlayer/HiRecorder 场景实现，StateMachine 状态机 |
| `engine/scene/player/` | technologies/scene-and-state-machine.md + technologies/flows.md | 播放器完整生命周期流程 |
| `engine/scene/recorder/` | technologies/scene-and-state-machine.md + technologies/flows.md | 录制器完整生命周期流程 |
| `engine/scene/standard/` | technologies/scene-and-state-machine.md | HstEngineFactory 引擎工厂，条件编译控制 |
| `engine/pipeline/core/` | technologies/pipeline-architecture.md | v1 PipelineCore/FilterBase/Port 模型 |
| `engine/pipeline/factory/` | technologies/pipeline-architecture.md | FilterFactory 过滤器工厂注册 |
| `engine/pipeline/filters/` | technologies/pipeline-architecture.md | Filter 节点实现 |
| `engine/pipeline/filters/codec/` | technologies/pipeline-architecture.md | 编解码 Filter |
| `engine/pipeline/filters/demux/` | technologies/pipeline-architecture.md | DemuxerFilter/DataPacker/TypeFinder |
| `engine/plugin/core/` | technologies/plugin-architecture.md | v1 PluginManager/PluginRegister 插件管理 |
| `engine/plugin/plugins/` | technologies/plugin-architecture.md | 各类插件实现 |
| `engine/plugin/plugins/ffmpeg_adapter/` | technologies/plugin-architecture.md | ffmpeg Adapter（LGPL 许可证约束） |
| `engine/plugin/plugins/codec_adapter/` | technologies/plugin-architecture.md | 硬件编解码 HDI 适配 |
| `engine/include/` | technologies/pipeline-architecture.md + technologies/plugin-architecture.md | v1 公共头文件 |
| `engine/foundation/` | architecture.md | OSAL 基础工具，平台适配 |
| `src/pipeline/` | technologies/pipeline-architecture.md | v2 Pipeline 实现 |
| `src/filter/` | technologies/pipeline-architecture.md | v2 Filter/FilterFactory 实现（Template Method） |
| `src/buffer/` | technologies/buffer-and-memory.md | AVBuffer/AVBufferQueue/AVMemory 内存模型 |
| `src/buffer/avbuffer/` | technologies/buffer-and-memory.md | AVBuffer/AVMemory/Allocator 实现 |
| `src/buffer/avbuffer_queue/` | technologies/buffer-and-memory.md | AVBufferQueue Producer/Consumer/IPC |
| `src/meta/` | technologies/meta-and-format.md | Meta/Format/Any 类型系统 |
| `src/capi/` | coding-standards.md | C API 实现（OH_AVBuffer/OH_AVFormat/OH_AVMemory） |
| `src/common/` | architecture.md | 共享工具（share_memory/interrupt_monitor/fdsan_fd） |
| `src/osal/` | architecture.md | OSAL 实现（pthread/FFRT） |
| `src/plugin/` | technologies/plugin-architecture.md | v2 PluginManagerV2/PluginPackage/PluginList |
| `interface/inner_api/` | architecture.md | InnerAPI 契约头文件 |
| `interface/inner_api/buffer/` | technologies/buffer-and-memory.md | Buffer 契约 |
| `interface/inner_api/meta/` | technologies/meta-and-format.md | Meta 契约 |
| `interface/inner_api/filter/` | technologies/pipeline-architecture.md | Filter 契约（FilterType/FilterState） |
| `interface/inner_api/pipeline/` | technologies/pipeline-architecture.md | Pipeline 契约 |
| `interface/inner_api/plugin/` | technologies/plugin-architecture.md | 插件契约 |
| `interface/inner_api/osal/` | architecture.md | OSAL 契约 |
| `interface/kits/c/` | coding-standards.md | C API 公共头文件 |
| `interface/histreamer/` | technologies/scene-and-state-machine.md | 引擎接口（CreateHiPlayer） |
| `services/media_monitor/` | architecture.md | MediaMonitor SA (3013) |
| `config.gni` | technologies/multi-platform-adaptation.md | 平台特性开关 |
| `BUILD.gn` | technologies/multi-platform-adaptation.md | 编译输出配置（mini/small/standard） |

### 高频修改场景→源码文件映射

| 修改场景 | 涉及源码文件 | 必读知识文档 |
|----------|------------|------------|
| Filter 新增/修改 | 对应 `engine/pipeline/filters/*/*.cpp` 或 `src/filter/` | technologies/pipeline-architecture.md |
| Pipeline 链路调整 | `src/pipeline/pipeline.cpp` 或 `engine/pipeline/core/pipeline_core.cpp` | technologies/pipeline-architecture.md |
| FilterFactory 注册 | `src/filter/filter_factory.cpp` 或 `engine/pipeline/factory/filter_factory.cpp` | technologies/pipeline-architecture.md |
| 插件新增/修改 | `engine/plugin/plugins/*/*.cpp` | technologies/plugin-architecture.md |
| 插件管理修改 | `src/plugin/plugin_manager_v2.cpp` 或 `engine/plugin/core/plugin_manager.cpp` | technologies/plugin-architecture.md |
| 插件动态加载 | `src/plugin/plugin_package.cpp`、`src/plugin/cached_plugin_package.cpp` | technologies/plugin-architecture.md |
| AVBuffer/AVMemory 修改 | `src/buffer/avbuffer/avbuffer.cpp`、`avmemory.cpp` | technologies/buffer-and-memory.md |
| AVBufferQueue 修改 | `src/buffer/avbuffer_queue/*.cpp` | technologies/buffer-and-memory.md |
| Meta/Format 修改 | `src/meta/meta.cpp`、`format.cpp`、`any.cpp` | technologies/meta-and-format.md |
| Tag 标签新增 | `interface/inner_api/meta/meta_key.h` | technologies/meta-and-format.md |
| 状态机调整 | `engine/scene/player/internal/state_machine.cpp`、`state*.h` | technologies/scene-and-state-machine.md |
| HiPlayer 引擎修改 | `engine/scene/player/standard/hiplayer_impl.cpp` | technologies/scene-and-state-machine.md + technologies/flows.md |
| HiRecorder 引擎修改 | `engine/scene/recorder/standard/hirecorder_impl.cpp` | technologies/scene-and-state-machine.md + technologies/flows.md |
| 引擎工厂修改 | `engine/scene/standard/hst_engine_factory.cpp` | technologies/scene-and-state-machine.md |
| C API 变更 | `src/capi/native_avbuffer.cpp`、`native_avformat.cpp`、`native_avmemory.cpp` | coding-standards.md |
| OSAL 修改 | `src/osal/task/*.cpp`、`src/osal/filesystem/*.cpp` | architecture.md |
| 平台适配修改 | `config.gni`、`BUILD.gn` | technologies/multi-platform-adaptation.md |
| MediaMonitor 修改 | `services/media_monitor/server/*.cpp` | architecture.md |
| 错误码/DFX 打点 | 对应模块错误处理代码 | coding-standards.md |

## 架构概要

### 三层架构

- **场景封装层**（`engine/scene/`）：HiPlayer/HiRecorder，状态机驱动，引擎工厂
- **Pipeline 框架层**（`engine/pipeline/` + `src/pipeline/` + `src/filter/`）：Filter 链容器，过滤器节点
- **插件层**（`engine/plugin/` + `src/plugin/`）：PluginManager/V2，可扩展插件

### v1/v2 双版本架构

| 版本 | 路径 | 数据模型 | 使用场景 |
|------|------|---------|---------|
| v1 (Legacy) | `engine/pipeline/core/` | Port(Push/Pull) 模型 | media_lite（mini/small） |
| v2 (inner_api) | `interface/inner_api/` + `src/` | LinkNext(Template Method) 模型 | player_framework（standard） |

### 核心处理链路

```
上层框架 (player_framework / media_lite)
  → HstEngineFactory (Score 打分)
  → HiPlayerImpl / HiRecorderImpl (场景引擎)
  → StateMachine (状态机校验)
  → Pipeline (过滤器链)
    ├─ 播放: MediaSource → Demuxer → Decoder → Sink
    └─ 录制: Capture → Encoder → Muxer → OutputSink
  → PluginManager/V2 (插件创建)
  → 插件 (ffmpeg/codec_adapter/HDI/Source/Sink)
  → HDI 硬件驱动
```

## 项目宪法

1. **三层依赖单向**：场景层 → Pipeline 层 → 插件层；依赖方向不可逆向。
2. **InnerAPI 是契约边界**：interface/inner_api 定义公共契约，上层仅依赖 inner_api 头文件，严禁直接依赖 src/ 或 engine/ 实现头文件。
3. **Pipeline 统一控制**：Prepare/Start/Stop/Flush 等生命周期操作必须通过 Pipeline 统一下发，严禁直接操作特定 Filter。
4. **插件通过 PluginManager 创建**：严禁绕过 PluginManager/PluginManagerV2 直接实例化插件。
5. **Filter 注册必须通过工厂**：新增 Filter 必须通过 FilterFactory（AutoRegisterFilter）注册，严禁直接 new。
6. **无 RTTI 约束**：严禁使用 typeid/dynamic_cast，使用 Any/AnyCast 和 ReinterpretPointerCast（-fno-rtti 编译）。
7. **状态机保护**：HiPlayer/HiRecorder 所有操作必须经过 StateMachine 校验，非法操作请求返回错误。
8. **多平台条件编译**：平台相关代码必须用 config.gni 的 media_foundation_enable_* 标志 + #ifdef 控制。
9. **DFX 可观测**：关键路径必须有 HiLog 日志和 HiTrace 打点。
10. **第三方许可证合规**：ffmpeg 组件受 LGPL 许可证约束，修改 ffmpeg Adapter 需评估许可证影响；codec_adapter 为 HDI 接口封装，严禁绕过 HDI 直接调用厂商接口。

## 边界约束

### 禁止事项（Do not）

- 禁止场景层依赖被 Pipeline 层依赖（单向依赖：场景层 → Pipeline 层 → 插件层）。
- 禁止绕过 PluginManager/PluginManagerV2 直接实例化插件。
- 禁止绕过 FilterFactory 直接 new Filter。
- 禁止直接操作 Pipeline 内特定 Filter 的生命周期方法（必须通过 Pipeline 统一下发）。
- 禁止上层直接依赖 src/ 或 engine/ 实现头文件（使用 interface/inner_api 契约头文件）。
- 禁止使用 typeid/dynamic_cast（-fno-rtti 环境），使用 Any/AnyCast 或 ReinterpretPointerCast。
- 禁止 v1 Port 模型与 v2 LinkNext 模型混用。
- 禁止平台相关代码不加条件编译。
- 禁止使用不安全 C 字符串函数（strcpy/sprintf），使用 bounds_checking_function（strncpy_s/snprintf_s）。
- 禁止动态库加载后不调用 dlclose 释放。
- 禁止 AVBufferQueue 生产者无限写入（容量上限 AVBUFFER_QUEUE_MAX_QUEUE_SIZE=500）。
- 禁止 C API 不校验魔数直接访问内部结构（native_mfmagic）。
- 禁止 ffmpeg Adapter 修改不评估 LGPL 许可证影响。
- 禁止新增元数据标签使用自定义字符串（必须在 Tag 类中定义 static constexpr 键）。
- 禁止新增 Filter/插件未注册到 FilterFactory/PluginList。
- 禁止直接编辑自动生成的插件注册导出符号（PLUGIN_DEFINITION 宏生成的注册入口）；应修改插件源码并重新构建。
- 禁止执行破坏性 git/文件操作（如 `rm -rf`、`git reset --hard`、`git push --force`）除非用户明确要求。
- 禁止移除或抑制 `MEDIA_LOG_I/W/E` 日志调用以消除测试输出。
- 禁止绕过权限检查或 IPC token 验证以通过测试。
- 禁止绕过 HDI 直接调用 hcodec/fcodec 厂商闭源接口。

### 需先确认（Ask before）

- 修改 `interface/inner_api/` 中公共契约头文件签名前需确认。
- 修改 `interface/kits/c/` 中 C API 公共接口签名/参数名/错误码前需确认。
- 修改 v1/v2 架构边界（engine/ vs interface/inner_api + src/）前需确认。
- 修改 `config.gni` 平台特性开关默认值前需确认。
- 修改 StateMachine 状态机（StateId/StateEvent/状态流转）前需确认。
- 修改 MediaSyncManager 音视频同步算法前需确认。
- 修改 CodecMode 同步/异步策略影响编解码行为前需确认。
- 修改 AVBuffer/AVBufferQueue IPC 序列化字段顺序前需确认。
- 修改 EventBean 序列化字段影响 IPC 兼容性前需确认。
- 修改 HstEngineFactory Score 打分逻辑前需确认。
- 新增或修改第三方依赖（bundle.json 依赖变更）前需确认。
- 修改 `config.gni` 新增特性开关影响编译输出前需确认。

### 架构不变量

- **三层依赖单向**：场景层 → Pipeline 层 → 插件层；不可逆向。
- **InnerAPI 是契约边界**：上层仅依赖 interface/inner_api 头文件。
- **Pipeline 统一控制**：生命周期操作通过 Pipeline 统一下发。
- **引擎可替换**：HstEngineFactory 通过 Score 打分选择，条件编译控制支持能力。
- **v1/v2 隔离**：v1（engine/）与 v2（interface/inner_api + src/）代码独立，不互相依赖。

### Agent 自主边界

- "5 个文件"是同一用户任务或同一会话累计阈值，不是单个 commit 阈值；不得拆提交绕过。
- 可自主完成：文档、知识路由、注释、局部测试补充和单模块小修。
- 可自主推进但需说明风险：同一模块累计不超过 5 个文件，行为边界清晰且可验证。
- 单仓或缺少 OpenHarmony 根环境时，不因无法编译而默认阻塞；编译环节按"构建和验证"取消。
- 单仓可自主完成的静态验证包括 `git diff --check`、`rg` 路径/符号核对、JSON/YAML 校验、
  BUILD 引用检查和头文件依赖检索。
- 若远程 CI 或构建机入口已给出，按该入口验证并记录结果；未给出时不要臆造 CI 结论。
- 跨模块或跨仓变化、设备行为、性能功耗结论、公开 API/ABI 变化，必须先确认。

## DFX 约束

- 所有错误路径在返回错误码前必须调用 `HiLog` 并带模块标签（LOG_DOMAIN 0xD002B0A）。
- 关键路径必须有 `HiTraceMeter` 打点。
- 媒体事件必须通过 `MediaMonitorManager::WriteLogMsg(EventBean)` 异步上报。
- C API 入口必须校验 `native_mfmagic` 魔数。
- `InterruptMonitor` 不可禁用；它防止音频中断时播放器状态不一致。

## 常见 Agent 失败模式

- Agent 使用 `dynamic_cast` 进行类型转换 → 编译错误。-fno-rtti 环境，使用 `AnyCast` 或 `ReinterpretPointerCast`。
- Agent 直接 `new DemuxerFilter()` 创建过滤器 → 绕过工厂。通过 `FilterFactory::CreateFilter()` 创建。
- Agent 直接实例化插件 `new FFmpegDemuxerPlugin()` → 绕过管理器。通过 `PluginManager::CreateDemuxerPlugin()` 创建。
- Agent 在 Filter 中直接调用下游 Filter 方法 → 绕过 Pipeline。通过 Pipeline 统一下发或 AVBufferQueue 传递。
- Agent 上层直接 include engine/ 内部头文件 → 破坏契约边界。include interface/inner_api/ 头文件。
- Agent 新增平台相关代码未加条件编译 → 编译失败。使用 config.gni 标志 + #ifdef。
- Agent 在回调线程中执行阻塞 IO → 阻塞回调。投递 Task 异步执行。
- Agent 使用 strcpy 拼接字符串 → 内存不安全。使用 strncpy_s/snprintf_s。
- Agent 新增 Tag 键使用自定义字符串 → 不规范。在 Tag 类中定义 static constexpr 字符串键。
- Agent 修改 IPC 序列化字段但只改一侧 → IPC 不匹配。必须同时修改 Proxy 和 Stub。
- Agent 文件描述符不通过 FdsanFd 管理 → fd 泄漏/双重关闭。使用 FdsanFd 封装。

## 验证

### 最低检查

- 构建：`./build.sh --product-name <product-name> --build-target histreamer --ccache`
- 单元测试：`./build.sh --product-name <product-name> --build-target media_foundation_unit_test`
- 场景测试：`./build.sh --product-name <product-name> --build-target histreamer_test`
- 静态检查：`git diff --check`

### 任务特定检查

| 变更类型 | 附加检查 |
|---------|---------|
| Filter 变更 | 验证 Filter 生命周期（Init→Prepare→Start→Stop→Release）；验证 FilterState 状态转换；验证上下游 AVBufferQueue 链接 |
| Pipeline 变更 | 验证 LinkFilters/LinkNext 链接正确；验证统一生命周期控制；验证 v1/v2 不混用 |
| 插件变更 | 验证 PluginList 注册；验证 PLUGIN_DEFINITION 导出；验证 Sniff/能力匹配；验证 dlclose 无泄漏 |
| FilterFactory 变更 | 验证 AutoRegisterFilter 静态注册；验证 CreateFilter 创建成功 |
| AVBuffer/AVBufferQueue 变更 | 验证 IPC 序列化（WriteToMessageParcel/ReadFromMessageParcel）；验证 Surface 模式；验证容量上限 |
| Meta/Format 变更 | 验证 ToParcel/FromParcel 序列化；验证 Tag 键定义在 meta_key.h |
| 状态机变更 | 验证状态转换覆盖所有操作请求；验证非法操作请求返回错误；验证三层（NAPI/Server/Engine）一致 |
| C API 变更 | 验证魔数校验（native_mfmagic）；验证 OH_AV* 兼容性；验证 SysCap 声明 |
| MediaMonitor 变更 | 验证 EventBean IPC 序列化；验证 oneway 异步上报；验证 SA 3013 配置 |
| 平台适配变更 | 验证 mini/small/standard 三档编译通过；验证 config.gni 特性开关 |
| 编解码变更 | 验证 SyncMode/AsyncMode 正确选择；验证 Flush 成对调用（FlushStart/FlushEnd） |
| 音视频同步变更 | 验证 MediaSyncManager 时间锚点更新；验证 PTS 单调递增 |
| IPC 变更 | 验证 Proxy 和 Stub 字段顺序一致；验证回调 ID 不变 |
| DFX 打点变更 | 验证 HiLog/HiTrace 打点完整；验证 MediaMonitor 事件上报 |

### 完成定义

最终回复按任务复杂度收口，不机械打印固定清单。
不适用的构建、设备、提交项可以省略，避免把简单任务回复拉长。

轻量任务（问答、文档删节、措辞调整、路径核对）：

- 简要说明结论或改动点。
- 说明已做的静态检查；未验证时用一句话说明原因。
- 只改 1 到 2 个文档时，不必逐条展开未修改文件、设备、提交状态。

代码、接口、构建、跨模块任务：

- 说明读取过的知识文档、修改文件、行为影响面和关键未改文件。
- 列出已执行的构建、单测、场景测试、fuzz 命令；未执行时说明原因。
- 说明资源释放情况（Filter/Plugin/Buffer/Plugin .so/fd 已释放）。
- 当前为单仓环境时，说明取消编译的原因和替代静态检查；
  完整 OpenHarmony 根环境时，说明使用的产品和 target。
- 列出未解决的风险或无法验证的内容，以及评审者应执行的手动验证。

### 最终响应要求

最终响应必须列出：(1) 变更文件，(2) 运行的检查及结果，(3) 未解决的风险或无法验证的内容。

### 验证降级

若构建或测试无法运行，报告：(1) 哪些验证步骤无法运行，(2) 原因，(3) 剩余未验证风险，(4) 评审者应执行的手动验证。
单仓环境可自主完成的静态验证包括 `git diff --check`、`rg` 路径/符号核对、JSON/YAML 校验、BUILD 引用检查和头文件依赖检索。
不要伪造构建结论或 CI 结果。
