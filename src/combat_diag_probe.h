namespace kcd2vr::motion59c4 { struct Key; enum class AttackOwnership; }
namespace kcd2vr::combo_zone_assist_cz { struct NativeCandidate; } // I12
#pragma once
#include <cstdint>
#include "vrik/contact_metadata59c7g.h"
namespace kcd2vr::combat_diag_probe {
// C61: C58 additive physics-lab observers. These are read-only proofs; no new damage authority.
struct LabAnsweredLiving58 {std::uint32_t entity=0;int part=-1;std::uint64_t sample=0,tick=0;std::uint32_t serial=0;int reaction=-1;};
unsigned LabTakeAnsweredLiving58(LabAnsweredLiving58* out,unsigned cap) noexcept;
unsigned LabDeathCandidates58(std::uint32_t* out,unsigned cap,std::uint64_t now) noexcept;
bool LabCorpseQualified58(std::uint32_t entity,std::uintptr_t identity,std::uint64_t epoch,std::uint64_t now,const char*& why) noexcept;

bool Install() noexcept;
// Exact post-enable receipt for our passive GetCharacter hook only. This does
// not authorize arbitrary detours, change the vtable, or create a character.
bool NativeCharacterEntryQualified63(std::uintptr_t whBase) noexcept;
// Worker-only formatting of bounded, read-only F3 native service records.
void DrainNativeServices59C5() noexcept;
// F4 gameplay admission requires its hook and exact publisher/caller contract,
// independently of the optional Combat recorder.
bool ArmF4ContactGate176() noexcept;
bool F4ContactGateReady176() noexcept;
void Uninstall() noexcept;
void SetEnabled(bool enabled) noexcept;
// V66.136: read-only hit/collision logging while physical VR weapons are active (F12>=4).
void SetVrWeaponHitLogging136(bool on) noexcept;
bool Enabled() noexcept;
// Installation inventory only; neither native invocation nor feature success.
std::uint32_t ReadinessMask168r1() noexcept;
// One owner for WHGame+D51854. Observation is before native hit filtering.
using ContactObserver115=void(*)(void*,const void*,const void*,const void*) noexcept;
bool RegisterContactObserver115(ContactObserver115 observer) noexcept;
void ClearContactObserver115(ContactObserver115 observer) noexcept;
std::uint64_t CollisionCalls() noexcept;
// V66.178R27 hybrid stage 2: the physical blade's body impact is submitted directly through BuildAndSubmitMeleeHit.
void SetGeometryHits178r27(bool on) noexcept;
bool GeometryHitsEnabled178r27() noexcept;
void SetSynchronized59c10r9(bool on) noexcept; // R59.16C10R9
bool Synchronized59c10r9() noexcept;
void SetIntent59c16(bool on) noexcept; // R59.16C10R16 F4 press 1: contact = immediate direct hit after the release
bool Intent59c16() noexcept;
void SetRung3Switches59c12(bool syncOverdueAny,bool recoveryOverride,bool noRowFallback) noexcept; // R59.16C10R12 (19:43 log)
void SetRung4Switches59c16(bool reclaimFromOwnGuard,bool strikeDuringStagger) noexcept; // R59.16C10R16 (F4 press 4)
void SetVrDirect18(bool ownHits,float npcBlockSpeed) noexcept; // C10R18 F4 press 4: blade owns Henry's hits; NPC block speed (x native, 0 = legacy ETA)
void SetCombatFlow59c17(bool intentLive,unsigned intentLiveMs,unsigned comboWindowMs) noexcept; // R59.16C10R17 press-1 live route, VR combo window
void SetComboWindowExtension(bool on) noexcept;
bool ComboWindowExtension() noexcept;
// I12 VrComboZoneAssist: read-only copy of Henry's live native combo candidates (recipe technique, cursor) for the
// press-time zone choice in native_input117.inl. 0 = no exact Henry manager / not readable / nothing in progress.
unsigned ComboCandidatesCZ(const void* actor,kcd2vr::combo_zone_assist_cz::NativeCandidate* out,unsigned max) noexcept;
void SetComboTestForce(bool on) noexcept; // DS-F6 VrComboTestForce: TEST window hold + hit-flag gate + C69 receipts
bool ComboTestForce() noexcept; // DS-F6
void CombatFlowReceipts59c17(char* out,std::size_t cap) noexcept; // R59.16C10R17 cumulative receipts for the normal log
void SetDecapitation59c7e(bool on,float minEnergy) noexcept; // R59.16C7E phase 1 (log only)
void NoteImpactBodyMaterial59c7e(std::uint64_t sample,int material) noexcept;
void SetGeometryAttackTypes178r27(int cut,int thrust) noexcept;
void NoteImpactPower59c8(std::uint64_t sample,float ratio,bool twoHanded) noexcept; // R59.16C8: strike energy ratio + two-handed grip beside the impact sample
bool GeometryRipostePress59c8(std::uint64_t now) noexcept;
void SetRiposteNative59c8(bool on) noexcept;
void SetHitPairFlag59c9(bool on) noexcept; // retired compatibility no-op
void SetStrikePower59c8(bool enabled,float twoHandedBonus,float minPower,float maxPower,unsigned repeatMs,bool ripostePressNative,unsigned riposteWindowMs) noexcept;

void SetVrHitPolicy5913(unsigned targetRepeatMs,bool hostileOnly) noexcept;
void SetVrNeutralHitMinEnergy59c4(float energy) noexcept; // R59.16C4
bool GeometryHitSubmit178r27(std::uint32_t entityId,const float pos[3],float energy,int kind,int part,int surface,std::uint64_t tick,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture59c3,const contact59c7g::Metadata* metadata=nullptr) noexcept; // kind: 0 cut, 1 thrust; V66.178R30: owner identity
bool GeometryWeaponContactSubmit178r43(std::uint32_t itemEntityId,const float pos[3],float energy,int kind,int part,int surface,std::uint64_t tick,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture59c3) noexcept; // V66.178R43: the opponent's weapon/shield item struck -> a blow into his block through the live route
void SetGeometryApply178r30(bool on) noexcept; // V66.178R30: 1 = apply branch of BuildAndSubmitMeleeHit (collision caller), 0 = evaluation only
void GeometryHitSummary178r27() noexcept;
void SetHybridTargetWait58(bool on) noexcept; // C65H1 bounded HYBRID actor-map retry, INI only
bool GeometryLockClick178r29(std::uint64_t now) noexcept; // V66.178R29: true while the one-shot R3 press window is open
int HenryInCombat178r31(std::uint64_t now) noexcept; // V66.178R31: Henry's state inCombat flag seen within 1 s (-1 unknown, 0 out of combat, 1 in combat)
// Fresh original-first current reciprocal opponent phase4/8, ordinary Henry ownership.
// Input eligibility only; KCD2 still decides dodge/perfect-dodge outcomes.
bool IncomingHmdDodgeH24(std::uint64_t now) noexcept;
bool EngagedHmdDodgeD74(std::uint64_t now) noexcept;
// Fresh original native +630 MAX-property +64D only; unknown/stale is hidden.
bool NativeBlockIndicatorH24(std::uint64_t now) noexcept;
void SetGeometryEngageNative178r31(bool on) noexcept; // V66.178R31: rung 2 lets the native attack lifecycle through while Henry is out of combat
bool GeometryEngageNative178r31() noexcept;
void SetGeometryOuter178r33(bool on) noexcept;
void SetGeometryRejectNativeSweep178r40(bool on) noexcept; // V66.178R40: Henry's canned sweep never hits in the live route
void SetGeometryHold178r39(unsigned ms,int phase) noexcept; // V66.178R39: hold a contact for the native strike phase
void SetGeometryContactTime178r46(bool on,int phaseMask) noexcept; // R46C1: legacy configuration diagnostic only; native phase writes retired
void SetGeometryDirectFallback178r37(bool on) noexcept;
// V66.178R59.6: VR state driver (phase/zone through the native notifying setters) + contact impulses.
void SetVrStateConsequence596(bool on) noexcept;bool VrStateConsequence596() noexcept;void SelectVrBackendRung596(bool rung3) noexcept;
bool VrStateEvent5912(const kcd2vr::motion59c4::Key& key,std::uint64_t sourceTick,int event,int zone,int technique) noexcept; // 1 prepare, 2 commit, 3 legacy refinement, 4 cancel, 6 qualified cut feint (producer thread)
// Cheap, exact-context publication query; no native polling. Gesture zero asks
// about this equipment context's preparation/strike lease. Unknown means the
// publisher changed during the read, so this tick cannot authorize a guard.
kcd2vr::motion59c4::AttackOwnership VrStateAttackOwnership59c10(const kcd2vr::motion59c4::Key& context,std::uint64_t now) noexcept;
void SetSwingStamina5916(bool on) noexcept;
void SetTelegraphStrike59c10(bool on) noexcept;
void SetDirectBlockTiming39(bool on,float atSkill0,float atSkill30) noexcept; // C39: VR DIRECT NPC block speed by skill (announced +450 for a waiting reciprocal opponent)
void SetNpcBlockEta59c10(float prepareSeconds,float strikeSeconds) noexcept; // R59.16C10R4B: +0x450 contact ETA published to the NPC block aligner (S33) // experimental forward offer: phase4, then phase8 on a later Henry update; no attack action
void SetContactImpulse596(bool on,float scale,float capNs,float massKg) noexcept;bool ContactImpulse596() noexcept;
void SetContactImpulseDiagnostics19c4r4(bool on) noexcept;
void ContactImpulseRequest596(std::uint32_t entity,int part,int body,const float pos[3],const float normal[3],float speed,std::uint64_t sourceTick,std::uintptr_t collider,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept;
void SetContactNeverLost595(bool on) noexcept; // Compatibility no-op: unsafe rebound fallback retired.
void SetGeometryLiveAction178r35(bool on) noexcept; // V66.178R35: route VR contacts into Henry's live native attack action (C5235C)
bool GeometryLiveAction178r35() noexcept;
void SetGeometryTelegraph178r34(bool on) noexcept; // V66.178R34: mirror the VR stroke into Henry's attack state so the enemy AI can block
void GeometryTelegraph178r34(int phase,int zone,std::uint64_t now) noexcept; // 0 idle, 1 pending (stroke prepared), 2 committed // V66.178R33: run the collision caller's outer continuation (victim reaction + OnHit) after an applied geometry hit
void NativeOwnerLatch178r33(std::uint64_t key,std::uint64_t until) noexcept; // V66.178R33: one owner per gesture - the native engagement swing owns contacts until `until`
bool NativeOwned178r33(std::uint64_t now) noexcept;
}
