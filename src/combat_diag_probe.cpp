#include "core/load_rearm_r96.h" // R96: save-load re-arm of session latches
#include "subsystems/masterstrike_combos/combo_zone_assist_cz.h" // I12 VrComboZoneAssist
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // I10/NF native kill animations switch
#include "subsystems/masterstrike_combos/paired_signals_d62c.h" // D62C blade contact / applied hit signals
#include "subsystems/physics/runtime/native_weapon_contact_event_h24.h"
#include "diagnostics/hmd_dodge_context_h24.h"
#include "subsystems/experimental/free_targeting_plus_r133.h" // R133: Henry's opponent edge for FREE PLUS
#include "subsystems/direct/npc_force_block153.h" // R153: Henry's phase + reciprocal opponent skill for the NPC defence roll
#include "subsystems/direct/tutorial_native_r159.h" // R159: "Tutorials: native hits only" gate of the direct route (geometry_hit178r27.inl)
#include "subsystems/direct/training_vanilla_r163.h" // R163: "Training fights: vanilla NPCs" gates (native_state_context.inl, counter stab, stagger rule)
#include "vrik/direct_response36.h"
#include "core/collab_test_h21.h"
#include "vrik/direct_counter39.h" // C39 DIRECT counter candidate receipt (pure)
#include "vrik/counter_stab40.h" // C40 DIRECT counter stab (pure policy)
#include "vrik/defense_observe37.h" // C37 read-only defense receipts
#include "vrik/defense_timing39.h" // C39 DIRECT: skill-scaled announced contact ETA for a waiting reciprocal opponent
#include "vrik/dirblock39.h" // C39 DIRBLOCK: directional NPC block for a waiting reciprocal opponent (pure)
#include "vrik/impale40.h" // C40 IMPALE BY ARMOUR: candidate thrust + depth model (pure), armour verdict mailbox
#include "vrik/live_attributes5912.h"
#include "vrik/npc_contact172.h"
#include "vrik/part_namespace62.h"
#include "diagnostics/combat_services59c5.h"
#include "vrik/motion_owned59c4.h"
#include "subsystems/direct/combat_router_h3.h"
#include "vrik/combo_test_ds.h" // DS-F6 VrComboTestForce (file scope: never inside the native namespace)
#include "vrik/combat_r9.h" // R9 combo trigger phase-2 slot keep (file scope)
#include "diagnostics/defense_publication19.h"
#include "diagnostics/native_defense_intent19c4.h"
#include "vrik/combo_lineage19.h"
#include "vrik/swing_effort59c14.h" // R59.16C10R14 energy-proportional stamina charges
#include "vrik/kick_hit34.h" // C37 VR leg kick transaction + its one native effort scope
#include "vrik/kick_style26.h" // C37C2 opt-in real-hit kick test switch
#include "diagnostics/capture168.h"
#include "subsystems/masterstrike_combos/combo_recovery_hold_r14.h" // D30-CMB carrier window budget
#include "subsystems/masterstrike_combos/combo_carrier_cb.h" // D30-CMB combo carrier keep-alive
#include "diagnostics/receipt_budget56.h" // C56 review: per-site F3 receipt budget (the Allow macro in native_hit_probe134.inl)
#include "diagnostics/geometry_scope40c1.h"
#include "diagnostics/combat_state_zone_d85.h" // D85 zone: proven combat state zone fields
#include "vrik/visible_failure.h" // NR1 one visible reason line
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h" // CB1 physical combo finisher gate
#include "vrik/contact_receipt58c1.h"
#include "vrik/finisher_observation_c2.h"
#include "vrik/gore32.h" // C32 lethal cut -> decapitation candidate
#include "vrik/blood49.h" // C49 BLOODY FIGHTS: qualified hits by Henry -> native frame
#include "vrik/contact_location59c10.h"
#include "vrik/body_combo_recipes59c7g.h"
#include "vrik/receipt_build59c7g.h"
#include "diagnostics/hook_cost178r22.h" // V66.178R22b per-hook cost
#include "vrik/attack_xi178r8.h" // V66.178R22: in-handler hook timer (native_combo_observer.inl)
#include "diagnostics/intent168.h"
#include "combat_diag_probe.h"
#include "diagnostics/owned_character_entry63.h"
#include "diagnostics/actor_lookup_h7.h"
#include "diagnostics/articulated_target_owner_h8.h"
#include "core/contact_backlog59c6.h"
#include "core/strike_power_rule59c10r6.h" // R59.16C10R6: 30..120 % damage rule
#include "diagnostics/contact_lease47c2.h"
#include "vrik/attack_clock50c2.h"
#include "vrik/fist_action_h12.h"
#include "diagnostic_trace.h"
#include "frik_pose_probe.h"
#include "animation_filter.h"
#include "retail_build112.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include "diagnostics/lab_state.h"
#include "diagnostics/combo_observe.h"
#include "diagnostics/combat_capture.h"
#include "diagnostics/combat_trace164.h"
#include "diagnostics/outcome_chain23.h" // C23b OUTCOME_CHAIN receipts (read-only)
#include "diagnostics/parry_trace176.h"
#include "vrik/contact_authority176.h"
#include "vrik/contact_handoff59c3.h"
#include "subsystems/hybrid/contact_policy.h"
#include "subsystems/hybrid/target_wait_policy.h"
#include "vrik/recovery_retire65h.h"
#include "vrik/native_answer59c4r2.h"
#include "vrik/contact_contract176.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include "core/readable_pages178r22c1.h"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <optional>
#include <mutex>
#include <utility>
#if defined(_MSC_VER)
#include <intrin.h>
#define KCD2VR_COMBO_CALLER() reinterpret_cast<std::uintptr_t>(_ReturnAddress())
#else
#define KCD2VR_COMBO_CALLER() reinterpret_cast<std::uintptr_t>(__builtin_return_address(0))
#endif

