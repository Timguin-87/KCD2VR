"""Source-only D86 elbow / supported-weapon gates. Builds no Windows mod or DLL."""
import argparse,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--baseline',type=Path);p.add_argument('--out',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out or Path(tempfile.mkdtemp(prefix='kcd2vr_vrik_d86_'));out.mkdir(parents=True,exist_ok=True)
def run(name,source,include=root/'src',negative=False):
 exe=out/name
 c=subprocess.run(['g++','-std=c++17','-O2','-I',str(include),'-I',str(root/'src/vrik'),str(source),'-o',str(exe)],capture_output=True,text=True)
 (out/(name+'.compile.txt')).write_text(c.stdout+c.stderr)
 if c.returncode:raise RuntimeError(name+' did not compile: '+c.stderr)
 r=subprocess.run([str(exe)],capture_output=True,text=True)
 (out/(name+'.result.txt')).write_text(r.stdout+r.stderr);print(r.stdout.strip())
 if (r.returncode==0)==negative:raise RuntimeError(name+' returned '+str(r.returncode))
for name in ('elbow_destination_d86','elbow_fresh_swivel_d86','two_hand_damping_d86','tracked_reference_d86'):
 run(name,root/'tests'/(name+'_test.cpp'))
# Compile the ACTUAL production call block, rather than a second handwritten
# integration. Only engine leaves are stand-ins. This catches type/scope errors
# and verifies ordinary/bow paths both reach the new inference.
s=(root/'src/vrik/coherent_pose.inl').read_text();start=s.index('        // D86: one body/hand destination');end=s.index('#include "../subsystems/vrik/pose_arm_solver.inl"',start);block=s[start:end]
bias=(root/'src/subsystems/vrik/pose_torso_stage.inl').read_text();needle='if(natural128&&g_chest_hand_bias128.load';b=bias.index(needle);e=bias.index('{',b);predicate=bias[b+3:e-1]
policy=next(line.strip() for line in s.splitlines() if 'const auto sidePolicyD45=' in line)
stub='''#include "vrik/elbow_destination_d86.h"
#include "vrik/elbow_hysteresis_d69.h"
#include "vrik/archer_pose169r2.h"
#include "diagnostics/vrik_trace_h29.h"
#include <cstdio>
namespace pc=kcd2vr::pose_core;
using V3=pc::Vec3;using Q4=pc::Quat;
struct Bone{V3 p{};Q4 q{};};
V3 PV(V3 p){return p;}V3 FromT(Bone b){return b.p;}Q4 PQ(Q4 q){return q;}Q4 QFromQuatT(Bone b){return b.q;}
V3 QRotateV(Q4 q,V3 p){return pc::rotate(q,p);}V3 Sub(V3 a,V3 b){return a-b;}
struct Rig{int head=0,pelvis=1,spine3=2;};
struct State{double upper[2]{.28,.28},fore[2]{.27,.27};Bone rest[5]{},work[5]{};V3 chestRightLocal{1,0,0},chestUpLocal{0,0,1},hingeNormalLocal[2]{{0,1,0},{0,-1,0}};Q4 boneToAnatomy[2]{};bool anatomyHand[2]{true,true};};
struct Tx{bool traceH29=true,physicalActive=true;std::uint64_t epoch=1;struct {bool bowHand169r2=true;} physicalOutput;struct {struct{bool carry146=false;}weapon[2];}physicalState;kcd2vr::diagnostics::vrik_h29::ArmContext traceArmH29[2];};
bool call(bool bow,bool passenger){
 State s{};Rig rig{};Tx tx{};int side=0,u=2,f=3,h=4;const std::uint64_t sidePolicyD45=17;V3 shoulder{.19,0,1.4},goals[2]{{.15,.15,1.6},{-.2,.45,1.4}},sideDown{.2,-.45,-1};Q4 chest{},handDesired{};
 s.rest[u].p=shoulder;s.rest[f].p={.25,-.1,1.4};s.rest[h].p=goals[0];s.work[0].p={0,0,1.66};s.work[1].p={0,0,.85};s.work[2].p={0,0,1.4};
 const bool ordinaryElbowD67=!bow&&!passenger,bowOwnsReachH17=bow,manualPose169r2=bow;
 const bool elbowHysteresisD69=false,torsoContinuousD67=true,torsoSearchLabD45=false;
 double torsoRadius=.115,archerWeight169r2=1.;auto torsoA_D45=s.work[1].p,torsoB_D45=s.work[2].p;
 std::optional<V3> torsoSeedD45{};pc::ElbowContinuity stagedElbow[2]{};struct {std::uint64_t sequence=1,tick=1000,timeNs=1000000000;}ts;
'''+block+'''
 const auto& basis=tx.traceArmH29[0].basisD86;
 const bool basisCopy=basis.available==!passenger&&(!basis.available||(basis.upper==u&&basis.forearm==f&&basis.hand==h&&basis.elbowPolicy==17&&basis.handConversionKnown&&pc::length(basis.restHand-s.rest[h].p)<1e-15&&pc::length(basis.restForearm-s.rest[f].p)<1e-15&&pc::angle(basis.desiredHandQ,handDesired)<1e-12));
 return basisCopy&&destinationD86.valid==!passenger&&tx.traceArmH29[0].anatomicalD86==!passenger&&pc::finite(torsoPreference);
}
bool biasOn(bool manualPose169r2){bool natural128=true,valid[2]{true,true};float archerYaw127=0;std::atomic<bool> g_chest_hand_bias128{true};return '''+predicate+''';}
std::uint64_t policyFor(bool ordinaryElbowD67,std::uint64_t elbowPolicyD45){bool torsoContinuousD67=ordinaryElbowD67,elbowExperimentD45=false,elbowHysteresisD69=false;'''+policy+'''return sidePolicyD45;}
int main(){bool ok=call(false,false)&&call(true,false)&&call(false,true)&&biasOn(false)&&biasOn(true)&&policyFor(false,17)==policyFor(true,17)&&policyFor(false,18)!=policyFor(false,17);std::printf("production_call_wiring=%s ordinary=checked bow=checked passenger=fallback bow_chest_basis=continuous bend_history=item_independent\\n",ok?"PASS":"FAIL");return ok?0:1;}
'''
f=out/'production_call_d86.cpp';f.write_text(stub);run('production_call_d86',f)
# Corrective D86: the reference receipt must come from the actual forearm,
# not an accidentally copied hand. The real production caller is mutated.
wrongBasis=stub.replace('basis.restForearm=PV(FromT(s.rest[f]));','basis.restForearm=PV(FromT(s.rest[h]));')
assert wrongBasis!=stub
f=out/'production_basis_wrong_reference_mutant.cpp';f.write_text(wrongBasis);run('production_basis_wrong_reference_mutant',f,negative=True)
# Also compile the actual activity-domain assignment from the body producer.
# This protects the call-site gate, not merely the pure tracked-frame helper.
foundation=(root/'src/subsystems/vrik/pose_frame_foundation.inl').read_text()
b=foundation.index('    using ReferenceDomainD86=');e=foundation.index('\n',foundation.index('    fi51.eyeNeutral=',b))
assignment=foundation[b:e]
domain='''#include "vrik/tracked_frame51.h"
#include <cstdio>
std::uint64_t GetTickCount64(){return 1000;}
static bool (*g_frame_neutral51)(std::uint64_t)=+[](std::uint64_t){return true;};
kcd2vr::tracked_frame51::Input assign(int kind){
 struct{bool diceBodyD81=false,ridingGaitD76=false;std::uint64_t ridingModeD76=55;struct{std::uintptr_t instance=111;std::uint64_t lifetime=7;}diceLeaseD81;}tx;
 tx.diceBodyD81=kind==1;tx.ridingGaitD76=kind==2;
 kcd2vr::tracked_frame51::Input fi51{};if(kind==3){fi51.workstationKindH51=1;fi51.workstationInstanceH51=222;fi51.workstationLifetimeH51=8;}
 bool ordinaryFresh51=true,ordinaryFrame51=kind!=4;
'''+assignment+'''
 return fi51;
}
int main(){bool ok=true;for(int i=0;i<=4;++i){auto x=assign(i);ok&=unsigned(x.referenceDomainD86)==unsigned(i)&&x.eyeNeutral==(i==0);}
 auto d=assign(1);ok&=d.referenceInstanceD86==111&&d.referenceLifetimeD86==7;
 std::printf("production_reference_domain_assignment=%s\\n",ok?"PASS":"FAIL");return ok?0:1;}
'''
f=out/'production_reference_domain_d86.cpp';f.write_text(domain);run('production_reference_domain_d86',f)
if a.baseline:
 for name in ('elbow_fresh_swivel_d86','two_hand_damping_d86'):
  run(name+'_baseline_negative',root/'tests'/(name+'_test.cpp'),a.baseline/'src',True)
print('Results: '+str(out))

mut=out/'mutation/vrik';mut.mkdir(parents=True,exist_ok=True)
h=(root/'src/vrik/tracked_frame51.h').read_text()
h=h.replace('        s.last.referenceDomainD86==i.referenceDomainD86&&s.last.referenceInstanceD86==i.referenceInstanceD86&&\n        s.last.referenceLifetimeD86==i.referenceLifetimeD86&&','')
(mut/'tracked_frame51.h').write_text(h)
run('tracked_reference_domain_mutant',root/'tests/tracked_reference_d86_test.cpp',mut.parent,True)
