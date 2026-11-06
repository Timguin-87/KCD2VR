// H18 VRIK elbow lane (0092): bounded pole-continuity policy host test.
// Host tests are NOT headset acceptance.
// Compile: cl /nologo /std:c++17 /EHsc /Od /Isrc tests\elbow_continuity60_test.cpp /Fe:ec60.exe
#include "../src/vrik/elbow_continuity60.h"
#include <cstdio>
#include <cmath>
#include <limits>
namespace pc = kcd2vr::pose_core;
using kcd2vr::elbow60::BoundedPole;
using kcd2vr::elbow60::bounded_pole;
using kcd2vr::elbow60::condition_pole;
using kcd2vr::elbow60::hand_moved_far;
using kcd2vr::elbow60::kDefaultMaxStepRadians;
static int total=0,pass=0;
static void Check(bool b,const char* n){++total;pass+=b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static pc::Vec3 V(double x,double y,double z){return {x,y,z};}
static pc::Vec3 turn(pc::Vec3 v,pc::Vec3 axis,double a){return v*std::cos(a)+pc::cross(axis,v)*std::sin(a);}
static double angleBetween(pc::Vec3 a,pc::Vec3 b){
    const auto ua=pc::unit(a),ub=pc::unit(b);
    return ua&&ub?std::acos(std::clamp(pc::dot(*ua,*ub),-1.0,1.0)):1e9;
}
int main(){
    const double deg=3.14159265358979323846/180.0;
    const double nan=std::numeric_limits<double>::quiet_NaN();
    const double inf=std::numeric_limits<double>::infinity();
    const pc::Vec3 axis{1,0,0};
    const pc::Vec3 prev{0,1,0};
    const pc::Vec3 raw30=turn(prev,axis,30*deg);   // inside the default 40-degree bound
    const pc::Vec3 raw100=turn(prev,axis,100*deg); // outside the bound
    // Blend inside the bound follows the raw pole exactly.
    const auto inside=condition_pole(prev,raw30,axis);
    Check(inside&&pc::length(*inside-raw30)<1e-12,"pole inside the 40-degree bound follows raw exactly");
    // A 100-degree raw jump is clamped to exactly 40 degrees, on the raw side.
    const auto clamped=condition_pole(prev,raw100,axis);
    Check(clamped&&std::abs(angleBetween(*clamped,prev)-40*deg)<1e-9,"100-degree raw jump clamps to the 40-degree default bound");
    Check(clamped&&pc::length(*clamped)>1-1e-12&&pc::length(*clamped)<1+1e-12,"clamped output stays unit length");
    Check(clamped&&std::abs(pc::dot(*clamped,axis))<1e-12,"clamped output stays perpendicular to the reach axis");
    Check(clamped&&pc::dot(pc::cross(axis,prev),*clamped)>0,"clamped output rotates toward the raw side, not the opposite hemisphere");
    // Explicit zero bound freezes the pole.
    const auto frozen=condition_pole(prev,raw100,axis,0.0);
    Check(frozen&&pc::length(*frozen-prev)<1e-12,"zero max step returns the previous pole unchanged");
    // Antipodal raw (180 degrees) is bounded, finite and deterministic: no NaN.
    const auto anti=condition_pole(prev,prev*(-1),axis);
    Check(anti&&std::abs(angleBetween(*anti,prev)-40*deg)<1e-9,"antipodal raw pole is bounded by the same 40-degree step");
    // Fail closed on every non-finite or out-of-range input.
    Check(!condition_pole(V(nan,1,0),raw30,axis),"NaN previous pole is refused");
    Check(!condition_pole(prev,V(inf,0,0),axis),"infinite raw pole is refused");
    Check(!condition_pole(prev,raw30,V(nan,0,0)),"NaN reach axis is refused");
    Check(!condition_pole(prev,raw30,V(0,0,0)),"zero reach axis is refused");
    Check(!condition_pole(V(0,0,0),raw30,axis),"zero previous pole is refused");
    Check(!condition_pole(prev,V(0,0,0),axis),"zero raw pole is refused");
    Check(!condition_pole(prev,raw30,axis,-0.01),"negative max step is refused");
    Check(!condition_pole(prev,raw30,axis,3.14159265358979323846+0.01),"max step above pi is refused");
    Check(!condition_pole(prev,raw30,axis,nan),"NaN max step is refused");
    Check(!condition_pole(prev,axis,axis),"raw pole parallel to the reach axis is refused");
    Check(!condition_pole(axis,raw30,axis),"previous pole parallel to the reach axis is refused");
    // Neutral rearm: a far hand move returns the raw pole unchanged (reseed).
    const auto far=bounded_pole(prev,raw100,axis,V(0,0,0),V(1.5,0,0),0.3);
    Check(far&&far->rearmed&&pc::length(far->pole-raw100)<1e-12,
        "far hand move rearms to the raw pole instead of dragging stale geometry");
    // Near hand keeps the bounded blend.
    const auto near=bounded_pole(prev,raw100,axis,V(0,0,0),V(0.05,0,0),0.3);
    Check(near&&!near->rearmed&&std::abs(angleBetween(near->pole,prev)-40*deg)<1e-9,
        "near hand keeps the 40-degree bounded blend");
    // The rearm threshold is strict: exactly at the distance it does not rearm.
    const auto edge=bounded_pole(prev,raw100,axis,V(0,0,0),V(0.3,0,0),0.3);
    Check(edge&&!edge->rearmed,"hand exactly at the rearm distance does not rearm");
    // Rearm output is projected perpendicular to the axis even with an axial raw component.
    const auto axialRaw=bounded_pole(prev,V(0.3,0,-1),axis,V(0,0,0),V(1.5,0,0),0.3);
    Check(axialRaw&&axialRaw->rearmed&&std::abs(pc::dot(axialRaw->pole,axis))<1e-12&&
        pc::length(axialRaw->pole)>1-1e-12&&pc::length(axialRaw->pole)<1+1e-12,
        "rearmed pole is projected perpendicular to the reach axis");
    // Fail closed on the combined entry point.
    Check(!bounded_pole(prev,raw100,axis,V(nan,0,0),V(0,0,0),0.3),"NaN previous hand is refused");
    Check(!bounded_pole(prev,raw100,axis,V(0,0,0),V(inf,0,0),0.3),"infinite hand is refused");
    Check(!bounded_pole(prev,raw100,axis,V(0,0,0),V(0,0,0),-1.0),"negative rearm distance is refused");
    Check(!bounded_pole(prev,raw100,axis,V(0,0,0),V(0,0,0),nan),"NaN rearm distance is refused");
    Check(!bounded_pole(prev,raw100,axis,V(0,0,0),V(0,0,0),0.3,-0.1),"negative max step on the combined entry is refused");
    // hand_moved_far predicate contract.
    Check(hand_moved_far(V(0,0,0),V(0.3001,0,0),0.3)&&!hand_moved_far(V(0,0,0),V(0.3,0,0),0.3),
        "hand_moved_far uses a strict distance comparison");
    Check(!hand_moved_far(V(nan,0,0),V(1,0,0),0.3)&&!hand_moved_far(V(0,0,0),V(1,0,0),-1.0),
        "hand_moved_far refuses non-finite or negative inputs as not-moved");
    // Repeated application never exceeds the bound and converges onto the raw pole.
    {
        pc::Vec3 current=prev;bool bounded=true,converged=false;
        for(int k=0;k<60;++k){
            const auto step=bounded_pole(current,raw100,axis,V(0,0,0),V(0.05,0,0),0.3);
            if(!step){bounded=false;break;}
            bounded=bounded&&angleBetween(step->pole,current)<=kDefaultMaxStepRadians+1e-9;
            current=step->pole;
            if(angleBetween(current,raw100)<1e-9){converged=true;break;}
        }
        Check(bounded&&converged,"repeated 40-degree steps converge onto the raw pole without a snap");
    }
    // Perpendicularity and bound hold across a wide raw-angle sweep, antipodes included.
    {
        bool sweep=true;
        for(int k=0;k<721;++k){
            const double a=-3.14+2*3.14*k/720.0;
            const auto out=condition_pole(prev,turn(prev,axis,a),axis);
            if(!out){sweep=false;break;}
            sweep=sweep&&std::abs(pc::dot(*out,axis))<1e-9&&
                pc::length(*out)>1-1e-9&&pc::length(*out)<1+1e-9&&
                angleBetween(*out,prev)<=kDefaultMaxStepRadians+1e-9;
        }
        Check(sweep,"full raw-angle sweep keeps output perpendicular, unit and within the bound");
    }
    std::printf("elbow_continuity60=%d/%d\n",pass,total);
    return pass!=total;
}
