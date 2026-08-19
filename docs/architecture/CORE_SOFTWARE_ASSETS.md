# GroundStation 与 FirmwarePX4 核心软件资产说明

> 整理日期：2026-08-13
> 项目根目录：`E:\MERIVUS`
> 本文只说明 `GroundStation` 和 `FirmwarePX4` 两项核心软件资产。路径、构建目标和现存产物均按当前本机目录核对；“文件存在”“历史构建成功”和“本次重新验证通过”是三种不同状态，不能混为一谈。

## 1. 一页理解整个软件系统

MERIVUS 软件可以理解为两个互相配合、但分别运行在不同设备上的系统：

```text
Windows 电脑
└─ GroundStation（地面站，总指挥部）
   ├─ 显示地图、飞机状态和视频
   ├─ 配置、任务规划、日志与固件升级
   ├─ 多机编队人工控制界面
   └─ 本机 AI Agent：当前生成建议，不直接控制真实飞机
             │
             │ MAVLink / 数传链路
             ▼
飞控主板
└─ FirmwarePX4（飞控固件，数字大脑）
   ├─ 读取 IMU、气压计、磁力计、GPS/RTK
   ├─ 状态估计、姿态/位置控制和失效保护
   ├─ 生成电机/舵机控制输出
   └─ 运行 MERIVUS 编队节点并与地面站通信
```

两者的根路径分别是：

```text
E:\MERIVUS\GroundStation
E:\MERIVUS\FirmwarePX4
```

注意：`E:\MERIVUS\FirmwarePX4\PX4` 只是外层容器，真正的 PX4 Git 仓库和编译根目录是：

```text
E:\MERIVUS\FirmwarePX4
```

## 2. 先分清源码、构建产物、staging 和发布包

| 名称 | 通俗比喻 | 工程含义 | 是否通常提交 Git |
| --- | --- | --- | --- |
| Source Code | 设计图纸、菜谱 | 人编写的 C++、QML、Python、CMake、配置和文档 | 是 |
| Build Products | 加工过程和半成品 | 编译器生成的 `.obj`、自动生成代码、缓存、符号和可执行文件 | 否 |
| staging | 彩排室、程序集结区 | 把主程序、DLL、插件、Agent 和资源按最终运行结构放在一起 | 否 |
| Installer / Release Package | 正式包装的成品 | 供用户安装、分发和回溯的安装包或固件包 | 作为发布资产保存，不混入源码提交 |

基本数据流：

```text
源码 + 固定工具链 + 构建配置
            ↓
中间构建文件
            ↓
staging 可运行目录
            ↓
测试、签名、打包
            ↓
正式发布包
```

“双击某个 EXE 能启动”并不自动代表它就是正式发布包。正式交付至少还需要：

- 明确对应的 Git 提交；
- 明确工具链与构建参数；
- 完成必要测试；
- 记录 SHA-256；
- 对安装包或程序集进行分发验证；
- 保证发布内容与批准版本同源。

## 3. GroundStation：Windows 地面站“总指挥部”

### 3.1 根目录与当前定位

绝对路径：

```text
E:\MERIVUS\GroundStation
```

它是一个基于 QGroundControl Custom Build 演进的 Qt/QML/C++ 项目，并加入 MERIVUS 多机调度界面、本机 Python Agent、协议约束和 Windows 构建脚本。

当前 Git 身份：

```text
分支：main
提交：9bb0807
远端：origin/main
```

当前工作区在本文整理前是干净状态。上述提交只说明源码身份，不自动证明当前 staging 里的每个二进制都由这个提交重新构建。

### 3.2 GroundStation 顶层结构

```text
E:\MERIVUS\GroundStation
├─ src\                  QGroundControl 主体 C++/QML 源码
├─ custom\               MERIVUS 定制功能和界面，主要自研入口
├─ agent\                本机 Python AI Agent 源码、测试和打包产物
├─ schemas\              QGC 与 Agent 之间的 JSON 数据契约
├─ configs\              可提交的示例配置和安全策略模板
├─ resources\            图标、字体、声音、校准和内置资源
├─ libs\                 MAVLink、SDL、OpenSSL 等第三方依赖源码/库
├─ tools\dev\            Windows 构建、Agent 打包和局部验证脚本
├─ deploy\windows\       Windows 安装器脚本、图标和驱动 MSI
├─ build\                当前本机的构建目录，不提交 Git
│  └─ ...\staging\       当前 Release 程序集结目录
├─ docs\                 MERIVUS 架构、交接、构建和发布文档
├─ test\                 QGC 测试数据
├─ translations\         多语言翻译源文件
├─ android\ / ios\       移动端资源和平台配置
├─ CMakeLists.txt         CMake 构建入口
├─ qgroundcontrol.pro     当前 Qt 5/qmake 主构建入口
├─ qgroundcontrol.qrc     主资源清单
├─ QGCPostLinkCommon.pri  链接后部署 Qt/GStreamer 等运行依赖
└─ AGENTS.md              本仓库的 Codex/Agent 协作规则
```

### 3.3 `src`：QGroundControl 主体源码

绝对路径：

```text
E:\MERIVUS\GroundStation\src
```

这里保存地面站的通用核心能力。重要子目录如下：

