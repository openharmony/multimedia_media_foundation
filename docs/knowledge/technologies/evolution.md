# v1→v2 架构演进

> HiStreamer 双版本架构演进决策与共存策略

## 演进概览

HiStreamer 从 v1（engine 内部架构）演进到 v2（inner_api 契约架构），两套架构目前并存。

| 阶段 | 特征 |
|------|------|
| 初始阶段 | v1：engine/ 内部 Pipeline（Port 模型），PluginManager（Sniff 打分） |
| 演进阶段 | v1 + v2 并存：新增 interface/inner_api 契约层 + src/ 实现 |
| 当前阶段 | v1/v2 双轨：v1 服务 media_lite，v2 服务 player_framework |

## v1 局限性（v2 改进点）

- v1 接口与实现混放在 engine/ 内 → v2 接口与实现分离（inner_api 契约 + src/ 实现）
- v1 Filter 间 Port 直接 Push/Pull 耦合 → v2 LinkNext + AVBufferQueue 标准化
- v1 直接 virtual 方法 → v2 Template Method（final + Do*）统一生命周期
- v1 Sniff 打分插件选择 → v2 PluginList 描述匹配 + PluginPackage 动态加载
- v1 FilterState 简单 → v2 FilterState 10 状态完整状态机

## 共存策略

两套代码完全独立，不互相依赖：

- **v1 路径**：`engine/pipeline/core/` + `engine/include/pipeline/core/` + `engine/plugin/core/`
- **v2 路径**：`interface/inner_api/` + `src/pipeline/` + `src/filter/` + `src/plugin/`

场景选择：
- standard 设备 → player_framework → HstEngineFactory → HiPlayerImpl(standard) → v2 Pipeline
- mini/small 设备 → media_lite → CreateHiPlayer() → HiPlayerImpl(lite) → v1 PipelineCore

公共组件（v1/v2 共用）：OSAL、Meta/Format/Any、AVBuffer/AVBufferQueue、C API。

## 关键约束

- **禁止** v1 Port 模型与 v2 LinkNext 模型混用
- **禁止** v2 代码依赖 engine/ 内部头文件
- **禁止** v1 代码依赖 interface/inner_api 头文件（保持独立性）

## 知识关联

- [[pipeline-architecture]] — Pipeline 架构
- [[plugin-architecture]] — 插件架构
- [[multi-platform-adaptation]] — 多平台适配
