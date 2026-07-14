# MERIVUS 本地模型输出稳定性

本阶段分支：`feat/ai-model-stability`。

本阶段目标是在已经接入本机 Ollama `qwen3:8b` 的基础上，提高模型输出 `proposal` 的稳定性和可验收性。范围仍限制在 Python Local Agent 和文档/测试：不增加云 Provider，不接 MCP，不增加命令执行器，不修改 Vehicle、MAVLink、PX4、SwarmController 或真实飞行动作链路。

## 核心边界

- Agent 只生成 `reply` 和可选 `proposal`。
- QGC C++ 仍负责最终 schema 校验、本地风险重算、策略判定和审计。
- 当前阶段 `flight_execution_enabled=false`。
- 所有飞行动作类 proposal 仍只是未执行建议，QGC 侧 `executable=false`。
- 不提交模型权重、Ollama 程序、`.env`、Token、API Key、构建产物或日志。

## 提示词变化

`agent/app/providers/system_prompt.py` 收紧了本地模型输出规则：

- 明确只能使用 MERIVUS 标准 command。
- 明确只能使用标准参数名：`vehicle_id`、`altitude_m`、`latitude`、`longitude`、`page`。
- 用户明确要求查询、打开页面、地图定位、任务草稿或飞行动作时，应优先返回结构化 `proposal`。
- 只有闲聊、信息不足、能力边界外、存在明显歧义或无法安全解析时才返回 `proposal=null`。
- 不允许声称已经执行，不允许生成 MAVLink、shell、script、PX4 参数写入内容。
- 不允许模型输出 `risk`、`localRisk`、`policyDecision`、`requiresConfirmation`、`executable`、`executed` 等越权字段。
- 缺失目标飞机时，飞行动作类建议必须使用 `vehicle_id=null` 或不生成 proposal；不得捏造飞机 ID。
- 缺失高度时不得默认起飞高度。

## 输出规范化

`agent/app/proposal_normalizer.py` 在 Ollama 输出进入 `AgentResponseData` 前做一次保守规范化：

- 将常见 command alias 映射到标准 command。
- 将常见参数别名映射到标准参数名。
- 删除模型越权字段和危险结构。
- 丢弃当前 command 不允许的参数。
- 对坐标、高度、`vehicle_id` 做基础类型和范围检查。
- 对无法安全规范化的 proposal 返回 `proposal=null`，并在 `reply` 中追加“无法形成结构化建议”的原因。

该规范化层不是最终安全边界。它只降低本地模型输出波动；最终安全仍由 QGC C++ 的 `AiSchemaValidator` 和 `AiCommandPolicy` 执行。

## Command alias

本阶段覆盖的主要别名包括：

- `status`、`query_status`、`vehicle.status` -> `vehicle.query_status`
- `battery`、`query_battery` -> `vehicle.query_battery`
- `gps`、`position` -> `vehicle.query_position`
- `rtk` -> `vehicle.query_rtk`
- `log`、`error_log` -> `log.explain_error`
- `select_vehicle` -> `ui.select_vehicle`
- `open_page` -> `ui.open_page`
- `focus_coordinate` -> `map.focus_coordinate`
- `mission_draft` -> `mission.create_draft`
- `mission_analysis`、`mission.analyse` -> `mission.analyze`
- `arm`、`force_arm` -> `vehicle.arm`
- `takeoff`、`take off` -> `vehicle.takeoff`
- `land` -> `vehicle.land`
- `rtl`、`return_to_launch`、`return to launch` -> `vehicle.rtl`
- `pause`、`hold` -> `vehicle.pause`
- `goto`、`go_to` -> `vehicle.goto`
- `upload_mission` -> `mission.upload`
- `start_mission` -> `mission.start`
- `write_param` -> `param.write`
- `send_mavlink`、`mavlink` -> `mavlink.send_raw`

## 参数 alias

主要参数别名映射：

