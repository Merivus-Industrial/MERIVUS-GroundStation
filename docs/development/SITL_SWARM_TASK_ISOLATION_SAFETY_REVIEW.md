# 六机编队、多机动作与实时任务隔离安全评审

## Command

- Intent/action: 固定六机编队启动；批量起飞/降落/标准返航；普通右键 goto；Shift 临时多航点 Mission。
- Risk class: 高。
- Target source: 地图框选或排他单选。
- Frozen target system IDs: 编队固定为 1～6；Shift 任务在首个航点时冻结。

## Preconditions

- [x] Link and telemetry freshness
- [x] Unique system IDs
- [x] Position and RTK quality（本次校验有效位置；RTK 质量仍由飞控健康检查负责）
- [x] Flight mode/capability
- [x] Armed state and altitude
- [ ] Battery/geofence/mission constraints（沿用 Vehicle/PX4 检查，尚未形成统一策略层）

## Lifecycle

- [x] Requested/validated/confirmed
- [x] Queued/sent
- [x] ACK/rejection（临时 Mission 上传/清理具备 ACK；编队启动/停止命令由 PX4 校验协议参数并返回 ACK）
- [x] Executing/completed（临时 Mission 使用末航点、距离和速度判断；编队命令 ACK 仅代表飞控已接收，当前仍没有独立的逐机“编队已建立/轨迹已完成”状态消息）
- [x] Timeout/cancel/retry
- [x] Partial-result reconciliation

## AI boundary

- [x] Versioned structured schema（不适用 AI；人工 QML 到固定 C++ API）
- [x] No free-form MAVLink fields
- [x] Ambiguity rejected
- [x] Coordinates/units validated
- [x] Deterministic preview and operator confirmation
- [ ] Injection, replay, and bypass tests

## Verification

- Mock/SITL scenario: 六机启动、批量起飞/降落/标准返航、跨机独立任务、Shift 替换、完成清理。
- Fault injection: 缺少 UAV、重复 ID、主机 3 秒无位置、从机断链、Mission 上传/清理失败。
- Audit record: 当前为 UI 结果与日志；尚未接统一持久审计。
- Rollback/abort: “结束编队”停止转发并让在线成员悬停；普通右键即时抢占临时 Mission。
- Reviewer decision: 操作者确认六机目标后允许默认使用版本 `1` 编队协议；编队与临时任务不再依赖环境变量。配套 `px4_fmu-v6c_default` 已纳入 `swarm_node`，但代码审计不等于实机链路已经验证。

## Remaining risks

- 编队使用标准 MAV_CMD ACK，但尚无事务 ID、能力协商或独立完成状态；“已接受”不能等同于已起飞或已建立队形。
- 主机失联时 QGC 会下发停止；若 QGC 同时失联，从机依靠机载 3 秒 `FOLLOW_TARGET` 超时转 AUTO_LOITER。该保护仍需 SITL 故障注入验证。
- 临时 Mission 完成判定使用末航点距离和地速阈值，仍需 SITL 实测校准。
- 配套 PX4 源码位于 `E:\MERIVUS\FirmwarePX4\PX4\PX4-Autopilot`；已具备版本化命令、ACK 和机载目标超时保护，实机发布前仍需补齐逐机能力/运行状态上报、SITL 故障注入和现场人工验收。
