# MERIVUS 配置模板

本目录保存可提交的示例配置和策略模板。它们用于文档化跨模块契约，不包含真实密钥、真实地址、生产凭据或设备敏感信息。

当前模板不替代运行时实现：

- Agent 请求/响应仍由 `agent/app/schemas.py` 校验。
- QGC proposal 仍由 `custom/src/Ai/AiSchemaValidator.*` 校验。
- QGC 本地安全策略仍由 `custom/src/Ai/AiCommandPolicy.*` 执行。

复制模板到本地真实配置时，应放在 Git 忽略路径或用户配置目录中。
