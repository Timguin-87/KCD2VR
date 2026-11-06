// Host fixture: production reducer + extracted UI, live-owner and INI statements.
#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cstdint>
#include <cwchar>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "vrik/melee_tuning_c54.h"
#include "vrik/thrust_tuning_d67.h"
#include "subsystems/ranged_reload/options_xr32.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "vrik/motion_trials_d67.h"
#include "fresh_uncapped_menu_state.inc"

namespace {
using namespace kcd2vr;
namespace motion=motion_trials_d67;
namespace live=live_settings_d39;
namespace capture=diagnostics::capture168;
struct UiRect {int l,t,r,b;};
#include "fresh_uncapped_menu_geometry.inc"

int failures=0,checks=0;
void check(bool ok,const char* name) {++checks;if(!ok){++failures;std::cout<<"FAIL "<<name<<'\n';}}
std::map<std::string,int> ini;
int ReadIniIntExplicit(const char*,const char* section,const char* key,int fallback) {
    check(std::string(section)=="Settings","INI reader preserves Settings section");
    const auto found=ini.find(key);return found==ini.end()?fallback:found->second;
}
bool wi(const char* key,int value) {ini[key]=value;return true;}
#include "fresh_uncapped_menu_owner.inc"

enum class MenuPage {Home,VrikExperimentsD61,LiveExperimentsD39};
MenuPage page=MenuPage::VrikExperimentsD61;
std::atomic<unsigned> liveGroupD39_{0};
int applies=0,saves=0,dirty=0;
bool ApplyState(const VrMenuState& s) {++applies;ApplyMotion(s);return true;}
bool save() {++saves;return SaveMotion();}
void MarkDirty() {++dirty;}
void GoToPage(MenuPage value,int) {page=value;}
constexpr unsigned kAccent=1,kPanel2=2,kText=3,kMuted=4;
constexpr int dc=0,font_small_=0,font_normal_=0;
constexpr unsigned DT_LEFT=1,DT_WORDBREAK=2;
constexpr unsigned RGB(unsigned r,unsigned g,unsigned b){return r|(g<<8)|(b<<16);}
struct Drawn {UiRect rect;std::wstring label;unsigned color;};
std::vector<Drawn> drawn;
void DrawButton(int,int,const UiRect& rect,const wchar_t* label,unsigned color) {drawn.push_back({rect,label,color});}
void pageTitle(const wchar_t*,const wchar_t*) {}
template<class... T> void Text(T...) {}
#include "fresh_uncapped_menu_actions.inc"

bool same(const UiRect& a,const UiRect& b){return a.l==b.l&&a.t==b.t&&a.r==b.r&&a.b==b.b;}
bool overlaps(const UiRect& a,const UiRect& b){return a.l<b.r&&b.l<a.r&&a.t<b.b&&b.t<a.b;}
constexpr UiRect choices[]{kVrikBaseD61,kVrikTimingD61,kVrikUncappedD67,kVrikElbowD61};
constexpr unsigned modes[]{0,2,4,2}; // legacy elbow button migrates to independent preference

void setSentinels(VrMenuState& s) {
    s.fps_cap_enabled=false;s.fps_cap_target=117.f;s.head_pose_scale=1.23f;
    s.presentation_zoom=.97f;s.render_scale=1.1f;s.live_d39.elbowOutwardD67=.45f;
    s.live_d39.elbowBackD67=.2f;s.live_d39.handSmoothing=true;
    s.free_hand_present_d61=true;s.live_d39.pickupCameraBridgeD55=false;
}
bool sentinels(const VrMenuState& s) {
    return !s.fps_cap_enabled&&s.fps_cap_target==117.f&&s.head_pose_scale==1.23f&&
        s.presentation_zoom==.97f&&s.render_scale==1.1f&&s.live_d39.elbowOutwardD67==.45f&&
        s.live_d39.elbowBackD67==.2f&&s.live_d39.handSmoothing&&s.free_hand_present_d61&&
        !s.live_d39.pickupCameraBridgeD55;
}

void classic() {
    const UiRect all[]{kVrikLeadD61,kVrikBaseD61,kVrikTimingD61,kVrikUncappedD67,kVrikElbowD61,kPickupCameraD65,kRepairTrialsD65};
    for(unsigned i=0;i<std::size(all);++i){
        check(all[i].l>=570&&all[i].r<=1070&&all[i].t>=100&&all[i].b<=672,"classic button stays in page bounds");
        for(unsigned j=i+1;j<std::size(all);++j)check(!overlaps(all[i],all[j]),"classic hitboxes do not overlap");
    }
    for(unsigned choice=0;choice<4;++choice){
        // Actual hit-test includes top/left and excludes bottom/right.
        const auto r=choices[choice];
        const std::array<std::array<int,2>,3> points{{{r.l,r.t},{(r.l+r.r)/2,(r.t+r.b)/2},{r.r-1,r.b-1}}};
        for(const auto& p:points){
            auto s=Fill();setSentinels(s);applies=saves=dirty=0;
            page=MenuPage::VrikExperimentsD61;
            ClickClassic(s,p[0],p[1]);
            check(s.vrik_comparison_d61==int(choice)&&s.live_d39.motionTrialD67==int(choice==3?3:modes[choice]),"legacy click selects requested mode");
            check(motion::MenuMode()==modes[choice]&&ini["VrMotionTrialD67"]==int(modes[choice]),"classic click applies and persists exact mode");
            check(applies==1&&saves==1&&dirty==1,"classic click applies/saves/invalidates once");
            check(sentinels(s),"classic selection preserves caps, calibration and other options");
            const auto reread=Fill();
            check(reread.vrik_comparison_d61==int(choice==3?1:choice)&&reread.live_d39.motionTrialD67==int(modes[choice]),"owner reread preserves canonical display and runtime mapping");
            if(choice==3)check(motion::ElbowPreferenceR92(),"legacy elbow choice preserves independent elbow preference");
            drawn.clear();DrawClassic(reread);
            unsigned selected=0;
            for(const auto& b:drawn){
                const bool timing=std::any_of(std::begin(choices),std::end(choices),[&](const UiRect& rect){return same(b.rect,rect);});
                if(timing&&b.color==kAccent){++selected;check(same(b.rect,choice==3?choices[1]:r),"draw highlights canonical timing hitbox");}
                if(same(b.rect,kVrikUncappedD67))check(b.label==L"Fresh Pose Uncapped","uncapped button has requested label");
            }
            check(selected==1,"draw has one selected timing mode");
        }
        for(const auto p:std::array<std::array<int,2>,2>{{{r.r,(r.t+r.b)/2},{(r.l+r.r)/2,r.b}}}){
            auto s=Fill();applies=saves=dirty=0;ClickClassic(s,p[0],p[1]);
            check(applies==0&&saves==0,"classic boundary outside a button does not apply");
        }
    }
    auto s=Fill();const bool pickup=s.live_d39.pickupCameraBridgeD55;
    ClickClassic(s,(kPickupCameraD65.l+kPickupCameraD65.r)/2,(kPickupCameraD65.t+kPickupCameraD65.b)/2);
    check(s.live_d39.pickupCameraBridgeD55!=pickup,"pickup toggle remains reachable");
    page=MenuPage::VrikExperimentsD61;
    ClickClassic(s,(kRepairTrialsD65.l+kRepairTrialsD65.r)/2,(kRepairTrialsD65.t+kRepairTrialsD65.b)/2);
    check(page==MenuPage::LiveExperimentsD39&&liveGroupD39_==unsigned(live::Group::MotionD67),"motion tuning link remains reachable");
}

void liveMenu() {
    page=MenuPage::VrikExperimentsD61;RedirectLegacy(page);
    check(page==MenuPage::LiveExperimentsD39&&liveGroupD39_==unsigned(live::Group::Hands),"legacy navigation opens shipped Hands settings");
    unsigned selected=unsigned(live::Group::Hands);
    const auto& rows=live::rows(selected);
    unsigned row=0;while(row<rows.size()&&rows[row].id!=live::Id::MotionTrialD67)++row;
    check(row<rows.size(),"Hands page exposes timing row");if(row==rows.size())return;
    check(!rows[row].session&&!rows[row].readOnly,"timing choice persists and is editable");
    const auto r=live::rowRect(row);
    motion::SetMenuMode(0);auto s=Fill();setSentinels(s);
    const bool elbowBefore=motion::ElbowPreferenceR92();
    for(unsigned expected:{2u,4u,0u}){
        applies=saves=0;
        const auto result=live::dispatch(selected,(r.l+r.r)/2,(r.t+r.b)/2,s,ApplyState,save);
        check(result==live::Click::Changed&&applies==1&&saves==1,"live row dispatch applies and saves");
        check(s.live_d39.motionTrialD67==int(expected)&&motion::MenuMode()==expected&&ini["VrMotionTrialD67"]==int(expected),"live row cycles baseline/capped/uncapped");
        check(motion::ElbowPreferenceR92()==elbowBefore,"timing row preserves independent elbow preference");
        check(sentinels(s),"live timing cycle preserves unrelated controls");
        const auto reread=Fill();
        check(reread.vrik_comparison_d61==int(motion::MenuChoiceForMode(expected)),"classic display follows live row changes");
        wchar_t label[160]{};live::label(reread,rows[row],label,std::size(label));
        const wchar_t* expectedLabel=expected==2?L"FRESH POSE CAPPED":expected==4?L"FRESH POSE UNCAPPED":L"BASELINE";
        check(std::wstring(label)==std::wstring(rows[row].title)+L": "+expectedLabel,"live row describes correct timing mode");
    }
    for(int invalid:{-99,-1,1,5,99,INT_MAX}){
        s.live_d39.motionTrialD67=invalid;live::advance(s,live::Id::MotionTrialD67);
        check(s.live_d39.motionTrialD67==0,"invalid reducer states preserve baseline fallback");
    }
}

void persistence() {
    ini.clear();motion::SetMenuMode(4);LoadMotion();
    check(motion::MenuMode()==4&&VrMenuState{}.live_d39.motionTrialD67==4,"R124: missing key and state default = fresh pose uncapped 4 (Miki's INI)");
    for(unsigned mode:modes){
        motion::SetMenuMode(mode);check(SaveMotion(),"production INI writer succeeds");
        motion::SetMenuMode(mode==0?2:0);LoadMotion();
        check(motion::MenuMode()==mode,"INI write/reload roundtrip preserves every explicit mode");
    }
    for(int invalid:{INT_MIN,-99,-1,1,5,99,INT_MAX}){
        ini["VrMotionTrialD67"]=invalid;LoadMotion();
        check(motion::MenuMode()==unsigned(invalid>=5?2:0),"invalid INI values retain canonical migrated clamp fallback");
    }
    for(unsigned bad:{1u,5u,99u,UINT_MAX}){
        motion::SetMenuMode(bad);const auto s=Fill();
        check(s.vrik_comparison_d61==0&&s.live_d39.motionTrialD67==0,"invalid live owner request remains baseline");
    }
}

void diagnosticIndependence() {
    constexpr std::uint64_t now=1000;
    capture::gate.select(capture::Domain::Vrik,true,now);
    for(unsigned rung:{0u,1u,2u,3u}){
        motion::SetMode(rung);ini["VrMotionTrialD67"]=4;LoadMotion();
        check(motion::MenuMode()==4&&motion::UncappedSelectedAt(now+1)&&!motion::TimingSelectedAt(now+1),"F7 rung cannot replace persistent uncapped timing");
        auto s=Fill();check(s.vrik_comparison_d61==2&&s.live_d39.motionTrialD67==4,"F7 leaves menu uncapped selection visible");
    }
    motion::SetMode(0);capture::gate.select(capture::Domain::Vrik,false,now+2);
    check(motion::UncappedSelectedAt(now+3),"ending diagnostic session preserves uncapped choice");
}
}

int main(){classic();liveMenu();persistence();diagnosticIndependence();
    std::cout<<(failures?"FAIL ":"PASS ")<<checks<<" checks, "<<failures<<" failures; portable source-connected UI/INI fixture, no headset run\n";
    return failures?1:0;
}
