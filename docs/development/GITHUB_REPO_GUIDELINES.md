# GitHub 仓库规范

目标仓库：

```text
https://github.com/Ale-xl/MERIVUS
```

允许的 remote 形式：

```text
https://github.com/Ale-xl/MERIVUS
git@github.com:Ale-xl/MERIVUS.git
ssh://git@ssh.github.com:443/Ale-xl/MERIVUS.git
```

## 推送前检查

```powershell
gh auth status
gh repo view Ale-xl/MERIVUS
git remote -v
git status --short
```

如果 `origin` 不是 `Ale-xl/MERIVUS`，停止并报告，不擅自修改 remote。

## 推送规则

- 推送当前功能分支，不推 main/master。
- 不使用 `--force`。
- 未经用户要求不创建 Pull Request。

示例：

```powershell
git push -u origin feat/ai-qa-intent-separation-and-repo-cleanup
```

## 仓库卫生

GitHub 仓库中不得出现：

- `.env`、Token、API Key。
- 厂商 PDF、DOCX、Excel、截图。
- Ollama 模型、模型权重、向量库。
- `build/`、`agent/build/`、`agent/dist/`、`staging/`。
- 真实 RTSP 凭据、物联网卡敏感信息或生产坐标。
