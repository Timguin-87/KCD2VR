// R59.16C10R8 hand/blade physics bodies: native layouts + pure follow logic (no native calls).
#include "vrik/phys_bodies59c10r8.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr::phys_bodies59c10r8;
static int fails=0;static void ck(bool b,const char* m){std::printf("%s %s\n",b?"PASS":"FAIL",m);fails+=!b;}
static bool near(float a,float b,float e=1e-3f){return std::fabs(a-b)<=e;}
int main(){
    // layouts were static_asserted in the header; check the MARK_UNUSED encodings
    float u=unusedFloat();std::uint32_t bits=0;std::memcpy(&bits,&u,4);ck(bits==0xFFBFFFFFu,"MARK_UNUSED float = 0xFFBFFFFF");
    ck(kUnusedI==std::int32_t(0x80000000u),"MARK_UNUSED int = 1<<31");
    Tuning t{};Target tg{};tg.valid=true;tg.p={1,2,3};tg.q={};tg.tick=1000;
    auto c=step(tg,{1,2,3.1f},{},true,1010,t,true);ck(c.op==Command::Op::Velocity&&near(c.v.z,-.1f/t.tau,1e-2f),"small error -> velocity closes it in tau");
    c=step(tg,{1,2,2.0f},{},true,1010,t,true);ck(c.op==Command::Op::Teleport&&near(c.p.z,3),"error > 0.6 m -> teleport to the target");
    c=step(tg,{1,2,2.5f},{},true,1010,t,true);ck(c.op==Command::Op::Velocity&&len(c.v)<=t.maxSpeed+1e-3f,"speed is capped");
    c=step(tg,{1,2,3},{},true,1200,t,true);ck(c.op==Command::Op::Park,"stale target (>80 ms) parks, never chases an old pose");
    Target bad=tg;bad.p.x=NAN;c=step(bad,{1,2,3},{},true,1010,t,true);ck(c.op==Command::Op::Park,"non-finite target parks");
    c=step(tg,{},{},false,1010,t,true);ck(c.op==Command::Op::Teleport,"no readable body pose -> teleport");
    // rotation: 90 deg about Z needs w.z = (pi/2)/tau (capped)
    Target rt=tg;const float s=std::sqrt(.5f);rt.q={0,0,s,s};c=step(rt,{1,2,3},{},true,1010,t,true);
    ck(c.op==Command::Op::Velocity&&c.w.z>0&&near(len(c.w),std::min(1.5707963f/t.tau,t.maxAngSpeed),.05f),"angular velocity toward the target orientation");
    c=step(rt,{1,2,3},{},true,1010,t,false);ck(len(c.w)==0,"spheres get no angular drive");
    // capsule from a blade segment along +X: centre midpoint, half length, local +Y mapped onto +X
    V3f ctr{};Qf q{};float hh=0;ck(capsuleFromSegment({0,0,0},{1,0,0},ctr,q,hh)&&near(ctr.x,.5f)&&near(hh,.5f),"capsule centre and half height");
    // rotate local +Y by q
    auto rot=[&](Qf r,V3f v){V3f u{r.x,r.y,r.z};V3f t2{2*(u.y*v.z-u.z*v.y),2*(u.z*v.x-u.x*v.z),2*(u.x*v.y-u.y*v.x)};
        return V3f{v.x+r.w*t2.x+(u.y*t2.z-u.z*t2.y),v.y+r.w*t2.y+(u.z*t2.x-u.x*t2.z),v.z+r.w*t2.z+(u.x*t2.y-u.y*t2.x)};};
    V3f ax=rot(q,{0,1,0});ck(near(ax.x,1,1e-3f)&&near(ax.y,0,1e-3f),"capsule axis points along the blade");
    ck(capsuleFromSegment({0,0,0},{0,-1,0},ctr,q,hh)&&near(rot(q,{0,1,0}).y,-1,1e-3f),"blade pointing down (180 deg) handled");
    ck(!capsuleFromSegment({0,0,0},{0,0,.01f},ctr,q,hh)&&!capsuleFromSegment({0,0,0},{0,0,5},ctr,q,hh),"degenerate / absurd blades rejected");
    std::printf("%d failures\n",fails);return fails;}
