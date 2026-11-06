param([Parameter(Mandatory=$true)][string]$OutputRoot,[string]$Tag='')
$ErrorActionPreference='Stop'
$lock=Get-Content -LiteralPath (Join-Path $PSScriptRoot '../data/build_dependencies32.json') -Raw | ConvertFrom-Json
$pin=$lock.DetourModKit
if($Tag -and $Tag -ne $pin.tag){throw 'Unpinned DetourModKit tag: use the pinned release or set DETOURMODKIT_ROOT to your verified compatible package'}
if(Test-Path -LiteralPath $OutputRoot){throw 'Dependency output exists: refusing to overwrite it'}
$tmp=Join-Path $env:TEMP ('kcd2vr_dmk_'+[guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $tmp | Out-Null
$archive=Join-Path $tmp $pin.asset
Invoke-WebRequest -Uri $pin.url -OutFile $archive -Headers @{'User-Agent'='KCD2VR-pinned-build'}
& (Join-Path $PSScriptRoot 'verify_dependency32.ps1') -Archive $archive -Algorithm SHA256 -Expected $pin.sha256
$expanded=Join-Path $tmp 'expanded'
Expand-Archive -LiteralPath $archive -DestinationPath $expanded
$cfg=@(Get-ChildItem -LiteralPath $expanded -Recurse -Directory -Filter DetourModKit | Where-Object {$_.Parent.Name -eq 'cmake'})
if($cfg.Count -ne 1){throw 'Expected one exact lib/cmake/DetourModKit package'}
$package=$cfg[0].Parent.Parent.Parent.FullName
if(-not (Test-Path -LiteralPath (Join-Path $cfg[0].FullName 'DetourModKitConfig.cmake'))){throw 'Package config absent'}
$destination=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Force -Path ([IO.Path]::GetDirectoryName($destination)) | Out-Null
Copy-Item -LiteralPath $package -Destination $destination -Recurse
@{tag=$pin.tag;url=$pin.url;sha256=$pin.sha256;verified=$true} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $destination 'KCD2VR_SOURCE.json')
Write-Output ('Pinned DetourModKit ready: '+$destination)
