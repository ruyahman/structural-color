param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$Test
)

$ErrorActionPreference = 'Stop'
$diffRoot = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (!(Test-Path -LiteralPath $vswhere)) {
    throw 'Install Visual Studio 2022 (or Build Tools) with Desktop development with C++.'
}
$msbuild = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'MSBuild\**\Bin\MSBuild.exe' |
    Select-Object -First 1
if (!$msbuild) {
    throw 'MSBuild with the MSVC C++ tools was not found.'
}
& $msbuild (Join-Path $diffRoot 'Diff\Diff.vcxproj') /t:Build "/p:Configuration=$Configuration" /p:Platform=Win32 /nologo /verbosity:minimal
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
if ($Test) {
    & $msbuild (Join-Path $diffRoot 'tests\DiffTests.vcxproj') /t:Build "/p:Configuration=$Configuration" /p:Platform=Win32 /nologo /verbosity:minimal
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    Push-Location (Join-Path $diffRoot 'Diff')
    try {
        & (Join-Path $diffRoot "build\tests\$Configuration\DiffTests.exe")
        $testExitCode = $LASTEXITCODE
    } finally { Pop-Location }
    exit $testExitCode
}
exit 0
