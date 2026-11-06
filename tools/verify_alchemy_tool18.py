#!/usr/bin/env python3
"""Read-only exact-retail proofs for the native alchemy entity attachment bridge."""
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
    source=read_source(ROOT/'src/vrik/alchemy_tool18.inl')
    arrays={n:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',s)) for n,s in
            re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    for at,name in re.findall(r'match\(base\+0x([0-9A-Fa-f]+),(\w+)\)',source):
        b=arrays[name];add('compiled pin '+name,pe.at(int(at,16),len(b)).hex(),b.hex())
    add('attachment name getter slot',hex(pe.q(0x3A657C0+0x10)-pe.base),'0x638480')
    add('current attachment object getter slot',hex(pe.q(0x3A657C0+0xE0)-pe.base),'0x19a2a90')
    for side,name in enumerate(['entity_right','entity_left']):add('alchemy attachment '+str(side),pe.cstr(pe.q(0x40AC298+8*side)-pe.base),name)
    # Returns the first held hand type unless empty (21), then the other hand.
    add('native first nonempty hand-type getter',pe.at(0x8D237C,18).hex(),'8b81a002000083f81575068b81a4020000c3')
    for vft in (0x3A5C670,0x3F60770,0x3A5CCD0):
        add('action started getter '+hex(vft),hex(pe.q(vft+0x20)-pe.base),'0x1a731d0')
    add('entity attachment placement slot',hex(pe.q(0x3A68F68+0x10)-pe.base),'0x4238b8')
    col=pe.q(0x3A68F68-8)-pe.base;td=struct.unpack('<I',pe.at(col+12,4))[0]
    add('entity attachment RTTI',pe.cstr(td+16),'.?AUCEntityAttachment@@')
    add('owner argument adjustment',pe.at(0x2E19628,7).hex(),'4881c108010000')
    for name,site,target in [('alchemy entity attach',0x2E19619,0x1085234),('publish native hand type',0x2E1963E,0x6C677C),('native alchemy detach',0x15FC2BB,0x10853CC)]:
        b=pe.at(site,5);got=site+5+struct.unpack('<i',b[1:])[0] if b[0] in (0xE8,0xE9) else -1
        add(name,hex(got),hex(target))
    return dict(dll=str(path),native_execution=False,headset_test=False,checks=checks,passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks))
def main():
    a=argparse.ArgumentParser();a.add_argument('--dll',type=Path,required=True);a.add_argument('--out',type=Path,required=True);args=a.parse_args()
    result=verify(args.dll);args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='checks'},indent=2))
    for c in result['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if result['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
