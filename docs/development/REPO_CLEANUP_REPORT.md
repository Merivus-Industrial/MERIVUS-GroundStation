# MERIVUS 仓库整理报告

整理分支：`integration/merivus-system-clean-main`

整理时间：2026-07-14

## 审计结果

- 远程仓库：`https://github.com/Ale-xl/MERIVUS`
- 本地 `origin`：`ssh://git@ssh.github.com:443/Ale-xl/MERIVUS.git`
- 默认分支：`main`
- main 当前提交：`026e8d0 完善 SITL 安全设置与地图指令交互`
- GitHub CLI 登录账号：`Ale-xl`
- 当前完整功能来源分支：`origin/feat/ai-model-fewshot-eval`

打开的 PR：

- `#2`：`codex/ai-assistant-bubble`，Draft。
- `#3`：`feat/ai-qa-intent-separation-and-repo-cleanup`，Open。
- `#4`：`feat/ai-model-fewshot-eval`，Open。

已合并 PR：

- `#1`：`codex/merivus-sitl-ui-safety-polish`，Merged。

## 备份

本次仅创建本地备份，未推送 backup 分支或 tag。

- `backup/before-repo-cleanup-20260714-1637`
- `backup/main-before-cleanup`
- `backup/ai-qa-intent-separation-before-cleanup`
- `backup/ai-model-fewshot-before-cleanup`
- `backup/current-before-repo-cleanup-20260714-1637`

## 功能来源判断

`origin/feat/ai-model-fewshot-eval` 包含以下能力，作为本次主线整理来源：

- QGC AI 面板。
- Python Local Agent。
- QGC C++ `AiAgentClient`。
- `AiServiceSupervisor`。
- Agent PyInstaller 打包。
- Ollama / `qwen3:8b` Provider。
- Provider 设置 UI。
- `ActionProposal`。
- `AiSchemaValidator`。
- `AiCommandPolicy`。
- QA/指令分离。
- 中文建议卡。
- README / docs / schemas / configs 系统化。
- 模型 few-shot / eval 优化。

## 集成方式

从最新 `main` 新建 `integration/merivus-system-clean-main`，使用 squash 合入：

```text
origin/feat/ai-model-fewshot-eval
```

本次 squash 没有产生冲突。

## 文档整理

保留核心文档：

- `docs/INDEX.md`
- `docs/architecture/PROJECT_OVERVIEW.md`
- `docs/architecture/CURRENT_STATE.md`
- `docs/architecture/TARGET_ARCHITECTURE.md`
- `docs/architecture/SAFETY_BOUNDARIES.md`
- `docs/architecture/AI_INTENT_POLICY.md`
- `docs/architecture/AGENT_MODEL_PROVIDERS.md`
- `docs/architecture/AGENT_SUPERVISOR.md`
- `docs/architecture/LOCAL_AGENT_HTTP.md`
- `docs/architecture/QGC_AGENT_CLIENT.md`
- `docs/architecture/HARDWARE_INTEGRATION.md`
- `docs/hardware/AIRFRAME_7INCH_PROTOTYPE.md`
- `docs/hardware/WEIGHT_AND_POWER_BUDGET.md`
- `docs/development/BUILD_WINDOWS.md`
- `docs/development/AGENT_DEVELOPMENT.md`
- `docs/development/GIT_WORKFLOW.md`
- `docs/development/GITHUB_REPO_GUIDELINES.md`

归档历史文档：

- `docs/BUILD_WINDOWS.md` -> `docs/archive/BUILD_WINDOWS_LEGACY.md`
- `docs/DEVELOPMENT_WORKFLOW.md` -> `docs/archive/DEVELOPMENT_WORKFLOW_LEGACY.md`
- `docs/ENVIRONMENT_SETUP.md` -> `docs/archive/ENVIRONMENT_SETUP_LEGACY.md`
- `docs/SITL_LINUX_SETUP.md` -> `docs/archive/SITL_LINUX_SETUP_LEGACY.md`
- `docs/PX4_114_SITL_MERIVUS_NOTES.md` -> `docs/archive/PX4_114_SITL_MERIVUS_NOTES.md`

原因：这些文件属于早期环境、SITL 和构建过程记录，已经被 `docs/development/*`、`docs/architecture/*` 与 `docs/INDEX.md` 覆盖为当前入口；仍保留历史参考价值，因此归档而非删除。

## 本地产物清理

已定向删除：

- `E:\MERIVUS\build`
- `E:\MERIVUS\agent\build`
- `E:\MERIVUS\agent\dist`
- `E:\MERIVUS\.pytest_cache`
- `E:\MERIVUS\agent\.pytest_cache`
- `E:\MERIVUS\agent\app\__pycache__`
- `E:\MERIVUS\agent\app\providers\__pycache__`
- `E:\MERIVUS\agent\app\services\__pycache__`
- `E:\MERIVUS\agent\tests\__pycache__`
- `E:\MERIVUS\agent\tools\__pycache__`

没有删除：

- 本地证书。
- 厂商 PDF。
- `.agents/`。
- `.qtcreator/`。
- 源码、测试、文档、schemas、configs、README、`.env.example`。

## schemas / configs

JSON 校验通过：

- `schemas/agent_request.schema.json`
- `schemas/agent_response.schema.json`
- `schemas/action_proposal.schema.json`
- `configs/agent.default.example.json`
- `configs/ai_command_policy.example.json`

YAML 模板完成文本结构检查：

- `configs/whitelist_commands.example.yaml`

## 验证结果

- `agent` 目录执行 `python -m pytest`：63 passed，6 warnings。
- 执行 `tools/dev/test-ai-intent-policy.ps1`：`AiIntentPolicyTest passed=27 failed=0`。
- 执行 `git diff --check`：通过。
- 执行 `tools/dev/build-merivus.ps1 -Configuration Release`：通过，生成 `build/Desktop_Qt_5_15_2_MSVC2019_64bit-Release/staging/MERIVUS.exe`。
- 执行 `tools/dev/build-agent.ps1 -Configuration Release`：通过，生成 `agent/dist/merivus-agent/merivus-agent.exe`，并复制到 MERIVUS staging。
- 使用打包后的 `merivus-agent.exe` 做 HTTP smoke：`/health` 返回 `ok`，`/merivus/info` 返回 `flight_execution_enabled=false`，`/merivus/agent` 返回 `status=ok`，proposal 不包含 `executed` 字段。
- 未执行真实飞行、真实 MAVLink 链路、真机端到端或 GUI 人工 smoke。

## 安全声明

- 没有新增真实飞行动作执行入口。
- 没有新增 Command Executor。
- 没有新增 MAVLink / Vehicle / Swarm / PX4 执行调用。
- AI proposal 当前继续由 QGC C++ 本地策略判定，并保持 `executable=false`。
- 本次不删除远程分支，不关闭旧 PR，不自动 merge。

## 分支处理建议

建议保留：

- `main`
- `integration/merivus-system-clean-main`

待 integration PR 合并后，可由用户确认后再处理：

- `codex/ai-assistant-bubble`
- `feat/ai-qa-intent-separation-and-repo-cleanup`
- `feat/ai-model-fewshot-eval`

建议后续命令示例：

```powershell
gh pr close <number> --comment "已由 integration/merivus-system-clean-main 统一集成"
git push origin --delete <branch>
```

以上命令本次未执行。
