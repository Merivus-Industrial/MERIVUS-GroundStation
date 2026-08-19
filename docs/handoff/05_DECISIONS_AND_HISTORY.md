# 工程决策与历史

## 决策记录

| 编号 / 决策 | 状态 | 采用原因 | 被替代或否决方案 | 当前影响 | 代码或文档证据 | 相关提交 |
| --- | --- | --- | --- | --- | --- | --- |
| ADR-001：QML 不保存 API Key，也不直接承担 Agent 网络请求 | 已采纳、已实现 | 降低密钥泄露、UI 阻塞和协议分散风险 | QML `XMLHttpRequest`、在 QML/仓库写 API Key | QML 调用 C++ Client；当前无云 Key 配置 | [`AiAgentClient.cc`](../../custom/src/Ai/AiAgentClient.cc)、[QGC Agent Client](../architecture/QGC_AGENT_CLIENT.md) | `e894666` |
| ADR-002：模型接入放在本机 Agent | 已采纳、已实现 | Provider、依赖和错误可与 QGC 隔离；Agent 崩溃不应阻塞飞控 UI | 在 QGC/QML 进程内直接运行或调用模型 | 形成 `127.0.0.1` HTTP 与 Supervisor 边界 | [`agent/app`](../../agent/app/)、[`AiServiceSupervisor.cc`](../../custom/src/Ai/AiServiceSupervisor.cc) | `e894666` |
| ADR-003：LLM 只生成 `reply` / `ActionProposal` | 已采纳、已实现 | 模型输出不可信，不能等同执行结果 | 模型直接返回 executed/MAVLink/参数写入 | proposal 必须经 QGC C++ 重算；固定 `executable=false` | [`ActionProposal.h`](../../custom/src/Ai/ActionProposal.h)、[接口契约](../architecture/INTERFACE_CONTRACTS.md) | `e894666` |
| ADR-004：Schema、Policy、明确确认、审计后才能接 Executor | 已采纳、部分实现 | 分层拒绝未知/危险输入并保留人工控制权 | 信任 Provider 风险字段或只靠 UI 白名单 | Schema/Policy/摘要审计已实现；统一确认和 Executor **尚未实现** | [`AiSchemaValidator.cc`](../../custom/src/Ai/AiSchemaValidator.cc)、[`AiCommandPolicy.cc`](../../custom/src/Ai/AiCommandPolicy.cc)、[安全边界](../architecture/SAFETY_BOUNDARIES.md) | `e894666` |
| ADR-005：外部网络和 AI 飞行执行默认关闭 | 已采纳、AI 路径已实现 | 失效安全、避免未经授权的云调用和真实动作 | 默认云 Provider、默认可执行 proposal | 默认 Mock/loopback；info 报告两项 false；未来启用需独立评审 | [`agent/app/config.py`](../../agent/app/config.py)、[`agent/app/providers/base.py`](../../agent/app/providers/base.py) | `e894666` |
| ADR-006：Agent 使用 PyInstaller onedir | 已采纳、POC 已实现 | 输出可审计、不临时解压、便于固定相对路径随 QGC staging 分发 | PyInstaller onefile、依赖开发机 Python | 发布路径为 `applicationDirPath()/agent/merivus-agent.exe` | [`merivus-agent.spec`](../../agent/merivus-agent.spec)、[Packaging POC](../architecture/AGENT_PACKAGING_POC.md) | `e894666`（历史阶段提交 `a337147`） |
| ADR-007：增加 Proposal Normalizer | 已采纳、已实现 | 本地模型可能带 markdown、嵌套 JSON、空/错字段或把解释误判为指令 | 直接反序列化模型首段文本 | 先规范化，再由 Python Schema 与 QGC C++ 双重约束 | [`proposal_normalizer.py`](../../agent/app/proposal_normalizer.py)、[Model Stability](../architecture/AI_MODEL_STABILITY.md) | `e894666`（历史阶段提交 `4fed48c`、`41dc3d5`） |
| ADR-008：本地模型与云 Provider 分离 | 已采纳、仅本地实现 | 本地 Ollama 不需要云密钥；云端涉及认证、出网和数据治理 | 把云 Key/Provider 直接塞入当前 QGC 设置 | 当前只支持 Mock/Ollama；云 Provider、MCP 尚未开始 | [`router.py`](../../agent/app/providers/router.py)、[Model Providers](../architecture/AGENT_MODEL_PROVIDERS.md) | `e894666` |
| ADR-009：GIS 显示数据与安全计算数据分离；3D Tiles 不是唯一碰撞依据 | 目标决策；文档中提及，代码侧待确认 | 显示瓦片可能缺少高度精度、时效、拓扑和责任边界；计算需 DEM/DSM/GeoTIFF/矢量/禁飞区 | 仅凭卫星图、倾斜摄影或 3D Tiles 自动判定安全 | `WaypointSafetyService` 尚未实现；未来只输出报告/建议，不自动改任务 | [目标架构](../architecture/TARGET_ARCHITECTURE.md)、[Roadmap](../architecture/ROADMAP.md) | 无可确认实现提交 |

## Git 里程碑

| 分支或提交 | 主要内容 | 当前状态 | 是否已合并 | 验证情况 |
| --- | --- | --- | --- | --- |
| `b3d6659` | MERIVUS 跨平台源码基线 | 主线祖先 | 是 | 仓库未保留该提交的完整验证报告 |
| `026e8d0` | SITL 安全设置与地图指令交互 | 主线祖先 | 是 | 有历史说明；本次未运行 SITL |
| 历史功能提交 `c7992a1`…`41dc3d5` | Local Agent、Client、Supervisor、Policy、Ollama、Normalizer/eval | 功能来源历史；`41dc3d5` 有备份标签但不是当前 HEAD 祖先 | 通过 squash 进入 `e894666`，不能仅用 ancestry 判定 | 阶段文档记录测试；本次只复测 Python 部分 |
| `e894666` | 将完整 AI/Agent 能力与文档 squash 集成主线 | 当前产品基线组成 | 是 | 整理报告记录 63 Python 测试、27 C++ Policy 测试、构建/打包/HTTP；本次未全量复跑 |
| `74b6039` | macOS Qt 安装 Python 环境 CI 修复 | 主线祖先 | 是 | 本次未运行 CI/macOS |
| `a40c9e4` / `v0.1.0-dev.1` | 阶段发布、AI 面板与 Supervisor 收口 | 当前有效产品基线 | 是 | 历史 QML/Release 增量构建；本次未构建 |
| `9bb0807` | 2026-08-19 仓库整理开始时的产品基线 | 主线祖先 | 是 | 作为本轮目录、代码、文档和远端状态审计起点 |

`main` 是唯一长期集成分支；功能分支完成 PR 验证并合并后应删除。`backup/before-repo-cleanup-20260714-1637` 是历史备份标签，不是长期开发分支。
