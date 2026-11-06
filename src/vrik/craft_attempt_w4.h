#pragma once
// W4 crafting attempt chain (Claude, 25 Oct 2026). Miki: "F5 rung 3 should say WHY it does not work,
// what is missing, not only no grip". One attempt = one physical Grip squeeze at an alchemy/smithing
// station. Each frame the owner reports which stages this hand reached; when the squeeze ends (release
// or 4 s) the FIRST missing stage is the answer. Pure, no native reads, no mailbox, no allocation.
// Stages in causal order:
//  RAW_INPUT          the controller squeeze itself (grip >= .55)
//  WORKSTATION_SAMPLE the tracked palm for this hand is valid and fresh in the station frame
//  NOMINATION         an item of the native table is inside this hand's reach
//  NATIVE_SELECTION   the game's own selection is that same item
//  A_DISPATCH         the contextual A (alch_use / native xi_a) was sent once
//  NATIVE_ACTION      the game started an action for it
//  ATTACH             the item is attached to this hand natively
//  WORK               the native work (grind/pour/forge) is running
//  RELEASE            the squeeze ended after the work (return/put handled natively)
#include <cstdint>
#include <cstdio>

namespace kcd2vr::craft_attempt_w4 {
enum class Stage : unsigned {RawInput=0,WorkstationSample,Nomination,NativeSelection,ADispatch,NativeAction,Attach,Work,Release,Count};
inline const char* name(Stage s) noexcept {
    constexpr const char* n[]{"RAW_INPUT","WORKSTATION_SAMPLE","NOMINATION","NATIVE_SELECTION","A_DISPATCH",
        "NATIVE_ACTION","ATTACH","WORK","RELEASE","COMPLETE"};
    const auto i=unsigned(s);return i<=unsigned(Stage::Count)?n[i]:"UNKNOWN";
}
struct Observation {
    float grip=0;bool sample=false,nominated=false,nativeKnown=false,dispatched=false,action=false,attached=false,work=false;
    std::uint32_t nominatedEntity=0,nativeEntity=0;int nominatedSlot=-1,nativeSlot=-1;double gap=-1;
};
struct Report {
    std::uint64_t id=0,startTick=0,endTick=0;int side=-1;unsigned reached=0;Stage firstMissing=Stage::Count;
    std::uint32_t nominatedEntity=0,nativeEntity=0;int nominatedSlot=-1,nativeSlot=-1;double gap=-1;bool timedOut=false;
};
inline constexpr std::uint64_t kMaxAttemptMs=4000;
// The first stage (in causal order) that was never observed during this squeeze.
inline Stage firstMissing(unsigned reached) noexcept {
    for(unsigned i=0;i<unsigned(Stage::Count);++i)if(!(reached&(1u<<i)))return Stage(i);
    return Stage::Count;
}
struct Tracker {
    struct Live {bool active=false,down=false;std::uint64_t id=0,start=0;unsigned reached=0;
        std::uint32_t nominatedEntity=0,nativeEntity=0;int nominatedSlot=-1,nativeSlot=-1;double gap=-1;} live[2]{};
    std::uint64_t serial=0,finished=0,complete=0;
    void reset() noexcept {live[0]={};live[1]={};}
    // true -> out holds a finished attempt for this side
    bool step(int side,const Observation& o,std::uint64_t now,Report& out) noexcept {
        out={};if(side<0||side>1)return false;auto& a=live[side];
        const bool down=o.grip>=(a.down?.35f:.55f);
        bool ended=false,timedOut=false;
        if(down&&!a.down){a={};a.active=true;a.down=true;a.id=++serial;a.start=now;}
        else if(!down&&a.down){a.down=false;ended=a.active;}
        if(a.active&&a.down&&now>=a.start&&now-a.start>kMaxAttemptMs){ended=true;timedOut=true;}
        if(a.active){
            unsigned r=a.reached|1u; // a live squeeze is RAW_INPUT
            const bool s=o.sample,n=s&&o.nominated,sel=n&&o.nativeKnown&&o.nativeEntity==o.nominatedEntity&&o.nativeSlot==o.nominatedSlot;
            if(s)r|=1u<<unsigned(Stage::WorkstationSample);
            if(n){r|=1u<<unsigned(Stage::Nomination);a.nominatedEntity=o.nominatedEntity;a.nominatedSlot=o.nominatedSlot;a.gap=o.gap;}
            if(o.nativeKnown){a.nativeEntity=o.nativeEntity;a.nativeSlot=o.nativeSlot;}
            if(sel)r|=1u<<unsigned(Stage::NativeSelection);
            // later stages only count on top of their causes; a stray native action is not ours
            if(sel&&o.dispatched)r|=1u<<unsigned(Stage::ADispatch);
            if((r>>unsigned(Stage::ADispatch))&1u){
                if(o.action)r|=1u<<unsigned(Stage::NativeAction);
                if(o.attached)r|=1u<<unsigned(Stage::Attach);
                if(o.work)r|=1u<<unsigned(Stage::Work);
            }
            if(ended&&!timedOut&&((r>>unsigned(Stage::Work))&1u))r|=1u<<unsigned(Stage::Release);
            a.reached=r;
        }
        if(!ended)return false;
        out.id=a.id;out.startTick=a.start;out.endTick=now;out.side=side;out.reached=a.reached;out.firstMissing=firstMissing(a.reached);
        out.nominatedEntity=a.nominatedEntity;out.nativeEntity=a.nativeEntity;out.nominatedSlot=a.nominatedSlot;out.nativeSlot=a.nativeSlot;
        out.gap=a.gap;out.timedOut=timedOut;++finished;if(out.firstMissing==Stage::Count)++complete;
        const bool stillDown=a.down;a={};a.down=stillDown; // a timed-out squeeze must be released before the next attempt
        return true;
    }
};
// One short visible line (<= 96 chars, no quotes) for F5 rung 3. kind: "alchemy"/"smithing".
inline void why(const Report& r,const char* kind,const char* selector,char* buf,std::size_t n) noexcept {
    if(!buf||!n)return;const char hand=r.side?'L':'R';
    switch(r.firstMissing){
    case Stage::Count:std::snprintf(buf,n,"%s %c: grab chain complete",kind,hand);break;
    case Stage::WorkstationSample:std::snprintf(buf,n,"%s %c: hand not tracked at the station",kind,hand);break;
    case Stage::Nomination:std::snprintf(buf,n,"%s %c: no item within reach of this hand",kind,hand);break;
    case Stage::NativeSelection:
        if(r.nativeEntity)std::snprintf(buf,n,"%s %c: hand %u, game picked %u (%s)",kind,hand,r.nominatedEntity,r.nativeEntity,selector?selector:"?");
        else std::snprintf(buf,n,"%s %c: hand %u, game picked nothing (%s)",kind,hand,r.nominatedEntity,selector?selector:"?");
        break;
    default:std::snprintf(buf,n,"%s %c: stopped before %s%s",kind,hand,name(r.firstMissing),r.timedOut?" (4s)":"");break;
    }
}
}
