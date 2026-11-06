#pragma once
#include "native_physics_phase_t4.h"
#include "core/collab_test_h21.h"
#include "core/mutual_admission_t25.h"
#include "vrik/static_grab_r89.h"
#include "core/snapshot_mailbox.h"
#include "vrik/passive_contact169r1.h"
#include "vrik/npc_contact172.h"
#include <algorithm>
// New F9 rigid-prop work. Pose publication copies identity only. All native
// body access and the one bounded velocity addition run in the audited phase.
namespace kcd2vr::physics::committed_push_h21 {
namespace np=native_physics_phase_t4;
struct World {std::uintptr_t world=0;std::uint64_t generation=0,tick=0;};
struct Target {npc_contact172::Witness contact{};std::uintptr_t world=0;int physicsId=-1;};
struct Evidence {Target target{};std::uint64_t generation=0,profileEpoch=0,sequence=0,tick=0;int side=-1;np::V3 velocity{};};
inline core::SnapshotMailbox<World> worldStamp;
inline core::SnapshotMailbox<Evidence> requests[2];
inline passive169r1::State motion[2]; // pose-owner only
inline std::uint64_t consumed[2]{},consumedEpoch[2]{}; // phase-owner only
inline bool CurrentWorld(World& out,std::uint64_t now) noexcept {
    return worldStamp.read(out)&&out.world&&out.generation&&out.tick&&
        (now>=out.tick?now-out.tick:out.tick-now)<=80;
}
inline void PublishCommitted(const passive169r1::Sample& sample,const Target& target) noexcept {
    if(sample.side<0||sample.side>1||sample.source!=passive169r1::Source::HandWorld)return;
    const auto d=passive169r1::update(motion[sample.side],sample);
    World world{};const auto& w=target.contact;
    if(!collab_h21::enabled.load(std::memory_order_acquire)||!sample.eligible||!sample.worldCoherent||
       sample.status!=passive169r1::Status::Contact||!d.motion.valid||d.motion.incoming<.5||
       static_grab_r89::ownsEntityR94(w.entityId)||
       mutual_admission_t25::pickupClaimsD72[sample.side].load(std::memory_order_acquire)||
       w.body!=npc_contact172::Body::Rigid||!w.ownerRead||!w.owner||!w.entityId||!w.collider||
       target.physicsId<0||!CurrentWorld(world,sample.tick)||target.world!=world.world)return;
    const auto n=pose_core::unit(sample.normal);if(!n)return;
    const double speed=std::min(2.0,d.motion.incoming); // velocity addition, not a fake impulse/damage
    Evidence e{};e.target=target;e.generation=world.generation;e.tick=sample.tick;e.sequence=sample.sequence;e.side=sample.side;
    e.profileEpoch=collab_h21::epoch.load(std::memory_order_acquire);
    e.velocity={float(-n->x*speed),float(-n->y*speed),float(-n->z*speed)};
    (void)requests[sample.side].publish(e);
}
inline void Reset(np::Context&) noexcept {(void)worldStamp.publish({});}
inline void Step(np::Context& context,std::uint64_t now) noexcept {
    if(!context.Current()||context.Kind()!=np::Phase::Step)return;
    (void)worldStamp.publish({reinterpret_cast<std::uintptr_t>(context.World()),context.Generation(),now});
    for(int side=0;side<2;++side){Evidence e{};
        if(!requests[side].read(e)||!collab_h21::current(e.profileEpoch)||e.side!=side||!e.sequence||
           e.generation!=context.Generation()||e.target.world!=reinterpret_cast<std::uintptr_t>(context.World())||
           !e.tick||now<e.tick||now-e.tick>80)continue;
        if(consumedEpoch[side]==e.profileEpoch&&e.sequence<=consumed[side])continue;
        consumedEpoch[side]=e.profileEpoch;consumed[side]=e.sequence; // entered/rejected once; never replay a cached velocity
        const auto& w=e.target.contact;
        if(static_grab_r89::ownsEntityR94(w.entityId)||
           mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)||
           w.body!=npc_contact172::Body::Rigid||!w.ownerRead||!w.owner||!w.entityId||!w.collider||e.target.physicsId<0)continue;
        np::Lease body;
        if(!np::AcquireRigid(context,e.target.physicsId,reinterpret_cast<void*>(w.collider),body))continue;
        np::RigidSnapshot s{};
        if(np::RigidEntityIdentity(context,body,reinterpret_cast<void*>(w.owner),w.entityId)&&np::ReadRigid(context,body,s)){
            const double target=std::sqrt(double(e.velocity.x)*e.velocity.x+double(e.velocity.y)*e.velocity.y+double(e.velocity.z)*e.velocity.z);
            if(!(target>0&&target<=2.001)){(void)np::Release(context,body);continue;}
            const auto direction=np::V3{float(e.velocity.x/target),float(e.velocity.y/target),float(e.velocity.z/target)};
            const double along=s.linearVelocity.x*direction.x+s.linearVelocity.y*direction.y+s.linearVelocity.z*direction.z;
            const double addition=std::clamp(target-along,0.0,2.0);
            if(addition<=.001){(void)np::Release(context,body);continue;}
            // Follow fresh hand motion along the contact normal. Adding the
            // full hand speed every sample pumps energy into a resting touch.
            const auto v=np::V3{float(s.linearVelocity.x+direction.x*addition),float(s.linearVelocity.y+direction.y*addition),float(s.linearVelocity.z+direction.z*addition)};
            const double speed=std::sqrt(double(v.x)*v.x+double(v.y)*v.y+double(v.z)*v.z);
            if(std::isfinite(speed)&&speed<=12.0)(void)np::SetVelocity(context,body,v,s.angularVelocity);
        }
        (void)np::Release(context,body);
    }
}
}
