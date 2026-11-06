"""Current source native rollback/event and Lua handover; no game/PAK build."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,sys
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default=None);ap.add_argument('--lua-deps',type=Path,required=True);args=ap.parse_args()
 out=args.out.resolve();out.mkdir(parents=True,exist_ok=True);rows=[]
 text=(ROOT/'src/subsystems/gore/native_gore.inl').read_text();a=text.index('    if(GoreEvent50(e))(void)g_limb_cut_ledger60');b=text.index('\n}\nstatic kcd2vr::gore_cut_d56',a)
 (out/'actual_limb_handover_r93.inc').write_text(text[a:b])
 emitted=out/'native_limb.lua'
 for fixture in ['gore_handover_r93_test','gore32_test','release_menu_r92_test','gore_lifecycle_d52_test']:
  for asan in ([False,True] if fixture=='gore_handover_r93_test' else [False]):
   name=fixture+('_asan' if asan else '');exe=executable_path(out/name);cmd=command(ROOT/'tests'/f'{fixture}.cpp',exe,[ROOT/'src',ROOT/'tests',out],args.compiler)
   if asan:cmd.insert(1,'/fsanitize=address')
   p=subprocess.run(cmd,capture_output=True,text=True);(out/f'{name}.build.log').write_text(p.stdout+p.stderr)
   row={'name':name,'compile_rc':p.returncode,'passed':False}
   if p.returncode==0:
    p=subprocess.run([str(exe),str(emitted)],capture_output=True,text=True);(out/f'{name}.run.log').write_text(p.stdout+p.stderr)
    row.update(run_rc=p.returncode,summary=p.stdout.strip(),passed=p.returncode==0)
   rows.append(row);print(name,row['passed'],flush=True)
 env=os.environ.copy();env['PYTHONPATH']=str(args.lua_deps);env['R93_GORE_CHUNK']=str(emitted)
 for fixture in ['test_gore_handover_r93','test_gore_d52_lua','test_gore_eligibility_d56','test_gore_d73','test_optional_gore_r93']:
  p=subprocess.run([sys.executable,str(ROOT/'tests'/f'{fixture}.py')],env=env,capture_output=True,text=True)
  (out/f'{fixture}.log').write_text(p.stdout+p.stderr);rows.append({'name':fixture,'run_rc':p.returncode,'passed':p.returncode==0,'summary':(p.stdout+p.stderr).strip()});print(fixture,p.returncode==0,flush=True)
 paths=['src/vrik/gorepak50.h','src/subsystems/gore/native_gore.inl','src/subsystems/gore/gore_policy.h','mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua','mods/KCD2VRGore/r93r3/D77_DONOR.json','tools/optional_gore_r93.py','tools/package_release_helpers_r19.py']
 report={'passed':all(r['passed'] for r in rows),'results':rows,'sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},'limits':['Native attachment calls are leaves; no game or headset claim.','No real optional PAK built; packaging tests use tiny synthetic assets.','Current install optional package identity unknown.']}
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