namespace kcd2vr::combat_diag_probe { namespace {
constexpr std::uint32_t kTimestamp=0x6A350E20u;
constexpr std::uintptr_t kOnCollisionRva=0xD51854ull;
constexpr unsigned char kEntry[]={0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0xB0};
using CollisionFn=void(__fastcall*)(void*,const void*,const void*,const void*);
std::optional<DetourModKit::hook::Hook> g_hook; CollisionFn g_original=nullptr;
std::atomic<ContactObserver115> g_contact_observer115{nullptr};
std::uintptr_t g_hook_base115=0;
std::array<unsigned char,sizeof(kEntry)> g_owned_entry115{};
static bool CaptureOnly168r1() noexcept {
    return diagnostics::combat_capture::captureOnly168r1(GetTickCount64());
}
static bool ObserveCombat168r1() noexcept {
    return diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
}
std::atomic<bool> g_enabled{false}; std::atomic<std::uint64_t> g_calls{0},g_last_log{0},g_last_signature{0};

// V66.178R23: the combo/collision captures issue thousands of reads per call (candidate names are copied
// byte by byte); one VirtualQuery per read cost ~98 ms per COMBO_RESET in the 21:18 log. A per-thread cache of
// the last 8 committed regions answers repeat queries; it is cleared at every capture entry and the memcpy
// stays under SEH, so a region freed mid-capture still faults safely instead of reading garbage.
struct ReadableRegion178r23{std::uintptr_t a=0,e=0;};
static thread_local ReadableRegion178r23 g_readable_cache178r23[8];static thread_local unsigned g_readable_next178r23=0;
void ClearReadableCache178r23() noexcept {for(auto& r:g_readable_cache178r23)r={};g_readable_next178r23=0;}
bool Readable(const void* p,std::size_t n=1) noexcept { if(!p)return false;const auto a=(std::uintptr_t)p;
    for(const auto& r:g_readable_cache178r23)if(r.e&&a>=r.a&&a<=r.e&&n<=r.e-a)return true;
    // R22C1 (ChatGPT): resident-page fast path first; the VirtualQuery fallback fills the region cache.
    std::uintptr_t rf=0,re=0;if(kcd2vr::memory126::residentReadable(p,n,rf,re)){auto& slot=g_readable_cache178r23[g_readable_next178r23++&7u];slot={rf,re};return true;}
    MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(p,&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;const auto e=(std::uintptr_t)m.BaseAddress+m.RegionSize;
    if(a<=e){auto& slot=g_readable_cache178r23[g_readable_next178r23++&7u];slot={(std::uintptr_t)m.BaseAddress,e};}
    return a<=e&&n<=e-a; }
bool Exact(std::uintptr_t b) noexcept { __try{if(!b)return false;auto*d=(IMAGE_DOS_HEADER*)b;if(d->e_magic!=IMAGE_DOS_SIGNATURE)return false;auto*n=(IMAGE_NT_HEADERS64*)(b+d->e_lfanew);return n->Signature==IMAGE_NT_SIGNATURE&&n->FileHeader.TimeDateStamp==kTimestamp&&Readable((void*)(b+kOnCollisionRva),sizeof(kEntry))&&std::memcmp((void*)(b+kOnCollisionRva),kEntry,sizeof(kEntry))==0;}__except(EXCEPTION_EXECUTE_HANDLER){return false;} }

// Obtain the contact character only by observing KCD2's own GetCharacter call.
// No GetCharacter invocation/poll from collision, render or mod worker threads.
using EntityGetCharacterFn6690=void*(__fastcall*)(void*,int);
std::optional<DetourModKit::hook::Hook> g_character_hook6690;
EntityGetCharacterFn6690 g_character_original6690=nullptr;
character_entry63::OwnedEntry g_character_entry63;
static bool CopyCharacterEntry63(std::uintptr_t at,void* out,std::size_t n) noexcept {
    if(!Readable(reinterpret_cast<const void*>(at),n))return false;
    __try{std::memcpy(out,reinterpret_cast<const void*>(at),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct ContactAssociation6690 {void* entity=nullptr;std::uint32_t id=0;std::uint64_t seq=0,expiry=0,epoch=0;};
std::mutex g_contact_association_mutex6690;
ContactAssociation6690 g_contact_association6690;
std::atomic<void*> g_contact_entity6694{nullptr};
std::atomic<std::uintptr_t> g_contact_henry_combat6695{0};
std::atomic<std::uint64_t> g_contact_henry_tick6695{0};
std::atomic<std::uint64_t> g_enriched6694{0},g_enrich_skipped6694{0};
static void RememberContactEntity6690(void* entity,std::uint32_t id,std::uint64_t seq) noexcept {
    std::unique_lock<std::mutex> lock(g_contact_association_mutex6690,std::try_to_lock);
    if(lock.owns_lock()){g_contact_association6690={entity,id,seq,static_cast<std::uint64_t>(GetTickCount64())+2500,diagnostics::lab::Snapshot()>>4};g_contact_entity6694.store(entity,std::memory_order_release);}
}
static void* __fastcall EntityCharacterDetour6690(void* self,int slot) noexcept {
    // V66.178R30 (R28 review perf 1): this GetCharacter observer runs ~90,000 times a second. Unrelated traffic (another
    // entity, another slot, no combat observation) is forwarded with ONE counter increment and no clock; only the
    // calls that can feed the contact observer are timed. HOOK_COST's PASSIVE_CHARACTER ms/maxUs is therefore the
    // cost of the relevant calls, its calls= the total (sampled timing, not an exact total time).
    const bool relevant=slot==0&&self==g_contact_entity6694.load(std::memory_order_acquire)&&ObserveCombat168r1();
    if(!relevant){kcd2vr::hookcost178r22::count(kcd2vr::hookcost178r22::EntityCharacter);return g_character_original6690?g_character_original6690(self,slot):nullptr;}
    kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::EntityCharacter};
    void* result=nullptr;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::EntityCharacter};result=g_character_original6690?g_character_original6690(self,slot):nullptr;}
    if(!result)return result;
    ContactAssociation6690 c{};
    {std::unique_lock<std::mutex> lock(g_contact_association_mutex6690,std::try_to_lock);
        if(!lock.owns_lock())return result;c=g_contact_association6690;}
    if(self==c.entity&&c.id&&c.epoch==(diagnostics::lab::Snapshot()>>4)&&GetTickCount64()<c.expiry)
        frik_pose_probe::ObserveCollisionAnimationCharacter(result,c.id,c.seq);
    return result;
}
static bool InstallCharacterObserver6690(std::uintptr_t b) noexcept {
    constexpr unsigned char entry[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x33,0xd2,0xe8,0x09,0x5f,0xf9,0xff,0x4c};
    if(!Readable(reinterpret_cast<void*>(b+0x6411AC),sizeof(entry))||std::memcmp(reinterpret_cast<void*>(b+0x6411AC),entry,sizeof(entry)))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6690_passive_contact_character",DetourModKit::hook::Target{DetourModKit::Address{b+0x6411AC}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&EntityCharacterDetour6690);if(!h)return false;
    g_character_hook6690=std::move(*h);g_character_original6690=g_character_hook6690->original<EntityGetCharacterFn6690>();
    if(!g_character_original6690||!g_character_hook6690->enable())return false;
    return g_character_entry63.publish(b,reinterpret_cast<std::uintptr_t>(g_character_original6690),g_character_hook6690->is_enabled(),CopyCharacterEntry63);
}
using EnrichFn6689=bool(__fastcall*)(void*,void*,const void*,const void*,const void*,const void*);
std::optional<DetourModKit::hook::Hook> g_enrich_hook6689;EnrichFn6689 g_enrich_original6689=nullptr;
static thread_local const void* g_event6689=nullptr;
static thread_local std::uint64_t g_event_sequence6689=0;
static bool ReadCopy6689(const void* p,void* out,std::size_t n) noexcept {
    if(!Readable(p,n))return false;__try{std::memcpy(out,p,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
#include "diagnostics/contact_path122.inl"
void __fastcall Detour(void* self,const void* p0,const void* p1,const void* collision) noexcept {
    if(auto observer=g_contact_observer115.load(std::memory_order_acquire))observer(self,p0,p1,collision);
    const bool vrTrace164=diagnostics::trace164::active(GetTickCount64(),frik_pose_probe::WeaponControlMode());
    if(!ObserveCombat168r1()&&!vrTrace164){if(g_original)g_original(self,p0,p1,collision);return;}
    const auto oldEvent=g_event6689;const auto oldSeq=g_event_sequence6689;
    g_event6689=collision;g_event_sequence6689=g_calls.fetch_add(1)+1;
    {ContactPathScope122 path(self,collision,g_event_sequence6689);
     if(g_original)g_original(self,p0,p1,collision);}
    g_event6689=oldEvent;g_event_sequence6689=oldSeq;
}
static bool __fastcall EnrichDetour6689(void* self,void* out,const void* event,const void* geom,const void* collider,const void* keys) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::Enrich};
    const bool enabled=ObserveCombat168r1()&&event==g_event6689;
    std::uint32_t entityKey=0;
    if(enabled)(void)ReadCopy6689(static_cast<const unsigned char*>(event)+0x24,&entityKey,sizeof(entityKey));
    const bool ok=g_enrich_original6689?g_enrich_original6689(self,out,event,geom,collider,keys):false;
    if(!enabled)return ok;
    const auto enriched=g_enriched6694.fetch_add(1)+1;
    // The exact native call wrote these enrichment fields; no wrapper pointer scan.
    void* actorLookup=nullptr;void* combatLookup=nullptr;
    if(ok&&out){
        (void)ReadCopy6689(static_cast<unsigned char*>(out)+0xD0,&actorLookup,sizeof(actorLookup));
        (void)ReadCopy6689(static_cast<unsigned char*>(out)+0xC8,&combatLookup,sizeof(combatLookup));
    }
    void* entityCandidate=nullptr;std::uintptr_t vft=0;std::uint32_t candidateId=0;bool entityJoin=false;
    if(actorLookup&&ReadCopy6689(static_cast<unsigned char*>(actorLookup)+0x38,&entityCandidate,sizeof(entityCandidate))&&entityCandidate&&
       ReadCopy6689(entityCandidate,&vft,sizeof(vft))&&vft==reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"))+0x4072C78&&
       ReadCopy6689(static_cast<unsigned char*>(entityCandidate)+0xC,&candidateId,sizeof(candidateId)))entityJoin=candidateId==entityKey;
    if(entityJoin){RememberContactEntity6690(entityCandidate,candidateId,g_event_sequence6689);
        if(entityCandidate==frik_pose_probe::LastPlayerEntity()){
            // Numeric correlation only; never used to authorize writes or
            // dereference an earlier actor after this native callback.
            g_contact_henry_combat6695.store(reinterpret_cast<std::uintptr_t>(combatLookup));
            g_contact_henry_tick6695.store(GetTickCount64());
        }}
    animation_filter::MarkContactForPlayback(g_event_sequence6689);
    const auto stamp=animation_filter::DiagnosticPlaybackStamp();
    const auto now=static_cast<std::uint64_t>(GetTickCount64());const std::uint64_t sig=std::uint64_t(entityKey)^(std::uint64_t(reinterpret_cast<std::uintptr_t>(actorLookup))<<17)^std::uint64_t(ok);
    const auto prev=g_last_log.load();
    if(now-prev<100|| (sig==g_last_signature.load()&&now-prev<1000)){g_enrich_skipped6694.fetch_add(1);return ok;}
    g_last_log.store(now);g_last_signature.store(sig);
    char m[640]{};std::snprintf(m,sizeof(m),"V66.90-F3-ENRICH eventSeq=%llu keyFromPayload24=%u nativeEnrichReturned=%d actorLookup=%p combatLookup=%p entityCandidate=%p idReadback=%u exactEntityJoin=%d isHenry=%d isHeldWeapon=%d participantRole=UNCLASSIFIED combatAccepted=UNCLAIMED physicsResponse=NONE",(unsigned long long)g_event_sequence6689,entityKey,ok,actorLookup,combatLookup,entityCandidate,candidateId,entityJoin,entityJoin&&entityCandidate==frik_pose_probe::LastPlayerEntity(),entityJoin&&(entityCandidate==frik_pose_probe::WeaponControlEntity(0)||entityCandidate==frik_pose_probe::WeaponControlEntity(1)));diagnostic_trace::Write(m);
    char context[640]{};std::snprintf(context,sizeof(context),"V66.90-F3-CONTACT_CONTEXT tick=%llu epoch=%llu eventSeq=%llu playbackSeq=%llu playbackAgeMs=%llu entity=%u role=UNCLASSIFIED acceptedHit=UNKNOWN comboState=UNKNOWN victimHealth=UNKNOWN finisherEligibility=UNKNOWN phaseRelation=NEAREST_PRIOR_SAMPLE",
        (unsigned long long)now,(unsigned long long)(diagnostics::lab::Snapshot()>>4),(unsigned long long)g_event_sequence6689,(unsigned long long)stamp.serial,(unsigned long long)(stamp.tick&&now>=stamp.tick?now-stamp.tick:UINT64_MAX),entityJoin?candidateId:0);diagnostic_trace::Write(context);
    char countLine[256]{};std::snprintf(countLine,sizeof(countLine),"V66.94-F3 COVERAGE callbacks=%llu enriched=%llu throttled=%llu event=%llu participantRole=UNCLASSIFIED",
        (unsigned long long)g_calls.load(),(unsigned long long)enriched,(unsigned long long)g_enrich_skipped6694.load(),(unsigned long long)g_event_sequence6689);diagnostic_trace::Write(countLine);
    return ok;
}
static bool InstallEnrichment6689(std::uintptr_t b) noexcept {
    constexpr unsigned char expected[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x48};
    if(!Readable(reinterpret_cast<void*>(b+0xD51B38),sizeof(expected))||std::memcmp(reinterpret_cast<void*>(b+0xD51B38),expected,sizeof(expected)))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6689_collision_enrichment",DetourModKit::hook::Target{DetourModKit::Address{b+0xD51B38}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&EnrichDetour6689);if(!r)return false;
    g_enrich_hook6689=std::move(*r);g_enrich_original6689=g_enrich_hook6689->original<EnrichFn6689>();return bool(g_enrich_hook6689->enable());
}

#include "subsystems/masterstrike_combos/native_combo_observer.inl"
#include "diagnostics/native_hit_probe134.inl"
#include "diagnostics/close_query164.inl"

} // namespace
bool Install() noexcept {
    const auto b=(std::uintptr_t)GetModuleHandleW(L"WHGame.dll");
    if(!retail_build112::Verify(b))return false;
    if(g_hook)return g_original&&b==g_hook_base115&&Readable((void*)(b+kOnCollisionRva),g_owned_entry115.size())&&
        std::memcmp((void*)(b+kOnCollisionRva),g_owned_entry115.data(),g_owned_entry115.size())==0;
    if(!Exact(b)){diagnostic_trace::Write("V66.89-F3 install refused: exact OnCollision entry mismatch");return false;}
    const auto cleanup=[](){if(g_enrich_hook6689){(void)g_enrich_hook6689->disable();g_enrich_hook6689.reset();}g_enrich_original6689=nullptr;};
    if(!InstallEnrichment6689(b)){cleanup();return false;}
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6689_combat_collision",DetourModKit::hook::Target{DetourModKit::Address{b+kOnCollisionRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&Detour);if(!h){cleanup();return false;}
    g_hook=std::move(*h);g_original=g_hook->original<CollisionFn>();
    if(!g_original||!g_hook->enable()){g_original=nullptr;g_hook.reset();cleanup();return false;}
    std::memcpy(g_owned_entry115.data(),reinterpret_cast<void*>(b+kOnCollisionRva),g_owned_entry115.size());g_hook_base115=b;
    if(!InstallCharacterObserver6690(b)){
        g_character_entry63.clear();if(g_character_hook6690){(void)g_character_hook6690->disable();g_character_hook6690.reset();}g_character_original6690=nullptr;
        diagnostic_trace::Write("V66.90-F3 CONTACT_ANIMATION_ASSOCIATION_BLOCKED passive character hook unavailable; Henry playback/contact records remain available");
    }else diagnostic_trace::Write("V66.90-F3 passive contact->native GetCharacter return observer READY; no active getter calls; participant role UNKNOWN");
    if(InstallCombo6691(b)) diagnostic_trace::Write("V66.91-F3-COMBO_BINDING READY source=NATIVE_STEP_FILTER_AND_RESET limit=CURSORS_CANDIDATES_NOT_HIT_ACCEPTANCE");
    else diagnostic_trace::Write("V66.91-F3-COMBO_BINDING BLOCKED reason=FINGERPRINT_OR_HOOK_INSTALL existingF3=UNCHANGED");
    nhp134::Install(b); // V66.134R1/135 read-only native hit/collision/combat-state probe (F3 rung 1)
    diagnostic_trace::Write(InstallCloseQuery164(b)?"V66.164 F3_CLOSE_QUERY ready source=687BBC_RETURN nativeEligibility=UNCHANGED":"V66.164 F3_CLOSE_QUERY unavailable exactContractRequired=1");
    diagnostic_trace::Write(InstallContactPath122(b)?"V66.122 F3_CONTACT_PATH ready stages=5 sameNativeCallback=1 mutation=NONE":
        "V66.122 F3_CONTACT_PATH unavailable originalCollisionUnchanged=1");
    diagnostic_trace::Write("V66.89-F3 READY native event+0x24 -> enrichment lookup; no wrapper scans; physical response NOT IMPLEMENTED");return true;
}
bool NativeCharacterEntryQualified63(std::uintptr_t base) noexcept {
    return base==reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"))&&retail_build112::Ready(base)&&
        g_character_entry63.qualifies(base,CopyCharacterEntry63);
}
void DrainNativeServices59C5() noexcept {nhp134::services59c5::Drain();}
bool F4ContactGateReady176() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!b||b!=nhp134::g_base||!retail_build112::Ready(b)||!nhp134::g_f4_contract176.load(std::memory_order_acquire)||
       !nhp134::g_process||!nhp134::g_process_original)return false;
    std::array<unsigned char,sizeof(nhp134::kProcessHitEntry)> current{};
    return nhp134::SehCopy146(reinterpret_cast<const void*>(b+nhp134::kProcessHit),current.data(),current.size())&&
        current==nhp134::g_process_owned176&&kcd2vr::contact176::retailContract([&](std::uintptr_t rva,void* out,std::size_t size) noexcept {
            return nhp134::SehCopy146(reinterpret_cast<const void*>(b+rva),out,size);
        });
}
bool ArmF4ContactGate176() noexcept {return Install()&&F4ContactGateReady176();}
// V66.178R27 geometry hits (hybrid stage 2) - see diagnostics/geometry_hit178r27.inl
void SetGeometryHits178r27(bool on) noexcept {nhp134::geom27::g_wanted96.store(on,std::memory_order_release);const bool was=nhp134::geom27::g_enabled.exchange(on);if(!on)nhp134::geom27::Clear();
    if(was!=on)nhp134::combo59c17::ClearLineage19();
    if(on&&!was&&nhp134::g_base){ // V66.178R38: the reaction-kind globals the collision caller uses (C52DF4 -> 0x5320F18, D499D8 death -> 0x5320D24) and the ordinary attack type constants (D4CBAA 0x53209D8, D53656 0x53209E0): are they combat_action_type ids (7 hitTorso, 8 hitBlockBroken, 55/56 masterStrike...)?
        char m[260]{};std::snprintf(m,sizeof(m),"V66.178R38 GEOM_KINDS hitKind(0x5320F18)=%d deathKind(0x5320D24)=%d g53209D8=%d g53209E0=%d (compare with Libs/Tables/combat/combat_action_type.xml)",
            nhp134::Rd<std::int32_t>(reinterpret_cast<const void*>(nhp134::g_base+0x5320F18ull),0,-99),nhp134::Rd<std::int32_t>(reinterpret_cast<const void*>(nhp134::g_base+0x5320D24ull),0,-99),
            nhp134::Rd<std::int32_t>(reinterpret_cast<const void*>(nhp134::g_base+0x53209D8ull),0,-99),nhp134::Rd<std::int32_t>(reinterpret_cast<const void*>(nhp134::g_base+0x53209E0ull),0,-99));diagnostic_trace::Write(m);}
    if(was!=on){nhp134::geom27::Summary();
    diagnostic_trace::Write(on?"R59C3 GEOM_HITS state=ON route=LIVE_ACTION_WHEN_SELECTED nativePrepare=SEMANTIC_ONLY target=EXACT_CONTACT_ENTITY authoredOrdinaryHits=DENIED_IN_F4":"V66.178R27 GEOM_HITS state=OFF");}}
bool GeometryHitsEnabled178r27() noexcept {return nhp134::geom27::Enabled();}
#include "subsystems/hybrid/public_route_controls.inl" // M2_OWNED_SOURCE
void CombatFlowReceipts59c17(char* out,std::size_t cap) noexcept { // R59.16C10R17: cumulative game-thread receipts for the normal log
    if(!out||!cap)return;namespace g=nhp134::geom27;namespace sv=nhp134::services59c5;namespace cb=nhp134::combo59c17;
    std::snprintf(out,cap,"hits{intentLive=%llu intentDirect=%llu strengthLeases=%llu consumed=%llu answeredByNpc=%llu skipped=%llu} combos{hitFlags=%llu edges=%llu sameValue=%llu unknownEdge=%llu deferredResets=%llu slotsExtended=%llu keptOpen=%llu lateResets=%llu hooks=%d} "
        "stamina{nativeStrikes=%llu scaled=%llu zeroed=%llu ownPairs=%llu fallbacks=%llu lastFactor=%.2f lastNative=%.2f lastScaled=%.2f}",
        (unsigned long long)g::g_intent_live_hits59c17.load(),(unsigned long long)g::g_intent_direct_hits59c17.load(),(unsigned long long)g::g_strength_leases59c17.load(),
        (unsigned long long)g::g_strength_consumed59c17.load(),(unsigned long long)g::g_answered_seen59c17.load(),(unsigned long long)g::g_answered_skips59c17.load(),
        (unsigned long long)g::g_combo_flags59c17.load(),(unsigned long long)g::g_combo_edges19.load(),(unsigned long long)g::g_combo_same_value19.load(),(unsigned long long)g::g_combo_unknown_edge19.load(),
        (unsigned long long)cb::g_deferred.load(),(unsigned long long)cb::g_extended.load(),(unsigned long long)cb::g_kept_open.load(),
        (unsigned long long)cb::g_late_resets.load(),cb::g_ready.load()?1:0,
        (unsigned long long)sv::strikesNative59c17.load(),(unsigned long long)sv::strikesScaled59c17.load(),(unsigned long long)sv::strikesZeroed59c17.load(),
        (unsigned long long)sv::ownScaled59c17.load(),(unsigned long long)g::vrstate596::g_stamina_fallbacks59c17.load(),
        double(sv::lastFactor59c17.load()),double(sv::lastNative59c17.load()),double(sv::lastScaled59c17.load()));
    namespace vs=g::vrstate596;const std::size_t used=std::strlen(out); // C10R18 F4 press 4 (VR DIRECT)
    if(used+1<cap)std::snprintf(out+used,cap-used," vrDirect{offers=%llu followupRefused=%llu contactsRefreshed=%llu hits=%llu blocked=%llu perfect=%llu npcRising128=%llu npcUp256=%llu telegraph=%d blockSpeed=%.2f ownsHits=%d nativeTimingStrikes=%llu followupInWrite=%llu followupAfterHit=%llu}",
        (unsigned long long)vs::g_forward_offers18.load(),(unsigned long long)vs::g_followup_suppressed18.load(),(unsigned long long)vs::g_contact_refreshed18.load(),
        (unsigned long long)vs::g_direct_hits18.load(),(unsigned long long)vs::g_direct_blocked18.load(),(unsigned long long)vs::g_direct_perfect18.load(),
        (unsigned long long)vs::g_direct_vs128_18.load(),(unsigned long long)vs::g_direct_vs256_18.load(),vs::g_telegraph_strike59c10.load()?1:0,
        double(vs::g_npc_block_speed18.load()),vs::g_own_hits18.load()?1:0,(unsigned long long)vs::g_native_clock_attempts19.load(),
        (unsigned long long)vs::g_followup_in_write19.load(),(unsigned long long)vs::g_followup_after_hit19.load());
}
// One effective policy for config and UI: extension OFF dominates the test
// preset. The session timer fault also cannot be bypassed by either control.
static bool ComboHitFlagWantedDsF6() noexcept {
    return nhp134::combo59c17::WindowMsH3()>0;
}
void SetComboWindowExtension(bool on) noexcept {
    const bool changed=nhp134::combo59c17::g_window_extend.exchange(on,std::memory_order_acq_rel)!=on;
    if(changed)nhp134::combo59c17::ClearLineage19();
    nhp134::geom27::g_combo_flag59c17.store(ComboHitFlagWantedDsF6(),std::memory_order_relaxed);
    if(changed)diagnostic_trace::Write(on?"R25 COMBO_WINDOW extension=ON owner=EXISTING_QUALIFIED_HENRY":"R25 COMBO_WINDOW extension=OFF nativePhases=UNCHANGED");
}
bool ComboWindowExtension() noexcept {return nhp134::combo59c17::g_window_extend.load(std::memory_order_acquire);}
unsigned ComboCandidatesCZ(const void* actor,kcd2vr::combo_zone_assist_cz::NativeCandidate* out,unsigned max) noexcept { // I12
    namespace dc=kcd2vr::diagnostics::combo;
    const auto base=g_combo_base6691.load(std::memory_order_acquire);
    const auto expected=reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity());
    std::uint64_t manager=0;
    if(!actor||!out||!max||!base||!expected||!ComboRead6691(reinterpret_cast<std::uintptr_t>(actor)+0x3B0,&manager,8)||!manager)return 0;
    auto reader=ComboRead6691;
    const auto s=dc::Capture(reader,dc::Layout{base},manager,expected);
    if(s.status!=dc::Status::Ok||s.truncated||s.identity.actor!=reinterpret_cast<std::uintptr_t>(actor))return 0;
    unsigned n=0;
    for(unsigned i=0;i<s.candidatesCopied&&n<max;++i){const auto& c=s.candidates[i];
        if(c.state>=s.statesCopied)continue;
        auto& o=out[n++];o={};std::memcpy(o.technique,c.techniqueRaw.data(),16);o.cursor=s.states[c.state].cursor;o.required=c.requiredSteps;
        if(c.nameExact)for(unsigned k=0;k+1<sizeof(o.name)&&c.name[k];++k)o.name[k]=c.name[k];}
    return n;
}
void SetCombatFlow59c17(bool intentLive,unsigned intentLiveMs,unsigned comboWindowMs) noexcept { // R59.16C10R17
    if(nhp134::geom27::g_intent_live59c17.load()!=intentLive||nhp134::geom27::g_intent_live_ms59c17.load()!=std::min(intentLiveMs,120u)||
       nhp134::combo59c17::g_window_ms.load()!=std::min(comboWindowMs,3000u))nhp134::combo59c17::ClearLineage19();
    nhp134::geom27::g_intent_live59c17.store(intentLive,std::memory_order_relaxed);
    nhp134::geom27::g_intent_live_ms59c17.store(std::min(intentLiveMs,120u),std::memory_order_relaxed); // contacts expire at 160 ms
    const unsigned window=std::min(comboWindowMs,3000u);nhp134::combo59c17::g_window_ms.store(window,std::memory_order_relaxed);
    nhp134::combo59c17::g_window_ms_wanted96.store(window,std::memory_order_relaxed); // R96: restored after a combo-timer fault on the next load
    const bool hitFlagGate=ComboHitFlagWantedDsF6();
    nhp134::geom27::g_combo_flag59c17.store(hitFlagGate,std::memory_order_relaxed);
    char m[220]{};std::snprintf(m,sizeof(m),"R59.16C10R17 COMBAT_FLOW intentLiveRoute=%d intentLiveWaitMs=%u comboWindowMs=%u comboHitFlag=%d",intentLive?1:0,std::min(intentLiveMs,120u),window,hitFlagGate?1:0);
    diagnostic_trace::Write(m);}
