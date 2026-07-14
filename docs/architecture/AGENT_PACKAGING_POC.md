# Agent Packaging POC

本文档记录 `release/agent-packaging-poc` 阶段，把 `agent/` 中的 Python Mock Agent 打包为 Windows onedir 可运行程序，并复制到 QGC Release staging 目录。

## 范围

- 使用 PyInstaller onedir 打包本机 Mock Agent。
- 输出主程序名为 `merivus-agent.exe`。
- 将完整 onedir 输出复制到 QGC Release staging：

```text
build/Desktop_Qt_5_15_2_MSVC2019_64bit-Release/staging/agent/
```

- 最终 Supervisor 发布路径保持不变：

```text
QCoreApplication::applicationDirPath()/agent/merivus-agent.exe
```

本阶段不接 OpenAI、DeepSeek、Gemini、Ollama、MCP、云端服务、MAVLink、Vehicle 飞行动作、Swarm 执行、PX4 修改或真实无人机。

## 选择 onedir 的原因

onedir 便于检查输出内容、排除 `.env`、PDF、虚拟环境和模型文件，也更接近后续 Windows 发布包的目录结构。相比 onefile，onedir 启动时不需要把运行库解压到临时目录，便于 QGC Supervisor 直接按固定相对路径启动。

## 本次工具版本

- Python：`3.11.9`
- PyInstaller：`6.21.0`
- pyinstaller-hooks-contrib：`2026.6`

## spec 入口

spec 文件：

```text
agent/merivus-agent.spec
```

入口脚本：

```text
agent/app/__main__.py
```

`__main__.py` 复用现有 `AgentSettings.from_env()` 和 `create_app(settings)`，通过 `uvicorn.run()` 绑定 `127.0.0.1` 和配置端口，不重复实现 FastAPI 应用。

## 构建脚本

脚本：

```powershell
.\tools\dev\build-agent.ps1 -Configuration Release
```

职责：

- 检查 Python。
- 检查当前 Python 环境中是否可执行 PyInstaller。
- 清理 `agent/build/` 和 `agent/dist/`。
- 运行 `agent/merivus-agent.spec`。
- 验证 `agent/dist/merivus-agent/merivus-agent.exe` 存在。
- 清理并重建 QGC `staging/agent/`。
- 复制完整 onedir 输出到 `staging/agent/`。
- 检查 staging 中没有 `.env`、PDF、虚拟环境目录。
- 出错时抛出异常并返回非零退出码。

脚本不修改系统 PowerShell 执行策略，不硬编码开发机仓库绝对路径、用户目录或 Python 虚拟环境路径。

## 输出结构

PyInstaller 输出：

```text
agent/dist/merivus-agent/
  merivus-agent.exe
  _internal/
```

QGC staging 输出：

```text
build/Desktop_Qt_5_15_2_MSVC2019_64bit-Release/staging/
  MERIVUS.exe
  agent/
    merivus-agent.exe
    _internal/
```

`agent/build/`、`agent/dist/`、QGC `build/` 和 staging 内容均不提交 Git。

## 验证结果

直接运行 `agent/dist/merivus-agent/merivus-agent.exe`：

- `GET /health` 返回 `status=ok`、`service=merivus-agent`、`provider=mock`。
- `GET /merivus/info` 返回 `provider=mock`、`model=mock-v1`、`external_network_enabled=false`、`flight_execution_enabled=false`。
- `POST /merivus/agent` 返回 Mock 回复。
- 起飞请求返回 `proposal.command=vehicle.takeoff`，仅作为结构化建议，不执行。
- 设置 `MERIVUS_LOCAL_TOKEN` 后，错误 Token 返回 `401`，正确 Token 返回 `200`。
- 终止 Agent 进程后端口释放。

直接运行 staging 路径：

- `build/.../staging/agent/merivus-agent.exe` 可启动。
- `GET /health` 返回正常。

GUI 人工冒烟：

- `staging/MERIVUS.exe` 能够按发布路径找到 `staging/agent/merivus-agent.exe`。
- AI 面板启用“本机智能体”后，Supervisor 能够自动启动打包 Agent。
- `/health` 正常，Supervisor 进入 `Healthy`。
- QML 显示 Agent 已连接。
- provider/model 显示 `mock/mock-v1`。
- 普通聊天请求能够返回 Mock 回复。
- 无 Vehicle 连接时未崩溃。
- 高风险 proposal 只显示为未执行建议，不触发飞行动作。

退出残留检查：

- 本次只读检查未发现 `MERIVUS` 或 `merivus-agent` 进程。
- 本次只读检查未发现 8765 端口监听。
- 由于本轮没有自动控制完整 GUI 关闭序列，后续仍建议重复执行“启动 QGC -> Supervisor 启动 Agent -> 关闭 QGC -> 检查进程和端口释放”的人工验证。

## Token 传递

Supervisor 启动 Agent 时生成内存 Token，并通过子进程环境传递：

```text
MERIVUS_LOCAL_TOKEN=<runtime-only-token>
```

`AiAgentClient` 对聊天 POST 携带：

```text
X-Merivus-Token: <runtime-only-token>
```

Token 不写入 Git、配置文件或日志。

## 日志和数据目录

当前 Mock Agent 不建立复杂日志系统，运行时不需要写入安装目录，也不会尝试写入 Program Files。请求日志只记录 method、path、status、request_id 和耗时，不记录完整用户消息。

后续正式发布阶段应把日志和配置目录放入 AppData，并继续避免记录 Token、完整用户消息、API Key 或真实飞行凭据。

## 已知限制

- GUI 主流程已人工验证；QGC 退出清理建议继续做可重复人工验证。
- 尚未在干净 Windows 电脑验证无 Python 环境时的启动行为。
- 尚未实现正式安装包、升级策略、签名、AppData 日志目录和崩溃收集。
- 当前仍是 Mock Provider，不包含真实模型、模型文件或 API Key。

## 下一阶段

建议下一阶段进入 `feat/ai-intent-policy`，先实现结构化意图、本地白名单、风险策略和拒绝规则。不要直接进入真实模型 Provider，避免模型输出靠近执行面。
