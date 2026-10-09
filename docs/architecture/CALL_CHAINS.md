# 关键调用链

## MAVLink 到 Vehicle

```text
Link 收到字节
  → MAVLinkProtocol 解析完整 mavlink_message_t
  → emit MAVLinkProtocol::messageReceived
  → Vehicle::_mavlinkMessageReceived
  → 插件与各管理器处理
  → 遍历 Vehicle::factGroups()
  → FactGroup::handleMessage
```

`VehicleFtcStatusFactGroup` 通过现有 FactGroup 循环接收消息。`Vehicle::_mavlinkMessageReceived` 没有增加 FTC 分支，这是为了让协议扩展继续走统一分发路径。

Qt 的信号槽连接和 QML 属性访问属于动态边界。GitNexus 对 `Vehicle` 给出的变更影响为 CRITICAL（426 个受影响节点），但无法完整解析 `_mavlinkMessageReceived` 的 Qt 调用者。因此修改 Vehicle 时同时检查构造顺序、FactGroup 注册和 QML 属性暴露，不能只依赖静态调用数。

## Fly View

```text
MainRootWindow
  → FlyView
  → FlyViewMap（custom qrc 别名）
  → CommandCenterOverlay
      → vehicle.escStatus
      → vehicle.ftcStatus
      → FtcStatusPanel
      → FtcStatusPalette
```

## FTC 状态刷新

```text
MERIVUS_FTC_* 消息
  → VehicleFtcStatusFactGroup::handleMessage
  → 协议版本检查
  → 更新对应消息族时间戳
  → 解码属性 / 更新电机模型
  → emit statusChanged
  → QML 绑定重算

500 ms 定时器
  → _refreshStale
  → 超过 3000 ms 标记 stale
  → 电机模型 AvailableRole 变 false
  → QML 显示 N/A
```

## 方言生成

```text
schemas/mavlink/*.xml
  + GroundStation 已跟踪的上游 message_definitions
  + FirmwarePX4 锁定的 pymavlink@18955a04...
  → tools/dev/generate-merivus-mavlink.ps1
  → libs/mavlink/include/mavlink/v2.0/{merivus,merivus_ftc}
```
