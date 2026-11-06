#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // I10/NF native kill animations switch
#include "activity_input163.h"
#include "vrik/alchemy_input19c4.h" // R8b: C71 F5_CRAFT_ANIMATION alchemyStage=
#include "core/native_door_start19c4.h"
#include "animation_filter.h"
#include "subsystems/ranged_reload/crossbow_clock_entry_d87.h"
#include "vrik/riding_vrik_d76.h"
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "subsystems/interactions/native_passenger_a34.h"
#include "frik_pose_probe.h"
#include "diagnostics/interaction_cycle_h4.h"
#include "diagnostics/hook_cost178r22.h"
#include "vrik/body_context115.h"
#include "vrik/left_holding_b2.h"
#include "vrik/body_context126.h"
#include "vrik/kick_style26.h" // C26 kick style
#include "vrik/visible_failure.h" // CL-E: one in-game line when a hit cuts the kick short
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include "diagnostics/combat_capture.h"
#include "diagnostics/lab_state.h"
#include "diagnostics/playback_snapshot.h"
#include "diagnostics/reload_milestones136r3.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include "core/readable_pages178r22c1.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>
#include <mutex>
#include <vector>
#include "diagnostics/animation_alias_catalog.h"
#include "vrik/vr_combat_ownership111.h"
#include "vrik/finisher_upper_d83.h" // D83 finisher

namespace kcd2vr::animation_filter {
namespace {
constexpr std::uint32_t  kSupportedWhTimestamp      = 0x6A350E20u;
constexpr std::uintptr_t kStartAnimationNameRva     = 0xAA7FA0ull;
constexpr std::uintptr_t kCCharInstanceVftRva       = 0x40602F8ull;
constexpr std::uintptr_t kCSkeletonAnimVftRva       = 0x3A711E0ull;
constexpr std::uintptr_t kCDefaultSkeletonVftRva    = 0x406B930ull;
constexpr std::ptrdiff_t kSkeletonAnimOffset        = 0x110;
constexpr std::ptrdiff_t kPoseDefaultSkeletonOffset = 0x190;

// Exact current-retail prologue of WHGame+0xAA7FA0. This is the name-based
// CSkeletonAnim start wrapper: RDX is the animation name; it resolves the name
// to an animation id and dispatches to the adjacent id-based virtual method.
constexpr std::array<unsigned char,16> kStartPrologue{
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57
};

using StartAnimationNameFn = bool(__fastcall*)(void*, const char*, void*);
using StartAnimationIdFn = bool(__fastcall*)(void*, int, void*);
std::optional<DetourModKit::hook::Hook> g_id_hook;
StartAnimationIdFn g_id_original=nullptr;
// These are exact retail contracts, verified against the packaged DLL fingerprints.
constexpr std::uintptr_t kStartAnimationIdRva=0xAA8514ull;
constexpr std::uintptr_t kAnimationIdLookupRva=0xAA805Cull;
constexpr std::uintptr_t kAnimationByIdRva=0x8417F4ull;
constexpr std::uintptr_t kAnimationEntryNameRva=0xAA90F8ull;
bool Readable(const void*,std::size_t) noexcept;
static bool CopyName6689(const char* src,char* dst,std::size_t cap) noexcept {
    if(!src||!dst||cap<2)return false;
    // One checked region normally covers the entire bounded name. Retain the
    // byte-checked fallback at region boundaries and SEH for a concurrent unmap.
    const bool whole=Readable(src,cap-1);
    __try { for(std::size_t i=0;i<cap-1;++i){ if(!whole&&!Readable(src+i,1))return false;
        dst[i]=src[i];if(!dst[i])return true; }dst[cap-1]=0;return false; }
    __except(EXCEPTION_EXECUTE_HANDLER){dst[0]=0;return false;}
}

std::optional<DetourModKit::hook::Hook> g_hook;
StartAnimationNameFn g_original=nullptr;
std::atomic<bool> g_ready{false};
std::atomic<int> g_mode{0};
std::atomic<void*> g_player_skeleton_anim{nullptr};
std::atomic<std::uint64_t> g_suppressed{0};
std::atomic<std::uint64_t> g_passed_player{0};
std::atomic<std::uint64_t> g_last_validate_log_ms{0};
std::atomic<std::uint64_t> g_last_suppress_log_ms{0};
std::atomic<int> g_start_diag_level{0};
std::atomic<void*> g_interaction_target_anim6689{nullptr};
std::atomic<std::uint32_t> g_interaction_target_id6689{0};
std::atomic<std::uint64_t> g_interaction_sequence6689{0};
std::atomic<std::uint64_t> g_last_start_diag_ms{0};
std::atomic<std::uint32_t> g_last_start_diag_hash{0};
std::atomic<bool> g_dialogue_camera_suppression{false};
std::atomic<std::uint64_t> g_dialogue_camera_suppressed{0};
std::atomic<std::uint64_t> g_native_interaction_handoff_until_ms{0};
std::atomic<std::uint32_t> g_native_interaction_handoff_hash{0};
std::atomic<int> g_recent_clenched_side{-1};
std::atomic<std::uint64_t> g_recent_clenched_start_ms{0};
std::atomic<int> g_physical_map_reading_request{0};
std::atomic<std::uint64_t> g_physical_map_reading_request_ms{0};
std::atomic<std::uint64_t> g_physical_map_reading_requests{0};
std::atomic<std::uint64_t> g_physical_map_reading_starts{0};

std::uintptr_t WhBase() noexcept {
    static std::atomic<std::uintptr_t> pinned{0};
    if(const auto base=pinned.load(std::memory_order_acquire))return base;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN,L"WHGame.dll",&module)||!module)return 0;
    const auto base=reinterpret_cast<std::uintptr_t>(module);
    pinned.store(base,std::memory_order_release);return base;
}

bool Readable(const void* p,std::size_t n) noexcept {
    return memory126::readableNow178r22c1(p,n);
}
template<class T> bool Read(const void* p,T& out) noexcept {
    if(!Readable(p,sizeof(T)))return false;
    __try {std::memcpy(&out,p,sizeof(T));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

bool ExactBuild(std::uintptr_t base) noexcept {
    if(!base)return false;
    const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base); if(!Readable(dos,sizeof(*dos))||dos->e_magic!=IMAGE_DOS_SIGNATURE)return false;
    const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+static_cast<std::uintptr_t>(dos->e_lfanew));
    return Readable(nt,sizeof(*nt))&&nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
}
bool MatchBytes(const void* p,const unsigned char* bytes,std::size_t n) noexcept { return Readable(p,n)&&std::memcmp(p,bytes,n)==0; }
bool MatchVft(void* obj,std::uintptr_t expected) noexcept { std::uintptr_t v=0; return obj&&Read(obj,v)&&v==expected; }

#include "diagnostics/playback_fingerprint.inl"

bool Eq(const char* a,const char* b) noexcept { return a&&b&&_stricmp(a,b)==0; }

template<std::size_t N> bool In(const char* name,const std::array<const char*,N>& names) noexcept {
    for(const char* s:names)if(Eq(name,s))return true; return false;
}

// These names come from the current-retail kcd_male_combat_database.adb, not
// from the older first-person mod. We suppress only the visual CAF start on the
// validated Henry CSkeletonAnim object; Mannequin/procedural combat stays alive.
constexpr std::array<const char*,5> kFistsIdle{
    "combat_attack_lg_z2_nwpn_add",
    "combat_attack_rg_z1_nwpn_add",
    "combat_lg_sz2_idle_nwpn_player",
    "combat_rg_sz1_idle_nwpn_player",
    "combat_rg_sz5_idle_nwpn_player",
};
constexpr std::array<const char*,12> kUnarmed{
    "1d-combat_player_fatkst_nwpn",
    "1d-combat_player_fatksw_nwpn",
    "combat_attack_lg_z2_nwpn_add",
    "combat_attack_rg_z1_nwpn_add",
    "combat_fatksw_nw_idle_player_over",
    "combat_fatksw_nw_start_player_over",
    "combat_free_blk_nw_player_over",
    "combat_lg_sz2_idle_nwpn_player",
    "combat_rg_sz1_idle_nwpn_player",
    "combat_rg_sz5_idle_nwpn_player",
    "aimposes_combat_rg_default",
    "aimposes_combat_lg_default",
};
constexpr std::array<const char*,20> kSword{
    "1d-combat_player_fatkst_lngsw",
    "1d-combat_player_fatksw_lngsw",
    "1d-combat_player_fatkst_shsw",
    "1d-combat_player_fatksw_shsw",
    "combat_fatksw_lngsw_idle_player_over",
    "combat_fatksw_lngsw_start_player_over",
    "combat_fatksw_shsw_idle_player_over",
    "combat_fatksw_shsw_start_player_over",
    "combat_free_blk_lngsw_player_over",
    "combat_free_blk_shsw_player_over",
    "combat_lg_sz0_idle_lngsw_player",
    "combat_lg_sz2_idle_lngsw_player",
    "combat_rg_sz1_idle_lngsw_player",
    "combat_rg_sz5_idle_lngsw_player",
    "combat_lg_sz0_idle_shsw_player",
    "combat_lg_sz2_idle_shsw_player",
    "combat_rg_sz1_idle_shsw_player",
    "combat_rg_sz5_idle_shsw_player",
    "aimposes_combat_rg_default",
    "aimposes_combat_lg_default",
};


