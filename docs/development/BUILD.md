# 构建与生成入口

## GroundStation

Windows 产品构建基线为 Qt 5.15.2、MSVC 2019 64-bit。完整安装和 GStreamer 要求见 [Windows 构建说明](BUILD_WINDOWS.md)。常用入口：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/build-merivus.ps1 -Configuration Release
```

构建产物位于 `build/Desktop_Qt_5_15_2_MSVC2019_64bit-Release/staging/`，不提交 Git。

## 生成 MERIVUS MAVLink 头文件

脚本默认从同级 `FirmwarePX4/src/modules/mavlink/mavlink` 使用锁定生成器，并校验子模块提交：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/generate-merivus-mavlink.ps1 -Python python
```

只检查仓库头文件能否复现：

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/generate-merivus-mavlink.ps1 -Python python -Check
```

Python 环境需要 FirmwarePX4 `pymavlink/requirements.txt` 中的 `lxml`、`future` 和 `wheel`。脚本只写临时目录和已跟踪的 `merivus`、`merivus_ftc` 头文件目录，不修改 MAVLink 子模块。

脚本固定 `PYTHONHASHSEED=0`，并机械清理生成头的行尾空格。锁定版本的生成器会把 Python 哈希写入主方言头文件；不固定种子时，协议相同也会产生字节不同的生成物。

## 轻量检查

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/test-ftc-telemetry-contract.ps1
git diff --check
```

有 Qt 工具链时再运行 `qmllint` 和 C++ 编译。没有对应工具时应在交付说明中写明，不能把静态脚本称为完整构建。

## Linux AppImage

当前 CI 在 Ubuntu 24.04 x86_64、Qt 5.15.2 下构建。`deploy/create_linux_appimage.sh` 使用同一构建环境的 SDL2 运行库和已生成的 staging；不再混入旧 Debian SDL/DirectFB 软件包。产物以 Ubuntu 24.04 的系统 ABI 为基线，不承诺兼容更旧发行版，仍需目标机器上的图形、音视频与设备访问验收。

打包工具固定为 AppImageKit 12 并校验 SHA-256；通过解包运行避免依赖 CI 的 FUSE 设备。缺失输入、运行库或校验失败会中止打包。工作流同时保留 AppImage、SHA-256 和 `.build-info`（源码提交、版本、构建类型、系统、Qt、编译器、SDL2 版本和打包工具摘要）。这些信息描述实际构建，不等同于跨环境逐字节重建已经验证。
