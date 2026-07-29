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
- [x] ACK/rejection（临时 Mission 上传/清理具备 ACK；旧编队启动包无正式 ACK）
- [x] Executing/completed（临时 Mission 使用末航点、距离和速度判断；旧编队缺少正式完成状态）
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
- Reviewer decision: 操作者确认六机目标后允许默认使用现有兼容协议；编队与临时任务不再依赖环境变量。地面站改动不等于机载编队算法或实机链路已经验证。

## Remaining risks

- 旧编队复用标准 `GPS_RAW_INT` 作为非标准兼容消息，没有事务 ID、正式 ACK 或完成消息。
- 主机失联时，QGC 只能命令仍在线的从机悬停；若 QGC 与机群同时失联，最终保护依赖机载 swarm/PX4 实现。
- 临时 Mission 完成判定使用末航点距离和地速阈值，仍需 SITL 实测校准。
- 当前仓库没有配套 PX4 SWARM 接收端和正方形队形算法源码；实机发布前仍需审计机载实现，并补齐版本化命令、逐机能力协商、ACK 和机载失联保护。
