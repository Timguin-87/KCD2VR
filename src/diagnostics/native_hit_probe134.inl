// Native weapon/combat/collision observers, plus a separately gated V176 F4
// ordinary-Henry contact admission boundary. Recorder leases never own gameplay.
// Read-only observers forward once with unchanged arguments/results; only the
// explicitly qualified F4 Process producer may be rejected before native work.
//
// Addresses: retail WHGame.dll 0x6A350E20 (Steam 1.5.6), cross-checked against
// libKCD2 (function starts verified with .pdata, prologues byte-checked at install):
//   0xD53610  C_CombatRPG::ProcessCollisionHit(this, S_HitInfo&, const S_CombatRPGHitRequest&, S_MeleeHitDetails&)
//             (returns early unless the attacker state's AttackTypeId is set)
//   0xD568B0  C_CombatRPG::BuildAndSubmitMeleeHit(this, S_HitInfo&, const float& strength, bool& post,
//             const S_CombatRPGHitRequest&, bool applyNative, E_CombatAttackType, S_MeleeHitDetails&)
//             (the single funnel into native damage; 4 native producers call it)
//   0x72F644  C_CombatActor::Update(float)  (vtable 0x3E3FAC8 slot 100; every combat actor, every frame)
//   0xF299BC  entity-system LOGGED EventPhysCollision client (AddEventClient id=2, bLogged=1, from 0xE1D6B8)
// Struct offsets from libKCD2 plus the exact-player chain already proven in this project:
// base C_CombatActor +0x1F8 is its owner/entity field; C_CombatPlayer +0x1F8 is C_Player*,
// then C_Player +0x38 is Henry's CEntity*. Actor +0x210 is S_CombatActorState*.
// S_CombatActorState properties hold their value at property+0x08; EventPhysCollision size 0xA8).

