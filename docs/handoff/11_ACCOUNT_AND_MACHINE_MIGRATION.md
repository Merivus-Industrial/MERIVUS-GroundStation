# 账号与电脑迁移清单

迁移前先区分“仓库内容”和“账号/本机状态”。Git 只会保存已提交并已推送的仓库文件；聊天记忆、登录状态、未提交文件和本机工具不会随 clone 自动恢复。

## 六类资产边界

| 类别 | 典型内容 | 同机换账号 | 换电脑 | 推荐处理 |
| --- | --- | --- | --- | --- |
| 仓库级文件 | 已提交源码、docs、schemas、configs、脚本、`AGENTS.md` | 不受 ChatGPT 账号影响 | 从正确远端 clone 可恢复 | 提交/推送前审查；用 tag/commit 校验 |
| 用户目录级 Codex 配置 | `%USERPROFILE%\.codex` 或自定义 `%CODEX_HOME%` 下的配置、Skills | 通常仍留在同一 Windows 用户目录，但插件/登录可能与账号绑定 | 不自动迁移 | 只备份经审查的 Skills/非敏感配置；不要复制 cache 或登录状态 |
| ChatGPT 账号级内容 | 对话、项目、记忆、连接器授权、插件可用性 | 新账号通常不会继承 | 不随电脑文件迁移 | 用本 handoff 重新建立上下文；重新安装/授权所需连接器 |
| GitHub 账号级内容 | 仓库权限、PR/Issue、组织、SSH/GPG 公钥登记 | 取决于当前 GitHub 登录 | 需重新登录/授权并登记新公钥 | `gh auth status`、远端只读检查；不要复制 Token |
| 本机开发环境 | Qt、VS、Python、Ollama、模型、Git、gh、VS Code、Qt Creator | 同机通常保留 | 必须重新安装/定位 | 按环境脚本检查；不要为了迁移把安装目录提交 Git |
| 密钥与认证信息 | API Key、PAT、SSH 私钥、Cookie、恢复码、`.env` 真值 | 可能仍在凭据管理器，但不应假设 | 必须通过官方流程重新授权或安全迁移 | 使用密码管理器/系统凭据机制；绝不提交 Git |

## 更换 ChatGPT / Codex 账号

### 通常仍保留

- `E:\MERIVUS` 工作区、Git 对象、当前分支和未提交文件，只要仍使用同一 Windows 用户与磁盘。
- 本机 Git 的 system/global/local 配置文件；但配置存在不代表新账号有远端权限。
- 用户目录中的 Codex Skills/非敏感配置可能仍在，但新账号是否可用、插件是否已安装应重新确认。
- Qt、VS、Python、Ollama、Git、gh、VS Code 等本机软件。

### 通常不会自动迁移

- 旧 ChatGPT/Codex 对话、账号记忆、项目内聊天上下文和自定义账号偏好。
- 连接器、插件、浏览器会话和第三方 OAuth 授权。
- 账号级 API 配额、组织权限、OpenAI/GitHub/云服务登录。

### 操作清单

1. 确认工作区无遗漏：`git status --short`、`git branch -vv`、`git remote -v`。
2. 对要保留的代码使用本地提交；需要跨设备时再由用户决定是否推送。未提交/忽略文件不会随 Git 自动迁移。
3. 新账号首轮只读使用 [AI / Codex 接手说明](10_AI_CODEX_HANDOFF.md) 的提示词。
4. 重新确认 Codex 插件、连接器和授权；不要从旧账号导出 Cookie 或登录状态。
5. 重新确认 GitHub 身份与仓库权限，避免把个人账号和组织账号混用。

## 更换电脑

### 需要获取或备份

- 克隆 MERIVUS 正确远端：当前审计 remote 为 `ssh://git@ssh.github.com:443/Ale-xl/MERIVUS.git`；迁移时先核对仓库所有者，不擅自改 remote。
- 如有尚未推送的本地分支/提交，使用受控 remote 或经审查的 `git bundle`；普通文件复制不能替代 Git 完整性检查。
- 仅备份经审查、无密钥的本地配置和用户级 Skills。
- 本地硬件手册是否可迁移取决于授权；不得因此提交到公开 Git。
- 未提交但必须保留的文件先人工分类，避免把日志、构建产物或凭据一起复制。

### 需要重新安装或定位

- Git 和 GitHub CLI。
- Qt 5.15.2 `msvc2019_64` Kit、Qt Creator 与 qmake/jom。
- Visual Studio 2022 C++ x64 工具链。
- Python 3.11 及 Agent 所需依赖；依赖安装由人工按项目文件执行，本任务不安装。
- VS Code / Codex。
- Ollama 和指定本地模型（仅在确需本地模型验证时）；模型权重不进入仓库。

### 用户级 Skills 恢复

1. 找到旧机 `%CODEX_HOME%\skills`；未设置时通常位于用户目录下的 `.codex\skills`。
2. 只复制自己创建或已审查的 Skill 源文件、引用和必要脚本。
3. 不复制插件 cache、临时下载、会话数据库、浏览器数据、Token 或登录状态。
4. 在新机启动新 Codex 任务，确认 Skills 被发现；缺失的官方/插件 Skill 应通过正常安装流程恢复。
5. Skill 中若有绝对路径，改为新机路径并重新审查权限。

## 新电脑验证顺序

### Git 与 GitHub CLI

```powershell
git --version
gh --version
gh auth status
git remote -v
git fetch --dry-run origin
git branch -vv
git status --short
```

`gh auth status` 和 `git fetch --dry-run` 可能访问网络；应由用户确认网络与账号后执行。本次文档任务只验证了命令版本，没有联网验证认证。

### Qt / Visual Studio / Python

在仓库根目录执行只读环境检查：

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/dev/check-windows-environment.ps1
python --version
```

环境检查通过只证明工具可定位，不等于 QGC 构建通过。完整构建应在独立任务中执行并记录产物与日志摘要。

### Agent

```powershell
cd agent
python -m pytest -p no:cacheprovider
```

仅在依赖已由人工准备时运行。测试通过不代表 Local Agent 服务、PyInstaller 或 QGC GUI 联调通过。

### Ollama

```powershell
ollama --version
ollama list
```

`ollama list` 需要本机服务。只有人工确认模型已安装后，才可显式运行本地 eval；不要自动 `pull`，不要连接云 Provider。

### QGC / Agent 联调

按 [环境、构建与运行](06_ENVIRONMENT_BUILD_AND_RUN.md) 与 [测试矩阵](07_TEST_AND_VERIFICATION_MATRIX.md) 从环境检查、局部测试、Mock 服务、GUI smoke、SITL 逐级验证。禁止跳到真实飞机。

## 永远不要提交或直接复制为迁移包

- API Key、GitHub Token、SSH 私钥、GPG 私钥。
- 邮箱密码、浏览器 Cookie、账号恢复代码、Codex/ChatGPT 登录状态。
- `.env` 真实值、操作系统凭据库、进程环境转储。
- 生产地址/坐标、设备凭据、真实飞行日志。
- 模型权重、构建产物、虚拟环境、插件 cache。

SSH 私钥如确需跨机，应通过专用安全流程处理；更推荐在新机生成新密钥并在 GitHub 登记公钥。

## 迁移完成判定

- 正确仓库、branch、tag 和 remote 可确认，工作区状态清楚。
- `AGENTS.md` 与本 handoff 可读，相对链接有效。
- GitHub/Codex/连接器均以新账号重新授权，没有复制登录状态。
- 环境脚本通过；各测试只按实际执行结果标记。
- 默认无外网模型、无真实设备连接、无 AI 飞行执行。