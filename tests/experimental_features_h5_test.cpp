// Actual Home/hub/nav/click/font/GDI clauses; no native gameplay or D3D queue.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <fstream>
#include <string>
#include <vector>
#include "vr_menu.h"
#include "subsystems/experimental/features_menu_h5.h"
#include "subsystems/experimental/claude_options_h8.h"
#include "immersion_menu45.h"
// H41 menu owner leaves: menu calls are real; these test doubles do not execute gameplay.
namespace kcd2vr::frik_pose_probe {static bool menuKickH41=false;bool FistKickEnabledH35() noexcept{return menuKickH41;}void SetFistKickEnabledH35(bool on) noexcept{menuKickH41=on;}}
namespace kcd2vr::grip_stow_cb1 {static bool menuStowH41=false;bool Enabled() noexcept{return menuStowH41;}void SetEnabled(bool on) noexcept{menuStowH41=on;}}
namespace menu_test_h5 {
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
namespace grip_stow_cb1=kcd2vr::grip_stow_cb1;
namespace experimental_features_h5=kcd2vr::experimental_features_h5;
namespace deepseek_h29=kcd2vr::deepseek_h29;
namespace live_settings_d39=kcd2vr::live_settings_d39;
namespace ef=experimental_features_h5;
namespace combat_menu49=kcd2vr::combat_menu49;
namespace immersion_menu45=kcd2vr::immersion_menu45;
using kcd2vr::VrMenuState;
struct UiRect {int l,t,r,b;};
unsigned checks=0,failed=0;
void check(bool v,const char* what){++checks;if(!v){++failed;std::printf("FAIL %s\n",what);}}
namespace diagnostic_trace {unsigned calls=0;void Write(const char*){++calls;}}
constexpr COLORREF kAccent=RGB(180,150,90),kPanel2=RGB(220,205,170),kText=RGB(53,35,22),kMuted=RGB(103,78,52),kPaperHi=RGB(239,222,178);
struct Painted {UiRect r;std::wstring text;bool control=false,checkbox=false;};
std::vector<Painted> paint;
void Text(HDC,HFONT,const wchar_t*,int,int,int,int,COLORREF,UINT flags=DT_LEFT|DT_VCENTER|DT_SINGLELINE);
void DrawCheck(HDC,HFONT,const UiRect&,bool,const wchar_t*);
void DrawButton(HDC,HFONT,const UiRect&,const wchar_t*,COLORREF color=kPanel2);
#include <experimental_menu_h5_rects.inc>
#include <experimental_menu_h5_render.inc>
void Text(HDC dc,HFONT font,const wchar_t* text,int x,int y,int w,int h,COLORREF color,UINT flags){
    paint.push_back({{x,y,x+w,y+h},text});const auto old=SelectObject(dc,font);
    RECT measured{0,0,w,0};DrawTextW(dc,text,-1,&measured,flags|DT_CALCRECT);
    check(measured.bottom-measured.top<=h,"actual font text fits original draw height");
    if(flags&DT_SINGLELINE){SIZE size{};GetTextExtentPoint32W(dc,text,int(std::wcslen(text)),&size);check(size.cx<=w,"actual font single-line label fits original draw width");}
    SelectObject(dc,old);NativeTextH5(dc,font,text,x,y,w,h,color,flags);
}
void DrawCheck(HDC dc,HFONT f,const UiRect& r,bool on,const wchar_t* text){paint.push_back({r,text,true,true});NativeDrawCheckH5(dc,f,r,on,text);}
void DrawButton(HDC dc,HFONT f,const UiRect& r,const wchar_t* text,COLORREF c){paint.push_back({r,text,true,false});NativeDrawButtonH5(dc,f,r,text,c);}
struct Counts {unsigned apply=0,save=0,recalibrate=0,restore=0,mode=0,safe=0,recenter=0;};
class MenuHarness {
public:
#include <experimental_menu_h5_enum.inc>
    enum class DragControl {None};
    kcd2vr::VrMenu::Callbacks callbacks_{};VrMenuState state{};Counts calls{};
    std::atomic<int> active_page_{int(MenuPage::Home)},page_slide_dir_{1},drag_control_{0};
    std::atomic<unsigned> experimentalGroupH5_{0},liveGroupD39_{0};
    std::atomic<int> liveStatusD39_{0};
    std::atomic<bool> page_animating_{false};std::atomic<std::uint64_t> page_anim_start_ms_{0};
    bool visible=false;unsigned dirty=0;bool refuseF9=false;std::vector<int> f9Requests;
    HFONT font_normal_=nullptr,font_small_=nullptr,font_tiny_=nullptr,font_title_=nullptr,font_hero_=nullptr;
    MenuHarness(){callbacks_.context=this;
        callbacks_.read_state=[](void* p,VrMenuState* out){*out=static_cast<MenuHarness*>(p)->state;};
        callbacks_.apply_state=[](void*p,const VrMenuState*s){auto&m=*static_cast<MenuHarness*>(p);++m.calls.apply;
            m.f9Requests.push_back(s->physical_f9_rung61);
            if(m.refuseF9&&s->physical_f9_rung61){m.state.physical_f9_rung61=0;m.state.physical_f9_next_h6=0;return false;}
            m.state=*s;m.state.physical_f9_next_h6=(s->physical_f9_rung61+1)%4;return true;};
        callbacks_.save_settings=[](void*p){++static_cast<MenuHarness*>(p)->calls.save;return true;};
        callbacks_.request_vrik_recalibrate=[](void*p){++static_cast<MenuHarness*>(p)->calls.recalibrate;};
        callbacks_.request_vrik_restore=[](void*p){++static_cast<MenuHarness*>(p)->calls.restore;};
        callbacks_.set_vrik_mode=[](void*p,bool){++static_cast<MenuHarness*>(p)->calls.mode;};
        callbacks_.request_safe_mono=[](void*p){++static_cast<MenuHarness*>(p)->calls.safe;};
        callbacks_.request_recenter=[](void*p){++static_cast<MenuHarness*>(p)->calls.recenter;};
    }
    ~MenuHarness(){for(auto f:{font_normal_,font_small_,font_tiny_,font_title_,font_hero_})if(f)DeleteObject(f);}
    void fonts(){
#include <experimental_menu_h5_fonts.inc>
    }
    bool IsVisible()const{return visible;}void SetVisible(bool on){visible=on;}
    void ReadState(VrMenuState& out)const{callbacks_.read_state(callbacks_.context,&out);}
    bool ApplyState(const VrMenuState& s){return callbacks_.apply_state(callbacks_.context,&s);}
    void MarkDirty(){++dirty;}
    void GoToPage(MenuPage,int);void UpdatePageAnimation();void CyclePage(int);void ToggleDeepSeekLabH29();
    void settle(){page_anim_start_ms_=GetTickCount64()-300;UpdatePageAnimation();}
    void HandleClick(int x,int y){
#include <experimental_menu_h5_click.inc>
    }
    void click(UiRect r){HandleClick(r.l+1,r.t+1);}
    void click(ef::Rect r){click(EH5(r));}
    void DrawLive(HDC dc){const auto& s=state;
#include <experimental_menu_h5_title.inc>
#include <experimental_menu_h5_live_draw.inc>
    }
    void DrawHub(HDC dc){const auto& s=state;
#include <experimental_menu_h5_title.inc>
#include <experimental_menu_h5_draw.inc>
    }
    void OpenRanged(int x,int y){
#define KCD2VR_MENU_CLICK_KV149MANUALBOWRELOAD
#include "subsystems/ranged_reload/menu.inl"
#undef KCD2VR_MENU_CLICK_KV149MANUALBOWRELOAD
    }
    void DrawRanged(HDC dc){const auto&s=state;
#include <experimental_menu_h5_title.inc>
#include <experimental_menu_h5_ranged_draw.inc>
    }
    void DrawHome(HDC dc){const auto&s=state;
#include <experimental_menu_h5_title.inc>
#include <experimental_menu_h5_home_left_draw.inc>
#include <experimental_menu_h5_home_draw.inc>
    }
};
#include <experimental_menu_h5_methods.inc>
bool allOff(){return !kcd2vr::hand_only_door_h40::displayEnabled.load()&&!kcd2vr::alchemy_own_head_h51::enabled.load()&&!frik_pose_probe::FistKickEnabledH35()&&!kcd2vr::grip_stow_cb1::Enabled()&&!deepseek_h29::angularVignette.load()&&!deepseek_h29::wallBackoutSpringH31.load()&&!deepseek_h29::fingerContactSpringH31.load()&&!deepseek_h29::WeaponNpcContactsEnabledH5();}
void snapshot(const MenuHarness&m,std::array<unsigned char,sizeof(VrMenuState)>& out){std::memcpy(out.data(),&m.state,out.size());}
bool sameState(const MenuHarness&m,const std::array<unsigned char,sizeof(VrMenuState)>& s){return std::memcmp(s.data(),&m.state,s.size())==0;}
void saveBitmap(const std::string& path,const void* bits){BITMAPFILEHEADER file{};file.bfType=0x4D42;file.bfOffBits=sizeof(file)+sizeof(BITMAPINFOHEADER);file.bfSize=file.bfOffBits+1100*680*4;
    BITMAPINFOHEADER info{};info.biSize=sizeof(info);info.biWidth=1100;info.biHeight=-680;info.biPlanes=1;info.biBitCount=32;info.biCompression=BI_RGB;
    std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(&file),sizeof(file));out.write(reinterpret_cast<const char*>(&info),sizeof(info));out.write(static_cast<const char*>(bits),1100*680*4);}
