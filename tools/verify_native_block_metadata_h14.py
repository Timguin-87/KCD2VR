"""Read-only retail BlockParams -> state payload proof; never executes game code."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import xml.etree.ElementTree as ET
import zipfile
from native_research112 import PE

EXPECTED='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path,required=True);p.add_argument('--tables',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();pe=PE(a.whgame);sha=hashlib.sha256(pe.data).hexdigest()
assert sha==EXPECTED and pe.stamp==0x6A350E20
checks=[]
def pin(name,rva,hexbytes):
    expected=bytes.fromhex(hexbytes);actual=pe.read(rva,len(expected))
    assert actual==expected,(name,hex(rva),actual.hex())
    checks.append(dict(name=name,rva=hex(rva),bytes=actual.hex()))
pin('BlockParams exact getter+60',0x3A8A490,struct.pack('<Q',pe.base+0x1A7D2B0).hex())
pin('BlockParams RTTI exact class name',0x4AFA620,b'.?AUS_CombatActorActionBlockParams@combatmodule@wh@@\0'.hex())
pin('BlockParams getter returns row+148 property',0x1A7D2B0,'488b4108488b8048010000c3')
pin('block helper copies property+8 through notifying state+270 setter',0xC53B99,'488d8b700200008b5008e82867ceff')
pin('block helper publishes preparing128',0xC53BD0,'ba80000000488b88100200004883c130e8e364ceff')
pin('native ready transition publishes256 without rewriting executed payload',0xC55595,'ba00010000488b88100200004883c130e81e4bceff')
pin('block cleanup resets same property to-1',0xC5503F,'488d8b7002000083caff4883c4205be97d52ceff')
pin('integer property setter stores changed payload+8 then notifies',0x93A2D0,'4883ec38448b4108448bca488bd1453bc175054883c438c3448949084883c118c744242000000000e89f55fdff')
pin('block row source rawDC resolves native enum and stores row+148',0x935DDA,'488d5530418b87dc000000894530e8d7e5040049898648010000')
pin('native input class lookup uses registry5320FF8',0x9843CB,'488d0d26cc9904')
pin('original block context0 getter exact slot',0x3B0A970,struct.pack('<Q',pe.base+0x1A7DDE0).hex())
pin('original block context getter indexes property+18 by contextDL',0x1A7DDE0,'0fb6c20fb6440818c3')
pin('original block input press tests native eligibility function',0x27C2A4D,'e89ed4ffff84c0747e')
pin('native block setter reads same context through state+630 getter',0x93A4EE,'488b8910020000418ad04881c130060000418af0488b01ff5008')
pin('original LTup only queues controller88',0x27BCE46,'c6838800000001')
pin('original LTdown clears queued88 then context0 press',0x27C2AC0,'488b0f4533c0b201c6838800000000e8007a17fe')
pin('native Update notices queued release88',0x9D42BF,'80bf88000000000f85d4ed8801')
pin('native Update issues context0 false then clears queue88',0x22630B4,'488b4f184533c033d2e812746dfec6878800000000')
pin('PB eligibility excludes phase8 via original predicate',0x908C83,'ba08000000e84f63e2ff84c07555')
pin('PB eligibility excludes phase512 via original predicate',0x908C95,'ba00020000e83d63e2ff84c07543')

tables={}
with zipfile.ZipFile(a.tables) as z:
    for name in ('combat_action_block.xml','combat_action_block.tbl','combat_input_class.xml','combat_zone.xml'):
        member='Libs/Tables/combat/'+name;b=z.read(member)
        tables[name]=dict(member=member,sha256=hashlib.sha256(b).hexdigest(),bytes=len(b))
        if name.endswith('.xml'):tables[name]['rows']=[dict(x.attrib) for x in ET.fromstring(b).iter() if x.tag==name[:-4]]
        else:raw=b
rows=tables['combat_action_block.xml']['rows'];n=struct.unpack_from('<I',raw,16)[0]
text_bytes=struct.unpack_from('<I',raw,20)[0]
assert n==len(rows)==222 and len(raw)==28+n*0x140+text_bytes
# Every corresponding raw record proves which enum supplies row+148; the
# native conversion above reads raw+DC. raw+D4 independently matches block zone.
for i,row in enumerate(rows):
    assert struct.unpack_from('<i',raw,28+i*0x140+0xDC)[0]==int(row['input_class_id'])==7
    assert struct.unpack_from('<i',raw,28+i*0x140+0xD4)[0]==int(row['block_zone_id'])
assert any(r.get('combat_input_class_id')=='7' and r.get('combat_input_class_name')=='block' for r in tables['combat_input_class.xml']['rows'])
assert not any(r.get('combat_zone_id')=='7' for r in tables['combat_zone.xml']['rows'])
for table in tables.values():table.pop('rows',None)
r=dict(sha256=sha,timestamp=hex(pe.stamp),pins=checks,tables=tables,
    blockRows=n,rawRecordOffset=28,rawRecordStride='0x140',nativeEnumSource='raw+DC -> 9843C4 -> row+148 -> BlockParams+60 -> C53BA3 -> state+278',
    meaning='7 is block input metadata copied by the native block helper, not an attack sector; remains through128 and256 until native cleanup',
    blockRelease='27BCE04 queues controller+88; 9D4244/22630A0 Update calls93A4D4(false,context0) before clearing88; immediate27C29E0 press clears88 and can cancel queued release',
    releaseAck='Readonly exact property state+630/vft3B0A968/getter1A7DDE0/context0 payload state+648: only0 ACK;1 held;anything else unknown. +308 is not input ACK.',
    nativeExecution=False,nativeWrites=False,scope='Exact retail static proof only; does not award native admission, block or damage')
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(r,indent=2)+'\n',encoding='utf8');print(f'PASS {len(checks)} exact retail pins +{n} matched native block records; native execution/writes=NONE')
