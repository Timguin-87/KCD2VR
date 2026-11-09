#include "release_menu_r92.h"
#include "subsystems/base/startup_graphics_d80.h"
#include "subsystems/dice/selection_marker_r92.h"
#include "vr_menu.h"
#include "ui_surface_geometry.h" // R127: HUD convergence range
#include "ui_hud_r160.h" // R160 UI: one icon size + tutorial position
#include "ui_native_canvas.h" // R160: dialogue size / vertical neutral point (pure header)
#include "subsystems/hud_ui_controls/prompt_policy_d80.h"
#include "subsystems/experimental/claude_options_h8.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "subsystems/experimental/features_menu_h5.h"
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "vrik/motion_trials_d67.h"
#include "vrik/gorepak50.h" // C50: the decap button says when the separate gore pak is missing
#include "combat_menu49.h" // C49 first fight page + shadow row layout (pure, host tested)
#include "immersion_menu45.h" // C45 IMMERSION page layout (pure, host tested)
#include "resource_ids26.h" // C26e embedded menu book skin
#include "device_hook.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "developer_mode.h"
#include "graphics_menu_options.h"
#include "stereo_upscaler_d81r2.h"
#include "native_dlss_stereo_d82.h" // R149 alternate-eye frame dump button
#include "upscaler_live_switch_d85.h" // D85 UPSCALERS

#include <commctrl.h>
#include <windowsx.h>
#include <d3dcompiler.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <climits>
#include <cstring>

