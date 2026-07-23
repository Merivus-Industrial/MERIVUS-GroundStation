# 下一步路线

## P0：当前唯一优先任务

**收敛非 AI 高风险 QGC 执行入口，并建立 Mock/SITL 可重复回归基线。**

当前只处理以下已确认入口：

- `custom/res/Merivus/CommandCenterOverlay.qml`：高度、地速、爬升速度等直接 `Vehicle` 调用。
- `custom/res/Merivus/FlyViewMap.qml`：单点/队列 goto 调用。
- `custom/src/Swarm/SwarmController.*`：guided goto、临时任务上传/启动和 legacy MAVLink forwarding。

该任务不等于启用 AI 执行，也不授权连接真实飞机。

## 完成标准

1. 形成逐入口清单：操作者动作、目标来源、底层 API/MAVLink、前置条件、确认、ACK/结果、超时和失败处理。
2. 高风险与 legacy 路径默认关闭或必须显式启用；状态对用户可见，不能靠隐藏变量或文案保证。
3. 下发前冻结 vehicle/system id 集合，避免焦点切换改变目标；多机部分失败可逐机追踪。
4. 统一检查 Link/遥测新鲜度、唯一 system id、位置/GPS、模式/能力、解锁状态、电池、geofence/mission 约束；无法确认时安全拒绝。
5. 确认界面明确显示命令、冻结目标、关键参数和风险；取消/超时不得发送。
6. 审计区分 requested、validated、confirmed、queued/sent、ACK/rejected、executing/completed，不把“已发送”写成“已完成”。
7. 为纯策略/目标冻结增加局部测试；使用 Mock Link 或 PX4 SITL 覆盖单机、多机、断链、重复 id、无位置、部分失败和取消。
8. 无 Agent、无 Ollama、无外网时，QGC 人工功能按设计降级；不得新增 AI 到执行层的引用。

## 需要修改的模块

- 上述 3 个入口及其最小必要的 QGC C++ 安全协调层。
- `custom/tests/` 或独立局部测试工程。
- 与该任务直接相关的安全说明、测试矩阵和 smoke checklist。

实现前先使用 [`safety-review`](../templates/safety-review.md) 和 [`feature-brief`](../templates/feature-brief.md) 固定范围与证据。

## 不应修改的模块

- `agent/`、Provider、Normalizer 和模型 prompt。
- AI Command Executor、云 Provider、MCP、Device Gateway、GIS、Media Service。
- PX4 源码与真实 PX4/RTK 参数。
- QGC 上游无关 UI、第三方依赖或构建系统重构。

## 最小验证范围

1. 静态交叉引用与 `git diff --check`。
2. 新增局部单元测试/策略测试。
3. QML 资源/语法的最小构建检查（环境具备时）。
4. Mock Link 回归；其后才是 PX4 SITL 单机与多机故障场景。
5. 明确记录未执行的完整构建、硬件和真实飞行验证。

## 主要风险

- 误改 QGC 原生人工控制语义，导致现有操作退化。
- 多机目标在确认与发送之间变化。
- legacy raw MAVLink 路径与新策略并存形成旁路。
- 把 UI 确认、发送成功或 MAVLink ACK 误写成动作完成。
- SITL 环境差异造成不可重复结果。

## 后续候选任务

| 优先级 | 任务 | 启动条件 |
| --- | --- | --- |
| P1 | 干净 Windows 可重复构建与 onedir 发布包验证 | P0 安全基线稳定；准备干净 VM，不自动安装依赖 |
| P1 | Local Agent loopback/出网状态语义与 capability 契约统一 | 独立 Agent 分支；只做本机网络边界和跨层契约测试 |
| P1 | 当前 HEAD 的 Mock QGC-Agent GUI 生命周期 smoke | 有可用 staging；不启 Ollama、不接飞机 |
| P2 | 固定版本 `qwen3:8b` 的真实本地模型评估 | 人工确认本机 Ollama/模型版本；仅本机且显式 opt-in |
| P2 | 修订过时的 AI Assistant、目标架构和 Agent README | 与代码结论稳定后单独做纯文档任务 |
| P2 | AI 会话保留与敏感信息策略 | 先定义数据最小化、删除和加密要求 |

## 暂缓

- AI 用户确认器、Command Executor 和任何真实飞行动作接入。
- 云 Provider、MCP、Device Gateway、Cloud API、数据库、Web Console、Media Service。
- `WaypointSafetyService` 生产实现和 3D Tiles 碰撞结论。
- 自动修改 PX4/RTK 参数、自动连接厂商生产服务或真实飞机。
- 安装包签名、自动升级和生产发布，直至可重复构建与安全回归建立。