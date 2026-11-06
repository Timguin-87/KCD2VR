#pragma once
// Published only by the existing exact Henry native attachment callback after
// ReadSmithTool36 succeeds and its lifecycle/character manager are rechecked.
// This mailbox records membership; it is not an item lookup, attach request,
// contact witness or permission to assign a forged result.
#include "../../core/snapshot_mailbox.h"
#include "../crafting/options_d67.h"
#include "../../vrik/smith_tool36.h"
#include "source_grip_h42.h"
#include "../../vrik/minigame_hands45.h"
#include <cstdint>
#include <cmath>
#include <algorithm>
namespace kcd2vr::subsystems::smithing::bridge67 {
struct NativeTool {
    smith_tool36::Binding binding{};
    std::uint64_t tick=0,poseEpoch=0,modeEpoch=0;
    bool joined=false;
};
inline core::SnapshotMailbox<NativeTool> tools[2];
// PRE-acquisition station sources; never masquerade as joined NativeTool.
inline core::SnapshotMailbox<source_h42::Frame> sourcesH42;
// The exact left native tongs/workpiece relationship owns the iron's fixture
// placement. Hammer contact/pose remains a separate tracked right-hand owner.
inline bool fixtureOwnedH21(const smith_tool36::Binding& b) noexcept {
    const auto mode=minigame_hands45::modeFor(minigame_hands45::Smithing);
    // D72: 6DOF semi support belongs to the workpiece/anvil surface solver.
    // It never leases the complete native wrist transform, even while resting.
    return (mode==minigame_hands45::SemiPassenger||mode==minigame_hands45::Gesture)&&
        bool(b)&&b.side==1&&(b.tool==int(smith_tool36::Tool::Tongs)||b.tool==int(smith_tool36::Tool::Workpiece));
}
// Lower-mode pose guidance is staged in the existing committed body packet.
// These are new pose tuning values, not the 20 cm source-selection radius:
// enter within 8 cm, release beyond 12 cm, blend over 150/200 ms. Native
// item membership/finger grip survives. Mode3 uses its separate workpiece
// surface support; it never enters this authored wrist-pose guidance.
struct FixturePose {
    smith_tool36::Binding binding{};
    std::uint64_t poseEpoch=0,sourceEpoch=0,modeEpoch=0,sample=0,tick=0;
    double tracking[3]{};
    double gap=0;
    float weight=0;
    bool valid=false,engaged=false,approachArmed=false;
};
inline bool fixtureJoined(const FixturePose& p,const smith_tool36::Binding& b,
    std::uint64_t poseEpoch,std::uint64_t modeEpoch,std::uint64_t sample,
    std::uint64_t now) noexcept {
    return fixtureOwnedH21(b)&&p.valid&&p.poseEpoch==poseEpoch&&p.modeEpoch==modeEpoch&&
        p.sample==sample&&p.tick&&now>=p.tick&&now-p.tick<=80&&
        smith_tool36::sameGripIdentity67(p.binding,b);
}
inline FixturePose fixtureStep(const FixturePose& old,const smith_tool36::Binding& b,
    std::uint64_t poseEpoch,std::uint64_t sourceEpoch,std::uint64_t modeEpoch,
    std::uint64_t sample,std::uint64_t tick,double gap,const double tracking[3]) noexcept {
    FixturePose p{};
    if(!fixtureOwnedH21(b)||!poseEpoch||!sourceEpoch||!modeEpoch||!sample||!tick||
       !std::isfinite(gap)||gap<0||!std::isfinite(tracking[0])||
       !std::isfinite(tracking[1])||!std::isfinite(tracking[2]))return p;
    p.binding=b;p.poseEpoch=poseEpoch;p.sourceEpoch=sourceEpoch;p.modeEpoch=modeEpoch;
    p.sample=sample;p.tick=tick;p.gap=gap;p.valid=true;
    std::copy_n(tracking,3,p.tracking);
    const bool continuation=old.valid&&old.poseEpoch==poseEpoch&&old.sourceEpoch==sourceEpoch&&
        old.modeEpoch==modeEpoch&&smith_tool36::sameGripIdentity67(old.binding,b)&&
        sample>=old.sample&&tick>=old.tick&&tick-old.tick<=80;
    if(!continuation){p.engaged=gap<=.08;p.approachArmed=gap>.08;return p;}
    if(sample==old.sample)return old; // paired/repeated pose consumer: no second blend step
    p.weight=old.weight;p.engaged=old.engaged;p.approachArmed=old.approachArmed;
    double moved2=0;for(int a=0;a<3;++a){const double d=tracking[a]-old.tracking[a];moved2+=d*d;}
    if(p.engaged&&gap>.12){p.engaged=false;p.approachArmed=true;}
    if(!p.engaged){
        if(gap>.08)p.approachArmed=true;
        // A moving authored wrist alone cannot recapture the released hand.
        if(p.approachArmed&&old.gap>.08&&gap<=.08&&moved2>.00035*.00035){
            p.engaged=true;p.approachArmed=false;
        }
    }
    const float dt=float(tick-old.tick)*.001f;
    const float target=p.engaged?1.f:0.f;
    p.weight+=std::clamp(target-p.weight,-dt/.2f,dt/.15f);
    return p;
}
// D83R2 smith: the current workpiece record's native map. Retail
// C_BlacksmithWorkpiece property registration 1813BB529..1813BB681 binds
// InitialPosition +38, MapSize +40 and MapOffset +48 (ebx=5, r9d=rbx+33/3B/43).
// Libs/Tables/minigame/BlacksmithWorkpieces.xml: sword and longsword use
// MapSize "0.7,0" (one axis); axe "0.18,0.3" and horseshoe "0.2,0.2" (two).
// Holder model+2F8 (vft 3A7BA18) keeps the record at +18; the D80 capture's
// SMITH_CONTACT68 rows read mapSize=(0.1800,0.3000) there for the axe.
// A non-zero second map axis is the game's own sideways workpiece axis.
// Read only; it grants no attachment, hit or placement permission.
struct WorkpieceMapD83R2 {
    std::uintptr_t instance=0,model=0,holder=0,definition=0;
    std::uint64_t lifetime=0,tick=0;
    float size[2]{};
    const char* refusal="NOT_READ";
    bool valid=false;
};
inline core::SnapshotMailbox<WorkpieceMapD83R2> workpieceMapD83R2;
template<class Reader> bool readWorkpieceMapD83R2(Reader&& rd,std::uintptr_t base,
    const smith_tool36::Binding& b,std::uint64_t now,WorkpieceMapD83R2& out) noexcept {
    out={};out.instance=b.instance;out.lifetime=b.lifetime;out.model=b.model;out.tick=now;
    const auto no=[&](const char* why){out.refusal=why;out.valid=false;return false;};
    if(!base||!b||!b.lifetime||!b.model)return no("SMITH_BINDING_INCOMPLETE");
    std::uintptr_t vft=0,holder=0,definition=0,again=0;float x=0,y=0;
    if(!rd(b.model,vft)||vft!=base+smith_tool36::kModelVft)return no("SMITH_MODEL_CLASS");
    if(!rd(b.model+smith_tool36::kWorkpieceSlot,holder)||!holder||
       !rd(holder,vft)||vft!=base+smith_tool36::kWorkpieceVft)return no("WORKPIECE_HOLDER_CLASS");
    if(!rd(holder+0x18,definition)||!definition)return no("WORKPIECE_RECORD_MISSING");
    if(!rd(definition+0x40,x)||!rd(definition+0x44,y))return no("WORKPIECE_MAP_UNREADABLE");
    if(!std::isfinite(x)||!std::isfinite(y)||x<0.f||y<0.f||x>10.f||y>10.f||!(x>0.f||y>0.f))
        return no("WORKPIECE_MAP_OUT_OF_RANGE");
    if(!rd(b.model+smith_tool36::kWorkpieceSlot,again)||again!=holder||
       !rd(holder+0x18,again)||again!=definition)return no("WORKPIECE_CHANGED_DURING_READ");
    out.holder=holder;out.definition=definition;out.size[0]=x;out.size[1]=y;
    out.refusal="NONE";out.valid=true;return true;
}
// -1 unknown (why says the reason), 0 one-axis piece (blades), 1 sideways axis.
inline int sidewaysD83R2(const WorkpieceMapD83R2& m,std::uintptr_t instance,
    std::uint64_t lifetime,const char*& why) noexcept {
    if(!instance||!lifetime||m.instance!=instance||m.lifetime!=lifetime){why="NOT_READ_FOR_THIS_STATION";return -1;}
    if(!m.valid){why=m.refusal?m.refusal:"NOT_READ";return -1;}
    why="NONE";return m.size[1]>.01f?1:0;
}
inline bool qualified(const NativeTool& t,std::uint64_t now,int side,std::uintptr_t instance,
    std::uint64_t lifetime,std::uint64_t poseEpoch,std::uint64_t modeEpoch) noexcept {
    if(side<0||side>1||!instance||!lifetime||!poseEpoch||!modeEpoch||!t.joined||
       !t.tick||now<t.tick||now-t.tick>80||t.poseEpoch!=poseEpoch||t.modeEpoch!=modeEpoch)return false;
    const auto& b=t.binding;
    if(!b||b.instance!=instance||b.lifetime!=lifetime||b.side!=side||!b.player||!b.character||
       !b.attachment||!b.model||!b.object||!b.sourceEntity||b.phase<0||b.phase>20)return false;
    // Current phase is independently rechecked by the native input/child lease.
    // A native 11->14 ack can precede the next attachment callback; rejecting an
    // otherwise same <=80ms tool proof just for that phase would drop the stroke.
    return side==0?(b.tool==int(smith_tool36::Tool::Hammer)&&b.key!=0):
        (b.tool==int(smith_tool36::Tool::Tongs)?b.key!=0:b.tool==int(smith_tool36::Tool::Workpiece));
}
}
