# MERIVUS 本机 Agent HTTP 契约

本文档记录 `feat/local-agent-http` 阶段的独立 Python 本机 Agent。该阶段只验证本机 HTTP 契约和 Mock Provider，不修改 QGC C++ 网络客户端，不增加 QProcess，不接真实大模型，不执行任何飞行动作。

## Agent 职责

- 在本机提供 `127.0.0.1:8765` HTTP 服务。
- 接收 QGC 后续阶段传入的文本、只读状态快照和 capability 白名单。
- 返回自然语言 `reply` 和可选结构化 `proposal`。
- 使用 Mock Provider 做确定性回复。
- 统一处理 JSON、Schema、Provider 和消息长度错误。
- 使用 Python 标准 logging 记录服务启动、Provider、request_id、HTTP 状态和请求耗时。

## QGC 职责

- QGC 是唯一能把 Agent 建议转成真实飞行操作的本地执行边界。
- QGC 后续应在 C++ 中实现 Agent Client、Schema 校验、超时、取消、错误展示和本地策略重算。
- QGC 不应信任 Agent 返回的 `context` 推断、`proposal` 风险级别或飞行状态。
- 本阶段不修改 `custom/res/Merivus/MerivusAIAssistantPanel.qml`、`src/comm/*`、`Vehicle.*`、`SwarmController.*` 或 PX4 源码。

## 安全边界

- 默认绑定 `127.0.0.1`，不开放局域网或公网访问。
- `external_network_enabled=false`。
- `flight_execution_enabled=false`。
- 不接 OpenAI、DeepSeek、Gemini、Ollama、MCP、GIS 真实服务、云服务器或数据库。
- 不接触 MAVLink、PX4 参数、Vehicle 接口或真实厂商 TCP/RTSP 链路。
- `proposal` 只是建议，不包含 `executed=true`、MAVLink 消息 ID、任意七参数数组、Shell 命令、PX4 参数写入或通用 `send_mavlink` 命令。

## 接口地址

```text
GET  http://127.0.0.1:8765/health
GET  http://127.0.0.1:8765/merivus/info
POST http://127.0.0.1:8765/merivus/agent
```

`GET /health` 返回：

```json
{
  "status": "ok",
  "service": "merivus-agent",
  "provider": "mock",
  "version": "0.1.0"
}
```

`GET /merivus/info` 返回 service、version、provider、model、`external_network_enabled`、`flight_execution_enabled`、`supported_capabilities` 和 `max_message_length`。本阶段固定为 `provider=mock`、`model=mock-v1`。

## 请求 Schema

```json
{
  "request_id": "uuid-or-local-id",
  "session_id": "session-id",
  "message": "用户输入",
  "context": {
    "vehicle_count": 1,
    "active_vehicle_id": 1,
    "connected": true,
    "armed": false,
    "flight_mode": "Position",
    "battery_percent": 78,
    "gps_valid": true,
    "rtk_fix_type": "none",
    "link_state": "connected"
  },
  "allowed_capabilities": [
    "vehicle.query_status"
  ]
}
```

约束：

- `request_id` 和 `session_id` 不能为空。
- `message` 去除首尾空白后不能为空，长度不能超过 `MERIVUS_AGENT_MAX_MESSAGE_LENGTH`。
- `context` 缺省为空对象，只接受当前列出的状态字段。
- `allowed_capabilities` 缺省为空数组。
- 无法解析的 JSON、缺失必填字段和未定义顶层字段会被拒绝。
- Agent 只把 `context` 当作回答素材，不把它视为可信控制状态。

## 响应 Schema

```json
{
  "request_id": "uuid-or-local-id",
  "reply": "文本回答",
  "proposal": {
    "command": "vehicle.takeoff",
    "arguments": {
      "vehicle_id": null
    },
    "summary": "建议目标无人机明确后再评估起飞操作"
  },
  "provider": "mock",
  "model": "mock-v1",
  "status": "ok"
}
```

`proposal` 可以为 `null`。高风险动作即使被 capability 授权，也只能作为结构化建议返回。

