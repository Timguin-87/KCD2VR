#include "subsystems/experimental/claude_options_h8.h"
#include <cassert>
#include <cwchar>
#include <map>
#include <string>
#include <vector>
namespace kcd2vr::physical {inline std::atomic<bool> pommelStrikeDefault69{true};}
namespace kcd2vr::kick26 {inline std::atomic<int> assistCm72{0};}
namespace kcd2vr::frik_pose_probe {inline float radius=.55f;float ClinchDistanceMeters32(){return radius;}void SetClinchDistanceMeters32(float f){radius=f;}}
using namespace kcd2vr;
struct State {
#define KCD2VR_M2_HOST_SLOT 274
#include "subsystems/experimental/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 bool kick_real_hit37c2=false;
};
static std::map<std::string,double> ini;
static void fill(State& out){
#define KCD2VR_M2_HOST_SLOT 34
#include "subsystems/experimental/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
static void apply(const State& state){
#define KCD2VR_M2_HOST_SLOT 80
#include "subsystems/experimental/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
static bool persist(){bool ok=true;auto wi=[](const char* n,int v){ini[n]=v;return true;};auto wb=[](const char* n,bool v){ini[n]=v;return true;};auto wf=[](const char* n,float v){ini[n]=v;return true;};
#define KCD2VR_M2_HOST_SLOT 116
#include "subsystems/experimental/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 return ok;
}
static bool ReadIniBoolExplicit(void*,const char*,const char* k,bool d){return ini.count(k)?ini[k]!=0:d;}
static int ReadIniIntExplicit(void*,const char*,const char* k,int d){return ini.count(k)?int(ini[k]):d;}
static float ReadIniFloatExplicit(void*,const char*,const char* k,float d){return ini.count(k)?float(ini[k]):d;}
static void load(){void* explicit_ini=nullptr;
#define KCD2VR_M2_HOST_SLOT 158
#include "subsystems/experimental/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
using UiRect=claude_h8::Rect;static UiRect U49(UiRect r){return r;}
static bool PtIn(UiRect r,int x,int y){return x>=r.l&&x<r.r&&y>=r.t&&y<r.b;}
constexpr UiRect kPageBack{620,70,680,95};
constexpr int kPanel2=1,kAccent=2,kMuted=3,DT_LEFT=0,DT_WORDBREAK=16;
enum class MenuPage{Experiments,Kicks26};enum class DragControl{None,KickAssistH8};
struct Menu {
 State state{};int applies=0,saves=0;DragControl drag=DragControl::None;bool back=false;MenuPage target=MenuPage::Experiments;
 std::vector<UiRect> controls;
 void ApplyState(const State& s){++applies;apply(s);}
 void MarkDirty(){}
 void GoToPage(MenuPage p,int){back=true;target=p;}
 void click(int x,int y){auto& s=state;auto save=[&](){++saves;assert(persist());};auto begin_drag=[&](UiRect r,DragControl d){if(!PtIn(r,x,y))return false;drag=d;return true;};
#define KCD2VR_H8_MENU_CLICK
#include "subsystems/experimental/claude_menu_h8.inl"
#undef KCD2VR_H8_MENU_CLICK
 }
 void DrawButton(int,int,UiRect r,const wchar_t*,int){controls.push_back(r);}
 void DrawSlider(int,int,UiRect r,const wchar_t*,float t,const wchar_t*){assert(t>=0&&t<=1);controls.push_back(r);}
 void Text(int,int,const wchar_t*,int,int,int,int,int,int){}
 void draw(){auto& s=state;int dc=0,font_small_=0;wchar_t v[128]{};auto pageTitle=[](const wchar_t*,const wchar_t*){};auto vrNav=[](){};
#define KCD2VR_H8_MENU_DRAW
#include "subsystems/experimental/claude_menu_h8.inl"
#undef KCD2VR_H8_MENU_DRAW
 }
};
int main(){
 assert(claude_h8::clean());load();Menu m;fill(m.state);assert(m.state.pommel_strike_h8&&!m.state.impale_main_probe_h8&&m.state.kick_assist_h8==0);
 m.click(650,150);assert(!physical::pommelStrikeDefault69&&m.saves==1);
 m.click(650,420);assert(claude_h8::impaleMainProbe&&m.saves==2);
 m.click(650,220);assert(!m.state.kick_real_hit37c2&&m.saves==2); // Retired direct-hit control cannot enable damage.
 m.click(650,320);assert(m.drag==DragControl::KickAssistH8&&m.saves==2);
 m.state.kick_assist_h8=claude_h8::assist(.5f);apply(m.state);persist();assert(kick26::assistCm72==15&&ini["VrKickAssist"]==15);
 physical::pommelStrikeDefault69=true;claude_h8::impaleMainProbe=false;kick26::assistCm72=0;load();fill(m.state);
 assert(!m.state.pommel_strike_h8&&m.state.impale_main_probe_h8&&m.state.kick_assist_h8==15);
 ini["VrKickAssist"]=999;load();assert(kick26::assistCm72==30);ini["VrKickAssist"]=-9;load();assert(kick26::assistCm72==0);
 assert(claude_h8::assist(-1)==0&&claude_h8::assist(2)==30&&claude_h8::assist(NAN)==0);
 m.state.kick_assist_h8=0;m.draw();assert(m.controls.size()==6);m.click(630,80);assert(m.back);m.click(650,360);assert(m.target==MenuPage::Kicks26);
}
