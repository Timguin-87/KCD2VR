// Full production animation TU through the retained D50 fixture.
#define main retained_d50_main
#include "riding_ownership_d50_test.cpp"
#undef main
int main(){
 if(retained_d50_main())return 1;
 namespace ride=kcd2vr::riding_vrik_d76;
 std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(0x40),queue(4+0x78),other(4+0x78),params(0x40),npc(0x600);
 hostModuleHandleW=image.data();const auto base=WhBase();
 putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
 putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
 putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(2));
 putD50(aliases.data()+4,std::uint64_t(123));putD50(aliases.data()+20,std::uint64_t(456));
 g_alias_catalog6694={{123,"combat_finish_lngsw_01"},{456,"2d_horse_walk.bspace"}};
 g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
 g_id_original=&originalIdD50;g_original=&originalNameD50;g_mode=int(SuppressionMode::AllSelectedCombat);
 kcd2vr::diagnostics::lab::state=3;kcd2vr::diagnostics::capture168::gate.mask=0;combatPoseTick112=hostTickCount64;
 putD50(queue.data(),std::uint32_t(1));putD50(queue.data()+4+0x10,1);putD50(queue.data()+4+0x50,std::uint16_t(1));
 putD50(other.data(),std::uint32_t(1));putD50(other.data()+4+0x10,0);putD50(other.data()+4+0x50,std::uint16_t(1));
 putD50(anim.data()+0x48,queue.data()+4);putD50(params.data()+0x10,0);
 bool arms=true;ride::Set(false);
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"OFF leaves ordinary mounted native owner");
 ride::Set(true);const auto enabled=ride::mode.load();ride::Set(true);
 ckD50(ride::mode.load()==enabled,"same setting does not reset generation");
 for(const auto* gait:{"2d_horse_walk","2d_horse_trot","2d_horse_canter","2d_horse_gallop","2d_horse_walk.bspace","2d_horse_trot.bspace","2d_horse_canter.bspace","2d_horse_gallop.bspace"}){
  g_alias_catalog6694[1].name=gait;arms=false;
  ckD50(NativeBodyRequired112(anim.data(),&arms)&&arms,"all and only current proven gait admits visible arms");
  ckD50(NativeBodyRequired112(anim.data()),"global/body/native trajectory caller still sees native ownership");
  const auto before=callsD50;StartAnimationIdDetour6689(anim.data(),1,params.data());
  ckD50(callsD50==before+1,"riding option never suppresses native gait enqueue");
  putD50(queue.data()+4+0x50,std::uint16_t(0));
  ckD50(!NativeBodyRequired112(anim.data(),&arms)&&!arms,"inactive entry immediately retires arm witness");
  putD50(queue.data()+4+0x50,std::uint16_t(1));
 }
 // D78R2: actual captured stationary owner and same-layer idle/gait overlap.
 g_alias_catalog6694[1].name="relaxed_idle_rider_01_horse";
 ride::Set(false);arms=true;
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"stationary riding remains native while preference is OFF");
 ride::Set(true);arms=false;
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&arms,"D78R2 observed stationary rider admits arms when selected");
 {
  std::vector<unsigned char> transition(4+2*0x78);
  putD50(transition.data(),std::uint32_t(2));
  putD50(transition.data()+4+0x50,std::uint16_t(1));putD50(transition.data()+4+0x78+0x50,std::uint16_t(1));
  putD50(anim.data()+0x48,transition.data()+4);
  g_alias_catalog6694[0].name="relaxed_idle_rider_01_horse";
  for(const auto* gait:{"2d_horse_walk.bspace","2d_horse_trot.bspace","2d_horse_canter.bspace"}){
   g_alias_catalog6694[1].name=gait;
   for(int first:{0,1}){
    putD50(transition.data()+4+0x10,first);putD50(transition.data()+4+0x78+0x10,1-first);arms=false;
    ckD50(NativeBodyRequired112(anim.data(),&arms)&&arms,"D78R2 idle/gait transition order cannot drop current arm permission");
   }
  }
  g_alias_catalog6694[0].name="combat_finish_lngsw_01";
  putD50(anim.data()+0x48,queue.data()+4);
 }
 // The observed mount/dismount pair owns native movement even with no gait.
 for(const auto* transfer:{"horse_mount_left_02","horse_mount_left_02_align","horse_dismount_left_02","horse_dismount_left_02_align"}){
  g_alias_catalog6694[1].name=transfer;
  for(bool enabledNow:{false,true}){
   ride::Set(enabledNow);arms=true;
   ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"D78R2 observed transfer retains native body independently of riding preference");
   const auto before=callsD50;StartAnimationIdDetour6689(anim.data(),1,params.data());
   ckD50(callsD50==before+1,"mount/dismount clip and its gameplay events remain enqueued");
  }
 }
 ride::Set(true);
 for(const auto* unknown:{"horse_mount_left_02_extra","horse_mount_left_02.caf","horse_dismount_left_02_extra"})
  ckD50(!kcd2vr::vr_combat111::NativeBodyAlias112(unknown),"transfer protection does not add an alias wildcard");
 g_alias_catalog6694[1].name="2d_horse_walk.bspace";
 putD50(anim.data()+0x40+0x48,other.data()+4);
 for(const auto* alias:{"horse_mount_left_02","horse_mount_left_02_align","horse_dismount_left_02","horse_dismount_left_02_align","combat_finish_lngsw_01","combat_clinch_sz2_az1_attacksync_kick_step2_nwpn_master","combat_rg_sz1_dz2_death_slash_lngsw_01"}){
  g_alias_catalog6694[0].name=alias;arms=true;
  ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"concurrent protected action after gait cannot borrow its witness");
 }
 putD50(anim.data()+0x40+0x48,static_cast<void*>(nullptr));
 putD50(anim.data()+0x48,other.data()+4);putD50(anim.data()+0x40+0x48,queue.data()+4);
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"protected owner before gait wins");
 putD50(anim.data()+0x48,static_cast<void*>(nullptr));
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"gait on unproven layer is native");
 putD50(anim.data()+0x40+0x48,static_cast<void*>(nullptr));putD50(anim.data()+0x48,queue.data()+4);
 putD50(queue.data()+4+0x10,4);
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"unknown active alias fails closed");putD50(queue.data()+4+0x10,1);
 kcd2vr::retail_build112::verifiedD50=false;
 ckD50(NativeBodyRequired112(anim.data(),&arms)&&!arms,"retail mismatch retires witness");kcd2vr::retail_build112::verifiedD50=true;
 ckD50(NativeBodyRequired112(npc.data(),&arms)&&!arms,"another skeleton cannot borrow Henry permission");
 for(unsigned native:{0u,1u}){kcd2vr::diagnostics::lab::state=native;arms=true;
  NativeBodyRequired112(anim.data(),&arms);ckD50(!arms,"native/reference policy never enables riding arms");}
 kcd2vr::diagnostics::lab::state=3;ride::Set(false);ride::Set(true);
 ckD50(ride::mode.load()!=enabled,"toggle roundtrip retires already captured witness generation");
 ckD50(kcd2vr::diagnostics::capture168::gate.mask.load()==0,"feature does not enable diagnostics");
 ride::Set(false);std::printf("D76 riding actual native owner %u/%u; native leaves mocked, headset NOT_RUN\n",checksD50-failuresD50,checksD50);
 return failuresD50?1:0;
}
