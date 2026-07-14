# MERIVUS Agent Provider 设置

本阶段分支：`feat/agent-provider-settings`。

本阶段目标是在 QGC AI 面板中加入本机 Agent Provider 运行时设置，让用户可以在 Mock 与本机 Ollama `qwen3:8b` 之间切换，并由 `AiServiceSupervisor` 在启动自托管 Agent 时传入对应环境变量。

范围仍限制在本机 Agent 生命周期和 QGC 只读交互：不接云 Provider，不接 MCP，不增加命令执行器，不修改 Vehicle、MAVLink、PX4、SwarmController 或真实飞行动作链路。

## 为什么 GUI 之前显示 mock / mock-v1

此前 Python Local Agent 的默认配置是 `MERIVUS_AGENT_PROVIDER=mock`。QGC AI 面板只展示 `/merivus/info` 返回的当前 Provider/Model，没有 Provider 选择控件；`AiServiceSupervisor` 启动 Agent 时也只注入 host、port 和本地 token，没有注入 Ollama 相关环境变量。

因此即使本机已经部署并测试了 Qwen3/Ollama，只要 QGC 自己启动 Agent，Agent 仍会按默认值运行在 MockProvider，GUI 自然显示 `mock / mock-v1`。

## QGC 设置项

AI 面板配置区新增：

- Provider：`Mock` / `Ollama`。
- 模型：默认 `qwen3:8b`，仅 Ollama 模式可编辑。
- Ollama 地址：默认 `http://127.0.0.1:11434`，限制为本机 HTTP 地址。
- 超时：默认 60 秒，范围 1-300 秒。
- 允许 Mock 回退：默认关闭。
- 当前 Provider/Model、Provider Ready、Provider Error、Models 继续来自 `/merivus/info`，用于显示 Agent 实际状态。

设置通过 `Qt.labs.settings` 持久化在 QGC 本地设置中，不写 `.env`，不写 token，不保存 API Key。

## Supervisor 环境变量

当 Agent 由 MERIVUS/QGC 自己启动时，`AiServiceSupervisor` 会注入：

```text
MERIVUS_AGENT_PROVIDER
MERIVUS_OLLAMA_BASE_URL
MERIVUS_OLLAMA_MODEL
MERIVUS_OLLAMA_TIMEOUT_SECONDS
MERIVUS_AGENT_ALLOW_MOCK_FALLBACK
```

默认值：

```text
MERIVUS_AGENT_PROVIDER=mock
MERIVUS_OLLAMA_BASE_URL=http://127.0.0.1:11434
MERIVUS_OLLAMA_MODEL=qwen3:8b
MERIVUS_OLLAMA_TIMEOUT_SECONDS=60
MERIVUS_AGENT_ALLOW_MOCK_FALLBACK=false
```

既有 `MERIVUS_AGENT_HOST=127.0.0.1`、`MERIVUS_AGENT_PORT=8765` 和内存本地 token 逻辑保持不变。token 不写入配置、日志或 Git。

## 外部 Agent 行为

如果 8765 端口上已经有兼容 Agent 在运行，且不是 QGC 当前进程启动的 Agent，`AiServiceSupervisor` 会进入外部 Agent 复用模式。

在 `ownsProcess=false` 时：

- QGC 不会杀掉外部 Agent。
- QGC 不会修改外部 Agent 的 Provider。
- 配置区继续展示 `/merivus/info` 返回的实际 Provider/Model/Ready/Error。
- 用户切换 Provider 或点击重启时，QGC 只提示需要手动停止外部 Agent 后重试，或确认外部 Agent 已用目标 Provider 启动。

## Provider Ready 行为

聊天发送前会检查 `provider_ready`。如果 Provider 未就绪，QGC 不会发送 `/merivus/agent` 请求，而是展示 `provider_error` 或本机模型服务检查提示。

常见状态：

- Mock：`provider_ready=true`，模型显示 `mock-v1`。
- Ollama 正常：`provider_ready=true`，模型显示 `qwen3:8b`，可用模型列表来自本机 Ollama。
- Ollama 未启动或模型未安装：`provider_ready=false`，聊天入口提示用户检查本机模型服务或切回 Mock。

## 安全边界

本阶段没有新增真实飞行动作执行能力。Agent 仍只返回 `reply` 和可选 `proposal`；QGC C++ 的 `AiSchemaValidator` 与 `AiCommandPolicy` 仍是最终安全边界，飞行动作类建议保持 `executable=false`。

本阶段没有恢复 QML `XMLHttpRequest`，没有新增 Vehicle/MAVLink/Swarm/PX4 写入路径，也没有自动安装 Ollama、自动 pull 模型、开启 LAN 监听或引入云端 API。
