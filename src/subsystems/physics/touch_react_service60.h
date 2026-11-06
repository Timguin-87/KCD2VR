#pragma once
#include "touch_react60.h"
#include "vrik/passive_contact169r1.h" // DS-F97: the live passive hand contact (from src root)
#include "vrik/npc_contact172.h" // DS-F105: the contact witness (the real target identity)
#include "core/snapshot_mailbox.h"
#include "core/haptics.h"
#include "core/collab_test_h21.h"
#include "vrik/lab58.h"
#include "vrik/lab_living_touch_ds.h"
#include "vrik/clinch_intent_d40.h"
#include <cstdint>
#include <optional>
// DS-F97: the TOUCH-REACTION SERVICE (Route A wiring). The game-thread tick
// consumes the live passive HAND contact (the dog-petting seam's Sample with
// source HandWorld + status Contact), classifies the soft part via the caller's
// witness, and fires the haptic + the caller's flesh-effect seam. NO physics is
// written to the living actor.
// DS-F101 (Codex touch-identity review): Sample.actor is HENRY'S CHARACTER
// pointer (reinterpret_cast of s.character), NOT the contacted NPC - it must
// never be reinterpreted as an EntityId. The contacted entity comes from the
// ACTUAL hit producer (the npc_contact172 witness's entityId) as the explicit
// targetEntity argument, with its own ownership/lifetime.
namespace kcd2vr::touch_react_service60 {
namespace tr=kcd2vr::touch_react60;namespace p=kcd2vr::passive169r1;
// ContactHand's existing feature rays need NPC bodies when F9 requests touch,
// including an otherwise world-only configuration. Keep that selection local
// to THIS free-hand call: held-weapon rays and the outer prepared context keep
// their original policy. No extra ray or change to native-reference ownership.
template<class Query> class ScopedNpcQueryH9 {
    Query& current;std::optional<Query> previous;
public:
    ScopedNpcQueryH9(Query& q,bool nativeReference) noexcept:current(q){
        if(!q.requested&&!nativeReference&&(kcd2vr::clinch_intent_d40::wantsContact()||kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingHitImpact)||kcd2vr::lab58::active(kcd2vr::lab58::Sub::LivingTouch))){
            previous.emplace(q);q={};q.requested=true;
        }
    }
    ScopedNpcQueryH9(const ScopedNpcQueryH9&)=delete;
    ScopedNpcQueryH9& operator=(const ScopedNpcQueryH9&)=delete;
    ~ScopedNpcQueryH9(){if(previous)current=*previous;}
};
struct Witness { // the caller fills from npc_contact172 (living + part)
    bool living=false;unsigned softMask=0;int part=-1;bool fistActive=false;
};
struct Plan {tr::Plan p{};bool fired=false;};
// The flesh-effect seam (the gore's Blood-style MaterialEffects call); filled
// once by the module init. Never called when null.
inline void(*fleshEffect)(const float pos[3],const float nrm[3],std::uint32_t entity,float strength) noexcept=nullptr;
inline Plan service(const p::Sample& s,std::uint32_t targetEntity,const Witness& w,std::uint64_t now) noexcept {
    Plan out{};
    if(s.source!=p::Source::HandWorld||s.status!=p::Status::Contact)return out;
    if(!targetEntity)return out; // no proven target: nothing fires
    tr::Contact c{};c.entity=targetEntity;c.side=s.side;c.part=w.part;
    c.softMask=w.softMask;c.living=w.living;c.fistActive=w.fistActive;
    c.point[0]=float(s.point.x);c.point[1]=float(s.point.y);c.point[2]=float(s.point.z);
    c.normal[0]=float(s.normal.x);c.normal[1]=float(s.normal.y);c.normal[2]=float(s.normal.z);
    c.tick=s.tick;
    out.p=tr::plan(c,now);
    if(out.p.ok){
        // the visible press response: a soft haptic + the game's own flesh effect
        (void)kcd2vr::haptics101::publish_event(s.side,0xF005F11Du+unsigned(s.side),.30f,45,now);
        if(fleshEffect)fleshEffect(out.p.point,out.p.normal,out.p.entity,.35f);
        out.fired=true;
    }
    return out;
}
// DS-F105: the PRODUCTION wiring - the pose-side passive evidence publishes
// the touch PLAN (the real witness identity); the game-thread drain fires it.
struct Evidence {
    p::Sample sample{}; // the hand contact (point/normal/side/tick)
    std::uint32_t entity=0;int part=-1;bool living=false,fist=false;
    std::uintptr_t collider=0,owner=0;bool ownerRead=false;
    // The F9 native touch reader uses the same committed contact. The body
    // class and original F9 epoch are evidence, not another interaction gate.
    kcd2vr::npc_contact172::Body body=kcd2vr::npc_contact172::Body::Unknown;
    std::uint64_t physicsEpoch=0,publicationR24=0; // exact committed body packet; contact creation only
    float grip=0; // same committed pose as this contact, not a later XR poll
};
inline core::SnapshotMailbox<Evidence> mailbox[2];
inline p::State nativeMotionH9[2]; // committed-pose producer only
inline std::uint64_t nativeMotionEpochH9[2]{};
struct TouchEdgeH21 {std::uint64_t actor=0,epoch=0,request=0,sequence=0,lastPulse=0;std::uintptr_t collider=0;std::uint32_t entity=0;bool touching=false;};
inline TouchEdgeH21 edgesH21[2]; // native rhythm consumer only
// The soft-part classifier (the part->chest/butt table). Filled once when
// Claude's part->soft mapping lands; until then NO soft part is admitted
// (fail-closed: the plan refuses NotSoft, nothing fires, no guessed limb).
inline unsigned(*softMaskFor)(std::uint32_t entity,int part,bool living) noexcept=nullptr;
inline void publishPlan(const p::Sample& s,const kcd2vr::npc_contact172::Witness& w,bool fist=false,float grip=0.f,std::uint64_t publication=0) noexcept {
    if(s.side<0||s.side>1||s.source!=p::Source::HandWorld)return;
    kcd2vr::clinch_intent_d40::publish(s,w,grip,publication,fist);
    Evidence e{};e.sample=s;e.entity=w.entityId;e.part=w.part;e.publicationR24=publication;
    e.living=(w.body==kcd2vr::npc_contact172::Body::Living);
    e.fist=fist;e.grip=grip;e.collider=w.collider;e.owner=w.owner;e.ownerRead=w.ownerRead;
    e.body=w.body;e.physicsEpoch=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    // The passive pose producer supplies endpoints, not an invented velocity.
    // Derive motion from consecutive ORIGINAL committed samples, as the rigid
    // push path does. The native drain may skip frames and must not re-time it.
    if(kcd2vr::collab_h21::npcPhysics()){
        if(nativeMotionEpochH9[s.side]!=e.physicsEpoch){nativeMotionH9[s.side]={};nativeMotionEpochH9[s.side]=e.physicsEpoch;}
        e.sample.motion=p::update(nativeMotionH9[s.side],s).motion;e.sample.measuredMotion=true;
    }
    (void)mailbox[s.side].publish(e);
}
inline void drain(std::uint64_t now) noexcept {
    // Default touch means the user's haptic contact response. It requires no
    // guessed anatomy, native flinch, flesh/blood FX or F9 physical mode.
    for(int side=0;side<2;++side){Evidence e{};auto& edge=edgesH21[side];
        if(!mailbox[side].read(e))continue;const auto& s=e.sample;
        if(!s.tick||now<s.tick||now-s.tick>80){edge.touching=false;continue;}
        if(s.actor!=edge.actor||s.epoch!=edge.epoch||s.request!=edge.request){
            edge={};edge.actor=s.actor;edge.epoch=s.epoch;edge.request=s.request;
        }
        if(!s.sequence||s.sequence<=edge.sequence)continue;edge.sequence=s.sequence;
        const bool touching=s.actor&&s.epoch&&s.request&&s.side==side&&s.eligible&&s.worldCoherent&&
            s.status==p::Status::Contact&&e.living&&!e.fist&&e.entity&&e.collider&&e.owner&&e.ownerRead&&
            pose_core::finite(s.point)&&bool(pose_core::unit(s.normal));
        const bool begin=touching&&(!edge.touching||edge.entity!=e.entity||edge.collider!=e.collider);
        edge.touching=touching;edge.entity=e.entity;edge.collider=e.collider;
        if(begin&&(!edge.lastPulse||now>=edge.lastPulse&&now-edge.lastPulse>=300)&&
           haptics101::publish_event(side,0xF005F11Du+unsigned(side),.20f,30,now))edge.lastPulse=now;
    }
}
}
