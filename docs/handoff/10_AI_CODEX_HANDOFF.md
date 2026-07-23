# 新 GPT / Codex 接手说明

## 精炼上下文

MERIVUS 是基于 QGroundControl Custom Build 的多无人机调度地面站研发项目。当前产品基线为 `a40c9e4` / `v0.1.0-dev.1`，属于开发测试初版，不是生产版本。仓库已包含 MERIVUS QML、多机与链路界面、Python Local Agent、Mock/Ollama Provider、C++ Client/Supervisor、`ActionProposal`、Schema/Policy/Audit 和 Windows 构建/打包脚本。

AI 当前只生成文本和结构化 proposal。C++ 会重新校验和计算本地策略，QML 只显示；当前没有 AI Command Executor、统一 AI 确认器或 AI 到 Vehicle/MAVLink/PX4/Swarm 的执行映射。

用户的长期目标是建立安全、可维护的多无人机调度平台，逐步覆盖 4G/RTK 链路、多机控制权、GIS 航点安全、故障保护、审计及必要的云端能力。长期目标不是当前能力。

## 当前状态摘要

- **已完成并在本次有局部验证**：Python Agent 63 项测试、5 个 JSON 解析、Windows 工具链定位、AI 无执行引用的静态审计。
- **已实现但未完整验证**：QGC AI UI、C++ Client/Supervisor/Policy、Ollama Provider、PyInstaller onedir、QGC Release 构建链。
- **未实现**：AI Executor/统一确认、云 Provider/MCP、生产鉴权、Device Gateway、GIS/Media 生产服务。
- **当前无证据**：当前 HEAD 的完整 QGC 构建、GUI 联调、真实 Ollama 复测、SITL 回归、签名发布包、真实飞机端到端。
- **唯一 P0**：收敛非 AI 高风险 QGC 执行入口并建立 Mock/SITL 回归；见 [下一步](09_NEXT_STEPS.md)。

## 关键工程决策

1. QML 不保存 API Key，不直接承担 Agent 网络请求。
2. 模型放在本机 Agent 边界；QGC 主功能不依赖 Agent 在线。
3. LLM 只能生成 `reply`/`ActionProposal`，不能声称执行成功。
4. Schema、Policy、明确确认、审计和执行器必须分层；当前只实现到显示/审计。
5. 外网、云 Provider 与 AI 飞行执行默认关闭；注意 Python 直接 env 路径尚非强制 loopback。
6. Agent 发布使用 PyInstaller onedir，不打包 Ollama/模型权重。
7. Normalizer 只减少模型波动，不是最终安全边界。
8. GIS 显示数据与安全计算数据分离；3D Tiles 不能作为唯一碰撞依据。

详见 [决策记录](05_DECISIONS_AND_HISTORY.md)。

## 不可违反的安全原则

- 不自动连接/控制真实飞机、厂商生产服务或云模型，不自动修改 PX4/RTK 参数。
- 不给 LLM/Agent 通用 MAVLink、shell、参数写入或 Vehicle 执行能力。
- 不绕过 C++ Schema、Policy、明确用户确认和审计边界。
- 默认只用 Unit Test、Mock、回放或 PX4 SITL；真实测试必须由人工现场授权并执行。
- 不提交 `.env`、Token、API Key、SSH 私钥、设备凭据、生产坐标、飞行日志、模型权重、厂商原始资料或构建产物。

## 用户偏好的工作方式

- 先检查再修改；先读 `AGENTS.md` 和 handoff。
- 优先最小范围修改，不顺手重构。
- 节省 Token 和时间，不重复扫描构建产物、第三方依赖和无关目录。
- 不伪造执行结果，明确区分“脚本存在、历史通过、本次通过、尚未执行”。
- 一项独立工作一个短生命周期分支；保留用户未提交修改。
- 未经要求不 push、merge、rebase、删分支或改远端。
- 完成时报告分支、提交、文件路径/位置、验证、未验证、风险和待确认项。

## 第一次接手必须执行的只读检查

在仓库根目录执行：

```powershell
git status --short
git branch --show-current
git branch -a
git branch -vv
git log --oneline --decorate -30
git remote -v
git tag --list
git ls-files docs custom agent schemas configs tools
```

然后按顺序阅读：

1. `AGENTS.md`
2. `docs/handoff/README.md`
3. `docs/handoff/02_CURRENT_STATE.md`
4. `docs/handoff/03_SYSTEM_ARCHITECTURE.md`
5. `docs/handoff/05_DECISIONS_AND_HISTORY.md`
6. `docs/handoff/07_TEST_AND_VERIFICATION_MATRIX.md`
7. `docs/handoff/08_RISKS_KNOWN_ISSUES.md`
8. `docs/handoff/09_NEXT_STEPS.md`
9. 与任务直接相关的原有 docs 和源码

只读首轮要核对代码与文档是否一致，不创建分支、不修改文件、不安装依赖、不启动外部模型或真实设备。

## Git 工作方式

1. 记录起始状态；存在用户修改时不覆盖。
2. 明确任务范围后，从当前长期基线创建一个短生命周期分支。
3. 只修改任务需要的文件；提交前检查 `git diff --check`、`git diff --stat` 和逐文件 diff。
4. 只有用户明确要求才 push/开 PR/merge；禁止 force push。
5. squash 历史不能只用 ancestry 判断是否合入，结合 `e894666` 整理报告、当前代码和标签。

## 如何判断任务完成

- 验收标准逐条有代码、配置、测试或人工记录证据。
- 失败模式和安全边界没有被弱化。
- 相关局部测试本次实际通过，或明确写“尚未执行/环境不具备”。
- 文档与当前代码一致，过时事实被标注而非复制。
- diff 仅包含授权范围，无敏感信息和构建产物。
- 对真实设备/网络的任何验证均有明确人工授权；否则保持未验证。

## 标准完成报告

```text
当前分支：
提交：<hash 或 未提交>
修改文件与位置：
- path：修改内容与影响
业务代码：是/否
本次已执行验证：
本次未执行验证：
风险与已知限制：
待确认：
推荐唯一下一步：
```

## 可复制的首次接手提示词

```text
你现在只读接手 E:\MERIVUS，不修改任何文件、不创建分支、不安装依赖、不运行完整构建，不启动云模型、厂商服务、MAVLink 或真实飞机，也不修改 PX4/RTK 参数。

先阅读仓库根目录 AGENTS.md，然后按顺序阅读 docs/handoff/README.md、02_CURRENT_STATE.md、03_SYSTEM_ARCHITECTURE.md、05_DECISIONS_AND_HISTORY.md、07_TEST_AND_VERIFICATION_MATRIX.md、08_RISKS_KNOWN_ISSUES.md、09_NEXT_STEPS.md。

执行只读检查：git status --short、git branch --show-current、git branch -a、git branch -vv、git log --oneline --decorate -30、git remote -v、git tag --list；再定向读取与当前 P0 相关的源码和原有 docs。不要扫描 build、dist、venv、node_modules、第三方依赖、大型二进制或日志。

以当前代码/配置、Git、实际测试证据、与代码一致的 docs、历史归档的顺序判断事实。重点核对：AI 是否仍只显示 proposal、是否存在 AI 到 Vehicle/MAVLink/Swarm/PX4 的执行调用、非 AI 高风险入口清单、文档冲突和验证缺口。

输出一份中文接手确认报告：当前分支与提交、工作区状态、项目阶段、已完成/已实现未验证/规划/暂停/替代/无法确认、关键安全边界、代码与文档冲突、待确认问题，并只推荐一项下一步 P0。不要把脚本存在写成测试通过，不要复述大段文档。
```