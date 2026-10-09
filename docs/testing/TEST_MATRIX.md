# 测试矩阵

| 层级 | 场景 | 方法 | 通过条件 | 当前状态 |
| --- | --- | --- | --- | --- |
| 协议 | XML 合法、单位、ID | `mavgen.py --strict-units` | 两个包装方言均生成成功 | 已通过 |
| 协议 | 两仓核心一致 | SHA-256 / 契约脚本 | 哈希均为 `0ae936...064d` | 已通过 |
| 协议 | 生成物可复现 | `generate-merivus-mavlink.ps1 -Check` | 文件列表与内容一致 | 已通过 |
| 静态闭环 | 字段—属性—QML | `test-ftc-telemetry-contract.ps1` | 必需属性和消费入口齐全，QML 无原始解码 | 已通过 |
| 固件静态 | 9 个 uORB 源与安全模式 | `Tools/merivus/verify_ftc_telemetry.py` | 主题齐全，不报告 ACTIVE | 已通过 |
| C++ | GroundStation 编译 | Qt 5.15.2 / MSVC 2019 Release | `VehicleFtcStatusFactGroup` 和生成头无编译错误 | 已通过（2026-09-02，全量 Release） |
| QML | lint | 对新增和改动 QML 运行 `qmllint` | 无语法、未解析属性和导入错误 | 待执行 |
| 固件 | SITL 编译 | `make px4_sitl_default` | `merivus` 方言生成并链接 | 待执行 |
| 固件 | FMUv6C 编译 | `make px4_fmu-v6c_default` | 固件体积与编译通过 | 待执行 |
| SITL | 四消息频率 | MAVLink Inspector / 抓包 | 约 5/5/10/1 Hz，允许调度器按带宽降频 | 待执行 |
| SITL | 模式语义 | 启停监测、影子分配、恢复候选 | 只出现 DISABLED/OBSERVE/SHADOW/CANDIDATE | 待执行 |
| 回放 | 百分比边界 | 构造 0、1、200、255 | 显示 0%、0.5%、100%、N/A | 待执行 |
| 回放 | 协议版本不匹配 | 发送 `protocol_version != 2` | 不解释字段，显示版本不兼容 | 待执行 |
| 回放 | 断流 | 停止各消息族超过 3 秒 | 对应局部数据变 N/A，不被其他消息刷新 | 待执行 |
| 兼容 | 非 FTC 飞控 | 连接标准 PX4/QGC 方言 | 飞行、ESC 与其他 UI 正常；FTC 显示不可用 | 待执行 |
| UI | 主题与分辨率 | 深浅主题，常用桌面尺寸 | 状态颜色可辨，面板不遮挡关键操作 | 待执行 |
| UI | 焦点切换 | 多机快速切换选择 | 不残留上一架飞机数据 | 待执行 |
| 硬件 | TELEM1 带宽 | FMUv6C + 57,600 baud 链路 | 无持续队列拥塞，关键标准消息延迟可接受 | 待执行 |
| 硬件 | 故障注入 | 安全台架或受控飞行方案 | 状态、掩码、恢复候选与原始日志一致 | 待执行 |

真实飞行测试必须单独评审风险、场地、失效处置和人工接管条件。静态检查、SITL 和台架结果不能直接替代飞行放行。
