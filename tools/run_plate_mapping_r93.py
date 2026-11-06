"""Focused plate hand-mapping checks. Run in a Visual Studio developer shell on Windows.

python tools/run_plate_mapping_r93.py --compiler cl --out <results-folder>
Host executables only: this never builds or loads the game DLL.
"""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,sys
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]
TESTS=[
 'plate_hand_mapping_r93_test.cpp','plate_hand_presentation_r93_test.cpp',
 'plate_instant_tracking_r93_test.cpp','plate_transfer_tracking_r93_test.cpp',
 'subsystems/alchemy/input67_test.cpp','plate_left_d70_test.cpp','alchemy_tilt_d56_test.cpp',
 'alchemy_tool_presentation_d39_test.cpp','alchemy_tool_contact_d53_test.cpp',
 'instant_mortar_d75_test.cpp','subsystems/alchemy/instant_native_d72_test.cpp',
 'subsystems/alchemy/bridge67_test.cpp']
SOURCES=[
 'src/subsystems/alchemy/input67.h','src/subsystems/alchemy/bridge67.h',
 'src/subsystems/alchemy/instant_native_d72.inl','src/alchemy_transfer_clip_d65.inl',
 'src/vrik/tool_attachment59c10.inl','src/vrik/alchemy_tool_contact_d53.inl']
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def run(cmd,out,log):
 p=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (out/log).write_text(p.stdout,encoding='utf-8');return p

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--compiler',default=os.environ.get('CXX','cl' if os.name=='nt' else 'g++'))
 ap.add_argument('--out',type=Path,required=True);args=ap.parse_args();out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
 gen=subprocess.run([sys.executable,str(ROOT/'tools/extract_instant_mortar_d75.py')],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (out/'generation.txt').write_text(gen.stdout,encoding='utf-8')
 if gen.returncode:print(gen.stdout);return gen.returncode
 provenance=json.loads((ROOT/'tests/generated/instant_mortar_d75_provenance.json').read_text())
 assert provenance['sha256']==sha(ROOT/'src/subsystems/alchemy/instant_native_d72.inl')
 rows=[]
 for name in TESTS:
  src=ROOT/'tests'/name;stem=src.stem;exe=executable_path(out/stem)
  compile_cmd=command(src,exe,[ROOT/'src',ROOT/'tests',ROOT/'tests/syntax_shims',ROOT/'tests/openxr_sdk'],args.compiler)
  p=run(compile_cmd,out,stem+'.compile.txt');row={'test':name,'compile_command':compile_cmd,'compile_exit':p.returncode}
  if p.returncode:row['status']='COMPILE_FAILED';print(p.stdout)
  else:
   result=run([str(exe)],out,stem+'.run.txt');row.update(run_exit=result.returncode,output=result.stdout.strip(),status='PASS' if not result.returncode else 'FAILED')
   print(result.stdout.strip())
  rows.append(row)
 result={'scope':'source-connected host tests; no DLL build, game/native-call execution or headset validation',
  'status':'PASS' if all(r['status']=='PASS' for r in rows) else 'FAILED','compiler':args.compiler,'tests':rows,
  'source_sha256':{s:sha(ROOT/s) for s in SOURCES},'generated_adapter_provenance':provenance,
  'limits':['Initial left plate pickup remains native until its exact right-hand attachment is observed.',
   'Native PlaceBowl still owns transfer semantics, authored particles and sounds. This does not make it instantaneous.',
   'Separate legacy alchemy_transfer_runtime_d65_test has a pre-existing D69 root3 Acquire assertion failure; reproduced on unmodified R92R2 baseline during review. It is not included in this plate-specific runner.']}
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 return 0 if result['status']=='PASS' else 1
if __name__=='__main__':raise SystemExit(main())
