"""C28 companion: preserve the exact installed working PAK; add one dialogue script.
No table regeneration or installation. --base must be the recorded working image.
"""
from pathlib import Path
import argparse, hashlib, json, os, tempfile, zipfile
import build_dialogue_original_initiator28 as dialogue
ROOT=Path(__file__).resolve().parents[1]
BASE_SHA='2c124153fcf10fa8f148cbf5c42ca15f08ea8c416780e96125083823e07e300c'
MEMBER='AI/speech/dialoginitiator_inner.xml'

def build(base:Path,scripts:Path,out:Path):
    if base.resolve()==out.resolve():raise ValueError('output must not overwrite the working base')
    if hashlib.sha256(base.read_bytes()).hexdigest()!=BASE_SHA:raise ValueError('working companion fingerprint mismatch')
    with zipfile.ZipFile(scripts) as z:patched=dialogue.build(z.read(MEMBER))
    if patched!=(ROOT/'companion/source'/MEMBER).read_bytes():raise ValueError('authored source differs from exact-retail recipe')
    with zipfile.ZipFile(base) as z:
        if z.testzip() is not None:raise ValueError('bad working companion CRC')
        names=z.namelist()
        if len(set(names))!=len(names) or MEMBER in names:raise ValueError('duplicate or unexpected dialogue member')
        old={n:z.read(n) for n in names}
    out.parent.mkdir(parents=True,exist_ok=True)
    fd,tmp=tempfile.mkstemp(prefix='C28.',suffix='.tmp',dir=out.parent);os.close(fd)
    try:
        with zipfile.ZipFile(tmp,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for n,data in list(old.items())+[(MEMBER,patched)]:
                info=zipfile.ZipInfo(n,(2026,10,16,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16
                z.writestr(info,data)
        with zipfile.ZipFile(tmp) as z:
            assert z.testzip() is None and z.namelist()==names+[MEMBER]
            for n,data in old.items():assert z.read(n)==data,n
            assert z.read(MEMBER)==patched
        os.replace(tmp,out)
    finally:
        if os.path.exists(tmp):os.unlink(tmp)
    return {'schema':1,'base_sha256':BASE_SHA,'pak_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),
        'preserved_members':[{'name':n,'sha256':hashlib.sha256(d).hexdigest()} for n,d in old.items()],
        'added_member':{'name':MEMBER,'sha256':hashlib.sha256(patched).hexdigest()},
        'table_changes':False,'native_bt_compile':'NOT_RUN','headset_validation':'NOT_RUN','installed':False}

def main():
    a=argparse.ArgumentParser(description=__doc__);a.add_argument('--base',required=True,type=Path);a.add_argument('--scripts',required=True,type=Path);a.add_argument('--out',required=True,type=Path)
    args=a.parse_args();report=build(args.base,args.scripts,args.out)
    args.out.with_suffix('.manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
if __name__=='__main__':main()