// Exact current-retail animation names which the supplied
// FirstPersonDialogsAndMercy ADB replaces with empty references. This list is
// generated from the supplied current animations package, not copied from the
// stale mod file. F5 only; normal gameplay never uses this filter.
constexpr std::array<const char*,27> kDialogueMercyCameraAnimations{
    "3d_ad_cameras_base","3d_ad_cameras_female_base","ad_cam_base_bench_add",
    "ad_cam_base_bench_drunk_add","ad_cam_female_base_bench_add",
    "behavior_cooking_pan_camp_player","behavior_cooking_pan_kitchen_player",
    "behavior_drying_player","behavior_smokehouse_sniff_player",
    "combat_finish_axe_01","combat_finish_axe_shld_01","combat_finish_lngsw_01",
    "combat_finish_lngsw_02","combat_finish_nw_break_neck_01",
    "combat_finish_shsw_01","combat_finish_shsw_02","combat_finish_shsw_shld_01",
    "grave_digging","herbs_picking_area_in","herbs_picking_area_loop",
    "herbs_picking_area_out","laundry_wash_player_03","quest_collection_carcasses",
    "quest_pick_magic_arrow_fail","quest_pick_magic_arrow_shovel",
    "quest_pick_magic_arrow_succes","short_butchering"
};


std::uint32_t HashName(const char* s) noexcept {
    std::uint32_t h=2166136261u; if(!s)return h;
    for(int i=0;i<192 && s[i];++i){ h^=static_cast<unsigned char>(s[i]); h*=16777619u; }
    return h;
}
const char* ClassifyName(const char* name) noexcept {
    if(!name)return "UNKNOWN";
    const bool unarmed=std::strstr(name,"nwpn")!=nullptr || std::strstr(name,"_nw_")!=nullptr;
    const bool longsword=std::strstr(name,"lngsw")!=nullptr;
    const bool shortsword=std::strstr(name,"shsw")!=nullptr;
    const bool block=std::strstr(name,"blk")!=nullptr;
    const bool attack=std::strstr(name,"attack")!=nullptr || std::strstr(name,"fatk")!=nullptr;
    const bool idle=std::strstr(name,"idle")!=nullptr;
    const bool kick=std::strstr(name,"kick")!=nullptr;
    const bool bow=std::strstr(name,"bow")!=nullptr || std::strstr(name,"arrow")!=nullptr;
    const bool crossbow=std::strstr(name,"crossbow")!=nullptr || std::strstr(name,"xbow")!=nullptr;
    const bool interact=std::strstr(name,"door")!=nullptr || std::strstr(name,"grab")!=nullptr ||
                        std::strstr(name,"pick")!=nullptr || std::strstr(name,"eat")!=nullptr ||
                        std::strstr(name,"drink")!=nullptr || std::strstr(name,"soup")!=nullptr;
    if(kick)return "KICK";
    if(crossbow)return attack?"CROSSBOW_ATTACK":"CROSSBOW";
    if(bow)return attack?"BOW_ATTACK":"BOW";
    if(interact)return "INTERACTION";
    if(unarmed&&block)return "UNARMED_BLOCK";
    if(unarmed&&attack)return "UNARMED_ATTACK";
    if(unarmed&&idle)return "UNARMED_IDLE";
    if(longsword&&block)return "LONGSWORD_BLOCK";
    if(longsword&&attack)return "LONGSWORD_ATTACK";
    if(longsword&&idle)return "LONGSWORD_IDLE";
    if(shortsword&&block)return "SHORTSWORD_BLOCK";
    if(shortsword&&attack)return "SHORTSWORD_ATTACK";
    if(shortsword&&idle)return "SHORTSWORD_IDLE";
    if(block)return "BLOCK"; if(attack)return "ATTACK"; if(idle)return "IDLE";
    if(std::strstr(name,"aimposes"))return "AIMPOSE";
    return "OTHER";
}

bool ShouldSuppress(const char* name,SuppressionMode mode,int layer=-1) noexcept {
    switch(mode){
        case SuppressionMode::FistsIdle:return In(name,kFistsIdle);
        case SuppressionMode::UnarmedCombat:return In(name,kUnarmed);
        case SuppressionMode::SwordCombat:return In(name,kSword);
        case SuppressionMode::AllSelectedCombat:return vr_combat111::SuppressPlayerMeleeVisual(name,layer);
        default:return false;
    }
}

static void LogSuppressed111(void* self,const char* name,SuppressionMode mode,const char* route,int id=-1,int layer=-1) noexcept {
    const auto n=g_suppressed.fetch_add(1,std::memory_order_relaxed)+1;
    if(!diagnostics::combat_capture::active(std::uint64_t(GetTickCount64())))return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=g_last_suppress_log_ms.load(std::memory_order_relaxed);
    if(n<=20 || (now-prev>=1000ull&&g_last_suppress_log_ms.compare_exchange_strong(prev,now,std::memory_order_acq_rel))) {
        char m[640]{}; std::snprintf(m,sizeof(m),
            "V66.111-ANIM SUPPRESS route=%s mode=%d(%s) playerAnim=%p id=%d layer=%d name='%s' count=%llu nativeStateWrites=NONE cafEventDelivery=NOT_CERTIFIED returnSuccess=1",
            route?route:"?",static_cast<int>(mode),ModeName(mode),self,id,layer,name?name:"",(unsigned long long)n); diagnostic_trace::Write(m);
    }
}

