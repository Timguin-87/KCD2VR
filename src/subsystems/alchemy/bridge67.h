#pragma once
#include "near_selection67.h"
#include "gesture_work67.h"
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/minigame_hands45.h"
#include "../../vrik/alchemy_tool18.h"
#include "../../vrik/alchemy_input19c4.h"
#include "../../vrik/workstation_sample46.h"
#include "../../tracking_sample59c6.h"
#include "../../diagnostics/capture168.h"
#include <atomic>
// Existing copied input endpoint; a declaration avoids the large frik header
// and its workstation include cycle. This is not a native state query.
namespace kcd2vr::frik_pose_probe {bool WorkstationFrame46(workstation_sample46::Frame&) noexcept;}
namespace kcd2vr::subsystems::alchemy::bridge67 {
namespace prox67=near67;
struct NativeTable {
    std::uintptr_t instance=0,player=0,playerEntity=0;
    std::uint64_t lifetime=0,tick=0,sequence=0;
    std::array<prox67::Ref,prox67::Capacity> refs{};
    std::uintptr_t action=0;bool actionKnown=false;bool placeMortar69=false;
    bool nativeRetortD56=false; // exact, current C_UseRetortAction; never a selected tool hint
    std::array<unsigned,2> handType68{{21,21}};bool handTypesKnown68=false;
};
struct NativeOffer {std::uintptr_t instance=0,player=0;std::uint64_t lifetime=0;prox67::Offer offer{};};
struct NativeFrame {NativeTable table{};std::array<prox67::Offer,prox67::Capacity> offers{};};
struct Nomination {prox67::Context context{};prox67::Decision decision{};std::uint64_t tick=0;};
struct NativeTool {
    std::uintptr_t instance=0,parent=0,character=0;
    std::uint64_t lifetime=0,tick=0,poseEpoch=0;
    std::uint32_t entity=0;int side=-1,type=-1;bool joined=false;
    alchemy_tool18::Binding binding{};
};
struct Control {
    std::uintptr_t instance=0,parent=0;std::uint64_t lifetime=0,tick=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
    std::uint32_t entity=0;int type=-1,side=-1;
    bool enabled=false,passenger=false,held[2]{},controllerMortar70=false;
    prox67::Ref destinationSourceH21{};bool destinationReleaseH21=false;
    std::uint64_t admissionTickR22=0; // same physical admission across native Attach publication
    bool trackedPresentation71=false; // Native semantics may run while tracked wrists keep an actually attached tool.
    bool gestureTransferD67=false; // exact observed local Return/Place; never manual/retort authority
    bool heldMortarPourD75=false; // current held left-mortar tilt over native pot6; contents only, never release/home
    bool clockAdmissionD47=false; // exact admitted bellows parent may be paused before its hand-type grant
    alchemy_tool18::Binding plateBindingR93{}; // exact native RIGHT binding; never rewritten to the gesture hand
    int plateGestureSideR93=-1; // only a joined left-hand plate pickup may differ from native side
    unsigned carafeD85=0; // D85 craft: carafe_d85::Bits from the input adapter (held/tilt/pot/home), never a native grant
};
// D67: a historical joined mailbox is not a current attachment. Shared by
// input admission recovery and its diagnostics; never a native attach grant.
inline bool currentToolD67(const NativeTool& t,const prox67::Context& c,std::uint64_t now,int side) noexcept {
    return side>=0&&side<2&&t.joined&&t.side==side&&t.entity&&t.type>=0&&t.type<int(prox67::Capacity)&&t.character&&
        t.instance==c.instance&&t.lifetime==c.lifetime&&t.poseEpoch==c.poseEpoch&&prox67::fresh(t.tick,now);
}
// A left gesture may initiate retail's right-hand plate pickup. Keep those
// identities separate: no forged native hand membership and no other-tool swap.
inline bool plateBindingCurrentR93(const alchemy_tool18::Binding& b,const NativeTable& t,
    const NativeTool& held,const NativeTool& other,std::uint64_t admission,std::uint64_t pose,std::uint64_t now) noexcept {
    return b&&b.type==0&&b.side==0&&b.player&&b.character&&b.attachment&&b.object&&b.lifetime&&!b.action&&!b.actionClass&&
        admission&&held.tick>admission&&held.joined&&!held.parent&&held.side==0&&held.type==0&&held.entity==b.entity&&
        held.instance==b.instance&&held.character==b.character&&held.lifetime==b.lifetime&&held.poseEpoch==pose&&
        prox67::fresh(held.tick,now)&&alchemy_tool18::same(held.binding,b)&&
        t.instance==b.instance&&t.player==b.player&&t.playerEntity&&t.lifetime==b.lifetime&&prox67::fresh(t.tick,now)&&
        t.handTypesKnown68&&t.handType68[0]==0&&t.handType68[1]==21&&prox67::same(t.refs[0],{0,b.entity})&&
        !(other.joined&&other.entity&&other.side==1&&other.instance==b.instance&&other.lifetime==b.lifetime&&
          other.poseEpoch==pose&&prox67::fresh(other.tick,now));
}
inline bool plateControlCurrentR93(const Control& c,const NativeTable& t,const NativeTool& held,
    const NativeTool& other,std::uint64_t now,std::uint64_t pose) noexcept {
    return c.enabled&&c.plateGestureSideR93==1&&c.side==0&&c.type==0&&c.entity==c.plateBindingR93.entity&&
        c.instance==c.plateBindingR93.instance&&c.lifetime==c.plateBindingR93.lifetime&&c.poseEpoch==pose&&
        prox67::fresh(c.tick,now)&&plateBindingCurrentR93(c.plateBindingR93,t,held,other,c.admissionTickR22,pose,now);
}
// D75: an ordinary held object is deliberately absent from hover candidates.
// Its open-Grip source return therefore needs a separate nomination, just as
// a logical herb does. Share the exact native ownership proof between producer
// and consumer; a copied joined bit or another current tool is insufficient.
// Resource, mortar/pestle, bellows and the stationary retort keep their own paths.
inline bool ordinarySourceReturnD75(const NativeTool& t,const NativeTable& table,
    const prox67::Context& c,prox67::Ref source,int side,std::uint64_t now) noexcept {
    if((source.slot!=0&&source.slot!=1&&source.slot!=5&&source.slot!=19)||
       !prox67::valid(source)||!currentToolD67(t,c,now,side)||t.parent||
       t.entity!=source.entity||t.type!=source.slot||
       table.instance!=c.instance||table.player!=c.player||table.lifetime!=c.lifetime||
       !prox67::fresh(table.tick,now)||!table.handTypesKnown68||
       table.handType68[unsigned(side)]!=unsigned(source.slot)||
       !prox67::same(table.refs[unsigned(source.slot)],source))return false;
    const auto& binding=t.binding;
    return binding&&binding.instance==c.instance&&binding.player==c.player&&
        binding.lifetime==c.lifetime&&binding.character==t.character&&
        binding.entity==source.entity&&binding.type==source.slot&&binding.side==side&&
        binding.attachment&&binding.object&&!binding.action&&!binding.actionClass;
}
inline std::atomic<bool> enabled{false};
inline core::SnapshotMailbox<NativeTable> table;
inline core::SnapshotMailbox<NativeOffer> offers[prox67::Capacity];
inline core::SnapshotMailbox<NativeFrame> completed;
inline core::SnapshotMailbox<Nomination> nomination;
inline core::SnapshotMailbox<NativeTool> tools[2];
inline core::SnapshotMailbox<Control> control;
// Published only after a committed exact native pestle was actually placed.
// Paused presentation is not a physics drop or a native recipe transition.
struct PestlePresentationD47 {
    alchemy_tool18::Binding binding{};std::uint64_t tick=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
    bool parked=false;
};
inline core::SnapshotMailbox<PestlePresentationD47> pestlePresentationD47;
inline bool pestleParkedD47(const PestlePresentationD47& p,const NativeTool& t,std::uint64_t now,std::uint64_t source,std::uint64_t mode) noexcept {
    return p.parked&&t.joined&&t.type==3&&t.side==0&&
        p.poseEpoch==t.poseEpoch&&p.sourceEpoch==source&&p.modeEpoch==mode&&prox67::fresh(p.tick,now)&&
        alchemy_tool18::sameMortarGrip69(p.binding,t.binding);
}
inline bool current(const NativeTable& t,std::uint64_t now) noexcept {
    return enabled.load(std::memory_order_acquire)&&t.instance&&t.player&&t.playerEntity&&t.lifetime&&t.sequence&&prox67::fresh(t.tick,now);
}
inline bool sameTable68(const NativeTable& a,const NativeTable& b) noexcept {
    if(!a.instance||a.instance!=b.instance||!a.player||a.player!=b.player||
       !a.playerEntity||a.playerEntity!=b.playerEntity||!a.lifetime||a.lifetime!=b.lifetime)return false;
    for(unsigned i=0;i<prox67::Capacity;++i)if(!prox67::same(a.refs[i],b.refs[i]))return false;
    return true;
}
inline bool selectionTable68(NativeTable& out,std::uint64_t now) noexcept {
    NativeTable t{};if(!table.read(t)||!current(t,now)){out={};return false;}out=t;return true;
}
// Called only by the exact retail candidate GetLocalBounds return observer,
// before head-facing/projection exclusions. No entity lookup/native call here.
inline void observeBounds(const NativeTable& observed,prox67::Ref ref,const geometry68::Bounds& bounds,std::uint64_t now) noexcept {
    NativeTable t{};
    if(!prox67::valid(ref)||!geometry68::valid(bounds)||!current(observed,now)||!selectionTable68(t,now)||!sameTable68(t,observed)||
       !prox67::same(t.refs[unsigned(ref.slot)],ref))return;
    prox67::Offer offer{};offer.ref=ref;offer.tick=now;offer.valid=offer.boundsValid=true;offer.bounds=bounds;
    // Compatibility metadata only. Live distances always use the full bounds;
    // this transformed box center is NOT a grip point or a vessel mouth.
    const auto center=geometry68::transform(bounds,{(double(bounds.local[0])+bounds.local[3])*.5,
        (double(bounds.local[1])+bounds.local[4])*.5,(double(bounds.local[2])+bounds.local[5])*.5});
    offer.point={center.x,center.y,center.z};
    (void)offers[unsigned(ref.slot)].publish({t.instance,t.player,t.lifetime,offer});
}
inline void selectionBounds68(const NativeTable& observed,prox67::Ref ref,const geometry68::Bounds& bounds,std::uint64_t now) noexcept {
    // Slot18's native candidate bounds enclose the whole two-metre mechanism.
    // Keep those bounds in the original collector, but never replace the
    // qualified current chain-handle near offer with that giant volume.
    if(ref.slot!=18)observeBounds(observed,ref,bounds,now);
}
// Only the exact current-pose part reader in PublishMinigameSourcesH42 may
// publish slot18. A failed read revokes its old offer immediately; native A
// and native candidate/usability/recipe semantics remain unchanged.
inline void selectionBellowsBoundsR16(const NativeTable& observed,prox67::Ref ref,
    const geometry68::Bounds* bounds,std::uint64_t now) noexcept {
    NativeTable t{};
    if(ref.slot!=18||!prox67::valid(ref)||!current(observed,now)||!selectionTable68(t,now)||
       !sameTable68(t,observed)||!prox67::same(t.refs[18],ref))return;
    if(bounds&&geometry68::valid(*bounds))observeBounds(t,ref,*bounds,now);
    else (void)offers[18].publish({t.instance,t.player,t.lifetime,{}});
}
// Called ONLY with an actual native AlchemyItem candidate offered for Henry.
inline void observe(std::uintptr_t player,std::uint32_t entity,prox67::Point point,std::uint64_t now) noexcept {
    NativeTable t{};if(!prox67::finite(point)||!table.read(t)||!current(t,now)||t.player!=player)return;
    for(unsigned i=0;i<prox67::Capacity;++i)if(i!=18&&t.refs[i].entity==entity&&prox67::valid(t.refs[i])){
        NativeOffer prior{};
        if(offers[i].read(prior)&&prior.instance==t.instance&&prior.player==t.player&&prior.lifetime==t.lifetime&&
           prox67::same(prior.offer.ref,t.refs[i])&&prior.offer.boundsValid&&prox67::fresh(prior.offer.tick,now))return;
        (void)offers[i].publish({t.instance,t.player,t.lifetime,{t.refs[i],point,now,true}});return;
    }
}
inline void finish(const NativeTable& t) noexcept {
    NativeFrame f{};f.table=t;
    for(unsigned i=0;i<prox67::Capacity;++i){NativeOffer o{};
        if(offers[i].read(o)&&o.instance==t.instance&&o.player==t.player&&o.lifetime==t.lifetime&&
           prox67::same(o.offer.ref,t.refs[i])&&o.offer.tick<=t.tick&&prox67::fresh(o.offer.tick,t.tick))f.offers[i]=o.offer;
    }
    (void)table.publish(t);(void)completed.publish(f);
}
inline bool controllerOwns(int side,std::uint32_t entity,std::uint64_t now,
    std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t poseEpoch) noexcept {
    Control c{};if(side<0||side>1||!enabled.load(std::memory_order_acquire)||!control.read(c)||!c.enabled||c.passenger||
       !instance||!lifetime||!poseEpoch||c.instance!=instance||c.lifetime!=lifetime||c.poseEpoch!=poseEpoch||
       !prox67::fresh(c.tick,now)||c.modeEpoch!=minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||!c.held[side])return false;
    NativeTable t{};
    if(!table.read(t)||!current(t,now)||t.instance!=c.instance||t.lifetime!=c.lifetime||
       !prox67::valid(prox67::Ref{c.type,c.entity})||!prox67::same(t.refs[unsigned(c.type)],prox67::Ref{c.type,c.entity}))return false;
    if(c.type==2||c.type==3)return t.refs[side?2:3].entity==entity;
    if(c.plateBindingR93&&side==1&&entity==c.entity){NativeTool held{},other{};
        return tools[0].read(held)&&tools[1].read(other)&&
            plateControlCurrentR93(c,t,held,other,now,poseEpoch);}
    return c.entity==entity&&c.side==side;
}
// Presentation only: an already attached native tool is not a new acquisition.
// The caller re-reads this exact binding from retail immediately before placement.
// No selection, native Use, clock credit, resource effect or inventory permission.
struct PresentationSnapshotR95 {Control control{};NativeTable table{};NativeTool attached{},other{};};
inline bool readPresentationR95(const alchemy_tool18::Binding& b,PresentationSnapshotR95& out) noexcept {
    out={};
    if(!b||b.side<0||b.side>1||alchemy_tool18::nativeFixtureD56(b.type)||
       !b.player||!b.character||!b.attachment||!b.object||!b.lifetime||
       !enabled.load(std::memory_order_acquire)||!control.read(out.control)||
       !table.read(out.table)||!tools[b.side].read(out.attached))return false;
    if(out.control.plateBindingR93&&b.type==0&&b.entity==out.control.plateBindingR93.entity)
        if(!tools[1].read(out.other))return false; // unreadable is not proof of an empty other hand
    return true;
}
inline bool presentationSnapshotOwnsR95(const alchemy_tool18::Binding& b,const PresentationSnapshotR95& snapshot,
    std::uint64_t now,std::uint64_t pose,Control* currentControlD49=nullptr,int* controllerSideR93=nullptr) noexcept {
    if(controllerSideR93)*controllerSideR93=b.side;
    const auto& c=snapshot.control;const auto& t=snapshot.table;const auto& attached=snapshot.attached;
    if(!b||b.side<0||b.side>1||alchemy_tool18::nativeFixtureD56(b.type)||!b.player||!b.character||!b.attachment||!b.object||!b.lifetime||!pose||
       !enabled.load(std::memory_order_acquire)||!c.enabled||!c.trackedPresentation71||
       c.instance!=b.instance||c.lifetime!=b.lifetime||c.poseEpoch!=pose||!prox67::fresh(c.tick,now)||
       c.modeEpoch!=minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       !c.sourceEpoch||c.sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       !current(t,now)||t.instance!=b.instance||t.player!=b.player||t.lifetime!=b.lifetime||
       !prox67::valid(prox67::Ref{b.type,b.entity})||!prox67::same(t.refs[unsigned(b.type)],{b.type,b.entity})||
       !attached.joined||attached.side!=b.side||attached.entity!=b.entity||
       attached.type!=b.type||attached.instance!=b.instance||attached.lifetime!=b.lifetime||attached.character!=b.character||
       attached.poseEpoch!=pose||!prox67::fresh(attached.tick,now)||!alchemy_tool18::same(attached.binding,b))return false;
    if(c.plateBindingR93&&b.type==0&&b.entity==c.plateBindingR93.entity){
        if(!minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy))||
           !alchemy_tool18::same(c.plateBindingR93,b)||!plateControlCurrentR93(c,t,attached,snapshot.other,now,pose))return false;
        if(controllerSideR93)*controllerSideR93=c.plateGestureSideR93;
    }
    if(currentControlD49)*currentControlD49=c;
    return true;
}
// Explicit-clock helper retained for deterministic snapshot tests. Live callers
// must use the read-then-clock adapter below: an entry clock may predate a valid
// publication that arrived while the callback was reading native attachments.
inline bool presentationOwns71(const alchemy_tool18::Binding& b,std::uint64_t now,std::uint64_t pose,
    Control* currentControlD49=nullptr,int* controllerSideR93=nullptr) noexcept {
    PresentationSnapshotR95 snapshot{};
    return readPresentationR95(b,snapshot)&&presentationSnapshotOwnsR95(b,snapshot,now,pose,currentControlD49,controllerSideR93);
}
template<class Clock> inline bool presentationOwnsCurrentR95(const alchemy_tool18::Binding& b,
    std::uint64_t pose,Clock&& clock,Control* currentControlD49=nullptr,int* controllerSideR93=nullptr) noexcept {
    PresentationSnapshotR95 snapshot{};
    if(!readPresentationR95(b,snapshot))return false;
    const std::uint64_t now=clock(); // after ALL snapshots, including the other-hand plate witness
    return presentationSnapshotOwnsR95(b,snapshot,now,pose,currentControlD49,controllerSideR93);
}
// Returns one source-qualified native-table nomination. Source acquisition
// uses the same 10/20 cm per-hand policy as its producer; legacy callers retain
// 10 cm. Only an actually attached mortar/pestle pair in the legacy policy may nominate
// a legal destination along its holding-hand aim. Neither grants native Use.
// W4 diagnostics: WHY the hand nomination did (not) steer the game's selector. Counted on every
// selector call (the hook calls this), so F5 can tell "hook never asked" from "hook asked, refused".
enum class Why68 : unsigned {Accepted=0,TableStale,NominationStale,ContextMismatch,ModeEpoch,NoHandNomination,
    RefNotRegistered,TopologyChanged,PalmNotCurrent,OfferMissing,OutOfReach,AimRefused,HeldNotOwned,Count};
