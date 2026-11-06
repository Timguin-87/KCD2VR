param([Parameter(Mandatory=$true)][string]$PackageRoot)
$ErrorActionPreference='Stop'
# Execute ONLY the actual immutable validator function. Never installer main,
# GameRoot handling, process checks, move/copy, backup, rollback or installation.
$taskInstaller=Join-Path $PackageRoot 'install_unified_companion63.ps1'
$taskTokens=$null;$taskParseErrors=$null
$taskAst=[Management.Automation.Language.Parser]::ParseFile($taskInstaller,[ref]$taskTokens,[ref]$taskParseErrors)
if ($taskParseErrors.Count) { throw 'Installer parsing failed.' }
$taskFunctions=@($taskAst.FindAll({param($node) $node -is [Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq 'Assert-Distribution63'},$true))
if ($taskFunctions.Count -ne 1) { throw 'Expected one actual distribution validator.' }
. ([ScriptBlock]::Create($taskFunctions[0].Extent.Text))
$taskFolder=Join-Path $PackageRoot 'Mods/KCD2VR'
$taskMarker=Get-Content -LiteralPath (Join-Path $taskFolder 'KCD2VR_UNIFIED63.json') -Raw | ConvertFrom-Json
Assert-Distribution63 $taskFolder $taskMarker
Write-Output 'PASS actual Assert-Distribution63 leaf; installer main NOT EXECUTED'
