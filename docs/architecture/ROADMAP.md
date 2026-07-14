# MERIVUS 分支路线图

原则：一个分支只完成一个清晰目标；每个分支必须可单独构建、可回滚、可验收。未经用户确认，不进入下一阶段。

## 阶段 0：基线与架构

分支：`docs/project-audit`
状态：本次执行

任务：

- 检查现有代码。
- 记录当前模块。
- 完成架构文档。
- 建立风险清单。
- 建立接口边界。
- 给出 MVP 定义。
- 不修改业务逻辑。

验收：

- 文档完整。
- 当前状态和目标状态清楚。
- 未验证内容明确标记。
- Release 增量构建通过。
- 在任何 RTK 接线或配置操作前，必须完成硬件版本、输入电压、串口电平、波特率和飞控接口确认。
- Codex 不得自动修改真实硬件参数。

## 前置热修：临时安全封锁

建议分支：`hotfix/safety-containment`

目标：仅临时收敛当前高风险入口，不建立完整安全框架。

任务：

- AI 面板中的 `takeoff`、`land`、`rtl`、`pause` 改为只显示建议，不调用 `Vehicle` 执行。
- 保留 UI 和聊天功能。
- 不删除已有代码，可通过明确的 feature flag 隔离。
- feature flag 默认关闭真实 AI 飞行动作。
- `SwarmController` 的 legacy MAVLink forwarding 增加开发开关，默认关闭。
- 临时任务上传后自动 `startMission` 增加开发开关，默认关闭。
- 原生 QGC 手动操作和 Link 功能不受影响。
- 不进行大范围重构。

验收：

- AI 输入飞行动作时只产生“未执行的建议”。
- 不调用 `guidedModeTakeoff`、`guidedModeLand`、`guidedModeRTL`、`pauseVehicle`。
- 未显式启用开发开关时，不发送 legacy forwarding 消息。
- 未显式启用开发开关时，不自动 `startMission`。
- Release 构建通过。
- 只允许 Mock 或 SITL 验证。
- 不进行真实飞机测试。

依赖：阶段 0。建议在阶段 1 前完成。

## 阶段 1：保护当前可用链路

建议分支：`test/current-link-baseline`

状态：completed

任务：

- 记录当前 TCP Link 配置方式。
- 将 `119.45.168.211` 和真实端口视为外部配置，不写死进业务代码。
- 不破坏原生 QGC Link 功能。
- 建立 Link 状态只读诊断。
- 增加连接、断开、重连和错误日志。
- 不自建服务器。
- 不改变真实指令发送方式。

验收：

- 原有 TCP 连接仍可使用。
- 未连接时不崩溃。
- 错误信息可读。
- IP 和端口没有散落硬编码。

依赖：阶段 0。

## 阶段 2：AI 界面模型整理

建议分支：`feat/ai-panel-foundation`

状态：当前已有 AI 面板基础 UI 和安全封锁；尚未作为独立整理分支完成。`feat/local-agent-http` 不修改该 UI。

任务：

- 整理现有 AI 面板状态。
- 保持 MERIVUS 当前风格。
- 建立消息列表、发送、等待、错误和离线状态。
- 使用 Mock 回复。
- 暂不连接真实模型。
- 暂不执行飞行命令。

验收：

- QML 界面不卡顿。
- 无 Agent 时可显示离线。
- 不包含 API Key。
- 不修改 `Vehicle` 执行逻辑。

依赖：阶段 0。建议在阶段 1 之后做，避免先扩大 AI 风险面。

## 阶段 3：本机 Agent 最小链路

建议分支：`feat/local-agent-http`

状态：completed

任务：

- 建立独立 `agent/` 目录。
- 建立 Python/FastAPI 最小服务。
- 提供 `/health`、`/merivus/info` 和 `/merivus/agent`。
- 第一版返回 Mock JSON。
- 定义请求和响应 JSON Schema。
- Agent 不访问 MAVLink、PX4 或真实云模型。
- 不修改 QGC C++ 网络客户端，不增加 QProcess，不接真实大模型。