| 绝对路径 | 功能 |
| --- | --- |
| `E:\MERIVUS\GroundStation\src\comm` | 串口、UDP/TCP 等通信链路管理 |
| `E:\MERIVUS\GroundStation\src\Vehicle` | 飞机对象、遥测状态、命令和参数交互核心 |
| `E:\MERIVUS\GroundStation\src\FirmwarePlugin` | 不同飞控固件的行为适配 |
| `E:\MERIVUS\GroundStation\src\AutoPilotPlugins` | 飞控设置页面和自动驾驶仪适配 |
| `E:\MERIVUS\GroundStation\src\VehicleSetup` | 固件、传感器、遥控、电源等车辆设置流程 |
| `E:\MERIVUS\GroundStation\src\MissionManager` | 航点、任务上传下载和任务协议 |
| `E:\MERIVUS\GroundStation\src\PlanView` | 任务规划界面和交互 |
| `E:\MERIVUS\GroundStation\src\FlightDisplay` | 飞行界面、仪表和运行时显示 |
| `E:\MERIVUS\GroundStation\src\FlightMap` | 地图显示和图层 |
| `E:\MERIVUS\GroundStation\src\GPS` | GPS/RTK 接收与状态管理 |
| `E:\MERIVUS\GroundStation\src\VideoManager` | 视频源和视频状态管理 |
| `E:\MERIVUS\GroundStation\src\VideoReceiver` | GStreamer 视频接收实现 |
| `E:\MERIVUS\GroundStation\src\AnalyzeView` | 日志下载和分析视图 |
| `E:\MERIVUS\GroundStation\src\FactSystem` | 参数、遥测字段和元数据表达体系 |
| `E:\MERIVUS\GroundStation\src\QmlControls` | 通用 QML 控件 |
| `E:\MERIVUS\GroundStation\src\Settings` | 应用配置项和持久化设置 |
| `E:\MERIVUS\GroundStation\src\Joystick` | 手柄/摇杆输入 |
| `E:\MERIVUS\GroundStation\src\Audio` | 告警和语音提示 |
| `E:\MERIVUS\GroundStation\src\main.cc` | Windows 地面站程序启动入口之一 |
| `E:\MERIVUS\GroundStation\src\QGCApplication.cc` | 应用初始化和全局生命周期 |

修改 `src` 相当于改动 QGroundControl 通用底座。MERIVUS 自研功能优先放入 `custom`，这样更容易追踪产品差异并继续吸收上游更新。

### 3.4 `custom`：MERIVUS 自研功能的主要入口

绝对路径：

```text
E:\MERIVUS\GroundStation\custom
```

结构：

```text
custom
├─ src
│  ├─ Ai                 QGC 侧 AI 客户端、Schema、Policy、审计
│  ├─ Swarm              多机编队控制器
│  ├─ Diagnostics        MERIVUS 链路诊断
│  ├─ FirmwarePlugin     MERIVUS 固件适配
│  └─ AutoPilotPlugin    MERIVUS 自动驾驶仪设置适配
├─ res
│  ├─ Merivus            指挥中心、飞行地图、工具栏、AI 面板 QML
│  ├─ Images             产品图标、编队和状态图形
│  └─ Custom             定制相机和控件资源
├─ tests                 自研 C++ 逻辑测试
├─ deploy                定制部署资源
├─ android               Android 定制资源
├─ custom.pri            将自研 C++/配置加入 qmake 构建
└─ custom.qrc 等         将 QML、图片等资源编入应用
```

关键源码：

| 绝对路径 | 功能 |
| --- | --- |
| `E:\MERIVUS\GroundStation\custom\src\Swarm\SwarmController.*` | 多机编队和成员控制核心 |
| `E:\MERIVUS\GroundStation\custom\src\Ai\AiAgentClient.*` | QGC 与本机 Agent 通信 |
| `E:\MERIVUS\GroundStation\custom\src\Ai\AiServiceSupervisor.*` | Agent 进程启动、健康和生命周期管理 |
| `E:\MERIVUS\GroundStation\custom\src\Ai\AiSchemaValidator.*` | 校验 Agent 返回的结构化数据 |
| `E:\MERIVUS\GroundStation\custom\src\Ai\AiCommandPolicy.*` | 本地安全策略和允许范围 |
| `E:\MERIVUS\GroundStation\custom\src\Ai\AiAuditEvent.*` | AI 相关审计事件表达 |
| `E:\MERIVUS\GroundStation\custom\res\Merivus\CommandCenterOverlay.qml` | 指挥中心主要覆盖层 |
| `E:\MERIVUS\GroundStation\custom\res\Merivus\FlyViewMap.qml` | MERIVUS 飞行地图界面 |
| `E:\MERIVUS\GroundStation\custom\res\Merivus\MerivusAIAssistantPanel.qml` | AI 助手面板 |
| `E:\MERIVUS\GroundStation\custom\res\Merivus\TelemetryValuesBar.qml` | 遥测数值条 |

当前安全边界：Agent/LLM 只能产生文本和 `ActionProposal`；现有交接证据表明 AI 飞行动作执行器尚未接通，不能把“界面显示建议”描述为“AI 已控制飞机”。

### 3.5 `agent`：随地面站运行的本机 Python 服务

绝对路径：

```text
E:\MERIVUS\GroundStation\agent
```

结构与含义：

| 路径 | 功能 | 资产类型 |
| --- | --- | --- |
| `agent\app` | Python 服务核心源码 | 源码 |
| `agent\app\providers` | Mock、Ollama 等模型 Provider 适配 | 源码 |
| `agent\app\services` | Agent 业务服务 | 源码 |
| `agent\tests` | Python 单元测试和 fixtures | 测试资产 |
| `agent\tools` | Agent 辅助脚本 | 工具源码 |
| `agent\requirements.txt` | 运行依赖清单 | 可复现配置 |
| `agent\requirements-dev.txt` | 测试/开发依赖 | 可复现配置 |
| `agent\merivus-agent.spec` | PyInstaller 打包定义 | 构建配置 |
| `agent\.env.example` | 无真实密钥的环境变量示例 | 配置模板 |
| `agent\.venv` | 本机 Python 虚拟环境 | 本地依赖，不提交 |
| `agent\build` | PyInstaller 中间文件 | 构建产物，不提交 |
| `agent\dist` | PyInstaller 可分发目录 | 构建产物，不提交 |