static void* AnimationSet6689(void* self) noexcept {
    void* character=nullptr;void* skeleton=nullptr;void* set=nullptr;
    if(!MatchVft(self,WhBase()+kCSkeletonAnimVftRva)||
       !Read(static_cast<unsigned char*>(self)+0x4E8,character)||!character||
       !Read(static_cast<unsigned char*>(character)+0xA70,skeleton)||!skeleton||
       !Read(static_cast<unsigned char*>(skeleton)+0x218,set))return nullptr;
    return set;
}
static int ResolveReferenceId6689(void* set,const char* name) noexcept {
    __try {return reinterpret_cast<int(__fastcall*)(void*,const char*)>(WhBase()+kAnimationIdLookupRva)(set,name);}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
// Retail GetNameByAnimID consults the alias dictionary before the optional CAF path.
// Calling +AA90F8 on an entry bypassed that dictionary and produced the literal
// "CAF filepath disabled" for most of the user's recordings.
// Immutable after publication. Native hashing is pure string work and is performed
// incrementally by the existing worker, never while copying an animation FIFO.
struct AliasCandidate6694 {std::uint64_t hash=0;const char* name=nullptr;};
static std::vector<AliasCandidate6694> g_alias_catalog6694;
static std::atomic<bool> g_alias_ready6694{false};
static std::atomic<bool> g_alias_fault6694{false};
static std::size_t g_alias_cursor6694=0;
static bool NativeAliasHash6694(const char* name,std::uint64_t& hash) noexcept {
    const auto b=WhBase();constexpr unsigned char sig[]{0x48,0x89,0x5c,0x24,0x08,0x89,0x54,0x24,0x10,0x55};
    if(!b||!MatchBytes(reinterpret_cast<void*>(b+0x6A3014),sig,sizeof(sig)))return false;
    __try {hash=reinterpret_cast<std::uint64_t(__fastcall*)(const char*,std::uint32_t)>(b+0x6A3014)(name,static_cast<std::uint32_t>(std::strlen(name)));return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void BuildAliasCatalog6694(bool motionRequired) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const bool recording=g_start_diag_level.load()>0||diagnostics::combat_capture::active(now)||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now);
    if(g_alias_ready6694.load()||g_alias_fault6694.load()||(!motionRequired&&!recording))return;
    try {
        const std::size_t count=sizeof(animation_aliases::names)/sizeof(animation_aliases::names[0]);
        if(g_alias_cursor6694==0){
            g_alias_catalog6694.reserve(count);
            if(recording)diagnostic_trace::Write(motionRequired?"V66.125R1 ALIAS CATALOG_BUILD demand=PHYSICAL_OWNERSHIP budget=64_PER_WORKER_TICK":
                "V66.125R1 ALIAS CATALOG_BUILD demand=DIAGNOSTIC budget=64_PER_WORKER_TICK");
        }
        for(int budget=0;budget<64&&g_alias_cursor6694<count;++budget){
            const char* name=animation_aliases::names[g_alias_cursor6694];std::uint64_t hash=0;
            if(!NativeAliasHash6694(name,hash)){g_alias_fault6694.store(true);if(recording)diagnostic_trace::Write("V66.94-ALIAS CATALOG_UNAVAILABLE nativeHashFaultOrMismatch=1 idsPreserved=1");return;}
            g_alias_catalog6694.push_back({hash,name});++g_alias_cursor6694;
        }
        if(g_alias_cursor6694==count){
            std::sort(g_alias_catalog6694.begin(),g_alias_catalog6694.end(),[](const auto&a,const auto&b){return a.hash<b.hash;});
            // A collision is deliberately unresolved, not arbitrarily the first name.
            for(std::size_t i=0;i<g_alias_catalog6694.size();){std::size_t j=i+1;while(j<g_alias_catalog6694.size()&&g_alias_catalog6694[j].hash==g_alias_catalog6694[i].hash)++j;
                if(j-i>1)for(std::size_t k=i;k<j;++k)g_alias_catalog6694[k].name=nullptr;i=j;}
            g_alias_ready6694.store(true,std::memory_order_release);
            if(recording)diagnostic_trace::Write("V66.94-ALIAS CATALOG_READY basis=SUPPLIED_VANILLA_ADB nativeHash=6A3014 ambiguity=UNRESOLVED noActiveClipInference=1");
        }
    }catch(...){g_alias_fault6694.store(true);}
}
static bool AliasHashForId113(void* set,int id,std::uint64_t& hash) noexcept {
    hash=0;void* entries=nullptr;std::uint32_t count=0;
    return set&&id>=0&&Read(static_cast<unsigned char*>(set)+8,entries)&&entries&&
       Read(static_cast<unsigned char*>(entries)-4,count)&&count<=2000000&&static_cast<unsigned>(id)<count&&
       Read(static_cast<unsigned char*>(entries)+std::size_t(id)*16,hash);
}
static bool CatalogName6694(void* set,int id,char* out,std::size_t cap) noexcept {
    std::uint64_t hash=0;
    if(!g_alias_ready6694.load(std::memory_order_acquire)||!AliasHashForId113(set,id,hash))return false;
    const auto it=std::lower_bound(g_alias_catalog6694.begin(),g_alias_catalog6694.end(),hash,[](const auto&a,std::uint64_t h){return a.hash<h;});
    if(it==g_alias_catalog6694.end()||it->hash!=hash||!it->name)return false;
    std::snprintf(out,cap,"%s",it->name);return true;
}
static bool NameForId6689(void* self,int id,char* name,std::size_t cap) noexcept {
    if(!name||cap<2||id<0)return false;name[0]=0;
    void* set=AnimationSet6689(self);if(!set)return false;
    std::uintptr_t vt=0,fn=0;
    if(Read(set,vt)&&vt==WhBase()+0x406BB38ull&&Read(reinterpret_cast<void*>(vt+0x38),fn)&&fn==WhBase()+0x37A0554ull){
        __try {
            const char* n=reinterpret_cast<const char*(__fastcall*)(void*,int)>(fn)(set,id);
            if(CopyName6689(n,name,cap)&&name[0]&&std::strcmp(name,"CAF filepath disabled")!=0&&
               !std::strstr(name,"Invalid")&&!std::strstr(name,"invalid")&&!std::strstr(name,"not found"))return true;
        }__except(EXCEPTION_EXECUTE_HANDLER){name[0]=0;}
    }
    name[0]=0;return CatalogName6694(set,id,name,cap);
}

static bool __fastcall StartAnimationIdDetour6689(void* self,int id,void* params) noexcept {
    const bool henry=self&&self==g_player_skeleton_anim.load(std::memory_order_acquire);
    int layer=-1;if(params)(void)Read(static_cast<unsigned char*>(params)+0x10,layer);
    char name[192]{};
    const auto mode=static_cast<SuppressionMode>(g_mode.load(std::memory_order_acquire));
    // V66.111: the older hook observed this route only after the native enqueue.
    // Resolve first and, for exact Henry + F3=2 visual ownership, return success
    // without enqueueing the authored melee pose. No native combat state fields are
    // written here. CAF events and action completion still require runtime proof
    // for that legacy policy. C51R1 policy 3 always calls the original enqueue:
    // the final tracked pose may replace visuals, never this event carrier.
    const auto lab=diagnostics::lab::Snapshot();
    const bool ownership=retail_build112::Ready(WhBase())&&vr_combat111::OwnershipReady112(diagnostics::lab::Get(lab,diagnostics::lab::Key::F3),
        diagnostics::lab::Get(lab,diagnostics::lab::Key::F5)==1,henry,GetTickCount64(),combatPoseTick112.load(std::memory_order_acquire));
    if(ownership&&vr_combat111::SuppressEnqueue114(diagnostics::lab::Get(lab,diagnostics::lab::Key::F3))&&mode==SuppressionMode::AllSelectedCombat&&NameForId6689(self,id,name,sizeof(name))&&ShouldSuppress(name,mode,layer)&&!NativeBodyRequired112(self)){
        LogSuppressed111(self,name,mode,"ID",id,layer);
        if(diagnostics::combat_capture::active(std::uint64_t(GetTickCount64()))){
            diagnostics::combat_capture::Event e{};e.kind=diagnostics::combat_capture::Kind::Clip;e.tick=GetTickCount64();e.actor=reinterpret_cast<std::uintptr_t>(self);e.id=id;e.layer=layer;e.flags=0x101;
            std::snprintf(e.name,sizeof(e.name),"%s",name);diagnostics::combat_capture::push(e);
        }
        return true;
    }

    const bool ok=g_id_original?g_id_original(self,id,params):false;
    if(ok)door_start19c4::started(reinterpret_cast<std::uintptr_t>(self),layer,id,GetTickCount64());
    if(ok&&henry&&diagnostics::combat_capture::active(std::uint64_t(GetTickCount64()))){
        diagnostics::combat_capture::Event e{};e.kind=diagnostics::combat_capture::Kind::Clip;e.tick=GetTickCount64();e.actor=reinterpret_cast<std::uintptr_t>(self);e.id=id;e.flags=1;e.layer=layer;
        (void)AliasHashForId113(AnimationSet6689(self),id,e.aliasHash);
        if(e.layer==0||e.layer==8||(diagnostics::combat_capture::sourceMask.load()&3u)){if(!name[0]&&!NameForId6689(self,id,name,sizeof(name)))std::snprintf(name,sizeof(name),"<UNRESOLVED_ID_%d>",id);std::snprintf(e.name,sizeof(e.name),"%s",name);diagnostics::combat_capture::push(e);}
    }
    const bool target=self==g_interaction_target_anim6689.load(std::memory_order_acquire);
    const auto diagNow175=std::uint64_t(GetTickCount64());
    if((henry||target)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,diagNow175)){
        if(!name[0]&&!NameForId6689(self,id,name,sizeof(name)))std::snprintf(name,sizeof(name),"<UNRESOLVED_ID_%d>",id);
        namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::Clip;e.tick=diagNow175;e.entity=reinterpret_cast<std::uintptr_t>(self);e.target=target?g_interaction_target_id6689.load():0;e.actionSeq=target?g_interaction_sequence6689.load():0;e.code=ok?1:0;e.detail=id;e.value=float(layer);e.flags=target?1u:0u;
        h4::copy(e.reason,sizeof(e.reason),"ORIGINAL_ID_START_RETURN_CURRENT_WATCH_ATTRIBUTION_UNVERIFIED");h4::copy(e.label,sizeof(e.label),name[0]?name:"<ID_ONLY_NAME_NOT_ALREADY_RESOLVED>");h4::observe(e,diagNow175);}
    if(!ok||(!henry&&!target)||(g_start_diag_level.load()<=0&&!diagnostics::combat_capture::active(diagNow175)&&
       !diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,diagNow175)))return ok;
    if(!name[0]&&!NameForId6689(self,id,name,sizeof(name)))std::snprintf(name,sizeof(name),"<UNRESOLVED_ID_%d>",id);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());const auto hash=HashName(name)^std::uint32_t(layer);
    const auto old=g_last_start_diag_hash.exchange(hash);
    if(hash==old&&now-g_last_start_diag_ms.load()<500)return ok;
    g_last_start_diag_ms.store(now);
    char m[512]{};std::snprintf(m,sizeof(m),"V66.89-ANIM-START success=1 role=%s requestTargetId=%u requestSeq=%llu actionAttribution=UNVERIFIED id=%d layer=%d name='%s' action=%s activeWeight=UNCLAIMED poseWrites=NONE",henry?"HENRY":"INTERACTION_OBJECT",target?g_interaction_target_id6689.load():0u,(unsigned long long)(target?g_interaction_sequence6689.load():0ull),id,layer,name,ClassifyName(name));diagnostic_trace::Write(m);
    return ok;
}

