# Local archive updater. No network, process injection, execution-policy changes,
# game launch, asset guessing, or writes to the nine retained head/limb PAKs.
Set-StrictMode -Version Latest
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

function Get-D56StreamHash($Stream) {
    $sha=[Security.Cryptography.SHA256]::Create()
    try { return ([BitConverter]::ToString($sha.ComputeHash($Stream))).Replace('-','').ToLowerInvariant() }
    finally { $sha.Dispose() }
}
function Get-D56FileHash([string]$Path) {
    $stream=[IO.File]::OpenRead($Path)
    try { return Get-D56StreamHash $stream } finally { $stream.Dispose() }
}
function Get-D56EntryHash($Entry) {
    $stream=$Entry.Open()
    try { return Get-D56StreamHash $stream } finally { $stream.Dispose() }
}
function Get-D56Entries($Zip) {
    $result=@{}
    foreach($entry in $Zip.Entries) {
        $name=$entry.FullName.Replace('\','/').ToLowerInvariant()
        if(!$name -or $name.StartsWith('/') -or $name.Contains(':') -or
           ($name.Split('/') -contains '..') -or $result.ContainsKey($name)) {
            throw "Unsafe or duplicate PAK member: $name"
        }
        $result.Add($name,$entry)
    }
    return $result
}
function Copy-D56Entry($Target,[string]$Name,$SourceStream) {
    $entry=$Target.CreateEntry($Name,[IO.Compression.CompressionLevel]::Optimal)
    $entry.LastWriteTime=[DateTimeOffset]::new(2026,10,29,0,0,0,[TimeSpan]::Zero)
    $stream=$entry.Open()
    try {$SourceStream.CopyTo($stream)} finally {$stream.Dispose()}
}
function Publish-D56Receipt([string]$Path,$Receipt) {
    $temp=$Path+'.D56-'+[Guid]::NewGuid().ToString('N')+'.tmp'
    try {
        [IO.File]::WriteAllText($temp,($Receipt | ConvertTo-Json -Depth 8),[Text.UTF8Encoding]::new($false))
        if(Test-Path -LiteralPath $Path){[IO.File]::Replace($temp,$Path,[NullString]::Value)}
        else {[IO.File]::Move($temp,$Path)}
    } finally {if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp -Force}}
}
function Invoke-D56GoreUpdate {
    param([Parameter(Mandatory=$true)][string]$InputPak,
          [Parameter(Mandatory=$true)][string]$PatchRoot,
          [Parameter(Mandatory=$true)][string[]]$AllowedMainHashes)
    $InputPak=[IO.Path]::GetFullPath($InputPak)
    $PatchRoot=[IO.Path]::GetFullPath($PatchRoot)
    $manifest=Get-Content -LiteralPath (Join-Path $PatchRoot 'GORE_D56_PATCH.json') -Raw | ConvertFrom-Json
    $scriptPath=Join-Path $PatchRoot 'zzkcd2vr_gore.lua'
    $overlayPath=Join-Path $PatchRoot 'material_wrappers.pak'
    if((Get-D56FileHash $scriptPath) -ne $manifest.script_sha256 -or
       (Get-D56FileHash $overlayPath) -ne $manifest.overlay_sha256) {throw 'D56 patch payload identity mismatch'}
    $before=Get-D56FileHash $InputPak
    $receiptPath=$InputPak+'.D56.json'
    if(Test-Path -LiteralPath $receiptPath) {
        $receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
        if($receipt.after_sha256 -eq $before -and $receipt.script_sha256 -eq $manifest.script_sha256 -and
           $receipt.overlay_sha256 -eq $manifest.overlay_sha256) {
            return [pscustomobject]@{status='ALREADY_CURRENT';path=$InputPak;sha256=$before}
        }
    }
    if($AllowedMainHashes -notcontains $before) {throw 'Installed main gore PAK is not the verified D52 or D54 archive; nothing changed'}
    # Never overwrite an earlier rollback file with a different archive.
    $backup=$InputPak+'.preD56.'+$before.Substring(0,12)+'.bak'
    if((Test-Path -LiteralPath $backup) -and (Get-D56FileHash $backup) -ne $before) {throw 'Rollback file identity mismatch; nothing changed'}
    $temp=$InputPak+'.D56-'+[Guid]::NewGuid().ToString('N')+'.tmp'
    $old=$null;$overlay=$null;$target=$null;$verify=$null
    try {
        $old=[IO.Compression.ZipFile]::OpenRead($InputPak)
        $overlay=[IO.Compression.ZipFile]::OpenRead($overlayPath)
        $previous=Get-D56Entries $old;$added=Get-D56Entries $overlay
        $scriptKey='scripts/startup/zzkcd2vr_gore.lua'
        if(!$previous.ContainsKey($scriptKey)){throw 'Canonical gore startup script missing'}
        $expected=@{};$expectedNames=@{}
        foreach($row in $manifest.members) {
            $key=$row.path.Replace('\','/').ToLowerInvariant()
            if($expected.ContainsKey($key)){throw 'Duplicate patch manifest member'}
            $expected[$key]=$row.sha256
        }
        if($expected.Count -ne $added.Count){throw 'Overlay member count mismatch'}
        foreach($key in $added.Keys) {
            if(!$expected.ContainsKey($key) -or (Get-D56EntryHash $added[$key]) -ne $expected[$key]) {throw "Overlay content mismatch: $key"}
            if($previous.ContainsKey($key) -and (Get-D56EntryHash $previous[$key]) -ne $expected[$key]) {throw "Unexpected overlay collision: $key"}
        }
        $target=[IO.Compression.ZipFile]::Open($temp,[IO.Compression.ZipArchiveMode]::Create)
        foreach($key in @($previous.Keys | Sort-Object)) {
            $member=$previous[$key];$expectedNames[$key]=$member.FullName
            if($key -eq $scriptKey) {
                $stream=[IO.File]::OpenRead($scriptPath);$expected[$key]=$manifest.script_sha256
            } else {
                $expected[$key]=Get-D56EntryHash $member;$stream=$member.Open()
            }
            try {Copy-D56Entry $target $member.FullName $stream} finally {$stream.Dispose()}
        }
        foreach($key in @($added.Keys | Sort-Object)) {
            if($previous.ContainsKey($key)){continue}
            $member=$added[$key];$expectedNames[$key]=$member.FullName;$stream=$member.Open()
            try {Copy-D56Entry $target $member.FullName $stream} finally {$stream.Dispose()}
        }
        $target.Dispose();$target=$null
        $verify=[IO.Compression.ZipFile]::OpenRead($temp);$actual=Get-D56Entries $verify
        if($actual.Count -ne $expected.Count){throw 'Output member count mismatch'}
        foreach($key in $expected.Keys) {
            if(!$actual.ContainsKey($key) -or (Get-D56EntryHash $actual[$key]) -ne $expected[$key]) {throw "Output content verification failed: $key"}
        }
        $verify.Dispose();$verify=$null;$old.Dispose();$old=$null;$overlay.Dispose();$overlay=$null
        $after=Get-D56FileHash $temp
        $receipt=[ordered]@{status='UPDATED';path=$InputPak;before_sha256=$before;after_sha256=$after;
            script_sha256=$manifest.script_sha256;overlay_sha256=$manifest.overlay_sha256;
            members_verified=$expected.Count;backup=$backup;headset_tested=$false}
        # Publish recovery data before replacing the asset. If receipt IO fails,
        # the original PAK is untouched. If replacement then fails/interruption
        # occurs, an ahead-of-PAK receipt is harmless: idempotence requires that
        # the actual current PAK hash equal after_sha256, otherwise normal exact
        # D52/D54 admission permits a retry.
        Publish-D56Receipt $receiptPath $receipt
        # All archive handles are closed. A concurrent writer revokes the swap.
        if((Get-D56FileHash $InputPak) -ne $before){throw 'Input archive changed during verification; nothing replaced'}
        if(Test-Path -LiteralPath $backup){[IO.File]::Replace($temp,$InputPak,[NullString]::Value)}
        else {[IO.File]::Replace($temp,$InputPak,$backup)}
        return [pscustomobject]$receipt
    } finally {
        foreach($resource in @($verify,$target,$overlay,$old)){if($null -ne $resource){$resource.Dispose()}}
        if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp -Force}
    }
}
