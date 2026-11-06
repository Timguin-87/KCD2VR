"""Read-only exact retail registration proof for the optional global dodge CVar."""
import argparse,hashlib,json,struct
from pathlib import Path
from native_research112 import PE
ap=argparse.ArgumentParser();ap.add_argument('dll');ap.add_argument('--output',type=Path);a=ap.parse_args()
p=PE(a.dll);expected='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
assert hashlib.sha256(p.data).hexdigest()==expected and p.stamp==0x6A350E20
pins={0x10AAB19:'f30f10156fda0003',0x10AAB21:'488d0568b5d902',0x10AAB28:'488d5360',0x10AAB2C:'48c744242800000000',0x10AAB35:'41b902000000',0x10AAB3B:'4889442420',0x10AAB40:'488d0d29b5d902',0x10AAB47:'e8c0df3201'}
for address,b in pins.items():assert p.read(address,len(bytes.fromhex(b)))==bytes.fromhex(b),hex(address)
name=b'wh_cs_Dodge_WindowMod\0';assert p.read(0x3E46070,len(name))==name
help=p.read(0x3E46090,160).split(b'\0')[0].decode();assert help=='Mod of Dodge slot window - determined as (t_hit - t_dodgeslot) from attack x this value.'
default=struct.unpack('<f',p.read(0x40B8590,4))[0];assert abs(default-.65)<1e-7
result={'result':'PASS','sha256':expected,'timestamp':hex(p.stamp),'instructionPins':{hex(k):v for k,v in pins.items()},'name':name[:-1].decode(),'help':help,'default':default,'registeredOffset':'0x60','scope':'GLOBAL_NATIVE_CVAR','nativeExecution':'NONE','writeBoundary':'existing fully qualified CombatWindowsConsoleH3, name-based native console only'}
if a.output:a.output.write_text(json.dumps(result,indent=2)+'\n')
print('D70 dodge CVar: exact fingerprint + 8 registration instructions + native name/help/default PASS; no native execution')
