// R153 host test: placing an entity that hangs on a parent (prop_parent_r153.h), used by PropPlace596
// (staged_pickup596c2.inl) for the instant-held pickup item. numbers from the R152 run (item 524663, 11:18:47).
// g++ -std=c++20 -I src -I src/vrik tests/prop_parent_r153_test.cpp
#include "vrik/prop_parent_r153.h"
#include <cstdio>
#include <cmath>
namespace pp=kcd2vr::prop_parent_r153;namespace pc=kcd2vr::pose_core;
struct Pose {pc::Quat q{};pc::Vec3 p{};};
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
static pc::Quat yaw(double deg){const double h=deg*3.141592653589793/360.;return {0,0,std::sin(h),std::cos(h)};}
// the engine: world = parent * local (the setter writes local)
static Pose engine(const Pose* parent,const Pose& local){return parent?pp::Compose(*parent,local):local;}
static bool near(pc::Vec3 a,pc::Vec3 b,double e=1e-6){return pc::length(a-b)<=e;}
int main(){
    {   // 1. no parent: the first readback is right -> PropPlace596 does nothing more (old behaviour)
        Pose want{yaw(30),{2994.40,676.30,52.80}};
        const auto world=engine(nullptr,want);
        ck(pp::Matches(world,want),"no parent: readback matches, no second placement");
    }
    {   // 2. the R152 case: parent at the item's old spot, identity rotation. local = wanted world -> doubled
        const Pose parent{{0,0,0,1},{2994.33789,676.37903,52.51316}};
        Pose want{yaw(10),{2994.36,676.40,52.57}};
        const auto doubled=engine(&parent,want);
        ck(near(doubled.p,{5988.69789,1352.77903,105.08316},1e-4),"old code: 2x world position (5988,1352,105 as in the log)");
        ck(!pp::Matches(doubled,want),"readback mismatch is seen");
        const auto learned=pp::Parent(doubled,want);
        ck(near(learned.p,parent.p,1e-6),"parent learned from one readback");
        const auto again=pp::LocalFor(learned,want);
        const auto placed=engine(&parent,again);
        ck(pp::Matches(placed,want)&&near(placed.p,want.p,1e-6),"second placement: item is where the hand wants it");
        // next frame: cached parent, one placement
        Pose next{yaw(25),{2994.50,676.10,52.90}};
        ck(pp::Matches(engine(&parent,pp::LocalFor(learned,next)),next),"cached parent: next frame right at once");
    }
    {   // 3. rotated parent (not identity), item away from it
        const Pose parent{yaw(73),{100,-40,12}};
        Pose want{yaw(-20),{101.2,-39.1,12.9}};
        const auto first=engine(&parent,want);
        ck(!pp::Matches(first,want),"rotated parent: first set is wrong");
        const auto learned=pp::Parent(first,want);
        ck(pp::Matches(engine(&parent,pp::LocalFor(learned,want)),want),"rotated parent: fixed by one relearn");
    }
    {   // 4. the parent went away (cached parent now wrong): the readback shows it and the relearn gives identity
        const Pose stale{{0,0,0,1},{2994,676,52}};
        Pose want{yaw(5),{2995,677,53}};
        const auto local=pp::LocalFor(stale,want);
        const auto world=engine(nullptr,local);
        ck(!pp::Matches(world,want),"stale parent seen");
        const auto learned=pp::Parent(world,local);
        ck(near(learned.p,{0,0,0},1e-6)&&std::fabs(std::fabs(learned.q.w)-1)<1e-9,"relearned parent = identity");
        ck(pp::Matches(engine(nullptr,pp::LocalFor(learned,want)),want),"placed right again");
    }
    {   // 5. tolerance: 1.5 cm / 4 deg off = matches, 3 cm or 10 deg = not
        Pose want{yaw(0),{1,2,3}};
        ck(pp::Matches(Pose{yaw(4),{1.015,2,3}},want),"small error still matches");
        ck(!pp::Matches(Pose{yaw(0),{1.03,2,3}},want),"3 cm = mismatch");
        ck(!pp::Matches(Pose{yaw(10),{1,2,3}},want),"10 deg = mismatch");
        ck(pp::Matches(Pose{{0,0,-std::sin(0.0),-1},{1,2,3}},want),"q and -q are the same rotation");
    }
    std::printf("prop_parent_r153_test: %u/%u passed\n",total-fail,total);
    return fail?1:0;
}
