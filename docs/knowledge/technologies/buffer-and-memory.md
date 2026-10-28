# Buffer 与内存模型

> MemoryType 设计决策、Surface 跨进程模式与关键约束

## MemoryType 设计

AVBuffer 底层内存分 5 种类型，类型决定分配器，不可混用：

| MemoryType | 分配方式 | 适用场景 |
|-----------|---------|---------|
| VIRTUAL_MEMORY | malloc | CPU 通用内存，进程内使用 |
| SHARED_MEMORY | Ashmem | 跨进程共享，通过 fd 传递 |
| SURFACE_MEMORY | Surface Buffer | 视频渲染，跨进程渲染直通 |
| HARDWARE_MEMORY | DMA buffer | 硬件编解码零拷贝直通 |
| CUSTOM_MEMORY | 用户自定义 | 自定义分配器 |

## AVBufferQueue 生产者-消费者模型

Filter 间通过 AVBufferQueue 零拷贝传递 AVBuffer。队列最大容量 AVBUFFER_QUEUE_MAX_QUEUE_SIZE=500，满时生产者阻塞，空时消费者阻塞。

## Surface 跨进程模式

视频解码器（如 codec_server 独立进程）通过 Surface 写入视频帧：

```
AVBufferQueue::CreateAsSurfaceProducer() → 返回 Surface 给解码器进程
  → 解码器写入 Surface Buffer
  → AVBufferQueueProducerProxy (IPC) → ProducerStub
  → Consumer 进程 AcquireBuffer 获取
```

AVBuffer 跨进程传递必须通过 WriteToMessageParcel/ReadFromMessageParcel 序列化（含 pts/dts/flag/meta/memory fd）。

## 关键约束

- **禁止** AVBuffer 跨进程传递未序列化
- **禁止** MemoryType 混用（Virtual 分配器不可分配 Surface 内存）
- **禁止** AVBufferQueue 生产者无限写入（容量上限 500）
- **禁止** Surface 模式不通过 CreateAsSurfaceProducer/Consumer 创建
- **禁止** C API 不校验魔数（native_mfmagic）

## 知识关联

- [[pipeline-architecture]] — Filter 通过 AVBufferQueue 传递数据
- [[meta-and-format]] — AVBuffer 携带 Meta 元数据
