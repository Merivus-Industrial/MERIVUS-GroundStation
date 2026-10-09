# FTC 后端验证

`FtcBackendTest.pro` 直接编译生产的 `VehicleFtcStatusFactGroup.cc/.h`，验证消息解码、状态属性、电机模型和过期通知。`standalone/FactGroup.h` 只隔离应用的 Fact 注册表；不替代 FTC 后端实现。这些测试不等同于完整 Vehicle、QML 渲染或控制链验证。

使用项目 Qt/MSVC 环境，在独立产物目录执行：

```text
qmake E:/MERIVUS/GroundStation/test/FTC/FtcBackendTest.pro
jom -j4
release/ftc-backend-test.exe
```

8 个 Qt Test 结果项包含初始化与清理，实际有 6 个测试方法。测试覆盖无数据、无效模型、不可观测、有效/退化/故障、历史值、3 秒停流、恢复及跨 stream 协议兼容性。

实时探针：

```text
release/ftc-backend-test.exe --live --output <新的完整.jsonl路径>
```

探针监听 localhost UDP 14655，运行 300 秒，记录真实 MAVLink CRC、心跳和生产 FTC 后端属性。Firmware 的 `Tools/merivus/ftc_validation/mavlink_bridge.py` 可同时把同一份 SITL 数据送入本探针与实际地面站 14550。探针只观察，不发送飞行命令。
