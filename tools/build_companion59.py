"""R59C4: rebuild the seven-file reference companion, validate structure, write a manifest.
Unvalidated additional tables require --experimental-tables and produce .pak.disabled only.
Table row patches (`<table>__kcdvr.xml`) and the ADB come from tools/gen_companion59.py (run it against your own
vanilla Tables/Animations extraction first when you change the recipe)."""
from pathlib import Path
import argparse,hashlib,json,os,tempfile,zipfile,xml.etree.ElementTree as E
r=Path(__file__).resolve().parents[1]
src=r/'companion/source';out=r/'companion/Mods/KCD2VR/Data/KCD2VR.pak'
parser=argparse.ArgumentParser(description='Build source pak; XML/CRC checks are NOT native database validation.')
parser.add_argument('--experimental-tables',action='store_true',help='Emit unvalidated table candidate as .pak.disabled; never replace working pak')
args=parser.parse_args()
allowed=set(json.loads((r/'companion/WORKING_BASE_MEMBERS59C4.json').read_text())['members'])
if args.experimental_tables:out=r/'companion/diagnostic_candidate_R59_5/KCD2VR.pak.disabled'
members={}
for p in sorted(src.rglob('*')):
    if not p.is_file():continue
    name=p.relative_to(src).as_posix()
    if not args.experimental_tables and name not in allowed:continue
    data=p.read_bytes()
    if p.suffix.lower() in('.xml','.adb'):E.fromstring(data)
    members[name]=data
profile=E.fromstring(members['Libs/Config/defaultProfile.xml'])
maps={m.get('name'):m for m in profile.iter('actionmap')}
special=maps['combat'].find("action[@name='attack_special']")
if not (special.get('onPress')==special.get('onRelease')=='1' and special.get('onHold')=='0' and special.get('xboxpad')=='xi_shoulderl'):
    raise ValueError('Unbalanced special-action binding')
locked=maps['combat_locked'].find("action[@name='attack_special']")
if not (locked.get('holdTriggerDelay')=='0.25' and locked.get('xboxpad')=='xi_triggerl_btn'):
    raise ValueError('Native locked LT binding changed')
# every row patch must reference fragments that exist in the shipped ADB
adb=members.get('Animations/Mannequin/ADB/kcd_male_combat_generated.adb',b'').decode('utf-8')
import re
for name,data in members.items():
    if name.startswith('Libs/Tables/combat/combat_action_') and name.endswith('__kcdvr.xml'):
        for g in re.findall(rb'mn_fragment_guid="([^"]+)"',data):
            if ('GUID="%s"'%g.decode()) not in adb:raise ValueError('row without fragment: %s %s'%(name,g.decode()))
out.parent.mkdir(parents=True,exist_ok=True)
fd,tmp=tempfile.mkstemp(prefix='KCD2VR.',suffix='.tmp',dir=out.parent);os.close(fd)
try:
    with zipfile.ZipFile(tmp,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for name,data in members.items():
            info=zipfile.ZipInfo(name,(2026,10,11,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16
            z.writestr(info,data)
    with zipfile.ZipFile(tmp) as z:
        if z.testzip() is not None or set(z.namelist())!=set(members):raise ValueError('Archive validation failed')
    os.replace(tmp,out)
finally:
    if os.path.exists(tmp):os.unlink(tmp)
manifest={'schema':2,'native_database_validated':False,'experimental_tables':args.experimental_tables,'members':[{'member':n,'sha256':hashlib.sha256(d).hexdigest(),'bytes':len(d)} for n,d in members.items()],
          'pak_sha256':hashlib.sha256(out.read_bytes()).hexdigest()}
(r/('companion/SOURCE_MANIFEST59_EXPERIMENTAL.json' if args.experimental_tables else 'companion/SOURCE_MANIFEST59.json')).write_text(json.dumps(manifest,indent=1))
print(json.dumps({'pak':str(out),'sha256':manifest['pak_sha256'],'members':len(members)}))
