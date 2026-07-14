# Windows 构建说明

当前 Windows 构建以 Qt 5.15.2 / MSVC 2019 64-bit 工具链为基线，脚本位于 `tools/dev/`。

## MERIVUS Release 构建

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/build-merivus.ps1 -Configuration Release
```

输出位于：

```text
build/Desktop_Qt_5_15_2_MSVC2019_64bit-Release/staging/MERIVUS.exe
```

该目录是本地构建产物，不提交 Git。

## Agent 打包

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/build-agent.ps1 -Configuration Release
```

脚本会用 PyInstaller 生成 `agent/dist/merivus-agent/merivus-agent.exe`，并复制到 QGC Release staging 的 `agent/` 目录。

## 局部测试

```powershell
cd agent
python -m pytest
```

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/test-ai-intent-policy.ps1
```

## 注意事项

- 不提交 `build/`、`agent/build/`、`agent/dist/`、`staging/`。
- 不把本地 Python venv、Ollama 模型或真实配置写入仓库。
- Release 构建通过不代表真实飞机验证通过；真实飞行必须另走人工安全流程。
