# MERIVUS 当前状态审计

审计时间：2026-07-06
审计分支：`docs/project-audit`
审计范围：阶段 0，仅检查仓库、构建、硬件文档和架构边界；未修改业务代码。

## 已经验证

- 当前仓库分支从 `codex/ai-assistant-bubble` 创建为 `docs/project-audit`。
- `git status` 显示进入审计前已有未提交改动：
  - `custom/res/Merivus/MerivusAIAssistantPanel.qml`
  - `custom/res/Merivus/ai-nine-star.svg`
  - 未跟踪目录 `docs/hardware/`
- 最近提交：
  - `c7cee1b Refine AI assistant panel layout`
  - `a4c7326 Enhance Merivus AI assistant panel`
  - `adad006 Add Merivus AI assistant bubble`
  - `026e8d0 完善 SITL 安全设置与地图指令交互`
  - `b3d6659 Initial MERIVUS cross-platform source baseline`
- 根目录没有 `AGENTS.md` 文件；本次遵循用户在任务中提供的全局协作规则。
- Windows 环境检查通过：
  - Qt 5.15.2 / `win32-msvc`
  - Visual Studio 2022 x64 工具链
  - `qmake`、`jom`、Python、Git 均可用
- 最小构建验证通过：
  - 命令：`.\tools\dev\build-merivus.ps1 -Configuration Release -Jobs 1`
  - 输出：`build\Desktop_Qt_5_15_2_MSVC2019_64bit-Release\staging\MERIVUS.exe`

## 从代码中确认

- 仓库是 QGroundControl 4.3 风格源码加 `custom` 定制层的混合工程，不是独立 QML 小项目。
- qmake 仍是主要 Windows 构建入口：
  - `qgroundcontrol.pro`
  - `custom/custom.pri`
  - `tools/dev/build-merivus.ps1`
  - `tools/dev/build-windows.ps1`
- `custom/custom.pri` 已将产品定制为 MERIVUS：
  - `TARGET = MERIVUS`
  - `QGC_APPLICATION_NAME = "MERIVUS"`
  - `QGC_ORG_DOMAIN = "com.merivus"`
  - 注册 `CustomPlugin` 和 `SwarmController`
- 程序入口仍沿用 QGC 原生主程序和资源加载机制，`custom/qgroundcontrol.qrc` 覆盖主 QML 资源。
- 主窗口 QML：`custom/res/Merivus/MainRootWindow.qml`
- 自定义顶栏：`custom/res/Merivus/MainToolBar.qml`
- 多机/指挥中心面板：`custom/res/Merivus/CommandCenterOverlay.qml`
- 自定义地图：`custom/res/Merivus/FlyViewMap.qml`
- AI 面板：`custom/res/Merivus/MerivusAIAssistantPanel.qml`
- 多机控制 C++：`custom/src/Swarm/SwarmController.cc`
- Link 配置仍主要复用 QGC 原生：
  - `src/ui/preferences/LinkSettings.qml`
  - `src/ui/preferences/TcpSettings.qml`
  - `src/comm/TCPLink.cc`
  - `src/comm/LinkManager.cc`
  - `src/comm/LinkConfiguration.cc`
- Vehicle 管理仍主要复用 QGC 原生：
  - `src/Vehicle/MultiVehicleManager.cc`
  - `src/Vehicle/Vehicle.cc`
  - `src/Vehicle/VehicleLinkManager.cc`
- Mission 管理仍主要复用 QGC 原生：
  - `src/MissionManager/MissionManager.cc`
  - `src/MissionManager/MissionController.cc`
  - `src/MissionManager/PlanMasterController.cc`
- 视频能力仍主要复用 QGC 原生：
  - `src/VideoManager/VideoManager.cc`
  - `src/VideoReceiver/*`
  - `src/Settings/VideoSettings.cc`
  - `src/FlightDisplay/FlyViewVideo.qml`
- GIS/地形/地图能力当前主要是 QGC 原生地图、地形和任务可视化能力；尚未发现独立 MERIVUS GIS Safety Service。

## 当前已经实现的功能

- MERIVUS 品牌化构建目标和自定义资源入口。
- 自定义主窗口、顶栏、飞行视图地图、飞行视图组件层。
- 多机显示与选择：
  - 使用 `QGroundControl.multiVehicleManager.vehicles`
  - 可设置 `QGroundControl.multiVehicleManager.activeVehicle`
