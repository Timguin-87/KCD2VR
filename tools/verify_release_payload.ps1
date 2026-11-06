param([string]$Binary='')
$ErrorActionPreference='Stop'
function PayloadHash([string]$Path){
    $stream=[IO.File]::OpenRead($Path)
    $sha=[Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','') }
    finally { $stream.Dispose();$sha.Dispose() }
}
try {
    $root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
    $manifestPath=Join-Path $root 'BUILD_MANIFEST.json'
    $expected=''
    if(Test-Path -LiteralPath $manifestPath){
        $manifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
        $expected=[string]$manifest.build_marker
        foreach($artifact in $manifest.artifacts){
            $path=[IO.Path]::GetFullPath((Join-Path $root ([string]$artifact.path)))
            if(-not $path.StartsWith($root+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Invalid payload path'}
            if(-not (Test-Path -LiteralPath $path)){throw ('Missing matching build file: '+$artifact.path)}
            if((PayloadHash $path) -ne [string]$artifact.sha256){throw ('Build file mismatch: '+$artifact.path)}
        }
        if(-not $Binary){if($manifest.binary){$Binary=Join-Path $root ([string]$manifest.binary)}else{$Binary=Join-Path $root 'dinput8.dll'}}
    }
    $identity=Join-Path $root 'src/build_identity.h'
    if(Test-Path -LiteralPath $identity){
        $text=Get-Content -LiteralPath $identity -Raw
        if($text -notmatch '#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"'){throw 'Source build marker missing'}
        if($expected -and $expected -ne $Matches[1]){throw 'Source and release manifest are different builds'}
        $expected=$Matches[1]
        if(-not $Binary){$Binary=Join-Path $root 'dist/prerelease/dinput8.dll'}
    }
    if(-not $expected -or -not $Binary){throw 'Extract a complete matching release or source package; build identity is missing'}
    if(-not (Test-Path -LiteralPath $Binary)){throw ('DLL missing: '+$Binary+'. Build the source first, or extract the complete release package.')}
    $bytes=[IO.File]::ReadAllBytes([IO.Path]::GetFullPath($Binary))
    if($bytes.Length -lt 256 -or $bytes[0] -ne 0x4D -or $bytes[1] -ne 0x5A){throw 'Not a Windows DLL'}
    $pe=[BitConverter]::ToInt32($bytes,0x3C)
    if($pe -lt 64 -or $pe -gt $bytes.Length-24 -or [BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550 -or [BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x8664){throw 'DLL is not an x64 PE image'}
    if(-not [Text.Encoding]::ASCII.GetString($bytes).Contains($expected)){throw 'DLL belongs to another build'}
    Write-Host ('PASS '+$expected)
    Write-Host ('DLL SHA256 '+(PayloadHash $Binary))
    Write-Host 'Matching files verified. No game files or settings were changed; headset behavior is not tested.'
    exit 0
} catch {
    Write-Host ('Verification failed: '+$_.Exception.Message)
    exit 1
}
