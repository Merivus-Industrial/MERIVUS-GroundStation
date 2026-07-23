# 环境、构建与运行

## 已确认的 Windows 基线

2026-07-20 执行 [`tools/dev/check-windows-environment.ps1`](../../tools/dev/check-windows-environment.ps1) 通过：

- Qt `5.15.2`，mkspec `win32-msvc`；Qt Kit 目录名为 `msvc2019_64`。
- Visual Studio 2022 Community x64 MSVC 工具链。
- Python `3.11.9`。
- Git `2.53.0.windows.2`。
- `qmake`、`jom` 不在当前 `PATH`，但脚本默认绝对路径可定位。
- GitHub CLI `2.95.0` 可执行；本次未联网核验登录。
- Ollama client `0.32.1` 可执行，但当前服务未运行。
- Qt Creator 可执行文件位于脚本默认工具目录；VS Code `1.129.0` 可执行。Codex 作为协作工具使用，不属于 QGC 构建依赖。

仓库通常位于 `E:\MERIVUS`，但脚本和文档链接应以仓库相对路径为主。构建脚本的 Qt/VS 默认绝对路径可通过参数覆盖。

## 关键命令矩阵

| 命令 | 执行目录 / 前置条件 | 会产生修改 | 外网 / 真实设备 | 验证状态 |
| --- | --- | --- | --- | --- |
| `powershell -NoProfile -ExecutionPolicy Bypass -File tools/dev/check-windows-environment.ps1` | 仓库根；Qt/Qt Creator/VS/Python/Git 位于默认或传入路径 | 否 | 不联网；不访问飞行设备 | 2026-07-20 通过 |
| `python -m pytest -p no:cacheprovider` | `agent/`；已安装 `requirements-dev.txt` | 通常仅可能产生忽略的 Python cache；本次禁用 pytest cache | 测试使用本地 mock，不调用真实 Ollama/外网/飞控 | 2026-07-20：63 passed，6 warnings |
| `.\run-agent.ps1` | `agent/`；Python 环境已安装 runtime 依赖；默认 Mock | 启动本机进程，可能产生忽略的 cache/控制台日志 | 默认不联网；不访问飞行设备 | 本次尚未执行；历史 HTTP smoke 有记录 |
| `powershell -ExecutionPolicy Bypass -File tools/dev/test-ai-intent-policy.ps1` | 仓库根；Qt 5.15.2、VS 2022 | 写入 `build/ai-intent-policy-tests` | 不联网；不访问飞行设备 | 本次未执行；2026-07-14 历史 `27/0` |
| `powershell -ExecutionPolicy Bypass -File tools/dev/build-merivus.ps1 -Configuration Release` | 仓库根；Qt/VS；当前脚本要求仓库 `build/` 已存在 | 写入 `build/.../staging`，并会改生成的 Makefile | 不联网；编译不访问飞行设备 | 本次未执行；历史增量构建通过 |
| `powershell -ExecutionPolicy Bypass -File tools/dev/build-agent.ps1 -Configuration Release` | 仓库根；Python/PyInstaller；QGC staging 已存在 | 清理并重建 `agent/build`、`agent/dist`、`staging/agent` | 不联网；不访问飞行设备 | 本次未执行；历史 POC 通过 |
| `python tools/run_model_eval.py --provider ollama --model qwen3:8b` | `agent/`；Ollama 服务和模型已由人工准备 | 输出评估摘要；可能产生本地运行 cache | 访问本机 `127.0.0.1:11434`，不应外网；不访问飞控 | 本次尚未执行，Ollama 未运行 |
| `.\build\Desktop_Qt_5_15_2_MSVC2019_64bit-Release\staging\MERIVUS.exe` | 仓库根；Release 已成功构建/打包 | 产生用户设置与运行日志的可能性 | 启动后是否连接网络/设备取决于人工配置；禁止自动接真实飞机 | 本次未启动 |

不要为了补全文档执行 `pip install`、Ollama `pull` 或完整重建。依赖安装说明只用于人工准备新环境。

## 工具角色

