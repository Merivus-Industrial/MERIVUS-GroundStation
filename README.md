# MERIVUS 无人机多机调度系统指挥中心

MERIVUS 是基于 QGroundControl / PX4 / 4G / RTK / 本机 AI Agent 的无人机多机调度地面站平台。本仓库保留 QGroundControl 上游工程结构，并在 `custom/`、`agent/`、`docs/`、`schemas/` 和 `configs/` 中沉淀 MERIVUS 的定制能力。

## 项目定位

MERIVUS 面向多无人机协同作业场景，负责地面站 UI、链路状态展示、调度交互、任务预览、AI 辅助解释和本地安全建议。它不替代飞控，不绕过 PX4 / QGC 原生安全流程，也不让 LLM 直接控制无人机。

```text
操作者
  -> MERIVUS 地面站
  -> Local Agent / ActionProposal
  -> QGC C++ Schema Validator / Local Policy
  -> 仅展示和预览
```

当前 AI 链路不会进入：

```text
ActionProposal -> Vehicle / MAVLink / Swarm / PX4
```

## 当前已实现

- QGC Custom Build 主界面与 MERIVUS 多机调度 UI。
- QGC AI 面板与中文建议卡。
- Python FastAPI Local Agent。
- QGC C++ `AiAgentClient`。
- `AiServiceSupervisor`，由 QGC 启动和守护本机 Agent。
- PyInstaller Agent 打包与 Release staging。
- Mock Provider 与 Ollama / `qwen3:8b` Provider。
- Provider 设置 UI、Provider Ready/Error/Models 展示。
- `ActionProposal`、Agent schema、QGC C++ schema 校验。
- `AiCommandPolicy` 本地风险和策略判定。
- QA/指令分离：解释类问题不误出建议卡。
- 模型 few-shot / eval 指标优化。
- 所有 AI proposal 当前均保持 `executable=false`。

## 当前未实现

- 云设备网关。
- 用户认证和权限系统。
- 数据库。
- GIS Safety Service。
- Media Service。
- MCP。
- 真实命令执行器。
- 安装包签名。
- 实机端到端验证。

## 安全边界

- LLM / Agent 不能直接控制无人机。
- Agent 不发送 MAVLink。
- Agent 不修改 PX4 参数。
- Agent 不访问 Vehicle / Swarm / PX4 执行入口。
- proposal 只是结构化建议，不代表执行结果。
- QGC C++ 的 `AiSchemaValidator` 与 `AiCommandPolicy` 是当前 AI 链路最终安全边界。
- 高风险动作只预览，不执行；当前没有 AI 真实飞行动作执行能力。

## 仓库结构

- `src/`：QGroundControl 上游主体代码。
- `custom/`：MERIVUS QGC Custom Build 入口、QML、资源和 C++ 扩展。
- `agent/`：MERIVUS Local Agent、Provider、schema、测试和打包 spec。
- `docs/`：架构、开发、硬件、流程和安全边界文档。
- `schemas/`：Agent request / response / ActionProposal JSON Schema 草案。
- `configs/`：配置模板和策略示例。
- `tools/dev/`：Windows 构建、Agent 打包和局部测试脚本。
- `custom/tests/`、`agent/tests/`：本地策略和 Agent 自动化测试。

更多文档入口见 [docs/INDEX.md](docs/INDEX.md)。

## 快速开发与验证

Windows 基线：

- Qt 5.15.2 / MSVC2019_64 Qt Kit。
- Visual Studio 2022 MSVC x64 工具链。
- Python 3.11。
- Ollama 与本机 `qwen3:8b`。

Agent 单元测试：

```powershell
cd agent
python -m pytest
```

QGC AI 策略测试：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/test-ai-intent-policy.ps1
```

Agent Release 打包：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/build-agent.ps1 -Configuration Release
```

MERIVUS Release 构建：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/build-merivus.ps1 -Configuration Release
```

本机真实模型评估必须显式 opt-in：

```powershell
python agent/tools/run_model_eval.py --provider ollama --model qwen3:8b
```

## 敏感文件与产物

不要提交：

- `.env`、Token、API Key、账号密码。
- PDF、DOCX、Excel、截图、视频。
- Ollama 模型、模型权重、本地模型目录。
- `build/`、`dist/`、`staging/`、`.pytest_cache/`、`__pycache__/`。
- RTSP 凭据、物联网卡信息、真实设备凭据。
- 真实飞行日志或生产坐标。

仓库应只提交源码、文档、schema、示例配置和必要资源。

## 许可证与上游

MERIVUS 基于开源 QGroundControl 二次开发，并保留上游项目和第三方依赖的许可证约束。相关规则见 [COPYING.md](COPYING.md) 与源文件中的许可证声明。
