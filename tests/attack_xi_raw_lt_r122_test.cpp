// R122: a physical LT passed to the game outside a fight must not hide the mod's own block/kick edges.
#include <cstdio>
#include "vrik/attack_xi178r8.h"
namespace x=kcd2vr::attack_xi178r8;
static int fails=0,checks=0;
#define CHECK(c) do{++checks;if(!(c)){++fails;std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);}}while(0)
int main(){
    x::enabled=true;
    // 1. raw LT held (not owned), guard presses block: first an LT-up pad, after the up was dispatched an owned down.
    x::reset();x::rawLtR122=true;
    float rt=0,lt=1;bool a=false;x::apply(100,rt,lt,a);CHECK(lt==1.f&&(x::ownedMask.load()&2u)==0);
    CHECK(x::translate(3,1,110));lt=1;x::apply(110,rt,lt,a);CHECK(lt==0.f&&(x::ownedMask.load()&2u)==0);
    lt=1;x::apply(118,rt,lt,a);CHECK(lt==0.f); // still waiting for the emitter's LT-up
    x::ackLtUp55c1();lt=0;x::apply(126,rt,lt,a);CHECK(lt==1.f&&(x::ownedMask.load()&2u)!=0&&x::ltPublished.load());
    x::rawLtR122=false; // FistTransportTrigger101 cuts the raw press once ltPublished
    CHECK(x::translate(3,2,300));lt=0;x::apply(300,rt,lt,a);CHECK(lt==0.f&&(x::ownedMask.load()&2u)==0);
    // 2. without a raw LT the guard press is immediate (fights unchanged).
    x::reset();CHECK(x::translate(3,1,400));lt=0;x::apply(400,rt,lt,a);CHECK(lt==1.f&&(x::ownedMask.load()&2u)!=0);
    // 3. kick (attack_special LT hold) during a raw LT: LT-up first, then the owned hold.
    x::reset();x::rawLtR122=true;CHECK(x::translate(4,1,500));lt=1;x::apply(500,rt,lt,a);CHECK(lt==0.f);
    x::ackLtUp55c1();x::rawLtR122=false;lt=0;x::apply(510,rt,lt,a);CHECK(lt==1.f&&(x::ownedMask.load()&2u)!=0);
    // 4. no raw LT: kick hold immediate.
    x::reset();CHECK(x::translate(4,1,600));lt=0;x::apply(600,rt,lt,a);CHECK(lt==1.f&&(x::ownedMask.load()&2u)!=0);
    // 5. raw LT alone passes untouched.
    x::reset();x::rawLtR122=true;lt=.8f;x::apply(700,rt,lt,a);CHECK(lt==.8f&&x::ownedMask.load()==0);
    std::printf("attack_xi_raw_lt_r122=%d/%d\n",checks-fails,checks);return fails?1:0;
}
