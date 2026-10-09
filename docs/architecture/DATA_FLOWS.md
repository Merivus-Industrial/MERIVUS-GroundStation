# 数据流

## 飞行遥测

```text
PX4 uORB
  → MavlinkStreamMerivusFtc*
  → MAVLink 2 帧
  → LinkInterface
  → MAVLinkProtocol
  → Vehicle
  → VehicleFtcStatusFactGroup
  → QML 只读属性 / FtcMotorStatusModel
  → 电机卡片与 FTC 面板
```

后端使用地面站接收时间维护 `lastUpdate`。四类消息各有接收标记和 3 秒过期标记。未接收不是数值 0；过期数据保留用于诊断，但 `available` 角色变为 false，界面显示 `N/A`。

百分比在线路上使用 `uint8_t`：`0..200` 对应 `0..100%`，步长 `0.5%`，`255` 表示不可用。后端统一还原为 `0..100`，QML 不重复缩放。

## ESC 与 FTC 的关系

ESC 数据描述转速、电压、电流、温度和在线标志。FTC 数据描述观测健康、效能、故障概率、置信度和控制裕度。两者并列显示，互不伪造：没有 FTC 消息时仍可显示 ESC；没有 ESC 时 FTC 观测也不会被写成实际转速。

## 人工指令

```text
CommandCenterOverlay
  → Guided / Swarm 控制器
  → 确认流程
  → Vehicle 命令发送
  → 飞控 ACK / 状态回传
```

选择状态、按钮状态和 AI proposal 都不是飞控 ACK。FTC 面板是只读状态面板，不进入人工指令链。

## AI 建议

```text
用户问题 → QGC Agent Client → Local Agent / Provider
         ← reply 或 ActionProposal
ActionProposal → QGC schema / 风险 / 策略检查 → 人工确认边界
```

当前 proposal 保持 `executable=false`。这条链与 FTC 恢复候选链互不接管。
