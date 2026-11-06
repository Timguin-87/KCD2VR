"""Add the reviewed hand-only door owner to the exact H39 combined Data PAK.

Outputs a new external staging directory. Never installs or overwrites an input.
All parent gameplay members and localization resources retain their exact bytes.
"""
from pathlib import Path
import argparse,copy,hashlib,json,shutil,zipfile

ROOT=Path(__file__).resolve().parents[1]
PARENT_SHA='61902d4ec41461f3100947d8390cbc16fb6331e21ebed7c2ce61b92233454a37'
MEMBER='Scripts/Startup/zzkcd2vr_hand_only_doors_h40.lua'
def sha(data):return hashlib.sha256(data).hexdigest()
def attrs(i):return {k:getattr(i,k) for k in ('filename','date_time','compress_type','comment','extra','create_system','create_version','extract_version','internal_attr','external_attr')}
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output',type=Path,required=True);a=ap.parse_args()
    out=a.output.resolve();parent=ROOT/'companion/C63_UNIFIED';base=parent/'Mods/KCD2VR';old=base/'Data/KCD2VR.pak'
    assert not out.exists(),'Refuse overwrite of staging or delivery'
    assert ROOT not in out.parents and out!=ROOT and not any(x.lower()=='steamapps' for x in out.parts),'External non-game staging only'
    assert sha(old.read_bytes())==PARENT_SHA,'Wrong exact parent PAK'
    script=ROOT/'mods/KCD2VRDoors/hand_only_h40.lua';payload=script.read_bytes();assert payload
    with zipfile.ZipFile(old) as z:
        assert z.testzip() is None
        infos=z.infolist();before={i.filename:z.read(i) for i in infos if not i.is_dir()};comment=z.comment
        assert len({n.casefold() for n in before})==len(before) and MEMBER.casefold() not in {n.casefold() for n in before}
    shutil.copytree(parent,out)
    pak=out/'Mods/KCD2VR/Data/KCD2VR.pak'
    with zipfile.ZipFile(pak,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        z.comment=comment
        for i in infos:z.writestr(copy.copy(i),before[i.filename] if not i.is_dir() else b'')
        i=zipfile.ZipInfo(MEMBER,(2026,10,23,0,0,0));i.compress_type=zipfile.ZIP_DEFLATED;z.writestr(i,payload)
    with zipfile.ZipFile(pak) as z:
        assert z.testzip() is None and len(z.namelist())==len(infos)+1
        for i in infos:
            assert z.read(i.filename)==(before[i.filename] if not i.is_dir() else b'')
            assert attrs(z.getinfo(i.filename))==attrs(i),'Parent ZIP metadata changed: '+i.filename
        assert z.read(MEMBER)==payload
    meta_path=out/'Mods/KCD2VR/KCD2VR_UNIFIED63.json';m=json.loads(meta_path.read_text(encoding='utf-8-sig'))
    m.update(candidate='H40_HAND_ONLY_DOORS_20261023',pak_sha256=sha(pak.read_bytes()),members=len(infos)+1,installed=False)
    m['hand_only_doors_h40']={'source':'mods/KCD2VRDoors/hand_only_h40.lua','member':MEMBER,'sha256':sha(payload),'parent_pak_sha256':PARENT_SHA,'native_Henry_action_started':False,'headset':'NOT_RUN'}
    m['members_provenance'].append({'path':MEMBER,'sha256':sha(payload),'sources':['mods/KCD2VRDoors/hand_only_h40.lua']})
    meta_path.write_text(json.dumps(m,indent=2)+'\n',encoding='utf-8',newline='\n')
    for p in parent.rglob('*'):
        if p.is_file() and p.relative_to(parent).as_posix() not in ('Mods/KCD2VR/Data/KCD2VR.pak','Mods/KCD2VR/KCD2VR_UNIFIED63.json'):
            assert (out/p.relative_to(parent)).read_bytes()==p.read_bytes(),'Unrelated distribution changed'
    receipt={'parent_pak_sha256':PARENT_SHA,'pak_sha256':sha(pak.read_bytes()),'added':{MEMBER:sha(payload)},'changed':[],'removed':[],'preserved_parent_members':len(before),'parent_member_metadata_preserved':True,'localization_unchanged':True,'installed':False,'headset':'NOT_RUN'}
    (out/'H40_BUILD_RECEIPT.json').write_text(json.dumps(receipt,indent=2)+'\n',newline='\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
