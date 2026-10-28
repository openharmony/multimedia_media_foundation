# 术语表

> 只说明模型不知道的术语，常识类不要写

## HiStreamer
OpenHarmony 多媒体子系统的轻量级媒体引擎组件，提供基于 Pipeline 的播放/录制媒体数据处理能力。向上对接 player_framework（standard 设备）或 media_lite（mini/small 设备），内部采用三层架构（场景封装层 → Pipeline 框架层 → 插件层）。

**触发加载**：当任务涉及"HiStreamer/媒体引擎/Pipeline引擎"时 → 必读 [[architecture]]

## v1/v2 双版本架构
HiStreamer 存在两套并行架构：v1（engine/，Port/Push/Pull 模型）和 v2（interface/inner_api/ + src/，LinkNext/Template Method 模型）。player_framework 使用 v2，media_lite 使用 v1。两套代码完全独立，不互相依赖。

**触发加载**：当任务涉及"v1/v2/Legacy/inner_api/Port模型/LinkNext模型"时 → 必读 [[evolution]] + [[pipeline-architecture]]

## 插件嗅探（Sniff）
Demuxer 插件注册 Sniffer 函数，PluginManager 循环调用返回 confidence 分数，自动选择最优解封装插件。新增格式只需注册 Sniffer 无需修改框架。

**触发加载**：当任务涉及"Sniff/Sniffer/插件选择/confidence"时 → 必读 [[plugin-architecture]]

## 插件动态加载（PluginPackage）
通过 dlopen/dlsym 从 `/system/lib64/media/media_plugins/` 目录加载 `.z.so` 插件包。v2 通过 PluginList 硬编码描述匹配 + PluginPackage 动态加载。

**触发加载**：当任务涉及"dlopen/PluginPackage/.z.so/插件动态库"时 → 必读 [[plugin-architecture]]

## 内存类型（MemoryType）
AVBuffer 底层内存分 5 种类型：VIRTUAL_MEMORY（malloc）、SHARED_MEMORY（Ashmem 跨进程）、SURFACE_MEMORY（Surface Buffer 视频渲染）、HARDWARE_MEMORY（DMA buffer 硬件零拷贝）、CUSTOM_MEMORY。类型决定分配器，不可混用。

**触发加载**：当任务涉及"MemoryType/AVAllocator/内存分配器"时 → 必读 [[buffer-and-memory]]

## 无 RTTI 类型系统（Any）
-fno-rtti 编译环境下，使用自定义 Any 类型安全容器替代 typeid/dynamic_cast。Meta 和 Format 底层均使用 Any 存储值。指针转换使用 ReinterpretPointerCast<T>。

**触发加载**：当任务涉及"Any/AnyCast/no-RTTI/HST_ANY_WITH_NO_RTTI"时 → 必读 [[meta-and-format]]

## 异步同步器（Synchronizer<K,V>）
OSAL 提供的异步意图-结果同步模板。StateMachine 使用 Synchronizer<StateEvent, ErrorCode> 同步状态转换——SendEvent 发送操作请求，WaitResult 等待结果。

**触发加载**：当任务涉及"Synchronizer/异步同步/状态机同步"时 → 必读 [[scene-and-state-machine]]

## 多平台适配（mini/small/standard）
config.gni 中 hst_is_mini_sys/hst_is_small_sys/hst_is_standard_sys 自动适配三档设备。mini 编译为静态库，small/standard 编译为共享库。v1 服务 lite 设备，v2 服务 standard 设备。

**触发加载**：当任务涉及"mini/small/standard/平台适配/config.gni"时 → 必读 [[multi-platform-adaptation]]
