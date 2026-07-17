# Git 工作流

## 分支

每个阶段使用独立分支，例如：

- `feat/local-agent-http`
- `feat/qgc-agent-client`
- `feat/ai-intent-policy`
- `feat/agent-provider-settings`

当前阶段示例：

```powershell
git switch -c feat/ai-qa-intent-separation-and-repo-cleanup
```

## 提交前检查

```powershell
git status --short
git diff --check
git status --ignored --short
```

敏感和产物检查：

```powershell
git ls-files | findstr /i "pdf docx xlsx log env dist build staging ollama models .venv"
```

命中 QGC 上游图片或源码文件不一定是问题；重点确认没有本地密钥、模型、构建产物、厂商手册或真实设备凭据。

## 禁止事项

- 不强推共享分支。
- 常规开发通过短生命周期分支和 PR 合入 `main`；仅在明确授权的阶段收口操作中允许受控快进并直接推送主线。
- 不提交 `.env`、Token、API Key、厂商 PDF、构建产物、模型文件。
- 不在 AI 相关分支顺手修改真实飞控执行逻辑。
