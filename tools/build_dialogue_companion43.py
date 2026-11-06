"""Keep native participant/twin lifecycle; ASI43 changes Henry presentation only.
Offline, no installation. Preserve every unrelated companion byte/member.
"""
from pathlib import Path
import zipfile,hashlib,json,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import build_dialogue_lua30 as oldlua
import build_dialogue_original_initiator28 as oldbt
import build_activity_dialogue47c1 as oldpreset

def assets(scripts):
    with zipfile.ZipFile(scripts) as z:
        out={}
        for name,expected in oldlua.INPUTS.items():
            data=z.read(name)
            if hashlib.sha256(data).hexdigest()!=expected:raise ValueError('retail Lua mismatch')
            out[name]=data # native true, not the earlier whole-controller bypass
        name='AI/speech/dialoginitiator_inner.xml'
        data=oldbt.build(z.read(name))
        text=data.decode('ascii')
        needle='$kcd2vrUseTwins28 = false&#10;'
        if text.count(needle)!=1:raise ValueError('unexpected shared request recipe')
        out[name]=text.replace(needle,'').encode('ascii')
        # Preserve the old preset's movement/rotation repair, but no longer
        # change the preset's native controller/twin choice.
        name='AI/speech/getparametersfrompreset.xml'
        current=oldpreset.patch_dialogue(z.read(name))
        needle=b'$dialogParameters.useTwinsAsInitiator = false&#10;$dialogParameters.movePlayer = false'
        if current.count(needle)!=1:raise ValueError('unexpected preset recipe')
        out[name]=current.replace(needle,b'$dialogParameters.movePlayer = false')
    return out

def main():
    import argparse
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--scripts',type=Path,required=True);a.add_argument('--base',type=Path,required=True)
    a.add_argument('--out',type=Path,required=True);a.add_argument('--write-source',action='store_true');p=a.parse_args()
    changed=assets(p.scripts)
    with zipfile.ZipFile(p.base) as z:
        if z.testzip():raise ValueError('base CRC')
        members={n:z.read(n) for n in z.namelist()}
        if len(members)!=len(z.namelist()):raise ValueError('duplicate member')
    if not set(changed)<=set(members):raise ValueError('missing existing dialogue member')
    before={n:hashlib.sha256(members[n]).hexdigest() for n in changed}
    members.update(changed);p.out.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(p.out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for n,d in members.items():z.writestr(n,d)
    with zipfile.ZipFile(p.out) as z:
        assert not z.testzip() and set(z.namelist())==set(members)
        for n,d in members.items():assert z.read(n)==d
    if p.write_source:
        for n,d in changed.items():(ROOT/'companion/source'/n).write_bytes(d)
    report={'pak_sha256':hashlib.sha256(p.out.read_bytes()).hexdigest(),'members':len(members),
            'changed':{n:{'before':before[n],'after':hashlib.sha256(d).hexdigest()} for n,d in changed.items()},
            'scope':'Native participant lifecycle; only Henry twin presentation is changed by exact ASI43 creator hook.',
            'native_bt_and_headset_acceptance':'NOT_RUN','installed':False}
    p.out.with_suffix('.C43.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
