#!/usr/bin/env python3
"""R146 (non-developer perf): compile the REAL diagnostic_trace::Write body (extracted from src/diagnostic_trace.cpp)
with tests/quiet_write_r146_test.cpp and check it admits exactly the rows the R145 rule admitted. Also checks
that the mod.cpp COMBAT_FLOW block keeps its 5 s clock outside the new quiet gate."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('output must be outside source')
out.mkdir(parents=True,exist_ok=True)
t=(root/'src/diagnostic_trace.cpp').read_text(encoding='utf-8')
s=t.index('void Write(const char* message) noexcept {');e=t.index('void WriteStatus(',s)
body=t[s:e]
assert body.index('gate.quiet(now)')<body.index('receiptDomain(message)'),'quiet must come before the text scan'
assert 'accept(message,now)' in body,'accept must use the same clock read'
(out/'quiet_write_r146_production.inc').write_text(body+'\n',encoding='utf-8')
mod=(root/'src/mod.cpp').read_text(encoding='utf-8')
i=mod.index('R59.16C10R17 COMBAT_FLOW rung=%d')
blk=mod[mod.rindex('static std::uint64_t flowLog59c17=0;',0,i):mod.index('H12 COMBAT_ELIGIBILITY rung=%d',i)+200]
assert 'if(rung>0&&t59c17-flowLog59c17>=5000){flowLog59c17=t59c17;if(!diagnostics::capture168::gate.quiet(t59c17)){' in blk,'5 s clock must stay outside the quiet gate'
assert blk.count('diagnostic_trace::Write(n);')==2 and 'diagnostic_trace::Write(n);}}' in blk
cap=(root/'src/diagnostics/capture168.h').read_text(encoding='utf-8')
assert 'bool quiet(std::uint64_t now) const noexcept' in cap and 'now-closed<=5000' in cap
cxx=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(cxx).name.lower() in ('cl','cl.exe')
exe=out/('quiet_write_r146.exe' if os.name=='nt' else 'quiet_write_r146')
src=str(root/'tests/quiet_write_r146_test.cpp')
args=([cxx,'/nologo','/std:c++20','/EHsc','/O2','/I'+str(root/'src'),'/I'+str(out),src,'/Fe'+str(exe),'/Fo'+str(out/'quiet_write_r146.obj')] if msvc else
      [cxx,'-std=c++20','-O2','-Wall','-Wextra','-Wno-unused-function','-I'+str(root/'src'),'-I'+str(out),src,'-o',str(exe)])
b=subprocess.run(args,capture_output=True,text=True);(out/'build.log').write_text(b.stdout+b.stderr)
files=['src/diagnostic_trace.cpp','src/diagnostics/capture168.h','src/mod.cpp','tests/quiet_write_r146_test.cpp']
receipt={'native_execution':False,'headset':False,'build_exit':b.returncode,'command':args,'inputs':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in files}}
if b.returncode==0:
    r=subprocess.run([str(exe)],capture_output=True,text=True);(out/'run.log').write_text(r.stdout+r.stderr);receipt.update(run_exit=r.returncode,summary=r.stdout+r.stderr);print(r.stdout+r.stderr)
else:print(b.stdout+b.stderr)
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n');raise SystemExit(0 if receipt.get('run_exit')==0 else 1)
