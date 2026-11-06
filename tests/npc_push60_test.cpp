// DS-F109: living-NPC push outcomes (pure, host-tested).
#include "../src/subsystems/physics/npc_push60.h"
#include <cstdio>
#include <cmath>
namespace np=kcd2vr::npc_push60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static np::Evidence e(float speed,std::uint64_t grip,bool sweep=false){np::Evidence x{};x.motion[0]=speed;x.gripMs=grip;x.lowSweep=sweep;return x;}
int main(){
    static_assert(!np::kNativeReactionProviderAvailable); // classifications are proposed intents, not native behavior
    {auto p=np::plan(e(.3f,100));CK(p.outcome==np::Outcome::None);} // a gentle touch does nothing
    {auto p=np::plan(e(1.f,400));CK(p.outcome==np::Outcome::Walk);} // a sustained press steps her back
    {auto p=np::plan(e(2.f,100));CK(p.outcome==np::Outcome::Stumble);} // a firm quick push staggers
    {auto p=np::plan(e(5.f,50));CK(p.outcome==np::Outcome::Knockdown);} // a hard shove knocks her down
    {auto p=np::plan(e(1.2f,50,true));CK(p.outcome==np::Outcome::Sweep);} // a low sweep trips the legs
    {auto p=np::plan(e(2.f,400));CK(p.outcome==np::Outcome::Walk);} // a firm push held = walk, not a stagger
    {auto x=e(2.f,100);x.motion[0]=std::nanf("");CK(np::plan(x).outcome==np::Outcome::None);} // non-finite refuses
    if(fails)return 1;std::puts("npc push F109 OK");return 0;
}
