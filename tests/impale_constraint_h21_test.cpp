// Actual production callback/service; only pinned native leaves are doubled.
#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdint>
static std::uint64_t GetTickCount64(){return 1000;}
#endif
#define KCD2VR_IMPALE_CONSTRAINT_H21_TEST 1
#include "../src/subsystems/gore/impale_constraint_h21.cpp"
#include <cassert>
#include <cstdio>
namespace p=kcd2vr::native_physics_phase_t4;
namespace w=kcd2vr::native_weapon_body_t4;
namespace s=kcd2vr::impale_constraint_h21;
namespace bodyOwnerD40=kcd2vr::corpse_body_owner_d40;
static p::Client client{};static bool current=true,present=false,buddyCurrent=true,pinned=false;
static int adds=0,removes=0,refs=0,releases=0;static bool uncertainRemove=false;
static p::Phase kind=p::Phase::Step;static int buddyValue=1,corpseValue=2,foreignValue=3;
static bool generationChanged=false,registryUnknownSameWorld=false;static unsigned abandoned=0;
static bool revokeOnAcquire=false;
static p::Lease buddy;
namespace kcd2vr::native_physics_phase_t4 {
struct Access {
 static void Held(Lease& l,bool held){l.held_=held;}
 static void Tick(bool reset=false){Context c;assert(client.step);(reset?client.reset:client.step)(c,nullptr);}
};
struct ArticulatedAccess{static void Held(ArticulatedLease& l,bool held){Access::Held(l.retained_,held);}};
bool Context::Current()const noexcept{return current;}
Phase Context::Kind()const noexcept{return kind;}
bool Preflight()noexcept{return true;}bool Install()noexcept{return true;}
bool RegisterClient(Client c)noexcept{client=c;return true;}
bool AcquireArticulated(Context&,const ArticulatedSource& in,ArticulatedLease& l)noexcept {
 assert(in.physicsId==-1&&in.body==&corpseValue&&in.part==379&&in.foreign==&foreignValue&&in.foreignType==2);
 ++refs;ArticulatedAccess::Held(l,true);
 if(revokeOnAcquire){revokeOnAcquire=false;kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);}
 return true;}
Registry ObserveRegistry(Context&,const ArticulatedLease&)noexcept{return generationChanged||registryUnknownSameWorld?Registry::Unknown:Registry::Same;}
bool ReadArticulatedPart(Context&,ArticulatedLease&,ArticulatedSnapshot&)noexcept{return true;}
bool Release(Context&,ArticulatedLease& l)noexcept{++releases;ArticulatedAccess::Held(l,false);return true;}
bool AbandonInvalidated(Context&,ArticulatedLease& l)noexcept{if(!generationChanged)return false;++abandoned;ArticulatedAccess::Held(l,false);return true;}
void* NativeHandle(Context&,Lease&,bool)noexcept{return &buddyValue;}
bool ReadArticulatedConstraint(Context&,ArticulatedLease&,int id,ConstraintSnapshot& out)noexcept {
 assert(id==77);out.exists=present;out.buddy=&buddyValue;return true;}
ConstraintResult AddArticulatedConstraint(Context&,ArticulatedLease&,Lease&,const kcd2vr::constraint_packet60::Packet& packet)noexcept {
 ++adds;assert(kcd2vr::constraint_packet60::get<unsigned>(packet,0x28)==379);present=true;
 return {ConstraintResult::Kind::Submitted,77,77};}
ConstraintResult RemoveArticulatedConstraint(Context&,ArticulatedLease&,Lease&,std::uint32_t id)noexcept {
 ++removes;assert(id==77);return {uncertainRemove?ConstraintResult::Kind::Uncertain:ConstraintResult::Kind::Submitted,1,77};}
}
namespace kcd2vr::native_weapon_body_t4 {
p::Lease* CurrentColliderLease(p::Context&,const Owner& owner,std::uint64_t generation)noexcept {
 assert(owner.key==42&&generation==9);return buddyCurrent?&buddy:nullptr;}
p::Lease* CleanupColliderLease(p::Context&,const Owner& owner,std::uint64_t generation)noexcept {
 assert(owner.key==42&&generation==9);return &buddy;}
bool PinColliderConstraint(p::Context&,const Owner&,std::uint64_t,bool pin)noexcept{pinned=pin;return true;}
}
static s::Input Make(std::uint64_t serial){s::Input in{};in.serial=serial;in.tick=GetTickCount64();
 in.physicsEpoch=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
 in.bodyGeneration=9;in.weapon.key=42;in.weapon.epoch=1;in.source={-1,379,&corpseValue,&foreignValue,2};in.nativeDeadCurrent=true;
 in.tip[1]=1;in.wound[1]=.7;return in;}
