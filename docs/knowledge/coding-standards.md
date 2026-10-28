# 编码铁律

## 通用编码铁律

> 描述通用编码规范，必须简洁明了，采用反模式方式描述

- **禁止**场景层依赖被 Pipeline 层依赖，**采用**单向依赖：场景层 → Pipeline 层 → 插件层
- **禁止**绕过 PluginManager/PluginManagerV2 直接实例化插件，**采用** PluginManager::CreateSourcePlugin/CreateDemuxerPlugin/CreateCodecPlugin 等方法创建
- **禁止**绕过 FilterFactory 直接 new Filter，**采用** AutoRegisterFilter<T> 静态注册 + FilterFactory::CreateFilter 创建
- **禁止**直接操作 Pipeline 内特定 Filter 的生命周期方法，**采用** Pipeline::Prepare/Start/Stop/Flush 统一下发
- **禁止**上层直接依赖 src/ 或 engine/ 实现头文件，**采用** interface/inner_api 契约头文件解耦
- **禁止**使用 typeid/dynamic_cast（-fno-rtti），**采用** Any/AnyCast 类型转换或 ReinterpretPointerCast<T> 指针转换
- **禁止**使用缩写或歧义命名，**采用**完整语义化命名（AudioDecoderFilter 非 ADF，PluginManager 非 PM）
- **禁止**成员变量与局部变量同名，**采用** trailing_ 后缀标注（currentState_、pluginState_）
- **禁止**使用 void* 传递业务数据，**采用** std::shared_ptr<AVBuffer> / std::shared_ptr<Plugin> 等强类型
- **禁止**裸指针跨模块传递，**采用** unique_ptr（唯一所有权）或 shared_ptr + weak_ptr（共享所有权打破循环）
- **禁止** C 风格强制类型转换，**采用** static_cast / reinterpret_cast 并注明理由（无 dynamic_cast 因 -fno-rtti）
- **禁止**公共头文件与内部实现混放，**采用** inner_api 头文件放 interface/inner_api/，与 src/ 和 engine/ 严格分离
- **禁止**多线程访问共享状态不加锁，**采用**互斥锁或原子变量保护（Filter 链、BufferQueue、PluginManager 注册表）
- **禁止**持锁状态下调用外部模块未知接口，**采用**锁内只操作本模块数据，跨模块调用释放锁后执行
- **禁止**回调线程执行长时间阻塞操作，**采用** Task 异步任务投递耗时处理
- **禁止**平台相关代码不加条件编译，**采用** config.gni media_foundation_enable_* 标志 + #ifdef 控制

## 其它编码铁律

> 描述安全、内存、性能等编码规范要求

- **禁止**使用不安全 C 字符串函数（strcpy/sprintf），**采用** bounds_checking_function（strncpy_s/snprintf_s）
- **禁止**文件描述符泄漏或双重关闭，**采用** fdsan_fd 封装 fd，RAII 管理
- **禁止**动态库加载后不调用 dlclose 释放，**采用** PluginPackage 引用计数归零后卸载 soHandler_
- **禁止** AVBufferQueue 生产者无限写入导致内存膨胀，**采用**容量上限 + 满时生产者阻塞等待
- **禁止** shared_ptr 循环引用不配合 weak_ptr，**采用** Filter 上下游、Callback 与引擎间使用 weak_ptr
- **禁止**音频渲染热路径分配堆内存，**采用** BufferPool 预分配复用，严禁每帧 new/malloc
- **禁止**视频解码输出路径冗余数据拷贝，**采用** AVBufferQueue 零拷贝 + SurfaceMemory 直通
- **禁止** C API 结构体不做魔数校验，**采用** native_mfmagic 校验结构体完整性
- **禁止**插件 .so 不声明 PLUGIN_DEFINITION 导出符号，**采用** PLUGIN_DEFINITION 宏明确声明插件注册入口
- **禁止** GN 构建遗漏 CFI 安全编译标志，**采用**引擎和插件共享库启用 CFI 强化
- **禁止** versionscript 遗漏导出符号声明，**采用** inner_api 导出符号明确声明，未声明符号隐藏
- **禁止**新增 Filter 未注册到 FilterFactory，**采用** AutoRegisterFilter<T> 静态注册
- **禁止**新增插件未注册到 PluginList 或 PluginPackage，**采用** PluginList::Add*Plugin 注册描述
- **禁止** Status 错误码使用随意值，**采用** Status 枚举定义值（OK=0, END_OF_STREAM=1, 错误负值）
- **禁止** Event 事件类型使用硬编码整数，**采用** EventType 枚举定义
- **禁止** Tag 键使用随意字符串，**采用** Tag 类静态 constexpr 字符串键
- **禁止** ffmpeg Adapter 修改不评估 LGPL 许可证影响，**采用**修改前进行许可证审查

## Agent 常见失败模式

> 记录 Agent 在本项目中高频出现的错误模式，编码时务必规避

| # | 失败模式 | 违反铁律 | 正确做法 |
|---|---------|---------|---------|
| 1 | 使用 dynamic_cast 进行类型转换 | 通用-6 | 使用 AnyCast 或 ReinterpretPointerCast（-fno-rtti 环境） |
| 2 | 直接 `new DemuxerFilter()` 创建过滤器 | 通用-3 | 通过 `FilterFactory::CreateFilter()` 创建 |
| 3 | 直接实例化插件 `new FFmpegDemuxerPlugin()` | 通用-2 | 通过 `PluginManager::CreateDemuxerPlugin()` 创建 |
| 4 | 在 Filter 中直接调用下游 Filter 的方法 | 通用-4 | 通过 Pipeline 统一下发或通过 Port/LinkNext 数据传递 |
| 5 | 上层直接 include engine/ 内部头文件 | 通用-5 | include interface/inner_api/ 契约头文件 |
| 6 | 新增平台相关代码未加条件编译 | 通用-15 | 使用 config.gni 标志 + #ifdef 条件编译 |
| 7 | 在回调线程中执行阻塞 IO | 通用-13 | 投递 Task 异步执行 |
| 8 | 使用 strcpy 拼接字符串 | 其它-1 | 使用 strncpy_s/snprintf_s |
| 9 | AVBuffer 跨进程传递未序列化 | — | 调用 WriteToMessageParcel/ReadFromMessageParcel |
| 10 | 新增 Tag 键使用自定义字符串 | 其它-15 | 在 Tag 类中定义 static constexpr 字符串键 |
