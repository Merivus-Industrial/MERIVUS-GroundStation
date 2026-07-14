[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [int]$Jobs = [Math]::Max(1, [Environment]::ProcessorCount - 1),
    [switch]$Reconfigure,
    [switch]$Clean,
    [string]$QtRoot = "E:\Qt\5.15.2\msvc2019_64",
    [string]$QtCreatorRoot = "E:\Qt\Tools\QtCreator",
    [string]$VisualStudioRoot = "D:\Program Files\Microsoft Visual Studio\2022\Community",
    [string]$BuildDirOverride = ""
)

$ErrorActionPreference = "Stop"

function Get-MakefileVariable {
    param(
        [string[]]$Lines,
        [string]$Name
    )

    $pattern = "^$([Regex]::Escape($Name))\s*="
    for ($index = 0; $index -lt $Lines.Count; $index++) {
        if ($Lines[$index] -notmatch $pattern) {
            continue
        }

        $value = $Lines[$index] -replace $pattern, ""
        while ($value.TrimEnd().EndsWith("\") -and ($index + 1) -lt $Lines.Count) {
            $value = $value.TrimEnd()
            $value = $value.Substring(0, $value.Length - 1)
            $index++
            $value = "$value $($Lines[$index].Trim())"
        }

        return ($value -replace "\s+", " ").Trim()
    }

    throw "Makefile variable not found: $Name"
}

function Expand-MakefileVariableReferences {
    param(
        [string]$Value,
        [hashtable]$Variables
    )

    $expanded = $Value
    for ($pass = 0; $pass -lt 8; $pass++) {
        $before = $expanded
        foreach ($name in $Variables.Keys) {
            $expanded = $expanded.Replace("`$($name)", [string]$Variables[$name])
        }
        if ($expanded -eq $before) {
            break
        }
    }

    return ($expanded -replace "\s+", " ").Trim()
}

function Enable-MsvcCompileResponseFiles {
    param([string]$BuildDir)

    $makefile = Join-Path $BuildDir "Makefile"
    if (-not (Test-Path -LiteralPath $makefile)) {
        throw "Makefile not found: $makefile"
    }

    $lines = [IO.File]::ReadAllLines($makefile)
    $variables = @{
        DEFINES = Get-MakefileVariable -Lines $lines -Name "DEFINES"
        INCPATH = Get-MakefileVariable -Lines $lines -Name "INCPATH"
        CFLAGS = Get-MakefileVariable -Lines $lines -Name "CFLAGS"
        CXXFLAGS = Get-MakefileVariable -Lines $lines -Name "CXXFLAGS"
    }

    $cFlags = Expand-MakefileVariableReferences -Value $variables.CFLAGS -Variables $variables
    $cxxFlags = Expand-MakefileVariableReferences -Value $variables.CXXFLAGS -Variables $variables
    $includePath = Expand-MakefileVariableReferences -Value $variables.INCPATH -Variables $variables

    $ascii = [Text.Encoding]::ASCII
    $cRsp = Join-Path $BuildDir "merivus_cl_c_common.rsp"
    $cxxRsp = Join-Path $BuildDir "merivus_cl_cxx_common.rsp"
    [IO.File]::WriteAllText($cRsp, "$cFlags`r`n$includePath`r`n", $ascii)
    [IO.File]::WriteAllText($cxxRsp, "$cxxFlags`r`n$includePath`r`n", $ascii)

    $makefileText = [IO.File]::ReadAllText($makefile)
    $alreadyPatched = $makefileText.Contains('@merivus_cl_cxx_common.rsp') -and $makefileText.Contains('@merivus_cl_c_common.rsp')
    $patchedText = [Regex]::Replace($makefileText, '(?m)^CC\s*=.*cl\.exe\s*$', 'CC = cl')
    $patchedText = [Regex]::Replace($patchedText, '(?m)^CXX\s*=.*cl\.exe\s*$', 'CXX = cl')
    $patchedText = [Regex]::Replace($patchedText, '(?m)^(LINKER|LINK|LINK_C)\s*=.*link\.exe\s*$', '$1 = link')
    $patchedText = $patchedText.Replace('$(CXXFLAGS) $(INCPATH)', '@merivus_cl_cxx_common.rsp')
    $patchedText = $patchedText.Replace('$(CFLAGS) $(INCPATH)', '@merivus_cl_c_common.rsp')
    $patchedText = [Regex]::Replace($patchedText, '"QMAKE_CC=[^"]*cl\.exe"', '"QMAKE_CC=cl"')
    $patchedText = [Regex]::Replace($patchedText, '"QMAKE_CXX=[^"]*cl\.exe"', '"QMAKE_CXX=cl"')
    $patchedText = [Regex]::Replace($patchedText, '"QMAKE_LINK=[^"]*link\.exe"', '"QMAKE_LINK=link"')
    $patchedText = [Regex]::Replace($patchedText, '"QMAKE_LINK_C=[^"]*link\.exe"', '"QMAKE_LINK_C=link"')

    if ($patchedText -eq $makefileText -and -not $alreadyPatched) {
        throw "MSVC response-file patch did not update the generated Makefile."
    }

    [IO.File]::WriteAllText($makefile, $patchedText, $ascii)
    Write-Host "Enabled MSVC compile response files in $BuildDir"
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$buildRoot = Join-Path $repoRoot "build"
if ([string]::IsNullOrWhiteSpace($BuildDirOverride)) {
    $buildDir = Join-Path $repoRoot "build\Desktop_Qt_5_15_2_MSVC2019_64bit-$Configuration"
} else {
    $buildDir = [IO.Path]::GetFullPath($BuildDirOverride)
}
$qmake = Join-Path $QtRoot "bin\qmake.exe"
$vcvars = Join-Path $VisualStudioRoot "VC\Auxiliary\Build\vcvars64.bat"
$project = Join-Path $repoRoot "qgroundcontrol.pro"

foreach ($path in @($qmake, $vcvars, $project, $buildRoot)) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required path not found: $path"
    }
}