namespace nhp134 {
constexpr std::uintptr_t kProcessHit=0xD53610ull,kSubmitHit=0xD568B0ull,kActorUpdate=0x72F644ull,kPhysCollision=0xF299BCull;
constexpr std::uintptr_t kCombatActorVft=0x3E3FAC8ull,kCombatPlayerVft=0x3A80610ull,
    kPlayerVft=0x3AA4B98ull,kEntityVft=0x4072C78ull;
constexpr unsigned char kProcessHitEntry[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20,0x55,0x41,0x54,0x41,0x55};
constexpr unsigned char kSubmitHitEntry[]{0x48,0x8B,0xC4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0x98};
constexpr unsigned char kActorUpdateEntry[]{0x48,0x89,0x5C,0x24,0x18,0xF3,0x0F,0x11,0x4C,0x24,0x10,0x57,0x48,0x83,0xEC,0x30};
constexpr unsigned char kPhysCollisionEntry[]{0x48,0x89,0x5C,0x24,0x08,0x55,0x56,0x57,0x48,0x8B,0xEC,0x48,0x81,0xEC,0x80,0x00,0x00,0x00};
// S_CombatActorState property offsets (value = offset + 8)
// D85 zone: +0x270 is the running attack row's input_class_id (0x938FD0 params slot 11), not a zone; it was
// named kExecutedZone before. the executed zone is symmetric(+0x150) ("kDefenseZone" here = the running attack
// row's attack_zone_id, row frame), see diagnostics/combat_state_zone_d85.h.
enum : std::uint32_t { kInCombat=0x000,kGuardStance=0x0C0,kAttackZone=0x0F0,kPendingZone=0x120,kDefenseZone=0x150,kCommittedZone=0x180,
    kAttackingHand=0x1B0,kAttackStrength=0x1E0,kAttackType=0x210,kInputClass=0x270,kAttackRequested=0x2A0,kIsAttacking=0x2D0,
    kIsBlocking=0x300,kOpponentZone=0x5D0,kBlockingHand=0x600,kBlockMode=0x650 };

namespace geom27 {void NoteInjectedSubmit178r35(void* hitInfo,void* details,bool ok,bool post) noexcept;bool NativeSweepRejected178r40(std::uintptr_t caller,void* rpg,const void* request) noexcept;bool InjectedContact40(const void* rpg,const void* request) noexcept;} // V66.178R35/R40
namespace geom27 {void NotePerfectBlock59c8(std::uint64_t now,const void* attacker) noexcept;} // R59.16C8
namespace geom27 {void NoteHenryAttackAnswered59c17(const kcd2vr::answer59c4r2::Record&) noexcept;} // copied pre-submit native lineage
namespace geom27 {namespace vrstate596 {bool SuppressNativeFollowup18(const void* rpg,std::uintptr_t callerRva,const void* request,bool applyNative,int attackType) noexcept;}} // C10R18
namespace geom27 {namespace vrstate596 {bool ReactionZoneBeginNR1(const void* attacker,int zone,const void*& hs,int& restore,int& written,const void* victim) noexcept;void ReactionZoneEndNR1(const void* attacker,const void* hs,int restore,int written) noexcept;}} // NR1 (defined in native_state_context.inl)
namespace geom27 {void NoteInjectedProcess50(bool returned) noexcept;void NoteInjectedReaction50(const void* out,int kind) noexcept;void NoteInjectedDispatch50(bool returned) noexcept;}
using ProcessHitFn=bool(__fastcall*)(void*,void*,const void*,void*);
using SubmitHitFn=bool(__fastcall*)(void*,void*,const float*,bool*,const void*,bool,int,void*);
using ActorUpdateFn=void(__fastcall*)(void*,float);
using PhysCollisionFn=int(*)(const void*);
using PhysProduceFnH24=int(__fastcall*)(void*,unsigned,const void*,void*);
PhysProduceFnH24 g_phys_produce_original_h24=nullptr;
std::optional<DetourModKit::hook::Hook> g_phys_produce_h24;
constexpr unsigned char kPhysProduceEntryH24[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x18,0x56,0x57,0x41,0x56,0x48,0x83,0xEC,0x30};
std::optional<DetourModKit::hook::Hook> g_process,g_submit,g_update,g_phys;
std::optional<DetourModKit::hook::Hook> g_reaction_build50,g_reaction_dispatch50;
ProcessHitFn g_process_original=nullptr;SubmitHitFn g_submit_original=nullptr;ActorUpdateFn g_update_original=nullptr;PhysCollisionFn g_phys_original=nullptr;
std::uintptr_t g_base=0;
std::atomic<bool> g_collision_contract_h24{false};
std::atomic<bool> g_f4_contract176{false};
std::array<unsigned char,sizeof(kProcessHitEntry)> g_process_owned176{};
std::atomic<std::uint64_t> g_counts[11]{}; // process, submit, submitPlayer, phys, physPlayer, stateChanges,
    // actorBase, actorPlayer, playerResolved, identityReject, npcOwnerUnresolved

// V66.136: also active in physical VR weapon rungs (F12>=4) so real VR swings are
// recorded, not only native-reference fights (F3==1). Still strictly read-only.
std::atomic<bool> g_vr_weapon_log136{false};
bool Active() noexcept {if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return false;return (g_enabled.load(std::memory_order_acquire)&&diagnostics::lab::Get(diagnostics::lab::Key::F3)==1)||
    g_vr_weapon_log136.load(std::memory_order_acquire)||CaptureOnly168r1();}
// Per-SITE rate cap: at most `perSecond` lines per call site per second.
// C56 review (Claude): the former shared per-category counter let a chatty
// site starve a rare refusal receipt in the same second (31 sites on category
// 4, limits 2..30). Every expansion of this macro owns its own static budget
// (a distinct closure type per site); `category` is kept for the call sites'
// readability only. See receipt_budget56.h and tests/receipt_budget56_test.cpp.
#define Allow(category,perSecond) \
    (::kcd2vr::diagnostics::capture168::active(::kcd2vr::diagnostics::capture168::Domain::Combat,GetTickCount64()) && \
     ::kcd2vr::receipt56::allow(([]() noexcept -> ::kcd2vr::receipt56::Site& {static ::kcd2vr::receipt56::Site site56;return site56;})(), \
        static_cast<unsigned>(perSecond),std::uint64_t(GetTickCount64())))
// V66.146R1 PERFORMANCE: the V134-V146 probe called ReadCopy6689 -> Readable()
// -> VirtualQuery before EVERY field read (~29 us each in the headset log, ~11
// reads per combat actor per frame). With F12>=4 or F3=1 that cost 20-60 fps.
// Probe reads are now a plain SEH-guarded copy: identical fail-closed result on
// bad memory (exception -> fallback), without a kernel call per field.
static bool SehCopy146(const void* p,void* out,std::size_t n) noexcept {
    if(!p||!out||!n)return false;
    __try{std::memcpy(out,p,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> T Rd(const void* base,std::uintptr_t off,T fallback) noexcept {
    T v{};return base&&SehCopy146(static_cast<const unsigned char*>(base)+off,&v,sizeof(v))?v:fallback;
}
#include "native_hit_receipt50c1.inl"
// Validated entity id of a CEntity pointer (exact vtable), else 0.
std::uint32_t EntityId(const void* entity) noexcept {
    if(!entity||Rd<std::uintptr_t>(entity,0,0)!=g_base+kEntityVft)return 0;
    return Rd<std::uint32_t>(entity,0xC,0);
}
// Validated retail combat-actor view. C_CombatPlayer is a zero-offset derived
// class, but its +0x1F8 backlink is C_Player*, not the final CEntity*. Preserve
// the older exact two-class allow-list and resolve Henry through the proven
// actor -> C_Player -> CEntity chain before comparing against LastPlayerEntity().
bool CombatActor(const void* actor,const void*& entity,const void*& state,bool record=true) noexcept {
    entity=nullptr;state=nullptr;
    if(!actor){if(record)g_counts[9].fetch_add(1,std::memory_order_relaxed);return false;}
    const auto vft=Rd<std::uintptr_t>(actor,0,0);
    const bool isPlayerActor=vft==g_base+kCombatPlayerVft;
    const bool isBaseActor=vft==g_base+kCombatActorVft;
    if(!isPlayerActor&&!isBaseActor){if(record)g_counts[9].fetch_add(1,std::memory_order_relaxed);return false;}
    state=Rd<const void*>(actor,0x210,nullptr);
    if(!state||Rd<const void*>(state,0xCE0,nullptr)!=actor){
        if(record)g_counts[9].fetch_add(1,std::memory_order_relaxed);state=nullptr;return false;
    }
    if(isPlayerActor){
        if(record)g_counts[7].fetch_add(1,std::memory_order_relaxed);
        const void* player=Rd<const void*>(actor,0x1F8,nullptr);
        if(!player||Rd<std::uintptr_t>(player,0,0)!=g_base+kPlayerVft||
           Rd<const void*>(player,0x278,nullptr)!=actor){
            if(record)g_counts[9].fetch_add(1,std::memory_order_relaxed);state=nullptr;return false;
        }
        entity=Rd<const void*>(player,0x38,nullptr);
        if(!entity||Rd<std::uintptr_t>(entity,0,0)!=g_base+kEntityVft){
            if(record)g_counts[9].fetch_add(1,std::memory_order_relaxed);entity=nullptr;state=nullptr;return false;
        }
        if(record)g_counts[8].fetch_add(1,std::memory_order_relaxed);
        return true;
    }
    if(record)g_counts[6].fetch_add(1,std::memory_order_relaxed);
    // V66.135R1: +0x1F8 is an owner link, not assumed to be the final CEntity.
    // Accept either a direct CEntity owner or the same owner->+0x38->CEntity shape
    // proven for Henry. If neither is true, keep combat-state observation but log
    // the owner vtable RVA once; do not invent an NPC entity id.
    const void* owner=Rd<const void*>(actor,0x1F8,nullptr);
    if(owner&&Rd<std::uintptr_t>(owner,0,0)==g_base+kEntityVft){entity=owner;return true;}
    const void* candidate=owner?Rd<const void*>(owner,0x38,nullptr):nullptr;
    if(candidate&&Rd<std::uintptr_t>(candidate,0,0)==g_base+kEntityVft){entity=candidate;return true;}
    if(record){
        g_counts[10].fetch_add(1,std::memory_order_relaxed);
        static std::atomic<std::uintptr_t> lastUnknownVft{0};const auto ov=owner?Rd<std::uintptr_t>(owner,0,0):0;
        if(ov&&lastUnknownVft.exchange(ov,std::memory_order_acq_rel)!=ov&&Allow(5,4)){char m[320]{};
            std::snprintf(m,sizeof(m),"V66.135R1-F3 NPC_OWNER_UNRESOLVED actor=%p owner=%p ownerVftRva=0x%llX stateValid=1 entityId=0 writes=NONE",
                actor,owner,(unsigned long long)(ov>=g_base?ov-g_base:ov));diagnostic_trace::Write(m);}
    }
    entity=nullptr;return true;
}
int I(const void* st,std::uint32_t prop) noexcept {return Rd<std::int32_t>(st,prop+8,-99);}
float F(const void* st,std::uint32_t prop) noexcept {return Rd<float>(st,prop+8,-99.f);}
int B(const void* st,std::uint32_t prop) noexcept {return int(Rd<std::uint8_t>(st,prop+8,0xFF));}
const char* Zone(int z) noexcept {
    switch(z){case 0:return "head";case 1:return "upperL";case 2:return "upperR";case 3:return "lowerL";case 4:return "lowerR";case 5:return "lower";case -1:return "none";default:return "?";}
}
// Compact state line: attack side + defense side.
int StateText(const void* st,char* out,std::size_t cap) noexcept {
    if(!st)return std::snprintf(out,cap,"state=NULL");
    return std::snprintf(out,cap,"inCombat=%d atk{type=%d str=%.2f zone=%s pending=%s committed=%s inputClass=%d executed=%s requested=%d attacking=%d hand=%d} "
        "def{blocking=%d zone=%s oppZone=%s blockMode=%d blockHand=%d stance=%d}",
        B(st,kInCombat),I(st,kAttackType),F(st,kAttackStrength),Zone(I(st,kAttackZone)),Zone(I(st,kPendingZone)),Zone(I(st,kCommittedZone)),
        I(st,kInputClass),Zone(::kcd2vr::zone_d85::executedHenryZone(I(st,kDefenseZone))),B(st,kAttackRequested),B(st,kIsAttacking),I(st,kAttackingHand),
        B(st,kIsBlocking),Zone(I(st,kDefenseZone)),Zone(I(st,kOpponentZone)),I(st,kBlockMode),I(st,kBlockingHand),I(st,kGuardStance));
}
bool IsPlayer(const void* entity) noexcept {return entity&&entity==frik_pose_probe::LastPlayerEntity();}

#include "native_defense_receipt27.inl"
#include "native_services59c5.inl"

// S_HitInfo result fields (after native fill).
int HitInfoText(const void* hi,char* out,std::size_t cap) noexcept {
    const float px=Rd<float>(hi,0x50,0),py=Rd<float>(hi,0x54,0),pz=Rd<float>(hi,0x58,0);
    return std::snprintf(out,cap,"hitInfo{attacker=%u target=%u hitType=%d combatData=%d dmg=%.2f stam=%.2f part=%d mat=%d strength=%d death=%d ko=%d stabbed=%d pos=(%.2f,%.2f,%.2f)}",
        Rd<std::uint32_t>(hi,0x00,0),Rd<std::uint32_t>(hi,0x28,0),Rd<std::int32_t>(hi,0x18,-1),Rd<std::int32_t>(hi,0x20,-1),
        Rd<float>(hi,0x30,0),Rd<float>(hi,0x2C,0),Rd<std::int32_t>(hi,0x3C,-1),Rd<std::int32_t>(hi,0x38,-1),Rd<std::int32_t>(hi,0x44,0),
        int(Rd<std::uint8_t>(hi,0x40,0)),int(Rd<std::uint8_t>(hi,0x41,0)),int(Rd<std::uint8_t>(hi,0x74,0)),px,py,pz);
}
int DetailsText(const void* de,char* out,std::size_t cap) noexcept {
    return std::snprintf(out,cap,"result{parryInPlace=%d blocked=%d perfectBlock=%d combo=%d/%d freeAttack=%d masterStrike=%d req=%d itemDestroyed=%d}",
        int(Rd<std::uint8_t>(de,0,0)),int(Rd<std::uint8_t>(de,1,0)),int(Rd<std::uint8_t>(de,3,0)),int(Rd<std::uint8_t>(de,4,0)),int(Rd<std::uint8_t>(de,5,0)),
        int(Rd<std::uint8_t>(de,6,0)),int(Rd<std::uint8_t>(de,7,0)),Rd<std::int32_t>(de,8,0),int(Rd<std::uint8_t>(de,0xC,0)));
}
int RequestText(const void* rq,char* out,std::size_t cap) noexcept {
    const void* details=Rd<const void*>(rq,0x18,nullptr);
    const auto* sb=Rd<const unsigned char*>(rq,0x20,nullptr);const auto* se=Rd<const unsigned char*>(rq,0x28,nullptr);
    const long long n=(sb&&se&&se>=sb)?(se-sb)/16:-1;
    return std::snprintf(out,cap,"request{bypassRepeat=%d pairFlag=%d unarmedWeakBlock=%d forceParry=%d victimHand=%d attackerHand=%d samples=%lld first{part=%d coef=%.2f energy=%.2f idx=%d} contact=(%.2f,%.2f,%.2f)}",
        int(Rd<std::uint8_t>(rq,0,0)),int(Rd<std::uint8_t>(rq,1,0)),int(Rd<std::uint8_t>(rq,2,0)),int(Rd<std::uint8_t>(rq,3,0)),
        Rd<std::int32_t>(rq,8,-1),Rd<std::int32_t>(rq,0xC,-1),n,
        n>0?Rd<std::int32_t>(sb,0,-1):-1,n>0?Rd<float>(sb,4,0):0.f,n>0?Rd<float>(sb,8,0):0.f,n>0?Rd<std::int32_t>(sb,0xC,0):0,
        Rd<float>(details,0,0),Rd<float>(details,4,0),Rd<float>(details,8,0));
}

// V176: associate a previously completed block edge with an incoming native
// result for the same live pair. This is temporal evidence, never proof that
// motion caused a perfect block. In particular an NPC blocking Henry is excluded.
static void RecordIncomingParry176(const diagnostics::parry176::Edge& edge,std::uint64_t started,
    const void* rpg,const void* request,const void* attacker,const void* target,
    const void* ae,const void* as,const void* te,const void* ts,
    const void* hitInfo,const void* details,bool returned,int attackType,std::uintptr_t callerRva) noexcept {
    namespace parry=diagnostics::parry176;
    if(!edge.sequence||!edge.epoch||!edge.pressReturned||!frik_pose_probe::CombatExperiment175Enabled()||
       callerRva!=0xD536BCull||!edge.tick||started<edge.tick||started-edge.tick>1500||
       (edge.cause==parry::Cause::Motion&&!edge.releaseReturned)||!ae||!as||!te||!ts||!attacker||!target||
       !IsPlayer(te)||IsPlayer(ae)||attacker==target)return;
    parry::Edge current{};
    if(!parry::Read(current)||current.sequence!=edge.sequence||current.epoch!=edge.epoch||
       current.publication!=edge.publication)return;
    const void *liveAe=nullptr,*liveAs=nullptr,*liveTe=nullptr,*liveTs=nullptr;
    if(Rd<const void*>(rpg,8,nullptr)!=attacker||Rd<const void*>(request,0x10,nullptr)!=target||
       !CombatActor(attacker,liveAe,liveAs,false)||!CombatActor(target,liveTe,liveTs,false)||
       liveAe!=ae||liveAs!=as||liveTe!=te||liveTs!=ts||
       edge.actor!=reinterpret_cast<std::uintptr_t>(target)||edge.state!=reinterpret_cast<std::uintptr_t>(ts)||
       edge.entity!=reinterpret_cast<std::uintptr_t>(te)||edge.player!=Rd<std::uintptr_t>(target,0x1F8,0)||
       edge.opponentActor!=reinterpret_cast<std::uintptr_t>(attacker)||edge.opponentState!=reinterpret_cast<std::uintptr_t>(as)||
       edge.opponentEntity!=reinterpret_cast<std::uintptr_t>(ae)||
       !edge.henryId||edge.henryId!=EntityId(te)||!edge.opponentId||edge.opponentId!=EntityId(ae)||
       Rd<const void*>(ts,0xCF8,nullptr)!=attacker||Rd<const void*>(as,0xCF8,nullptr)!=target)return;
    // A false Submit result need not initialize its output structures. Never
    // convert prior bytes into an observed native block/perfect-block result.
    const bool fields=returned&&hitInfo&&details&&Rd<std::uint32_t>(hitInfo,0,0)==edge.opponentId&&
        Rd<std::uint32_t>(hitInfo,0x28,0)==edge.henryId;
    const auto blocked55c2=fields?Rd<std::uint8_t>(details,1,255):255;
    const auto perfect55c2=fields?Rd<std::uint8_t>(details,3,255):255;
    if(fields&&blocked55c2<=1&&perfect55c2<=1){
        parry::results55c2.fetch_add(1,std::memory_order_relaxed);
        if(blocked55c2)parry::blocks55c2.fetch_add(1,std::memory_order_relaxed);
        if(perfect55c2)parry::perfect55c2.fetch_add(1,std::memory_order_relaxed);
    }
    if(!Active()||!Allow(6,20))return;
    char message[1000]{};std::snprintf(message,sizeof(message),
        "V66.176 F4_PARRY_RESULT edge=%llu cause=%s inputTick=%llu hitTick=%llu ageMs=%llu henry=%u opponent=%u incomingSector=%d incomingKind=NATIVE_ATTACK_SECTOR manualHeld=%d releaseReturned=%d pressReturned=%d nativeSubmitReturned=%d nativeResultFields=%s blocked=%d perfectBlock=%d masterStrike=%d health=%.3f stamina=%.3f incomingAttackType=%d association=FRESH_SAME_PAIR_TEMPORAL_NOT_CAUSAL",
        (unsigned long long)edge.sequence,parry::Name(edge.cause),(unsigned long long)edge.tick,
        (unsigned long long)started,(unsigned long long)(started-edge.tick),edge.henryId,edge.opponentId,edge.incomingSector,
        edge.manualHeld,edge.releaseReturned,edge.pressReturned,returned,fields?"VALID":"UNAVAILABLE",
        fields?int(Rd<std::uint8_t>(details,1,0)):-1,fields?int(Rd<std::uint8_t>(details,3,0)):-1,
        fields?int(Rd<std::uint8_t>(details,7,0)):-1,fields?Rd<float>(hitInfo,0x30,0.f):0.f,
        fields?Rd<float>(hitInfo,0x2C,0.f):0.f,attackType);
    diagnostic_trace::Write(message);
}

// Only the exact ordinary outgoing Henry handler is owned by this F4 gate.
// Its existing native false branch destroys the request and exits normally.
// NPC incoming hits, D538D2 and synthetic/followup producers remain native.
static bool RejectOrdinaryContact176(std::uintptr_t callerRva,void* rpg,const void* request,bool capture) noexcept {
    namespace contact=kcd2vr::contact176;
    if(callerRva!=0xC52534ull||!g_f4_contract176.load(std::memory_order_acquire)||
       !frik_pose_probe::CombatExperiment175Enabled())return false;
    const void* actor=Rd<const void*>(rpg,8,nullptr);const void *entity=nullptr,*state=nullptr;
    if(!CombatActor(actor,entity,state,false)||!IsPlayer(entity))return false;
    const auto henryId=EntityId(entity);if(!henryId)return false;
    const int nativeType=I(state,kAttackType);if(nativeType!=0&&nativeType!=1)return false;
    contact::Live live{};const bool qualified=frik_pose_probe::ReadF4ContactLive176(live);
    contact::Decision decision=contact::Decision::Identity;
    const void* target=Rd<const void*>(request,0x10,nullptr);const void *te=nullptr,*ts=nullptr;
    const bool targetValid=target&&target!=actor&&CombatActor(target,te,ts,false)&&te&&te!=entity;
    const auto targetId=targetValid?EntityId(te):0u;
    if(qualified&&live.identity.actor==reinterpret_cast<std::uintptr_t>(actor)&&
       live.identity.entity==reinterpret_cast<std::uintptr_t>(entity)&&
       live.identity.collisions==Rd<std::uintptr_t>(actor,0x218,0)){
        decision=contact::admit(live,Rd<std::uintptr_t>(request,0x18,0),targetId);
    }
    // V66.178R26: gate mode 0 = pass-through (every native sweep contact is processed, as in V174 and before);
    // the decision is still computed and logged so the ticket logic can be judged without blocking play.
    const bool gateOn=frik_pose_probe::F4ContactGateMode178r26()!=0;
    const bool reject=gateOn&&decision!=contact::Decision::Admit;
    if(capture&&Allow(7,30)){
        const auto* scope=contact::current;const auto now=std::uint64_t(GetTickCount64());
        const auto tick=scope?scope->ticket.tick:0;const auto sample=scope?scope->identity.sample:0;
        const auto serial=scope?scope->ticket.serial:0;char message[960]{};
        const auto trace=diagnostics::trace164::for_owner(Rd<std::uintptr_t>(actor,0x218,0));
        std::snprintf(message,sizeof(message),
            "V66.176 F4_CONTACT_ADMISSION decision=%s nativeCalled=%d liveQualified=%d henry=%u target=%u release=%llu releaseAgeMs=%llu poseSample=%llu callerRva=0xC52534 geometry=SAME_SYNCHRONOUS_TRACKED_SWEEP request=NATIVE_CONSTRUCTION energy=NATIVE_UNMODIFIED material=NATIVE_UNMODIFIED damage=UNCLAIMED coverage=ORDINARY_HENRY_ONLY prediction=%llu sweep=%llu contact=%llu nativeAttackType=%d",
            contact::name(decision),!reject,qualified,henryId,targetId,(unsigned long long)serial,
            (unsigned long long)(tick&&now>=tick?now-tick:UINT64_MAX),(unsigned long long)sample,
            (unsigned long long)trace.prediction,(unsigned long long)trace.sweep,(unsigned long long)trace.contact,nativeType);
        diagnostic_trace::Write(message);
    }
    return reject;
}

#include "fist_receiver_h12.inl"

bool __fastcall ProcessHitDetour(void* rpg,void* hitInfo,const void* request,void* details) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ProcessHit};
    const bool capture=Active();
    const auto ret=KCD2VR_COMBO_CALLER();const auto callerRva=(g_base&&ret>g_base)?ret-g_base:0;
    if(callerRva==0xC52534ull&&g_f4_contract176.load(std::memory_order_acquire)){
        const void* actorH11=Rd<const void*>(rpg,8,nullptr);const void *entityH11=nullptr,*stateH11=nullptr;
        if(CombatActor(actorH11,entityH11,stateH11,false)&&IsPlayer(entityH11)){
            const int typeH11=I(stateH11,kAttackType);
            if(typeH11==5||typeH11==6||typeH11==7){
                const void* contactH11=Rd<const void*>(request,0x18,nullptr);float pointH11[3]{};
                const bool readH11=contactH11&&SehCopy146(contactH11,pointH11,sizeof(pointH11));
                const int admissionH11=frik_pose_probe::FistContactAdmissionH12(actorH11,readH11?pointH11:nullptr,I(stateH11,0x1B0),contactH11);
                if(capture&&Allow(7,48)){
                    const auto& evidenceH31=kcd2vr::fist_action_h12::receiptH31;char textH31[640]{};
                    std::snprintf(textH31,sizeof(textH31),
                        "H31 FIST_CONTACT admitted=%d reason=%s nativeType=%d nativeHand=%d vrSide=%d gesture=%llu gestureSample=%llu palmSample=%llu committedSample=%llu palmAgeMs=%llu trigger=%.3f grip=%.3f distanceM=%.4f geometryBound=%s nativeDamage=%s originalContact=NATIVE_CONSTRUCTION",
                        admissionH11,kcd2vr::fist_action_h12::name(evidenceH31.failure),typeH11,evidenceH31.nativeHand,evidenceH31.side,
                        (unsigned long long)evidenceH31.serial,(unsigned long long)evidenceH31.gestureSample,(unsigned long long)evidenceH31.palmSample,
                        (unsigned long long)evidenceH31.committedSample,(unsigned long long)evidenceH31.palmAge,double(evidenceH31.trigger),double(evidenceH31.grip),evidenceH31.distance,
                        evidenceH31.nativeShapeH31?"ORIGINAL_NATIVE_SHAPE":"PALM_0.120M",admissionH11==0?"NOT_CALLED":"ORIGINAL_PENDING");
                    diagnostic_trace::Write(textH31);
                }
                if(admissionH11==0){
                    return false;
                }
            }
        }
    }
    if(geom27::NativeSweepRejected178r40(callerRva,rpg,request))return false; // V66.178R40: one producer per stroke in the live route
    const bool injected50=geom27::InjectedContact40(rpg,request);
    if(!injected50&&RejectOrdinaryContact176(callerRva,rpg,request,capture))return false;
    if(!capture){const bool result=g_process_original?g_process_original(rpg,hitInfo,request,details):false;
        if(injected50)geom27::NoteInjectedProcess50(result);return result;}
    g_counts[0].fetch_add(1,std::memory_order_relaxed);
    const void* attacker=Rd<const void*>(rpg,0x8,nullptr);const void* target=Rd<const void*>(request,0x10,nullptr);
    const void *ae=nullptr,*as=nullptr,*te=nullptr,*ts=nullptr;
    const bool aok=CombatActor(attacker,ae,as),tok=CombatActor(target,te,ts);
    const bool player=(aok&&IsPlayer(ae))||(tok&&IsPlayer(te));
    const auto trace164=diagnostics::trace164::for_owner(reinterpret_cast<std::uintptr_t>(Rd<const void*>(attacker,0x218,nullptr)));
    char pre[1400]{};int n=0;
    if(player){
        char a[400]{},t[400]{},r[400]{};StateText(as,a,sizeof(a));StateText(ts,t,sizeof(t));RequestText(request,r,sizeof(r));
        n=std::snprintf(pre,sizeof(pre),"attacker=%u%s %s | target=%u%s %s | %s",EntityId(ae),IsPlayer(ae)?"(PLAYER)":"",a,EntityId(te),IsPlayer(te)?"(PLAYER)":"",t,r);
    }
    const bool ok=g_process_original?g_process_original(rpg,hitInfo,request,details):false;
    if(injected50)geom27::NoteInjectedProcess50(ok);
    if(player&&Allow(0,30)){
        char h[400]{},d[256]{};HitInfoText(hitInfo,h,sizeof(h));DetailsText(details,d,sizeof(d));
        char m[2600]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 PROCESS_HIT f12=%d returned=%d %s %s %s prediction=%llu sweep=%llu contact=%llu redirected=%d causalJoin=SAME_NATIVE_STACK_OR_NONE",frik_pose_probe::WeaponControlMode(),ok?1:0,pre,d,h,
            (unsigned long long)trace164.prediction,(unsigned long long)trace164.sweep,(unsigned long long)trace164.contact,trace164.redirected);diagnostic_trace::Write(m);
    }
    // V66.136: raw native S_CombatCollisionDetails (0xD8) + first hit sample for
    // player<->NPC hits: the exact template a future direct VR hit producer must match.
    static std::atomic<unsigned> dumps{0};
    if(player&&tok&&target&&dumps.fetch_add(1,std::memory_order_relaxed)<40){
        const void* det=Rd<const void*>(request,0x18,nullptr);unsigned char raw[0xD8]{};
        if(det&&SehCopy146(det,raw,sizeof(raw))){char hex[0xD8*2+1]{};
            for(std::size_t k=0;k<sizeof(raw);++k)std::snprintf(hex+k*2,3,"%02X",raw[k]);
            char m[700]{};std::snprintf(m,sizeof(m),"V66.136-HIT_TEMPLATE returned=%d f12=%d detailsD8=%s",ok?1:0,frik_pose_probe::WeaponControlMode(),hex);diagnostic_trace::Write(m);}
    }
    (void)n;return ok;
}
// C4R3: only the application branch can consume a later contact. Native A/B/C
// evaluation also fills predicted damage/defense fields before the AI chooses
// and accepts its action. Preserve that original call; never claim it here.
static kcd2vr::answer59c4r2::Record AnswerBefore19c4r2(void* rpg,const void* request,int attackType,bool injected,bool applyNative,std::uintptr_t caller) noexcept {
    kcd2vr::answer59c4r2::Record answer{};
    if(!applyNative||injected||!Synchronized59c10r9()||!frik_pose_probe::CombatExperiment175Enabled())return answer;
    const void* attacker=Rd<const void*>(rpg,8,nullptr);const void *ae=nullptr,*as=nullptr;
    if(!CombatActor(attacker,ae,as,false)||!IsPlayer(ae))return answer;
    answer.attacker=reinterpret_cast<std::uintptr_t>(attacker);
    answer.target=reinterpret_cast<std::uintptr_t>(Rd<const void*>(request,0x10,nullptr));
    answer.effect=kcd2vr::answer59c4r2::Effect::Application;
    answer.attackerId=EntityId(ae);
    const void *te=nullptr,*ts=nullptr;
    if(CombatActor(reinterpret_cast<const void*>(answer.target),te,ts,false))answer.targetId=EntityId(te);
    if(g_base&&(caller==g_base+0x173BF58ull||caller==g_base+0xD536BCull))
        (void)frik_pose_probe::GeometryNativeAnswer19c4r2(attacker,reinterpret_cast<const void*>(answer.target),attackType,answer.lineage);
    return answer;
}
static void NoteAnswer59c17(kcd2vr::answer59c4r2::Record answer,const void* hitInfo,const void* details,std::uintptr_t caller) noexcept {
    if(answer.effect!=kcd2vr::answer59c4r2::Effect::Application||!details||!g_base||caller<g_base)return;
    const auto rva=caller-g_base;const bool defence=rva==0x173BF58ull||rva==0xD536BCull;
    answer.kind=Rd<std::uint8_t>(details,7,0)?4:!defence?5:Rd<std::uint8_t>(details,3,0)?3:Rd<std::uint8_t>(details,0,0)?2:Rd<std::uint8_t>(details,1,0)?1:5;
    if(!answer.kind||!answer.attacker||!answer.target)return;
    // Only audited ordinary defense producers may carry ordinary action lineage.
    // Other native master-strike producers retain conservative UNKNOWN handling.
    if(!defence)answer.lineage={};
    // A successful application with missing/mismatched result ownership is
    // still conservative no-redelivery, never a fabricated exact block/hit.
    if(!answer.attackerId||!answer.targetId||Rd<std::uint32_t>(hitInfo,0,0)!=answer.attackerId||
       Rd<std::uint32_t>(hitInfo,0x28,0)!=answer.targetId)answer.lineage={};
    // C56 review (Claude): receipt-only copies (struck part, surface material, producer). Not a match input.
    if(Active()){answer.part=Rd<std::int32_t>(hitInfo,0x3C,-1);answer.material=Rd<std::int32_t>(hitInfo,0x38,-1);answer.callerRva=rva;}
    answer.tick=std::uint64_t(GetTickCount64());geom27::NoteHenryAttackAnswered59c17(answer);
}
// C23b OUTCOME_CHAIN receipt (read-only, after the original returned): Henry-ATTACKER results only. A false
// return is no result; apply==0 (the 6th argument) is an evaluation and is only counted as skipped.
static void OutcomeSubmit23(void* rpg,void* hitInfo,void* details,bool ok,bool apply) noexcept {
    if(!Active()||!ok||!g_base)return; // C26C1: no new native qualification reads off recorder
    const void* attacker=Rd<const void*>(rpg,8,nullptr);const void *ae=nullptr,*as=nullptr;
    if(!CombatActor(attacker,ae,as,false)||!IsPlayer(ae))return;
    kcd2vr::outcome23::Hit h{};h.ok=true;h.apply=apply;
    if(apply&&hitInfo&&details){
        h.parry=Rd<std::uint8_t>(details,0,0);h.blocked=Rd<std::uint8_t>(details,1,0);h.perfect=Rd<std::uint8_t>(details,3,0);
        h.combo=Rd<std::uint8_t>(details,4,0);h.comboEnd=Rd<std::uint8_t>(details,5,0);h.master=Rd<std::uint8_t>(details,7,0);
        h.death=Rd<std::uint8_t>(hitInfo,0x40,0);h.ko=Rd<std::uint8_t>(hitInfo,0x41,0);h.target=Rd<std::uint32_t>(hitInfo,0x28,0u);}
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.hit(0,h,now,kcd2vr::outcome23::counters);});
}
// C23b OUTCOME_CHAIN: the SAME installed 90C850 receipt detour, entered through this thin wrapper so the
// R50C1 receipt unit stays unchanged. Only 1483580's own dispatch (call at 148391F, return 1483924) nested in
// OUR owned D4AD94 call is noted; arguments and the native result pass through untouched.
bool __fastcall ReactionDispatchDetour23(const void* target,void* reaction) noexcept {
    const auto caller=KCD2VR_COMBO_CALLER();
#include "../subsystems/hybrid/native_cost_before.inl" // M2_OWNED_SOURCE
    defense27::DispatchScope dispatch27(target,reaction,caller);
    const bool result=ReactionDispatchDetour50(target,reaction);
#include "../subsystems/hybrid/native_cost_completed.inl" // M2_OWNED_SOURCE
    dispatch27.Finish(result);
    if(g_base&&caller==g_base+0x1483924ull&&kcd2vr::outcome23::onStack(reinterpret_cast<std::uintptr_t>(target))){
        const auto now=std::uint64_t(GetTickCount64());
        kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.dispatch(result,now,kcd2vr::outcome23::counters);});}
    return result;
}
#include "../subsystems/gore/native_kill_observer_r94.inl" // applied-result observation only
bool __fastcall SubmitHitDetour(void* rpg,void* hitInfo,const float* strength,bool* post,const void* request,bool applyNative,int attackType,void* details) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::SubmitHit};
    kcd2vr::motion59c4::ClaimReservationC2 nativeApplication54;
