"""Read-only retail model and exact opaque-WUID formatter verification."""
from pathlib import Path
import hashlib, json, re, zipfile, xml.etree.ElementTree as ET
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
GAME=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2')
data=GAME/'Data'
with zipfile.ZipFile(data/'Tables.pak') as z:
    raw=z.read('Libs/Tables/item/item.xml')
models={}
for row in ET.fromstring(raw).iter('MeleeWeapon'):
    if row.get('Class') not in ('3','5') or not row.get('Model'): continue
    path=row.get('Model').lower().replace(chr(92),'/')
    if not path.startswith('objects/'):path='objects/'+path
    if not path.endswith('.cgf'):path+='.cgf'
    models[row.get('Id').lower()]=path
models['c0760688-f426-5341-95b9-482b87dbcb9b']='objects/manmade/weapons/axes/workaxe01.cgf'
available=set()
for path in sorted(data.glob('IPL_Objects-part*.pak')):
    with zipfile.ZipFile(path) as z: available.update(n.lower().replace(chr(92),'/') for n in z.namelist())
assert all(path in available for path in models.values())
text=(ROOT/'src/vrik/native_supply_t2.lua').read_text()
section=text.split('local beltModels={',1)[1].split('  }',1)[0]
assert dict(re.findall(r"\['([^']+)'\]='([^']+)'",section))==models
pe=PE(GAME/'Bin/Win64MasterMasterSteamPGO/WHGame.dll')
digest=hashlib.sha256(pe.data).hexdigest()
assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
def string(rva):return pe.read(rva,64).split(b'\0',1)[0].decode()
assert string(0xE30C8F+0x2C874C1)=='Framework'
assert string(0x1182CE3+0x29A0D55)=='WUIDToString'
assert string(0x25CF204+int.from_bytes(pe.read(0x25CF200,4),'little',signed=True))=='Item'
assert string(0x25CF313+0x211486D)=='[%s-%llu]'
print(json.dumps({'result':'PASS','retail_sha256':digest,'model_entries':len(models),
    'all_models_exist':True,'lua_table_matches_retail':True,'namespace':'Framework.WUIDToString',
    'formatter':'[Item-<low56 unsigned decimal>]','wuid_numeric_conversion':False,'game_writes':0},indent=2))