namespace kcd2vr {

namespace {

constexpr UINT_PTR kSubclassId = 0x4B43443256524D45ull; // "KCD2VRME"
constexpr UINT kMsgToggleMenu = WM_APP + 0x462;
constexpr UINT kMsgSetMenuVisible = WM_APP + 0x463;

struct UiRect { int l, t, r, b; };
constexpr UiRect kRangedBow32{620,140,1038,178},kRangedZoom32{620,202,1038,240};
constexpr UiRect EH5(experimental_features_h5::Rect r){return {r.l,r.t,r.r,r.b};}
constexpr UiRect ED39(live_settings_d39::Rect r){return {r.l,r.t,r.r,r.b};}
constexpr UiRect kDsVignetteH29{620,180,1040,222};
constexpr UiRect kDsFeaturesH5{900,135,1040,165};
constexpr UiRect kDsOffH29{620,260,1040,300};
constexpr UiRect kDsWallH31{620,320,1040,358};
constexpr UiRect kDsFingerH31{620,446,1040,484};
constexpr UiRect kDsFingerHelpH31{620,490,1040,556};
constexpr UiRect kDsPendingH31{620,564,1040,662};
constexpr bool DeepSeekLayoutH31(){
    const UiRect rows[]{kDsFeaturesH5,kDsVignetteH29,kDsOffH29,kDsWallH31,kDsFingerH31,kDsFingerHelpH31,kDsPendingH31};
    for(unsigned i=0;i<std::size(rows);++i){
        if(rows[i].l<570||rows[i].r>1070||rows[i].t<100||rows[i].b>672)return false;
        if(i&&rows[i-1].b>=rows[i].t)return false;
    }
    return true;
}
static_assert(DeepSeekLayoutH31(),"DeepSeek trial controls must fit the existing page without overlap");
bool PtIn(const UiRect& r, int x, int y) { return x >= r.l && x < r.r && y >= r.t && y < r.b; }
#define KCD2VR_M2_HOST_SLOT 5022 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=5022
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
// V58 two-page/open-book layout for the 1100x680 texture.
// Left page: tracking/projection. Right page: quality/dialogue/input/recorders.
constexpr UiRect kBridgeRect      { 45, 112, 510, 137 };
constexpr UiRect kHeadRotRect     { 45, 143, 260, 168 };
constexpr UiRect kHeadPosRect     { 280,143, 510, 168 };
constexpr UiRect kDiagRect        { 45, 174, 260, 199 };
constexpr UiRect kVrUiLayoutRect  { 280,174, 510, 199 }; // V62.4 native semantic HUD master
constexpr UiRect kCameraSideRect  { 45, 250, 510, 270 };
constexpr UiRect kCameraBackRect  { 45, 284, 510, 304 };
constexpr UiRect kCameraHeightRect{ 45, 318, 510, 338 };

constexpr UiRect kPresentPrevRect { -10,-10,-5,-5 };
constexpr UiRect kPresentNextRect { -10,-10,-5,-5 };
constexpr UiRect kRecenterRect    { 45, 354, 175, 376 };
constexpr UiRect kResetViewRect   { 185,354, 340, 376 };
constexpr UiRect kResetUiRect     { 350,354, 510, 376 };
constexpr UiRect kZoomSliderRect  { 45, 394, 510, 414 };
constexpr UiRect kFovApplyRect    { 45, 430, 118, 450 };
constexpr UiRect kFovSliderRect   { 128,430, 510, 450 };
constexpr UiRect kSourceFovRect   { 45, 466, 510, 486 };
constexpr UiRect kCullScaleRect   { -10,-10,-5,-5 }; // V61.20 retired: cull fix is fixed gameplay-only path
constexpr UiRect kUiScaleRect     { -10,-10,-5,-5 }; // V61.45 retired: generic live SetScale bridge did not affect KCD2 HUD
constexpr UiRect kDialogueTextScaleRect { -10,-10,-5,-5 }; // V61.45 retired: fc_setSubtitleSize is a mode enum, not a percentage
constexpr UiRect kTransientZoomRetainRect { -10,-10,-5,-5 }; // V61.45 retired: V60.1 fixed source-FOV guard is authoritative
// V61.27: HUD XML width/height is a viewport/clipping control, not a safe live HUD scale.
// Retire those controls from the combined menu and reuse the space for projection-centering diagnostics.
constexpr UiRect kCenterXRect     { 45, 492, 510, 512 };
constexpr UiRect kCenterYRect     { 45, 522, 510, 542 };
constexpr UiRect kCenterYResetRect{ 45, 550, 250, 578 };
constexpr UiRect kHudXmlScaleRect { -10,-10,-5,-5 };
constexpr UiRect kHudXmlXRect     { -10,-10,-5,-5 };
constexpr UiRect kHudXmlYRect     { -10,-10,-5,-5 };
constexpr UiRect kHudXmlWriteRect { -10,-10,-5,-5 };
constexpr UiRect kHudXmlResetRect { -10,-10,-5,-5 };
constexpr UiRect kFlatUiIpdRect   { -10,-10,-5,-5 };

constexpr UiRect kBalancedRect    { 590,112,806,137 };
constexpr UiRect kHeadSyncRect    { 824,112,1048,137 }; // V62.4: direct mode-2 SYNC_NOW toggle

// V62.4 dedicated native UI tuning page. All values are applied only on
// explicit APPLY or slider release, never every render/XR frame.
constexpr UiRect kUiPageMasterRect { 45,112,510,140 };
constexpr UiRect kUiAutoRect       { 590,112,1048,140 };
constexpr UiRect kUiCompactRect    { 45,158,1048,182 };
constexpr UiRect kUiSubScaleRect   { 45,216,510,240 };
constexpr UiRect kUiSubYRect       { 45,270,510,294 };
constexpr UiRect kUiDlgScaleRect   { 590,216,1048,240 };
constexpr UiRect kUiDlgInsetRect   { 590,270,1048,294 };
constexpr UiRect kUiHintScaleRect  { 590,334,1048,358 };
constexpr UiRect kUiHintInsetRect  { 590,386,1048,410 };
constexpr UiRect kUiHintYRect      { 590,438,1048,462 };
constexpr UiRect kUiPageBackRect   { 45,585,225,619 };
constexpr UiRect kUiPageResetRect  { 590,585,810,619 };
constexpr UiRect kUiPageApplyRect  { 828,585,1048,619 };
constexpr UiRect kCycloMonoRect   { 590,158, 730,192 };
constexpr UiRect kCycloP1Rect     { 748,158, 888,192 };
constexpr UiRect kCycloP2Rect     { 906,158,1048,192 };
constexpr UiRect kCycloP3Rect     { 590,200,1048,234 };
constexpr UiRect kDepthStrengthRect{ 590,248,1048,270 };
constexpr UiRect kCycloP4Rect     { 748,200, 888,234 };
constexpr UiRect kCycloP5Rect     { 906,200,1048,234 };
constexpr UiRect kFakeDofRect     { -10,-10,-5,-5 };
constexpr UiRect kEngineDofRect   { -10,-10,-5,-5 };
constexpr UiRect kHfovApplyRect   { -10,-10,-5,-5 };
constexpr UiRect kHfovSliderRect  { -10,-10,-5,-5 };
constexpr UiRect kFpsApplyRect    { 590,285, 663,310 };
constexpr UiRect kFpsSliderRect   { 675,285,1048,310 };
constexpr UiRect kClarityRect     { 590,326,1048,348 };
constexpr UiRect kDialogPrevRect  { 590,362, 625,388 };
constexpr UiRect kDialogNextRect  {1013,362,1048,388 };
constexpr UiRect kStereoMonoRect  { 590,402, 730,432 };
constexpr UiRect kStereoAfrRect   { 748,402, 888,432 };
constexpr UiRect kStereoDualRect  { 906,402,1048,432 };
constexpr UiRect kStereoIpdRect   { 590,442,1048,462 };
constexpr UiRect kTouchRect       { -10,-10,-5,-5 };
constexpr UiRect kTouchAttackRect { -10,-10,-5,-5 };

// V56/V58 action recorder buttons: read-only native animation/IK correlation.
constexpr UiRect kRecWeaponRect   { -10,-10,-5,-5 };
constexpr UiRect kRecPickupRect   { -10,-10,-5,-5 };
constexpr UiRect kRecJumpRect   { -10,-10,-5,-5 };
constexpr UiRect kRecBowRect   { -10,-10,-5,-5 };
constexpr UiRect kRecAlchemyRect   { -10,-10,-5,-5 };
constexpr UiRect kRecSmithRect   { -10,-10,-5,-5 };
constexpr UiRect kRecDialogRect   { -10,-10,-5,-5 };
constexpr UiRect kRecMiscRect   { -10,-10,-5,-5 };
constexpr UiRect kRecHorseRect   { -10,-10,-5,-5 };

constexpr UiRect kMatrixRect      {590,585,732,619};
constexpr UiRect kSafeMonoRect    {748,585,890,619};
constexpr UiRect kSaveRect        {-10,-10,-5,-5};

constexpr int kNavCount = 19;
constexpr POINT kNavCenters[kNavCount] = {
    {152,155},{395,155},{278,186},{278,237},{278,293},{278,354},{278,409},{278,464},{278,519},
    {698,124},{936,124},{720,187},{956,186},{820,241},{820,307},{820,354},{820,385},{661,609},{977,609}
};


// V62.7 parchment-book page system. The left page remains identity/status; the
// right page slides between uncluttered configuration pages.
constexpr UiRect kHomeModeMono{130,350,500,388};
constexpr UiRect kHomeModeCorrected{130,400,500,438};
constexpr UiRect kHomeModeAlternateEyeR142{130,450,312,488};
constexpr UiRect kHomeAerWarpR154{318,450,404,488}; // R154: stale-eye warp tier ROT / 6DOF / MV (shown only in ALT EYE)
constexpr UiRect kHomeAerDumpR149{410,450,500,488}; // R149: alternate-eye 4-frame debug dump (shown only in that mode)
constexpr UiRect kHomeModePseudo{130,450,500,488};
constexpr UiRect kCombatProx40{620,230,1038,252},kCombatWeight40{620,300,1038,322},kCombatStats40{620,360,1038,400},kCombatInstant40{620,130,1038,162},kCombatForce40{620,174,1038,206},kCombatMore40{620,540,1038,574},kCombatFlow40{620,590,1038,624};
constexpr UiRect kHomePseudoEffect{130,535,500,557};
constexpr UiRect kHomeFps{130,578,500,600};
constexpr UiRect kVrikExperimentsD61{130,542,500,572};
constexpr UiRect kVrikLeadD61{620,190,1038,228};
constexpr UiRect kVrikBaseD61{620,312,1038,346},kVrikTimingD61{620,354,1038,388},kVrikUncappedD67{620,396,1038,430},kVrikElbowD61{620,438,1038,472};
constexpr UiRect kStabPageD67{620,430,1038,465};
constexpr UiRect kPickupCameraD65{620,490,1038,526},kRepairTrialsD65{620,578,1038,612};
constexpr UiRect kNativeFinisherD61{620,356,1038,388};
constexpr UiRect kHomeHeadSync{130,610,310,638};
constexpr UiRect kH29TuneNav{130,495,500,533},kH29Type{620,165,1038,195},kH29Cut{620,225,1038,247},kH29Stab{620,305,1038,327},kH29Punch{620,385,1038,407},kH29Block{620,465,1038,487},kH29TravelR8{620,545,1038,567},kFingerMiddleC54{620,495,1038,517},kFingerMiddleMedialH31{620,555,1038,577};
constexpr UiRect kGuardWidthD56{620,190,1038,212},kGuardHeightD56{620,250,1038,272},
    kGuardDepthD61{620,310,1038,332},kGuardEntryD56{620,370,1038,392},
    kGuardReleaseD56{620,430,1038,452},kGuardSettleD56{620,490,1038,512},
    kGuardDistanceD56{620,550,1038,572};
constexpr bool GuardSliderLayoutD61(){
    const UiRect rows[]{kGuardWidthD56,kGuardHeightD56,kGuardDepthD61,kGuardEntryD56,kGuardReleaseD56,kGuardSettleD56,kGuardDistanceD56};
    for(unsigned i=0;i<std::size(rows);++i)if(rows[i].l<620||rows[i].r>1038||rows[i].t<180||rows[i].b>580||(i&&rows[i-1].b+30>rows[i].t))return false;
    return true;
}
static_assert(GuardSliderLayoutD61(),"Guard controls and labels must not overlap");
// R120 SHIELD BOX page: nav button on the BLOCK page (the BLOCK page does not draw kGuardNavH41 there), the ON/OFF
// switch and one slider per shield_block_r120::Slot (47 px apart: 24 px label + 22 px track).
constexpr UiRect kShieldNavR120{904,112,1038,142},kShieldToggleR120{620,146,1038,172};
constexpr UiRect ShieldSliderR120(unsigned i) noexcept {return {620,196+47*int(i),1038,218+47*int(i)};}
static_assert(ShieldSliderR120(0).t-24>=kShieldToggleR120.b&&ShieldSliderR120(shield_block_r120::Count-1).b<=660,"Shield box sliders must fit the page");
constexpr UiRect kWeaponNavH41{620,112,754,142},kFistNavH41{762,112,896,142},kGuardNavH41{904,112,1038,142},kWindowNavH41{834,482,1038,514},kNeckTrialH41{620,410,1038,442},kArcherTrialH41{620,458,1038,490},kNativeKillNF{620,506,1038,538};
constexpr UiRect kFingerTab40{130,450,500,488},kFingerSide40{620,130,1038,160},kFingerOpen40{620,255,1038,277},kFingerClosed40{620,315,1038,337},kFingerLateral40{620,375,1038,397},kFingerProximal55{620,435,1038,457},kFingerReset40{620,615,1038,645},kGraphicsHead40{130,610,500,638};
constexpr bool H29SliderLayout(){
 const UiRect rows[]{kH29Cut,kH29Stab,kH29Punch,kH29Block,kH29TravelR8};
 for(unsigned i=0;i<5;++i){if(rows[i].l<570||rows[i].r>1070||rows[i].t<100||rows[i].b>672)return false;if(i&&rows[i-1].b+30>rows[i].t)return false;}
 return kH29TuneNav.t>kHomeModeCorrected.b&&kH29TuneNav.b<kHomeFps.t&&kFingerMiddleC54.t>kFingerProximal55.b+20&&kFingerMiddleC54.b+30<kFingerMiddleMedialH31.t&&kFingerMiddleMedialH31.b+30<kFingerReset40.t&&kFingerReset40.b<672;
}
static_assert(H29SliderLayout(),"H29 sensitivity and finger sliders must fit without overlapping labels or menu buttons");

inline UiRect FingerButton40(int i){return {620+i*84,190,698+i*84,222};}
constexpr UiRect kHomeF2Fix{320,610,500,638};
// R150 HAND OFFSETS page: four groups (right pos, right rot, left pos, left rot), three compact sliders each.
// Slider i = side*6 + {X,Y,Z,pitch,yaw,roll}; the label/value line sits 22 px above each 22 px track.
constexpr UiRect kHomeHandOffsetsR150{130,610,500,640};
constexpr int HandOffsetRowTopR150(int row) noexcept {return 112+row*80+(row>=2?18:0);}
constexpr UiRect HandOffsetSliderR150(unsigned i) noexcept {
    const int row=int(i/3u),col=int(i%3u),top=HandOffsetRowTopR150(row)+48;
    return {620+col*144,top,750+col*144,top+22};
}
constexpr UiRect kHandOffsetResetRR150{620,470,824,500},kHandOffsetResetLR150{834,470,1038,500};
constexpr UiRect kHandAlignRR153{620,508,824,538},kHandAlignLR153{834,508,1038,538}; // R153 freeze hand, squeeze grip to solve
constexpr bool HandOffsetLayoutR150(){
    for(unsigned i=0;i<12;++i){const auto r=HandOffsetSliderR150(i);if(r.l<620||r.r>1038||r.t-22<HandOffsetRowTopR150(int(i/3u))+22)return false;}
    return HandOffsetSliderR150(11).b<kHandOffsetResetRR150.t&&kHandOffsetResetRR150.b<kHandAlignRR153.t&&kHandAlignRR153.b<=546;
}
static_assert(HandOffsetLayoutR150(),"Hand offset sliders, headers and reset buttons must not overlap");
inline float HandOffsetTR150(const UiRect& r,int x) noexcept {return std::clamp(float(x-r.l-6)/float(std::max(1,r.r-r.l-12)),0.f,1.f);}

constexpr UiRect kHomeVrConfig{620,132,1028,182};
constexpr UiRect kHomeGraphics{620,197,1028,247};
constexpr UiRect kHomeUi{620,262,1028,312};
constexpr UiRect kHomeActions{620,392,1028,442};
constexpr UiRect kHomeDialogue{620,327,1028,377};
constexpr UiRect kHomeVrikNative{620,452,820,488};
constexpr UiRect kHomeVrikTracked{838,452,1038,488};
constexpr UiRect kHomeVrikRecalibrate{620,494,756,520};
constexpr UiRect kHomeVrikRestore43{762,494,896,520};  // C43: apply the stored calibration again (no T-pose)
constexpr UiRect kHomeDirectHandsR135{902,494,1038,520};
constexpr UiRect kHomeBest{620,526,820,550};
constexpr UiRect kHomeAutoRestore43{832,526,1038,550};  // C43: VrCalibrationAutoRestore checkbox
constexpr UiRect kHomeHeadSyncOld{-10,-10,-5,-5};
constexpr UiRect kHomeSave=EH5(experimental_features_h5::kHomeSave);
constexpr UiRect kHomeFeaturesH5=EH5(experimental_features_h5::kHomeFeatures);
constexpr UiRect kHomeExperiments=EH5(experimental_features_h5::kHomeCombat);
constexpr bool HomeNavigationH5(){
    const UiRect rows[]{kHomeModeMono,kHomeModeCorrected,kHomeModeAlternateEyeR142,kHomeAerWarpR154,kHomeAerDumpR149,kVrikExperimentsD61,kH29TuneNav,kHomeFps,
        kHomeVrConfig,kHomeGraphics,kHomeUi,kHomeDialogue,kHomeActions,kHomeVrikNative,kHomeVrikTracked,
        kHomeVrikRecalibrate,kHomeVrikRestore43,kHomeDirectHandsR135,kHomeBest,kHomeAutoRestore43,kHomeSave,kHomeFeaturesH5,kHomeExperiments,kHomeHandOffsetsR150};
    for(unsigned i=0;i<std::size(rows);++i){const auto r=rows[i];if(r.l<0||r.r>1070||r.t<100||r.b>672)return false;
        for(unsigned j=i+1;j<std::size(rows);++j){const auto s=rows[j];if(r.l<s.r&&s.l<r.r&&r.t<s.b&&s.t<r.b)return false;}}
    return kHomeAutoRestore43.b<kHomeSave.t&&kHomeFeaturesH5.b<kHomeExperiments.t;
}
static_assert(HomeNavigationH5(),"Home click targets, including SAVE/calibration and both experiment pages, must remain separate");
// R59.16C10d EXPERIMENTS page layout: toggles top, sliders below (x 620..1038).
// C49: first EXPERIMENTS page = 4 main sliders + FREE SWING; the old page-1 rows below moved unchanged to CombatTuning49.
constexpr UiRect U49(combat_menu49::Rect r){ return UiRect{r.l,r.t,r.r,r.b}; }
constexpr UiRect kCm49Proximity=U49(combat_menu49::kProximity),kCm49BlockSpeed=U49(combat_menu49::kBlockSpeed),kCm49Weight=U49(combat_menu49::kWeight),kCm49Skill=U49(combat_menu49::kSkill);
constexpr UiRect kCm49InstantFree=U49(combat_menu49::kInstantFree),kCm49More=U49(claude_h8::kMore),kCm49Flow=U49(combat_menu49::kFlow);
constexpr UiRect kCm54Defaults=U49(combat_menu49::kDefaults54);
constexpr UiRect kCm49ImpaleKill=U49(combat_menu49::kImpaleKill),kCm49DecapKill=U49(combat_menu49::kDecapKill); // C45 kill-move row
static_assert(combat_menu49::clean(),"C45 EXPERIMENTS first page: a rect overlaps, leaves the clip or enters the header");
static_assert(immersion_menu45::clean(),"C45 IMMERSION page: a rect overlaps, leaves the clip or enters the header");
constexpr UiRect kExpTelegraphWindup{620,130,824,152},kExpTelegraphStrike{834,130,1038,152},kExpWindupHold{620,160,824,182},kExpGuardCounter{834,160,1038,182},kExpContextualHud{620,190,824,212},kExpBaseline35{834,190,1038,212};
constexpr UiRect kExpGuardEnterSpeed{620,232,1038,254},kExpGuardEnterDwell{620,268,1038,290},kExpGuardExitSpeed{620,304,1038,326},kExpGuardAfterAttack{620,340,1038,362};
constexpr UiRect kExpStrikeRefJ{620,388,1038,410},kExpStrikeMaxPower{620,424,1038,446},kExpStrikeTwoHanded{620,460,1038,482},kExpHeadRaise{620,496,1038,518};
constexpr UiRect kExpAutoWindup19c4r1{620,580,1038,610};
constexpr UiRect kCm43Touch{620,618,824,646},kCm43Fall{834,618,1038,646}; // C43T/C46: MORE COMBAT TUNING bottom row (page 1 row 630 overlapped COMBAT FLOW), inside the clip (672)
constexpr UiRect kExpMore59c12{620,540,1038,570}; // R59.16C10R12: page 1 ends here (the panel clips at y=630; R5's rows 644-666 were never visible)
// R59.16C10R12 COMBAT TUNING 2 (Experiments2): toggles then the three sliders that were cut off on page 1.
constexpr UiRect kExpWallSlide{620,130,824,152},kExpAttackGate{834,130,1038,152};
constexpr UiRect kExpSyncOverdue59c12{620,160,1038,182},kExpStrikeRecovery59c12{620,190,1038,212},kExpHitNoRow59c12{620,220,1038,242};
constexpr UiRect kExpReclaimGuard59c16{620,250,824,272},kExpStagger59c16{834,250,1038,272}; // R59.16C10R16 (F4 press 4)
constexpr UiRect kExpNpcEtaPrepare{620,290,1038,312},kExpNpcProximity36{620,326,1038,348},kExpAttackReach{620,362,1038,384}; // R59.16C10R5, moved to page 2 in R12
constexpr UiRect kExpWeaponFeelMore59c14{620,590,1038,620}; // R59.16C10R14: page 2 -> WEAPON FEEL
constexpr UiRect kExpDirBlock39{620,444,1038,466}; // C39 DIRBLOCK (one button; text above shortened to h 46)
constexpr UiRect kExpImpale40{620,420,1038,442}; // C40 IMPALE BY ARMOUR (help text above shortened to one line, h 22)
constexpr UiRect kExpDirectBlock39{620,470,1038,492},kExpDirectBlockLo39{620,518,1038,540},kExpDirectBlockHi39{620,566,1038,588}; // C39 VR DIRECT NPC block speed by skill
constexpr UiRect kNpcDifficulty{620,166,1038,188},kNpcDefenseSpeed{620,315,1038,337},kNpcUnarmedSpacing{620,480,1038,502};
// R59.16C10R14 WEAPON FEEL page: weight, fatigue ramp, one-handed polearm, stamina per stroke, rung-2 strike gates, bow hide.
constexpr UiRect kWfWeight59c14{620,130,1038,152},kWfFatigue59c14{620,166,1038,188},kWfPole59c14{620,202,1038,224},kWfStamina59c14{620,238,1038,260};
constexpr UiRect kWfMinScale59c14{620,274,1038,296},kWfMinInterval59c14{620,310,1038,332};
constexpr UiRect kWfMinGate59c14{620,350,1038,376},kWfMasterGame59c14{620,384,1038,410},kWfBowHide59c14{620,418,1038,444};
constexpr UiRect kWfTwoHandAxis59c16{620,452,1038,478}; // R59.16C10R16
constexpr UiRect kWfStatEffect59c17{620,490,1038,512},kWfCombatFlow59c17{620,520,1038,548}; // R59.16C10R17
// R59.16C10R17 COMBAT FLOW page: four switches, three sliders, help text.
constexpr UiRect kCfGuardStrict59c17{620,130,1038,152},kCfNoCooldown59c17{620,160,1038,182},kCfMsStance59c17{620,190,1038,212},kCfIntentLive59c17{620,220,1038,242};
constexpr UiRect kCfHitTime59c17{620,262,1038,284},kCfComboWindow59c17{750,298,1038,320},kCfLiveWait59c17{620,334,1038,356};
constexpr UiRect kCfDirectTelegraph18{620,370,1038,392},kCfDirectOwnHits18{620,398,1038,420}; // C10R18 F4 press 4
constexpr UiRect kCfMsHandoff23{620,470,1038,492}; // C24b master-strike window handoff speed
constexpr UiRect kCfInstantFree38{620,500,1038,522},kCfHideLean38{620,528,1038,550}; // C38 SYNC swing start / swing lean
constexpr UiRect kCfStagger39{620,556,1038,578}; // C39 DIRECT: block press out of own hit reaction/recovery in the native PB opening
constexpr UiRect kCfComboExtend59c17{620,298,738,320}; // R25: separate gate beside seconds slider
constexpr UiRect kCfComboZoneCZ{620,436,828,458},kCfComboHoldN2{830,436,1038,458}; // I12 COMBO HELP row: zone assist | recovery hold (free row between F4 DIRECT and MS handoff)
constexpr UiRect kCfMsTestForce{620,608,828,630},kCfComboTestForce{830,608,1038,630}; // DS-F5/F6: two half-width TEST toggles (replaces the shortened C38/C39 help text)
constexpr UiRect kCfKicks26{620,632,1038,660}; // C26: COMBAT FLOW > KICKS
constexpr UiRect kCfCounter40{620,584,1038,606}; // C40: COMBAT FLOW > COUNTER STAB (help text below shortened)
constexpr UiRect kCs40Toggle{620,130,1038,158},kCs40Energy{620,206,1038,228},kCs40Mul{620,266,1038,288},kCs40Window{620,326,1038,348}; // C40 COUNTER STAB page
constexpr UiRect kMs47Mode{620,592,1038,614},kMs47Energy{620,648,1038,670}; // C47 master-strike gate (help text above shortened)
constexpr UiRect kCs44Perk{620,358,1038,380},kCs44Stagger{620,388,1038,410},kCs44HangKg{620,436,1038,458},kCs44KillEnergy{620,490,1038,512}; // C44: perk gate, Henry stagger rule, impale weight + kill energy
constexpr UiRect kWfSim26{620,638,1038,664}; // C26c WEAPON FEEL: SIMULATE HENRY
constexpr UiRect kKkProfile52{620,394,1038,422},kKkReach52{620,458,1038,480}; // C52 KICK STYLE (native timing / old) + reach slider (label above)
constexpr UiRect kKkStyle26{620,130,1038,158},kKkLeg26{620,168,1038,190},kKkReach26{620,206,1038,228},kKkImpale26{620,244,1038,272}; // C26 KICKS page
constexpr UiRect kKkRoute33{620,282,1038,310},kKkFallback33{620,318,1038,346}; // C33
constexpr UiRect kKkRealHit37c2{620,356,1038,384}; // C37C2 opt-in real-hit kick test
// R109 MISC pages (COMBAT EXPERIMENTS > MISC 1/2/3): rows for saved combat keys that had no reachable menu row any more.
// Sliders: label 24 px above a 22 px track, 36 px pitch (as on MORE COMBAT TUNING). Checks/buttons: 26 px, 28 px pitch.
constexpr UiRect kMiscNavR109{620,612,1038,640}; // live COMBAT group: the MISC > button sits above the shortened status line
constexpr UiRect kMiscTab1R109{694,72,806,102},kMiscTab2R109{814,72,920,102},kMiscTab3R109{928,72,1040,102}; // same places as VIEW / UI / CONTROLS
constexpr UiRect MiscSliderR109(int i,int top){return UiRect{620,top+36*i,1038,top+22+36*i};}
constexpr UiRect MiscRowR109(int i,int top){return UiRect{620,top+28*i,1038,top+26+28*i};}
// MISC 1: enemy side
constexpr UiRect kM1DifficultyR109=MiscSliderR109(0,140),kM1DefenseSpeedR109=MiscSliderR109(1,140),kM1BlockAnimR109=MiscSliderR109(2,140),kM1ParryAnimR109=MiscSliderR109(3,140),kM1DodgeAnimR109=MiscSliderR109(4,140),kM1SpacingR109=MiscSliderR109(5,140);
constexpr UiRect kM1BlockSpeed18R109=MiscSliderR109(6,140),kM1DirectLoR109=MiscSliderR109(7,140),kM1DirectHiR109=MiscSliderR109(8,140),kM1EtaPrepareR109=MiscSliderR109(9,140),kM1EtaStrikeR109=MiscSliderR109(10,140);
constexpr UiRect kM1RaiseR109=MiscRowR109(0,534),kM1BySkillR109=MiscRowR109(1,534),kM1DirBlockR109=MiscRowR109(2,534),kM1StatusR109{620,622,1038,668};
// MISC 2: VR DIRECT hits, counter stab, master strike gate
constexpr UiRect kM2OwnHitsR109=MiscRowR109(0,116),kM2SyncOverdueR109=MiscRowR109(1,116),kM2RecoveryR109=MiscRowR109(2,116),kM2NoRowR109=MiscRowR109(3,116),kM2ReclaimR109=MiscRowR109(4,116),kM2StaggerGuardR109=MiscRowR109(5,116);
constexpr UiRect kM2CounterR109=MiscRowR109(6,116),kM2CounterPerkR109=MiscRowR109(7,116),kM2IntentLiveR109=MiscRowR109(8,116),kM2StaggerRuleR109=MiscRowR109(9,116),kM2MsModeR109=MiscRowR109(10,116);
constexpr UiRect kM2CounterEnergyR109=MiscSliderR109(0,454),kM2CounterMulR109=MiscSliderR109(1,454),kM2CounterWindowR109=MiscSliderR109(2,454),kM2IntentWaitR109=MiscSliderR109(3,454),kM2HelpR109{620,592,1038,668};
// MISC 3: native window timing (F12 router), Henry's windows, strike power rule, impale
constexpr UiRect kM3RouterR109{620,116,1038,142},kM3StrikePowerR109{620,148,1038,174};
constexpr UiRect kM3WinComboR109=MiscSliderR109(0,204),kM3WinRiposteR109=MiscSliderR109(1,204),kM3WinStandardR109=MiscSliderR109(2,204),kM3WinAiR109=MiscSliderR109(3,204),kM3WinPlR109=MiscSliderR109(4,204),kM3WinBlockR109=MiscSliderR109(5,204);
constexpr UiRect kM3PbScaleR109=MiscSliderR109(6,204),kM3MsScaleR109=MiscSliderR109(7,204),kM3RiposteMsR109=MiscSliderR109(8,204),kM3MinPowerR109=MiscSliderR109(9,204),kM3RepeatMsR109=MiscSliderR109(10,204),kM3HangMassR109=MiscSliderR109(11,204),kM3KillEnergyR109=MiscSliderR109(12,204);
constexpr bool MiscLayoutR109(){
    const UiRect m1[]{kM1DifficultyR109,kM1DefenseSpeedR109,kM1BlockAnimR109,kM1ParryAnimR109,kM1DodgeAnimR109,kM1SpacingR109,kM1BlockSpeed18R109,kM1DirectLoR109,kM1DirectHiR109,kM1EtaPrepareR109,kM1EtaStrikeR109,kM1RaiseR109,kM1BySkillR109,kM1DirBlockR109,kM1StatusR109};
    const UiRect m2[]{kM2OwnHitsR109,kM2SyncOverdueR109,kM2RecoveryR109,kM2NoRowR109,kM2ReclaimR109,kM2StaggerGuardR109,kM2CounterR109,kM2CounterPerkR109,kM2IntentLiveR109,kM2StaggerRuleR109,kM2MsModeR109,kM2CounterEnergyR109,kM2CounterMulR109,kM2CounterWindowR109,kM2IntentWaitR109,kM2HelpR109};
    const UiRect m3[]{kM3RouterR109,kM3StrikePowerR109,kM3WinComboR109,kM3WinRiposteR109,kM3WinStandardR109,kM3WinAiR109,kM3WinPlR109,kM3WinBlockR109,kM3PbScaleR109,kM3MsScaleR109,kM3RiposteMsR109,kM3MinPowerR109,kM3RepeatMsR109,kM3HangMassR109,kM3KillEnergyR109};
    const UiRect* pages[]{m1,m2,m3};const unsigned counts[]{unsigned(std::size(m1)),unsigned(std::size(m2)),unsigned(std::size(m3))};
    for(unsigned p=0;p<3;++p)for(unsigned i=0;i<counts[p];++i){const auto r=pages[p][i];
        if(r.l<620||r.r>1038||r.t<104||r.b>672)return false; // below the nav row (72..102), inside the clip (672)
        if(i&&pages[p][i-1].b>r.t)return false;} // rows in reading order, no overlap
    return true;
}
static_assert(MiscLayoutR109(),"R109 MISC rows must stay inside the page in reading order");
constexpr UiRect kUtilityRecenter{-10,-10,-5,-5};

constexpr UiRect kPageBack{600,72,682,102};
constexpr UiRect kVrNavView{694,72,806,102};
constexpr UiRect kVrNavHud{814,72,920,102};
constexpr UiRect kVrNavActions{928,72,1040,102};

constexpr UiRect kV4HeadHeight{620,142,1038,166};
constexpr UiRect kV4HeadBack{620,196,1038,220};
constexpr UiRect kV4GameFov{620,250,948,274};
constexpr UiRect kV4GameFovApply{960,248,1038,276};
constexpr UiRect kV4SourceFov{620,304,1038,328};
constexpr UiRect kV4Zoom{620,358,1038,382};
constexpr UiRect kV4CenterX{620,400,1038,424};
constexpr UiRect kV4CenterY{620,438,1038,462};
constexpr UiRect kV4FlatScale{620,476,1038,500};
constexpr UiRect kV4AutoProjectionCenter{620,506,1038,530};
constexpr UiRect kV4RecalculateProjection{620,506,816,532};
constexpr UiRect kV4CenterProjection{842,506,1038,532};
constexpr UiRect kV4Recenter{620,535,816,567};
constexpr UiRect kV4ResetView{842,535,1038,567};

constexpr UiRect kV4UiMaster{620,112,822,136};
constexpr UiRect kV4UiAuto{832,112,1038,136};
constexpr UiRect kV4UiCompact{620,154,1038,178};
constexpr UiRect kV4UiSafeWidth{620,202,1038,226};
constexpr UiRect kPauseOverlay124{620,548,824,572}; // only on INTERACTION / PROMPTS; R160 was 508..532; R160 UI: left half, one icon size right half
constexpr UiRect kContextualHud162r1{620,580,822,604}; // R141: left half, Minimal HUD right half
constexpr UiRect kMinimalHudR141{832,580,1038,604}; // R141 VR HUD page: Minimal HUD (VrMinimalHudR141)
constexpr UiRect kHudLag22{620,610,1038,634}; // C22b VR HUD page (R115: moved down one row)
constexpr UiRect kHudQuickSlotsR115{620,504,1038,528}; // R115 VR HUD page: weapon/potion slots only while selecting
constexpr UiRect kHudBlockMsgR124{620,640,822,664}; // R124 VR HUD page: BLOCK message on/off (VrShowBlockIndicator); R141 left half
constexpr UiRect kHudTutorialsR141{832,640,1038,664}; // R141 VR HUD page: tutorial messages shown/hidden (VrHideTutorialsR141)
constexpr UiRect kV4UiSafeHeight{620,250,1038,274};
constexpr UiRect kAutoUpper49{620,298,1038,322};
constexpr UiRect kAutoLower49{620,346,1038,370};
constexpr UiRect kV4UiTopBoundary{620,250,1038,274};
constexpr UiRect kV4UiBottomBoundary{620,298,1038,322};
constexpr UiRect kV4UiUniformCenterX{620,346,1038,370};
constexpr UiRect kV451HudOffsetX{620,298,1038,322};
constexpr UiRect kV451HudOffsetY{620,346,1038,370};
constexpr UiRect kV4ManualOffsetX30c1{620,346,824,370};
constexpr UiRect kV4ManualOffsetY30c1{834,346,1038,370};
// V64.83: page tabs deliberately sit ABOVE the visibility checks.
constexpr UiRect kV4UiElementTab{620,394,824,430};
constexpr UiRect kV4UiPromptTab{834,394,1038,430};
constexpr UiRect kV4Crosshair{620,442,822,466};
constexpr UiRect kV4LowerIcons{832,442,1038,466};
constexpr UiRect kV4Compass{620,472,822,496};
constexpr UiRect kHudStatus170{832,472,1038,496};
constexpr UiRect kV4UiReset{620,540,806,570};
constexpr UiRect kV4UiApply{818,540,1038,570};
// Legacy hidden main-page controls retained for compatibility with drag enums.
constexpr UiRect kV4SubScale{-10,-10,-5,-5};
constexpr UiRect kV4SubY{-10,-10,-5,-5};
constexpr UiRect kV4DlgScale{-10,-10,-5,-5};
constexpr UiRect kV4DlgY{-10,-10,-5,-5};
constexpr UiRect kV4DlgInset{-10,-10,-5,-5};
constexpr UiRect kV4HintScale{620,138,1038,162};
constexpr UiRect kV431HudDialogueScale{-10,-10,-5,-5};
constexpr UiRect kV431HudDialogueY{-10,-10,-5,-5};
constexpr UiRect kV431HudDialogueInset{-10,-10,-5,-5};
constexpr UiRect kV4HintInset{620,178,1038,202};
constexpr UiRect kV4HintY{620,218,1038,242};

// V62.9 UI ELEMENTS page: persistent compass / HP-stamina / lower quick-slot transforms.
constexpr UiRect kV4ElemCompassScale{620,170,1038,194};
constexpr UiRect kV4ElemCompassY{620,218,1038,242};
constexpr UiRect kV4ElemLowerScale{620,330,1038,354};
constexpr UiRect kV4ElemLowerY{620,378,1038,402};
constexpr UiRect kV4ElemLowerSpread{620,426,1038,450};
constexpr UiRect kCombatCrosshairR112{620,474,1038,498}; // R112 big directional combat crosshair size
constexpr UiRect kCombatCrossR125{620,508,1038,532}; // R125 ELEMENT CONTROLS: combat direction cross on/off (VrCombatCrossR125)
constexpr UiRect kV4ElemBack{620,542,806,574};
constexpr UiRect kV4ElemSave{818,542,1038,574};
// Stats controls are retained in state but intentionally not exposed on this compact page.
constexpr UiRect kV4ElemStatsScale{620,252,1038,276};
constexpr UiRect kV4ElemStatsX{-10,-10,-5,-5};
constexpr UiRect kV4ElemStatsY{-10,-10,-5,-5};
constexpr UiRect kV4ElemBest{-10,-10,-5,-5};

// V64.86 INTERACTION / PROMPTS page. Hints is KCD2's br.actionHints layer;
// TutorialMessage is the separate tl.tutorial control/help layer.
constexpr UiRect kUiPromptHintScale{620,160,1038,184};
constexpr UiRect kUiPromptHintInset{620,208,1038,232};
// R145 "Crafting UI size" (VrMinigamePromptScale): the inset row slot, that row is only drawn without the corrected HUD
// (hud_corrected_active is always true since C33), and the crafting slider is checked first on a click.
constexpr UiRect kCraftUiSizeR145{620,208,1038,232};
constexpr UiRect kUiPromptHintY{620,256,1038,280};
constexpr UiRect kUiControlPromptScale{620,356,824,380}; // R160 UI: left half, tutorial position right half
constexpr UiRect kUiControlPromptX{620,404,1038,428};
constexpr UiRect kUiControlPromptY{620,452,1038,476};
constexpr UiRect kCinematicSizeR107{620,288,1038,312}; // R107 cutscene / dialogue / loading size
// R160: the two dialogue rows sit in the (hidden under the corrected HUD) control prompt X/Y slots; HUD convergence,
// pause overlay and the two buttons moved down one row each (also fixes the pause label drawn over the convergence track).
constexpr UiRect kDlgSizeR160{620,404,1038,428}; // R160 INTERACTION / PROMPTS: dialogue text size (VrUiDialogueScale)
constexpr UiRect kDlgYR160{620,452,1038,476}; // R160 INTERACTION / PROMPTS: dialogue vertical position (VrUiDialogueY)
constexpr UiRect kHudDistanceR110{620,500,1038,524}; // R110 HUD distance (same angular size); R160 was 480..504
constexpr UiRect kUiPanelBack{620,596,802,628}; // R160 was 548..580
constexpr UiRect kUiPanelApply{820,596,1038,628}; // R160 was 548..580
// R160 UI (developer mode, INTERACTION / PROMPTS): the two new rows share a row with an existing slider (half width each)
constexpr UiRect kTutorialYR160{834,356,1038,380}; // R160 UI: tutorial box position (VrUiTutorialYR160), dev only
constexpr UiRect kUnifiedDevR160{834,548,1038,572}; // R160 UI: one icon size (VrUiUnifiedScaleR160), same value as the simple page
// R160 UI simple VR HUD page (developer mode OFF). Miki: "the lower icons are not really visible because of the darker
// background at the low edge of the VR menu book". MIS_MENU_BOOK.bmp on the right page (x 620..1038): plain parchment
// down to y ~495, the flower border + coat of arms 495..590, the dark wooden edge from ~625. every row (incl. the
// slider labels 24 px above a track) stays in 104..494, checked by SimpleHudLayoutR160 below.
constexpr UiRect kSimpleIconR160{620,132,1038,156};   // "Icon size" (VrUiUnifiedScaleR160)
constexpr UiRect kSimpleCraftR160{620,182,1038,206};  // "Crafting / minigame icon size" (VrMinigamePromptScale)
constexpr UiRect kModeImmersiveR160{620,228,754,254},kModeMinimalR160{762,228,896,254},kModeNormalR160{904,228,1038,254}; // HUD mode
constexpr UiRect SimpleTickR160(int i){return UiRect{(i&1)?834:620,262+30*(i>>1),(i&1)?1038:824,288+30*(i>>1)};}
constexpr int kSimpleTicksR160=11; // 6 rows of 2, the last right slot is empty
constexpr UiRect kSimpleHudDistR160{620,468,1038,492}; // "HUD distance" (VrHudDistanceR110 / INF)
constexpr bool SimpleHudLayoutR160(){
    const UiRect sliders[]{kSimpleIconR160,kSimpleCraftR160,kSimpleHudDistR160};
    for(const auto& r:sliders)if(r.l<620||r.r>1038||r.t-26<104||r.b>494)return false; // label + badge 26 px above the track
    const UiRect rows[]{kModeImmersiveR160,kModeMinimalR160,kModeNormalR160};
    for(const auto& r:rows)if(r.t<kSimpleCraftR160.b+20||r.b>SimpleTickR160(0).t-8)return false; // header text 20 px above
    for(int i=0;i<kSimpleTicksR160;++i){const auto r=SimpleTickR160(i);if(r.l<620||r.r>1038||r.b>kSimpleHudDistR160.t-30)return false;}
    return kSimpleIconR160.b+26<=kSimpleCraftR160.t;
}
static_assert(SimpleHudLayoutR160(),"R160 UI simple VR HUD page rows must stay above the dark lower edge of the book, no overlap");
// R160 UI: the eleven ticks of the simple VR HUD page (Miki: "the ticks from the previous version (all of them - show block,
// compass, status icons, etc., combat crosshair on/off and dot crosshair on/off etc.)" + "a tutorial on/off tick"). all
// existing keys, the same fields the developer pages / Experiments rows use. inverted = the field means HIDDEN.
template<class S> auto SimpleTickFieldR160(S& s,int i,bool& inverted,const wchar_t*& label) noexcept -> decltype(&s.contextual_hud) {
    inverted=false;
    switch(i){
    case 0:label=L"Block message";return &s.show_block_indicator_h24;           // VrShowBlockIndicator
    case 1:label=L"Tutorials";inverted=true;return &s.live_d39.hideTutorialsR141; // VrHideTutorialsR141 (ticked = shown)
    case 2:label=L"Compass";return &s.hud_compass_visible;                       // VrHudCompassVisible
    case 3:label=L"Status icons";return &s.hud_status_icons_visible;             // VrHudStatusIconsVisible
    case 4:label=L"Lower icons";return &s.hud_lower_icons_visible;               // VrHudLowerIconsVisible (health / lower status)
    case 5:label=L"Quick slots fade";return &s.quick_slots_on_selection_r115;    // VrHudQuickSlotFadeR112
    case 6:label=L"Combat crosshair";return &s.live_d39.combatCrossR125;         // VrCombatCrossR125
    case 7:label=L"Dot crosshair";return &s.hud_crosshair_visible;               // VrHudCrosshairVisible
    case 8:label=L"Cinematic bars";return &s.live_d39.cinematicBarsR107;         // VrCinematicBarsR107
    case 9:label=L"Lockpick solid";return &s.live_d39.lockpickOpaqueR141;        // VrLockpickOpaqueR141
    case 10:label=L"Slow HUD follow";return &s.hud_lag_on;                       // VrHudFollowSlow (old VR HUD page tick)
    default:label=L"";return nullptr;
    }
}
// HUD mode: 0 IMMERSIVE (VrContextualHud palm glance), 1 MINIMAL IMMERSIVE (VrMinimalHudR141, palm glance forced on by
// it), 2 NORMAL (both off). existing keys only, no new mode key.
template<class S> int HudModeR160(const S& s) noexcept {return s.live_d39.minimalHudR141?1:s.contextual_hud?0:2;}
template<class S> void SetHudModeR160(S& s,int mode) noexcept {s.contextual_hud=mode==0;s.live_d39.minimalHudR141=mode==1;}
// Hidden legacy rectangles keep old persistence drag enums build-compatible.
constexpr UiRect kUiPanelCookScale{-10,-10,-5,-5};
constexpr UiRect kUiPanelCookX{-10,-10,-5,-5};
constexpr UiRect kUiPanelCookY{-10,-10,-5,-5};
constexpr UiRect kUiPanelItemScale{-10,-10,-5,-5};
constexpr UiRect kUiPanelItemX{-10,-10,-5,-5};
constexpr UiRect kUiPanelItemY{-10,-10,-5,-5};


constexpr UiRect kV4DialogNative=U49(immersion_menu45::kNative); // C45 grid (immersion_menu45.h)
constexpr UiRect kV4DialogFirst{756,146,892,180};
constexpr UiRect kV4DialogNoCamera=U49(immersion_menu45::kFirstPerson); // C45
constexpr UiRect kV4DialogMedium{620,188,810,222};
constexpr UiRect kV4DialogCloseShot{840,188,1038,222};
constexpr UiRect kV4DialogCloseup{620,230,810,264};
constexpr UiRect kV4DialogMaster{840,230,1038,264};
constexpr UiRect kV4DialogForce{620,270,1038,296};
constexpr UiRect kV4DialogZoomToggle{620,302,824,330};
constexpr UiRect kV4DialogFovToggle{834,302,1038,330};
constexpr UiRect kV4DialogZoom{620,340,1038,362};
constexpr UiRect kV4DialogFov{620,378,1038,400};
constexpr UiRect kV4DialogUiScale=U49(immersion_menu45::kDialogueUiScale); // C45: was 418..440 on top of PRECISE CONTACTS
constexpr UiRect kV4DialogUiY{620,456,1038,478};
constexpr UiRect kV4DialogUiInset{620,494,1038,516};
constexpr UiRect kV149UserHolsters=U49(immersion_menu45::kHolsters),kV149ManualBowReload=U49(immersion_menu45::kBowReload); // C45: one row
constexpr UiRect kPreciseContacts44=U49(immersion_menu45::kPreciseContacts);
constexpr UiRect kBloodyFights49=U49(immersion_menu45::kBloodyFights); // C49 BLOODY FIGHTS (right half of the PRECISE CONTACTS row)
constexpr UiRect kHeadDodge54=U49(immersion_menu45::kHeadDodge54);
constexpr UiRect kDialogueSmooth38=U49(immersion_menu45::kSmoothDialogue); // C38
constexpr UiRect kDecapitation59c7e=U49(immersion_menu45::kDecapitation); // R59.16C7E (half width since R59.16C10R7)
constexpr UiRect kRagdollPhysics59c10r7=U49(immersion_menu45::kRagdoll); // R59.16C10R7
constexpr UiRect kMinigameHands45=U49(immersion_menu45::kMinigameHands); // C45 MINIGAME HANDS: NATIVE / SEMI-PASSENGER
// R59.16C10R8 Immersion > EXPERIMENTS page (all switchable experiments in one place; the combat tuning page stays)
constexpr UiRect kImmersionExperiments59c10r8=U49(immersion_menu45::kExperiments); // R59.16C10R15: moved up from y 656 (below the old clip: never visible)
constexpr UiRect kHmdHeight54=U49(immersion_menu45::kHmdHeight54);
constexpr UiRect kIxPhysBodies{620,130,1038,162},kIxBodyFollow{620,172,824,204},kIxRoomscale{834,172,1038,204},
    kIxGestures{620,214,824,246},kIxRagdoll{834,214,1038,246},kIxDecap{620,256,824,288},kIxCombat{834,256,1038,288},
    kIxImpale32{620,298,1038,330}, // C32
    kIxRagdollPush33{620,340,1038,372}; // C33
constexpr UiRect kIxMapRegion32{620,430,1038,466},kIxClinch32{620,478,1038,514};
constexpr UiRect kIxChestDaggerT3{620,382,1038,418};
constexpr UiRect kTimedPromptsD80{620,528,824,558},kPromptScaleD80{834,528,1038,558};
constexpr UiRect kSupportSlot34=U49(immersion_menu45::kSupportSlot); // C34b/C37: slt_1 may replace the synthetic fallback, never a native frame (sword slider shares the row)
constexpr UiRect kWeaponGrip59c7f=U49(immersion_menu45::kWeaponGrip),kSupportSword59c7f=U49(immersion_menu45::kSupportSword),kSupportPole59c7f=U49(immersion_menu45::kSupportPole); // R59.16C7F live hand/weapon placement
constexpr UiRect kAutoPalm59c8=U49(immersion_menu45::kAutoPalm),kWeaponReleaseT1=U49(immersion_menu45::kWeaponRelease),kTargetDwell59c8=U49(immersion_menu45::kTargetDwell); // R59.16C8
constexpr UiRect kV4NoLetterbox{620,530,1038,556};
constexpr UiRect kV4ActionNative{620,220,810,262};
constexpr UiRect kV4ActionFirst{840,220,1038,262};
constexpr UiRect kActivityHands59c7f[5]{{620,290,1038,320},{620,330,1038,360},{620,370,1038,400},{620,410,1038,440},{620,450,1038,480}};
constexpr UiRect kSmithAutoHold36{620,488,1038,514}; // C37: ACTIONS > SMITHING TOOLS (auto-hold without Grip)
constexpr UiRect kGrabMode61{834,598,1038,624}; // C61 hold mode
constexpr UiRect kGrabReach43{620,598,824,624}; // C43: ACTIONS > GRAB: HAND STAYS ON CONTROLLER
constexpr UiRect kDoorGestures46{620,635,1038,661}; // C46R1: ACTIONS > DOORS
constexpr UiRect kActionOwner59c11{620,130,824,160},kActionHoldAt59c11{834,130,1038,160}; // R59.16C10R11: time owner mode + hold point (click cycles)
constexpr UiRect kQuickAccessTouchR112{834,366,1038,394}; // R113 VR CONFIG > CONTROLS (R118: right half)
constexpr UiRect kControllerLayoutR118{620,366,824,394}; // R118 VR CONFIG > CONTROLS: LAYOUT OPENXR / STEAMVR / PS VR2
constexpr UiRect kV4CtrlHeadLook{620,398,1038,430};
constexpr UiRect kV4CtrlSnap{620,436,816,458}; // C49 review: was 438..472 under the PROMPT ICONS label and the glyph row
constexpr UiRect kV4CtrlSmooth{842,436,1038,458};
constexpr UiRect kV476GlyphXbox{620,474,820,498};
constexpr UiRect kV476GlyphPs{838,474,1038,498};
constexpr UiRect kV6510bPromptLab{620,504,824,532};
constexpr UiRect kComfortVignette51{834,504,1038,532};
constexpr UiRect kV4CtrlLaser{620,536,754,562};
constexpr UiRect kSmoothMove51{762,536,896,562};
constexpr UiRect kHmdWalkingDirection134{904,536,1038,562};
constexpr UiRect kV4CtrlDisplayAuto{620,582,820,606};
constexpr UiRect kV4CtrlDisplayVr{-10,-10,-5,-5}; // V64.89 retired user-facing forced-native mode
constexpr UiRect kV4CtrlDisplayTheater{838,582,1038,606};
constexpr UiRect kRoomscale59{620,612,1038,636}; // V66.178R59
constexpr UiRect kGestureTalk59c5{620,642,1038,666}; // R59.16C5 (C49 review: was 660..678, cut by the 672 page clip)

constexpr UiRect kV4ResPrev{620,142,660,174};
constexpr UiRect kV4ResNext{998,142,1038,174};
constexpr UiRect kV4ResApply{700,196,960,230};
constexpr UiRect kGfxShadowQ49[3]{U49(combat_menu49::kGfxShadow[0]),U49(combat_menu49::kGfxShadow[1]),U49(combat_menu49::kGfxShadow[2])},kGfxEye26=U49(combat_menu49::kGfxEye); // C49: was kGfxShadows26{620,234,826,256} + kGfxEye26{832,..}
constexpr UiRect kV4BestGraphics{-10,-10,-5,-5}; // retired: no hidden global graphics preset
constexpr UiRect kXrScalePrev{620,260,650,290};
constexpr UiRect kXrScaleNext{1008,260,1038,290};
constexpr UiRect kXrScaleApply{874,260,998,290};
constexpr UiRect kV4ViewDistance{620,316,1038,332};
constexpr UiRect kV4Sharpness{620,492,1038,508};
constexpr UiRect kV4Gamma{620,360,1038,376};
constexpr UiRect kV4Brightness{620,404,1038,420};
constexpr UiRect kV4Contrast{620,448,1038,464};
constexpr UiRect kV4FpsToggle{620,532,790,560};
constexpr UiRect kV4Fps{804,536,1038,560};

// Native KCD2 upscaler rows. Cycling stages only; Save for Restart persists without live renderer writes.
constexpr UiRect kUpTechPrev{620,572,650,598};
constexpr UiRect kUpTechNext{1008,572,1038,598};
constexpr UiRect kUpQualPrev{620,604,650,630};
constexpr UiRect kUpQualNext{1008,604,1038,630};
constexpr UiRect kUpPresetPrev{620,636,650,662};
constexpr UiRect kUpPresetNext{830,636,860,662};
constexpr UiRect kUpApply{874,636,1038,662};
// C49 compile-time layout guards: the shadow row vs its GRAPHICS neighbours, the first fight page vs the shared header.
constexpr bool Ov49(const UiRect& a,const UiRect& b){ return a.l<b.r&&b.l<a.r&&a.t<b.b&&b.t<a.b; }
static_assert(!Ov49(kGfxShadowQ49[2],kGfxEye26)&&!Ov49(kGfxShadowQ49[0],kV4ResApply)&&!Ov49(kGfxShadowQ49[1],kV4ResApply)&&!Ov49(kGfxShadowQ49[2],kV4ResApply)&&!Ov49(kGfxEye26,kV4ResApply),"C49 shadow row overlaps SAVE SOURCE RES");
static_assert(!Ov49(kGfxShadowQ49[0],kXrScalePrev)&&!Ov49(kGfxEye26,kXrScaleNext)&&!Ov49(kGfxEye26,kXrScaleApply)&&kGfxShadowQ49[0].b<=kXrScalePrev.t&&kGfxShadowQ49[0].t>=kV4ResApply.b,"C49 shadow row overlaps the XR eye row");
static_assert(!Ov49(kCm43Touch,kExpAutoWindup19c4r1)&&!Ov49(kCm43Fall,kExpAutoWindup19c4r1)&&!Ov49(kCm43Touch,kCm43Fall)&&kCm43Touch.b<=672&&kCm43Fall.b<=672,"C46 MORE COMBAT TUNING: the C43T row overlaps or leaves the clip");
static_assert(!Ov49(kCm49Proximity,kPageBack)&&!Ov49(kCm49Proximity,kVrNavView)&&kCm49Proximity.t-combat_menu49::kSliderLabel>=kPageBack.b,"C49 first slider label overlaps the nav row");
static int UpQualityCount(int tech) noexcept { return graphics_menu_options::QualityCount(tech); }
static const wchar_t* UpTechName(int t) noexcept { return t==4?L"UPSCALER: INTEL XeSS":t==2?L"UPSCALER: NVIDIA DLSS":t==1?L"UPSCALER: AMD FSR":t==0?L"UPSCALER: OFF / NATIVE AA":L"UPSCALER: UNKNOWN / UNAVAILABLE"; }
static const wchar_t* UpQualityName(int t,int q) noexcept {
    static const wchar_t* dl[6]={L"DLSS: Auto",L"DLSS: Quality",L"DLSS: Balanced",L"DLSS: Performance",L"DLSS: Ultra Performance",L"DLAA / Native AA"};
    static const wchar_t* fs[5]={L"FSR: Quality",L"FSR: Balanced",L"FSR: Performance",L"FSR: Ultra Performance",L"FSR: Native AA"};
    static const wchar_t* xe[7]={L"XeSS: Ultra Quality Plus",L"XeSS: Ultra Quality",L"XeSS: Quality",L"XeSS: Balanced",L"XeSS: Performance",L"XeSS: Ultra Performance",L"XeSS: Native AA"};
    if(t==4)return xe[std::clamp(q,0,6)];
    if(t==2) return dl[std::clamp(q,0,5)]; if(t==1) return fs[std::clamp(q,0,4)]; return t==0?L"QUALITY: native game AA":L"Select an upscaler to change settings";
}
// R151: DLSS render presets as NGX values (Default=0, A=1 .. F=6, J=10, K=11, L=12, M=13). Current DLSS 4.x
// runtimes only ship F, J, K, L and M; A-E were removed, so the menu cycles Default/F/J/K/L/M. An older saved
// value (A-E) is still shown by its letter until the user cycles away from it.
static constexpr int kDlssPresetsR151[]{0,6,10,11,12,13};
static const wchar_t* UpPresetName(int t,int p) noexcept {
    static const wchar_t* pr[14]={L"Preset: Default",L"Preset: A",L"Preset: B",L"Preset: C",L"Preset: D",L"Preset: E",L"Preset: F",
        L"Preset: G",L"Preset: H",L"Preset: I",L"Preset: J (DLSS 4)",L"Preset: K (DLSS 4)",L"Preset: L (DLSS 4.5)",L"Preset: M (DLSS 4.5)"};
    return t==2?pr[std::clamp(p,0,13)]:L"Preset: -";
}
static int StepDlssPresetR151(int current,int dir) noexcept {
    constexpr int n=int(std::size(kDlssPresetsR151));int idx=-1;
    for(int i=0;i<n;++i)if(kDlssPresetsR151[i]==current){idx=i;break;}
    if(idx<0){ // unlisted (legacy A-E): step to the nearest listed value in that direction
        if(dir>0){for(int i=0;i<n;++i)if(kDlssPresetsR151[i]>current)return kDlssPresetsR151[i];return kDlssPresetsR151[0];}
        for(int i=n-1;i>=0;--i)if(kDlssPresetsR151[i]<current)return kDlssPresetsR151[i];return kDlssPresetsR151[n-1];}
    return kDlssPresetsR151[(idx+(dir>0?1:n-1))%n];
}

constexpr UiRect kV4ModeMono{620,162,1038,204};
constexpr UiRect kV4ModeCorrected{620,220,1038,262};
constexpr UiRect kV4ModePseudo{-10,-10,-5,-5};
constexpr UiRect kV4ModeNative{620,336,1038,378};
constexpr UiRect kV4PseudoEffect{620,414,1038,438};

struct V624ResolutionPreset { int w; int h; const wchar_t* label; };
constexpr V624ResolutionPreset kV624Res[] = {
    // Normal game/display presets.
    {1280,720,L"1280 x 720  (HD)"},
    {1600,900,L"1600 x 900"},
    {1920,1080,L"1920 x 1080  (Full HD)"},
    {2560,1080,L"2560 x 1080  (Ultrawide)"},
    {2560,1440,L"2560 x 1440  (1440p)"},
    {3440,1440,L"3440 x 1440  (Ultrawide)"},
    {3840,1600,L"3840 x 1600  (Ultrawide)"},
    {3840,2160,L"3840 x 2160  (4K)"},

    // Proven VR source shapes retained for users who prefer the denser near-square raster.
    {1920,1920,L"Square 1920 x 1920  (1.0x)"},
    {2304,2304,L"Square 2304 x 2304  (1.2x)"},
    {2880,2880,L"Square 2880 x 2880  (1.5x)"},
    {3840,3840,L"Square 3840 x 3840  (2.0x)"},
    {1920,1800,L"Nearly square 1920 x 1800  (1.0x)"},
    {2304,2160,L"Nearly square 2304 x 2160  (1.2x)"},
    {2880,2700,L"Nearly square 2880 x 2700  (1.5x)"},
    {3840,3600,L"Nearly square 3840 x 3600  (2.0x)"}
};
constexpr int kV624ResCount=static_cast<int>(sizeof(kV624Res)/sizeof(kV624Res[0]));

// V46.3.1 visual-only parchment palette. No engine or D3D12 behavior is
// changed by these values; the menu still uses the exact V46.3 renderer path.
COLORREF kBg       = RGB(72, 48, 30);      // dark leather / desk surround
COLORREF kPanel    = RGB(226, 207, 160);   // aged parchment
COLORREF kPanel2   = RGB(205, 181, 129);   // darker inset parchment
COLORREF kAccent   = RGB(128, 48, 36);     // rubric / sealing-wax red
COLORREF kText     = RGB(53, 35, 22);      // iron-gall ink
COLORREF kMuted    = RGB(103, 78, 52);     // faded brown ink
COLORREF kGood     = RGB(63, 91, 52);      // muted manuscript green
COLORREF kWarn     = RGB(139, 78, 31);     // ochre warning
COLORREF kRule     = RGB(115, 83, 49);
COLORREF kPaperHi  = RGB(239, 222, 178);

HBITMAP LoadBookSkinBitmap()
{
    static HBITMAP skin = nullptr;
    static bool attempted = false;
    if (attempted) return skin;
    attempted = true;
    HMODULE self = nullptr;
    wchar_t path[MAX_PATH]{};
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&LoadBookSkinBitmap), &self) &&
        GetModuleFileNameW(self, path, MAX_PATH)) {
        wchar_t* slash = wcsrchr(path, L'\\');
        if (slash) *(slash + 1) = L'\0';
        wcscat_s(path, L"MIS_MENU_BOOK.bmp");
        // A loose MIS_MENU_BOOK.bmp beside the ASI still wins (user override); otherwise the copy built into the DLL.
        skin = GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES ?
            static_cast<HBITMAP>(LoadImageW(nullptr, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION)) : nullptr;
    }
    const bool fromFile26 = skin != nullptr;
    if (!skin && self) // C26e: embedded resource (kcd2vr_resources26.rc)
        skin = static_cast<HBITMAP>(LoadImageW(self, MAKEINTRESOURCEW(IDB_KCD2VR_MENU_BOOK), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
    debug_trace::Write(skin ? (fromFile26 ? "V63.0-MENU MIS book skin loaded from MIS_MENU_BOOK.bmp (override)" : "C26e-MENU MIS book skin loaded from the DLL resource")
                            : "V63.0-MENU book skin missing (no file, no resource); procedural parchment fallback");
    return skin;
}

void DrawBookSkin(HDC dc)
{
    HBITMAP skin = LoadBookSkinBitmap();
    if (!skin) return;
    HDC src = CreateCompatibleDC(dc);
    HGDIOBJ old = SelectObject(src, skin);
    BITMAP bm{}; GetObject(skin, sizeof(bm), &bm);
    SetStretchBltMode(dc, HALFTONE);
    StretchBlt(dc, 0, 0, 1100, 680, src, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(src, old);
    DeleteDC(src);
}

void Fill(HDC dc, const UiRect& r, COLORREF c)
{
    RECT rr{r.l, r.t, r.r, r.b};
    HBRUSH b = CreateSolidBrush(c);
    FillRect(dc, &rr, b);
    DeleteObject(b);
}

void Line(HDC dc, int x1, int y1, int x2, int y2, COLORREF c, int width = 1)
{
    HPEN p = CreatePen(PS_SOLID, width, c);
    HGDIOBJ old = SelectObject(dc, p);
    MoveToEx(dc, x1, y1, nullptr);
    LineTo(dc, x2, y2);
    SelectObject(dc, old);
    DeleteObject(p);
}

void Text(HDC dc, HFONT font, const wchar_t* text, int x, int y, int w, int h,
          COLORREF color, UINT flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
    RECT r{x, y, x + w, y + h};
    HGDIOBJ old = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &r, flags);
    SelectObject(dc, old);
}

void DrawCheck(HDC dc, HFONT font, const UiRect& r, bool checked, const wchar_t* label)
{
    const UiRect box{r.l, r.t + 3, r.l + 18, r.t + 21};
    Fill(dc, box, checked ? kAccent : kPaperHi);
    if (checked) Text(dc, font, L"✓", box.l + 1, box.t - 2, 18, 22, RGB(244,230,190), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    Text(dc, font, label, r.l + 28, r.t, r.r - r.l - 28, r.b - r.t, kText);
}

float Clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
float SmoothStep01(float v) { v=Clamp01(v); return v*v*(3.0f-2.0f*v); }

// V64.3R readability pass: controls on the right-hand book page keep their
// original hit box, but the visible/active slider track is ~20% shorter.  The
// freed right edge is reserved for a high-contrast value badge so numeric
// values remain readable over the illustrated parchment background.
UiRect SliderTrackRect(const UiRect& r) noexcept
{
    UiRect out=r;
    if(r.l>=590 && r.r>r.l){
        const int width=r.r-r.l;
        out.r=r.l+std::max(24,static_cast<int>(std::lround(width*0.80)));
    }
    return out;
}
float SliderTFromX(const UiRect& r,int x) noexcept
{
    const UiRect tr=SliderTrackRect(r);
    return Clamp01(static_cast<float>(x-tr.l)/static_cast<float>(std::max(1,tr.r-tr.l)));
}

void DrawSlider(HDC dc, HFONT font, const UiRect& r, const wchar_t* label, float t, const wchar_t* value)
{
    const UiRect tr=SliderTrackRect(r);
    Text(dc, font, label, r.l, r.t - 24, std::max(120,tr.r-r.l-6), 20, kText);
    if(r.l>=590){
        const UiRect badge{std::max(tr.r+8,r.r-94),r.t-25,r.r,r.t-3};
        Fill(dc,badge,kPaperHi);
        Text(dc,font,value,badge.l+3,badge.t,badge.r-badge.l-6,badge.b-badge.t,kText,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
    } else {
        Text(dc, font, value, r.r - 120, r.t - 24, 120, 20, kText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }
    const int cy = (r.t + r.b) / 2;
    Line(dc, tr.l + 4, cy, tr.r - 4, cy, kRule, 4);
    const int x = tr.l + 4 + static_cast<int>((tr.r - tr.l - 8) * Clamp01(t));
    Line(dc, tr.l + 4, cy, x, cy, kAccent, 4);
    Fill(dc, UiRect{x - 6, cy - 8, x + 7, cy + 9}, kPaperHi);
}

void DrawButton(HDC dc, HFONT font, const UiRect& r, const wchar_t* label, COLORREF color = kPanel2)
{
    Fill(dc, r, color);
    Text(dc, font, label, r.l, r.t, r.r-r.l, r.b-r.t, kText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}




bool CompileShader(const char* source, const char* entry, const char* target, ID3DBlob** out)
{
    if (!out) return false;
    *out = nullptr;
    ID3DBlob* errors = nullptr;
    const HRESULT hr = D3DCompile(source, std::strlen(source), "KCD2VRMenu", nullptr, nullptr,
                                  entry, target, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, out, &errors);
    if (FAILED(hr)) {
        if (errors) {
            char msg[512]{};
            std::snprintf(msg, sizeof(msg), "V46.3-MENU shader compile failed: %.440s",
                          static_cast<const char*>(errors->GetBufferPointer()));
            debug_trace::Write(msg);
            errors->Release();
        }
        return false;
    }
    if (errors) errors->Release();
    return true;
}

} // namespace

bool VrMenu::Initialize(ID3D12Device* game_device, HWND game_window, const Callbacks& callbacks)
{
    if (!game_device || !game_window) return false;
    if (initialized_.load(std::memory_order_acquire)) return true;

    device_ = game_device;
    game_window_ = game_window;
    callbacks_ = callbacks;

    // V63.0: seed the Graphics resolution selector from the persisted KCD2VR.ini
    // source resolution instead of always opening on the hardcoded 1920x1800 item.
    {
        VrMenuState initial{};
        ReadState(initial);
        for(int i=0;i<kV624ResCount;++i){
            if(kV624Res[i].w==initial.source_width && kV624Res[i].h==initial.source_height){
                resolution_index_.store(i,std::memory_order_release);
                break;
            }
        }
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = kPanelWidth;
    bmi.bmiHeader.biHeight = -kPanelHeight; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    memory_dc_ = CreateCompatibleDC(nullptr);
    dib_ = CreateDIBSection(memory_dc_, &bmi, DIB_RGB_COLORS, &dib_bits_, nullptr, 0);
    if (!memory_dc_ || !dib_ || !dib_bits_) {
        Shutdown();
        return false;
    }
    old_bitmap_ = SelectObject(memory_dc_, dib_);
    // V64.13: keep the proven menu geometry/sizes, but replace the decorative serif/Old-English
    // faces with the Windows UI sans-serif family. The previous faces were readable on desktop
    // but too ornate/soft in-headset at the existing angular size. No font asset is bundled.
    font_normal_ = CreateFontW(-14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_SWISS, L"Segoe UI Semibold");
    font_small_ = CreateFontW(-11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_SWISS, L"Segoe UI Semibold");
    font_tiny_ = CreateFontW(-9, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, L"Segoe UI Semibold");
    font_title_ = CreateFontW(-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_SWISS, L"Segoe UI Semibold");
    font_hero_ = CreateFontW(-43, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, L"Segoe UI Semibold");

    input_hook_installed_ = SetWindowSubclass(game_window_, &VrMenu::WindowSubclassProc, kSubclassId, reinterpret_cast<DWORD_PTR>(this)) != FALSE;
    if (!input_hook_installed_) {
        debug_trace::Write("V46.3-MENU: SetWindowSubclass failed; overlay can render but mouse/keyboard menu input is unavailable");
    }

    initialized_.store(true, std::memory_order_release);
    dirty_.store(true, std::memory_order_release);
    debug_trace::Write("V62.7-MENU initialized: parchment multi-page book + slide transitions + aim-pose pointer + stick page navigation");
    return true;
}

void VrMenu::Shutdown()
{
    SetVisible(false);
    if (game_window_ && input_hook_installed_)
        RemoveWindowSubclass(game_window_, &VrMenu::WindowSubclassProc, kSubclassId);
    input_hook_installed_ = false;

    ReleaseGpuResources();

    if (memory_dc_ && old_bitmap_) SelectObject(memory_dc_, old_bitmap_);
    if (font_hero_) DeleteObject(font_hero_);
    if (font_title_) DeleteObject(font_title_);
    if (font_tiny_) DeleteObject(font_tiny_);
    if (font_small_) DeleteObject(font_small_);
    if (font_normal_) DeleteObject(font_normal_);
    if (dib_) DeleteObject(dib_);
    if (memory_dc_) DeleteDC(memory_dc_);
    font_hero_ = font_title_ = font_small_ = font_normal_ = nullptr;
    dib_ = nullptr; memory_dc_ = nullptr; old_bitmap_ = nullptr; dib_bits_ = nullptr;

    callbacks_ = {};
    game_window_ = nullptr;
    device_ = nullptr;
    initialized_.store(false, std::memory_order_release);
}

void VrMenu::ReleaseGpuResources()
{
    if (fence_ && fence_value_ && fence_->GetCompletedValue() < fence_value_ && fence_event_) {
        if (SUCCEEDED(fence_->SetEventOnCompletion(fence_value_, fence_event_)))
            WaitForSingleObject(fence_event_, 1000);
    }
    DeviceHook::SetInternalMenuCommandList(nullptr);
    if (fence_event_) { CloseHandle(fence_event_); fence_event_ = nullptr; }
    if (upload_buffer_) { if (upload_mapped_) upload_buffer_->Unmap(0, nullptr); upload_buffer_->Release(); }
    if (ui_texture_) ui_texture_->Release();
    if (pipeline_state_) pipeline_state_->Release();
    if (root_signature_) root_signature_->Release();
    if (srv_heap_) srv_heap_->Release();
    if (rtv_heap_) rtv_heap_->Release();
    if (fence_) fence_->Release();
    if (command_list_) command_list_->Release();
    if (allocator_) allocator_->Release();
    upload_buffer_ = nullptr; upload_mapped_ = nullptr; ui_texture_ = nullptr;
    pipeline_state_ = nullptr; root_signature_ = nullptr; srv_heap_ = nullptr; rtv_heap_ = nullptr;
    fence_ = nullptr; command_list_ = nullptr; allocator_ = nullptr;
    fence_value_ = 0; ui_texture_ready_ = false; render_target_format_ = DXGI_FORMAT_UNKNOWN;
}

void VrMenu::ReadState(VrMenuState& out) const
{
    out = VrMenuState{};
    if (callbacks_.read_state) callbacks_.read_state(callbacks_.context, &out);
}

bool VrMenu::ApplyState(const VrMenuState& state)
{
    const bool ok = callbacks_.apply_state && callbacks_.apply_state(callbacks_.context, &state);
    if (ok) MarkDirty();
    return ok;
}

void VrMenu::MarkDirty()
{
    dirty_.store(true, std::memory_order_release);
}

void VrMenu::Toggle()
{
    if (game_window_ && input_hook_installed_) {
        const DWORD window_tid = GetWindowThreadProcessId(game_window_, nullptr);
        if (window_tid && window_tid != GetCurrentThreadId()) {
            DWORD_PTR ignored = 0;
            if (SendMessageTimeoutW(game_window_, kMsgToggleMenu, 0, 0,
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored)) return;
        }
    }
    SetVisible(!visible_.load(std::memory_order_acquire));
}

void VrMenu::SetBlockIndicatorH24(bool active)
{
    if(block_indicator_h24_.exchange(active,std::memory_order_acq_rel)!=active)MarkDirty();
    // R120: redraw when the label switches between BLOCK and SHIELD BLOCK while the block stays held.
    const bool shield=active&&shield_block_r120::indicatorShield.load(std::memory_order_relaxed);
    if(block_indicator_shield_r120_.exchange(shield,std::memory_order_acq_rel)!=shield)MarkDirty();
}

void VrMenu::SetCaptureCountdown(bool active, int seconds_remaining)
{
    // -1 = active VRIK, -2 = calibration/activation failed. Publish text before visibility.
    const int priorSeconds=capture_countdown_seconds_.exchange(seconds_remaining,std::memory_order_acq_rel);
    const bool priorActive=capture_countdown_.exchange(active,std::memory_order_acq_rel);
    if(priorSeconds!=seconds_remaining||priorActive!=active)MarkDirty();
}

void VrMenu::SetVisible(bool visible)
{
    if (game_window_ && input_hook_installed_) {
        const DWORD window_tid = GetWindowThreadProcessId(game_window_, nullptr);
        if (window_tid && window_tid != GetCurrentThreadId()) {
            DWORD_PTR ignored = 0;
            if (SendMessageTimeoutW(game_window_, kMsgSetMenuVisible, visible ? 1 : 0, 0,
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored)) return;
        }
    }
    const bool old = visible_.exchange(visible, std::memory_order_acq_rel);
    if (old == visible) return;

    if (visible) {
        // V62.4: always open on the clean parchment home page.
        ui_tuning_page_.store(false,std::memory_order_release);
        active_page_.store(static_cast<int>(MenuPage::Home),std::memory_order_release);
        page_animating_.store(false,std::memory_order_release);
        old_clip_valid_ = GetClipCursor(&old_clip_rect_) != FALSE;
        ClipCursor(nullptr);
        cursor_show_increments_ = 0;
        int count = ShowCursor(TRUE);
        ++cursor_show_increments_;
        while (count < 0 && cursor_show_increments_ < 16) {
            count = ShowCursor(TRUE);
            ++cursor_show_increments_;
        }
        MarkDirty();
        debug_trace::Write("V46.3-MENU OPEN");
    } else {
        drag_control_.store(static_cast<int>(DragControl::None), std::memory_order_release);
        preview_transparent_.store(false,std::memory_order_release); mouse_down_started_ms_=0; vr_trigger_started_ms_=0;
        if (GetCapture() == game_window_) ReleaseCapture();
        for (int i = 0; i < cursor_show_increments_; ++i) ShowCursor(FALSE);
        cursor_show_increments_ = 0;
        if (old_clip_valid_) ClipCursor(&old_clip_rect_);
        old_clip_valid_ = false;
        debug_trace::Write("V46.3-MENU CLOSED");
    }
}

LRESULT CALLBACK VrMenu::WindowSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                             UINT_PTR subclass_id, DWORD_PTR ref_data)
{
    (void)subclass_id;
    auto* self = reinterpret_cast<VrMenu*>(ref_data);
    LRESULT result = 0;
    if (self && self->HandleWindowMessage(hwnd, msg, wparam, lparam, result)) return result;
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool VrMenu::HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& result)
{
    if (msg == kMsgToggleMenu) {
        SetVisible(!visible_.load(std::memory_order_acquire)); result = 0; return true;
    }
    if (msg == kMsgSetMenuVisible) {
        SetVisible(wparam != 0); result = 0; return true;
    }
    // Reserve the remaining mod hotkeys from vanilla input.
    if ((msg == WM_KEYDOWN || msg == WM_KEYUP) &&
        (wparam == VK_F2 || wparam == VK_F3 || wparam == VK_F5 || wparam == VK_F6 || wparam == VK_F7 || wparam == VK_F8 || wparam == VK_F10 || wparam == VK_F11 || wparam == VK_F12) &&
        developer_mode::FunctionKeyAvailable(unsigned(wparam-VK_F1+1),(GetAsyncKeyState(VK_SHIFT)&0x8000)!=0,(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0)) {
        result = 0;
        return true;
    }
    const bool visible = visible_.load(std::memory_order_acquire);
    if (!visible) return false;

    if (msg == WM_KEYUP && wparam == VK_ESCAPE) {
        SetVisible(false); result = 0; return true;
    }
    if (msg == WM_SETCURSOR) {
        SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32512))); result = TRUE; return true;
    }

    auto local_xy = [&](int& x, int& y) {
        int cx = GET_X_LPARAM(lparam);
        int cy = GET_Y_LPARAM(lparam);
        RECT cr{};
        const std::uint32_t bw = last_backbuffer_width_.load(std::memory_order_acquire);
        const std::uint32_t bh = last_backbuffer_height_.load(std::memory_order_acquire);
        if (GetClientRect(hwnd, &cr)) {
            const int cw = cr.right - cr.left;
            const int ch = cr.bottom - cr.top;
            if (cw > 0 && ch > 0 && bw > 0 && bh > 0) {
                cx = static_cast<int>((static_cast<long long>(cx) * bw) / cw);
                cy = static_cast<int>((static_cast<long long>(cy) * bh) / ch);
            }
        }
        x = cx - kPanelX;
        y = cy - kPanelY;
    };

    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        // V46.5: consume game mouse messages while the menu is open, but do
        // not use them for interaction. PollInput() is authoritative so raw
        // input/fullscreen/window-scaling cannot make visual cursor and hit
        // boxes diverge or double-toggle controls.
        MarkDirty(); result = 0; return true;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_INPUT:
    case WM_KEYDOWN:
    case WM_CHAR:
        result = 0; return true;
    default:
        break;
    }
    return false;
}

void VrMenu::ToggleDeepSeekLabH29()
{
    if(!developer_mode::Enabled())return;
    if(IsVisible()&&static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::DeepSeekLabH29){SetVisible(false);return;}
    SetVisible(true);
    GoToPage(MenuPage::DeepSeekLabH29,+1);
}

bool VrMenu::IsDeveloperPage(MenuPage page) noexcept
{
    switch(page){
    case MenuPage::Home:case MenuPage::Graphics:case MenuPage::VrView:
    case MenuPage::VrHud:case MenuPage::VrHudElementControls:case MenuPage::VrHudElements:
    case MenuPage::VrControls:case MenuPage::ReleaseImmersionR92:return false;
    default:return true;
    }
}

bool VrMenu::IsMiscPageR109(MenuPage page) noexcept
{
    return page==MenuPage::MiscCombat1R109||page==MenuPage::MiscCombat2R109||page==MenuPage::MiscCombat3R109;
}

void VrMenu::GoToPage(MenuPage page, int direction)
{
    // One owner for duplicated experiment settings; old navigation opens its group.
    if(page==MenuPage::Experiments||page==MenuPage::CounterStab40){liveGroupD39_.store(unsigned(live_settings_d39::Group::Combat));page=MenuPage::LiveExperimentsD39;}
    if(page==MenuPage::VrikExperimentsD61){liveGroupD39_.store(unsigned(live_settings_d39::Group::Hands));page=MenuPage::LiveExperimentsD39;}

    if(!developer_mode::Enabled()&&IsDeveloperPage(page))page=MenuPage::Home;
    const auto current=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(current==page && !page_animating_.load(std::memory_order_acquire)){MarkDirty();return;}
    active_page_.store(static_cast<int>(page),std::memory_order_release);
    page_slide_dir_.store(direction<0?-1:1,std::memory_order_release);
    page_anim_start_ms_.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
    page_animating_.store(true,std::memory_order_release);
    drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
    MarkDirty();
}

void VrMenu::UpdatePageAnimation()
{
    if(!page_animating_.load(std::memory_order_acquire)) return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto begin=page_anim_start_ms_.load(std::memory_order_acquire);
    if(begin==0 || now-begin>=220ull) {
        page_animating_.store(false,std::memory_order_release);
        MarkDirty();
        return;
    }
    MarkDirty();
}

void VrMenu::CyclePage(int direction)
{
    static constexpr MenuPage pages[]={MenuPage::Home,MenuPage::VrView,MenuPage::VrHud,MenuPage::VrControls,MenuPage::HandOffsetsR150,MenuPage::ReleaseImmersionR92,MenuPage::Dialogue,MenuPage::VrActions,MenuPage::Graphics,MenuPage::LiveExperimentsD39,MenuPage::ExperimentalFeaturesH5,MenuPage::NpcTuning,MenuPage::ClaudeExperimentsH8,MenuPage::CombatTuning49,MenuPage::Experiments2,MenuPage::WeaponFeel59c14,MenuPage::CombatFlow59c17,MenuPage::Kicks26,MenuPage::MeleeSensitivityH29,MenuPage::FistSensitivityH41,MenuPage::GuardSettingsH41,MenuPage::ShieldBoxR120,MenuPage::DefenceWindowsH41,MenuPage::MiscCombat1R109,MenuPage::MiscCombat2R109,MenuPage::MiscCombat3R109}; // R109 MISC pages cycle with the stick too
    const auto cur=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    int idx=0;
    for(int i=0;i<static_cast<int>(sizeof(pages)/sizeof(pages[0]));++i) if(pages[i]==cur){ idx=i; break; }
    const int count=static_cast<int>(sizeof(pages)/sizeof(pages[0]));
    do {idx=(idx+(direction<0?-1:1)+count)%count;}
    while(!developer_mode::Enabled()&&IsDeveloperPage(pages[idx]));
    GoToPage(pages[idx],direction);
}

void VrMenu::HandleClick(int x, int y)
{
    if(page_animating_.load(std::memory_order_acquire)) return;
    VrMenuState s{}; ReadState(s);
    auto save=[&](){ if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); };
    auto selectStereoD81R2=[&](){
        if(s.stereo_mode!=3){
            int actual=-1,quality=0,preset=0;
            const bool known=callbacks_.read_upscaler&&callbacks_.read_upscaler(callbacks_.context,&actual,&quality,&preset);
            if(!stereo_upscaler_d81r2::canKeep(3,known,actual)){
                // Show the explicit next-start repair; do not change live AA or
                // claim that Stereo was selected when its history is unsupported.
                up_tech_.store(0);up_quality_.store(0);up_preset_.store(0);
                up_status_.store(4);up_seeded_.store(true);
                GoToPage(MenuPage::Graphics,+1);return;
            }
        }
        s.stereo_mode=3;s.cyclopean_mode=0;
        if(ApplyState(s)){
            save(); // persist the request; the worker alone may activate Stereo
            if(!callbacks_.stereo_ready||!callbacks_.stereo_ready(callbacks_.context)){
                up_status_.store(5);GoToPage(MenuPage::Graphics,+1);
            }
        }else{
            // The backend re-read may have become unknown/unsupported after
            // the UI read. Offer an explicit supported choice without saving.
            up_tech_.store(0);up_quality_.store(0);up_preset_.store(0);
            up_status_.store(4);up_seeded_.store(true);GoToPage(MenuPage::Graphics,+1);
        }
    };
    auto selectAlternateEyeR142=[&](){
        int actual=-1,quality=0,preset=0;
        const bool known=callbacks_.read_upscaler&&callbacks_.read_upscaler(callbacks_.context,&actual,&quality,&preset);
        if(!known||actual!=2){
            up_tech_.store(2);up_quality_.store(graphics_menu_options::DefaultQuality(2));up_preset_.store(0);
            up_status_.store(4);up_seeded_.store(true);GoToPage(MenuPage::Graphics,+1);return;
        }
        s.stereo_mode=4;s.cyclopean_mode=0;
        if(ApplyState(s)){
            save();
            if(!callbacks_.stereo_ready||!callbacks_.stereo_ready(callbacks_.context)){
                up_status_.store(5);GoToPage(MenuPage::Graphics,+1);
            }
        }else{
            up_tech_.store(2);up_quality_.store(graphics_menu_options::DefaultQuality(2));up_preset_.store(0);
            up_status_.store(4);up_seeded_.store(true);GoToPage(MenuPage::Graphics,+1);
        }
    };
    if(s.matrix_running){ if(callbacks_.request_safe_mono) callbacks_.request_safe_mono(callbacks_.context); return; }

    const auto page=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(!developer_mode::Enabled()&&IsDeveloperPage(page)){GoToPage(MenuPage::Home,-1);return;}
    if(page==MenuPage::NpcTuning&&PtIn(kPageBack,x,y)){GoToPage(MenuPage::Experiments,-1);return;}
    if(page==MenuPage::ClaudeExperimentsH8&&PtIn(kPageBack,x,y)){GoToPage(MenuPage::Experiments,-1);return;}
    if(IsMiscPageR109(page)){ // R109 MISC: BACK returns to the live COMBAT group, the three tabs switch pages
        if(PtIn(kPageBack,x,y)){GoToPage(MenuPage::Experiments,-1);return;}
        if(PtIn(kMiscTab1R109,x,y)){GoToPage(MenuPage::MiscCombat1R109,+1);return;}
        if(PtIn(kMiscTab2R109,x,y)){GoToPage(MenuPage::MiscCombat2R109,+1);return;}
        if(PtIn(kMiscTab3R109,x,y)){GoToPage(MenuPage::MiscCombat3R109,+1);return;}
    }
    if(page==MenuPage::RangedOptions32){
#define KCD2VR_RANGED32_CLICK
#include "subsystems/ranged_reload/menu_xr32.inl"
#undef KCD2VR_RANGED32_CLICK
    }
    if(page==MenuPage::ShieldBoxR120&&PtIn(kPageBack,x,y)){GoToPage(MenuPage::GuardSettingsH41,-1);return;} // R120
    if(page==MenuPage::HandOffsetsR150){ // R150 HAND OFFSETS
        if(PtIn(kPageBack,x,y)){GoToPage(MenuPage::Home,-1);return;}
        if(PtIn(kHandOffsetResetRR150,x,y)||PtIn(kHandOffsetResetLR150,x,y)){
            const int side=PtIn(kHandOffsetResetRR150,x,y)?0:1;for(int i=0;i<6;++i)s.hand_offset_r150[side*6+i]=0.f;
            ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kHandAlignRR153,x,y)||PtIn(kHandAlignLR153,x,y)){ // R153: start, or cancel when already aligning
            const int side=PtIn(kHandAlignRR153,x,y)?0:1;
            frik_pose_probe::RequestHandAlignR153(side,frik_pose_probe::HandAlignStateR153(side)==0);MarkDirty();return;}
        for(unsigned i=0;i<12;++i)if(PtIn(HandOffsetSliderR150(i),x,y)){handOffsetSlotR150_.store(i,std::memory_order_release);
            drag_control_.store(static_cast<int>(DragControl::HandOffsetR150),std::memory_order_release);HandleDrag(x,y);return;}
        return;
    }
    if(page==MenuPage::ImmersionExperiments){
#define KCD2VR_MENU_CLICK_IMMERSIONEXPERIMENTS
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_CLICK_IMMERSIONEXPERIMENTS
}
    if((page==MenuPage::ExperimentalFeaturesH5||page==MenuPage::DeepSeekLabH29)&&PtIn(kPageBack,x,y)){GoToPage(MenuPage::LiveExperimentsD39,-1);return;}
    if(page!=MenuPage::Home && PtIn(kPageBack,x,y)){ GoToPage(MenuPage::Home,-1); return; }
    if(page==MenuPage::ReleaseImmersionR92){
        namespace release=release_menu_r92;namespace live=live_settings_d39;
        // R124: three pages (page 3 = BLOCK message); < steps back, > steps forward, both wrap
        if(PtIn(ED39(live::kMotionPrevD69),x,y)){releasePageR92_.store((releasePageR92_.load()+release::kPages-1)%release::kPages);MarkDirty();return;}
        if(PtIn(ED39(live::kMotionNextD69),x,y)){releasePageR92_.store((releasePageR92_.load()+1)%release::kPages);MarkDirty();return;}
        const auto& rows=release::rows(releasePageR92_.load());
        for(unsigned i=0;i<rows.size();++i)if(PtIn(ED39(live::rowRect(i)),x,y)&&release::advance(s,rows[i].id)){
            if(ApplyState(s))save();
            if(rows[i].id==release::Row::Gore){const int pak=kcd2vr::gorepak50::state.load(); // R98: pak state goes to KCD2VR.log only, never to the menu
                char m[300]{};std::snprintf(m,sizeof(m),"%s VrBloodyFights=%d gorePak=%s",DecapTrace46(s,"R98-IMMERSION"),s.bloody_fights49?1:0,pak==1?"INSTALLED":pak==2?"MISSING (Mods/kcd2vr_gore not found: no cuts until it is installed)":"NOT_CHECKED_YET");diagnostic_trace::Write(m);}
            MarkDirty();return;
        }
        return;
    }
    if(page==MenuPage::LiveExperimentsD39){
        namespace live=live_settings_d39;unsigned selected=liveGroupD39_.load();
        if(live::group(selected)==live::Group::Combat&&PtIn(kMiscNavR109,x,y)){GoToPage(MenuPage::MiscCombat1R109,+1);return;} // R109 MISC >
        const auto result=live::dispatch(selected,x,y,s,[&](const VrMenuState& requested){return ApplyState(requested);},
            [&](){return callbacks_.save_settings&&callbacks_.save_settings(callbacks_.context);});
        liveGroupD39_.store(selected);
        if(result==live::Click::Trials)GoToPage(live::group(selected)==live::Group::Combat?MenuPage::CombatTuning49:MenuPage::ExperimentalFeaturesH5,+1);
        else if(result==live::Click::Combat)GoToPage(live::group(selected)==live::Group::Combat?MenuPage::CombatFlow59c17:MenuPage::Experiments,+1);
        else if(result==live::Click::Crafting)GoToPage(live::group(selected)==live::Group::Combat?MenuPage::MeleeSensitivityH29:MenuPage::Crafting67,+1);
        else if(result!=live::Click::None){liveStatusD39_.store(result==live::Click::ApplyFailed?1:result==live::Click::SaveFailed?2:0);MarkDirty();}
        return;
    }
    if(page==MenuPage::ExperimentalFeaturesH5){
        namespace ef=experimental_features_h5;
        for(unsigned i=0;i<ef::kGroups.size();++i)if(PtIn(EH5(ef::kGroups[i]),x,y)){experimentalGroupH5_.store(i);MarkDirty();return;}
        if(PtIn(EH5(ef::kCombatLink),x,y)){GoToPage(MenuPage::Experiments,+1);return;}
        if(PtIn(EH5(ef::kAllOff),x,y)){
            deepseek_h29::DisableAll();
            frik_pose_probe::SetFistKickEnabledH35(false);grip_stow_cb1::SetEnabled(false);
            kcd2vr::alchemy_own_head_h51::enabled.store(false,std::memory_order_release);
            kcd2vr::hand_only_door_h40::displayEnabled.store(false,std::memory_order_release);
            if(s.physical_f9_rung61!=0){ReadState(s);s.physical_f9_rung61=0;ApplyState(s);}
            MarkDirty();return; // Session owners only; no save or INI/default reset.
        }
        const auto cards=ef::cards(ef::group(experimentalGroupH5_.load()));
        for(unsigned i=0;i<cards.size();++i)if(cards[i].trial!=ef::Trial::None&&PtIn(EH5(ef::kCards[i]),x,y)){
            if(cards[i].trial==ef::Trial::F9){
                s.physical_f9_rung61=std::clamp(s.physical_f9_next_h6,0,3);
                ApplyState(s);MarkDirty();return; // Physical gameplay selection; F9 recording is independent.
            }
            if(ef::toggle(cards[i].trial))MarkDirty();return;
        }
        return; // Status rows do not pretend to own unavailable native functions.
    }
    if(page==MenuPage::DeepSeekLabH29){
        if(PtIn(kDsFeaturesH5,x,y)){GoToPage(MenuPage::ExperimentalFeaturesH5,+1);return;}
        if(PtIn(kDsWallH31,x,y)){
            const bool on=!deepseek_h29::wallBackoutSpringH31.load(std::memory_order_acquire);
            deepseek_h29::wallBackoutSpringH31.store(on,std::memory_order_release);
            diagnostic_trace::Write(on?"H31 DEEPSEEK checkedWallBackout=ON sessionOnly=1":"H31 DEEPSEEK checkedWallBackout=OFF sessionOnly=1");
            MarkDirty();
        }else if(PtIn(kDsFingerH31,x,y)){
            const bool on=!deepseek_h29::fingerContactSpringH31.load(std::memory_order_acquire);
            deepseek_h29::fingerContactSpringH31.store(on,std::memory_order_release);
            diagnostic_trace::Write(on?"H31 DEEPSEEK fingerContactResponse=ON sessionOnly=1":"H31 DEEPSEEK fingerContactResponse=OFF sessionOnly=1");
            MarkDirty();
        }else if(PtIn(kDsOffH29,x,y)){deepseek_h29::DisableAll();MarkDirty();}
        return;
    }

    if(page==MenuPage::VrikExperimentsD61){
        if(PtIn(kPageBack,x,y)){GoToPage(MenuPage::Home,-1);return;}
        if(PtIn(kVrikLeadD61,x,y)){s.free_hand_present_d61=!s.free_hand_present_d61;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kPickupCameraD65,x,y)){s.live_d39.pickupCameraBridgeD55=!s.live_d39.pickupCameraBridgeD55;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kRepairTrialsD65,x,y)){liveGroupD39_.store(unsigned(live_settings_d39::Group::MotionD67));GoToPage(MenuPage::LiveExperimentsD39,+1);return;}
        const UiRect choices[]{kVrikBaseD61,kVrikTimingD61,kVrikUncappedD67,kVrikElbowD61};
        for(unsigned i=0;i<std::size(choices);++i)if(PtIn(choices[i],x,y)){s.vrik_comparison_d61=int(i);s.live_d39.motionTrialD67=int(motion_trials_d67::MenuModeForChoice(i));ApplyState(s);save();MarkDirty();return;}
        return;
    }
    if(page==MenuPage::Home){
        if(developer_mode::Enabled()&&PtIn(kVrikExperimentsD61,x,y)){GoToPage(MenuPage::VrikExperimentsD61,+1);return;}
        if(developer_mode::Enabled()&&PtIn(kH29TuneNav,x,y)){GoToPage(MenuPage::MeleeSensitivityH29,+1);return;}
        if(PtIn(kHomeModeMono,x,y)){ s.stereo_mode=0; s.cyclopean_mode=0; ApplyState(s); save(); return; }
        if(PtIn(kHomeModeCorrected,x,y)){ selectStereoD81R2(); return; }
        if(PtIn(kHomeModeAlternateEyeR142,x,y)){ selectAlternateEyeR142(); return; }
        if(s.stereo_mode==4&&PtIn(kHomeAerWarpR154,x,y)){ // R154: MV -> 6DOF -> ROT -> MV
            s.aer_warp_mode_r154=(std::clamp(s.aer_warp_mode_r154,0,2)+2)%3;ApplyState(s);save();MarkDirty();return;
        }
        if(s.stereo_mode==4&&PtIn(kHomeAerDumpR149,x,y)){
            // Starts 3 s later so the menu can be closed and the problem area looked at.
            const bool armed=native_dlss_stereo_d82::RequestAerFrameDump(4,3000);
            MessageBeep(armed?MB_OK:MB_ICONHAND);MarkDirty();return;
        }
        if(PtIn(kHomeFps,x,y)){
            s.fps_cap_enabled=!s.fps_cap_enabled;
            ApplyState(s);
            if(callbacks_.apply_fps_cap) callbacks_.apply_fps_cap(callbacks_.context,s.fps_cap_enabled?s.fps_cap_target:0.0f);
            save(); MarkDirty(); return;
        }
        if(PtIn(kHomeVrConfig,x,y)){ GoToPage(MenuPage::VrView,+1); return; }
        if(PtIn(kHomeGraphics,x,y)){ GoToPage(MenuPage::Graphics,+1); return; }
        if(PtIn(kHomeUi,x,y)){ GoToPage(MenuPage::VrHud,+1); return; }
        if(PtIn(kHomeDialogue,x,y)){ GoToPage(MenuPage::ReleaseImmersionR92,+1); return; }
        if(PtIn(kHomeActions,x,y)){ GoToPage(MenuPage::VrControls,+1); return; }
        if(developer_mode::Enabled()&&PtIn(kHomeFeaturesH5,x,y)){GoToPage(MenuPage::LiveExperimentsD39,+1);return;}
        if(developer_mode::Enabled()&&PtIn(kHomeExperiments,x,y)){ GoToPage(MenuPage::Experiments,+1); return; } // R59.16C10d
        if(PtIn(kHomeVrikNative,x,y)){ if(callbacks_.set_vrik_mode) callbacks_.set_vrik_mode(callbacks_.context,false); MarkDirty(); return; }
        if(PtIn(kHomeVrikTracked,x,y)){ if(callbacks_.set_vrik_mode) callbacks_.set_vrik_mode(callbacks_.context,true); MarkDirty(); return; }
        if(PtIn(kHomeVrikRecalibrate,x,y)){ if(callbacks_.request_vrik_recalibrate) callbacks_.request_vrik_recalibrate(callbacks_.context); MarkDirty(); return; }
        if(PtIn(kHomeVrikRestore43,x,y)){ if(callbacks_.request_vrik_restore) callbacks_.request_vrik_restore(callbacks_.context); MarkDirty(); return; } // C43
        if(PtIn(kHomeDirectHandsR135,x,y)){s.direct_controller_hands_r135=!s.direct_controller_hands_r135;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kHomeHandOffsetsR150,x,y)){GoToPage(MenuPage::HandOffsetsR150,+1);return;} // R150
        if(PtIn(kHomeAutoRestore43,x,y)){ s.calibration_auto_restore43=!s.calibration_auto_restore43; ApplyState(s); save(); diagnostic_trace::Write(s.calibration_auto_restore43?"C43-HOME VrCalibrationAutoRestore=ON":"C43-HOME VrCalibrationAutoRestore=OFF"); MarkDirty(); return; }
        if(PtIn(kHomeBest,x,y)){ s.balanced_graphics=!s.balanced_graphics; ApplyState(s); save(); return; }
        
        if(PtIn(kHomeSave,x,y)){ if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); return; }
        if(PtIn(kUtilityRecenter,x,y)){ if(callbacks_.request_recenter) callbacks_.request_recenter(callbacks_.context); return; }
        return;
    }

    if(page==MenuPage::VrView || page==MenuPage::VrHud || page==MenuPage::VrHudElementControls || page==MenuPage::VrHudElements || page==MenuPage::VrControls || page==MenuPage::Dialogue || page==MenuPage::VrActions || page==MenuPage::Experiments || page==MenuPage::ClaudeExperimentsH8 || page==MenuPage::CombatTuning49 || page==MenuPage::Experiments2 || page==MenuPage::WeaponFeel59c14 || page==MenuPage::CombatFlow59c17 || page==MenuPage::Kicks26 || page==MenuPage::CounterStab40){
        if(PtIn(kVrNavView,x,y)){ GoToPage(MenuPage::VrView,page==MenuPage::VrView?+1:-1); return; }
        if(PtIn(kVrNavHud,x,y)){ GoToPage(MenuPage::VrHud,+1); return; }
        if(PtIn(kVrNavActions,x,y)){ GoToPage(MenuPage::VrControls,+1); return; }
    }

    auto begin_drag=[&](const UiRect& r,DragControl dc)->bool{
        if(!PtIn(r,x,y)) return false;
        drag_control_.store(static_cast<int>(dc),std::memory_order_release); HandleDrag(x,y); return true;
    };

    if(page==MenuPage::NpcTuning){
#define KCD2VR_MENU_CLICK_NPC_TUNING
#include "subsystems/direct/menu.inl"
#undef KCD2VR_MENU_CLICK_NPC_TUNING
    }

    if(IsMiscPageR109(page)){ // R109 MISC 1/2/3: every row applies at once and saves (same flip/drag pattern as the other combat pages)
        auto flip=[&](const UiRect& r,bool& value,const char* name)->bool{ if(!PtIn(r,x,y))return false; value=!value; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R109-MISC %s=%s",name,value?"ON":"OFF"); diagnostic_trace::Write(m); MarkDirty(); return true; };
        auto cycleN=[&](const UiRect& r,int& value,const char* name,int n)->bool{ if(!PtIn(r,x,y))return false; value=(std::clamp(value,0,n-1)+1)%n; ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"R109-MISC %s=%d",name,value); diagnostic_trace::Write(m); MarkDirty(); return true; };
        auto cycle3=[&](const UiRect& r,int& value,const char* name)->bool{ return cycleN(r,value,name,3); };
        if(page==MenuPage::MiscCombat1R109){
            if(begin_drag(kM1DifficultyR109,DragControl::M1DifficultyR109)||begin_drag(kM1DefenseSpeedR109,DragControl::M1DefenseSpeedR109)||begin_drag(kM1BlockAnimR109,DragControl::M1BlockAnimR109)||begin_drag(kM1ParryAnimR109,DragControl::M1ParryAnimR109)||begin_drag(kM1DodgeAnimR109,DragControl::M1DodgeAnimR109)||begin_drag(kM1SpacingR109,DragControl::M1SpacingR109)||
               begin_drag(kM1BlockSpeed18R109,DragControl::M1BlockSpeed18R109)||begin_drag(kM1DirectLoR109,DragControl::M1DirectLoR109)||begin_drag(kM1DirectHiR109,DragControl::M1DirectHiR109)||begin_drag(kM1EtaPrepareR109,DragControl::M1EtaPrepareR109)||begin_drag(kM1EtaStrikeR109,DragControl::M1EtaStrikeR109))return;
            if(flip(kM1RaiseR109,s.npc_defense_raise109,"VrNpcDefenseRaiseExperiment")||flip(kM1BySkillR109,s.exp_direct_block_timing39,"VrDirectNpcBlockBySkill")||flip(kM1DirBlockR109,s.exp_direct_dirblock39,"VrDirectNpcDirectionalBlock"))return;
            return;
        }
        if(page==MenuPage::MiscCombat2R109){
            if(begin_drag(kM2CounterEnergyR109,DragControl::M2CounterEnergyR109)||begin_drag(kM2CounterMulR109,DragControl::M2CounterMulR109)||begin_drag(kM2CounterWindowR109,DragControl::M2CounterWindowR109)||begin_drag(kM2IntentWaitR109,DragControl::M2IntentWaitR109))return;
            if(flip(kM2OwnHitsR109,s.exp_own_hits18,"VrDirectOwnsHits")||flip(kM2SyncOverdueR109,s.exp_sync_overdue_direct,"VrSyncOverdueDirect")||flip(kM2RecoveryR109,s.exp_strike_in_recovery,"VrDirectStrikeInRecovery")||flip(kM2NoRowR109,s.exp_hit_without_row,"VrDirectHitWithoutDefenseRow")||
               flip(kM2ReclaimR109,s.exp_reclaim_guard,"VrReclaimPhaseFromOwnGuard")||flip(kM2StaggerGuardR109,s.exp_direct_stagger_guard39,"VrDirectGuardOutOfStagger")||flip(kM2CounterR109,s.exp_counter_stab40,"VrDirectCounterStab")||flip(kM2CounterPerkR109,s.exp_counter_stab_perk44,"VrDirectCounterStabNeedsPerk")||
               flip(kM2IntentLiveR109,s.exp_intent_live,"VrIntentLiveRoute"))return;
            if(cycle3(kM2StaggerRuleR109,s.exp_stagger_rule44,"VrDirectStaggerRule")||cycleN(kM2MsModeR109,s.exp_ms_stab_only47,"VrMasterStrikeModeR120",4))return; // R120: 4th choice INSTANT
            return;
        }
        if(page==MenuPage::MiscCombat3R109){
            if(begin_drag(kM3WinComboR109,DragControl::M3WinComboR109)||begin_drag(kM3WinRiposteR109,DragControl::M3WinRiposteR109)||begin_drag(kM3WinStandardR109,DragControl::M3WinStandardR109)||begin_drag(kM3WinAiR109,DragControl::M3WinAiR109)||begin_drag(kM3WinPlR109,DragControl::M3WinPlR109)||begin_drag(kM3WinBlockR109,DragControl::M3WinBlockR109)||
               begin_drag(kM3PbScaleR109,DragControl::M3PbScaleR109)||begin_drag(kM3MsScaleR109,DragControl::M3MsScaleR109)||begin_drag(kM3RiposteMsR109,DragControl::M3RiposteMsR109)||begin_drag(kM3MinPowerR109,DragControl::M3MinPowerR109)||begin_drag(kM3RepeatMsR109,DragControl::M3RepeatMsR109)||begin_drag(kM3HangMassR109,DragControl::M3HangMassR109)||begin_drag(kM3KillEnergyR109,DragControl::M3KillEnergyR109))return;
            if(cycle3(kM3RouterR109,s.win_router_mode109,"VrCombatRouter")||flip(kM3StrikePowerR109,s.strike_power_on109,"VrStrikePower"))return;
            return;
        }
    }

    if(page==MenuPage::ClaudeExperimentsH8){
#define KCD2VR_H8_MENU_CLICK
#include "subsystems/experimental/claude_menu_h8.inl"
#undef KCD2VR_H8_MENU_CLICK
    }
    if(page==MenuPage::VrView){
        if(begin_drag(kV4HeadHeight,DragControl::CameraHeight)) return;
        if(begin_drag(kV4HeadBack,DragControl::CameraBack)) return;
        const bool projectiveRemap=s.cyclopean_mode==1;
        if(begin_drag(kV4GameFov,DragControl::GameFov)) return;
        if(PtIn(kV4GameFovApply,x,y)){
            if(callbacks_.apply_game_fov) callbacks_.apply_game_fov(callbacks_.context,s.game_fov_target);
            save(); MarkDirty(); return;
        }
        // H26: either visible FOV slider edits the same shared setting.
        if(begin_drag(kV4SourceFov,DragControl::SourceFov)) return;
        if(!projectiveRemap && begin_drag(kV4Zoom,DragControl::Zoom)) return;
        if(begin_drag(kV4CenterY,DragControl::ProjectionPitchTrim)) return;
        if(PtIn(kV4RecalculateProjection,x,y)){
            s.cyclopean_mode=1; s.presentation_zoom=1.0f;
            ApplyState(s);
            if(callbacks_.recalculate_projection) (void)callbacks_.recalculate_projection(callbacks_.context);
            save(); MarkDirty(); return;
        }
        if(PtIn(kV4CenterProjection,x,y)){
            ApplyState(s);
            if(callbacks_.center_projection) (void)callbacks_.center_projection(callbacks_.context);
            save(); MarkDirty(); return;
        }
        if(begin_drag(kV4FlatScale,DragControl::FlatScreenScale)) return;
        if(PtIn(kV4Recenter,x,y)){ if(callbacks_.request_recenter) callbacks_.request_recenter(callbacks_.context); return; }
        if(PtIn(kV4ResetView,x,y)){
            // R121: RESET VIEW puts "Head up / down" back to the shipped default (+0.035 m eye clearance above Henry's
            // collar, V66.110/111 after the V66.109 headset run), not to 0.00 - before, the reset gave a different height than a new INI.
            s.camera_back_offset_m=0.0f; s.camera_height_offset_m=0.035f;
            s.stereo_mode=0; s.cyclopean_mode=0;
            // V66.31 restores the actual V65.27 RESET VIEW contract: neutral 97/97.
            // Correct FOV then recalculates whichever independent Game/VR FOV pair the user selects.
            s.game_fov_target=95.0f; s.source_vertical_fov_deg=95.0f; s.corrected_center_scale_fov_deg=95.0f; s.presentation_zoom=0.95f; s.depth_stereo_strength=0.30f;
            s.auto_center_x_trim=0.00f; s.auto_center_y_trim=0.00f; s.presentation_center_x=0.0f; s.presentation_center_y=0.0f; s.auto_projection_center=false; s.projection_pitch_trim_deg=0.0f; s.flat_screen_scale=1.70f;
            ApplyState(s);
            if(callbacks_.apply_game_fov) callbacks_.apply_game_fov(callbacks_.context,s.game_fov_target);
            save(); return;
        }
        return;
    }

    if(page==MenuPage::VrHud){
#define KCD2VR_MENU_CLICK_VRHUD
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_CLICK_VRHUD
}

    if(page==MenuPage::VrHudElementControls){
#define KCD2VR_MENU_CLICK_VRHUDELEMENTCONTROLS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_CLICK_VRHUDELEMENTCONTROLS
}

    if(page==MenuPage::VrHudElements){
#define KCD2VR_MENU_CLICK_VRHUDELEMENTS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_CLICK_VRHUDELEMENTS
}

    if(page==MenuPage::VrControls){
#define KCD2VR_MENU_CLICK_VRCONTROLS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_CLICK_VRCONTROLS
}

    if(page==MenuPage::Experiments&&PtIn(kWindowNavH41,x,y)){GoToPage(MenuPage::DefenceWindowsH41,+1);return;}
    if(page==MenuPage::Experiments&&PtIn(U49(combat_menu49::kNote),x,y)){s.bypass_mod_vetoes_h41=!s.bypass_mod_vetoes_h41;ApplyState(s);save();MarkDirty();return;}
    if(page==MenuPage::Experiments){
        if(PtIn(U49(claude_h8::kOpenCombat),x,y)){GoToPage(MenuPage::ClaudeExperimentsH8,+1);return;}
#define KCD2VR_MENU_CLICK_EXPERIMENTS
#include "subsystems/hybrid/menu.inl"
#undef KCD2VR_MENU_CLICK_EXPERIMENTS
}
    if(page==MenuPage::CombatTuning49){
#define KCD2VR_MENU_CLICK_COMBATTUNING49
#include "subsystems/hybrid/menu.inl"
#undef KCD2VR_MENU_CLICK_COMBATTUNING49
}
    if(page==MenuPage::Experiments2){
#define KCD2VR_MENU_CLICK_EXPERIMENTS2
#include "subsystems/direct/menu.inl"
#undef KCD2VR_MENU_CLICK_EXPERIMENTS2
}
    if(page==MenuPage::WeaponFeel59c14){
#define KCD2VR_MENU_CLICK_WEAPONFEEL59C14
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_CLICK_WEAPONFEEL59C14
}
    if(page==MenuPage::CombatFlow59c17){
#define KCD2VR_MENU_CLICK_COMBATFLOW59C17
#include "subsystems/masterstrike_combos/menu.inl"
#undef KCD2VR_MENU_CLICK_COMBATFLOW59C17
}
    if(page==MenuPage::CounterStab40){
#define KCD2VR_MENU_CLICK_COUNTERSTAB40
#include "subsystems/masterstrike_combos/menu.inl"
#undef KCD2VR_MENU_CLICK_COUNTERSTAB40
}
    if(page==MenuPage::Kicks26){
#define KCD2VR_MENU_CLICK_KICKS26
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_CLICK_KICKS26
}
    if(page==MenuPage::Dialogue){
#define KCD2VR_MENU_CLICK_DIALOGUE
#include "subsystems/dialogues/menu.inl"
#undef KCD2VR_MENU_CLICK_DIALOGUE
}
    if(page==MenuPage::Crafting67){
#define KCD2VR_MENU_CLICK_CRAFTING67
#include "subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_CLICK_CRAFTING67
    }

    if(page==MenuPage::VrActions){
#define KCD2VR_MENU_CLICK_VRACTIONS
#include "subsystems/interactions/menu.inl"
#undef KCD2VR_MENU_CLICK_VRACTIONS
}

    if(page==MenuPage::GuardSettingsH41&&PtIn(kShieldNavR120,x,y)){GoToPage(MenuPage::ShieldBoxR120,+1);return;} // R120
    if(page==MenuPage::ShieldBoxR120){ // R120: switch (Experiments row VrShieldBlockBoxR120) + sliders, saved
        if(PtIn(kShieldToggleR120,x,y)){s.live_d39.shieldBlockBoxR120=!s.live_d39.shieldBlockBoxR120;ApplyState(s);save();MarkDirty();return;}
        for(unsigned i=0;i<shield_block_r120::Count;++i)if(PtIn(ShieldSliderR120(i),x,y)){shieldSlotR120_.store(i,std::memory_order_release);if(begin_drag(ShieldSliderR120(i),DragControl::ShieldBoxR120))return;}
    }
    if(page==MenuPage::MeleeSensitivityH29||page==MenuPage::FistSensitivityH41||page==MenuPage::GuardSettingsH41||page==MenuPage::ShieldBoxR120||page==MenuPage::DefenceWindowsH41){
        if(PtIn(kFistNavH41,x,y)){GoToPage(MenuPage::FistSensitivityH41,+1);return;}
        if(PtIn(kGuardNavH41,x,y)){GoToPage(MenuPage::GuardSettingsH41,+1);return;}
        if(PtIn(kWeaponNavH41,x,y)){GoToPage(MenuPage::MeleeSensitivityH29,+1);return;}
    }
    if(page==MenuPage::FistSensitivityH41){
        if(begin_drag(kH29Cut,DragControl::H29Punch)||begin_drag(kH29Stab,DragControl::FistTravelH41))return;return;
    }
    if(page==MenuPage::GuardSettingsH41){
        if(begin_drag(kGuardWidthD56,DragControl::GuardWidthH41)||begin_drag(kGuardHeightD56,DragControl::GuardHeightH41)||begin_drag(kGuardDepthD61,DragControl::GuardDepthD61)||begin_drag(kGuardEntryD56,DragControl::GuardEntryH41)||begin_drag(kGuardReleaseD56,DragControl::GuardReleaseH41)||begin_drag(kGuardSettleD56,DragControl::H29Block)||begin_drag(kGuardDistanceD56,DragControl::GuardDistanceD56))return;return;
    }
    if(page==MenuPage::DefenceWindowsH41){
        if(PtIn(kNativeFinisherD61,x,y)){s.native_finisher_test_d61=!s.native_finisher_test_d61;ApplyState(s);MarkDirty();return;}
        if(PtIn(kNeckTrialH41,x,y)){s.chest_trial_h41=s.chest_trial_h41>0?0.f:.6f;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kArcherTrialH41,x,y)){s.archer_trial_h41=!s.archer_trial_h41;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kNativeKillNF,x,y)){s.native_kill_nf=!s.native_kill_nf;ApplyState(s);save();MarkDirty();return;} // I10/NF
        if(begin_drag(kH29Cut,DragControl::PbWindowH41)||begin_drag(kH29Stab,DragControl::MsWindowH41))return;return;
    }
    if(page==MenuPage::StabSensitivityD67){
        if(PtIn(kPageBack,x,y)){GoToPage(MenuPage::MeleeSensitivityH29,-1);return;}
        if(PtIn(kH29Type,x,y)){meleeTypeH29_.store(melee_tuning_ui_c54::next(meleeTypeH29_.load()));MarkDirty();return;}
        if(PtIn(kH29Block,x,y)){s.independent_thrust_d67=!s.independent_thrust_d67;ApplyState(s);save();MarkDirty();return;}
        if(begin_drag(kH29Cut,DragControl::StabSpeedD67)||begin_drag(kH29Stab,DragControl::StabTravelD67))return;
        return;
    }
    if(page==MenuPage::MeleeSensitivityH29){
        if(PtIn(kStabPageD67,x,y)){GoToPage(MenuPage::StabSensitivityD67,+1);return;}
        if(PtIn(kH29Type,x,y)){meleeTypeH29_.store(melee_tuning_ui_c54::next(meleeTypeH29_.load()));MarkDirty();return;}
        if(begin_drag(kH29TravelR8,DragControl::H29TravelR8)||begin_drag(kH29Cut,DragControl::H29Cut)||begin_drag(kH29Stab,DragControl::H29Stab)||begin_drag(kH29Punch,DragControl::BladeAxisH41))return;
        return;
    }
    if(page==MenuPage::Graphics){
        if(PtIn(kGraphicsHead40,x,y)){s.fix_head_sync=!s.fix_head_sync;ApplyState(s);save();MarkDirty();return;}
        if(PtIn(kV4ResPrev,x,y)||PtIn(kV4ResNext,x,y)){
            int selected=graphicsPresetR92_.load();if(selected<1||selected>5)selected=3;
            selected=PtIn(kV4ResPrev,x,y)?(selected==1?5:selected-1):(selected==5?1:selected+1);
            graphicsPresetR92_.store(selected);graphicsPresetStatusR92_.store(0);MarkDirty();return;
        }
        if(PtIn(kV4ResApply,x,y)){
            const int selected=graphicsPresetR92_.load();
            const bool ok=selected>=1&&selected<=5&&callbacks_.apply_graphics_preset&&callbacks_.apply_graphics_preset(callbacks_.context,selected);
            graphicsPresetStatusR92_.store(ok?1:2);MarkDirty();return;
        }
        if(begin_drag(kV4Sharpness,DragControl::EngineClarity)) return;
        if(begin_drag(kV4ViewDistance,DragControl::MergedMesh)) return;
        if(begin_drag(kV4Gamma,DragControl::Gamma)) return;
        if(begin_drag(kV4Brightness,DragControl::Brightness)) return;
        if(begin_drag(kV4Contrast,DragControl::Contrast)) return;
        for(int q=0;q<3;++q) if(PtIn(kGfxShadowQ49[q],x,y)){ s.vr_shadow_quality49=q; ApplyState(s); save(); diagnostic_trace::Write(q==0?"C49-GRAPHICS VrShadowQuality=0 LOW (VR2 cascades only)":q==1?"C49-GRAPHICS VrShadowQuality=1 MEDIUM (cascade fix, 2048 sun shadow map)":"C49-GRAPHICS VrShadowQuality=2 HIGH (cascade fix, 4096 sun shadow map)"); MarkDirty(); return; } // C49: was the C26d VR SHADOWS on/off
        if(PtIn(kGfxEye26,x,y)){ s.eye_head26=!s.eye_head26; ApplyState(s); save(); diagnostic_trace::Write(s.eye_head26?"C26-GRAPHICS VrEyeAdaptHeadSteady=ON":"C26-GRAPHICS VrEyeAdaptHeadSteady=OFF"); MarkDirty(); return; } // C26d
        if(PtIn(kV4FpsToggle,x,y)){ s.fps_cap_enabled=!s.fps_cap_enabled; ApplyState(s); if(callbacks_.apply_fps_cap) callbacks_.apply_fps_cap(callbacks_.context,s.fps_cap_enabled?s.fps_cap_target:0.0f); save(); MarkDirty(); return; }
        if(begin_drag(kV4Fps,DragControl::FpsCap)) return;
        const int t=up_tech_.load();
        const bool stereoD82=stereo_upscaler_d81r2::stereoPolicy(s.stereo_mode);
        if(PtIn(kUpTechPrev,x,y)||PtIn(kUpTechNext,x,y)){
            const int direction=PtIn(kUpTechNext,x,y)?1:-1;int nt=graphics_menu_options::Next(t,direction);
            for(int attempt=0;stereoD82&&!stereo_upscaler_d81r2::canSelect(s.stereo_mode,nt)&&attempt<8;++attempt)
                nt=graphics_menu_options::Next(nt,direction);
            up_tech_.store(nt);up_quality_.store(graphics_menu_options::DefaultQuality(nt));up_preset_.store(0);up_status_.store(0);MarkDirty();return;
        }
        const int qc=UpQualityCount(t);
        if(t>0&&PtIn(kUpQualPrev,x,y)){ up_quality_.store((up_quality_.load()+qc-1)%qc); up_status_.store(0); MarkDirty(); return; }
        if(t>0&&PtIn(kUpQualNext,x,y)){ up_quality_.store((up_quality_.load()+1)%qc); up_status_.store(0); MarkDirty(); return; }
        if(t==2 && PtIn(kUpPresetPrev,x,y)){ up_preset_.store(StepDlssPresetR151(up_preset_.load(),-1)); up_status_.store(0); MarkDirty(); return; }
        if(t==2 && PtIn(kUpPresetNext,x,y)){ up_preset_.store(StepDlssPresetR151(up_preset_.load(),+1)); up_status_.store(0); MarkDirty(); return; }
        if(PtIn(kUpApply,x,y)){
            if(!stereo_upscaler_d81r2::canSelect(s.stereo_mode,t)){up_status_.store(4);MarkDirty();return;}
            const bool ok=graphics_menu_options::Supported(t)&&callbacks_.apply_upscaler && callbacks_.apply_upscaler(callbacks_.context,t,up_quality_.load(),up_preset_.load());
            up_status_.store(ok?3:2);MarkDirty();return;
        }
        return;
    }

    if(page==MenuPage::ViewModes){
        if(PtIn(kV4ModeMono,x,y)){ s.stereo_mode=0; s.cyclopean_mode=0; ApplyState(s); save(); return; }
        if(PtIn(kV4ModeCorrected,x,y)){ selectStereoD81R2(); return; }
        if(begin_drag(kV4PseudoEffect,DragControl::DepthStereoStrength)) return;
    }
}


