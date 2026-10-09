# QML 组件索引

| 组件 | 运行时职责 | 数据入口 |
| --- | --- | --- |
| `custom/res/Merivus/MainRootWindow.qml` | 产品主窗口与页面装配 | QGC 全局对象 |
| `src/FlightDisplay/FlyView.qml` | 飞行页根组件 | Active Vehicle、视频、地图 |
| `custom/res/Merivus/FlyViewMap.qml` | 产品飞行地图与叠加层 | 地图、Vehicle、Guided 控制器 |
| `custom/res/Merivus/CommandCenterOverlay.qml` | 人工操作侧栏、ESC/FTC 卡片、多机列表 | `focusVehicle`、`rightVehicle` |
| `custom/res/Merivus/FtcStatusPanel.qml` | FTC 摘要与详情弹窗 | `vehicle.ftcStatus` |
| `custom/res/Merivus/FtcStatusPalette.qml` | FTC 严重度到主题色 | `QGCPalette` |
| `custom/res/Merivus/MerivusToolTip.qml` | 产品悬浮提示 | 调用方文本 |
| `custom/res/Merivus/FlyViewWidgetLayer.qml` | Fly View 工具层 | 飞行控制与检查单 |
| `custom/res/Merivus/MerivusAIAssistantPanel.qml` | AI 问答与建议面板 | Agent client / supervisor |
| `custom/res/Merivus/MainToolBar.qml` | 产品工具栏 | 全局状态与 Vehicle |

同名 QML 可能同时存在于 `src/` 和 `custom/`。运行时版本以 `custom/qgroundcontrol.qrc` 的资源别名为准。
