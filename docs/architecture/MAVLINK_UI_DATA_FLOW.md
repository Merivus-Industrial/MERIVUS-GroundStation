# MAVLink 到 UI 的数据流

## 分层

| 层 | 输入 | 输出 | 责任 |
| --- | --- | --- | --- |
| 传输层 | 串口、UDP、TCP 字节 | `mavlink_message_t` | 解帧、校验、链路统计 |
| Vehicle 分发 | `mavlink_message_t` | 各管理器和 FactGroup 调用 | 维持统一消息入口 |
| FTC 后端 | `60000..60003` | 类型化 Qt 属性、列表模型 | 版本、缩放、过期、枚举文本 |
| QML | `vehicle.ftcStatus` | 卡片、颜色、详情 | 展示与交互，不解码协议 |

`VehicleFtcStatusFactGroup` 只挂到既有 FactGroup 集合。这样新增消息不需要在 `Vehicle::_mavlinkMessageReceived` 中维护第二份路由表。

## 可用性

- `available=false`：没有兼容的 FTC 协议数据。非 FTC 飞控走这条路径，其他 GroundStation 功能不受影响。
- `protocolCompatible=false`：已经收到 FTC 消息，但 `protocol_version` 不是 2。界面明确显示版本不兼容。
- `stale=true`：曾收到兼容数据，但所有已出现消息族都超过 3 秒未更新。
- `motorStale`、`controlStale`、`extremeStale`、`diagnosticsStale`：分别控制局部数据显示，不能用一个新消息掩盖另一族过期。

## 数值规则

后端把紧凑百分比解码为 `0..100`。线路值 `255` 转成 `-1` 作为内部不可用标记；QML 统一显示 `--` 或 `N/A`，不把它格式化成 `-1%`。健康分数是观测量，不是剩余寿命。

## 状态与颜色

枚举到中文文本和严重度的映射在 C++ 后端集中维护。QML 只接收 `normal`、`warning`、`critical`、`unavailable`，再由 `FtcStatusPalette.qml` 映射到 `QGCPalette`。界面不根据百分比自设告警阈值。
