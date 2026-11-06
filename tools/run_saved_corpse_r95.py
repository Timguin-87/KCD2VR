"""Focused R95 saved-corpse body/part and lifecycle tests (explicit native leaves)."""
from pathlib import Path
import argparse,subprocess,sys,json,os
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default=None);ap.add_argument('--lua-path',type=Path);ap.add_argument('--asan',action='store_true');args=ap.parse_args()
 out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 source=(ROOT/'src/subsystems/gore/native_part_anchor.inl').read_text()
 a=source.index('static bool PartStatus47(');b=source.index('// A NEW post-death contact',a);actual=source[a:b]
 a=source.index('struct AnchorPart47');b=source.index('// H18 successor recontact:',a)
 actual+=source[a:b]+source[source.index('// R95: saved-corpse Grip'):]
 (out/'saved_part_native_r95.inc').write_text(actual)
 subprocess.run([sys.executable,str(ROOT/'tools/extract_corpse_contact_h7.py'),str(out/'corpse_contact_native_h7.inc')],check=True)
 rows=[]
 for name in ['saved_corpse_native_d72','saved_corpse_part_r95','corpse_grip_cg1','corpse_body_owner_d40','part_anchor59c7g','lab58']:
  exe=executable_path(out/name);cmd=command(ROOT/'tests'/f'{name}_test.cpp',exe,[ROOT/'src',ROOT/'tests',out],args.compiler)
  if args.asan:cmd.insert(1,'/fsanitize=address' if Path(cmd[0]).name.lower() in ('cl','cl.exe') else '-fsanitize=address,undefined')
  build=subprocess.run(cmd,capture_output=True,text=True);(out/f'{name}.build.txt').write_text(build.stdout+build.stderr)
  row={'name':name,'command':cmd,'build_rc':build.returncode,'passed':False}
  if not build.returncode:
   run=subprocess.run([str(exe)],capture_output=True,text=True);(out/f'{name}.run.txt').write_text(run.stdout+run.stderr);row.update(run_rc=run.returncode,passed=run.returncode==0)
  rows.append(row);print(name, 'PASS' if row['passed'] else 'FAIL',flush=True)
 env=dict(os.environ)
 if args.lua_path:env['PYTHONPATH']=str(args.lua_path)+os.pathsep+env.get('PYTHONPATH','')
 run=subprocess.run([sys.executable,str(ROOT/'tests/test_saved_corpse_part_r95.py')],capture_output=True,text=True,env=env)
 (out/'lua.run.txt').write_text(run.stdout+run.stderr);rows.append({'name':'actual_lua51','passed':run.returncode==0,'run_rc':run.returncode})
 (out/'results.json').write_text(json.dumps({'scope':'Actual production adapter/helpers, mocked native leaves; actual Lua 5.1; no game/headset execution','results':rows},indent=2)+'\n')
 return 0 if all(r['passed'] for r in rows) else 1
if __name__=='__main__':raise SystemExit(main())
