#pragma once
// Version-qualified semantic intent. These are NOT native attack-type IDs.
#include "pose_math.h"
#include "fist_guard175.h"
#include "guard_parry176.h"
#include "native_parry55c2.h"
#include "native_master_window23.h"
#include "direct_counter39.h" // C39 DIRECT guard-out-of-stagger policy + counter receipts (pure)
#include "torch_offhand_d60.h" // bounded observation only; no torch combat authorization
#include <cstdint>
namespace kcd2vr::native_combat104 {
enum class Gesture : unsigned { Punch,Slash,Thrust,Pommel };
struct Intent {Gesture gesture=Gesture::Punch;int side=0,zone=0,inputIndex=0;int expectedType=-1;};
inline Intent intent(Gesture g,int side,pose_core::Vec3 velocity) noexcept {
    Intent i{};i.gesture=g;i.side=side;
    // Retail combat_attack_config: heavy directional swing, light high/low
    // direct attack, special context action. The engine chooses the real result.
    // A special request is NOT proof of a particular pommel animation.
    switch(g){
    case Gesture::Punch:i.inputIndex=0;i.zone=0;i.expectedType=5;break;
    case Gesture::Thrust:i.inputIndex=0;i.zone=5;i.expectedType=0;break;
    case Gesture::Pommel:i.inputIndex=2;i.zone=-1;i.expectedType=-1;break;
    case Gesture::Slash:
        i.inputIndex=1;i.expectedType=1;
        i.zone=std::abs(velocity.z)>std::abs(velocity.x)*1.25?0:(velocity.x>=0?1:2);break;
    }
    return i;
}
struct Observed {
    int phase=0,type=-1,input=-1,zone=-1,hand=2;
    // +2D8 is request/candidate state (939CD8), not a continuous strike window.
    bool requested=false,requestCandidate=false,blocking=false;
};
inline bool valid(Observed s) noexcept {
    return s.phase>=0&&s.phase<=0x7ff&&s.type>=-1&&s.type<=64&&s.input>=-1&&s.input<256&&
        s.zone>=-1&&s.zone<=5&&s.hand>=0&&s.hand<=2;
}
// H14: C53B99..BA3 publishes the native BlockParams input-class property
// into +270, then enters block preparation128; C55584 advances it to256.
// The observed7 is block metadata, NOT an attack sector. Keep valid() strict
// for direction/guard writes and classify only the captured native tuple.
inline bool blockMetadataH14(Observed s) noexcept {
    return (s.phase==128||s.phase==256)&&s.type==-1&&s.input==-1&&s.zone==7&&s.hand==1&&
        !s.requested&&!s.requestCandidate&&!s.blocking;
}
// R18 H15 line48975: the ORIGINAL prepare can coexist with a preparing
// native block row. Observation alone grants neither a zone nor new input.
inline bool pendingBlockInputR18(Observed s) noexcept {
    return s.phase==128&&s.type==-1&&s.input>=0&&s.input<=128&&s.zone==7&&s.hand==1&&
        s.requested&&!s.requestCandidate&&!s.blocking;
}
inline bool observedContextValidH14(Observed s) noexcept {return valid(s)||blockMetadataH14(s)||pendingBlockInputR18(s);}
inline bool changed(Observed a,Observed b) noexcept {
    return a.phase!=b.phase||a.type!=b.type||a.input!=b.input||a.zone!=b.zone||a.hand!=b.hand||
        a.requested!=b.requested||a.requestCandidate!=b.requestCandidate||a.blocking!=b.blocking;
}
inline bool entered(Observed before,Observed after) noexcept {
    return (!before.requestCandidate&&after.requestCandidate)||
        (before.phase!=after.phase&&(after.phase==4||after.phase==8));
}
} // namespace

namespace kcd2vr::native_combat104 {
// Optional finite-capture receipt from the SAME reads as NativeObserve104.
// Partial fields are observations, never safe admission or a second readback.
struct ReadReceipt59 {
    const char* why="NONE";unsigned offset=0,readMask=0;
    unsigned requested=0,candidate=0,blocking=0;
};
template<class Reader>
bool observe59(std::uintptr_t actor,std::uintptr_t state,Observed& s,Reader&& read,ReadReceipt59& receipt) noexcept {
    s={};receipt={};std::uintptr_t current=0,back=0;
    const auto fail=[&](const char* why,unsigned off){receipt.why=why;receipt.offset=off;return false;};
    const auto at=[&](std::uintptr_t base,unsigned offset,auto& value,unsigned bit){
        if(!read(reinterpret_cast<const void*>(base+offset),value))return fail("READ_FAILED",offset);
        receipt.readMask|=1u<<bit;return true;
    };
    if(!state)return fail("STATE_ABSENT",0);
    if(!at(actor,0x210,current,0))return false;
    if(current!=state)return fail("ACTOR_STATE_CHANGED",0x210);
    if(!at(state,0xCE0,back,1))return false;
    if(back!=actor)return fail("STATE_ACTOR_CHANGED",0xCE0);
    unsigned char requested=0,candidate=0,blocking=0;
    if(!at(state,0x38,s.phase,2)||!at(state,0x218,s.type,3)||!at(state,0x248,s.input,4)||
       !at(state,0x278,s.zone,5)||!at(state,0x1B8,s.hand,6))return false;
    if(!at(state,0x2A8,requested,7))return false;receipt.requested=requested;
    if(!at(state,0x2D8,candidate,8))return false;receipt.candidate=candidate;
    if(!at(state,0x308,blocking,9))return false;receipt.blocking=blocking;
    if(requested>1)return fail("BOOL_RANGE",0x2A8);
    if(candidate>1)return fail("BOOL_RANGE",0x2D8);
    if(blocking>1)return fail("BOOL_RANGE",0x308);
    s.requested=requested!=0;s.requestCandidate=candidate!=0;s.blocking=blocking!=0;
    if(observedContextValidH14(s))return true;
    if(s.phase<0||s.phase>0x7ff)return fail("TYPED_RANGE",0x38);
    if(s.type< -1||s.type>64)return fail("TYPED_RANGE",0x218);
    if(s.input< -1||s.input>=256)return fail("TYPED_RANGE",0x248);
    if(s.zone< -1||s.zone>5)return fail("TYPED_RANGE",0x278);
    return fail("TYPED_RANGE",0x1B8);
}
} // namespace kcd2vr::native_combat104
