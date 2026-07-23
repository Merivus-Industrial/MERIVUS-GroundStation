# 测试与验证矩阵

状态说明：代码/脚本存在不等于测试通过；“历史通过”只说明对应记录时点，不自动证明当前分支已复测。

| 模块 | 实现状态 | 验证方式 | 最近证据 | 验证结果 | 未验证内容 |
| --- | --- | --- | --- | --- | --- |
| Agent 静态代码 | 已实现 | 定向读取 app/provider/schema/tests | 2026-07-20 当前 `a40c9e4` 基线 | 文件与入口存在 | 运行时长期稳定性 |
| Agent 单元测试 | 测试存在 | `agent/`：`python -m pytest -p no:cacheprovider` | 2026-07-20，本次 2.22 秒 | **63 passed，6 warnings**（FastAPI/Starlette deprecation） | warnings 升级兼容；真实服务和真实模型不在这些测试内 |
| JSON Schema/示例配置 | 已实现 | PowerShell `ConvertFrom-Json` | 2026-07-20 | 3 个 schema + 2 个示例 JSON 解析通过 | 未用 JSON Schema validator 复跑语义；YAML 未自动解析 |
| C++ Schema/Policy 单元测试 | 测试存在 | `tools/dev/test-ai-intent-policy.ps1` | 2026-07-14 [整理报告](../development/REPO_CLEANUP_REPORT.md) | 历史 `27 passed / 0 failed` | **本次尚未执行**，不能声明当前 HEAD 通过 |
| Windows 工具链 | 检查脚本存在 | `check-windows-environment.ps1` | 2026-07-20 | Qt 5.15.2、MSVC、Python、Git 检查通过 | 环境存在不等于完整构建通过 |
| QML 语法/资源 | 已实现 | qmake 资源生成/历史构建 | 2026-07-17 [阶段说明](../releases/0.1.0-dev.1.md) | 历史记录称通过 | **本次尚未执行**；无独立产物保存在仓库 |
| Local Agent 启动 | 已实现 | `run-agent.ps1` / `python -m app` + `/health` | 2026-07-14 历史打包 HTTP smoke | 历史通过 | 本次未启动开发或打包服务 |
| Mock QGC-Agent 联调 | 已实现 | GUI/HTTP 人工 smoke | [Agent Supervisor](../architecture/AGENT_SUPERVISOR.md) 称已做打包 GUI smoke；整理报告称当轮未做 GUI smoke | **文档证据冲突，待确认** | 当前 HEAD 的 GUI、退出释放端口、重启/端口冲突 |
| Ollama Provider 单元测试 | 已实现 | mock `httpx` transport | 2026-07-20 pytest 包含 15 项 Provider 测试 | 单元测试通过 | 不代表 Ollama 服务或 `qwen3:8b` 可用 |
| Ollama 本地模型 | 已实现接入 | 本地 `/api/tags`、`/api/chat`、模型 eval | 2026-07-20 `ollama --version` 提示服务未运行 | **本次未验证** | 结构化输出可靠性、速度、中文 QA/指令分离、模型版本 |
| Proposal Normalizer | 已实现 | Python 单元测试/fixture | 2026-07-20 pytest | 本次相关测试通过 | 真实模型长尾输出仍未知 |
| QGC Release 构建 | 脚本存在 | `build-merivus.ps1 -Configuration Release` | 2026-07-17 阶段记录 | 历史增量构建通过 | 本次未构建；干净电脑/干净 build 未验证 |
| Agent onedir 打包 | spec/脚本存在 | `build-agent.ps1` + EXE health/chat | 2026-07-14 历史 POC | 历史通过 | 本次未打包；干净电脑、运行库、签名/升级未验证 |
| 发布包验证 | 非正式 staging | 人工启动、GUI、退出、无敏感文件 | 只有阶段文档描述 | 证据不完整 | 正式安装包、签名、升级、卸载、长期运行均未验证 |
| 网络/云 Provider | 未实现 | 应使用受控 mock/测试环境 | 仓库未找到实现 | 未验证 | 鉴权、出网、隐私、重试、成本、限流 |
| Agent loopback/出网约束 | 默认值与 QGC 侧约束已实现 | 静态读取 Python 配置与 Supervisor | 2026-07-20 | QGC 自托管路径强制 loopback；Python 直接 env 路径并非完整强制 | 非 loopback host/base URL 拒绝测试、状态语义与部署策略 |
| AI MAVLink 执行 | 明确未实现 | 静态交叉引用 | 2026-07-20 | 确认 AI proposal 停在显示层 | 不应进行真实执行验证；未来需独立设计 |
| 非 AI MAVLink/Vehicle 路径 | 代码存在 | 静态审计、Mock/SITL | 2026-07-20 定向搜索 | 确认 `CommandCenterOverlay`、`FlyViewMap`、`SwarmController` 有真实调用 | 本次未运行；统一前置检查/确认/审计不足 |
| PX4 SITL | QGC 基础与历史笔记存在 | PX4 SITL 多机回归 | 仅提交/归档文档线索 | 仓库中未找到当前可复现的通过结果 | **本次尚未执行** |
| 真实 4G/RTK 链路 | 资料/历史描述存在 | 人工台架/链路记录 | 硬件文档 | 当前无可复现仓库证据 | 长期、断链、干扰、视频并发 |
| 真实飞机 | 不属于自动化范围 | 人工安全流程 | 用户描述/硬件文档 | **当前无实机验证** | 全部端到端飞行与故障保护验收 |

## 本次明确没有执行

- 没有完整或增量编译 QGC。
- 没有运行 C++ Policy 测试、QML/GUI smoke、Local Agent HTTP 服务或 PyInstaller。
- 没有启动 Ollama 服务、调用 `qwen3:8b` 或任何云 Provider。
- 没有启动 PX4 SITL、连接 MAVLink、厂商服务器、4G/RTK 设备或真实飞机。
- 没有安装任何依赖或下载任何软件/模型。