inline const char* name(Why68 w) noexcept {
    constexpr const char* n[]{"ACCEPTED","TABLE_STALE","NOMINATION_STALE","CONTEXT_MISMATCH","MODE_EPOCH","NO_HAND_NOMINATION",
        "REF_NOT_REGISTERED","TOPOLOGY_CHANGED","PALM_NOT_CURRENT","OFFER_MISSING","OUT_OF_REACH","AIM_REFUSED","HELD_NOT_OWNED"};
    const auto i=unsigned(w);return i<unsigned(Why68::Count)?n[i]:"UNKNOWN";
}
inline std::atomic<std::uint64_t> why68[unsigned(Why68::Count)]{};
inline std::atomic<unsigned> lastWhy68{unsigned(Why68::Count)};
inline int refuse68(Why68 w,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return -1;
    why68[unsigned(w)].fetch_add(1,std::memory_order_relaxed);lastWhy68.store(unsigned(w),std::memory_order_relaxed);
    return -1;
}
inline int accept68(int side,std::uint64_t now) noexcept {(void)refuse68(Why68::Accepted,now);return side;}
// R8b: what the native selector did with the hand's nominee (selector hooks in interaction_native_selection68.inl).
// candidate = the native offered it as an object candidate this frame, forced = its head-facing test was replaced
// by the hand, filtered = another candidate was dropped because the hand owns the selection.
inline std::atomic<std::uint32_t> steerNominee68{0};
inline std::atomic<std::uint64_t> steerCandidateTick68{0},steerForcedTick68{0},steerFilteredTick68{0};
inline std::atomic<std::uint64_t> steerCandidates68{0},steerForced68{0},steerFiltered68{0};
inline void steerNote68(std::uint32_t nominee,std::atomic<std::uint64_t>& tick,std::atomic<std::uint64_t>& count,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    steerNominee68.store(nominee,std::memory_order_relaxed);tick.store(now,std::memory_order_relaxed);count.fetch_add(1,std::memory_order_relaxed);
}
// Candidate/override observations, not proof of why input admission failed.
// Native facing may already be true, and the selection/hint adapter can refuse
// a valid native entity later. Keep diagnostics from blaming the wrong gate.
enum class Steer68 : unsigned {Unknown=0,NeverCandidate,CandidateNotFacing,ForcedButRefused};
inline Steer68 steerWhy68(std::uint32_t nominee,std::uint32_t lastNominee,std::uint64_t candidateTick,std::uint64_t forcedTick,std::uint64_t now) noexcept {
    if(!nominee)return Steer68::Unknown;
    const auto recent=[now](std::uint64_t t) noexcept {return t&&now>=t&&now-t<=250;};
    if(lastNominee!=nominee||!recent(candidateTick))return Steer68::NeverCandidate;
    return recent(forcedTick)?Steer68::ForcedButRefused:Steer68::CandidateNotFacing;
}
inline const char* name(Steer68 s) noexcept {
    switch(s){case Steer68::NeverCandidate:return "game never offered it as a target";
    case Steer68::CandidateNotFacing:return "candidate observed; facing override not observed";case Steer68::ForcedButRefused:return "facing overridden; input admission not observed";
    default:return "unknown";}
}
inline int selectionOwner68(const NativeTable& observed,Nomination& n,std::uint64_t now) noexcept {
    NativeTable t{};workstation_sample46::Frame frame{};
    if(!current(observed,now)||!selectionTable68(t,now)||!sameTable68(t,observed))return refuse68(Why68::TableStale,now);
    if(!nomination.read(n)||!prox67::fresh(n.tick,now))return refuse68(Why68::NominationStale,now);
    if(!prox67::valid(n.context)||n.context.instance!=t.instance||n.context.player!=t.player||n.context.lifetime!=t.lifetime||
       n.context.tableGeneration!=t.lifetime)return refuse68(Why68::ContextMismatch,now);
    if(n.context.generation!=minigame_hands45::modeEpoch67.load(std::memory_order_acquire))return refuse68(Why68::ModeEpoch,now);
    if(n.decision.source==prox67::Source::Gaze)return refuse68(Why68::NoHandNomination,now);
    const bool handsSelection=n.decision.proximityCraft1&&n.decision.handsOnlyD53&&
        minigame_hands45::modeFor(minigame_hands45::Alchemy)==minigame_hands45::SixDofVrik;
    if(n.decision.handsOnlyD53&&!handsSelection)return refuse68(Why68::ModeEpoch,now);
    const bool hasRef=prox67::valid(n.decision.ref);
    if(hasRef&&!prox67::same(t.refs[unsigned(n.decision.ref.slot)],n.decision.ref))return refuse68(Why68::RefNotRegistered,now);
    for(unsigned i=0;i<prox67::Capacity;++i)if(!prox67::same(t.refs[i],n.decision.table[i]))return refuse68(Why68::TopologyChanged,now);
    const int side=n.decision.source==prox67::Source::Left?1:0;const auto& p=n.decision.palm;
    if(!frik_pose_probe::WorkstationFrame46(frame)||!frame.allowed||!workstation_sample46::fresh(frame,now)||
       frame.poseEpoch!=n.context.poseEpoch||frame.sourceEpoch!=n.context.sourceEpoch||
       frame.sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||!frame.hand[side].valid||
       !workstation_sample46::finite(frame.hand[side])||!p.valid||p.side!=side||!prox67::finite(p.point)||
       !prox67::fresh(p.tick,now)||p.tick>n.tick||p.poseEpoch!=frame.poseEpoch||p.sourceEpoch!=frame.sourceEpoch||
       p.generation!=n.context.generation||!std::isfinite(p.grip)||p.grip<0||p.grip>1)return refuse68(Why68::PalmNotCurrent,now);
    const bool heldSelection=n.decision.proximityCraft1&&n.decision.gripSelectionOwnedCraft1&&
        p.grip>=.35&&std::isfinite(frame.hand[side].grip)&&frame.hand[side].grip>=.35f;
    if(n.decision.gripSelectionOwnedCraft1&&!heldSelection)return refuse68(Why68::NoHandNomination,now);
    if(!hasRef)return (heldSelection||handsSelection)?accept68(side,now):refuse68(Why68::NoHandNomination,now);
    NativeOffer o{};
    if(!offers[unsigned(n.decision.ref.slot)].read(o)||o.instance!=t.instance||o.player!=t.player||o.lifetime!=t.lifetime||
       !prox67::same(o.offer.ref,n.decision.ref)||!o.offer.valid||!o.offer.boundsValid||!prox67::fresh(o.offer.tick,now)||
       !geometry68::valid(o.offer.bounds)){
        if(heldSelection||handsSelection){n.decision.ref={};return accept68(side,now);}
        return refuse68(Why68::OfferMissing,now);
    }
    if(n.decision.proximityCraft1){
        // The consumer rechecks CURRENT bounds and hand role, not the producer's
        // cached distance. An idle hand outside the region yields to native;
        // a held hand owns an empty selection instead of a remote target.
        // This is selection permission only: native usability/final selection
        // and actual tool attachments remain independently required.
        if(n.decision.aimedDestinationH21)return refuse68(Why68::AimRefused,now);
        const auto gap=prox67::distanceToOffer(o.offer,p.point);
        alchemy_input19c4::Selection selectedD44{};
        const bool selectedPickupD44=n.decision.sourceAcquisitionD39&&n.decision.selectedPickupD44&&
            side==1&&n.decision.ref.slot>=13&&n.decision.ref.slot<=16&&
            p.grip>=.55&&frame.hand[side].grip>=.55f&&
            alchemy_input19c4::selectionSnapshot(t.instance,t.lifetime,now,selectedD44)&&
            selectedD44.entity==n.decision.ref.entity&&selectedD44.type==unsigned(n.decision.ref.slot)&&
            reach_craft1::contains(n.decision.ref.slot,side,gap);
        // D72: a held vessel/herb can point UP while its contents go DOWN.
        // Recheck native hand membership before replacing the acquisition ray
        // with the5cm-lateral recipient corridor. No copied producer flag alone
        // grants this selection path, and native usability/action stays final.
        bool contentsHeldD72=false;
        if(handsSelection&&n.decision.pourRecipientD72){
            const auto source=n.decision.heldSourceH21;NativeTool held{};
            if(prox67::valid(source)&&prox67::same(t.refs[unsigned(source.slot)],source)&&
               !prox67::same(source,n.decision.ref)){
                if(source.slot>=7&&source.slot<=9)
                    contentsHeldD72=side==0&&t.handTypesKnown68&&t.handType68[0]==unsigned(source.slot);
                else if(source.slot==0&&side==1){
                    Control c{};NativeTool native{},other{};
                    if(control.read(c)&&tools[0].read(native)){(void)tools[1].read(other);
                        contentsHeldD72=c.entity==source.entity&&c.modeEpoch==n.context.generation&&
                            c.sourceEpoch==n.context.sourceEpoch&&plateControlCurrentR93(c,t,native,other,now,frame.poseEpoch);}
                    if(!contentsHeldD72&&tools[side].read(held))contentsHeldD72=currentToolD67(held,n.context,now,side)&&
                        held.entity==source.entity&&held.type==0&&held.binding&&held.binding.player==t.player&&held.binding.side==side;
                }
                else if((source.slot==0||source.slot==2)&&tools[side].read(held))
                    contentsHeldD72=currentToolD67(held,n.context,now,side)&&held.entity==source.entity&&held.type==source.slot&&
                        held.binding&&held.binding.player==t.player&&held.binding.entity==source.entity&&held.binding.side==side;
            }
        }
        // Source return is a release destination, never a new acquisition.
        // D74 covered logical herbs only; D75 also proves the current ordinary
        // attachment. A regrip or stale/foreign binding cannot borrow this path.
        bool sourceHeldD75=false;
        if(handsSelection&&n.decision.selfReturnRecipientD74){
            NativeTool returningD75{};
            sourceHeldD75=(side==0&&n.decision.ref.slot>=7&&n.decision.ref.slot<=9&&
                t.handTypesKnown68&&t.handType68[0]==unsigned(n.decision.ref.slot))||
                (tools[side].read(returningD75)&&ordinarySourceReturnD75(returningD75,t,n.context,n.decision.ref,side,now));
        }
        if(handsSelection&&n.decision.selfReturnRecipientD74&&side==1&&n.decision.ref.slot==0&&!sourceHeldD75){
            Control c{};NativeTool native{},other{};
            if(control.read(c)&&tools[0].read(native)){(void)tools[1].read(other);
                sourceHeldD75=c.entity==n.decision.ref.entity&&c.modeEpoch==n.context.generation&&
                    c.sourceEpoch==n.context.sourceEpoch&&plateControlCurrentR93(c,t,native,other,now,frame.poseEpoch);}
        }
        const bool ownSourceReturnD75=handsSelection&&n.decision.selfReturnRecipientD74&&
            !n.decision.sourceAcquisitionD39&&prox67::same(n.decision.heldSourceH21,n.decision.ref)&&
            sourceHeldD75&&p.grip<.35&&frame.hand[side].grip<.35f;
        if(n.decision.selfReturnRecipientD74&&!ownSourceReturnD75)return refuse68(Why68::AimRefused,now);
        const bool within=handsSelection?(reach_craft1::handsContainsD53(n.decision.ref.slot,side,gap)&&
            (ownSourceReturnD75?true:n.decision.pourRecipientD72?(contentsHeldD72&&prox67::pourRecipientD72(o.offer,p)):
            (!n.decision.palmRayD70||prox67::palmCorridorD70(o.offer,p,n.decision.aimH21,now)))):
            selectedPickupD44||(n.decision.sourceAcquisitionD39?
            // Recheck both the captured source policy and the CURRENT live
            // choice. Tightening the menu cannot authorize an old wider grab.
            reach_craft1::pickupContainsD39(n.decision.ref.slot,side,gap,n.decision.pickupReachD39)&&
            reach_craft1::pickupContainsD39(n.decision.ref.slot,side,gap,reach_craft1::getPickupReachD39()):
            reach_craft1::contains(n.decision.ref.slot,side,gap));
        if(within)return accept68(side,now);
        if(heldSelection||handsSelection){n.decision.ref={};return accept68(side,now);} // current hand owns no item here
        return refuse68(Why68::OutOfReach,now);
    }
    if(n.decision.aimedDestinationH21){
        Control c{};NativeTool tool{},pestle{};const auto& aim=n.decision.aimH21;
        const bool release=n.decision.aimReleaseH21;
        if(side!=1||!prox67::mortarDestinationH21(n.decision.ref.slot)||n.decision.heldSourceH21.slot!=2||
           !prox67::same(t.refs[2],n.decision.heldSourceH21)||!aim.valid||aim.side!=side||aim.tick!=p.tick||
           aim.poseEpoch!=p.poseEpoch||aim.sourceEpoch!=p.sourceEpoch||aim.generation!=p.generation||
           !control.read(c)||!c.enabled||c.passenger||!prox67::fresh(c.tick,now)||c.tick<n.tick||
           c.instance!=t.instance||c.lifetime!=t.lifetime||c.poseEpoch!=frame.poseEpoch||c.sourceEpoch!=frame.sourceEpoch||
           c.modeEpoch!=n.context.generation||(c.type!=2&&c.type!=3)||c.entity!=t.refs[unsigned(c.type)].entity||
           !prox67::same(c.destinationSourceH21,n.decision.heldSourceH21)||c.destinationReleaseH21!=release||
           (release?(p.grip>=.35||frame.hand[side].grip>=.35f):(p.grip<.35||frame.hand[side].grip<.35f))||
           !tools[side].read(tool)||!tool.joined||!tool.character||tool.side!=side||tool.type!=2||
           tool.entity!=n.decision.heldSourceH21.entity||tool.instance!=t.instance||tool.lifetime!=t.lifetime||
           tool.poseEpoch!=frame.poseEpoch||!prox67::fresh(tool.tick,now)||!tool.binding||
           tool.binding.player!=t.player||tool.binding.side!=side||tool.binding.type!=2||tool.binding.entity!=tool.entity||
           !tools[0].read(pestle)||!pestle.joined||pestle.side!=0||pestle.type!=3||pestle.entity!=t.refs[3].entity||
           pestle.instance!=t.instance||pestle.lifetime!=t.lifetime||pestle.poseEpoch!=frame.poseEpoch||
           pestle.character!=tool.character||!prox67::fresh(pestle.tick,now)||!pestle.binding||
           pestle.binding.player!=t.player||pestle.binding.side!=0||pestle.binding.type!=3||pestle.binding.entity!=pestle.entity||
           tool.parent!=pestle.parent||tool.binding.action!=tool.parent||pestle.binding.action!=pestle.parent||
           (t.actionKnown&&tool.parent!=t.action))return refuse68(Why68::AimRefused,now);
        const auto hit=geometry68::ray(o.offer.bounds,{p.point.x,p.point.y,p.point.z},
            {aim.direction.x,aim.direction.y,aim.direction.z});
        return hit.valid?accept68(side,now):refuse68(Why68::AimRefused,now);
    }
    const double gap=prox67::distanceToOffer(o.offer,p.point);
    if(!std::isfinite(gap))return refuse68(Why68::OutOfReach,now);
    if(gap<=prox67::Enter)return accept68(side,now);
    Control c{};NativeTool tool{};
    if(!n.decision.gripping||p.grip<.35||frame.hand[side].grip<.35f||!control.read(c)||
       c.tick<n.tick||c.sourceEpoch!=frame.sourceEpoch||c.poseEpoch!=frame.poseEpoch||
       c.modeEpoch!=n.context.generation||!controllerOwns(side,n.decision.ref.entity,now,t.instance,t.lifetime,frame.poseEpoch)||
       !tools[side].read(tool)||!tool.joined||tool.side!=side||tool.entity!=n.decision.ref.entity||
       tool.instance!=t.instance||tool.lifetime!=t.lifetime||tool.poseEpoch!=frame.poseEpoch||
       !tool.character||!prox67::fresh(tool.tick,now)||!tool.binding||tool.binding.player!=t.player||
       tool.binding.entity!=tool.entity||tool.binding.side!=side||tool.binding.type!=tool.type)return refuse68(Why68::HeldNotOwned,now);
    return accept68(side,now);
}
inline bool selectionNear68(const NativeTable& observed,std::uint32_t& entity,std::uint64_t now) noexcept {
    entity=0;Nomination n{};if(selectionOwner68(observed,n,now)<0)return false;entity=n.decision.ref.entity;return true;
}
// -1 means gaze; 0/1 means the one qualified near palm or held destination. Native
// final selection remains mandatory before the input adapter emits alch_use.
inline int selection(std::uintptr_t player,std::uint32_t candidate,std::uint64_t now,bool& nominated) noexcept {
    nominated=false;NativeTable t{};Nomination n{};
    if(!selectionTable68(t,now)||player!=t.player)return -1;
    const int side=selectionOwner68(t,n,now);if(side<0)return -1;
    nominated=n.decision.ref.entity==candidate;return side;
}
inline bool nativeRetortD56(std::uint64_t now) noexcept {
    NativeTable t{};
    return table.read(t)&&current(t,now)&&t.actionKnown&&t.action&&t.nativeRetortD56;
}
inline bool visualPassenger71(std::uint64_t now) noexcept {
    if(nativeRetortD56(now)&&!minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy)))return true;
    Control c{};return control.read(c)&&c.enabled&&c.passenger&&!c.trackedPresentation71&&prox67::fresh(c.tick,now)&&
        c.modeEpoch==minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
}
inline bool passenger(std::uint64_t now) noexcept {
    Control c{};return control.read(c)&&c.enabled&&c.passenger&&prox67::fresh(c.tick,now)&&
        c.modeEpoch==minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
}
}
