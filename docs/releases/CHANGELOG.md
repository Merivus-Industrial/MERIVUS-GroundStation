# MERIVUS 版本变更记录

本项目在开发测试阶段采用语义化版本的预发布标识：`主版本.次版本.修订版本-dev.序号`。带 `dev` 的版本只用于开发、联调、SITL 和受控验证，不代表生产可用性。

## [0.1.0-dev.1] - 2026-07-17

阶段性初版（开发测试版）。

### 新增

- MERIVUS 定制地面站主界面、多机调度视图、状态面板和地图交互。
- 全局 AI 悬浮入口与可缩放聊天面板，跨主页面和设置页面保留。
- 本机 FastAPI Agent、Mock Provider 与 Ollama Provider。
- QGC C++ `AiAgentClient`、`AiServiceSupervisor`、Schema 校验与本地命令策略。
- Agent 打包脚本、自动化测试、模型评估样例和开发文档体系。

### 改进

- 优化 AI 面板默认尺寸、自由缩放边缘、悬浮反馈、消息气泡和输入区域。
- 开发环境可自动发现仓库中的 Agent、Python 启动器或已打包 Agent，减少手工环境变量配置。
- 完善中文界面、安全边界、构建流程、硬件约束和仓库维护说明。

### 安全限制

- AI 只返回问答内容与结构化建议，所有 proposal 保持 `executable=false`。
- 当前没有 AI 命令执行器，不允许 LLM 直接调用 Vehicle、MAVLink、PX4 或 Swarm 执行入口。
- 尚未完成真实无人机端到端验证、安装包签名、云端鉴权、GIS Safety Service 与生产部署。

[0.1.0-dev.1]: 0.1.0-dev.1.md
