# 场景与状态机

> HiPlayer/HiRecorder 状态机设计与关键约束

## 状态机设计

StateMachine 继承 OSAL::Task，在独立线程处理状态转换。通过 Synchronizer<StateEvent, ErrorCode> 实现异步同步——SendEvent 发送操作请求，WaitResult 等待结果。每个 State 校验操作请求合法性，非法操作请求返回错误。

## 播放器状态流转

```
IDLE ──SetSource──→ INIT ──Prepare──→ PREPARING ──完成──→ READY
  ↑                                              ↓ Play
  │                                         PLAYING ←─Resume─ PAUSE
  │                                              ↓ Stop       ↓ Stop
  └──────────────── RESET ←── STOPPED ←──────────┘       ↓ Seek
                                                          EOS
```

各状态限制允许的操作（如 IDLE 仅 SetSource，PLAYING 允许 Pause/Stop/Seek），非法操作返回错误。

## 录制器状态流转

```
INIT ──SetSource──→ READY ──Prepare──→ RECORDING_SETTING ──完成──→ RECORDING
                       ↑                    ↑                      ↓ Pause
                   RESET               PAUSE ←──Resume────────── PAUSE
                                              ↓ Stop
                                          RECORDING_SETTING
```

## 引擎工厂条件编译

HstEngineFactory 通过条件编译控制支持能力：SUPPORT_PLAYER / SUPPORT_RECORDER / SUPPORT_METADATA / SUPPORT_CODEC / SUPPORT_MUXER。

## 关键约束

- **禁止**所有操作不经过 StateMachine 校验（非法操作请求返回错误）
- **禁止**引擎直接持有特定 Filter 引用（通过 Pipeline 控制）
- **禁止**状态转换不同步 NAPI、Server、Engine 三层
- **禁止** HstEngineFactory 硬编码引擎创建（条件编译控制）

## 知识关联

- [[pipeline-architecture]] — Pipeline 架构
- [[flows]] — 播放/录制完整流程
