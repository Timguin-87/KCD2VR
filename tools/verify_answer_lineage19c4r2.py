"""Read-only current-state construction evidence for native answer association."""
import argparse,hashlib,json,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
pe=PE(a.dll);checks=[]
def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
add('retail hash',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA);add('timestamp',pe.timestamp,EXPECTED_TS)
for name,rva,expected in [
 ('FOLLOWUP_C current RPG actor',0x173BDA5,'488b4108'),
 ('FOLLOWUP_C current actor state and opponent',0x173BDB5,'488b8810020000488bb1f80c0000'),
 ('defense uses current attacking actor state/type',0xC55C5C,'498b8d10020000498bbd880300004881c110020000'),
 ('native attack type virtual getter',0xC55C7D,'ff5008'),
 ('returned native result is tested',0x173BF58,'84c07425'),
]:add(name,pe.at(rva,len(bytes.fromhex(expected))).hex(),expected)
for name,rva,target in [('defense calls FOLLOWUP_C',0xC55C8F,0x173BD74),('FOLLOWUP_C submits',0x173BF53,0xD568B0)]:
 b=pe.at(rva,5);actual=rva+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1;add(name,hex(actual),hex(target))
result=dict(dll=str(a.dll),passed=sum(x['passed'] for x in checks),total=len(checks),all_passed=all(x['passed'] for x in checks),checks=checks,
 nativeExecution=False,claim='This producer reads current native actor state; association additionally requires the existing exact main-action/clock proof and full release-history match before submit. No new native hook or result override.')
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
print({k:v for k,v in result.items() if k not in ('checks','claim')})
for x in checks:
 if not x['passed']:print('FAIL',x)
raise SystemExit(0 if result['all_passed'] else 1)
