# MERIVUS 项目总览

MERIVUS 是面向多无人机协同作业的地面站与调度系统。当前阶段以 QGroundControl Custom Build 为基础，逐步扩展多机调度 UI、本机 AI Agent、结构化建议、安全策略、4G/TCP 链路展示、RTK 状态和后续云端/网关能力。

## 当前已完成

- QGC/MERIVUS 主界面和多机调度相关 UI 基线。
- 本机 Local Agent，支持 Mock 与 Ollama `qwen3:8b` Provider。
- QGC C++ `AiAgentClient` 和 `AiServiceSupervisor`。
- Provider Ready/Error/Models 状态展示和 Provider 运行时选择。
- `ActionProposal` schema、本地风险与策略评估。
- 本地模型输出 normalizer 和问答/指令分离。
- Agent Windows onedir 打包 POC。

## 当前明确未完成

- AI 真实飞行动作执行。
- 云 Provider、MCP、命令执行器。
- Device Gateway、Cloud API、Web Console、GIS Safety Service、Media Service 的生产实现。
- 真实飞机自动化测试。

## 安全原则

LLM/Agent 不能直接控制无人机。所有模型输出只能作为 `reply` 或 `ActionProposal` 进入 QGC，本地 C++ 必须重新做 schema 校验、风险计算和策略判定。当前所有 proposal 均保持 `executable=false`。

## 未来模块

- `MerivusGroundControl`：地面站客户端。
- `Merivus Local Agent`：本机 AI/规则服务。
- `Device Gateway`：设备接入和链路网关。
- `Cloud API`：账号、组织、设备、任务、审计和遥测。
- `Web Console`：Web 管理控制台。
- `GIS Safety Service`：地理围栏、地形和禁飞区分析。
- `Media Service`：视频链路、录像和回放。
- `PX4 Flight Stack`：飞控执行端。
