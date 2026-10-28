# Meta 与 Format 类型系统

> 无 RTTI 设计决策与关键约束

## 无 RTTI 类型系统

HiStreamer 在 -fno-rtti 编译环境下使用自定义类型系统替代标准 C++ RTTI：

```
Any (类型安全容器，替代 typeid/dynamic_cast)
  ↓ 被使用于
Meta (Tag 键 → Any 值，支持 IPC 序列化)
  ↓ 互转
Format (字符串键 → 多类型值，C API 通过 OH_AVFormat 暴露)
```

- **Any**：内部存储类型标识（AnyValueType 枚举），通过 AnyCast<T> 安全转换，IsSameTypeWith 判断类型
- **ReinterpretPointerCast<T>**：无 RTTI 环境下的指针转换，替代 dynamic_cast
- **Meta**：Tag 类定义 static constexpr 字符串键（REGULAR_*/MEDIA_*/AUDIO_*/VIDEO_*/BUFFER_* 等），通过 DEFINE_INSERT_GET_FUNC 宏定义 tag-type 映射
- **Format**：字符串键值对容器，支持 Int/Long/Float/Double/String/Buffer 类型，可与 Meta 互转

## 关键约束

- **禁止**使用 typeid/dynamic_cast（-fno-rtti 环境），使用 Any/AnyCast 或 ReinterpretPointerCast
- **禁止**新增元数据标签使用自定义字符串（必须在 Tag 类 meta_key.h 中定义 static constexpr 键）
- **禁止** Meta 跨进程传递不通过 ToParcel/FromParcel
- **禁止**使用 void* 传递类型化数据，使用 Any

## 知识关联

- [[buffer-and-memory]] — AVBuffer 携带 Meta 元数据
- [[pipeline-architecture]] — Filter 间传递 Meta
