// Execute both real animation detours and real FIFO handoff policy. All memory
// is fixture-owned; original calls are counters, never fixed-address game code.
#include "../src/animation_filter.cpp"
#include <cstdlib>
namespace kcd2vr::retail_build112 {
bool hostVerified112=true;
bool Ready(std::uintptr_t module) noexcept {return module&&hostVerified112;}
}
static bool changeCombatSession23=false;
namespace kcd2vr::diagnostic_trace {void Write(const char* line) noexcept {
    if(changeCombatSession23&&line&&std::strstr(line,"V66.90-PLAYBACK SNAP ")){
        changeCombatSession23=false;namespace cap=kcd2vr::diagnostics::capture168;
        cap::gate.select(cap::Domain::Combat,false,hostTickCount64);
        cap::gate.select(cap::Domain::Combat,true,hostTickCount64);
    }
}}
namespace kcd2vr::frik_pose_probe {std::uint64_t PoseEpochH24() noexcept {return 0;} bool NativeFreeKickVisualR24() noexcept {return false;}} // explicit host-only current-body leaf; no live owner installed
using namespace kcd2vr::animation_filter;
static int checks=0,calls=0;
static void ck(bool b,const char* text){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",text);std::exit(1);}}
template<class T>static void put(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static bool originalId(void*,int,void*){++calls;return false;}
static bool originalName(void*,const char*,void*){++calls;return false;}
int main(){
    std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),queue(4+0x78),params(0x40),npc(0x600);
    hostModuleHandleW=image.data();const auto base=WhBase();
    put(anim.data(),base+kCSkeletonAnimVftRva);put(anim.data()+0x4E8,character.data());
    put(character.data()+0xA70,skeleton.data());put(skeleton.data()+0x218,set.data());
    put(set.data()+8,aliases.data()+4);put(aliases.data(),std::uint32_t(2));
    put(aliases.data()+4,std::uint64_t(123));put(aliases.data()+20,std::uint64_t(456));
    g_alias_catalog6694={{123,"combat_free_blk_lngsw_player_over"},{456,"combat_finish_lngsw_01"}};
    g_alias_ready6694=true;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
    g_id_original=&originalId;g_original=&originalName;g_mode=int(SuppressionMode::AllSelectedCombat);
    kcd2vr::diagnostics::lab::state=2;combatPoseTick112=hostTickCount64;
    put(params.data()+0x10,6);
    ck(StartAnimationIdDetour6689(anim.data(),0,params.data())&&calls==0,"ID route suppresses before native enqueue");
    ck(StartAnimationDetour(anim.data(),"combat_free_blk_lngsw_player_over",params.data())&&calls==0,"name route suppresses before enqueue");
    ck(!StartAnimationIdDetour6689(npc.data(),0,params.data())&&calls==1,"NPC keeps original return");
    ck(!StartAnimationIdDetour6689(nullptr,0,params.data())&&calls==2,"null cannot identify Henry");
    kcd2vr::retail_build112::hostVerified112=false;
    ck(!StartAnimationIdDetour6689(anim.data(),0,params.data()),"unverified SHA preserves native ID path");
    ck(!StartAnimationDetour(anim.data(),"combat_free_blk_lngsw_player_over",params.data()),"unverified SHA preserves native NAME path");
    kcd2vr::retail_build112::hostVerified112=true;
    kcd2vr::diagnostics::lab::state=3;
    const int beforeComparison=calls;
    ck(!StartAnimationIdDetour6689(anim.data(),0,params.data())&&calls==beforeComparison+1,"F3=3 keeps native ID enqueue and original return for clip events");
    ck(!StartAnimationDetour(anim.data(),"combat_free_blk_lngsw_player_over",params.data())&&calls==beforeComparison+2,"F3=3 keeps native NAME enqueue for final-pose comparison");
    kcd2vr::diagnostics::lab::state=2;
    put(params.data()+0x10,0);ck(!StartAnimationIdDetour6689(anim.data(),0,params.data()),"full-body layer native");
    put(params.data()+0x10,6);combatPoseTick112=hostTickCount64-101;
    ck(!StartAnimationIdDetour6689(anim.data(),0,params.data()),"tracking/calibration lease expires");
    combatPoseTick112=hostTickCount64;kcd2vr::diagnostics::lab::state=4;
    ck(!StartAnimationIdDetour6689(anim.data(),0,params.data()),"F5 reference revokes cached suppression mode");
    kcd2vr::diagnostics::lab::state=2;
    put(anim.data()+0x48,queue.data()+4);put(queue.data(),std::uint32_t(1));
    put(queue.data()+4+0x10,1);put(queue.data()+4+0x50,std::uint16_t(1));
    ck(NativeBodyRequired112(anim.data()),"active protected native FIFO yields whole body");
    ck(!StartAnimationIdDetour6689(anim.data(),0,params.data()),"ordinary request cannot steal body during paired native action");
    put(queue.data()+4+0x50,std::uint16_t(0));ck(!NativeBodyRequired112(anim.data()),"inactive FIFO entry does not create a timer lease");
    put(queue.data()+4+0x50,std::uint16_t(1));put(queue.data()+4+0x10,999);
    ck(NativeBodyRequired112(anim.data()),"unresolved active ID retains native body");
    // Live V114 reproduction: a layer-4 sword hold was permanently mistaken for
    // mounted HolsterWeapon layer 7. Other protected clips must still win.
    g_alias_catalog6694[1].name="holditem_idle_sword_rhand";
    put(queue.data()+4+0x10,1);put(anim.data()+0x48,static_cast<void*>(nullptr));
    put(anim.data()+4*0x40+0x48,queue.data()+4);
    for(unsigned rung:{2u,3u}){kcd2vr::diagnostics::lab::state=rung;
        ck(!NativeBodyRequired112(anim.data()),"F3 ownership available during ordinary sword-holding idle on layer 4");}
    put(anim.data()+4*0x40+0x48,static_cast<void*>(nullptr));put(anim.data()+7*0x40+0x48,queue.data()+4);
    ck(NativeBodyRequired112(anim.data()),"same alias on mounted holster layer 7 still hands body to native");
    put(anim.data()+7*0x40+0x48,static_cast<void*>(nullptr));put(anim.data()+4*0x40+0x48,queue.data()+4);
    auto protection=queue;put(protection.data()+4+0x10,0);put(anim.data()+0x48,protection.data()+4);
    for(const char* protectedName:{"combat_finish_lngsw_01","combat_rg_sz1_dz2_death_slash_lngsw_01"}){
        g_alias_catalog6694[0].name=protectedName;
        ck(NativeBodyRequired112(anim.data()),"active protected body action overrides holding-idle exception");}
    // R50C1: Henry's own hit-connect clip keeps VR arms in the opt-in hybrid mode.
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=true;
    g_alias_catalog6694[0].name="combat_rg_sz1_dz2_hit_slash_lngsw_01";
    ck(!NativeBodyRequired112(anim.data()),"R50: attack-connect clip no longer hands the body to native");
    kcd2vr::vr_combat111::attackConnectKeepsVrik50.store(false);
    ck(NativeBodyRequired112(anim.data()),"R50: with VrAttackConnectKeepsVrik=false the connect clip is protected again");
    kcd2vr::vr_combat111::attackConnectKeepsVrik50.store(true);g_alias_catalog6694[0].name="combat_finish_lngsw_01";
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=false;
    // V66.178R31: the victim flinch families from the R29 logs keep VRIK (the arms stay on the controllers); Henry's own
    // attack clips containing "hit" (above) do not qualify.
    for(const char* flinch:{"combat_rg_sz1_hitlowmovement_lngsw","combat_hit_small_right_add","hitreaction_idle_medium_head_left"}){
        g_alias_catalog6694[0].name=flinch;
        ck(kcd2vr::vr_combat111::HitReactionAlias178r31(flinch),"R31 hit reaction alias recognised");
        ck(!NativeBodyRequired112(anim.data()),"R31 hit reaction keeps VRIK ownership");}
    ck(!kcd2vr::vr_combat111::HitReactionAlias178r31("combat_rg_sz1_dz2_hit_slash_lngsw_01"),"R31 attack clip with 'hit' is not a reaction");
    ck(!kcd2vr::vr_combat111::HitReactionAlias178r31("combat_clinch_hit_lngsw"),"R31 clinch stays native body");
    g_alias_catalog6694[0].name="combat_rg_sz1_dz2_death_slash_lngsw_01"; // restore a protected action for the checks below (R50: the hit-connect clip is VRIK-kept)
    put(anim.data()+0x48,static_cast<void*>(nullptr));g_playback_layout_ready6690=false;
    ck(NativeBodyRequired112(anim.data()),"unknown FIFO layout remains fail closed");g_playback_layout_ready6690=true;
    ck(NativeBodyRequired112(npc.data()),"wrong animation identity remains fail closed");
    // Execute the actual FIFO guard with F3 disabled, as in the user's F12=7
    // log. Motion's guard must not depend on the visual-suppression hotkey.
    kcd2vr::diagnostics::lab::state=0;
    put(anim.data()+0x48,protection.data()+4);
    ck(!PlayerNativeBodyRequired112(),"F3=0 retains the existing visual ownership policy");
    ck(PlayerNativeBodyRequiredForMotion122(),"F12 motion independently protects active native hit reactions at F3=0");
    g_alias_catalog6694[0].name="combat_finish_lngsw_01";
    ck(PlayerNativeBodyRequiredForMotion122(),"F12 motion independently protects finishers at F3=0");
    put(anim.data()+0x48,static_cast<void*>(nullptr));
    ck(!PlayerNativeBodyRequiredForMotion122(),"ordinary layer-four sword hold can qualify with F3=0");
    put(anim.data()+4*0x40+0x48,static_cast<void*>(nullptr));
    g_alias_catalog6694[1].name="fa_disgust_02";put(anim.data()+7*0x40+0x48,queue.data()+4);
    ck(!PlayerNativeBodyRequiredForMotion122(),"proven facial leaf tracks on layer seven cannot veto F12 body motion");
    put(anim.data()+0x48,protection.data()+4);
    ck(PlayerNativeBodyRequiredForMotion122(),"finisher remains native when facial expression plays alongside it");
    g_alias_catalog6694[0].name="combat_rg_sz1_dz2_death_slash_lngsw_01"; // R50: a still-protected clip
    ck(PlayerNativeBodyRequiredForMotion122(),"hit reaction remains native alongside facial expression");
    put(anim.data()+0x48,static_cast<void*>(nullptr));put(anim.data()+7*0x40+0x48,static_cast<void*>(nullptr));
    put(anim.data()+6*0x40+0x48,queue.data()+4);
    ck(PlayerNativeBodyRequiredForMotion122(),"facial exception cannot qualify a different layer");
    put(anim.data()+6*0x40+0x48,static_cast<void*>(nullptr));
    put(params.data()+0x10,7);const auto beforeFacial=calls;
    ck(!StartAnimationIdDetour6689(anim.data(),1,params.data())&&calls==beforeFacial+1,
       "facial clip itself keeps its original native enqueue and return");
    // C10R4: retail DBA proves fa_open_mouth has facial leaf tracks only. The
    // latest runtime put it on layer1, where it incorrectly vetoed the entire body.
    g_alias_catalog6694[1].name="fa_open_mouth";
    put(anim.data()+1*0x40+0x48,queue.data()+4);
    ck(!PlayerNativeBodyRequiredForMotion122(),"exact fa_open_mouth layer1 keeps motion ownership with F3 off");
    for(unsigned rung:{2u,3u}){kcd2vr::diagnostics::lab::state=rung;
        ck(!NativeBodyRequired112(anim.data()),"exact fa_open_mouth layer1 cannot veto either VR visual owner");}
    put(anim.data()+0x48,protection.data()+4);
    for(const char* protectedName:{"combat_finish_lngsw_01","combat_rg_sz1_dz2_death_slash_lngsw_01",
                                 "combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master"}){
        g_alias_catalog6694[0].name=protectedName;
        ck(NativeBodyRequired112(anim.data()),"paired/death/finisher wins alongside mouth facial clip");}
    put(anim.data()+0x48,static_cast<void*>(nullptr));
    put(anim.data()+1*0x40+0x48,static_cast<void*>(nullptr));
    put(anim.data()+2*0x40+0x48,queue.data()+4);
    ck(NativeBodyRequired112(anim.data()),"mouth exception does not extend to an unobserved layer");
    put(anim.data()+2*0x40+0x48,static_cast<void*>(nullptr));
    put(anim.data()+1*0x40+0x48,queue.data()+4);
    ck(!kcd2vr::vr_combat111::BodylessFacial126("fa_open_mouth_other",1)&&
       !kcd2vr::vr_combat111::BodylessFacial126(nullptr,1),"mouth exception is exact and null safe");
    // R108: the combat-stance torch hold (kcd_male_database.adb HoldItemLeft "l_torch+combat+player" ->
    // combat_idle01_shsw_torch_over, layer 3) keeps the VR body; the same clip on a FullBody layer (the
    // CombatAttackMercy overlay, layer 1) stays native.
    put(anim.data()+1*0x40+0x48,static_cast<void*>(nullptr));
    g_alias_catalog6694[1].name="combat_idle01_shsw_torch_over";
    put(anim.data()+3*0x40+0x48,queue.data()+4);
    ck(!NativeBodyRequired112(anim.data()),"R108 combat torch hold on HoldItemLeft layer 3 keeps the VR body (sword+torch fights)");
    put(anim.data()+3*0x40+0x48,static_cast<void*>(nullptr));
    put(anim.data()+1*0x40+0x48,queue.data()+4);
    ck(NativeBodyRequired112(anim.data()),"R108 the same torch overlay on a FullBody layer stays native");
    g_alias_catalog6694[1].name="fa_open_mouth";
    kcd2vr::retail_build112::hostVerified112=false;
    ck(NativeBodyRequired112(anim.data()),"mouth exception cannot bypass retail fingerprint");
    kcd2vr::retail_build112::hostVerified112=true;
    put(anim.data()+1*0x40+0x48,static_cast<void*>(nullptr));
    kcd2vr::diagnostics::lab::state=2;put(params.data()+0x10,1);const auto beforeMouth=calls;
    ck(!StartAnimationIdDetour6689(anim.data(),1,params.data())&&calls==beforeMouth+1,
       "mouth ID animation continues native playback and return");
    ck(!StartAnimationDetour(anim.data(),"fa_open_mouth",params.data())&&calls==beforeMouth+2,
       "mouth NAME animation continues native playback and return");
    kcd2vr::diagnostics::lab::state=0;
    g_playback_layout_ready6690=false;
    ck(PlayerNativeBodyRequiredForMotion122(),"motion fails closed on unknown FIFO layout even with F3=0");g_playback_layout_ready6690=true;
    std::uint64_t rawHash=0;
    ck(AliasHashForId113(set.data(),1,rawHash)&&rawHash==456,"raw alias hash remains observable independently of catalogue resolution");
    ck(!AliasHashForId113(set.data(),-1,rawHash)&&!AliasHashForId113(set.data(),2,rawHash),"raw alias recorder rejects invalid ID bounds");
    // The user has no tracked feet. An ordinary native kick keeps its authored
    // lower body without handing the upper body to an arbitrary animation timer.
    kcd2vr::diagnostics::lab::state=3;
    kcd2vr::diagnostics::capture168::gate.mask=0;
    put(anim.data()+0x48,queue.data()+4);put(queue.data()+4+0x10,1);
    put(queue.data()+4+0x50,std::uint16_t(1));
    g_alias_catalog6694[1].name="combat_rg_sz1_dz0_hit_slash_lngsw_01";
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=false;
    ck(NativeBodyRequired112(anim.data()),"attack-connect exception is off outside F4 rung 2");
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=true;
    ck(!NativeBodyRequired112(anim.data()),"ordinary Henry attack-connect keeps VR arms in F4 rung 2");
    for(const char* unarmed:{"combat_lg_sz2_dz0_hit_nwpn","combat_lg_sz2_dz1_hit_nwpn","combat_lg_sz2_dz2_hit_nwpn",
        "combat_lg_sz2_dz5_hit_nwpn","combat_lg_sz2_dz5_hit_nwpn_nwpn","combat_rg_sz1_dz0_hit_nwpn",
        "combat_rg_sz1_dz1_hit_nwpn","combat_rg_sz1_dz2_hit_nwpn","combat_rg_sz1_dz5_hit_nwpn",
        "combat_rg_sz5_dz0_hit_nwpn","combat_rg_sz5_dz1_hit_nwpn","combat_rg_sz5_dz2_hit_nwpn",
        "combat_rg_sz5_dz5_hit_nwpn","combat_rg_sz5_dz5_hit_nwpn_nwpn"}){
        g_alias_catalog6694[1].name=unarmed;
        ck(!NativeBodyRequired112(anim.data()),"D56 exact retail unarmed normal attack-connect preserves VR arms");
    }
    for(const char* denied:{"combat_rg_sz1_dz0_hit_nwpn_slave","combat_rg_sz1_dz0_hit_nwpn_master",
        "combat_rg_sz1_dz0_hit_nwpn_kill","combat_rg_sz1_dz0_hit_nwpn.bspace","combat_rg_sz3_dz0_hit_nwpn"})
        ck(!kcd2vr::vr_combat111::OwnAttackConnectAlias50(denied),"D56 unarmed exception cannot admit paired/kill/suffix/unknown aliases");
    for(const char* protectedClip:{"combat_rg_sz1_dz0_hit_slash_lngsw_master","combat_mounted_sz1_dz0_hit_slash_lngsw","combat_rg_sz1_dz0_hit_stab_kill"}){
        ck(!kcd2vr::vr_combat111::OwnAttackConnectAlias50(protectedClip),"protected name cannot qualify as ordinary attack-connect");
    }
    for(const char* protectedClip:{"combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master","1d_bow_horse_walk_bwd","combat_finish_lngsw_01"}){
        g_alias_catalog6694[1].name=protectedClip;
        ck(NativeBodyRequired112(anim.data()),"paired, mounted and kill actions cannot borrow ordinary attack-connect ownership");
    }
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=false;
    for(const char* kick:{"1d-combat_rg_sz5_az5_natk_kick_nwpn",
                         "1d-combat_rg_z5_natksw_dog_kick_nwpn",
                         "1d-combat_rg_sz5_az5_natk_kick_nwpn.bspace",
                         "1d-combat_rg_z5_natksw_dog_kick_nwpn.bspace",
                         "combat_rg_sz5_az5_natk_kick_nwpn_add"}){
        g_alias_catalog6694[1].name=kick;nativeKickActive175=false;
        kcd2vr::kick26::nativeClipStart=0;kcd2vr::vr_combat111::hideSwingLean38=false;
        ck(!NativeBodyRequired112(anim.data())&&NativeKickActive175(),
           "exact active native kick, including authored .bspace, selects lower-body handover with diagnostics and lean preference OFF");
        ck(kcd2vr::kick26::nativeClipStart.load()==hostTickCount64,
           "actual native FIFO observer stamps a real kick-start edge for each exact alias form");
    }
    kcd2vr::vr_combat111::hideSwingLean38=true;
    for(const char* unrelated:{"1d-combat_rg_sz5_az5_natk_kick_nwpn_evil.bspace",
         "1d-combat_rg_sz5_az5_natk_kick_nwpn.bspace.extra","1d-combat_rg_sz5_az5_natk_kick_nwpn.caf",
         "combat_rg_sz5_az5_natk_kick_nwpn_add.bspace","combat_door_kick_unrelated.bspace"}){
        g_alias_catalog6694[1].name=unrelated;
        ck(!NativeBodyRequired112(anim.data())&&!NativeKickActive175(),"unknown alias or unproven suffix cannot borrow ordinary native kick ownership");
    }
    g_alias_catalog6694[1].name="1d-combat_rg_sz5_az5_natk_kick_nwpn.bspace";
    ck(NativeBodyRequired112(npc.data())&&!NativeKickActive175(),"authored suffix cannot grant native kick ownership to another animation instance");
    put(queue.data()+4+0x50,std::uint16_t(0));
    ck(!NativeBodyRequired112(anim.data())&&!NativeKickActive175(),"native kick handover ends with active FIFO bit, without timer");
    put(queue.data()+4+0x50,std::uint16_t(1));g_alias_catalog6694[1].name="combat_door_kick_unrelated";
    ck(!NativeBodyRequired112(anim.data())&&!NativeKickActive175(),"arbitrary kick substring cannot borrow ordinary combat kick policy");
    g_alias_catalog6694[1].name="combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master";
    ck(NativeBodyRequired112(anim.data())&&!NativeKickActive175(),"paired clinch kick keeps complete native body ownership");
    g_alias_catalog6694[1].name="1d-combat_rg_sz5_az5_natk_kick_nwpn";
    ck(!NativeBodyRequired112(anim.data())&&NativeKickActive175(),"ordinary kick can become active again");
    put(queue.data()+4+0x10,999);
    ck(NativeBodyRequired112(anim.data())&&!NativeKickActive175(),"unknown FIFO clears selective kick ownership and retains native body");
    put(anim.data()+0x48,static_cast<void*>(nullptr));
    // Exercise the actual worker service with a completed hash batch. This
    // fixture never executes the fixed-address native hashing function.
    g_alias_ready6694=false;g_alias_fault6694=false;g_start_diag_level=0;
    kcd2vr::diagnostics::lab::state=0;kcd2vr::diagnostics::combat_capture::until=0;
    g_alias_cursor6694=std::size(kcd2vr::animation_aliases::names);
    g_alias_catalog6694={{456,"combat_finish_lngsw_01"},{123,"combat_free_blk_lngsw_player_over"}};
    ServiceAliasCatalog(false);
    ck(!g_alias_ready6694,"V125R1 idle worker without alias consumers remains lazy");
    ServiceAliasCatalog(true);
    ck(g_alias_ready6694&&g_alias_catalog6694[0].hash==123,"V125R1 F12-only demand publishes the hashed catalogue with F3/F5 off");
    ck(kcd2vr::diagnostics::lab::state==0,"V125R1 alias demand does not enable animation suppression or diagnostic modes");
    g_alias_ready6694=false;g_alias_fault6694=true;ServiceAliasCatalog(true);
    ck(!g_alias_ready6694,"V125R1 demand cannot bypass a catalogue/hash fault");
    // Actual existing FIFO sampler -> copied cache -> skinning bridge API.
    namespace cap23=kcd2vr::diagnostics::capture168;
    hostTickCount64=60000;g_alias_ready6694=true;g_alias_fault6694=false;
    g_playback_layout_ready6690=true;g_streams6690[0]={};
    g_alias_catalog6694={{123,"combat_free_blk_lngsw_player_over"},{456,"1d-combat_rg_sz5_az5_natk_kick_nwpn.bspace"}};
    put(anim.data()+0x48,queue.data()+4);put(queue.data()+4+0x10,1);
    put(queue.data()+4+0x50,std::uint32_t(1));put(queue.data()+4+0x44,.607920f);
    put(queue.data()+4+0x16,std::uint8_t(2));
    cap23::gate.select(cap23::Domain::Combat,true,hostTickCount64);
    SamplePlayback6690(anim.data(),true);KickPlaybackOutput23 copied{};
    ck(ReadKickPlaybackOutput23(anim.data(),copied)&&copied.id==1&&copied.layer==0&&copied.fifo==0&&
       copied.segment==2&&copied.segmentTime==.607920f&&copied.tick==hostTickCount64&&copied.serial&&
       std::strcmp(copied.name,"1d-combat_rg_sz5_az5_natk_kick_nwpn.bspace")==0,
       "existing real FIFO sampler/name cache yields exact copied kick identity and phase, no new query");
    ck(!ReadKickPlaybackOutput23(npc.data(),copied)&&copied.id==-1,"other character cannot borrow current Henry cached clip");
    const auto savedEpoch=g_streams6690[0].epoch;g_streams6690[0].epoch++;
    ck(!ReadKickPlaybackOutput23(anim.data(),copied),"replaced playback epoch is unknown");g_streams6690[0].epoch=savedEpoch;
    hostTickCount64+=501;ck(!ReadKickPlaybackOutput23(anim.data(),copied),"cached clip older than500ms is unknown, not current FIFO claim");
    SamplePlayback6690(anim.data(),true);ck(ReadKickPlaybackOutput23(anim.data(),copied),"next real native callback refreshes existing cached phase");
    cap23::gate.select(cap23::Domain::Combat,false,hostTickCount64);cap23::gate.select(cap23::Domain::Interaction,true,hostTickCount64);
    ck(!ReadKickPlaybackOutput23(anim.data(),copied),"F5 alone cannot enable F3-only kick output cache reads");
    cap23::gate.select(cap23::Domain::Combat,true,hostTickCount64);
    ck(!ReadKickPlaybackOutput23(anim.data(),copied),"new F3 lease does not borrow preceding session cache");
    hostTickCount64+=500;SamplePlayback6690(anim.data(),true);
    ck(ReadKickPlaybackOutput23(anim.data(),copied),"existing sampler explicitly joins new current F3 session");
    hostTickCount64+=1000;changeCombatSession23=true;SamplePlayback6690(anim.data(),true);
    ck(!changeCombatSession23&&!ReadKickPlaybackOutput23(anim.data(),copied),"lease replaced during native sample cannot relabel old cache as new-session observation");
    hostTickCount64+=500;SamplePlayback6690(anim.data(),true);
    ck(ReadKickPlaybackOutput23(anim.data(),copied),"later native callback naturally refreshes current session after replacement");
    g_streams6690[0].names={};
    ck(!ReadKickPlaybackOutput23(anim.data(),copied),"unresolved name remains unknown without resolving an extra native dictionary call");
    cap23::gate.select(cap23::Domain::Combat,false,hostTickCount64);cap23::gate.select(cap23::Domain::Interaction,false,hostTickCount64);
    hostTickCount64+=1000;cap23::gate.select(cap23::Domain::Hands,true,hostTickCount64);
    SamplePlayback6690(anim.data(),true);HandPlaybackOutput28 oldHands28{};
    ck(g_streams6690[0].lastSample==hostTickCount64&&!ReadHandPlaybackOutput28(anim.data(),oldHands28),
       "current Hands capture group records interaction FIFO without granting HandReference cache authority");
    cap23::gate.select(cap23::Domain::HandReference,true,hostTickCount64);
    HandPlaybackOutput28 hand28{};SamplePlayback6690(anim.data(),true);
    ck(ReadHandPlaybackOutput28(anim.data(),hand28)&&hand28.count==1&&hand28.clips[0].id==1&&hand28.clips[0].segment==2&&hand28.clips[0].segmentTime==.607920f&&hand28.tick==hostTickCount64,"Hands-only actual sampled native FIFO yields current independently stamped phase and alias cache");
    ck(!ReadHandPlaybackOutput28(npc.data(),hand28),"Hands clip cache cannot cross native actor");
    hostTickCount64+=99;const auto sample28=g_streams6690[0].lastSample;SamplePlayback6690(anim.data(),true);ck(g_streams6690[0].lastSample==sample28,"Hands sampler makes no extra native FIFO observation before100ms");
    hostTickCount64+=1;SamplePlayback6690(anim.data(),true);ck(g_streams6690[0].lastSample==hostTickCount64,"Hands sampler observes next actual native callback at100ms");
    cap23::gate.select(cap23::Domain::HandReference,false,hostTickCount64);ck(!ReadHandPlaybackOutput28(anim.data(),hand28),"Shift recorder OFF invalidates copied phase immediately");
    cap23::gate.select(cap23::Domain::HandReference,true,hostTickCount64);ck(!ReadHandPlaybackOutput28(anim.data(),hand28),"new Hands lease cannot borrow old FIFO cache");
    SamplePlayback6690(anim.data(),true);ck(ReadHandPlaybackOutput28(anim.data(),hand28),"same native owner naturally refreshes new Hands lease");
    std::printf("PASS %d production animation ownership checks\n",checks);
    return 0;
}
