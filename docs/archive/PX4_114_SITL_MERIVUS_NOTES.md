# PX4 v1.14 SITL 与 MERIVUS 地面站说明

## 本次地面站改动

- `custom/res/Merivus/CommandCenterOverlay.qml`
  - 统一左侧上排三个控制卡片：高度、地速、爬升速度。
  - 三个控制卡片都包含实时数据、命令输入框、`+/-` 按钮和下发按钮。
  - 爬升速度通过 `Vehicle.sendCommand` 下发 `MAV_CMD_DO_CHANGE_SPEED`，其中 speed type 为 `2`。
  - 下排航向、电池、GPS 卡片已压缩尺寸，使上下两排布局更一致。
  - 地图目标摘要在选中单机或队列时更醒目。
  - 无人机列表右侧“清除”按钮改为圆角紧凑样式。
  - 右侧姿态数据目标选择改为复选框样式，同一时刻仍只显示一架无人机的数据。

- `custom/res/Merivus/FlyViewMap.qml`
  - 左键拖动框选动画改为 MERIVUS/QGC 主题色，带透明渐变填充、圆角边框和柔和淡出。
  - 右键指点飞行反馈改为航点圆点、短竖线、光晕和双层水波纹动画。
  - `Shift + 右键` 队列航点保留琥珀色，普通指点飞行跟随系统主题色。

- `src/AutoPilotPlugins/PX4/SafetyComponent.qml`
  - 将自定义 GNSS/视觉定位故障保护卡片缩减为 PX4 参数驱动的导航丢失设置。
  - 导航丢失动作绑定 `COM_POSCTL_NAVL`，延迟绑定 `COM_POS_FS_DELAY`。
  - 将 IoT/TCP 卡片缩减为 MERIVUS 侧 TCP 检测入口，并保留 PX4 MAVLink 数据链路故障保护参数。
  - MAVLink 数据链路丢失动作绑定 `NAV_DLL_ACT`，超时绑定 `COM_DL_LOSS_T`。
  - 删除新增安全卡片中无实际后端的数据源、动作和恢复策略占位下拉框。

## PX4 v1.14 与 SITL 侧需要处理的内容

PX4 v1.14 没有真正意义上的“禁用摇杆/杆量输入”模式，也就是较新版本中类似 `COM_RC_IN_MODE=4` 的行为。v1.14 中 `COM_RC_IN_MODE=1` 主要是禁用传统 RC 检查，但 commander 仍可能要求存在 MAVLink manual-control/joystick 输入，才允许进入 Mission 模式。因此，当 MERIVUS 禁用虚拟摇杆且没有发送 manual-control 输入时，Mission 模式仍可能被拒绝，并提示 `No manual control input`。

建议采用以下方案之一：

1. 继续使用 PX4 v1.14，但由 MERIVUS 或一个 SITL companion 进程发送 MAVLink `MANUAL_CONTROL` 心跳/输入，用它代表地面站鼠标控制替代虚拟摇杆后的“人工控制链路”。
2. 回移植较新 PX4 的“禁用杆量输入”逻辑到 v1.14 的 commander/manual-control 检查中，然后在 SITL 中设置该模式。
3. 升级 PX4 到支持禁用杆量输入 `COM_RC_IN_MODE` 取值的版本，再在 SITL 中设置对应取值。

建议检查或修改的 PX4 位置：

- `ROMFS/px4fmu_common/init.d-posix/rcS`
  - SITL 参数覆盖应放在所选 airframe 默认参数之后，否则可能被 airframe 脚本覆盖。
  - 每次启动后用 `param show COM_RC_IN_MODE` 验证实际运行值。

- PX4 commander 中 `COM_RC_IN_MODE` 的参数定义
  - 只有在明确回移植“禁用杆量输入”行为时，才扩展可接受取值。

- PX4 commander/manual-control 预检逻辑
  - 如果回移植，应只在新增的禁用杆量输入模式下跳过 manual-control 存在性检查。
  - 不要对普通 RC 或 joystick 模式跳过该检查。

- SITL 回归测试相关故障保护参数
  - `COM_POSCTL_NAVL`：位置控制导航丢失后的响应动作。
  - `COM_POS_FS_DELAY`：导航丢失延迟。
  - `NAV_DLL_ACT`：MAVLink 数据链路丢失动作，需要返航时设置为 Return。
  - `COM_DL_LOSS_T`：MAVLink 数据链路丢失超时。

## 安全卡片数据判定规则

- 导航/GNSS 丢失应以 PX4 估计器有效性为准，不应由 MERIVUS 仅凭地图状态猜测。
  - 故障动作和延迟使用 PX4 参数 `COM_POSCTL_NAVL` 与 `COM_POS_FS_DELAY`。
  - 地面站只显示遥测状态：GPS 卫星数、全局/局部位置有效性、EKF 健康状态、当前导航状态。
  - 如果 PX4 已报告导航丢失且配置动作为 Return，MERIVUS 应显示故障并避免继续发送冲突的任务或指点飞行指令。

- 云端 IoT TCP 丢失属于 MERIVUS 侧链路故障，此时 MAVLink 可能仍然可用。
  - 判定条件：`当前时间 - 最近一次云端 TCP 心跳时间 > 配置的 TCP 超时时间`。
  - 页面显示数据：TCP 连接/断开状态、最近心跳距今时间、配置超时值、当前处置状态。
  - 处置动作：只要 MAVLink 仍连接，就由 MERIVUS 通过 MAVLink 下发 Return；如果 MAVLink 也断开，则必须由 PX4 机载参数 `NAV_DLL_ACT`/`COM_DL_LOSS_T` 兜底。

- 多航点任务卡在两个航点之间时，应先修复 RC/manual-control 入口条件，再继续排查航点任务本身。
  - 先确认 Mission 模式不再因为 `No manual control input` 被拒绝。
  - 再确认上传/开始临时任务前，全局位置估计有效。
  - 如果卡住后再次下发指令提示安全条件不满足，应检查导航有效性、地理围栏、电池故障保护、数据链路丢失和当前导航状态。

## 验证清单

- 启动 PX4 SITL 后，用 `param show COM_RC_IN_MODE` 确认运行时参数值。
- 在评估航点行为前，确认 Mission 模式不再提示 `No manual control input`。
- 上传临时任务前，确认全局位置估计有效。
- 分别测试单次右键指点飞行和 `Shift + 右键` 队列航线。
- TCP/MAVLink 丢失测试时要区分两类链路：云端 TCP 丢失由 MERIVUS 在 MAVLink 可用时处理，MAVLink 数据链路丢失由 PX4 机载故障保护参数处理。
