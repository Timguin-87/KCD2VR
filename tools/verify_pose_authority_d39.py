#!/usr/bin/env python3
"""Compile the actual production shoulder-admission lambda and fixed-length IK.

Usage: python tools/verify_pose_authority_d39.py [--source SOURCE] [--output DIR]
An immutable D38 source tree is a negative control: its 15 cm cap fails these
endpoint checks. This fixture stubs only model-pose storage/composition; it does
not copy or replace the shoulder-admission or arm-solver math.
"""
from pathlib import Path
import argparse
import subprocess
import tempfile

p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
p.add_argument("--output", type=Path)
args = p.parse_args()
source = args.source.resolve()
output = args.output or Path(tempfile.mkdtemp(prefix="kcd2vr_pose_authority_d39_"))
output.mkdir(parents=True, exist_ok=True)
text = (source / "src/subsystems/vrik/pose_reach_input.inl").read_text()
begin = text.index("    V3 gestureShouldersR38[2]")
end = text.index("    V3 modelIntentGoalsH30", begin)
lambda_text = text[begin:end]
physical = (source / "src/vrik/physical_weapon.h").read_text()
reach = next(line for line in physical.splitlines() if line.startswith("inline double reach_max("))
code = r'''
#include "vrik/upper_body.h"
#include "vrik/frik_elbow_lab.h"
#include "vrik/shoulder_reach_d38.h"
#include <cstdio>
namespace pc=kcd2vr::pose_core;
namespace ph { REACH_FUNCTION }
struct V3 {float x{},y{},z{};};
static pc::Vec3 PV(V3 v){return {v.x,v.y,v.z};}
static V3 VP(pc::Vec3 v){return {float(v.x),float(v.y),float(v.z)};}
static V3 Sub(V3 a,V3 b){return VP(PV(a)-PV(b));}
static float Len(V3 a){return float(pc::length(PV(a)));}
static V3 FromT(V3 a){return a;}
struct State {V3 work[4]{};double upper[2]{.2663,.2663},fore[2]{.2615,.2615};};
static void SetWorldPosition128(State& s,int i,V3 p){s.work[i]=p;}
static bool Compose6689(State&){return true;}
static int checks=0,failures=0;
static double maxError=0,maxUpper=0,maxFore=0;
static void check(bool b,const char* message){++checks;if(!b){++failures;if(failures<8)std::printf("FAIL %s\n",message);}}
static void run(double reach,int side,bool elbowLab,bool physicalAdmission,pc::Vec3 axis){
    State s{};const struct {int upper_r=0,upper_l=1,hand_r=2,hand_l=3;} rig;
    const struct {std::uint64_t epoch=4;} tx;
    const struct {std::uint64_t sequence=123,tick=4000;} ts;
    pc::ElbowContinuity stagedElbow[2]{};
    s.work[0]={.249f,.0436f,1.3413f};s.work[1]={-.249f,.0436f,1.3413f};
    s.work[2]=VP(PV(s.work[0])+pc::Vec3{0,0,-.5});s.work[3]=VP(PV(s.work[1])+pc::Vec3{0,0,-.5});
    const auto goal=VP(PV(s.work[side])+axis*reach);
PRODUCTION_LAMBDA
    const bool admitted=accommodateShoulderR38(side,goal,elbowLab,physicalAdmission);
    check(admitted,"actual production shoulder admission succeeded");
    const auto root=PV(s.work[side]);
    const auto axisNow=pc::unit(PV(goal)-root).value_or(pc::Vec3{0,0,-1});
    const auto pole=pc::unit(pc::cross(axisNow,std::abs(axisNow.z)<.8?pc::Vec3{0,0,1}:pc::Vec3{0,1,0})).value();
    const double reserve=elbowLab?kcd2vr::frik_elbow_lab::clamp_triangle(reach,s.upper[side],s.fore[side]).reserve:-1.;
    const auto arm=pc::solve_arm(root,PV(goal),s.upper[side],s.fore[side],pole,pole,reserve);
    check(bool(arm),"actual production arm solver succeeded");if(!arm)return;
    const double error=pc::length(arm->hand-PV(goal));maxError=std::max(maxError,error);
    maxUpper=std::max(maxUpper,std::abs(pc::length(arm->elbow-root)-s.upper[side]));
    maxFore=std::max(maxFore,std::abs(pc::length(arm->hand-arm->elbow)-s.fore[side]));
    check(error<.000002,"admitted wrist is authoritative after actual production shoulder lambda and arm solver");
    check(maxUpper<1e-8&&maxFore<1e-8,"upper/forearm lengths unchanged");
}
int main(){
    for(int warning=0;warning<3;++warning){kcd2vr::shoulder_reach_d38::maxShiftM.store(warning==0?.02f:warning==1?.15f:.4f);
        for(int side=0;side<2;++side)for(int lab=0;lab<2;++lab)for(int physical=0;physical<2;++physical)
            for(double reach:{0.,.01,.1,.3,.52,.78,1.2})for(int n=0;n<12;++n){
                const double a=n*3.141592653589793/6.;const auto axis=*pc::unit(pc::Vec3{std::cos(a),std::sin(a),.4});
                run(reach,side,lab,physical,axis);
            }
    }
    std::printf("actual shoulder admission -> solve_arm: %d checks, %d failures; max wrist error %.9f mm, bone errors %.12f/%.12f m\n",checks,failures,maxError*1000,maxUpper,maxFore);
    return failures?1:0;
}
'''.replace("PRODUCTION_LAMBDA", lambda_text).replace("REACH_FUNCTION", reach)
fixture = output / "production_shoulder_admission_d39.cpp"
fixture.write_text(code)
binary = output / "production_shoulder_admission_d39"
subprocess.run(["c++", "-std=c++17", "-O2", "-I", str(source / "src"), str(fixture), "-o", str(binary)], check=True)
result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout, end="")
(output / "shoulder_admission_result.txt").write_text(result.stdout)
raise SystemExit(result.returncode)
