# 项目概述

## 定位与目标

MERIVUS 是面向多无人机协同作业的桌面地面站与调度系统，当前仓库保留 QGroundControl 上游工程结构，通过 `custom/` 承载 UI、调度和 C++ 扩展，通过 `agent/` 承载本机 AI 服务。核心目标是复用成熟的地图、Vehicle、Mission、Link、MAVLink 与 PX4/APM 支持，在其上增加 MERIVUS 多机交互、链路诊断、AI 辅助解释和可审计的安全建议。

项目任务背景将底座称为“QGroundControl 4.3 定制”。仓库中未找到可独立验证上游精确为 4.3 的版本元数据或完整上游 Git 历史，因此：

- “基于 QGroundControl 定制”可由目录结构、许可证和源码确认；
- “精确为 4.3”属于项目约定，**待通过上游来源提交、压缩包版本或负责人记录确认**；
- 当前可确认的工具链基线是 Qt 5.15.2、qmake、MSVC2019 Qt ABI 与 Visual Studio 2022 工具集。

选择 QGC Custom Build 的工程理由是复用已存在的飞控协议、任务规划、地图、参数系统、视频与跨平台 Qt/QML 基础，避免重写安全敏感的地面站底座，并把 MERIVUS 变化尽量限制在 `custom/`。证据见 [仓库结构](../architecture/REPO_STRUCTURE.md) 和 [`custom/custom.pri`](../../custom/custom.pri)。

## 当前系统边界

| 领域 | 当前职责 | 当前不承担 |
| --- | --- | --- |
| QGC/MERIVUS 软件 | UI、地图、Vehicle/Link 状态、人工操作、AI 建议展示、本地 Schema/Policy | 不在 QML 内运行 LLM，不把 Agent 输出直接变成飞行动作 |
| Local Agent | Provider 路由、问答、结构化 proposal、输出规范化 | 不访问 Vehicle/MAVLink/PX4，不声明动作成功 |
| AI 模型 | 生成文本与候选意图 | 不决定本地风险、不获得执行权限 |
| 飞控/PX4 | 真实飞行控制与失联保护 | 不由本次 AI 链路自动调用或改参 |
| 硬件/通信 | Pixhawk、4G、RTK、供电与视频的实物系统 | 当前文档不等于完成台架、链路或飞行验证 |

## 当前阶段能力

- MERIVUS QGC Custom Build UI 与多机相关界面已落库。
- AI Assistant 可使用本地规则或本机 Agent；Agent 支持 Mock 和本地 Ollama `qwen3:8b`。
- QGC C++ 层完成 Agent HTTP、生命周期、Schema、Policy 与摘要审计。
- proposal 只能显示/预览，`executable=false`；当前没有 AI Command Executor。
- Windows 构建、Policy 局部测试和 Agent onedir 打包脚本已落库；历史文档记录过成功结果，本次只复测 Agent 单元测试与环境检查。

## 长期方向（不是当前能力）

- 多无人机控制权、调度与长期 4G/RTK 链路治理。
- GIS 航点安全、地形/建筑物/禁飞区分析与未知区域策略。
- Device Gateway、Cloud API、Web Console、账号/组织/权限和审计。
- 视频与 Media Service、任务/遥测数据服务。
- 经 Schema、Policy、明确用户确认、SITL 回归和人工安全评审后的受控执行能力。
- 故障保护最终仍由 PX4/飞控负责，云端和 LLM 不持有底层控制权。

## 本阶段明确不做

- 云 Provider、MCP、生产数据库和自建完整云平台。
- `WaypointSafetyService` 生产实现或用 3D Tiles 直接做碰撞结论。
- AI 自动解锁、起飞、任务上传、参数写入或通用 MAVLink。
- 自动真实飞机测试、自动连接厂商服务器、自动修改硬件/RTK/PX4 参数。
- 把未来路线、硬件估算或历史聊天描述写成已验证能力。
