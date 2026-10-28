# 插件架构

> v1/v2 插件管理设计决策与关键约束

## v1/v2 插件管理对比

| 维度 | v1 (PluginManager) | v2 (PluginManagerV2) |
|------|---------------------|----------------------|
| 选择机制 | Sniff 嗅探打分（循环调用 sniffer 返回 confidence） | PluginList 硬编码描述匹配（按 MIME/名称/rank） |
| 注册方式 | AutoRegisterPlugin 模板静态注册 | PluginList::Add*Plugin 描述 + PluginPackage 动态加载 |
| 动态加载 | 无 | dlopen/dlsym 从 `/system/lib64/media/media_plugins/` 加载 .z.so |
| 使用场景 | media_lite（mini/small） | player_framework（standard） |

## 插件类型

插件按 PluginType 枚举分类：SOURCE / DEMUXER / AUDIO_DECODER / AUDIO_ENCODER / VIDEO_DECODER / VIDEO_ENCODER / AUDIO_SINK / VIDEO_SINK / MUXER / OUTPUT_SINK。

插件实现路径：`engine/plugin/plugins/`（含 ffmpeg_adapter 软解、codec_adapter 硬解 HDI、hdi_adapter 音频输出、source/demuxer/sink 原生插件）。

## 关键约束

- **禁止**绕过 PluginManager/PluginManagerV2 直接实例化插件
- **禁止**插件 .so 不声明 PLUGIN_DEFINITION 导出符号
- **禁止**新增插件未注册到 PluginList 或 PluginPackage
- **禁止**动态库加载后不调用 dlclose 释放（PluginPackage 引用计数归零后卸载）
- **禁止** ffmpeg Adapter 修改不评估 LGPL 许可证影响
- **禁止** codec_adapter 绕过 HDI 直接调用 hcodec/fcodec 厂商闭源接口

## 知识关联

- [[pipeline-architecture]] — Filter 通过 PluginManager 创建插件
- [[evolution]] — v1→v2 架构演进