验收：

- 可手动启动 Agent。
- health 正常。
- POST 返回固定 JSON。
- 单元测试通过。
- 高风险动作只返回 proposal，不执行、不声称成功。
- capability 未授权时不生成 proposal。
- `external_network_enabled=false`、`flight_execution_enabled=false`。

依赖：阶段 0、临时安全封锁、阶段 1。QGC UI 契约收敛将在阶段 4 继续处理。

## 阶段 4：QGC 与 Agent 通信

建议分支：`feat/qgc-agent-client`

状态：completed

任务：

- 实现 C++ `AiAgentClient`。
- 使用 `QNetworkAccessManager` 异步请求。
- QML 不直接访问网络。
- 解析 `reply` 和 `proposal`。
- 增加请求 ID、session ID、超时、取消和错误状态。
- 只显示回复，不执行 proposal。
- Agent 仍由开发者手动启动，不使用 `QProcess`。

验收：

- QGC 能与 Mock Agent 通信。
- Agent 离线时飞控主功能正常。
- 不阻塞 UI。
- JSON 无效时安全拒绝。
- 不接真实模型、不执行飞行动作、不修改 PX4。

依赖：阶段 3。

## 阶段 5：Agent 进程监管

建议分支：`feat/agent-supervisor`

状态：completed

任务：

- 实现 `AiServiceSupervisor`。
- 使用普通 `QProcess` 启动 Agent，不使用 `startDetached`。
- 发布模式 Agent 位于 `applicationDirPath()/agent/merivus-agent.exe`。
- 开发模式仅在 `MERIVUS_AGENT_DEV_PYTHON` 和 `MERIVUS_AGENT_DEV_ROOT` 显式配置后启用。
- 启动前检查 `/health`，复用兼容外部 Agent，识别不兼容端口冲突。
- 启动后轮询 `/health`，运行期定期检查。
- QGC 退出时只关闭自己启动的 Agent。
- 使用内存本地 Token 保护 Supervisor 启动的 POST 聊天请求。
- 不使用开发机绝对路径。

验收：

- 发布包放置 `agent/merivus-agent.exe` 后可由地面站启动 Agent。
- 未安装 Agent 时显示清晰状态，QGC 继续可用。
- Agent 崩溃不影响 QGC，并受最多 2 次自动重启限制。
- 安装路径变化后仍按 `applicationDirPath()` 查找 Agent。
- Token 不写入配置、日志或 Git。

未完成或后续验证：

- 最终 Windows 打包尚未完成。
- GUI 全流程仍需在可交互环境人工冒烟验证。
- 日志写入用户可写目录属于发布打包阶段继续确认。

依赖：阶段 3、4。

## 阶段 5.5：Agent 发布打包 POC

建议分支：`release/agent-packaging-poc`

状态：completed

任务：

- 使用 PyInstaller onedir 打包 `agent/` 中的 Python Mock Agent。
- 输出 `merivus-agent.exe`。
- 新增 `agent/merivus-agent.spec` 和 `tools/dev/build-agent.ps1`。
- 复制完整 onedir 输出到 QGC Release `staging/agent/`。
- 验证 `staging/agent/merivus-agent.exe` 可直接运行并返回 health。
- 不提交 `agent/build/`、`agent/dist/`、QGC build、staging、Token、`.env`、厂商 PDF 或日志。

验收：

- `GET /health`、`GET /merivus/info`、`POST /merivus/agent` 通过。
- Token 正反验证通过。
- 起飞请求只返回未执行 proposal。
- 输出目录不包含源码、虚拟环境、PDF、密钥或模型文件。
- Supervisor 发布路径保持 `applicationDirPath()/agent/merivus-agent.exe`。

未完成或后续验证：

- GUI 全流程需要人工在 `staging/MERIVUS.exe` 中验证。
- 干净 Windows 电脑仍需验证。
- 正式安装包、签名、AppData 日志目录和升级策略仍属于后续发布阶段。

