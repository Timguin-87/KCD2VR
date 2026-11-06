"""Read-only supplied retail console pins and developer Lua gate. No DLL load."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse
import hashlib
import json
import struct

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--native',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
p.add_argument('--out',type=Path,required=True)
a=p.parse_args()
b=a.native.read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0];opt=pe+24
assert b[pe:pe+4]==b'PE\0\0'
sections=[]
start=opt+struct.unpack_from('<H',b,pe+20)[0]
for i in range(struct.unpack_from('<H',b,pe+6)[0]):
    _,va,size,raw=struct.unpack_from('<IIII',b,start+40*i+8)
    sections.append((va,va+size,raw))
def at(rva,size):
    for va,end,raw in sections:
        if va<=rva and rva+size<=end:
            return b[raw+rva-va:raw+rva-va+size]
    raise ValueError(hex(rva))
base=struct.unpack_from('<Q',b,opt+24)[0]
checks=[]
def check(name,ok,**detail):checks.append(dict(name=name,passed=bool(ok),**detail))
check('retail_sha256',hashlib.sha256(b).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7')
check('timestamp',struct.unpack_from('<I',b,pe+8)[0]==0x6A350E20)
check('image_base',base==0x180000000)
for slot,target in [(0xd0,0x8D9348),(0x118,0x79EB88)]:
    actual=struct.unpack('<Q',at(0x3DCF850+slot,8))[0]-base
    check('console_slot_'+hex(slot),actual==target,target=hex(actual))
# Native ExecuteString dispatches r9b=defer to its existing deferred branch.
check('deferred_branch',at(0x79EBB2,9)==bytes.fromhex('4584c90f858380a901'))
# The script branch first prints the text, then calls native IsDevMode.
gate=bytes.fromhex('488b8f00020000488b01ff90f803000084c0745a')
check('lua_developer_mode_gate',at(0x79F10E,len(gate))==gate,rva='0x79f10e')
source=read_source(Path(__file__).resolve().parents[1]/'src/mod.cpp')
s=source.index('bool SafeConsoleExecuteStringCandidate(');e=source.index('// C43D:',s);body=source[s:e]
check('no_guessed_print_call','vt[0xD0' not in body and 'vt[0xd0' not in body)
check('full_retail_guard','retail_build112::Ready(base)' in body)
check('script_prefix_refused',"*text=='#'||*text=='@'" in body)
result={'kind':'READ_ONLY_RETAIL_CONSOLE_AUDIT','native_execution':False,'checks':checks,
        'passed':sum(c['passed'] for c in checks),'total':len(checks),
        'limit':'Correct ExecuteString is not proof of retail UI Lua execution. No game or script executed.'}
a.out.write_text(json.dumps(result,indent=2)+'\n')
print(f"console audit {result['passed']}/{result['total']}")
raise SystemExit(0 if result['passed']==result['total'] else 1)