int run(const std::string& output){
    using P=MenuHarness::MenuPage;static_assert(int(P::Experiments)==10&&int(P::DeepSeekLabH29)==23&&int(P::ExperimentalFeaturesH5)==24,"existing menu IDs retained");
    check(!kcd2vr::alchemy_own_head_h51::enabled.load()&&!ef::enabled(ef::Trial::AlchemyEye),"actual alchemy frame owner and Tools card default OFF");
    const auto alchemyCard=ef::cards(ef::Group::Tools)[1];const std::wstring alchemyStatus=alchemyCard.detail;
    check(alchemyCard.trial==ef::Trial::AlchemyEye&&alchemyStatus.find(L"HOST CHECKED")!=alchemyStatus.npos&&alchemyStatus.find(L"headset not run")!=alchemyStatus.npos&&alchemyStatus.find(L"FREE VRIK")!=alchemyStatus.npos&&alchemyStatus.find(L"full ingredient/tool cycle remains unverified")!=alchemyStatus.npos,"actual alchemy card states host/headset, required mode and incomplete-cycle limits");
    check(allOff(),"real shared trials start OFF");check(ef::clean()&&HomeNavigationH5(),"actual Home and category layouts do not overlap");
    MenuHarness m;m.fonts();check(m.state.physical_f9_rung61==0,"existing session F9 state defaults OFF");
    m.state.finger_middle_shift_mm_h31[0]=13.5f;m.state.finger_middle_distal_medial_h31[1]=7.5f;m.state.stereo_mode=3;m.state.calibration_auto_restore43=true;
    m.state.weapon_grip_cm=1.25f;m.state.phys_bodies=1;m.state.comfort_vignette51=2;
    std::array<unsigned char,sizeof(VrMenuState)> baseline{};snapshot(m,baseline);
    const auto savedPreserved=[&](){auto copy=m.state;copy.physical_f9_rung61=0;copy.physical_f9_next_h6=1;return std::memcmp(&copy,baseline.data(),sizeof(copy))==0;};
    m.click(kHomeFeaturesH5);check(m.active_page_==int(P::LiveExperimentsD39)&&allOff()&&sameState(m,baseline),"actual Home live-settings navigation enables nothing and preserves settings");
    m.settle();m.click(ED39(live_settings_d39::kTrials));check(m.active_page_==int(P::ExperimentalFeaturesH5)&&allOff()&&sameState(m,baseline),"live hub retains quick trial navigation without enabling an owner");
    const auto firstDirty=m.dirty;m.click(ef::kCards[0]);check(allOff()&&m.dirty==firstDirty,"actual page animation refuses premature trial clicks");m.settle();
    m.click(ef::kCards[0]);check(deepseek_h29::fingerContactSpringH31.load()&&!deepseek_h29::wallBackoutSpringH31.load(),"actual Hands card reaches existing finger owner only");
    m.click(ef::kCards[1]);check(kcd2vr::grip_stow_cb1::Enabled(),"actual stow menu reaches its owner");
    m.click(ef::kGroups[3]);m.click(ef::kCards[1]);check(frik_pose_probe::FistKickEnabledH35(),"actual native kick menu reaches its owner");
    m.click(ef::kGroups[2]);
    namespace door=kcd2vr::hand_only_door_h40;
    const auto doorCard=ef::cards(ef::Group::Tools)[0];
    check(doorCard.trial==ef::Trial::DoorHandDisplay&&!door::displayEnabled.load(),"door display has a real default-OFF control separate from native Grip operation");
    door::claims[0]=123;door::entered[0]=123;const auto originalSerial=door::service.serial;
    const auto beforeDoor=m.calls;const auto beforeDoorDirty=m.dirty;
    m.click(ef::kCards[0]);
    check(door::displayEnabled.load()&&ef::enabled(ef::Trial::DoorHandDisplay)&&m.dirty>beforeDoorDirty,"actual Tools door click reaches the same display consumer atomic");
    check(door::claims[0]==123&&door::entered[0]==123&&door::service.serial==originalSerial&&m.calls.apply==beforeDoor.apply&&m.calls.save==beforeDoor.save&&sameState(m,baseline),"door display toggle preserves original operation/claim and saved body settings");
    m.click(ef::kCards[0]);check(!door::displayEnabled.load()&&door::claims[0]==123,"same door control disables presentation without releasing the native operation");
    m.click(ef::kCards[0]);m.click(ef::kAllOff);
    check(!door::displayEnabled.load()&&door::claims[0]==123&&door::entered[0]==123,"All Page Trials Off clears display while leaving native Grip owner intact");
    door::claims[0]=0;door::entered[0]=0;
    m.click(ef::kCards[1]);
    check(kcd2vr::alchemy_own_head_h51::enabled.load()&&ef::enabled(ef::Trial::AlchemyEye),"actual Tools alchemy card reaches existing committed-frame option");
    check(sameState(m,baseline)&&m.calls.apply==0&&m.calls.save==0,"alchemy trial toggles only session owner without native action, full settings apply or INI save");
    m.click(ef::kCards[1]);check(!kcd2vr::alchemy_own_head_h51::enabled.load(),"same alchemy card disables its actual owner");
    m.click(ef::kCards[1]);m.click(ef::kAllOff);check(allOff(),"page off revokes kick/stow and alchemy owner");
    m.click(ef::kGroups[0]);m.click(ef::kCards[0]);
    m.click(ef::kGroups[1]);m.click(ef::kCards[2]);check(deepseek_h29::wallBackoutSpringH31.load(),"actual Physics card reaches existing wall owner");
    const auto oldWord=deepseek_h29::ReadWeaponNpcContactControlH5();m.click(ef::kCards[1]);check(deepseek_h29::WeaponNpcContactsEnabledH5()&&deepseek_h29::ReadWeaponNpcContactControlH5()!=oldWord,"actual Physics card reaches immutable interval self-pair API");
    m.click(ef::kGroups[4]);m.click(ef::kCards[0]);check(deepseek_h29::angularVignette.load(),"actual View card reaches existing aperture owner");
    check(sameState(m,baseline)&&m.calls.apply==0&&m.calls.save==0,"four session trial toggles never apply or save persistent settings");
    m.click(ef::kGroups[1]);
    for(int mode:{1,2,3}){m.click(ef::kCards[0]);check(m.state.physical_f9_rung61==mode&&m.f9Requests.back()==mode,"actual F9 row sends the current existing next-rung cursor through ApplyState");check(savedPreserved(),"F9 cycle changes only session cursor/rung");}
    check(m.calls.apply==3&&m.calls.save==0,"F9 requests use existing adapter, not INI save or independent native enable");
    m.click(ef::kAllOff);check(allOff()&&m.state.physical_f9_rung61==0&&m.f9Requests.back()==0&&m.calls.apply==4&&sameState(m,baseline),"actual page OFF revokes trials and uses existing F9 retirement request without saved reset");
    const auto idleApply=m.calls.apply;m.click(ef::kAllOff);check(m.calls.apply==idleApply&&m.calls.save==0,"page OFF while F9 already OFF does not reapply full settings");
    m.refuseF9=true;m.state.physical_f9_next_h6=2;m.click(ef::kCards[0]);check(m.f9Requests.back()==2&&m.state.physical_f9_rung61==0&&m.state.physical_f9_next_h6==0,"rejected native install keeps owner-provided current/next state");
    m.click(ef::kCards[0]);check(m.f9Requests.back()==0&&m.state.physical_f9_rung61==0,"next actual click exits via owner-provided OFF instead of retrying installation");m.refuseF9=false;
    check(sameState(m,baseline)&&m.calls.save==0,"refused F9 cycle preserves stored fields and never saves");
    m.click(ef::kCards[1]);const auto retiredWord=deepseek_h29::ReadWeaponNpcContactControlH5();m.click(ef::kAllOff);m.click(ef::kCards[1]);check(deepseek_h29::WeaponNpcContactsEnabledH5()&&deepseek_h29::ReadWeaponNpcContactControlH5()!=retiredWord,"actual UI OFF/ON cannot reuse an old self-pair interval");m.click(ef::kAllOff);
    m.ToggleDeepSeekLabH29();m.settle();check(m.active_page_==int(P::DeepSeekLabH29)&&allOff(),"existing Shift F12 navigation changes no owner");
    m.click(kDsWallH31);check(deepseek_h29::wallBackoutSpringH31.load(),"legacy actual wall button shares new hub owner");m.click(kDsFeaturesH5);m.settle();check(m.active_page_==int(P::ExperimentalFeaturesH5)&&deepseek_h29::wallBackoutSpringH31.load()&&sameState(m,baseline),"legacy link opens hub without rearming or resetting");
    kcd2vr::claude_h8::impaleMainProbe=true;m.click(ef::kAllOff);check(allOff()&&kcd2vr::claude_h8::impaleMainProbe.load(),"page OFF is scoped to listed owners, not an unrelated legacy experiment or INI reset");
    for(unsigned g=0;g<unsigned(ef::Group::Count);++g){m.click(ef::kGroups[g]);const auto cards=ef::cards(ef::Group(g));
        for(unsigned row=0;row<cards.size();++row)if(cards[row].trial==ef::Trial::None){const auto d=m.dirty,a=m.calls.apply,save=m.calls.save;m.click(ef::kCards[row]);m.click(ef::kDetails[row]);check(allOff()&&m.dirty==d&&m.calls.apply==a&&m.calls.save==save,"status/missing-function rows have no fake toggle or callback");}}
    m.experimentalGroupH5_=999;m.click(ef::kCards[0]);check(deepseek_h29::fingerContactSpringH31.load(),"invalid group bounds to actual Hands without invalid memory");m.click(ef::kAllOff);
    m.click(ef::kCombatLink);check(m.active_page_==int(P::Experiments)&&allOff(),"combat link enables no experiment");m.settle();
    m.active_page_=int(P::Graphics);m.CyclePage(1);check(m.active_page_==int(P::LiveExperimentsD39),"actual page cycle reaches live settings");m.settle();m.CyclePage(1);check(m.active_page_==int(P::ExperimentalFeaturesH5),"actual page cycle reaches quick trials before combat");m.settle();m.CyclePage(1);check(m.active_page_==int(P::Experiments),"actual next cycle reaches existing combat page");m.settle();m.CyclePage(-1);check(m.active_page_==int(P::ExperimentalFeaturesH5),"actual reverse cycle returns to hub");
    m.settle();m.click(kPageBack);m.settle();check(m.active_page_==int(P::LiveExperimentsD39),"quick-trials Back returns to live experiments");m.click(kPageBack);m.settle();m.click(kHomeSave);check(m.calls.save==1&&sameState(m,baseline),"moved SAVE still invokes original callback exactly once");
    m.click(kHomeVrikRecalibrate);m.click(kHomeVrikRestore43);check(m.calls.recalibrate==1&&m.calls.restore==1&&sameState(m,baseline),"existing calibration controls retain original callbacks");m.click(kHomeVrikNative);m.click(kHomeVrikTracked);check(m.calls.mode==2,"existing native/VRIK callbacks remain intact");
    m.click(kHomeExperiments);m.settle();check(m.active_page_==int(P::Experiments)&&allOff(),"lower Home combat entry retains destination");
    m.GoToPage(P::ExperimentalFeaturesH5,1);m.settle();m.experimentalGroupH5_=1;m.state.matrix_running=true;const auto applyBefore=m.calls.apply;m.click(ef::kCards[0]);check(allOff()&&m.calls.safe==1&&m.calls.apply==applyBefore,"actual matrix safety preempts new F9 owner requests");m.state.matrix_running=false;
    m.callbacks_.save_settings=nullptr;m.GoToPage(P::Home,1);m.settle();m.click(kHomeSave);check(m.calls.save==1,"missing SAVE callback remains safe");
    HDC dc=CreateCompatibleDC(nullptr);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=1100;info.bmiHeader.biHeight=-680;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
    void* bits=nullptr;auto bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);const auto old=SelectObject(dc,bitmap);check(dc&&bitmap&&bits,"real GDI offscreen surface available");
    for(unsigned g=0;g<unsigned(ef::Group::Count);++g){std::memset(bits,0xEE,1100*680*4);paint.clear();m.experimentalGroupH5_=g;m.DrawHub(dc);unsigned checksPainted=0;for(const auto&p:paint)if(p.checkbox)++checksPainted;
        check(checksPainted==(g==0?2u:g==1?2u:g==2?2u:g==3?1u:g==4?1u:0u),"actual draw emits only implemented trial checkboxes");
        bool f9Mode=false;for(const auto&p:paint)f9Mode|=p.text.find(L"F9 MODE:")==0;check(f9Mode==(g==1),"F9 is an owner-mode button rather than false collision readiness");saveBitmap(output+"/experimental_group_"+std::to_string(g)+".bmp",bits);}
    std::memset(bits,0xEE,1100*680*4);m.experimentalGroupH5_=1;m.state.physical_f9_rung61=3;paint.clear();m.DrawHub(dc);bool trackedLabel=false;
    for(const auto&p:paint)trackedLabel|=p.text==L"F9 MODE: TRACKED / CORPSE LAB";
    check(trackedLabel,"rung3 names its actual tracked/corpse owner instead of the retired rung2 body");
    saveBitmap(output+"/f9_rung3.bmp",bits);m.state.physical_f9_rung61=0;
    for(int status:{-1,0}){std::memset(bits,0xEE,1100*680*4);paint.clear();m.state.vrik_calibration_persistence_state=status;m.DrawHome(dc);bool feature=false,combat=false,save=false;for(const auto&p:paint){feature|=p.text==L"EXPERIMENTS / LIVE SETTINGS";combat|=p.text==L"COMBAT EXPERIMENTS";save|=p.text==L"SAVE SETTINGS";}
        check(feature&&combat&&save,"actual Home retains SAVE and both experiment entries");saveBitmap(output+"/home_"+std::to_string(status)+".bmp",bits);}
    for(unsigned g=0;g<unsigned(live_settings_d39::Group::Count);++g){std::memset(bits,0xEE,1100*680*4);paint.clear();m.liveGroupD39_=g;m.DrawLive(dc);
        unsigned rows=0;for(const auto& row:live_settings_d39::kRows[g])rows+=row.id!=live_settings_d39::Id::None&&!row.readOnly;
        unsigned buttons=0;for(const auto& painted:paint)buttons+=painted.control;check(buttons==rows+live_settings_d39::kTabs.size()+4,"actual live settings draws eight tabs, links, Back and editable rows; status stays read-only");
        saveBitmap(output+"/live_experiments_"+std::to_string(g)+".bmp",bits);}
    // Actual new ranged module entry/click/draw; this keeps the same native
    // read/apply/save leaves as the existing menu fixture, not native execution.
    MenuHarness ranged;ranged.fonts();ranged.state.finger_middle_shift_mm_h31[0]=13.5f;
    const auto rangedBaseline=ranged.state;
    ranged.OpenRanged(kV149ManualBowReload.l+1,kV149ManualBowReload.t+1);ranged.settle();
    check(ranged.active_page_==int(P::RangedOptions32)&&int(P::RangedOptions32)==28,"actual Immersion ranged entry retains the current page ID after separate fist/guard/defence pages");
    ranged.click(kRangedZoom32);check(!ranged.state.crossbow_no_zoom32&&ranged.calls.apply==1&&ranged.calls.save==1,"actual crossbow camera preference toggle applies/saves once");
    ranged.click(kRangedZoom32);check(ranged.state.crossbow_no_zoom32&&ranged.calls.apply==2&&ranged.calls.save==2,"actual native zoom preference roundtrip");
    ranged.click(kRangedBow32);ranged.click(kRangedBow32);
    check(ranged.state.bow_manual_reload==rangedBaseline.bow_manual_reload&&ranged.calls.apply==4&&ranged.calls.save==4,"existing Bow quiver/auto roundtrip retained on ranged page");
    check(ranged.state.finger_middle_shift_mm_h31[0]==13.5f&&ranged.state.physical_f9_rung61==0,"ranged preference changes preserve fingers and session physics");
    std::memset(bits,0xEE,1100*680*4);paint.clear();ranged.DrawRanged(dc);saveBitmap(output+"/ranged_options_xr32.bmp",bits);
    ranged.click(kPageBack);ranged.settle();check(ranged.active_page_==int(P::Home),"ranged back follows actual shared Home navigation");
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);
    check(m.calls.recenter==0&&allOff()&&m.state.physical_f9_rung61==0,"complete UI test leaves real session trials off without recenter/native activity");
    std::printf("experimental_features_h5=%u/%u native_calls=NONE full_D3D_or_headset=NOT_RUN\n",checks-failed,checks);return failed?1:0;
}
}
int main(int argc,char**argv){if(argc!=2)return 2;return menu_test_h5::run(argv[1]);}
