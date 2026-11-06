#pragma once
#include "options_d65c.h"
#include "../../vrik/instant_activities_d72.h"
// XR34 owns one ACTUAL admitted native reload child. No inventory, cock, load,
// animation-event or RPG-state writes are part of this interface.
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include "../../core/snapshot_mailbox.h"
#include "reload_diagnostics_d48.h"
#include "crossbow_clock_entry_d87.h"
#include "crossbow_refusal_d85.h" // D85 xbow: one-line refusal text
namespace kcd2vr::crossbow_native_xr34 {
struct Vec {double x=0,y=0,z=0;};
inline Vec operator-(Vec a,Vec b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline double dot(Vec a,Vec b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double length(Vec a) noexcept {return std::sqrt(dot(a,a));}
inline bool finite(Vec a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline Vec scale(Vec a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
struct Wrist {float q[4]{0,0,0,1},p[3]{};};
// D66C: rotate v by the CONJUGATE of q (frame -> local). Used for the world up
// axis of the heavy crank "level" rule; q is the committed weapon model quat.
inline Vec rotateInverse(const float (&q)[4],Vec v) noexcept {
    const double x=-q[0],y=-q[1],z=-q[2],w=q[3];
    const double tx=2*(y*v.z-z*v.y),ty=2*(z*v.x-x*v.z),tz=2*(x*v.y-y*v.x);
    return {v.x+w*tx+(y*tz-z*ty),v.y+w*ty+(z*tx-x*tz),v.z+w*tz+(x*ty-y*tx)};
}
struct Source {
    std::uintptr_t player=0,equipment=0,manager=0,character=0,weaponItem=0,ammoItem=0;
    std::uint64_t publication=0,key=0,ammoKey=0,generation=0,body=0,tracking=0,request=0,sample=0,tick=0,completedLease=0;
    unsigned actor=0;int primary=-1,classId=-1;bool allowed=false;
    Vec hand[2]{}; // committed allowed wrists, in the same weapon-local frame
    // D78: unmodified controller goals from this same committed transaction.
    // Never feed a constrained/mechanism wrist back into gesture progress.
    Vec intentHandD78[2]{};Wrist intentWristD78[2]{};bool intentKnownD78=false;
    Vec nativeGrip{}; // original native working wrist in that same frame
    double grip[2]{};
    Wrist nativeWrist[2]{},allowedWrist[2]{},weaponModel{};float world[8]{0,0,0,1,0,0,0,1}; // original committed frame, never rebased
    // D66C heavy crank chest gate. The producers evaluate the chest box in the
    // tracking snapshot of this same sample (head/right/left, Y up), because the
    // packet's solved head is absent while the reload working hand is a passenger.
    // chestKnown=false means no tracked head/hands for this sample.
    bool chestKnown=false,handInChest[2]{};
    // Captured at admission. Live preference changes do not rewrite an owner.
    bool instantD72=false,quiverKnownD72=false,handInQuiverD72[2]{};
    // D81: physical ownership is independent of the optional fast native tail.
    // A saved InstantReloads=false must not turn physical reload into a gesture
    // which merely starts Henry's animation. Captured for this occurrence.
    bool physicalD81=false;
    unsigned entryGripD72=0;std::uint64_t entryGripTickD72=0; // 1 string, 2 pouch tool, 3 bolt
    Wrist primaryGripD72{},nativeWeaponD72{};
    std::uintptr_t nativePoseClockD78=0;float nativePoseNormalizedD78=-1,nativePoseActiveTimeD78=-1;
    std::uint64_t cancellationR93=0; // publication context, never pose age
    std::uint64_t bodyPublicationR95=0; // exact body transaction, including repeated XR samples
    bool auxiliaryUnavailableR95=false; // non-authorizing pause witness; tick/sample remain original
    bool auxiliaryReleasedR95=false; // release-only observation while paused, never an acquisition edge

};
inline bool handDrivenD81(const Source& s) noexcept {return s.physicalD81||s.instantD72;}
inline bool frameValid(const Source& s) noexcept {
    for(const auto* wrists:{s.nativeWrist,s.allowedWrist})for(int side=0;side<2;++side){double n=0;
        for(float q:wrists[side].q){if(!std::isfinite(q))return false;n+=double(q)*q;}
        if(n<.99||n>1.01)return false;for(float p:wrists[side].p)if(!std::isfinite(p))return false;}
    {double n=0;for(float q:s.weaponModel.q){if(!std::isfinite(q))return false;n+=double(q)*q;}
        if(n<.99||n>1.01)return false;for(float p:s.weaponModel.p)if(!std::isfinite(p))return false;}
    if(handDrivenD81(s))for(const auto* pose:{&s.primaryGripD72,&s.nativeWeaponD72}){double n=0;
        for(float q:pose->q){if(!std::isfinite(q))return false;n+=double(q)*q;}
        if(n<.99||n>1.01)return false;for(float p:pose->p)if(!std::isfinite(p))return false;}
    if(s.nativePoseClockD78&&(!std::isfinite(s.nativePoseNormalizedD78)||s.nativePoseNormalizedD78<0||
       s.nativePoseNormalizedD78>1||!std::isfinite(s.nativePoseActiveTimeD78)||s.nativePoseActiveTimeD78<0))return false;
    if(s.intentKnownD78)for(int side=0;side<2;++side){double n=0;
        if(!finite(s.intentHandD78[side]))return false;
        for(float q:s.intentWristD78[side].q){if(!std::isfinite(q))return false;n+=double(q)*q;}
        if(n<.99||n>1.01)return false;for(float p:s.intentWristD78[side].p)if(!std::isfinite(p))return false;}
    double n=0;for(unsigned i=0;i<8;++i){if(!std::isfinite(s.world[i]))return false;if(i<4)n+=double(s.world[i])*s.world[i];}
    return n>.99&&n<1.01&&s.world[7]>.1f&&s.world[7]<10.f;
}
inline bool fresh(const Source& s,std::uint64_t now) noexcept {
    return frameValid(s)&&s.allowed&&s.player&&s.equipment&&s.manager&&s.character&&s.key&&s.generation&&
        s.weaponItem&&s.ammoItem&&s.ammoKey&&s.body&&s.tracking&&s.request&&s.sample&&s.tick&&now>=s.tick&&now-s.tick<=80&&s.actor&&
        s.primary>=0&&s.primary<2&&(s.classId==10||s.classId==14||s.classId==15)&&
        finite(s.hand[0])&&finite(s.hand[1])&&finite(s.nativeGrip)&&
        std::isfinite(s.grip[0])&&std::isfinite(s.grip[1]);
}
inline bool sameSource(const Source& a,const Source& b) noexcept {
    return a.player==b.player&&a.equipment==b.equipment&&a.manager==b.manager&&
        a.character==b.character&&a.actor==b.actor&&a.key==b.key&&a.weaponItem==b.weaponItem&&
        a.ammoItem==b.ammoItem&&a.ammoKey==b.ammoKey&&a.generation==b.generation&&
        a.body==b.body&&a.tracking==b.tracking&&a.request==b.request&&a.primary==b.primary&&a.classId==b.classId;
}
struct Owner {
    Source source{};std::uintptr_t controller=0,main=0,parent=0,child=0,context=0,root=0;
    unsigned token=0;std::uint64_t serial=0;
};
inline bool sameOwner(const Owner& a,const Owner& b) noexcept {
    return a.serial&&a.serial==b.serial&&sameSource(a.source,b.source)&&a.controller==b.controller&&
        a.main==b.main&&a.parent==b.parent&&a.child==b.child&&a.context==b.context&&a.token==b.token;
}
enum class Phase:unsigned {None,AwaitClip,Setup,Span,TakeBolt,PlaceBolt,Tail,Retiring,Unavailable,NativeTransitionD80};
enum class Reason:unsigned {Off,Ready,WaitingClip,WaitingTool,WaitingNeutral,WaitingGrip,Working,
    WaitingBolt,WaitingPlacement,NativeAck,NativeComplete,NativeFallback,OwnerChanged,UnknownMembership,
    ForeignSpeed,PendingRestore,ClipUnsupported,ClockUnsupported,GeometryUnavailable,InputLost,NativeStop,
    UnapprovedInterval,UnapprovedCompletion,PhysicalBindingLost,WaitingFirstPose,
    WaitingChest,ChestUnknown,WaitingQuiverD72,WaitingMountD72,WaitingMechanismD78,NativeTransitionD80,WaitingPlacementPoseR94,WaitingSourceR95}; // D66C heavy crank: chest visit gate / no tracked head+hands this sample
// D65C: names for the one visible line ("crossbow: ...") and the F11 rows.
inline const char* reasonName(Reason r) noexcept {
    switch(r){
    case Reason::Off:return "OFF";case Reason::Ready:return "READY";case Reason::WaitingClip:return "WAITING_CLIP";
    case Reason::WaitingTool:return "WAITING_TOOL";case Reason::WaitingNeutral:return "OPEN_YOUR_HAND_FIRST";case Reason::WaitingGrip:return "GRIP_THE_STRING";
    case Reason::Working:return "WORKING";case Reason::WaitingBolt:return "WAITING_BOLT";case Reason::WaitingPlacement:return "PLACE_THE_BOLT";
    case Reason::NativeAck:return "NATIVE_ACK";case Reason::NativeComplete:return "NATIVE_COMPLETE";case Reason::NativeFallback:return "NATIVE_FALLBACK";
    case Reason::OwnerChanged:return "OWNER_CHANGED";case Reason::UnknownMembership:return "UNKNOWN_MEMBERSHIP";case Reason::ForeignSpeed:return "FOREIGN_SPEED";
    case Reason::PendingRestore:return "PENDING_RESTORE";case Reason::ClipUnsupported:return "CLIP_UNSUPPORTED";case Reason::ClockUnsupported:return "CLOCK_UNSUPPORTED";
    case Reason::GeometryUnavailable:return "GEOMETRY_UNAVAILABLE";case Reason::InputLost:return "INPUT_LOST";case Reason::NativeStop:return "NATIVE_STOP";
    case Reason::UnapprovedInterval:return "UNAPPROVED_INTERVAL";case Reason::UnapprovedCompletion:return "UNAPPROVED_COMPLETION";
    case Reason::PhysicalBindingLost:return "PHYSICAL_BINDING_LOST";case Reason::WaitingFirstPose:return "WAITING_FIRST_POSE";
    case Reason::WaitingChest:return "GRIP_THE_TOOL_AT_THE_CHEST_POUCH";case Reason::ChestUnknown:return "CHEST_UNKNOWN(no tracked head/hands)";
    case Reason::WaitingQuiverD72:return "TAKE_A_BOLT_FROM_THE_RIGHT_QUIVER";case Reason::WaitingMountD72:return "ENGAGE_THE_TOOL_WITH_THE_STRING";
    case Reason::WaitingMechanismD78:return "WAITING_FOR_NATIVE_LEVER_POSE";
    case Reason::WaitingPlacementPoseR94:return "WAITING_FOR_CURRENT_HELD_ITEM_POSE";
    case Reason::WaitingSourceR95:return "WAITING_FOR_COMPLETED_RELOAD_SOURCE";
    case Reason::NativeTransitionD80:return "NATIVE_MARKERLESS_TRANSITION";default:return "UNKNOWN";
    }
}
// D65C: a restore() that gives the clock back to native BEFORE the gesture
// finished is a refusal the player must see. POD only; the executor formats.
inline std::atomic<std::uint64_t> fallbackSerialD65C{0};
inline std::atomic<unsigned> fallbackReasonD65C{0};
inline bool fallbackVisibleD65C(Reason r) noexcept {
    switch(r){
    case Reason::NativeFallback:case Reason::UnknownMembership:case Reason::ForeignSpeed:case Reason::ClipUnsupported:
    case Reason::ClockUnsupported:case Reason::GeometryUnavailable:case Reason::InputLost:case Reason::UnapprovedInterval:
    case Reason::UnapprovedCompletion:case Reason::PhysicalBindingLost:case Reason::ChestUnknown:return true;
    default:return false;
    }
}
inline void noteFallbackD65C(Reason r,bool gestureLive) noexcept {
    if(!gestureLive||!fallbackVisibleD65C(r))return;
    fallbackReasonD65C.store(unsigned(r),std::memory_order_relaxed);fallbackSerialD65C.fetch_add(1,std::memory_order_acq_rel);
}
// D65C: the last source refusal (static literal) so a "native reload" line can say why.
inline std::atomic<const char*> lastSourceRefusalD65C{nullptr};
inline std::atomic<const char*> lastClipRefusalD72{nullptr};
// D85 xbow: paused join window. While this Client holds the native reload at
// rate 0 (first tracked pose, string/lever pose join) native time does not
// move: child+5C is our read-back zero, the retail event-scan caller skips a
// CAF whose time did not change (WHGame 841AB3 ucomiss xmm6,xmm7 / je 841B4C)
// and the D81R1 capture shows the reload CAF at segmentTime 0.0000 50 ms into
// WAITING_FIRST_POSE. No ammo, cock or load event can fire while we wait, so a
// slow pose job, a 45 fps frame or a CAF still streaming in no longer ends the
// hand reload. Owner, child token, membership and identity are still checked
// every tick; losing them still falls back on that same tick.
inline constexpr std::uint64_t kPausedJoinMsD85=500;
inline std::atomic<unsigned> fallbackStageD85{0}; // 1 first tracked pose, 2 string/lever pose join
inline std::atomic<const char*> lastBootstrapRefusalD85{nullptr}; // first missing first-pose admission term
inline std::atomic<std::uint64_t> gripRequestTickD85{0}; // last tracked grip that asked the game for a reload
struct Clip {
    std::uintptr_t list=0,entries=0,path=0,animation=0;int id=-1,segment=-1,count=0;
    float duration=0,spawn=-1,ammo=-1,cock=-1,load=-1,despawn=-1;
    float firstNativeEffectR92=-1; // earliest exact retail effect handler, not just cock/load
    bool cocking=false,loading=false,qualified=false,nativeTransitionD80=false;
};
struct Clock {
    std::uintptr_t entry=0;float normalized=0,playbackScale=0,remaining=0,activeTime=0;
    float controllerScale=0,characterScale=0;int segment=-1;bool qualified=false;
};
struct Spatial {
    std::uint64_t serial=0,sample=0,tick=0;bool qualified=false,toolBound=false,toolVisible=false;
    bool handBoltBound=false,handBoltPlaced=false,nativeCocked=false,nativeLoaded=false,nockBound=false;
    bool toolPoseReady=false,boltPoseReady=false,toolWeaponBoundD72=false,toolPendingD72=false;
    bool boltAlignmentKnownD78=false,boltAlignedD78=false;
    bool handBoltPendingR95=false; // native parent has a bolt ID; hand attachment not complete yet
    std::uint64_t placementTickR94=0; // original successful entity-placement receipt; never Source.tick
    std::uintptr_t toolItem=0,toolEntity=0,boltEntity=0,toolAttachment=0,boltAttachment=0;
    std::uint64_t toolKey=0,boltKey=0;unsigned toolId=0,boltId=0;int nativeSide=-1;
    Vec grip{},axis{},stringRest{},rail{};double travel=0;
    Vec stringCurrentD78{};Clock stringClockD78{};std::uint64_t stringTickD78=0;bool stringMechanismD78=false;
    Vec toolMountD72{},toolHandleD72{};bool toolMechanismD72=false;
    Wrist toolWristD78{};Clock mechanismClockD78{};std::uint64_t mechanismTickD78=0;bool toolMountedPoseD78=false;
    // Heavy crank (class 15) contract, D66C. The producer sets crankQualified
    // only when it filled the crank values: grip = the string rest joint (where
    // the crank engages the string), axis = rail direction string -> stirrup
    // (forward, the opposite of the class 14 pull), travel = VrCrankTravelD66C
    // (.04..0.20 m, default .08). The Client then runs the generic grasp/pull
    // with the "level" rule (lateral <= 5 cm, vertical <= 4 cm) and the chest
    // gate before the spawn event. crankProgress is kept for old readers; the
    // Client computes the progress itself from the grasp start.
    bool crankQualified=false;double crankProgress=0;
};
// D66C heavy crank constants (one place; fixture and producer read the same).
inline constexpr double kCrankTravelDefaultD66C=.08,kCrankTravelMinD66C=.04,kCrankTravelMaxD66C=.20;
inline constexpr double kCrankLateralD66C=.05,kCrankVerticalD66C=.04;
// Settings are stored as float, then transported as double. Admit the exact UI
// endpoints despite that representation change; reject non-finite/out-of-range input.
inline bool crankTravelValidD66C(double value) noexcept {
    return std::isfinite(value)&&value>=kCrankTravelMinD66C-1e-6&&value<=kCrankTravelMaxD66C+1e-6;
}
// Chest box, head relative: 10..50 cm below the tracked head and within 32 cm of
// its vertical axis (below = head up minus hand up, radius = horizontal distance).
// R108: was 10..40 / 25. The tracked head is the eye point, 8-10 cm in front
// of the skull centre, and the controller sits 5-10 cm beyond the palm, so a
// hand at the sternum or at the belt pouch can measure more than 40 cm below
// and more than 25 cm out. His R106 run had five right Grips at the uncocked
// goat's foot (17:09:16-20) with no request and no line; the log has no head
// row for them, so the zone is widened for those offsets and the grip request
// now prints the measured below/out numbers. The quiver box starts 6 cm BEHIND
// and 6 cm RIGHT of the head, the string zone is weapon local: neither
// overlaps the wider pouch.
// Hand back "near the weapon": within 30 cm of the string rest (weapon local).
inline constexpr double kChestBelowMinD66C=.10,kChestBelowMaxD66C=.50,kChestRadiusD66C=.32,kChestReturnD66C=.30;
inline bool chestBoxD66C(double below,double radius) noexcept {
    return std::isfinite(below)&&std::isfinite(radius)&&below>=kChestBelowMinD66C&&below<=kChestBelowMaxD66C&&radius<=kChestRadiusD66C;
}
// head/hand in one Y-up frame (the tracking snapshot). Any non-finite input = not in the box.
inline bool handInChestD66C(Vec head,Vec hand) noexcept {
    if(!finite(head)||!finite(hand))return false;
    const Vec d=hand-head;return chestBoxD66C(-d.y,std::sqrt(d.x*d.x+d.z*d.z));
}
// Exact heavy player data spells this marker "DespawLever". The native
// dispatcher does not recognize the typo as a removal event.
inline constexpr unsigned kDespawnLeverTypoCrcD66C=0x441B89A7u;
// Local child clock transport only. Original interval scan, native callbacks,
// costs and finalizer remain intact. Never use this for a foreign child.
inline constexpr float kClockTransportMaxD72=65536.f;
struct HandConstraintD78 {Vec start{},axis{};double travel=0,progress=0;bool active=false,mounted=false;Vec stringGripD78{};};
struct Status {Owner owner{};Phase phase=Phase::None;Reason reason=Reason::Off;float progress=0,speed=0;
    std::uint64_t started=0,firstPoseDeadline=0;bool clockOwned=false,cocking=false,loading=false,placeTool=false,placeBolt=false,sourceEstablished=false;
    bool toolTakenD72=false,toolEngagedD72=false,boltTakenD72=false,toolMountedD78=false;
    HandConstraintD78 constraintD78{};};
// Fresh immutable passenger from the ONE native executor. The original copied
// Source wrists/frame retain their tick; no global actor or renewed timestamp.
struct Passenger {Owner owner{};Source frame{};Clip clip{};Clock clock{};Spatial binding{};std::uint64_t publication=0;
    std::uintptr_t root=0;int childStatus=0,workingHand=-1;bool valid=false,bootstrap=false;
    HandConstraintD78 constraintD78{};};
// One explicit presentation owner per reload hand. The native torso, action
// clock and stock/event accounting remain outside the ordinary weapon kernel.
enum class HandOwnerR95 : unsigned {Native,PrimaryGrip,FreeWorking,HandTool,HandBolt,String,MountedMechanism};
struct HandPolicyR95 {
    HandOwnerR95 owner[2]{HandOwnerR95::Native,HandOwnerR95::Native};
    bool tracked[2]{},worldContact[2]{},nativeFingers[2]{true,true};
};
inline HandPolicyR95 handPolicyR95(const Passenger& p) noexcept {
    HandPolicyR95 out{};const int primary=p.frame.primary,working=p.workingHand;
    if(!p.valid||primary<0||primary>1||working!=1-primary)return out;
    out.tracked[working]=true;
    if(!handDrivenD81(p.owner.source))return out;
    out.tracked[primary]=true;out.owner[primary]=HandOwnerR95::PrimaryGrip;
    const bool constrained=p.constraintD78.active&&!p.frame.auxiliaryUnavailableR95;
    if(constrained)out.owner[working]=p.constraintD78.mounted?HandOwnerR95::MountedMechanism:HandOwnerR95::String;
    else if(p.binding.handBoltBound)out.owner[working]=HandOwnerR95::HandBolt;
    else if(p.binding.toolBound&&!p.binding.toolWeaponBoundD72)out.owner[working]=HandOwnerR95::HandTool;
    else {out.owner[working]=HandOwnerR95::FreeWorking;out.worldContact[working]=true;out.nativeFingers[working]=false;}
    return out;
}
#include "crossbow_constraint_d78.h"

// This is current native ACTION admission, not a fresh tracked-pose frame.
// Its fixed original-start deadline is never renewed by a later observation.
struct Admission {Owner owner{};Clip clip{};Clock clock{};Spatial binding{};
    std::uint64_t publication=0,tick=0,started=0,deadline=0;std::uintptr_t root=0;int childStatus=0;bool valid=false;};
inline Passenger PassengerFromAdmission(const Admission& a) noexcept {
    Passenger p{};p.owner=a.owner;p.frame=a.owner.source;p.clip=a.clip;p.clock=a.clock;p.binding=a.binding;
    p.publication=a.publication;p.root=a.root;p.childStatus=a.childStatus;p.workingHand=1-p.frame.primary;
    p.valid=a.valid;p.bootstrap=true;return p;
}
// Sticky for this ONE lease: source withdrawal cannot reopen first-pose
// bootstrap merely because the native tick had not yet consumed publication.
inline std::atomic<std::uint64_t> completedNativeLease{0};
using ReadFn=bool(*)(std::uintptr_t,void*,std::size_t) noexcept;
using WriteFn=bool(*)(std::uintptr_t,const void*,std::size_t) noexcept;
// Raw quiver intent has its own tracking timestamp. It can remember a reach
// while a native socket is pending, but cannot advance time or place an item.
struct QuiverInputR95 {std::uint64_t sample=0,tick=0;double grip=0;bool inside=false;};
using QuiverReaderR95=bool(*)(const Owner&,QuiverInputR95&) noexcept;
inline std::atomic<QuiverReaderR95> quiverReaderR95{nullptr};
struct Ops {
    ReadFn read=nullptr;WriteFn write=nullptr;
    bool(*source)(Source&) noexcept=nullptr;
    bool(*spatial)(const Owner&,const Source&,Spatial&) noexcept=nullptr;
    bool(*clip)(const Owner&,int,int,Clip&) noexcept=nullptr;
    bool(*clock)(const Owner&,const Clip&,Clock&) noexcept=nullptr;
    bool(*current)(const Owner&) noexcept=nullptr;
    bool(*newAdmission)(const Owner&) noexcept=nullptr;
    bool(*binding)(const Owner&,Spatial&) noexcept=nullptr; // identity-only check during an auxiliary gap
    QuiverReaderR95 quiver=nullptr;
};
// D81: an ordinary idle -> reload crossfade can retain more than one native
// CAF in this layer. Select the exact admitted animation, never FIFO[0] or an
// arbitrary alias. A missing/duplicate identity cannot grant a clock lease.
inline bool selectedClockEntryD81(ReadFn read,std::uintptr_t character,unsigned layer,int animation,
    std::uintptr_t& selected,bool inertTransitionD87=false,crossbow_clock_entry_d87::SelectionObservation* observed=nullptr) noexcept {
    const auto fail=[&](const char* why) noexcept {if(observed)observed->why=why;return false;};
    if(observed){observed->layer=layer;observed->layers=1;}
    selected=0;if(!read||!character||layer>15||animation<0)return fail("CLOCK_FIFO_ARGUMENTS");
    std::uintptr_t entries=0;unsigned count=0;
    if(!read(character+0x110+std::size_t(layer)*0x40+0x48,&entries,sizeof(entries)))return fail("CLOCK_FIFO_POINTER_READ");
    if(observed)observed->entries=entries;
    if(!entries)return fail("CLOCK_FIFO_POINTER_NULL");
    if(!read(entries-4,&count,sizeof(count)))return fail("CLOCK_FIFO_COUNT_READ");
    if(observed)observed->count=count;
    if(count<1||count>16)return fail("CLOCK_FIFO_COUNT_RANGE");
    std::uintptr_t retired[16]{};unsigned retiredCount=0;
    for(unsigned i=0;i<count;++i){const auto at=entries+std::size_t(i)*0x78;unsigned id=~0u;
        if(!read(at+0x10,&id,sizeof(id)))return fail("CLOCK_FIFO_ANIMATION_ID_READ");
        if(id!=unsigned(animation))continue;
        if(!selected){selected=at;continue;}
        // Repeated reloads can retain the previous identical transition CAF
        // at 1.0 while the new occurrence is at 0.0 (D85R5 F11 00:07:28).
        // Only an already event-free CAF with an inert terminal predecessor
        // is distinguishable. Two live matches, a forced scan or any effectful
        // clip remain ambiguous and cannot acquire this clock.
        if(observed)observed->duplicate=true;
        if(!inertTransitionD87)return fail("CLOCK_DUPLICATE_WITHOUT_EVENT_FREE_PROOF");
        const bool before=crossbow_clock_entry_d87::inertTerminal(read,selected,observed?&observed->earlier:nullptr);
        const bool next=crossbow_clock_entry_d87::inertTerminal(read,at,observed?&observed->later:nullptr);
        if(before==next)return fail(before?"CLOCK_DUPLICATE_BOTH_TERMINAL":"CLOCK_DUPLICATE_NOT_INERT");
        retired[retiredCount++]=before?selected:at;if(before)selected=at;
    }
    for(unsigned i=0;i<retiredCount;++i){unsigned id=~0u;
        if(!read(retired[i]+0x10,&id,sizeof(id))||id!=unsigned(animation)||
           !crossbow_clock_entry_d87::inertTerminal(read,retired[i]))return fail("CLOCK_RETIRED_ENTRY_CHANGED");}
    std::uintptr_t again=0;unsigned n=0;
    if(observed)observed->selected=selected;
    const bool stable=selected&&read(character+0x110+std::size_t(layer)*0x40+0x48,&again,sizeof(again))&&again==entries&&
        read(entries-4,&n,sizeof(n))&&n==count;
    if(!stable)return fail("CLOCK_FIFO_CHANGED_OR_ID_ABSENT");
    return true;
}
// Retail kcd_male_controllerdefs.xml declares UpperBody as 6+2 and FullBody
// as 0+3. Player light/medium Load uses the former; heavy/stealth Load uses
// the latter. Their native fragments contain one AnimLayer, so the remaining
// scope slots must be empty. The old layers==1 join rejected both real roots
// before the first tracked reload wrist could ever be published.
// Keep the original single-layer contract and admit only these two authored
// profiles. A populated companion slot needs its own clock/event proof.
inline bool selectedScopeClockEntryD82(ReadFn read,std::uintptr_t character,unsigned layer,unsigned layers,
    int animation,std::uintptr_t& selected,bool inertTransitionD87=false,crossbow_clock_entry_d87::SelectionObservation* observed=nullptr) noexcept {
    const auto fail=[&](const char* why) noexcept {if(observed)observed->why=why;return false;};
    if(observed){observed->layer=layer;observed->layers=layers;}
    selected=0;
    if(layers!=1&&!((layer==6&&layers==2)||(layer==0&&layers==3)))return fail("CLOCK_SCOPE_PROFILE");
    const bool chosen=selectedClockEntryD81(read,character,layer,animation,selected,inertTransitionD87,observed);
    if(observed)observed->layers=layers;
    if(!chosen)return false;
    std::uintptr_t empty[2]{};
    for(unsigned i=1;i<layers;++i){unsigned count=~0u;
        const auto at=character+0x110+std::size_t(layer+i)*0x40+0x48;
        if(!read(at,&empty[i-1],sizeof(empty[i-1]))||!empty[i-1]||
           !read(empty[i-1]-4,&count,sizeof(count))||count)return fail("CLOCK_COMPANION_NOT_EMPTY");
    }
    for(unsigned i=1;i<layers;++i){std::uintptr_t again=0;unsigned count=~0u;
        const auto at=character+0x110+std::size_t(layer+i)*0x40+0x48;
        if(!read(at,&again,sizeof(again))||again!=empty[i-1]||
           !read(again-4,&count,sizeof(count))||count)return fail("CLOCK_COMPANION_CHANGED");
    }
    return true;
}
enum class Member:int {Unknown=-1,Absent,Queued,Root,Exiting};
template<class T> inline bool rd(const Ops& io,std::uintptr_t p,T& value) noexcept {
    return io.read&&p&&io.read(p,&value,sizeof(value));
}
// Compare native containers BEFORE any retained child is dereferenced. This is
// the shared exact retail membership layout, not a stale pointer/vft lease.
inline Member membership(const Ops& io,std::uintptr_t base,const Owner& o,std::uintptr_t* root=nullptr) noexcept {
    std::uintptr_t v=0,scopes=0,first=0,last=0;unsigned count=0;
    if(root)*root=0;
    if(!o.child||!rd(io,o.controller,v)||v!=base+0x40519E8||!rd(io,o.controller+0x34,count)||count>24||
       !rd(io,o.controller+0x38,scopes)||(count&&!scopes)||!rd(io,o.controller+0x48,first)||
       !rd(io,o.controller+0x50,last)||last<first||last-first>1024||(last-first)%8||(!first&&last))return Member::Unknown;
    Member result=Member::Absent;bool nonroot=false;
    for(auto at=first;at<last;at+=8){std::uintptr_t item=0;if(!rd(io,at,item))return Member::Unknown;if(item==o.child)result=Member::Queued;}
    for(unsigned i=0;i<count;++i){const auto scope=scopes+std::size_t(i)*0x168;std::uintptr_t playing=0,exiting=0;
        if(!rd(io,scope+0x148,playing)||!rd(io,scope+0x150,exiting))return Member::Unknown;
        if(playing==o.child||exiting==o.child){std::uintptr_t actual=0;nonroot=true;
            if(!rd(io,o.child+0x30,actual))return Member::Unknown;
            if(actual==scope){result=playing==o.child?Member::Root:Member::Exiting;if(root)*root=scope;}}
    }
    if(nonroot&&result==Member::Absent)return Member::Unknown;
    std::uintptr_t again=0;unsigned n=0;
    if(!rd(io,o.controller+0x34,n)||n!=count||!rd(io,o.controller+0x38,again)||again!=scopes||
       !rd(io,o.controller+0x48,again)||again!=first||!rd(io,o.controller+0x50,again)||again!=last)return Member::Unknown;
    return result;
}
// D69 diagnostic-only first refusal; never authority or a retry request.
enum class StartReasonD69 : unsigned {NotAttempted,PendingRestore,NoBase,NoCurrentReader,CurrentOwner,
    SourceInvalid,SpeedInvalid,MembershipUnknown,NotQueued,ActiveUnreadable,AlreadyActive,HandAdmission,InitialPauseFailed,Taken};
inline const char* startReasonNameD69(StartReasonD69 r) noexcept {
    switch(r){
    case StartReasonD69::NotAttempted:return "START_NOT_ATTEMPTED";
    case StartReasonD69::PendingRestore:return "START_PENDING_RESTORE";
    case StartReasonD69::NoBase:return "START_NO_BASE";
    case StartReasonD69::NoCurrentReader:return "START_NO_CURRENT_READER";
    case StartReasonD69::CurrentOwner:return "START_CURRENT_OWNER_REFUSED";
    case StartReasonD69::SourceInvalid:return "START_SOURCE_INVALID_OR_STALE";
    case StartReasonD69::SpeedInvalid:return "START_SPEED_INVALID";
    case StartReasonD69::MembershipUnknown:return "START_MEMBERSHIP_UNKNOWN";
    case StartReasonD69::NotQueued:return "START_CHILD_NOT_QUEUED";
    case StartReasonD69::ActiveUnreadable:return "START_ACTIVE_TIME_UNREADABLE";
    case StartReasonD69::AlreadyActive:return "START_CHILD_ALREADY_ACTIVE";
    case StartReasonD69::HandAdmission:return "START_HAND_ADMISSION_REFUSED";
    case StartReasonD69::InitialPauseFailed:return "START_INITIAL_PAUSE_FAILED";
    case StartReasonD69::Taken:return "TAKEN";
    default:return "START_UNKNOWN";
    }
}
class Client {
    StartReasonD69 startReasonD69_=StartReasonD69::NotAttempted;
    Ops io_{};std::uintptr_t base_=0;std::uint64_t next_=0,lastSample_=0;
    Status state_{};Clip clip_{};float saved_=1,last_=1,candidate_=1,permitted_=0;
    std::uint64_t started_=0;
    bool wrote_=false,uncertain_=false,neutral_=false,grasp_=false,physical_=false;
    bool bindingCaptured_=false;Spatial graspBinding_{};
    Vec start_{},stringGripD78_{};double progress_=0,crankGraspTravelD66_=0;
    Source continuationSourceR95_{},gripAdmissionSourceR95_{};std::uint64_t gripAdmissionTickR95_=0;
    bool continuationArmedR95_=false,continuationAdmissionR95_=false;
    Owner attemptedActionR95_{};bool attemptedActionLiveR95_=false;std::uint64_t attemptedActionTickR95_=0; // R108: when it was attempted
    bool entryGripConsumedD72_=false;std::uint64_t mechanismWaitD78_=0;
    std::uint64_t placementTickR94_=0,setupWaitR95R2_=0;
    QuiverInputR95 quiverInputR95_{};bool quiverReachR95_=false;
    void observeQuiverR95(const Source& s,std::uint64_t now) noexcept {
        QuiverInputR95 input{};
        const bool raw=io_.quiver&&io_.quiver(state_.owner,input);
        if(!raw&&!s.auxiliaryUnavailableR95&&s.quiverKnownD72){
            const int hand=1-s.primary;input={s.sample,s.tick,s.grip[hand],s.handInQuiverD72[hand]};
        }
        if(input.sample&&input.tick&&now>=input.tick&&now-input.tick<=80&&
           std::isfinite(input.grip)&&input.grip>=0&&input.grip<=1){
            // A completed body can trail raw tracking. It cannot overwrite a
            // newer reach/release or renew that observation's original age.
            if(input.sample>quiverInputR95_.sample&&input.tick>=quiverInputR95_.tick){
                if(quiverInputR95_.tick&&input.tick-quiverInputR95_.tick>80)quiverReachR95_=false;
                quiverInputR95_=input;
                if(input.grip<.25){quiverReachR95_=false;entryGripConsumedD72_=true;}
                else if(input.grip>.65&&input.inside)quiverReachR95_=true;
            }
        }
        if(!quiverInputR95_.tick||now<quiverInputR95_.tick||now-quiverInputR95_.tick>80)
            quiverReachR95_=false;
        if(!raw&&s.auxiliaryReleasedR95)quiverReachR95_=false;
    }
    bool chestVisited_=false,chestGate_=false; // D66C heavy crank: hand went to the chest, then came back to the string
    bool write(float speed) noexcept {
        float old=0;if(!rd(io_,state_.owner.child+0x5C,old)||!std::isfinite(old))return false;
        if(wrote_&&old!=last_&&(!uncertain_||old!=candidate_)){state_.reason=Reason::ForeignSpeed;return false;}
        candidate_=speed;uncertain_=true;
        if(!io_.write||!io_.write(state_.owner.child+0x5C,&speed,sizeof(speed)))return false;
        float after=-1;if(!rd(io_,state_.owner.child+0x5C,after)||after!=speed)return false;
        wrote_=true;uncertain_=false;last_=speed;state_.speed=speed;state_.clockOwned=true;return true;
    }
    void rememberPlacementR94(const Spatial& spatial,std::uint64_t now) noexcept {
        // Only a successful readback supplies this timestamp. Re-reading an
        // unchanged receipt never turns it into a newer rendered placement.
        if(spatial.placementTickR94&&now>=spatial.placementTickR94&&now-spatial.placementTickR94<=80)
            placementTickR94_=std::max(placementTickR94_,spatial.placementTickR94);
    }
    bool pausePlacementR94(const Source& s,const Clock& clock,std::uint64_t now) noexcept {
        if(!wrote_||uncertain_||!placementTickR94_||now<placementTickR94_||now-placementTickR94_>80)return false;
        // Fresh input, exact native owner/clock and unchanged captured item
        // identity were checked by the caller. A moving pull pauses the same
        // owned clock too; write() compares against our last verified rate.
        // No gesture or native interval credit is earned during this gap.
        state_.constraintD78={};permitted_=clock.normalized;
        if(s.grip[1-s.primary]<.25){grasp_=false;state_.toolEngagedD72=false;neutral_=true;}
        state_.reason=Reason::WaitingPlacementPoseR94;
        return write(0); // stop only this exact, still-owned clock
    }
    bool setupJoinCurrentR95R2(bool waiting,std::uint64_t now) noexcept {
        if(!waiting){setupWaitR95R2_=0;return true;}
        // Only a deliberately requested native tool/bolt join starts this
        // deadline. Fresh body packets cannot renew a missing first placement.
        if(!setupWaitR95R2_)setupWaitR95R2_=now;
        if(now>=setupWaitR95R2_&&now-setupWaitR95R2_<=kPausedJoinMsD85)return true;
        fallbackStageD85.store(3,std::memory_order_relaxed);
        (void)restore(Reason::GeometryUnavailable);return false;
    }
    bool current(std::uint64_t now,Source& s) noexcept {
        return io_.source&&io_.source(s)&&fresh(s,now)&&sameSource(s,state_.owner.source)&&
            io_.current&&io_.current(state_.owner);
    }
    bool bootstrapCurrent(std::uint64_t now) noexcept {
        return !state_.sourceEstablished&&state_.started&&now>=state_.started&&now<=state_.firstPoseDeadline&&
            io_.current&&io_.current(state_.owner);
    }
public:
    void configure(std::uintptr_t base,const Ops& io) noexcept {base_=base;io_=io;}
    StartReasonD69 startReasonD69() const noexcept {return startReasonD69_;}
    void resetStartReasonD69() noexcept {startReasonD69_=StartReasonD69::NotAttempted;}
    const Status& status() const noexcept {return state_;}
    const Clip& clip() const noexcept {return clip_;}
    bool pending() const noexcept {return wrote_||uncertain_;}
    bool newGripR95(const Source& s,std::uint64_t now) const noexcept {
        const bool grip=s.entryGripD72&&s.entryGripD72<=3&&s.entryGripTickD72&&
            now>=s.entryGripTickD72&&now-s.entryGripTickD72<=80;
        return grip&&!(gripAdmissionTickR95_==s.entryGripTickD72&&sameSource(s,gripAdmissionSourceR95_));
    }
    bool acceptsNewActionR95(const Source& s,std::uint64_t now) const noexcept {
        return newGripR95(s,now)||(continuationArmedR95_&&sameSource(s,continuationSourceR95_));
    }
    bool restore(Reason reason) noexcept {
        if(reason!=Reason::NativeStop&&reason!=Reason::OwnerChanged)
            continuationArmedR95_=false;
        // Presentation ownership ends immediately even if the original clock
        // still has a retained restore debt. No old constraint survives loss.
        state_.constraintD78={};state_.toolMountedD78=false;
        noteFallbackD65C(reason,pending()&&state_.clockOwned&&!physical_);
        if(!pending()){state_.clockOwned=false;state_.reason=reason;return true;}
        const auto member=membership(io_,base_,state_.owner);
        if(member==Member::Unknown){state_.phase=Phase::Retiring;state_.reason=Reason::PendingRestore;return false;}
        if(member==Member::Absent){wrote_=uncertain_=false;state_.clockOwned=false;state_.reason=reason;state_.phase=Phase::None;return true;}
        std::uintptr_t vf=0,context=0;unsigned token=0;
        if(!rd(io_,state_.owner.child,vf)||!rd(io_,state_.owner.child+8,context)||!rd(io_,state_.owner.child+0x54,token)){
            state_.phase=Phase::Retiring;state_.reason=Reason::PendingRestore;return false;}
        if(vf!=base_+0x3A450B0||context!=state_.owner.context||token!=state_.owner.token){
            wrote_=uncertain_=false;state_.clockOwned=false;state_.reason=reason;state_.phase=Phase::None;return true;}
        float speed=0;if(!rd(io_,state_.owner.child+0x5C,speed)){state_.phase=Phase::Retiring;state_.reason=Reason::PendingRestore;return false;}
        if(speed!=last_&&(!uncertain_||speed!=candidate_)){
            wrote_=uncertain_=false;state_.clockOwned=false;state_.reason=Reason::ForeignSpeed;state_.phase=Phase::Unavailable;return true;}
        if(!io_.write||!io_.write(state_.owner.child+0x5C,&saved_,sizeof(saved_))||
           !rd(io_,state_.owner.child+0x5C,speed)||speed!=saved_){state_.phase=Phase::Retiring;state_.reason=Reason::PendingRestore;return false;}
        wrote_=uncertain_=false;state_.clockOwned=false;state_.speed=saved_;state_.reason=reason;state_.phase=Phase::None;return true;
    }
    bool startNewAfterRestore(const Owner& admitted,float saved,std::uint64_t now) noexcept {
        resetStartReasonD69();
        const auto refuse=[&](StartReasonD69 why) noexcept {startReasonD69_=why;return false;};
        // Same short-circuit order and native reads as the former aggregate.
        if(pending())return refuse(StartReasonD69::PendingRestore);
        auto attempted=admitted;attempted.serial=attemptedActionR95_.serial;
        // R108: "live" is bounded. The flag cleared only on the native Stop
        // hook of that parent; an action that ended through completion /
        // destroy kept it, and the allocator can hand the next reload action
        // the same parent/child/context addresses -> a NEW attempt refused as
        // AlreadyActive until restart (a latch). No authored reload lasts
        // 20 s (same bound as kXbowNativeStaleR96); a running action is still
        // refused below by its own active flag (child+0x10 != 0).
        const bool attemptedLive=attemptedActionLiveR95_&&attemptedActionTickR95_&&now>=attemptedActionTickR95_&&
            now-attemptedActionTickR95_<=20000;
        if(attemptedLive&&sameOwner(attempted,attemptedActionR95_))
            return refuse(StartReasonD69::AlreadyActive); // same action stays native after failure
        if(!base_)return refuse(StartReasonD69::NoBase);
        if(!io_.current)return refuse(StartReasonD69::NoCurrentReader);
        if(!io_.current(admitted))return refuse(StartReasonD69::CurrentOwner);
        if(!fresh(admitted.source,now))return refuse(StartReasonD69::SourceInvalid);
        if(!std::isfinite(saved)||saved<=0||saved>8)return refuse(StartReasonD69::SpeedInvalid);
        const auto member=membership(io_,base_,admitted);float active=-1;
        if(member!=Member::Queued)return refuse(member==Member::Unknown?StartReasonD69::MembershipUnknown:StartReasonD69::NotQueued);
        if(!rd(io_,admitted.child+0x10,active))return refuse(StartReasonD69::ActiveUnreadable);
        if(active!=0)return refuse(StartReasonD69::AlreadyActive);
        if(io_.newAdmission&&!io_.newAdmission(admitted))return refuse(StartReasonD69::HandAdmission);
        continuationAdmissionR95_=continuationArmedR95_&&sameSource(admitted.source,continuationSourceR95_)&&
            !newGripR95(admitted.source,now);
        if(newGripR95(admitted.source,now)){gripAdmissionSourceR95_=admitted.source;gripAdmissionTickR95_=admitted.source.entryGripTickD72;}
        continuationArmedR95_=false;
        state_={};state_.owner=admitted;state_.owner.serial=++next_;
        attemptedActionR95_=state_.owner;attemptedActionLiveR95_=true;attemptedActionTickR95_=now?now:1;state_.phase=Phase::AwaitClip;state_.reason=Reason::WaitingClip;
        completedNativeLease.store(0,std::memory_order_release);
        placementTickR94_=setupWaitR95R2_=0;quiverInputR95_={};quiverReachR95_=false;clip_={};saved_=last_=saved;neutral_=grasp_=physical_=false;progress_=0;lastSample_=0;permitted_=0;started_=now;
        bindingCaptured_=false;graspBinding_={};chestVisited_=chestGate_=false;crankGraspTravelD66_=0;entryGripConsumedD72_=false;mechanismWaitD78_=0;stringGripD78_={};
        state_.started=now;state_.firstPoseDeadline=now<=UINT64_MAX-kPausedJoinMsD85?now+kPausedJoinMsD85:UINT64_MAX; // D85 xbow
        fallbackStageD85.store(0,std::memory_order_relaxed);lastBootstrapRefusalD85.store(nullptr,std::memory_order_relaxed);
        const bool paused=write(0); // no old cleanup/native restore inside the admission lease
        startReasonD69_=paused?StartReasonD69::Taken:StartReasonD69::InitialPauseFailed;return paused;
    }
    // Atomic/POD-only finish metadata: uncertain first writes need restore on
    // the next existing owner tick even if an exception skips the normal tail.
    void markStartFailure() noexcept {if(pending()){state_.phase=Phase::Retiring;state_.reason=Reason::PendingRestore;}}
    bool start(const Owner& admitted,float saved,std::uint64_t now) noexcept {
        if(!restore(Reason::OwnerChanged))return false;
        if(startNewAfterRestore(admitted,saved,now))return true;
        if(pending())(void)restore(Reason::NativeFallback);return false;
    }
    bool metadata(int animation,int scope,std::uint64_t now) noexcept {
        if(!pending()||(state_.phase!=Phase::AwaitClip&&state_.phase!=Phase::NativeTransitionD80))return false;
        Source s{};Clip c{};
        if(!(current(now,s)||bootstrapCurrent(now))||!io_.clip||!io_.clip(state_.owner,animation,scope,c)||!c.qualified||
           !c.list||!c.entries||c.count<1||c.count>256||c.segment!=0||c.duration<=.1f||c.duration>60||
           (c.nativeTransitionD80?(c.cocking||c.loading||c.spawn>=0||c.ammo>=0||c.cock>=0||c.load>=0||c.despawn>=0):
           (c.cocking==c.loading||(c.cocking&&(!(c.cock>0&&c.cock<1)))||
           (c.loading&&(!(c.ammo>=0&&c.load>c.ammo&&c.load<1)))))){
            (void)restore(Reason::ClipUnsupported);return false;}
        if(continuationAdmissionR95_&&!c.nativeTransitionD80&&!c.loading){
            (void)restore(Reason::NativeFallback);return false;}
        // A qualified reload child can first select an audio-only transition.
        // It has no gesture endpoint: retain the exact action/source lease, but
        // forward native time once the first completed wrist exists. The next
        // real metadata callback may select cock/load without a second LT.
        const bool wasNativeTransitionD80=state_.phase==Phase::NativeTransitionD80;
        placementTickR94_=setupWaitR95R2_=0;clip_=c;state_.cocking=c.cocking;state_.loading=c.loading;
        state_.placeTool=c.cocking;state_.placeBolt=c.loading;
        state_.constraintD78={};state_.toolMountedD78=false;
        state_.phase=c.nativeTransitionD80?Phase::NativeTransitionD80:Phase::Setup;
        state_.reason=c.nativeTransitionD80?Reason::NativeTransitionD80:Reason::Ready;
        // Metadata is observed original-first inside native Update. The
        // selected CAF can already scan its initial event-free interval in
        // that same Update. A zero limit revoked the reload on harmless
        // initial motion before the next pre-controller tick. Admit only the
        // exact table-proven prefix BEFORE its earliest native effect, then
        // pause. No ammo/cock/load/flag event inherits transition permission.
        if(wasNativeTransitionD80&&!c.nativeTransitionD80){
            permitted_=std::isfinite(c.firstNativeEffectR92)&&c.firstNativeEffectR92>0?
                std::max(0.f,c.firstNativeEffectR92-1e-4f):0.f;
            if(!write(0)){(void)restore(Reason::NativeFallback);return false;}
        }
        return true;
    }
    void tick(std::uintptr_t ctrl,float dt,std::uint64_t now) noexcept {
        if(!pending()||ctrl!=state_.owner.controller)return;
        if(state_.phase==Phase::Retiring){(void)restore(Reason::NativeStop);return;}
        Source s{};const bool sourceCurrent=current(now,s);
        if((sourceCurrent&&s.completedLease==state_.owner.serial)||completedNativeLease.load(std::memory_order_acquire)==state_.owner.serial)
            state_.sourceEstablished=true;
        if(!state_.sourceEstablished){
            const auto member=membership(io_,base_,state_.owner);
            // No native interval or resource event can advance before the FIRST
            // completed tracked wrist. Native Queue/metadata may precede the
            // selected FIFO and its pose. Hold only our existing zero during
            // the ORIGINAL paused window (D85 xbow: kPausedJoinMsD85, fixed at
            // start, never renewed); requiring the missing clock here
            // discarded the owner immediately, before it could produce a pose.
            // The passenger producer still needs a fully qualified clock.
            if(bootstrapCurrent(now)&&(member==Member::Queued||member==Member::Root)&&last_==0){
                state_.reason=Reason::WaitingFirstPose;if(!write(0))(void)restore(Reason::NativeFallback);return;}
            fallbackStageD85.store(1,std::memory_order_relaxed); // D85 xbow: no first tracked pose
            (void)restore(Reason::InputLost);return;
        }
        if(!sourceCurrent){(void)restore(Reason::InputLost);return;}
        if(handDrivenD81(state_.owner.source))observeQuiverR95(s,now);
        const auto member=membership(io_,base_,state_.owner);
        if((state_.phase==Phase::AwaitClip||member==Member::Queued)&&(now<started_||now-started_>1000)){
            (void)restore(Reason::ClipUnsupported);return;}
        if(member==Member::Queued){if(!write(0))(void)restore(Reason::NativeFallback);return;}
        if(member!=Member::Root){(void)restore(member==Member::Unknown?Reason::UnknownMembership:Reason::NativeStop);return;}
        if(state_.phase==Phase::AwaitClip){
            if(now<started_||now-started_>1000){(void)restore(Reason::ClipUnsupported);return;}
            if(!write(0))(void)restore(Reason::NativeFallback);return;}
        Clock clock{};Spatial spatial{};
        if(!io_.clock||!io_.clock(state_.owner,clip_,clock)||!clock.qualified||clock.segment!=clip_.segment||
           !std::isfinite(dt)||dt<=0||dt>.08f||clock.controllerScale<=0||clock.controllerScale>8||
           clock.characterScale!=clock.controllerScale||clock.playbackScale<=0||clock.playbackScale>8||
           !std::isfinite(clock.normalized)||clock.normalized<0||clock.normalized>1||clock.remaining<0){
            (void)restore(Reason::ClockUnsupported);return;}
        if(state_.phase==Phase::NativeTransitionD80){
            // R95: this effect-free transition does not depend on a tool/socket
            // payload. A missing auxiliary pose cannot modulate its native rate.
            // The original source freshness and exact native clock still gate it.
            // No tool/string constraint, time compression, resource admission
            // or inherited completion credit. Current source/owner/clip clock
            // were qualified above; no old pose age or bootstrap is renewed.
            if(s.auxiliaryReleasedR95){grasp_=false;neutral_=true;entryGripConsumedD72_=true;}
            if(s.sample>lastSample_){lastSample_=s.sample;
                if(s.grip[1-s.primary]<.25){neutral_=true;entryGripConsumedD72_=true;}}
            state_.reason=Reason::NativeTransitionD80;permitted_=1.f;
            if(!write(saved_))(void)restore(Reason::NativeFallback);
            return;
        }
        if(s.auxiliaryUnavailableR95){
            // This is the ORIGINAL last complete source, never an ordinary
            // body relabelled as reload input. Its 80-ms age was checked by
            // current(); a new failed observation cannot extend that budget.
            // A captured tool/bolt replacement is a revocation, not a pause.
            if(bindingCaptured_){Spatial binding{};
                const bool same=io_.binding&&io_.binding(state_.owner,binding)&&binding.qualified&&
                    binding.serial==state_.owner.serial&&
                    (clip_.cocking?(binding.toolBound&&binding.toolKey==graspBinding_.toolKey&&
                        binding.toolId==graspBinding_.toolId&&binding.toolItem==graspBinding_.toolItem&&
                        binding.toolEntity==graspBinding_.toolEntity&&binding.toolAttachment==graspBinding_.toolAttachment):
                        (binding.handBoltBound&&binding.boltKey==graspBinding_.boltKey&&binding.boltId==graspBinding_.boltId&&
                        binding.boltEntity==graspBinding_.boltEntity&&binding.boltAttachment==graspBinding_.boltAttachment));
                if(!same){(void)restore(Reason::PhysicalBindingLost);return;}
            }
            state_.constraintD78={};permitted_=clock.normalized;
            // A real open hand during the gap is consumed without nocking or
            // stroke credit. Recovery must not replay that release as a valid
            // placement against a later socket/pose.
            if(s.auxiliaryReleasedR95){grasp_=false;state_.toolEngagedD72=false;
                neutral_=true;entryGripConsumedD72_=true;}
            state_.reason=Reason::WaitingSourceR95;
            if(!write(0))(void)restore(Reason::NativeFallback);
            return; // no new sample, Grip edge, stroke, event or ammunition credit
        }
        if(!io_.spatial||!io_.spatial(state_.owner,s,spatial)||!spatial.qualified||spatial.serial!=state_.owner.serial||
           spatial.sample>s.sample||!spatial.tick||now<spatial.tick||now-spatial.tick>80){
            (void)restore(Reason::GeometryUnavailable);return;}
        const int hand=1-s.primary;float target=clock.normalized;
        const float edge=clip_.cocking?clip_.cock:clip_.load;
        if(handDrivenD81(state_.owner.source)){
            // Placement gaps are checked before consuming any new input edge.
            if(state_.phase==Phase::Span){
                if(bindingCaptured_&&(!spatial.toolBound||spatial.toolKey!=graspBinding_.toolKey||
                   spatial.toolId!=graspBinding_.toolId||spatial.toolItem!=graspBinding_.toolItem||
                   spatial.toolEntity!=graspBinding_.toolEntity||spatial.toolAttachment!=graspBinding_.toolAttachment)){
                    (void)restore(Reason::PhysicalBindingLost);return;}
                if(bindingCaptured_&&!spatial.toolPoseReady&&!state_.toolMountedD78){
                    if(pausePlacementR94(s,clock,now))return;
                    (void)restore(Reason::PhysicalBindingLost);return;}
                if(bindingCaptured_&&spatial.toolPoseReady)rememberPlacementR94(spatial,now);
            }
            if(state_.phase==Phase::PlaceBolt){
                if(!spatial.handBoltBound||spatial.boltId!=graspBinding_.boltId||
                   spatial.boltKey!=graspBinding_.boltKey||spatial.boltEntity!=graspBinding_.boltEntity||
                   spatial.boltAttachment!=graspBinding_.boltAttachment){(void)restore(Reason::PhysicalBindingLost);return;}
                if(!spatial.boltPoseReady){
                    if(pausePlacementR94(s,clock,now))return;
                    (void)restore(Reason::PhysicalBindingLost);return;}
                rememberPlacementR94(spatial,now);
            }
            // A source-scoped originating Grip survives native admission. It
            // never becomes generic credit after an item/body/request change.
            const auto& origin=state_.owner.source;
            const Vec intent=intentHandD78(s,hand),originIntent=intentHandD78(origin,hand);
            state_.constraintD78.active=false;
            const bool sample=s.sample>lastSample_;if(sample){lastSample_=s.sample;
                if(s.grip[hand]<.25){neutral_=true;entryGripConsumedD72_=true;}}
            const bool entry=!entryGripConsumedD72_&&origin.entryGripD72&&origin.entryGripTickD72&&
                origin.entryGripTickD72<=state_.started&&state_.started-origin.entryGripTickD72<=80;
            const bool releasedAheadR95R2=clip_.loading&&quiverInputR95_.sample>=s.sample&&
                quiverInputR95_.tick>=s.tick&&now>=quiverInputR95_.tick&&now-quiverInputR95_.tick<=80&&quiverInputR95_.grip<.25;
            const bool held=s.grip[hand]>.65&&!releasedAheadR95R2;
            // D76: the exact initiating Grip already occurred at the string,
            // pouch or quiver before native Start/metadata. It is a one-use
            // acquisition intent, not a demand to remain in that region while
            // the native prop/first completed wrist arrives. Release consumes
            // it above; the current owner, held Grip and actual prop/readback
            // are still mandatory. No event or inventory credit is synthesized.
            const bool pouchEntry=entry&&origin.entryGripD72==2&&origin.chestKnown&&origin.handInChest[hand];
            const bool quiverEntry=entry&&origin.entryGripD72==3&&origin.quiverKnownD72&&origin.handInQuiverD72[hand];
            const float setup=clip_.cocking?std::max(0.f,clip_.spawn):clip_.ammo;
            if(!(setup<edge)){(void)restore(Reason::ClipUnsupported);return;}
            const float beforeSetup=std::max(0.f,setup-1e-4f);
            const float afterSetup=std::min(setup+1e-5f,edge-1e-4f);
            if(state_.phase==Phase::Setup){
                target=beforeSetup;
                if(clip_.cocking&&s.classId==10){state_.phase=Phase::Span;target=afterSetup;}
                else if(clip_.cocking){
                    if(!s.chestKnown){(void)restore(Reason::ChestUnknown);return;}
                    const bool pouchGrip=held&&((s.handInChest[hand]&&neutral_)||pouchEntry);
                    const bool waitingMechanism=s.intentKnownD78&&s.classId==15&&pouchGrip&&spatial.toolBound&&
                        !spatial.toolPoseReady&&clock.normalized>=beforeSetup-1e-5f;
                    const bool waitingPlacementR95R2=!state_.toolTakenD72&&pouchGrip&&
                        !(spatial.toolBound&&spatial.toolPoseReady)&&clock.normalized>=beforeSetup-1e-5f;
                    const bool waitingRevealR95R2=state_.toolTakenD72&&!spatial.toolVisible&&clock.normalized>=afterSetup-1e-5f;
                    if(!setupJoinCurrentR95R2(waitingPlacementR95R2||waitingRevealR95R2,now))return;
                    // Native acquired main+80 before the hidden setup. Reveal
                    // only after the SAME real tool was placed/read back at the
                    // tracked pouch hand; the original SpawnLever does showing.
                    if(!state_.toolTakenD72&&sample&&pouchGrip&&spatial.toolBound&&spatial.toolPoseReady&&
                       clock.normalized>=beforeSetup-1e-5f){
                        state_.toolTakenD72=true;entryGripConsumedD72_=true;neutral_=false;grasp_=true;
                        bindingCaptured_=true;graspBinding_=spatial;rememberPlacementR94(spatial,now);
                    }
                    if(state_.toolTakenD72){target=afterSetup;
                        if(spatial.toolVisible){state_.phase=Phase::Span;state_.reason=Reason::WaitingMountD72;}}
                    else state_.reason=waitingMechanism?Reason::WaitingMechanismD78:
                        waitingPlacementR95R2?Reason::WaitingPlacementPoseR94:Reason::WaitingChest;
                }else{
                    if(!s.quiverKnownD72){(void)restore(Reason::GeometryUnavailable);return;}
                    // Native reload changes clips between cocking and loading. Requiring
                    // a fresh neutral Grip in this child discarded the same quiver
                    // reach that already retrieves the working bow arrow.
                    const bool newerQuiver=quiverInputR95_.tick>=s.tick&&quiverInputR95_.sample>=s.sample&&
                        now>=quiverInputR95_.tick&&now-quiverInputR95_.tick<=80;
                    const bool quiverGrip=(newerQuiver?quiverInputR95_.grip>.65:held)&&
                        ((newerQuiver?quiverInputR95_.inside:s.handInQuiverD72[hand])||quiverEntry||quiverReachR95_);
                    // LoadReachedAmmo remains the native finite-stock acquire;
                    // it cannot run before the tracked quiver Grip.
                    if(!state_.boltTakenD72&&sample&&quiverGrip){state_.boltTakenD72=true;quiverReachR95_=false;
                        entryGripConsumedD72_=true;neutral_=false;grasp_=true;}
                    if(state_.boltTakenD72){target=afterSetup;
                        // Opening before the first achieved bolt placement is
                        // input retirement, never a deferred nock. A later
                        // binding/pose cannot replay that release at the rail.
                        if(s.grip[hand]<.25||releasedAheadR95R2){grasp_=false;neutral_=true;}
                        const bool joined=spatial.handBoltBound&&spatial.boltPoseReady;
                        if(!setupJoinCurrentR95R2(!joined&&clock.normalized>=afterSetup-1e-5f,now))return;
                        if(joined){state_.phase=Phase::PlaceBolt;
                            bindingCaptured_=true;graspBinding_=spatial;rememberPlacementR94(spatial,now);}
                        else state_.reason=Reason::WaitingBolt;
                    }else{(void)setupJoinCurrentR95R2(false,now);state_.reason=Reason::WaitingQuiverD72;}
                }
            }
            if(state_.phase==Phase::Span){
                const bool tool=s.classId!=10,crank=s.classId==15;
                const bool travelValid=crank?(spatial.crankQualified&&crankTravelValidD66C(spatial.travel)):
                    (std::isfinite(spatial.travel)&&spatial.travel>=.03&&spatial.travel<=.5);
                if(!finite(spatial.axis)||std::abs(length(spatial.axis)-1)>1e-4||!finite(spatial.stringRest)||
                   !travelValid){(void)restore(Reason::GeometryUnavailable);return;}
                // The native tool remains bound while this reload owns the
                // working hand. A real release/regrip may resume that same
                // read-back prop; it cannot acquire another tool or skip the
                // subsequent string contact and full stroke requirements.
                if(sample&&tool&&state_.toolTakenD72&&bindingCaptured_&&!grasp_&&neutral_&&held){
                    grasp_=true;neutral_=false;
                }
                const bool mechanism=crank&&s.intentKnownD78;
                const bool stringMechanism=!crank&&s.intentKnownD78;
                const bool mechanismCurrent=mechanism?(spatial.toolMechanismD72&&
                    sameMechanismClockD78(spatial.mechanismClockD78,clock)&&spatial.mechanismTickD78&&
                    now>=spatial.mechanismTickD78&&now-spatial.mechanismTickD78<=80):
                    !stringMechanism||(spatial.stringMechanismD78&&finite(spatial.stringCurrentD78)&&
                    sameMechanismClockD78(spatial.stringClockD78,clock)&&spatial.stringTickD78&&
                    now>=spatial.stringTickD78&&now-spatial.stringTickD78<=80);
                const bool stringEntry=!tool&&entry&&origin.entryGripD72==1&&
                    length(originIntent-spatial.stringRest)<.12;
                const bool qualifiedGrip=held&&(neutral_||stringEntry);
                // A mounted lever remains on its native socket after release.
                // Regripping it requires the CURRENT measured handle, never
                // the former free-hand prop destination or string-rest proxy.
                // After an interrupted pull, the string is already part-way back.
                // Acquire its measured current position; demanding its rest
                // position made the second half of a pull unreachable.
                const Vec contact=state_.toolMountedD78?spatial.toolHandleD72:
                    (stringMechanism&&mechanismCurrent&&progress_>0?spatial.stringCurrentD78:spatial.stringRest);
                const bool contactGrip=(stringEntry||length(intent-contact)<.12)&&
                    (!s.intentKnownD78||stringEntry||length(s.hand[hand]-contact)<.12)&&
                    ((tool&&state_.toolTakenD72&&grasp_)||(!tool&&qualifiedGrip));
                if(sample&&!state_.toolEngagedD72&&mechanismCurrent&&contactGrip){
                    state_.toolEngagedD72=true;entryGripConsumedD72_=true;neutral_=false;grasp_=true;
                    const Vec acquired=stringEntry?originIntent:intent;
                    if(crank)crankGraspTravelD66_=spatial.travel;
                    const double travel=crank?crankGraspTravelD66_:spatial.travel;
                    // Keep earned native progress while rebasing the new
                    // grasp. The remaining half-stroke must remain half a
                    // stroke; do not require a second full pull after release.
                    start_=acquired-scale(spatial.axis,progress_*travel);
                    if(stringMechanism)stringGripD78_=acquired-spatial.stringCurrentD78;
                    if(mechanism)state_.toolMountedD78=true;
                }
                if(sample&&grasp_&&s.grip[hand]<.25){grasp_=false;state_.toolEngagedD72=false;neutral_=true;}
                const bool mountedReady=mechanism?(mechanismCurrent&&spatial.toolMountedPoseD78&&spatial.toolPoseReady):mechanismCurrent;
                const HandConstraintD78 contactDomain{start_,spatial.axis,crank?crankGraspTravelD66_:spatial.travel,progress_,true,mechanism,stringGripD78_};
                if(sample&&s.intentKnownD78&&state_.toolEngagedD72&&grasp_&&mountedReady&&
                   !reloadContactDomainD78(contactDomain,spatial,intent)){
                    // Presentation and event authority share one contact domain.
                    // Re-entering while still squeezing is not a new acquisition.
                    grasp_=false;state_.toolEngagedD72=false;neutral_=false;
                }
                if(sample&&state_.toolEngagedD72&&grasp_&&mountedReady){
                    const Vec delta=intent-start_;const double axial=dot(delta,spatial.axis);
                    const double lateral=std::sqrt(std::max(0.,dot(delta,delta)-axial*axial));
                    // Only unmodified controller intent earns stroke credit.
                    // The constrained hand/native tool pose is a destination.
                    if(lateral<=.12)progress_=std::max(progress_,std::clamp(axial/(crank?crankGraspTravelD66_:spatial.travel),0.,1.));
                }
                state_.progress=float(progress_);physical_=progress_>=1.&&mountedReady;
                target=std::min(afterSetup+(edge-afterSetup)*float(progress_),edge-1e-4f);
                state_.reason=state_.toolEngagedD72?Reason::Working:tool?Reason::WaitingMountD72:Reason::WaitingGrip;
                if((mechanism&&((state_.toolMountedD78&&!mountedReady)||
                   (!state_.toolMountedD78&&!state_.toolEngagedD72&&contactGrip&&!mechanismCurrent)))||
                   (stringMechanism&&!mechanismCurrent&&(state_.toolEngagedD72||contactGrip))){
                    // Native pose work may follow the owner update. Hold this
                    // clock while that exact join catches up, bounded to the
                    // D85 paused window (target = current clock: rate 0).
                    // Missing pose never grants a stroke or a native event.
                    if(!mechanismWaitD78_)mechanismWaitD78_=now;
                    if(now<mechanismWaitD78_||now-mechanismWaitD78_>kPausedJoinMsD85){
                        fallbackStageD85.store(2,std::memory_order_relaxed);(void)restore(Reason::GeometryUnavailable);return;}
                    target=clock.normalized;state_.reason=Reason::WaitingMechanismD78;
                }else mechanismWaitD78_=0;
                // R98: one presentation writer for the engaged hand. The string
                // witness lags one frame whenever the clock moved; keep the same
                // constraint then (no stroke credit, see laggingStringClockR98).
                const bool stringLagR98=stringMechanism&&!mechanismCurrent&&spatial.stringMechanismD78&&
                    finite(spatial.stringCurrentD78)&&laggingStringClockR98(spatial.stringClockD78,clock)&&
                    spatial.stringTickD78&&now>=spatial.stringTickD78&&now-spatial.stringTickD78<=80;
                if(s.intentKnownD78&&state_.toolEngagedD72&&grasp_&&(mountedReady||stringLagR98)){
                    state_.constraintD78={start_,spatial.axis,crank?crankGraspTravelD66_:spatial.travel,
                        progress_,true,mechanism,stringGripD78_};
                }
            }
            if(state_.phase==Phase::PlaceBolt){
                target=afterSetup;state_.reason=Reason::WaitingPlacement;
                if(sample&&!grasp_&&neutral_&&held){grasp_=true;neutral_=false;}
                if(sample&&grasp_&&s.grip[hand]<.25){physical_=spatial.handBoltPlaced;grasp_=false;
                    // A misplaced release has earned no load event. Keep the
                    // SAME native bolt and paused owner so the player can grip
                    // it again and place it. Never resume automatic loading.
                    if(!physical_)neutral_=true;}
            }
            if(physical_){state_.phase=Phase::Tail;target=1.f;state_.reason=Reason::Working;}
            if(state_.phase==Phase::Tail){target=1.f;
                if(clip_.cocking?spatial.nativeCocked:(spatial.nativeLoaded&&spatial.nockBound)){
                    state_.reason=Reason::NativeAck;
                    if(physical_&&clip_.cocking){continuationSourceR95_=state_.owner.source;continuationArmedR95_=true;}
                }}
        }else{
        if(state_.phase==Phase::Setup){
            const float setup=clip_.cocking?std::max(0.f,clip_.spawn):clip_.ammo;
            // The selected native event interval owns setup. Never grant a
            // frame that also crosses the unapproved native cock/load boundary.
            if(!(setup<edge)){(void)restore(Reason::ClipUnsupported);return;}
            target=std::min(setup+1e-5f,edge-1e-4f);
            // D66C heavy crank: the crank may only spawn after the working hand
            // visited the chest and came back to the string. Until then the clock
            // is held just BEFORE the native spawn event (never crosses it).
            bool gate=true;
            if(clip_.cocking&&s.classId==15){
                if(!s.chestKnown){(void)restore(Reason::ChestUnknown);return;}
                if(!spatial.crankQualified||!finite(spatial.stringRest)){(void)restore(Reason::GeometryUnavailable);return;}
                if(s.handInChest[hand])chestVisited_=true;
                else if(chestVisited_&&length(s.hand[hand]-spatial.stringRest)<kChestReturnD66C)chestGate_=true;
                gate=chestGate_;
                if(!gate)target=std::max(0.f,setup-1e-4f);
            }
            const bool ready=gate&&(clip_.cocking?(s.classId==10||(spatial.toolBound&&spatial.toolVisible)):spatial.handBoltBound);
            if(ready){state_.phase=clip_.cocking?Phase::Span:Phase::TakeBolt;state_.reason=Reason::WaitingNeutral;}
            else state_.reason=!gate?Reason::WaitingChest:clip_.cocking?Reason::WaitingTool:Reason::WaitingBolt;
        }
        if(state_.phase==Phase::Span||state_.phase==Phase::TakeBolt||state_.phase==Phase::PlaceBolt){
            if(bindingCaptured_){
                const bool same=clip_.cocking?
                    (spatial.toolBound&&spatial.toolPoseReady&&spatial.toolKey==graspBinding_.toolKey&&spatial.toolId==graspBinding_.toolId&&
                     spatial.toolItem==graspBinding_.toolItem&&spatial.toolEntity==graspBinding_.toolEntity&&spatial.toolAttachment==graspBinding_.toolAttachment):
                    (spatial.handBoltBound&&spatial.boltPoseReady&&spatial.boltKey==graspBinding_.boltKey&&spatial.boltId==graspBinding_.boltId&&
                     spatial.boltEntity==graspBinding_.boltEntity&&spatial.boltAttachment==graspBinding_.boltAttachment);
                if(!same){(void)restore(Reason::PhysicalBindingLost);return;}
            }
            if(s.sample>lastSample_){lastSample_=s.sample;
                if(s.grip[hand]<.25)neutral_=true;
                if(clip_.cocking){
                    // D66C: the heavy crank (class 15) runs the same grasp/pull as the
                    // goat's foot, with the producer's crank contract (string rest grip,
                    // forward rail axis, VrCrankTravelD66C) and the stricter "level" rule.
                    const bool crank=s.classId==15;
                    if(crank&&!spatial.crankQualified){(void)restore(Reason::GeometryUnavailable);return;}
                    const double axisLength=length(spatial.axis);
                    const bool travelValid=crank?crankTravelValidD66C(spatial.travel):
                        (std::isfinite(spatial.travel)&&spatial.travel>=.03&&spatial.travel<=.5);
                    if(!finite(spatial.axis)||std::abs(axisLength-1)>1e-4||!travelValid){
                        (void)restore(Reason::GeometryUnavailable);return;}
                    if(!grasp_&&neutral_&&spatial.toolPoseReady&&s.grip[hand]>.65&&length(s.hand[hand]-spatial.grip)<.12){
                        grasp_=true;start_=s.hand[hand];if(crank)crankGraspTravelD66_=spatial.travel;if(s.classId!=10){bindingCaptured_=true;graspBinding_=spatial;}}
                    if(grasp_&&s.grip[hand]<.25){grasp_=false;neutral_=true;}
                    if(grasp_){const auto delta=s.hand[hand]-start_;const auto axial=dot(delta,spatial.axis);
                        const double lateral=std::sqrt(std::max(0.,dot(delta,delta)-axial*axial));
                        bool level=lateral<=(crank?kCrankLateralD66C:.08);
                        if(crank&&level){ // no lifting/dropping relative to the rail: off-axis part along model up (+Z)
                            const Vec perp=delta-scale(spatial.axis,axial);
                            const float worldQ[4]{s.world[0],s.world[1],s.world[2],s.world[3]};
                            level=std::abs(dot(perp,rotateInverse(s.weaponModel.q,rotateInverse(worldQ,{0,0,1}))))<=kCrankVerticalD66C;}
                        if(level)progress_=std::max(progress_,std::clamp(axial/(crank?crankGraspTravelD66_:spatial.travel),0.,1.));}
                    physical_=progress_>=1.;
                }else{
                    if(!grasp_&&neutral_&&spatial.handBoltBound&&spatial.boltPoseReady&&s.grip[hand]>.65){
                        grasp_=true;state_.phase=Phase::PlaceBolt;bindingCaptured_=true;graspBinding_=spatial;}
                    if(grasp_&&s.grip[hand]<.25){physical_=spatial.handBoltPlaced;grasp_=false;}
                }
            }
            state_.progress=float(progress_);
            if(physical_){state_.phase=Phase::Tail;target=1.f;state_.reason=Reason::Working;}
            else if(clip_.cocking){const float setup=std::max(0.f,clip_.spawn);
                target=setup+(edge-setup)*float(progress_);target=std::min(target,edge-1e-4f);state_.reason=grasp_?Reason::Working:Reason::WaitingGrip;}
            else{target=std::min(clip_.ammo+1e-5f,edge-1e-4f);state_.reason=Reason::WaitingPlacement;}
        }
        if(state_.phase==Phase::Tail){target=1.f;
            if(clip_.cocking?spatial.nativeCocked:(spatial.nativeLoaded&&spatial.nockBound)){
                    state_.reason=Reason::NativeAck;
                    if(physical_&&clip_.cocking){continuationSourceR95_=state_.owner.source;continuationArmedR95_=true;}
                }}
        }
        const bool handMilestoneD81=physical_&&handDrivenD81(state_.owner.source)&&
            !state_.owner.source.instantD72&&clock.normalized<edge;
        permitted_=physical_?(handMilestoneD81?std::min(1.f,edge+1e-5f):1.f):std::min(target,edge-1e-4f);
        const double normalized=std::max(0.,double(permitted_)-clock.normalized);
        const double nativeSeconds=(physical_&&clock.remaining<=1e-5f)?1e-3:
            normalized*clip_.duration/clock.playbackScale;
        // Hand displacement must reach its permitted animation coordinate in
        // this update even when fast native tails are OFF. Only unperformed
        // native tail time retains the optional speed setting.
        const bool transport=state_.owner.source.instantD72||
            (handDrivenD81(state_.owner.source)&&(state_.phase!=Phase::Tail||handMilestoneD81));
        const double maximumRate=transport?kClockTransportMaxD72:saved_;
        const double offered=std::min(nativeSeconds,double(dt)*clock.controllerScale*maximumRate);
        // Completion lookahead consumes this same local speed. It must not
        // reach an unapproved finalizer even on clips with late/missing events.
        const double completionCap=physical_?offered:std::max(0.,double(clock.remaining)-1e-5);
        const float rate=float(std::min(offered,completionCap)/(double(dt)*clock.controllerScale));
        if(!write(std::clamp(rate,0.f,float(maximumRate))))(void)restore(Reason::NativeFallback);
    }
    bool interval(std::uintptr_t list,int animation,int segment,float from,float to,std::uint64_t diagnosticNowD87=0) noexcept {
        if(!pending()||!clip_.qualified||list!=clip_.list||animation!=clip_.id||segment!=clip_.segment)return false;
        if(!std::isfinite(from)||!std::isfinite(to)||from<0||to<from||to>1||
           (state_.phase!=Phase::NativeTransitionD80&&!physical_&&to>permitted_+1e-5f)){
            if(diagnosticNowD87)reload_diagnostics_d48::recordNativeFaultD87(state_,"INTERVAL_LIMIT",diagnosticNowD87,
                {},list,clip_.list,animation,segment,from,to,permitted_,physical_?1u:0u);
            (void)restore(Reason::UnapprovedInterval);return false;}
        return true;
    }
    float permittedIntervalD87() const noexcept {return permitted_;}
    void eventBefore(unsigned crc) noexcept {
        // Withdraw BEFORE the original native removal/transfer. Clock authority
        // remains native and may finish its tail; no placement can race cleanup.
        if(crc==0xEF667DEAu)state_.placeBolt=false;
        // The retail heavy-player typo has no native hide handler. Keep its
        // still-visible tool owned until real DespawnLever or native Stop.
        if(crc==0x3D6870EBu)state_.placeTool=false;
    }
    void completionBefore(std::uintptr_t parent,std::uintptr_t child) noexcept {
        if(parent==state_.owner.parent&&child==state_.owner.child&&pending()&&!physical_)
            (void)restore(state_.phase==Phase::NativeTransitionD80?Reason::NativeComplete:Reason::UnapprovedCompletion);
        // R108: the completed action's lifetime is over; the next action at the
        // same addresses is a new attempt (see startNewAfterRestore).
        if(parent==attemptedActionR95_.parent&&child==attemptedActionR95_.child)attemptedActionLiveR95_=false;
    }
    void stopBefore(std::uintptr_t parent) noexcept {if(parent==state_.owner.parent){
        (void)restore(Reason::NativeStop);
        attemptedActionLiveR95_=false; // native Stop ended this lifetime; pending() still blocks a new owner
    }}
};

// R108: geometry consumes the exact source already accepted by its caller.
using SpatialReader=bool(*)(const Owner&,const Source&,Spatial&) noexcept;
// POD bridges only. Native calls/writes remain in the SINGLE existing executor.
inline core::SnapshotMailbox<Source> sourceMailbox;
inline core::SnapshotMailbox<Status> statusMailbox;
inline core::SnapshotMailbox<Passenger> passengerMailbox;
inline core::SnapshotMailbox<Admission> admissionMailbox;
inline std::atomic<bool> sourceInterested{false},cleanupInterested{false};
inline std::atomic<std::uint64_t> sourcePublication{0},activeNativeLease{0},revokedNativeLease{0},passengerPublication{0};
inline std::atomic<std::uint64_t> sourceCancellationR93{1};
inline std::atomic_flag sourceWriterR93=ATOMIC_FLAG_INIT;
inline std::atomic<bool> integrated{false}; // set only after ALL reviewed native seams are installed
using RegistrationReader=bool(*)() noexcept;
using IdentityReader=bool(*)(const Owner&) noexcept;
using BindingReader=bool(*)(const Owner&,Spatial&) noexcept;
inline std::atomic<RegistrationReader> registrationReader{nullptr};
inline std::atomic<IdentityReader> identityReader{nullptr};
inline std::atomic<BindingReader> bindingReader{nullptr};
inline bool Registered() noexcept {const auto fn=registrationReader.load(std::memory_order_acquire);
    return integrated.load(std::memory_order_acquire)&&fn&&fn()&&
        fn==registrationReader.load(std::memory_order_acquire)&&integrated.load(std::memory_order_acquire);}
inline void WithdrawRegistration() noexcept {integrated.store(false,std::memory_order_release);
    sourceCancellationR93.fetch_add(1,std::memory_order_acq_rel);sourceInterested.store(false,std::memory_order_release);}
inline void RefreshRegistration() noexcept {const auto fn=registrationReader.load(std::memory_order_acquire);
    const bool ready=fn&&fn();integrated.store(ready,std::memory_order_release);if(!ready)sourceInterested.store(false,std::memory_order_release);}
inline std::atomic<SpatialReader> spatialReader{nullptr};
using BoundaryObserver=void(*)(void*,void*) noexcept;
inline std::atomic<BoundaryObserver> completionObserver{nullptr};
using EventObserver=void(*)(void*,const void*) noexcept;
inline std::atomic<EventObserver> eventObserver{nullptr};
inline void BeforeNativeEvent(void* parent,const void* event) noexcept {
    if(auto fn=eventObserver.load(std::memory_order_acquire))fn(parent,event);
}
// Reused by the ALREADY INSTALLED F11 finalizer mid-hook. No second hook at
//289D0B4 and no dependency on the recorder's active/session/budget guards.
inline void BeforeNativeComplete(void* parent,void* child) noexcept {
    if(auto fn=completionObserver.load(std::memory_order_acquire))fn(parent,child);
}
// A positive refresh must not withdraw the preceding coherent pose while the
// mailbox copy is in flight. Native controller callbacks run concurrently: a
// temporary false here used to terminate a valid reload as INPUT_LOST. Keep the
// original source tick and all native owner checks; no freshness is extended.
// Explicit loss has its own epoch, so an in-flight positive publication cannot
// resurrect input after a withdrawal. Serialize only the bounded POD publish.
inline void PublishSource(const Source& s) noexcept {
    if(!s.allowed||!Registered()){
        sourceCancellationR93.fetch_add(1,std::memory_order_acq_rel);
        sourceInterested.store(false,std::memory_order_release);return;
    }
    if(sourceWriterR93.test_and_set(std::memory_order_acquire))return;
    struct UnlockR93 {~UnlockR93(){sourceWriterR93.clear(std::memory_order_release);}} unlock;
    const auto cancellation=sourceCancellationR93.load(std::memory_order_acquire);
    const auto serial=sourcePublication.fetch_add(1,std::memory_order_acq_rel)+1;
    Source packet=s;packet.publication=serial;packet.cancellationR93=cancellation;
    if(sourceMailbox.publish(packet)&&cancellation==sourceCancellationR93.load(std::memory_order_acquire)&&Registered()){
        Status state{};
        if(s.completedLease&&s.completedLease==activeNativeLease.load(std::memory_order_acquire)&&
           statusMailbox.read(state)&&state.clockOwned&&state.owner.serial==s.completedLease&&sameSource(s,state.owner.source))
            completedNativeLease.store(s.completedLease,std::memory_order_release);
        sourceInterested.store(true,std::memory_order_release);
    }
}
inline void RevokeNativeParent(void* parent) noexcept {
    Status state{};if(statusMailbox.read(state)&&state.clockOwned&&state.owner.parent==reinterpret_cast<std::uintptr_t>(parent))
        revokedNativeLease.store(state.owner.serial,std::memory_order_release);
}
inline bool ReadSource(Source& s) noexcept {
    const auto cancellation=sourceCancellationR93.load(std::memory_order_acquire);
    return Registered()&&sourceInterested.load(std::memory_order_acquire)&&sourceMailbox.read(s)&&s.allowed&&
        s.cancellationR93==cancellation&&cancellation==sourceCancellationR93.load(std::memory_order_acquire)&&
        sourceInterested.load(std::memory_order_acquire)&&Registered();
}
inline bool ReadAdmission(Admission& p,std::uint64_t now) noexcept {
    Status state{};const auto serial=activeNativeLease.load(std::memory_order_acquire);
    return Registered()&&serial&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        completedNativeLease.load(std::memory_order_acquire)!=serial&&statusMailbox.read(state)&&state.clockOwned&&!state.sourceEstablished&&state.phase!=Phase::Retiring&&
        admissionMailbox.read(p)&&p.valid&&p.publication&&p.publication==passengerPublication.load(std::memory_order_acquire)&&
        sameOwner(p.owner,state.owner)&&p.owner.serial==serial&&p.started==state.started&&p.deadline==state.firstPoseDeadline&&
        now>=p.started&&now<=p.deadline&&p.tick&&now>=p.tick&&now-p.tick<=80&&
        p.root&&p.childStatus==2&&p.clock.qualified&&p.clip.qualified&&p.binding.qualified&&
        serial==activeNativeLease.load(std::memory_order_acquire)&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        completedNativeLease.load(std::memory_order_acquire)!=serial&&p.publication==passengerPublication.load(std::memory_order_acquire)&&Registered();
}
inline bool ReadPassenger(Passenger& p,std::uint64_t now) noexcept {
    Status state{};Source current{};
    const auto serial=activeNativeLease.load(std::memory_order_acquire);
    return serial&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&ReadSource(current)&&statusMailbox.read(state)&&state.clockOwned&&state.sourceEstablished&&
        completedNativeLease.load(std::memory_order_acquire)==serial&&state.phase!=Phase::Retiring&&state.reason!=Reason::NativeAck&&(state.placeTool||state.placeBolt||state.phase==Phase::NativeTransitionD80)&&state.owner.serial==serial&&
        passengerMailbox.read(p)&&p.valid&&p.publication&&p.publication==passengerPublication.load(std::memory_order_acquire)&&p.owner.serial==activeNativeLease.load(std::memory_order_acquire)&&
        sameOwner(p.owner,state.owner)&&sameSource(current,p.frame)&&current.publication==p.frame.publication&&
        current.sample==p.frame.sample&&current.tick==p.frame.tick&&fresh(p.frame,now)&&
        p.workingHand==1-p.frame.primary&&p.childStatus==2&&p.root&&p.clock.qualified&&p.clip.qualified&&
        p.binding.qualified&&p.binding.serial==p.owner.serial&&p.binding.sample==p.frame.sample&&p.binding.tick==p.frame.tick&&
        serial==activeNativeLease.load(std::memory_order_acquire)&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        p.publication==passengerPublication.load(std::memory_order_acquire);
}
// D87: a completed wrist from this very native clock may be published after
// its post-controller passenger. Treat that as a descendant of the same
// lease, not as loss of reload ownership. This is NOT permission to write the
// passenger's older wrists: pose producers use current tracked goals; prop
// writers still require their target's exact committed body packet. Native
// clock/owner/bindings are rechecked by ReloadPassengerCurrentXR34.
// ReadPassenger remains the exact-source primitive for other consumers.
inline bool ReadPoseLeaseD87(Passenger& p,std::uint64_t now) noexcept {
    if(ReadPassenger(p,now))return true;
    Status state{};Source current{};
    const auto serial=activeNativeLease.load(std::memory_order_acquire);
    return serial&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        ReadSource(current)&&fresh(current,now)&&current.completedLease==serial&&
        statusMailbox.read(state)&&state.clockOwned&&state.sourceEstablished&&state.owner.serial==serial&&
        handDrivenD81(state.owner.source)&&state.phase!=Phase::Retiring&&state.reason!=Reason::NativeAck&&
        (state.placeTool||state.placeBolt||state.phase==Phase::NativeTransitionD80)&&
        completedNativeLease.load(std::memory_order_acquire)==serial&&
        passengerMailbox.read(p)&&p.valid&&!p.bootstrap&&p.publication&&
        p.publication==passengerPublication.load(std::memory_order_acquire)&&sameOwner(p.owner,state.owner)&&
        sameSource(current,p.frame)&&sameSource(p.frame,state.owner.source)&&fresh(p.frame,now)&&
        current.sample>=p.frame.sample&&current.tick>=p.frame.tick&&
        current.nativePoseClockD78==p.clock.entry&&current.nativePoseNormalizedD78==p.clock.normalized&&
        current.nativePoseActiveTimeD78==p.clock.activeTime&&
        p.workingHand==1-p.frame.primary&&p.childStatus==2&&p.root&&p.clock.qualified&&p.clip.qualified&&
        p.binding.qualified&&p.binding.serial==serial&&p.binding.sample==p.frame.sample&&p.binding.tick==p.frame.tick&&
        serial==activeNativeLease.load(std::memory_order_acquire)&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        p.publication==passengerPublication.load(std::memory_order_acquire);
}
// D86: the weapon pose job observes a native clock; it is not a Henry hand
// writer. A newer completed Henry source can be published between the native
// controller update and that weapon job. ReadPassenger must reject its old
// hand frame, but that does not invalidate an unchanged native clock. Keep the
// exact native publication/owner/clip and both source ages; live clock fields
// are rechecked at the untouched weapon skinning boundary before/after reads.
// This function grants read-only mechanism observation, never wrist/prop writes
// or native events. Pose consumers use a separately qualified completed source
// at the same clock; prop destinations still require the exact committed frame.
inline bool ReadMechanismWitnessD86(Passenger& p,std::uint64_t now) noexcept {
    Status state{};Source current{};
    const auto serial=activeNativeLease.load(std::memory_order_acquire);
    return serial&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&ReadSource(current)&&fresh(current,now)&&
        statusMailbox.read(state)&&state.clockOwned&&state.sourceEstablished&&state.placeTool&&
        handDrivenD81(state.owner.source)&&state.owner.source.intentKnownD78&&
        completedNativeLease.load(std::memory_order_acquire)==serial&&state.phase!=Phase::Retiring&&state.reason!=Reason::NativeAck&&
        passengerMailbox.read(p)&&p.valid&&!p.bootstrap&&p.publication&&
        p.publication==passengerPublication.load(std::memory_order_acquire)&&sameOwner(p.owner,state.owner)&&p.owner.serial==serial&&
        sameSource(current,p.frame)&&sameSource(p.frame,state.owner.source)&&fresh(p.frame,now)&&
        current.sample>=p.frame.sample&&current.tick>=p.frame.tick&&
        p.workingHand==1-p.frame.primary&&p.childStatus==2&&p.root&&p.clock.qualified&&p.clip.qualified&&p.clip.cocking&&
        serial==activeNativeLease.load(std::memory_order_acquire)&&revokedNativeLease.load(std::memory_order_acquire)!=serial&&
        p.publication==passengerPublication.load(std::memory_order_acquire);
}
inline void SetSpatialReader(SpatialReader fn) noexcept {spatialReader.store(fn,std::memory_order_release);}
inline bool Interested() noexcept {return sourceInterested.load(std::memory_order_acquire)||cleanupInterested.load(std::memory_order_acquire);}
inline bool ReadStatus(Status& s) noexcept {return statusMailbox.read(s)&&
    (!s.clockOwned||(s.owner.serial&&activeNativeLease.load(std::memory_order_acquire)==s.owner.serial&&revokedNativeLease.load(std::memory_order_acquire)!=s.owner.serial));}
}
