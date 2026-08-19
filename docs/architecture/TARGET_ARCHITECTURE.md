# MERIVUS 目标架构

本文档描述目标分层和责任边界。除“从代码中确认”和“硬件手册确认”外，均为目标设计或当前假设，不代表已经实现。

## 目标分层

```text
PX4 Flight Stack
  ↑/↓ MAVLink 2
HyperLTE 4G 模块 / 厂商云 / 未来 MERIVUS Device Gateway
  ↑/↓ TCP MAVLink
MerivusGroundControl(QGC custom desktop)
  ↔ 127.0.0.1 HTTP
Merivus Local Agent
  ↔ 模型 Provider / MCP 工具 / GIS 查询
Merivus Cloud API / Web Console / GIS Safety / Media Service
```

## QGC 桌面客户端

### 负责

- QML 用户界面、地图、飞行器显示、多机选择。
- 复用 QGC `Vehicle`、`MultiVehicleManager`、`MissionManager`、`LinkManager`。
- 使用 C++ `QNetworkAccessManager` 与本机 Agent 异步通信。
- 使用 `QProcess` 启动和监管本机 Agent。
- 本地飞行指令白名单、前置条件检查、风险分级、确认弹窗和审计。
- Agent 离线时保持地图、遥测、Link 设置和手动飞控能力可用。

### 不负责

- 不直接承载 LLM 推理、模型 Provider、MCP 编排。
- 不承担设备网关、云端账户、视频转发、GIS 计算服务。
- 不把通用 `send_mavlink` 暴露给 AI 或云端。

## 本机 Agent

### 建议形态

- 开发期：`agent/` 下 Python/FastAPI 服务。
- 发布期：打包为 `agent/merivus-agent.exe`。
- 默认 HTTP endpoint：`http://127.0.0.1:8765/merivus/agent`。

### 负责

- AI 模型适配和 Provider 路由。
- 对话上下文管理。
- 日志解释、GIS 查询编排、自然语言意图解析。
- 返回文本回复和结构化 `ActionProposal`。

### 不负责

- 不访问飞控串口。
- 不直接发送 MAVLink。
- 不自行执行解锁、起飞、降落、返航、任务上传或参数写入。
- 不保存真实 API Key 到代码仓库。

## 设备接入服务

### 目标职责

- 多个 HyperLTE/4G 设备 TCP 长连接。
- 设备身份识别、认证、心跳、断线重连。
- MAVLink 帧路由和飞行器连接映射。
- 控制权租约、限流、审计。
- 后续支持 TLS 或 VPN 接入。

### 当前阶段建议

- 不立即替换厂商云服务器。
- 先记录现有 QGC TCP Link 可用链路。
- POC 只使用模拟 MAVLink 或回放数据，不接真实飞行控制。

## 云端业务服务

账号、组织、权限、飞行器归属、个人设置、地图资产、数据库与部署的完整目标契约见 [账号体系与云端后端工程设计](ACCOUNT_AND_CLOUD_BACKEND_DESIGN.md)。本节只保留系统总览，不作为账号与数据库实现的唯一规范。

### 目标职责

- 用户认证、组织、角色和权限。
- 无人机、物联网卡、用户绑定。
- 任务管理、软件配置、日志查询。
- AI 模型配置和 Web Console。

### 当前阶段建议

- 不急于在 QGC 仓库内实现。
- 先定义数据边界和接口契约，等本地链路、Agent、安全策略稳定后再启动。

## 数据系统

### 建议组件

- PostgreSQL：用户、设备、任务、权限、审计。
- PostGIS：航线、禁飞区、建筑物和空间查询。
- Redis：在线状态、会话、缓存、控制权租约。
- 对象存储：视频、日志、GeoTIFF、DEM、DSM、3D Tiles。
- 遥测第一版可用 PostgreSQL 分区表保存关键状态，不同步保存每个 MAVLink 包。

## 视频服务

### 目标原则

- 控制链路和视频链路逻辑分离。
- 视频失败不能阻塞 MAVLink 心跳和安全控制。
- QGC 优先复用原生视频能力。
- RTSP/SRT/WebRTC 地址和端口必须配置化。

### 当前阶段建议

- 先记录厂商 RTSP 接入方式和端口规则。
- 不立即自建 Media Service。
- 多机视频必须能明确绑定到对应飞行器。

## GIS 安全服务

### 目标原则

- 显示数据和计算数据分离。
- 3D Tiles、倾斜摄影、卫星图可以用于展示，但不能作为唯一碰撞计算依据。
- 计算应基于 DEM/DSM、GeoTIFF、建筑物矢量、禁飞区、机场、桥梁、电线等数据。
- 输出安全报告和修改建议，不自动修改任务。

## 从代码中确认

- 当前 QGC 自定义层已具备接入点：`custom/custom.pri`、`custom/qgroundcontrol.qrc`、`custom/res/Merivus/*`、`custom/src/Swarm/*`。
- 当前 AI 面板 endpoint 已配置为 `http://127.0.0.1:8765/merivus/agent`，但通信在 QML 中完成，后续应迁移到 C++ 客户端。
- 当前视频、Link、Vehicle、Mission 基础能力主要来自 QGC 原生模块。

## 当前假设

- MERIVUS 桌面端继续以 QGC custom build 方式演进，避免大范围 fork 原生 QGC 文件。
- Agent、Device Gateway、Cloud API、GIS Safety、Media Service 应尽量独立目录或独立仓库，不直接混入 QGC `src/`。
- QGC 是唯一能把 AI proposal 转成真实飞行操作的本地执行边界。

## 待确认事项

- Agent 是否只允许本机 `127.0.0.1`，是否需要本机认证 token。
- 设备网关未来是兼容厂商云还是完全替代。
- 多机控制权租约的业务规则。
- 视频协议第一版优先 RTSP、SRT 还是 WebRTC。
- GIS 数据来源、精度、更新周期和责任边界。
