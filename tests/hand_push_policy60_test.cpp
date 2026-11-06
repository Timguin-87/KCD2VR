// DS-F106: hand-push policy (pure, host-tested). The cooldown is 350 ms per
// entity, so every admitted case spaces its ticks by >=400 ms.
#include "../src/subsystems/physics/hand_push_policy60.h"
#include <cstdio>
#include <cmath>
namespace hp=kcd2vr::hand_push60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static hp::Evidence e(float speed){hp::Evidence x{};x.entity=500;x.bodyKind=hp::kBodyRigid;
    x.motion[0]=speed;x.motion[1]=0;x.motion[2]=0;x.tick=1000;return x;}
int main(){
    static_assert(hp::kBodyRigid==int(kcd2vr::npc_contact172::Body::Rigid));
    static_assert(hp::kBodyLiving==int(kcd2vr::npc_contact172::Body::Living));
    static_assert(hp::kBodyArticulated==int(kcd2vr::npc_contact172::Body::Articulated));
    {auto p=hp::plan(e(2.f),1000);CK(p.ok&&p.impulse[0]>1.9f&&p.impulse[0]<=2.1f);}
    {auto p=hp::plan(e(.2f),1400);CK(p.why==hp::Why::Slow);} // a resting hand never pushes (and never admits)
    {auto p=hp::plan(e(9.f),1800);CK(p.ok&&p.impulse[0]<=hp::kMaxPushSpeed+1e-3f);} // capped at 6
    {auto x=e(2.f);x.bodyKind=hp::kBodyLiving;auto p=hp::plan(x,2200);CK(p.why==hp::Why::LivingOrCorpse);} // never push a living actor
    {auto x=e(2.f);x.bodyKind=hp::kBodyArticulated;auto p=hp::plan(x,2600);CK(p.why==hp::Why::LivingOrCorpse);} // a corpse is the impale's
    {auto x=e(2.f);x.bodyKind=0;auto p=hp::plan(x,3000);CK(p.why==hp::Why::NotRigid);}
    {auto x=e(2.f);x.entity=0;auto p=hp::plan(x,3400);CK(p.why==hp::Why::NoEntity);}
    {auto x=e(2.f);x.motion[0]=std::nanf("");auto p=hp::plan(x,3800);CK(p.why==hp::Why::NonFinite);}
    {hp::plan(e(2.f),4200);auto p=hp::plan(e(2.f),4300);CK(p.why==hp::Why::TooSoon);} // the 350ms one-shot
    {hp::plan(e(2.f),5000);auto p=hp::plan(e(2.f),5400);CK(p.ok);} // after the cooldown
    // a different entity admits immediately (per-entity cooldown)
    {hp::plan(e(2.f),5800);auto y=e(2.f);y.entity=501;CK(hp::plan(y,5900).ok);}
    if(fails)return 1;std::puts("hand push F106 OK");return 0;
}