当前 Agent 独立可执行文件：

```text
E:\MERIVUS\GroundStation\agent\dist\merivus-agent\merivus-agent.exe
```

它还会被复制到 staging：

```text
E:\MERIVUS\GroundStation\build\Desktop_Qt_5_15_2_MSVC2019_64bit_Release\staging\agent\merivus-agent.exe
```

因此最终地面站不是只有一个进程：`MERIVUS.exe` 是 Qt 地面站主程序，`merivus-agent.exe` 是配套的本机服务程序。

### 3.6 `schemas` 与 `configs`：跨模块契约和模板

绝对路径：

```text
E:\MERIVUS\GroundStation\schemas
E:\MERIVUS\GroundStation\configs
```

`schemas` 当前包含：

- `action_proposal.schema.json`：AI 动作建议的数据结构；
- `agent_request.schema.json`：QGC 发给 Agent 的请求结构；
- `agent_response.schema.json`：Agent 返回结构。

`configs` 当前包含：

- `agent.default.example.json`：Agent 示例配置；
- `ai_command_policy.example.json`：AI 命令策略示例；
- `whitelist_commands.example.yaml`：允许命令示例。

它们是可提交模板，不应包含真实 Token、生产地址、设备凭据或敏感坐标。模板不替代运行时代码校验；实际请求仍由 Python Schema 和 QGC C++ Schema/Policy 双重约束。

### 3.7 `resources`、`libs` 与平台目录

| 路径 | 功能 |
| --- | --- |
| `E:\MERIVUS\GroundStation\resources` | 应用图标、字体、音频、校准资源、内置固件和 SDL 数据 |
| `E:\MERIVUS\GroundStation\libs` | MAVLink、SDL2、OpenSSL、Eigen、zlib 等第三方依赖 |
| `E:\MERIVUS\GroundStation\translations` | 多语言 `.ts` 翻译源文件 |
| `E:\MERIVUS\GroundStation\android` | Android Manifest、图标和平台代码 |
| `E:\MERIVUS\GroundStation\ios` | iOS 图标、Info.plist 和平台配置 |
| `E:\MERIVUS\GroundStation\debian` | Debian/Linux 打包元数据 |
| `E:\MERIVUS\GroundStation\VideoReceiverApp` | 独立视频接收示例/工具应用 |
| `E:\MERIVUS\GroundStation\custom-example` | QGC Custom Build 示例，不是 MERIVUS 当前自研主入口 |

`libs` 中的第三方组件不是 MERIVUS 自研代码。升级时必须检查对应许可证、版本兼容和上游改动。

### 3.7.1 GroundStation 其他顶层文件夹字典

| 绝对路径 | 含义和维护建议 |
| --- | --- |
| `E:\MERIVUS\GroundStation\.github` | GitHub Issue 模板和 Actions 工作流；属于仓库协作/CI 配置 |
| `E:\MERIVUS\GroundStation\.qtcreator` | 本机 Qt Creator 工程状态；具有机器相关性，不作为核心源码 |
| `E:\MERIVUS\GroundStation\cmake` | CMake 辅助模块和平台部署规则；当前 Windows Qt 5 主路径仍以 qmake 工程为重点 |
| `E:\MERIVUS\GroundStation\design-system` | MERIVUS 设计系统规范入口 |
| `E:\MERIVUS\GroundStation\doc` | QGroundControl 上游 Doxygen/架构图等传统文档资源 |
| `E:\MERIVUS\GroundStation\docs` | MERIVUS 当前架构、开发、交接、硬件、发布和模板文档，项目说明优先入口 |
| `E:\MERIVUS\GroundStation\test` | 航点任务、日志配置等测试数据，不等同于完整自动化测试套件 |
| `E:\MERIVUS\GroundStation\tools` | 上游通用工具及 `tools\dev` 自研工程脚本 |

根目录还包含 `qgroundcontrol.pro`、`CMakeLists.txt`、`.pri` 和 `.qrc` 等构建/资源入口。它们负责把 `src`、`custom`、`libs` 和资源组合成应用；修改目录而未同步这些清单，可能导致代码存在但没有进入构建。

### 3.8 `tools\dev`：本项目推荐的工程命令入口

绝对路径：

```text
E:\MERIVUS\GroundStation\tools\dev
```

| 脚本 | 用途 |
| --- | --- |
| `build-merivus.ps1` | 使用 Qt 5.15.2、MSVC 工具链构建 MERIVUS |
| `build-agent.ps1` | 用 PyInstaller 打包 Agent 并复制到 staging |
| `build-windows.ps1` | Windows 构建辅助流程 |
| `check-windows-environment.ps1` | 检查 Windows 构建环境 |
| `check-linux-sitl-environment.sh` | 检查 Linux/SITL 环境 |
| `test-ai-intent-policy.ps1` | 验证 AI 意图策略 |
| `test-sitl-swarm-task-isolation.ps1` | 验证 SITL 编队任务隔离 |

推荐 Release 构建命令：

```powershell
Set-Location 'E:\MERIVUS\GroundStation'
powershell -ExecutionPolicy Bypass -File tools\dev\build-merivus.ps1 -Configuration Release
powershell -ExecutionPolicy Bypass -File tools\dev\build-agent.ps1 -Configuration Release
```