#include "../subsystems/hybrid/native_application_claim.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/native_followup_barrier.inl" // M2_OWNED_SOURCE
    defense27::EvaluationScope evaluation27(rpg,request,applyNative,KCD2VR_COMBO_CALLER());
    const bool capture=Active();
    // Accepted-hit feedback belongs to the current physical gameplay owner.
    // F3's log switch must not enable/disable it. The notification below still
    // validates the fresh held melee frame, haptics ownership and pulse limits.
    const bool feedback=frik_pose_probe::PhysicalWeaponInputOwned6698();
    const bool inject35=geom27::InjectedContact40(rpg,request); // V66.178R35: a VR contact routed through the live attack action
    const auto answer19c4r2=AnswerBefore19c4r2(rpg,request,attackType,inject35,applyNative,KCD2VR_COMBO_CALLER());
    const auto parryStart176=std::uint64_t(GetTickCount64());diagnostics::parry176::Edge parryEdge176{};
    const bool parryWatch55c2=frik_pose_probe::CombatExperiment175Enabled()&&diagnostics::parry176::Read(parryEdge176)&&
        parryStart176>=parryEdge176.tick&&parryStart176-parryEdge176.tick<=1500;
    if(!capture&&!feedback&&!parryWatch55c2){
        if(g_submit_original)nativeApplication54.nativeCallStarted();
        const bool r=SubmitObservedNativeKillR94(rpg,hitInfo,strength,post,request,applyNative,attackType,details);
        evaluation27.Finish(r,details);
        if(applyNative&&r)kcd2vr::paired_signals_d62c::NoteAppliedHit(Rd<std::uint32_t>(hitInfo,0x00,0u),Rd<std::uint32_t>(hitInfo,0x28,0u),std::uint64_t(GetTickCount64())); // D62C
        if(inject35)geom27::NoteInjectedSubmit178r35(hitInfo,details,r,post?*post:false);
        else if(r)NoteAnswer59c17(answer19c4r2,hitInfo,details,KCD2VR_COMBO_CALLER());
        OutcomeSubmit23(rpg,hitInfo,details,r,applyNative); // C23b receipt only (apply=0 evaluations only counted)
        return r;}
    if(capture)g_counts[1].fetch_add(1,std::memory_order_relaxed);
    const auto ret=KCD2VR_COMBO_CALLER();const std::uintptr_t callerRva=(g_base&&ret>g_base)?ret-g_base:0;
    // Producer identity: 0xD536BC = ProcessCollisionHit (collision path); the other
    // three callers (0xC6153E, 0xC6248A, 0x173BF53) build synthetic hit details.
    const char* producer=callerRva==0xD536BCull?"COLLISION(ProcessCollisionHit)":callerRva==0xC61543ull?"SYNTHETIC_A(0xC60F94 automation-attack)":
        callerRva==0xC6248Full?"FOLLOWUP_B(0xC62288)":callerRva==0x173BF58ull?"FOLLOWUP_C(0x173BD74)":"OTHER";
    const void* attacker=Rd<const void*>(rpg,0x8,nullptr);const void* target=Rd<const void*>(request,0x10,nullptr);
    const void *ae=nullptr,*as=nullptr,*te=nullptr,*ts=nullptr;
    const bool aok=CombatActor(attacker,ae,as,capture);
    const bool parryPair55c2=parryWatch55c2&&callerRva==0xD536BCull&&
        reinterpret_cast<std::uintptr_t>(attacker)==parryEdge176.opponentActor&&reinterpret_cast<std::uintptr_t>(target)==parryEdge176.actor;
    const bool tok=(capture||parryPair55c2)&&CombatActor(target,te,ts,capture);
    const bool player=(aok&&IsPlayer(ae))||(tok&&IsPlayer(te));
    const auto trace164=capture?diagnostics::trace164::for_owner(reinterpret_cast<std::uintptr_t>(Rd<const void*>(attacker,0x218,nullptr))):diagnostics::trace164::for_owner(0);
    const float s=capture&&strength?Rd<float>(strength,0,-1.f):-1.f;
    char r[400]{};if(capture&&player)RequestText(request,r,sizeof(r));
    if(g_submit_original)nativeApplication54.nativeCallStarted();
    const bool ok=SubmitObservedNativeKillR94(rpg,hitInfo,strength,post,request,applyNative,attackType,details);
    evaluation27.Finish(ok,details);
    if(applyNative&&ok)kcd2vr::paired_signals_d62c::NoteAppliedHit(Rd<std::uint32_t>(hitInfo,0x00,0u),Rd<std::uint32_t>(hitInfo,0x28,0u),std::uint64_t(GetTickCount64())); // D62C
    if(inject35)geom27::NoteInjectedSubmit178r35(hitInfo,details,ok,post?*post:false); // V66.178R35
    // R59.16C8: Henry's own perfect block (collision producer, victim = player) opens the riposte window. Resolved only when
    // the result says perfectBlock, so the identity read costs nothing on ordinary hits.
    if(applyNative&&ok&&callerRva==0xD536BCull&&Rd<std::uint8_t>(details,3,0)!=0&&!(aok&&IsPlayer(ae))){
        const void *te8=te,*ts8=ts;if(!tok){(void)CombatActor(target,te8,ts8,false);}
        if(te8&&IsPlayer(te8))geom27::NotePerfectBlock59c8(std::uint64_t(GetTickCount64()),attacker);}
    // Only an applied native consequence can consume the physical release.
    // Evaluation results remain available to the native defensive selector.
    if(ok&&!inject35)NoteAnswer59c17(answer19c4r2,hitInfo,details,ret);
    OutcomeSubmit23(rpg,hitInfo,details,ok,applyNative); // C23b receipt only
    if(applyNative&&parryWatch55c2&&tok&&aok&&parryEdge176.sequence)RecordIncomingParry176(parryEdge176,parryStart176,rpg,request,attacker,target,
        ae,as,te,ts,hitInfo,details,ok,attackType,callerRva);
    if(capture&&player)services59c5::Hit(attacker,target,ae,te,hitInfo,details,ok,callerRva,applyNative);
    if(player){if(capture)g_counts[2].fetch_add(1,std::memory_order_relaxed);
        // V66.136R2 presentation-only feedback: only an already-native, accepted
        // collision producer for Henry as ATTACKER can pulse the physical weapon.
        // No arguments/results are changed and synthetic/follow-up producers never qualify.
        if(applyNative&&feedback&&ok&&callerRva==0xD536BCull&&aok&&IsPlayer(ae)){
            const auto hitTarget=Rd<std::uint32_t>(hitInfo,0x28,0u);
            const auto attackerId=EntityId(ae);
            if(hitTarget&&hitTarget!=attackerId){
                const auto* sb=Rd<const unsigned char*>(request,0x20,nullptr);const auto* se=Rd<const unsigned char*>(request,0x28,nullptr);
                const float energy=(sb&&se&&se>=sb+16)?Rd<float>(sb,8,0.f):0.f;
                frik_pose_probe::NotifyNativeWeaponImpact136R2(Rd<float>(hitInfo,0x30,0.f),Rd<float>(hitInfo,0x2C,0.f),energy,
                    Rd<std::uint8_t>(details,1,0)!=0,Rd<std::uint8_t>(details,3,0)!=0,Rd<std::uint8_t>(details,7,0)!=0,hitTarget);
            }
        }
        if(capture&&Allow(1,30)){char h[400]{},d[256]{};HitInfoText(hitInfo,h,sizeof(h));DetailsText(details,d,sizeof(d));
            char m[1900]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 SUBMIT_HIT producer=%s callerRva=0x%llX returned=%d attacker=%u%s target=%u%s attackType=%d strength=%.3f applyNative=%d postEffects=%d effect=%s %s %s %s prediction=%llu sweep=%llu contact=%llu redirected=%d causalJoin=SAME_NATIVE_STACK_OR_NONE",
                producer,(unsigned long long)callerRva,ok?1:0,EntityId(ae),IsPlayer(ae)?"(PLAYER)":"",EntityId(te),IsPlayer(te)?"(PLAYER)":"",attackType,s,applyNative?1:0,
                post?int(Rd<std::uint8_t>(post,0,0)):-1,applyNative?"APPLICATION":"EVALUATION",r,d,h,(unsigned long long)trace164.prediction,(unsigned long long)trace164.sweep,(unsigned long long)trace164.contact,trace164.redirected);diagnostic_trace::Write(m);}}
    return ok;
}
// Combat state changes (how the AI reacts to the player's attack zone). One small
// table of recently seen actors; a line is written only when a watched value changes.
static void RearmCombatAfterLoadR96() noexcept; // R96: defined after the combo window owners below
#include "geometry_hit178r27.inl" // V66.178R27 hybrid stage 2: direct BuildAndSubmitMeleeHit from the physical blade's impact
#include "kick_drain37.inl" // Procedural contact is observation only; native R3 owns kick admission and outcomes.
// kick_apply37c2.inl (retired research, never included) was removed in D36; it is in D35 and older trees.

