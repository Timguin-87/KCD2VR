param(
    [string]$DetourModKitRoot=$env:DETOURMODKIT_ROOT,
    [string]$BuildDirectory='',
    [string]$OpenXrSdkSource=$env:OPENXR_SDK_SOURCE,
    [string]$OptionalGoreSource='',
    [ValidateSet('Release','RelWithDebInfo')][string]$Configuration='RelWithDebInfo',
    [ValidateRange(1,32)][int]$Parallel=2,
    [switch]$Asi,
    [switch]$ConfigureOnly,
    [Parameter(ValueFromRemainingArguments=$true)][string[]]$CMakeArgs
)
$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$log=Join-Path $source 'build_release.log'
Set-Content -LiteralPath $log -Value ('KCD2VR local build '+(Get-Date -Format o))
function Note([string]$message){Write-Host $message;Add-Content -LiteralPath $log -Value $message}
function Native([string]$exe,[string[]]$arguments){
    Note ($exe+' '+($arguments -join ' '))
    $previous=$ErrorActionPreference
    try {
        # WinPS5 wraps native stderr as error records even for compiler warnings.
        # Preserve all output; only the actual exit code decides success.
        $ErrorActionPreference='Continue'
        & $exe @arguments 2>&1 | ForEach-Object { $line=[string]$_;Write-Host $line;Add-Content -LiteralPath $log -Value $line }
        $code=$LASTEXITCODE
    } finally { $ErrorActionPreference=$previous }
    if($code -ne 0){throw ('Command failed with exit '+$code+'. See '+$log)}
}
function Sha256([string]$path){
    $stream=[IO.File]::OpenRead($path);$hasher=[Security.Cryptography.SHA256]::Create()
    try {return ([BitConverter]::ToString($hasher.ComputeHash($stream))).Replace('-','')}
    finally {$stream.Dispose();$hasher.Dispose()}
}
function DmkValid([string]$path){return $path -and (Test-Path -LiteralPath (Join-Path $path 'lib/cmake/DetourModKit/DetourModKitConfig.cmake'))}
try {
    $identity=Get-Content -LiteralPath (Join-Path $source 'src/build_identity.h') -Raw
    if($identity -notmatch '#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"'){throw 'Build identity is missing'}
    $marker=$Matches[1]
    if(-not $BuildDirectory){
        $localBuild48=Join-Path $source $(if($Asi){'build-asi'}else{'build-prerelease'})
        $BuildDirectory=$localBuild48
        # MSBuild's OpenXR tracking paths add about190characters. A deep source path
        # can fail FTK1011 even with long-path-aware compiler tools. Source stays put.
        if($localBuild48.Length -gt 80){
            $hasher48=[Security.Cryptography.SHA256]::Create()
            try {$id48=([BitConverter]::ToString($hasher48.ComputeHash([Text.Encoding]::UTF8.GetBytes($source.ToLowerInvariant())))).Replace('-','').Substring(0,12)}finally{$hasher48.Dispose()}
            $BuildDirectory=Join-Path ([IO.Path]::GetPathRoot($source)) ('kcd2vr-build/'+$id48+$(if($Asi){'-asi'}else{'-dll'}))
            if(-not $OpenXrSdkSource){$existingXr48=Join-Path $localBuild48 '_deps/openxr_sdk_source-src';if(Test-Path -LiteralPath (Join-Path $existingXr48 'src/loader/CMakeLists.txt')){$OpenXrSdkSource=$existingXr48}}
            Note ('Deep source path: using short build directory '+$BuildDirectory+'; source and previous cache are retained')
        }
    }
    $BuildDirectory=[IO.Path]::GetFullPath($BuildDirectory)
    Note ('Source '+$source)
    Note ('Build '+$BuildDirectory+' (existing cache and INI are retained)')
    $vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $vs=''
    if(Test-Path -LiteralPath $vswhere){$vs=(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1)}
    if(-not $vs){throw 'Visual Studio C++ x64 build tools were not found. This build does not install or update Visual Studio.'}
    $cmake=Get-Command cmake.exe -ErrorAction SilentlyContinue
    if($cmake){$cmake=$cmake.Source}else{$cmake=Join-Path $vs 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'}
    if(-not (Test-Path -LiteralPath $cmake)){throw 'CMake was not found in PATH or Visual Studio'}
    # The source archive carries the exact reviewed adapter-capacity128 dependency.
    # Never silently substitute an ancestor/cache/default64 package after extraction.
    if(-not $DetourModKitRoot){$DetourModKitRoot=Join-Path $source 'deps/DetourModKit'}
    if(-not (DmkValid $DetourModKitRoot)){throw ('Reviewed bundled DetourModKit missing: '+$DetourModKitRoot+'. Extract the complete source archive.')}
    $python=$env:KCD2VR_PYTHON
    if(-not $python){
        $found=Get-Command python.exe -ErrorAction SilentlyContinue
        if($found -and $found.Source -notmatch 'WindowsApps'){$python=$found.Source}
    }
    if(-not $python){$candidate=Join-Path $env:USERPROFILE 'miniconda3/python.exe';if(Test-Path -LiteralPath $candidate){$python=$candidate}}
    if(-not $python -or -not (Test-Path -LiteralPath $python)){throw 'Python3 was not found; set KCD2VR_PYTHON to a real python.exe'}
    if(Test-Path -LiteralPath (Join-Path $DetourModKitRoot 'COMPRESSED_LIBRARY.json')){
        $dependencyOutput=Join-Path $BuildDirectory '_deps/DetourModKit'
        Native $python @((Join-Path $PSScriptRoot 'prepare_dmk_dependency.py'),'--package',$DetourModKitRoot,'--out',$dependencyOutput)
        $DetourModKitRoot=$dependencyOutput
    }
    $library=Join-Path $DetourModKitRoot 'lib/libDetourModKit.lib'
    if(-not (Test-Path -LiteralPath $library) -or
       (Sha256 $library) -ne 'C01DFD67D2A72BFB7FC5A8666F6BCD8DAD93D530DAFE5AFCF7FDBCF9F02F6E54'){
        throw 'DetourModKit library differs from the reviewed capacity128 dependency. No automatic dependency replacement was made.'
    }
    Note ('Verified capacity128 dependency '+$DetourModKitRoot)
    $companionRoot48=Join-Path $BuildDirectory 'patch2_companion'
    $companionArguments=@((Join-Path $PSScriptRoot 'package_release_helpers_r19.py'),'--source',$source,'--prepare-companion','--out',$companionRoot48)
    if($OptionalGoreSource){$companionArguments+=@('--optional-gore-source',[IO.Path]::GetFullPath($OptionalGoreSource))}
    Native $python $companionArguments
    $proxy=if($Asi){'OFF'}else{'ON'}
    $configure=@('-S',$source,'-B',$BuildDirectory,'-G','Visual Studio 17 2022','-A','x64',
        ('-DCMAKE_PREFIX_PATH='+$DetourModKitRoot),('-DPython3_EXECUTABLE='+$python))
    if($OpenXrSdkSource){
        if(-not (Test-Path -LiteralPath (Join-Path $OpenXrSdkSource 'src/loader/CMakeLists.txt'))){throw 'Invalid OpenXrSdkSource'}
        $configure+=('-DFETCHCONTENT_SOURCE_DIR_OPENXR_SDK_SOURCE='+$OpenXrSdkSource)
    }
    # Old -D arguments remain accepted. Release identity cannot be overridden by
    # an old shared SDK or proxy flag supplied by a historical wrapper.
    $configure+=@($CMakeArgs)
    $configure+=('-DDetourModKit_DIR='+(Join-Path $DetourModKitRoot 'lib/cmake/DetourModKit')) # explicit root overrides an old cached package
    $configure+=@('-DKCD2VR_SINGLE_BINARY=ON',('-DKCD2VR_PROXY_DINPUT8='+$proxy))
    $configure+=('-DKCD2VR_COMPANION_SOURCE_ROOT='+$companionRoot48)
    Native $cmake $configure
    if($ConfigureOnly){Note 'Configure-only check succeeded; no binary was built or installed.';exit 0}
    Native $cmake @('--build',$BuildDirectory,'--config',$Configuration,'--target','KCD2VR','--parallel',[string]$Parallel,'--','/p:VcpkgEnabled=false')
    $name=if($Asi){'KCD2VR.asi'}else{'dinput8.dll'}
    $binary=Join-Path $BuildDirectory ('release/'+$Configuration+'/'+$name)
    if(-not (Test-Path -LiteralPath $binary)){$binary=Join-Path $BuildDirectory ('release/'+$name)}
    if(-not (Test-Path -LiteralPath $binary)){throw ('Fresh output missing: '+$name)}
    $report=Join-Path $BuildDirectory 'single-binary-verification.json'
    $verify=@((Join-Path $PSScriptRoot 'verify_single_binary_b1.py'),'--binary',$binary,'--marker',$marker,'--report',$report)
    if($Asi){$verify+='--asi'}
    Native $python $verify
    $destination=Join-Path $source $(if($Asi){'dist/asi'}else{'dist/prerelease'})
    New-Item -ItemType Directory -Force -Path $destination | Out-Null
    Copy-Item -LiteralPath $binary -Destination (Join-Path $destination $name) -Force
    if(-not $Asi){
        Native $python @((Join-Path $PSScriptRoot 'package_release_helpers_r19.py'),'--source',$source,'--binary',$binary,'--out',$destination)
    }
    Note ('VERIFIED '+$marker)
    Note ('Output '+(Join-Path $destination $name))
    Note 'Menu art and OpenXR loader are linked inside. No separate ASI/BMP/OpenXR loader is packaged.'
    Note 'The matching companion PAK remains game content. Existing Windows/VC/OpenXR runtime prerequisites remain.'
    Note 'No game files, game INI, saves or calibration were installed or changed. This is a local test candidate.'
    exit 0
} catch {
    $message='BUILD FAILED: '+$_.Exception.Message
    [Console]::Error.WriteLine($message)
    Add-Content -LiteralPath $log -Value $message
    [Console]::Error.WriteLine('Full log: '+$log)
    exit 1
}
