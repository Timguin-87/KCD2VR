#pragma once
#include "native_door19c4.h"
#include "snapshot_mailbox.h"
#include "door_pre_press66.h" // DS-F25: press-edge door token from the pre-press descriptor (file scope, both TUs)
#include "../diagnostics/door_receipt63.h"
#include <atomic>
#include <cstdint>

namespace kcd2vr::door_use46 {
// Input intent is separate from native acceptance. It identifies ONE squeeze,
// ONE currently offered ordinary door and the producing hand/reference space.
// No entity lookup, native call or animation write is performed by this module.
// Ordinary play and rung 2 share spatial acquisition/native acceptance.
// Keep the raw rung in source identity; only the Lua motion command is mapped.
// Rung 1 records ordinary input; rung 3 selects the legacy motion owner.
inline bool spatialMode(unsigned mode) noexcept {return mode==0u||mode==1u||mode==2u;}
inline bool inputMode(unsigned mode) noexcept {return spatialMode(mode)||mode==3u;}
inline bool inputAvailable(unsigned mode,bool comparison) noexcept {return inputMode(mode)&&(mode<=1u||comparison);}
inline int motionMode(int mode) noexcept {return mode==0?2:mode;}
inline std::atomic<bool> enabled{true};
inline std::atomic<std::uint64_t> settingsEpoch{1};
struct Intent {
    std::uint64_t token=0,tick=0,press=0,selectedTick=0,poseTick=0;
    std::uint64_t poseEpoch=0,sourceGeneration=0,trackingEpoch=0;
    std::uintptr_t player=0;std::uint32_t target=0;int side=-1;
    door19c4::Point motion{},point{};float distance=0;bool sent=false;
    std::uint64_t settingsEpoch=1;
    std::uint64_t physicalUseSerial62=0;
};
inline bool acceptedSourceCurrent62(const Intent&,std::uint64_t) noexcept;
inline core::SnapshotMailbox<Intent> intent;
inline std::atomic<std::uint64_t> nextToken{0},consumed{0};
inline void SetEnabled(bool value) noexcept {
    if(enabled.exchange(value,std::memory_order_acq_rel)!=value){
        settingsEpoch.fetch_add(1,std::memory_order_acq_rel);(void)intent.publish({});
    }
}
inline bool fresh(std::uint64_t tick,std::uint64_t now,unsigned limit=80) noexcept {
    return tick&&now>=tick&&now-tick<=limit;
}
inline bool valid(const Intent& i,std::uint64_t now) noexcept {
    return enabled.load(std::memory_order_acquire)&&i.token&&i.sent&&i.target&&i.target<=16777215u&&
        i.side>=0&&i.side<2&&i.player&&i.poseEpoch&&i.sourceGeneration&&i.trackingEpoch&&
        i.settingsEpoch==settingsEpoch.load(std::memory_order_acquire)&&fresh(i.tick,now)&&fresh(i.poseTick,now)&&
        door19c4::finite(i.motion)&&door19c4::finite(i.point);
}
inline bool read(Intent& out,std::uint64_t now) noexcept {
    return intent.read(out)&&valid(out,now)&&acceptedSourceCurrent62(out,now);
}
inline bool current(const Intent& expected,std::uint64_t now,Intent* latest=nullptr) noexcept {
    Intent i{};const bool ok=read(i,now)&&i.token==expected.token&&i.target==expected.target&&
        i.side==expected.side&&i.player==expected.player&&i.poseEpoch==expected.poseEpoch&&
        i.sourceGeneration==expected.sourceGeneration&&i.trackingEpoch==expected.trackingEpoch&&i.settingsEpoch==expected.settingsEpoch;
    if(ok&&latest)*latest=i;return ok;
}
inline bool consume(const Intent& expected,std::uint64_t now) noexcept {
    if(!current(expected,now))return false;
    auto seen=consumed.load(std::memory_order_acquire);
    // Monotonic input tokens prevent reentrant or repeated accepted native
    // callbacks from granting two door owners for one physical squeeze.
    while(seen<expected.token){
        if(consumed.compare_exchange_weak(seen,expected.token,std::memory_order_acq_rel))
            return current(expected,now);
    }
    return false;
}
struct Selection {
    bool ordinary=false;std::uint32_t target=0;int side=-1;
    std::uint64_t tick=0,poseTick=0,poseEpoch=0,sourceGeneration=0;
    std::uintptr_t player=0;door19c4::Point point{};float distance=0;bool direct64=false;
};
struct Input {
    bool available=false,held=false;int side=-1;
    std::uint64_t now=0,press=0,poseTick=0,poseEpoch=0,sourceGeneration=0,trackingEpoch=0;
    std::uintptr_t player=0;door19c4::Point motion{};bool waitingForRay49=false,scoped54=true;
    std::uint64_t experimentEpoch64=0;unsigned interactionMode64=2;
    bool waitingForPoint65=false;std::uint32_t resumeTarget65=0;
};
// A successful physical transport edge is distinct from both candidate ranking
// and native action acceptance. One immutable press may be joined later to the
// exact door the native interactor actually accepted, never to a guessed Use.
struct PhysicalUse62 {Input input{};std::uint64_t serial=0,settings=0;std::uint32_t target=0;};
struct AcceptedUse62 {PhysicalUse62 pulse{};Selection selection{};bool valid=false;};
inline core::SnapshotMailbox<Input> currentInput62;
inline core::SnapshotMailbox<PhysicalUse62> physicalUse62;
inline core::SnapshotMailbox<Intent> acceptedIntent62;
inline std::atomic<std::uint64_t> nextPhysical62{0},claimedPhysical62{0},retiredPhysical62{0};
inline void retirePhysical62(std::uint64_t serial) noexcept {
    auto seen=retiredPhysical62.load(std::memory_order_acquire);
    while(seen<serial&&!retiredPhysical62.compare_exchange_weak(seen,serial,std::memory_order_acq_rel)){}
}
// The native input queue can accept an original Use after250ms. Retain its
// identity only while the producer continuously validates this SAME squeeze.
// Every live input/pose/surface still has the original80ms freshness bound.
inline bool inputCurrent62(const Input& i,std::uint64_t now) noexcept {
    return enabled.load(std::memory_order_acquire)&&i.scoped54&&i.available&&i.held&&
        i.side>=0&&i.side<2&&i.press&&i.now>=i.press&&i.player&&i.poseEpoch&&i.sourceGeneration&&i.trackingEpoch&&
        fresh(i.now,now)&&fresh(i.poseTick,now)&&door19c4::finite(i.motion);
}
inline bool sourceSame62(const Input& a,const Input& b) noexcept {
    return a.side==b.side&&a.press==b.press&&a.player==b.player&&a.poseEpoch==b.poseEpoch&&
        a.sourceGeneration==b.sourceGeneration&&a.trackingEpoch==b.trackingEpoch&&a.experimentEpoch64==b.experimentEpoch64&&
        a.interactionMode64==b.interactionMode64;
}
inline bool pulseCurrent62(const PhysicalUse62& p,const Input& i,std::uint64_t now) noexcept {
    return p.serial&&p.serial>claimedPhysical62.load(std::memory_order_acquire)&&
        p.serial>retiredPhysical62.load(std::memory_order_acquire)&&
        p.settings==settingsEpoch.load(std::memory_order_acquire)&&
        inputCurrent62(p.input,p.input.now)&&inputCurrent62(i,now)&&
        sourceSame62(p.input,i)&&i.now>=p.input.now&&i.poseTick>=p.input.poseTick;
}
inline bool acceptedSourceCurrent62(const Intent& i,std::uint64_t now) noexcept {
    if(!i.physicalUseSerial62)return true;
    Input current{};PhysicalUse62 pulse{};
    return currentInput62.read(current)&&inputCurrent62(current,now)&&physicalUse62.read(pulse)&&
        pulse.serial==i.physicalUseSerial62&&pulse.serial>retiredPhysical62.load(std::memory_order_acquire)&&pulse.settings==i.settingsEpoch&&
        sourceSame62(pulse.input,current)&&current.side==i.side&&current.press==i.press&&
        current.player==i.player&&current.poseEpoch==i.poseEpoch&&current.sourceGeneration==i.sourceGeneration&&
        current.trackingEpoch==i.trackingEpoch;
}
inline bool captureAccepted62(const Selection& s,std::uint64_t now,AcceptedUse62& out) noexcept {
    out={};Input input{};PhysicalUse62 pulse{};Intent prior{};
    if((intent.read(prior)&&valid(prior,now))||!currentInput62.read(input)||!physicalUse62.read(pulse)||
       !pulseCurrent62(pulse,input,now)||!s.ordinary||!s.target||s.target>16777215u||s.side!=input.side||
       (pulse.target&&pulse.target!=s.target)||
       s.player!=input.player||s.poseEpoch!=input.poseEpoch||s.sourceGeneration!=input.sourceGeneration||
       !fresh(s.tick,now)||!fresh(s.poseTick,now)||!std::isfinite(s.distance)||s.distance<0||s.distance>.20f||
       !door19c4::finite(s.point))return false;
    out={pulse,s,true};return true;
}
inline bool previewAccepted62(const AcceptedUse62& expected,std::uint64_t now,Intent& out) noexcept {
    out={};Input input{};PhysicalUse62 pulse{};Intent prior{};
    if(!expected.valid||(intent.read(prior)&&valid(prior,now))||!physicalUse62.read(pulse)||pulse.serial!=expected.pulse.serial||
       !currentInput62.read(input)||!pulseCurrent62(pulse,input,now)||!sourceSame62(input,expected.pulse.input)||
       !fresh(expected.selection.tick,now)||!fresh(expected.selection.poseTick,now))return false;
    const auto& s=expected.selection;
    out.tick=input.now;out.press=pulse.input.press;out.selectedTick=s.tick;out.poseTick=input.poseTick;
    out.poseEpoch=input.poseEpoch;out.sourceGeneration=input.sourceGeneration;out.trackingEpoch=input.trackingEpoch;
    out.player=input.player;out.target=s.target;out.side=input.side;out.motion=input.motion;out.motion.y=0;out.point=s.point;out.distance=s.distance;
    out.sent=true;out.settingsEpoch=pulse.settings;out.physicalUseSerial62=pulse.serial;
    return true;
}
inline bool publishAccepted62(const AcceptedUse62& expected,std::uint64_t now,Intent& out) noexcept {
    out={};Intent i{};if(!previewAccepted62(expected,now,i))return false;
    const auto serial=expected.pulse.serial;
    auto seen=claimedPhysical62.load(std::memory_order_acquire);
    while(seen<serial){if(claimedPhysical62.compare_exchange_weak(seen,serial,std::memory_order_acq_rel)){
        i.token=nextToken.fetch_add(1,std::memory_order_acq_rel)+1;
        if(!valid(i,now)||!acceptedSourceCurrent62(i,now)||!intent.publish(i)||!acceptedIntent62.publish(i))return false;
        out=i;return true;
    }}
    return false;
}
struct Plan {bool owns=false,pulse=false;std::uint32_t target=0;int side=-1;};
struct Controller {
    Intent chosen{};std::uint64_t press=0,tick=0;int side=-1;
    bool classified=false,door=false,deferred=false,cancelled=false,nativeOnly54=false;
    Input classifiedSource64{};std::uint64_t classifiedSettings64=0;
    Input transportedSource64{};std::uint64_t transportedSettings64=0;bool transportRetired64=false;
    Input lastPointSourceH31{}; // last fully available point; raw-only gaps never refresh its clocks
    bool pointGapPendingH31=false; // never re-press generic held Use after a gap released it
    std::uint64_t resumedPress65=0;std::uint32_t resumedTarget65=0;int resumedSide65=-1;
    std::uint64_t transportedPress62=0,transportedSerial62=0;bool nativeHeld62=false;
    std::uint64_t pulsePress62=0;int pulseSide62=-1; // DS-F34: exactly one transport pulse per physical press+side, even on re-entrant updates before complete()
    void transport62(const Input& in,bool sentPulse,bool held) noexcept {
        const bool heldEdge=held&&!nativeHeld62;nativeHeld62=held;
        if(!in.press||transportedPress62==in.press||(!sentPulse&&!heldEdge))return;
        transportedPress62=in.press;
        // Even a source-refused real input edge must not become a second Use
        // for a different candidate later in the same physical squeeze.
        if(cancelled||!inputCurrent62(in,in.now))return;
        transportedSource64=in;transportedSettings64=settingsEpoch.load(std::memory_order_acquire);
        transportedSerial62=nextPhysical62.fetch_add(1,std::memory_order_acq_rel)+1;
        (void)physicalUse62.publish({in,transportedSerial62,settingsEpoch.load(std::memory_order_acquire),door?chosen.target:0});
    }
    void cancel() noexcept {
        if(door||deferred){cancelled=true;Intent live{};if(chosen.token&&intent.read(live)&&live.token==chosen.token)(void)intent.publish({});}
    }
    Plan update(const Input& in,const Selection& selection) noexcept {
        const bool pointCurrentH31=inputCurrent62(in,in.now);
        // Check the previous complete point BEFORE a reacquired point can
        // replace it. Fresh raw gap updates do not renew this 80ms lease.
        if(pointGapPendingH31&&transportedSerial62&&!inputCurrent62(lastPointSourceH31,in.now)){
            retirePhysical62(transportedSerial62);transportRetired64=true;
        }
        if(pointCurrentH31&&!transportRetired64)lastPointSourceH31=in;
        // A real Use can be waiting in the native queue when the palm-point
        // publication misses one frame. Preserve only that SAME unclaimed
        // transport while its raw squeeze and last real point are both fresh.
        // Raw evidence grants neither a new Use nor a native selection: keep
        // the acceptance mailbox unavailable until a real point returns.
        Input rawH31=in;rawH31.available=true;
        PhysicalUse62 originalH31{};
        const bool pointGapH31=!in.available&&in.waitingForPoint65&&!cancelled&&!transportRetired64&&
            transportedSerial62&&transportedPress62==in.press&&
            (!chosen.token||consumed.load(std::memory_order_acquire)<chosen.token)&&
            transportedSettings64==settingsEpoch.load(std::memory_order_acquire)&&
            (!tick||(in.now>=tick&&in.now-tick<=80))&&
            inputCurrent62(lastPointSourceH31,in.now)&&sourceSame62(lastPointSourceH31,in)&&
            physicalUse62.read(originalH31)&&originalH31.serial==transportedSerial62&&
            pulseCurrent62(originalH31,rawH31,in.now);
        if(pointGapH31){
            (void)currentInput62.publish({});
            Intent publishedH31{};
            if(chosen.token&&intent.read(publishedH31)&&publishedH31.token==chosen.token)
                (void)intent.publish({}); // normal direct-door tokens have no physicalUseSerial62
            pointGapPendingH31=true;tick=in.now;
            return {true,false,door?chosen.target:0,in.side};
        }
        // Sticky retirement is shared with the native acceptance reader. A
        // later fresh pose with copied identity must not revive a press which
        // crossed loss/release/menu/settings/source ownership or an update gap.
        if(transportedSerial62&&(!inputCurrent62(in,in.now)||!sourceSame62(transportedSource64,in)||
           transportedSettings64!=settingsEpoch.load(std::memory_order_acquire)||
           (tick&&(in.now<tick||in.now-tick>80))))retirePhysical62(transportedSerial62);
        (void)currentInput62.publish(inputCurrent62(in,in.now)?in:Input{});
        // A proven retained native Door resumes through its separate Grip
        // lease. This press must never create a second native Use, even if the
        // operation finishes while Grip remains closed.
        if(in.resumeTarget65&&inputCurrent62(in,in.now)){
            cancel();resumedPress65=in.press;resumedSide65=in.side;resumedTarget65=in.resumeTarget65;
        }
        if(resumedPress65&&in.press==resumedPress65&&in.side==resumedSide65){
            tick=in.now;return {true,false,resumedTarget65,resumedSide65};
        }
        if(chosen.token&&chosen.settingsEpoch!=settingsEpoch.load(std::memory_order_acquire))cancel();
        Input pendingFamily65=in;
        if(in.waitingForPoint65&&!chosen.sent&&transportedPress62!=in.press){
            // The real Grip is closed, although its native20-ms hold settle
            // has not elapsed. Remember identity now without authorizing Use.
            pendingFamily65.available=true;pendingFamily65.held=true;
        }
        if(!classifiedSource64.press&&inputCurrent62(pendingFamily65,in.now)){
            classifiedSource64=pendingFamily65;classifiedSettings64=settingsEpoch.load(std::memory_order_acquire);
        }
        if(classifiedSource64.press&&in.press==classifiedSource64.press&&in.side==classifiedSource64.side&&
           inputCurrent62(classifiedSource64,classifiedSource64.now)&&
           (!inputCurrent62(pendingFamily65,in.now)||!sourceSame62(classifiedSource64,in)||
            classifiedSettings64!=settingsEpoch.load(std::memory_order_acquire)))transportRetired64=true;
        // A transported contextual long-hold is a transaction too. On source
        // retirement release its native held input once, then suppress this
        // squeeze until a real release/new press. Dropping only its entity lock
        // would let the old held Use act on an unrelated new player's target.
        // This is scoped only to an actual previously valid F5 physical input;
        // setting OFF before that transport leaves native controls unchanged.
        if(transportedSource64.press&&in.press==transportedSource64.press&&in.side==transportedSource64.side){
            if(!inputCurrent62(in,in.now)||!sourceSame62(transportedSource64,in)||
               transportedSettings64!=settingsEpoch.load(std::memory_order_acquire))transportRetired64=true;
        }
        if(transportRetired64&&in.press==press&&in.side==side){cancel();tick=in.now;return {true,false,chosen.target,side};}
        if(!enabled.load(std::memory_order_acquire)||!in.scoped54){
            cancel();const bool prior=(door||deferred||(cancelled&&!classified&&!nativeOnly54))&&in.side==side&&in.press==press;
            // OFF preserves native controls. Remember a currently closed hand
            // so turning semi mode back ON cannot create a new press for it.
            if(in.side>=0&&in.side<2&&in.press&&!prior){*this={};side=in.side;press=in.press;cancelled=true;nativeOnly54=true;}
            tick=in.now;return {prior,false,chosen.target,side};
        }
        const bool continuous=!tick||(in.now>=tick&&in.now-tick<=80);
        // C49: the first squeeze precedes publication of its native palm ray.
        // Await that publication only while real tracking is fresh, before any
        // token/native Use. Loss of a live held door still cancels immediately.
        if(!in.available&&(in.waitingForPoint65||(in.waitingForRay49&&!classified))&&continuous&&!chosen.sent&&transportedPress62!=in.press&&
           (!cancelled||side!=in.side||press!=in.press)&&
           in.side>=0&&in.side<2&&in.press&&in.now>=in.press){
            if(side!=in.side||press!=in.press){cancel();*this={};side=in.side;press=in.press;
                if(inputCurrent62(pendingFamily65,in.now)){classifiedSource64=pendingFamily65;classifiedSettings64=settingsEpoch.load(std::memory_order_acquire);}}
            deferred=true;tick=in.now;return {true,false,0,side};
        }
        if(!in.available||!in.now||!continuous||
           in.side<0||in.side>1||!in.press||in.now<in.press){
            cancel();
            // A first squeeze can lose tracking before the 20-ms settle. Do
            // not later turn that same squeeze into a new native Use. A copied
            // door class here only suppresses input; it grants no ownership.
            if(in.side>=0&&in.side<2&&in.press){
                if(side!=in.side||press!=in.press){*this={};side=in.side;press=in.press;}
                cancelled=true;
                if(selection.ordinary&&selection.target&&selection.side==side){
                    classified=door=true;chosen.target=selection.target;
                }
            }
            // A refused unclassified squeeze owns suppression too. Availability
            // may fluctuate while the real Grip is still closed; that cannot
            // alternate native held Use ON/OFF and dispatch an unrelated action.
            const bool suppress=door||deferred||(cancelled&&!classified);
            tick=in.now;return {suppress&&in.side==side&&in.press==press,false,chosen.target,side};
        }
        tick=in.now;
        if(side!=in.side||press!=in.press){cancel();*this={};side=in.side;press=in.press;tick=in.now;
            if(pointCurrentH31)lastPointSourceH31=in;
            if(inputCurrent62(pendingFamily65,in.now)){classifiedSource64=pendingFamily65;classifiedSettings64=settingsEpoch.load(std::memory_order_acquire);}}
        if(!in.held)return {}; // Same 20-ms settle as the ordinary held-input producer.
        if(cancelled)return {door||deferred||!classified,false,chosen.target,side};
        // Waiting for a first offer has no wall-clock expiry, but it is still
        // one exact current source family. Recenter/load/settings changes may
        // not convert this old held squeeze into a new request.
        // Native already accepted the exact current door for this transported
        // press. Adopt it once without issuing a second Use; candidate changes
        // after this point cannot retarget the accepted door.
        Intent accepted{};
        if(!door&&acceptedIntent62.read(accepted)&&valid(accepted,in.now)&&acceptedSourceCurrent62(accepted,in.now)&&accepted.sent&&
           accepted.side==in.side&&accepted.press==in.press&&accepted.player==in.player&&
           accepted.poseEpoch==in.poseEpoch&&accepted.sourceGeneration==in.sourceGeneration&&
           accepted.trackingEpoch==in.trackingEpoch&&accepted.settingsEpoch==settingsEpoch.load(std::memory_order_acquire)){
            chosen=accepted;classified=door=true;deferred=false;
            classifiedSource64=in;classifiedSettings64=settingsEpoch.load(std::memory_order_acquire);
        }
        // A direct native Door may first appear AFTER Grip, while a nearby
        // Pickable is still qualifying its deliberate hold. Before any real
        // pulse/held edge, this is the same hand/request, not a retarget of a
        // native transaction. Never reopen a transported or source-changed one.
        if(classified&&!door&&selection.direct64&&selection.ordinary&&
           transportedPress62!=press&&!nativeHeld62&&inputCurrent62(in,in.now)&&
           inputCurrent62(classifiedSource64,classifiedSource64.now)&&sourceSame62(classifiedSource64,in)&&
           selection.side==side&&selection.target&&fresh(selection.tick,in.now)&&fresh(selection.poseTick,in.now)&&
           selection.player==in.player&&selection.poseEpoch==in.poseEpoch&&selection.sourceGeneration==in.sourceGeneration)
            classified=false;
        if(!classified){
            // H23: the pre-press descriptor cache may only warm the pose-
            // owner's surface query. Its bounds centre is not a contact point,
            // its cached age is not a new selection, and distance=0 must never
            // manufacture permission for native Use. Wait for the actual native
            // nominee plus its current same-door20cm surface Selection from the
            // producer. WarmDoorSurfaceOnPress66 continues to use the cache.
            const Selection& effective=selection;
            // A grip locks one palm immediately, but native selection can take
            // another frame to replace the other hand's previous candidate.
            // Do not permanently classify that other hand or send Use for it.
            if(!effective.target||effective.side!=side||!fresh(effective.tick,in.now)){
                // Still no Use/transaction: a deliberate held Grip may reach
                // the first eligible near-door later. Current source, tracking
                // and context remain mandatory throughout native acceptance.
                deferred=true;
                return {true,false,0,side};
            }
            classified=true;deferred=false;
            classifiedSource64=in;classifiedSettings64=settingsEpoch.load(std::memory_order_acquire);
            door=effective.ordinary&&effective.target&&effective.target<=16777215u&&effective.side==side;
            if(door){
                const bool selected=fresh(effective.tick,in.now)&&fresh(effective.poseTick,in.now)&&
                    effective.player&&effective.player==in.player&&effective.poseEpoch&&effective.poseEpoch==in.poseEpoch&&
                    effective.sourceGeneration&&effective.sourceGeneration==in.sourceGeneration&&
                    std::isfinite(effective.distance)&&effective.distance>=0&&effective.distance<=.20f&&
                    door19c4::finite(effective.point);
                if(!selected){cancelled=true;return {true,false,effective.target,side};}
                chosen.token=nextToken.fetch_add(1,std::memory_order_acq_rel)+1;
                chosen.settingsEpoch=settingsEpoch.load(std::memory_order_acquire);
                chosen.press=press;chosen.selectedTick=effective.tick;chosen.target=effective.target;chosen.side=side;
                chosen.player=effective.player;chosen.poseEpoch=effective.poseEpoch;chosen.sourceGeneration=effective.sourceGeneration;
                chosen.trackingEpoch=in.trackingEpoch;chosen.point=effective.point;chosen.distance=effective.distance;
            }
        }
        if(!door)return pointGapPendingH31?Plan{true,false,0,side}:Plan{};
        if(cancelled)return {true,false,chosen.target,side};
        if(!in.player||in.player!=chosen.player||in.poseEpoch!=chosen.poseEpoch||
           in.sourceGeneration!=chosen.sourceGeneration||!in.trackingEpoch||in.trackingEpoch!=chosen.trackingEpoch||
           !fresh(in.poseTick,in.now)||!door19c4::finite(in.motion)){
            cancel();return {true,false,chosen.target,side};
        }
        chosen.tick=in.now;chosen.poseTick=in.poseTick;chosen.motion=in.motion;
        // XY/Z convention here is OpenXR: ignore vertical Y for door work.
        // Henry's game-world approach/turn cannot become a controller gesture.
        chosen.motion.y=0;
        if(chosen.sent&&!intent.publish(chosen)){cancel();return {true,false,chosen.target,side};}
        // DS-F34: the pulse is ONE-SHOT per physical press+side - a re-entrant
        // update during the transport (before complete) must not pulse twice.
        const bool pulse=!chosen.sent&&!(pulsePress62==press&&pulseSide62==side);
        if(pulse){pulsePress62=press;pulseSide62=side;}
        return {true,pulse,chosen.target,side};
    }
    void complete(const Plan& plan,bool sent) noexcept {
        if(!plan.pulse)return;
        chosen.sent=sent;
        if(!sent||!intent.publish(chosen))cancel();
    }
};
}