## Mock Provider

Mock Provider 规则：

- 状态类问题：说明当前只是本机 Mock Agent，并仅复述请求中已有的 `connected`、`armed`、`battery_percent`、`flight_mode`，不补全缺失状态。
- 日志、报错、错误、故障类问题：说明可验证日志分析接口，但尚未接入实际日志工具。
- 航线、航点、地形、建筑、禁飞区类问题：说明可生成分析流程，但尚未连接 GIS Safety Service，不能作为真实安全结论。
- 高风险动作：识别起飞、解锁、降落、返航、暂停、悬停、改变模式、上传任务、启动任务；在 capability 允许时只返回 proposal。
- 普通问题：返回统一演示回答，不连接外部网络。

## Capability 控制

Mock Provider 只能提出 `allowed_capabilities` 中允许的能力。例：用户要求 `vehicle.takeoff`，但请求未包含该 capability 时：

- `proposal=null`。
- `reply` 说明该能力未被 QGC 授权。
- 不生成 `vehicle_id` 等可执行参数。

如果 capability 已包含高风险动作，Agent 仍不执行、不声称成功、不改变任何飞行器状态。

## Token 预留

本阶段不使用云端 API Key。可选 `MERIVUS_LOCAL_TOKEN`：

- 未配置时，开发模式允许本机 POST 访问。
- 配置后，`POST /merivus/agent` 要求 `X-Merivus-Token`。
- Token 不写入日志、不出现在错误响应、不提交到 Git。

正式发布时应由 QGC 启动 Agent 时生成随机会话 Token；本阶段不实现 QProcess 传递。

## 错误处理

错误响应可包含：

```json
{
  "error_code": "validation_error",
  "message": "请求字段未通过验证。",
  "request_id": "req-1"
}
```

统一处理 JSON 格式错误、空消息、消息超长、缺少 `request_id`、缺少 `session_id`、Provider 不存在和 Provider 内部异常。客户端响应不暴露 Python 堆栈、文件绝对路径、Token 或环境变量内容。

## 日志策略

记录：

- 服务启动、host、port、provider。
- request_id、HTTP 状态、请求完成时间。
- Provider 错误。

不记录：

- 完整请求正文。
- API Key、Token、RTSP 凭据。
- 用户隐私数据。
- 完整飞行日志内容。

## 本阶段不包含

- QGC C++ `AiAgentClient`。
- QProcess Agent 监管。
- QML 直接网络调用改造。
- 真实模型 Provider。
- GIS Safety Service。
- MAVLink、Vehicle、SwarmController 或 PX4 接入。
- 真实硬件测试、真实厂商 TCP/RTSP 连接。

## 下一阶段计划

下一阶段建议进入 `feat/qgc-agent-client`：

- 在 QGC C++ 中实现异步 HTTP Client。
- 使用 `QNetworkAccessManager`，不在 QML 中直接访问网络。
- 解析 `reply` 和 `proposal`。
- Agent 离线、超时、无效 JSON 时只显示错误，不影响飞控主功能。
- 第一版只显示回复和建议，不执行 proposal。

## 模型 Provider 更新（feat/agent-model-providers）

Python Local Agent 现在支持 `mock` 与本机 `ollama` Provider。默认仍为 `mock`；设置 `MERIVUS_AGENT_PROVIDER=ollama` 后，Agent 通过 `GET /api/tags` 检查本机 Ollama 和 `qwen3:8b`，通过 `POST /api/chat` 生成 `reply/proposal` 结构化输出。

`/merivus/info` 新增 `provider_ready`、`provider_error`、`available_models`，并继续返回 `external_network_enabled=false`、`flight_execution_enabled=false`。Agent 不自动安装 Ollama、不自动 pull 模型、不打包 Ollama 程序或模型权重。

Provider 输出只允许 `reply` 和 `proposal`；`validationStatus`、`policyDecision`、`localRisk`、`requiresConfirmation`、`executable` 仍只由 QGC C++ 本地策略层生成。
