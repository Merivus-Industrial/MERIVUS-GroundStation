# AI 问答与指令分离

本阶段分支：`feat/ai-qa-intent-separation-and-repo-cleanup`。

## 背景

用户输入“未获得有效位置估计和EKF2报警是什么原因？”时，本地模型曾将其误判为 `vehicle.query_position` proposal。由于没有明确 `vehicle_id`，QGC 本地策略会拒绝该 proposal，并显示一张未执行建议卡片。

该输入本质上是故障原因解释，不是查询某架无人机位置，也不是飞行动作建议。

## 修复策略

本阶段增加一层保守的用户意图分类：

- `answer_only`
- `log_explanation`
- `status_query`
- `ui_action`
- `flight_proposal`
- `forbidden_command`

当用户输入包含“是什么意思、什么原因、为什么、如何解释、报警、报错、故障原因、EKF2、GPS未定位、未获得有效位置估计、Preflight Fail、No GPS”等解释类信号时，优先按问答或日志解释处理。

除非用户明确说“查询一号机状态、查询三号机位置、读取当前RTK状态”等，否则这类输入应返回 `proposal=null`。

## 实现位置

- `agent/app/providers/system_prompt.py`：提示词明确问答优先、明确指令才生成 proposal。
- `agent/app/proposal_normalizer.py`：新增 `classify_user_intent()`，在模型输出后做最终保守裁决。
- `agent/app/providers/ollama.py`：将用户原始 message 传入 normalizer。
- `agent/tests/fixtures/model_eval_cases.json`：新增解释类问答评估样例。
- `custom/res/Merivus/MerivusAIAssistantPanel.qml`：建议卡片中文化。

## 安全边界

该修复不绕过 `AiSchemaValidator` 或 `AiCommandPolicy`。它只在 Agent 输出进入 QGC 之前减少错误 proposal；所有保留下来的 proposal 仍由 QGC C++ 重新校验，并保持 `executable=false`。

本阶段没有新增云 Provider、MCP、命令执行器、Vehicle/MAVLink/Swarm/PX4 执行路径。
