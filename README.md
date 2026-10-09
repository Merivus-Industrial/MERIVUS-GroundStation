# MERIVUS GroundStation

基于 QGroundControl 的多无人机调度地面站，由 **[Ale-xl](https://github.com/Ale-xl)** 维护。配套飞控为 [MERIVUS-FirmwarePX4](https://github.com/Ale-xl/MERIVUS-FirmwarePX4)。本仓库包含界面、人工多机调度、FTC 遥测展示，以及只提供建议的本机 AI Agent。

> 当前为开发测试版本 `0.1.0-dev.1`，不是生产发行版。已有代码、历史验证和本次验证是不同状态；FTC 主动控制与植保覆盖作业均不能因界面或方案存在而视为已经验收。

## 当前能力与边界

| 范围 | 当前内容 | 限制 |
| --- | --- | --- |
| 地面站 | QGC Custom Build、地图、飞行器选择、链路与状态展示 | 沿用 QGC/PX4 原生安全流程 |
| 人工多机调度 | 明确目标的任务交互与编队协议 | 需要 Mock/SITL 和现场分阶段验证 |
| FTC 遥测 | 四类自定义 MAVLink 消息、类型化后端、状态面板、协议和过期判断 | 只展示飞控报告，不代表主动控制已验证 |
| 本机 AI Agent | FastAPI、Mock/Ollama、C++ 客户端与服务监管器、Schema/Policy 校验 | `ActionProposal` 保持 `executable=false`，不进入飞行执行链 |
| 植保区域覆盖 | [ADR-0001 设计方案](docs/adr/0001-plant-protection-area-coverage.md) | 待实现与验证，不包含喷洒设备控制 |

AI/LLM 不发送 MAVLink、不修改 PX4 参数，也不调用 Vehicle 或 Swarm 的飞行动作入口。云设备网关、生产认证、GIS Safety Service、真实命令执行器及安装包签名尚未完成。

## 获取源码

```sh
git clone https://github.com/Ale-xl/MERIVUS-GroundStation.git GroundStation
git clone --recursive https://github.com/Ale-xl/MERIVUS-FirmwarePX4.git FirmwarePX4
```

两个仓库建议放在同一父目录，便于交叉检查 FTC 协议和复现 MAVLink 生成。GroundStation 的当前 Git 树将第三方依赖作为源码快照跟踪，没有 Git 子模块；FirmwarePX4 使用锁定提交的子模块。依赖来源和上游版权按各目录许可证保留。

## 开发与验证

Windows 产品基线为 Qt 5.15.2 / MSVC2019_64 Qt Kit、Visual Studio 2022 x64 工具链和 Python 3.11。环境与构建入口见 [Windows 构建说明](docs/development/BUILD_WINDOWS.md) 和 [构建与生成](docs/development/BUILD.md)。

```powershell
# 静态契约检查，不连接真实飞机
pwsh -File tools/dev/test-ftc-telemetry-contract.ps1
pwsh -File tools/dev/test-field-telemetry-contract.ps1
pwsh -File tools/dev/test-takeoff-health-contract.ps1
pwsh -File tools/dev/test-sitl-swarm-task-isolation.ps1
pwsh -File tools/dev/test-version-contract.ps1

# 配置好 Python 测试环境后执行 Agent 测试
cd agent
python -m pytest
```

完整构建入口：`tools/dev/build-merivus.ps1 -Configuration Release`。FTC 后端单元测试见 [test/FTC](test/FTC/README.md)。真实模型评估需显式选择 Provider，不由默认验证自动调用。

## 仓库结构

| 路径 | 职责 |
| --- | --- |
| `src/`、`libs/` | QGC 主体与第三方依赖源码 |
| `custom/` | MERIVUS 界面、C++ 扩展、人工调度与策略测试 |
| `agent/` | 本机 AI 服务、Provider、测试与打包 |
| `schemas/`、`configs/` | 协议源文件、JSON Schema 和配置模板 |
| `tools/dev/`、`test/FTC/` | 构建、协议生成、静态检查与后端测试 |
| `docs/` | 当前架构、接口、设计决策、开发和验证说明 |

## 文档与贡献

- [文档索引](docs/INDEX.md) · [工程交接入口](docs/handoff/README.md)
- [当前状态](docs/architecture/CURRENT_STATE.md) · [模块状态](docs/PROJECT_STATUS.md)
- [FTC 遥测契约](docs/architecture/FTC_TELEMETRY_CONTRACT.md) · [测试矩阵](docs/testing/TEST_MATRIX.md)
- [贡献指南](CONTRIBUTING.md) · [版本记录](docs/releases/CHANGELOG.md)

本仓库延续原 `Ale-xl/MERIVUS` 历史与 PR 记录，产品导入起点为 `b3d6659`。`.mailmap` 统一维护者旧身份的显示名称，保留原提交 SHA；具体归属边界见贡献指南。禁止提交真实凭据、生产坐标、飞行日志、模型权重、厂商原始资料和构建产物。

## 许可证与上游

基于 [QGroundControl](https://github.com/mavlink/qgroundcontrol) 二次开发，保留上游和第三方版权、许可证约束。产品历史始于源码快照，不能把快照导入作者当作全部源码作者。见 [COPYING.md](COPYING.md) 与各源文件声明。
