# MERIVUS 当前 TCP Link 基线

本文档记录 `test/current-link-baseline` 分支对当前 QGC TCP Link 的保护和诊断方式。本分支只做只读诊断，不接管原生连接、断开、重连或 MAVLink 发送路径。

## 当前 QGC 原生 Link 路径

现有通信链路继续由 QGroundControl 原生组件负责：

- `src/ui/preferences/LinkSettings.qml`：原生 Link 配置列表、添加、编辑、连接和断开入口。
- `src/ui/preferences/TcpSettings.qml`：TCP 主机和端口编辑界面。
- `src/comm/LinkConfiguration.*`：持久化 Link 配置对象，设置根路径为 `LinkConfigurations`。
- `src/comm/LinkManager.*`：创建、连接、断开和管理 Link。
- `src/comm/TCPLink.*`：原生 TCP Socket 拥有者和连接实现。
- `src/Vehicle/VehicleLinkManager.*`：Vehicle 侧链路关联和通信丢失判断。

## MERIVUS 诊断层职责

新增 `custom/src/Diagnostics/MerivusLinkDiagnostics.*`，职责仅限：

- 读取 `QGroundControl.linkManager.linkConfigurations` 对应的现有配置；
- 识别 `LinkConfiguration::TypeTcp`；
- 读取 TCP 配置名称、host、port；
- 读取 `LinkConfiguration.link` 和 `LinkInterface::isConnected()`；
- 监听已有 Link 的 `connected`、`disconnected`、`communicationError` 信号；
- 向 QML 暴露只读摘要；
- 使用 `merivus.link.diagnostics` 记录状态变化。

## 不拥有 Socket 的说明

MERIVUS 诊断层不创建 `QTcpSocket`，不调用 `connectToHost`，不发送 MAVLink，不实现重连。TCP Socket 仍只由原生 `TCPLink` 拥有。

## 配置保存位置

TCP Link 配置由原生 QGC 保存到 `LinkConfigurations` 设置组。主机和端口来自 `TCPConfiguration::host()` 与 `TCPConfiguration::port()`。历史厂商服务器地址和端口只能作为用户配置或历史记录，不能写成默认值或业务常量。

## 可获得的状态

当前代码可以可靠读取：

- `configuredLinkCount`：非动态 Link 配置数量；
- `tcpLinkCount`：TCP Link 配置数量；
- `connectedTcpLinkCount`：已连接 TCP Link 数量；
- `hasConfiguredTcpLink`；
- `hasConnectedTcpLink`；
- `summaryState`；
- `summaryText`；
- `lastStateChange`；
- 单个 TCP Link 的 `displayName`、`linkType`、`host`、`port`、`connected`、`lastError`。

顶栏 TCP 状态从原先硬编码“未配置”改为读取诊断摘要。点击行为仍打开原生 Link 设置页。

## 无法获得或未实现的状态

当前不伪造以下信息：

- 真实链路延迟：没有现成来源，显示为未知；
- 设备 ID 或 Vehicle/system id 映射：本分支未建立可靠映射，显示为未绑定；
- 原生 TCP 的完整连接中细分状态：原生 `TCPLink` 当前同步等待连接，公开 API 只能稳定区分 `link` 是否存在和 `isConnected()`；
- 错误根因推断：只保留原始错误摘要，不编造原因；
- 视频链路状态：本分支只诊断 QGC MAVLink TCP Link，不诊断 RTSP/视频链路。

## 多 Link 处理方式

同时存在多个 TCP Link 时，摘要显示已连接数量，例如 `1/2 已连接`。诊断详情保留每条 TCP Link 的名称、host、port 和连接状态。本分支不建立完整设备管理页，也不默认假设只有一架无人机。

## Mock/SITL 测试方法

允许的验证方式：

- 无任何 Link 配置启动，观察顶栏显示“未配置”；
- 添加 TCP 配置但不连接，观察“已配置/未连接”；
- 使用本地 Mock TCP 服务验证连接状态变化；
- 使用 PX4 SITL 或 MAVLink 回放验证原生 Link 行为仍由 QGC 管理；
- 断开本地连接后观察摘要恢复；
- 查看 `merivus.link.diagnostics` 日志分类下的配置发现、连接、断开和错误摘要。

禁止自动连接真实厂商服务器、自动控制真实无人机、自动修改 PX4 参数或自动测试电机。

## 当前厂商服务器依赖

当前项目仍依赖厂商 4G 数据链路和用户在 QGC 原生 Link 设置中保存的 TCP 配置。MERIVUS 本分支不替换厂商服务器，不自动连接历史地址，不自建云服务。

## 后续自建设备网关边界

后续如果建设 MERIVUS 自有设备网关，应在独立分支设计认证、设备注册、链路保活、审计日志和安全策略。本分支只提供当前 QGC TCP Link 的只读诊断，不作为网关实现。
