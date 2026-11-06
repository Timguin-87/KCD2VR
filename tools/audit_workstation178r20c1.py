#!/usr/bin/env python3
"""Read-only exact-retail proof for the R20C1 smithing observer. No dependencies."""
import argparse, hashlib, json, struct
from pathlib import Path

def audit(path):
    data=Path(path).read_bytes()
    digest=hashlib.sha256(data).hexdigest()
    assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7', 'unsupported retail SHA-256'
    u16=lambda off:struct.unpack_from('<H',data,off)[0]
    u32=lambda off:struct.unpack_from('<I',data,off)[0]
    u64=lambda off:struct.unpack_from('<Q',data,off)[0]
    pe=u32(0x3c);assert data[pe:pe+4]==b'PE\0\0'
    assert u32(pe+8)==0x6A350E20
    optional=pe+24;base=u64(optional+24);sections=optional+u16(pe+20)
    table=[]
    for i in range(u16(pe+6)):
        s=sections+i*40;table.append((u32(s+12),u32(s+16),u32(s+20)))
    def read(rva,size):
        for va,rawsize,off in table:
            if va<=rva and rva+size<=va+rawsize:return data[off+rva-va:off+rva-va+size]
        raise AssertionError('RVA outside file-backed section: '+hex(rva))
    pins={
        0x858188:'48 89 5c 24 10 57 48 83 ec 20 48 8b d9 33 d2',
        0x85786C:'e9 17 09 00 00',
        0xB67DB5:'48 89 51 08',
        0xB67DB9:'48 8d 05 c0 fa 3f 03',
        0xB67E55:'48 8d 05 10 fa ce ff',
        0x9EF3AF:'48 89 43 68',
        0x9EF464:'48 89 93 98 00 00 00',
        0x72DD20:'48 8b 41 30 c3',
        0x9EEAB4:'48 8d 05 c5 b7 07 03',
        0x9EEB60:'48 8d 05 a1 b6 07 03',
        0x9EEB75:'48 89 86 80 00 00 00',
        0x9EEB83:'44 88 96 88 00 00 00',
        0x1A7DE10:'0f b6 41 08 c3',
        0x8581CC:'3c 0c 0f 84 de 00 00 00',
        0x8581DD:'3c 0d 0f 84 cd 00 00 00',
        0x858240:'e8 ab 0f 00 00',
        0x858249:'48 8d 15 a8 a9 ad 04',
        0x859281:'c7 45 f0 0b 0c 0d 0e',
        0x859294:'66 c7 45 f4 10 11',
        0x8593D2:'3c 08 0f 94 c0',
    }
    for rva,value in pins.items():
        expected=bytes.fromhex(value);assert read(rva,len(expected))==expected,hex(rva)
    for slot,fn in [(0x3A6A380,0x72DD20),(0x3A6A210,0x1A7DE10)]:
        assert struct.unpack('<Q',read(slot,8))[0]==base+fn
    # MSVC RTTI: same C_Blacksmithing descriptor, secondary subobject offset 0x68.
    primary_col=struct.unpack('<Q',read(0x3A6A388,8))[0]-base
    secondary_col=struct.unpack('<Q',read(0x3A6A318,8))[0]-base
    assert struct.unpack('<I',read(primary_col+4,4))[0]==0
    assert struct.unpack('<I',read(secondary_col+4,4))[0]==0x68
    assert read(primary_col+12,4)==read(secondary_col+12,4)
    spans={'native_map_refresh':(0x858188,0x858321),'is_anvil':(0x8591F0,0x8592C7),
           'smith_constructor':(0x9EF36C,0x9EF561),'context_constructor':(0x9EEAA0,0x9EEDE8),
           'listener_constructor':(0xB67D90,0xB68130)}
    return {'file_sha256':digest,'timestamp':'0x6A350E20','byte_pins':len(pins),'vtable_pins':2,
            'rtti_secondary_offset':'0x68','native_execution':False,
            'layout':{'listener_vft':'0x3F67880','listener_owner':'+8 = smith+0x68',
                      'smith_context':'+0x98','context_vft':'0x3A6A280','phase_vft':'0x3A6A208',
                      'context_phase':'+0x88'},
            'anvil_map_phases':[11,14,16,17],'inspection_override_phases':[12,13],'forge_phase':8,
            'spans':{name:{'start':hex(lo),'end':hex(hi),'sha256':hashlib.sha256(read(lo,hi-lo)).hexdigest()} for name,(lo,hi) in spans.items()}}
if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('dll');ap.add_argument('--out',required=True);args=ap.parse_args()
    result=audit(args.dll);Path(args.out).write_text(json.dumps(result,indent=2)+'\n')
    print('PASS exact retail SHA/timestamp, 20 instruction pins, 2 vtable getters and RTTI owner offset; no native execution')