// DS-F6: VrComboTestForce - TEST window hold + hit-flag gate + C69 receipts.
// Changing the toggle retires the current combo lineage; the game still decides
// recipe matching, advance and the finisher. The setter writes no native state;
// the qualified callback owns timer rearming while extension is enabled.
void SetComboTestForce(bool on) noexcept {
    const bool changed=kcd2vr::combo_test_ds::force.exchange(on,std::memory_order_acq_rel)!=on;
    if(changed)nhp134::combo59c17::ClearLineage19();
    const bool hitFlagGate=ComboHitFlagWantedDsF6();
    nhp134::geom27::g_combo_flag59c17.store(hitFlagGate,std::memory_order_relaxed);
    if(!changed)return;
    char m[220]{};std::snprintf(m,sizeof(m),"DS-F6 COMBO_TEST force=%d windowMs=%u hitFlagGate=%d result=RECEIPTS_AND_WINDOW_HOLD_ONLY",
        on?1:0,kcd2vr::combo_test_ds::kWindowMs,hitFlagGate?1:0);
    diagnostic_trace::Write(m);
}
bool ComboTestForce() noexcept {return kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed);}
#include "subsystems/direct/public_hit_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/gore/public_contact_control.inl" // M2_OWNED_SOURCE
void NoteImpactBodyMaterial59c7e(std::uint64_t sample,int material) noexcept {nhp134::geom27::NoteBodyMaterial59c7e(sample,material);}
void NoteImpactPower59c8(std::uint64_t sample,float ratio,bool twoHanded) noexcept {nhp134::geom27::NotePower59c8(sample,ratio,twoHanded);} // R59.16C8
void SetHitPairFlag59c9(bool) noexcept {} // retired unproven request-byte experiment; old INIs cannot revive it
#include "subsystems/masterstrike_combos/public_window_controls.inl" // M2_OWNED_SOURCE
void SetStrikePower59c8(bool enabled,float twoHandedBonus,float minPower,float maxPower,unsigned repeatMs,bool ripostePressNative,unsigned riposteWindowMs) noexcept {
    namespace pw=nhp134::geom27::power59c8;namespace rp=nhp134::geom27::riposte59c8;
    pw::g_enabled.store(enabled);pw::g_two_handed.store(std::clamp(twoHandedBonus,1.f,2.f));pw::g_min.store(std::clamp(minPower,.05f,1.f));pw::g_max.store(std::clamp(maxPower,.5f,2.f));
    pw::g_repeat_ms.store(std::clamp(repeatMs,200u,10000u));rp::g_native.store(false);(void)ripostePressNative;rp::g_window_ms.store(std::clamp(riposteWindowMs,100u,3000u));
    char m[260]{};std::snprintf(m,sizeof(m),"R59.16C8 STRIKE_POWER enabled=%d base=0.30+0.70*E/Eref clamp=[%.2f,%.2f] twoHanded=x%.2f repeatWindowMs=%u ripostePress=%s windowMs=%u (strength arg + AttackStrength; native RPG/armour unchanged)",
        enabled?1:0,pw::g_min.load(),pw::g_max.load(),pw::g_two_handed.load(),pw::g_repeat_ms.load(),"DIRECT_x1.0(no native press)",rp::g_window_ms.load());diagnostic_trace::Write(m);}
