# MAVLink 索引

| 消息或组件 | GroundStation 入口 | 用途 |
| --- | --- | --- |
| `HEARTBEAT` | `Vehicle` / 固件插件 | 发现飞行器、模式和状态基础 |
| `SYS_STATUS` | `Vehicle` 及 FactGroups | 系统与传感器状态 |
| `ATTITUDE`、`ATTITUDE_QUATERNION` | `Vehicle` | 姿态显示 |
| `GLOBAL_POSITION_INT`、`LOCAL_POSITION_NED` | `Vehicle` | 地图与位置显示 |
| `GPS_RAW_INT`、`GPS2_RAW` | `VehicleGPS*FactGroup` | 定位状态 |
| `BATTERY_STATUS` | `VehicleBatteryFactGroup` | 电池模型 |
| `ESC_INFO`、`ESC_STATUS` | `VehicleEscStatusFactGroup` | 前四个 ESC 的接口、在线、转速、电参和温度 |
| `ESTIMATOR_STATUS` | `VehicleEstimatorStatusFactGroup` | 估计器状态 |
| `MERIVUS_FTC_MOTOR_STATUS` 60000 | `VehicleFtcStatusFactGroup` | 电机健康、效能与故障观测 |
| `MERIVUS_FTC_CONTROL_STATUS` 60001 | 同上 | 控制裕度与 FTC 集成阶段 |
| `MERIVUS_FTC_EXTREME_STATUS` 60002 | 同上 | 撞击、失控与恢复候选 |
| `MERIVUS_FTC_DIAGNOSTICS` 60003 | 同上 | 低频工程诊断与 SITL 注入状态 |

方言入口为 `libs/mavlink/include/mavlink/v2.0/merivus/mavlink.h`。生成源与兼容规则见 [FTC 遥测契约](../architecture/FTC_TELEMETRY_CONTRACT.md)。
