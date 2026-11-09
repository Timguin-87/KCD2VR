// R158 host test: a dropped (not stowed) instant pickup item gets the scenery-prop gravity rules
// (src/vrik/instant_drop_r158.h, serviced by static_grab_r89.inl InstantDropR158).
// g++ -std=c++20 -I src -I src/vrik tests/instant_drop_r158_test.cpp -o instant_drop_r158_test && ./instant_drop_r158_test
#include "vrik/instant_drop_r158.h"
#include "vrik/prop_parent_r153.h"
#include <cstdio>
#include <cstring>
#include <limits>
namespace id=kcd2vr::instant_drop_r158;namespace pp=kcd2vr::prop_parent_r153;namespace pc=kcd2vr::pose_core;
struct Pose {pc::Quat q{};pc::Vec3 p{};};
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
static id::Request req(std::uint32_t entity,std::uint64_t tick,double vx=0,bool hand=false){
    id::Request r{};r.id=entity;r.entity=0x1000+entity;r.tick=tick;r.v[0]=vx;r.handVelocity=hand;return r;}
int main(){
    {   // 1. one drop = one request, serviced once (no latch, no timer retry)
        id::Slot s{};id::Request out{};
        ck(!s.take(1000,out)&&!out.id,"empty slot: nothing to do");
        s.post(req(524663,1000,1.2,true));
        ck(s.take(1016,out)&&out.id==524663&&out.entity==0x1000+524663&&out.handVelocity,"next frame takes the drop");
        ck(!s.take(1032,out),"taken once only");
    }
    {   // 2. invalid posts are ignored
        id::Slot s{};
        s.post(req(0,1000));ck(!s.pending,"no entity id: ignored");
        auto r=req(5,1000);r.entity=0;s.post(r);ck(!s.pending,"no entity pointer: ignored");
        s.post(req(5,0));ck(!s.pending,"no tick: ignored");
    }
    {   // 3. age: a drop that waited > 0.5 s is consumed and skipped (it does not come back later)
        id::Slot s{};id::Request out{};
        s.post(req(7,1000));ck(s.take(1500,out),"0.5 s is still serviced");
        s.post(req(7,1000));ck(!s.take(1501,out)&&!s.pending,"older: skipped and gone");
        s.post(req(7,2000));ck(!s.take(1990,out)&&!s.pending,"future stamp (clock jump): skipped");
        s.post(req(7,1000));s.post(req(8,1010));ck(s.take(1020,out)&&out.id==8,"the latest drop wins (one instant item at a time)");
    }
    {   // 4. hand velocity: written again after the wake only when the release had one
        ck(id::rewriteVelocity(req(1,1,1.5,true)),"thrown: hand velocity again after the wake");
        ck(!id::rewriteVelocity(req(1,1,1.5,false)),"no trail (still hand / servo hold): gravity alone");
        ck(!id::rewriteVelocity(req(1,1,.01,true)),"1 cm/s: gravity alone (no zero write over the fall)");
        auto n=req(1,1,0,true);n.v[1]=std::numeric_limits<double>::quiet_NaN();ck(!id::rewriteVelocity(n),"NaN: never written");
        auto d=req(1,1,0,true);d.v[0]=.03;d.v[1]=.04;ck(id::rewriteVelocity(d)&&std::fabs(id::speed(d)-.05)<1e-9,"speed is the vector length");
    }
    {   // 5. Lua chunks: DetachThis(1) = keep the world transform, AwakePhysics(1); fit the 320-byte buffer for any id
        char a[320]{},w[320]{};
        const int na=id::detachChunk(a,sizeof(a),4294967295u),nw=id::wakeChunk(w,sizeof(w),4294967295u);
        ck(na>0&&std::size_t(na)<sizeof(a)&&nw>0&&std::size_t(nw)<sizeof(w),"chunks fit");
        ck(std::strstr(a,"System.GetEntity(4294967295)")&&std::strstr(a,"pcall(e.DetachThis,e,1)"),"detach: this entity, keep transform flag 1");
        ck(std::strstr(w,"pcall(e.AwakePhysics,e,1)")&&std::strstr(w,"assert("),"wake: AwakePhysics(1), refusal = chunk false");
        ck(!std::strstr(w,"DetachThis")&&!std::strstr(a,"AwakePhysics"),"two separate steps (wake also runs when no parent)");
    }
    {   // 6. pose after the detach: kept -> no write; moved (local became world) -> put back where he let go
        const Pose parent{{0,0,0,1},{2994.33789,676.37903,52.51316}};
        const Pose local{{0,0,0,1},{.02,-.07,.06}};
        const Pose world=pp::Compose(parent,local);
        ck(pp::Matches(world,world),"DetachThis(1) kept the world pose: nothing written");
        ck(!pp::Matches(local,world),"a detach that dropped the parent space (item would jump 3 km): seen -> put back");
    }
    std::printf("instant_drop_r158_test: %u/%u passed\n",total-fail,total);
    return fail?1:0;
}