bool __fastcall StartAnimationDetour(void* self,const char* name,void* params) noexcept {
    if(g_dialogue_camera_suppression.load(std::memory_order_acquire) && name && Readable(name,1) && In(name,kDialogueMercyCameraAnimations)) {
        const auto n=g_dialogue_camera_suppressed.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=16 || (n%100ull)==0){ char m[352]{}; std::snprintf(m,sizeof(m),
            "V65.03-F5 ADB-RUNTIME-SUPPRESS n=%llu self=%p name='%.*s' result=successful-no-enqueue",
            (unsigned long long)n,self,180,name); diagnostic_trace::Write(m); }
        return true;
    }
    const void* player=g_player_skeleton_anim.load(std::memory_order_acquire);
    const auto mode=static_cast<SuppressionMode>(g_mode.load(std::memory_order_acquire));
    if(player&&self==player) {
        g_passed_player.fetch_add(1,std::memory_order_relaxed);
        // V66.69 F4: consume a queued standing-reading request only on Henry's
        // native animation thread and reuse KCD2's live start-parameter object.
        // Stale requests fail closed; this never calls animation systems from the worker.
        const auto nowMap=static_cast<std::uint64_t>(GetTickCount64());
        const auto reqMs=g_physical_map_reading_request_ms.load(std::memory_order_acquire);
        int mapReq=g_physical_map_reading_request.load(std::memory_order_acquire);
        if(mapReq>0 && reqMs && nowMap>=reqMs && nowMap-reqMs<=1500ull && g_original){
            int expected=mapReq;
            if(g_physical_map_reading_request.compare_exchange_strong(expected,0,std::memory_order_acq_rel)){
                const char* clip=mapReq==1?"reading_stand_inv_in":(mapReq==2?"reading_stand_inv_loop":"reading_stand_inv_out");
                bool started=false; __try{started=g_original(self,clip,params);}__except(EXCEPTION_EXECUTE_HANDLER){started=false;}
                if(started)g_physical_map_reading_starts.fetch_add(1,std::memory_order_relaxed);
                char rm[384]{};std::snprintf(rm,sizeof(rm),
                    "V66.69-F4-MAP-READ phase=%d clip=%s nativeThread=1 params=%p result=%s requests=%llu starts=%llu",
                    mapReq,clip,params,started?"STARTED":"FAILED",
                    (unsigned long long)g_physical_map_reading_requests.load(),
                    (unsigned long long)g_physical_map_reading_starts.load());diagnostic_trace::Write(rm);
            }
        } else if(mapReq>0 && reqMs && nowMap>reqMs+1500ull){
            g_physical_map_reading_request.store(0,std::memory_order_release);
            diagnostic_trace::Write("V66.69-F4-MAP-READ request expired; no validated Henry StartAnimation callback -> fail closed");
        }
        // V66.89 observation is read-only. No request-time donor promotion and
        // no 1.8-second whole-body handoff on a name heuristic. ID observation below
        // logs only successfully enqueued animations; pose qualification uses FIFO.
        int layer=-1;if(params)(void)Read(static_cast<unsigned char*>(params)+0x10,layer);
        const auto lab=diagnostics::lab::Snapshot();
        const bool ownership=retail_build112::Ready(WhBase())&&vr_combat111::OwnershipReady112(diagnostics::lab::Get(lab,diagnostics::lab::Key::F3),
            diagnostics::lab::Get(lab,diagnostics::lab::Key::F5)==1,true,GetTickCount64(),combatPoseTick112.load(std::memory_order_acquire));
        char boundedName[192]{};
        if(mode!=SuppressionMode::Off && (mode!=SuppressionMode::AllSelectedCombat||(ownership&&vr_combat111::SuppressEnqueue114(diagnostics::lab::Get(lab,diagnostics::lab::Key::F3)))) &&
           CopyName6689(name,boundedName,sizeof(boundedName)) && ShouldSuppress(boundedName,mode,layer) &&
           (mode!=SuppressionMode::AllSelectedCombat||!NativeBodyRequired112(self))) {
            LogSuppressed111(self,boundedName,mode,"NAME",-1,layer);
            // Mannequin sees success while this exact Henry visual pose is not
            // enqueued. Native state fields are not written; action timing remains unverified.
            return true;
        }
    }
    const bool ok=g_original?g_original(self,name,params):false;
    if(ok){int doorLayer=-1;if(params)(void)Read(static_cast<unsigned char*>(params)+0x10,doorLayer);
        door_start19c4::started(reinterpret_cast<std::uintptr_t>(self),doorLayer,-1,GetTickCount64());}
    if(ok&&player&&self==player&&diagnostics::combat_capture::active(std::uint64_t(GetTickCount64()))){
        diagnostics::combat_capture::Event e{};e.kind=diagnostics::combat_capture::Kind::Clip;e.tick=GetTickCount64();e.actor=reinterpret_cast<std::uintptr_t>(self);e.flags=2;
        if(params)(void)Read(static_cast<unsigned char*>(params)+0x10,e.layer);
        if((e.layer==0||e.layer==8||(diagnostics::combat_capture::sourceMask.load()&3u))&&CopyName6689(name,e.name,sizeof(e.name)))diagnostics::combat_capture::push(e);
    }
    return ok;
}

} // namespace

#include "diagnostics/playback_probe.inl"

