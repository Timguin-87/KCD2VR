// Full production animation TU. Native memory/calls below are explicit fixture
// leaves; no game execution, visual result or Windows SEH claim.
#ifndef PRODUCTION_ANIMATION_SOURCE_D50
#define PRODUCTION_ANIMATION_SOURCE_D50 "../src/animation_filter.cpp"
#endif
#include PRODUCTION_ANIMATION_SOURCE_D50
#include <cstdio>
#include <string>
namespace kcd2vr::retail_build112 {
bool verifiedD50=true;
bool Ready(std::uintptr_t module) noexcept {return module&&verifiedD50;}
}
static unsigned bodyGateWritesD50=0;
namespace kcd2vr::diagnostic_trace {void Write(const char* line) noexcept {
    if(line&&std::strstr(line,"V66.178R30C1 BODY_GATE "))++bodyGateWritesD50;
}}
namespace kcd2vr::frik_pose_probe {
std::uint64_t PoseEpochH24() noexcept {return 0;}
bool NativeFreeKickVisualR24() noexcept {return false;}
}
using namespace kcd2vr::animation_filter;
static unsigned checksD50=0,failuresD50=0,callsD50=0;
static void ckD50(bool value,const char* why){++checksD50;if(!value){++failuresD50;std::printf("FAIL %s\n",why);}}
template<class T>static void putD50(unsigned char* at,T value){std::memcpy(at,&value,sizeof(value));}
static bool originalIdD50(void*,int,void*){++callsD50;return false;}
static bool originalNameD50(void*,const char*,void*){++callsD50;return false;}
int main(){
    namespace policy=kcd2vr::vr_combat111;
    namespace cap=kcd2vr::diagnostics::capture168;
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),queue(4+0x78),other(4+0x78),params(0x40),npc(0x600);
    hostModuleHandleW=image.data();const auto base=WhBase();
    putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
    putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
    putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(2));
    putD50(aliases.data()+4,std::uint64_t(123));putD50(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"combat_finish_lngsw_01"},{456,"2d_horse_walk.bspace"}};
    g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    g_id_original=&originalIdD50;g_original=&originalNameD50;g_mode=int(SuppressionMode::AllSelectedCombat);
    kcd2vr::diagnostics::lab::state=3;cap::gate.mask=0;combatPoseTick112=hostTickCount64;
    putD50(queue.data(),std::uint32_t(1));putD50(queue.data()+4+0x10,1);putD50(queue.data()+4+0x50,std::uint16_t(1));
    putD50(other.data(),std::uint32_t(1));putD50(other.data()+4+0x10,0);putD50(other.data()+4+0x50,std::uint16_t(1));
    putD50(anim.data()+0x48,queue.data()+4);putD50(params.data()+0x10,0);
    for(bool lean:{false,true}){
        policy::hideSwingLean38=lean;
        for(const char* alias:{"2d_horse_walk","2d_horse_trot","2d_horse_canter","2d_horse_gallop"}){
            const std::string live=std::string(alias)+".bspace";
            ckD50(policy::NativeBodyAlias112(alias),"existing bare mounted alias retained");
            ckD50(policy::NativeBodyAlias112(live.c_str()),"same verified mounted asset with suffix recognized");
            g_alias_catalog6694[1].name=live.c_str();
            ckD50(NativeBodyRequired112(anim.data()),"active mounted FIFO retains native ownership for each gait");
            const auto before=callsD50;
            ckD50(!StartAnimationIdDetour6689(anim.data(),1,params.data())&&callsD50==before+1,"native gait enqueue and return preserved");
            putD50(queue.data()+4+0x50,std::uint16_t(0));
            ckD50(!NativeBodyRequired112(anim.data()),"inactive gait never leaves timer ownership");
            putD50(queue.data()+4+0x50,std::uint16_t(1));
        }
    }
    for(const char* bad:{"2d_horse_walk.caf","2d_horse_walk.bspace.extra","2d_horse_walk_evil.bspace","other/2d_horse_walk.bspace","2d_horse_unknown.bspace","combat_finish_lngsw_01.bspace"})
        ckD50(!policy::NativeBodyAlias112(bad),"unproven name/path/suffix not broadened");
    ckD50(!policy::NativeBodyAlias112(nullptr),"null alias safe");
    putD50(anim.data()+0x48,static_cast<void*>(nullptr));putD50(anim.data()+0x40+0x48,queue.data()+4);
    g_alias_catalog6694[1].name="fa_idle_neutral_03";putD50(params.data()+0x10,1);
    for(unsigned mode:{2u,3u}){kcd2vr::diagnostics::lab::state=mode;
        ckD50(!NativeBodyRequired112(anim.data()),"proven facial leaf clip alone cannot claim whole body");
        const auto before=callsD50;
        ckD50(!StartAnimationIdDetour6689(anim.data(),1,params.data())&&callsD50==before+1,"facial ID clip still plays once");
        ckD50(!StartAnimationDetour(anim.data(),"fa_idle_neutral_03",params.data())&&callsD50==before+2,"facial name clip still plays once");
    }
    kcd2vr::diagnostics::lab::state=0;
    ckD50(!PlayerNativeBodyRequiredForMotion122(),"independent motion guard also ignores proven face-only owner");
    kcd2vr::diagnostics::lab::state=3;
    putD50(anim.data()+0x48,other.data()+4);
    for(const char* protectedName:{"combat_finish_lngsw_01","combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master","combat_rg_sz1_dz2_death_slash_lngsw_01","2d_horse_canter.bspace"}){
        g_alias_catalog6694[0].name=protectedName;
        ckD50(NativeBodyRequired112(anim.data()),"concurrent true protected action still wins over facial exception");
    }
    putD50(anim.data()+0x48,static_cast<void*>(nullptr));putD50(anim.data()+0x40+0x48,static_cast<void*>(nullptr));
    putD50(anim.data()+2*0x40+0x48,queue.data()+4);
    ckD50(NativeBodyRequired112(anim.data()),"unobserved facial layer remains protected");
    ckD50(!policy::BodylessFacial126("fa_idle_neutral_03_extra",1)&&!policy::BodylessFacial126(nullptr,1),"facial exception is exact and null safe");
    putD50(anim.data()+2*0x40+0x48,static_cast<void*>(nullptr));putD50(anim.data()+0x40+0x48,queue.data()+4);
    kcd2vr::retail_build112::verifiedD50=false;
    ckD50(NativeBodyRequired112(anim.data()),"retail identity remains mandatory");
    kcd2vr::retail_build112::verifiedD50=true;
    ckD50(NativeBodyRequired112(npc.data()),"other character cannot borrow facial exception");
    ckD50(cap::gate.mask.load()==0,"classification does not enable a recorder");
    ckD50(bodyGateWritesD50==0,"disabled capture skips actual BODY_GATE writer and optional native hash branch");
    for(auto domain:{cap::Domain::Bug,cap::Domain::Vrik,cap::Domain::Combat}){
        hostTickCount64+=3000;cap::gate.select(domain,true,hostTickCount64);
        const auto before=bodyGateWritesD50;
        BodyDecision115(false,"QUALIFIED_LIVE_POSE");
        ckD50(bodyGateWritesD50==before+1,"each requested owner capture permits bounded BODY_GATE evidence");
        hostTickCount64+=cap::Duration;BodyDecision115(false,"QUALIFIED_LIVE_POSE");
        ckD50(bodyGateWritesD50==before+1,"expired owner capture suppresses actual BODY_GATE writer");
        cap::gate.select(domain,false,hostTickCount64);
    }
    std::printf("D50 riding/facial production ownership %u/%u; native playback is mocked, headset NOT_RUN\n",checksD50-failuresD50,checksD50);
    return failuresD50?1:0;
}
