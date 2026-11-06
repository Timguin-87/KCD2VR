"""Actual current-H36 fence/command statements with actual T24 claim policy."""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess
from export_holster_claim_t24 import proposal, ROOT


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--current-root',type=Path,required=True)
    parser.add_argument('--revision',required=True)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args();out=args.out.resolve()
    if out==ROOT or ROOT in out.parents:raise ValueError('external output required')
    out.mkdir(parents=True,exist_ok=True)
    changes=proposal(args.current_root,args.revision)
    source=changes['src/subsystems/throw_drop/holster_gestures.inl'][1]
    fences=source.split('    const bool sharedFenceH36=',1)[1].split('    if(rightFenceH36||leftFenceH36)',1)[0]
    boxing='    const bool boxingHands='+source.split('    const bool boxingHands=',1)[1].split(';',1)[0]+';'
    native=changes['src/vrik/physical_native.inl'][1]
    consumer=native.split('        // T24: a native-empty utility preview still owns this hand.\n',1)[1].split('        if(c.side==0&&',1)[0]
    code=r'''
#include <atomic>
#include <cstdio>
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "vrik/equipment_gesture114.h"
namespace ac=kcd2vr::ammo_carry_t18;
namespace equipment114=kcd2vr::equipment114;
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(33)};
static std::atomic<bool> g_native_held_release_inhibit_t4{false};
struct Input{std::uint64_t bodyEpoch=44,epoch=55;double grip[2]{1,1};};
struct Equipment{bool valid=true,handKnown[2]{true,true};std::uint64_t handKey[2]{};};
struct Output{bool solverOwnedT4[2]{};};
static bool sharedBusy=false,retired=false;
static bool NativeHeldBusyT4(){return sharedBusy;}
static bool PhysLeftHolsterRetiredH36(const Input&,const Equipment&){return retired;}
static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request){
 return kcd2vr::hand_claim_t24::competing(side,{11,22,character,body,request});}
struct Fences{bool right,left,boxing;};
static Fences evaluate(const Input& in,const Equipment& e,const Output& out){
 const bool sharedFenceH36=@FENCES@
 @BOXING@
 return {rightFenceH36,leftFenceH36,boxingHands};
}
namespace ph{enum class CommandKind{Draw,Holster,Other};struct Command{CommandKind kind;int side;};}
static unsigned popped=0;
struct Queue{void pop(ph::Command&){++popped;}}g_phys_commands6698;
static bool nativeAccept(ph::Command c){Equipment e{};e.epoch=44;return false;}
'''.replace('static bool nativeAccept(ph::Command c){Equipment e{};e.epoch=44;return false;}',
'''static bool nativeAccept(ph::Command c){struct {std::uint64_t epoch=44,request=55;}e;
 for(int once=0;once<1;++once){
 @CONSUMER@
 return true;
 }return false;
}''')
    code=code.replace('@FENCES@',fences).replace('@BOXING@',boxing).replace('@CONSUMER@',consumer)
    code+=r'''
static unsigned checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::printf("FAIL %u %s\n",checks,#x);}}while(0)
static void reset(){ac::hands=0;for(auto& c:ac::claims)c.publish({});sharedBusy=false;retired=false;g_native_held_release_inhibit_t4=false;popped=0;}
static void claim(int side,bool foreign=false){ac::claims[side].publish({foreign?111u:11u,foreign?222u:22u,33,77,101,202,44,55,1,1000,9,ac::Stage::Held});ac::hands=1u<<side;}
int main(){Input in;Equipment e;Output out;
 for(int side=0;side<2;++side){reset();claim(side);auto f=evaluate(in,e,out);CHECK(f.right==(side==0));CHECK(f.left==(side==1));CHECK(!f.boxing);
  for(auto kind:{ph::CommandKind::Draw,ph::CommandKind::Holster}){CHECK(!nativeAccept({kind,side}));CHECK(nativeAccept({kind,1-side}));}
  CHECK(nativeAccept({ph::CommandKind::Other,side}));CHECK(e.handKey[0]==0&&e.handKey[1]==0);CHECK(popped==2);
 }
 reset();auto f=evaluate(in,e,out);CHECK(!f.right&&!f.left&&f.boxing);
 sharedBusy=true;f=evaluate(in,e,out);CHECK(f.right&&f.left);sharedBusy=false;
 out.solverOwnedT4[1]=true;f=evaluate(in,e,out);CHECK(f.right&&f.left);out={};
 g_native_held_release_inhibit_t4=true;f=evaluate(in,e,out);CHECK(f.right&&f.left);retired=true;f=evaluate(in,e,out);CHECK(f.right&&!f.left);
 reset();claim(0,true);f=evaluate(in,e,out);CHECK(!f.right&&!f.left);CHECK(nativeAccept({ph::CommandKind::Draw,0}));
 reset();claim(0);++in.epoch;f=evaluate(in,e,out);CHECK(f.right&&!f.left); // old sameactor debt still owns hand
 std::printf("{\"checks\":%u,\"failures\":%u,\"native_execution\":false}\n",checks,failures);return failures?3:0;
}
'''
    src=out/'actual_fences.cpp';src.write_text(code)
    vc='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cmd=out/'run.cmd';cmd.write_text('@echo off\ncall "'+vc+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" actual_fences.cpp /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    run=subprocess.run(['cmd','/c',str(cmd)],capture_output=True,text=True)
    (out/'RUN.log').write_text(run.stdout+run.stderr);print(run.stdout+run.stderr)
    rows=[line for line in run.stdout.splitlines() if line.startswith('{"checks":')]
    result={'passed':run.returncode==0 and bool(rows),'fixture':json.loads(rows[-1]) if rows else None,
      'source_hash':hashlib.sha256(src.read_bytes()).hexdigest(),'revision':args.revision,
      'limits':['Only actual production fence/boxing/queued command statements and actual claim policy are executed.',
                'Native equipment/physics leaves are explicit substitutes. No full native holster completion or headset claim.']}
    (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
    return not result['passed']


if __name__=='__main__':raise SystemExit(main())
