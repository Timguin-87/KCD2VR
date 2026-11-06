[CmdletBinding(SupportsShouldProcess = $true)]
param([Parameter(Mandatory = $true)][string]$GameRoot)

$ErrorActionPreference = 'Stop'
$gamePath = (Resolve-Path -LiteralPath $GameRoot).Path.TrimEnd('\', '/')
$prefix = $gamePath + [IO.Path]::DirectorySeparatorChar
$oldMapPath = [IO.Path]::GetFullPath((Join-Path $gamePath 'Mods/KCD2VRMap/Data/KCD2VR_Map.pak'))
if (-not $oldMapPath.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Map target escapes the specified game directory.'
}
if (-not (Test-Path -LiteralPath $oldMapPath -PathType Leaf)) {
    Write-Output 'Known obsolete map PAK is absent. No files changed.'
    return
}
$expectedHash = '92ca21322d3f0359bcfcd9ea1d8e1dfb912cc4956d8a21b79d1ce261bae62bff'
$actualHash = (Get-FileHash -LiteralPath $oldMapPath -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualHash -ne $expectedHash) {
    throw "Different map PAK: $actualHash. File preserved; this helper only retires the known blank-map version."
}
$backupName = 'map_t1_' + (Get-Date -Format 'yyyyMMdd_HHmmss') + '_' + [Guid]::NewGuid().ToString('N')
$backupDirectory = [IO.Path]::GetFullPath((Join-Path $gamePath ('KCD2VR_Backups/' + $backupName)))
if (-not $backupDirectory.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Backup target escapes the specified game directory.'
}
if ($PSCmdlet.ShouldProcess($oldMapPath, 'Move the verified obsolete map PAK to an unmounted backup')) {
    New-Item -ItemType Directory -Path $backupDirectory | Out-Null
    $backupPath = Join-Path $backupDirectory 'KCD2VR_Map.pak'
    Move-Item -LiteralPath $oldMapPath -Destination $backupPath
    if ((Get-FileHash -LiteralPath $backupPath -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash) {
        throw "Backup verification failed. Inspect $backupPath; no other file was changed."
    }
    Write-Output ('Obsolete blank-map PAK backed up to: ' + $backupPath)
    Write-Output ('To undo, move that exact file back to: ' + $oldMapPath)
}
