"""Read-only retail/source proof for the six automatic-combat scalar readers.

No native function is invoked and no game/source data is modified.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct
from native_research112 import PE

ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('--whgame',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
ap.add_argument('--out',type=Path,required=True)
a=ap.parse_args();root=Path(__file__).resolve().parents[1];p=PE(a.whgame)
digest=lambda data:hashlib.sha256(data).hexdigest()
assert p.stamp==0x6A350E20 and digest(p.data)=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
source=root/'src/subsystems/direct/npc_tuning.cpp';text=source.read_text()
field_text=text.split('std::array<Field,10> fields{{',1)[1].split('struct Reader',1)[0]
fields=[(int(x,16),float(y.rstrip('f'))) for x,y in re.findall(r'\{(0x[0-9A-Fa-f]+),([.0-9]+f)\}',field_text)]
assert fields==[(0xC8C,.8),(0xCA4,2.),(0xCC8,1.),(0xCCC,2.),(0xCD0,2.),(0xCD4,3.),(0xCE0,.5),(0xCE4,1.),(0xC60,3.),(0xD00,.6)]
reader_text=text.split('std::array<Reader,6> readers{{',1)[1].split('struct alignas',1)[0]
readers=[(int(x,16),int(y),int(z)) for x,y,z in re.findall(r'\{(0x[0-9A-Fa-f]+),(\d+),(\d+)\}',reader_text)]
assert readers==[(0x72C0D9,0,1),(0x4C7362,1,1),(0x9130BD,2,6),(0x2CF7945,8,1),(0xADBBB1,9,1),(0x122478D,1,1)]
assert 'constexpr std::size_t extent=0xD0C;' in text
entry_text=text.split('constexpr unsigned char entry[]{',1)[1].split('};',1)[0]
entry=bytes(int(x,0) for x in re.findall(r'0x[0-9A-Fa-f]+|\b0\b',entry_text))
assert p.read(0x649D30,len(entry))==entry
calls_text=text.split('std::array<std::array<unsigned char,5>,6> calls{{',1)[1].split('for(std::size_t i=',1)[0]
calls=bytes(int(x,16) for x in re.findall(r'0x[0-9A-Fa-f]+',calls_text));assert len(calls)==30
pins=[]
for i,(site,first,count) in enumerate(readers):
    actual=p.read(site,5);assert actual==calls[i*5:i*5+5] and actual[0]==0xE8
    assert site+5+struct.unpack('<i',actual[1:])[0]==0x649D30
    pins.append({'callRVA':hex(site),'returnRVA':hex(site+5),'callBytes':actual.hex(),'tunedFields':[hex(fields[j][0]) for j in range(first,first+count)]})
# Exact selected getter result retention and scalar loads in each native reader.
loads={0x72C0FD:'f3440f10858c0c0000',0x4C73A6:'f30f108da40c0000',
       0x91319A:'f30f108bd00c0000',0x9131A6:'f30f1093c80c0000',
       0x9131CD:'f30f108bd40c0000',0x9131D5:'f30f109bcc0c0000',
       0x913253:'f30f1083e00c0000',0x91327B:'f30f1083e40c0000',
       0x2CF7A06:'f30f1083600c0000',0xADBCB2:'f30f1095000d0000',
       0x122480A:'f3410f1086a40c0000'}
for site,raw in loads.items():assert p.read(site,len(bytes.fromhex(raw)))==bytes.fromhex(raw)
# Guarded accessor's returned object and native RTTI; it is not table532E588.
assert p.read(0x649D57,7)==bytes.fromhex('488d0572c6f904')
assert p.read(0x649D5E,5)==bytes.fromhex('4883c428c3')
vft=0x3F18BD0;col=struct.unpack('<Q',p.read(vft-8,8))[0]-p.base
td=struct.unpack('<I',p.read(col+12,4))[0]
assert p.read(td+16,100).split(b'\0')[0]==b'.?AUS_Constants@rpgmodule@wh@@'
ranges={'maxAttack':(0x72C0AC,0x72C1D1),'maxAttackCold':(0x222B80C,0x222B86A),
        'sigma':(0x4C7338,0x4C7412),'sigmaCold':(0x21F6CAE,0x21F6CBE),
        'zones':(0x913090,0x9132ED),'trick':(0x2CF7920,0x2CF7AA5),
        'clinch':(0xADBB8C,0xADBD23),'sigmaSecond':(0x1224760,0x12248AF)}
report={'result':'PASS_EXACT_RETAIL_AND_SOURCE','retailSHA256':digest(p.data),'timestamp':hex(p.stamp),
        'sourceSHA256':digest(source.read_bytes()),'headerSHA256':digest((root/'src/subsystems/direct/npc_tuning.h').read_bytes()),
        'getterRVA':'0x649D30','nativeObjectRVA':'0x55E63D0','readExtent':'0xD0C',
        'sixReaderPins':pins,'nativeScalarLoadPins':{hex(k):v for k,v in loads.items()},
        'auditedReadOnlyRanges':{k:{'begin':hex(x),'endExclusive':hex(y),'sha256':digest(p.read(x,y-x))} for k,(x,y) in ranges.items()},
        'policy':'Current freshly returned native scalar A -> exact earlier C8 scalar H.0 is original pointer identity;100 is H. This is not historical PAK/perk sequence replay.',
        'ownership':'Original native getter remains sole native object owner. Only six automatic-combat formula callsites receive thread-local/per-site read-only scalar snapshots. All other callers, including the second clinch getter, receive the exact native pointer.',
        'limits':['Static pin/read-extent proof is not live/native/headset execution.','These are automatic-combat formula callsites; no guessed actor-classification gate is added.','Same-site reentry refreshes its own current-value snapshot, while different sites/threads have independent storage. No Constants pointer escapes the audited reader ranges.'],
        'nativeExecution':False,'nativeMemoryWrites':False}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
print('PASS exact retail:6 CALL pins +11 selected native scalar loads +8 audited main/cold ranges; source field masks/endpoints match')
