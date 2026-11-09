// Actual callback, entry guard, commit, click routes and draw blocks are
// generated from current source by tools/check_stereo_menu_d82.py.
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
    bool (*stereo_ready)(void*)=nullptr;
};
struct NativeIO {
    bool console=true,modeCvar=true,readOk=true,saveOk=true;
    int actual=0,quality=1,preset=0,reads=0,saveCalls=0,stateSaves=0;
    int displayedMode=-99,persistedMode=3;
    bool pendingSave=false;
    bool dlssReady=false,fsrReady=false;
    std::vector<int> readinessReads;
    std::vector<int> modeReadScript;
    std::vector<std::pair<std::string,int>> saved;
} io;
namespace native_upscaler_stereo_d82 {
// Native aggregate readiness is the only new policy leaf substituted here.
// The source entry guard and UI/backend readbacks remain literal production.
bool CanEnterStereo(int tech) noexcept {
    io.readinessReads.push_back(tech);
    return tech==0 || (tech==1&&io.fsrReady) || (tech==2&&io.dlssReady);
}
}
enum CVar { Mode=1,Quality=2,Preset=3 };
static std::atomic<void*> nativeUpscalerCvarD82{reinterpret_cast<void*>(Mode)};
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
    void RequestMenuSave(bool){++io.stateSaves;io.pendingSave=true;}
    void FlushSettings();
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
    io=NativeIO{};io.persistedMode=mode;drawing.clear();mod.vr_production_stereo_mode_=mode;mod.matrix_running_for_ui_=false;
    mod.InstallCallbacks();mod.menu_callbacks.context=&mod;
    mod.menu_callbacks.read_state=[](void* p,VrMenuState* s){auto* m=static_cast<Mod*>(p);
        s->stereo_mode=io.displayedMode==-99?m->vr_production_stereo_mode_.load():io.displayedMode;
        s->matrix_running=m->matrix_running_for_ui_.load();};
    mod.menu_callbacks.apply_state=[](void* p,const VrMenuState* s){return static_cast<Mod*>(p)->ApplyMenuState(*s);};
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
    // Exact production save callback: all selected native values are preserved
    // for restart. A save never changes the currently loaded native renderer.
    for(int mode:{0,3})for(int tech:{0,1,2,4})for(int quality=0;quality<graphics_menu_options::QualityCount(tech);++quality){
        attach(mod,menu,mode);io.actual=4;
        const bool allowed=mode!=3||tech!=4;
        check(menu.callbacks_.apply_upscaler(&mod,tech,quality,2)==allowed,"Current backend mode authorizes Native/FSR/DLSS; Stereo refuses XeSS");
        check(io.actual==4,"Saving never mutates the live renderer technology");
        check(io.readinessReads.empty(),"Next-start save does not require a currently captured vendor pair");
        if(allowed){
            check(io.saveCalls==1&&saved("r_SuperResolution_Mode")==tech,"Callback saves exact requested technology once");
            check(io.saved.size()==std::size_t(tech==2?3:tech>0?2:1),"Callback writes only the selected technology's owned keys");
            if(tech>0)check(saved(graphics_menu_options::QualityKey(tech))==quality,"Callback preserves every native quality including native-AA variants");
            if(tech==2)check(saved("r_SuperResolution_NVIDIA_DLSS_Preset")==2,"Callback preserves exact DLSS preset");
        }else check(io.saveCalls==0&&io.saved.empty(),"Unsupported Stereo selection does not write config");
    }
    for(int preset=0;preset<=6;++preset){attach(mod,menu);check(menu.callbacks_.apply_upscaler(&mod,2,5,preset),"Stereo DLAA retains all seven DLSS presets");check(saved("r_SuperResolution_NVIDIA_DLSS_Preset")==preset,"DLSS preset mapping is exact");}
    for(int mode:{0,3})for(int tech:{-1,3,9}){attach(mod,menu,mode);check(!menu.callbacks_.apply_upscaler(&mod,tech,0,0)&&io.saveCalls==0,"Invalid technologies fail before filesystem writer");}
    for(int tech:{1,2,4})for(int quality:{-1,graphics_menu_options::QualityCount(tech)}){
        attach(mod,menu,0);check(!menu.callbacks_.apply_upscaler(&mod,tech,quality,0)&&io.saveCalls==0,"Invalid native quality fails without clamping or writing");
    }
    for(int preset:{-1,7}){attach(mod,menu);check(!menu.callbacks_.apply_upscaler(&mod,2,1,preset)&&io.saveCalls==0,"Invalid preset cannot reach writer");}
    attach(mod,menu);check(!menu.callbacks_.apply_upscaler(nullptr,2,1,0)&&io.saveCalls==0,"Null context cannot bypass backend guard");
    attach(mod,menu);io.saveOk=false;check(!menu.callbacks_.apply_upscaler(&mod,2,1,0)&&io.saveCalls==1&&io.saved.empty(),"Actual save failure reaches caller");

    // Home and ViewModes execute the same actual selection path. The user request
    // is saved independently of the actual worker readiness gate.
    for(Page page:{Page::Home,Page::ViewModes}){
        const auto& stereo=page==Page::Home?kHomeModeCorrected:kV4ModeCorrected;
        const auto& mono=page==Page::Home?kHomeModeMono:kV4ModeMono;
        for(int actual:{0,1,2}){
            attach(mod,menu,0);io.actual=actual;io.dlssReady=io.fsrReady=true;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==3&&io.stateSaves==1,"Actual Native or ready captured temporal pair permits Stereo and one mode save");
            check(io.readinessReads==std::vector<int>{actual},"Read-only activation callback checks the actual native technology");
            check(io.actual==actual&&io.saveCalls==0,"Live Stereo entry never edits native config or technology");
        }
        for(int actual:{1,2}){
            attach(mod,menu,0);io.actual=actual;io.quality=graphics_menu_options::QualityCount(actual)-1;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==3&&io.stateSaves==1&&io.pendingSave,"Uncaptured temporal resources still accept and queue the explicit Stereo request");
            check(!WorkerStereoReadyForTest(),"The actual worker readiness operand remains false before twins exist");
            check(menu.active_page_==int(Page::Graphics)&&menu.up_status_==5,"Saved request awaiting initialization explains restart on Graphics");
            VrMenuState state{};state.stereo_mode=0;menu.DrawUpscaler(state);
            check(menu.up_tech_==actual&&menu.up_quality_==io.quality,"First Graphics draw seeds the actual current technology and quality");
            check(menu.up_status_==5,"First Graphics draw preserves the restart explanation after readback");
            check(io.saveCalls==0&&io.actual==actual,"Pending request and explanatory draw leave native graphics config/resources untouched");
            mod.FlushSettings();check(io.persistedMode==3&&!io.pendingSave,"Actual settings-writer line persists Stereo intent instead of leaving Mono0");
            mod.vr_production_stereo_mode_=io.persistedMode; // simulated next launch
            check(stereo_upscaler_d81r2::stereoPolicy(io.persistedMode),"Reloaded saved request enables early vendor capture next launch");
            if(actual==1)io.fsrReady=true;else io.dlssReady=true; // native creation leaf on restart
            check(WorkerStereoReadyForTest(),"Actual worker readiness operand opens only after the restarted vendor pair exists");
        }
        for(int actual:{-1,3,4,9}){
            attach(mod,menu,0);io.actual=actual;io.dlssReady=io.fsrReady=true;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0,"Unknown/unsupported actual native mode cannot enter Stereo");
            check(menu.active_page_==int(Page::Graphics)&&menu.up_status_==4&&menu.up_tech_==0&&menu.up_seeded_,"Unknown/unsupported readback stages an explicit supported choice");
            check(io.readinessReads.empty()&&io.saveCalls==0&&io.actual==actual,"Unsupported renderer is refused before readiness or writer");
        }
        for(int failure=0;failure<4;++failure){
            attach(mod,menu,0);if(failure==0)io.console=false;if(failure==1)io.modeCvar=false;if(failure==2)io.readOk=false;
            if(failure==3)menu.callbacks_.read_upscaler=nullptr;
            click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0&&menu.up_status_==4,"Missing/failed readback cannot authorize Stereo");
        }
        attach(mod,menu,0);io.actual=2;io.modeReadScript={0,2};click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==3&&io.stateSaves==1&&io.reads==3&&menu.up_status_==5,"Supported temporal change after UI Native read is saved, with activation deferred until initialized");
        check(!WorkerStereoReadyForTest(),"Changed but uncaptured temporal mode cannot pass actual worker start gate");
        attach(mod,menu,0);io.actual=4;io.modeReadScript={2,4};io.dlssReady=true;click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==0&&io.stateSaves==0&&io.readinessReads.empty(),"Backend refuses unsupported native mode after earlier valid UI read");
        attach(mod,menu,0);io.actual=1;io.modeReadScript={0,1};io.fsrReady=true;click(menu,page,stereo);
        check(mod.vr_production_stereo_mode_==3&&io.readinessReads==std::vector<int>{1},"Backend accepts a changed supported mode only through that mode's readiness");
        for(int actual:{-1,2,4}){
            attach(mod,menu,0);io.actual=actual;io.displayedMode=3;click(menu,page,stereo);
            check(mod.vr_production_stereo_mode_==(actual==2?3:0)&&io.stateSaves==(actual==2?1:0),"Stale displayed Stereo cannot bypass supported-mode validation, but missing twins do not discard a valid request");
            check(!WorkerStereoReadyForTest(),"Stale displayed Stereo cannot bypass actual runtime readiness");
        }
        attach(mod,menu,3);io.actual=4;click(menu,page,mono);
        check(mod.vr_production_stereo_mode_==0&&io.stateSaves==1&&io.saveCalls==0,"Mono escape remains available regardless of current temporal mode");
    }
    for(int actual:{-1,3,4}){attach(mod,menu,0);io.actual=actual;VrMenuState request{};check(!mod.ApplyMenuState(request)&&mod.vr_production_stereo_mode_==0,"Direct caller cannot bypass actual supported-mode validation");}
    for(int actual:{1,2}){attach(mod,menu,0);io.actual=actual;VrMenuState request{};check(mod.ApplyMenuState(request)&&mod.vr_production_stereo_mode_==3,"Direct caller can retain a supported Stereo request before creation");check(!WorkerStereoReadyForTest(),"Direct request alone never opens the worker start gate");}
    attach(mod,menu,0);io.actual=2;io.dlssReady=true;VrMenuState request{};check(mod.ApplyMenuState(request)&&mod.vr_production_stereo_mode_==3&&WorkerStereoReadyForTest(),"Captured resources separately permit actual activation");
    attach(mod,menu,0);io.actual=2;click(menu,Page::Home,kHomeModeCorrected);click(menu,Page::Home,kHomeModeMono);mod.FlushSettings();
    check(mod.vr_production_stereo_mode_==0&&io.persistedMode==0,"Explicit Mono cancels a queued Stereo request through the existing mode setting");
    attach(mod,menu,0);io.actual=2;menu.callbacks_.stereo_ready=nullptr;click(menu,Page::Home,kHomeModeCorrected);
    check(mod.vr_production_stereo_mode_==3&&io.stateSaves==1&&menu.up_status_==5,"Missing readiness callback cannot claim activation or discard the requested next-start mode");
    attach(mod,menu,0);check(!menu.callbacks_.stereo_ready(nullptr),"Readiness callback requires a valid context");
    attach(mod,menu,0);mod.matrix_running_for_ui_=true;check(!mod.ApplyMenuState(request)&&mod.vr_production_stereo_mode_==0,"Existing active-matrix exclusion remains intact");

    // Production Graphics click handlers stage values only. The expected cycle
    // is an external contract: Native -> FSR -> DLSS -> Native in Stereo.
    constexpr int cycleInputs[]{-1,0,1,2,4,3,99};
    constexpr int nextStereo[]{0,1,2,0,0,0,0},prevStereo[]{2,2,0,1,2,2,2};
    for(unsigned i=0;i<std::size(cycleInputs);++i)for(int direction:{-1,1}){
        attach(mod,menu);menu.up_tech_=cycleInputs[i];menu.up_quality_=5;menu.up_preset_=6;
        click(menu,Page::Graphics,direction<0?kUpTechPrev:kUpTechNext);
        const int expected=direction<0?prevStereo[i]:nextStereo[i];
        check(menu.up_tech_==expected&&menu.up_quality_==graphics_menu_options::DefaultQuality(expected)&&menu.up_preset_==0,"Stereo cycle skips unsupported XeSS and resets only staged per-tech values");
        check(io.saveCalls==0&&io.actual==0,"Technology arrows do not save or change loaded renderer");
    }
    for(int mode:{0,3})for(int tech:{1,2}){
        attach(mod,menu,mode);menu.up_tech_=tech;menu.up_quality_=graphics_menu_options::QualityCount(tech)-1;
        click(menu,Page::Graphics,kUpQualNext);check(menu.up_quality_==0,"Temporal quality next wraps under both production modes");
        click(menu,Page::Graphics,kUpQualPrev);check(menu.up_quality_==graphics_menu_options::QualityCount(tech)-1,"Temporal quality previous wraps under both production modes");
        menu.up_preset_=6;click(menu,Page::Graphics,kUpPresetNext);check(menu.up_preset_==(tech==2?10:6),"Preset arrows change DLSS only (F -> J)");
        if(tech==2){click(menu,Page::Graphics,kUpPresetPrev);check(menu.up_preset_==6,"DLSS preset previous returns J -> F");}
        check(io.saveCalls==0,"Quality/preset staging does not write config");
    }
    attach(mod,menu);menu.up_tech_=0;menu.up_quality_=0;menu.up_preset_=0;
    for(const auto& rect:{kUpQualNext,kUpQualPrev,kUpPresetNext,kUpPresetPrev})click(menu,Page::Graphics,rect);
    check(menu.up_quality_==0&&menu.up_preset_==0&&io.saveCalls==0,"Native quality and preset controls are inert");
    for(int tech:{0,1,2}){
        attach(mod,menu);menu.up_tech_=tech;menu.up_quality_=graphics_menu_options::QualityCount(tech)-1;menu.up_preset_=6;io.actual=4;
        click(menu,Page::Graphics,kUpApply);
        check(menu.up_status_==3&&saved("r_SuperResolution_Mode")==tech&&io.actual==4,"Stereo save supports all three technologies for restart without changing current renderer");
        if(tech>0)check(saved(graphics_menu_options::QualityKey(tech))==graphics_menu_options::QualityCount(tech)-1,"Graphics save includes exact native-AA quality");
    }
    for(int tech:{-1,3,4}){attach(mod,menu);menu.up_tech_=tech;click(menu,Page::Graphics,kUpApply);check(io.saveCalls==0&&menu.up_status_==4,"Unsupported stale staged value is refused at UI boundary");}
    attach(mod,menu,3);io.displayedMode=0;menu.up_tech_=4;menu.up_quality_=2;click(menu,Page::Graphics,kUpApply);
    check(io.saveCalls==0&&menu.up_status_==2,"Current Stereo backend refuses stale Mono UI saving XeSS");
    for(int tech:{1,2}){attach(mod,menu,3);io.displayedMode=0;menu.up_tech_=tech;menu.up_quality_=0;click(menu,Page::Graphics,kUpApply);check(menu.up_status_==3&&saved("r_SuperResolution_Mode")==tech,"Stale displayed mode does not block supported backend save");}
    attach(mod,menu);menu.up_tech_=2;menu.up_quality_=1;io.saveOk=false;click(menu,Page::Graphics,kUpApply);check(menu.up_status_==2&&io.saved.empty(),"Graphics reports actual config IO failure");
    for(int tech:{0,1,2,4})for(int direction:{-1,1}){attach(mod,menu,0);menu.up_tech_=tech;click(menu,Page::Graphics,direction<0?kUpTechPrev:kUpTechNext);check(menu.up_tech_==graphics_menu_options::Next(tech,direction),"Mono technology choices preserve existing behavior");}

    // Exact drawing calls are bounded in existing rows and admit unfinished
    // headset validation. This does not simulate native font rasterization.
    for(int mode:{0,3})for(int status:{0,1,2,3,4,5}){
        attach(mod,menu,mode);menu.up_seeded_=true;menu.up_tech_=2;menu.up_quality_=5;menu.up_status_=status;
        VrMenuState state{};state.stereo_mode=mode;menu.DrawUpscaler(state);menu.DrawViewText();
        bool note=false,saveLabel=false,honest=false;
        for(const auto& draw:drawing){
            check(draw.rect.l>=570&&draw.rect.r<=1070&&draw.rect.t>=0&&draw.rect.b<=672,"Graphics/View text remains in the bounded existing page");
            if(draw.rect.t==604&&draw.rect.l==655)note=draw.text==(status==5?L"RESTART TO INITIALIZE STEREO UPSCALING":status==4?L"SELECT NATIVE / DLSS / FSR":UpQualityName(2,5));
            if(draw.rect.l==kUpApply.l&&draw.rect.t==kUpApply.t)saveLabel=draw.text==(status==3?L"SAVED: RESTART":status==2?L"SAVE FAILED":L"SAVE / RESTART");
            if(draw.rect.t==472)honest=draw.text.find(L"headset test is still needed")!=std::wstring::npos&&draw.text.find(L"XeSS is not supported")!=std::wstring::npos;
        }
        check(note&&saveLabel,"Quality row and save button distinguish restart, selection, IO failure, and native quality");
        check(honest,"View guidance states unfinished headset validation and unsupported XeSS");
    }
    attach(mod,menu);menu.up_status_=5;io.readOk=false;VrMenuState state{};menu.DrawUpscaler(state);
    check(menu.up_status_==5&&menu.up_tech_==-1,"Failed first read also preserves explicit restart explanation");
    attach(mod,menu);io.actual=3;menu.DrawUpscaler(state);check(menu.up_tech_==-1&&menu.up_status_==0,"Unknown renderer readback remains unknown rather than guessed Native");
    std::printf("D82 extracted stereo menu/callback checks=%d failures=%d; actual source, native IO/readiness leaves substituted, headset NOT_RUN\n",checks,failures);
    return failures?1:0;
}
