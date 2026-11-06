"""Compare V171 with committed V170, retaining untouched runtime boundaries."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json,subprocess,re
ROOT=Path(__file__).resolve().parents[1]
BASE='f71b5f1150240b489cc56759c7009076e88584a6'
INTENDED={'src/mod.cpp','src/flashui_probe.cpp','src/flashui_probe.h','src/vr_menu.cpp',
          'src/ui_hud_transition170.h','src/action_map_probe.cpp','src/action_map_probe.h'}
def normalized(data):return data.replace(b'\r\n',b'\n')
def sha(data):return hashlib.sha256(data).hexdigest()
def main():
    paths=subprocess.check_output(['git','ls-tree','-r','--name-only',BASE,'src','runtime'],cwd=ROOT,text=True).splitlines()
    protected={};changed=[]
    for name in paths:
        old=subprocess.check_output(['git','show',BASE+':'+name],cwd=ROOT)
        new=(ROOT/name).read_bytes()
        if normalized(old)!=normalized(new):
            assert name in INTENDED,name
            changed.append(name)
        else:protected[name]=sha(normalized(new))
    record={'baseline_commit':BASE,'files':protected,'intentional_existing_source_changes':changed,
            'new_headers':['src/ui_hud_elements171.h','src/ui_palm171.h']}
    (ROOT/'tests/protected_baseline171.json').write_text(json.dumps(record,indent=2)+'\n')
    logs={}
    game=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO')
    for name in ['KCD2VR.log','KCD2VR-diagnostics.log']:
        data=(game/name).read_bytes();lines=data.decode(errors='replace').splitlines()
        selected=[s for s in lines if any(p in s for p in ['KCD2VR BUILD','V66.170-HUD TRANSITION','V66.120R2-HUD-GEOMETRY','native-canvas parents fitted'])]
        logs[name]={'sha256':sha(data),'bytes':len(data),'lines':len(lines),'selected_records':selected,
                    'raw_log_redistributed':False}
    source=read_source(ROOT/'src/flashui_probe.cpp')
    assert 'HudRender162r1(' not in source and 'last_hint_reassert_ms' not in read_source(ROOT/'src/mod.cpp')
    import pefile
    dll=game/'WHGame.dll';assert sha(dll.read_bytes())=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pe=pefile.PE(str(dll),fast_load=True);b=pe.OPTIONAL_HEADER.ImageBase
    assert pe.FILE_HEADER.TimeDateStamp==0x6a350e20
    verify=source[source.index('bool VerifyHudDisplay170('):]
    for name,rva,slot in [('setter',0x7f4520,0xc8),('getter',0x7f4784,0xd0)]:
        import struct
        raw=re.search('constexpr unsigned char '+name+r'\[\]=\{([^}]+)\};',verify).group(1)
        sig=bytes(int(x.strip(),16) for x in raw.split(','));assert pe.get_data(rva,len(sig))==sig
        assert struct.unpack('<Q',pe.get_data(0x3a55ba0+slot,8))[0]==b+rva
    record.update(logs=logs,exact_display_abi_verified=True,headset_verified=False)
    (ROOT/'review/V66.171/preservation_and_logs.json').write_text(json.dumps(record,indent=2)+'\n')
    print(json.dumps({'unchanged_source_runtime_files':len(protected),'changed':changed,'native_abi_verified':True}))
if __name__=='__main__':main()
