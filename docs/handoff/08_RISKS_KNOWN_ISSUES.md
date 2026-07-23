# 风险与已知问题

风险等级只表示当前影响，不代表已经发生。缓解措施分为“已存在”和“仍需完成”，避免把计划写成落实结果。

| 等级 | 风险 / 已知问题 | 当前证据 | 已存在的缓解 | 仍需完成 |
| --- | --- | --- | --- | --- |
| 高 | 非 AI 高风险入口分散，Policy 与真实执行层不是同一链路 | `CommandCenterOverlay.qml` 直接调用 guided/sendCommand；`FlyViewMap.qml` 调 `SwarmController`；后者可 goto、startMission、发 MAVLink | AI 路径与这些入口隔离；现有 QGC/飞控仍有自身检查 | 统一入口清单、默认关闭/显式启用、冻结目标、前置条件、确认、审计和 Mock/SITL 回归 |
| 高 | 用户确认可能被误解或绕过 | AI QML 的“确认”函数不执行；非 AI 入口各自处理，未形成统一确认服务 | AI `executable=false`，无 AI 执行按钮 | 对每个真实动作证明无法绕过确认；区分“已发送、ACK、执行中、完成” |
| 高 | Python 直接启动路径可由环境变量使用非 loopback host/Ollama URL，但 info 仍报告外网关闭 | `agent/app/config.py` 只收敛 `0.0.0.0`；QGC Supervisor 才强制 Ollama loopback | 默认值和 QGC 自托管路径使用 loopback | Python 侧拒绝非 loopback，或准确报告出网状态；增加配置测试 |
| 高 | 尚未进行真实飞机端到端验证 | 仓库只有用户/硬件文档描述，无可复现结果 | 默认只允许 Unit/Mock/回放/SITL；AI 不执行 | 真实测试仅能在独立人工安全计划、现场授权和可回退条件下进行 |
| 高 | GIS 数据精度、时效和责任边界未确定 | 仅目标架构/Roadmap；无 `WaypointSafetyService` 生产代码 | 文档明确显示数据与计算数据分离，3D Tiles 不作唯一依据 | 明确 DEM/DSM/矢量/禁飞区来源、版本、精度、未知区域策略与审计 |
| 中 | 本地模型结构化输出与意图遵循不稳定 | 历史 72 例指标未全通过；本次未运行真实模型 | Normalizer、few-shot、Python Schema、C++ Schema/Policy | 在固定模型版本上复测，记录失败分类；不得以指标作为执行授权 |
| 中 | Provider capability 枚举存在跨层差异 | Python `DEFAULT_CAPABILITIES` 与 QML/C++ 枚举不完全一致 | QML 当前请求传入自己的受控列表；C++ 未知命令默认拒绝 | 建立单一版本化契约和跨语言一致性测试 |
| 中 | Agent 生命周期、端口冲突和崩溃恢复复杂 | Supervisor 有 health、重启、外部复用和端口冲突分支；本次未做 GUI 联调 | 不 kill 未知进程；只关闭自己启动的 Agent；重启次数受限 | 测试退出释放端口、外部 Agent、冲突服务、连续失败和异常退出 |
| 中 | Agent 未安装或打包运行时依赖缺失 | onedir spec/脚本存在，本次未打包或在干净电脑运行 | UI 有 `NotInstalled`/Error 降级；QGC 主功能不应依赖 Agent | 干净 Windows VM 验证运行库、路径、许可证、签名、升级和卸载 |
| 中 | Windows 路径与构建环境依赖本机布局 | 脚本默认 `E:\Qt`、`D:\Program Files...`，且 `build-merivus.ps1` 要求根 `build/` 已存在 | 参数可覆盖 Qt/VS 路径；脚本限制清理范围 | 干净克隆运行手册、路径无关检查和可重复构建 |
| 中 | QML/C++/Python 三层联调容易出现契约漂移 | 旧 `intent` 文档、当前 `proposal` 代码与若干阶段文档并存 | JSON/Pydantic/C++ 校验与局部测试存在 | 契约版本化、端到端 Mock 测试、更新或明确归档过时文档 |
| 中 | 密钥、配置、任务上下文或日志泄露 | Token 通过子进程环境传递；日志策略只记录摘要；真实部署未审计 | 示例值为空，QML 无 API Key，审计不记完整消息 | 检查进程环境/崩溃转储/日志权限；禁止提交 `.env` 和凭据 |
| 中 | 端口 `8765` 固定，可能与其他服务冲突 | Client/Supervisor 默认固定端口 | 兼容性 health 检查；不终止未知进程 | 明确端口配置/占用诊断策略并做回归 |
| 中 | 旧文档把历史阶段写成当前能力 | `MERIVUS_AI_ASSISTANT.md`、`TARGET_ARCHITECTURE.md`、`agent/README.md` 与代码冲突 | handoff 标明推荐版本和冲突，以代码为准 | 后续单独修正文档，不在业务任务中混改 |
| 低 | FastAPI/Starlette 测试存在弃用 warning | 本次 `63 passed, 6 warnings` | 当前不影响测试通过 | 依赖升级前建立兼容分支并消除 warning；不得为此无关升级全部依赖 |
| 低 | Git 历史经过 squash，旧功能提交不是当前 HEAD 祖先 | `e894666` 集成报告与备份标签 | 里程碑表同时记录 squash 和历史标签 | 判断合并状态时结合当前代码和整理报告，不能只用 ancestry |

## 当前最重要风险判断

1. 最大安全风险不是 AI 已经能飞，而是**非 AI 的真实执行入口分散且缺少统一证明链**。
2. AI 当前停在 proposal 显示层；未来若引入确认器/Executor，必须新建独立设计和测试，不能复用“文案确认”冒充安全控制。
3. 真实飞机、生产网络、云 Provider、GIS 碰撞结论都没有当前仓库可复现验收证据。

详细历史风险见 [风险登记](../architecture/RISK_REGISTER.md)，但其中部分“当前证据”已过时，应以本文件、[当前状态](02_CURRENT_STATE.md) 和代码为准。