依赖：阶段 5。

## 阶段 6：结构化意图与安全白名单

建议分支：`feat/ai-intent-policy`

状态：completed

任务：

- 定义 `ActionProposal`。
- 定义固定 command 枚举。
- 实现 Schema validator 和 `AiCommandPolicy`。
- 第一版只支持只读或 UI 操作。
- 未知命令一律拒绝。
- 风险级别由本地规则确定。

验收：

- 未知命令无法执行。
- 参数缺失时拒绝。
- 模型文本不能直接成为 MAVLink。
- 有审计记录。

依赖：阶段 4。

## 阶段 7：高风险命令确认框架

建议分支：`feat/command-confirmation`

任务：

- 建立 `AiCommandExecutor`。
- 建立确认弹窗。
- 建立执行前状态检查。
- 先使用 Mock Executor。
- 为 Hold、RTL、Land 预留接口。
- 自动解锁和自动起飞继续禁止。

验收：

- 无确认不执行。
- 取消后不执行。
- 飞行器断开时拒绝。
- 所有尝试有日志。

依赖：阶段 6。

## 阶段 8：RTK 状态集成

建议分支：`feat/rtk-status-integration`

任务：

- 在任何 RTK 接线或配置操作前，必须完成硬件版本、输入电压、串口电平、波特率和飞控接口确认。
- Codex 不得自动修改真实硬件参数。
- 读取 QGC 已有 GPS/RTK 状态。
- 展示 Fix 类型、卫星数、精度和航向状态。
- 不由 QGC 重新配置 RTK 硬件。
- Hyper982 配置只做文档和显示，不随意写入飞控。

验收：

- 无 RTK 时正常降级。
- RTK 与普通 GPS 状态区分。
- 不影响原生 GPS 逻辑。

依赖：阶段 1。

## 阶段 9：设备网关 POC

建议分支：`poc/device-gateway`

任务：

- 建立最小 TCP Server。
- 支持多个模拟设备连接。
- 使用模拟 MAVLink 或回放数据。
- 建立设备 ID、心跳、连接和断开日志。
- 不替换厂商服务器。
- 不开放未认证公网端口。

验收：

- 多个模拟设备可连接。
- 数据不串机。
- 断线可识别。
- 未认证连接被拒绝。

依赖：阶段 1。建议独立 `backend/` 或独立仓库。

## 阶段 10：云端基础

建议分支：`feat/cloud-foundation`

任务：

- 用户认证、组织、角色和权限。
- 设备注册和绑定。
- PostgreSQL 基础模型。
- 审计日志。

验收：

- 用户只能访问授权设备。
- 控制权限和查看权限分开。
- 数据库迁移可重复执行。

依赖：设备网关边界确认后再启动。

## 阶段 11：遥测记录与多机状态

建议分支：`feat/telemetry-pipeline`

任务：

- 定义统一遥测事件。
- 保存关键遥测和最新状态。
- 明确实时数据和历史数据边界。
- 不阻塞控制链路写数据库。

验收：

- 多机遥测不串线。
- 网络中断后状态可恢复。
- 可查询历史轨迹。

依赖：阶段 9、10。

## 阶段 12：视频链路

建议分支：`feat/video-pipeline`

任务：

- 记录 RTSP 地址配置方式。
- 视频端口配置化。
- 复用 QGC 原生视频能力。
- 控制链路和视频链路独立。
- 增加无视频、重连和超时状态。

验收：

- 视频断开时飞控链路正常。
- RTSP 地址不硬编码。
- 多机视频可绑定飞行器。

依赖：阶段 1。

## 阶段 13：GIS 安全服务

建议分支：`feat/gis-safety-service`

任务：

- 建立 `WaypointSafetyService` 接口。
- 第一版只做离线或服务端分析。
- 输出地形、建筑、禁飞区冲突。
- 不自动修改任务。

验收：

- 输入输出 Schema 固定。
- 数据缺失时明确告警。
- 未知区域不判定为安全。