int main(){
 assert(!s::Submit(Make(1))&&adds==0);kcd2vr::collab_h21::setPhysicsMode(3);
 bodyOwnerD40::Lease handLease,servoLease;
 const bodyOwnerD40::Key handKey{bodyOwnerD40::Slot::RightHand,reinterpret_cast<std::uintptr_t>(&foreignValue),reinterpret_cast<std::uintptr_t>(&corpseValue),77,1,379};
 assert(bodyOwnerD40::claims.acquire(handLease,handKey));
 assert(!s::Submit(Make(1))&&!s::MotorBlocked()&&adds==0); // actual service cannot steal a held victim
 bodyOwnerD40::claims.release(handLease);
 auto in=Make(1);in.nativeDeadCurrent=false;assert(!s::Submit(in)&&adds==0);
 in=Make(1);in.tick-=100;assert(!s::Submit(in)&&adds==0);
 in=Make(1);assert(s::Submit(in)&&s::MotorBlocked());p::Access::Tick();
 assert(adds==1&&refs==1&&pinned&&s::MotorBlocked());
 assert(!bodyOwnerD40::claims.acquire(handLease,handKey));
 const bodyOwnerD40::Key servoKey{bodyOwnerD40::Slot::ImpaleServo,handKey.entity,handKey.body,1,1,379};
 assert(bodyOwnerD40::claims.acquire(servoLease,servoKey));bodyOwnerD40::claims.release(servoLease);
 assert(!bodyOwnerD40::claims.acquire(handLease,handKey)); // ending display/servo does not retire the joint's claim
 in.tick=GetTickCount64();assert(s::Submit(in));p::Access::Tick();assert(adds==1&&removes==0);
 // A different serial cannot steal entered ownership or resume velocity motor.
 assert(!s::Submit(Make(2))&&s::MotorBlocked());
 s::Cancel(1);p::Access::Tick();assert(removes==1&&pinned&&s::MotorBlocked());
 assert(!bodyOwnerD40::claims.acquire(handLease,handKey)); // Remove submitted is still not Absent
 p::Access::Tick();assert(removes==1&&s::MotorBlocked()); // submission != Absent
 present=false;p::Access::Tick();assert(!pinned&&!s::MotorBlocked()&&releases==1);
 assert(bodyOwnerD40::claims.acquire(handLease,handKey));bodyOwnerD40::claims.release(handLease);
 assert(!s::Submit(Make(1))); // an ended original transaction cannot silently re-add its joint
 // No physical buddy: actual admission refuses before Add and fallback unblocks.
 buddyCurrent=false;assert(s::Submit(Make(2)));p::Access::Tick();
 assert(adds==1&&!s::MotorBlocked());assert(!s::Submit(Make(2)));
 buddyCurrent=true;assert(s::Submit(Make(3)));p::Access::Tick();assert(adds==2&&pinned);
 uncertainRemove=true;s::Cancel(3);p::Access::Tick();assert(removes==2&&s::MotorBlocked());
 for(int i=0;i<12;++i)p::Access::Tick();assert(removes==2&&pinned&&s::MotorBlocked());
 present=false;p::Access::Tick();assert(!s::MotorBlocked()&&!pinned&&releases==2);
 // Shutdown cleanup uses current typed owner; never a raw queue or ordinary tick.
 uncertainRemove=false;assert(s::Submit(Make(4)));p::Access::Tick();kind=p::Phase::Shutdown;
 p::Access::Tick(true);assert(removes==3&&pinned);present=false;p::Access::Tick(true);
 assert(!pinned&&!s::MotorBlocked());
 kind=p::Phase::Step;assert(s::Submit(Make(5)));p::Access::Tick();assert(pinned);
 kind=p::Phase::WorldReset;p::Access::Tick(true);assert(removes==3&&s::MotorBlocked());
 // Native reset retires the old world/buddy; next authenticated generation can
 // abandon its unconfirmed reference, never label it an observed Absent.
 pinned=false;generationChanged=true;kind=p::Phase::WorldReplaced;p::Access::Tick(true);
 assert(abandoned==1&&!s::MotorBlocked()&&removes==3);
 generationChanged=false;kind=p::Phase::Step;auto six=Make(6);assert(s::Submit(six));
 kcd2vr::collab_h21::setPhysicsMode(0);p::Access::Tick();assert(adds==4&&!s::MotorBlocked());
 // An OFF/ON cycle is revocation, even when no physics callback saw OFF.
 kcd2vr::collab_h21::setPhysicsMode(3);auto seven=Make(7);assert(s::Submit(seven));
 kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);
 assert(!s::Submit(seven));p::Access::Tick();assert(adds==4&&!s::MotorBlocked());
 auto eight=Make(8);assert(s::Submit(eight));p::Access::Tick();assert(adds==5&&pinned);
 kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);
 // Even a fresh newly republished producer sample cannot re-own the old joint.
 auto renewed=Make(8);assert(!s::Submit(renewed));p::Access::Tick();assert(removes==4&&pinned);
 p::Access::Tick();assert(removes==4&&s::MotorBlocked());present=false;p::Access::Tick();
 assert(!pinned&&!s::MotorBlocked());
 assert(!s::Submit(Make(8)));p::Access::Tick();assert(adds==5); // no re-add from a renewed receipt
 revokeOnAcquire=true;assert(s::Submit(Make(9)));p::Access::Tick();assert(adds==5&&!pinned&&!s::MotorBlocked());
 assert(s::Submit(Make(10)));p::Access::Tick();assert(adds==6&&pinned);
 registryUnknownSameWorld=true;p::Access::Tick();assert(abandoned==1&&removes==4&&pinned&&s::MotorBlocked());
 // A same-generation registry fault is NOT permission to abandon the ID/pin.
 p::Access::Tick();assert(abandoned==1&&removes==4&&pinned&&s::MotorBlocked());
 registryUnknownSameWorld=false;s::Cancel(10);p::Access::Tick();assert(removes==5&&pinned);
 present=false;p::Access::Tick();assert(!pinned&&!s::MotorBlocked());
 // If publication could not replace a previously published request, its old
 // body/serial must not borrow a newly reserved per-body motor incarnation.
 const auto beforeStaleAddsD40=adds;
 assert(s::Submit(Make(11)));assert(s::input.publish(Make(12)));p::Access::Tick();
 assert(adds==beforeStaleAddsD40&&!s::MotorBlocked());
 assert(bodyOwnerD40::claims.acquire(handLease,handKey));bodyOwnerD40::claims.release(handLease);
 std::puts("PASS H21 actual articulated impale service: per-body hand/servo exclusion, ownership through Absent, no-replay, current buddy, shutdown");
}