void VrMenu::PollInput()
{
    if(!game_window_ || !visible_.load(std::memory_order_acquire)) return;
    if(capture_countdown_.load(std::memory_order_acquire)){ mouse_left_prev_=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0; return; }

    if(vr_pointer_valid_.load(std::memory_order_acquire)){mouse_left_prev_=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;return;}
    POINT pt{}; int x=INT_MIN,y=INT_MIN;
    if(GetCursorPos(&pt)&&ScreenToClient(game_window_,&pt)){
        RECT cr{}; int mx=pt.x,my=pt.y;
        const std::uint32_t bw=last_backbuffer_width_.load(std::memory_order_acquire), bh=last_backbuffer_height_.load(std::memory_order_acquire);
        if(GetClientRect(game_window_,&cr)){
            const int cw=cr.right-cr.left,ch=cr.bottom-cr.top;
            if(cw>0&&ch>0&&bw>0&&bh>0){ mx=static_cast<int>((static_cast<long long>(mx)*bw)/cw); my=static_cast<int>((static_cast<long long>(my)*bh)/ch); }
        }
        // V59.7: F7 is an XR layer, not a desktop rectangle. Map the real OS
        // cursor directly across the game client into the 1100x680 menu canvas.
        // This keeps physical-mouse interaction usable even when desktop mirror is off.
        if(GetClientRect(game_window_,&cr)){
            const int cw=cr.right-cr.left,ch=cr.bottom-cr.top;
            if(cw>0&&ch>0){ x=static_cast<int>((static_cast<long long>(pt.x)*kPanelWidth)/cw); y=static_cast<int>((static_cast<long long>(pt.y)*kPanelHeight)/ch); }
        }
        if(x>=0&&y>=0&&x<kPanelWidth&&y<kPanelHeight){ software_cursor_x_=x; software_cursor_y_=y; software_cursor_active_=true; MarkDirty(); }
    }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool left=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;
    if(left&&!mouse_left_prev_&&x>=0&&y>=0&&x<kPanelWidth&&y<kPanelHeight){
        mouse_down_started_ms_=now; preview_transparent_.store(false,std::memory_order_release); HandleClick(x,y);
        char m[160]{}; std::snprintf(m,sizeof(m),"V58-MENU-MOUSE down panel=(%d,%d)",x,y); debug_trace::Write(m);
    } else if(left&&drag_control_.load(std::memory_order_acquire)!=static_cast<int>(DragControl::None)&&x!=INT_MIN){
        if(mouse_down_started_ms_&&now-mouse_down_started_ms_>=500ull&&!preview_transparent_.exchange(true,std::memory_order_acq_rel)){ MarkDirty(); debug_trace::Write("V58-MENU preview alpha -> 20% after 0.5s slider hold"); }
        HandleDrag(x,y);
    }
    if(!left&&mouse_left_prev_){
        const auto released=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
        drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
        CommitDrag(released);
        mouse_down_started_ms_=0;
        if(preview_transparent_.exchange(false,std::memory_order_acq_rel)) MarkDirty();
    }
    mouse_left_prev_=left;

    const bool kup=(GetAsyncKeyState(VK_UP)&0x8000)!=0,kdn=(GetAsyncKeyState(VK_DOWN)&0x8000)!=0,kl=(GetAsyncKeyState(VK_LEFT)&0x8000)!=0,kr=(GetAsyncKeyState(VK_RIGHT)&0x8000)!=0,ke=(GetAsyncKeyState(VK_RETURN)&0x8000)!=0;
    if(kl&&!key_left_prev_) CyclePage(-1);
    if(kr&&!key_right_prev_) CyclePage(+1);
    if(ke&&!key_enter_prev_ && software_cursor_active_) HandleClick(software_cursor_x_,software_cursor_y_);
    key_up_prev_=kup; key_down_prev_=kdn; key_left_prev_=kl; key_right_prev_=kr; key_enter_prev_=ke;
}