bool Install() noexcept {
    Uninstall(); const auto b=WhBase(); if(!ExactBuild(b))return false;
    void* target=reinterpret_cast<void*>(b+kStartAnimationNameRva);
    if(!MatchBytes(target,kStartPrologue.data(),kStartPrologue.size())) {
        diagnostic_trace::Write("V64.10-ANIM StartAnimation signature mismatch; runtime suppression disabled fail-closed"); return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6410_skeletonanim_start_name",DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(target)}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&StartAnimationDetour); if(!h)return false;
    g_hook=std::move(*h); g_original=g_hook->original<StartAnimationNameFn>();
    if(!g_original||!g_hook->enable()){g_original=nullptr;g_hook.reset();return false;}
    constexpr unsigned char idSignature[]{0x48,0x89,0x5C,0x24,0x20,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
    void* idTarget=reinterpret_cast<void*>(b+kStartAnimationIdRva);
    if(!MatchBytes(idTarget,idSignature,sizeof(idSignature))){Uninstall();return false;}
    DetourModKit::hook::InlineRequest idRequest{"kcd2vr_v6689_skeletonanim_start_id",DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(idTarget)}},DetourModKit::hook::Options{}};
    auto ih=DetourModKit::hook::inline_at(std::move(idRequest),&StartAnimationIdDetour6689);
    if(!ih){Uninstall();return false;}g_id_hook=std::move(*ih);g_id_original=g_id_hook->original<StartAnimationIdFn>();
    if(!g_id_original||!g_id_hook->enable()){Uninstall();return false;}
    g_playback_layout_ready6690.store(ValidatePlaybackLayout6690(b));
    diagnostic_trace::Write(g_playback_layout_ready6690.load()?"V66.90-PLAYBACK READY exact queue-layout fingerprints matched; reference10Hz/live4Hz attempts; bounded2Hz/1Hz snapshots; per-clip weights UNKNOWN":"V66.90-PLAYBACK BLOCKED queue-layout fingerprint mismatch; baseline animation hooks unchanged");
    g_ready.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.89-ANIM exact name + ID hooks READY; native handoff timers disabled; FIFO reference observation available");
    return true;
}
void Uninstall() noexcept {
    combatPoseTick112.store(0,std::memory_order_release);
    nativeKickActive175.store(false,std::memory_order_release);
    g_playback_layout_ready6690.store(false,std::memory_order_release);
    g_interaction_target_anim6689.store(nullptr,std::memory_order_release);g_interaction_target_id6689.store(0);g_interaction_sequence6689.store(0);
    g_dialogue_camera_suppression.store(false,std::memory_order_release);
    g_native_interaction_handoff_until_ms.store(0,std::memory_order_release);
    g_native_interaction_handoff_hash.store(0,std::memory_order_release);
    g_recent_clenched_side.store(-1,std::memory_order_release);
    g_recent_clenched_start_ms.store(0,std::memory_order_release);
    g_physical_map_reading_request.store(0,std::memory_order_release);
    g_physical_map_reading_request_ms.store(0,std::memory_order_release);
    g_start_diag_level.store(0,std::memory_order_release);
    g_mode.store(0,std::memory_order_release); g_player_skeleton_anim.store(nullptr,std::memory_order_release); g_ready.store(false,std::memory_order_release);
    if(g_id_hook){(void)g_id_hook->disable();g_id_hook.reset();}g_id_original=nullptr;
    if(g_hook){(void)g_hook->disable();g_hook.reset();} g_original=nullptr;
}
void SetMode(SuppressionMode mode) noexcept {
    const int m=std::clamp(static_cast<int>(mode),0,4); g_mode.store(m,std::memory_order_release);
    char line[224]{}; std::snprintf(line,sizeof(line),"V64.10-ANIM mode=%d %s playerValidated=%d suppressed=%llu",m,ModeName(static_cast<SuppressionMode>(m)),PlayerAnimationObjectValidated()?1:0,(unsigned long long)SuppressedCount()); diagnostic_trace::Write(line);
}
SuppressionMode Mode() noexcept { return static_cast<SuppressionMode>(g_mode.load(std::memory_order_acquire)); }
const char* ModeName(SuppressionMode mode) noexcept {
    switch(mode){case SuppressionMode::Off:return "OFF/PASS-THROUGH";case SuppressionMode::FistsIdle:return "FISTS+IDLE";case SuppressionMode::UnarmedCombat:return "UNARMED COMBAT";case SuppressionMode::SwordCombat:return "SWORD COMBAT";case SuppressionMode::AllSelectedCombat:return "ALL SELECTED COMBAT";default:return "UNKNOWN";}
}
void ObservePlayerSkinning(void* pose,void* skeleton_owner,void* char_plus_8) noexcept {
    if(!g_ready.load(std::memory_order_acquire)||!pose||!skeleton_owner||!char_plus_8){g_player_skeleton_anim.store(nullptr,std::memory_order_release);nativeKickActive175=false;return;}
    const auto b=WhBase(); if(!ExactBuild(b))return;
    auto* ci=static_cast<unsigned char*>(char_plus_8)-8;
    auto* sa=ci+kSkeletonAnimOffset;
    void* poseSkel=nullptr; const bool poseLink=Read(static_cast<unsigned char*>(pose)+kPoseDefaultSkeletonOffset,poseSkel)&&poseSkel==skeleton_owner;
    const bool ok=poseLink&&MatchVft(ci,b+kCCharInstanceVftRva)&&MatchVft(sa,b+kCSkeletonAnimVftRva)&&MatchVft(skeleton_owner,b+kCDefaultSkeletonVftRva);
    g_player_skeleton_anim.store(ok?sa:nullptr,std::memory_order_release);
    if(!ok)nativeKickActive175=false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=g_last_validate_log_ms.load(std::memory_order_relaxed);
    if(diagnostics::combat_capture::active(now)&&now-prev>=10000ull&&g_last_validate_log_ms.compare_exchange_strong(prev,now,std::memory_order_acq_rel)){
        char m[512]{}; std::snprintf(m,sizeof(m),
            "V64.10-ANIM PLAYER-LINK pose=%p skel=%p char=%p skelAnim=%p pose+190Match=%d charVft=%d animVft=%d skelVft=%d VALID=%d",
            pose,skeleton_owner,ci,sa,poseLink?1:0,MatchVft(ci,b+kCCharInstanceVftRva)?1:0,MatchVft(sa,b+kCSkeletonAnimVftRva)?1:0,MatchVft(skeleton_owner,b+kCDefaultSkeletonVftRva)?1:0,ok?1:0); diagnostic_trace::Write(m);
    }
}
bool Ready() noexcept { return g_ready.load(std::memory_order_acquire); }
bool PlayerAnimationObjectValidated() noexcept { return g_player_skeleton_anim.load(std::memory_order_acquire)!=nullptr; }
// R59.16C10R11: Henry's validated CSkeletonAnim for the action time owner (nullptr until the skinning link validates).
void* PlayerSkeletonAnim() noexcept { return g_player_skeleton_anim.load(std::memory_order_acquire); }
std::uint64_t SuppressedCount() noexcept { return g_suppressed.load(std::memory_order_relaxed); }
void ServiceAliasCatalog(bool motionRequired) noexcept { BuildAliasCatalog6694(motionRequired); }