// C27 immutable entry snapshot for diagnostic lineage only. During our synchronous
// phase setter the publication snapshot is not committed yet, so its TLS scope
// identifies the game-thread key. All other callbacks use the atomic seqlock.
namespace defense27 {
bool CopyDirectKey(const void* attacker,mo::Key& key) noexcept {
    namespace vs=geom27::vrstate596;key={};
    if(!Active()||!mo::selected.load()||vs::g_fault.load()||vs::g_reset.load()||!vs::g_consequence.load())return false;
    const auto player=Rd<std::uintptr_t>(attacker,0x1F8,0);
    const auto state=Rd<const void*>(attacker,0x210,nullptr);const auto now=std::uint64_t(GetTickCount64());
    if(state&&vs::t_own_phase_write19.state==state&&vs::g_owner_state==state){
        const int phase=I(state,0x30);
        if((phase==4||phase==8)&&now<vs::g_deadline&&player==vs::g_key5912.player)key=vs::g_key5912;
    }else{
        const auto& s=vs::g_attack_snapshot;const auto revision=s.revision.load(std::memory_order_acquire);if(revision&1)return false;
        const auto tick=s.tick.load(std::memory_order_acquire),deadline=s.deadline.load(std::memory_order_acquire);
        const int stage=s.stage.load(std::memory_order_acquire);
        key={s.gesture.load(),s.weapon.load(),s.generation.load(),s.body.load(),s.request.load(),s.recovery.load(),s.player.load()};
        if(s.revision.load(std::memory_order_acquire)!=revision||!mo::fresh(tick,now,150)||now>=deadline||
           (stage!=4&&stage!=8)||player!=key.player){key={};return false;}
    }
    if(!mo::valid(key)||key.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       !frik_pose_probe::GeometryOwnerValid178r30(key.weapon,key.generation,key.body,key.request)){key={};return false;}return true;
}
}

