// DS-F96: touch-reaction policy (pure, host-tested).
#include "../src/subsystems/physics/touch_react60.h"
#include <cstdio>
#include <cmath>
namespace tr=kcd2vr::touch_react60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static tr::Contact c(unsigned soft,std::uint64_t tick){tr::Contact x{};x.entity=77;x.side=0;x.part=1;
    x.softMask=soft;x.living=true;x.point[0]=0;x.point[1]=1;x.point[2]=0;x.tick=tick;return x;}
int main(){
    {auto p=tr::plan(c(tr::kChest,1000),1000);CK(p.ok&&p.softMask==tr::kChest&&p.side==0);}
    {auto p=tr::plan(c(0,1100),1100);CK(p.why==tr::Why::NotSoft);}
    {auto x=c(tr::kChest,1200);x.living=false;auto p=tr::plan(x,1200);CK(p.why==tr::Why::NotLiving);}
    {auto x=c(tr::kChest,1300);x.fistActive=true;auto p=tr::plan(x,1300);CK(p.why==tr::Why::Haunted);}
    {auto x=c(tr::kChest,1400);x.point[0]=std::nanf("");auto p=tr::plan(x,1400);CK(p.why==tr::Why::NoPoint);}
    {tr::plan(c(tr::kChest,2000),2000);
     auto p=tr::plan(c(tr::kChest,2200),2200);CK(p.why==tr::Why::TooSoon);} // 300 ms one-shot
    {tr::plan(c(tr::kChest,3000),3000);
     auto p=tr::plan(c(tr::kChest,3400),3400);CK(p.ok);} // after the cooldown
    // DS-F98 (Claude review): two NPCs alternating do NOT bypass the one-shot
    {auto x=c(tr::kChest,4000);x.entity=101;tr::plan(x,4000);
     auto y=c(tr::kChest,4050);y.entity=202;CK(tr::plan(y,4050).ok); // a different entity admits
     auto z=c(tr::kChest,4100);z.entity=101;CK(tr::plan(z,4100).why==tr::Why::TooSoon);} // the SAME entity is still cooling
    // DS-F98: a zero tick still honours the cooldown
    {auto x=c(tr::kChest,0);tr::plan(x,5000);
     auto y=c(tr::kChest,0);CK(tr::plan(y,5100).why==tr::Why::TooSoon);}
    if(fails)return 1;std::puts("touch react F96 OK");return 0;
}