void VrMenu::HandleDrag(int x, int y)
{
    (void)y; VrMenuState s{}; ReadState(s); if(s.matrix_running) return;
    const auto dc=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
    auto t=[&](const UiRect& r){ return SliderTFromX(r,x); };
    switch(dc){
    case DragControl::BladeAxisH41:s.blade_axis_cosine_h41=std::round((.60f+t(kH29Punch)*.35f)*100.f)/100.f;break;
    case DragControl::StabSpeedD67:s.thrust_d67[meleeTypeH29_.load()].speed=std::round((.65f+t(kH29Cut)*2.35f)*20.f)/20.f;break;
    case DragControl::StabTravelD67:s.thrust_d67[meleeTypeH29_.load()].travel=std::round((.06f+t(kH29Stab)*.24f)*1000.f)/1000.f;break;
    case DragControl::H29Cut:s.melee_types_c54[meleeTypeH29_.load()].intentH29=std::round((.5f+t(kH29Cut)*1.5f)*20.f)/20.f;break;
    case DragControl::H29Stab:s.melee_types_c54[meleeTypeH29_.load()].sensitivityH29=std::round((.25f+t(kH29Stab)*1.75f)*20.f)/20.f;break;
    case DragControl::H29Punch:s.punch_sensitivity_c54=std::round((.5f+t(kH29Cut)*1.5f)*20.f)/20.f;break;
    case DragControl::FistTravelH41:s.punch_minimum_m_d65=std::round((.045f+t(kH29Stab)*.155f)*1000.f)/1000.f;break;
    case DragControl::GuardDepthD61:s.guard_depth_d61=std::round((.10f+t(kGuardDepthD61)*.70f)*100.f)/100.f;break;
    case DragControl::GuardDistanceD56:s.guard_body_distance_d56=std::round((.35f+t(kGuardDistanceD56)*.70f)*100.f)/100.f;break;
    case DragControl::GuardWidthH41:s.guard_width_h41=.25f+t(kGuardWidthD56)*.50f;break;
    case DragControl::GuardHeightH41:s.guard_height_h41=.10f+t(kGuardHeightD56)*.25f;break;
    case DragControl::GuardEntryH41:s.exp_guard_enter_speed=t(kGuardEntryD56)*5.f;break;
    case DragControl::GuardReleaseH41:s.exp_guard_exit_speed=t(kGuardReleaseD56)*6.f;break;
    case DragControl::HandOffsetR150:{const unsigned i=handOffsetSlotR150_.load(std::memory_order_acquire); // R150
        if(i<12){const float u=HandOffsetTR150(HandOffsetSliderR150(i),x);
            s.hand_offset_r150[i]=(i%6u)<3u?std::round((-20.f+u*40.f)*2.f)*.5f:std::round(-180.f+u*360.f);}
        break;}
    case DragControl::ShieldBoxR120:{const unsigned i=shieldSlotR120_.load(std::memory_order_acquire); // R120
        if(i<shield_block_r120::Count){const auto& spec=shield_block_r120::kSpecs[i];
            s.shield_box_r120[i]=shield_block_r120::clampValue(i,spec.lo+t(ShieldSliderR120(i))*(spec.hi-spec.lo));}
        break;}
    case DragControl::PbWindowH41:s.pb_window_h41=1.f+t(kH29Cut);break;
    case DragControl::MsWindowH41:s.ms_window_h41=1.f+t(kH29Stab);break;
    case DragControl::H29TravelR8:s.melee_types_c54[meleeTypeH29_.load()].travelR8=std::round((.4f+t(kH29TravelR8)*1.6f)*20.f)/20.f;break;
    case DragControl::H29Block:s.block_sensitivity_h29=std::round((.5f+t(kGuardSettleD56))*20.f)/20.f;break;
    case DragControl::FingerMiddleShiftH31:s.finger_middle_shift_mm_h31[fingerSide40_.load()]=std::round(t(kFingerMiddleC54)*40.f)*.5f;break;
    case DragControl::FingerMiddleMedialH31:s.finger_middle_distal_medial_h31[fingerSide40_.load()]=std::round((-20.f+t(kFingerMiddleMedialH31)*40.f)*2.f)*.5f;break;
    case DragControl::FingerMiddleLateralC54:s.finger_middle_lateral_c54[fingerSide40_.load()][fingerDigit40_.load()]=std::round((-20+t(kFingerMiddleC54)*40)*2)/2;break;
    case DragControl::CameraBack: s.camera_back_offset_m=-0.30f+t(kV4HeadBack)*0.60f; s.camera_back_offset_m=std::round(s.camera_back_offset_m*100.0f)/100.0f; break;
    case DragControl::CameraHeight: s.camera_height_offset_m=-0.30f+t(kV4HeadHeight)*0.60f; s.camera_height_offset_m=std::round(s.camera_height_offset_m*100.0f)/100.0f; break;
    case DragControl::Zoom: s.presentation_zoom=0.85f+t(kV4Zoom)*0.35f; s.presentation_zoom=std::round(s.presentation_zoom*100.0f)/100.0f; break;
    case DragControl::GameFov: s.game_fov_target=70.0f+t(kV4GameFov)*60.0f; s.game_fov_target=std::round(s.game_fov_target); s.source_vertical_fov_deg=s.corrected_center_scale_fov_deg=s.game_fov_target; break;
    case DragControl::SourceFov:
        s.game_fov_target=std::round(70.0f+t(kV4SourceFov)*60.0f);
        s.source_vertical_fov_deg=s.corrected_center_scale_fov_deg=s.game_fov_target;
        break;
    case DragControl::CenterX: s.auto_center_x_trim=-0.12f+t(kV4CenterX)*0.24f; s.auto_center_x_trim=std::round(s.auto_center_x_trim*1000.0f)/1000.0f; break;
    case DragControl::CenterY: s.auto_center_y_trim=-0.12f+t(kV4CenterY)*0.24f; s.auto_center_y_trim=std::round(s.auto_center_y_trim*1000.0f)/1000.0f; break;
    case DragControl::ProjectionPitchTrim: s.projection_pitch_trim_deg=-15.0f+t(kV4CenterY)*30.0f; s.projection_pitch_trim_deg=std::round(s.projection_pitch_trim_deg*10.0f)/10.0f; break;
    case DragControl::CorrectedPlanarY: s.presentation_center_y=-0.20f+t(kV4CenterY)*0.40f; s.presentation_center_y=std::round(s.presentation_center_y*1000.0f)/1000.0f; break;
    case DragControl::FlatScreenScale: s.flat_screen_scale=0.45f+t(kV4FlatScale)*2.55f; s.flat_screen_scale=std::round(s.flat_screen_scale*100.0f)/100.0f; break;
    case DragControl::HudDistanceR110:{ // R127 HUD convergence: 0.75..10 m log scale on the first 90 %, the last 10 % = INF (VrHudInfinityR125)
        namespace g=kcd2vr::ui_surface_geometry;
        const float tt=t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHud?kSimpleHudDistR160:kHudDistanceR110);s.live_d39.hudInfinityR125=tt>=0.92f; // R160 UI: + simple VR HUD page row
        if(!s.live_d39.hudInfinityR125){
            const float d=g::kHudConvergenceMinR127*std::pow(g::kHudConvergenceMaxR127/g::kHudConvergenceMinR127,std::min(tt/0.90f,1.0f));
            s.hud_distance_r110=d<2.0f?std::round(d*20.0f)/20.0f:std::round(d*10.0f)/10.0f; // 5 cm steps below 2 m, 10 cm above
        }
        break;}
    case DragControl::CombatCrosshairR112: s.combat_crosshair_scale_r112=std::round((0.25f+t(kCombatCrosshairR112)*1.25f)*20.0f)/20.0f; break; // R112 25..150 %, 5 % steps
    case DragControl::PassiveFullFrameScale: s.passive_full_frame_scale=1.00f+t(kCinematicSizeR107)*1.00f; s.passive_full_frame_scale=std::round(s.passive_full_frame_scale*100.0f)/100.0f; break;
    case DragControl::UnifiedScaleR160: s.vr_ui_unified_scale_r160=std::round((0.50f+t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHud?kSimpleIconR160:kUnifiedDevR160)*1.00f)*20.0f)/20.0f; break; // R160 UI: 50..150 %, 5 % steps
    case DragControl::TutorialYR160: s.vr_ui_tutorial_y_r160=std::round((-540.0f+t(kTutorialYR160)*1080.0f)/5.0f)*5.0f; break; // R160 UI: -540..+540 stage px, 5 px steps
    case DragControl::PauseOverlaySize: s.pause_overlay_size=0.45f+t(kPauseOverlay124)*0.70f; s.pause_overlay_size=std::round(s.pause_overlay_size*100.0f)/100.0f; break;
    case DragControl::CraftUiSizeR145: // R145 Crafting UI size 25..100 %, 5 % steps, live at the station (the movie owner re-applies)
        prompt_policy_d80::minigameScale.store(std::round((0.25f+t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHud?kSimpleCraftR160:kCraftUiSizeR145)*0.75f)*20.0f)/20.0f,std::memory_order_release); // R160 UI: + simple VR HUD page row
        prompt_policy_d80::Refresh();MarkDirty();break;
    case DragControl::MergedMesh: s.merged_mesh_target=30.0f+t(kV4ViewDistance)*270.0f; s.merged_mesh_target=std::round(s.merged_mesh_target); break;
    case DragControl::EngineClarity: s.engine_clarity=t(kV4Sharpness)*0.75f; s.engine_clarity=std::round(s.engine_clarity*100.0f)/100.0f; break;
    case DragControl::Gamma: s.gamma=0.50f+t(kV4Gamma)*1.50f; s.gamma=std::round(s.gamma*100.0f)/100.0f; break;
    case DragControl::Brightness: s.brightness=-0.50f+t(kV4Brightness)*1.00f; s.brightness=std::round(s.brightness*100.0f)/100.0f; break;
    case DragControl::Contrast: s.contrast=0.50f+t(kV4Contrast)*1.50f; s.contrast=std::round(s.contrast*100.0f)/100.0f; break;
    case DragControl::FpsCap: { const UiRect& rr=(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::Home)?kHomeFps:kV4Fps; s.fps_cap_target=60.0f+t(rr)*60.0f; s.fps_cap_target=std::round(s.fps_cap_target); break; }
    case DragControl::DepthStereoStrength: {
        const auto p=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
        const UiRect& rr=(p==MenuPage::Home)?kHomePseudoEffect:kV4PseudoEffect;
        s.depth_stereo_strength=std::clamp(t(rr),0.0f,1.0f);
        s.depth_stereo_strength=std::round(s.depth_stereo_strength*1000.0f)/1000.0f;
        break;
    }
    case DragControl::UiSubtitleScale: s.vr_ui_subtitle_scale=0.45f+t(kV4SubScale)*0.45f; s.vr_ui_subtitle_scale=std::round(s.vr_ui_subtitle_scale*100.0f)/100.0f; break;
    case DragControl::UiSubtitleY: s.vr_ui_subtitle_y=-600.0f+t(kV4SubY)*1200.0f; s.vr_ui_subtitle_y=std::round(s.vr_ui_subtitle_y/5.0f)*5.0f; break;
    case DragControl::UiInteractionMenuScale: s.vr_ui_interaction_menu_scale=0.45f+t(kUiPanelCookScale)*0.65f; s.vr_ui_interaction_menu_scale=std::round(s.vr_ui_interaction_menu_scale*100.0f)/100.0f; break;
    case DragControl::UiInteractionMenuX: s.vr_ui_interaction_menu_x=-400.0f+t(kUiPanelCookX)*800.0f; s.vr_ui_interaction_menu_x=std::round(s.vr_ui_interaction_menu_x/2.0f)*2.0f; break;
    case DragControl::UiInteractionMenuY: s.vr_ui_interaction_menu_y=-300.0f+t(kUiPanelCookY)*600.0f; s.vr_ui_interaction_menu_y=std::round(s.vr_ui_interaction_menu_y/2.0f)*2.0f; break;
    case DragControl::UiItemPanelScale: s.vr_ui_item_panel_scale=0.45f+t(kUiPanelItemScale)*0.80f; s.vr_ui_item_panel_scale=std::round(s.vr_ui_item_panel_scale*100.0f)/100.0f; break;
    case DragControl::UiItemPanelX: s.vr_ui_item_panel_x=-400.0f+t(kUiPanelItemX)*800.0f; s.vr_ui_item_panel_x=std::round(s.vr_ui_item_panel_x/2.0f)*2.0f; break;
    case DragControl::UiItemPanelY: s.vr_ui_item_panel_y=-300.0f+t(kUiPanelItemY)*600.0f; s.vr_ui_item_panel_y=std::round(s.vr_ui_item_panel_y/2.0f)*2.0f; break;
    case DragControl::WeaponGrip59c7f: s.weapon_grip_cm=std::round(-15.f+t(kWeaponGrip59c7f)*30.f); break; // R59.16C7F
    case DragControl::ExpGuardEnterSpeed59c10: s.exp_guard_enter_speed=std::round(t(kExpGuardEnterSpeed)*5.f*10.f)/10.f; break;     // 0..5 m/s, 0.1 steps (0 = off)
    case DragControl::ExpGuardEnterDwell59c10: s.exp_guard_enter_dwell_ms=int(std::round(t(kExpGuardEnterDwell)*600.f/10.f)*10); break; // 0..600 ms
    case DragControl::ExpGuardExitSpeed59c10: s.exp_guard_exit_speed=std::round(t(kExpGuardExitSpeed)*6.f*10.f)/10.f; break;        // 0..6 m/s (0 = position only)
    case DragControl::ExpGuardAfterAttack59c10: s.exp_guard_after_attack_ms=int(std::round(t(kExpGuardAfterAttack)*2000.f/25.f)*25); break; // 0..2000 ms
    case DragControl::ExpStrikeRefJ59c10: s.exp_strike_reference_j=std::round(10.f+t(kExpStrikeRefJ)*190.f); break;                // 10..200 J
    case DragControl::ExpStrikeMaxPower59c10: s.exp_strike_max_power=std::round((0.5f+t(kExpStrikeMaxPower)*1.5f)*20.f)/20.f; break; // 0.50..2.00
    case DragControl::ExpStrikeTwoHanded59c10: s.exp_strike_two_handed=std::round((1.f+t(kExpStrikeTwoHanded)*1.f)*20.f)/20.f; break; // 1.00..2.00
    case DragControl::ExpHeadRaise59c10: s.exp_head_raise_cm=std::round(t(kExpHeadRaise)*60.f); break;                                // 0..60 cm
    case DragControl::ExpNpcEtaPrepare59c10: s.exp_npc_eta_prepare=std::round((0.10f+t(kExpNpcEtaPrepare)*0.50f)*100.f)/100.f; break; // 0.10..0.60 s (R59.16C10R16; was 0.05)
    case DragControl::ExpNpcProximity36: s.exp_npc_proximity_radius36=std::round((0.20f+t(static_cast<MenuPage>(active_page_.load())==MenuPage::Experiments?kCombatProx40:kExpNpcProximity36)*0.40f)*100.f)/100.f; break; // 20..60 cm, 1 cm steps
    case DragControl::ExpAttackReach59c10: s.exp_attack_reach_m=std::round((1.f+t(kExpAttackReach)*4.f)*10.f)/10.f; break;            // 1.0..5.0 m
    case DragControl::FingerOpen40:s.fingers40[fingerSide40_.load()][fingerDigit40_.load()][0]=std::round(t(kFingerOpen40)*100)/100;break;
    case DragControl::FingerClosed40:s.fingers40[fingerSide40_.load()][fingerDigit40_.load()][1]=std::round(t(kFingerClosed40)*100)/100;break;
    case DragControl::FingerProximal55:s.fingers40[fingerSide40_.load()][fingerDigit40_.load()][3]=std::round((-30+t(kFingerProximal55)*60)*2)/2;break;
    case DragControl::FingerLateral40:s.fingers40[fingerSide40_.load()][fingerDigit40_.load()][2]=std::round((-20+t(kFingerLateral40)*40)*2)/2;break;
    case DragControl::ExpDirectBlockLo39: s.exp_direct_block_lo39=std::round((1.f+t(kExpDirectBlockLo39)*2.f)*10.f)/10.f; break;      // C39: 1.0..3.0 x
    case DragControl::ExpDirectBlockHi39: s.exp_direct_block_hi39=std::round((1.f+t(kExpDirectBlockHi39)*2.f)*10.f)/10.f; break;      // C39: 1.0..3.0 x
    case DragControl::NpcDifficulty: s.npc_difficulty_percent=std::round(t(kNpcDifficulty)*100.f); break;
    case DragControl::NpcDefenseSpeed: s.npc_defense_speed=std::round((1.f+t(kNpcDefenseSpeed)*2.f)*20.f)/20.f; break;
    case DragControl::NpcUnarmedSpacing: s.npc_unarmed_spacing_percent=std::round(25.f+t(kNpcUnarmedSpacing)*75.f); break;
    case DragControl::ExpProximity49: s.exp_npc_proximity_radius36=combat_menu49::proximityFromT(t(kCm49Proximity)); break;            // C49: 0.20..0.60 m
    case DragControl::ExpBlockSpeed49: { const auto b=combat_menu49::blockFromSlider(combat_menu49::blockFromT(t(kCm49BlockSpeed))); s.exp_direct_block_timing39=b.bySkill; s.exp_direct_block_lo39=b.lo; s.exp_direct_block_hi39=b.hi; break; } // C49: 1.0 (native)..3.0 x
    case DragControl::ExpWeight49: s.exp_weight_feel=combat_menu49::weightFromT(t(kCm49Weight)); break;                              // C49: = WEAPON FEEL weight slider
    case DragControl::CounterEnergy40: s.exp_counter_stab_energy40=std::round((0.2f+t(kCs40Energy)*1.8f)*20.f)/20.f; break;          // C40: 0.20..2.00, 0.05 steps
    case DragControl::CounterMul40: s.exp_counter_stab_mul40=std::round((1.f+t(kCs40Mul)*1.f)*20.f)/20.f; break;                      // C40: x1.00..x2.00
    case DragControl::CounterWindow40: s.exp_counter_stab_window40=int(std::round((150.f+t(kCs40Window)*850.f)/25.f)*25); break;      // C40: 150..1000 ms
    case DragControl::HangMass44: s.impale_hang_mass44=std::round((20.f+t(kCs44HangKg)*60.f)/5.f)*5.f; break;                          // C44: 20..80 kg, 5 kg steps
    case DragControl::MsStabEnergy47: s.exp_ms_stab_energy47=std::round(t(kMs47Energy)*20.f)/20.f; break;                         // C47: 0..1.00, 0.05 steps
    case DragControl::KillEnergy44: s.impale_kill_energy44=std::round(t(kCs44KillEnergy)*20.f)/20.f; break;                           // C44: 0..1.00, 0.05 steps
    case DragControl::ExpSkill49: s.stat_sim26=combat_menu49::skillFromT(t(kCm49Skill)); break;                                       // C49: OFF/LOW/MEDIUM/HIGH stops
    case DragControl::ExpWeightFeel59c14: s.exp_weight_feel=std::round((0.2f+t(kWfWeight59c14)*1.8f)*20.f)/20.f; break;               // R59.16C10R14: 0.20..2.00
    case DragControl::ExpFatigueRamp59c14: s.exp_fatigue_ramp=std::round(t(kWfFatigue59c14)*20.f)/20.f; break;                        // 0..1
    case DragControl::ExpPoleOneHand59c14: s.exp_pole_one_hand=std::round((0.15f+t(kWfPole59c14)*0.85f)*20.f)/20.f; break;            // 0.15..1.00
    case DragControl::ExpSwingStamina59c14: s.exp_swing_stamina_scale=std::round(t(kWfStamina59c14)*2.f*10.f)/10.f; break;             // 0..2
    case DragControl::ExpWeaponStatEffect59c17: s.exp_weapon_stat_effect=std::round(t(kWfStatEffect59c17)*1.5f*20.f)/20.f; break;     // R59.16C10R17: 0..1.5
    case DragControl::ExpNativeHitTime59c17: {const float h=std::round(t(kCfHitTime59c17)*.60f*100.f)/100.f;s.exp_native_hit_time=h<.10f?0.f:h;break;} // 0 (native) / 0.10..0.60 s
    case DragControl::ExpComboWindow59c17: s.exp_combo_window_ms=int(std::round(t(kCfComboWindow59c17)*3000.f/50.f)*50); break;      // 0..3000 ms
    case DragControl::ExpIntentLiveMs59c17: s.exp_intent_live_ms=int(std::round(t(kCfLiveWait59c17)*120.f/10.f)*10); break;           // 0..120 ms (contacts expire at 160)
    case DragControl::KickAssistH8:s.kick_assist_h8=claude_h8::assist(t(U49(claude_h8::kAssist)));break;
    case DragControl::KickReach26: s.kick_reach_ms26=300+int(std::round(t(kKkReach26)*900.f/10.f)*10); break; // C26: 300..1200 ms
    case DragControl::KickReach52: s.kick_reach52=60+int(std::round(t(kKkReach52)*40.f)); break; // C52: 60..100 % of the leg length
    case DragControl::ExpMsHandoff23: {const float tt=t(kCfMsHandoff23);s.exp_ms_handoff23=tt<.03f?-1.f:std::round((1.5f+tt*3.f)*20.f)/20.f;} break; // C24b
    case DragControl::ExpStrikeMinScale59c14: s.exp_strike_min_scale=std::round(t(kWfMinScale59c14)*20.f)/20.f; break;                 // 0..1
    case DragControl::ExpStrikeMinInterval59c14: s.exp_strike_min_interval_ms=int(std::round(t(kWfMinInterval59c14)*1000.f/25.f)*25); break; // 0..1000 ms
    case DragControl::TargetDwell59c8: s.target_dwell_s=std::round(t(kTargetDwell59c8)*1.5f*20.f)/20.f; break; // R59.16C8: 0..1.5 s in 0.05 steps
    case DragControl::SupportSword59c7f: s.support_sword_cm=std::round(6.f+t(kSupportSword59c7f)*8.f); break;
    case DragControl::SupportPole59c7f: s.support_pole_cm=std::round(6.f+t(kSupportPole59c7f)*54.f); break;
    case DragControl::UiDialogueScale: s.vr_ui_dialogue_scale=0.30f+t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHudElements?kDlgSizeR160:kV4DialogUiScale)*1.20f; s.vr_ui_dialogue_scale=std::round(s.vr_ui_dialogue_scale*100.0f)/100.0f; break;
    case DragControl::UiDialogueY: s.vr_ui_dialogue_y=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHudElements? // R160: the usable range only (-540..+300 px around the old placement)
        kcd2vr::ui_native_canvas::kDialogueYMinR160+t(kDlgYR160)*(kcd2vr::ui_native_canvas::kDialogueYMaxR160-kcd2vr::ui_native_canvas::kDialogueYMinR160):-720.0f+t(kV4DialogUiY)*1440.0f; s.vr_ui_dialogue_y=std::round(s.vr_ui_dialogue_y/5.0f)*5.0f; break;
    case DragControl::UiDialogueInset: s.vr_ui_dialogue_inset_pct=t(kV4DialogUiInset)*80.0f; s.vr_ui_dialogue_inset_pct=std::round(s.vr_ui_dialogue_inset_pct); break;
    case DragControl::DialogueZoomCompensation: { const float tt=t(kV4DialogZoom); s.dialogue_zoom_compensation=0.05f+tt*2.45f; s.dialogue_zoom_compensation=std::round(s.dialogue_zoom_compensation*100.0f)/100.0f; break; }
    case DragControl::DialogueFov: s.dialogue_fov_deg=70.0f+t(kV4DialogFov)*70.0f; s.dialogue_fov_deg=std::round(s.dialogue_fov_deg*2.0f)/2.0f; break;
    case DragControl::UiHintsScale: { const UiRect& rr=(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHudElements)?kUiPromptHintScale:kV4HintScale; s.vr_ui_hints_scale=0.25f+t(rr)*1.25f; s.vr_ui_hints_scale=std::round(s.vr_ui_hints_scale*100.0f)/100.0f; break; }
    case DragControl::UiHintsInset: { const UiRect& rr=(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHudElements)?kUiPromptHintInset:kV4HintInset; s.vr_ui_hints_inset_pct=t(rr)*75.0f; s.vr_ui_hints_inset_pct=std::round(s.vr_ui_hints_inset_pct); break; }
    case DragControl::UiHintsY: { const UiRect& rr=(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHudElements)?kUiPromptHintY:kV4HintY; s.vr_ui_hints_y=-300.0f+t(rr)*600.0f; s.vr_ui_hints_y=std::round(s.vr_ui_hints_y/2.0f)*2.0f; break; }
    case DragControl::UiControlPromptScale: s.vr_ui_control_prompt_scale=0.30f+t(kUiControlPromptScale)*1.20f; s.vr_ui_control_prompt_scale=std::round(s.vr_ui_control_prompt_scale*100.0f)/100.0f; break;
    case DragControl::UiControlPromptX: s.vr_ui_control_prompt_x=-350.0f+t(kUiControlPromptX)*700.0f; s.vr_ui_control_prompt_x=std::round(s.vr_ui_control_prompt_x/2.0f)*2.0f; break;
    case DragControl::UiControlPromptY: s.vr_ui_control_prompt_y=-300.0f+t(kUiControlPromptY)*600.0f; s.vr_ui_control_prompt_y=std::round(s.vr_ui_control_prompt_y/2.0f)*2.0f; break;
    case DragControl::UiCompactness:
        if(s.auto_correct_hud||s.hud_corrected_active)s.auto_hud_icon_scale=std::round((.50f+t(kV4UiCompact)*5.50f)*100.f)/100.f;
        else if(s.hud_native_canvas_fit)s.hud_native_canvas_size=std::round((.70f+t(kV4UiCompact)*.80f)*100.f)/100.f;
        else{s.vr_ui_compactness=.25f+t(kV4UiCompact)*.95f;s.vr_ui_compactness=std::round(s.vr_ui_compactness*100.f)/100.f;}break;
    case DragControl::UiSafeWidth:
        if(!(s.auto_correct_hud||s.hud_corrected_active)&&s.hud_native_canvas_fit)s.hud_native_canvas_spread_x=std::round((.45f+t(kV4UiSafeWidth)*.65f)*100.f)/100.f;
        else{s.vr_ui_safe_width=0.60f+t(kV4UiSafeWidth)*0.80f;s.vr_ui_safe_width=std::round(s.vr_ui_safe_width*100.0f)/100.0f;}break;
    case DragControl::UiSafeHeight:
        if(!(s.auto_correct_hud||s.hud_corrected_active)&&s.hud_native_canvas_fit)s.hud_native_canvas_spread_y=std::round((.45f+t(kV4UiSafeHeight)*.65f)*100.f)/100.f;
        else{s.vr_ui_safe_height=0.60f+t(kV4UiSafeHeight)*0.80f;s.vr_ui_safe_height=std::round(s.vr_ui_safe_height*100.0f)/100.0f;}break;
    case DragControl::UiTopBoundary:
        s.vr_ui_top_boundary=.55f+t((s.auto_correct_hud||s.hud_corrected_active)?kAutoUpper49:kV4UiTopBoundary)*2.45f; s.vr_ui_top_boundary=std::round(s.vr_ui_top_boundary*100.0f)/100.0f; break;
    case DragControl::UiBottomBoundary:
        s.vr_ui_bottom_boundary=.55f+t((s.auto_correct_hud||s.hud_corrected_active)?kAutoLower49:kV4UiBottomBoundary)*2.45f; s.vr_ui_bottom_boundary=std::round(s.vr_ui_bottom_boundary*100.0f)/100.0f; break;
    case DragControl::UiUniformCenterX:
        s.vr_ui_center_offset_x=-250.0f+t(kV4UiUniformCenterX)*500.0f; s.vr_ui_center_offset_x=std::round(s.vr_ui_center_offset_x/2.0f)*2.0f; break;
    case DragControl::UiHudOffsetX: s.vr_ui_center_offset_x=-250.0f+t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHud&&!(s.auto_correct_hud||s.hud_corrected_active)&&!s.hud_native_canvas_fit?kV4ManualOffsetX30c1:kV451HudOffsetX)*500.0f; s.vr_ui_center_offset_x=std::round(s.vr_ui_center_offset_x/2.0f)*2.0f; break;
    case DragControl::UiHudOffsetY: s.vr_ui_center_offset_y=-250.0f+t(static_cast<MenuPage>(active_page_.load(std::memory_order_acquire))==MenuPage::VrHud&&!(s.auto_correct_hud||s.hud_corrected_active)&&!s.hud_native_canvas_fit?kV4ManualOffsetY30c1:kV451HudOffsetY)*500.0f; s.vr_ui_center_offset_y=std::round(s.vr_ui_center_offset_y/2.0f)*2.0f; break;
    case DragControl::UiCompassScale: s.vr_ui_compass_scale=0.15f+t(kV4ElemCompassScale)*1.35f; s.vr_ui_compass_scale=std::round(s.vr_ui_compass_scale*100.0f)/100.0f; break;
    case DragControl::UiCompassY: s.vr_ui_compass_y=-300.0f+t(kV4ElemCompassY)*600.0f; s.vr_ui_compass_y=std::round(s.vr_ui_compass_y*10.0f)/10.0f; break;
    case DragControl::UiStatsScale: s.vr_ui_stats_scale=0.15f+t(kV4ElemStatsScale)*1.35f; s.vr_ui_stats_scale=std::round(s.vr_ui_stats_scale*100.0f)/100.0f; break;
    case DragControl::UiStatsX: s.vr_ui_stats_x=-700.0f+t(kV4ElemStatsX)*1400.0f; s.vr_ui_stats_x=std::round(s.vr_ui_stats_x/5.0f)*5.0f; break;
    case DragControl::UiStatsY: s.vr_ui_stats_y=-20.0f+t(kV4ElemStatsY)*120.0f; s.vr_ui_stats_y=std::round(s.vr_ui_stats_y*10.0f)/10.0f; break;
    case DragControl::UiLowerScale: s.vr_ui_lower_scale=0.15f+t(kV4ElemLowerScale)*1.85f; s.vr_ui_lower_scale=std::round(s.vr_ui_lower_scale*100.0f)/100.0f; break;
    case DragControl::UiLowerY: s.vr_ui_lower_y=-250.0f+t(kV4ElemLowerY)*500.0f; s.vr_ui_lower_y=std::round(s.vr_ui_lower_y); break;
    case DragControl::UiLowerSpread: s.vr_ui_lower_spread=-200.0f+t(kV4ElemLowerSpread)*400.0f; s.vr_ui_lower_spread=std::round(s.vr_ui_lower_spread); break;
    // R109 MISC 1: enemy side
    case DragControl::M1DifficultyR109: s.npc_difficulty_percent=std::round(t(kM1DifficultyR109)*100.f); break; // 0..100 %
    case DragControl::M1DefenseSpeedR109: s.npc_defense_speed=std::round((1.f+t(kM1DefenseSpeedR109)*2.f)*20.f)/20.f; break; // x1.00..3.00
    case DragControl::M1BlockAnimR109: s.npc_block_anim_speed109=std::round((1.f+t(kM1BlockAnimR109)*2.f)*20.f)/20.f; break; // x1.00..3.00
    case DragControl::M1ParryAnimR109: s.npc_parry_anim_speed109=std::round((1.f+t(kM1ParryAnimR109)*2.f)*20.f)/20.f; break;
    case DragControl::M1DodgeAnimR109: s.npc_dodge_anim_speed109=std::round((1.f+t(kM1DodgeAnimR109)*2.f)*20.f)/20.f; break;
    case DragControl::M1SpacingR109: s.npc_unarmed_spacing_percent=std::round(25.f+t(kM1SpacingR109)*75.f); break; // 25..100 %
    case DragControl::M1BlockSpeed18R109: {const float tt=t(kM1BlockSpeed18R109);s.exp_npc_block_speed18=tt<.03f?0.f:std::round((1.f+tt*2.f)*20.f)/20.f;break;} // 0 = legacy ETA values, else x1.00..3.00
    case DragControl::M1DirectLoR109: s.exp_direct_block_lo39=std::round((1.f+t(kM1DirectLoR109)*2.f)*10.f)/10.f; break; // x1.0..3.0
    case DragControl::M1DirectHiR109: s.exp_direct_block_hi39=std::round((1.f+t(kM1DirectHiR109)*2.f)*10.f)/10.f; break;
    case DragControl::M1EtaPrepareR109: s.exp_npc_eta_prepare=std::round((0.10f+t(kM1EtaPrepareR109)*0.50f)*100.f)/100.f; break; // 0.10..0.60 s
    case DragControl::M1EtaStrikeR109: s.exp_npc_eta_strike=std::round((0.10f+t(kM1EtaStrikeR109)*0.50f)*100.f)/100.f; break;
    // R109 MISC 2: counter stab / live intent
    case DragControl::M2CounterEnergyR109: s.exp_counter_stab_energy40=std::round((0.2f+t(kM2CounterEnergyR109)*1.8f)*20.f)/20.f; break; // 0.20..2.00
    case DragControl::M2CounterMulR109: s.exp_counter_stab_mul40=std::round((1.f+t(kM2CounterMulR109)*1.f)*20.f)/20.f; break; // x1.00..2.00
    case DragControl::M2CounterWindowR109: s.exp_counter_stab_window40=int(std::round((150.f+t(kM2CounterWindowR109)*850.f)/25.f)*25); break; // 150..1000 ms
    case DragControl::M2IntentWaitR109: s.exp_intent_live_ms=int(std::round(t(kM2IntentWaitR109)*120.f/10.f)*10); break; // 0..120 ms
    // R109 MISC 3: native window timing, Henry's windows, strike power rule, impale
    case DragControl::M3WinComboR109: s.win_combo_s109=std::round((.5f+t(kM3WinComboR109)*2.5f)*10.f)/10.f; break; // 0.5..3.0 s
    case DragControl::M3WinRiposteR109: s.win_riposte_s109=std::round((.1f+t(kM3WinRiposteR109)*1.4f)*20.f)/20.f; break; // 0.10..1.50 s
    case DragControl::M3WinStandardR109: s.win_standard_s109=std::round((.1f+t(kM3WinStandardR109)*1.9f)*20.f)/20.f; break; // 0.10..2.00 s
    case DragControl::M3WinAiR109: s.win_ai_mod109=std::round(t(kM3WinAiR109)*20.f)/20.f; break; // 0..1
    case DragControl::M3WinPlR109: s.win_pl_mod109=std::round(t(kM3WinPlR109)*20.f)/20.f; break; // 0..1
    case DragControl::M3WinBlockR109: s.win_block_speed_mod109=std::round((.8f+t(kM3WinBlockR109)*1.6f)*20.f)/20.f; break; // 0.80..2.40
    case DragControl::M3PbScaleR109: s.pb_window_h41=std::round((1.f+t(kM3PbScaleR109))*20.f)/20.f; break; // x1.00..2.00 (same value as the DEFENCE WINDOWS page)
    case DragControl::M3MsScaleR109: s.ms_window_h41=std::round((1.f+t(kM3MsScaleR109))*20.f)/20.f; break;
    case DragControl::M3RiposteMsR109: s.riposte_window_ms109=int(std::round((100.f+t(kM3RiposteMsR109)*2900.f)/50.f)*50); break; // 100..3000 ms
    case DragControl::M3MinPowerR109: s.strike_min_power109=std::round((.05f+t(kM3MinPowerR109)*.95f)*20.f)/20.f; break; // 0.05..1.00
    case DragControl::M3RepeatMsR109: s.strike_repeat_ms109=int(std::round((200.f+t(kM3RepeatMsR109)*9800.f)/100.f)*100); break; // 200..10000 ms
    case DragControl::M3HangMassR109: s.impale_hang_mass44=std::round((20.f+t(kM3HangMassR109)*60.f)/5.f)*5.f; break; // 20..80 kg
    case DragControl::M3KillEnergyR109: s.impale_kill_energy44=std::round(t(kM3KillEnergyR109)*20.f)/20.f; break; // 0..1
    default:return;
    }
    ApplyState(s);

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(dc==DragControl::UiSubtitleScale || dc==DragControl::UiSubtitleY || dc==DragControl::UiDialogueScale || dc==DragControl::UiDialogueY || dc==DragControl::UiDialogueInset || dc==DragControl::UiHintsScale || dc==DragControl::UiHintsInset || dc==DragControl::UiHintsY || dc==DragControl::UiControlPromptScale || dc==DragControl::UiControlPromptX || dc==DragControl::UiControlPromptY || dc==DragControl::UiCompactness || dc==DragControl::UiSafeWidth || dc==DragControl::UiSafeHeight || dc==DragControl::UiTopBoundary || dc==DragControl::UiBottomBoundary || dc==DragControl::UiUniformCenterX || dc==DragControl::UiHudOffsetX || dc==DragControl::UiHudOffsetY || dc==DragControl::UiInteractionMenuScale || dc==DragControl::UiInteractionMenuX || dc==DragControl::UiInteractionMenuY || dc==DragControl::UiItemPanelScale || dc==DragControl::UiItemPanelX || dc==DragControl::UiItemPanelY){
        static std::uint64_t last=0; if(now-last>=80ull){ last=now; if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); }
    } else if(dc==DragControl::UiCompassScale || dc==DragControl::UiCompassY || dc==DragControl::UiStatsScale || dc==DragControl::UiStatsX || dc==DragControl::UiStatsY || dc==DragControl::UiLowerScale || dc==DragControl::UiLowerY || dc==DragControl::UiLowerSpread || dc==DragControl::CombatCrosshairR112 || dc==DragControl::UnifiedScaleR160 || dc==DragControl::TutorialYR160){ // R160 UI: live while dragging
        static std::uint64_t last_elements=0; if(now-last_elements>=80ull){ last_elements=now; if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); }
    }
    if(dc==DragControl::Gamma || dc==DragControl::Brightness || dc==DragControl::Contrast){
        static std::uint64_t last=0; if(now-last>=100ull){ last=now; if(callbacks_.apply_graphics_color) callbacks_.apply_graphics_color(callbacks_.context,s.gamma,s.brightness,s.contrast); }
    }
}

