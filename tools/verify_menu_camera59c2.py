#!/usr/bin/env python3
"""Reproduce the R59C2 retail-camera contract, read-only. No SDK vtable imports."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--dll',required=True);ap.add_argument('--out',required=True);args=ap.parse_args()
    b=Path(args.dll).read_bytes();sha=hashlib.sha256(b).hexdigest()
    assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    pe=struct.unpack_from('<I',b,0x3c)[0];assert b[pe:pe+4]==b'PE\0\0'
    count=struct.unpack_from('<H',b,pe+6)[0];stamp=struct.unpack_from('<I',b,pe+8)[0]
    assert stamp==0x6A350E20
    opt=pe+24;assert struct.unpack_from('<H',b,opt)[0]==0x20b
    base=struct.unpack_from('<Q',b,opt+24)[0];table=opt+struct.unpack_from('<H',b,pe+20)[0]
    sections=[struct.unpack_from('<IIII',b,table+40*i+8) for i in range(count)]
    def read(rva,n):
        for vs,va,rs,raw in sections:
            if va<=rva and rva+n<=va+rs:return b[raw+rva-va:raw+rva-va+n]
        raise ValueError(hex(rva))
    def u32(at):return struct.unpack('<I',read(at,4))[0]
    def ptr(at):return struct.unpack('<Q',read(at,8))[0]-base
    def rtti(vtable,expected,offset):
        col=ptr(vtable-8);values=struct.unpack('<6I',read(col,24))
        assert values[0]==1 and values[1]==offset and values[5]==col
        name=read(values[3]+16,128).split(b'\0')[0].decode();assert name==expected
        return {'vtable':hex(vtable),'COL':hex(col),'subobject_offset':offset,'class':name}
    source=read_source(Path(__file__).resolve().parents[1]/'src/ui_menu_camera59c2.cpp')
    arrays={name:bytes(int(x,16) for x in re.findall(r'0x([0-9a-f]+)',body))
            for name,body in re.findall(r'constexpr unsigned char (\w+)\[\]=\{([^}]+)\}',source)}
    pins={}
    for name,rva in [('r',0x9E5314),('a',0x14E40A0),('s',0x1630B60),('call',0x9E53BC)]:
        data=arrays[name];assert read(rva,len(data))==data,(name,hex(rva));pins[hex(rva)]=data.hex()
    assert ptr(0x3A5A288)==0x9E5314
    assert [ptr(0x3B8F420),ptr(0x3B8F428)]==[0x14E40A0,0x9E5BE8]
    assert [ptr(0x3B8F498),ptr(0x3B8F4A0)]==[0x1630B60,0x15A1020]
    assert read(0x9E5371,7)==bytes.fromhex('ff501084c0747a') # active false -> native fallback
    assert read(0x9E53BC,3)==bytes.fromhex('ff5008') # selected camera output only on true
    result={'sha256':sha,'timestamp':hex(stamp),'pins':pins,'RTTI':[
        rtti(0x3A5A280,'.?AVC_CameraUI@game@wh@@',0),
        rtti(0x3B8F418,'.?AVC_UIApseCamera@guimodule@wh@@',16),
        rtti(0x3B8F490,'.?AVC_UIShopCamera@guimodule@wh@@',16)],
        'functions':{hex(a):{'end_exclusive':hex(z),'sha256':hashlib.sha256(read(a,z-a)).hexdigest(),'hex':read(a,z-a).hex()}
                     for a,z in [(0x9E5314,0x9E540C),(0x14E40A0,0x14E40DF),(0x1630B60,0x1630B9F)]},
        'native_execution':False,'camera_to_present_latency_measured':False}
    out=Path(args.out);out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(result,indent=2)+'\n');print('PASS: SHA, timestamp, live pins, retail RTTI and active->output consumer')
if __name__=='__main__':main()
