# 当前真实状态

## Git 与阶段

| 项目 | 状态 |
| --- | --- |
| 更新时间 | 2026-07-20 |
| 文档分支 | `docs/project-handoff` |
| 审计代码基线 | `a40c9e4`（`v0.1.0-dev.1`，同时为审计时 `main` / `origin/main`） |
| 工作区 | 开始审计时已有未跟踪的 `docs/handoff/` 草稿；无其他已跟踪或无关修改，草稿经复核后继续补齐 |
| 当前阶段 | `0.1.0-dev.1` 开发测试初版已收口，进入安全收敛与可重复验证阶段 |
| 当前唯一优先任务 | 非 AI 高风险 QGC 执行入口的统一安全收敛与 Mock/SITL 回归基线 |

## 已完成且本次有验证证据

| 模块 | 相关文件/目录 | 对应提交 | 本次验证 | 尚未验证 |
| --- | --- | --- | --- | --- |
| Python Agent、Schema、Mock/Ollama 适配与 Normalizer | [`agent/app`](../../agent/app/)、[`agent/tests`](../../agent/tests/) | `e894666`（squash 集成） | 2026-07-20：`63 passed, 6 warnings`（2.22 秒）；Ollama 测试使用 mock transport | 当前真实 Ollama 服务/模型输出、长期运行 |
| JSON Schema 与示例 JSON | [`schemas`](../../schemas/)、[`configs`](../../configs/) | `e894666` | 5 个 JSON 文件由 PowerShell `ConvertFrom-Json` 解析通过 | JSON Schema 语义未用独立校验器复跑；YAML 只做静态存在检查 |
| Windows 工具链可定位 | [`tools/dev/check-windows-environment.ps1`](../../tools/dev/check-windows-environment.ps1) | `e894666` | Qt 5.15.2、MSVC、Python 3.11.9、Git 检查通过 | 没有进行本次完整编译 |
| AI 不接真实执行层 | [`custom/src/Ai`](../../custom/src/Ai/)、[`MerivusAIAssistantPanel.qml`](../../custom/res/Merivus/MerivusAIAssistantPanel.qml) | `e894666`、`a40c9e4` | 静态交叉引用：QML 只展示 proposal；未发现 AI 到 Vehicle/MAVLink/Swarm/PX4 的执行调用 | 仍需构建后 GUI/运行时回归 |

## 已实现但未完整验证

| 模块 | 当前代码事实 | 最近证据 | 本次未验证内容 |
| --- | --- | --- | --- |
| QML AI Assistant 面板 | 已注册 `AiAgentClient`/`AiServiceSupervisor`；无活动 `XMLHttpRequest`；建议卡无执行按钮 | `a40c9e4`；[阶段发布记录](../releases/0.1.0-dev.1.md) | 当前 HEAD 的 QML 资源生成、GUI 行为、尺寸持久化 |
| C++ `AiAgentClient` | 异步调用 health/info/chat；响应后执行 `AiSchemaValidator` 与 `AiCommandPolicy` | `e894666`；历史 Policy 测试 `27/0` 见[整理报告](../development/REPO_CLEANUP_REPORT.md) | 本次未编译、未运行 C++ 测试或 HTTP 联调 |
| `AiServiceSupervisor` | 管理 QProcess、健康检查、重启、端口冲突和内存 Token；优先打包 Agent | `e894666`、`a40c9e4` | 当前 GUI 生命周期、退出释放端口、崩溃重启 |
| Ollama / `qwen3:8b` | Provider、路由、配置与模型评估脚本存在；QGC 设置会把 URL 收敛到 loopback | `e894666`；相关阶段文档 | 2026-07-20 Ollama 未运行，未执行真实模型评估 |
| PyInstaller onedir | spec 和 `build-agent.ps1` 存在；历史 POC 有打包/HTTP 证据 | `e894666`；[Packaging POC](../architecture/AGENT_PACKAGING_POC.md) | 本次未打包、未验证干净电脑/签名安装包 |
| QGC Release 构建 | qmake/VS 脚本存在；历史记录称增量构建通过 | `a40c9e4`；[版本说明](../releases/0.1.0-dev.1.md) | 本次未完整/增量构建，未做干净环境构建 |