脚本当前默认依赖本机 Qt、Visual Studio 和 GStreamer 安装路径。换电脑后不能假定路径相同，应先运行环境检查并按机器实际情况传参。

### 3.9 `build`：构建产物，不是源码

当前构建目录：

```text
E:\MERIVUS\GroundStation\build\Desktop_Qt_5_15_2_MSVC2019_64bit_Release
```

其中大量内容是编译器和 Qt 工具生成的：

- `.obj`：C/C++ 目标文件；
- `moc_*.cpp/.obj`：Qt Meta-Object Compiler 自动生成文件；
- `*_qml.cpp/.obj`：QML 编译缓存/自动生成代码；
- `.pch`：预编译头；
- `.pdb`：Windows 调试符号；
- `Makefile`：qmake 生成的构建规则；
- `.qm`：翻译编译产物；
- `staging`：链接后整理好的可运行目录。

这些文件体积大、与工具链和绝对路径相关，可以重新生成，不应当作源码编辑，也不应提交 Git。

### 3.10 `staging`：可运行程序集结区

绝对路径：

```text
E:\MERIVUS\GroundStation\build\Desktop_Qt_5_15_2_MSVC2019_64bit_Release\staging
```

当前 staging 包含：

```text
staging
├─ MERIVUS.exe                 地面站主程序
├─ agent\merivus-agent.exe     本机 Agent
├─ Qt5*.dll                    Qt 运行库
├─ platforms\qwindows.dll     Windows Qt 平台插件
├─ QtQuick* / QtQml*           QML 模块
├─ geoservices\               地图服务插件
├─ gstreamer-plugins\         GStreamer 视频插件
├─ libexec\gstreamer-1.0\     GStreamer 插件扫描程序
├─ audio / mediaservice       音视频插件
├─ imageformats / sqldrivers  图片和数据库插件
├─ translations\             Qt 翻译文件
├─ vc_redist.x64.exe           MSVC 运行库安装程序
└─ 其他 DLL、配置和资源
```

最重要的理解：

```text
MERIVUS.exe ≠ 完整发布包

MERIVUS.exe
  + Qt DLL/QML
  + Windows 平台插件
  + GStreamer DLL/插件
  + SDL/OpenSSL 等依赖
  + merivus-agent.exe 及内部文件
  = 当前可运行 staging 程序集
```

不能只把 `MERIVUS.exe` 单独复制到另一台电脑并期待完整功能。应复制完整 staging、使用正式安装包，或按发布流程部署所有依赖。

### 3.11 当前地面站产物状态

截至本文核对时：

| 项目 | 当前路径/状态 |
| --- | --- |
| 地面站 EXE | `E:\MERIVUS\GroundStation\build\Desktop_Qt_5_15_2_MSVC2019_64bit_Release\staging\MERIVUS.exe` |
| EXE 大小 | `33,910,272` 字节 |
| EXE SHA-256 | `5D4A647018A10C0EE0CBE36F82DE398F8A17CD7692896BA41D87549B9E4F6A44` |
| staging Agent | `E:\MERIVUS\GroundStation\build\Desktop_Qt_5_15_2_MSVC2019_64bit_Release\staging\agent\merivus-agent.exe` |
| Agent SHA-256 | `89F944AF9CD3DDF20DE1B031B181B453E9E28AC0EBE7E5DDCAE635C4A72E8C57` |
| 正式安装器 | 当前未发现 `MERIVUS-installer.exe` |
| Windows 驱动包素材 | `E:\MERIVUS\GroundStation\deploy\windows\driver.msi` |

`deploy\windows\nullsoft_installer.nsi` 和 `QGCPostLinkInstaller.pri` 说明项目具备 NSIS 安装器构建入口，但“脚本存在”不等于“当前安装包已经生成”。当前 staging 更准确的称呼是“本地 Release 可运行程序集”，还不是经签名和发布验证的正式安装产品。

### 3.12 GroundStation 从源码到用户程序

```text
E:\MERIVUS\GroundStation\src + custom + agent
                    │
                    │ Qt 5.15.2 / MSVC / qmake / PyInstaller
                    ▼
E:\MERIVUS\GroundStation\build\...
                    │
                    ├─ C++/QML 中间文件
                    ├─ MERIVUS.exe
                    └─ staging 完整运行依赖
                              │
                              │ 测试、签名、NSIS 打包
                              ▼
                         正式安装包
```

## 4. FirmwarePX4：无人机的“数字大脑”

### 4.1 目录层级和真实仓库根

目录关系：

```text
E:\MERIVUS\FirmwarePX4
└─ PX4
   └─ PX4-Autopilot       真正的 Git 仓库和 make 执行目录
```

真正路径：

```text
E:\MERIVUS\FirmwarePX4
```

当前 Git 身份：

```text
分支：main
提交：47a595fec7
远端：origin/main
上游基础：PX4 v1.14 系列的 MERIVUS 定制仓库
目标飞控：Pixhawk 6C Mini / FMUv6C
主要固件目标：px4_fmu-v6c_default
仿真目标：px4_sitl_default
```

### 4.2 FirmwarePX4 顶层结构

