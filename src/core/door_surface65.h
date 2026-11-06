#pragma once
#include "door_hold65.h"
#include <array>
#include <atomic>
#include <cmath>
#include <limits>

// Same-door eligibility is supplied by a native completed descriptor or the
// exact retained native operation. A world-query owner supplies the surface.
// Neither publication alone authorizes Use or animation ownership.
namespace kcd2vr::door_surface65 {
using Point=door19c4::Point;
inline constexpr float maxDistance=.20f;
// Native descriptor WORLD centre aims the first query (hint only), or the
// current palm direction when degenerate; then six world axes. All probes share the
// existing18-ray/1000us frame budget and start at the actual committed palm.
inline constexpr unsigned probeRayCount=7;
struct Request {
    std::uint32_t target=0;int side=-1;
    std::uint64_t press=0,tick=0,poseTick=0,poseEpoch=0,trackingEpoch=0;
    std::uint64_t sourceGeneration=0,settingsEpoch=0,experimentEpoch=0;
    std::uintptr_t player=0;Point palm{},nativePoint{};unsigned interactionMode64=2;
};
struct Witness {
    Request source{};std::uint64_t queryTick=0,sample=0;
    Point actualPalm{},point{},normal{};std::uint32_t entityId=0;
    std::uintptr_t opaqueOwner=0,opaqueCollider=0;float distance=0;bool valid=false;
};
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return door_hold65::fresh(tick,now);}
inline bool current(const Request& r,std::uint64_t now) noexcept {
    return r.target&&r.target<=16777215u&&r.side>=0&&r.side<2&&r.press&&now>=r.press&&
        r.player&&r.poseEpoch&&r.trackingEpoch&&r.sourceGeneration&&r.settingsEpoch&&r.interactionMode64==2&&
        fresh(r.tick,now)&&fresh(r.poseTick,now)&&door19c4::finite(r.palm)&&door19c4::finite(r.nativePoint);
}
inline bool same(const Request& r,const door_hold65::Hand& h) noexcept {
    return r.side==h.side&&r.press==h.press&&r.player==h.player&&r.poseEpoch==h.poseEpoch&&
        r.trackingEpoch==h.trackingEpoch&&r.sourceGeneration==h.sourceGeneration&&
        r.settingsEpoch==h.settingsEpoch&&r.experimentEpoch==h.experimentEpoch;
}
inline Request request(std::uint32_t target,const door_hold65::Hand& h,std::uint64_t nativeTick,Point nativePoint) noexcept {
    return {target,h.side,h.press,nativeTick,h.poseTick,h.poseEpoch,h.trackingEpoch,h.sourceGeneration,
        h.settingsEpoch,h.experimentEpoch,h.player,h.point,nativePoint,2};
}
// NativeDirectDoor54r1 payload+28 is already a transformed WORLD bounds
// centre, not a handle. Aim an actual occluded ray only; this never supplies
// hit/normal/entity/contact evidence and never authorizes native Use.
inline Point queryDirectionH24(const std::array<Request,8>& requests,unsigned count,Point palm,Point fallback) noexcept {
 Point direction=fallback;float nearest=std::numeric_limits<float>::infinity();
 for(unsigned i=0;i<count&&i<requests.size();++i){
  const auto delta=door19c4::sub(requests[i].nativePoint,palm);const auto d2=door19c4::dot(delta,delta);
  if(!door19c4::finite(delta)||!std::isfinite(d2)||d2<=1.e-8f||d2>=nearest)continue;
  const auto scale=maxDistance/std::sqrt(d2);direction={delta.x*scale,delta.y*scale,delta.z*scale};nearest=d2;
 }
 return direction;
}
struct Requests {std::array<Request,8> value{};};
struct Witnesses {std::array<Witness,8> value{};};
inline core::SnapshotMailbox<Requests> requestMailbox;
inline core::SnapshotMailbox<Witnesses> witnessMailbox;
inline std::atomic_flag requestBusy=ATOMIC_FLAG_INIT,witnessBusy=ATOMIC_FLAG_INIT;
inline bool offer(const Request& r,std::uint64_t now) noexcept {
    if(!current(r,now)||requestBusy.test_and_set(std::memory_order_acquire))return false;
    Requests table{};(void)requestMailbox.read(table);unsigned slot=0;
    for(unsigned n=0;n<table.value.size();++n){const auto& old=table.value[n];
        if(old.target==r.target&&old.side==r.side){slot=n;break;}
        if(!current(old,now)||old.tick<table.value[slot].tick)slot=n;
    }
    table.value[slot]=r;const bool ok=requestMailbox.publish(table);requestBusy.clear(std::memory_order_release);return ok;
}
inline unsigned requests(std::uint64_t now,std::uintptr_t player,std::uint64_t poseEpoch,int side,
                         std::array<Request,8>& out) noexcept {
    out={};Requests table{};unsigned count=0;if(!requestMailbox.read(table))return 0;
    for(const auto& r:table.value)if(current(r,now)&&r.player==player&&r.poseEpoch==poseEpoch&&r.side==side)out[count++]=r;
    return count;
}
inline bool current(const Witness& w,std::uint64_t now) noexcept {
    const auto d=door19c4::sub(w.point,w.actualPalm);const float distance2=door19c4::dot(d,d),normal2=door19c4::dot(w.normal,w.normal);
    return w.valid&&w.sample&&w.entityId==w.source.target&&w.opaqueOwner&&w.opaqueCollider&&
        current(w.source,now)&&fresh(w.queryTick,now)&&w.queryTick>=w.source.tick&&
        door19c4::finite(w.actualPalm)&&door19c4::finite(w.point)&&door19c4::finite(w.normal)&&
        std::isfinite(distance2)&&distance2>=0&&distance2<=maxDistance*maxDistance&&
        std::isfinite(normal2)&&normal2>.64f&&normal2<1.44f&&
        std::isfinite(w.distance)&&w.distance>=0&&w.distance<=maxDistance;
}
inline bool publish(const Witness& w) noexcept {
    if(!current(w,w.queryTick)||witnessBusy.test_and_set(std::memory_order_acquire))return false;
    Witnesses table{};(void)witnessMailbox.read(table);unsigned slot=0;
    for(unsigned n=0;n<table.value.size();++n){const auto& old=table.value[n];
        if(old.source.target==w.source.target&&old.source.side==w.source.side){slot=n;break;}
        if(!current(old,w.queryTick)||old.queryTick<table.value[slot].queryTick)slot=n;
    }
    table.value[slot]=w;const bool ok=witnessMailbox.publish(table);witnessBusy.clear(std::memory_order_release);return ok;
}
inline bool withdraw(int side,std::uint64_t sample,std::uint64_t queryTick) noexcept {
    if(side<0||side>1||!sample||!queryTick||witnessBusy.test_and_set(std::memory_order_acquire))return false;
    Witnesses table{};(void)witnessMailbox.read(table);
    for(auto& w:table.value)if(w.source.side==side&&w.sample<=sample&&w.queryTick<=queryTick)w={};
    const bool ok=witnessMailbox.publish(table);witnessBusy.clear(std::memory_order_release);return ok;
}
inline bool read(std::uint32_t target,const door_hold65::Hand& h,std::uint64_t now,Witness& out) noexcept {
    out={};Witnesses table{};if(!door_hold65::current(h,now)||h.grip<.30f||!h.press||!witnessMailbox.read(table))return false;
    float nearest=1e9f;for(const auto& w:table.value){if((target&&w.source.target!=target)||!same(w.source,h)||!current(w,now))continue;
        const auto delta=door19c4::sub(w.point,h.point);const float distance2=door19c4::dot(delta,delta);
        if(!std::isfinite(distance2)||distance2>maxDistance*maxDistance||distance2>=nearest)continue;
        out=w;out.distance=std::sqrt(distance2);nearest=distance2;
    }
    return out.valid;
}
}
