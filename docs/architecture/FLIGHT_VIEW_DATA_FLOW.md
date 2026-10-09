# Fly View 数据流

## 页面装配

```text
custom/res/Merivus/MainRootWindow.qml
  └─ QGroundControl/FlightDisplay/FlyView.qml
      ├─ FlyViewWidgetLayer
      └─ FlyViewMap.qml（custom/qgroundcontrol.qrc 覆盖）
          └─ CommandCenterOverlay.qml
              ├─ 人工控制参数
              ├─ ESC/电机卡片
              ├─ FtcStatusPanel.qml
              ├─ 无人机列表
              └─ 视频停靠区域
```

## 焦点飞行器

`CommandCenterOverlay` 优先使用已选列表中的第一架飞行器；没有选择时使用 `activeVehicle`。右侧面板可单独选择 `rightVehicle`。FTC 面板和电机卡片跟随 `focusVehicle`，切换焦点后不应继续显示上一架飞机的对象。

## 电机卡片

四个紧凑卡片继续使用 `vehicle.escStatus` 显示转速和在线状态；如果当前电机有新鲜的 FTC 数据，再显示健康 `H`、效能 `E` 和严重度边框。FTC 不可用时显示 `H -- · E --`，ESC 数据保持原样。

悬浮提示合并两类信息：ESC 的转速、电流、电压、温度，以及 FTC 的健康、效能、故障概率、置信度和故障分类。提示明确写出健康值不是剩余寿命。

## FTC 面板

紧凑区显示系统状态、控制模式、最小姿态裕度、失控状态和恢复状态。详情弹窗显示最多 12 个电机、控制轴裕度、撞击/失控分数和低频诊断。

面板对 `OBSERVE`、`SHADOW`、`CANDIDATE` 明确提示“未向执行器下发 FTC 命令”。只有收到未来协议明确声明的 `ACTIVE` 才能显示已接入控制；当前固件不会发送该状态。