```text
E:\MERIVUS\FirmwarePX4
├─ boards\               每种飞控板的硬件配置、启动和驱动选择
├─ src\
│  ├─ drivers\           传感器、GPS、总线和硬件驱动
│  ├─ modules\           commander、EKF2、MAVLink、控制器等模块
│  ├─ lib\               公共算法与基础库
│  ├─ systemcmds\        NSH/PX4 命令行命令
│  ├─ examples\          示例模块
│  └─ include\           公共头文件
├─ msg\                  uORB 消息定义
├─ ROMFS\                固件内置启动脚本、机架和默认资源
├─ platforms\            NuttX、POSIX、ROS 2 等平台层
├─ Tools\                工具链安装、仿真、上传和打包脚本
├─ cmake\                CMake 构建规则
├─ Documentation\merivus MERIVUS 固件专项文档
├─ integrationtests\     集成测试
├─ test\ / test_data\   测试用例和固定输入数据
├─ launch\               SITL/MAVROS 启动配置
├─ validation\           模块配置等 Schema
├─ .github\workflows\    CI 构建与检查
├─ .gitmodules            外部依赖子模块清单
├─ build\                本地或交付固件输出，不作为源码
├─ Makefile               PX4 make 入口
└─ CMakeLists.txt         顶层 CMake 配置
```

### 4.3 `boards`：硬件与固件之间的契约

FMUv6C 关键目录：

```text
E:\MERIVUS\FirmwarePX4\boards\px4\fmu-v6c
```

结构与作用：

| 路径 | 作用 |
| --- | --- |
| `default.px4board` | 决定主固件编入哪些驱动、模块和系统命令 |
| `bootloader.px4board` | bootloader 构建配置，不是日常主固件 |
| `firmware.prototype` | 固件打包元数据模板 |
| `init\rc.board_defaults` | 板级/产品默认参数 |
| `init\rc.board_sensors` | 上电时启动哪些板载传感器及总线参数 |
| `src\board_config.h` | 硬件版本、引脚和板级宏 |
| `src\spi.cpp` | SPI 总线、片选、DRDY 和不同硬件版本设备表 |
| `src\i2c.cpp` | I2C 设备识别和总线配置 |
| `src\timer_config.cpp` | PWM/定时器资源映射 |
| `src\manifest.c` | 不同硬件版本对应的设备清单 |
| `nuttx-config` | NuttX 内核、链接脚本、DMA 和底层板级配置 |
| `extras` | 随主固件打包的 bootloader/IO 固件依赖 |

当前 `default.px4board` 已启用的关键能力包括：

- BMI088、ICM-42688-P IMU；
- MS5611 气压计；
- GPS、磁力计、光流、距离传感器；
- EKF2、commander、navigator；
- 多旋翼、固定翼和 VTOL 控制模块；
- MAVLink、logger、uORB、listener、top 等调试能力；
- `swarm_node` MERIVUS 编队模块。

当前 V6C22（Pixhawk 6C Mini Rev 2）在 `spi.cpp` 和 `manifest.c` 中具有明确硬件版本分支，组合为 BMI088 + ICM-42688-P。`rc.board_sensors` 负责按硬件版本启动 BMI088/BMI055、ICM-42688-P、MS5611 和 IST8310。

`rc.board_defaults` 还定义了 MERIVUS 产品默认通信契约，例如：

- GPS1 接 Hyper982；
- TELEM1 接 HyperLte 地面链路；
- GPS 协议、波特率和双天线偏角；
- EKF2 GPS/高度参考；
- MAVLink 实例、模式、带宽和流控。

这些配置会影响真实硬件通信，不应由 AI、自动脚本或未确认流程直接写入飞控。

### 4.4 `src\drivers`：把芯片原始数据带入 PX4

绝对路径：

```text
E:\MERIVUS\FirmwarePX4\src\drivers
```

主要职责：

- 访问 SPI、I2C、UART、CAN 等总线；
- 识别 ICM-42688-P、BMI088、MS5611、IST8310、GPS 等设备；
- 读取寄存器和原始测量；
- 转换单位、检查错误、打时间戳；
- 向 uORB 发布传感器消息。

数据链可以简化为：

```text
传感器芯片
   ↓ SPI / I2C / UART
src\drivers
   ↓ uORB 原始/校准测量
EKF2 与控制模块
```

### 4.5 `src\modules`：飞控运行时模块

绝对路径：

```text
E:\MERIVUS\FirmwarePX4\src\modules
```

这里包含 PX4 的主要运行逻辑，例如：

| 模块类型 | 作用 |
| --- | --- |
| `commander` | 解锁、飞行模式、状态机和失效保护 |
| `ekf2` | IMU、GPS/RTK、气压、磁力计等状态估计 |
| `mavlink` | 与 GroundStation/伴随计算机通信 |
| `navigator` | 航线、返航和任务导航 |
| `mc_*_control` | 多旋翼位置、姿态和角速度控制 |
| `fw_*_control` | 固定翼控制 |
| `vtol_att_control` | VTOL 模式和转换控制 |
| `logger` | 生成 ULog 飞行日志 |
| `sensors` | 传感器汇总、校准和选择 |
| `swarm_node` | MERIVUS 分阶段编队协议 |

MERIVUS 编队模块路径：

```text
E:\MERIVUS\FirmwarePX4\src\modules\swarm_node
```

当前编队协议包含 `PREPARE`、`COMMIT`、`RELEASE` 和 `ABORT` 阶段。它是固件内的自研差异之一，但源码存在不代表当前版本已经完成真实六机飞行验证。

### 4.6 `msg`：PX4 内部 uORB 数据契约

绝对路径：

```text
E:\MERIVUS\FirmwarePX4\msg
```

`.msg` 文件定义 PX4 模块间消息的数据字段。构建时会据此生成 C/C++ 消息类型。

MERIVUS 自定义消息：

```text
E:\MERIVUS\FirmwarePX4\msg\SwarmCommand.msg
```