void VrMenu::CommitDrag(DragControl released)
{
    VrMenuState s{}; ReadState(s);
    auto save=[&](){ if(callbacks_.save_settings) (void)callbacks_.save_settings(callbacks_.context); };
    if(released==DragControl::StabSpeedD67||released==DragControl::StabTravelD67){ApplyState(s);save();return;}
    if(released==DragControl::HudDistanceR110){ApplyState(s);save();return;} // R125: distance / INF saved on release
    if(released==DragControl::CraftUiSizeR145){prompt_policy_d80::Refresh();save();return;} // R145 VrMinigamePromptScale saved on release
    if(released>=DragControl::M1DifficultyR109&&released<=DragControl::M3KillEnergyR109){ApplyState(s);save();return;} // R109 MISC sliders: apply + save on release
    if(released==DragControl::NpcDifficulty||released==DragControl::NpcDefenseSpeed||released==DragControl::NpcUnarmedSpacing){ApplyState(s);save();return;}
    if(released==DragControl::BladeAxisH41||released==DragControl::FistTravelH41||released==DragControl::GuardDistanceD56||released==DragControl::GuardDepthD61||released==DragControl::GuardWidthH41||released==DragControl::GuardHeightH41||released==DragControl::GuardEntryH41||released==DragControl::GuardReleaseH41||released==DragControl::ShieldBoxR120||released==DragControl::HandOffsetR150||released==DragControl::PbWindowH41||released==DragControl::MsWindowH41||released==DragControl::H29TravelR8||released==DragControl::FingerMiddleShiftH31||released==DragControl::FingerMiddleMedialH31||released==DragControl::H29Cut||released==DragControl::H29Stab||released==DragControl::H29Punch||released==DragControl::H29Block||released==DragControl::FingerMiddleLateralC54){ApplyState(s);save();return;}
    if(released==DragControl::GameFov){ save(); return; } // shared setting persists from either slider
    if(released==DragControl::CameraBack || released==DragControl::CameraHeight || released==DragControl::Zoom || released==DragControl::SourceFov || released==DragControl::CenterY || released==DragControl::CorrectedPlanarY || released==DragControl::ProjectionPitchTrim || released==DragControl::FlatScreenScale || released==DragControl::DepthStereoStrength){ save(); return; }
    if(released==DragControl::MergedMesh){ if(callbacks_.apply_merged_mesh) callbacks_.apply_merged_mesh(callbacks_.context,s.merged_mesh_target); save(); return; }
    if(released==DragControl::FpsCap){ if(s.fps_cap_enabled && callbacks_.apply_fps_cap) callbacks_.apply_fps_cap(callbacks_.context,s.fps_cap_target); save(); return; }
    if(released==DragControl::Gamma || released==DragControl::Brightness || released==DragControl::Contrast){ if(callbacks_.apply_graphics_color) callbacks_.apply_graphics_color(callbacks_.context,s.gamma,s.brightness,s.contrast); save(); return; }
    if(released==DragControl::DialogueZoomCompensation || released==DragControl::DialogueFov){ save(); return; }
    if(released==DragControl::KickAssistH8){save();return;}
    if(released==DragControl::WeaponGrip59c7f || released==DragControl::SupportSword59c7f || released==DragControl::SupportPole59c7f || released==DragControl::TargetDwell59c8){ save(); return; } // R59.16C7F/C8
    if(released==DragControl::FingerOpen40||released==DragControl::FingerClosed40||released==DragControl::FingerLateral40||released==DragControl::FingerProximal55){ApplyState(s);save();return;}
    if(released==DragControl::ExpGuardEnterSpeed59c10||released==DragControl::ExpGuardEnterDwell59c10||released==DragControl::ExpGuardExitSpeed59c10||released==DragControl::ExpGuardAfterAttack59c10||released==DragControl::ExpStrikeRefJ59c10||released==DragControl::ExpStrikeMaxPower59c10||released==DragControl::ExpStrikeTwoHanded59c10||released==DragControl::ExpHeadRaise59c10||released==DragControl::ExpNpcEtaPrepare59c10||released==DragControl::ExpNpcProximity36||released==DragControl::ExpAttackReach59c10||
       released==DragControl::ExpWeightFeel59c14||released==DragControl::ExpFatigueRamp59c14||released==DragControl::ExpPoleOneHand59c14||released==DragControl::ExpSwingStamina59c14||released==DragControl::ExpStrikeMinScale59c14||released==DragControl::ExpStrikeMinInterval59c14||
       released==DragControl::ExpWeaponStatEffect59c17||released==DragControl::ExpNativeHitTime59c17||released==DragControl::ExpComboWindow59c17||released==DragControl::ExpIntentLiveMs59c17||
       released==DragControl::ExpMsHandoff23||released==DragControl::KickReach26||released==DragControl::KickReach52||
       released==DragControl::ExpDirectBlockLo39||released==DragControl::ExpDirectBlockHi39){ ApplyState(s); save(); return; } // R59.16C10d / R14 / R17 / C10R18 / C39
    if(released==DragControl::CounterEnergy40||released==DragControl::CounterMul40||released==DragControl::CounterWindow40){ // C40
        ApplyState(s); save(); char m[160]{}; std::snprintf(m,sizeof(m),"C40-COUNTER-STAB VrDirectCounterStabEnergy=%.2f VrDirectCounterStabStrength=%.2f VrDirectCounterStabWindowMs=%d",
            double(s.exp_counter_stab_energy40),double(s.exp_counter_stab_mul40),s.exp_counter_stab_window40); diagnostic_trace::Write(m); return; }
    if(released==DragControl::MsStabEnergy47){ ApplyState(s); save(); char m[96]{}; std::snprintf(m,sizeof(m),"C47-MS-GATE VrMasterStrikeStabEnergy=%.2f",double(s.exp_ms_stab_energy47)); diagnostic_trace::Write(m); return; } // C47
    if(released==DragControl::HangMass44||released==DragControl::KillEnergy44){ // C44
        ApplyState(s); save(); char m[160]{}; std::snprintf(m,sizeof(m),"C44-IMPALE VrImpaleHangMassKg=%.0f VrImpaleKillEnergy=%.2f",double(s.impale_hang_mass44),double(s.impale_kill_energy44)); diagnostic_trace::Write(m); return; }
    if(released==DragControl::ExpProximity49||released==DragControl::ExpBlockSpeed49||released==DragControl::ExpWeight49||released==DragControl::ExpSkill49){ // C49 first fight page: one line per release
        ApplyState(s); save(); char m[200]{};
        std::snprintf(m,sizeof(m),"C49-EXPERIMENTS VrNpcProximityRadius36=%.2f VrDirectNpcBlockBySkill=%d Low=%.2f High=%.2f VrWeaponWeightFeel=%.2f VrSimulateHenry=%d",
            double(s.exp_npc_proximity_radius36),s.exp_direct_block_timing39?1:0,double(s.exp_direct_block_lo39),double(s.exp_direct_block_hi39),double(s.exp_weight_feel),s.stat_sim26);
        diagnostic_trace::Write(m); return; }
    if(released==DragControl::UiInteractionMenuScale || released==DragControl::UiInteractionMenuX || released==DragControl::UiInteractionMenuY){ save(); return; }
    if(released==DragControl::UiSubtitleScale || released==DragControl::UiSubtitleY || released==DragControl::UiDialogueScale || released==DragControl::UiDialogueY || released==DragControl::UiDialogueInset || released==DragControl::UiHintsScale || released==DragControl::UiHintsInset || released==DragControl::UiHintsY || released==DragControl::UiControlPromptScale || released==DragControl::UiControlPromptX || released==DragControl::UiControlPromptY || released==DragControl::UiCompactness || released==DragControl::UiSafeWidth || released==DragControl::UiSafeHeight || released==DragControl::UiHudOffsetX || released==DragControl::UiHudOffsetY){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); save(); return; }
    if(released==DragControl::UiCompassScale || released==DragControl::UiCompassY || released==DragControl::UiStatsScale || released==DragControl::UiStatsX || released==DragControl::UiStatsY || released==DragControl::UiLowerScale || released==DragControl::UiLowerY || released==DragControl::UiLowerSpread || released==DragControl::CombatCrosshairR112 || released==DragControl::UnifiedScaleR160 || released==DragControl::TutorialYR160){ if(callbacks_.apply_vr_ui_layout) callbacks_.apply_vr_ui_layout(callbacks_.context,&s); save(); return; }
}

