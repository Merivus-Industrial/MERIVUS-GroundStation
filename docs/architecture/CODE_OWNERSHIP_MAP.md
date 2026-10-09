# 代码责任边界

这里的“责任方”指长期维护位置，不指个人。

| 区域 | 唯一职责 | 主要入口 | 不应承担 |
| --- | --- | --- | --- |
| `libs/mavlink/.../merivus*` | 生成后的 MAVLink C 头文件 | `merivus/mavlink.h` | 手工定义业务语义 |
| `schemas/mavlink` | GroundStation 方言源文件 | `merivus.xml`、`merivus_ftc.xml` | UI 文案、解码逻辑 |
| `src/MAVLinkProtocol` 与 Link 层 | 字节流解析、消息分发 | `MAVLinkProtocol::messageReceived` | FTC 业务状态 |
| `src/Vehicle/Vehicle` | 单飞行器对象、FactGroup 分发 | `_mavlinkMessageReceived` | 在主处理函数堆 FTC 特判 |
| `VehicleFtcStatusFactGroup` | FTC 解码、版本、过期、文本、列表模型 | `handleMessage` | 控制飞行器、推断剩余寿命 |
| `VehicleEscStatusFactGroup` | 标准 ESC 消息解码 | `ESC_INFO`、`ESC_STATUS` | FTC 故障分类 |
| `custom/res/Merivus` | 产品 Fly View 与交互 | `CommandCenterOverlay.qml` | 原始 MAVLink 解码 |
| `FtcStatusPalette.qml` | FTC 严重度到 QGC 调色板的映射 | `colorFor` | 重新判断系统状态 |
| `tools/dev` | 构建、生成和静态契约检查 | `generate-merivus-mavlink.ps1` | 保存本机绝对环境配置 |
| `docs` | 当前契约、入口和验证方法 | `docs/README.md` | 复制源码或构建产物 |

跨仓协议源文件在 FirmwarePX4 和 GroundStation 各保存一份，原因是两仓都要独立构建。两份 `merivus_ftc.xml` 必须逐字节一致，契约检查以 SHA-256 阻止漂移。
