# MERIVUS 本机 Mock Agent

本目录提供 MERIVUS 本机智能体第一阶段服务。当前实现只用于验证本地 HTTP 契约、Schema、Mock Provider 和安全边界，不连接真实大模型、MAVLink、PX4、GIS 服务、数据库或真实无人机。

## 范围

- 默认监听 `127.0.0.1:8765`。
- 提供 `GET /health`、`GET /merivus/info`、`POST /merivus/agent`。
- Provider 固定为 `mock`，模型标识为 `mock-v1`。
- `external_network_enabled=false`。
- `flight_execution_enabled=false`。
- 高风险飞行动作只返回 `proposal`，不执行、不声称成功、不改变飞行器状态。

## 配置

支持环境变量：

```text
MERIVUS_AGENT_HOST=127.0.0.1
MERIVUS_AGENT_PORT=8765
MERIVUS_AGENT_PROVIDER=mock
MERIVUS_AGENT_LOG_LEVEL=INFO
MERIVUS_AGENT_MAX_MESSAGE_LENGTH=8000
MERIVUS_LOCAL_TOKEN=
```

`MERIVUS_AGENT_HOST` 不应设置为 `0.0.0.0`。当前代码会把该值收敛回 `127.0.0.1`。

`MERIVUS_LOCAL_TOKEN` 为空时，开发模式允许本机 POST 访问；配置后，`POST /merivus/agent` 需要请求头 `X-Merivus-Token`。正式发布时应由 QGC 启动 Agent 时生成随机会话 Token；本阶段不实现 QProcess 传递。

## 开发命令

```powershell
cd agent
python -m pip install -r requirements-dev.txt
python -m pytest
python -m uvicorn app.main:app --host 127.0.0.1 --port 8765
```

也可以使用：

```powershell
.\run-agent.ps1
```

脚本优先使用 `agent/.venv`；仅在虚拟环境不存在时才使用 `PATH` 中可正常运行的 Python。脚本不会修改系统执行策略。

## 手动验证

GET:

```powershell
Invoke-RestMethod http://127.0.0.1:8765/health
```

POST:

```powershell
$body = @{
  request_id = "demo-1"
  session_id = "local-dev"
  message = "请查询无人机状态"
  context = @{
    connected = $true
    armed = $false
    battery_percent = 78
    flight_mode = "Position"
  }
  allowed_capabilities = @("vehicle.query_status")
} | ConvertTo-Json -Depth 5
$utf8Body = [System.Text.Encoding]::UTF8.GetBytes($body)

Invoke-RestMethod `
  -Method Post `
  -Uri http://127.0.0.1:8765/merivus/agent `
  -ContentType "application/json; charset=utf-8" `
  -Body $utf8Body
```

示例数据仅为本机开发快照，不包含真实飞行器敏感数据。

## 请求 Schema

```json
{
  "request_id": "string",
  "session_id": "string",
  "message": "string",
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
  "allowed_capabilities": ["vehicle.query_status"]
}
```

`context` 缺省为空对象，`allowed_capabilities` 缺省为空数组。未定义顶层字段会被拒绝。Agent 不把 `context` 视为可信控制状态，只用于回答。

## 响应 Schema

```json
{
  "request_id": "string",
  "reply": "string",
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

`proposal` 可以为 `null`。它只是结构化建议，不包含 `executed`、MAVLink 消息 ID、七参数数组、Shell 命令或 PX4 参数写入。
