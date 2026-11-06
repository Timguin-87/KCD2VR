#pragma once
#include <cstring>
#include <cstdint>
#include <algorithm>
#include "combat_visual_aliases112.h"
#include <atomic>

namespace kcd2vr::vr_combat111 {

// Fresh, exact Henry clinch playback only authorizes native INPUT, not an outcome.
inline std::atomic<std::uint64_t> clinchInputTick55c1{0};
inline bool ClinchAlias55c1(const char* n) noexcept {
    return n&&std::strncmp(n,"combat_clinch",13)==0&&
        (std::strstr(n,"clinchguard_")||std::strstr(n,"clinchstartguard_")||std::strstr(n,"clinchtransition_"))&&
        std::strstr(n,"_master")&&!std::strstr(n,"kill")&&!std::strstr(n,"death");
}
inline bool ClinchInput55c1(std::uint64_t now) noexcept {
    const auto t=clinchInputTick55c1.load(std::memory_order_acquire);
    return t&&now>=t&&now-t<=80;
}
inline bool F3OwnsPlayerCombat(unsigned f3) noexcept { return f3==2u||f3==3u; }
// These are internal motion-policy values. Public F3 is a recorder and no
// longer selects this state; calibrated/F10/F12/F4 transitions own it.
inline bool MotionOwnsPlayerCombat175(unsigned motionPolicy) noexcept {return F3OwnsPlayerCombat(motionPolicy);}
// Internal policy 3 owns the final visible pose while native clips/events run.
// Internal policy 2 retains the historical enqueue-suppression comparison.
// C51R1: returning success without enqueueing an ordinary melee clip also drops
// its declared audio_trigger/foley events (review/V66.112/evidence/assets.json).
// Modern F4/F12 use policy 3: retain the one original native clip and its result;
// the completed calibrated pose owns visible head/hands/weapon independently.
// Legacy saved hide preferences remain readable, but cannot delete event carriers
// in policy 3. This does not admit an attack, change its clock, or replay a sound.
inline std::atomic<bool> hideNativeClips178r25{false};       // legacy INI VrCombatHideNativeClips
inline std::atomic<bool> hideNativeClipsActive178r28{false}; // current F4 pose ownership; also qualifies the attack-connect exception below
inline std::atomic<bool> nativeEngageClipVisible178r33{false};
inline std::atomic<bool> hideEngagedClips50{true}; // legacy INI VrCombatHideEngagedClips
inline bool SuppressEnqueue114(unsigned motionPolicy) noexcept {return motionPolicy==2u;}

inline bool OwnershipReady112(unsigned f3,bool nativeReference,bool exactHenry,
                              std::uint64_t now,std::uint64_t poseTick) noexcept {
    return F3OwnsPlayerCombat(f3) && !nativeReference && exactHenry && poseTick &&
           now>=poseTick && now-poseTick<=100;
}

// C38 (17 Oct log, F4 SYNC): the FREE attack swing is a blend space; the native name lookup returns it WITH its file
// extension ('1d-combat_player_fatksw_lngsw.bspace', 27 starts on layer 6) while the ADB-derived list holds the alias
// without it, so only the start overlay was ever hidden and Henry's upper body leaned into every free swing.
// INI VrHideSwingLean (default on): a trailing ".bspace" also matches the SAME exact alias/layer pair. Nothing else
// changes: normal-attack (natk) blend spaces are not in the list and stay native, full-body layers stay native.
inline std::atomic<bool> hideSwingLean38{true};
inline bool AliasEquals38(const char* name,const char* alias) noexcept {
    if(std::strcmp(name,alias)==0)return true;
    if(!hideSwingLean38.load(std::memory_order_relaxed))return false;
    const std::size_t n=std::strlen(alias);
    return std::strncmp(name,alias,n)==0&&std::strcmp(name+n,".bspace")==0;
}
inline bool SuppressPlayerMeleeVisual(const char* name,int layer=-1) noexcept {
    // Unknown aliases/layers and all full-body layers are native. Asset names
    // alone cannot distinguish a paired action or its gameplay events.
    if(!name||layer<3||layer>6)return false;
    for(const auto& alias:kVisualAliases112)
        if((alias.layers&(1u<<layer)) && AliasEquals38(name,alias.name))return true;
    return false;
}
inline bool NativeBodyAlias112(const char* name) noexcept {
    if(!name)return false;
    const auto* end=kNativeBodyAliases112+sizeof(kNativeBodyAliases112)/sizeof(kNativeBodyAliases112[0]);
    const auto* found=std::lower_bound(kNativeBodyAliases112,end,name,[](const char* a,const char* b){return std::strcmp(a,b)<0;});
    if(found!=end&&std::strcmp(*found,name)==0)return true;
    // D78 capture: the actual mount/dismount names (and their alignment
    // siblings) were absent from the generated catalog. Keep their native
    // trajectories until those exact active FIFO entries retire; they never
    // qualify for the optional ordinary-riding arm overlay.
    for(const char* transfer:{"horse_mount_left_02","horse_mount_left_02_align",
                              "horse_dismount_left_02","horse_dismount_left_02_align"})
        if(std::strcmp(name,transfer)==0)return true;
    // D50: the four mounted gait assets verified in the D46 capture arrive
    // with their authored suffix. Preserve their existing native ownership
    // across idle/walk/trot/canter/gallop. This is independent of swing-lean
    // preferences; no suffix wildcard is granted to other protected aliases.
    if(std::strncmp(name,"2d_horse_",9)!=0)return false;
    for(const char* gait:{"2d_horse_walk.bspace","2d_horse_trot.bspace",
                         "2d_horse_canter.bspace","2d_horse_gallop.bspace"})
        if(std::strcmp(name,gait)==0)return true;
    return false;
}
// V66.178R31: being HIT must not hand the arms back to the native animation. The 00:10 log: 52 of 54 pose
// rejections were NATIVE_BODY_HANDOFF on hit reactions (hitlowmovement, hit_small_*_add, dz*_hit_*) - the
// moment the VRIK hands jumped to the native pose after every blow. Clinches, kills, deaths, knock-downs,
// slave/finisher clips keep whole-body native ownership. Only the victim flinch is released to VRIK.
inline std::atomic<bool> hitReactionsKeepVrik178r31{true}; // INI VrHitReactionKeepsVrik
inline bool HitReactionAlias178r31(const char* name) noexcept {
    if(!name||!std::strstr(name,"hit"))return false;
    // Victim reactions only (the R29 logs: combat_*_hitlowmovement_*, combat_hit_small_*_add, hitreaction_idle_*). Henry's own
    // attack clips that merely contain "hit" (combat_rg_sz1_dz2_hit_slash_lngsw_01 = his slash connecting, with an attack and a
    // defense zone in the name) and the paired/whole-body clips stay native-body.
    static const char* whole[]{"clinch","kill","death","knock","_slave","finish","execut","grab","ragdoll","slash","thrust","stab","_dz","_az","natk","pommel"};
    for(const char* w:whole)if(std::strstr(name,w))return false;
    return std::strstr(name,"hitlowmovement")||std::strstr(name,"hit_small")||std::strstr(name,"hitreaction")||std::strstr(name,"_hitsw")||std::strstr(name,"_hitld");
}
// V66.178R50: Henry's own attack-CONNECT clips (combat_[lr]g_szN_dzN_hit_<slash|stab>_<weapon>, layer 0, played by the
// game when his hit lands) handed the whole body to the native animation for the length of the clip - the arms snapped
// away from the controllers right after every successful VR hit. Keep VRIK on the arms; the clip still plays under them.
inline std::atomic<bool> attackConnectKeepsVrik50{true}; // INI VrAttackConnectKeepsVrik
inline bool OwnAttackConnectAlias50(const char* name) noexcept {
    if(!name||std::strncmp(name,"combat_",7)!=0||!std::strstr(name,"_hit_"))return false;
    static const char* whole[]{"clinch","kill","death","knock","_slave","_master","finish","execut","grab","ragdoll","sync","hitlowmovement","hit_small","hitreaction","horse","mounted","rider","_pair","_paired"};
    for(const char* w:whole)if(std::strstr(name,w))return false;
    // D56: these exact ordinary unarmed hit-connect clips are in CombatHitGen
    // beside the already-qualified sword connects (normal hit + sZ/dZ tags and
    // Withdraw event). Victim flinches use separate reaction fragments. Native
    // clip/procedural events continue unchanged; this only retains VR arms.
    static constexpr const char* unarmedD56[]{
        "combat_lg_sz2_dz0_hit_nwpn","combat_lg_sz2_dz1_hit_nwpn","combat_lg_sz2_dz2_hit_nwpn",
        "combat_lg_sz2_dz5_hit_nwpn","combat_lg_sz2_dz5_hit_nwpn_nwpn",
        "combat_rg_sz1_dz0_hit_nwpn","combat_rg_sz1_dz1_hit_nwpn","combat_rg_sz1_dz2_hit_nwpn","combat_rg_sz1_dz5_hit_nwpn",
        "combat_rg_sz5_dz0_hit_nwpn","combat_rg_sz5_dz1_hit_nwpn","combat_rg_sz5_dz2_hit_nwpn",
        "combat_rg_sz5_dz5_hit_nwpn","combat_rg_sz5_dz5_hit_nwpn_nwpn"};
    for(const auto* alias:unarmedD56)if(std::strcmp(name,alias)==0)return true;
    return std::strstr(name,"_sz")&&std::strstr(name,"_dz")&&(std::strstr(name,"slash")||std::strstr(name,"stab")||std::strstr(name,"thrust")||std::strstr(name,"attack"));
}
inline bool NativeKickAlias175(const char* name) noexcept {
    // Exact ordinary kick aliases in the supplied retail animation catalog.
    // Paired clinch kicks remain covered by NativeBodyAlias112's whole-body
    // ownership. A generic substring such as "kick" would include door actions
    // and the victim's hit reaction, neither of which is Henry's kick limb pose.
    if(!name)return false;
    // Native NameForId includes the authored extension for blendspaces (C38).
    // Only these two known blendspaces admit that exact suffix, independent of
    // the unrelated swing-lean preference. No substring/path/CAF wildcard.
    for(const char* alias:{"1d-combat_rg_sz5_az5_natk_kick_nwpn","1d-combat_rg_z5_natksw_dog_kick_nwpn"}){
        const auto n=std::strlen(alias);
        if(std::strncmp(name,alias,n)==0&&(name[n]=='\0'||std::strcmp(name+n,".bspace")==0))return true;
    }
    return std::strcmp(name,"combat_rg_sz5_az5_natk_kick_nwpn_add")==0;
}

} // namespace kcd2vr::vr_combat111