// Sparse evidence for every fail-closed reason; never change ownership because
// a diagnostic name/lookup is unavailable. Layer-qualified asset exceptions only.
static bool BodyDecision115(bool native,const char* reason,int layer=-1,int id=-1,const char* name="-") noexcept {
    static std::atomic<std::uint64_t> dueByOwner30c1[2]{};auto& next=dueByOwner30c1[native?1:0];
    const auto now=std::uint64_t(GetTickCount64());auto due=next.load();
    if(diagnostics::ownership48::active(now)){
        static thread_local bool have48=false,prior48=false;static thread_local std::uint64_t last48=0;static thread_local std::uint32_t hash48=0;
        const auto signature48=HashName(name)^HashName(reason)^unsigned(layer+1);
        if(!have48||native!=prior48||signature48!=hash48||now<last48||now-last48>=100){
            diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::Body;e.tick=now;e.thread=GetCurrentThreadId();
            e.actor=reinterpret_cast<std::uintptr_t>(g_player_skeleton_anim.load());e.epoch=kcd2vr::frik_pose_probe::PoseEpochH24();
            e.code=layer;e.flags=native?1u:0u;e.item=unsigned(id);e.v[0]=float(diagnostics::lab::Get(diagnostics::lab::Key::F3));
            char detail[128]{};std::snprintf(detail,sizeof(detail),"%s:%s",reason,name);diagnostics::ownership48::name(e,detail);(void)diagnostics::ownership48::push(e);
            have48=true;prior48=native;hash48=signature48;last48=now;
        }
    }

    vr_combat111::clinchInputTick55c1.store(native&&layer==0&&!std::strcmp(reason,"PROTECTED_ACTION")&&
        vr_combat111::ClinchAlias55c1(name)?now:0,std::memory_order_release);
    if(native)nativeKickActive175.store(false,std::memory_order_release);
    // Diagnostic alias readback/formatting is optional; gameplay decisions and
    // the clinch/kick updates above remain independent of recording.
    if(diagnostics::capture168::accept("V66.178R30C1 BODY_GATE ",now)&&
       now>=due&&next.compare_exchange_strong(due,now+2000)){
        std::uint64_t hash=0;bool hashKnown=false;
        if(id>=0&&!std::strcmp(reason,"UNKNOWN_ALIAS")){
            void* set=AnimationSet6689(g_player_skeleton_anim.load(std::memory_order_acquire));
            hashKnown=MatchVft(set,WhBase()+0x406BB38ull)&&AliasHashForId113(set,id,hash);
        }
        char m[448]{};std::snprintf(m,sizeof(m),"V66.178R30C1 BODY_GATE native=%d reason=%s layer=%d id=%d alias=%s unknown=FAIL_CLOSED aliasHashKnown=%d aliasHash=%016llX catalogReady=%d catalogFault=%d",
            native?1:0,reason,layer,id,name,hashKnown,(unsigned long long)hash,g_alias_ready6694.load()?1:0,g_alias_fault6694.load()?1:0);
        diagnostic_trace::Write(m);
    }
    return native;
}
static bool NativePassengerBodyA34(void* anim) noexcept {
    return anim&&anim==g_player_skeleton_anim.load(std::memory_order_acquire)&&
        kcd2vr::native_passenger_a34::bodyCurrent(reinterpret_cast<std::uintptr_t>(anim)-kSkeletonAnimOffset,
            kcd2vr::frik_pose_probe::PoseEpochH24(),std::uint64_t(GetTickCount64()));
}
// D51 changes body ownership only for the selected native backup. The baseline
// protected list and its riding/facial exceptions remain unchanged. All callers
// below have validated Henry and inspect an active FIFO entry on its real layer.
static bool NativeBodyForLayerD51(const char* name,int layer) noexcept {
    const auto anim=g_player_skeleton_anim.load(std::memory_order_acquire);
    if(anim&&kcd2vr::native_passenger_a34::trackedWashAlias(name,layer,
       reinterpret_cast<std::uintptr_t>(anim)-kSkeletonAnimOffset,kcd2vr::frik_pose_probe::PoseEpochH24(),std::uint64_t(GetTickCount64())))return false;
    return vr_combat111::NativeBodyAlias112(name)||kcd2vr::native_kill_nf::BackupBodyAliasD51(name,layer);
}
// NF2: qualified kill observation, independent of which protected entry InspectNativeBody112 returns first.
// scans EVERY live FIFO entry of Henry's validated anim for a protected (native-body) kill alias; any read failure =
// the occurrence is unknown and is retired. only runs while the switch is on.
static void ObserveKillNF2(void* anim) noexcept {
    namespace nk=kcd2vr::native_kill_nf;
    const auto gen=nk::BeginScan();if(!gen)return; // NF3: generation captured BEFORE the scan
    const auto now=std::uint64_t(GetTickCount64());
    for(int layer=0;layer<=8;++layer){
        unsigned char* entries=nullptr;std::uint32_t count=0;
        if(!Read(static_cast<unsigned char*>(anim)+layer*0x40+0x48,entries)){nk::Observe(gen,0,false,-1,-1,now);return;}
        if(!entries)continue;
        if(!Read(entries-4,count)||count>16){nk::Observe(gen,0,false,-1,-1,now);return;}
        for(std::uint32_t i=0;i<count;++i){
            std::uint16_t state=0;int id=-1;
            if(!Read(entries+i*0x78+0x50,state)||!Read(entries+i*0x78+0x10,id)){nk::Observe(gen,0,false,-1,-1,now);return;}
            if(!(state&1))continue;
            char name[192]{};
            if(!NameForId6689(anim,id,name,sizeof(name)))continue; // unknown alias: not a qualified kill
            if((nk::KillAlias(name)||nk::BackupBodyAliasD51(name,layer))&&NativeBodyForLayerD51(name,layer)&&!vr_combat111::SharedHoldingIdle115(name,layer)&&
               !vr_combat111::LeftHoldingIdleB2(name,layer)&&!vr_combat111::BodylessFacial126(name,layer)){
                nk::Observe(gen,reinterpret_cast<std::uintptr_t>(anim),true,layer,id,now);return;}
        }
    }
    nk::Observe(gen,reinterpret_cast<std::uintptr_t>(anim),false,-1,-1,now);
}
// Short state update only; native FIFO reads run outside this lock. All body
// consumers share one completed-mount deadline, including callers without an
// optional riding output pointer.
static std::mutex g_riding_mutex_d81;
static riding_vrik_d76::SessionD81 g_riding_session_d81;
static bool ObserveRidingD81(void* anim,std::uint64_t policy,bool qualified,
                            bool mount=false,bool dismount=false,bool gait=false,bool protectedAction=false,std::uint64_t* lifetimeR95=nullptr) noexcept {
    if(!anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire))return false;
    std::lock_guard lock(g_riding_mutex_d81);
    const bool admitted=riding_vrik_d76::AdvanceD81(g_riding_session_d81,
        {reinterpret_cast<std::uintptr_t>(anim),policy,std::uint64_t(GetTickCount64()),
         qualified,mount,dismount,gait,protectedAction,kcd2vr::frik_pose_probe::PoseEpochH24(),
         kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)});
    if(lifetimeR95)*lifetimeR95=admitted?riding_vrik_d76::lifetimeR95.load(std::memory_order_acquire):0;
    return admitted;
}
// D83 finisher: result of the last body inspection on THIS thread (the skinning callback reads it
// right after NativeBodyRequired112). Side is set only when every protected clip on Henry is a
// finisher-type paired clip (masterstrike / combo ending / kill), never for clinches.
static thread_local unsigned t_finisherSideD83=0;
static thread_local char t_finisherAliasD83[192]{};
unsigned FinisherUpperSideD83(const char** alias) noexcept {
    if(alias)*alias=t_finisherAliasD83;
    return t_finisherSideD83;
}
static bool InspectNativeBody112(void* anim,bool* ridingGaitOnlyD76=nullptr,std::uint64_t* ridingLifetimeR95=nullptr) noexcept {
    if(ridingGaitOnlyD76)*ridingGaitOnlyD76=false;
    t_finisherSideD83=0;t_finisherAliasD83[0]=0;
    unsigned finisherMaskD83=0;bool otherProtectedD83=false; // D83 finisher
    const auto ridingModeD76=riding_vrik_d76::mode.load(std::memory_order_acquire);
    const bool inspectRidingD76=(ridingModeD76&1)!=0;
    bool ridingFoundD76=false,ridingBlockedD76=false,mountD81=false,dismountD81=false,seatedSeenR98=false;
    bool getupR96=false;int getupLayerR96=-1,getupIdR96=-1;char getupNameR96[64]{};
    int protectedLayerD81=-1,protectedIdD81=-1;char protectedNameD81[192]{};
    const auto invalidD81=[&](const char* why,int layer=-1,int id=-1){
        (void)ObserveRidingD81(anim,ridingModeD76,false);
        return BodyDecision115(true,why,layer,id);
    };
    hookcost178r22::Scope cost{hookcost178r22::AnimationBody};
    if(!retail_build112::Ready(WhBase())){kcd2vr::native_kill_nf::Retire();return invalidD81("RETAIL_SHA");}
    if(!anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire)||!MatchVft(anim,WhBase()+kCSkeletonAnimVftRva)){kcd2vr::native_kill_nf::Retire();return invalidD81("ANIM_IDENTITY");}
    if(NativePassengerBodyA34(anim)){kcd2vr::native_kill_nf::Retire();return invalidD81("NATIVE_PASSENGER_A34");}
    if(!g_playback_layout_ready6690.load(std::memory_order_acquire)){kcd2vr::native_kill_nf::Retire();return invalidD81("FIFO_LAYOUT");}
    ObserveKillNF2(anim); // NF2: before the first-owner return below
    bool kick175=false;int kickId175=-1,kickLayer175=-1;
    for(int layer=0;layer<=8;++layer){
        unsigned char* entries=nullptr;std::uint32_t count=0;
        if(!Read(static_cast<unsigned char*>(anim)+layer*0x40+0x48,entries))return invalidD81("FIFO_READ",layer);
        if(!entries)continue;
        if(!Read(entries-4,count)||count>16)return invalidD81("FIFO_COUNT",layer);
        for(std::uint32_t i=0;i<count;++i){
            std::uint16_t state=0;int id=-1;
            if(!Read(entries+i*0x78+0x50,state)||!Read(entries+i*0x78+0x10,id))return invalidD81("ENTRY_READ",layer);
            if(!(state&1))continue;
            char name[192]{};
            if(!NameForId6689(anim,id,name,sizeof(name)))return invalidD81("UNKNOWN_ALIAS",layer,id);
            mountD81|=riding_vrik_d76::MountD81(name);
            dismountD81|=riding_vrik_d76::DismountD81(name);
            if(!getupR96&&riding_vrik_d76::GetupR96(name)){getupR96=true;getupLayerR96=layer;getupIdR96=id;std::snprintf(getupNameR96,sizeof(getupNameR96),"%s",name);}
            const bool freeKickR24=(!std::strcmp(name,"1d-combat_player_fatkst_nwpn.bspace")||
                !std::strcmp(name,"1d-combat_player_fatkst_nwpn_torch.bspace"))&&
                kcd2vr::frik_pose_probe::NativeFreeKickVisualR24();
            if(vr_combat111::NativeKickAlias175(name)||freeKickR24){kick175=true;kickId175=id;kickLayer175=layer;}
            if(vr_combat111::HitReactionAlias178r31(name))kcd2vr::kick26::lastHitReactionCLE.store(std::uint64_t(GetTickCount64()),std::memory_order_relaxed);
            // R95: exact seated gait/idle/combat aliases share the mounted frame.
            // The native clips and combat semantics continue unchanged. Scan
            // every layer: mount/dismount and unrelated protected owners win.
            const bool ordinaryRiderR92=riding_vrik_d76::SeatedR95(name,layer);
            seatedSeenR98|=ordinaryRiderR92;
            if(inspectRidingD76&&ordinaryRiderR92){ridingFoundD76=true;continue;}
            // Paired/death/finisher ownership stays native until the actual
            // paired alignment boundary is proven. Catalog identity and an
            // OFF preference alone cannot surrender that protected body owner.
            // This exact facial clip has only 107 leaf control tracks in the
            // supplied DBA. Keep it playing; it does not own arm/root motion.
            if((NativeBodyForLayerD51(name,layer)||ordinaryRiderR92)&&!vr_combat111::SharedHoldingIdle115(name,layer)&&!vr_combat111::LeftHoldingIdleB2(name,layer)&&
               !vr_combat111::BodylessFacial126(name,layer)){
                // D81: complete the layer scan before recognizing mount completion.
                // Gait overlapping mount/alignment cannot start a seat lifetime (R97: no extra wait).
                if(layer==0&&vr_combat111::hideNativeClipsActive178r28.load(std::memory_order_relaxed)&&
                   vr_combat111::attackConnectKeepsVrik50.load(std::memory_order_relaxed)&&vr_combat111::OwnAttackConnectAlias50(name)){
                    static std::atomic<std::uint64_t> last50{0};const auto now50=std::uint64_t(GetTickCount64());auto l50=last50.load();
                    if(now50-l50>=1000&&last50.compare_exchange_strong(l50,now50)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R50 ATTACK_CONNECT_VRIK_KEPT layer=%d id=%d alias=%s (Henry's hit-connect clip plays under VR arms)",layer,id,name);diagnostic_trace::Write(m);}
                    continue;}
                if(vr_combat111::hitReactionsKeepVrik178r31.load(std::memory_order_relaxed)&&vr_combat111::HitReactionAlias178r31(name)){
                    static std::atomic<std::uint64_t> last{0};const auto now=std::uint64_t(GetTickCount64());auto l=last.load();
                    if(now-l>=1000&&last.compare_exchange_strong(l,now)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R31 HIT_REACTION_VRIK_KEPT layer=%d id=%d alias=%s (flinch plays under VR arms; no native body handoff)",layer,id,name);diagnostic_trace::Write(m);}
                    continue;}
                ridingBlockedD76=true;
                {const auto sideD83=kcd2vr::finisher_upper_d83::Classify(name,layer); // D83 finisher
                 if(sideD83==kcd2vr::finisher_upper_d83::None)otherProtectedD83=true;else{finisherMaskD83|=unsigned(sideD83);
                     if(!t_finisherAliasD83[0])std::snprintf(t_finisherAliasD83,sizeof(t_finisherAliasD83),"%s",name);}}
                if(protectedLayerD81<0){protectedLayerD81=layer;protectedIdD81=id;
                    std::snprintf(protectedNameD81,sizeof(protectedNameD81),"%s",name);}
            }
        }
    }
    if(mountD81||dismountD81||seatedSeenR98) // R98: body height COMBINED rider evidence (time-bounded)
        riding_vrik_d76::riderSeenTickR98.store(std::uint64_t(GetTickCount64()),std::memory_order_release);
    const bool mountedVrikD81=ObserveRidingD81(anim,ridingModeD76,
        ridingModeD76==riding_vrik_d76::mode.load(std::memory_order_acquire),
        mountD81,dismountD81,ridingFoundD76,ridingBlockedD76,ridingLifetimeR95);
    if(mountD81||dismountD81)return BodyDecision115(true,"RIDING_NATIVE_TRANSFER_R95");
    if(getupR96)return BodyDecision115(true,"NATIVE_GETUP_R96",getupLayerR96,getupIdR96,getupNameR96);
    if(ridingBlockedD76){
        // D83 finisher: body stays native (same gate/events); the pose callback may overlay VR head+upper torso.
        // Mixed master+slave or any non-finisher protected clip (clinch, death...) keeps the old whole-body native.
        if(!otherProtectedD83&&(finisherMaskD83==1u||finisherMaskD83==2u))t_finisherSideD83=finisherMaskD83;
        else t_finisherAliasD83[0]=0;
        return BodyDecision115(true,"PROTECTED_ACTION",protectedLayerD81,protectedIdD81,protectedNameD81);
    }
    const bool priorKick175=nativeKickActive175.exchange(kick175,std::memory_order_acq_rel);
    if(kick175&&!priorKick175)kcd2vr::kick26::nativeClipStart.store(std::uint64_t(GetTickCount64()),std::memory_order_release); // C26
    if(!kick175&&priorKick175){ // CL-E: the game replaced the kick with a hit reaction before the foot landed
        const auto nowE=std::uint64_t(GetTickCount64()),hitE=kcd2vr::kick26::lastHitReactionCLE.load(std::memory_order_relaxed),
            startE=kcd2vr::kick26::nativeClipStart.load(std::memory_order_acquire);
        if(hitE&&hitE>=startE&&nowE>=startE&&nowE-startE<std::uint64_t(kcd2vr::kick26::reachMs.load(std::memory_order_relaxed)))
            (void)kcd2vr::visible_failure::publish("kick: cut short by a hit reaction",nowE);
    }
    if(kick175!=priorKick175&&diagnostics::combat_capture::active(std::uint64_t(GetTickCount64()))){
        diagnostics::combat_capture::Event e{};e.kind=diagnostics::combat_capture::Kind::State;e.tick=GetTickCount64();
        e.actor=reinterpret_cast<std::uintptr_t>(anim);e.id=kickId175;e.layer=kickLayer175;e.flags=kick175?1u:0u;
        diagnostics::combat_capture::name(e,kick175?"NATIVE_KICK_ACTIVE lowerBody=NATIVE upperBody=VR hit=UNCLAIMED":"NATIVE_KICK_END lowerBody=CONFIGURED noTimer=1");
        diagnostics::combat_capture::push(e);
    }
    if(ridingFoundD76){
        if(ridingGaitOnlyD76)*ridingGaitOnlyD76=mountedVrikD81;
        return BodyDecision115(!mountedVrikD81,mountedVrikD81?"RIDING_NORMAL_VRIK_D81":"RIDING_AFTER_MOUNT_WAIT_D81");
    }
    return BodyDecision115(false,"QUALIFIED_LIVE_POSE");
}

bool NativeBodyRequired112(void* anim,bool* ridingGaitOnlyD76,std::uint64_t* ridingLifetimeR95) noexcept {
    if(ridingLifetimeR95)*ridingLifetimeR95=0;
    if(ridingGaitOnlyD76)*ridingGaitOnlyD76=false;
    t_finisherSideD83=0;t_finisherAliasD83[0]=0; // D83 finisher
    if(NativePassengerBodyA34(anim)){(void)ObserveRidingD81(anim,riding_vrik_d76::mode.load(),false);kcd2vr::native_kill_nf::Retire();return BodyDecision115(true,"NATIVE_PASSENGER_A34");}
    if(!vr_combat111::MotionOwnsPlayerCombat175(diagnostics::lab::Get(diagnostics::lab::Key::F3))){
        (void)ObserveRidingD81(anim,riding_vrik_d76::mode.load(std::memory_order_acquire),false);
        nativeKickActive175=false;kcd2vr::native_kill_nf::Retire();return false;}
    return InspectNativeBody112(anim,ridingGaitOnlyD76,ridingLifetimeR95);
}

bool PlayerNativeBodyRequired112() noexcept {return NativeBodyRequired112(g_player_skeleton_anim.load(std::memory_order_acquire));}
bool ReloadPassengerOnlyXR34(void* anim,std::uintptr_t expected,int animation,bool inertTransitionD87) noexcept {
    if(!expected||animation<0||!retail_build112::Ready(WhBase())||
       !anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire)||
       !MatchVft(anim,WhBase()+kCSkeletonAnimVftRva)||NativePassengerBodyA34(anim)||
       !g_playback_layout_ready6690.load(std::memory_order_acquire))return false;
    bool found=false;
    for(int layer=0;layer<=8;++layer){
        unsigned char* entries=nullptr;std::uint32_t count=0;
        if(!Read(static_cast<unsigned char*>(anim)+layer*0x40+0x48,entries))return false;
        if(!entries)continue;
        if(!Read(entries-4,count)||count>16)return false;
        for(std::uint32_t i=0;i<count;++i){const auto at=entries+i*0x78;
            std::uint16_t state=0;int id=-1;
            if(!Read(at+0x50,state)||!Read(at+0x10,id))return false;
            if(!(state&1))continue;
            if(reinterpret_cast<std::uintptr_t>(at)==expected&&id==animation){
                if(found)return false;found=true;continue;
            }
            // Only the exact same event-free transition may leave a terminal
            // FIFO predecessor. Other reload/paired/hit entries stay protected.
            const auto readEntryD87=[](std::uintptr_t p,void* out,std::size_t n) noexcept {
                if(!Readable(reinterpret_cast<void*>(p),n))return false;
                __try {std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
                __except(EXCEPTION_EXECUTE_HANDLER){return false;}
            };
            if(inertTransitionD87&&id==animation&&
               kcd2vr::crossbow_clock_entry_d87::inertTerminal(readEntryD87,reinterpret_cast<std::uintptr_t>(at)))continue;
            char name[192]{};
            if(!NameForId6689(anim,id,name,sizeof(name)))return false;
            // A concurrent protected action cannot borrow the reload's narrow
            // working hand. In particular no R31 hit/paired exception applies.
            if(NativeBodyForLayerD51(name,layer)&&
               !vr_combat111::SharedHoldingIdle115(name,layer)&&
               !vr_combat111::LeftHoldingIdleB2(name,layer)&&
               !vr_combat111::BodylessFacial126(name,layer))return false;
        }
    }
    return found;
}
bool AdmittedNativeClipOnlyD72(void* anim,std::uintptr_t entry,int animation) noexcept {
    return ReloadPassengerOnlyXR34(anim,entry,animation);
}
// F12=7 is selectable with F3=0. Its native trajectory write must still yield
// to hit reactions/paired actions. This read-only check does not enable F3
// animation suppression or change any other rung's body ownership.
bool PlayerNativeBodyRequiredForMotion122() noexcept {return InspectNativeBody112(g_player_skeleton_anim.load(std::memory_order_acquire));}
void SetStartDiagnosticsLevel(int level) noexcept { g_start_diag_level.store(std::clamp(level,0,5),std::memory_order_release); }
void SetDialogueCameraSuppression(bool enabled) noexcept {
    g_dialogue_camera_suppression.store(enabled,std::memory_order_release);
    char m[192]{}; std::snprintf(m,sizeof(m),"V65.03-F5 runtime ADB suppression=%s count=%llu",enabled?"ON":"OFF",
        (unsigned long long)g_dialogue_camera_suppressed.load(std::memory_order_relaxed)); diagnostic_trace::Write(m);
}



bool NativeInteractionHandOffActive() noexcept { return false; }

// Retail FIFO: verified +0x78 count, +0x90 const entry, 0x78 stride,
// local animation ID +0x10 and activated bit +0x50 (native enqueue +0xAA9528).
// Eligibility is deliberately named SOLE_ACTIVATED_STABLE, not 'weight == 1'.
// Read the same CAF header selected by the exact retail SampleAnimation routine.
// The +0x800 flag means CREATED, not proof of resident controller data; sampling
// returns its own status 3 when controller data is absent. No synchronous loading.
bool ResolveCurveReference(void* anim,const char* alias,CurveReferenceInfo& out) noexcept {
    out={};void* set=AnimationSet6689(anim);std::uintptr_t vf=0,fn=0;
    if(!set||!ExactBuild(WhBase())||!Read(set,vf)||vf!=WhBase()+0x406BB38ull||
       !Read(reinterpret_cast<void*>(vf+0xB8),fn)||fn!=WhBase()+0x37A2150ull)return false;
    const int id=ResolveReferenceId6689(set,alias);if(id<0)return false;
    void* entries=nullptr;int count=0;
    if(!Read(static_cast<unsigned char*>(set)+8,entries)||!entries||
       !Read(static_cast<unsigned char*>(entries)-4,count)||id>=count||count>2000000)return false;
    const auto* e=static_cast<unsigned char*>(entries)+std::size_t(id)*16;
    int global=-1;std::uint16_t type=0;std::uint64_t key=0;
    if(!Read(e,key)||!Read(e+8,global)||!Read(e+12,type)||type!=0||global<0||global>2000000)return false;
    void* manager=nullptr;void* headers=nullptr;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x5189BE0ull),manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+0x280,headers)||!headers)return false;
    std::uint32_t flags=0;
    if(!Read(static_cast<unsigned char*>(headers)+std::size_t(global)*0xA8+0x18,flags))return false;
    out.id=id;out.alias=key;out.flags=flags;return true;
}
int SampleCurveReference(void* anim,int id,std::uint32_t crc,float phase,float* result) noexcept {
    if(!result||id<0||!std::isfinite(phase)||phase<0||phase>1)return -1;
    void* set=AnimationSet6689(anim);std::uintptr_t vf=0,fn=0;
    if(!set||!Read(set,vf)||vf!=WhBase()+0x406BB38ull||
       !Read(reinterpret_cast<void*>(vf+0xB8),fn)||fn!=WhBase()+0x37A2150ull)return -1;
    // Native integer result; five-argument x64 ABI: phase in XMM2, CRC R9D,
    // QuatT output on the stack. Verified +37A2150 and skeleton CRC getter +84415C.
    __try { return reinterpret_cast<int(__fastcall*)(void*,int,float,std::uint32_t,float*)>(fn)(set,id,phase,crc,result); }
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}

