# Merivus Local Agent 开发

Local Agent 位于 `agent/`，当前使用 FastAPI 和 Pydantic，提供本机 HTTP 接口。

## 接口

- `GET /health`
- `GET /merivus/info`
- `POST /merivus/agent`

默认监听 `127.0.0.1:8765`。QGC 自托管启动时会注入内存 token，token 不写入配置或日志。

## Provider

- `mock`：默认演示 Provider。
- `ollama`：本机 Ollama Provider，默认模型 `qwen3:8b`。

相关环境变量：

```text
MERIVUS_AGENT_PROVIDER=mock
MERIVUS_OLLAMA_BASE_URL=http://127.0.0.1:11434
MERIVUS_OLLAMA_MODEL=qwen3:8b
MERIVUS_OLLAMA_TIMEOUT_SECONDS=60
MERIVUS_AGENT_ALLOW_MOCK_FALLBACK=false
```

Agent 不自动安装 Ollama，不自动 pull 模型，不访问云端 Provider。

## 输出边界

Agent 只返回：

- `reply`
- 可选 `proposal`

问答、报警解释、故障原因、协议说明等解释类输入应返回 `proposal=null`。只有明确查询、UI 建议、任务建议或飞行动作建议才返回结构化 proposal。即使返回 proposal，QGC C++ 仍会重新校验并保持 `executable=false`。

## 测试

```powershell
cd agent
python -m pytest
```

真实模型评估必须显式 opt-in：

```powershell
python tools/run_model_eval.py --run-real-model
```
