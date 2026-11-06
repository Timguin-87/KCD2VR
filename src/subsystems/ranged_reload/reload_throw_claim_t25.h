#pragma once
// SOURCE-ONLY T25/XR34 addendum. No hooks, native calls, clock/restore gates,
// lease expiry, latest-pose freshness requirement or second native executor.
#include "crossbow_native_xr34.h"
#include "../throw_drop/hand_claim_t24.h"
namespace kcd2vr::action_time_owner59c11 { bool CrossbowNativeAdmissionBusyXR34() noexcept; }
namespace kcd2vr::reload_throw_claim_t25 {
namespace xr=crossbow_native_xr34;
enum class Admission : unsigned char { Unknown,Allowed,Denied };
// Use the ORIGINAL source actor/body/request, never current globals that could
// relabel an old cleanup owner. This is new nomination/presentation only.
inline bool workingHandUnclaimed(const xr::Source& s) noexcept {
    if(s.primary<0||s.primary>1||!s.player||!s.manager||!s.character||!s.body||!s.request)return false;
    return !hand_claim_t24::competing(1-s.primary,{s.player,s.manager,s.character,s.body,s.request});
}
struct DebtReads {
    std::uint64_t beforeLease=0,afterLease=0;
    bool beforeCleanup=false,afterCleanup=false,firstKnown=false,secondKnown=false;
    bool beforeBoundaryBusy=false,afterBoundaryBusy=false;
    xr::Status first{},second{};
};
inline bool sameDebt(const xr::Status& a,const xr::Status& b) noexcept {
    return xr::sameOwner(a.owner,b.owner)&&a.clockOwned==b.clockOwned&&a.phase==b.phase&&
        a.reason==b.reason&&a.started==b.started&&a.firstPoseDeadline==b.firstPoseDeadline&&
        a.placeTool==b.placeTool&&a.placeBolt==b.placeBolt&&a.sourceEstablished==b.sourceEstablished;
}
inline Admission classifyReloadDebt(int side,std::uintptr_t player,const DebtReads& r) noexcept {
    if(side<0||side>1||!player||r.beforeBoundaryBusy||r.afterBoundaryBusy)return Admission::Unknown;
    // Explicit idle fastpath. A withdrawn source/disabled feature is NOT the
    // same as absence: any actual cleanup debt takes the raw snapshot route.
    if(!r.beforeLease&&!r.afterLease&&!r.beforeCleanup&&!r.afterCleanup)return Admission::Allowed;
    if(r.beforeLease!=r.afterLease||r.beforeCleanup!=r.afterCleanup||!r.firstKnown||!r.secondKnown||
       !sameDebt(r.first,r.second))return Admission::Unknown;
    const auto& status=r.second;const auto& o=status.owner;const auto& s=o.source;
    if(!o.serial||!s.player||!s.manager||!s.character||!s.body||!s.request||s.primary<0||s.primary>1||
       static_cast<unsigned>(status.phase)>static_cast<unsigned>(xr::Phase::NativeTransitionD80)||
       (r.afterLease&&r.afterLease!=o.serial)||(!status.clockOwned&&!r.afterCleanup))return Admission::Unknown;
    // Existing public T22 getter has no qualified caller manager. A different
    // player alone cannot prove foreign retirement; conservatively UNKNOWN.
    if(player!=s.player)return Admission::Unknown;
    // Same actor retains ORIGINAL hand debt even through body/request/character
    // changes, phase Retiring, revoked lease, or no active lease + cleanup true.
    return side==1-s.primary?Admission::Denied:Admission::Allowed;
}
inline Admission observeReloadDebt(int side,std::uintptr_t player) noexcept {
    if(side<0||side>1||!player)return Admission::Unknown;
    DebtReads r{};
    r.beforeBoundaryBusy=action_time_owner59c11::CrossbowNativeAdmissionBusyXR34();
    r.beforeLease=xr::activeNativeLease.load(std::memory_order_acquire);
    r.beforeCleanup=xr::cleanupInterested.load(std::memory_order_acquire);
    if(!r.beforeLease&&!r.beforeCleanup){
        r.afterCleanup=xr::cleanupInterested.load(std::memory_order_acquire);
        r.afterLease=xr::activeNativeLease.load(std::memory_order_acquire);
        r.afterBoundaryBusy=action_time_owner59c11::CrossbowNativeAdmissionBusyXR34();
        if(!r.beforeBoundaryBusy&&!r.afterBoundaryBusy&&!r.afterLease&&!r.afterCleanup)return Admission::Allowed;
    }
    r.firstKnown=xr::statusMailbox.read(r.first);r.secondKnown=xr::statusMailbox.read(r.second);
    r.afterCleanup=xr::cleanupInterested.load(std::memory_order_acquire);
    r.afterLease=xr::activeNativeLease.load(std::memory_order_acquire);
    r.afterBoundaryBusy=action_time_owner59c11::CrossbowNativeAdmissionBusyXR34();
    return classifyReloadDebt(side,player,r);
}
}
