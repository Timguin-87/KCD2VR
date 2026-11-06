#!/usr/bin/env python3
"""Read-only exact-retail proofs for the C37 smithing tool reader (src/vrik/smith_tool36.inl).

Every match()/callTo()/slot() pin compiled into SmithToolPins36 is re-checked
against WHGame.dll, plus RTTI class names, the hand/entity attachment name
tables and the 'tongs'/'hammer' factory strings. Static only: no game run.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
ROOT=Path(__file__).resolve().parents[1]
def rtti(pe,vft):
    col=pe.q(vft-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0];return pe.cstr(td+16)
def verify(path):
    pe=PE(path);checks=[]
    def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    add('SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('timestamp',hex(pe.timestamp),hex(EXPECTED_TS))
    source=read_source(ROOT/'src/vrik/smith_tool36.inl')
    arrays={n:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',s)) for n,s in
            re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    for at,name in re.findall(r'match\(base\+0x([0-9A-Fa-f]+),(\w+)\)',source):
        b=arrays[name];add('compiled pin '+name,pe.at(int(at,16),len(b)).hex(),b.hex())
    for at,target in re.findall(r'callTo\(base\+0x([0-9A-Fa-f]+),base\+0x([0-9A-Fa-f]+)\)',source):
        a=int(at,16);b=pe.at(a,5);got=a+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1
        add('compiled call '+at,hex(got),hex(int(target,16)))
    for vft,index,target in re.findall(r'slot\(base\+0x([0-9A-Fa-f]+),(0x[0-9A-Fa-f]+|\d+),base\+0x([0-9A-Fa-f]+)\)',source):
        add(f'compiled slot {vft}[{index}]',hex(pe.q(int(vft,16)+8*int(index,0))-pe.base),hex(int(target,16)))
    for vft,name in [(0x3A6A390,'.?AVC_Blacksmithing@playermodule@wh@@'),(0x3A6A320,'.?AVC_Blacksmithing@playermodule@wh@@'),
                     (0x3A6A2C8,'.?AVC_Blacksmithing@playermodule@wh@@'),
                     (0x3A7BA48,'.?AVC_BlacksmithingSlotToolAttachment@playermodule@wh@@'),
                     (0x3A7BAA8,'.?AVC_BlacksmithingToolAttachment@playermodule@wh@@'),
                     (0x3A7BA18,'.?AVC_BlacksmithingWorkpieceAttachment@playermodule@wh@@'),
                     (0x3A68F68,'.?AUCEntityAttachment@@')]:
        add('RTTI '+hex(vft),rtti(pe,vft),name)
    for i,name in enumerate(['entity_right','entity_left','hand_right','hand_left']):
        add('attachment name '+name,pe.cstr(pe.q(0x40AC298+8*i)-pe.base),name)
    add('tongs factory string',pe.cstr(0xB67890+struct.unpack('<i',pe.at(0xB6788C,4))[0]),'tongs')
    add('hammer factory string',pe.cstr(0xB67938+struct.unpack('<i',pe.at(0xB67934,4))[0]),'hammer')
    add('forgeBag factory string',pe.cstr(0xB677E8+struct.unpack('<i',pe.at(0xB677E4,4))[0]),'forgeBag')
    add('workpiece assetslot string',pe.cstr(0xB666A4+struct.unpack('<i',pe.at(0xB666A0,4))[0]),'Assetslot')
    # C37 alchemy stage receipts decode the director's live action by exact RTTI.
    header=read_source(ROOT/'src/vrik/alchemy_input19c4.h')
    for rva,name in re.findall(r'case 0x([0-9A-Fa-f]+):return "(\w+)"',header):
        add('alchemy action class '+name,rtti(pe,int(rva,16)),'.?AVC_'+name+'Action@playermodule@wh@@')
    add('alchemy director class',rtti(pe,0x3BFA5B0),'.?AVC_AlchemyActionDirector@playermodule@wh@@')
    add('smithing start calls C_Minigame::Start',hex(0xB6753A+struct.unpack('<i',pe.at(0xB67536,4))[0]),hex(0x9F0970))
    return dict(dll=str(path),native_execution=False,headset_test=False,checks=checks,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
def main():
    a=argparse.ArgumentParser();a.add_argument('--dll',type=Path,required=True);a.add_argument('--out',type=Path,required=True);args=a.parse_args()
    result=verify(args.dll);args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='checks'},indent=2))
    for c in result['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if result['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
