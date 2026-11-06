#pragma once
#include "subsystems/interactions/washing_gesture_d59.h"
#include "subsystems/interactions/native_passenger_a34.h"
#include "core/snapshot_mailbox.h"
#include "native_script_t2.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <cstdio>
#include <cstring>
namespace kcd2vr::washing_d59 {
using CurrentReader=bool(*)(const Frame&,std::uint64_t) noexcept;
inline std::atomic<bool> enabled{false};
inline std::atomic<std::uint64_t> modeEpoch{1};
inline std::atomic<CurrentReader> currentReader{nullptr};
inline core::SnapshotMailbox<Frame> frames;
inline std::atomic<bool> intent{false},active{false},latched{false},frameAllowed{false};
inline std::atomic<std::uint32_t> nominee{0};inline std::atomic<std::uint64_t> candidateEpoch{1};
inline void SetCurrentReader(CurrentReader f) noexcept {currentReader.store(f,std::memory_order_release);}
inline bool Enabled() noexcept {return enabled.load(std::memory_order_acquire);}
inline void SetEnabled(bool on) noexcept {native_passenger_a34::trackedWashD72.store(on,std::memory_order_release);if(enabled.exchange(on,std::memory_order_acq_rel)!=on)modeEpoch.fetch_add(1,std::memory_order_acq_rel);}
inline void Publish(const Frame& f) noexcept {
    (void)frames.publish(f);
    const auto target=f.allowed&&(f.openEmptyMask&3)==3?f.target:0u;
    if(nominee.exchange(target,std::memory_order_acq_rel)!=target)candidateEpoch.fetch_add(1,std::memory_order_acq_rel);
    intent.store(target!=0,std::memory_order_release);
    frameAllowed.store(f.allowed&&(f.openEmptyMask&3)==3,std::memory_order_release);
}
inline bool Wanted() noexcept {return active.load(std::memory_order_acquire)||(Enabled()&&(intent.load(std::memory_order_acquire)||(latched.load(std::memory_order_acquire)&&frameAllowed.load(std::memory_order_acquire))));}
inline void Trace(const char* edge,std::uint64_t serial,std::uint32_t target,std::uint64_t now,const char* result) noexcept {
    if(!diagnostics::capture168::gate.active(diagnostics::capture168::Domain::Interaction,now))return;
    char line[320]{};std::snprintf(line,sizeof(line),"D59 F5_WASH edge=%s serial=%llu target=%u result=%s recognition=AUTHORED_TUB_TRIGGER_VOLUME cleaning=NATIVE_3300MS fade=NATIVE_TWO_EVENTS camera=UNCHANGED reward=UNCLAIMED",
        edge,(unsigned long long)serial,target,result);diagnostic_trace::Write(line);
}
inline void TraceGeometry(const Frame& f,const Volume& v,std::uint64_t serial,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::gate.active(diagnostics::capture168::Domain::Interaction,now))return;
    char line[640]{};std::snprintf(line,sizeof(line),"D59 F5_WASH_VOLUME serial=%llu target=%u so=%u bounds=(%.3f,%.3f,%.3f)/(%.3f,%.3f,%.3f) palmR=(%.3f,%.3f,%.3f) palmL=(%.3f,%.3f,%.3f) hmd=(%.3f,%.3f,%.3f) body=%llu tracking=%llu policy=%llu waterMesh=UNCLAIMED",
        (unsigned long long)serial,v.target,v.smartObject,v.lo[0],v.lo[1],v.lo[2],v.hi[0],v.hi[1],v.hi[2],
        f.palm[0][0],f.palm[0][1],f.palm[0][2],f.palm[1][0],f.palm[1][1],f.palm[1][2],f.hmd[0],f.hmd[1],f.hmd[2],
        (unsigned long long)f.body,(unsigned long long)f.tracking,(unsigned long long)f.policy);diagnostic_trace::Write(line);
}
inline void Service(const native_script_t2::Context& ctx) noexcept {
    // Only this original-first native-script callback owns gesture/service state.
    static Gesture gesture{};static Volume volume{};static Frame owner{};
    static std::uint64_t serials=0,serial=0,lastProbe=0,seenMode=0,vm=0,nextProbe=0;
    static std::uint32_t retryTarget=0,retryPlayer=0;static unsigned failedProbes=0;static std::uint64_t seenCandidate=0;static bool dippedReported=false,spent=false;static Frame spentOwner{};static Volume spentVolume{};static std::uint64_t withdrawn=0,lastWithdrawSample=0;
    const auto mode=modeEpoch.load(std::memory_order_acquire);
    auto cancel=[&]() noexcept {
        if(serial&&ctx.Current()){
            char in[96]{},out[96]{};std::snprintf(in,sizeof(in),"CANCEL|%llu|%u|%u",(unsigned long long)serial,owner.player,volume.target);
            (void)ctx.CallPacket("kcd2vr_wash_d59",in,out,sizeof(out));
        }
        gesture.Reset();volume={};owner={};serial=lastProbe=0;dippedReported=false;active.store(false,std::memory_order_release);
    };
    if(!ctx.Current())return;
    if(mode!=seenMode||ctx.generation!=vm){cancel();seenMode=mode;vm=ctx.generation;failedProbes=0;nextProbe=0;withdrawn=lastWithdrawSample=0;}
    const auto candidate=candidateEpoch.load(std::memory_order_acquire);
    if(candidate!=seenCandidate){failedProbes=0;nextProbe=0;seenCandidate=candidate;}
    Frame f{};const auto reader=currentReader.load(std::memory_order_acquire);
    if(!Enabled()||!frames.read(f)||!Current(f,ctx.now)||!reader||!reader(f,ctx.now)){
        withdrawn=lastWithdrawSample=0;
        if(serial){Trace("CANCEL",serial,volume.target,ctx.now,"OWNER_TRACKING_HAND_OR_POLICY_LOST");cancel();}return;
    }
    if(spent){
        // A committed/unknown native call cannot rearm merely because native
        // action ownership temporarily hid the free-hand frame. Retain only a
        // local consumed-gesture latch; no native lease/effect is held here.
        if(serial)cancel();
        const bool same=f.player==spentOwner.player&&f.actor==spentOwner.actor&&f.character==spentOwner.character&&
            f.body==spentOwner.body&&f.tracking==spentOwner.tracking;
        if(!same){spent=false;withdrawn=0;latched.store(false,std::memory_order_release);active.store(false,std::memory_order_release);return;}
        // Wanted may sleep through unavailable frames, so continuity must be
        // checked from the last observed good sample, not elapsed wall time.
        if(!lastWithdrawSample||!Fresh(lastWithdrawSample,ctx.now,80))withdrawn=0;
        lastWithdrawSample=f.tick;
        const bool outside=!InScoopVolume(f.palm[0],spentVolume)&&!InScoopVolume(f.palm[1],spentVolume)&&
            !NearFace(f.palm[0],f)&&!NearFace(f.palm[1],f);
        if(!outside)withdrawn=0;else if(!withdrawn)withdrawn=ctx.now;
        if(withdrawn&&ctx.now>=withdrawn&&ctx.now-withdrawn>=250){spent=false;withdrawn=0;latched.store(false,std::memory_order_release);active.store(false,std::memory_order_release);}
        return;
    }
    if(serial&&!SameOwner(owner,f)){cancel();return;}
    if(!serial){
        if(f.target!=retryTarget||f.player!=retryPlayer){retryTarget=f.target;retryPlayer=f.player;failedProbes=0;nextProbe=0;}
        if(!f.target||!Fresh(f.targetTick,ctx.now,120)||failedProbes>=8||ctx.now<nextProbe)return;
        if(++serials>9007199254740991ull)return;
        serial=serials;owner=f;volume.target=f.target;active.store(true,std::memory_order_release);
    }
    if(!gesture.consumed&&(!lastProbe||ctx.now<lastProbe||ctx.now-lastProbe>=120)){
        char in[128]{},out[320]{};std::snprintf(in,sizeof(in),"PROBE|%llu|%u|%u",(unsigned long long)serial,owner.player,volume.target);
        if(!ctx.CallPacket("kcd2vr_wash_d59",in,out,sizeof(out))){++failedProbes;nextProbe=ctx.now+250;Trace("PROBE",serial,volume.target,ctx.now,"VM_TRANSPORT_FAILED");cancel();return;}
        Volume next{};unsigned long long got=0;int used=0;
        const int fields=std::sscanf(out,"OK|%llu|READY|%u|%u|%f|%f|%f|%f|%f|%f%n",&got,&next.target,&next.smartObject,
            &next.lo[0],&next.lo[1],&next.lo[2],&next.hi[0],&next.hi[1],&next.hi[2],&used);
        next.tick=ctx.now;
        if(fields!=9||out[used]||got!=serial||next.target!=volume.target||!Valid(next)){
            ++failedProbes;nextProbe=ctx.now+250;Trace("CANCEL",serial,volume.target,ctx.now,out[0]?out:"NATIVE_WASH_ELIGIBILITY_OR_VOLUME_UNAVAILABLE");cancel();return;
        }
        if(lastProbe&&volume.smartObject!=next.smartObject){cancel();return;} // Includes unlinked <-> linked changes.
        volume=next;lastProbe=ctx.now;failedProbes=0;
        if(!gesture.begun){
            if(!gesture.Begin(f,volume,ctx.now)){cancel();return;}
            Trace("ARM",serial,volume.target,ctx.now,"NATIVE_TRIGGER_USE_ELIGIBLE");TraceGeometry(f,volume,serial,ctx.now);
        }
    }
    const Step step=gesture.Advance(f,volume,ctx.now);
    if(step==Step::Cancelled){Trace("CANCEL",serial,volume.target,ctx.now,"GESTURE_CONTEXT_ENDED");cancel();return;}
    if(step==Step::Dipped&&!dippedReported){dippedReported=true;Trace("DIP",serial,volume.target,ctx.now,"BOTH_OPEN_HANDS_ENTERED_AUTHORED_VOLUME");}
    if(step!=Step::Face)return;
    // Spend the one face episode before entering Lua. An error/unknown return
    // never retries the native handler. Lua independently consumes the serial.
    if(!Enabled()||mode!=modeEpoch.load(std::memory_order_acquire)||!reader(f,ctx.now)) {cancel();return;}
    char in[128]{},out[160]{};std::snprintf(in,sizeof(in),"COMMIT|%llu|%u|%u",(unsigned long long)serial,owner.player,volume.target);
    spent=true;spentOwner=f;spentVolume=volume;withdrawn=lastWithdrawSample=0;latched.store(true,std::memory_order_release);
    const bool ok=ctx.CallPacket("kcd2vr_wash_d59",in,out,sizeof(out));
    char expected[128]{};std::snprintf(expected,sizeof(expected),"OK|%llu|DISPATCHED",(unsigned long long)serial);
    Trace("FACE",serial,volume.target,ctx.now,ok&&!std::strcmp(out,expected)?"NATIVE_HANDLER_DISPATCHED":"NATIVE_RESULT_UNKNOWN_OR_REFUSED_NO_RETRY");
}
} // namespace kcd2vr::washing_d59
