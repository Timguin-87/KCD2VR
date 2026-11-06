#!/usr/bin/env python3
"""Verify the retail ordinary-attack -> native time-coordinator static chain."""
import argparse,hashlib,json,struct
from pathlib import Path

def audit(path):
    b=path.read_bytes();digest=hashlib.sha256(b).hexdigest()
    assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    u16=lambda n:struct.unpack_from('<H',b,n)[0]
    u32=lambda n:struct.unpack_from('<I',b,n)[0]
    pe=u32(0x3c);assert b[pe:pe+4]==b'PE\0\0' and u32(pe+8)==0x6A350E20
    optional=pe+24;base=struct.unpack_from('<Q',b,optional+24)[0];sections=optional+u16(pe+20)
    def read(rva,n):
        for i in range(u16(pe+6)):
            s=sections+40*i;va,raw,off=u32(s+12),u32(s+16),u32(s+20)
            if va<=rva and rva+n<=va+raw:return b[off+rva-va:off+rva-va+n]
        raise AssertionError(hex(rva))
    pins={
        0xB27308:'48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 40',
        0xD4DD2C:'48 89 5c 24 10 48 89 74 24 18 57 48 83 ec 20',
        0xB27425:'48 8b 87 a0 00 00 00 48 8b d6 0f 10 44 24 20 f3 0f 7f 40 28 48 8b 8f a0 00 00 00 48 8b 01 ff 50 38',
        0x9391AF:'48 8b 89 c0 03 00 00 e8 95 47 41 00',
        0xD4DA46:'48 8b 4f 08 48 8b 89 88 03 00 00 e8 d6 02 00 00 80 7f 2c 00 0f 28 d8 74 08 f3 0f 10 1d dd c4 34 03',
        0xD4DC0B:'48 8b 47 08 41 0f 28 cb 48 8b 88 10 02 00 00 48 81 c1 50 04 00 00 e8 86 67 c9 ff 48 8b 47 08 41 0f 28 cc 48 8b 88 10 02 00 00 48 81 c1 f0 03 00 00 e8 6b 67 c9 ff 48 8b 47 08 41 0f 28 cd 48 8b 88 10 02 00 00 48 81 c1 20 04 00 00 e8 50 67 c9 ff',
        0x9E3D23:'48 8b 4b 08 f3 0f 10 4b 10 48 8b 89 a0 03 00 00 e8 c4 a5 36 00',
        0xD4E32A:'48 85 c0 74 12 48 8b 08 0f 28 ce 48 8b 91 d8 00 00 00 48 8b c8 ff d2',
        0x1A7D270:'48 8b 41 08 f3 0f 10 80 58 01 00 00 c3',
        0x1A7D250:'48 8b 41 08 f3 0f 10 80 5c 01 00 00 c3',
        0x1A7D230:'48 8b 41 08 f3 0f 10 80 64 01 00 00 c3',
        0x1A7D2B0:'48 8b 41 08 48 8b 80 48 01 00 00 c3',
        0x1A7D2F0:'48 8b 41 08 8b 80 24 02 00 00 c3',
        0x1A72810:'8b 41 3c c3',
    }
    for rva,value in pins.items():assert read(rva,len(bytes.fromhex(value)))==bytes.fromhex(value),hex(rva)
    vtables={0x3A61C58+0x1C8:0xB27308,0x3A61C58+0x80:0x1A72810,0x3A61040+0x38:0x938FD0,
        0x3A61540+0x18:0x1A7D270,0x3A61540+0x20:0x1A7D250,0x3A61540+0x28:0x1A7D230,
        0x3A61540+0x50:0x1A7D2B0,0x3A61540+0x80:0x1A7D2F0}
    for slot,fn in vtables.items():assert struct.unpack('<Q',read(slot,8))[0]==base+fn,hex(slot)
    hashes={}
    for label,start,end in [('attack_start',0xB27308,0xB275F7),('helper_start',0x938FD0,0x939310),
        ('time_initialize',0xD4D950,0xD4DD2C),('rpg_speed',0xD4DD2C,0xD4DE0C),
        ('time_update',0x9E3B80,0x9E3F9C),('animation_rate_dispatch',0xD4E2FC,0xD4E34B)]:
        hashes[label]={'rva':hex(start),'bytes':end-start,'sha256':hashlib.sha256(read(start,end-start)).hexdigest()}
    return {'retail_sha256':digest,'timestamp':'0x6A350E20','pins':{hex(k):v for k,v in pins.items()},
        'vtable_targets':{hex(k):hex(v) for k,v in vtables.items()},'functions':hashes,
        'scope':'Only exact ordinary Henry attack start; pointer members actor+388 and actor+3C0, not embedded objects.',
        'limits':'Static identity/call/dataflow proof only. Does not prove live match rate, delivered animation rate, hit acceptance, NPC spacing or FPS.'}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('dll',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    result=audit(a.dll);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('PASS exact retail SHA/timestamp, 14 instruction pins, 8 virtual targets; native timing dataflow recorded, no game execution')
