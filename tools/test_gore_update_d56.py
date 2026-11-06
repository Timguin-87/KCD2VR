"""Execute the shipped PowerShell archive updater on adversarial fixture PAKs.

The unavailable installed D52 asset archive is not represented as tested.
Fixtures inject their own approved source hashes at the core function boundary;
the release entry point retains fixed real D52/D54 hashes.
"""
from pathlib import Path
import argparse, hashlib, json, os, subprocess, zipfile

ROOT=Path(__file__).resolve().parents[1]
def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def zipwrite(path,rows):
    with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED) as z:
        for name,data in rows:z.writestr(name,data)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--pwsh',required=True,type=Path);ap.add_argument('--out',required=True,type=Path)
    a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    env=os.environ.copy()
    for key in ('XDG_CACHE_HOME','XDG_CONFIG_HOME','XDG_DATA_HOME'):
        folder=out/key.lower();folder.mkdir(exist_ok=True);env[key]=str(folder)
    runner=out/'run.ps1';runner.write_text('''param([string]$Core,[string]$Pak,[string]$Patch,[string]$Allowed,[switch]$ReceiptFault)
$ErrorActionPreference='Stop'
try {
 . $Core
 if($ReceiptFault){function Publish-D56Receipt([string]$Path,$Receipt){throw 'INJECTED receipt IO fault'}}
 Invoke-D56GoreUpdate -InputPak $Pak -PatchRoot $Patch -AllowedMainHashes @($Allowed) | ConvertTo-Json -Depth 8
}
catch { Write-Output ('REFUSED: '+$_.Exception.Message);exit 13 }
''')
    rows=[]
    overlay=[('objects/hair_d54.cdf',b'<CharacterDefinition fixture="1"/>'),('objects/helm_d54.cdf',b'<CharacterDefinition fixture="2"/>')]
    base=[('Scripts/Startup/zzkcd2vr_gore.lua',b'-- old script'),('objects/head.skin',bytes(range(256))*1000),('objects/original_material.mtl',b'<Material/>')]
    def fixture(name,source=None):
        folder=out/name;folder.mkdir(exist_ok=True);patch=folder/'patch';patch.mkdir(exist_ok=True)
        pak=folder/'kcd2vr_gore.pak';zipwrite(pak,base if source is None else source)
        (patch/'zzkcd2vr_gore.lua').write_bytes(b'-- new D56 script\nfunction kcd2vr_gore_eligible_d56() return "male" end\n')
        zipwrite(patch/'material_wrappers.pak',overlay)
        manifest={'script_sha256':sha(patch/'zzkcd2vr_gore.lua'),'overlay_sha256':sha(patch/'material_wrappers.pak'),
            'members':[{'path':n,'sha256':hashlib.sha256(b).hexdigest()} for n,b in overlay]}
        (patch/'GORE_D56_PATCH.json').write_text(json.dumps(manifest));return pak,patch,sha(pak)
    def run(name,pak,patch,allowed,ok=True,receipt_fault=False):
        before=sha(pak)
        proc=subprocess.run([str(a.pwsh),'-NoLogo','-NoProfile','-NonInteractive','-File',str(runner),
            '-Core',str(ROOT/'tools/gore_update_d56.ps1'),'-Pak',str(pak),'-Patch',str(patch),'-Allowed',allowed]+(['-ReceiptFault'] if receipt_fault else []),capture_output=True,text=True,env=env)
        if ok:
            assert proc.returncode==0,(name,proc.stdout,proc.stderr)
            result=json.loads(proc.stdout);assert result['status'] in ('UPDATED','ALREADY_CURRENT')
        else:
            assert proc.returncode==13 and sha(pak)==before,(name,proc.stdout,proc.stderr)
            assert not list(pak.parent.glob('*.tmp')),name
            result={'status':'REFUSED_WITH_ORIGINAL_UNCHANGED','reason':proc.stdout.strip()}
        rows.append({'case':name,'result':result});return result
    pak,patch,h=fixture('d52');r=run('D52-style add, script replace, streamed assets',pak,patch,h)
    assert sha(r['backup'])==h
    with zipfile.ZipFile(pak) as z:
        assert z.testzip() is None and z.read(base[1][0])==base[1][1] and z.read(base[2][0])==base[2][1]
        assert z.read(base[0][0])==(patch/'zzkcd2vr_gore.lua').read_bytes()
        for n,b in overlay:assert z.read(n)==b
    current=sha(pak);run('Repeat is no-op and retains rollback',pak,patch,h);assert sha(pak)==current and sha(r['backup'])==h
    pak,patch,h=fixture('d54',base+overlay);run('D54-style existing identical wrappers without duplicates',pak,patch,h)
    pak,patch,h=fixture('unknown');run('Unknown installed archive rejected',pak,patch,'0'*64,False)
    pak,patch,h=fixture('tampered');(patch/'zzkcd2vr_gore.lua').write_text('tamper');run('Tampered payload rejected',pak,patch,h,False)
    pak,patch,h=fixture('duplicate',base+[('scripts/startup/ZZKCD2VR_GORE.lua',b'other')]);run('Case-duplicate base rejected',pak,patch,h,False)
    pak,patch,h=fixture('collision',base+[(overlay[0][0],b'foreign')]);run('Changed material collision rejected',pak,patch,h,False)
    pak,patch,h=fixture('missing',base[1:]);run('Missing canonical startup rejected',pak,patch,h,False)
    pak,patch,h=fixture('unsafe',base+[('../foreign',b'outside')]);run('Unsafe member rejected',pak,patch,h,False)
    pak,patch,h=fixture('rollback');Path(str(pak)+'.preD56.'+h[:12]+'.bak').write_bytes(b'unrelated')
    run('Unrelated existing rollback preserved',pak,patch,h,False)
    pak,patch,h=fixture('receipt-io');run('Receipt IO fault leaves approved original unchanged',pak,patch,h,False,True)
    run('Retry succeeds after receipt IO is restored',pak,patch,h)
    # An ahead-of-PAK receipt (interruption before atomic replacement) must not
    # make the approved original look current or block a clean retry.
    pak,patch,h=fixture('ahead-receipt')
    manifest=json.loads((patch/'GORE_D56_PATCH.json').read_text())
    Path(str(pak)+'.D56.json').write_text(json.dumps({'after_sha256':'f'*64,
        'script_sha256':manifest['script_sha256'],'overlay_sha256':manifest['overlay_sha256']}))
    run('Ahead-of-PAK receipt permits exact-base retry',pak,patch,h)
    pak,patch,h=fixture('existing-exact-backup')
    rollback=Path(str(pak)+'.preD56.'+h[:12]+'.bak');rollback.write_bytes(pak.read_bytes())
    run('Existing exact rollback retained during replacement',pak,patch,h);assert sha(rollback)==h
    result={'passed':True,'runtime':'PowerShell '+subprocess.check_output([str(a.pwsh),'-NoProfile','-Command','$PSVersionTable.PSVersion.ToString()'],text=True,env=env).strip(),
        'cases':rows,'actual_installed_D52_archive_executed':False,'windows_NTFS_executed':False,'native_game_executed':False}
    (out/'RESULTS.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'passed':True,'cases':len(rows),'runtime':result['runtime']},indent=2))

if __name__=='__main__':main()