struct Watch {const void* actor=nullptr;std::int32_t v[8]{};std::uint64_t seen=0;};
std::array<Watch,24> g_watch{};std::mutex g_watch_mutex;
void Summary() noexcept;
namespace carrier_cb {void Service(void* actor) noexcept;} // D30-CMB combo carrier release (native_combo_carrier_cb.inl)
void __fastcall ActorUpdateDetour(void* actor,float frameTime) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ActorUpdate};
    carrier_cb::Service(actor); // before the native update: a deferred natural attack end is released/dropped here, never mid-update
    if(g_update_original){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ActorUpdate};g_update_original(actor,frameTime);}
    // H20: HMD dodge needs native combat context in fists/default modes too.
    // Geometry fallback and recorder lifetimes must not own this read-only cache.
    if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft){geom27::PublishHenryCombatContextH20(actor);geom27::PublishNpcDefense153(actor); // R153
        frik_pose_probe::CounterStepAfterUpdate160(actor);} // R160: VrCombatNoStep - Henry's attack keeps its start position (forward lunge removed)
    if(geom27::victim5916c3::count.load(std::memory_order_relaxed))geom27::victim5916c3::Observe(actor,std::uint64_t(GetTickCount64()));
    if(geom27::Enabled()){geom27::ActorLookupScopeH7 actorLookupScopeH7;geom27::ActorSeen27(actor);if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft){geom27::ObserveHenry178r31(actor);geom27::Drain(actor);geom27::KickDrain37(actor);}}
    else{if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft)services59c5::staminaScaling59c17.store(false,std::memory_order_relaxed); // R59.16C10R17: no fallback -> no discount
        if(geom27::TelegraphDirty178r34()&&Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft)geom27::TelegraphRestore178r34(actor);
        if(geom27::vrstate596::g_stage.load()!=0&&Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft){const void *he=nullptr,*hs=nullptr; // R59.8: cleanup outside the rung
            if(CombatActor(actor,he,hs,false))geom27::vrstate596::Tick(hs,std::uint64_t(GetTickCount64()));}} // V66.178R34 // V66.178R27 (game thread, after the native update)
    if(!Active())return;
    {static std::atomic<std::uint64_t> next{0};const auto t=std::uint64_t(GetTickCount64());auto n=next.load();
     if(t>=n&&next.compare_exchange_strong(n,t+10000))Summary();} // 10 s coverage line while F3==1
    // Full NPC state census belongs to explicit F3 reference capture. Ordinary
    // F12 handling keeps Henry's state and actual hit/contact callbacks without
    // traversing every unrelated fighter in a battle on every frame.
    const bool reference=g_enabled.load(std::memory_order_acquire)&&diagnostics::lab::Get(diagnostics::lab::Key::F3)==1;
    if(!reference&&Rd<std::uintptr_t>(actor,0,0)!=g_base+kCombatPlayerVft)return;
    const void *entity=nullptr,*st=nullptr;if(!CombatActor(actor,entity,st)||!B(st,kInCombat))return;
    const bool player59=IsPlayer(entity);if(player59)geom27::TraceClock47C2(actor,st);
    const std::int32_t v[8]{I(st,kAttackZone),I(st,kCommittedZone),B(st,kIsAttacking),I(st,kAttackType),
        B(st,kIsBlocking),I(st,kDefenseZone),I(st,kOpponentZone),I(st,kBlockMode)};
    std::unique_lock<std::mutex> lock(g_watch_mutex,std::try_to_lock);if(!lock.owns_lock())return;
    const auto now=std::uint64_t(GetTickCount64());Watch* slot=nullptr;Watch* oldest=&g_watch[0];
    for(auto& w:g_watch){if(w.actor==actor){slot=&w;break;}if(w.seen<oldest->seen)oldest=&w;}
    bool fresh=false;if(!slot){slot=oldest;*slot={};slot->actor=actor;fresh=true;}
    slot->seen=now;
    if(!fresh&&std::memcmp(slot->v,v,sizeof(v))==0)return;
    std::memcpy(slot->v,v,sizeof(v));
    g_counts[5].fetch_add(1,std::memory_order_relaxed);
    if(!Allow(2,40))return;
    char s[400]{};StateText(st,s,sizeof(s));
    const auto id59=EntityId(entity); // same entity-id read the existing COMBAT_STATE line used
    if(!player59)kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){
        k.npc59(reinterpret_cast<std::uintptr_t>(actor),id59,v[5],v[4],v[6],now);});
    char m[600]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 COMBAT_STATE actor=%u%s %s%s",id59,player59?"(PLAYER)":"",s,fresh?" (first seen)":"");
    diagnostic_trace::Write(m);
}
// Physics collisions (bumps, walls, props, bodies) that the entity system receives
// as LOGGED events. Only events touching the player entity are written in full;
// all others are only counted. Living-entity contacts that physics never logs
// (for example capsule sliding) will not appear: that absence is itself evidence.
// Exact .pdata entry BA7168: native world collision-event producer. All four
// arguments and native return are forwarded once, no registration replacement.
static int __fastcall PhysCollisionProduceDetourH24(void* world,unsigned flags,const void* event,void* queuedOut) noexcept {
    return kcd2vr::native_weapon_contact_h24::observeProducer(world,flags,event,queuedOut,
        g_collision_contract_h24.load(std::memory_order_acquire),
        [](const void* p,void* out,std::size_t size){return SehCopy146(p,out,size);},
        [](){return std::uint64_t(GetTickCount64());},[](){std::uintptr_t current=0;
            (void)SehCopy146(reinterpret_cast<const void*>(g_base+0x492D830),&current,sizeof(current));return current;},
        [](void* w,unsigned f,const void* e,void* out){return g_phys_produce_original_h24?g_phys_produce_original_h24(w,f,e,out):0;});
}
int PhysCollisionDetour(const void* ev) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::PhysCollision};
    // Logged delivery remains diagnostic-only; the real producer stamped the
    // contact source/time at occurrence, before this event could be delayed.
    int r=1;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::PhysCollision};r=g_phys_original?g_phys_original(ev):1;}
    if(!Active()||!ev||Rd<std::int32_t>(ev,0x8,-1)!=2)return r;
    g_counts[3].fetch_add(1,std::memory_order_relaxed);
    const void* fd[2]{Rd<const void*>(ev,0x20,nullptr),Rd<const void*>(ev,0x28,nullptr)};
    const int fi[2]{Rd<std::int32_t>(ev,0x30,-1),Rd<std::int32_t>(ev,0x34,-1)};
    const bool p0=fi[0]==2&&IsPlayer(fd[0]),p1=fi[1]==2&&IsPlayer(fd[1]);
    if(!p0&&!p1)return r;
    g_counts[4].fetch_add(1,std::memory_order_relaxed);
    if(!Allow(3,20))return r;
    const int self=p0?0:1,other=1-self;
    auto v=[&](std::uintptr_t o){return std::array<float,3>{Rd<float>(ev,o,0),Rd<float>(ev,o+4,0),Rd<float>(ev,o+8,0)};};
    const auto pt=v(0x3C),nrm=v(0x48),v0=v(0x54),v1=v(0x60);
    const float rel=std::sqrt((v0[0]-v1[0])*(v0[0]-v1[0])+(v0[1]-v1[1])*(v0[1]-v1[1])+(v0[2]-v1[2])*(v0[2]-v1[2]));
    const char* kind=fi[other]==0?"TERRAIN":fi[other]==1?"STATIC(wall/building)":fi[other]==2?"ENTITY":"OTHER";
    char m[700]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 PHYS_COLLISION player=side%d other=%s otherEntity=%u otherForeign=%d pt=(%.2f,%.2f,%.2f) n=(%.2f,%.2f,%.2f) relSpeed=%.2f "
        "mass=%.1f/%.1f part=%d/%d mat=%d/%d penetration=%.3f impulse=%.2f radius=%.3f",
        self,kind,fi[other]==2?EntityId(fd[other]):0u,fi[other],pt[0],pt[1],pt[2],nrm[0],nrm[1],nrm[2],rel,
        Rd<float>(ev,0x6C+4*self,0),Rd<float>(ev,0x6C+4*other,0),Rd<std::int32_t>(ev,0x74+4*self,-1),Rd<std::int32_t>(ev,0x74+4*other,-1),
        int(Rd<std::int16_t>(ev,0x7C+2*self,-1)),int(Rd<std::int16_t>(ev,0x7C+2*other,-1)),
        Rd<float>(ev,0x84,0),Rd<float>(ev,0x88,0),Rd<float>(ev,0x8C,0));
    diagnostic_trace::Write(m);return r;
}
template<class Fn,class D,std::size_t N> bool Hook(std::optional<DetourModKit::hook::Hook>& h,Fn& original,std::uintptr_t rva,const unsigned char (&entry)[N],D detour,const char* name) noexcept {
    if(!Readable(reinterpret_cast<void*>(g_base+rva),N)||std::memcmp(reinterpret_cast<void*>(g_base+rva),entry,N)!=0){
        char m[192]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 %s FAIL-CLOSED prologue mismatch at WHGame+0x%llX",name,(unsigned long long)rva);diagnostic_trace::Write(m);return false;}
    DetourModKit::hook::InlineRequest req{name,DetourModKit::hook::Target{DetourModKit::Address{g_base+rva}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),detour);if(!r)return false;
    h=std::move(*r);original=h->original<Fn>();
    if(!original||!h->enable()){h.reset();original=nullptr;return false;}return true;
}
#include "native_services_install59c5.inl"
#include "fist_receiver_install_h12.inl"
#include "native_defense_offer19c4.inl"
#include "native_defense_install27.inl"
#include "../subsystems/masterstrike_combos/native_combo_grace.inl" // R59.16C10R17 VR combo window (Henry only, F4 press 1/3)
#include "../subsystems/masterstrike_combos/native_combo_carrier_cb.inl" // D30-CMB combo carrier keep-alive (COMBO HELP: HOLD)
// R96 (complaint 5): an SEH fault inside one native combat call used to turn VR hits / the combo window / the combo
// carrier / the VR state writer off until restart. The next save/level load re-arms them to the user's own settings
// (one log line). Called on the game thread at the top of geom27::Drain, the same thread that sets the faults.
static void RearmCombatAfterLoadR96() noexcept {
    static kcd2vr::load_rearm_r96::Seen seen;if(!seen.take())return;
    namespace lr=kcd2vr::load_rearm_r96;char m[360]{};int at=0;
    const auto note=[&](bool was,const char* what){if(was&&at>=0&&at<int(sizeof(m)))at+=std::snprintf(m+at,sizeof(m)-at," %s",what);};
    if(geom27::g_wanted96.load(std::memory_order_acquire)&&!geom27::g_enabled.exchange(true,std::memory_order_acq_rel))note(true,"VR_HITS");
    note(lr::rearm(geom27::g_outer_fault178r33),"HIT_CONTINUATION");
    note(lr::rearm(geom27::g_live_fault178r35),"LIVE_ROUTE");
    note(lr::rearm(geom27::vrstate596::g_fault),"VR_STATE_WRITER");
    note(lr::rearm(geom27::vrstate596::g_effort_fault),"SWING_STAMINA");
    if(lr::rearm(combo59c17::g_timer_fault_h19)){combo59c17::g_window_ms.store(combo59c17::g_window_ms_wanted96.load(std::memory_order_relaxed),std::memory_order_relaxed);note(true,"COMBO_WINDOW");}
    note(lr::rearm(kcd2vr::combo_carrier_cb::retired),"COMBO_CARRIER");
    if(at>0){char line[420]{};std::snprintf(line,sizeof(line),"R96 LOAD_REARM combat after save/level load:%s",m);diagnostic_trace::Write(line);}
}
void Install(std::uintptr_t base) noexcept {
    g_base=base;g_f4_contract176=false;
    g_collision_contract_h24.store(retail_build112::Ready(base)&&kcd2vr::native_weapon_contact_h24::contract(
        [&](std::uintptr_t rva,void* out,std::size_t size){return SehCopy146(reinterpret_cast<const void*>(base+rva),out,size);}),std::memory_order_release);
    const bool contract176=kcd2vr::contact176::retailContract([&](std::uintptr_t rva,void* out,std::size_t size) noexcept {
        return SehCopy146(reinterpret_cast<const void*>(base+rva),out,size);
    });
    const bool fistH12=ArmFistReceiverH12(base);
    diagnostic_trace::Write(fistH12?"H12 FIST_RECEIVER ready=1 boundary=C5235C motion=NATIVE_UNCHANGED contact=CAUSAL_VR_FIST":"H12 FIST_RECEIVER ready=0 boxingContact=FAIL_CLOSED weaponLive=EXACT_UNPATCHED_ONLY");
    const bool a=Hook(g_process,g_process_original,kProcessHit,kProcessHitEntry,&ProcessHitDetour,"kcd2vr_v66134_process_collision_hit");
    if(a&&contract176&&SehCopy146(reinterpret_cast<const void*>(base+kProcessHit),g_process_owned176.data(),g_process_owned176.size()))
        g_f4_contract176.store(true,std::memory_order_release);
    const bool b=Hook(g_submit,g_submit_original,kSubmitHit,kSubmitHitEntry,&SubmitHitDetour,"kcd2vr_v66134_submit_melee_hit");
    const bool c=Hook(g_update,g_update_original,kActorUpdate,kActorUpdateEntry,&ActorUpdateDetour,"kcd2vr_v66134_combat_actor_update");
    const bool d=Hook(g_phys,g_phys_original,kPhysCollision,kPhysCollisionEntry,&PhysCollisionDetour,"kcd2vr_v66134_logged_phys_collision");
    const bool producerH24=g_collision_contract_h24.load(std::memory_order_acquire)&&
        Hook(g_phys_produce_h24,g_phys_produce_original_h24,0xBA7168,kPhysProduceEntryH24,&PhysCollisionProduceDetourH24,"kcd2vr_h24_collision_occurrence");
    if(!producerH24)g_collision_contract_h24.store(false,std::memory_order_release);
    const bool rb=Hook(g_reaction_build50,g_reaction_build_original50,kReactionBuild50,kReactionBuildEntry50,&ReactionBuildDetour50,"kcd2vr_r50c1_reaction_build_receipt");
    const bool rd=Hook(g_reaction_dispatch50,g_reaction_dispatch_original50,kReactionDispatch50,kReactionDispatchEntry50,&ReactionDispatchDetour23,"kcd2vr_r50c1_reaction_dispatch_receipt"); // C23b: same hook, receipt wrapper
    services59c5::Install();
    diagnostic_trace::Write(defense27::Install()?"V66.178R50C1 REACTION_RECEIPTS C27 npcDefenseCandidate=1 observationOnly=1":"V66.178R50C1 REACTION_RECEIPTS C27 npcDefenseCandidate=0 nativeBehavior=UNCHANGED");
    diagnostic_trace::Write(defense_offer19c4::Install()?"C19C4 DEFENSE_INTENT ready=1 scope=HENRY_VR_PHASE4 nativeEligibility=RETAINED":"C19C4 DEFENSE_INTENT ready=0 native behavior unchanged");
#include "../subsystems/masterstrike_combos/native_install.inl" // M2_OWNED_SOURCE
    char receipt[192]{};std::snprintf(receipt,sizeof(receipt),"V66.178R50C1 REACTION_RECEIPTS build=%d dispatch=%d scope=SAME_TRACKED_CONTACT_STACK writes=NONE",rb,rd);diagnostic_trace::Write(receipt);
    char m[256]{};std::snprintf(m,sizeof(m),"V66.134R1-F3R1 NATIVE_WEAPON_COMBAT_PROBE processHit=%d submitHit=%d combatState=%d physCollision=%d active=F3==1 writes=NONE",a,b,c,d);
    diagnostic_trace::Write(m);
}
void Uninstall() noexcept {
    g_collision_contract_h24.store(false,std::memory_order_release);
    defense27::Uninstall();
    defense_offer19c4::Uninstall();
#include "../subsystems/masterstrike_combos/native_uninstall.inl" // M2_OWNED_SOURCE
    services59c5::Uninstall();
    g_f4_contract176.store(false,std::memory_order_release);
    (void)ReleaseFistReceiverH12();
    for(auto* h:{&g_process,&g_submit,&g_update,&g_phys,&g_reaction_build50,&g_reaction_dispatch50,&g_phys_produce_h24})if(*h){(void)(*h)->disable();h->reset();}
    g_process_original=nullptr;g_submit_original=nullptr;g_update_original=nullptr;g_phys_original=nullptr;g_phys_produce_original_h24=nullptr;
    g_reaction_build_original50=nullptr;g_reaction_dispatch_original50=nullptr;
}
void SetVrWeaponLogging(bool on) noexcept {g_vr_weapon_log136.store(on,std::memory_order_release);}
void Summary() noexcept {
    char m[512]{};std::snprintf(m,sizeof(m),"V66.135R1-F3 SUMMARY processHit=%llu submitHit=%llu submitPlayer=%llu physCollisions=%llu physPlayer=%llu combatStateChanges=%llu actorBase=%llu actorPlayer=%llu playerResolved=%llu identityReject=%llu npcOwnerUnresolved=%llu",
        (unsigned long long)g_counts[0].load(),(unsigned long long)g_counts[1].load(),(unsigned long long)g_counts[2].load(),
        (unsigned long long)g_counts[3].load(),(unsigned long long)g_counts[4].load(),(unsigned long long)g_counts[5].load(),
        (unsigned long long)g_counts[6].load(),(unsigned long long)g_counts[7].load(),(unsigned long long)g_counts[8].load(),(unsigned long long)g_counts[9].load(),
        (unsigned long long)g_counts[10].load());
    diagnostic_trace::Write(m);
#include "../subsystems/masterstrike_combos/native_summary.inl" // M2_OWNED_SOURCE
}
}

// Macro applies only to this recorder and its nested includes.
#undef Allow
