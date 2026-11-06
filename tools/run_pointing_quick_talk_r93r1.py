"""Focused actual adapter/pad, native selector/descriptor and menu preference checks."""
from pathlib import Path
import argparse, hashlib, json, os, re, subprocess, sys
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]
CASES=['normal','dog-low','index-on','setting-off','no-target','held','support','unknown','holster','pickup','corpse','door','near-grab','trigger','inward','behind','too-close','too-low','nonfinite','raise','self-pat','x','choice1','choice2','choice3','choice4','grip-release','tracking-loss','new-held','menu','rearm']
def extracted(text):
 a=text.index('struct GestureFrame59c5');b=text.index('static void ApplyUtilityGestures54',a)
 c=text.index('static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4')
 d=text.index('// M2_SLOT_END 5016',c)
 return text[a:b]+text[c:d]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default='cl');ap.add_argument('--baseline-commit');ap.add_argument('--lua-deps',type=Path);args=ap.parse_args()
 out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True);rows=[]
 def run_build(name,fixture,adapter=None,cases=None,negative=False,sanitized=False):
  folder=out/name;folder.mkdir(exist_ok=True);source=ROOT/'tests'/fixture
  if adapter is not None:
   (folder/'actual_pointing.inc').write_text(adapter,encoding='utf-8')
   source=folder/'wrapper.cpp';source.write_text('#define QUICK_TALK_HOST_INC_H37 "'+(folder/'actual_pointing.inc').as_posix()+'"\n#include "'+(ROOT/'tests'/fixture).as_posix()+'"\n')
  exe=executable_path(folder/'test');cmd=command(source,exe,[ROOT/'tests/syntax_shims',ROOT/'src',ROOT/'tests',ROOT/'tests/openxr_sdk',ROOT/'deps/DetourModKit/include',folder],args.compiler)
  msvc=Path(cmd[0]).name.lower() in ('cl','cl.exe')
  if msvc:
   cmd=[('/Od' if v=='/O2' else '/std:c++latest' if v=='/std:c++20' else v) for v in cmd]
   if sanitized:cmd.insert(1,'/fsanitize=address')
  elif sanitized:cmd[1:1]=['-fsanitize=address,undefined','-fno-omit-frame-pointer']
  result=subprocess.run(cmd,capture_output=True,text=True,cwd=folder);(folder/'build.log').write_text(result.stdout+result.stderr)
  row=dict(name=name,compile_rc=result.returncode,command=cmd,passed=False,runs=[])
  if result.returncode==0:
   for case in (cases if cases else [None]):
    ran=subprocess.run([str(exe)]+([case] if case else []),capture_output=True,text=True,cwd=folder)
    (folder/((case or 'test')+'.log')).write_text(ran.stdout+ran.stderr)
    passed=(ran.returncode==1 and 'FAIL ' in ran.stdout and not ran.stderr.strip()) if negative else ran.returncode==0
    row['runs'].append(dict(case=case,exit=ran.returncode,passed=passed,output=ran.stdout+ran.stderr))
   row['passed']=all(r['passed'] for r in row['runs'])
  rows.append(row);print(name+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
 adapter=extracted((ROOT/'src/subsystems/hud_ui_controls/host.inl').read_text())
 run_build('actual_host','quick_talk_point_r93r1_test.cpp',adapter,CASES)
 run_build('actual_host_asan','quick_talk_point_r93r1_test.cpp',adapter,CASES,sanitized=True)
 if args.baseline_commit:
  old=subprocess.run(['git','show',args.baseline_commit+':src/subsystems/hud_ui_controls/host.inl'],cwd=ROOT,capture_output=True,text=True,check=True).stdout
  run_build('baseline_host_negative','quick_talk_point_r93r1_test.cpp',extracted(old),['normal','index-on','dog-low','raise','self-pat'],True)
 h37=re.search(r'foreach\(case ([^)]+)\)\n add_test\(NAME quick_talk_combat_h37_', (ROOT/'tests/CMakeLists.txt').read_text()).group(1).split()
 run_build('retained_combat_h37','quick_talk_combat_h37_test.cpp',cases=h37)
 for fixture in ['quick_talk_host_h4','quick_talk_arbitration_h4','gesture_talk_policy_d33','gesture_talk59c5','quick_talk_target_d78']:
  run_build(fixture,fixture+'_test.cpp')
 # The release menu uses the real Windows headers, not the native callback shim.
 menu=out/'release_menu';menu.mkdir(exist_ok=True);exe=executable_path(menu/'test');cmd=command(ROOT/'tests/release_menu_r92_test.cpp',exe,[ROOT/'src',ROOT/'deps/DetourModKit/include',ROOT/'tests/openxr_sdk'],args.compiler)
 if Path(cmd[0]).name.lower() in ('cl','cl.exe'):cmd=[('/std:c++latest' if v=='/std:c++20' else '/Od' if v=='/O2' else v) for v in cmd]
 built=subprocess.run(cmd,capture_output=True,text=True,cwd=menu);(menu/'build.log').write_text(built.stdout+built.stderr);row=dict(name='release_menu_same_preference',compile_rc=built.returncode,passed=False)
 if not built.returncode:
  ran=subprocess.run([str(exe)],capture_output=True,text=True);(menu/'test.log').write_text(ran.stdout+ran.stderr);row.update(passed=ran.returncode==0,exit=ran.returncode,output=ran.stdout+ran.stderr)
 rows.append(row);print(row['name']+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
 env=dict(os.environ)
 if args.lua_deps:env['PYTHONPATH']=str(args.lua_deps)+os.pathsep+env.get('PYTHONPATH','')
 lua=subprocess.run([sys.executable,str(ROOT/'tools/test_chat_target_d78.py'),'--out',str(out/'lua_result.json')],capture_output=True,text=True,env=env)
 (out/'lua.log').write_text(lua.stdout+lua.stderr);rows.append(dict(name='actual_lua51_descriptor',passed=lua.returncode==0,exit=lua.returncode,output=lua.stdout+lua.stderr));print('actual_lua51_descriptor: '+('PASS' if lua.returncode==0 else 'FAIL'),flush=True)
 paths=['src/subsystems/hud_ui_controls/host.inl','src/vrik/gesture_talk59c5.h','src/vrik/gesture_talk_policy_d33.h','src/subsystems/interactions/quick_talk_candidate_d78.inl','src/release_menu_r92.h','companion/source/Scripts/Startup/zzkcd2vr_chat_target_d78.lua','src/subsystems/experimental/live_settings_d39.h']
 report=dict(passed=all(r['passed'] for r in rows),results=rows,sha256={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},limits=['Native chat target remains existing head ChatPolicy/LOS selection; pointing is the optional opener, not a new hand-ray target selector.','Native actor/item/tracking/menu observations and action leaves are explicit fixtures; no engine or headset execution.','Baseline negative uses verbatim prior host configuration against current classifier and test leaves.'])
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
