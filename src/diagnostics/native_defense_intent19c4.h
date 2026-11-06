#pragma once
#include <cstdint>
#include <cstddef>
#include <cmath>
#include "../vrik/motion_owned59c4.h"

// The native ordinary attack publishes its admitted-intent byte (+5A0)
// immediately before phase4 (B27446..B27476). This is an INPUT to the block
// trigger's own eligibility test, never a block/hit result. The VR provider
// supplies it only for a current deliberate, reachable threat to the exact
// reciprocal opponent. The lease lasts one synchronous phase notification.
namespace kcd2vr::defense_intent19c4 {
struct Context {
    motion59c4::Key key{};
    std::uintptr_t actor=0,state=0,target=0,targetState=0;
    std::uint64_t now=0,onset=0,deadline=0;
    int oldPhase=-99,defense=-1,original=-1;
    float remaining=0;
};
template<class Read> int Bool(Read read,std::uintptr_t base,std::uintptr_t state,unsigned prop) noexcept {
    std::uintptr_t vft=0;unsigned char value=255;
    if(!state||!read(state+prop,&vft,sizeof(vft))||vft!=base+0x3B0A6A0||
       !read(state+prop+8,&value,1)||value>1)return -1;
    return int(value);
}
template<class Read> bool SameActor(Read read,std::uintptr_t base,const Context& c) noexcept {
    std::uintptr_t vft=0,state=0,actor=0,player=0,playerVft=0,playerActor=0;
    // Henry is C_CombatPlayer, not an instance of the base C_CombatActor
    // used by NPCs. Use the same concrete retail identity/backlinks as the
    // established native player owner reader; a base actor must not qualify.
    return c.actor&&c.state&&read(c.actor,&vft,8)&&vft==base+0x3A80610&&
        read(c.actor+0x210,&state,8)&&state==c.state&&
        read(c.state+0xCE0,&actor,8)&&actor==c.actor&&
        read(c.actor+0x1F8,&player,8)&&player==c.key.player&&
        read(player,&playerVft,8)&&playerVft==base+0x3AA4B98&&
        read(player+0x278,&playerActor,8)&&playerActor==c.actor;
}
template<class Read> bool Qualify(Read read,std::uintptr_t base,Context& c) noexcept {
    if(!base||!motion59c4::valid(c.key)||!c.now||!c.onset||c.now<c.onset||
       c.deadline<=c.now||c.deadline<=c.onset||c.deadline-c.onset>600||c.deadline-c.now>600||!std::isfinite(c.remaining)||
       c.remaining<=0.f||c.remaining>.600001f||c.defense<0||c.defense>5||
       c.oldPhase==4||!SameActor(read,base,c))return false;
    std::uintptr_t target=0,vft=0,back=0,state=0,opponent=0;
    if(!read(c.state+0xCF8,&target,8)||!target||target==c.actor||
       !read(target,&vft,8)||vft!=base+0x3E3FAC8||
       !read(target+0x210,&state,8)||!state||!read(state+0xCE0,&back,8)||back!=target||
       !read(state+0xCF8,&opponent,8)||opponent!=c.actor)return false;
    // Native range computation stays authoritative. No fabricated distance or
    // assumed weapon tip. Both properties have the exact byte-property type.
    const int reach=Bool(read,base,c.state,0x4B0),extended=Bool(read,base,c.state,0x4E0);
    if(reach<0||extended<0||(!reach&&!extended))return false;
    c.original=Bool(read,base,c.state,0x5A0);
    if(c.original<0)return false;
    c.target=target;c.targetState=state;return true;
}
template<class Read> bool SameThreat(Read read,std::uintptr_t base,const Context& c) noexcept {
    if(!SameActor(read,base,c))return false;
    std::uintptr_t target=0,state=0,back=0,vft=0,opponent=0;
    return read(c.state+0xCF8,&target,8)&&target==c.target&&
        read(target,&vft,8)&&vft==base+0x3E3FAC8&&
        read(target+0x210,&state,8)&&state==c.targetState&&
        read(state+0xCE0,&back,8)&&back==target&&
        read(state+0xCF8,&opponent,8)&&opponent==c.actor;
}
// Returning a native true unchanged is important: never acquire a lease over
// a value that was already native-owned. False->true is compare-restored only
// if the same actor allocation still owns this field and no native action won.
template<class Read,class Set> bool Begin(Read read,Set set,std::uintptr_t base,Context& c,bool& written) noexcept {
    written=false;if(!Qualify(read,base,c))return false;
    if(c.original)return true;
    written=true; // even failed readback may follow a successful native write
    return set(c.state,0x5A0,true)&&SameThreat(read,base,c)&&Bool(read,base,c.state,0x5A0)==1;
}
template<class Read,class Set> bool Finish(Read read,Set set,std::uintptr_t base,const Context& c,bool written,bool nativeAction) noexcept {
    if(!written||nativeAction)return true;
    if(!SameActor(read,base,c))return false; // do not write into a replacement allocation
    const int live=Bool(read,base,c.state,0x5A0);
    if(live<0)return false;
    if(live!=1)return true; // a synchronous native writer already took ownership
    return set(c.state,0x5A0,c.original!=0)&&Bool(read,base,c.state,0x5A0)==c.original;
}
}
