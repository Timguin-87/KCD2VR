"""Run the actual draw consumer against split/atomic publication schedules; no DLL."""
from pathlib import Path
import argparse,hashlib,json,subprocess
ROOT=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True)
ap.add_argument('--baseline-source',type=Path,help='Optional complete R93R2 source, for negative control outside Git')
a=ap.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
path='src/subsystems/ranged_reload/bow_string128.inl'
current=(ROOT/path).read_text()
before=None
if a.baseline_source:before=(a.baseline_source/path).read_text()
else:
 try:before=subprocess.check_output(['git','show','5b47eb121f55c7d2c3b60c12a6c73416befe293c:'+path],cwd=ROOT,text=True,stderr=subprocess.DEVNULL)
 except (OSError,subprocess.CalledProcessError):pass
def slice_draw(s,old=False):
 start=s.index('    PhysFrame6698 f{};const auto complete=' if old else '    CommittedFrame6695 body36c1{};',s.index('    if(!drawing){r.drawReceipt48=false;return;}'))
 end=s.index('    std::array<QuatT28,kBowJoints128> local{},model{};',start)
 return s[start:end]
vs=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
rows=[]
cases=[('debug',slice_draw(current),'/Od',False),('optimized',slice_draw(current),'/O2',False),('asan',slice_draw(current),'/O1 /fsanitize=address /Zi',False)]
if before is not None:cases.append(('baseline_negative',slice_draw(before,True),'/O2',True))
else:print('Baseline negative control NOT_RUN: provide --baseline-source or pinned Git history',flush=True)
for name,fragment,flags,negative in cases:
 folder=out/name;folder.mkdir(exist_ok=True);(folder/'bow_draw_consumer_r93.inc').write_text(fragment)
 cmd=folder/'run.cmd';exe=folder/'test.exe'
 cmd.write_text(f'@echo off\ncall "{vs}" >nul\ncl /nologo /std:c++latest /EHsc /DNOMINMAX /utf-8 {flags} /I"{ROOT / "src"}" /I"{folder}" "{ROOT / "tests/bow_draw_publication_r93_test.cpp"}" /Fo"{folder / "test.obj"}" /Fe"{exe}" > "{folder / "compile.log"}" 2>&1\nif errorlevel 1 exit /b 100\n"{exe}" > "{folder / "result.log"}" 2>&1\nexit /b %errorlevel%\n')
 run=subprocess.run(['cmd','/c',str(cmd)],cwd=folder)
 passed=(run.returncode not in [0,100]) if negative else run.returncode==0
 rows.append(dict(name=name,exit=run.returncode,expected_negative=negative,passed=passed));print(name,run.returncode,passed,flush=True)
report={'passed':all(r['passed'] for r in rows),'runs':rows,'baseline_negative_control':'RUN' if before is not None else 'NOT_RUN: R93R2 source unavailable','production_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in [path,'src/vrik/bow_draw_publication_r93.h']},'limits':'Executes extracted current pre-write bow consumer and current producer with native read/pose leaves stubbed; does not render frames, run native animation or validate the headset.'}
(out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')
raise SystemExit(0 if report['passed'] else 1)
