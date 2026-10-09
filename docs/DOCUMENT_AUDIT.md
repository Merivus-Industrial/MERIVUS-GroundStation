# 文档审计

审计日期：2026-09-01

## 结论

原有文档以 AI Agent、多机任务、硬件和构建为主，内容大多仍有效，但缺少代码责任图、调用链、Vehicle/QML 索引和 FTC 遥测契约。本轮补齐这些入口，不复制已有专题文档。

| 文档 | 结论 | 处理 |
| --- | --- | --- |
| `docs/INDEX.md` | 有效，缺少本轮新增入口 | 更新索引 |
| `docs/handoff/README.md` | 有效，偏 Agent 与阶段交接 | 保留，新增系统化入口 |
| `docs/architecture/PROJECT_OVERVIEW.md` | 有效，尚未覆盖 FTC 链路 | 由 `PROJECT_STATUS.md` 补充当前状态 |
| `docs/architecture/CURRENT_STATE.md` | 有效，属于此前产品状态 | 保留，不让它承担代码索引职责 |
| `docs/architecture/INTERFACE_CONTRACTS.md` | 有效，主要描述 Agent/外部接口 | FTC 使用独立契约 |
| `docs/architecture/SAFETY_BOUNDARIES.md` | 有效 | FTC 文档引用同一安全边界，不另造规则 |
| `docs/development/BUILD_WINDOWS.md` | 有效，记录 Qt/MSVC/GStreamer 基线 | `BUILD.md` 作为跨入口导航 |
| SITL 与 AI 测试文档 | 有效，覆盖各自专题 | `TEST_MATRIX.md` 汇总，不复制步骤 |
| `docs/releases/*` | 有效，属于版本历史 | 不改写为当前架构说明 |

## GitNexus 生成物审计

`npx gitnexus@latest analyze` 生成的 `.gitnexus/` 索引保留在本机并由目录内 `.gitignore` 忽略。自动生成的 `CLAUDE.md` 和 `.claude/skills/` 已删除，避免与仓库现有 `AGENTS.md` 形成两套规则。

最终索引统计：5,518 个文件、127,409 个节点、175,295 条边、2,256 个聚类、252 条流程；对应提交以本地 `.gitnexus/meta.json` 的 `lastCommit` 为准。工具跳过了 101 个超过 512 KiB 的文件。Eigen 等模板代码触发调用候选上限；10,274 个候选入口中有 10,074 个未进入流程排名，2,587 个被调函数受最大分支数限制，22 次遍历触发单入口预算。Qt 信号槽、QML 属性和跨语言绑定也存在静态分析缺口。因此本文档只把 GitNexus 用于 C++ 结构定位和影响提示，QML 主路径另用资源表与源码引用核对。

## 维护规则

- 一个事实只保留一个主文档；其他文档用链接引用。
- 架构图写当前路径，不写已被替代的方案。
- 运行结果、提交号和产物校验值进入发布记录，不长期堆在架构文档。
- 文档新增链接后运行链接检查；删除源码入口时同步更新索引。