依赖：阶段 6 的 proposal 契约。

## 阶段 14：模型 Provider

建议分支：`feat/agent-model-providers`

状态：not started

任务：

- Agent 实现 Provider router。
- 支持 Mock、本地模型、一个云 API。
- API Key 不写入代码。
- Provider 输出统一格式并经 Schema 校验。

验收：

- 无模型时返回清晰错误。
- 模型切换不修改 QML。
- 输出格式统一。

依赖：阶段 3、6。

## 阶段 15：Windows 发布打包

建议分支：`release/windows-packaging`

任务：

- 构建 `MerivusGroundControl.exe` 或确认现有 `MERIVUS.exe` 命名。
- 收集 Qt 运行库。
- 打包 `agent/merivus-agent.exe`。
- 配置和日志写入 AppData。
- 不把源码、虚拟环境、API Key、模型文件放入基础包。

验收：

- 无开发环境电脑可启动。
- 不依赖源码路径。
- Agent 可启动。
- AI 关闭时地面站正常使用。

依赖：阶段 5、14。

## 推荐 MVP

三个月内合理 MVP：

- 保持当前 QGC 基础飞控和 TCP Link 可用。
- Link 诊断和配置基线。
- AI 面板只读/Mock/建议模式。
- 本机 Mock Agent。
- QGC C++ Agent client。
- 本地 proposal schema 和只读/低风险白名单。
- RTK 状态显示。
- 视频 RTSP 配置化与失败降级。
- 文档化硬件接入和 Windows 发布路径。

暂缓内容：

- 自建完整云服务器替代厂商服务。
- 真实多机自动任务调度。
- AI 直接执行起飞、降落、返航。
- 机载电脑、ROS、YOLO、视觉避障。
- 商业计费、完整 Web Console、大规模遥测数据平台。

## 构建稳定性补记：feat/ai-intent-policy

阶段 6 的 AI 意图策略功能保持 completed。本次仅补齐 Release 构建稳定性：Qt 5.15.2 / MSVC 的生成 Makefile 在大型 QGC 工程中会形成超长 `cl.exe` 命令行，`jom` 可能以 `-1073740791` 崩溃并误导到资源或 qmlcache 阶段。构建脚本现在在 qmake 后为 MSVC 编译 flags 生成响应文件，并使用 `nmake` 执行 Release 构建，避免修改 Qt 安装目录或关闭 Quick Compiler。

下一阶段仍建议进入 `feat/command-confirmation` 或继续细化本地安全策略；模型 Provider 阶段保持 not started，不建议直接接入真实模型。

## 阶段 14 更新：本地模型 Provider

分支：`feat/agent-model-providers`
状态：进行中

本阶段范围收窄为仅接入本机 Ollama Provider 和 ProviderRouter，保留 MockProvider，不接云 Provider、MCP、数据库或飞行动作执行。默认模型为用户已完成本机部署验证的 `qwen3:8b`。验收重点是结构化输出、错误处理、显式 fallback、安全回归、Agent 打包和 QGC 只读展示。

本阶段完成后不建议直接进入云 Provider；建议先继续做本地模型联调、提示词/结构化输出稳定性、安全策略回归和发布包验证。
## 阶段 14.5：本地模型输出稳定性

分支：`feat/ai-model-stability`
状态：进行中，本阶段代码和本地验证已完成，真实模型评估仍有剩余风险。

任务：

- 收紧本地 `qwen3:8b` 系统提示词。
- 增加 Agent 侧 proposal normalizer。
- 规范化 command alias 和参数 alias。
- 删除模型越权字段，无法安全整理时降级为 `proposal=null`。
- 增加 50 条以上中文评估样例。
- 增加显式 opt-in 的本地 Ollama 评估脚本。
- 保持 QGC C++ 本地安全策略为最终边界。

验收：

- Agent 单元测试通过。
- `AiIntentPolicyTest` 通过。
- Agent Release 打包通过。
- MERIVUS Release 构建通过。
- 安全关键字回归确认未新增真实执行链路。
- 真实 `qwen3:8b` 评估结果记录为剩余风险，不作为进入真实执行链路的依据。

