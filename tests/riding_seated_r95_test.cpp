// Actual animation-filter TU and the current shared-frame implementation.
// Native FIFO/identity/ticks are explicit host leaves; no game/GPU claim.
#define main retained_d81_r95_main
#include "riding_handoff_d81_test.cpp"
#undef main
#include "../src/vrik/tracked_frame51.h"
int main(){
 if(retained_d81_r95_main())return 1;
 namespace ride=kcd2vr::riding_vrik_d76;namespace f=kcd2vr::tracked_frame51;
 std::vector<unsigned char> image(0x1000),anim(0x600),character(0xB00),skeleton(0x300),set(0x40),aliases(4+2*16),seat(4+0x78),action(4+0x78),params(0x40);
 hostModuleHandleW=image.data();const auto base=WhBase();
 putD50(anim.data(),base+kCSkeletonAnimVftRva);putD50(anim.data()+0x4E8,character.data());
 putD50(character.data()+0xA70,skeleton.data());putD50(skeleton.data()+0x218,set.data());
 putD50(set.data()+8,aliases.data()+4);putD50(aliases.data(),std::uint32_t(2));
 putD50(aliases.data()+4,std::uint64_t(100));putD50(aliases.data()+20,std::uint64_t(101));
 g_alias_catalog6694={{100,"2d_combat_rg_horse_walk_sword.bspace"},{101,"combat_horse_rg_natksw_start_lngsw"}};
 g_alias_ready6694=true;g_alias_fault6694=false;g_playback_layout_ready6690=true;g_player_skeleton_anim=anim.data();
 g_id_original=&originalIdD50;g_original=&originalNameD50;g_mode=int(SuppressionMode::AllSelectedCombat);
 kcd2vr::diagnostics::lab::state=3;ride::Set(true);g_riding_session_d81={};hostTickCount64=30000;
 putD50(seat.data(),std::uint32_t(1));putD50(seat.data()+4+0x10,0);putD50(seat.data()+4+0x50,std::uint16_t(1));
 putD50(action.data(),std::uint32_t(1));putD50(action.data()+4+0x10,1);putD50(action.data()+4+0x50,std::uint16_t(1));
 putD50(anim.data()+0x48,seat.data()+4);putD50(anim.data()+6*0x40+0x48,action.data()+4);
 bool riding=false;std::uint64_t first=0;
 ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"first observed seated save starts one-second settle");
 hostTickCount64+=1000;
 for(const auto* name:{"2d_combat_rg_horse_walk_sword.bspace","2d_combat_rg_horse_trot_sword.bspace",
                     "horse_combat_rg_idle_lngsw","horse_combat_rg_gallop_to_fast_stop_shld","crossbow_horse_loaded_idle_rider_01"}){
  g_alias_catalog6694[0].name=name;
  std::uint64_t observed=0;
  ckD50(!NativeBodyRequired112(anim.data(),&riding,&observed)&&riding,"seated sword/ranged gait and combat keep full mounted context");
  ckD50(observed&&observed==ride::lifetimeR95.load(),"session lifetime is captured with the successful FIFO decision");
  const auto token=ride::lifetimeR95.load();if(!first)first=token;
  ckD50(token==first,"seat gait/idle/attack transitions do not reseed one mounted lifetime");
  const auto before=callsD50;putD50(params.data()+0x10,0);StartAnimationIdDetour6689(anim.data(),0,params.data());
  ckD50(callsD50==before+1,"seated native clips still reach original and retain gameplay events");
 }
 for(const auto* transfer:{"horse_mount_left_02","horse_mount_right_01_align","horse_dismount_hlbrd_right_01","horse_dismount_unsaddled_left_01_align"}){
  g_alias_catalog6694[1].name=transfer;const auto prior=ride::lifetimeR95.load();
  std::uint64_t observed=99;
  ckD50(NativeBodyRequired112(anim.data(),&riding,&observed)&&!riding&&!observed,"every exact mount/dismount transfer wins and clears its old lifetime");
  ckD50(ride::lifetimeR95.load()!=prior,"transfer retires prior seat reference");
  g_alias_catalog6694[1].name="combat_horse_rg_natksw_start_lngsw";
  ckD50(NativeBodyRequired112(anim.data(),&riding)&&!riding,"remount starts a current-seat settle");
  hostTickCount64+=1000;
  ckD50(!NativeBodyRequired112(anim.data(),&riding)&&riding,"remount uses current seat with no permanently latched native fallback");
 }
 g_alias_catalog6694[0].name="relaxed_idle_both_player";g_alias_catalog6694[1].name="fa_run_01";
 ckD50(!NativeBodyRequired112(anim.data(),&riding)&&!riding,"standing body does not inherit prior mounted context");
 for(const auto* bad:{"horse_combat_rg_idle_lngsw_unknown","horse_mount_left_02_extra","quest_petting_horse_loop","horserider_fall_left","combat_horserider_take_down_lngsw_left_m"})
  ckD50(!ride::SeatedR95(bad,0),"no wildcard seated authority for unknown, transfers, nearby horses or unseating");
 // The captured seated native pelvis was0.0863m in its own model frame. HMD
 // standing-Z mode rejects that >45cm difference; the exact mounted frame must
 // use current seat height and never borrow calibration/previous mount height.
 f::Input in{};in.stamp={8,218602,484997156,0x100};in.control=2;in.allowed=true;in.hmdHeight=true;
 in.restHead={0,.003922,1.55418};in.restPelvis={0,0,.95};in.calibratedHead={-.0051,-.0112,1.5163};
 in.nativePelvis={0,0,.0863};in.nativeHead={0,0,.7114};f::State st{};f::Frame out{};
 ckD50(!f::advance(st,in,out),"captured mounted basis reproduces standing-height residual refusal");
 in.hmdHeight=false;in.referenceDomainD86=f::ReferenceDomainD86::Mounted;in.referenceLifetimeD86=10;
 ckD50(f::advance(st,in,out)&&out.valid&&!out.neutral,"mounted current seat admits shared frame without standing donor");
 const double seatedZ=out.anchor.z;const auto calibration=in.calibratedHead;
 ckD50(std::abs(seatedZ-(.0863+1.55418-.95))<1e-9,"seat height is native pelvis plus fixed rest torso anatomy");
 // Same pointers and XR stamp, but remount/native model origin has shifted2m.
 // Fresh lifetime must defeat duplicate/filter history; world seat stays equal.
 in.smooth=true;in.nativePelvis.z-=2;in.nativeHead.z-=2;in.referenceLifetimeD86=11;
 ckD50(f::advance(st,in,out)&&std::abs(out.anchor.z+2-seatedZ)<1e-9,"remount rebases current local seat instead of leaving body two metres above it");
 ckD50(f::m::length(in.calibratedHead-calibration)==0,"remount never resets saved calibration");
 in.referenceDomainD86=f::ReferenceDomainD86::Ordinary;in.referenceLifetimeD86=0;in.hmdHeight=true;
 in.nativePelvis=in.restPelvis;in.nativeHead=in.restHead;
 ckD50(f::advance(st,in,out)&&std::abs(out.anchor.z-in.restHead.z)<1e-9,"dismount restores ordinary height in same source stamp");
 ride::SessionD81 session{};ride::ObservationD81 obs{0x100,ride::mode.load(),50000,true,false,false,true,false,8};
 ckD50(!ride::AdvanceD81(session,obs),"loaded seated save starts settle without requiring a previous mount animation");
 obs.now+=1000;ckD50(ride::AdvanceD81(session,obs),"loaded seated save hands over after settle");
 auto token=ride::lifetimeR95.load();++obs.bodyEpochR95;
 ckD50(!ride::AdvanceD81(session,obs)&&ride::lifetimeR95.load()!=token,"same pointer world reload starts a new seat on body epoch");
 obs.now+=1000;ckD50(ride::AdvanceD81(session,obs),"changed body epoch recovers after settle");
 token=ride::lifetimeR95.load();++obs.actor;
 ckD50(!ride::AdvanceD81(session,obs)&&ride::lifetimeR95.load()!=token,"replacement actor cannot reuse seat lifetime");
 obs.now+=1000;ckD50(ride::AdvanceD81(session,obs),"replacement actor recovers after settle");
 obs.qualified=false;ckD50(!ride::AdvanceD81(session,obs),"lost native evidence withdraws mounted owner");
 obs.qualified=true;ckD50(!ride::AdvanceD81(session,obs),"next qualified loaded seat starts bounded recovery");
 obs.now+=1000;ckD50(ride::AdvanceD81(session,obs),"next qualified loaded seat recovers automatically");
 std::printf("R95 seated ownership/frame %u/%u; native leaves synthetic; headset NOT_RUN\n",checksD50-failuresD50,checksD50);
 return failuresD50?1:0;
}
