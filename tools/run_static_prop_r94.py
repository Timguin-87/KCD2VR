"""Exercise actual small-prop native adapter with only the fixed GetProxy leaf modeled."""
from pathlib import Path
import argparse,subprocess,tempfile,json,hashlib,os
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path);ap.add_argument('--compiler',default='cl'if os.name=='nt'else'g++');a=ap.parse_args()
 out=a.out.resolve()if a.out else Path(tempfile.mkdtemp(prefix='kcd2vr_static_r94_'));out.mkdir(parents=True,exist_ok=True)
 source=(ROOT/'src/vrik/static_grab_r89.inl').read_text();contact=(ROOT/'src/vrik/lab_prop58.inl').read_text()
 same=contact[contact.index('static bool SameSmallContactR24('):contact.index('// Normal small props')]
 weapon=contact[contact.index('static bool SmallWeaponEvidenceR94('):contact.index('static bool SmallContactSourceR24(')]
 prepare=contact[contact.index('static bool PrepareWeaponContactR94('):contact.index('static void SmallContactTickR24(')]
 leaf='reinterpret_cast<void*(__fastcall*)(void*,int)>(WhBase()+0x5D70C4)(entity,1)';assert source.count(leaf)==1
 adapter=out/'actual_static_r94.inc';adapter.write_text(source.replace(leaf,'TestGetProxyR94(entity,1)'))
 contact_adapter=out/'actual_weapon_contact_r94.inc';contact_adapter.write_text(same+weapon+prepare)
 rows=[]
 for name in ['static_grab_native_r91','static_grab_r89','brush_swap_r100']: # R100: brush -> loose entity swap rules
  for mode in ['debug','optimized']:
   d=out/(name+'_'+mode);d.mkdir(exist_ok=True);exe=executable_path(d/'test')
   cmd=command(ROOT/'tests'/(name+'_test.cpp'),exe,[ROOT/'src'],a.compiler)
   if a.compiler.lower()in('cl','cl.exe'):
    if mode=='debug':cmd=[('/Od'if x=='/O2'else x)for x in cmd]
    cmd[1:1]=['/DEXCEPTION_EXECUTE_HANDLER=1','/DKCD2VR_STATIC_R94_ADAPTER="'+adapter.as_posix()+'"','/DKCD2VR_STATIC_R94_CONTACT_ADAPTER="'+contact_adapter.as_posix()+'"']
   else:cmd[1:1]=['-DKCD2VR_STATIC_R94_ADAPTER="'+adapter.as_posix()+'"','-DKCD2VR_STATIC_R94_CONTACT_ADAPTER="'+contact_adapter.as_posix()+'"']
   b=subprocess.run(cmd,cwd=d,capture_output=True,text=True);(d/'build.log').write_text(b.stdout+b.stderr);row={'test':name+'_'+mode,'compile_exit':b.returncode,'passed':False}
   if not b.returncode:
    r=subprocess.run([str(exe)],capture_output=True,text=True);(d/'run.log').write_text(r.stdout+r.stderr);row.update(exit=r.returncode,output=r.stdout,passed=r.returncode==0)
   print(row,flush=True);rows.append(row)
 (out/'RESULT.json').write_text(json.dumps({'passed':all(r['passed']for r in rows),'tests':rows,'source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest()for p in ['src/vrik/static_grab_r89.inl','src/vrik/static_grab_r89.h','src/vrik/lab_prop58.inl','src/vrik/brush_swap_r100.h']},'limits':['Native memory/GetProxy/script/physical simulation are modeled leaves; no engine or headset execution.']},indent=2))
 return 0 if all(r['passed']for r in rows)else 1
if __name__=='__main__':raise SystemExit(main())