- 指挥中心面板显示遥测、链路、电池、GPS、ESC 等信息。
- 指挥中心面板可对焦点飞行器下发高度、速度、爬升相关命令。
- `SwarmController` 支持：
  - 多机选择
  - `executeGoto`
  - `executeQueuedGoto`
  - 临时任务上传后启动任务
  - 一类 legacy MAVLink 转发逻辑
- AI 悬浮面板已具备：
  - 聊天 UI
  - 本地规则模式
  - 可配置 Agent endpoint
  - QML `XMLHttpRequest` 调用 `http://127.0.0.1:8765/merivus/agent`
  - 对 `takeoff`、`land`、`rtl`、`pause` 的简易 intent 白名单
  - 用户确认后调用 `Vehicle` 接口执行动作

## 尚未实现或未发现的功能

- 独立 Python/FastAPI Agent 目录或可运行服务。
- C++ `AiAgentClient`。
- `QProcess` Agent 进程监管。
- 统一 `ActionProposal` C++ 数据结构。
- 独立 Schema validator。
- 本地 `AiCommandPolicy` 风险重算。
- 操作审计日志。
- 只读/低风险第一版 AI 能力边界。
- 设备网关、云端 API、Web Console、Media Service、GIS Safety Service。
- 多机 4G 设备身份、控制权租约、遥测分发、限流和认证。
- RTK 状态的 MERIVUS 专用显示与 Hyper982 配置归档。
- Windows 发布安装包和 Agent 打包。

## 当前仓库类型判断

当前仓库属于“QGC 原生源码 + custom 目录扩展 + 部分自定义 QML/C++ 覆盖”的混合修改。

判断依据：

- 保留完整 `src/`、`libs/`、`resources/`、`qgroundcontrol.pro` 等 QGC 结构。
- `custom/custom.pri` 注入 `CustomPlugin`、MERIVUS 应用名和自定义资源。
- `custom/qgroundcontrol.qrc` 覆盖 `MainRootWindow.qml`、`FlyViewMap.qml`、`GuidedActionsController.qml` 等 QGC QML 资源。
- 自定义 `SwarmController` 已进入 C++ 构建。

## 当前架构问题

- AI 面板网络通信在 QML 中直接使用 `XMLHttpRequest`，与目标架构中“QML 只负责显示，C++ 使用 `QNetworkAccessManager` 通信”不一致。
- AI 面板已经允许用户确认后直接调用起飞、降落、返航、暂停，早于目标边界中“第一版禁止自动解锁/自动起飞，先只做低风险建议”的安全收敛顺序。
- `SwarmController` 已经能上传临时任务并启动任务，且存在 legacy MAVLink 包发送逻辑，安全边界需要重新梳理。
- 指挥中心 UI 中存在直接下发高度、速度、爬升命令的入口，尚未看到统一白名单、审计和风险分级。
- 顶栏 TCP 状态目前显示“未配置”，尚未对接真实 Link 配置状态。
- 4G TCP 地址与端口没有发现硬编码在业务代码中，这是好现象；但也没有 MERIVUS 专用配置/诊断层。
- 架构文档此前不完整，硬件、接口、安全边界、路线图和风险清单缺少统一落点。

## 用户描述但尚未验证

- 真实 HyperLTE 链路曾经通过 `119.45.168.211:62178` 连接 QGC。
- 无桨叶实机测试中 QGC 可接收飞控数据、下发简单命令，电机有响应。
- 当前无人机没有安装机载电脑。
- 项目目标是基于 QGroundControl 4.3 和 PX4 v1.14.0。

## 当前假设

- 现有 `custom/res/Merivus/*` 是当前 MERIVUS 主要业务定制区域，后续优先在 `custom` 和独立 `agent/`、`backend/` 目录扩展。
- 当前阶段不应把设备网关、AI Agent、云端服务直接塞进 QGC 源码树的 `src/` 内。
- 真实飞行相关功能后续必须使用 Mock、Unit Test、PX4 SITL 或 MAVLink 回放验证。

## 待确认事项

- QGC 4.3 的确切上游 commit 或发布标签。
- `codex/ai-assistant-bubble` 分支上的未提交 QML/SVG 改动是否由用户保留、提交或另行整理。
- `docs/hardware/` 是否应纳入 Git 跟踪，或仅作为本地资料。
- 是否允许第一阶段先禁用/收敛 AI 面板和 `SwarmController` 中的高风险执行入口。
- 真实 4G 数传端口是否仍为 `62178`，是否存在多张卡、多设备端口分配规则。
