"""Execute the current production lab Tick/retirement adapter with counted leaves.
No native game code executes. Source extraction ensures the OFF boundary is the
actual adapter, rather than a separately copied model of its policy.
"""
from pathlib import Path
import argparse,subprocess,json,hashlib
arg=argparse.ArgumentParser();arg.add_argument('out',type=Path);arg.add_argument('--asan',action='store_true');args=arg.parse_args()
root=Path(__file__).resolve().parents[1];text=(root/'src/vrik/lab58.inl').read_text()
def body(signature):
 start=text.index(signature);brace=text.index('{',start);depth=1;i=brace+1
 while depth:
  if text[i]=='{':depth+=1
  elif text[i]=='}':depth-=1
  i+=1
 return text[start:i]
prefix=r"""
#include "vrik/lab58.h"
#include <atomic>
#include <cassert>
#include <cstdio>
namespace L=kcd2vr::lab58;
static unsigned watchdogs=0,installs=0,drives=0,clears=0,observers=0,frameReads=0;
static bool saved[2]{};
namespace kcd2vr::corpse_grip_cg1 {
struct Claims {std::atomic<unsigned> livePress[2]{};std::atomic<bool> nativeHeld[2]{};
 void held(int side,bool value){nativeHeld[side]=value;}};
inline Claims claims;
}
static L::DragState g_drag[2];static unsigned g_dragRoot[2]{},g_dragPressCg1[2]{};
struct Pending{bool valid=false;};static Pending g_dragPendingCg1[2];
struct Motor{unsigned incarnation=0;};static Motor g_dragMotorD40[2];
struct Anchor{bool active=false;};static Anchor g_dragAnchorD40[2];
struct Pub{int side=0;struct Current{unsigned tick=0;}current;
 void clear(){++clears;current={};}};static Pub g_dragInputCg1[2];
static bool g_labServiceDirtyD80=false;
static bool SavedDragOwnsD72(int side){return saved[side];}
static void SavedWatchdogD72(std::uint64_t){++watchdogs;if(L::rung!=3)saved[0]=saved[1]=false;}
static void ClearDragOwnerD40(int side){g_dragRoot[side]=g_dragPressCg1[side]=0;g_dragPendingCg1[side]={};
 g_dragMotorD40[side]={};g_dragAnchorD40[side]={};g_dragInputCg1[side].clear();}
static void InstallSavedDragD72(){++installs;}
static void DragTick(std::uint64_t){++drives;}
static void PhysicsDiagnosticsW5(std::uint64_t,int){++observers;}
struct PhysFrame6698{};struct Frame{bool read(PhysFrame6698&){++frameReads;return false;}}g_phys_frame6698;
static void RememberLiving(const PhysFrame6698&){}
static void LivingTick(std::uint64_t){}
"""
checks=r"""
int main(){
 for(int i=0;i<200;++i){Tick(100+i);RetireUnservicedCg1(100+i);}
 assert(!watchdogs&&!installs&&!drives&&!clears&&!observers&&!frameReads);
 L::rung=3;L::f9Rung=3;L::corpseDrag=true;Tick(400);
 assert(watchdogs==1&&installs==1&&drives==1&&g_labServiceDirtyD80);
 // Legacy owned hand/candidate is cleared once at disable; no final velocity.
 g_drag[0].held=true;g_dragRoot[0]=8;g_dragInputCg1[0].current.tick=400;
 kcd2vr::corpse_grip_cg1::claims.livePress[0]=8;kcd2vr::corpse_grip_cg1::claims.nativeHeld[0]=true;
 L::rung=0;L::f9Rung=0;Tick(401);
 assert(!g_drag[0].held&&!g_dragRoot[0]&&!g_dragInputCg1[0].current.tick&&clears==1);
 assert(!kcd2vr::corpse_grip_cg1::claims.livePress[0]&&!kcd2vr::corpse_grip_cg1::claims.nativeHeld[0]);
 const auto previous=watchdogs;
 for(int i=0;i<200;++i)Tick(500+i);
 assert(watchdogs==previous&&clears==1&&drives==1);
 // Native-script ownership must continue its watchdog even after OFF, then stop.
 saved[1]=true;Tick(800);assert(watchdogs==previous+1&&!saved[1]);Tick(801);assert(watchdogs==previous+1);
 L::f9Rung=1;Tick(900);assert(observers==2&&installs==1&&drives==1&&!frameReads);
 std::puts("D80 actual lab adapter: idle no reads/clear/drive, once-only retirement, saved-owner cleanup, observer separation PASS");
}
"""
# A disabled candidate read must exit before tracking/hand/native queries.
read=body('bool ReadCorpseGrabCandidateSideCg1(')
assert read.index('!kcd2vr::lab58::active')<read.index('lab58n::DragInputCurrentCg1')
args.out.mkdir(parents=True,exist_ok=True);suffix='_asan' if args.asan else ''
cpp=args.out/('lab_idle_adapter_d80'+suffix+'.cpp');exe=cpp.with_suffix('.exe')
cpp.write_text(prefix+body('static void RetireUnservicedCg1(')+'\n'+body('static void Tick(')+checks)
cmd=['cl.exe','/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/DNOMINMAX','/I'+str(root/'src'),str(cpp),'/Fo'+str(cpp.with_suffix('.obj')),'/Fe'+str(exe),'/Fd'+str(cpp.with_suffix('.pdb'))]
if args.asan:cmd+=['/Zi','/fsanitize=address']
cmd+=['/link','/INCREMENTAL:NO']
x=subprocess.run(cmd,cwd=args.out,capture_output=True,text=True);log=x.stdout+x.stderr
rec={'source_sha256':hashlib.sha256((root/'src/vrik/lab58.inl').read_bytes()).hexdigest(),'build_exit':x.returncode,'command':cmd}
if x.returncode==0:
 y=subprocess.run([str(exe)],cwd=args.out,capture_output=True,text=True);log+=y.stdout+y.stderr;rec['run_exit']=y.returncode
rec['output']=log;cpp.with_suffix('.json').write_text(json.dumps(rec,indent=2)+'\n');print(log);raise SystemExit(rec.get('run_exit',1))
