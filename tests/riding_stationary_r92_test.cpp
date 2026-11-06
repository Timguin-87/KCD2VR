// Actual animation-filter TU through its existing native-memory host fixture.
#define main retained_d81_main
#include "riding_handoff_d81_test.cpp"
#undef main
int main(){
 if(retained_d81_main())return 1;
 namespace ride=kcd2vr::riding_vrik_d76;
 std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(4+2*16),idle(4+0x78),transfer(4+0x78),params(0x40);
 hostModuleHandleW=image.data();const auto base=WhBase();
 putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
 putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
 putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(2));
 putD50(aliases.data()+4,std::uint64_t(100));putD50(aliases.data()+20,std::uint64_t(101));
 g_alias_catalog6694={{100,"relaxed_idle_rider_01"},{101,"horse_mount_left_02"}};
 g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
 g_id_original=&originalIdD50;g_original=&originalNameD50;
 g_mode=int(SuppressionMode::AllSelectedCombat);kcd2vr::diagnostics::lab::state=3;
 kcd2vr::diagnostics::capture168::gate.mask=0;
 putD50(idle.data(),std::uint32_t(1));putD50(idle.data()+4+0x10,0);putD50(idle.data()+4+0x50,std::uint16_t(1));
 putD50(transfer.data(),std::uint32_t(1));putD50(transfer.data()+4+0x10,1);putD50(transfer.data()+4+0x50,std::uint16_t(1));
 putD50(anim.data()+0x48,idle.data()+4);putD50(params.data()+0x10,0);
 bool riding=false;
 for(const auto* alias:{"relaxed_idle_rider_01","relaxed_idle_rider_01_horse","relaxed_idle_rider_01_uphill",
                      "relaxed_idle_rider_01_var1","relaxed_idle_rider_01_var1_horse",
                      "relaxed_idle_rider_01.caf","relaxed_idle_rider_01_horse.caf","relaxed_idle_rider_01_uphill.caf",
                      "relaxed_idle_rider_01_var1.caf","relaxed_idle_rider_01_var1_horse.caf"}){
  g_alias_catalog6694[0].name=alias;ride::Set(false);
  ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"riding OFF leaves every exact stationary variant native");
  ride::Set(true);hostTickCount64+=2000;
  putD50(anim.data()+0x40+0x48,transfer.data()+4);g_alias_catalog6694[1].name="horse_mount_left_02";
  ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"stationary idle cannot override climbing up");
  putD50(anim.data()+0x40+0x48,static_cast<void*>(nullptr));
  ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"stationary completed seat starts settle without requiring movement");
  hostTickCount64+=1000;
  ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"stationary rider admits normal VRIK without any gait or speed");
  const auto before=callsD50;StartAnimationIdDetour6689(anim.data(),0,params.data());
  ckD50(callsD50==before+1,"stationary native clip and events still execute");
  g_alias_catalog6694[0].name="2d_horse_walk.bspace";
  ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"walking does not restart deadline");
  g_alias_catalog6694[0].name=alias;
  ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"stopping does not release upper body");
  for(const auto* protectedAlias:{"horse_dismount_left_02","horse_dismount_left_02_align","combat_finish_lngsw_01"}){
   g_alias_catalog6694[1].name=protectedAlias;putD50(anim.data()+0x40+0x48,transfer.data()+4);
   ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"dismount and protected actions retain native ownership");
  }
  putD50(anim.data()+0x40+0x48,static_cast<void*>(nullptr));
 }
 for(const auto* other:{"relaxed_idle_rider_reading","relaxed_idle_rider_01_extra","horse_mount_left_02","horse_dismount_left_02"})
  ckD50(!ride::Gait(other,0),"no wildcard permission for reading, unknown or transfers");
 ckD50(!ride::Gait("relaxed_idle_rider_01",1),"unproven layer cannot create mounted context");
 ckD50(kcd2vr::diagnostics::capture168::gate.mask.load()==0,"feature never enables diagnostics");
 std::printf("R92 stationary riding %u/%u; native leaves synthetic; headset NOT_RUN\n",checksD50-failuresD50,checksD50);
 return failuresD50?1:0;
}
