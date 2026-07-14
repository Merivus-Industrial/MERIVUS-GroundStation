# MERIVUS Windows Build

Use the verified build entry point:

```powershell
.\tools\dev\check-windows-environment.ps1
.\tools\dev\build-merivus.ps1 -Configuration Release
```

The binary is emitted to:

```text
build\Desktop_Qt_5_15_2_MSVC2019_64bit-Release\staging\MERIVUS.exe
```

Use `-Reconfigure` after qmake project or resource changes. Use `-Clean` only
when a clean rebuild is necessary; the script verifies that deletion remains
inside this repository's `build` directory.
