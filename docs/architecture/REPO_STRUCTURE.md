# MERIVUS 仓库结构

MERIVUS 保留 QGroundControl 上游结构，避免过早重排导致后续合并困难。MERIVUS 自研内容优先落在 `custom/`、`agent/`、`docs/`、`schemas/`、`configs/` 和 `tools/dev/`。

## 关键目录

- `custom/`：Custom Build 入口、QML、资源和 C++ 扩展。
- `custom/res/Merivus/`：MERIVUS 主界面、AI 面板和调度 UI。
- `custom/src/Ai/`：AI Agent client、Supervisor、ActionProposal、schema 校验和策略。
- `agent/`：Python FastAPI Local Agent、Provider、测试和 PyInstaller spec。
- `docs/architecture/`：架构、边界、路线图和阶段记录。
- `docs/development/`：构建、Agent 开发、Git/GitHub 流程。
- `docs/hardware/`：硬件接入和原型说明。
- `schemas/`：JSON Schema 契约草案。
- `configs/`：配置模板和策略示例。
- `tools/dev/`：本地构建、打包、测试脚本。

## 不提交内容

- `.env`、Token、API Key、真实 RTSP 地址、真实物联网卡敏感信息。
- `agent/build/`、`agent/dist/`、`build/`、`staging/`。
- Ollama 模型、模型权重、向量库、临时日志。
- 厂商 PDF、DOCX、Excel、截图等非源码资料。

如果发现 build/dist/staging 已被 Git 跟踪，先停止并报告，不直接改历史。