其他常见消息包括 `SensorGps.msg`、`VehicleAttitude.msg`、`VehicleLocalPosition.msg` 和 `VehicleStatus.msg`。修改消息定义会影响发布者、订阅者、日志和相关通信适配，必须同步验证。

### 4.7 `ROMFS`：烧进固件的运行资源

绝对路径：

```text
E:\MERIVUS\FirmwarePX4\ROMFS
```

其中 `px4fmu_common` 保存 PX4 FMU 通用启动脚本、机架配置和运行时资源。这些文件在构建时进入固件包，飞控上电后由启动流程使用。

要区分：

- `boards\...\init`：特定硬件板的启动与默认配置；
- `ROMFS\px4fmu_common`：多个板型共享的固件内运行资源。

### 4.8 `platforms`：NuttX 与其他运行平台

绝对路径：

```text
E:\MERIVUS\FirmwarePX4\platforms
```

| 子目录 | 用途 |
| --- | --- |
| `platforms\nuttx` | Pixhawk 等实时飞控使用的 NuttX 平台层 |
| `platforms\posix` | Linux/SITL 等 POSIX 平台 |
| `platforms\common` | 跨平台公共实现 |
| `platforms\ros2` | ROS 2 相关平台适配 |
| `platforms\qurt` | QuRT 平台支持 |

`platforms\nuttx\NuttX` 下含 Git 子模块。缺少或版本不一致时，即使 PX4 主仓源码完整也可能无法编译。

### 4.8.1 FirmwarePX4 其他顶层文件夹字典

| 绝对路径 | 含义和维护建议 |
| --- | --- |
| `E:\MERIVUS\FirmwarePX4\.ci` | PX4 Jenkins 编译和硬件 CI 定义 |
| `E:\MERIVUS\FirmwarePX4\.github` | GitHub Issue/PR 模板和 Actions 工作流 |
| `E:\MERIVUS\FirmwarePX4\.devcontainer` | 可复现开发容器配置 |
| `E:\MERIVUS\FirmwarePX4\.vscode` | VS Code 构建、调试和插件建议配置 |
| `E:\MERIVUS\FirmwarePX4\integrationtests` | Python 系统级集成测试 |
| `E:\MERIVUS\FirmwarePX4\launch` | PX4、MAVROS 和多机 SITL 启动文件 |
| `E:\MERIVUS\FirmwarePX4\posix-configs` | SITL、Raspberry Pi、BeagleBone 等 POSIX 目标配置 |
| `E:\MERIVUS\FirmwarePX4\test` | MAVSDK/ROS 等测试入口 |
| `E:\MERIVUS\FirmwarePX4\test_data` | 遥控协议等固定测试输入，保证重复测试从相同数据开始 |
| `E:\MERIVUS\FirmwarePX4\validation` | 模块配置 Schema 等静态验证资源 |

`boards` 下还存在 PX4、Holybro、CUAV、CubePilot、HKUST、MicoAir 等多个厂商/板型目录。本文只把 `boards\px4\fmu-v6c` 视为 MERIVUS 当前目标板入口；其他板型不能因为同在源码树中就被视为已经支持或验证。

### 4.9 `Tools`、`cmake` 与 `Makefile`：固件加工流水线

| 路径 | 作用 |
| --- | --- |
| `Tools\setup\ubuntu.sh` | 安装 Ubuntu PX4/NuttX 构建依赖 |
| `Tools\simulation` | jMAVSim、Gazebo Classic 等仿真支持 |
| `Tools\px_uploader.py` | 固件上传工具 |
| `Tools\package_firmware.py` / `px_mkfw.py` | 固件打包工具 |
| `cmake` | 板级、模块和平台 CMake 规则 |
| `Makefile` | 用户执行的统一构建入口，内部调用 CMake/Ninja |

`make px4_fmu-v6c_default` 不是“一个 C 编译命令”，而是驱动整套配置、代码生成、编译、链接和固件打包流程。

### 4.10 `.gitmodules`：必须锁定的外部依赖

PX4 仓库使用多个 Git 子模块，例如：

- MAVLink 定义；
- NuttX 内核和 Apps；
- PX4 GPS Drivers；
- DroneCAN/libuavcan；
- Gazebo/jMAVSim；
- 加密库和事件库。

当前本机有部分可选子模块未初始化，`git submodule status --recursive` 中以 `-` 开头。目标硬件构建是否需要它们取决于配置，但正式构建前应执行：

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

并记录子模块提交。只记录主仓 commit 不足以完整复现固件。

## 5. PX4 正确构建流程

### 5.1 原命令纠正

原文中的命令：

```text
make fmu_v6c_defeat
```

不是当前仓库有效目标。正确命令是：

```bash
make px4_fmu-v6c_default
```

错误点：

- 缺少 `px4_` 前缀；
- `fmu-v6c` 中间应使用连字符；
- `default` 被误写成 `defeat`。

### 5.2 Windows 和 Ubuntu 的职责

| 环境 | 推荐职责 |
| --- | --- |
| Windows | 编辑源码、Git 管理、保存发布产物、运行 GroundStation/QGroundControl 刷写 |
| Ubuntu 22.04 虚拟机 | 初始化子模块、安装 ARM 工具链、编译 PX4、计算 Linux 侧哈希 |
| GitHub | 在两台环境之间同步可追溯源码提交 |

PX4 并非理论上只能在 Linux 编译，但本项目已将 Ubuntu 22.04/PX4 v1.14 工具链定义为推荐、受支持的固件构建环境。普通 Windows CMD/Git Bash 并不具备完整 NuttX/ARM 工具链，不应作为项目发布构建入口。

