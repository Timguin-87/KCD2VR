#!/usr/bin/env python3
"""Read-only T2 VM/map/material native pins and retail projection proof.

No native DLL execution, injection, game process access or installed-file write.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct,xml.etree.ElementTree as ET,zipfile
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS

ROOT=Path(__file__).resolve().parents[1]
def verify(dll,data):
    pe=PE(dll);checks=[]
    def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    def raw(name,rva,expected):add(name,pe.at(rva,len(bytes.fromhex(expected))).hex(),expected)
    def call(name,rva,target):
        b=pe.at(rva,5);actual=rva+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1
        add(name,hex(actual),hex(target))
    add('full exact DLL SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('native timestamp',pe.timestamp,EXPECTED_TS)
    add('exact CEntity GetId vslot08',hex(pe.q(0x4072C78+8)-pe.base),'0x1a76130')
    raw('CEntity GetId reads only uint32 at0C',0x1A76130,'8b410cc3')
    source=read_source(ROOT/'src/native_script_t2.cpp')
    raw('native Lua interpreter state argument',0x714CD5,'488bf1')
    raw('native Lua stack-last/top/TValue16 capacity proof',0x7156AE,'488b4e388bc3482b4e10c1e004489848896e30483bc80f8e2448b101')
    arrays={n:bytes(int(x,0) for x in re.findall(r'0x[0-9a-fA-F]+|\b\d+\b',body)) for n,body in re.findall(r'const unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    for rva,name in re.findall(r'Pin\((0x[0-9A-Fa-f]+),(\w+),sizeof\(\w+\)\)',source):
        add('compiled VM pin '+name+' '+rva,pe.at(int(rva,16),len(arrays[name])).hex(),arrays[name].hex())
    add('CScriptSystem Update exact vslot',hex(pe.q(0x3B8C610+8)-pe.base),'0xa26b94')
    raw('current script VM self+10',0xA26C75,'488b4e10')
    raw('native Lua value stores string tag4 and increments16',0x718314,'488903c74308040000004883471010')
    raw('native Lua string length+10',0x717214,'48397b10')
    raw('native Lua string data+18',0x71721A,'488d5318')
    raw('native positive settop uses current call base+18',0x717AF6,'488b51184c8b49104a8d04c2')
    raw('native type reads value tag+8',0x715E15,'8b4008')
    raw('compass model world XY at50',0x65CDFC,'f20f104350')
    raw('compass vector end68',0x65D256,'4d8b7e68')
    raw('compass vector begin60',0x65D25D,'498b7660')
    raw('current-level waypoint getter',0x2AFEE18,'40534883ec30488bda488d542420')
    call('waypoint uses current-level selection',0x2AFEE26,0xDCB460)
    raw('waypoint selected world XY at50',0x2AFEE35,'488b4050')
    for name,vf,expected in [('compass',0x3C118B8,'.?AVC_UICompass@guimodule@wh@@'),('apse',0x3ED9140,'.?AVC_UIApse@guimodule@wh@@')]:
        col=pe.q(vf-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0];add(name+' RTTI',pe.cstr(td+16),expected)
    # Actual material XML type parser and actual .ui/@instance resolver.
    add('TexType5 native name',pe.cstr(0x40264B0),'Dynamic 2D-Map')
    raw('TexType5 enum branch',0x34C2438,'0fb6ca83f905774a7440')
    raw('TexType material XML attribute',0x45A1CA,'488d1587e45d03')
    add('TexType XML key',pe.cstr(0x3A38658),'TexType')
    call('TexType native textual enum decode',0x21EF7D5,0x34C2D6C)
    raw('TexType supports numeric text through strtol',0x21EF7F4,'448d420aff159a4f8101')
    add('native material UI instance format',pe.cstr(0x3EDB148),'%s@%d.ui')
    add('CFlashUI exact instance resolver slot',hex(pe.q(0x404A7D8+0x90)-pe.base),'0x73d924')
    call('UI instance lookup wrapper',0x73D930,0x73D940)
    call('UI filename/instance parser',0x73D967,0x73D9E4)
    add('native accepted UI extension',pe.cstr(0x3A4BAA0),'ui')
    raw('native parser splits at @',0x73DAB5,'80f940')
    raw('native parser converts instance digits',0x73DAF6,'ff153c6c2c03')
    call('native element-name lookup',0x73D981,0x7F4DF0)
    raw('native explicit instance getter',0x73D9AF,'ff5020')
    add('native sv_map registered key',pe.cstr(0x3DD0128),'sv_map')
    raw('native sv_map string registration name',0xE40496,'488d0d8bfcf802')
    call('native sv_map string registration',0xE4049D,0xB95258)
    raw('native load path finds sv_map',0xB8EFFF,'488d1522112403488b01ff90b8000000')
    raw('native load path sets requested map string',0xB8F00F,'498b5710488bc8488b00488b12ff5048')
    # Native AS2 consumes this same table. Detail suffixes are in the CROPPED
    # page grid; see the independent retail-pixel correlation tool/report.
    with zipfile.ZipFile(data/'Tables.pak') as z:level=z.read('Libs/Tables/level.xml')
    add('retail level.xml hash',hashlib.sha256(level).hexdigest(),'57d93cc1365c04b07ed1f5c146668a75094f0547c6ced67a35e0c0b7fdd7775e')
    r=ET.fromstring(level)
    expected={'trosecko':dict(CompassOffset='0',MapLevelSize='4096,4096',MapWorldOrig='496,704',MapWorldSize='2652,2652',MapTilesSize='3,3',MapTilesCropOrig='0,0',MapTilesCropSize='3,3'),
              'kutnohorsko':dict(CompassOffset='45',MapLevelSize='4096,4096',MapWorldOrig='0,0',MapWorldSize='4096,4096',MapTilesSize='6,6',MapTilesCropOrig='0,1',MapTilesCropSize='6,5')}
    for name,attributes in expected.items():
        row=next(x for x in r.iter('LevelData') if x.get('LevelName')==name)
        for k,v in attributes.items():add(name+' '+k,row.get(k),v)
    return dict(schema=1,native_execution=False,headset_test=False,checks=checks,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--dll',type=Path,required=True);ap.add_argument('--data',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    r=verify(a.dll,a.data);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_bytes((json.dumps(r,indent=2)+'\n').encode())
    print(json.dumps({k:v for k,v in r.items() if k!='checks'}))
    for c in r['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if r['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
