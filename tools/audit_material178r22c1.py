#!/usr/bin/env python3
"""Exact retail material-effect caller/constructor pins; no native execution."""
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
        0xB725C4:'48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20 33',
        0xB726CA:'40 88 b3 d8 00 00 00', # final byte field within 0xE0 allocation
        0xB72700:'c3',
        0xB24AE0:'48 8d 4c 24 40 e8 da da 04 00', # stack object -> constructor
        0xB24B67:'ff 50 50', # same live service execute slot
        0xB24C28:'ff 50 30', # runtime custom row/surface lookup
    }
    for rva,value in pins.items():assert read(rva,len(bytes.fromhex(value)))==bytes.fromhex(value),hex(rva)
    for slot,fn in [(0x4056CB8,0x52AA38),(0x4056CD8,0x52A9A4)]:
        assert struct.unpack('<Q',read(slot,8))[0]==base+fn,hex(slot)
    return {'retail_sha256':digest,'timestamp':'0x6A350E20','pins':{hex(k):v for k,v in pins.items()},
        'constructor_sha256':hashlib.sha256(read(0xB725C4,0xB72701-0xB725C4)).hexdigest(),
        'dispatcher_sha256':hashlib.sha256(read(0xB24A0C,0xB24B8E-0xB24A0C)).hexdigest(),
        'service_global_rva':'0x492D920','service_vtable_rva':'0x4056C88',
        'lookup_rva':'0x52AA38','execute_rva':'0x52A9A4','params_bytes':224,
        'limits':'Static exact-image contract only; no native execution or effect playback acceptance.'}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('dll',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    result=audit(a.dll);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('PASS exact retail image, material constructor/caller pins and lookup/execute vtable targets; no native execution')