| 工具 | 项目中的作用 | 当前确认范围 |
| --- | --- | --- |
| Qt 5.15.2 / qmake | QGC Qt/QML 构建基线 | 环境脚本本次定位成功；未构建 |
| Visual Studio 2022 / MSVC x64 | 使用兼容 `msvc2019_64` Qt ABI 编译 | `cl.exe` 本次定位成功 |
| `nmake` / `jom` | `build-merivus.ps1` 使用 `nmake`；旧替代脚本使用 `jom` | 文件/路径存在；本次未编译 |
| Qt Creator | IDE 与 Qt Kit 管理 | 可执行文件存在；本次未启动 |
| VS Code / Codex | 编辑、审计与协作 | 本次使用仓库工作区；不替代 Qt 工具链 |
| Python 3.11 | Local Agent、pytest、PyInstaller 入口 | Python 与 pytest 本次可用 |
| Ollama | 本机模型服务 | 仅 client 存在；服务本次未运行 |
| Git / GitHub CLI | 版本控制与远端协作 | Git 本次使用；`gh` 仅检查版本，未核验认证/联网 |

## 开发运行与联调边界

1. 开发模式可在 `agent/` 执行 `./run-agent.ps1`，默认启动 Mock Agent；本次未启动。
2. QGC 启动时，`AiServiceSupervisor` 先检查 `127.0.0.1:8765`，复用兼容外部 Agent，或查找 staging 中的 onedir Agent、仓库打包 Agent、显式开发 Python，最后才尝试当前仓库 Python。
3. QGC 自托管 Agent 时注入 loopback host、固定端口、内存 Token 和 Provider 设置；`AiAgentClient` 只允许 `http://127.0.0.1:<port>`。
4. 联调只验证 health/info/chat、proposal 显示和进程退出；当前不得把 proposal 接到 `Vehicle`/MAVLink，也不得自动连接真实飞机。

## Agent 环境变量

| 变量 | 默认/用途 | 边界 |
| --- | --- | --- |
| `MERIVUS_AGENT_HOST` | `127.0.0.1` | `0.0.0.0` 会被 Agent 收敛回 loopback |
| `MERIVUS_AGENT_PORT` | `8765` | 固定端口可能冲突；Supervisor 不 kill 未知进程 |
| `MERIVUS_AGENT_PROVIDER` | `mock`，可选 `ollama` | 未知 Provider 安全失败 |
| `MERIVUS_OLLAMA_BASE_URL` | `http://127.0.0.1:11434` | QGC Supervisor 强制 loopback；直接 Python env 尚未强制，待统一 |
| `MERIVUS_OLLAMA_MODEL` | `qwen3:8b` | 模型不随仓库/安装包分发 |
| `MERIVUS_OLLAMA_TIMEOUT_SECONDS` | `60` | 需避免 UI 长时间无反馈 |
| `MERIVUS_AGENT_ALLOW_MOCK_FALLBACK` | `false` | fallback 必须显式，避免把 Mock 答案误认真实模型结果 |
| `MERIVUS_AGENT_LOG_LEVEL` | `INFO` | 不得记录 Token、完整敏感请求或真实飞行数据 |
| `MERIVUS_AGENT_MAX_MESSAGE_LENGTH` | `8000` | 请求长度上限 |
| `MERIVUS_LOCAL_TOKEN` | 空；Supervisor 启动时生成 | 只存在内存/子进程环境，不进 QML、配置、日志或 Git |
| `MERIVUS_AGENT_DEV_PYTHON` / `MERIVUS_AGENT_DEV_ROOT` | 无默认值 | Supervisor 的显式开发启动入口；不要硬编码个人路径 |

## 已发现的命令/文档差异

- [Windows 构建说明](../development/BUILD_WINDOWS.md) 推荐 `build-merivus.ps1`；当前脚本在创建具体 build 目录前先要求仓库根 `build/` 已存在，干净克隆需先人工创建该空目录或修正脚本。此行为本次未改。
- `build-windows.ps1` 使用 `jom`，`build-merivus.ps1` 使用 `nmake` 并修改生成 Makefile 以启用 response files；当前推荐入口是后者，旧脚本仅作为替代/诊断路径。
- [`agent/README.md`](../../agent/README.md) 仍称 Provider 固定 Mock、QProcess Token 未实现；当前代码和 [Agent 开发](../development/AGENT_DEVELOPMENT.md) 已支持 Ollama 与 Supervisor Token，以代码为准。
- [AI Assistant 使用说明](../MERIVUS_AI_ASSISTANT.md) 仍给出旧 `intent` 请求/响应和“确认后调用 Vehicle”的说明；当前运行契约是 `reply + proposal` 且始终不执行。
- [Model Stability](../architecture/AI_MODEL_STABILITY.md) 称真实模型调用必须带 `--run-real-model`；当前脚本也接受显式 `--provider ollama` 作为 opt-in。以 `agent/tools/run_model_eval.py --help` 和源码为准。
