# 项目状态

更新：2026-09-17。最终分支 `codex/ftc-full-validation`，收口前源码提交 `e7444f32b2f81c699e8b881a1262ca8f1945f5ee`。

FTC 已同步 v2 extensions，typed backend 区分模型无效和消息过期，主 H/E 不显示无效历史估计。主界面保持简洁，估计内部诊断放在详情；ACTIVE 仅根据固件实际反馈显示。参见 [协议契约](architecture/FTC_TELEMETRY_CONTRACT.md)。

Qt Test 共 8 个结果项通过，Windows Qt 5.15.2/MSVC Release 构建通过。实际 VM—Windows 链路解码 26092 个包且 CRC 错误为 0，已验证 3 秒过期与恢复；这不构成绝对零丢包证明。FTC 详情面板的最终视觉样式、多机布局和目标硬件链路仍需复核。跨仓结论见 `E:/MERIVUS/FirmwarePX4/docs/testing/FTC_VALIDATION_SUMMARY.md`。
