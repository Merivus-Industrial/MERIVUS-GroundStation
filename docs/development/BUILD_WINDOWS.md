# Windows 构建说明

当前 Windows 构建以 Qt 5.15.2 / MSVC 2019 64-bit 工具链为基线，脚本位于 `tools/dev/`。

## GStreamer / RTSP 视频

Windows 构建需要同时安装 GStreamer MSVC x86_64 的 Runtime 和 Development
组件。qmake 优先读取官方安装器设置的
`GSTREAMER_1_0_ROOT_MSVC_X86_64`，并兼容默认的
`C:\Program Files\gstreamer\1.0\msvc_x86_64`、旧版
`C:\gstreamer\1.0\msvc_x86_64` 和 CI 使用的 D 盘路径。

MERIVUS 的 Windows 视频构建当前限定在 GStreamer `1.18.x` 系列；
本机已使用 `1.18.6` 完成干净 Release 构建。Runtime 安装器必须选择
**Complete**。如果 `gst-inspect-1.0 avdec_h264` 报告插件不存在，
说明 `gstlibav` 软件解码组件未安装；此时默认 D3D11 解码仍可工作，
但“Force software decoder”不可用。

安装或修改环境变量后，需要重启 Qt Creator，并重新运行 qmake；已有
Makefile 不会自动获得新环境。配置输出应包含：

```text
Using GStreamer from ...
Including support for video streaming
```

官方 MSVC 包中的三个主要运行库名为
`gstreamer-1.0-0.dll`、`gstvideo-1.0-0.dll` 和
`gstbase-1.0-0.dll`，没有 `lib` 前缀。成功链接后，qmake 会把
GStreamer 运行库、插件和 `gst-plugin-scanner.exe` 复制到 staging。
复制前会删除 staging 中原有的 GStreamer 插件目录，避免 1.18 与其他
版本的插件混装。切换 GStreamer 或 MSVC 版本时仍应使用
`-Clean -Reconfigure` 进行干净构建。

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
