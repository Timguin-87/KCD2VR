[CmdletBinding(SupportsShouldProcess=$true)]
param([Parameter(Mandatory=$true)][string]$GameRoot)
$ErrorActionPreference='Stop'
function Assert-PlainPath63([string]$taskChecked) {
 $taskWalk=[IO.Path]::GetFullPath($taskChecked)
 while ($taskWalk) {
  if ((Test-Path -LiteralPath $taskWalk) -and ((Get-Item -LiteralPath $taskWalk -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw ('Refusing linked path ancestor: '+$taskWalk) }
  $taskParent=[IO.Path]::GetDirectoryName($taskWalk)
  if ($taskParent -eq $taskWalk) { break };$taskWalk=$taskParent
 }
}
function Assert-Distribution63([string]$taskFolder,[object]$taskExpected) {
 $taskAllowed=@{'Data/KCD2VR.pak'=$taskExpected.pak_sha256;'KCD2VR_UNIFIED63.json'=$null}
 if ($taskExpected.version -ne 63 -or !$taskExpected.distribution_files) { throw 'Missing C63 distribution inventory.' }
 foreach ($taskProperty in $taskExpected.distribution_files.PSObject.Properties) {
  if ($taskProperty.Name.StartsWith('/') -or $taskProperty.Name.Split('/') -contains '..' -or $taskProperty.Name.Contains('\')) { throw 'Unsafe distribution inventory path.' }
  $taskAllowed[$taskProperty.Name]=$taskProperty.Value
 }
 $taskDirs=@([IO.Path]::GetFullPath($taskFolder));$taskFound=@{}
 while ($taskDirs.Count) {
  $taskDir=$taskDirs[0];$taskDirs=@($taskDirs | Select-Object -Skip 1)
  foreach ($taskEntry in (Get-ChildItem -LiteralPath $taskDir -Force)) {
   if ($taskEntry.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw ('Linked distribution member: '+$taskEntry.FullName) }
   if ($taskEntry.PSIsContainer) { $taskDirs+=$taskEntry.FullName;continue }
   $taskRelative=$taskEntry.FullName.Substring($taskFolder.TrimEnd('\').Length+1).Replace('\','/')
   if (!$taskAllowed.ContainsKey($taskRelative) -or $taskFound.ContainsKey($taskRelative)) { throw ('Unexpected/duplicate distribution member: '+$taskRelative) }
   $taskFound[$taskRelative]=$true
   if ($taskAllowed[$taskRelative] -and (Get-FileHash -LiteralPath $taskEntry.FullName -Algorithm SHA256).Hash.ToLowerInvariant() -ne $taskAllowed[$taskRelative]) { throw ('Distribution member hash mismatch: '+$taskRelative) }
  }
 }
 if ($taskFound.Count -ne $taskAllowed.Count) { throw 'Incomplete distribution inventory.' }
}
$taskRoot=(Resolve-Path -LiteralPath $GameRoot).Path.TrimEnd('\')
$taskSource=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'Mods\KCD2VR'))
Assert-PlainPath63 $taskRoot
Assert-PlainPath63 $taskSource
Assert-PlainPath63 (Join-Path $taskSource 'Data')
Assert-PlainPath63 (Join-Path $taskSource 'Localization')
$taskMarker=Join-Path $taskSource 'KCD2VR_UNIFIED63.json' 
if (!(Test-Path -LiteralPath $taskMarker -PathType Leaf)) { throw 'Run the installer from the extracted C63 companion delivery, beside Mods.' }
$taskExpected=Get-Content -LiteralPath $taskMarker -Raw | ConvertFrom-Json
Assert-Distribution63 $taskSource $taskExpected
$taskPak=Join-Path $taskSource 'Data\KCD2VR.pak'
if ((Get-FileHash -LiteralPath $taskPak -Algorithm SHA256).Hash.ToLowerInvariant() -ne $taskExpected.pak_sha256) { throw 'Companion PAK hash mismatch.' }
$taskDll=Join-Path $taskRoot 'Bin\Win64MasterMasterSteamPGO\WHGame.dll'
if (!(Test-Path -LiteralPath $taskDll -PathType Leaf)) { throw 'This is not the supported KCD2 game folder.' }
if ((Get-FileHash -LiteralPath $taskDll -Algorithm SHA256).Hash.ToLowerInvariant() -ne 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7') { throw 'Unsupported WHGame binary.' }
if (!$WhatIfPreference -and (Get-Process -Name KingdomCome -ErrorAction SilentlyContinue)) { throw 'Close the game before replacing mounted companion archives.' }
$taskMods=[IO.Path]::GetFullPath((Join-Path $taskRoot 'Mods'))
$taskBackup=[IO.Path]::GetFullPath((Join-Path $taskRoot ('KCD2VR_Backups\companion63_'+[DateTime]::Now.ToString('yyyyMMdd_HHmmss')+'_'+[Guid]::NewGuid().ToString('N'))))
Assert-PlainPath63 $taskMods
Assert-PlainPath63 $taskBackup
$taskNames=@('KCD2VR','KCD2VRDoors','KCD2VRMap','kcd2vr_lab','KCD2VRThrowingAssets')
foreach ($taskName in $taskNames) {
 $taskOld=[IO.Path]::GetFullPath((Join-Path $taskMods $taskName))
 $taskSaved=[IO.Path]::GetFullPath((Join-Path $taskBackup $taskName))
 if (!$taskOld.StartsWith($taskMods+'\',[StringComparison]::OrdinalIgnoreCase) -or !$taskSaved.StartsWith($taskBackup+'\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Unsafe companion path.' }
 if ((Test-Path -LiteralPath $taskOld) -and ((Get-Item -LiteralPath $taskOld).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw 'Refusing a linked companion directory; review it manually.' }
}
if (!$PSCmdlet.ShouldProcess($taskRoot,'Back up exactly KCD2VR/KCD2VRDoors/KCD2VRMap/kcd2vr_lab/KCD2VRThrowingAssets and install the unified C63 companion; ASI, INI, gore and other mods stay unchanged')) { return }
New-Item -ItemType Directory -Path $taskBackup -Force | Out-Null
New-Item -ItemType Directory -Path $taskMods -Force | Out-Null
$taskMoved=@();$taskNewStarted=$false
try {
 foreach ($taskName in $taskNames) {
  $taskOld=Join-Path $taskMods $taskName
  if (Test-Path -LiteralPath $taskOld) { Move-Item -LiteralPath $taskOld -Destination (Join-Path $taskBackup $taskName);$taskMoved+=$taskName }
 }
 $taskNewStarted=$true
 Copy-Item -LiteralPath $taskSource -Destination (Join-Path $taskMods 'KCD2VR') -Recurse
 $taskInstalled=Join-Path $taskMods 'KCD2VR\Data\KCD2VR.pak'
 if ((Get-FileHash -LiteralPath $taskInstalled -Algorithm SHA256).Hash.ToLowerInvariant() -ne $taskExpected.pak_sha256) { throw 'Installed PAK copy hash mismatch.' }
 Assert-Distribution63 (Join-Path $taskMods 'KCD2VR') $taskExpected
 Write-Output ('Companion installed. Recoverable backup: '+$taskBackup)
 Write-Output 'Install the matching reviewed ASI separately. Keep your game-side INI/calibration. Gore remains optional.'
} catch {
 $taskFailure=$_;$taskRollbackErrors=@()
 # Move any partial new directory aside, then restore the exact old folders.
 $taskPartial=Join-Path $taskMods 'KCD2VR'
 if ($taskNewStarted -and (Test-Path -LiteralPath $taskPartial)) { try { Move-Item -LiteralPath $taskPartial -Destination (Join-Path $taskBackup 'FAILED_NEW_KCD2VR') } catch { $taskRollbackErrors+=$_.Exception.Message } }
 foreach ($taskName in $taskMoved) { try { if (Test-Path -LiteralPath (Join-Path $taskMods $taskName)) { throw ('Restore destination occupied; original retained in backup: '+$taskName) };Move-Item -LiteralPath (Join-Path $taskBackup $taskName) -Destination (Join-Path $taskMods $taskName) } catch { $taskRollbackErrors+=$_.Exception.Message } }
 if ($taskRollbackErrors.Count) { Write-Warning ('Automatic rollback incomplete; originals remain recoverable in '+$taskBackup+': '+($taskRollbackErrors -join '; ')) }
 throw $taskFailure
}
