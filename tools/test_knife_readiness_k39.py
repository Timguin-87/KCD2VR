"""K39 actual native readiness + actual registry census + real owned reload hooks. Native game calls are not executed."""
from pathlib import Path
import subprocess,re,hashlib,json,argparse
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
BASE='7875a7e57c41c1971d2685b3436c7c6177881620'
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',required=True,type=Path);ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--control-permanent-owner-fault',action='store_true');a=ap.parse_args()
 out=a.out.resolve();assert ROOT!=out and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 def text(p):return (ROOT/p).read_text()
 def part(t,start,end):return t[t.index(start):t.index(end,t.index(start))]
 def function(t,start):
  a=t.index(start);b=t.index('{',a);depth=1;i=b+1
  while depth:
   if t[i]=='{':depth+=1
   elif t[i]=='}':depth-=1
   i+=1
  return t[a:i]+'\n'

 reload=text('src/diagnostics/reload_lifecycle178r36c2.inl');sites=text('src/diagnostics/reload_lifecycle_sites178r36c2.inl')
 calls=re.findall(r'\}\}, (\w+)\}',sites);assert len(calls)==16
 pieces={'t30_sites.inc':sites,'t30_callbacks.inc':'\n'.join('void '+n+'(ReloadContext36&) noexcept {}' for n in calls)}
 pieces['t30_owner.inc']=part(reload,'static std::array<std::optional<DetourModKit::hook::Hook>,kReloadSites36.size()> g_reload_hooks36;','static rl36::Ledger59c1').replace('auto h=mid_at(','auto h=TestMidAt(')
 pieces['t30_shutdown.inc']=reload[reload.index('static void ShutdownReloadNative36()'):reload.index('\n}',reload.index('static void ShutdownReloadNative36()'))+2]
 pieces['t30_query.inc']=part(text('src/frik_pose_probe.cpp'),'bool NativeReloadHandRemoveEntryQualifiedT30(','ThrowAuthorityT22 ThrowNativeAuthorityT22(')
 native=text('src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl');paid=text('src/subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl')
 transient=text('src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl')
 pieces['t30_native.inc']=function(native,'static bool AmmoNativePinsT18(')
 pieces['t30_paidnative.inc']=function(paid,'static bool AmmoPaidNativePinsT20(')
 pieces['t30_paid.inc']=function(paid,'static bool AmmoPaidPinsT20(')
 pieces['t30_transient.inc']=function(transient,'static bool AmmoTransientPinsT20(')
 eligibility=text('src/subsystems/throw_drop/runtime/native_ammo_eligibility_t20.inl')
 pieces['k39_census.inc']=part(eligibility,'static std::atomic<bool> g_ammo_masks_ready_t20','static bool AmmoReadT20(')+function(eligibility,'static bool AmmoReadT20(')+function(eligibility,'static bool AmmoMaskLeaseT20(')+function(eligibility,'static void InvalidateAmmoEligibilityT20(')+function(eligibility,'static bool ObserveAmmoMasksT20(')
 service=text('src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl')
 pieces['k39_ready.inc']=part(service,'struct AmmoReadinessK39','static bool AmmoServiceOwnsT22(')
 pieces['k39_service_scan.inc']=part(service,'static std::atomic<bool> g_ammo_hooks_ready22','static bool AmmoServiceInstallT22(')+function(service,'static bool AmmoServiceReadinessWantedT22(')+function(service,'static void AmmoServiceReadinessT22(')
 pieces['k39_session.inc']=function(native,'static std::uint64_t AmmoNoteSessionK39(')
 block=part(service,'    static unsigned noted27=~0u,notes28=0;','    if(!ctx.Current())return;\n    { // K32:')
 splitBegin=block.index('    const unsigned preflight27=');splitEnd=block.index('    if(sessionK39&&notes28<16',splitBegin)
 pieces['k39_notes.inc']=block[:splitBegin]+block[splitEnd:]

 if a.control_permanent_owner_fault:
  original=pieces['k39_service_scan.inc'];mutated=original.replace('ownerFaultK39?rk::Scan::OwnerFault:rk::Scan::Permanent','ownerFaultK39?rk::Scan::Permanent:rk::Scan::Permanent');assert mutated!=original;pieces['k39_service_scan.inc']=mutated
 for n,s in pieces.items():(out/n).write_text(s)
 pe=PE(a.dll);assert hashlib.sha256(pe.data).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 pins={}
 for s in [pieces['t30_native.inc'],pieces['t30_paidnative.inc'],pieces['t30_transient.inc']]:
  for m in re.finditer(r'\{0x([0-9A-Fa-f]+),\{([^}]+)\}\}',s):
   rva=int(m[1],16);data=bytes(int(v.strip(),0) for v in m[2].split(','));assert pe.read(rva,len(data))==data
   if rva in pins:assert pins[rva]==data
   pins[rva]=data
 for m in re.finditer(r'\{0x([0-9A-Fa-f]+), \{\{([^}]+)\}\}',sites):
  rva=int(m[1],16);data=bytes(int(v.strip(),0) for v in m[2].split(','));assert pe.read(rva,len(data))==data
  if rva in pins:assert pins[rva]==data
  pins[rva]=data
 assert len(pins)>=40
 (out/'t30_pins.inc').write_text('\n'.join('{0x%X,{%s}},'%(r,','.join('0x%02x'%b for b in v)) for r,v in pins.items()))
 dmk=ROOT/'deps/DetourModKit'
 libs=' '.join('"'+str(dmk/'lib'/(s+'.lib'))+'"' for s in ['libDetourModKit','safetyhook','Zydis','Zycore'])
 runs={}
 for asan in (False,True):
  name='asan' if asan else 'debug';cmd=out/(name+'.cmd')
  cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /MD /Od /Zi /UNDEBUG '+('/fsanitize=address /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION ' if asan else '')+'/D KCD2VR_AMMO_ELIGIBILITY_T20_HOST_TEST /I "'+str(ROOT/'src')+'" /I "'+str(out)+'" /I "'+str(dmk/'include')+'" "'+str(ROOT/'tests/subsystems/throw_drop/knife_readiness_k39_test.cpp')+'" '+libs+' /Fe:'+name+'.exe /link user32.lib advapi32.lib psapi.lib dbghelp.lib\nif errorlevel 1 exit /b %errorlevel%\n'+name+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);log=r.stdout+r.stderr;(out/(name+'.log')).write_text(log);print(name,r.returncode,log[-2400:]);
  if a.control_permanent_owner_fault:
   assert r.returncode!=0 and 'error C' not in log and 'FAILED check=51' in log
   runs[name]={'expected_failure':True,'exit':r.returncode};continue
  assert r.returncode==0
  runs[name]={'exit':r.returncode,'fixture':json.loads(next(s for s in log.splitlines() if s.startswith('{"checks":')))}
 result={'passed':True,'negative_control':a.control_permanent_owner_fault,'base':'7875a7e57c41c1971d2685b3436c7c6177881620','runs':runs,'retail_code_pins':len(pins),'fragment_sha256':{n:hashlib.sha256(s.encode()).hexdigest() for n,s in pieces.items()},'actual_owner_factory_leaf':'TestMidAt delegates actual DMK mid_at; native game not invoked','game_execution':False,'native_admission_leaves':'actual mask census, actual readiness/retry code; factory/category handle and retail identity substitutes on allocated test image'}
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
if __name__=='__main__':main()
