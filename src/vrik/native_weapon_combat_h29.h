#pragma once
#include "physical_weapon.h"
#include "native_weapon_actor_join_h28.h"
#include "solver_rpg_provenance_h29.h"
namespace kcd2vr::native_weapon_combat_h29 {
namespace p=physical;namespace m=pose_core;namespace sc=solver_command_h28;
struct Contact {p::ImpactRecord178r30 impact{};contact59c7g::Metadata metadata{};};
// Called on the native frame. Input is a copied occurrence, never a latest ray.
// No contact may acquire a gesture from an attack that started AFTER it.
inline bool bind(const native_weapon_actor_join_h28::Contact& joined,
 const sc::Stamp& proof,const p::Output::SolverGestureH29& gesture,const p::Profile& weapon,
 std::uint64_t now,Contact& out) noexcept {
 out={};const auto& e=joined.occurrence;const auto& w=joined.witness;
 if(!e.serial||!gesture.gesture||!gesture.onsetSample||!gesture.tick||!sc::fresh(e.tick,now)||
    e.tick<gesture.tick||e.tick-gesture.tick>160||e.source.sourceSample<gesture.onsetSample||
    proof.sourceSample!=e.source.sourceSample||proof.sourceTick!=e.source.sourceTick||
    proof.observedTick!=e.source.observedTick||proof.owner!=e.source.owner||
    proof.owner.key!=weapon.key||proof.owner.generation!=weapon.generation||
    !w.ownerRead||!w.entityId||w.collider!=e.otherBody||w.owner!=e.otherForeign||w.part!=e.otherPart||
    (w.body!=npc_contact172::Body::Living&&w.body!=npc_contact172::Body::Articulated))return false;
 const m::Vec3 self{e.selfVelocity.x,e.selfVelocity.y,e.selfVelocity.z},other{e.otherVelocity.x,e.otherVelocity.y,e.otherVelocity.z};
 const auto direction=m::unit(gesture.worldDirection);
 const auto motion=m::unit(self);const double speed=m::length(self-other);
 // Retreat, stationary pressure and an NPC walking into a still blade cannot
 // borrow a committed swing. Native RPG still decides defense and damage.
 if(!direction||!motion||m::dot(*direction,*motion)<=.25||!std::isfinite(speed)||speed<.8||speed>40)return false;
 auto& r=out.impact;r.valid=true;r.collider=w.collider;r.gesture59c3=gesture.gesture;r.tick=e.tick;
 r.key=weapon.key;r.generation=weapon.generation;r.epoch=proof.owner.epoch;r.request=proof.owner.request;
 r.sample=e.source.sourceSample;r.speed=speed;r.entityId=w.entityId;r.body=int(w.body);r.part=w.part;
 r.kind=gesture.kind;r.point={e.point.x,e.point.y,e.point.z};r.normal={e.normal.x,e.normal.y,e.normal.z};r.originZone47=gesture.origin;
 // Event idmat is NOT ray surfaceIdx/bodyMaterial. Unknown domains stay -1.
 out.metadata.collider=r.collider;out.metadata.weaponClass=weapon.classId;out.metadata.originZone47=gesture.origin;
 out.metadata.sourceSolverH28=proof;out.metadata.occurrenceSerialH28=e.serial;
 return true;
}
}
