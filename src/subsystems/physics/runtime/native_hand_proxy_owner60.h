#pragma once
#include "native_hand_proxy_t4.h"
#include "native_weapon_body_t4_policy.h"
#include "native_physics_phase_t4.h" // DS-F68: the Context/Lease/RigidSnapshot types the seam uses
#include <cstdint>
// DS-F58: the hand grip proxy NATIVE owner. Its OWN records (side 0/1), its
// OWN cookie/foreignType (never the weapon owner's), driven by the same
// audited phase::Context seams the T4 weapon owner uses: 8463B8 world
// factory -> AdoptCreatedRigid -> ReadyRigid -> bounded SetVelocity motor ->
// retire/destroy. The policy (native_hand_proxy_t4) only REQUESTS; the
// receipts are produced here from the native seam results, joined to the
// exact incarnation. The phase step calls ServiceStep(context) - the call
// site wiring is a separate stage.
namespace kcd2vr::physics::hand_proxy60 {
struct ProxyInput {
    kcd2vr::native_hand_proxy_t4::Input input{};
    std::uintptr_t player=0;std::uint64_t trackingEpoch=0;
};
struct phase_rigid_snapshot { // DS-F68: the seam-facing rigid snapshot (mirrors phase::RigidSnapshot)
    float posX=0,posY=0,posZ=0,linX=0,linY=0,linZ=0,angX=0,angY=0,angZ=0;
    float qX=0,qY=0,qZ=0,qW=1;float mass=0,scale=0;std::uint32_t flags=0;int simClass=-1;bool active=false;
};
// DS-F68 (Codex directive): the injectable native seam. The real owner wires
// the audited T4 calls (8463B8 factory, AdoptCreatedRigid, ReadyRigid,
// ReadRigid, SetVelocity, ObserveRegistry, Release, 846E10 destroy) at
// Install; the connected host fixture wires a MOCK seam to drive the full
// lifecycle end-to-end. Every seam result is joined to the incarnation.
struct Seam {
    void* (*create)(int side,const kcd2vr::native_hand_proxy_t4::Pose& pose,std::uint64_t generation) noexcept=nullptr; // the 8463B8 factory
    bool  (*adopt)(int side,void* body) noexcept=nullptr;    // AdoptCreatedRigid (the cookie is per-side)
    bool  (*ready)(int side) noexcept=nullptr;               // ReadyRigid + a strictly later Serial
    bool  (*readRigid)(int side,phase_rigid_snapshot& out) noexcept=nullptr; // ReadRigid (measured pose + mass)
    bool  (*setVelocity)(int side,float vx,float vy,float vz,float ax,float ay,float az) noexcept=nullptr; // DS-F78: SetVelocity (bounded motor, linear + angular)
    int   (*observeRegistry)(int side) noexcept=nullptr;     // ObserveRegistry: 0=Unknown,1=Same,2=Retired
    bool  (*release)(int side) noexcept=nullptr;             // Release (the held ref)
    int   (*destroy)(int side,void* body) noexcept=nullptr;  // 846E10 queued destroy: >0 pending, -999 fault
    bool  (*abandon)(int side) noexcept=nullptr;             // AbandonInvalidated (world replacement)
    // DS-F77 (Codex geometry): the audited noncolliding geometry path (the
    // weapon's MakeGeometry/AddGeometry/RemoveGeometry pins) - mass>0 is what
    // makes ReadRigid qualify the proxy.
    void* (*makeGeometry)(int side) noexcept=nullptr;        // capsule primitive + register (8102E0/8100EC/97CA04)
    bool  (*addGeometry)(int side,void* geometry) noexcept=nullptr; // AddGeometry(collision=false, mass) via 526194
    bool  (*removeGeometry)(int side,void* body) noexcept=nullptr;  // DS-F84: RemoveGeometry 38FEE3C + part-count + Unregister 84A954 (returns the proof)
    // H24 physical-presence mode reuses THIS body, never a second hand owner.
    // The mode is source identity, so grab/presence handoff must retire first.
    bool (*presenceSource)(int,const kcd2vr::native_hand_proxy_t4::Input&) noexcept=nullptr;
    bool (*configurePresence)(int) noexcept=nullptr; // living filter + collision readback after clearance
    // +1 exact self pair Present; 0 pending, -1 refused. Geometry remains collision-off.
    int (*preparePresencePair)(int) noexcept=nullptr;
    // Restore class4 BEFORE Remove. +1 exact original pair Absent and buddy released.
    int (*retirePresencePair)(int) noexcept=nullptr;
    bool (*activatePresencePair)(int) noexcept=nullptr; // after Present and configured collision shape only

};
bool SubmitPresenceH24(int side,const kcd2vr::native_hand_proxy_t4::Input&) noexcept; // POD only; never success/creation receipt
void CancelPresenceH24(int side) noexcept;
using PresencePredicateH24=bool(*)(const kcd2vr::native_hand_proxy_t4::Input&) noexcept;
bool PresenceReadyH24() noexcept;
bool SetPresencePredicateH24(PresencePredicateH24) noexcept; // startup-only atomic/mailbox source proof; no native calls
bool Install(std::uintptr_t base) noexcept; // the real seam (same exact pins as the T4 owner)
void InstallForTest(const Seam& seam) noexcept; // DS-F68: the fixture's mock seam
bool SubmitHandOwned(native_physics_phase_t4::Context&,int side,const kcd2vr::native_hand_proxy_t4::Input&) noexcept;
bool RetireOwned(native_physics_phase_t4::Context&,int side) noexcept;
bool PinConstraint(native_physics_phase_t4::Context&,int side,std::uint64_t incarnation,bool pin) noexcept;
native_physics_phase_t4::Lease* CleanupProxyLease(native_physics_phase_t4::Context&,int side,std::uint64_t incarnation) noexcept;
void ServiceReset(native_physics_phase_t4::Context&) noexcept;
native_physics_phase_t4::Lease* CurrentProxyLease(native_physics_phase_t4::Context&,int side,std::uint64_t incarnation) noexcept;
void SubmitHand(int side,const kcd2vr::native_hand_proxy_t4::Input& in) noexcept; // committed controller pose per hand
void Retire(int side) noexcept;
bool ReadAchieved(int side,kcd2vr::native_hand_proxy_t4::Achieved& out) noexcept;
struct PresenceFeedbackH24 {kcd2vr::native_hand_proxy_t4::Source source{};
    kcd2vr::native_hand_proxy_t4::Pose pose{};std::uint64_t tick=0,incarnation=0;bool valid=false;};
// Copied measured pose only. No native body pointer escapes to the pose producer.
bool ReadPresenceFeedbackH24(int side,PresenceFeedbackH24&) noexcept;

// Serialized physics-owner stack only: copied targets and retained debt,
// no body/world queries. Includes physical hand presence independent of F9.
bool NeedsPhysicsStepD80(std::uint64_t now) noexcept;
void ServiceStep(class native_physics_phase_t4::Context& context) noexcept; // per-step: policy + native receipts + motor
void ServiceStepFixture(int side,std::uint64_t generation,std::uint64_t serial,std::uint64_t now) noexcept; // DS-F68: the fixture's direct drive (no phase::Context)
std::uintptr_t Base() noexcept;          // DS-F62: the pinned module base (for the executor's gate)
bool ClientRegistered() noexcept;        // DS-F62: the registration gate proof
int SideForBuddy(std::uintptr_t buddy) noexcept; // DS-F62: the buddy token convention (low bits = side)
kcd2vr::native_physics_phase_t4::Lease* CurrentProxyLease(int side) noexcept; // DS-F74 (Codex T4 wrapper): the SAME-STACK buddy lease for phase::AddConstraint
}
