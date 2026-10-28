# 多平台适配

> mini/small/standard 三档设备适配决策与关键约束

## 三档设备分类

| 设备档 | 系统 | 内核 | 编译输出 | Pipeline 版本 |
|--------|------|------|---------|--------------|
| mini | Level 0 | liteos_m | 静态库（complete_static_lib） | v1 |
| small | Level 1 | liteos_a | 共享库 + 插件目录 | v1 |
| standard | Level 2+ | Linux | group + 多 inner_kits | v2 |

mini 设备不支持动态库加载，全部静态链接。config.gni 中 hst_is_mini_sys/hst_is_small_sys/hst_is_standard_sys 自动检测平台。

## 特性开关

config.gni 中 `media_foundation_enable_*` 标志控制功能编译，按平台设置不同默认值：
- standard：启用 ffmpeg_adapter、recorder、video、codec_adapter、http_source
- small/mini：启用 file_source、ffmpeg_adapter、http_source、hdi_adapter

## standard vs lite 实现差异

| 维度 | standard | lite |
|------|----------|------|
| Pipeline | v2 (inner_api LinkNext) | v1 (Port 模型) |
| 音频输出 | AudioServerSink (AudioRenderer) | HDI Audio Sink |
| 视频输出 | SurfaceSink | 无/简化 |
| 插件加载 | PluginManagerV2 (动态 .so) | PluginManager (静态注册) |

## 关键约束

- **禁止**平台相关代码不加条件编译（config.gni 标志 + #ifdef 控制）
- **禁止** mini 设备使用动态库（仅静态库）
- **禁止** lite 设备使用 v2 inner_api（使用 v1 engine）
- **禁止**新增特性开关不在 config.gni 中声明默认值

## 知识关联

- [[evolution]] — v1/v2 架构演进
- [[architecture]] — 整体架构