void SetGeometryAttackTypes178r27(int cut,int thrust) noexcept {nhp134::geom27::g_type_cut.store(cut);nhp134::geom27::g_type_thrust.store(thrust);}
bool GeometryHitSubmit178r27(std::uint32_t entityId,const float pos[3],float energy,int kind,int part,int surface,std::uint64_t tick,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture59c3,const contact59c7g::Metadata* metadata) noexcept {
    kcd2vr::paired_signals_d62c::NoteVrContact(entityId,tick); // D62C: our blade touched this entity (only kept while a Henry pair is live)
    if(!nhp134::geom27::Enabled())return false;nhp134::geom27::Pending p{};p.entityId=entityId;std::memcpy(p.pos,pos,sizeof(p.pos));p.energy=energy;p.kind=kind;p.part=part;p.surface=surface;if(metadata){p.metadata59c7g=*metadata;p.bodyMaterial=metadata->bodyMaterial;}(void)nhp134::geom27::TakePower59c8(sample,p.powerRatio59c8,p.twoHanded59c8);p.tick=tick;p.key=key;p.generation=generation;p.epoch=epoch;p.request=request;p.sample=sample;p.gesture59c3=gesture59c3;return nhp134::geom27::Submit(p);}
bool GeometryWeaponContactSubmit178r43(std::uint32_t itemEntityId,const float pos[3],float energy,int kind,int part,int surface,std::uint64_t tick,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture59c3) noexcept {
    // Item ownership and part mapping are unproven. Preserve the API for callers,
    // but do not enqueue an item as damage against an arbitrary current opponent.
    return false;}