- `drone_id`、`drone`、`uav_id`、`uav`、`vehicle`、`target_vehicle`、`aircraft_id`、`vehicleId` -> `vehicle_id`
- `height`、`altitude`、`alt`、`takeoff_height`、`target_altitude`、`targetAltitude` -> `altitude_m`
- `lat` -> `latitude`
- `lng`、`lon` -> `longitude`
- `view`、`panel` -> `page`

## 测试与评估

本阶段新增 `agent/tests/fixtures/model_eval_cases.json`，包含 52 条中文评估样例，覆盖只读查询、UI-only、地图定位、任务草稿/分析、高风险飞行动作、强制拒绝和越权字段场景。

`agent/tools/run_model_eval.py` 是显式 opt-in 工具，必须传入 `--run-real-model` 才会调用本机 Ollama。该脚本只输出统计和失败摘要，不保存完整模型回复。

当前真实 `qwen3:8b` 评估仍未达到全通过：52 条样例中 command 匹配 28 条，proposal 形态匹配 29 条。失败主要集中在模型仍倾向于把状态查询、日志解释、任务分析或高风险动作回答成纯文本。后续建议继续优先做 prompt/eval 迭代，而不是进入云 Provider 或真实执行链路。

## GUI Provider 设置补充：feat/agent-provider-settings

本阶段之后，QGC AI 面板可以选择 `Mock` 或 `Ollama`，并把 `qwen3:8b`、Ollama 本机地址、超时和 fallback 设置传给由 QGC 自己启动的 Local Agent。该改动只影响 Agent 启动环境和 GUI 状态展示，不改变模型 normalizer、ActionProposal 策略或真实飞行动作边界。

当 `/merivus/info` 返回 `provider_ready=false` 时，QGC 聊天入口会提示 Provider 未就绪并停止发送 Agent 请求，避免把未启动的 Ollama 或缺失模型表现成普通聊天失败。

## few-shot / eval 增强补充：feat/ai-model-fewshot-eval

本轮继续限定在本地 `qwen3:8b` 稳定性范围内，不新增 DeepSeek、OpenAI、Gemini、MCP、云端服务或 Command Executor，也不修改 Vehicle / MAVLink / PX4 / Swarm 执行链路。

主要变化：

- `system_prompt.py` 增加少量高质量 few-shot 示例，覆盖纯问答不出 proposal、状态查询、位置查询、起飞建议和原始 MAVLink 禁止类请求。
- few-shot 示例只包含 `reply` 与 `proposal`，不包含 `executable`、`risk`、`policyDecision`、`requiresConfirmation` 等本地策略字段。
- `proposal_normalizer.py` 增加保守 recovery：当模型明确返回 `proposal=null`，且用户文本是可模板化解析的明确指令时，才补全只读或预览 proposal。
- recovery 不处理问答类、模糊类、地名转坐标、默认高度、默认飞机 ID、`param.write` 或 `mavlink.send_raw`。
- `model_eval_cases.json` 保持 72 条用例，并补充 `intent_type`、`expected_arguments`、`allow_normalizer_recovery`、`must_not_execute`、`category` 字段。
- `run_model_eval.py` 输出分类指标：QA no-proposal precision、Command proposal recall、Command accuracy、Argument accuracy、Safety invariant、Forbidden rejection。

旧评估基线为 `command_match=46/72`、`proposal_shape_match=46/72`。新指标用于定位失败类型，不保存完整模型回复日志。当前目标不是追求 100%，而是提高明确指令 proposal 召回，同时保持 QA 类问题稳定不误出 proposal。

本轮真实 `qwen3:8b` 评估结果：

- total：72。
- QA no-proposal：37/37，100.0%。
- Command proposal recall：24/32，75.0%。
- Command accuracy：23/32，71.9%。
- Argument accuracy：10/11，90.9%。
- Safety invariant：72/72，100.0%。
- Forbidden rejection：5/6，83.3%。

剩余失败主要集中在目标或参数不足的高风险动作、解锁类请求、启动任务类请求，以及一条 `查看GPS` 被模型误判为 `vehicle.query_rtk` 的命令混淆。后续仍建议继续 prompt/eval 迭代，不进入执行链路。
