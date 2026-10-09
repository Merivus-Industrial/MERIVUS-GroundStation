# 变更影响指南

## 先判断入口

| 要改的内容 | 先查 | 连带检查 |
| --- | --- | --- |
| MAVLink 字段或枚举 | `schemas/mavlink` | 固件 XML、生成头、发送流、后端、契约文档 |
| 消息频率 | FirmwarePX4 `mavlink_main.cpp` | TELEM1 带宽、过期阈值、测试矩阵 |
| Vehicle 属性 | `Vehicle.h/.cc` | 构造顺序、FactGroup 注册、QML 绑定、单元测试 |
| FTC 文本或严重度 | `VehicleFtcStatusFactGroup.cc` | 面板、工具提示、翻译提取 |
| FTC 颜色 | `FtcStatusPalette.qml` | 深浅主题、告警对比度 |
| Fly View 组件 | `custom/qgroundcontrol.qrc` | 实际资源别名、不同分辨率、触控区域 |
| 电机卡片 | `CommandCenterOverlay.qml` | ESC 无数据、FTC 无数据、焦点切换、12 电机详情 |

## 高影响文件

GitNexus 把 `Vehicle` 评为 CRITICAL：426 个节点可能受影响，其中 91 个为直接影响。它是全局飞行器模型，不宜加入协议特判。本轮只增加一个成员、一个 QML 属性和一次 FactGroup 注册，消息解析留在独立类中。

QML、Qt 信号槽、元对象属性和资源别名不能由静态图完整还原。改这些边界时需要源码核对与运行测试，不能用“GitNexus 未发现调用者”证明安全。

## 最小验证

- 协议：生成器严格校验、两仓哈希、长度/CRC 检查。
- 后端：消息解码测试、版本不匹配、3 秒过期、百分比 `255`。
- QML：无 FTC 飞控、断流、协议不兼容、主题、分辨率、焦点切换。
- 固件：SITL 消息频率、带宽统计、FMUv6C 编译、真实 TELEM1 链路。
