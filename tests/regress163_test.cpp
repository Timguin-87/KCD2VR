#include "head_aim120r3.h"
#include "activity_input163.h"
#include <cassert>
#include <cstdio>
int main(){
    using namespace kcd2vr;
    for(int weapon=-1;weapon<=8;++weapon)for(float trigger:{0.f,.6f,1.f})
        assert(!head_aim120r3::Allowed(true,true,false,true,true,false,weapon,trigger));
    using namespace ui_native_state;
    Snapshot s{};s.visible=Bit(Crafting)|Bit(AlchemyBook);
    assert(!activity_input163::Read(s).nativeControls); // unobserved is not known
    s.known=s.visible;assert(!activity_input163::Read(s).nativeControls); // stale textures/crafting must not steal input
    assert(activity_input163::Read(s,true).nativeControls);
    s.visible=0;assert(!activity_input163::Read(s).nativeControls);
    s.known=s.visible=Bit(Hud)|Bit(Overlay)|Bit(GeneralBook)|Bit(ForgePlan)|Bit(AlchemyBook);
    assert(!activity_input163::Read(s).nativeControls); // broad HUD/book presence is not a minigame
    s.known=s.visible=Bit(LockPicking);assert(activity_input163::Read(s).nativeControls);
    std::puts("PASS retired HMD input and observed-only activity input ownership");
}
