# MERIVUS 文档索引

文档只维护当前实现、稳定契约和可重复操作。版本历史由 Git、发布记录和 PR 保存，不在仓库中复制源码、构建产物或阶段性排障快照。

## 接手入口

- [工程交接入口](handoff/README.md)
- [项目总览](architecture/PROJECT_OVERVIEW.md)
- [当前状态](architecture/CURRENT_STATE.md)
- [仓库结构](architecture/REPO_STRUCTURE.md)
- [风险登记](architecture/RISK_REGISTER.md)

## 架构与安全契约

- [接口边界](architecture/INTERFACE_CONTRACTS.md)
- [安全边界](architecture/SAFETY_BOUNDARIES.md)
- [硬件集成边界](architecture/HARDWARE_INTEGRATION.md)
- [当前链路基线](architecture/CURRENT_LINK_BASELINE.md)
- [Local Agent HTTP](architecture/LOCAL_AGENT_HTTP.md)
- [QGC Agent Client](architecture/QGC_AGENT_CLIENT.md)
- [Agent Supervisor](architecture/AGENT_SUPERVISOR.md)
- [Agent Provider 设置](architecture/AGENT_PROVIDER_SETTINGS.md)
- [Agent 模型 Provider](architecture/AGENT_MODEL_PROVIDERS.md)
- [AI Intent Policy](architecture/AI_INTENT_POLICY.md)
- [AI 问答与指令分离](architecture/AI_QA_INTENT_SEPARATION.md)
- [AI 模型稳定性](architecture/AI_MODEL_STABILITY.md)

## 开发与验证

- [Windows 构建](development/BUILD_WINDOWS.md)
- [Agent 开发](development/AGENT_DEVELOPMENT.md)
- [AI GUI 手工烟测清单](development/AI_GUI_SMOKE_CHECKLIST.md)
- [PX4 起飞健康检查与自动解锁契约](development/PX4_TAKEOFF_HEALTH_CHECK.md)
- [SITL 多机任务测试](development/SITL_MULTI_VEHICLE_MISSION_TEST.md)
- [SITL 编队任务说明](development/SITL_SWARM_TASK_ISOLATION_FEATURE_BRIEF.md)
- [SITL 编队安全评审](development/SITL_SWARM_TASK_ISOLATION_SAFETY_REVIEW.md)

## 硬件与版本

- [硬件目录说明](hardware/README.md)
- [7 寸原型机](hardware/AIRFRAME_7INCH_PROTOTYPE.md)
- [重量与电源预算](hardware/WEIGHT_AND_POWER_BUDGET.md)
- [版本变更记录](releases/CHANGELOG.md)
- [`0.1.0-dev.1` 阶段说明](releases/0.1.0-dev.1.md)

## 仓库卫生

不要提交 DOCX/PDF、源码全文存档、厂商资料、真实凭据、飞行日志、模型文件、缓存或构建产物。历史资料需要回看时使用 Git 历史；尚未落地的设计进入 Issue 或 ADR，并明确责任边界与验收条件。
