$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$text=Get-Content -Raw -LiteralPath (Join-Path $root 'tools/build_release.ps1')
$begin=$text.IndexOf('    if(-not $BuildDirectory){')
$end=$text.IndexOf('    $BuildDirectory=[IO.Path]::GetFullPath($BuildDirectory)',$begin)
if($begin -lt 0 -or $end -lt $begin){throw 'Actual build-path selector missing'}
$selector=[ScriptBlock]::Create($text.Substring($begin,$end-$begin))
function Note([string]$message){}
$OpenXrSdkSource='retained-explicit-sdk';$Asi=$false
$source='D:/short';$BuildDirectory='';. $selector
if($BuildDirectory -ne (Join-Path $source 'build-prerelease')){throw 'Short source changed'}
$source='D:/KCD2VR-review-builds/H41_STEP30_F9_INDEPENDENT_GRIP_20261024/delivery_8a8c547_CANONICAL_HOST_QA2/SOURCE_8a8c547/KCD2VR_SOURCE';$saved=$source;$BuildDirectory='';. $selector
$first=$BuildDirectory
if($BuildDirectory.Length -gt 60 -or $source -ne $saved){throw 'Deep-source selector failed'}
$BuildDirectory='';. $selector
if($BuildDirectory -ne $first){throw 'Cache identity unstable'}
$source+='other';$BuildDirectory='';. $selector
if($BuildDirectory -eq $first){throw 'Different sources share one cache'}
$BuildDirectory='explicit-build';. $selector
if($BuildDirectory -ne 'explicit-build' -or $OpenXrSdkSource -ne 'retained-explicit-sdk'){throw 'Explicit selections changed'}
$Asi=$true;$BuildDirectory='';. $selector
if($BuildDirectory -notmatch '-asi$'){throw 'ASI and proxy share one cache'}
'PASS6 actual build-path selection checks; no source/cache/INI mutation'
