# MERIVUS 文档索引

## 版本与发布

- [版本变更记录](releases/CHANGELOG.md)
- [`0.1.0-dev.1` 阶段性初版](releases/0.1.0-dev.1.md)

## 项目与架构

- [项目总览](architecture/PROJECT_OVERVIEW.md)
- [当前状态](architecture/CURRENT_STATE.md)
- [仓库结构](architecture/REPO_STRUCTURE.md)
- [目标架构](architecture/TARGET_ARCHITECTURE.md)
- [接口边界](architecture/INTERFACE_CONTRACTS.md)
- [安全边界](architecture/SAFETY_BOUNDARIES.md)
- [路线图](architecture/ROADMAP.md)
- [风险登记](architecture/RISK_REGISTER.md)

## AI / Agent

- [Local Agent HTTP](architecture/LOCAL_AGENT_HTTP.md)
- [QGC Agent Client](architecture/QGC_AGENT_CLIENT.md)
- [Agent Supervisor](architecture/AGENT_SUPERVISOR.md)
- [Agent Packaging POC](architecture/AGENT_PACKAGING_POC.md)
- [Agent Provider Settings](architecture/AGENT_PROVIDER_SETTINGS.md)
- [Agent Model Providers](architecture/AGENT_MODEL_PROVIDERS.md)
- [AI Intent Policy](architecture/AI_INTENT_POLICY.md)
- [AI Model Stability](architecture/AI_MODEL_STABILITY.md)
- [AI 问答与指令分离](architecture/AI_QA_INTENT_SEPARATION.md)
- [AI Assistant 使用说明](MERIVUS_AI_ASSISTANT.md)

## 开发

- [Windows 构建](development/BUILD_WINDOWS.md)
- [Agent 开发](development/AGENT_DEVELOPMENT.md)
- [AI GUI 手工烟测清单](development/AI_GUI_SMOKE_CHECKLIST.md)
- [仓库整理报告](development/REPO_CLEANUP_REPORT.md)
- [Git 工作流](development/GIT_WORKFLOW.md)
- [GitHub 仓库规范](development/GITHUB_REPO_GUIDELINES.md)

## 硬件

- [硬件目录说明](hardware/README.md)
- [7 寸原型机](hardware/AIRFRAME_7INCH_PROTOTYPE.md)
- [重量与电源预算](hardware/WEIGHT_AND_POWER_BUDGET.md)

## 归档

以下文档保留历史参考价值，但不再作为当前主入口：

- [旧版 Windows 构建说明](archive/BUILD_WINDOWS_LEGACY.md)
- [旧版开发流程说明](archive/DEVELOPMENT_WORKFLOW_LEGACY.md)
- [旧版环境准备说明](archive/ENVIRONMENT_SETUP_LEGACY.md)
- [旧版 Linux SITL 准备说明](archive/SITL_LINUX_SETUP_LEGACY.md)
- [PX4 v1.14 SITL 历史笔记](archive/PX4_114_SITL_MERIVUS_NOTES.md)

## 仓库卫生

本仓库不提交厂商 PDF、真实凭据、模型文件或构建产物。本地手册可放在 `docs/hardware/local-manuals/`，该目录下 PDF 默认被 `.gitignore` 排除。