## 正在进行

- 本分支正在建立交接文档，不修改业务实现。
- 产品代码没有可由当前分支确认的其他“正在进行”功能；历史 roadmap 中的“进行中”不能直接视为当前在开发。

## 尚未开始或仅规划

- AI 用户确认框架与 Command Executor。
- 云 Provider、MCP、Device Gateway、Cloud API、Web Console、数据库、Media Service。
- `WaypointSafetyService` / GIS Safety Service 生产代码。
- 生产鉴权、控制权租约、安装包签名、升级和发布流水线。
- 当前可复现的真实 4G/RTK 多机长期测试与真实飞机端到端测试。

## 已暂停 / 暂缓

- 自建云平台、设备网关、Media/GIS 服务和真实 AI 执行均应暂缓，先完成当前 P0 安全收敛。
- 仓库中未找到能够确认“某个当前分支正在暂停等待恢复”的活动状态；远端仅保留 `main`。

## 已否决或被替代

- QML 直接发 Agent HTTP：已被 C++ `AiAgentClient` 替代。
- QML/仓库保存 API Key：安全决策明确否决。
- 旧 `intent` 直接动作描述：当前 Agent 契约以 `reply + proposal` 为主，QGC 本地重算策略。
- Agent `onefile` 打包：当前 POC 选择 onedir；onefile 未采用。
- `docs/archive` 中旧环境/构建/SITL 文件：已被 `docs/development` 与当前架构文档替代为主入口，保留历史参考。

## 当前阻塞与待确认

- 生产/实机能力被安全验证、确认/审计框架、SITL 回归和人工授权阻塞；这不是本次文档任务的执行许可。
- `QGroundControl 4.3` 精确上游版本在仓库中无独立证据，待负责人补充来源。
- [Agent Supervisor](../architecture/AGENT_SUPERVISOR.md) 称做过打包 GUI 冒烟，而 [仓库整理报告](../development/REPO_CLEANUP_REPORT.md) 的 2026-07-14 结果称未执行 GUI 人工 smoke；需要原始测试记录确认。
- [`docs/MERIVUS_AI_ASSISTANT.md`](../MERIVUS_AI_ASSISTANT.md) 仍称飞行动作“用户确认后调用 `Vehicle`”，并使用旧 `intent` 响应示例；当前 [`MerivusAIAssistantPanel.qml`](../../custom/res/Merivus/MerivusAIAssistantPanel.qml) 的确认函数只输出“建议未执行”，当前 Agent 契约为 `reply + proposal`。以代码为准。
- [目标架构](../architecture/TARGET_ARCHITECTURE.md) 的“从代码中确认/待确认”仍写 QML 直连、Token 待定；当前已经由 C++ Client/Supervisor 与内存 Token 替代。
- `agent/README.md` 的“仅 Mock / Token 传递未实现”已被当前代码替代，应视为部分过时。
- 直接通过环境变量启动 Agent 时，Python 侧除仅将 `0.0.0.0` 收敛为 loopback 外，未拒绝其他监听地址；`MERIVUS_OLLAMA_BASE_URL` 也未像 QGC Supervisor 一样强制 loopback，却仍报告 `external_network_enabled=false`。生产化前应统一约束与状态语义。
- `agent/app/config.py` 的 `DEFAULT_CAPABILITIES` 含 `vehicle.query_gps`、`vehicle.hold`、`vehicle.change_mode`，与 QML/C++ 当前枚举并不完全一致；当前聊天请求使用 QML 自己的 capability 列表，生产化前仍需统一契约。

## 无法从仓库确认

- 上游 QGroundControl 的精确 release/commit 是否为 4.3。
- 历史 GUI smoke 的原始截图、操作记录和执行环境。
- 历史真实 `qwen3:8b` 指标是否可在当前机器和当前模型版本复现。
- 当前 4G/RTK 实物配置、链路长期稳定性和真实飞机端到端结果。
- 签名安装包、升级/卸载和干净电脑发布验证。