$resolvedBuildRoot = (Resolve-Path -LiteralPath $buildRoot).Path
$buildRootPrefix = $resolvedBuildRoot.TrimEnd([IO.Path]::DirectorySeparatorChar, [IO.Path]::AltDirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
if ($buildDir -ne $resolvedBuildRoot -and -not $buildDir.StartsWith($buildRootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildDirOverride must stay under the repository build directory: $buildDir"
}

if ($Clean -and (Test-Path -LiteralPath $buildDir)) {
    $resolvedBuild = (Resolve-Path -LiteralPath $buildDir).Path
    if (-not $resolvedBuild.StartsWith($resolvedBuildRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to clean outside the repository build directory: $resolvedBuild"
    }
    Remove-Item -LiteralPath $resolvedBuild -Recurse -Force
}

New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

$compilerProbe = & $env:ComSpec /d /s /c "call `"$vcvars`" >nul && where cl.exe && where link.exe && where nmake.exe"
if ($LASTEXITCODE -ne 0 -or -not $compilerProbe) {
    throw "MSVC cl.exe/link.exe/nmake.exe were not found after vcvars64 initialization."
}
$cl = $compilerProbe | Where-Object { (Split-Path $_ -Leaf) -ieq "cl.exe" } | Select-Object -First 1
$link = $compilerProbe | Where-Object { (Split-Path $_ -Leaf) -ieq "link.exe" } | Select-Object -First 1
$nmake = $compilerProbe | Where-Object { (Split-Path $_ -Leaf) -ieq "nmake.exe" } | Select-Object -First 1
if (-not $cl -or -not $link -or -not $nmake) {
    throw "MSVC tool probe did not return cl.exe, link.exe and nmake.exe."
}

if ($Reconfigure -or -not (Test-Path -LiteralPath (Join-Path $buildDir "Makefile"))) {
    $configName = $Configuration.ToLowerInvariant()
    $qmakeBatchPath = Join-Path $buildDir "run-qmake.cmd"
    $qmakeBatchLines = @(
        "@echo off",
        "call `"$vcvars`"",
        "cd /d `"$buildDir`"",
        "`"$qmake`" `"$project`" CONFIG+=$configName QMAKE_CC=cl QMAKE_CXX=cl QMAKE_LINK=link QMAKE_LINK_C=link",
        "exit /b %ERRORLEVEL%"
    )
    Set-Content -LiteralPath $qmakeBatchPath -Value $qmakeBatchLines -Encoding ASCII

    Write-Host "Configuring $Configuration in $buildDir"
    & $env:ComSpec /d /s /c "`"$qmakeBatchPath`""
    if ($LASTEXITCODE -ne 0) {
        throw "QMake failed with exit code $LASTEXITCODE"
    }
}

Enable-MsvcCompileResponseFiles -BuildDir $buildDir

$buildBatchPath = Join-Path $buildDir "run-build.cmd"
$buildBatchLines = @(
    "@echo off",
    "call `"$vcvars`"",
    "cd /d `"$buildDir`"",
    "`"$nmake`"",
    "exit /b %ERRORLEVEL%"
)
Set-Content -LiteralPath $buildBatchPath -Value $buildBatchLines -Encoding ASCII

Write-Host "Building $Configuration in $buildDir"
& $env:ComSpec /d /s /c "`"$buildBatchPath`""
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}

$binary = Join-Path $buildDir "staging\MERIVUS.exe"
if (-not (Test-Path -LiteralPath $binary)) {
    throw "Build command succeeded but MERIVUS.exe was not found at: $binary"
}

Write-Host "Build succeeded: $binary"
