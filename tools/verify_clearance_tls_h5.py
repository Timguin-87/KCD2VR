#!/usr/bin/env python3
"""Read-only exact-retail proof for the T4 capsule-query caller guard.

No native instruction is executed. Direct-call enumeration is a bounded static
witness, not proof that computed indirect invocations cannot exist.
"""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
from verify_native_grasp13 import PE, EXPECTED_SHA, EXPECTED_TS

ROOT=Path(__file__).resolve().parents[1]
PINS={
    0xAF5EF4:'4883ec288b0d926d6e04', # exact read-only getter, same TLS index
    0xAF5EFE:'c744243004000000', # default4
    0xAF5F06:'ff151cdaf002', # TlsGetValue
    0xAF5F24:'8b0410', # read selected TLS/local integer
    0x811877:'4881c1f9170000', # caller-indexed query lock
    0x811884:'498d1c8e',
    0x50B647:'c70100000000', # native guard starts unacquired
    0x50B65C:'be00000100',
    0x50B66D:'f00fb132', # exclusive, not recursive/shared
    0x6EFFE0:'f00fb19ff45f0000', # native Step acquires5FF4
    0x6EFFFD:'f00fb19fdc5f0000', # then5FDC
    0x6F036A:'f001b7dc5f0000', # our hook BEFORE both unlocks
    0x6F0371:'f001b7f45f0000',
    0x6F038B:'f00fb19fdc5f0000', # reacquire before second migration
    0x11E3CA4:'c70104000000', # migration writes old main slot4
    0x11E3CC6:'c70200000000', # new main slot0
}
CALLS={0x81170E:0xAF5EF4,0x811892:0x50B638,
       0x6F00C1:0x11E3C80,0x6F03A5:0x11E3C80}

def verify(path):
    p=PE(path);checks=[]
    def check(name,actual,expected):
        checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    check('DLL SHA-256',hashlib.sha256(p.data).hexdigest(),EXPECTED_SHA)
    check('TimeDateStamp',hex(p.timestamp),hex(EXPECTED_TS))
    check('imagebase',hex(p.base),'0x180000000')
    for rva,expected in PINS.items():
        check(f'native proof {rva:#x}',p.at(rva,len(bytes.fromhex(expected))).hex(),expected)
    for rva,target in CALLS.items():
        b=p.at(rva,5);actual=rva+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1
        check(f'native call {rva:#x}',hex(actual),hex(target))
    check('primitive query slot',hex(p.q(0x407FC80+0x1C8)-p.base),'0x8115bc')
    source=(ROOT/'src/subsystems/physics/runtime/native_weapon_clearance_t4.cpp').read_text()
    # Verify every runtime byte pin against the DLL, including the two newpins.
    arrays={name:bytes(int(x,16) if x.lower().startswith('0x') else int(x)
        for x in re.findall(r'0x[\da-fA-F]+|\b\d+\b',body))
        for name,body in re.findall(r'unsigned char (\w+)\[\]\{([^}]+)\}',source)}
    for rva,name in re.findall(r'Pin\((0x[\da-fA-F]+),(\w+),sizeof \w+\)',source):
        value=arrays[name];check('runtime pin '+name,p.at(int(rva,16),len(value)).hex(),value.hex())
    # Known migration writes occur in the same TimeStep function; the complete
    # native listings provide ordering with the exclusive mutation lock.
    nt=p.u32(0x3c);opt=nt+24
    er,es=struct.unpack('<II',p.data[opt+112+3*8:opt+120+3*8])
    fs=[struct.unpack('<III',p.at(er+i,12)) for i in range(0,es,12)]
    refs=[]
    for va,rs,ro in p.sections:
        if va>=0x3A02000:continue
        data=p.data[ro:ro+rs];pos=0
        while True:
            i=data.find(b'\xe8',pos)
            if i<0 or i+5>len(data):break
            site=va+i;target=site+5+struct.unpack_from('<i',data,i+1)[0]
            if target==0x11E3C80:
                owner=next(((a,b) for a,b,_ in fs if a<=site<b),None)
                if owner:refs.append(dict(site=hex(site),function=[hex(x) for x in owner]))
            pos=i+1
    check('identified direct migration sites',[r['site'] for r in refs],['0x6f00c1','0x6f03a5'])
    check('same TimeStep function',[r['function'][0] for r in refs],['0x6efd50','0x6efd50'])
    return dict(binary=str(path),sha256=hashlib.sha256(p.data).hexdigest(),checks=checks,
        migrationDirectCalls=refs,passed=sum(c['passed'] for c in checks),total=len(checks),
        limitation='Same-current-world known migration calls are serialized by5FDC. No computed-indirect-call or headset-success claim.')

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('dll',type=Path);parser.add_argument('--out',type=Path)
    args=parser.parse_args();result=verify(args.dll)
    if args.out:args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"clearance_tls_h5 native proof {result['passed']}/{result['total']}")
    for c in result['checks']:
        if not c['passed']:print('FAIL',c)
    raise SystemExit(0 if result['passed']==result['total'] else 1)
