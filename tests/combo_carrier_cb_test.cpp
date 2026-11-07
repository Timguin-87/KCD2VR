// D30-CMB combo carrier keep-alive: pure decision/release rules (combo_carrier_cb.h)
#include "subsystems/masterstrike_combos/combo_carrier_cb.h"
#include "vrik/master_gate47.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::combo_carrier_cb;
static unsigned checks=0,fails=0;
static void check(bool ok,const char* what){++checks;if(!ok){++fails;std::fprintf(stderr,"FAIL %s\n",what);}}
int main(){
    const std::uintptr_t base=0x140000000ull;
    Window w{};w.actor=0x1000;w.attack=0x2000;w.manager=0x3000;w.fragment=0x4000;w.key=77;w.serial=5;w.tick=1000;w.thread=9;
    check(armed(w,1050),"armed fresh");check(!armed(w,1200),"armed expires after 120 ms");
    Window empty{};empty.serial=6;check(!armed(empty,1000),"disarm window");
    Arrival a{};a.fragment=0x4000;a.fragmentVt=base+kFragmentVt;a.attackOfWindowFragmentSlot=0x4000;a.ret=base+kNotifyCallRet;a.outerRet=base+kNotifyRet;
    a.functor=0x5000;a.functorVt=base+kFunctorVt;a.functorFn=base+kEndDelegate;a.functorTarget=0x2000;a.once=0;a.thread=9;
    Token none{};
    auto d=decide(true,w,true,a,none,base);check(d.verdict==Verdict::Defer&&d.why==Why::Defer,"natural armed end is deferred");
    check(decide(false,w,true,a,none,base).verdict==Verdict::Native,"off = native");
    check(decide(true,w,false,a,none,base).verdict==Verdict::Native,"not armed = native");
    {auto x=a;x.outerRet=base+0x4AE38F;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"timed replacement 4AE389 = native");}
    {auto x=a;x.ret=base+0x12345;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"forced stop caller = native");}
    {auto x=a;x.once=1;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"once already set = native");}
    {auto x=a;x.fragment=0x4444;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"other fragment (NPC) = native");}
    {auto x=a;x.functorTarget=0x9999;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"functor bound elsewhere = native");}
    {auto x=a;x.functorFn=base+1;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"other delegate fn = native");}
    {auto x=a;x.fragmentVt=base+1;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"other fragment class = native");}
    {auto x=a;x.attackOfWindowFragmentSlot=0x7;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"attack+80 changed = native");}
    {auto x=a;x.thread=3;check(decide(true,w,true,x,none,base).verdict==Verdict::Native,"other thread = native");}
    const Token t=make(w,a,1000,windowMs(1000));
    check(t.live&&t.fragment==0x4000&&t.attack==0x2000&&t.deadline==2200,"token built with capped deadline");
    // with a live token
    check(decide(true,w,true,a,t,base).why==Why::Again,"repeated natural notification stays deferred");
    {auto x=a;x.ret=base+0x90CD93;auto r=decide(true,w,true,x,t,base);check(r.verdict==Verdict::DropNative,"Exit route on token drops it, native runs");}
    {auto x=a;x.once=1;check(decide(true,w,true,x,t,base).verdict==Verdict::DropNative,"once set by native on token = drop");}
    {auto x=a;x.fragment=0x8000;check(decide(true,w,true,x,t,base).verdict==Verdict::Native,"one token at a time: other fragment native, token kept");}
    check(decide(false,w,true,a,t,base).why==Why::Again,"toggle off with live token: natural repeat still deferred until service releases");
    // release
    Live l{};l.readOk=true;l.managerMain=0x2000;l.attackVt=base+kAttackVt;l.attackFragment=0x4000;l.fragmentVt=base+kFragmentVt;l.functor=0x5000;l.functorTarget=0x2000;l.once=0;l.thread=9;
    check(release(t,true,77,l,1500,base).what==Release::Keep,"window still armed: keep");
    {auto r=release(t,false,77,l,1500,base);check(r.what==Release::Call&&r.why==ReleaseWhy::Ended,"window ended: call original once");}
    {auto r=release(t,true,77,l,2300,base);check(r.what==Release::Call&&r.why==ReleaseWhy::Deadline,"deadline: call even if armed");}
    {auto r=release(t,true,78,l,1500,base);check(r.what==Release::Call,"new stroke key ends the old window");}
    {auto x=l;x.managerMain=0x6000;auto r=release(t,false,77,x,1500,base);check(r.what==Release::Drop&&r.why==ReleaseWhy::Replaced,"slot replaced: drop, never retire the new action");}
    {auto x=l;x.readOk=false;check(release(t,false,77,x,1500,base).what==Release::Drop,"read failure: drop");}
    {auto x=l;x.attackFragment=0x9;check(release(t,false,77,x,1500,base).what==Release::Drop,"fragment swapped: drop");}
    {auto x=l;x.functor=0x9;check(release(t,false,77,x,1500,base).what==Release::Drop,"functor changed (route B unbound): drop");}
    {auto x=l;x.once=1;check(release(t,false,77,x,1500,base).what==Release::Drop,"native already fired: drop");}
    {auto x=l;x.attackVt=base+1;check(release(t,false,77,x,1500,base).what==Release::Drop,"attack destroyed/reused: drop");}
    {auto x=l;x.thread=4;check(release(t,false,77,x,1500,base).what==Release::Keep,"other thread: wait");}
    {auto x=l;x.thread=4;check(release(t,false,77,x,2200+kStrandMs,base).what==Release::Strand,"other thread past cap: stranded, never called");}
    Token dead{};check(release(dead,false,0,l,1500,base).what==Release::Keep,"no token: nothing");
    {// D31: the main action is [[manager+0x38]+0], not manager+0x38
     std::uintptr_t mem[4]{};const std::uintptr_t mgr=0x1000,slots=0x3000;
     auto read=[&](std::uintptr_t a,std::uintptr_t& v){if(a==mgr+0x38){v=slots;return true;}if(a==slots){v=0x2000;return true;}return false;};
     std::uintptr_t main=0;check(readManagerMain(read,mgr,main)&&main==0x2000,"two-step main read finds the attack");
     check(main!=slots,"never the slots pointer");
     auto empty=[&](std::uintptr_t a,std::uintptr_t& v){if(a==mgr+0x38){v=0;return true;}return false;};
     check(!readManagerMain(empty,mgr,main),"no slots: read fails (drop)");
     check(!readManagerMain(read,0,main),"no manager: read fails");(void)mem;}
    {namespace mg=kcd2vr::master_gate47;
     check(mg::cueOnOpen(true,true,3,0),"MS cue on a new open window");check(!mg::cueOnOpen(true,true,3,3),"MS cue once per window");
     check(!mg::cueOnOpen(false,true,4,0),"MS cue off");check(!mg::cueOnOpen(true,false,4,0),"no cue without open edge");}
    std::printf("combo_carrier_cb %u checks, %u failures\n",checks,fails);
    return fails?1:0;
}
