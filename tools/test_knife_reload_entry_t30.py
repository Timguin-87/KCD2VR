"""Real DMK owned MID installation + unchanged native knife admission facts."""
from pathlib import Path
import subprocess,re,hashlib,json,argparse
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
BASE='c31b4f70e7191ae14c62f69a5e8907faa1273c4b'
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',required=True,type=Path);ap.add_argument('--dll',required=True,type=Path);a=ap.parse_args()
 out=a.out.resolve();assert ROOT!=out and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 def text(p):return (ROOT/p).read_text()
 def part(t,start,end):return t[t.index(start):t.index(end,t.index(start))]
 reload=text('src/diagnostics/reload_lifecycle178r36c2.inl');sites=text('src/diagnostics/reload_lifecycle_sites178r36c2.inl')
 calls=re.findall(r'\}\}, (\w+)\}',sites);assert len(calls)==16
 pieces={'t30_sites.inc':sites,'t30_callbacks.inc':'\n'.join('void '+n+'(ReloadContext36&) noexcept {}' for n in calls)}
 pieces['t30_owner.inc']=part(reload,'static std::array<std::optional<DetourModKit::hook::Hook>,kReloadSites36.size()> g_reload_hooks36;','static rl36::Ledger59c1').replace('auto h=mid_at(','auto h=TestMidAt(')
 pieces['t30_shutdown.inc']=reload[reload.index('static void ShutdownReloadNative36()'):reload.index('\n}',reload.index('static void ShutdownReloadNative36()'))+2]
 pieces['t30_query.inc']=part(text('src/frik_pose_probe.cpp'),'bool NativeReloadHandRemoveEntryQualifiedT30(','ThrowAuthorityT22 ThrowNativeAuthorityT22(')
 native=text('src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl');paid=text('src/subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl')
 transient=text('src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl')
 pieces['t30_native.inc']=part(native,'static bool AmmoNativePinsT18()','static bool AmmoInputT18(')
 pieces['t30_paidnative.inc']=part(paid,'static bool AmmoPaidNativePinsT20()','static bool AmmoPaidPinsT20()')
 pieces['t30_paid.inc']=part(paid,'static bool AmmoPaidPinsT20()','static bool AmmoPaidFamilyT20(')
 pieces['t30_transient.inc']=part(transient,'static bool AmmoTransientPinsT20()','// These pins qualify only NEW preview')
 for file,key,start,end,name in [('native_ammo_carrier_t18.inl','t30_oldnative.inc','static bool AmmoNativePinsT18()','static bool AmmoInputT18(','AmmoNativePinsT18'),('native_paid_ammo_release_t20.inl','t30_oldpaidnative.inc','static bool AmmoPaidNativePinsT20()','static bool AmmoPaidPinsT20()','AmmoPaidNativePinsT20'),('native_paid_ammo_release_t20.inl','t30_oldpaid.inc','static bool AmmoPaidPinsT20()','static bool AmmoPaidFamilyT20(','AmmoPaidPinsT20')]:
  old=subprocess.check_output(['git','show',BASE+':src/subsystems/throw_drop/runtime/'+file],cwd=ROOT).decode();p=part(old,start,end).replace(name+'()','Old'+name+'()',1)
  if key=='t30_oldpaid.inc':p=p.replace('&&AmmoPaidNativePinsT20()','&&OldAmmoPaidNativePinsT20()')
  pieces[key]=p
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
 libs=' '.join('"E:/backup/DetourModKit/lib/'+s+'.lib"' for s in ['libDetourModKit','safetyhook','Zydis','Zycore'])
 runs={}
 for asan in (False,True):
  name='asan' if asan else 'debug';cmd=out/(name+'.cmd')
  cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /MD /Od /Zi /UNDEBUG '+('/fsanitize=address /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION ' if asan else '')+'/I "'+str(ROOT/'src')+'" /I "'+str(out)+'" /I "E:/backup/DetourModKit/include" "'+str(ROOT/'tests/subsystems/throw_drop/knife_reload_entry_t30_test.cpp')+'" '+libs+' /Fe:'+name+'.exe /link user32.lib advapi32.lib psapi.lib dbghelp.lib\nif errorlevel 1 exit /b %errorlevel%\n'+name+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);log=r.stdout+r.stderr;(out/(name+'.log')).write_text(log);print(name,r.returncode,log[-2400:]);assert r.returncode==0
  runs[name]={'exit':r.returncode,'fixture':json.loads(next(s for s in log.splitlines() if s.startswith('{"checks":')))}
 result={'passed':True,'base':BASE,'runs':runs,'retail_code_pins':len(pins),'fragment_sha256':{n:hashlib.sha256(s.encode()).hexdigest() for n,s in pieces.items()},'actual_owner_factory_leaf':'TestMidAt delegates actual DMK mid_at; failure case at index8 only','game_execution':False,'native_admission_leaves':'factoryhook/mask/retail fingerprint of allocated test image explicit substitutes'}
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
if __name__=='__main__':main()
