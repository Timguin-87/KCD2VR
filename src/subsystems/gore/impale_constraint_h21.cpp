#include "impale_constraint_h21.h"
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/impale_hold60.h"
#include "../physics/corpse_body_owner_d40.h"
#include <atomic>
#ifndef KCD2VR_IMPALE_CONSTRAINT_H21_TEST
#include <Windows.h>
#endif
namespace kcd2vr::impale_constraint_h21 { namespace {
namespace p=native_physics_phase_t4;namespace w=native_weapon_body_t4;
namespace bodyOwnerD40=corpse_body_owner_d40;
core::SnapshotMailbox<Input> input;
std::atomic<std::uint64_t> cancelled{0},admitted{0},refused{0};
std::atomic<std::uint64_t> admittedPhysicsEpoch{0};
std::atomic<bool> blocked{false},registered{false};
std::atomic<const char*> failure{"NONE"};
std::atomic_flag installing=ATOMIC_FLAG_INIT;
std::atomic_flag servicing=ATOMIC_FLAG_INIT;
struct Record {Input in{};p::ArticulatedLease corpse{};bodyOwnerD40::Lease motor{};std::uint32_t id=0;
 bool entered=false,removeEntered=false,pinned=false,quarantined=false;} record;
bool Fresh(const Input& in) noexcept {const auto now=std::uint64_t(GetTickCount64());
 return in.serial&&in.nativeDeadCurrent&&in.bodyGeneration&&in.tick&&now>=in.tick&&now-in.tick<=80;}
bodyOwnerD40::Key MotorKeyD40(const Input& in) noexcept {
 return {bodyOwnerD40::Slot::ImpaleConstraint,reinterpret_cast<std::uintptr_t>(in.source.foreign),
  reinterpret_cast<std::uintptr_t>(in.source.body),in.serial,in.weapon.epoch,in.source.part};
}
void Fail(const char* why) noexcept {failure.store(why,std::memory_order_release);}
void Finish(p::Context& c) noexcept {
 if(record.pinned&&!w::PinColliderConstraint(c,record.in.weapon,record.in.bodyGeneration,false)){
  record.quarantined=true;Fail("BUDDY_UNPIN_CURRENT_OWNER_UNPROVEN");return;}
 record.pinned=false;
 if(record.corpse.Held()&&!p::Release(c,record.corpse)){
  record.quarantined=true;Fail("CORPSE_RELEASE_CURRENT_REGISTRY_UNPROVEN");return;}
 bodyOwnerD40::claims.release(record.motor);
 record.in={};record.id=0;record.entered=record.removeEntered=record.quarantined=false;
 admitted.store(0,std::memory_order_release);blocked.store(false,std::memory_order_release);
}
void Service(p::Context& c,bool reset) noexcept {
 if(!c.Current())return;
 if(servicing.test_and_set(std::memory_order_acquire))return;
 struct Unlock{~Unlock(){servicing.clear(std::memory_order_release);}} unlock;
 // ObserveRegistry intentionally reports Unknown for old world/generation
 // leases. Ask the typed invalidation predicate FIRST: it succeeds only in a
 // current authenticated context with a different world/generation and makes
 // no call through the old native pointer. Unknown in the SAME world remains
 // quarantined below; it must never be mistaken for invalidation or Absent.
 if(record.corpse.Held()&&p::AbandonInvalidated(c,record.corpse)){
  refused.store(record.in.serial,std::memory_order_release);
  bodyOwnerD40::claims.release(record.motor);
  record.in={};record.id=0;record.entered=record.removeEntered=record.pinned=record.quarantined=false;
  admitted.store(0);blocked.store(false);Fail("WORLD_INVALIDATED_UNCONFIRMED_REFERENCE");return;
 }
 if(c.Kind()==p::Phase::WorldReset){
  // The authenticated native reset owns world/constraint destruction. This
  // phase does not admit Action. Preserve the original ID/reference until a
  // later generation proves invalidation; never manufacture an Absent ACK.
  const auto reserved=admitted.load(std::memory_order_acquire);
  if(reserved)cancelled.store(reserved,std::memory_order_release);
  return;
 }
 if(record.corpse.Held()){
  const auto registry=p::ObserveRegistry(c,record.corpse);
  if(registry!=p::Registry::Same){record.quarantined=true;Fail("CORPSE_REGISTRY_UNKNOWN");return;}
 }
 Input latest{};const bool read=input.read(latest);
 const bool reserved=read&&bodyOwnerD40::claims.current(record.motor)&&bodyOwnerD40::same(record.motor.key,MotorKeyD40(latest))&&
  latest.serial==admitted.load(std::memory_order_acquire)&&latest.physicsEpoch==admittedPhysicsEpoch.load(std::memory_order_acquire);
 bool continuing=reserved&&collab_h21::npcPhysics()&&!reset&&read&&Fresh(latest)&&collab_h21::current(latest.physicsEpoch)&&
  latest.physicsEpoch==record.in.physicsEpoch&&latest.serial==record.in.serial&&
  cancelled.load(std::memory_order_acquire)!=latest.serial&&latest.source.body==record.in.source.body&&
  latest.source.part==record.in.source.part&&latest.source.foreign==record.in.source.foreign&&
  latest.source.foreignType==record.in.source.foreignType&&latest.weapon==record.in.weapon&&
  latest.bodyGeneration==record.in.bodyGeneration;
 if(continuing&&record.corpse.Held()){
  p::ArticulatedSnapshot part{};continuing=p::ReadArticulatedPart(c,record.corpse,part);
 }
 if(record.entered){
  if(!record.id){record.quarantined=true;Fail("ADD_ENTERED_ID_UNKNOWN_NO_REPLAY");return;}
  p::ConstraintSnapshot status{};
  if(!p::ReadArticulatedConstraint(c,record.corpse,int(record.id),status)){
   record.quarantined=true;Fail("ORIGINAL_CONSTRAINT_STATUS_UNKNOWN");return;}
  if(!status.exists){refused.store(record.in.serial,std::memory_order_release);Finish(c);return;}
  auto* buddy=w::CleanupColliderLease(c,record.in.weapon,record.in.bodyGeneration);
  if(!buddy||p::NativeHandle(c,*buddy)!=status.buddy){
   record.quarantined=true;Fail("ORIGINAL_CONSTRAINT_BUDDY_MISMATCH");return;}
  if(continuing&&!record.quarantined&&!record.removeEntered)return;
  if(!continuing)refused.store(record.in.serial,std::memory_order_release);
  if(record.removeEntered)return;
  record.removeEntered=true; // Spend BEFORE native entry; no uncertain retry.
  const auto result=p::RemoveArticulatedConstraint(c,record.corpse,*buddy,record.id);
  if(result.kind!=p::ConstraintResult::Kind::Submitted){record.quarantined=true;Fail("REMOVE_REFUSED_OR_UNCERTAIN_NO_REPLAY");}
  return; // Even submitted removal must receive original-ID Absent later.
 }
 if(record.corpse.Held()){Finish(c);return;}
 if(reset||!reserved||!collab_h21::npcPhysics()||!read||!Fresh(latest)||!collab_h21::current(latest.physicsEpoch)||cancelled.load()==latest.serial||refused.load()==latest.serial){
  if(read&&latest.serial&&!collab_h21::current(latest.physicsEpoch))refused.store(latest.serial,std::memory_order_release);
  if(!record.corpse.Held()){bodyOwnerD40::claims.release(record.motor);admitted.store(0);blocked.store(false);}return;}
 record.in=latest;
 auto* buddy=w::CurrentColliderLease(c,latest.weapon,latest.bodyGeneration);
 if(!buddy){refused.store(latest.serial);Fail("NO_CURRENT_PHYSICAL_WEAPON_BUDDY");Finish(c);return;}
 if(!p::AcquireArticulated(c,latest.source,record.corpse)){
  refused.store(latest.serial);Fail("CURRENT_ARTICULATED_PART_ACQUIRE_REFUSED");Finish(c);return;}
 if(!w::PinColliderConstraint(c,latest.weapon,latest.bodyGeneration,true)){
  refused.store(latest.serial);Fail("WEAPON_BUDDY_PIN_REFUSED");Finish(c);return;}
 record.pinned=true;
 impale_hold60::Request request{};request.serial=latest.serial;
 request.ragdollBody=reinterpret_cast<std::uintptr_t>(latest.source.body);
 request.weaponBody=reinterpret_cast<std::uintptr_t>(p::NativeHandle(c,*buddy));request.part=latest.source.part;
 for(int i=0;i<3;++i){request.grip[i]=latest.grip[i];request.tip[i]=latest.tip[i];request.wound[i]=latest.wound[i];}
 const auto plan=impale_hold60::plan(request);
 if(!plan.ok){refused.store(latest.serial);Fail("CURRENT_BLADE_WOUND_GEOMETRY_REFUSED");Finish(c);return;}
 if(!bodyOwnerD40::claims.current(record.motor)||!collab_h21::npcPhysics()||!collab_h21::current(latest.physicsEpoch)||cancelled.load()==latest.serial){
  refused.store(latest.serial);Fail("ORIGINAL_F9_EPOCH_REVOKED_DURING_ACQUIRE");Finish(c);return;}
 const auto result=p::AddArticulatedConstraint(c,record.corpse,*buddy,plan.add);
 if(result.kind==p::ConstraintResult::Kind::Refused){refused.store(latest.serial);Fail("NATIVE_ADD_REFUSED");Finish(c);return;}
 record.entered=true;record.id=result.id;
 if(result.kind==p::ConstraintResult::Kind::Uncertain||!record.id){record.quarantined=true;Fail("NATIVE_ADD_UNCERTAIN_NO_REPLAY");}
}
void Step(p::Context& c,void*) noexcept {Service(c,false);}
void Reset(p::Context& c,void*) noexcept {Service(c,true);}
}
bool Submit(const Input& in) noexcept {
 if(!collab_h21::npcPhysics()){Fail("F9_GRADE3_NPC_PHYSICS_NOT_SELECTED");return false;}
 if(!collab_h21::current(in.physicsEpoch)){Fail("ORIGINAL_F9_PHYSICS_EPOCH_REVOKED");return false;}
 if(!Fresh(in)||!in.source.body||!in.source.foreign||in.source.part<0){Fail("CURRENT_NATIVE_DEAD_SOURCE_FRAME_INVALID_OR_STALE");return false;}
 if(refused.load()==in.serial)return false;
 if(!registered.load(std::memory_order_acquire)){
  if(installing.test_and_set(std::memory_order_acquire))return false;
  const bool ready=p::Preflight()&&p::RegisterClient({Step,Reset,nullptr})&&p::Install();
  registered.store(ready,std::memory_order_release);installing.clear(std::memory_order_release);
  if(!ready){Fail("ARTICULATED_PHASE_CLIENT_UNAVAILABLE");return false;}
 }
 const auto existing=admitted.load(std::memory_order_acquire);if(existing&&existing!=in.serial)return false;
 if(servicing.test_and_set(std::memory_order_acquire))return MotorBlocked();
 struct Unlock{~Unlock(){servicing.clear(std::memory_order_release);}} unlock;
 const auto owned=admitted.load(std::memory_order_acquire);if(owned&&owned!=in.serial)return false;
 if(owned&&admittedPhysicsEpoch.load(std::memory_order_acquire)!=in.physicsEpoch){Fail("ORIGINAL_F9_PHYSICS_EPOCH_REVOKED");return false;}
 if(!bodyOwnerD40::claims.acquire(record.motor,MotorKeyD40(in))){Fail("BODY_OWNED_BY_HAND_OR_ANOTHER_IMPALE");return false;}
 // Publishing reserves motor ownership BEFORE a later phase can enter Add.
 blocked.store(true,std::memory_order_release);admittedPhysicsEpoch.store(in.physicsEpoch,std::memory_order_release);admitted.store(in.serial,std::memory_order_release);
 if(!input.publish(in))return true;return true;
}
void Cancel(std::uint64_t serial) noexcept {if(serial)cancelled.store(serial,std::memory_order_release);}
bool MotorBlocked() noexcept{return blocked.load(std::memory_order_acquire);}
const char* Failure() noexcept{return failure.load(std::memory_order_acquire);}
void Refuse(const char* why) noexcept{if(why)Fail(why);}
}
