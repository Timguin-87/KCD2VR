// The complete production animation TU; only OS/native leaves are mocked.
// No executable patching, Windows SEH or headset result is claimed here.
#ifndef PRODUCTION_ANIMATION_SOURCE_D51
#define PRODUCTION_ANIMATION_SOURCE_D51 "../src/animation_filter.cpp"
#endif
#include PRODUCTION_ANIMATION_SOURCE_D51
#include "subsystems/masterstrike_combos/native_paired_aliases_d51.h"
#include <cstdio>
#include <string>
namespace kcd2vr::retail_build112 {
bool verifiedD51=true;
bool Ready(std::uintptr_t module) noexcept {return module&&verifiedD51;}
}
static unsigned diagnosticWritesD51=0;
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {++diagnosticWritesD51;}}
namespace kcd2vr::frik_pose_probe {
std::uint64_t PoseEpochH24() noexcept {return 0;}
bool NativeFreeKickVisualR24() noexcept {return false;}
}
using namespace kcd2vr::animation_filter;
static unsigned checksD51=0,failuresD51=0,callsD51=0;
static bool originalResultD51=false;
static void checkD51(bool value,const char* why){++checksD51;if(!value){++failuresD51;std::printf("FAIL %s\n",why);}}
template<class T>static void putD51(unsigned char* at,T value){std::memcpy(at,&value,sizeof(value));}
static bool originalIdD51(void*,int,void*){++callsD51;return originalResultD51;}
static bool originalNameD51(void*,const char*,void*){++callsD51;return originalResultD51;}
int main(){
    namespace nk=kcd2vr::native_kill_nf;
    namespace policy=kcd2vr::vr_combat111;
    namespace aliasesD51=kcd2vr::native_paired_aliases_d51;
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),
        queue(4+0x78),other(4+0x78),params(0x40),npc(0x600);
    hostModuleHandleW=image.data();const auto base=WhBase();
    putD51(anim.data(),base+kCSkeletonAnimVftRva);putD51(anim.data()+0x4E8,character.data());
    putD51(character.data()+0xA70,skeleton.data());putD51(skeleton.data()+0x218,set.data());
    putD51(set.data()+8,aliases.data()+4);putD51(aliases.data(),std::uint32_t(2));
    putD51(aliases.data()+4,std::uint64_t(123));putD51(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"combat_finish_lngsw_01"},{456,"combat_rg_sz1_dz2_masterstrike_lngsw_master"}};
    g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    g_id_original=&originalIdD51;g_original=&originalNameD51;g_mode=int(SuppressionMode::AllSelectedCombat);
    kcd2vr::diagnostics::lab::state=3;kcd2vr::diagnostics::capture168::gate.mask=0;combatPoseTick112=hostTickCount64;
    putD51(queue.data(),std::uint32_t(1));putD51(queue.data()+4+0x10,1);putD51(queue.data()+4+0x50,std::uint16_t(1));
    putD51(other.data(),std::uint32_t(1));putD51(other.data()+4+0x10,0);putD51(other.data()+4+0x50,std::uint16_t(1));
    putD51(anim.data()+0x48,queue.data()+4);putD51(params.data()+0x10,0);
    checkD51(!nk::enabled.load(),"native paired playback defaults OFF");
    checkD51(aliasesD51::count==153,"exact authored table has 153 distinct aliases");
    for(const char* alias:aliasesD51::aliases){
        g_alias_catalog6694[1].name=alias;
        const bool baseline=policy::NativeBodyAlias112(alias);
        nk::SetEnabled(false);
        checkD51(NativeBodyRequired112(anim.data())==baseline,"OFF preserves old protected ownership for every authored alias");
        checkD51(!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),"OFF cannot create playback occurrence");
        nk::SetEnabled(true);
        checkD51(NativeBodyRequired112(anim.data()),"ON recognizes every authored alias on Henry active layer 0");
        checkD51(nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),"ON clock stand-down sees the same authored occurrence");
        const auto serial=nk::liveSerial.load(),generation=nk::generation.load();
        nk::SetEnabled(true);
        checkD51(nk::liveSerial.load()==serial&&nk::generation.load()==generation,"unchanged preference application preserves the playing occurrence");
        // Extra alias eligibility is layer 0 only. Existing protection on
        // other layers remains whatever the baseline predicate already said.
        for(int layer:{1,6}){
            putD51(anim.data()+0x48,static_cast<void*>(nullptr));putD51(anim.data()+layer*0x40+0x48,queue.data()+4);
            checkD51(NativeBodyRequired112(anim.data())==baseline,"fallback does not extend ownership to unproven runtime layers");
            checkD51(nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64)==(baseline&&nk::KillAlias(alias)),
                     "other-layer clock ownership retains only the old protected kill rule");
            putD51(anim.data()+layer*0x40+0x48,static_cast<void*>(nullptr));
        }
        putD51(anim.data()+0x48,queue.data()+4);
        putD51(queue.data()+4+0x50,std::uint16_t(0));
        checkD51(!NativeBodyRequired112(anim.data())&&!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),
                 "inactive paired FIFO entry immediately retires ownership");
        putD51(queue.data()+4+0x50,std::uint16_t(1));
    }
    g_alias_catalog6694[1].name="combat_rg_sz1_dz2_masterstrike_lngsw_master";
    for(unsigned f3:{2u,3u})for(bool selected:{false,true})for(bool nativeResult:{false,true}){
        nk::SetEnabled(selected);kcd2vr::diagnostics::lab::state=f3;originalResultD51=nativeResult;
        const auto before=callsD51;
        checkD51(StartAnimationIdDetour6689(anim.data(),1,params.data())==nativeResult&&callsD51==before+1,
                 "ID enqueue preserves native false/true and calls original once");
        checkD51(StartAnimationDetour(anim.data(),g_alias_catalog6694[1].name,params.data())==nativeResult&&callsD51==before+2,
                 "name enqueue preserves native false/true and calls original once");
    }
    kcd2vr::diagnostics::lab::state=0;nk::SetEnabled(false);
    checkD51(!PlayerNativeBodyRequiredForMotion122(),"independent motion reader OFF preserves nonlethal baseline");
    nk::SetEnabled(true);
    checkD51(PlayerNativeBodyRequiredForMotion122()&&nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),
             "independent motion reader uses the selected fallback without enabling F3");
    kcd2vr::diagnostics::lab::state=3;
    // A second, explicit reload entry cannot lend its narrow exception to a
    // concurrent selected native paired owner.
    putD51(anim.data()+6*0x40+0x48,other.data()+4);
    checkD51(!ReloadPassengerOnlyXR34(anim.data(),reinterpret_cast<std::uintptr_t>(other.data()+4),0),
             "concurrent paired fallback cannot borrow reload passenger ownership");
    nk::SetEnabled(false);
    checkD51(ReloadPassengerOnlyXR34(anim.data(),reinterpret_cast<std::uintptr_t>(other.data()+4),0),
             "OFF restores baseline reload exclusion behavior");
    putD51(anim.data()+6*0x40+0x48,static_cast<void*>(nullptr));
    nk::SetEnabled(true);
    for(const char* bad:{"combat_rg_sz1_dz2_masterstrike_lngsw_master_extra","combat_rg_sz1_dz2_masterstrike_lngsw_master.caf",
                        "combat_combo_unknown_solo","combat_unknown_master"}){
        g_alias_catalog6694[1].name=bad;
        checkD51(!NativeBodyRequired112(anim.data())&&!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),
                 "prefixes, suffixes and unknown aliases do not grant paired ownership");
    }
    g_alias_catalog6694[1].name="combat_rg_sz1_dz2_masterstrike_lngsw_master";
    checkD51(NativeBodyRequired112(anim.data()),"qualified fallback restored");
    checkD51(NativeBodyRequired112(npc.data())&&!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),
             "unvalidated NPC cannot publish Henry playback occurrence");
    checkD51(NativeBodyRequired112(anim.data()),"Henry re-observation is required after identity loss");
    kcd2vr::retail_build112::verifiedD51=false;
    checkD51(NativeBodyRequired112(anim.data())&&!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),
             "unverified retail keeps existing fail-closed owner but retires optional playback occurrence");
    kcd2vr::retail_build112::verifiedD51=true;nk::SetEnabled(false);
    g_alias_catalog6694[1].name="combat_finish_lngsw_01";
    checkD51(NativeBodyRequired112(anim.data()),"backup OFF still retains existing protected native reactions");
    checkD51(diagnosticWritesD51==0,"disabled F captures produce no optional animation diagnostics");
    std::printf("D51 paired playback production TU: %u/%u; original native calls mocked; headset NOT_RUN\n",checksD51-failuresD51,checksD51);
    return failuresD51?1:0;
}
