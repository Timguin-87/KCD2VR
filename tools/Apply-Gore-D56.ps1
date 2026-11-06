param([string]$GameRoot)
$ErrorActionPreference='Stop'
try {
    if(!$GameRoot){
        $beside=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
        if(Test-Path -LiteralPath (Join-Path $beside 'Bin/Win64MasterMasterSteamPGO/WHGame.dll')){$GameRoot=$beside}
    }
    if(!$GameRoot){$GameRoot=Read-Host 'KingdomComeDeliverance2 game folder (the folder containing Mods and Bin)'}
    $GameRoot=[IO.Path]::GetFullPath($GameRoot.Trim('"'))
    $bin=Join-Path $GameRoot 'Bin/Win64MasterMasterSteamPGO'
    if(!(Test-Path -LiteralPath (Join-Path $bin 'WHGame.dll'))){throw 'Selected folder is not the expected KCD2 game root'}
    if(Get-Process -Name 'KingdomCome','KingdomComeDeliverance2' -ErrorAction SilentlyContinue){throw 'Close KCD2 before updating its PAK'}
    . (Join-Path $PSScriptRoot 'gore_update_d56.ps1')
    $patch=Join-Path $PSScriptRoot 'gore_patch'
    $manifest=Get-Content -LiteralPath (Join-Path $patch 'GORE_D56_PATCH.json') -Raw | ConvertFrom-Json
    foreach($asset in $manifest.retained_assets) {
        $path=Join-Path $GameRoot $asset.path
        if(!(Test-Path -LiteralPath $path) -or (Get-D56FileHash $path) -ne $asset.sha256){throw "Required retained D52/D54 gore asset missing or changed: $($asset.path)"}
    }
    $main=Join-Path $GameRoot 'Mods/kcd2vr_gore/Data/kcd2vr_gore.pak'
    $result=Invoke-D56GoreUpdate -InputPak $main -PatchRoot $patch -AllowedMainHashes @(
      '2dbcbe96dbc09a9df9124cde2720656e07c68d105f1a466c9ac0cf1a51a020cb',
      'dd0451c3abc87920cefc251a17c4a55d46ab5c2738f6020ef9d0e070cf48aab4')
    $result | ConvertTo-Json -Depth 8
    Write-Host 'D56 gore script and material wrappers verified. The original PAK backup is beside the updated file.'
} catch {Write-Error $_;exit 1}
