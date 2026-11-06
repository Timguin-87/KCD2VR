#pragma once
#include "physical_weapon.h"

namespace kcd2vr::environment_fx178r22c1 {
// Visual/audio evidence, never attack or damage permission. The frame is the
// completed constrained weapon publication, not a raw controller or cached ray.
template<class Frame> bool scrape(const Frame& f,int side) noexcept {
    if(side<0||side>1||!f.input.wallSlide58)return false;
    const auto& o=f.output;const auto& motion=o.contactMotion169r1[side];
    // This is the accepted tangential checked path, never wallSlid58's old
    // back-out flag or accumulated controller pressure against a stationary tip.
    return o.wallScraped58[side]&&!o.wallBackout58[side]&&motion.valid&&
        std::isfinite(o.impactSpeed[side])&&o.impactSpeed[side]<=.5&&
        std::isfinite(motion.slipSpeed)&&motion.slipSpeed>=.05&&motion.slipSpeed<=20&&
        std::isfinite(motion.slipDistance)&&motion.slipDistance>=.0001&&motion.slipDistance<=.25;
}
template<class Frame> bool contactCandidate58(const Frame& f,int side) noexcept {
    if(side<0||side>1)return false;
    const auto& i=f.input;const auto& o=f.output;const auto& c=o.contact119[side];
    const auto& p=f.profiles[side];const auto& w=c.native172;
    const bool environment=w.body==npc_contact172::Body::Physical||w.body==npc_contact172::Body::Rigid||
        (w.body==npc_contact172::Body::Unknown&&w.mask==npc_contact172::StaticTerrain);
    return i.allowed&&i.contact&&i.haveWorld&&i.tick&&i.sample&&
        i.tracked[side]&&i.intentTracked[side]&&o.active[side]&&o.owned[side]&&o.blocked[side]&&
        p.key&&p.generation&&p.melee()&&c.known&&c.plane&&!c.limited115&&
        environment&&w.collider&&w.surfaceIdx>=0&&
        pose_core::finite(c.point)&&pose_core::finite(c.normal)&&
        pose_core::length(c.normal)>.9&&pose_core::length(c.normal)<1.1&&
        f.publishWorldDeltaM122<=.002&&f.publishWorldDeltaRad122<=.002&&
        std::isfinite(i.worldScale)&&i.worldScale>.1&&i.worldScale<10&&
        std::abs(double(f.world[7])-i.worldScale)<1e-5;
}
template<class Frame> bool candidate(const Frame& f,int side) noexcept {
    return contactCandidate58(f,side)&&std::isfinite(f.output.impactSpeed[side])&&
        (f.output.impactSpeed[side]>.5||scrape(f,side));
}
template<class Frame> bool current(const Frame& event,const Frame& live,int side,std::uint64_t now) noexcept {
    if(!candidate(event,side)||!event.player||!event.manager||!event.request||
       now<event.input.tick||now-event.input.tick>80||
       !live.input.allowed||!live.input.contact||
       !live.input.tracked[side]||!live.input.intentTracked[side]||
       !live.output.active[side]||!live.output.owned[side]||
       live.input.sample<event.input.sample||live.input.tick<event.input.tick||
       now<live.input.tick||now-live.input.tick>80||
       live.player!=event.player||live.manager!=event.manager||live.request!=event.request||
       live.input.epoch!=event.input.epoch||live.input.bodyEpoch!=event.input.bodyEpoch)return false;
    const auto& a=event.profiles[side];const auto& b=live.profiles[side];
    if(a.key!=b.key||a.generation!=b.generation||a.entity!=b.entity||a.classId!=b.classId)return false;
    if(scrape(event,side)){
        const auto& x=event.output.contact119[side];const auto& y=live.output.contact119[side];
        if(!candidate(live,side)||!scrape(live,side)||x.native172.collider!=y.native172.collider||
           x.native172.entityId!=y.native172.entityId||x.native172.surfaceIdx!=y.native172.surfaceIdx||
           pose_core::dot(x.normal,y.normal)<.98)return false;
    }
    return true;
}
// One side's copied owner/contact episode. Invalid/mode-off input retires it;
// no new owner inherits distance or the previous owner's cadence.
struct ScrapeGate58 {
    std::uintptr_t player=0,manager=0,collider=0;
    std::uint64_t request=0,epoch=0,bodyEpoch=0,key=0,generation=0,sample=0,tick=0,lastFx=0;
    std::uint32_t entity=0,target=0;int surface=-1;double distance=0;
    pose_core::Vec3 normal{};
    void reset() noexcept {*this={};}
    template<class Frame> bool update(const Frame& f,int side,std::uint64_t now) noexcept {
        if(!contactCandidate58(f,side)||!f.player||!f.manager||!f.request||
           !f.input.epoch||!f.input.bodyEpoch||now<f.input.tick||now-f.input.tick>80){reset();return false;}
        const auto& p=f.profiles[side];const auto& c=f.output.contact119[side];
        const auto fp=reinterpret_cast<std::uintptr_t>(f.player),fm=reinterpret_cast<std::uintptr_t>(f.manager);
        const bool same=player==fp&&manager==fm&&request==f.request&&epoch==f.input.epoch&&bodyEpoch==f.input.bodyEpoch&&
            key==p.key&&generation==p.generation&&entity==p.entity&&collider==c.native172.collider&&
            target==c.native172.entityId&&surface==c.native172.surfaceIdx&&pose_core::dot(normal,c.normal)>=.98;
        if(same&&sample==f.input.sample)return false;
        const bool continuous=same&&f.input.sample>sample&&f.input.tick>=tick&&f.input.tick-tick<=80;
        if(!continuous){reset();player=fp;manager=fm;request=f.request;epoch=f.input.epoch;bodyEpoch=f.input.bodyEpoch;
            key=p.key;generation=p.generation;entity=p.entity;collider=c.native172.collider;
            target=c.native172.entityId;surface=c.native172.surfaceIdx;normal=c.normal;}
        sample=f.input.sample;tick=f.input.tick;
        if(!scrape(f,side)){distance=0;return false;} // rest retains this owner's cadence
        distance=std::min(.25,distance+f.output.contactMotion169r1[side].slipDistance);
        if(distance<.004||(lastFx&&(tick<lastFx||tick-lastFx<140)))return false;
        distance=0;lastFx=tick;return true;
    }
};
// C63: a blade pressed against the same obstacle can produce another genuine
// inward controller-motion impact after the recoil cooldown. That is not a new
// environmental strike while the checked weapon never left contact. Permit
// only its first normal impact; the independent checked tangential scrape path
// above may continue. Invalid/tracking-lost/deferred frames cannot rearm it.
struct ImpactEpisode63 {
    std::uintptr_t player=0,manager=0;
    std::uint64_t request=0,epoch=0,bodyEpoch=0,key=0,generation=0,sample=0,tick=0;
    std::uint32_t entity=0;bool touching=false;
    void reset() noexcept {*this={};}
    template<class Frame> bool update(const Frame& f,int side,std::uint64_t now) noexcept {
        if(side<0||side>1)return false;
        const auto& i=f.input;const auto& o=f.output;const auto& p=f.profiles[side];
        if(!i.allowed||!i.contact||!i.haveWorld||!i.tick||!i.sample||!i.epoch||!i.bodyEpoch||
           !i.tracked[side]||!i.intentTracked[side]||!o.active[side]||!o.owned[side]||
           !f.player||!f.manager||!f.request||!p.key||!p.generation||!p.melee()||
           now<i.tick||now-i.tick>80)return false;
        const auto fp=reinterpret_cast<std::uintptr_t>(f.player),fm=reinterpret_cast<std::uintptr_t>(f.manager);
        const bool same=player==fp&&manager==fm&&request==f.request&&epoch==i.epoch&&bodyEpoch==i.bodyEpoch&&
            key==p.key&&generation==p.generation&&entity==p.entity;
        if(!same){reset();player=fp;manager=fm;request=f.request;epoch=i.epoch;bodyEpoch=i.bodyEpoch;
            key=p.key;generation=p.generation;entity=p.entity;}
        if(sample&&(i.sample<=sample||i.tick<tick))return false;
        sample=i.sample;tick=i.tick;
        const auto& c=o.contact119[side];
        const bool checked=c.known&&!c.limited115&&!o.sweepDeferred115[side]&&
            f.publishWorldDeltaM122<=.002&&f.publishWorldDeltaRad122<=.002&&
            std::isfinite(i.worldScale)&&i.worldScale>.1&&i.worldScale<10&&
            std::abs(double(f.world[7])-i.worldScale)<1e-5;
        if(!checked)return false;
        if(!o.blocked[side]){touching=false;return false;} // only a checked clear path releases this episode
        if(!contactCandidate58(f,side))return false; // NPC/unknown contacts do not start an environment episode
        const bool wasTouching=touching;touching=true;
        return !wasTouching&&candidate(f,side)&&!scrape(f,side);
    }
};
}
