// C51R1: execute the real NAME/ID detours. The fixture original stands in for
// native enqueue, not FMOD: this proves the event carrier is retained exactly
// once and the game return is preserved. It does not prove audible playback.
#include "../src/animation_filter.cpp"
#include <cstdlib>

namespace kcd2vr::retail_build112 {
bool hostVerified51=true;
bool Ready(std::uintptr_t module) noexcept {return module&&hostVerified51;}
}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::animation_filter;
namespace vc=kcd2vr::vr_combat111;
static int checks51=0,nameCalls51=0,idCalls51=0,carriers51=0;
static bool nativeAccept51=true,delegate51=false;
static void* expectedSelf51=nullptr;
static void* expectedParams51=nullptr;
static int expectedId51=0;
static void ck51(bool yes,const char* why) {
    ++checks51;
    if(!yes){std::fprintf(stderr,"FAIL C51 native events: %s\n",why);std::exit(1);}
}
template<class T> static void put51(unsigned char* p,T value){std::memcpy(p,&value,sizeof(value));}
static bool originalId51(void* self,int id,void* params) {
    ++idCalls51;
    ck51(self==expectedSelf51&&params==expectedParams51&&id==expectedId51,"ID original arguments preserved");
    if(nativeAccept51)++carriers51;
    return nativeAccept51;
}
static bool originalName51(void* self,const char*,void* params) {
    ++nameCalls51;
    ck51(self==expectedSelf51&&params==expectedParams51,"NAME original arguments preserved");
    // The real name wrapper resolves and dispatches to ID. Running both real
    // hooks here detects duplicate fallback enqueue or a remaining inner veto.
    if(delegate51)return StartAnimationIdDetour6689(self,expectedId51,params);
    if(nativeAccept51)++carriers51;
    return nativeAccept51;
}
int main() {
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),params(0x40),npc(0x600),queue(4+0x78);
    hostModuleHandleW=image.data();const auto base=WhBase();
    put51(anim.data(),base+kCSkeletonAnimVftRva);put51(anim.data()+0x4E8,character.data());
    put51(character.data()+0xA70,skeleton.data());put51(skeleton.data()+0x218,set.data());
    put51(set.data()+8,aliases.data()+4);put51(aliases.data(),std::uint32_t(2));
    put51(aliases.data()+4,std::uint64_t(123));put51(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"1d-combat_player_fatksw_lngsw"},{456,"combat_finish_lngsw_01"}};
    g_alias_ready6694=true;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    g_id_original=&originalId51;g_original=&originalName51;g_mode=int(SuppressionMode::AllSelectedCombat);
    expectedSelf51=anim.data();expectedParams51=params.data();
    kcd2vr::diagnostics::lab::state=3;combatPoseTick112=hostTickCount64;
    kcd2vr::diagnostics::capture168::gate.mask=0; // gameplay cannot depend on F3 recording
    put51(params.data()+0x10,6);
    for(bool hide:{false,true})for(bool engaged:{false,true}) {
        vc::hideNativeClips178r25=hide;vc::hideEngagedClips50=engaged;vc::hideNativeClipsActive178r28=true;
        const int beforeId=idCalls51,beforeCarriers=carriers51;
        ck51(StartAnimationIdDetour6689(anim.data(),0,params.data()),"native successful ID result preserved in active F4");
        ck51(idCalls51==beforeId+1&&carriers51==beforeCarriers+1,"ID event carrier enqueued exactly once");
        ck51(!vc::SuppressEnqueue114(3),"modern policy preserves events with every saved hide preference");
    }
    for(const char* name:{"1d-combat_player_fatksw_lngsw","1d-combat_player_fatksw_lngsw.bspace"}) {
        g_alias_catalog6694[0].name=name;
        ck51(vc::SuppressPlayerMeleeVisual(name,6),"regression alias belonged to suppressed native swing family");
        const int beforeName=nameCalls51,beforeId=idCalls51,beforeCarriers=carriers51;
        delegate51=true;
        ck51(StartAnimationDetour(anim.data(),name,params.data()),"nested NAME to ID native result preserved");
        ck51(nameCalls51==beforeName+1&&idCalls51==beforeId+1&&carriers51==beforeCarriers+1,"native wrapper reaches one original ID enqueue without duplication");
    }
    nativeAccept51=false;
    const int rejectedCarriers=carriers51,beforeRejectId=idCalls51;
    ck51(!StartAnimationDetour(anim.data(),g_alias_catalog6694[0].name,params.data()),"native rejection is never fabricated as successful enqueue");
    ck51(idCalls51==beforeRejectId+1&&carriers51==rejectedCarriers,"rejected clip gets no retry or substitute event");
    nativeAccept51=true;delegate51=false;
    for(const auto& alias:vc::kVisualAliases112) {
        g_alias_catalog6694[0].name=alias.name;
        for(int layer=3;layer<=6;++layer)if(alias.layers&(1u<<layer)) {
            put51(params.data()+0x10,layer);const int before=carriers51;
            ck51(StartAnimationIdDetour6689(anim.data(),0,params.data()),"each former visual alias keeps original ID carrier");
            ck51(StartAnimationDetour(anim.data(),alias.name,params.data()),"each former visual alias keeps original NAME carrier");
            ck51(carriers51==before+2,"one event carrier per actual native request");
        }
    }
    g_alias_catalog6694[0].name="1d-combat_player_fatksw_lngsw";put51(params.data()+0x10,6);
    for(unsigned policy:{0u,1u,3u}) {
        kcd2vr::diagnostics::lab::state=policy;
        ck51(StartAnimationIdDetour6689(anim.data(),0,params.data()),"normal handling and modern policy keep native sound carrier");
    }
    kcd2vr::diagnostics::lab::state=3;combatPoseTick112=hostTickCount64-101;
    ck51(StartAnimationIdDetour6689(anim.data(),0,params.data()),"loss of tracked pose leaves native clip intact");
    combatPoseTick112=hostTickCount64;
    kcd2vr::retail_build112::hostVerified51=false;
    ck51(StartAnimationIdDetour6689(anim.data(),0,params.data()),"unverified retail context does not synthesize or block a native event");
    kcd2vr::retail_build112::hostVerified51=true;
    expectedSelf51=npc.data();
    ck51(StartAnimationIdDetour6689(npc.data(),0,params.data()),"NPC original retained");
    expectedSelf51=nullptr;
    ck51(StartAnimationIdDetour6689(nullptr,0,params.data()),"null identity passes to original without impersonating Henry");
    expectedSelf51=anim.data();
    // Native paired/finisher body ownership is still a FIFO question. Merely
    // preserving a clip never makes that pose available to the VR writer.
    put51(anim.data()+0x48,queue.data()+4);put51(queue.data(),std::uint32_t(1));
    put51(queue.data()+4+0x10,1);put51(queue.data()+4+0x50,std::uint16_t(1));
    for(const char* protectedName:{"combat_finish_lngsw_01","combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master","combat_rg_sz1_dz2_death_slash_lngsw_01"}) {
        g_alias_catalog6694[1].name=protectedName;
        ck51(NativeBodyRequired112(anim.data()),"paired, finisher and death FIFO still owns native body");
        const int before=carriers51;
        ck51(StartAnimationIdDetour6689(anim.data(),0,params.data())&&carriers51==before+1,"protected native owner preserves ordinary original enqueue once");
    }
    put51(anim.data()+0x48,static_cast<void*>(nullptr));
    // Compatibility-only policy 2 remains a separate historical comparison.
    // It is not modern F4's policy 3, nor the public F3 recorder switch.
    kcd2vr::diagnostics::lab::state=2;
    const int legacyId=idCalls51,legacyCarriers=carriers51;
    ck51(vc::SuppressEnqueue114(2),"legacy comparison stays explicitly separate");
    ck51(StartAnimationIdDetour6689(anim.data(),0,params.data())&&idCalls51==legacyId&&carriers51==legacyCarriers,"legacy comparison retains original historical suppression contract");
    std::printf("PASS C51 native event carrier: %d checks; original starts=%d/%d, accepted carriers=%d; audio engine not executed\n",checks51,nameCalls51,idCalls51,carriers51);
}