void VrMenu::DrawPanel(HDC dc, const VrMenuState& s)
{
    UpdatePageAnimation();
    RECT full{0,0,kPanelWidth,kPanelHeight}; HBRUSH bg=CreateSolidBrush(kBg); FillRect(dc,&full,bg); DeleteObject(bg);
    // V66.88 calibration-only presentation: IsVisible() is false, but IsLayerVisible()
    // remains true through capture_countdown_. Render no manuscript/menu chrome here.
    if(capture_countdown_.load(std::memory_order_acquire)){
        const int sec=capture_countdown_seconds_.load(std::memory_order_acquire);
        Fill(dc,UiRect{0,0,kPanelWidth,kPanelHeight},RGB(0,0,0));
        Text(dc,font_hero_?font_hero_:font_title_,sec==-1?L"VRIK ON":sec==-2?L"CALIBRATION NOT ACTIVE":L"MAKE A T-POSE",70,105,960,90,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        wchar_t c[96]{}; if(sec>0) std::swprintf(c,96,L"%d",sec); else if(sec==0) std::swprintf(c,96,L"NOW - HOLD STILL");
        Text(dc,font_hero_?font_hero_:font_title_,c,70,215,960,145,kWarn,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_title_,sec==-1?L"You can lower your arms":sec==-2?L"Return to gameplay and select VRIK to retry":L"Stand upright. Look straight ahead.",100,365,900,54,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_normal_,sec<0?L"Saved finger poses are unchanged.":L"Arms straight out to the sides at shoulder height. Palms down.",80,435,940,54,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_normal_,sec<0?L"":L"Hold both controllers still through NOW (2 seconds).",100,505,900,54,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        return;
    }
    if(IsDiagnosticOnlyH24()){
        block_indicator_h24::draw(dc,font_title_,kPanelWidth,kPanelHeight,block_indicator_shield_r120_.load(std::memory_order_acquire));
        return;
    }
    if(LoadBookSkinBitmap()) DrawBookSkin(dc);
    else {
        Fill(dc,UiRect{8,8,kPanelWidth-8,kPanelHeight-8},kPanel);
        Line(dc,14,14,kPanelWidth-14,14,kRule,2); Line(dc,14,kPanelHeight-14,kPanelWidth-14,kPanelHeight-14,kRule,2);
        Line(dc,14,14,14,kPanelHeight-14,kRule,2); Line(dc,kPanelWidth-14,14,kPanelWidth-14,kPanelHeight-14,kRule,2);
        Fill(dc,UiRect{538,30,562,kPanelHeight-30},kPanel2); Line(dc,550,34,550,kPanelHeight-34,kRule,2);
    }

    // V62.7: the supplied manuscript art owns the left page. Avoid duplicate
    // title/status text over the illuminated title. Fallback text is used only
    // if the external bitmap is missing.
    if(!LoadBookSkinBitmap()) {
        Text(dc,font_hero_,L"KCD2VR",92,180,362,58,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        Text(dc,font_small_,L"VR CONTROL MENU",92,242,362,26,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }

    const auto page=static_cast<MenuPage>(active_page_.load(std::memory_order_acquire));
    if(page==MenuPage::Home){
        DrawButton(dc,font_small_,kHomeModeMono,L"MONO",(s.stereo_mode!=3&&s.stereo_mode!=4)?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeModeCorrected,L"STEREO",s.stereo_mode==3?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeModeAlternateEyeR142,L"ALT EYE (DLSS)",s.stereo_mode==4?kAccent:kPanel2);
        if(s.stereo_mode==4){ // R154: what carries the eye that was not rendered this frame
            const int wm=std::clamp(s.aer_warp_mode_r154,0,2);
            DrawButton(dc,font_tiny_?font_tiny_:font_small_,kHomeAerWarpR154,wm==2?L"WARP MV":wm==1?L"WARP 6DOF":L"WARP ROT",wm>0?kAccent:kPanel2);
        }
        if(s.stereo_mode==4)DrawButton(dc,font_tiny_?font_tiny_:font_small_,kHomeAerDumpR149,
            native_dlss_stereo_d82::AerFrameDumpBusy()?L"DUMPING":L"DUMP",RGB(176,148,98));
        if(developer_mode::Enabled())DrawButton(dc,font_small_,kVrikExperimentsD61,L"VRIK EXPERIMENTS  >",kPanel2);
        if(developer_mode::Enabled())DrawButton(dc,font_small_,kH29TuneNav,L"WEAPON / FIST SENSITIVITY  >",kPanel2);
        wchar_t fpsHome[96]{}; std::swprintf(fpsHome,96,L"FPS LIMIT: %s  %.0f",s.fps_cap_enabled?L"ON":L"OFF",static_cast<double>(s.fps_cap_target));
        DrawButton(dc,font_small_,kHomeFps,fpsHome,s.fps_cap_enabled?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeHandOffsetsR150,L"HAND OFFSETS  >",kPanel2); // R150
        
    }
    int slide=0;
    if(page_animating_.load(std::memory_order_acquire)){
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); const auto begin=page_anim_start_ms_.load(std::memory_order_acquire);
        const float q=begin?Clamp01(static_cast<float>(now-begin)/220.0f):1.0f;
        slide=static_cast<int>(page_slide_dir_.load(std::memory_order_acquire)*(1.0f-SmoothStep01(q))*430.0f);
    }
    const int saved=SaveDC(dc); IntersectClipRect(dc,570,45,1070,672); /* R59.16C10R15: was 630 - hid IMMERSION > EXPERIMENTS (physics hands + blade), target dwell, CONTROLS roomscale/gestures, GRAPHICS preset apply */ SetViewportOrgEx(dc,slide,0,nullptr);

    auto pageTitle=[&](const wchar_t* title,const wchar_t* subtitle){ Text(dc,font_title_,title,600,32,440,34,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE); if(subtitle) Text(dc,font_small_,subtitle,600,62,440,24,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE); };
    auto vrNav=[&](){
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_small_,kVrNavView,L"VIEW",page==MenuPage::VrView?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrNavHud,L"UI",(page==MenuPage::VrHud||page==MenuPage::VrHudElementControls||page==MenuPage::VrHudElements)?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrNavActions,L"CONTROLS",page==MenuPage::VrControls?kAccent:kPanel2);
    };
    wchar_t v[128]{};

    if(page==MenuPage::Home){
        pageTitle(L"KCD2VR",L"Choose a section");
        DrawButton(dc,font_title_,kHomeVrConfig,L"VR CONFIG",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeGraphics,L"GRAPHICS",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeUi,L"UI SETTINGS",RGB(188,158,104));
        DrawButton(dc,font_title_,kHomeDialogue,L"IMMERSION AND CONTROLS",RGB(188,158,104));
        DrawButton(dc,font_small_,kHomeActions,L"UI / CONTROLLER CONTROLS",RGB(176,148,98));
        if(developer_mode::Enabled())DrawButton(dc,font_small_,kHomeFeaturesH5,L"EXPERIMENTS / LIVE SETTINGS",RGB(176,148,98));
        if(developer_mode::Enabled())DrawButton(dc,font_small_,kHomeExperiments,L"COMBAT EXPERIMENTS",RGB(176,148,98));
        DrawButton(dc,font_small_,kHomeVrikNative,L"NATIVE TOUCH",s.vrik_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kHomeVrikTracked,L"VRIK",s.vrik_mode!=0?kAccent:kPanel2);
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kHomeVrikRecalibrate,
            s.direct_controller_hands_r135?L"HEIGHT RESET":L"RECALIBRATE",RGB(176,148,98));
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kHomeVrikRestore43,L"RESTORE CAL",RGB(176,148,98)); // C43: stored F8 profile, no T-pose
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kHomeDirectHandsR135,
            s.direct_controller_hands_r135?L"HANDS 1:1 ON":L"HANDS LEGACY",s.direct_controller_hands_r135?kAccent:kPanel2);
        DrawCheck(dc,font_small_,kHomeBest,s.balanced_graphics,L"Best VR settings");
        DrawCheck(dc,font_tiny_?font_tiny_:font_small_,kHomeAutoRestore43,s.calibration_auto_restore43,L"Auto restore calibration"); // C43
        if(s.vrik_calibration_ui_state!=0){
            const wchar_t* cal=L"MAKE A T-POSE";
            wchar_t counter[32]{};
            if(s.vrik_calibration_ui_state>=1 && s.vrik_calibration_ui_state<=3) std::swprintf(counter,32,L"%d",s.vrik_calibration_ui_state);
            else if(s.vrik_calibration_ui_state==4) std::swprintf(counter,32,L"NOW");
            else if(s.vrik_calibration_ui_state==5) std::swprintf(counter,32,L"COMPLETE");
            else std::swprintf(counter,32,L"NOT SAVED");
            Text(dc,font_normal_,cal,130,462,370,34,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            Text(dc,font_title_,counter,130,498,370,58,kAccent,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }
        DrawButton(dc,font_small_,kHomeSave,L"SAVE SETTINGS",RGB(176,148,98));
        Text(dc,font_tiny_?font_tiny_:font_small_,s.vrik_calibration_persistence_state<0?
            L"CALIBRATION NOT SAVED: check write permission for KCD2VR.ini. Current in-memory data remains active.":s.vrik_finger_capture_active?
            L"Native finger capture: relax, press BOTH triggers, wait for fists, then release.":
            L"Shift+F8: arm calibration. Saved arm reach and finger references reload automatically.",620,580,418,28,kMuted,DT_CENTER|DT_WORDBREAK);
    } else if(page==MenuPage::VrView){
        pageTitle(L"VR CONFIG",L"Head position / FOV / framing / F6 Theater screen"); vrNav();
        std::swprintf(v,128,L"%+.2f m",static_cast<double>(s.camera_height_offset_m)); DrawSlider(dc,font_small_,kV4HeadHeight,L"Head up / down",(s.camera_height_offset_m+0.30f)/0.60f,v);
        std::swprintf(v,128,L"%+.2f m",static_cast<double>(s.camera_back_offset_m)); DrawSlider(dc,font_small_,kV4HeadBack,L"Head forward / backward",(s.camera_back_offset_m+0.30f)/0.60f,v);
        const bool projectiveRemap=s.cyclopean_mode==1;
        std::swprintf(v,128,L"%.0f deg",static_cast<double>(s.game_fov_target)); DrawSlider(dc,font_small_,kV4GameFov,L"Game FOV",(s.game_fov_target-70.0f)/60.0f,v); DrawButton(dc,font_tiny_?font_tiny_:font_small_,kV4GameFovApply,L"APPLY",kAccent);
        std::swprintf(v,128,L"%.1f deg",static_cast<double>(s.source_vertical_fov_deg));
        DrawSlider(dc,font_small_,kV4SourceFov,L"VR FOV (linked)",(s.source_vertical_fov_deg-70.0f)/60.0f,v);
        if(projectiveRemap){
            DrawSlider(dc,font_small_,kV4Zoom,L"Corrected scale — calculated",(1.0f-0.85f)/0.35f,L"auto");
        } else {
            std::swprintf(v,128,L"%.2fx",static_cast<double>(s.presentation_zoom)); DrawSlider(dc,font_small_,kV4Zoom,L"VR zoom / framing",(s.presentation_zoom-0.85f)/0.35f,v);
        }
        std::swprintf(v,128,L"%+.1f deg",static_cast<double>(s.projection_pitch_trim_deg)); DrawSlider(dc,font_small_,kV4CenterY,L"Vertical rotational offset",(s.projection_pitch_trim_deg+15.0f)/30.0f,v);
        Text(dc,font_tiny_?font_tiny_:font_small_,projectiveRemap
            ? L"CORRECT FOV ACTIVE — Game FOV and VR FOV share one value. Projection mapping uses the live per-eye OpenXR frusta."
            : L"Game FOV and VR FOV stay linked. CORRECT FOV recalculates the OpenXR projection mapping for this shared value.",620,398,418,48,projectiveRemap?kAccent:kMuted,DT_CENTER|DT_WORDBREAK);
        std::swprintf(v,128,L"%.2fx",static_cast<double>(s.flat_screen_scale)); DrawSlider(dc,font_small_,kV4FlatScale,L"Inventory / Shop / Theater size",(s.flat_screen_scale-0.45f)/2.55f,v);
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kV4RecalculateProjection,L"CORRECT FOV",projectiveRemap?kAccent:kPanel2);
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kV4CenterProjection,L"CENTER PROJECTION",kPanel2);
        DrawButton(dc,font_small_,kV4Recenter,L"RECENTER",RGB(176,148,98)); DrawButton(dc,font_small_,kV4ResetView,L"RESET VIEW",RGB(176,148,98));
    } else if(page==MenuPage::VrHud){
#define KCD2VR_MENU_DRAW_VRHUD
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_DRAW_VRHUD
} else if(page==MenuPage::VrHudElementControls){
#define KCD2VR_MENU_DRAW_VRHUDELEMENTCONTROLS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_DRAW_VRHUDELEMENTCONTROLS
} else if(page==MenuPage::VrHudElements){
#define KCD2VR_MENU_DRAW_VRHUDELEMENTS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_DRAW_VRHUDELEMENTS
} else if(page==MenuPage::VrControls){
#define KCD2VR_MENU_DRAW_VRCONTROLS
#include "subsystems/hud_ui_controls/menu.inl"
#undef KCD2VR_MENU_DRAW_VRCONTROLS
} else if(page==MenuPage::Dialogue){
#define KCD2VR_MENU_DRAW_DIALOGUE
#include "subsystems/dialogues/menu.inl"
#undef KCD2VR_MENU_DRAW_DIALOGUE
} else if(page==MenuPage::Crafting67){
#define KCD2VR_MENU_DRAW_CRAFTING67
#include "subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_DRAW_CRAFTING67
} else if(page==MenuPage::ClaudeExperimentsH8){
#define KCD2VR_H8_MENU_DRAW
#include "subsystems/experimental/claude_menu_h8.inl"
#undef KCD2VR_H8_MENU_DRAW
} else if(page==MenuPage::RangedOptions32){
#define KCD2VR_RANGED32_DRAW
#include "subsystems/ranged_reload/menu_xr32.inl"
#undef KCD2VR_RANGED32_DRAW
} else if(page==MenuPage::ImmersionExperiments){
#define KCD2VR_MENU_DRAW_IMMERSIONEXPERIMENTS
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_DRAW_IMMERSIONEXPERIMENTS
} else if(page==MenuPage::Experiments){
#define KCD2VR_MENU_DRAW_EXPERIMENTS
#include "subsystems/hybrid/menu.inl"
#undef KCD2VR_MENU_DRAW_EXPERIMENTS
} else if(page==MenuPage::CombatTuning49){
#define KCD2VR_MENU_DRAW_COMBATTUNING49
#include "subsystems/hybrid/menu.inl"
#undef KCD2VR_MENU_DRAW_COMBATTUNING49
} else if(page==MenuPage::Experiments2){
#define KCD2VR_MENU_DRAW_EXPERIMENTS2
#include "subsystems/direct/menu.inl"
#undef KCD2VR_MENU_DRAW_EXPERIMENTS2
} else if(page==MenuPage::NpcTuning){
#define KCD2VR_MENU_DRAW_NPC_TUNING
#include "subsystems/direct/menu.inl"
#undef KCD2VR_MENU_DRAW_NPC_TUNING
} else if(page==MenuPage::WeaponFeel59c14){
#define KCD2VR_MENU_DRAW_WEAPONFEEL59C14
#include "subsystems/physics/menu.inl"
#undef KCD2VR_MENU_DRAW_WEAPONFEEL59C14
} else if(page==MenuPage::CombatFlow59c17){
#define KCD2VR_MENU_DRAW_COMBATFLOW59C17
#include "subsystems/masterstrike_combos/menu.inl"
#undef KCD2VR_MENU_DRAW_COMBATFLOW59C17
} else if(page==MenuPage::CounterStab40){
#define KCD2VR_MENU_DRAW_COUNTERSTAB40
#include "subsystems/masterstrike_combos/menu.inl"
#undef KCD2VR_MENU_DRAW_COUNTERSTAB40
} else if(page==MenuPage::Kicks26){
#define KCD2VR_MENU_DRAW_KICKS26
#include "subsystems/experimental/menu.inl"
#undef KCD2VR_MENU_DRAW_KICKS26
} else if(page==MenuPage::VrActions){
#define KCD2VR_MENU_DRAW_VRACTIONS
#include "subsystems/interactions/menu.inl"
#undef KCD2VR_MENU_DRAW_VRACTIONS
} else if(page==MenuPage::ReleaseImmersionR92){
        namespace release=release_menu_r92;namespace live=live_settings_d39;
        pageTitle(L"IMMERSION AND CONTROLS",developer_mode::Enabled()?L"Developer mode / shared gameplay settings":L"Release mode / shared gameplay settings");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const unsigned selected=releasePageR92_.load()%release::kPages; // R124: 3 pages
        {wchar_t pg[24]{};std::swprintf(pg,std::size(pg),L"< PAGE %u",(selected+release::kPages-1)%release::kPages+1);
         DrawButton(dc,font_small_,ED39(live::kMotionPrevD69),pg,kPanel2);
         std::swprintf(pg,std::size(pg),L"PAGE %u/%u >",selected+1,release::kPages);
         DrawButton(dc,font_small_,ED39(live::kMotionNextD69),pg,kAccent);}
        const auto& rows=release::rows(selected);
        for(unsigned n=0;n<rows.size();++n){
            if(rows[n].id==release::Row::None)continue;
            wchar_t label[160]{};std::swprintf(label,std::size(label),L"%ls: %ls",rows[n].title,release::value(s,rows[n].id));
            DrawButton(dc,font_small_,ED39(live::rowRect(n)),label,kPanel2);
            const auto detail=live::detailRect(n);
            Text(dc,font_normal_,rows[n].detail,detail.l,detail.t,detail.r-detail.l,detail.b-detail.t,kText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        }
        Text(dc,font_small_,L"Developer mode is selected in the startup launcher and takes effect next launch. These controls use the same saved settings as the detailed pages.",620,600,418,55,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::LiveExperimentsD39){
        namespace live=live_settings_d39;
        pageTitle(live::group(liveGroupD39_.load())==live::Group::Combat?L"COMBAT EXPERIMENTS":L"EXPERIMENTS",L"Live settings / automatic background save");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const auto selectedPage=liveGroupD39_.load();
        const auto selected=live::group(selectedPage);
        if(selected==live::Group::Body||selected==live::Group::MotionD67||selected==live::Group::Combat||selected==live::Group::Hands||selected==live::Group::Objects||selected==live::Group::Gear){
            const bool second=selectedPage==live::kGearPage3R125||selectedPage==live::kGearPage2R118||selectedPage==live::kBodyPage2D76||live::motionPage2D69(selectedPage)||selectedPage==live::kCombatPage2D70||selectedPage==live::kHandsPage2D71||selectedPage==live::kObjectsPage2D72||selectedPage==live::kObjectsPage3R158;
            const bool body=selected==live::Group::Body,combat=selected==live::Group::Combat,hands=selected==live::Group::Hands,objects=selected==live::Group::Objects,gear=selected==live::Group::Gear;
            DrawButton(dc,font_small_,ED39(live::kMotionPrevD69),L"< PAGE 1",second?kPanel2:kAccent);
            Text(dc,font_small_,gear?(selectedPage==live::kGearPage3R125?L"HUD + impale 3 / 3":second?L"Controller 2 / 3":L"Gear 1 / 3"):body?(second?L"Body trials 2 / 2":L"Body controls 1 / 2"):objects?(selectedPage==live::kObjectsPage3R158?L"Held props 3 / 3":second?L"INSTANT 2 / 3":L"Objects 1 / 3"):hands?(second?L"VRIK trials 2 / 2":L"VRIK controls 1 / 2"):combat?(second?L"Combat trial 2 / 2":L"Combat controls 1 / 2"):(second?L"VRIK trials 2 / 2":L"VRIK controls 1 / 2"),786,88,156,24,kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            DrawButton(dc,font_small_,ED39(live::kMotionNextD69),L"PAGE 2 >",second?kAccent:kPanel2);
        }
        for(unsigned n=0;n<live::kTabs.size();++n)DrawButton(dc,font_small_,ED39(live::kTabs[n]),live::kTabsText[n],unsigned(selected)==n?kAccent:kPanel2);
        const auto& rows=live::rows(selectedPage);
        for(unsigned n=0;n<rows.size();++n){
            if(rows[n].id==live::Id::None)continue;
            wchar_t label[160]{};live::label(s,rows[n],label,std::size(label));
            if(rows[n].readOnly){const auto r=live::rowRect(n);Text(dc,font_small_,label,r.l+8,r.t,r.r-r.l-16,r.b-r.t,kMuted,DT_LEFT|DT_VCENTER|DT_SINGLELINE);}
            else DrawButton(dc,font_small_,ED39(live::rowRect(n)),label,rows[n].session?RGB(176,148,98):kPanel2);
            const auto detail=live::detailRect(n);
            Text(dc,font_normal_,rows[n].detail,detail.l,detail.t,detail.r-detail.l,detail.b-detail.t,kText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        }
        DrawButton(dc,font_small_,ED39(live::kTrials),selected==live::Group::Combat?L"MORE TUNING >":L"MORE TRIALS >",kPanel2);
        DrawButton(dc,font_small_,ED39(live::kCombat),selected==live::Group::Combat?L"COMBAT FLOW >":L"COMBAT >",kPanel2);
        DrawButton(dc,font_small_,ED39(live::kCrafting),selected==live::Group::Combat?L"SENSITIVITY >":L"CRAFTING >",kPanel2);
        const auto status=liveStatusD39_.load();
        const bool miscNavR109=selected==live::Group::Combat; // R109: the COMBAT group gets a MISC > button; its status line moves down one row
        if(miscNavR109)DrawButton(dc,font_small_,kMiscNavR109,L"MISC  >  (enemy difficulty / speeds, direct hits, counter stab, windows, strike power)",RGB(176,148,98));
        Text(dc,miscNavR109&&font_tiny_?font_tiny_:font_small_,status==1?L"The runtime owner refused this change. The displayed value is re-read from that owner.":status==2?L"Applied now, but saving failed. This change may not survive a restart.":L"Click a row to change it. Values come from the live owners. Amber rows are session-only. Renderer and upscaler startup settings remain on Graphics.",620,miscNavR109?644:616,418,miscNavR109?28:48,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::ExperimentalFeaturesH5){
        namespace ef=experimental_features_h5;
        pageTitle(L"EXPERIMENTAL FEATURES",L"");

        Text(dc,font_small_,L"Quick trials / combined headset not run.",690,76,348,24,kMuted,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const auto group=ef::group(experimentalGroupH5_.load());
        for(unsigned i=0;i<ef::kGroups.size();++i)DrawButton(dc,font_small_,EH5(ef::kGroups[i]),ef::tab(ef::Group(i)),unsigned(group)==i?kAccent:kPanel2);
        Text(dc,font_normal_,ef::heading(group),ef::kHeading.l,ef::kHeading.t,ef::kHeading.r-ef::kHeading.l,ef::kHeading.b-ef::kHeading.t,kText);
        DrawButton(dc,font_small_,EH5(ef::kCombatLink),L"COMBAT >",kPanel2);
        const auto cards=ef::cards(group);
        for(unsigned i=0;i<cards.size();++i){
            const auto& card=cards[i];const auto r=ef::kCards[i],d=ef::kDetails[i];
            if(card.trial==ef::Trial::F9)DrawButton(dc,font_small_,EH5(r),ef::f9Label(s.physical_f9_rung61),s.physical_f9_rung61?kAccent:kPanel2);
            else if(card.trial!=ef::Trial::None)DrawCheck(dc,font_small_,EH5(r),ef::enabled(card.trial),card.title);
            else Text(dc,font_small_,card.title,r.l,r.t,r.r-r.l,r.b-r.t,kMuted,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
            Text(dc,font_small_,card.detail,d.l,d.t,d.r-d.l,d.b-d.t,kMuted,DT_LEFT|DT_WORDBREAK);
        }
        DrawButton(dc,font_small_,EH5(ef::kAllOff),L"ALL PAGE TRIALS OFF",RGB(176,148,98));
        Text(dc,font_tiny_?font_tiny_:font_small_,L"Requests these trials and the physical gameplay session OFF. This button does not save. Grip-stow, alchemy head and kick preferences can be saved later through their normal settings. Opening this page enables nothing.",ef::kScope.l,ef::kScope.t,ef::kScope.r-ef::kScope.l,ef::kScope.b-ef::kScope.t,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::DeepSeekLabH29){
        pageTitle(L"DEEPSEEK EXPERIMENTS",L"Shift+F12 | Explicit opt-in | OFF on every launch");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        Text(dc,font_small_,L"REVIEWED TRIAL",620,135,250,30,kText,DT_LEFT);
        DrawButton(dc,font_small_,kDsFeaturesH5,L"ALL FEATURES >",kPanel2);
        DrawButton(dc,font_small_,kDsOffH29,L"ALL EXPERIMENTS OFF",RGB(176,148,98));
        DrawCheck(dc,font_small_,kDsWallH31,deepseek_h29::wallBackoutSpringH31.load(std::memory_order_acquire),L"Checked wall backout assist (experiment)");
        Text(dc,font_small_,L"Checked world geometry only. Adds an outward offset when pulling a stuck weapon free (up to 2.5 cm and 25% of the pull). No hit/damage effect. Session only.",620,365,418,75,kMuted,DT_LEFT|DT_WORDBREAK);
        DrawCheck(dc,font_small_,kDsFingerH31,deepseek_h29::fingerContactSpringH31.load(std::memory_order_acquire),L"Free-hand finger contact response (experiment)");
        Text(dc,font_small_,L"Free empty hands with precise contacts active. Small checked extra-clearance curl; no wrist, physics or hit changes. Session only; OFF on launch.",kDsFingerHelpH31.l,kDsFingerHelpH31.t,kDsFingerHelpH31.r-kDsFingerHelpH31.l,kDsFingerHelpH31.b-kDsFingerHelpH31.t,kMuted,DT_LEFT|DT_WORDBREAK);
        Text(dc,font_small_,L"BODY AND TURN CONTROLS\nEXPERIMENTS > BODY contains live 6DOF, roomscale and snap-turn choices.\nThe switches on this page remain separate session trials. NPC grasp and other unconnected proposals have no fake enable switch.",kDsPendingH31.l,kDsPendingH31.t,kDsPendingH31.r-kDsPendingH31.l,kDsPendingH31.b-kDsPendingH31.t,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::MeleeSensitivityH29){
        pageTitle(L"COMBAT SENSITIVITY",L"Adjust speed, travel and stroke direction. Native combat rules remain.");DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        const int id=meleeTypeH29_.load();const auto tune=melee_tuning_c54::bounded(s.melee_types_c54[id]);
        DrawButton(dc,font_small_,kH29Type,melee_tuning_ui_c54::label(id),kAccent);
        std::swprintf(v,128,L"%.0f%%",double(tune.intentH29)*100);DrawSlider(dc,font_small_,kH29Cut,L"Selected weapon: intent sensitivity",(tune.intentH29-.5f)/1.5f,v);
        std::swprintf(v,128,L"%.0f%%",double(tune.sensitivityH29)*100);DrawSlider(dc,font_small_,kH29Stab,L"Selected weapon: speed / energy sensitivity",(tune.sensitivityH29-.25f)/1.75f,v);
        std::swprintf(v,128,L"%.2f",double(s.blade_axis_cosine_h41));DrawSlider(dc,font_small_,kH29Punch,L"All melee: thrust / pommel axis alignment",(s.blade_axis_cosine_h41-.60f)/.35f,v);
        DrawButton(dc,font_small_,kStabPageD67,L"STABS > independent speed and travel",kPanel2);
        std::swprintf(v,128,L"%.0f%% of default",double(tune.travelR8)*100);DrawSlider(dc,font_small_,kH29TravelR8,L"Selected weapon: minimum tip travel",(tune.travelR8-.4f)/1.6f,v);
        DrawButton(dc,font_small_,kFistNavH41,L"FISTS",kPanel2);DrawButton(dc,font_small_,kGuardNavH41,L"BLOCK",kPanel2);
        Text(dc,font_small_,L"Weapon type changes only weapon attack controls. Fists and blocking have separate pages. Distance changes existing free-space travel, not speed or native damage.",620,590,418,65,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::StabSensitivityD67){
        pageTitle(L"STAB SENSITIVITY",L"Forward thrust recognition; native hit rules remain");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        const int id=meleeTypeH29_.load();const auto tune=thrust_tuning_d67::bounded(s.thrust_d67[id]);
        DrawButton(dc,font_small_,kH29Type,melee_tuning_ui_c54::label(id),kAccent);
        std::swprintf(v,128,L"%.2f m/s",double(tune.speed));DrawSlider(dc,font_small_,kH29Cut,L"Minimum forward stab speed",(tune.speed-.65f)/2.35f,v);
        std::swprintf(v,128,L"%.1f cm",double(tune.travel)*100);DrawSlider(dc,font_small_,kH29Stab,L"Minimum forward stab travel",(tune.travel-.06f)/.24f,v);
        Text(dc,font_small_,L"Lower values recognize shorter or slower deliberate thrusts. These distances are effective values, including polearms. Real early actor contact keeps its native path. Walking, pullback, cuts and pommel strokes keep their own rules.",620,350,418,90,kMuted,DT_LEFT|DT_WORDBREAK);
        DrawButton(dc,font_small_,kH29Block,s.independent_thrust_d67?L"[X] Independent stabbing":L"[ ] Independent stabbing (legacy coupling)",s.independent_thrust_d67?kAccent:kPanel2);
        Text(dc,font_small_,L"ON separates thrust speed and distance from swing tuning. OFF restores the previous common sensitivity. This changes recognition, never guarantees damage or a counter.",620,520,418,100,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::FistSensitivityH41){
        pageTitle(L"FISTS / PUNCHES",L"Independent of every weapon type");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        DrawButton(dc,font_small_,kWeaponNavH41,L"WEAPONS",kPanel2);DrawButton(dc,font_small_,kGuardNavH41,L"BLOCK",kPanel2);
        std::swprintf(v,128,L"%.0f%%",double(s.punch_sensitivity_c54)*100);DrawSlider(dc,font_small_,kH29Cut,L"Punch speed / energy sensitivity",(s.punch_sensitivity_c54-.5f)/1.5f,v);
        std::swprintf(v,128,L"%.1f cm",double(s.punch_minimum_m_d65)*100);DrawSlider(dc,font_small_,kH29Stab,L"Ordinary punch travel (base)",(s.punch_minimum_m_d65-.045f)/.155f,v);
        std::swprintf(v,128,L"With your sensitivity and travel settings: %.1f cm",double(s.punch_minimum_m_d65*s.punch_travel_h41/std::max(.5f,s.punch_sensitivity_c54))*100);
        Text(dc,font_small_,v,620,340,418,32,kMuted,DT_LEFT|DT_WORDBREAK);
        Text(dc,font_small_,L"Punches require Trigger AND Grip strictly above 60%. Walking or head motion alone is not a punch. Backward preparation is not an attack. Finger poses are unchanged.",620,385,418,90,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::GuardSettingsH41){
        pageTitle(L"BLOCKING BOX",L"Face-height guard; native outcomes");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        DrawButton(dc,font_small_,kFistNavH41,L"FISTS",kPanel2);DrawButton(dc,font_small_,kWeaponNavH41,L"WEAPONS",kPanel2);
        DrawButton(dc,font_small_,kShieldNavR120,L"SHIELD BOX >",kAccent); // R120
        std::swprintf(v,128,L"%.0f cm",double(s.guard_width_h41)*200);DrawSlider(dc,font_small_,kGuardWidthD56,L"Horizontal plane width",(s.guard_width_h41-.25f)/.5f,v);
        std::swprintf(v,128,L"%.0f cm",double(s.guard_height_h41)*200);DrawSlider(dc,font_small_,kGuardHeightD56,L"Vertical plane height",(s.guard_height_h41-.1f)/.25f,v);
        std::swprintf(v,128,L"%.0f cm",double(s.guard_depth_d61)*100);DrawSlider(dc,font_small_,kGuardDepthD61,L"Face-level block box depth",(s.guard_depth_d61-.10f)/.70f,v);
        std::swprintf(v,128,L"%.1f m/s",double(s.exp_guard_enter_speed));DrawSlider(dc,font_small_,kGuardEntryD56,L"Block ON: maximum movement speed",s.exp_guard_enter_speed/5.f,v);
        std::swprintf(v,128,L"%.1f m/s",double(s.exp_guard_exit_speed));DrawSlider(dc,font_small_,kGuardReleaseD56,L"Block release: forward movement speed",s.exp_guard_exit_speed/6.f,v);
        std::swprintf(v,128,L"%.0f%%",double(s.block_sensitivity_h29)*100);DrawSlider(dc,font_small_,kGuardSettleD56,L"Block settling sensitivity",s.block_sensitivity_h29-.5f,v);
        std::swprintf(v,128,L"%.0f cm",double(s.guard_body_distance_d56)*100);DrawSlider(dc,font_small_,kGuardDistanceD56,L"Maximum guard hand distance from chest",(s.guard_body_distance_d56-.35f)/.70f,v);
        Text(dc,font_small_,L"Depth adjusts front-to-back coverage. The separate chest-distance limit still applies to weapons and shields. Native block results remain unchanged.",620,605,418,50,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::HandOffsetsR150){ // R150 hand offsets, controller grip space
        pageTitle(L"HAND OFFSETS",L"Move / rotate each hand relative to its controller");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        static constexpr const wchar_t* heads[4]{L"RIGHT HAND  position (cm)",L"RIGHT HAND  rotation (deg)",L"LEFT HAND  position (cm)",L"LEFT HAND  rotation (deg)"};
        static constexpr const wchar_t* axes[6]{L"X right",L"Y up",L"Z fwd",L"Pitch",L"Yaw",L"Roll"};
        HFONT tiny=font_tiny_?font_tiny_:font_small_;
        for(int row=0;row<4;++row)Text(dc,font_small_,heads[row],620,HandOffsetRowTopR150(row),418,22,kAccent,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        for(unsigned i=0;i<12;++i){
            const UiRect r=HandOffsetSliderR150(i);const bool pos=(i%6u)<3u;const float val=s.hand_offset_r150[i];
            std::swprintf(v,128,pos?L"%ls %+.1f":L"%ls %+.0f",axes[i%6u],double(val));
            Text(dc,tiny,v,r.l,r.t-22,r.r-r.l,20,kText,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
            const float u=pos?(val+20.f)/40.f:(val+180.f)/360.f;
            const int cy=(r.t+r.b)/2,mid=(r.l+r.r)/2,x0=r.l+6,x1=r.r-6,kx=x0+int((x1-x0)*Clamp01(u));
            Line(dc,x0,cy,x1,cy,kRule,4);Line(dc,mid,cy-6,mid,cy+6,kRule,2);Line(dc,std::min(mid,kx),cy,std::max(mid,kx),cy,kAccent,4);
            Fill(dc,UiRect{kx-5,cy-8,kx+6,cy+9},kPaperHi);
        }
        DrawButton(dc,font_small_,kHandOffsetResetRR150,L"RESET RIGHT",RGB(176,148,98));
        DrawButton(dc,font_small_,kHandOffsetResetLR150,L"RESET LEFT",RGB(176,148,98));
        for(int side=0;side<2;++side){ // R153
            const int st=frik_pose_probe::HandAlignStateR153(side);
            const wchar_t* label=st==0?(side?L"ALIGN LEFT":L"ALIGN RIGHT"):st==1?L"RELEASE GRIP...":L"SQUEEZE GRIP TO SET";
            DrawButton(dc,font_small_,side?kHandAlignLR153:kHandAlignRR153,label,st?kAccent:RGB(176,148,98));
        }
        Text(dc,font_small_,L"ALIGN freezes that hand. Match the controller to the frozen hand and squeeze grip to set its offsets (click again to cancel). Offsets follow the controller (X right, Y up, Z forward); saved automatically.",620,546,418,90,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(page==MenuPage::ShieldBoxR120){ // R120 shield block box
        pageTitle(L"SHIELD BLOCK BOX",L"Left-hand shield; weapon block box unchanged");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        DrawButton(dc,font_small_,kWeaponNavH41,L"WEAPONS",kPanel2);DrawButton(dc,font_small_,kFistNavH41,L"FISTS",kPanel2);DrawButton(dc,font_small_,kGuardNavH41,L"BLOCK",kPanel2);
        const bool shieldOnR120=s.live_d39.shieldBlockBoxR120;
        DrawButton(dc,font_small_,kShieldToggleR120,shieldOnR120?L"[X] Shield block box (OFF = old wide rule, block nearly always)":L"[ ] Shield block box (OFF = old wide rule, block nearly always)",shieldOnR120?kAccent:kPanel2);
        for(unsigned i=0;i<shield_block_r120::Count;++i){
            const auto& spec=shield_block_r120::kSpecs[i];const float value=shield_block_r120::clampValue(i,s.shield_box_r120[i]);
            std::swprintf(v,128,spec.format,double(value*spec.shown));
            DrawSlider(dc,font_small_,ShieldSliderR120(i),spec.label,(value-spec.lo)/(spec.hi-spec.lo),v);
        }
} else if(page==MenuPage::VrikExperimentsD61){
        pageTitle(L"VRIK EXPERIMENTS",L"Optional comparisons; baseline remains available");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        DrawButton(dc,font_small_,kVrikLeadD61,s.free_hand_present_d61?L"[X] Free-hand presentation lead":L"[ ] Free-hand presentation lead",s.free_hand_present_d61?kAccent:kPanel2);
        Text(dc,font_small_,L"Optional lead for empty hands after the normal collision solve. Yields on contact, held items or interaction input. Uses spare collision capacity. Free-hand smoothing takes priority.",620,238,418,65,kMuted,DT_LEFT|DT_WORDBREAK);
        DrawButton(dc,font_small_,kVrikBaseD61,L"BASELINE motion",s.vrik_comparison_d61==0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrikTimingD61,L"FRESH POSE timing (Shift+F7 rung 2)",s.vrik_comparison_d61==1?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrikUncappedD67,L"Fresh Pose Uncapped",s.vrik_comparison_d61==2?kAccent:kPanel2);
        DrawButton(dc,font_small_,kVrikElbowD61,L"ELBOW reach preference (Shift+F7 rung 3)",s.vrik_comparison_d61==3?kAccent:kPanel2);
        DrawButton(dc,font_small_,kPickupCameraD65,s.live_d39.pickupCameraBridgeD55?L"[X] Pickup camera handover trial":L"[ ] Pickup camera handover trial",s.live_d39.pickupCameraBridgeD55?kAccent:kPanel2);
        Text(dc,font_small_,L"Fresh hands per game update. Head view unchanged. Shift+F7 shows availability.",620,534,418,38,kMuted,DT_LEFT|DT_WORDBREAK);
        DrawButton(dc,font_small_,kRepairTrialsD65,L"MOTION TUNING / CRAFTING TRIALS >",kPanel2);
} else if(page==MenuPage::DefenceWindowsH41){
        pageTitle(L"EXPERIMENTAL DEFENCE WINDOWS",L"Native allowance; not guaranteed success");DrawButton(dc,font_small_,kPageBack,L"< BACK",kPanel2);
        std::swprintf(v,128,L"%.2fx",double(s.pb_window_h41));DrawSlider(dc,font_small_,kH29Cut,L"Henry perfect-block allowance",s.pb_window_h41-1.f,v);
        std::swprintf(v,128,L"%.2fx",double(s.ms_window_h41));DrawSlider(dc,font_small_,kH29Stab,L"Henry masterstrike allowance",s.ms_window_h41-1.f,v);
        DrawButton(dc,font_small_,kNativeFinisherD61,s.native_finisher_test_d61?L"[X] Native finisher validation (test)":L"[ ] Native finisher validation (test)",s.native_finisher_test_d61?kAccent:kPanel2);
        DrawButton(dc,font_small_,kNeckTrialH41,s.chest_trial_h41>0?L"[X] Experimental chest / neck look-down":L"[ ] Experimental chest / neck look-down",s.chest_trial_h41>0?kAccent:kPanel2);
        DrawButton(dc,font_small_,kArcherTrialH41,s.archer_trial_h41?L"[X] Experimental archer stance":L"[ ] Experimental archer stance",s.archer_trial_h41?kAccent:kPanel2);
        DrawButton(dc,font_small_,kNativeKillNF,s.native_kill_nf?L"[X] Native paired backup (masterstrikes / finishers)":L"[ ] Native paired backup (masterstrikes / finishers)",s.native_kill_nf?kAccent:kPanel2); // I10/NF
        Text(dc,font_small_,L"Native validation removes only the extra VR finish-move filter. Native skills, sequence, target and costs remain. Paired backup also allows native animation. Tests default OFF.",620,548,418,80,kMuted,DT_LEFT|DT_WORDBREAK);
} else if(IsMiscPageR109(page)){ // R109 MISC 1/2/3 (COMBAT EXPERIMENTS > MISC): rows for saved combat keys that had no reachable row
        pageTitle(page==MenuPage::MiscCombat1R109?L"MISC 1: ENEMY":page==MenuPage::MiscCombat2R109?L"MISC 2: DIRECT HITS / COUNTER STAB":L"MISC 3: WINDOWS / STRIKE POWER",L"Combat experiments / changes apply at once and are saved");
        DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_small_,kMiscTab1R109,L"MISC 1",page==MenuPage::MiscCombat1R109?kAccent:kPanel2);
        DrawButton(dc,font_small_,kMiscTab2R109,L"MISC 2",page==MenuPage::MiscCombat2R109?kAccent:kPanel2);
        DrawButton(dc,font_small_,kMiscTab3R109,L"MISC 3",page==MenuPage::MiscCombat3R109?kAccent:kPanel2);
        if(page==MenuPage::MiscCombat1R109){
            std::swprintf(v,128,L"%.0f %%",double(s.npc_difficulty_percent)); DrawSlider(dc,font_small_,kM1DifficultyR109,L"Enemy difficulty: 0 % = native AI, 100 % = the harder C8 AI values",s.npc_difficulty_percent/100.f,v);
            std::swprintf(v,128,L"x%.2f",double(s.npc_defense_speed)); DrawSlider(dc,font_small_,kM1DefenseSpeedR109,L"Enemy defence speed, all (above 1 replaces the 3 rows below; BY SKILL ON replaces all 4)",(s.npc_defense_speed-1.f)/2.f,v);
            std::swprintf(v,128,L"x%.2f",double(s.npc_block_anim_speed109)); DrawSlider(dc,font_small_,kM1BlockAnimR109,L"Enemy block raise speed (only with EARLIER CONTACT below)",(s.npc_block_anim_speed109-1.f)/2.f,v);
            std::swprintf(v,128,L"x%.2f",double(s.npc_parry_anim_speed109)); DrawSlider(dc,font_small_,kM1ParryAnimR109,L"Enemy parry raise speed (only with EARLIER CONTACT below)",(s.npc_parry_anim_speed109-1.f)/2.f,v);
            std::swprintf(v,128,L"x%.2f",double(s.npc_dodge_anim_speed109)); DrawSlider(dc,font_small_,kM1DodgeAnimR109,L"Enemy dodge clip speed",(s.npc_dodge_anim_speed109-1.f)/2.f,v);
            std::swprintf(v,128,L"%.0f %%",double(s.npc_unarmed_spacing_percent)); DrawSlider(dc,font_small_,kM1SpacingR109,L"Unarmed enemy keeps this much of the native distance (60 % = boxing, 100 % = native)",(s.npc_unarmed_spacing_percent-25.f)/75.f,v);
            if(s.exp_npc_block_speed18<=0.f)std::swprintf(v,128,L"legacy ETA");else std::swprintf(v,128,L"x%.2f",double(s.exp_npc_block_speed18));
            DrawSlider(dc,font_small_,kM1BlockSpeed18R109,L"Enemy guard timing on VR DIRECT hits (x1 = native, higher = earlier guard, 0 = old ETA)",s.exp_npc_block_speed18<=0.f?0.f:(s.exp_npc_block_speed18-1.f)/2.f,v);
            std::swprintf(v,128,L"x%.1f",double(s.exp_direct_block_lo39)); DrawSlider(dc,font_small_,kM1DirectLoR109,L"Enemy block / parry / dodge speed at weapon skill 0 (BY SKILL below)",(s.exp_direct_block_lo39-1.f)/2.f,v);
            std::swprintf(v,128,L"x%.1f",double(s.exp_direct_block_hi39)); DrawSlider(dc,font_small_,kM1DirectHiR109,L"Enemy block / parry / dodge speed at weapon skill 30 (BY SKILL below)",(s.exp_direct_block_hi39-1.f)/2.f,v);
            std::swprintf(v,128,L"%.2f s",double(s.exp_npc_eta_prepare)); DrawSlider(dc,font_small_,kM1EtaPrepareR109,L"Time to contact told to the enemy when you prepare a stroke",(s.exp_npc_eta_prepare-.10f)/.50f,v);
            std::swprintf(v,128,L"%.2f s",double(s.exp_npc_eta_strike)); DrawSlider(dc,font_small_,kM1EtaStrikeR109,L"Time to contact told to the enemy when you strike",(s.exp_npc_eta_strike-.10f)/.50f,v);
            DrawCheck(dc,font_small_,kM1RaiseR109,s.npc_defense_raise109,L"EARLIER CONTACT: enemy block / parry clip peak moved to the shortened time (raise experiment)");
            DrawCheck(dc,font_small_,kM1BySkillR109,s.exp_direct_block_timing39,L"BY SKILL: enemy skill sets how often (25-75 %) and how fast he blocks / parries / dodges");
            DrawCheck(dc,font_small_,kM1DirBlockR109,s.exp_direct_dirblock39,L"Directional block: the waiting enemy is told the real side your blade comes from");
            std::swprintf(v,128,L"Difficulty: %ls. Defence speed: %ls. Unarmed distance: %ls.",s.npc_difficulty_percent<=0.f?L"native":s.npc_difficulty_ready?L"control ready":L"not active, native kept",
                (s.exp_direct_block_timing39?s.exp_direct_block_lo39<=1.f&&s.exp_direct_block_hi39<=1.f:s.npc_defense_speed<=1.f&&!s.npc_defense_raise109&&s.npc_dodge_anim_speed109<=1.f)?L"native":s.npc_defense_ready?L"control ready":L"not active, native kept",
                s.npc_unarmed_spacing_percent>=100.f?L"native":s.npc_unarmed_spacing_ready?L"control ready":L"unavailable (see KCD2VR.log)");
            Text(dc,font_tiny_?font_tiny_:font_small_,v,kM1StatusR109.l,kM1StatusR109.t,kM1StatusR109.r-kM1StatusR109.l,kM1StatusR109.b-kM1StatusR109.t,kMuted,DT_LEFT|DT_WORDBREAK);
        } else if(page==MenuPage::MiscCombat2R109){
            DrawCheck(dc,font_small_,kM2OwnHitsR109,s.exp_own_hits18,L"VR DIRECT: the mod delivers your blade hits itself (OFF = only the native hit)");
            DrawCheck(dc,font_small_,kM2SyncOverdueR109,s.exp_sync_overdue_direct,L"Late blade contacts go direct, without waiting for the native release");
            DrawCheck(dc,font_small_,kM2RecoveryR109,s.exp_strike_in_recovery,L"A strike may start from the tail of your previous strike (recovery)");
            DrawCheck(dc,font_small_,kM2NoRowR109,s.exp_hit_without_row,L"A strike is published even when the game returns no defence row");
            DrawCheck(dc,font_small_,kM2ReclaimR109,s.exp_reclaim_guard,L"Take the attack phase back out of your own guard");
            DrawCheck(dc,font_small_,kM2StaggerGuardR109,s.exp_direct_stagger_guard39,L"Block press out of your own hit reaction / recovery in the native perfect-block opening");
            DrawCheck(dc,font_small_,kM2CounterR109,s.exp_counter_stab40,L"Counter stab: a strong opposite-side stab into his attack start");
            DrawCheck(dc,font_small_,kM2CounterPerkR109,s.exp_counter_stab_perk44,L"Counter stab only with the game's Master strike perk");
            DrawCheck(dc,font_small_,kM2IntentLiveR109,s.exp_intent_live,L"Live intent route: hits are announced from the live stroke, after the wait below");
            DrawButton(dc,font_small_,kM2StaggerRuleR109,s.exp_stagger_rule44==0?L"WHILE YOU ARE STAGGERED: STRIKE OUT OF THE STAGGER":s.exp_stagger_rule44==2?L"WHILE YOU ARE STAGGERED: DIRECT HIT AT HALF STRENGTH":L"WHILE YOU ARE STAGGERED: NO DIRECT HIT (native)",s.exp_stagger_rule44!=1?kAccent:kPanel2);
            DrawButton(dc,font_small_,kM2MsModeR109,s.exp_ms_stab_only47==0?L"MASTER STRIKE: NATIVE (every press in the window, paired clip)":s.exp_ms_stab_only47==2?L"MASTER STRIKE: OFF (combos / ordinary attack only)":s.exp_ms_stab_only47==3?L"MASTER STRIKE: INSTANT (game grants it, your blade hits, no paired clip)":L"MASTER STRIKE: NATIVE, CUT OR STAB ONLY (paired clip)",s.exp_ms_stab_only47!=1?kAccent:kPanel2);
            std::swprintf(v,128,L"%.2f",double(s.exp_counter_stab_energy40)); DrawSlider(dc,font_small_,kM2CounterEnergyR109,L"Counter stab: minimum stroke energy (1.00 = your reference stroke)",(s.exp_counter_stab_energy40-.2f)/1.8f,v);
            std::swprintf(v,128,L"x%.2f",double(s.exp_counter_stab_mul40)); DrawSlider(dc,font_small_,kM2CounterMulR109,L"Counter stab: strength multiplier (inside the strike power cap)",s.exp_counter_stab_mul40-1.f,v);
            std::swprintf(v,128,L"%d ms",s.exp_counter_stab_window40); DrawSlider(dc,font_small_,kM2CounterWindowR109,L"Counter stab: window after his attack start",(s.exp_counter_stab_window40-150)/850.f,v);
            std::swprintf(v,128,L"%d ms",s.exp_intent_live_ms); DrawSlider(dc,font_small_,kM2IntentWaitR109,L"Live intent: wait before the hit is sent",s.exp_intent_live_ms/120.f,v);
            Text(dc,font_tiny_?font_tiny_:font_small_,L"These switches change how the mod hands your hits to the game (VR DIRECT). Native damage, perks and outcomes stay the game's. The master strike gate only decides which press may open the native window; the game still needs the perk and its own window.",kM2HelpR109.l,kM2HelpR109.t,kM2HelpR109.r-kM2HelpR109.l,kM2HelpR109.b-kM2HelpR109.t,kMuted,DT_LEFT|DT_WORDBREAK);
        } else {
            DrawButton(dc,font_small_,kM3RouterR109,s.win_router_mode109==2?L"NATIVE WINDOW TIMING (F12): ON - the six window values below are written":s.win_router_mode109==1?L"NATIVE WINDOW TIMING (F12): OBSERVE ONLY - nothing written":L"NATIVE WINDOW TIMING (F12): OFF - the game's own windows",s.win_router_mode109?kAccent:kPanel2);
            DrawCheck(dc,font_small_,kM3StrikePowerR109,s.strike_power_on109,L"Strike power rule: the mod scales the native swing by your stroke (OFF = every hit 100 % native)");
            std::swprintf(v,128,L"%.1f s",double(s.win_combo_s109)); DrawSlider(dc,font_small_,kM3WinComboR109,L"Window: combo slot duration (wh_cs_ComboSlotDuration)",(s.win_combo_s109-.5f)/2.5f,v);
            std::swprintf(v,128,L"%.2f s",double(s.win_riposte_s109)); DrawSlider(dc,font_small_,kM3WinRiposteR109,L"Window: riposte slot duration (wh_cs_RiposteSlotDuration)",(s.win_riposte_s109-.1f)/1.4f,v);
            std::swprintf(v,128,L"%.2f s",double(s.win_standard_s109)); DrawSlider(dc,font_small_,kM3WinStandardR109,L"Window: standard slot duration (wh_cs_StandardSlotDuration)",(s.win_standard_s109-.1f)/1.9f,v);
            std::swprintf(v,128,L"%.2f",double(s.win_ai_mod109)); DrawSlider(dc,font_small_,kM3WinAiR109,L"Window: enemy slot duration mod (wh_cs_AISlotDurationMod)",s.win_ai_mod109,v);
            std::swprintf(v,128,L"%.2f",double(s.win_pl_mod109)); DrawSlider(dc,font_small_,kM3WinPlR109,L"Window: player slot duration mod (wh_cs_PLSlotDurationMod)",s.win_pl_mod109,v);
            std::swprintf(v,128,L"x%.2f",double(s.win_block_speed_mod109)); DrawSlider(dc,font_small_,kM3WinBlockR109,L"Window: block speed mod (wh_cs_TimeAlignment_BlockSpeedMod)",(s.win_block_speed_mod109-.8f)/1.6f,v);
            std::swprintf(v,128,L"x%.2f",double(s.pb_window_h41)); DrawSlider(dc,font_small_,kM3PbScaleR109,L"Henry perfect-block allowance (x1 = native)",s.pb_window_h41-1.f,v);
            std::swprintf(v,128,L"x%.2f",double(s.ms_window_h41)); DrawSlider(dc,font_small_,kM3MsScaleR109,L"Henry masterstrike allowance (x1 = native; replaces VrPlayerCombatWindowScaleH26)",s.ms_window_h41-1.f,v);
            std::swprintf(v,128,L"%d ms",s.riposte_window_ms109); DrawSlider(dc,font_small_,kM3RiposteMsR109,L"Riposte window after your perfect block",(s.riposte_window_ms109-100)/2900.f,v);
            std::swprintf(v,128,L"%.0f %%",double(s.strike_min_power109)*100.); DrawSlider(dc,font_small_,kM3MinPowerR109,L"Strike power floor: the weakest hit is still this much of a native swing",(s.strike_min_power109-.05f)/.95f,v);
            std::swprintf(v,128,L"%.1f s",double(s.strike_repeat_ms109)*.001); DrawSlider(dc,font_small_,kM3RepeatMsR109,L"Repeat rule: weak strokes on the same enemy inside this time lose power",(s.strike_repeat_ms109-200)/9800.f,v);
            std::swprintf(v,128,L"%.0f kg",double(s.impale_hang_mass44)); DrawSlider(dc,font_small_,kM3HangMassR109,L"Impaled body: felt weight on the blade",(s.impale_hang_mass44-20.f)/60.f,v);
            std::swprintf(v,128,L"%.2f",double(s.impale_kill_energy44)); DrawSlider(dc,font_small_,kM3KillEnergyR109,L"Killing stab impales from this stroke energy (0 = every killing stab)",s.impale_kill_energy44,v);
        }
} else if(page==MenuPage::Graphics){
        DrawCheck(dc,font_small_,kGraphicsHead40,s.fix_head_sync,L"Head smooth (fresh pose sync)");
        std::swprintf(v,128,L"%.0f%%",double(s.engine_clarity*100.f)); DrawSlider(dc,font_small_,kV4Sharpness,L"Sharpness",s.engine_clarity/.75f,v);
        pageTitle(L"GRAPHICS",stereo_upscaler_d81r2::stereoPolicy(s.stereo_mode)?L"Apply requests a live switch; status confirms stereo output.":L"Graphics preset and live upscaling / saved automatically"); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        if(graphicsPresetR92_.load()==-2)graphicsPresetR92_.store(callbacks_.read_graphics_preset?callbacks_.read_graphics_preset(callbacks_.context):-1);
        DrawButton(dc,font_small_,kV4ResPrev,L"<",RGB(176,148,98));DrawButton(dc,font_small_,kV4ResNext,L">",RGB(176,148,98));
        const int presetR92=graphicsPresetR92_.load();const wchar_t* presetNameR92=L"CUSTOM / CURRENT";
        for(const auto& preset:startup_graphics_d80::presets)if(preset.value==presetR92)presetNameR92=preset.label;
        Text(dc,font_normal_,presetNameR92,670,140,318,34,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        const int statusR92=graphicsPresetStatusR92_.load();
        DrawButton(dc,font_small_,kV4ResApply,statusR92==1?L"PRESET APPLIED / SAVED":statusR92==2?L"PRESET FAILED / CHECK SETTINGS":L"APPLY GRAPHICS PRESET",kAccent);
        { static const wchar_t* sq[3]={L"SHADOWS LOW",L"MEDIUM 2048",L"HIGH 4096"}; // C49 SHADOW QUALITY 3-step selector (was VR SHADOWS on/off)
          for(int q=0;q<3;++q) DrawButton(dc,font_tiny_?font_tiny_:font_small_,kGfxShadowQ49[q],sq[q],s.vr_shadow_quality49==q?kAccent:kPanel2); }
        DrawButton(dc,font_tiny_?font_tiny_:font_small_,kGfxEye26,s.eye_head26?L"[X] BRIGHTNESS: HEAD STEADY":L"[ ] BRIGHTNESS: NATIVE",s.eye_head26?kAccent:kPanel2); // C26d
        std::swprintf(v,128,L"%.0f",static_cast<double>(s.merged_mesh_target)); DrawSlider(dc,font_small_,kV4ViewDistance,L"View distance",(s.merged_mesh_target-30.0f)/270.0f,v);
        std::swprintf(v,128,L"%.2f",static_cast<double>(s.gamma)); DrawSlider(dc,font_small_,kV4Gamma,L"Gamma",(s.gamma-0.50f)/1.50f,v);
        std::swprintf(v,128,L"%+.2f",static_cast<double>(s.brightness)); DrawSlider(dc,font_small_,kV4Brightness,L"Brightness",s.brightness+0.50f,v);
        std::swprintf(v,128,L"%.2f",static_cast<double>(s.contrast)); DrawSlider(dc,font_small_,kV4Contrast,L"Contrast",(s.contrast-0.50f)/1.50f,v);
        DrawButton(dc,font_small_,kV4FpsToggle,s.fps_cap_enabled?L"FPS CAP: ON":L"FPS CAP: OFF",s.fps_cap_enabled?kAccent:kPanel2);
        std::swprintf(v,128,L"%.0f FPS",static_cast<double>(s.fps_cap_target)); DrawSlider(dc,font_small_,kV4Fps,L"Target",(s.fps_cap_target-60.0f)/60.0f,v);
        if(!up_seeded_.exchange(true) && callbacks_.read_upscaler){ int t=-1,q=1,p=0; if(callbacks_.read_upscaler(callbacks_.context,&t,&q,&p)){ up_tech_.store(t); up_quality_.store(q); up_preset_.store(p); if(up_status_.load()!=5)up_status_.store(1); }else{up_tech_.store(-1);if(up_status_.load()!=5)up_status_.store(0);} }
        { const int t=up_tech_.load(),q=up_quality_.load(),p=up_preset_.load(),st=up_status_.load();
          const wchar_t* liveD85=st==3?upscaler_live_switch_d85::Instance().MenuNote():nullptr; // D85 UPSCALERS
          DrawButton(dc,font_small_,kUpTechPrev,L"<",RGB(176,148,98)); DrawButton(dc,font_small_,kUpTechNext,L">",RGB(176,148,98)); Text(dc,font_small_,UpTechName(t),655,572,348,26,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
          DrawButton(dc,font_small_,kUpQualPrev,L"<",t<=0?kPanel2:RGB(176,148,98)); DrawButton(dc,font_small_,kUpQualNext,L">",t<=0?kPanel2:RGB(176,148,98));
          Text(dc,font_small_,st==5?L"RESTART TO INITIALIZE STEREO UPSCALING":st==4?L"SELECT NATIVE / DLSS / FSR / XeSS":liveD85?liveD85:UpQualityName(t,q),655,604,348,26,kText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
          const bool presetEnabledD81R2=t==2;
          DrawButton(dc,font_small_,kUpPresetPrev,L"<",presetEnabledD81R2?RGB(176,148,98):kPanel2); DrawButton(dc,font_small_,kUpPresetNext,L">",presetEnabledD81R2?RGB(176,148,98):kPanel2); Text(dc,font_small_,UpPresetName(t,p),652,636,176,26,presetEnabledD81R2?kText:kMuted,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
          DrawButton(dc,font_small_,kUpApply,st==3?L"SAVED / REQUESTED":(st==2?L"SAVE FAILED":L"APPLY + SAVE"),st==3?kPanel2:kAccent); }

    } else if(page==MenuPage::ViewModes){
        pageTitle(L"VR VIEW MODES",L"Clean presentation modes"); DrawButton(dc,font_small_,kPageBack,L"< BACK",RGB(176,148,98));
        DrawButton(dc,font_normal_,kV4ModeMono,L"MONO",s.cyclopean_mode==0?kAccent:kPanel2);
        DrawButton(dc,font_normal_,kV4ModeCorrected,L"STEREO",s.stereo_mode==3?kAccent:kPanel2);
        Text(dc,font_small_,L"DLSS, FSR and XeSS stereo use one upscaler per eye (XeSS new in D85); a headset test is still needed. Pick one in Graphics and apply, it switches live.",620,472,418,82,kMuted,DT_CENTER|DT_WORDBREAK);
    }

    RestoreDC(dc,saved);
    // Tiny opaque texel helps the XR laser/pointer sampling remain stable at the book edge.
    Fill(dc,UiRect{0,0,3,3},RGB(194,157,76)); // shared medieval-gold laser texel
    Fill(dc,UiRect{3,0,6,3},RGB(210,42,36));   // V66.69 F4 hand-interaction red laser texel
}

bool VrMenu::UpdateCpuBitmap()
{
    if (!memory_dc_ || !dib_bits_) return false;
    VrMenuState state{}; ReadState(state);
    DrawPanel(memory_dc_, state);
    const bool calibration=capture_countdown_.load(std::memory_order_acquire)||IsDiagnosticOnlyH24();
    if (!calibration && software_cursor_active_) {
        const int x=software_cursor_x_, y=software_cursor_y_;
        HPEN pen=CreatePen(PS_SOLID,3,RGB(230,210,130)); HGDIOBJ old=SelectObject(memory_dc_,pen);
        MoveToEx(memory_dc_,x-10,y,nullptr); LineTo(memory_dc_,x+10,y); MoveToEx(memory_dc_,x,y-10,nullptr); LineTo(memory_dc_,x,y+10);
        SelectObject(memory_dc_,old); DeleteObject(pen);
    }

    // V59.7 physical mouse: the F7 book is an XR layer, so use normalized
    // client coordinates directly as panel coordinates. This remains usable
    // even when the desktop mirror is disabled.
    POINT pt{};
    if (!calibration && !vr_pointer_valid_.load(std::memory_order_acquire) && game_window_ && GetCursorPos(&pt) && ScreenToClient(game_window_, &pt)) {
        RECT cr{};
        if (GetClientRect(game_window_, &cr)) {
            const int cw=cr.right-cr.left, ch=cr.bottom-cr.top;
            if (cw>0 && ch>0) {
                const int x=std::clamp(static_cast<int>((static_cast<long long>(pt.x)*kPanelWidth)/cw),0,kPanelWidth-1);
                const int y=std::clamp(static_cast<int>((static_cast<long long>(pt.y)*kPanelHeight)/ch),0,kPanelHeight-1);
                Line(memory_dc_, x-7, y, x+8, y, RGB(255,255,255), 3);
                Line(memory_dc_, x, y-7, x, y+8, RGB(255,255,255), 3);
                Line(memory_dc_, x-5, y, x+6, y, kAccent, 1);
                Line(memory_dc_, x, y-5, x, y+6, kAccent, 1);
            }
        }
    }

    // BI_RGB GDI writes BGR but leaves alpha undefined/zero. Force uniform
    // panel alpha so the D3D12 blend stage produces a translucent overlay.
    auto* pixels = static_cast<std::uint8_t*>(dib_bits_);
    const std::size_t count = static_cast<std::size_t>(kPanelWidth) * kPanelHeight;
    const std::uint8_t alpha = preview_transparent_.load(std::memory_order_acquire) ? 51u : 238u;
    for (std::size_t i=0;i<count;++i) {
        auto* p=pixels+i*4;
        // Countdown is text only; no leftover manuscript/background rectangle.
        p[3]=calibration?block_indicator_h24::keyedAlpha(p[0],p[1],p[2]):alpha;
    }
    dice_marker_r92::writeRingBgra(pixels,kPanelWidth,kPanelHeight);
    return true;
}

bool VrMenu::EnsureGpuResources(ID3D12Resource* backbuffer)
{
    // R111: the menu texture, upload buffer, fence and command list do NOT depend on any backbuffer. R110 SteamVR:
    // every COMMON/PRESENT barrier of any command list (SteamVR records its own on the game device) becomes the
    // "latest present resource"; its format differed from the game backbuffer, so the old code destroyed and rebuilt
    // the whole menu (texture included, shaders recompiled) on every game frame -> the XR thread saw a null menu
    // texture on most frames (menu layer + laser on ~2 of 90 frames, log 20:33:56-58) and the frame rate dropped.
    // Now: core objects once (retried next tick when a call fails), only the desktop-mirror PSO follows a format.
    if (!device_) return false;
    if (!ui_texture_ || !upload_mapped_ || !root_signature_ || !command_list_ || !fence_) {
        if (!CreateCoreResourcesR111()) { ReleaseGpuResources(); return false; }
    }
    if (!backbuffer) return true;
    return EnsureDesktopPipelineR111(backbuffer->GetDesc().Format);
}

bool VrMenu::CreateCoreResourcesR111()
{
    ReleaseGpuResources(); // a half-built earlier attempt starts clean
    HRESULT hr = S_OK;
    hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
    if (FAILED(hr)) return false;
    hr = device_->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator_,nullptr,IID_PPV_ARGS(&command_list_));
    if (FAILED(hr)) return false;
    command_list_->Close();
    DeviceHook::SetInternalMenuCommandList(command_list_);

    hr = device_->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence_));
    if (FAILED(hr)) return false;
    fence_event_ = CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if (!fence_event_) return false;

    D3D12_DESCRIPTOR_HEAP_DESC rtv{}; rtv.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV; rtv.NumDescriptors=1;
    hr = device_->CreateDescriptorHeap(&rtv,IID_PPV_ARGS(&rtv_heap_)); if (FAILED(hr)) return false;
    D3D12_DESCRIPTOR_HEAP_DESC srv{}; srv.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; srv.NumDescriptors=1; srv.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = device_->CreateDescriptorHeap(&srv,IID_PPV_ARGS(&srv_heap_)); if (FAILED(hr)) return false;

    D3D12_DESCRIPTOR_RANGE range{}; range.RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_SRV; range.NumDescriptors=1; range.BaseShaderRegister=0; range.OffsetInDescriptorsFromTableStart=0;
    D3D12_ROOT_PARAMETER root{}; root.ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; root.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL; root.DescriptorTable.NumDescriptorRanges=1; root.DescriptorTable.pDescriptorRanges=&range;
    D3D12_STATIC_SAMPLER_DESC samp{}; samp.Filter=D3D12_FILTER_MIN_MAG_MIP_LINEAR; samp.AddressU=samp.AddressV=samp.AddressW=D3D12_TEXTURE_ADDRESS_MODE_CLAMP; samp.MipLODBias=0.0f; samp.MaxAnisotropy=1; samp.ComparisonFunc=D3D12_COMPARISON_FUNC_ALWAYS; samp.BorderColor=D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; samp.MinLOD=0.0f; samp.MaxLOD=D3D12_FLOAT32_MAX; samp.ShaderRegister=0; samp.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL;
    D3D12_ROOT_SIGNATURE_DESC rs{}; rs.NumParameters=1; rs.pParameters=&root; rs.NumStaticSamplers=1; rs.pStaticSamplers=&samp; rs.Flags=D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    ID3DBlob* sig=nullptr; ID3DBlob* err=nullptr;
    hr = D3D12SerializeRootSignature(&rs,D3D_ROOT_SIGNATURE_VERSION_1,&sig,&err);
    if (err) err->Release(); if (FAILED(hr)||!sig) return false;
    hr = device_->CreateRootSignature(0,sig->GetBufferPointer(),sig->GetBufferSize(),IID_PPV_ARGS(&root_signature_)); sig->Release(); if (FAILED(hr)) return false;


    D3D12_RESOURCE_DESC td{}; td.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D; td.Width=kPanelWidth; td.Height=kPanelHeight; td.DepthOrArraySize=1; td.MipLevels=1; td.Format=DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count=1; td.Layout=D3D12_TEXTURE_LAYOUT_UNKNOWN;
    D3D12_HEAP_PROPERTIES def{}; def.Type=D3D12_HEAP_TYPE_DEFAULT;
    hr=device_->CreateCommittedResource(&def,D3D12_HEAP_FLAG_NONE,&td,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&ui_texture_)); if(FAILED(hr)) return false;
    device_->GetCopyableFootprints(&td,0,1,0,&upload_footprint_,&upload_num_rows_,&upload_row_size_,&upload_total_bytes_);
    D3D12_RESOURCE_DESC bd{}; bd.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER; bd.Width=upload_total_bytes_; bd.Height=1; bd.DepthOrArraySize=1; bd.MipLevels=1; bd.SampleDesc.Count=1; bd.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    D3D12_HEAP_PROPERTIES up{}; up.Type=D3D12_HEAP_TYPE_UPLOAD;
    hr=device_->CreateCommittedResource(&up,D3D12_HEAP_FLAG_NONE,&bd,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&upload_buffer_)); if(FAILED(hr)) return false;
    D3D12_RANGE no_read{0,0}; hr=upload_buffer_->Map(0,&no_read,&upload_mapped_); if(FAILED(hr)) return false;

    D3D12_SHADER_RESOURCE_VIEW_DESC sd{}; sd.Format=DXGI_FORMAT_B8G8R8A8_UNORM; sd.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D; sd.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; sd.Texture2D.MipLevels=1;
    device_->CreateShaderResourceView(ui_texture_,&sd,srv_heap_->GetCPUDescriptorHandleForHeapStart());
    dirty_.store(true,std::memory_order_release);
    debug_trace::Write("V46.3-MENU GPU renderer initialized on game's existing D3D12 device");
    return true;
}

bool VrMenu::EnsureDesktopPipelineR111(DXGI_FORMAT format)
{
    if (pipeline_state_ && render_target_format_ == format) return true;
    if (!device_ || !root_signature_) return false;
    if (pipeline_state_) {
        // only the PSO follows the mirror target format; never release it while our GPU work may still use it
        if (fence_ && fence_value_ && fence_->GetCompletedValue() < fence_value_) {
            if (FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_)) || WaitForSingleObject(fence_event_,1000)!=WAIT_OBJECT_0) return false;
        }
        pipeline_state_->Release(); pipeline_state_ = nullptr; render_target_format_ = DXGI_FORMAT_UNKNOWN;
    }
    HRESULT hr = S_OK;
    static const char* vs_src = R"(
struct O { float4 p:SV_Position; float2 uv:TEXCOORD0; };
O main(uint id:SV_VertexID) {
  float2 p[6]={float2(-1,1),float2(1,1),float2(-1,-1),float2(-1,-1),float2(1,1),float2(1,-1)};
  float2 u[6]={float2(0,0),float2(1,0),float2(0,1),float2(0,1),float2(1,0),float2(1,1)};
  O o; o.p=float4(p[id],0,1); o.uv=u[id]; return o;
})";
    static const char* ps_src = R"(
Texture2D tex0:register(t0); SamplerState samp0:register(s0);
float4 main(float4 p:SV_Position,float2 uv:TEXCOORD0):SV_Target { return tex0.Sample(samp0,uv); }
)";
    ID3DBlob* vs=nullptr; ID3DBlob* ps=nullptr;
    if (!CompileShader(vs_src,"main","vs_5_0",&vs) || !CompileShader(ps_src,"main","ps_5_0",&ps)) { if(vs)vs->Release(); if(ps)ps->Release(); return false; }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature=root_signature_;
    pso.VS={vs->GetBufferPointer(),vs->GetBufferSize()}; pso.PS={ps->GetBufferPointer(),ps->GetBufferSize()};
    pso.BlendState.AlphaToCoverageEnable=FALSE; pso.BlendState.IndependentBlendEnable=FALSE;
    auto& rt=pso.BlendState.RenderTarget[0]; rt.BlendEnable=TRUE; rt.SrcBlend=D3D12_BLEND_SRC_ALPHA; rt.DestBlend=D3D12_BLEND_INV_SRC_ALPHA; rt.BlendOp=D3D12_BLEND_OP_ADD; rt.SrcBlendAlpha=D3D12_BLEND_ONE; rt.DestBlendAlpha=D3D12_BLEND_INV_SRC_ALPHA; rt.BlendOpAlpha=D3D12_BLEND_OP_ADD; rt.RenderTargetWriteMask=D3D12_COLOR_WRITE_ENABLE_ALL;
    pso.SampleMask=UINT_MAX;
    pso.RasterizerState.FillMode=D3D12_FILL_MODE_SOLID; pso.RasterizerState.CullMode=D3D12_CULL_MODE_NONE; pso.RasterizerState.DepthClipEnable=TRUE;
    pso.DepthStencilState.DepthEnable=FALSE; pso.DepthStencilState.DepthWriteMask=D3D12_DEPTH_WRITE_MASK_ZERO; pso.DepthStencilState.DepthFunc=D3D12_COMPARISON_FUNC_ALWAYS; pso.DepthStencilState.StencilEnable=FALSE; pso.DepthStencilState.StencilReadMask=D3D12_DEFAULT_STENCIL_READ_MASK; pso.DepthStencilState.StencilWriteMask=D3D12_DEFAULT_STENCIL_WRITE_MASK; D3D12_DEPTH_STENCILOP_DESC sop{}; sop.StencilFailOp=D3D12_STENCIL_OP_KEEP; sop.StencilDepthFailOp=D3D12_STENCIL_OP_KEEP; sop.StencilPassOp=D3D12_STENCIL_OP_KEEP; sop.StencilFunc=D3D12_COMPARISON_FUNC_ALWAYS; pso.DepthStencilState.FrontFace=sop; pso.DepthStencilState.BackFace=sop;
    pso.PrimitiveTopologyType=D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; pso.NumRenderTargets=1; pso.RTVFormats[0]=format; pso.SampleDesc.Count=1;
    hr=device_->CreateGraphicsPipelineState(&pso,IID_PPV_ARGS(&pipeline_state_)); vs->Release(); ps->Release(); if(FAILED(hr)) return false;
    render_target_format_=format;
    return true;
}

bool VrMenu::UploadBitmap(ID3D12GraphicsCommandList* list)
{
    if (!list || !upload_mapped_ || !dib_bits_ || !ui_texture_) return false;
    if (!UpdateCpuBitmap()) return false;
    auto* dst=static_cast<std::uint8_t*>(upload_mapped_);
    auto* src=static_cast<const std::uint8_t*>(dib_bits_);
    const UINT src_pitch=kPanelWidth*4;
    for(UINT row=0; row<upload_num_rows_ && row<static_cast<UINT>(kPanelHeight); ++row)
        std::memcpy(dst + static_cast<std::size_t>(upload_footprint_.Offset) + static_cast<std::size_t>(row)*upload_footprint_.Footprint.RowPitch,
                    src + static_cast<std::size_t>(row)*src_pitch, src_pitch);

    if (ui_texture_ready_) {
        D3D12_RESOURCE_BARRIER b{}; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource=ui_texture_; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; b.Transition.StateBefore=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; b.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST; list->ResourceBarrier(1,&b);
    }
    D3D12_TEXTURE_COPY_LOCATION dstloc{}; dstloc.pResource=ui_texture_; dstloc.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; dstloc.SubresourceIndex=0;
    D3D12_TEXTURE_COPY_LOCATION srcloc{}; srcloc.pResource=upload_buffer_; srcloc.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; srcloc.PlacedFootprint=upload_footprint_;
    list->CopyTextureRegion(&dstloc,0,0,0,&srcloc,nullptr);
    D3D12_RESOURCE_BARRIER b{}; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource=ui_texture_; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; b.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; b.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; list->ResourceBarrier(1,&b);
    ui_texture_ready_=true;
    dirty_.store(false,std::memory_order_release);
    // Keep the UI texture refreshing for the short page-slide animation.
    if(page_animating_.load(std::memory_order_acquire)) dirty_.store(true,std::memory_order_release);
    return true;
}

bool VrMenu::RenderOverlay(ID3D12CommandQueue* queue)
{
    // V50: F7 is no longer composited into KCD2's backbuffer. We only keep the
    // existing 1100x680 parchment texture current; XrManager copies this texture
    // into a dedicated OpenXR UI swapchain and submits it as a second layer.
    // R111: no backbuffer needed (copy only); a still-running previous upload is skipped (the texture keeps its
    // last complete content) instead of stalling the game's render thread up to 1 s.
    if (!queue || !EnsureGpuResources(nullptr)) return false;
    if (fence_value_ && fence_->GetCompletedValue() < fence_value_) return ui_texture_ready_;
    if (FAILED(allocator_->Reset())) return false;
    if (FAILED(command_list_->Reset(allocator_,nullptr))) return false;

    static thread_local std::uint64_t last_refresh=0;
    const auto now=GetTickCount64();
    if (now-last_refresh>=250) { dirty_.store(true,std::memory_order_release); last_refresh=now; }
    if (dirty_.load(std::memory_order_acquire) && !UploadBitmap(command_list_)) { command_list_->Close(); return ui_texture_ready_; }
    if (!ui_texture_ready_) { command_list_->Close(); return false; }

    if (FAILED(command_list_->Close())) return false;
    ID3D12CommandList* lists[]={command_list_};
    if(!DeviceHook::ExecuteCommandListsBypass(queue,1,lists)) return false;
    ++fence_value_;
    if(FAILED(queue->Signal(fence_,fence_value_))) return false;
    static std::atomic<unsigned long long> uploads{0};
    const auto n=uploads.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=4 || (n%600ull)==0) {
        char msg[256]{};
        std::snprintf(msg,sizeof(msg),"V50-MENU-TEXTURE ready call=%llu texture=%p size=%dx%d; backbuffer untouched",
            n, ui_texture_, kPanelWidth, kPanelHeight);
        debug_trace::Write(msg);
    }
    return true;
}

bool VrMenu::RenderDesktopOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer)
{
    if(!queue || !backbuffer || !ui_texture_ready_ || !EnsureGpuResources(backbuffer)) return false;
    if(fence_value_ && fence_->GetCompletedValue()<fence_value_) {
        if(FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_))) return false;
        if(WaitForSingleObject(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    }
    if(FAILED(allocator_->Reset())) return false;
    if(FAILED(command_list_->Reset(allocator_,pipeline_state_))) return false;

    D3D12_RESOURCE_BARRIER to_rt{}; to_rt.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; to_rt.Transition.pResource=backbuffer; to_rt.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; to_rt.Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT; to_rt.Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list_->ResourceBarrier(1,&to_rt);
    device_->CreateRenderTargetView(backbuffer,nullptr,rtv_heap_->GetCPUDescriptorHandleForHeapStart());
    ID3D12DescriptorHeap* heaps[]={srv_heap_}; command_list_->SetDescriptorHeaps(1,heaps);
    command_list_->SetGraphicsRootSignature(root_signature_); command_list_->SetPipelineState(pipeline_state_); command_list_->SetGraphicsRootDescriptorTable(0,srv_heap_->GetGPUDescriptorHandleForHeapStart());
    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const auto bd=backbuffer->GetDesc();
    last_backbuffer_width_.store(static_cast<std::uint32_t>(bd.Width),std::memory_order_release);
    last_backbuffer_height_.store(static_cast<std::uint32_t>(bd.Height),std::memory_order_release);
    // Fit the whole 1100x680 book on the desktop while preserving aspect ratio.
    const float max_w=static_cast<float>(bd.Width)*0.94f, max_h=static_cast<float>(bd.Height)*0.92f;
    const float scale=std::min(max_w/static_cast<float>(kPanelWidth), max_h/static_cast<float>(kPanelHeight));
    const float w=static_cast<float>(kPanelWidth)*scale, h=static_cast<float>(kPanelHeight)*scale;
    const float x=(static_cast<float>(bd.Width)-w)*0.5f, y=(static_cast<float>(bd.Height)-h)*0.5f;
    D3D12_VIEWPORT vp{}; vp.TopLeftX=x; vp.TopLeftY=y; vp.Width=w; vp.Height=h; vp.MinDepth=0; vp.MaxDepth=1;
    D3D12_RECT sc{static_cast<LONG>(x),static_cast<LONG>(y),static_cast<LONG>(x+w),static_cast<LONG>(y+h)};
    command_list_->RSSetViewports(1,&vp); command_list_->RSSetScissorRects(1,&sc);
    const auto rtv=rtv_heap_->GetCPUDescriptorHandleForHeapStart(); command_list_->OMSetRenderTargets(1,&rtv,FALSE,nullptr);
    command_list_->DrawInstanced(6,1,0,0);
    D3D12_RESOURCE_BARRIER to_present=to_rt; to_present.Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; to_present.Transition.StateAfter=D3D12_RESOURCE_STATE_PRESENT; command_list_->ResourceBarrier(1,&to_present);
    if(FAILED(command_list_->Close())) return false;
    ID3D12CommandList* lists[]={command_list_}; if(!DeviceHook::ExecuteCommandListsBypass(queue,1,lists)) return false;
    ++fence_value_; if(FAILED(queue->Signal(fence_,fence_value_))) return false;
    return true;
}

void VrMenu::PumpInput()
{
    PollInput();
}

void VrMenu::PumpVrInput(bool active, int pointer_x, int pointer_y, bool pointer_valid, float trigger, float thumb_x, float thumb_y, bool a_pressed, bool b_pressed)
{
    vr_pointer_valid_.store(active&&pointer_valid&&visible_.load(std::memory_order_acquire),std::memory_order_release);
    const bool trig=trigger>0.55f;
    if(!visible_.load(std::memory_order_acquire)||!active){
        vr_trigger_prev_=trig; vr_trigger_started_ms_=0; vr_cursor_last_ms_=0;
        vr_stick_left_prev_=vr_stick_right_prev_=vr_stick_up_prev_=vr_stick_down_prev_=false;
        preview_transparent_.store(false,std::memory_order_release); return;
    }
    if(b_pressed){ SetVisible(false); debug_trace::Write("V62.4-MENU-VR B=close"); return; }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!software_cursor_active_){ software_cursor_x_=kPanelWidth/2; software_cursor_y_=kPanelHeight/2; }

    // Page navigation is independent of ray validity. This fixes the prior bug
    // where a valid Touch pointer suppressed thumbstick tab/page switching.
    const bool stick_left=thumb_x<-0.75f, stick_right=thumb_x>0.75f;
    if(stick_left&&!vr_stick_left_prev_) { CyclePage(-1); debug_trace::Write("V62.4-MENU stick LEFT -> previous page"); }
    if(stick_right&&!vr_stick_right_prev_) { CyclePage(+1); debug_trace::Write("V62.4-MENU stick RIGHT -> next page"); }
    vr_stick_left_prev_=stick_left; vr_stick_right_prev_=stick_right;

    if(pointer_valid){
        software_cursor_x_=std::clamp(pointer_x,0,kPanelWidth-1);
        software_cursor_y_=std::clamp(pointer_y,0,kPanelHeight-1);
        software_cursor_active_=true;
        static bool logged=false; if(!logged){ logged=true; debug_trace::Write("V62.4-MENU Touch AIM ray cursor active"); }
    } else {
        if(vr_cursor_last_ms_==0) vr_cursor_last_ms_=now;
        const float dt=std::clamp(static_cast<float>(now-vr_cursor_last_ms_)/1000.0f,0.0f,0.05f);
        const float sy=std::fabs(thumb_y)>0.16f?thumb_y:0.0f;
        // Horizontal stick is reserved for page turns; vertical still moves the
        // fallback software cursor when the aim ray misses the book.
        software_cursor_y_=std::clamp(software_cursor_y_-static_cast<int>(sy*620.0f*dt),0,kPanelHeight-1);
        software_cursor_active_=true;
    }
    vr_cursor_last_ms_=now; MarkDirty();
    if(diagnostics::ownership48::active(now)){
        static std::uint64_t last48=0;if(!last48||now<last48||now-last48>=100){last48=now;
            diagnostics::ownership48::Event e{};e.stage=8;e.tick=now;e.thread=GetCurrentThreadId();e.flags=pointer_valid?1u:0u;e.code=0;
            e.v[0]=float(pointer_x);e.v[1]=float(pointer_y);e.v[2]=float(software_cursor_x_);e.v[3]=float(software_cursor_y_);e.v[4]=trigger;
            diagnostics::ownership48::name(e,"MOD_MENU_PANEL_RAY_TO_CLICK");(void)diagnostics::ownership48::push(e);
        }
    }


    if(trig&&!vr_trigger_prev_){
        vr_trigger_started_ms_=now; preview_transparent_.store(false,std::memory_order_release);
        HandleClick(software_cursor_x_,software_cursor_y_);
        debug_trace::Write(pointer_valid?"V62.4-MENU-VR aim-select":"V62.4-MENU-VR fallback-select");
    } else if(trig&&drag_control_.load(std::memory_order_acquire)!=static_cast<int>(DragControl::None)){
        if(vr_trigger_started_ms_&&now-vr_trigger_started_ms_>=500ull&&!preview_transparent_.exchange(true,std::memory_order_acq_rel)){ MarkDirty(); }
        HandleDrag(software_cursor_x_,software_cursor_y_);
    }
    if(!trig&&vr_trigger_prev_){
        const auto released=static_cast<DragControl>(drag_control_.load(std::memory_order_acquire));
        drag_control_.store(static_cast<int>(DragControl::None),std::memory_order_release);
        CommitDrag(released); vr_trigger_started_ms_=0;
        if(preview_transparent_.exchange(false,std::memory_order_acq_rel)) MarkDirty();
    }
    if(a_pressed) HandleClick(software_cursor_x_,software_cursor_y_);
    vr_trigger_prev_=trig;
}

void VrMenu::OnRenderThreadTick(ID3D12CommandQueue* queue)
{
    if (!initialized_.load(std::memory_order_acquire)) return;
    const bool visible=visible_.load(std::memory_order_acquire);
    if(!IsLayerVisible() && ui_texture_ready_) return; // one hidden initialization only; never a periodic hidden draw
    // R111: upload only on the game's own DIRECT queue. XrManager copies this texture into the menu swapchain on that
    // same queue right after this call, so queue order = upload before copy. R110 followed "the batch that carried
    // the latest PRESENT barrier", which under SteamVR is often a runtime batch (other queue, other format).
    // No backbuffer is touched. Cadence: a new game present, or 33 ms without one, never faster than 8 ms.
    ID3D12Device* gameDevice=nullptr; ID3D12CommandQueue* gameQueue=nullptr;
    if (!queue || !DeviceHook::TryGetDevice(&gameDevice,&gameQueue) || queue!=gameQueue) return;
    if(visible) ClipCursor(nullptr); // KCD2 may re-clip every frame; keep mouse free while menu is open.
    std::uint64_t serial=0;
    if (!DeviceHook::GetLatestPresentIdentity(&serial,nullptr,nullptr,nullptr)) serial=0;
    const std::uint64_t now=GetTickCount64();
    const bool newPresent=serial!=0 && serial!=last_present_serial_;
    if (now-last_upload_ms_r111_<8) return; // also paces a retry after a failed create (no latch)
    if (ui_texture_ready_ && !newPresent && now-last_upload_ms_r111_<33) return;
    last_present_serial_ = serial; last_upload_ms_r111_ = now;
    const bool ok = RenderOverlay(queue);
    if (!ok) {
        static std::atomic<unsigned> failures{0};
        const auto n = failures.fetch_add(1, std::memory_order_relaxed) + 1;
        if (n <= 6 || (n % 300u) == 0) debug_trace::Write("V46.3-MENU render attempt failed; VR bridge continues unaffected");
    }
}

void VrMenu::OnDesktopOverlayTick(ID3D12CommandQueue* queue)
{
    if(!initialized_.load(std::memory_order_acquire) || !visible_.load(std::memory_order_acquire)) return;
    if(!DeviceHook::CurrentExecuteSubmittedPresent()) return;
    ID3D12Resource* source=nullptr; std::uint64_t serial=0;
    if(!DeviceHook::TryGetLatestPresentResourceTagged(&source,nullptr,&serial) || !source) return;
    if(serial==0 || serial==last_desktop_serial_){ source->Release(); return; }
    last_desktop_serial_=serial;
    const bool ok=RenderDesktopOverlay(queue,source);
    if(!ok){ static std::atomic<unsigned> fails{0}; const auto n=fails.fetch_add(1,std::memory_order_relaxed)+1; if(n<=4 || n%300u==0) debug_trace::Write("V58-MENU desktop hybrid draw failed; XR menu remains independent"); }
    source->Release();
}

} // namespace kcd2vr
