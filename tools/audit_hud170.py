"""Record preservation, retail display ABI and supplied runtime-log limits."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json,re,struct,subprocess,sys
ROOT=Path(__file__).resolve().parents[1]
BASE='1925d3e12a21987342f9522005527a6f461dc163'
def sha(data):return hashlib.sha256(data).hexdigest()
def normalized(data):return data.replace(b'\r\n',b'\n')
def main():
    if len(sys.argv)>1:sys.path.insert(0,sys.argv[1])
    import pefile
    game=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2')
    dll=game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll'
    digest=sha(dll.read_bytes());assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pe=pefile.PE(str(dll),fast_load=True);b=pe.OPTIONAL_HEADER.ImageBase
    assert pe.FILE_HEADER.TimeDateStamp==0x6a350e20
    abi={}
    source=read_source(ROOT/'src/flashui_probe.cpp')
    verify=source[source.index('bool VerifyHudDisplay170(std::uintptr_t b) noexcept {'):]
    for name,rva,slot in [('setter',0x7f4520,0xc8),('getter',0x7f4784,0xd0)]:
        raw=re.search('constexpr unsigned char '+name+r'\[\]=\{([^}]+)\};',verify).group(1)
        sig=bytes(int(x.strip(),16) for x in raw.split(','))
        assert pe.get_data(rva,len(sig))==sig
        assert struct.unpack('<Q',pe.get_data(0x3a55ba0+slot,8))[0]==b+rva
        abi[name]=dict(rva=hex(rva),entry=sig.hex(),slot=hex(slot))
    assert 'HudRender162r1(' not in source and 'hud162r1_scoped_draw' not in source
    intended={'src/mod.cpp','src/mod.h','src/flashui_probe.cpp','src/flashui_probe.h','src/vr_menu.cpp','src/vr_menu.h'}
    paths=subprocess.check_output(['git','ls-tree','-r','--name-only',BASE,'src','runtime'],cwd=ROOT,text=True).splitlines()
    protected={};changed=[]
    for name in paths:
        old=subprocess.check_output(['git','show',BASE+':'+name],cwd=ROOT)
        new=(ROOT/name).read_bytes()
        if normalized(old)!=normalized(new):
            assert name in intended,name
            changed.append(name)
        else:protected[name]=sha(normalized(new))
    logs={}
    for name in ['KCD2VR.log','KCD2VR-diagnostics.log']:
        p=game/'Bin/Win64MasterMasterSteamPGO'/name;data=p.read_bytes();text=data.decode('utf-8',errors='replace')
        selected=[line for line in text.splitlines() if 'KCD2VR BUILD' in line or 'BLOCKED_CALIBRATION_REQUIRED' in line or 'HUD_COST' in line or 'HUD_COVERAGE' in line]
        logs[name]=dict(sha256=sha(data),bytes=len(data),selected_records=selected,raw_log_redistributed=False)
    asi=(ROOT/'build/release/RelWithDebInfo/KCD2VR.asi').read_bytes()
    assert b'KCD2VR-V66.170-MANUAL-HUD-PALM-TRANSITIONS' in asi
    record=dict(baseline_commit=BASE,source_zip_sha256=sha(Path('C:/Users/turcs/Downloads/KCD2VR_V66.169R2_BOW_POSE_HILT_SOURCE.zip').read_bytes()),
                qualified_retail_sha256=digest,display_abi=abi,hud_render_hook_installed=False,
                intentional_changed_existing_source=changed,protected_normalized_sha256=protected,
                supplied_logs=logs,asi_sha256=sha(asi),headset_verified=False,fps_measured=False)
    out=ROOT/'review/V66.170/preservation_and_native_evidence.json';out.write_text(json.dumps(record,indent=2),encoding='utf-8')
    print(json.dumps(dict(protected_files=len(protected),intentional_changes=changed,asi_sha256=sha(asi),output=str(out))))
if __name__=='__main__':main()
