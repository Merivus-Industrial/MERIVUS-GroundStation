# GroundStation 系统图

```text
┌────────────────────────── 飞行器 / PX4 ──────────────────────────┐
│ uORB FTC 主题 → PX4 MAVLink streams → MERIVUS 方言 60000..60003 │
└──────────────────────────────┬────────────────────────────────────┘
                               │ 串口 / UDP / TCP，MAVLink 2
┌──────────────────────────────▼────────────────────────────────────┐
│                         GroundStation                             │
│ LinkInterface → MAVLinkProtocol::messageReceived                  │
│                         │                                         │
│                         ▼                                         │
│ Vehicle::_mavlinkMessageReceived                                  │
│     ├─ FirmwarePlugin / ComponentInformation / managers           │
│     └─ FactGroup::handleMessage                                   │
│          ├─ VehicleEscStatusFactGroup                             │
│          └─ VehicleFtcStatusFactGroup                             │
│                ├─ 协议版本检查                                    │
│                ├─ 3 秒过期判定                                   │
│                ├─ 枚举 → 中文状态与严重度                         │
│                └─ FtcMotorStatusModel（最多 12 电机）             │
│                                                                  │
│ MainRootWindow → FlyView → FlyViewMap → CommandCenterOverlay      │
│                                      ├─ ESC/FTC 电机卡片          │
│                                      └─ FtcStatusPanel / 详情     │
└───────────────────────────────────────────────────────────────────┘

┌──────────────────────── 本机 Agent 边界 ──────────────────────────┐
│ QGC C++ client ↔ Local Agent / Provider                            │
│ 输出只能是 reply 或 ActionProposal；执行前仍由 QGC 策略与人工确认  │
└───────────────────────────────────────────────────────────────────┘
```

## 真实主界面入口

- `custom/res/Merivus/MainRootWindow.qml` 加载 `FlyView`。
- `src/FlightDisplay/FlyView.qml` 是飞行页根组件。
- `custom/qgroundcontrol.qrc` 把产品版 `FlyViewMap.qml` 映射到 QGC 资源路径。
- `custom/res/Merivus/FlyViewMap.qml` 创建 `CommandCenterOverlay`。
- `custom/res/Merivus/CommandCenterOverlay.qml` 是当前人工操作侧栏、电机卡片和 FTC 面板的真实入口。

资源别名决定运行时加载哪份 QML。只搜索同名文件不足以判断真实页面，改动前必须同时核对 `custom/qgroundcontrol.qrc`。
