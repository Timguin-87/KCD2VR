#pragma once
#include "vrik/crafting_menu_h18.h"
#include "vrik/native_crouch_r93.h"
#include "combat_menu49.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cwchar>
namespace kcd2vr::release_menu_r92 {
enum class Row { Vignette,Turn,Alchemy,Smith,Crouch,Hud,Glyphs,Dialogue,Enemies,ActivityPad,Roomscale,FastBlock,PointingTalk,Impale,BodyHeight,Gore,BlockIndicator,HudInfinity,CombatCross,MinimalHud,Tutorials,None };
struct Entry {Row id;const wchar_t* title;const wchar_t* detail;};
inline constexpr std::array<Entry,8> pages[3]{{{
 {Row::Vignette,L"Comfort vignette",L"Darken peripheral vision while moving."},
 {Row::Turn,L"Joystick turning",L"Choose fixed-angle snap turns or smooth rotation."},
 {Row::Alchemy,L"Alchemy hands",L"Semi follows native actions; 6DOF uses tracked hands."},
 {Row::Smith,L"Smithing hands",L"Semi follows native actions; 6DOF uses tracked tools."},
 {Row::Crouch,L"Crouching",L"Physical head movement or the native button; walking stays smooth."},
 {Row::Hud,L"Immersive HUD",L"Show the HUD when looking at an open palm."},
 {Row::Glyphs,L"Button icons",L"Change artwork only; controller bindings stay the same."},
 {Row::Dialogue,L"Dialogue view",L"Cinematic native cameras or immersive first person."}
}},{{
 {Row::Enemies,L"Enemy combat",L"Normal: 0% stance, 1.3x defense; Hard: 50%, 1.7x."},
 {Row::ActivityPad,L"Activity gamepad controls",L"Session only: normal controller actions in activities."},
 {Row::Roomscale,L"Physical walking and dodges",L"Choose room movement, headset dodges, both, or neither."},
 {Row::FastBlock,L"Fast block response",L"Enter your tuned frontal guard after 0.10s; native outcome."},
 {Row::PointingTalk,L"Index-finger pointing Quick Talk",L"Point while looking at a person or dog; keep pointing to choose. X stays available."},
 {Row::Impale,L"Physical impalements",L"Let eligible corpses follow the blade; pull out to release."},
 {Row::Gore,L"Gore",L"Off, blood x2, or blood x2 + decapitation/dismemberment on kills (needs the gore pak)."},
 {Row::BodyHeight,L"Body height",L"HMD, native, or HMD with native height on horse and in crouch."}
}},{{
 // R124 (Miki: "everybody should have an option to make block message show or not, developer and non developer")
 {Row::BlockIndicator,L"BLOCK message",L"Show BLOCK / SHIELD BLOCK in the view while the game holds your block."},
 // R125 (Miki: "the HUD is still slightly double" / "hard disable the combat cursor, leave only the icons")
 {Row::HudInfinity,L"HUD depth",L"INF: each eye its own HUD copy, flat and never double. DISTANCE: one HUD at the VR HUD distance."},
 {Row::CombatCross,L"Combat direction cross",L"HIDDEN: only the lock / perfect block / master strike icons show in a fight."},
 // R141 (Miki: "only interaction prompts and health and compass by the hand looking and dialogue text, everything other disabled")
 {Row::MinimalHud,L"Minimal HUD",L"ON: only prompts, dialogue, subtitles and compass / health on the palm. Everything else stays hidden."},
 {Row::Tutorials,L"Tutorial messages",L"HIDDEN: no tutorial popups (the game's own Tutorials option). Minimal HUD hides them too."}, // R141
 {Row::None,L"",L""},{Row::None,L"",L""},{Row::None,L"",L""}
}}};
inline constexpr unsigned kPages=3;
inline constexpr const auto& rows(unsigned page) noexcept{return pages[page<kPages?page:0];}
template<class S> bool advance(S& s,Row row) noexcept {
 switch(row){
 case Row::Vignette:s.comfort_vignette51=(std::clamp(s.comfort_vignette51,0,3)+1)%4;break;
 case Row::Turn:s.vr_turn_mode=s.vr_turn_mode==0?1:0;break;
 case Row::Alchemy:crafting_menu_h18::select(s,minigame_hands45::Alchemy,s.alchemy_mode67==3?1:3);break;
 case Row::Smith:crafting_menu_h18::select(s,minigame_hands45::Smithing,s.smithing_mode67==3?1:3);break;
 case Row::Crouch:native_crouch_r93::select(s);break;
 case Row::BodyHeight:s.body_height_mode98=native_crouch_r93::next_body_height_r98(s.body_height_mode98);break;
 case Row::Hud:s.contextual_hud=!s.contextual_hud;break;
 case Row::Glyphs:s.vr_pad_glyph_style=s.vr_pad_glyph_style==4?2:4;break;
 case Row::Dialogue:s.dialogue_camera_mode=s.dialogue_camera_mode==2?0:2;s.cyclopean_mode=0;break;
 case Row::Enemies:{const bool hardcore=std::fabs(s.npc_difficulty_percent-50.f)<.01f&&std::fabs(s.npc_defense_speed-1.7f)<.001f;s.npc_difficulty_percent=hardcore?0.f:50.f;s.npc_defense_speed=hardcore?1.3f:1.7f;break;}
 case Row::ActivityPad:s.native_activity_controls163=!s.native_activity_controls163;break;
 case Row::Roomscale:s.roomscale_mode=(std::clamp(s.roomscale_mode,0,3)+1)%4;s.live_d39.nativeRoomscaleD76=s.roomscale_mode==1||s.roomscale_mode==2;break;
 case Row::FastBlock:s.fast_block_r92=!s.fast_block_r92;break;
 case Row::BlockIndicator:s.show_block_indicator_h24=!s.show_block_indicator_h24;break; // R124: same VrShowBlockIndicator key as COMBAT
 case Row::HudInfinity:s.live_d39.hudInfinityR125=!s.live_d39.hudInfinityR125;break; // R125: VrHudInfinityR125
 case Row::CombatCross:s.live_d39.combatCrossR125=!s.live_d39.combatCrossR125;break; // R125: VrCombatCrossR125
 case Row::MinimalHud:s.live_d39.minimalHudR141=!s.live_d39.minimalHudR141;break; // R141: VrMinimalHudR141
 case Row::Tutorials:s.live_d39.hideTutorialsR141=!s.live_d39.hideTutorialsR141;break; // R141: VrHideTutorialsR141
 case Row::PointingTalk:s.gesture_talk=!s.gesture_talk;break;
 case Row::Impale:s.impale_hang=combat_menu49::impaleToggle(s.impale_hang,s.impale_hang_on45);break;
 case Row::Gore:{const int next=((s.decapitation||s.dismemberment_d80?2:s.bloody_fights49?1:0)+1)%3;s.bloody_fights49=next>=1;s.decapitation=next==2;s.dismemberment_d80=next==2;break;} // R98: OFF -> BLOOD x2 (VrBloodyFights) -> + VrDecapitation + VrDismemberment
 default:return false;
 }return true;
}
template<class S> const wchar_t* value(const S& s,Row row) noexcept {
 switch(row){
 case Row::Vignette:return s.comfort_vignette51==0?L"OFF":s.comfort_vignette51==1?L"LOW":s.comfort_vignette51==2?L"MEDIUM":L"STRONG";
 case Row::Turn:return s.vr_turn_mode==0?L"SNAP":L"SMOOTH";
 case Row::Alchemy:return crafting_menu_h18::label(s.alchemy_mode67);
 case Row::Smith:return crafting_menu_h18::label(s.smithing_mode67);
 case Row::Crouch:return native_crouch_r93::label(s);
 case Row::BodyHeight:return native_crouch_r93::body_height_label_r98(s.body_height_mode98);
 case Row::Hud:return s.contextual_hud?L"ON":L"OFF";
 case Row::Glyphs:return s.vr_pad_glyph_style==4?L"PLAYSTATION":L"XBOX";
 case Row::Dialogue:return s.dialogue_camera_mode==2?L"IMMERSIVE":L"CINEMATIC";
 case Row::Enemies:return std::fabs(s.npc_difficulty_percent-50.f)<.01f&&std::fabs(s.npc_defense_speed-1.7f)<.001f?L"HARDCORE":std::fabs(s.npc_difficulty_percent)<.01f&&std::fabs(s.npc_defense_speed-1.3f)<.001f?L"NORMAL":L"CUSTOM";
 case Row::ActivityPad:return s.native_activity_controls163?L"ON":L"OFF";
 case Row::Roomscale:return s.roomscale_mode==1?L"WALK":s.roomscale_mode==2?L"WALK + DODGE":s.roomscale_mode==3?L"DODGE":L"OFF";
 case Row::FastBlock:return s.fast_block_r92?L"ON":L"OFF";
 case Row::BlockIndicator:return s.show_block_indicator_h24?L"SHOWN":L"HIDDEN";
 case Row::HudInfinity:return s.live_d39.hudInfinityR125?L"INF":L"DISTANCE";
 case Row::CombatCross:return s.live_d39.combatCrossR125?L"SHOWN":L"HIDDEN";
 case Row::MinimalHud:return s.live_d39.minimalHudR141?L"ON":L"OFF";
 case Row::Tutorials:return s.live_d39.hideTutorialsR141?L"HIDDEN":L"SHOWN";
 case Row::PointingTalk:return s.gesture_talk?L"ON":L"OFF";
 case Row::Impale:return s.impale_hang?L"ON":L"OFF";
 case Row::Gore:return s.decapitation&&s.dismemberment_d80?(s.bloody_fights49?L"BLOOD x2 + HEADS + LIMBS":L"HEADS + LIMBS"):s.decapitation?L"HEADS ONLY":s.dismemberment_d80?L"LIMBS ONLY":s.bloody_fights49?L"BLOOD x2":L"OFF";
 default:return L"";
 }
}
}
