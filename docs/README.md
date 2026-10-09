# GroundStation 工程文档

这组文档描述当前代码，不保存阶段性猜测。首次接手先读：

1. [项目状态](PROJECT_STATUS.md)
2. [系统图](architecture/SYSTEM_MAP.md)
3. [代码责任边界](architecture/CODE_OWNERSHIP_MAP.md)
4. [数据流](architecture/DATA_FLOWS.md)
5. [构建说明](development/BUILD.md)
6. [测试矩阵](testing/TEST_MATRIX.md)

FTC 遥测链路的协议、后端和界面约束见 [FTC 遥测契约](architecture/FTC_TELEMETRY_CONTRACT.md)。现有 AI、硬件和发布文档仍由 [文档索引](INDEX.md) 汇总。

FTC 的跨仓验证结论由 `E:/MERIVUS/FirmwarePX4/docs/testing/FTC_VALIDATION_SUMMARY.md` 统一维护；本仓只保留协议、后端、UI 与测试入口。

文档中的“已实现”必须能落到源码、配置或可重复检查。“计划”“候选”“推断”会单独标明，不能当成飞行能力承诺。