bool ReadPoseReferenceState(void* anim,PoseReferenceState& out) noexcept {
    out={};if(!anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire))return false;
    const auto b=WhBase();if(!ExactBuild(b)||!MatchVft(anim,b+kCSkeletonAnimVftRva))return false;
    void* set=AnimationSet6689(anim);if(!set)return false;
    static std::mutex cacheMutex;std::unique_lock<std::mutex> lock(cacheMutex,std::try_to_lock);if(!lock.owns_lock())return false;
    static void* cachedSet=nullptr;static int ids[5]{-1,-1,-1,-1,-1};
    static std::uint64_t lastResolve=0;static unsigned resolveAttempts=0;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool changed=set!=cachedSet;
    bool missing=false;for(int id:ids)missing=missing||id<0;
    if(changed){lastResolve=0;resolveAttempts=0;for(int& id:ids)id=-1;}
    // Do not retain negative startup results forever. Six bounded attempts per
    // animation-set lifetime, >=2s apart; successful IDs are not repeatedly resolved.
    if(changed||(missing&&resolveAttempts<6&&now>=lastResolve&&now-lastResolve>=2000)){
        // ADB aZ1/lg -> HoldItemRight; aZ2/rg -> HoldItemLeft.
        constexpr const char* names[]{"combat_attack_lg_z2_nwpn_add","combat_attack_rg_z1_nwpn_add","combat_lg_sz2_idle_nwpn_player","combat_rg_sz1_idle_nwpn_player","combat_rg_sz5_idle_nwpn_player"};
        for(int i=0;i<5;++i){
            // small SEH leaf helper avoids MSVC C2712 with the RAII lock here
            if(ids[i]<0)ids[i]=ResolveReferenceId6689(set,names[i]);
        }cachedSet=set;lastResolve=now;++resolveAttempts;
        if(diagnostics::combat_capture::active(now)){
            char m[384]{};std::snprintf(m,sizeof(m),
                "V66.92R1-FINGER-DONOR-IDS set=%p attempt=%u ids={closedR:%d closedL:%d idle0:%d idle1:%d idle2:%d} resolution=NOT_ENDPOINT_READINESS",
                set,resolveAttempts,ids[0],ids[1],ids[2],ids[3],ids[4]);diagnostic_trace::Write(m);
        }
    }
    bool closedSeen[2]{false,false};bool idle=false;std::uint64_t idleSerial=0;
    for(int layer=0;layer<16;++layer){
        auto* layerPtr=static_cast<unsigned char*>(anim)+std::size_t(layer)*0x40;
        unsigned char* entries=nullptr;std::uint32_t count=0;
        if(!Read(layerPtr+0x48,entries))return false;
        if(!entries)continue; // unused layer is empty, not a failed pose reference
        if(!Read(entries-4,count)||count>16)return false;
        for(std::uint32_t i=0;i<count;++i){int id=-1;std::uint16_t state=0;
            if(!Read(entries+i*0x78+0x10,id)||!Read(entries+i*0x78+0x50,state))return false;
            if(!(state&1))continue;
            for(int side=0;side<2;++side)if(ids[side]>=0&&id==ids[side]){
                closedSeen[side]=true;
                if(count==1){out.closed[side]=true;out.serial[side]=(std::uint64_t(std::uint32_t(id))+1)*32+std::uint64_t(layer+1);}
            }
            if(count==1&&((ids[2]>=0&&id==ids[2])||(ids[3]>=0&&id==ids[3])||(ids[4]>=0&&id==ids[4]))) {idle=true;idleSerial=(std::uint64_t(std::uint32_t(id))+1)*32+std::uint64_t(layer+1);}
        }
    }
    for(int side=0;side<2;++side)if(idle&&!closedSeen[side]){out.relaxed[side]=true;out.serial[side]=idleSerial;}
    return true;
}

void SetInteractionAnimationTarget(void* anim,std::uint32_t id,std::uint64_t sequence) noexcept {
    RegisterStream6690(1,anim,id,sequence);
    g_interaction_target_anim6689.store(nullptr,std::memory_order_release);
    g_interaction_target_id6689.store(id,std::memory_order_release);g_interaction_sequence6689.store(sequence,std::memory_order_release);
    g_interaction_target_anim6689.store(anim,std::memory_order_release);
}
void RequestPhysicalMapReadingAnimation(int phase) noexcept {
    phase=std::clamp(phase,0,3);
    if(phase<=0){g_physical_map_reading_request.store(0,std::memory_order_release);g_physical_map_reading_request_ms.store(0,std::memory_order_release);return;}
    g_physical_map_reading_request_ms.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
    g_physical_map_reading_request.store(phase,std::memory_order_release);
    g_physical_map_reading_requests.fetch_add(1,std::memory_order_relaxed);
}
int PendingPhysicalMapReadingAnimation() noexcept { return g_physical_map_reading_request.load(std::memory_order_acquire); }
std::uint64_t PhysicalMapReadingAnimationStarts() noexcept { return g_physical_map_reading_starts.load(std::memory_order_acquire); }

} // namespace kcd2vr::animation_filter
