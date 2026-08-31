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

## 高度、速度与最低输出的控制边界

- `MAV_CMD_NAV_TAKEOFF` 的 `param7` 是 AMSL 目标高度；命令本身没有“起飞速度”字段。地面站将界面中的相对起飞高度换算为 AMSL 后发送。
- PX4 多旋翼自动起飞爬升速度由 `MPC_TKO_SPEED` 控制。指挥中心的“起飞速度”按钮直接写入该参数；不得再用 `MAV_CMD_DO_CHANGE_SPEED` 冒充垂直起飞速度命令。
- `MPC_THR_MIN` 是自动推力控制的最小集体推力，`MPC_MANTHR_MIN` 是手动/稳定模式最低油门，`DSHOT_MIN` 是解锁后 DShot 最低输出。三者职责不同，不能用来指定 MAVLink 引导起飞速度。
- 无桨执行完整起飞指令时，飞控检测不到高度上升，会持续提高推力追踪目标高度。因此无桨测试只能验证命令、解锁和停止链路，不能据此判断真实起飞速度或悬停油门；发现电机迅速升速时应立即停止。

## 可重复检查

Windows PowerShell：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/dev/test-takeoff-health-contract.ps1
```

该检查验证上述状态机边界是否仍由唯一责任方维护。完整行为仍需通过 MockLink/SITL 验证 ACK、事件先后顺序、超时和拒绝路径；真实飞行只能由现场人员在拆桨或等效防护下执行。
