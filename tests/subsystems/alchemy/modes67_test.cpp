// Actual per-activity admission gates; no simulated native/tool acceptance.
#include "../../../src/vrik/workstation178r20c1.h"
#include "../../../src/immersion_menu45.h"
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
namespace m=kcd2vr::minigame_hands45;
namespace w=kcd2vr::workstation178r20c1;
namespace c=kcd2vr::combat_rules178r10;
static unsigned checks=0;
static void ck(bool b){if(++checks&&!b){std::fprintf(stderr,"modes67 check %u failed\n",checks);std::abort();}}
namespace cm=kcd2vr::crafting_menu_h18;
namespace immersion_menu45=kcd2vr::immersion_menu45;
using UiRect=kcd2vr::combat_menu49::Rect;
static constexpr UiRect U49(UiRect r){return r;}
static constexpr UiRect kMinigameHands45=immersion_menu45::kMinigameHands;
static bool PtIn(UiRect r,int x,int y){return x>=r.l&&x<r.r&&y>=r.t&&y<r.b;}
struct UiStateH18 {int alchemy_mode67=0,smithing_mode67=0;bool activity_hands59c7f[4]{};bool smith_auto_hold36=false,craft_trial_h41=false;};
static unsigned appliedH18=0,savedH18=0,dirtyH18=0;
static void ApplyState(const UiStateH18& s){++appliedH18;m::setMode(m::Alchemy,s.alchemy_mode67);m::setMode(m::Smithing,s.smithing_mode67);}
static void save(){++savedH18;}static void MarkDirty(){++dirtyH18;}
struct DrawH18 {UiRect rect;std::wstring label;int color;};
static std::vector<DrawH18> drawnH18;
static constexpr int kAccent=1,kPanel2=2,kText=3,kMuted=4,DT_SINGLELINE=1,DT_WORDBREAK=2;
static void DrawButton(int,int,UiRect r,const wchar_t* label,int color){drawnH18.push_back({r,label,color});}
template<class... Args>static void Text(Args&&...){}
static void drawImmersionH18(UiStateH18& s){int dc=0,font_small_=0;wchar_t v[128]{};
#define KCD2VR_MENU_DRAW_IMMERSION_CRAFTING_H18
#include "../../../src/subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_DRAW_IMMERSION_CRAFTING_H18
}
static void drawDetailsH18(UiStateH18& s){int dc=0,font_small_=0;
 const auto pageTitle=[](const wchar_t*,const wchar_t*){};const auto vrNav=[](){};
#define KCD2VR_MENU_DRAW_CRAFTING67
#include "../../../src/subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_DRAW_CRAFTING67
}
// Execute the actual owned menu click contributions, not a copied click model.
static void clickImmersionH18(UiStateH18& s,int x,int y){
#define KCD2VR_MENU_CLICK_IMMERSION_CRAFTING_H18
#include "../../../src/subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_CLICK_IMMERSION_CRAFTING_H18
}
static void clickDetailsH18(UiStateH18& s,int x,int y){
#define KCD2VR_MENU_CLICK_CRAFTING67
#include "../../../src/subsystems/alchemy/menu.inl"
#undef KCD2VR_MENU_CLICK_CRAFTING67
}
static void uiCasesH18(){
 UiStateH18 s{};
 for(unsigned kind:{unsigned(m::Alchemy),unsigned(m::Smithing)}){
  const auto r=kind==m::Alchemy?immersion_menu45::kMinigameHands:immersion_menu45::kSmithingModeH18;
  for(int target:{1,2,3,0}){
   const int other=kind==m::Alchemy?s.smithing_mode67:s.alchemy_mode67;
   const auto count=appliedH18;clickImmersionH18(s,(r.l+r.r)/2,(r.t+r.b)/2);
   ck(cm::selected(s,cm::Kind(kind))==target&&m::modeFor(kind)==target);
   ck((kind==m::Alchemy?s.smithing_mode67:s.alchemy_mode67)==other);
   ck(appliedH18==count+1&&savedH18==appliedH18&&dirtyH18==appliedH18);
  }
  for(int target=0;target<cm::kModeCount;++target){
   const int other=kind==m::Alchemy?s.smithing_mode67:s.alchemy_mode67;
   const auto r2=cm::kChoices[kind==m::Alchemy?0:1][target];
   const auto count=appliedH18;clickDetailsH18(s,(r2.l+r2.r)/2,(r2.t+r2.b)/2);
   ck(cm::selected(s,cm::Kind(kind))==target&&m::modeFor(kind)==target);
   ck((kind==m::Alchemy?s.smithing_mode67:s.alchemy_mode67)==other);
   ck(appliedH18==count+1&&savedH18==appliedH18&&dirtyH18==appliedH18);
  }
 }
 ck(s.activity_hands59c7f[0]&&s.activity_hands59c7f[1]&&s.smith_auto_hold36);
 const auto count=appliedH18;clickImmersionH18(s,100,100);clickDetailsH18(s,100,100);ck(appliedH18==count);
 ck(!std::wcscmp(cm::label(0),L"NATIVE")&&!std::wcscmp(cm::label(1),L"SEMI")&&!std::wcscmp(cm::label(2),L"FREE VRIK")&&!std::wcscmp(cm::label(3),L"6DOF VRIK"));
 cm::select(s,m::Alchemy,2);cm::select(s,m::Smithing,1);drawnH18.clear();drawImmersionH18(s);
 ck(drawnH18.size()==2&&drawnH18[0].label==L"ALCHEMY: FREE VRIK"&&drawnH18[1].label==L"SMITHING: SEMI");
 ck(drawnH18[0].rect.l==immersion_menu45::kMinigameHands.l&&drawnH18[1].rect.l==immersion_menu45::kSmithingModeH18.l);
 drawnH18.clear();drawDetailsH18(s);ck(drawnH18.size()==9);
 ck(!s.craft_trial_h41);clickDetailsH18(s,700,620);ck(s.craft_trial_h41);clickDetailsH18(s,700,620);ck(!s.craft_trial_h41);
 for(unsigned i=0;i<drawnH18.size();++i){const auto& draw=drawnH18[i];
  if(i<8){ck(draw.label==cm::label(int(i%4)));ck(draw.color==((i==2||i==5)?kAccent:kPanel2));}
  else ck(draw.label.find(L"CRAFT1")!=std::wstring::npos);
  ck(kcd2vr::combat_menu49::inside(draw.rect,kcd2vr::combat_menu49::kClip));
  for(unsigned j=i+1;j<drawnH18.size();++j)ck(!kcd2vr::combat_menu49::overlap(draw.rect,drawnH18[j].rect));
 }
}
int main(){
 static_assert(m::Native==0&&m::SemiPassenger==1&&m::Gesture==2&&m::SixDofVrik==3,"saved mode ABI");
 for(int value=-2;value<=5;++value)ck(m::clampMode(value)==(value>=0&&value<=3?value:0));
 ck(m::freeTrackedMode(m::Gesture)&&m::freeTrackedMode(m::SixDofVrik)&&!m::freeTrackedMode(m::Native)&&!m::freeTrackedMode(m::SemiPassenger));
 ck(m::sixDofMode(m::SixDofVrik)&&!m::sixDofMode(m::Gesture));
 m::mode=m::SixDofVrik;m::alchemyMode67=-1;m::smithingMode67=-1;
 ck(m::modeFor(m::Alchemy)==m::SixDofVrik&&m::modeFor(m::Smithing)==m::SixDofVrik);
 m::setMode(m::Alchemy,m::Gesture);const auto oldEpoch=m::modeEpoch67.load();m::setMode(m::Alchemy,m::SixDofVrik);
 ck(m::modeEpoch67.load()==oldEpoch+1&&m::modeFor(m::Alchemy)==m::SixDofVrik&&m::modeFor(m::Smithing)==m::SixDofVrik);
 m::setMode(m::Alchemy,m::SixDofVrik);ck(m::modeEpoch67.load()==oldEpoch+1);
 m::mode=m::SemiPassenger;m::alchemyMode67=-1;m::smithingMode67=-1;
 c::workstationEnabled=true;c::f5Rung=0;w::setExperiment36(false);
 w::gesturesEverywhere59c2=false;
 ck(m::modeFor(m::Alchemy)==m::SemiPassenger&&m::modeFor(m::Smithing)==m::SemiPassenger);
 m::setMode(m::Alchemy,m::Gesture);m::setMode(m::Smithing,m::Native);
 ck(m::modeFor(m::Alchemy)==m::Gesture&&m::modeFor(m::Smithing)==m::Native);
 m::publish(m::Alchemy,1000);
 ck(m::witnessCurrent67(m::Alchemy,1080)&&!m::witnessCurrent67(m::Smithing,1000)&&!m::witnessCurrent67(m::Alchemy,1081));
 ck(w::fullVrAlchemy63(1000)&&w::ownsActivityWrist45(1000)&&w::gesturesOn(1000));
 ck(!w::semiPassengerOwns45(1000)&&!w::passengerAlchemy63(1000));
 ck(!w::gestureSmith67(1000)&&!w::fullVrAlchemy63(1081));
 // F5 rung 1 changes no owner; saved mode is independently sufficient.
 c::f5Rung=1;w::setExperiment36(false);
 ck(w::fullVrAlchemy63(1000)&&w::ownsActivityWrist45(1000));
 m::publish(m::Smithing,1000);
 ck(!m::witnessCurrent67(m::Alchemy,1000)&&m::witnessCurrent67(m::Smithing,1000));
 ck(!w::fullVrAlchemy63(1000)&&!w::gestureSmith67(1000)&&!w::ownsActivityWrist45(1000));
 m::setMode(m::Smithing,m::Gesture);
 ck(w::gestureSmith67(1000)&&w::gesturesOn(1000)&&w::ownsActivityWrist45(1000));
 ck(m::modeFor(m::Alchemy)==m::Gesture);
 m::setMode(m::Alchemy,m::Native);
 ck(w::gestureSmith67(1000)&&m::modeFor(m::Alchemy)==m::Native);
 m::publish(m::Alchemy,1000);
 ck(!w::fullVrAlchemy63(1000)&&!w::ownsActivityWrist45(1000));
 m::setMode(m::Alchemy,m::SemiPassenger);
 ck(w::semiPassengerOwns45(1000)&&w::passengerAlchemy63(1000)&&!w::fullVrAlchemy63(1000));
 // The existing F5=2 alternative remains, but Native remains authoritative.
 c::f5Rung=2;w::setExperiment36(true);
 ck(w::fullVrAlchemy63(1000));
 m::setMode(m::Alchemy,m::Native);
 ck(!w::fullVrAlchemy63(1000)&&!w::ownsActivityWrist45(1000));
 m::setMode(m::Alchemy,m::Gesture);c::workstationEnabled=false;
 ck(!w::fullVrAlchemy63(1000)&&!w::ownsActivityWrist45(1000)&&!w::gesturesOn(1000));
 c::workstationEnabled=true;m::publish(m::None,1000);
 ck(!w::fullVrAlchemy63(1000)&&!w::gestureSmith67(1000));
 ck(m::modeFor(m::Alchemy)==m::Gesture&&m::modeFor(m::Smithing)==m::Gesture);
 // H18: the saved Native choice vetoes global gestures only for its fresh
 // activity. Cross-activity preferences and stale/unknown witnesses cannot
 // disable normal world gestures, and guided F5 keeps its explicit override.
 w::gesturesEverywhere59c2=true;c::workstationEnabled=true;
 for(unsigned kind:{unsigned(m::Alchemy),unsigned(m::Smithing)})for(int mode=0;mode<cm::kModeCount;++mode){
  m::setMode(kind,mode);m::setMode(kind==m::Alchemy?m::Smithing:m::Alchemy,mode==m::Native?m::Gesture:m::Native);
  for(int rung=0;rung<4;++rung)for(bool armed:{false,true}){
   c::f5Rung=rung;w::setExperiment36(armed);m::publish(kind,4000);
   const bool guided=rung==3&&armed;
   ck(w::gesturesOn(4000)==(mode!=m::Native||guided));
   ck(w::gesturesOn(4080)==(mode!=m::Native||guided));
   ck(w::gesturesOn(4081)); // stale witness is not an activity owner
   ck(w::gesturesOn(3999)); // future witness is not an activity owner
   if(m::freeTrackedMode(mode))ck(kind==m::Alchemy?w::fullVrAlchemy63(4000):w::gestureSmith67(4000));
  }
 }
 c::f5Rung=0;w::setExperiment36(false);m::publish(m::None,0);ck(w::gesturesOn(4000));
 c::workstationEnabled=false;ck(!w::gesturesOn(4000));c::workstationEnabled=true;
 uiCasesH18();
 std::printf("modes67: %u checks PASS (actual Immersion/details clicks, per-activity Native/Semi/Free/6DOF, saved ABI, mode epochs, F5 and freshness)\n",checks);
}
