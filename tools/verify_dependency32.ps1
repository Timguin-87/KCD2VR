param(
    [Parameter(Mandatory=$true)][string]$Archive,
    [Parameter(Mandatory=$true)][ValidateSet('SHA256','SHA512')][string]$Algorithm,
    [Parameter(Mandatory=$true)][string]$Expected
)
$ErrorActionPreference='Stop'
$expectedLength=if($Algorithm -eq 'SHA256'){64}else{128}
if($Expected.Length -ne $expectedLength -or $Expected -notmatch '^[0-9a-fA-F]+$'){throw 'Invalid pinned digest'}
$stream=[IO.File]::OpenRead([IO.Path]::GetFullPath($Archive))
$hasher=if($Algorithm -eq 'SHA256'){[Security.Cryptography.SHA256]::Create()}else{[Security.Cryptography.SHA512]::Create()}
try {$actual=[BitConverter]::ToString($hasher.ComputeHash($stream)).Replace('-','')}
finally {$hasher.Dispose();$stream.Dispose()}
if($actual -ne $Expected){throw ('Dependency fingerprint mismatch: '+[IO.Path]::GetFileName($Archive))}
Write-Output ('VERIFIED '+$Algorithm+' '+$actual.ToLowerInvariant())
