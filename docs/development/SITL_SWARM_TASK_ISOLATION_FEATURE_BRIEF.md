# SITL 编队恢复与实时任务隔离功能说明

## Outcome

恢复 QGC 原有的 SITL 编队触发语义：UAV-1 为固定主机，UAV-2～UAV-6 为从机，点击“编队”只触发既有 swarm 链路，不读取或启动任意机载 Mission。同时保证普通右键指点和 Shift 多航点任务按车辆隔离，不因切换焦点继承其他车辆的任务。

## Operator workflow

1. 启动 QGC 前显式设置 `MERIVUS_DEV_ENABLE_SWARM_LEGACY_FORWARDING=1`；需要执行 Shift 临时 Mission 时另设 `MERIVUS_DEV_ENABLE_SWARM_AUTO_START_MISSION=1`。
2. 启动 UAV-1：`make px4_sitl_default gazebo`。
3. 启动五架从机：`./Tools/simulation/gazebo-classic/sitl_multiple_run.sh -m iris -n 5`。
4. 在地图框选 UAV-1～UAV-6，确认界面核对固定主从关系后启动编队。
5. 普通点击车辆会排他选择该机；普通右键立即更新当前选择的目标点。
6. Shift+右键第一个航点会冻结目标 ID 和几何关系；释放 Shift 后确认任务。目标中已有任务时，确认内容会明确说明替换。
7. 临时任务到达末点并悬停后，QGC 请求清除该机临时 Mission；未收到清除 ACK 时标记为残留任务。

## Layer and owners

- Hardware: 不在本次范围。
- LTE/RTK: 不在本次范围。
- QGC C++: `custom/src/Swarm/SwarmController.*` 负责 SITL 开关、固定目标校验、主机位置转发、临时任务逐机状态和清理。
- QML: `GuidedActionsController.qml`、`CommandCenterOverlay.qml`、`FlyViewMap.qml`、`PlanView.qml` 负责确认、选择冻结、任务显示和禁止跨机继承。
- MAVLink: 沿用既有 SITL `GPS_RAW_INT` 启动/转发兼容协议；不作为实机生产协议。
- Cloud/AI: 不接入。

## Safety impact

- Worst credible failure: 旧目标集合残留导致指令串机；错误启动旧 Mission；主机数据失效后从机继续盲飞。
- Required interlocks: 两个开发环境变量默认关闭；编队目标必须恰为 UAV-1～UAV-6；重复编队拒绝；主机遥测超时后在线从机进入悬停。
- Human confirmation: 编队启动、编队结束、Shift 临时任务和任务替换均需确认；普通右键单点按 MOBA 语义即时执行。
- Audit evidence: UI 结果区分已调度与已跳过；Mission 清理失败会明确提示残留。

## Scope

- Allowed files/components: `custom/src/Swarm/`、相关 MERIVUS Fly/Guided QML、`src/PlanView/PlanView.qml`、开发测试与说明。
- Prohibited files/components: `agent/`、AI Executor、PX4 源码、真实飞行参数、云服务。
- Upstream QGC impact: 仅收紧 Plan View 切换车辆时的草稿处理；连接车辆之间不再允许把上一架的编辑草稿保留到新车辆上下文。

## States and failures

- Empty/loading/stale/offline: 无选择或坐标无效时拒绝；断链目标不发送；清除失败标记 stale。
- Timeout/cancel/retry: 主机位置超过 3 秒未更新时停止转发并命令在线从机悬停；Shift 草稿在切换选择时取消。
- Partial success: 普通多机指令可列出跳过目标；固定六机编队要求整组通过，不允许部分启动。
- Recovery: 活动编队通过“结束编队”停止；残留临时 Mission 在下一次替换确认后覆盖。

## Acceptance criteria

- “编队”不再调用 `executeStartMissions`，只调用带冻结目标的 `sendStartCommand`。
- 编队仅在显式 SITL 开关开启且目标严格为 1～6 时可启动。
- UAV-1 位置只转发给本次冻结的 UAV-2～UAV-6，不回发主机或其他连接车辆。
- 普通点击另一架机后，上一架的实时任务继续执行，但新右键指令只发给新焦点机。
- Shift 草稿在首点冻结目标和参考位置；目标切换会取消草稿。
- 同一车辆已有临时任务时，新的 Shift 队列必须确认替换；普通右键可即时抢占。
- 临时 Mission 完成后请求清除，旧路线不再作为当前任务恢复显示。

## Verification plan

- 执行 `tools/dev/test-sitl-swarm-task-isolation.ps1` 和 `git diff --check`。
- 环境具备时进行 QML/C++ 局部构建。
- 在 PX4 SITL 覆盖固定六机编队、错误 ID、缺机、重复启动、主机断链、从机断链、跨机下达任务、Shift 替换和 Mission 清理失败。
- 不连接真实飞机。
