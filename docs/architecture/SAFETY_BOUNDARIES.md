# MERIVUS 安全边界

本文档定义不可越过的飞行安全边界，并记录当前代码中需要收敛的位置。

## 硬性规则

- LLM 只生成文本回复和结构化动作建议。
- LLM、MCP、Agent、云端服务不得直接发送任意 MAVLink。
- 真正执行动作必须经过 QGC 本地 C++ 白名单。
- 高风险命令必须要求用户明确确认。
- 第一版禁止自动解锁和自动起飞。
- 不允许模型自由写入 PX4 参数。
- 不允许给模型通用 `send_mavlink` 接口。
- 云端不直接持有无人机底层控制权。
- 失联保护最终由 PX4 执行。
- Agent 崩溃不得影响地图、遥测、Link 设置和手动控制。
- 视频阻塞不得影响 MAVLink 心跳。
- 不在真实无人机上自动运行测试。
- 测试默认使用 Mock、Unit Test、PX4 SITL 或 MAVLink 回放。
- 用户明确授权前，不修改真实飞控参数。
- 禁止在代码、QML、配置文件和 Git 中写入真实 API Key。
- 服务器 IP、端口、模型和密钥必须配置化。

## 第一版允许能力

- 查询无人机状态。
- 查询电量。
- 查询 GPS/RTK 状态。
- 查询连接状态。
- 解释错误日志。
- 选择无人机。
- 打开页面。
- 地图定位。
- 创建任务草稿。
- 航线安全分析。

## 第一版禁止能力

- 自动解锁。
- 自动起飞。
- 自动飞行。
- 任意参数写入。
- 任意 MAVLink 发送。
- 无确认返航。
- 无确认降落。
- 直接上传并启动真实任务。

## 从代码中确认的风险点

### AI 面板

文件：`custom/res/Merivus/MerivusAIAssistantPanel.qml`

- 当前 QML 默认路径不直接调用 Agent endpoint；Agent HTTP 由 C++ `AiAgentClient` 处理。
- 当前支持 `takeoff`、`land`、`rtl`、`pause` intent。
- 当前 AI 面板不会调用 `guidedModeTakeoff`、`guidedModeLand`、`guidedModeRTL` 或 `pauseVehicle`；飞行动作只显示为未执行建议。

风险：

- AI proposal 白名单已收敛到 C++ `AiCommandPolicy`；QML 只显示本地判定结果。
- 风险级别和基础审计已进入 C++；确认弹窗和执行前状态检查仍属于后续阶段。
- 当前 AI 路径符合“只输出建议，不执行飞行动作”的第一版安全目标。

### 指挥中心面板

文件：`custom/res/Merivus/CommandCenterOverlay.qml`

- 当前可直接对焦点飞行器下发高度、速度、爬升相关命令。
- 爬升命令使用 `sendCommand`。

风险：

- 命令入口分散在 UI 中。
- 缺少统一审计和风险确认策略。
- 后续 AI 不应复用这些入口绕过本地策略。

### SwarmController

文件：`custom/src/Swarm/SwarmController.cc`

- `executeGoto` 可调度多机 goto。
- `executeQueuedGoto` 可上传临时任务。
- 上传完成后可调用 `startMission`。
- `sendStartCommand` / legacy forwarding 存在 MAVLink `GPS_RAW_INT` 打包和发送逻辑。

风险：

- 多机任务上传和启动属于高风险能力。
- 直接 MAVLink 打包逻辑与“不得提供通用 MAVLink 接口给 AI/Agent”原则相关，需要隔离、命名和审计。
- 当前安全检查主要是 armed 和最低高度，不足以覆盖链路质量、模式、定位、地理围栏、用户确认、控制权等条件。

## 推荐收敛方向

- 在 `hotfix/safety-containment` 中先做临时安全封锁：AI 飞行动作只生成建议，不执行；`SwarmController` legacy forwarding 和临时任务自动启动均由默认关闭的开发开关保护。
- 阶段 1 先保护当前 TCP Link，可增加只读诊断，不改真实命令发送路径。
- 阶段 2 先把 AI 面板收敛为 Mock/只读/建议，不接真实飞行动作。
- 阶段 4 后 QML 不直接访问网络，统一经 C++ `AiAgentClient`。
- 阶段 6 建立 `ActionProposal`、schema 校验、本地风险重算和固定 command 枚举。
- 阶段 7 再做高风险命令确认框架，先使用 Mock Executor。
- `SwarmController` 中的真实任务上传、启动和 legacy MAVLink 逻辑应单独审计，必要时加 feature flag 或隐藏入口。

## 用户描述但尚未验证

- 无桨叶实机测试中起飞等简单操作会使电机响应。
- 基础 MAVLink 上下行链路已成立。

## 当前假设

- 后续真实飞行功能需要人工现场测试计划，不由 Codex 自动触发。
- 第一版 MVP 应偏向只读、显示、配置化、诊断和 Mock AI。

## 待确认事项

- 是否允许在近期分支中临时禁用 AI 起飞/降落/返航/暂停执行。
- 是否保留 `SwarmController` legacy forwarding 逻辑，还是先隔离为开发调试功能。
- 高风险命令确认弹窗的产品文案和验收标准。
- 操作审计日志写入位置、保留时间和导出策略。

## 阶段 6：AI 意图策略落地

`feat/ai-intent-policy` 已把 Agent proposal 纳入 C++ 本地策略：

- schema 失败、未知命令和危险结构均拒绝。
- 风险级别由 QGC 本地计算，不信任 Agent 自报风险。
- 高风险命令只显示为 `PreviewOnly` 或 `Deny`。
- `param.write` 和 `mavlink.send_raw` 强制拒绝。
- QML 只显示建议卡片，没有执行按钮。
- 审计记录进入 `merivus.ai.policy`，不记录 Token、完整用户消息或敏感凭据。

剩余风险仍包括非 AI 的指挥中心和 `SwarmController` 真实操作入口；这些不是本阶段新增能力，后续需要单独审计和确认框架。