#pragma once
#include "module.h"
#include "../../vrik/attack_clock50c2.h"

namespace kcd2vr::subsystems::masterstrike_combos {
// Deliberately separate from attackclock50::inspect: its phase4/8 restriction
// remains the authority for clock/contact writes. Native phase16 is ONLY an
// observation at the existing original-first combo-trigger callback.
template<class Read> bool InspectNativeSlotF1(const attackclock50::Binding& b,std::uintptr_t base,Read read,bool continuation=false) noexcept {
    auto eq=[&](std::uintptr_t p,std::uintptr_t expected){std::uintptr_t x=0;return read(p,&x,8)&&x==expected;};
    auto integer=[&](std::uintptr_t p,int expected){int x=-1;return read(p,&x,4)&&x==expected;};
    auto byte=[&](std::uintptr_t p,unsigned char expected){unsigned char x=255;return read(p,&x,1)&&x==expected;};
    const auto a=b.intent.actor,s=b.intent.state,p=b.intent.player;
    if(!base||!a||!s||!p||!b.action||!b.helper||!b.clock||!b.manager||!b.row||!b.entity||
       b.intent.side!=0||(b.intent.kind!=attack_timing178r22c2::Kind::Slash&&b.intent.kind!=attack_timing178r22c2::Kind::Thrust)||
       !eq(a,base+0x3A80610)||!eq(a+0x1F8,p)||!eq(p,base+0x3AA4B98)||!eq(p+0x38,b.entity)||
       !eq(a+0x210,s)||!eq(s+0xCE0,a)||!eq(a+0x200,b.manager)||!eq(a+0x3C0,b.clock)||!eq(b.clock+8,a))return false;
    std::uintptr_t slots=0;
    // Native manager membership precedes EVERY dereference of the borrowed action.
    if(!read(b.manager+0x38,&slots,8)||!slots||!eq(slots,b.action))return false;
    if(!eq(b.action,base+0x3A61C58)||!integer(b.action+0x3C,0)||!eq(b.action+0x40,b.manager)||
       !byte(b.action+0x28,1)||!eq(b.action+0x70,a)||!eq(b.action+0xA0,b.helper)||!eq(b.action+0x58,b.row)||
       !eq(b.helper,base+0x3A61040)||!eq(b.helper+8,a)||!eq(b.helper+0x20,b.action+0x88)||
       !eq(b.action+0x88,base+0x3A61440)||!byte(b.clock+0x14,1))return false;
    std::uintptr_t type=0,gate=0;int kind=-1;
    return read(b.row+0x148,&type,8)&&type&&read(type+8,&kind,4)&&(kind==0||kind==1)&&
        read(a+0x220,&gate,8)&&gate&&byte(gate+0x28,0)&&
        (integer(s+0x38,16)||(continuation&&integer(s+0x38,32)&&byte(s+0x868,1)));
}
}
