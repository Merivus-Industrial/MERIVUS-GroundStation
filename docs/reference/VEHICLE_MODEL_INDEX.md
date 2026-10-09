# Vehicle 模型索引

| QML 属性 | C++ 类型 | 主要数据 | 备注 |
| --- | --- | --- | --- |
| `vehicle.gps` | `VehicleGPSFactGroup` | 主 GPS | 定位与卫星数 |
| `vehicle.gps2` | `VehicleGPS2FactGroup` | 第二 GPS | 依固件消息可用 |
| `vehicle.wind` | `VehicleWindFactGroup` | 风速风向 | FactGroup |
| `vehicle.vibration` | `VehicleVibrationFactGroup` | 三轴振动 | FactGroup |
| `vehicle.temperature` | `VehicleTemperatureFactGroup` | 温度 | FactGroup |
| `vehicle.hygrometer` | `VehicleHygrometerFactGroup` | 温湿度 | 产品扩展 |
| `vehicle.escStatus` | `VehicleEscStatusFactGroup` | ESC 信息与状态 | 当前紧凑卡片使用前四个 ESC |
| `vehicle.ftcStatus` | `VehicleFtcStatusFactGroup` | FTC 四消息族 | 只读；带协议和过期语义 |
| `vehicle.estimatorStatus` | `VehicleEstimatorStatusFactGroup` | 估计器状态 | FactGroup |
| `vehicle.terrain` | `TerrainFactGroup` | 地形状态 | 与 TerrainProtocolHandler 配合 |
| `vehicle.batteries` | `QmlObjectListModel` | 多电池 | 列表模型 |

FTC 常用属性：

- 总体：`enabled`、`available`、`lastUpdate`、`stale`、`protocolVersion`、`protocolCompatible`。
- 消息族：`motorAvailable/motorStale`、`controlAvailable/controlStale`、`extremeAvailable/extremeStale`、`diagnosticsAvailable/diagnosticsStale`。
- 电机：`motorCount`、`motors`；角色含 `health`、`effectiveness`、`faultProbability`、`confidence`、`faultTypeText`、`degraded`、`failed`、`available`、`severity`。
- 控制与事件：`controlModeText`、各轴 authority、`impactTypeText`、`locStateText`、`recoveryStateText`。

新增 Vehicle 属性前先看 [变更影响指南](../development/CHANGE_IMPACT_GUIDE.md)。
