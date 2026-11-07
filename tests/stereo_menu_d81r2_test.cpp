// Actual callback, entry guard, commit, click routes and draw blocks are
// generated from current source by tools/check_stereo_menu_d81r2.py.
#include "graphics_menu_options.h"
#include "graphics_owner31.h"
#include "stereo_upscaler_d81r2.h"
#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <string>
#include <utility>
#include <vector>

namespace kcd2vr {
namespace developer_mode {bool Enabled() noexcept {return false;}}
namespace diagnostic_trace {void Write(const char*) noexcept {}}
static std::uint64_t GetTickCount64() noexcept {return 100;}
struct UiRect {int l,t,r,b;};
#include "actual_rects.inc"
constexpr int RGB(int r,int g,int b){return r|(g<<8)|(b<<16);}
constexpr int DT_CENTER=1,DT_VCENTER=2,DT_SINGLELINE=4,DT_WORDBREAK=8;
constexpr int kPanel2=1,kAccent=2,kText=3,kMuted=4;
struct Draw {std::wstring text;UiRect rect;};
static std::vector<Draw> drawing;
static void DrawButton(int,int,const UiRect& r,const wchar_t* text,int){drawing.push_back({text,r});}
static void Text(int,int,const wchar_t* text,int x,int y,int w,int h,int,int){drawing.push_back({text,{x,y,x+w,y+h}});}

struct VrMenuState {int stereo_mode=3,cyclopean_mode=0,presentation_mode=3;bool matrix_running=false,diagnostic_rendering=false;};
struct Callbacks {
    void* context=nullptr;
    void (*read_state)(void*,VrMenuState*)=nullptr;
    bool (*apply_state)(void*,const VrMenuState*)=nullptr;
    bool (*save_settings)(void*)=nullptr;
    void (*request_safe_mono)(void*)=nullptr;
    bool (*read_upscaler)(void*,int*,int*,int*)=nullptr;
    bool (*apply_upscaler)(void*,int,int,int)=nullptr;
};
struct NativeIO {
    bool console=true,modeCvar=true,readOk=true,saveOk=true;
    int actual=0,quality=1,preset=0,reads=0,saveCalls=0,stateSaves=0;
    int displayedMode=-99;
    std::vector<int> modeReadScript;
    std::vector<std::pair<std::string,int>> saved;
} io;
enum CVar { Mode=1,Quality=2,Preset=3 };
static void* SafeGetCryConsole(){return io.console?&io:nullptr;}
static void* SafeConsoleGetCVar(void*,const char* name){
    if(std::strcmp(name,"r_SuperResolution_Mode")==0)return io.modeCvar?reinterpret_cast<void*>(Mode):nullptr;
    return reinterpret_cast<void*>(std::strstr(name,"Preset")?Preset:Quality);
}
static int SafeCVarGetInt(void* cvar,int fallback){
    if(reinterpret_cast<std::uintptr_t>(cvar)==Mode){
        ++io.reads;if(!io.readOk)return fallback;
        if(!io.modeReadScript.empty()){const int value=io.modeReadScript.front();io.modeReadScript.erase(io.modeReadScript.begin());return value;}
        return io.actual;
    }
    return reinterpret_cast<std::uintptr_t>(cvar)==Preset?io.preset:io.quality;
}
static bool SaveGraphicsSettingsToUserCfgR27(const std::vector<std::pair<std::string,int>>& values){
    ++io.saveCalls;if(!io.saveOk)return false;io.saved=values;return true;
}
class Mod {
public:
    std::atomic<int> vr_production_stereo_mode_{3};
    std::atomic<bool> matrix_running_for_ui_{false};
    Callbacks menu_callbacks{};
    bool ApplyMenuState(const VrMenuState&);
    void InstallCallbacks();
};
#include "actual_mod.inc"

class VrMenu {
public:
#include "actual_menu_enum.inc"
    enum class DragControl {None};
    Callbacks callbacks_{};
    std::atomic<bool> page_animating_{false},dirty_{false},up_seeded_{false};
    std::atomic<int> active_page_{0},page_slide_dir_{0},drag_control_{0};
    std::atomic<std::uint64_t> page_anim_start_ms_{0};
    std::atomic<int> up_tech_{-1},up_quality_{1},up_preset_{0},up_status_{0};
    int font_small_=11,font_title_=20;
    static bool IsDeveloperPage(MenuPage){return false;}
    void ReadState(VrMenuState&) const;
    bool ApplyState(const VrMenuState&);
    void MarkDirty();
    void GoToPage(MenuPage,int);
    void HandleClick(int,int);
    void DrawUpscaler(const VrMenuState&);
    void DrawViewText();
};
#include "actual_menu.inc"
#include "actual_draw.inc"
}
using namespace kcd2vr;
static int checks=0,failures=0;
static void check(bool ok,const char* message){++checks;if(!ok){++failures;std::printf("FAIL %s\n",message);}}
static void attach(Mod& mod,VrMenu& menu,int mode=3){
    io=NativeIO{};drawing.clear();mod.vr_production_stereo_mode_=mode;mod.matrix_running_for_ui_=false;
    mod.InstallCallbacks();mod.menu_callbacks.context=&mod;
    mod.menu_callbacks.read_state=[](void* p,VrMenuState* s){auto* m=static_cast<Mod*>(p);
        s->stereo_mode=io.displayedMode==-99?m->vr_production_stereo_mode_.load():io.displayedMode;
        s->matrix_running=m->matrix_running_for_ui_.load();};
    mod.menu_callbacks.apply_state=[](void* p,const VrMenuState* s){return static_cast<Mod*>(p)->ApplyMenuState(*s);};
    mod.menu_callbacks.save_settings=[](void*){++io.stateSaves;return true;};
    menu.callbacks_=mod.menu_callbacks;menu.page_animating_=false;menu.active_page_=0;
    menu.up_tech_=-1;menu.up_quality_=1;menu.up_preset_=0;menu.up_status_=0;menu.up_seeded_=false;
}
static void click(VrMenu& menu,VrMenu::MenuPage page,const UiRect& rectangle){
    menu.active_page_=int(page);menu.page_animating_=false;
    menu.HandleClick((rectangle.l+rectangle.r)/2,(rectangle.t+rectangle.b)/2);
}
static int saved(const char* key){for(const auto& value:io.saved)if(value.first==key)return value.second;return -99;}
int main(){
    using Page=VrMenu::MenuPage;Mod mod;VrMenu menu;
    // Exact save callback validates its current backend mode even without UI.
    for(int tech:{1,2,4})for(int quality=0;quality<graphics_menu_options::QualityCount(tech);++quality){
        attach(mod,menu);io.actual=tech;
        check(!menu.callbacks_.apply_upscaler(&mod,tech,quality,0),"Stereo callback rejects every temporal quality, including DLAA/native-AA variants");
        check(io.saveCalls==0&&io.saved.empty()&&io.actual==tech,"Rejected callback has no config or live renderer write");
        attach(mod,menu,0);io.actual=0;
        check(menu.callbacks_.apply_upscaler(&mod,tech,quality,2),"Mono callback preserves all native technology qualities");
        check(saved("r_SuperResolution_Mode")==tech&&io.actual==0,"Mono saves selected next-start mode without live writes");
        check(saved(graphics_menu_options::QualityKey(tech))==quality,"Mono preserves exact selected quality");
    }
    attach(mod,menu);io.actual=2;
    check(menu.callbacks_.apply_upscaler(&mod,0,0,0),"Stereo allows Native next-start save");
    check(io.saved.size()==1&&saved("r_SuperResolution_Mode")==0&&io.actual==2,"Native writes exactly mode0 and leaves current temporal resources intact");
    attach(mod,menu);check(!menu.callbacks_.apply_upscaler(nullptr,2,1,0)&&io.saveCalls==0,"Null context cannot bypass backend Stereo guard");
    attach(mod,menu);io.saveOk=false;
    check(!menu.callbacks_.apply_upscaler(&mod,0,0,0)&&io.saveCalls==1&&io.saved.empty(),"Native save reports actual IO failure");
    for(int tech:{-1,3,9}){attach(mod,menu,0);check(!menu.callbacks_.apply_upscaler(&mod,tech,0,0)&&io.saveCalls==0,"Mono still rejects invalid technologies");}

    // Literal Home and ViewModes click routes preserve live mode and send the
    // user to an explicit next-start Native repair when readback is unsafe.
    for(Page page:{Page::Home,Page::ViewModes}){
        const auto& stereo=page==Page::Home?kHomeModeCorrected:kV4ModeCorrected;
        const auto& mono=page==Page::Home?kHomeModeMono:kV4ModeMono;
        for(int actual:{1,2,4}){
            attach(mod,menu,0);io.actual=actual;io.quality=5;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0,"Unsupported actual upscaler does not enter/save Stereo");
            check(menu.active_page_==int(Page::Graphics)&&menu.up_status_==4&&menu.up_tech_==0&&menu.up_seeded_,"Blocked Stereo selection stages Native repair on Graphics");
            check(io.actual==actual&&io.saveCalls==0,"Stereo selection never switches live upscaler or implicitly saves config");
        }
        for(int failure=0;failure<4;++failure){
            attach(mod,menu,0);if(failure==0)io.console=false;if(failure==1)io.modeCvar=false;if(failure==2)io.readOk=false;
            if(failure==3)menu.callbacks_.read_upscaler=nullptr;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0&&menu.up_status_==4,"Missing/failed upscaler readback cannot authorize Stereo");
        }
        attach(mod,menu,0);io.actual=0;click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==3&&io.stateSaves==1,"Actual Native allows Home/View Stereo selection and one save");
        check(io.actual==0&&io.saveCalls==0,"Compatible Stereo entry does not edit upscaler");
        attach(mod,menu,0);io.actual=2;io.modeReadScript={0,2};click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0,"Backend re-read rejects actual upscaler changing after UI Native read");
        check(io.reads==2&&io.actual==2&&io.saveCalls==0,"Race path performs backend read and no native/config mutation");
        attach(mod,menu,0);io.actual=2;io.displayedMode=3;click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0,"Stale UI Stereo state cannot skip backend mono-to-stereo guard");
        attach(mod,menu,3);io.actual=2;click(menu,page,mono);
        check(mod.vr_production_stereo_mode_==0&&io.stateSaves==1,"Mono escape remains available even with actual temporal mode");
    }
    for(int actual:{-1,1,2,4}){
        attach(mod,menu,0);io.actual=actual;VrMenuState request{};
        check(!mod.ApplyMenuState(request)&&mod.vr_production_stereo_mode_==0,"Direct ApplyMenuState callers cannot bypass actual renderer guard");
    }

    // Actual Graphics controls, with stale UI/backend boundary interleavings.
    for(int tech:{-1,0,1,2,4})for(const auto& button:{kUpTechPrev,kUpTechNext}){
        attach(mod,menu);menu.up_tech_=tech;menu.up_quality_=5;menu.up_preset_=6;
        click(menu,Page::Graphics,button);
        check(menu.up_tech_==0&&menu.up_quality_==0&&menu.up_preset_==0&&io.saveCalls==0,"Stereo tech cycling always stages Native without writing");
    }
    for(const auto& button:{kUpQualPrev,kUpQualNext,kUpPresetPrev,kUpPresetNext}){
        attach(mod,menu);menu.up_tech_=2;menu.up_quality_=5;menu.up_preset_=4;
        click(menu,Page::Graphics,button);
        check(menu.up_quality_==5&&menu.up_preset_==4&&io.saveCalls==0,"Stereo disables temporal quality/preset edits");
    }
    for(int tech:{1,2,4}){
        attach(mod,menu);menu.up_tech_=tech;menu.up_quality_=graphics_menu_options::QualityCount(tech)-1;
        click(menu,Page::Graphics,kUpApply);
        check(io.saveCalls==0&&menu.up_status_==4,"Stereo stale staged temporal Apply is blocked in the UI");
        attach(mod,menu,3);io.displayedMode=0;menu.up_tech_=tech;menu.up_quality_=graphics_menu_options::DefaultQuality(tech);
        click(menu,Page::Graphics,kUpApply);
        check(io.saveCalls==0&&menu.up_status_==2,"Callback current Stereo blocks stale Mono UI Apply");
    }
    attach(mod,menu);menu.up_tech_=0;menu.up_quality_=0;io.actual=2;
    click(menu,Page::Graphics,kUpApply);
    check(menu.up_status_==3&&saved("r_SuperResolution_Mode")==0&&io.actual==2,"Graphics Native save reports restart and preserves live mode");
    attach(mod,menu);menu.up_tech_=0;io.saveOk=false;click(menu,Page::Graphics,kUpApply);
    check(menu.up_status_==2&&io.saved.empty(),"Graphics displays actual Native save failure");
    attach(mod,menu,0);menu.up_tech_=2;menu.up_quality_=5;menu.up_preset_=6;
    click(menu,Page::Graphics,kUpApply);
    check(menu.up_status_==3&&saved("r_SuperResolution_Mode")==2&&saved("r_SuperResolution_NVIDIA_DLSS_QualityMode")==5&&saved("r_SuperResolution_NVIDIA_DLSS_Preset")==6,"Mono Graphics retains DLAA and exact preset save");
    for(int tech:{0,1,2,4})for(int direction:{-1,1}){
        attach(mod,menu,0);menu.up_tech_=tech;
        click(menu,Page::Graphics,direction<0?kUpTechPrev:kUpTechNext);
        check(menu.up_tech_==graphics_menu_options::Next(tech,direction),"Mono technology cycling remains unchanged");
    }
    attach(mod,menu,0);menu.up_tech_=2;menu.up_quality_=5;menu.up_preset_=6;
    click(menu,Page::Graphics,kUpQualNext);check(menu.up_quality_==0,"Mono quality wrapping unchanged");
    click(menu,Page::Graphics,kUpPresetNext);check(menu.up_preset_==0,"Mono preset wrapping unchanged");

    // Literal drawing calls: bounded existing rows, no new footer. Native font
    // rasterization is intentionally not imitated by a second UI implementation.
    for(int mode:{0,3})for(int status:{0,3,4}){
        attach(mod,menu,mode);menu.up_seeded_=true;menu.up_tech_=0;menu.up_quality_=0;menu.up_status_=status;
        VrMenuState state{};state.stereo_mode=mode;menu.DrawUpscaler(state);menu.DrawViewText();
        for(const auto& draw:drawing)check(draw.rect.l>=570&&draw.rect.r<=1070&&draw.rect.t>=0&&draw.rect.b<=672,"Actual Graphics/View drawing stays in original bounded page");
        bool note=false;for(const auto& draw:drawing)if(draw.rect.t==604){
            note=draw.text==(status==4?L"SELECT NATIVE, SAVE, RESTART":mode==3?L"DLSS / FSR / XeSS: MONO ONLY":UpQualityName(0,0));
        }
        check(note,"Existing quality row shows compatible mode/restart explanation");
    }
    std::printf("D81R2 extracted stereo menu/callback %d/%d; actual production slices, native IO mocked, headset NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