### 5.3 推荐工作流

```text
Windows 修改 FirmwarePX4 源码
        ↓
Git commit / push（形成不可变源码身份）
        ↓
Ubuntu 虚拟机 git pull --ff-only
        ↓
初始化并核对子模块
        ↓
在 Ubuntu 本地文件系统编译
        ↓
生成 .px4 / .elf 和其他中间产物
        ↓
记录 commit、子模块、工具链、SHA-256
        ↓
复制 .px4 回 Windows
        ↓
Windows 再算 SHA-256 并与 Linux 对比
        ↓
人工审核后通过 GroundStation 刷写
```

优先使用 Git 同步源码，不建议反复覆盖整个源代码文件夹。VMware 共享目录适合传输产物，不适合直接作为长期 Linux 编译目录。

### 5.4 Ubuntu 中的命令

假设虚拟机中仓库路径为 `~/src/FirmwarePX4`：

```bash
cd ~/src/FirmwarePX4
git switch main
git pull --ff-only
git submodule sync --recursive
git submodule update --init --recursive
git status --short

git rev-parse HEAD
git submodule status --recursive
arm-none-eabi-gcc --version | head -n 1

make px4_fmu-v6c_default
```

预期主产物：

```text
build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4
build/px4_fmu-v6c_default/px4_fmu-v6c_default.elf
```

`.bin` 是否在具体构建目录中生成，取决于板型和打包目标；日常 GroundStation/QGroundControl 自定义固件刷写以 `.px4` 为准，不应把“必须同时拿到 `.bin`”写成强制条件。

验证和记录：

```bash
ls -lh build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4
sha256sum build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4
```

### 5.5 `.px4`、`.elf`、`.bin` 的区别

| 文件 | 用途 | 是否用于日常地面站自定义刷写 |
| --- | --- | --- |
| `.px4` | PX4 固件包，包含映像与板型/版本等元数据 | 是，首选 |
| `.elf` | 带符号的链接产物，用于调试、反汇编和分析 | 否 |
| 主固件 `.bin` | 裸二进制映像，适用于特定底层烧录流程 | 通常否 |
| bootloader `.bin` | 引导程序，只用于 bootloader 生产/恢复流程 | 绝对不能当普通固件随意刷写 |
| IO 固件 `.bin` | PX4IO 协处理器固件，可被主固件打包 | 不是 FMU 主固件 |

源码目录中的以下文件属于板级依赖，不是本次 Linux 主固件构建输出：

```text
boards\px4\fmu-v6c\extras\px4_fmu-v6c_bootloader.bin
boards\px4\fmu-v6c\extras\px4_io-v2_default.bin
```

### 5.6 当前本机 FirmwarePX4 产物状态

当前存在一个版本化 `.px4`：

```text
E:\MERIVUS\FirmwarePX4\build\px4_fmu-v6c_default-v1.14.0-1.0.0-9-g47a595f.px4
```

核对信息：

```text
大小：1,818,089 字节
修改时间：2026-08-06 17:53:12
SHA-256：38250FAB693C4671B86AD6B69AD6362B7521680306E497C38951CDB912DF08A1
```

但当前 Windows 目录中不存在完整的：

```text
build\px4_fmu-v6c_default\
```

也未发现对应主固件 `.elf` 或主固件 `.bin`。因此严谨表述是：

- 当前本机保存有一个 `.px4` 固件包；
- 文件名中的 `g47a595f` 与当前提交前缀一致；
- 仅凭文件名和存在状态，不能证明它是在当前机器、当前工具链中刚刚构建；
- 用于部署前仍应核对发布记录、来源、工具链、测试证据和 SHA-256。

## 6. 从 Ubuntu 把固件带回 Windows

### 6.1 VMware 共享文件夹

例如将 Windows 输出目录设置为：

```text
E:\MERIVUS\FirmwareOutput
```

虚拟机内可能挂载为：

```text
/mnt/hgfs/MERIVUS/FirmwareOutput
```

复制：

```bash
cp build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4 \
  /mnt/hgfs/MERIVUS/FirmwareOutput/
```

随后在 Ubuntu 比较源文件和共享目录文件：

```bash
sha256sum \
  build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4 \
  /mnt/hgfs/MERIVUS/FirmwareOutput/px4_fmu-v6c_default.px4
```

Windows 再复核：

```powershell
Get-FileHash -Algorithm SHA256 `
  'E:\MERIVUS\FirmwareOutput\px4_fmu-v6c_default.px4'
