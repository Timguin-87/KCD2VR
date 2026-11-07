// R98 scratch host check of release_menu_r92.h body-height row (vr_menu.h needs MSVC/windows.h).
#include "release_menu_r92.h"
#include <cstdio>
#include <cwchar>
struct Live {bool nativeRoomscaleD76=false;};
struct S {
 Live live_d39{};int comfort_vignette51=0,vr_turn_mode=0,alchemy_mode67=1,smithing_mode67=1,vr_pad_glyph_style=2,dialogue_camera_mode=0,cyclopean_mode=0,roomscale_mode=0,impale_hang=0,impale_hang_on45=0;
 bool activity_hands59c7f[4]{},smith_auto_hold36=true,native_crouch_r93=false,contextual_hud=false,native_activity_controls163=false,fast_block_r92=false,gesture_talk=false;
 int body_height_mode98=2;float npc_difficulty_percent=0,npc_defense_speed=1.3f;
 bool bloody_fights49=false,decapitation=false,dismemberment_d80=false; // R98 gore row
};
int main(){namespace r=kcd2vr::release_menu_r92;int n=0,f=0;auto ck=[&](bool ok,const char* w){++n;if(!ok){++f;std::printf("FAIL %s\n",w);}};
 S s{};ck(std::wcscmp(r::value(s,r::Row::BodyHeight),L"HMD + NATIVE (RIDE/CROUCH)")==0,"default combined");
 ck(r::rows(1)[7].id==r::Row::BodyHeight,"visible on Immersion and Controls page 2");
 ck(r::advance(s,r::Row::BodyHeight)&&s.body_height_mode98==0&&std::wcscmp(r::value(s,r::Row::BodyHeight),L"HMD HEIGHT")==0,"-> HMD");
 ck(r::advance(s,r::Row::BodyHeight)&&s.body_height_mode98==1&&std::wcscmp(r::value(s,r::Row::BodyHeight),L"NATIVE HEIGHT")==0,"-> NATIVE");
 ck(std::wcscmp(r::value(s,r::Row::Crouch),L"NATIVE (NATIVE HEIGHT)")==0,"crouch row honest in native height");
 ck(r::advance(s,r::Row::Crouch)&&s.native_crouch_r93&&s.body_height_mode98==1,"crouch row keeps body height");
 ck(r::advance(s,r::Row::BodyHeight)&&s.body_height_mode98==2&&std::wcscmp(r::value(s,r::Row::Crouch),L"NATIVE BUTTON")==0,"-> combined, crouch label");
 std::printf("R98 release body height row %d/%d\n",n-f,n);return f?1:0;}
