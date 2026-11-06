param([string]$OutputDirectory='', [switch]$WithoutSanitizer)
$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if(-not $OutputDirectory){$OutputDirectory=Join-Path $source 'build-d71-checks'}
$OutputDirectory=[IO.Path]::GetFullPath($OutputDirectory)
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vs=& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1
if(-not $vs){throw 'Visual Studio C++ x64 tools required'}
$toolVersion=(Get-Content -LiteralPath (Join-Path $vs 'VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt') -Raw).Trim()
# ASAN runtime must be visible to child test processes, as it is under vcvars64.
$env:PATH=(Join-Path $vs ('VC/Tools/MSVC/'+$toolVersion+'/bin/Hostx64/x64'))+';'+$env:PATH
$san=if($WithoutSanitizer){'OFF'}else{'ON'}
& cmake -S (Join-Path $source 'tests') -B $OutputDirectory -G 'Visual Studio 17 2022' -A x64 -DKCD2VR_D67R1_FOCUSED_ONLY=ON ("-DKCD2VR_HOST_ASAN="+$san)
if($LASTEXITCODE){throw 'D71 test configure failed'}
& cmake --build $OutputDirectory --config RelWithDebInfo --parallel 2
if($LASTEXITCODE){throw 'D71 test compile failed'}
& ctest --test-dir $OutputDirectory -C RelWithDebInfo --timeout 30 --output-on-failure
if($LASTEXITCODE){throw 'D71 focused test failed'}
