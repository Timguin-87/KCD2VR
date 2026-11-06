#!/usr/bin/env python3
"""Read-only exact retail IsUsableByHold ABI/caller/result proof; no native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
ROOT=Path(__file__).resolve().parents[1]
def verify(path):
    pe=PE(path);checks=[]
    def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    add('SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA)
    add('timestamp',hex(pe.timestamp),hex(EXPECTED_TS))
    source=read_source(ROOT/'src/activity_input163.cpp')
    part=source[source.index('bool VerifyAlchemyInput19c4'):source.index('Kind175 ActiveKind175')]
    arrays={n:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',s)) for n,s in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',part)}
    for at,name in re.findall(r'b\+0x([0-9A-Fa-f]+)\),(\w+),sizeof',part):
        value=arrays[name];add('compiled pin '+name,pe.at(int(at,16),len(value)).hex(),value.hex())
    add('current interactor exact primary vft',hex(pe.q(0x3A6A0C8)-pe.base),'0x19d2578')
    col=pe.q(0x3A6A0C8-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0]
    add('current interactor RTTI',pe.cstr(td+16),'.?AVC_PlayerInteractor@entitymodule@wh@@')
    add('native OnUse selected entity kind resolution',pe.at(0xD5AD91,5).hex(),'e8d2e7ffff')
    add('native OnUse selected kind native eligibility and pending verb',pe.at(0xD5ADA6,17).hex(),'8bd3e8e371b7ff84c07406899f88060000')
    # IsUsableByHold native Lua registration -> 8D1E40 (r8 entity ID, r9 type).
    add('IsUsableByHold script registration name',pe.cstr(0x3C18D40),'IsUsableByHold')
    b=pe.at(0x17C0F39,7);add('IsUsableByHold callback',hex(0x17C0F40+struct.unpack('<i',b[3:])[0]),'0x8d1e40')
    add('type argument preserved by script handler',pe.at(0x8D1E56,3).hex(),'418bf9')
    add('component lookup type3',pe.at(0x8D1E68,8).hex(),'41b003e8d0237501')
    add('hold query types bounded20',pe.at(0x8D1E79,3).hex(),'83ff14')
    add('hold query bool = string nonempty',pe.at(0x8D1E97,14).hex(),'488b08403831488bcb0f95442450')
    add('sret argument preserved',pe.at(0x8D1ED5,3).hex(),'488bda')
    add('type17 native remap to5',pe.at(0x8D1ED8,18).hex(),'83f811488d54244841b805000000440f45c0')
    add('action string native getter',pe.at(0x8D1EF9,12).hex(),'488b01488bd3ff90d8000000')
    add('sret returned untouched',pe.at(0x8D1F0F,9).hex(),'488bc34883c4205bc3')
    stage=read_source(ROOT/'src/alchemy_stage19c4.inl')
    arrays={n:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',s)) for n,s in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',stage)}
    for at,name in re.findall(r'MatchBytes\(b\+0x([0-9A-Fa-f]+),(\w+),sizeof',stage):
        value=arrays[name];add('compiled stage pin '+name,pe.at(int(at,16),len(value)).hex(),value.hex())
    col=pe.q(0x3A47948-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0]
    add('child RTTI',pe.cstr(td+16),'.?AVC_AlchemyAnimAction@playermodule@wh@@')
    add('parent RTTI',pe.cstr(struct.unpack('<I',pe.at(pe.q(0x3A5C670-8)-pe.base+12,4))[0]+16),'.?AVC_UseResourceAction@playermodule@wh@@')
    add('child vft constructed',hex(0x6C6D3F+struct.unpack('<i',pe.at(0x6C6D3B,4))[0]),'0x3a47948')
    add('parent same intrusive child passed to queue',pe.at(0x7014A4,12).hex(),'488b4b58488b1f48895c2460')
    add('native queue actual child argument',pe.at(0x6C6C76,21).hex(),'488b13488b084c8b8198000000488bc84883c4205b')
    add('resource attach event',pe.at(0x6C29EE,5).hex(),'3b4168741e')
    add('resource detach event',pe.at(0x6C29F3,5).hex(),'3b416c745b')
    b=pe.at(0x6C2A89,5);add('resource commit after native detach',hex(0x6C2A8E+struct.unpack('<i',b[1:])[0]),'0x6c531c')
    return dict(dll=str(path),native_execution=False,headset_test=False,checks=checks,
        passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
def main():
    p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    result=verify(a.dll);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='checks'},indent=2))
    for c in result['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if result['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
