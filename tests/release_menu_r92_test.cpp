#include "vr_menu.h"
#include "release_menu_r92.h"
#include "vrik/motion_trials_d67.h"
#include "subsystems/base/startup_graphics_d80.h"
#include <cstdio>
#include <cstdlib>
int main(){
 namespace r=kcd2vr::release_menu_r92;namespace l=kcd2vr::live_settings_d39;
 int n=0;auto check=[&](bool ok){++n;if(!ok){std::printf("FAIL %d\n",n);std::exit(1);}};
 kcd2vr::VrMenuState s{};s.stereo_mode=3;s.camera_back_offset_m=.123f;s.camera_height_offset_m=.234f;
 s.contextual_hud=false;s.vr_pad_glyph_style=2;s.alchemy_mode67=0;s.smithing_mode67=0;s.activity_hands59c7f[0]=false;s.activity_hands59c7f[1]=false;
 check(r::advance(s,r::Row::Alchemy)&&s.alchemy_mode67==3&&s.activity_hands59c7f[0]);
 check(r::advance(s,r::Row::Alchemy)&&s.alchemy_mode67==1);
 check(r::advance(s,r::Row::Smith)&&s.smithing_mode67==3&&s.activity_hands59c7f[1]&&s.smith_auto_hold36);
 check(r::advance(s,r::Row::Smith)&&s.smithing_mode67==1);
 check(r::advance(s,r::Row::Hud)&&s.contextual_hud);
 check(r::advance(s,r::Row::Glyphs)&&s.vr_pad_glyph_style==4);
 check(r::advance(s,r::Row::Glyphs)&&s.vr_pad_glyph_style==2);
 s.hmd_body_height54=false;check(r::advance(s,r::Row::Crouch)&&s.hmd_body_height54);
 s.npc_difficulty_percent=0;s.npc_defense_speed=1.3f;check(std::wcscmp(r::value(s,r::Row::Enemies),L"NORMAL")==0);
 check(r::advance(s,r::Row::Enemies)&&s.npc_difficulty_percent==50.f&&s.npc_defense_speed==1.7f);
 check(std::wcscmp(r::value(s,r::Row::Enemies),L"HARDCORE")==0);
 check(r::advance(s,r::Row::Enemies)&&s.npc_difficulty_percent==0.f&&s.npc_defense_speed==1.3f);
 s.npc_defense_speed=1.5f;check(std::wcscmp(r::value(s,r::Row::Enemies),L"CUSTOM")==0);
 s.dialogue_camera_mode=0;check(r::advance(s,r::Row::Dialogue)&&s.dialogue_camera_mode==2);
 check(r::advance(s,r::Row::Dialogue)&&s.dialogue_camera_mode==0);
 check(r::advance(s,r::Row::FastBlock)&&s.fast_block_r92);check(r::advance(s,r::Row::FastBlock)&&!s.fast_block_r92);
 s.roomscale_mode=0;for(int expected=1;expected<=4;++expected){check(r::advance(s,r::Row::Roomscale)&&s.roomscale_mode==expected%4);check(s.live_d39.nativeRoomscaleD76==(expected==1||expected==2));}
 for(int expected=1;expected<=4;++expected){check(r::advance(s,r::Row::Vignette)&&s.comfort_vignette51==expected%4);}
 check(!s.gesture_talk&&r::rows(1)[4].id==r::Row::PointingTalk);
 check(r::advance(s,r::Row::PointingTalk)&&s.gesture_talk&&std::wcscmp(r::value(s,r::Row::PointingTalk),L"ON")==0);
 check(l::advance(s,l::Id::QuickTalk)&&!s.gesture_talk&&std::wcscmp(r::value(s,r::Row::PointingTalk),L"OFF")==0);
 check(l::advance(s,l::Id::QuickTalk)&&s.gesture_talk);
 check(r::advance(s,r::Row::PointingTalk)&&!s.gesture_talk);
 check(r::rows(1)[5].id==r::Row::Impale);
 s.impale_hang=3;s.impale_hang_on45=3;
 check(r::advance(s,r::Row::Impale)&&s.impale_hang==0&&std::wcscmp(r::value(s,r::Row::Impale),L"OFF")==0);
 check(r::advance(s,r::Row::Impale)&&s.impale_hang==3&&std::wcscmp(r::value(s,r::Row::Impale),L"ON")==0);
 s.impale_hang=0;s.impale_hang_on45=0;check(r::advance(s,r::Row::Impale)&&s.impale_hang==2);
 check(!r::advance(s,r::Row::None));
 check(s.stereo_mode==3&&s.camera_back_offset_m==.123f&&s.camera_height_offset_m==.234f);
 for(const auto& page:r::pages)for(unsigned i=0;i<page.size();++i){check(l::rowRect(i).b<l::detailRect(i).t);if(i)check(l::detailRect(i-1).b<l::rowRect(i).t);}
 std::vector<std::pair<std::string,int>> values;
 for(int preset=1;preset<=5;++preset){check(kcd2vr::startup_graphics_d80::settings(-1,0,preset,values));check(values.size()==12);for(const auto& v:values)check(v.first.rfind("sys_spec_",0)==0&&v.second==preset);}

 namespace m=kcd2vr::motion_trials_d67;
 m::SetMode(0);m::SetElbowPreferenceR92(false);m::SetMenuMode(4);
 const auto oldPolicy=m::Policy(4);m::SetElbowPreferenceR92(true);
 check(m::MenuMode()==4&&m::UncappedSelectedAt(1)&&!m::TimingSelectedAt(1));
 check(m::ElbowSelectedForModeR92(4)&&m::Policy(4)!=oldPolicy);
 m::SetMenuMode(0);check(m::ElbowPreferenceR92()&&m::MenuMode()==0&&!m::TimingSelectedAt(1));
 m::SetMenuMode(2);check(m::ElbowPreferenceR92()&&m::TimingSelectedAt(1));
 m::SetElbowPreferenceR92(false);check(m::MenuMode()==2&&m::TimingSelectedAt(1));
 m::SetMenuMode(3);check(m::MenuMode()==2&&m::ElbowPreferenceR92()&&m::TimingSelectedAt(1));
 s.live_d39.motionTrialD67=4;s.live_d39.elbowSolverR89=false;s.live_d39.frikElbowsD48=false;s.live_d39.elbowPreferenceR92=false;
 check(l::advance(s,l::Id::ElbowSolverD48)&&s.live_d39.elbowSolverR89&&s.live_d39.motionTrialD67==4);
 check(l::advance(s,l::Id::ElbowSolverD48)&&s.live_d39.frikElbowsD48&&!s.live_d39.elbowSolverR89&&s.live_d39.motionTrialD67==4);
 check(l::advance(s,l::Id::ElbowSolverD48)&&s.live_d39.elbowPreferenceR92&&!s.live_d39.frikElbowsD48&&s.live_d39.motionTrialD67==4);
 check(l::advance(s,l::Id::ElbowSolverD48)&&!s.live_d39.elbowPreferenceR92&&s.live_d39.motionTrialD67==4);
 for(int expected:{0,2,4,0})check(l::advance(s,l::Id::MotionTrialD67)&&s.live_d39.motionTrialD67==expected);
 unsigned timingRows=0,elbowRows=0;
 for(unsigned page=0;page<=l::kBodyPage2D76;++page)for(const auto& row:l::rows(page)){
  if(row.id==l::Id::MotionTrialD67){++timingRows;check(l::group(page)==l::Group::Hands);}
  if(row.id==l::Id::ElbowSolverD48){++elbowRows;check(l::group(page)==l::Group::Hands);}
 }
 check(timingRows==1&&elbowRows==1);
 std::printf("PASS %d release menu / existing preset checks\n",n);
}
