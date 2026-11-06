"""Exact retail proof of D568 evaluation/application branches; no native execution."""
import argparse,hashlib,json,struct
from pathlib import Path
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
pe=PE(a.dll);checks=[]
def add(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
add('exact retail SHA256',hashlib.sha256(pe.data).hexdigest(),EXPECTED_SHA);add('timestamp',pe.timestamp,EXPECTED_TS)
for name,rva,expected in [
 ('collision passes application1',0xD536A7,'c644242801'),
 ('FOLLOWUP_C zero register',0x173BDAC,'4533ed'),
 ('FOLLOWUP_C sixth argument0',0x173BF26,'44886c2428'),
 ('FOLLOWUP_B zero register',0xC622BA,'33db'),
 ('FOLLOWUP_B sixth argument0',0xC6246A,'885c2428'),
 ('synthetic A zero register before submit',0xC613EF,'4533ed'),
 ('synthetic A sixth argument0',0xC6151B,'44886c2428'),
 ('sixth argument branch to evaluation',0xD56EE6,'4438b5d80000000f8485010000'),
 ('application result global dispatch',0xD56FDB,'e8904f6701488b542430488b8830010000488b89b0000000488b01ff10'),
 ('evaluation result flags after shared calculations',0xD57178,'488b5d088a45c488038a45c6884303'),
 ('native candidate request special byte',0x173BE67,'48c744246800010001'),
 ('candidate predicate reads predicted HP delta',0x1E588CD,'f30f5c7330'),
 ('native evaluation false is no answer',0x173BF58,'84c07425'),
 ('result false branch cleanup',0x173BF81,'418add488b4d88'),
 ('RPG getter vft constructor',0x1810434,'488d05254b41024889051e70dd03'),
]:add(name,pe.at(rva,len(bytes.fromhex(expected))).hex(),expected)
for name,rva,target in [
 ('collision submit',0xD536B7,0xD568B0),('evaluation C submit',0x173BF53,0xD568B0),
 ('evaluation B submit',0xC6248A,0xD568B0),('evaluation A submit',0xC6153E,0xD568B0),
 ('application recipient effect1',0xD56F9F,0xD55414),('application recipient effect2',0xD56FC8,0xD55414),
 ('evaluation fills HitInfo',0xD57129,0x725DAC),('evaluation bool result query',0xD5713B,0x7265C4),
 ('evaluation value result query',0xD5714F,0xD55490),('evaluation GUID comparison',0xD57157,0x23CFB18),
 ('candidate predicate after evaluation',0x173BF66,0x1E58890),('candidate selection',0xC55C8F,0x173BD74),
 ('defensive choice calls candidate selection',0x14837AF,0xC55BB4),('later native action dispatch',0x148391F,0x90C850),
]:
 b=pe.at(rva,5);actual=rva+5+struct.unpack('<i',b[1:])[0] if b[0]==0xE8 else -1;add(name,hex(actual),hex(target))
result=dict(passed=sum(c['passed'] for c in checks),total=len(checks),all_passed=all(c['passed'] for c in checks),checks=checks,
 nativeExecution=False,claim='The zero sixth argument calculates candidate results; nonzero executes the application dispatch. Candidate flags precede selection/controller acceptance. No state/flag/result overrides.')
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n')
print({k:v for k,v in result.items() if k not in ('checks','claim')})
for c in checks:
 if not c['passed']:print('FAIL',c)
raise SystemExit(0 if result['all_passed'] else 1)