```

两边 SHA-256 不一致时禁止刷写。

### 6.2 为什么不直接在共享文件夹编译

PX4/NuttX 依赖 Unix 文件权限、符号链接、大小写和大量小文件操作。VMware 共享目录可能改变元数据、权限或 I/O 行为，容易造成假 Git 改动和构建异常。推荐：

```text
Ubuntu 本地 ext4 目录：保存源码并编译
VMware 共享目录：只传输最终固件和发布记录
```

## 7. 使用 GroundStation 刷写 Pixhawk 6C Mini

### 7.1 烧录前准备

- 确认飞控确实为 Pixhawk 6C Mini / FMUv6C；
- 确认选择 `px4_fmu-v6c_default.px4`，不是其他板型；
- 保存现有参数和必要校准记录；
- 断开电池和不必要的外设供电；
- 拆除螺旋桨或采取等效机械防护；
- 关闭其他占用飞控 USB/串口的软件；
- 核对固件来源、Git 提交和 SHA-256；
- 不把 bootloader `.bin`、IO `.bin` 或 `.elf` 当作自定义主固件。

### 7.2 标准流程

1. 打开完整 staging 中的 `MERIVUS.exe`，或使用经过批准的正式安装版 GroundStation。
2. 进入“车辆设置 → 固件”。
3. 按界面提示连接 Pixhawk 6C Mini USB。
4. 打开高级设置，选择“Custom firmware file / 本地自定义固件”。
5. 选择经过校验的 `px4_fmu-v6c_default.px4`。
6. 等待擦除、写入、校验和重启全部完成。
7. 不在中途拔线、断电或关闭软件。
8. 重连后核对固件版本和目标板型。
9. 重新检查参数、传感器、GPS/RTK、遥控、输出映射和失效保护。
10. 在拆桨条件下完成基础功能检查，再按单机、双机、六机逐级验证。

### 7.3 关于“重新拔插一次 USB”

原说明把“连接后必须再拔插一次 USB”写成固定步骤并不严谨。标准做法是进入固件页面后按界面提示连接设备，让地面站检测 bootloader；只有界面提示、设备未被识别或需要重新进入 bootloader 时才重新插拔。

频繁、无提示地拔插可能造成：

- 设备枚举混乱；
- 选错串口或目标设备；
- 在擦写过程中断电；
- USB 接口机械损伤。

因此应以当前 GroundStation 固件页面提示和现场刷写 SOP 为准，而不是把二次拔插当作所有情况下都必须执行的动作。

### 7.4 刷写完成不等于可以起飞

固件写入成功只证明数据已经写入并通过刷写阶段校验，不证明：

- 参数与新固件完全兼容；
- 传感器方向和校准正确；
- 电机顺序、旋向和输出协议正确；
- GPS/RTK、4G、遥控和 failsafe 正常；
- 编队协议和真实六机链路已验证；
- 飞机在当前载荷和环境中安全。

刷写后仍需执行参数差异审查、传感器检查、拆桨输出检查、SITL/Mock 证据复核和现场安全流程。

## 8. 两项软件资产如何配套发布

推荐为一次可追溯版本建立发布记录：

| 记录项 | GroundStation | FirmwarePX4 |
| --- | --- | --- |
| 源码仓库 | `E:\MERIVUS\GroundStation` | `E:\MERIVUS\FirmwarePX4` |
| Git commit | 地面站提交 | 固件提交 |
| 子模块 | GroundStation 依赖版本 | PX4 全部子模块提交 |
| 工具链 | Qt、MSVC、GStreamer、Python/PyInstaller | Ubuntu、ARM GCC、CMake、Ninja、Python |
| 构建配置 | Release、定制选项 | `px4_fmu-v6c_default` |
| 主产物 | 安装器或完整 staging | `.px4` |
| 完整性 | SHA-256 | SHA-256 |
| 验证 | 启动、UI、链路、Agent、打包验证 | 静态检查、SITL、板级地面测试、分阶段实机 |

发布时应保证：

```text
GroundStation 版本
    ↕ 协议和功能矩阵匹配
FirmwarePX4 版本
```

不能只写“最新版”。应记录明确的 commit、版本号和哈希，避免地面站和飞控协议不匹配。

## 9. 日常操作速查

### 9.1 查 GroundStation 状态

```powershell
git -C 'E:\MERIVUS\GroundStation' status --short
git -C 'E:\MERIVUS\GroundStation' rev-parse HEAD
```

### 9.2 构建 GroundStation

```powershell
Set-Location 'E:\MERIVUS\GroundStation'
powershell -ExecutionPolicy Bypass -File tools\dev\check-windows-environment.ps1
powershell -ExecutionPolicy Bypass -File tools\dev\build-merivus.ps1 -Configuration Release
powershell -ExecutionPolicy Bypass -File tools\dev\build-agent.ps1 -Configuration Release
```

### 9.3 查 FirmwarePX4 状态

```powershell
git -C 'E:\MERIVUS\FirmwarePX4' status --short
git -C 'E:\MERIVUS\FirmwarePX4' rev-parse HEAD
git -C 'E:\MERIVUS\FirmwarePX4' submodule status --recursive
```

### 9.4 在 Ubuntu 编译固件

```bash
cd ~/src/FirmwarePX4
git status --short
git submodule update --init --recursive
make px4_fmu-v6c_default
sha256sum build/px4_fmu-v6c_default/px4_fmu-v6c_default.px4
```

### 9.5 Windows 检查固件哈希

```powershell
Get-FileHash -Algorithm SHA256 'E:\MERIVUS\FirmwareOutput\px4_fmu-v6c_default.px4'
```

## 10. 最终结论

### GroundStation

`E:\MERIVUS\GroundStation` 不只是一个 EXE 文件，而是“源码 + MERIVUS 定制层 + 本机 Agent + 构建脚本 + Windows staging + 安装器素材”的完整地面站工程。

当前可以确认存在完整 staging 和 `MERIVUS.exe`，但未发现正式 `MERIVUS-installer.exe`。直接交付时应使用完整程序集或重新生成并验证安装包，不能只复制单个 EXE。

### FirmwarePX4

`E:\MERIVUS\FirmwarePX4` 是无人机飞控固件源码仓库，包含板级配置、传感器驱动、EKF/控制模块、uORB 消息、启动资源、编队模块、构建工具和测试。

正确 FMUv6C 构建命令是：

```bash
make px4_fmu-v6c_default
```

日常 GroundStation 自定义固件刷写应选择 `.px4`。当前 Windows 目录保存有一个版本化 `.px4` 文件，但没有完整的同目录 Linux 中间构建树；部署前仍需按提交、子模块、工具链、测试和 SHA-256 完成来源核验。