下一步建议：继续迭代本地模型 prompt/eval 和 GUI 人工冒烟；暂不进入云 Provider、MCP 或命令执行器。

## 阶段 14 状态更新：本地 Ollama Provider

`feat/agent-model-providers` 已完成本地 Ollama Provider、ProviderRouter、Mock fallback 控制、QGC Provider 信息展示和 Agent 打包依赖更新。云 Provider、MCP、数据库和真实飞行动作执行均未开始。

## 阶段 7 状态提醒：命令确认/执行器

`feat/command-confirmation` 仍未开始。本阶段没有新增 `AiCommandExecutor`，也没有任何真实飞行动作执行能力。

## 阶段 14.6：QGC Agent Provider 设置

分支：`feat/agent-provider-settings`
状态：completed。

任务：

- 在 QGC AI 面板中提供 Mock/Ollama Provider 选择。
- 默认 Ollama 模型为 `qwen3:8b`，默认地址为 `http://127.0.0.1:11434`。
- 由 `AiServiceSupervisor` 启动自托管 Agent 时注入 Provider/Ollama 环境变量。
- 外部 Agent 复用模式只显示实际 `/merivus/info` 状态，不杀进程、不修改 Provider。
- Provider 未就绪时，聊天入口给出清晰提示。

验收：

- Mock 和 Ollama 设置均可在 GUI 中显示和切换。
- Agent 单元测试、`AiIntentPolicyTest`、Agent Release 打包和 MERIVUS Release 构建通过。
- 安全关键字回归确认未新增真实执行链路。
- 不写 `.env`、不记录 token、不自动安装或 pull 模型、不开放 LAN 监听。

下一步建议：继续本地 Qwen3 prompt/eval 迭代和 GUI 人工冒烟；暂不进入云 Provider、MCP 或命令执行器。

## 阶段 14.7：AI 问答与指令分离 / 仓库清理

分支：`feat/ai-qa-intent-separation-and-repo-cleanup`
状态：completed。

任务：

- 修复解释类问答被误判为查询 proposal 的问题。
- 中文化 AI 建议卡片中的用户可见字段。
- 补充纯问答评估集。
- 整理 README、文档索引、开发说明、schema 和配置模板。
- 检查仓库忽略规则和 GitHub 推送卫生。

验收：

- “未获得有效位置估计和EKF2报警是什么原因？” 返回中文解释，`proposal=null`。
- 明确查询和飞行动作建议仍可生成 proposal。
- 所有 proposal 仍保持 `executable=false`。
- 不新增真实飞行动作执行链路。

## 阶段 14.8：本地模型 few-shot / eval 稳定性增强

分支：`feat/ai-model-fewshot-eval`
状态：completed。

任务：

- 增加 `qwen3:8b` few-shot 示例，提高明确指令的 proposal 召回。
- 增加 normalizer recovery，只对明确模板化指令补全 proposal。
- 重构模型评估指标，区分 QA no-proposal、command recall、command accuracy、argument accuracy、safety invariant 和 forbidden rejection。
- 为 eval fixture 增加 `intent_type`、`expected_arguments`、`allow_normalizer_recovery`、`must_not_execute`、`category` 字段。
- 新增 GUI 手工 smoke checklist。

安全边界：

- 不新增 DeepSeek、OpenAI、Gemini、MCP 或云端服务。
- 不新增 Command Executor。
- 不新增 MAVLink / Vehicle / Swarm / PX4 执行调用。
- 不使用真实无人机。
- 高风险动作仍由 QGC C++ 本地策略保持 `executable=false`。

验收：

- Agent 单元测试通过。
- `run_model_eval.py --provider ollama --model qwen3:8b` 输出新分类指标。
- `AiIntentPolicyTest` 继续通过。
- Agent 打包和 MERIVUS Release 构建继续通过。
- 安全关键词检查确认未新增真实执行链路。