void SetGeometryApply178r30(bool on) noexcept {nhp134::geom27::g_apply178r30.store(on,std::memory_order_relaxed);}
void GeometryHitSummary178r27() noexcept {nhp134::geom27::Summary();}
#include "subsystems/hybrid/public_target_control.inl" // M2_OWNED_SOURCE
bool GeometryLockClick178r29(std::uint64_t now) noexcept {return nhp134::geom27::LockClickPending(now);}
int HenryInCombat178r31(std::uint64_t now) noexcept {return nhp134::geom27::HenryInCombat178r31(now);}
bool NativeBlockIndicatorH24(std::uint64_t now) noexcept {
    kcd2vr::hmd_dodge_h24::BlockContext c{};
    return retail_build112::Ready(nhp134::g_base)&&frik_pose_probe::BlockIndicatorGameplayH24(now)&&
        kcd2vr::hmd_dodge_h24::blockPublished.read(c)&&
        kcd2vr::hmd_dodge_h24::blockCurrent(c,reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),
            frik_pose_probe::PoseEpochH24(),now)&&c.maximum!=0;
}
bool IncomingHmdDodgeH24(std::uint64_t now) noexcept {
    kcd2vr::hmd_dodge_h24::Context c{};
    return kcd2vr::hmd_dodge_h24::published.read(c)&&kcd2vr::hmd_dodge_h24::current(c,
        reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),frik_pose_probe::PoseEpochH24(),now);
}
bool EngagedHmdDodgeD74(std::uint64_t now) noexcept {
    if(!retail_build112::Ready(nhp134::g_base))return false;
    kcd2vr::hmd_dodge_h24::Context c{};
    return kcd2vr::hmd_dodge_h24::published.read(c)&&kcd2vr::hmd_dodge_h24::currentEngaged(c,
        reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),frik_pose_probe::PoseEpochH24(),now);
}
void SetGeometryEngageNative178r31(bool on) noexcept {nhp134::geom27::g_engage_native178r31.store(on,std::memory_order_relaxed);}
bool GeometryEngageNative178r31() noexcept {return nhp134::geom27::g_engage_native178r31.load(std::memory_order_relaxed);}
#include "subsystems/hybrid/public_sweep_control.inl" // M2_OWNED_SOURCE
void SetGeometryContactTime178r46(bool on,int) noexcept {
    if(on)diagnostic_trace::Write("V66.178R46C1 GEOM_CONTACT_TIME requested=1 enabled=0 reason=UNVERIFIED_PHASE_WRITE_RETIRED nativeWindow=REQUIRED (R50C2: see VrMotionAttackClock)");
}
void SetGeometryHold178r39(unsigned ms,int phase) noexcept {nhp134::geom27::g_hold_ms178r39.store(ms,std::memory_order_relaxed);(void)phase;nhp134::geom27::g_strike_phase178r39.store(8,std::memory_order_relaxed);}
#include "subsystems/direct/public_context_controls.inl" // M2_OWNED_SOURCE
void SetContactImpulse596(bool on,float scale,float capNs,float massKg) noexcept {namespace i=nhp134::geom27::impulse596;
    // The saved preference may remain ON, but native corpse impulses are F9 rung3 only.
    on=on&&kcd2vr::collab_h21::npcPhysics();
    // Deliver still requires exact native death/current-body proof.
    i::g_scale.store(scale);i::g_cap.store(capNs);i::g_mass.store(massKg);if(i::g_enabled.exchange(on)!=on)++i::g_session;
    diagnostic_trace::Write(on?"C4R4 CONTACT_IMPULSE=1 existingArticulatedOnly=1 noPoint=1 maxRequestedImpulse=menu(<=80Ns) perBodyMs=500 globalCVarWrites=0":"C4R4 CONTACT_IMPULSE=0");}
