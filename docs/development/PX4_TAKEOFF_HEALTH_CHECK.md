# PX4 起飞健康检查与自动解锁契约

## 目标

所有 PX4 引导起飞统一由 `PX4FirmwarePlugin` 负责刷新飞控预检结果、判断是否允许起飞并处理自动解锁。单机 UI 和编队入口都调用 `Vehicle::guidedModeTakeoff`，不得分别复制健康检查逻辑。

## 不变量

1. 起飞前发送 `MAV_CMD_RUN_PREARM_CHECKS`；
2. 只有同时收到命令接受 ACK 和比请求前更新的健康报告，才允许继续；
3. 报告不受支持、超时、链路无响应或 `canTakeoff == false` 时安全阻止起飞；
4. `MAV_CMD_NAV_TAKEOFF` 被接受但飞行器仍未解锁时，再刷新一次报告；只有 `canTakeoff` 与 `canArm` 都成立才自动解锁；
5. 起飞或解锁被拒绝后再次刷新报告，为操作者显示飞控当前给出的原因；
6. 每架飞行器拥有独立阶段、计时器、报告序号和待起飞高度，禁止共享瞬时状态。

`HealthAndArmingCheckReport::updateSequence` 只在飞控发送新的健康结果时递增。飞行模式切换引起的本地重算不得伪装成新的飞控报告。

## 责任边界

- `HealthAndArmingCheckReport`：把 PX4 事件结果收敛为 `canTakeoff`、`canArm`、GPS 状态和可本地化的失败原因；
- `PX4FirmwarePlugin`：拥有异步起飞/解锁状态机和超时策略；
- `Vehicle`：提供统一引导起飞入口并标记报告更新来源；
- `SwarmController`：只筛除断链、已飞行、不支持起飞或高度无效的成员，不缓存起飞健康结论；编队形成前的静态健康门槛仍保留；
- QML：只显示状态和触发动作，不决定飞控安全条件。

## 可重复检查

Windows PowerShell：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/dev/test-takeoff-health-contract.ps1
```

该检查验证上述状态机边界是否仍由唯一责任方维护。完整行为仍需通过 MockLink/SITL 验证 ACK、事件先后顺序、超时和拒绝路径；真实飞行只能由现场人员在拆桨或等效防护下执行。
