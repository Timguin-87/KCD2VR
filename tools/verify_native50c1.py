"""Verify R50C1 retail instruction evidence without loading/executing WHGame."""
import argparse,hashlib,json,struct
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();raw=a.dll.read_bytes();digest=hashlib.sha256(raw).hexdigest()
assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7','Unsupported retail binary'
pe=struct.unpack_from('<I',raw,0x3c)[0];assert raw[pe:pe+4]==b'PE\0\0'
assert struct.unpack_from('<I',raw,pe+8)[0]==0x6A350E20
n=struct.unpack_from('<H',raw,pe+6)[0];os=struct.unpack_from('<H',raw,pe+20)[0]
sections=[struct.unpack_from('<IIII',raw,pe+24+os+i*40+8) for i in range(n)]
def read(rva,size):
 for vs,va,length,off in sections:
  if va<=rva and rva+size<=va+length:return raw[off+rva-va:off+rva-va+size]
 raise ValueError(f'Unmapped RVA {rva:x}')
anchors=[
 (0xD499D8,'48 89 5c 24 08 48 89 6c 24 18 48 89 74 24 20 41 56','Reaction factory observer entry'),
 (0x90C850,'40 53 48 83 ec 20 48 8b 99 00 02 00 00','Reaction dispatch observer entry; actor+200'),
 (0xC5252F,'e8 dc 10 10 00','C5235C invokes D53610'),
 (0xD536B7,'e8 f4 31 00 00','D53610 invokes D568B0'),
 (0xC5259E,'e8 35 74 0f 00','Native handler creates reaction'),
 (0xC52614,'e8 37 a2 cb ff','Native handler dispatches reaction'),
 (0x22B8156,'32 c0','Repeat-deadline rejection returns false'),
 (0xA73FAB,'83 f8 16','Reaction veto checks action category 22'),
 (0xA73FB0,'83 f8 09','Reaction veto checks action category 9'),
 (0x688B4A,'48 81 c1 a0 0b 00 00','Reaction veto property is BA0, not attack phase 30'),
 (0x9CC4F2,'e8 e5 9a f4 ff','Action manager invokes transition routine'),
 (0x9CC52F,'e8 c4 07 00 00','Action manager installs replacement through 9CCCF8'),
 (0xD4D166,'48 81 c1 f0 03 00 00','Prepared phase checks independent deadline 3F0'),
 (0xD4D17E,'e8 c9 01 00 00','Helper update invokes strike entry D4D34C'),
 (0xC52D05,'48 81 c1 20 04 00 00','Strength sample reads independent timer 420'),
 (0x9E3C16,'0f 28 c8','Coordinator frame-time return site; observation only'),
]
records=[]
for rva,hexstr,meaning in anchors:
 expected=bytes.fromhex(hexstr);actual=read(rva,len(expected));assert actual==expected,(hex(rva),actual.hex(),hexstr)
 records.append({'rva':hex(rva),'bytes':actual.hex(),'meaning':meaning})
names=['wh_cs_DisableCombatCamera','wh_cs_PlayerInputLockAreaWidth','wh_cs_PlayerInputLockingTolerance',
 'wh_cs_DelayedPerfectBlockInSlot','wh_cs_PerfectBlock_SpatialVolumeRadius','wh_cs_AIForceBlockMode']
strings={name:raw.find(name.encode()+b'\0') for name in names}
assert all(offset>=0 for offset in strings.values()),'CVar name absent from pinned binary'
a.out.parent.mkdir(parents=True,exist_ok=True)
a.out.write_text(json.dumps({'dll':str(a.dll),'sha256':digest,'timestamp':'0x6A350E20','nativeExecution':False,
 'anchors':records,'cvarStringFileOffsets':strings,'cvarSemantics':'Names verified; defaults/actor scope are not established by string presence.'},indent=2)+'\n')
print(f'PASS {len(records)} instruction anchors and {len(strings)} CVar names; no native execution')