void SetContactImpulseDiagnostics19c4r4(bool on) noexcept {namespace i=nhp134::geom27::impulse596;if(i::g_observe.exchange(on)!=on)++i::g_session;}
bool ContactImpulse596() noexcept {namespace i=nhp134::geom27::impulse596;return i::g_enabled.load()||i::g_observe.load();}
void ContactImpulseRequest596(std::uint32_t entity,int part,int body,const float pos[3],const float normal[3],float speed,std::uint64_t sourceTick,std::uintptr_t collider,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {nhp134::geom27::impulse596::Request(entity,part,body,pos,normal,speed,sourceTick,collider,key,generation,epoch,request);}
// C61: use the same native death and outcome ledgers as the existing impulse evaluator.
unsigned LabDeathCandidates58(std::uint32_t* out,unsigned cap,std::uint64_t now) noexcept {return nhp134::geom27::impulse596::DeathCandidates58(out,cap,now);}
bool LabCorpseQualified58(std::uint32_t entity,std::uintptr_t identity,std::uint64_t epoch,std::uint64_t now,const char*& why) noexcept {return nhp134::geom27::impulse596::CorpseQualified58(entity,identity,epoch,now,why);}
unsigned LabTakeAnsweredLiving58(LabAnsweredLiving58* out,unsigned cap) noexcept {
    nhp134::geom27::impulse596::AnsweredLiving58 tmp[8]{};
    const unsigned n=nhp134::geom27::impulse596::TakeAnsweredLiving58(tmp,std::min(cap,8u));
    for(unsigned i=0;i<n;++i)out[i]={tmp[i].entity,tmp[i].part,tmp[i].sample,tmp[i].tick,tmp[i].serial,tmp[i].reaction};
    return n;
}
#include "subsystems/direct/public_retired_fallback.inl" // M2_OWNED_SOURCE
void SetContactNeverLost595(bool) noexcept {} // Retired: no stale/rebound contact authority.
#include "subsystems/hybrid/public_native_route_controls.inl" // M2_OWNED_SOURCE
void SetGeometryOuter178r33(bool on) noexcept {nhp134::geom27::g_outer178r33.store(on,std::memory_order_relaxed);}
void NativeOwnerLatch178r33(std::uint64_t key,std::uint64_t until) noexcept {nhp134::geom27::NativeOwnerLatch178r33(key,until);}
bool NativeOwned178r33(std::uint64_t now) noexcept {return nhp134::geom27::NativeOwned178r33(now);}
void SetVrWeaponHitLogging136(bool on) noexcept {nhp134::SetVrWeaponLogging(on);}
void Uninstall() noexcept {if(g_close_query164){(void)g_close_query164->disable();g_close_query164.reset();}nhp134::Uninstall();ShutdownContactPath122();g_contact_observer115=nullptr;g_hook_base115=0;g_enabled.store(false,std::memory_order_release);UninstallCombo6691();g_character_entry63.clear();if(g_character_hook6690){(void)g_character_hook6690->disable();g_character_hook6690.reset();}g_character_original6690=nullptr;if(g_enrich_hook6689){(void)g_enrich_hook6689->disable();g_enrich_hook6689.reset();}g_enrich_original6689=nullptr;g_enabled.store(false);if(g_hook){(void)g_hook->disable();g_hook.reset();}g_original=nullptr;}
bool RegisterContactObserver115(ContactObserver115 observer) noexcept {
    if(!observer||!Install())return false;
    const auto b=g_hook_base115;std::uintptr_t method=0;
    if(!ReadCopy6689(reinterpret_cast<void*>(b+0x3B912F0),&method,sizeof(method))||method!=b+kOnCollisionRva)return false;
    ContactObserver115 empty=nullptr;
    return g_contact_observer115.compare_exchange_strong(empty,observer,std::memory_order_acq_rel)||empty==observer;
}
void ClearContactObserver115(ContactObserver115 observer) noexcept {
    (void)g_contact_observer115.compare_exchange_strong(observer,nullptr,std::memory_order_acq_rel);
}
void SetEnabled(bool e) noexcept {const bool old=g_enabled.exchange(e,std::memory_order_acq_rel);if(old==e)return;if(e)ComboStatus6691();if(old&&!e)nhp134::Summary();if(old!=e)diagnostic_trace::Write(e?"V66.89-F3 ON native enrichment observer (no physical response)":"V66.89-F3 OFF");}
std::uint32_t ReadinessMask168r1() noexcept {
    return (g_hook&&g_original?1u:0u)|(g_enrich_hook6689&&g_enrich_original6689?2u:0u)|
        (g_character_hook6690&&g_character_original6690?4u:0u)|
        (g_combo_step_hook6691&&g_combo_reset_hook6691?8u:0u)|
        (nhp134::g_process&&nhp134::g_process_original?16u:0u)|
        (nhp134::g_submit&&nhp134::g_submit_original?32u:0u)|
        (nhp134::g_update&&nhp134::g_update_original?64u:0u)|
        (nhp134::g_phys&&nhp134::g_phys_original?128u:0u)|
        (g_close_query164?256u:0u)|(g_path_ready122.load()?512u:0u);
}
bool Enabled() noexcept{return (g_enabled.load(std::memory_order_acquire)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()));}std::uint64_t CollisionCalls() noexcept{return g_calls.load(std::memory_order_relaxed);} }
