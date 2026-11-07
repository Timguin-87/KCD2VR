// Executes the complete production animation TU against modeled FIFO/native IO.
// This is an ownership regression, not native game or headset acceptance.
#ifndef PRODUCTION_ANIMATION_SOURCE_D63
#define PRODUCTION_ANIMATION_SOURCE_D63 "../src/animation_filter.cpp"
#endif
#include PRODUCTION_ANIMATION_SOURCE_D63
#include "subsystems/masterstrike_combos/paired_attacker_aliases_d63.h"
#include <cstdio>
#include <string>
namespace kcd2vr::retail_build112 {
bool verifiedD63=true;
bool Ready(std::uintptr_t module) noexcept {return module&&verifiedD63;}
}
static unsigned diagnosticWritesD63=0;
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {++diagnosticWritesD63;}}
namespace kcd2vr::frik_pose_probe {
std::uint64_t PoseEpochH24() noexcept {return 0;}
bool NativeFreeKickVisualR24() noexcept {return false;}
}
using namespace kcd2vr::animation_filter;
static unsigned checks=0,failures=0,nativeCalls=0;
static bool nativeResult=false;
static void ck(bool value,const char* why){++checks;if(!value){++failures;std::printf("FAIL %s\n",why);}}
template<class T>static void put(unsigned char* at,T value){std::memcpy(at,&value,sizeof(value));}
static bool originalId(void*,int,void*){++nativeCalls;return nativeResult;}
static bool originalName(void*,const char*,void*){++nativeCalls;return nativeResult;}
int main(){
    namespace nk=kcd2vr::native_kill_nf;
    namespace policy=kcd2vr::vr_combat111;
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),
        fifo(4+2*0x78),params(0x40),npc(0x600);
    hostModuleHandleW=image.data();const auto base=WhBase();
    put(anim.data(),base+kCSkeletonAnimVftRva);put(anim.data()+0x4E8,character.data());
    put(character.data()+0xA70,skeleton.data());put(skeleton.data()+0x218,set.data());
    put(set.data()+8,aliases.data()+4);put(aliases.data(),std::uint32_t(2));
    put(aliases.data()+4,std::uint64_t(123));put(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"combat_rg_sz1_dz2_masterstrikedeath_lngsw_master"},
                        {456,"combat_lg_sz2_dz1_masterstrike_nwpn_slave"}};
    g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    g_id_original=&originalId;g_original=&originalName;g_mode=int(SuppressionMode::AllSelectedCombat);
    kcd2vr::diagnostics::lab::state=3;kcd2vr::diagnostics::capture168::gate.mask=0;combatPoseTick112=hostTickCount64;
    put(fifo.data(),std::uint32_t(1));put(fifo.data()+4+0x10,0);put(fifo.data()+4+0x50,std::uint16_t(1));
    put(anim.data()+0x48,fifo.data()+4);put(params.data()+0x10,0);
    // This exact death alias is the 21:55:52/54/56 user-recorded occurrence.
    // Policy correction: paired alignment is unproven, so preference OFF alone
    // cannot grant VR body ownership. Native event carrier still runs once.
    nk::SetEnabled(false);
    ck(NativeBodyRequired112(anim.data()),"recorded paired death remains native until attacker/victim alignment is proven");
    ck(PlayerNativeBodyRequired112(),"ordinary gameplay reader preserves protected paired ownership");
    ck(PlayerNativeBodyRequiredForMotion122(),"independent motion reader preserves protected paired ownership");
    ck(!nk::StandDown(reinterpret_cast<std::uintptr_t>(anim.data()),hostTickCount64),"OFF grants no native clock stand-down");
    for(const auto* alias:kcd2vr::paired_attacker_aliases_d63::aliases){
        g_alias_catalog6694[0].name=alias;nk::SetEnabled(false);
        ck(NativeBodyRequired112(anim.data())==policy::NativeBodyAlias112(alias),"catalog membership alone cannot change existing protected-body ownership");
        nk::SetEnabled(true);
        ck(NativeBodyRequired112(anim.data()),"exact attacker member ON retains native backup");
    }
    g_alias_catalog6694[0].name="combat_rg_sz1_dz2_masterstrikedeath_lngsw_master";
    for(bool selected:{false,true})for(bool result:{false,true}){
        nk::SetEnabled(selected);nativeResult=result;const auto before=nativeCalls;
        ck(StartAnimationIdDetour6689(anim.data(),0,params.data())==result&&nativeCalls==before+1,
           "native paired event carrier called once and original result preserved by ID");
        ck(StartAnimationDetour(anim.data(),g_alias_catalog6694[0].name,params.data())==result&&nativeCalls==before+2,
           "native paired event carrier called once and original result preserved by name");
    }
    nk::SetEnabled(false);
    for(const auto* alias:{"combat_lg_sz2_dz1_masterstrike_nwpn_slave","combat_combodeath_c05_left_blunt_slave",
                          "combat_clinch1_sz0_az0_ripostesync_step0_lngsw_master","combat_finish_lngsw_01"}){
        g_alias_catalog6694[0].name=alias;
        ck(NativeBodyRequired112(anim.data())==policy::NativeBodyAlias112(alias),"victim/clinch/finish native policy preserved");
    }
    g_alias_catalog6694[0].name="combat_rg_sz1_dz2_masterstrikedeath_lngsw_master";
    put(fifo.data(),std::uint32_t(2));put(fifo.data()+4+0x78+0x10,1);put(fifo.data()+4+0x78+0x50,std::uint16_t(1));
    ck(NativeBodyRequired112(anim.data()),"concurrent victim clip still owns native body");
    put(fifo.data(),std::uint32_t(1));
    for(int layer:{1,6}){
        put(anim.data()+0x48,static_cast<void*>(nullptr));put(anim.data()+layer*0x40+0x48,fifo.data()+4);
        ck(NativeBodyRequired112(anim.data()),"unproven runtime layers retain original death ownership");
        put(anim.data()+layer*0x40+0x48,static_cast<void*>(nullptr));
    }
    put(anim.data()+0x48,fifo.data()+4);put(fifo.data()+4+0x10,300);
    ck(NativeBodyRequired112(anim.data()),"unknown live FIFO alias remains fail-closed");
    put(fifo.data()+4+0x10,0);
    ck(NativeBodyRequired112(npc.data()),"unvalidated NPC cannot borrow Henry identity");
    kcd2vr::retail_build112::verifiedD63=false;
    ck(NativeBodyRequired112(anim.data()),"unknown retail retains native owner");
    kcd2vr::retail_build112::verifiedD63=true;
    nk::SetEnabled(true);ck(NativeBodyRequired112(anim.data()),"ON takes current native owner");
    nk::SetEnabled(false);ck(NativeBodyRequired112(anim.data()),"live OFF does not release an unproven paired death owner");
    ck(diagnosticWritesD63==0,"disabled captures produce no new diagnostic stream");
    std::printf("D63 production paired presentation: %u/%u; native leaves mocked; game/headset NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
