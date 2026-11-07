// Owned base startup UI; included once by dllmain.cpp. No game/native hooks.
#include <windows.h>
#include "core/config_file_guard_d62.h" // D62: before main-INI pending merge
#include "core/default_ini_r118.h" // R118: built-in default KCD2VR.ini, written only when missing
#include "controller_layout_r118.h" // R118: Controller layout choice (OPENXR / STEAMVR / PS VR2)
#include "startup_setup_s15.h"
#include "startup_graphics_d80.h"
#include "view_defaults_b2.h"
#include "vrik/gesture_talk_policy_d33.h" // D33: headset preset -> prompt glyph family
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cwchar>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <shellapi.h>
namespace kcd2vr::startup_s15 {
namespace fs=std::filesystem;
struct Field {const wchar_t* label;const wchar_t* key;const char* defaultKey;double low,high,fallback;bool integer;};
static constexpr Field fields[]={
 {L"OpenXR render scale (0.6 - 1.3x)",L"VrRenderScale","VrRenderScale",.6,1.3,1,false},
 {L"Custom game source width",L"VrSourceWidth","VrSourceWidth",640,4096,1920,true},
 {L"Custom game source height",L"VrSourceHeight","VrSourceHeight",640,4096,1800,true},
 {L"Game + VR FOV",L"VrGameFov","VrGameFov",70,130,95,false},
 {L"VR FOV (linked)",L"VrSourceVerticalFovDegrees","VrSourceVerticalFovDegrees",70,130,95,false},
 {L"Immersive zoom",L"VrPresentationZoom","VrPresentationZoom",.5,2,1,false},
 {L"Theater / flat-screen scale",L"VrFlatScreenScale","VrFlatScreenScale",.3,2,1.05,false}
};
// Panel-native pixels per eye, not the runtime's lens-corrected render target.
// Sources: developers.meta.com/vr/essentials/render-scale/;
// playstation.com/en-sg/ps-vr2/ps-vr2-tech-specs/;
// store.steampowered.com/hardware/steamframe?l=german
struct Headset {const wchar_t* id;const wchar_t* label;int width,height;};
static constexpr Headset headsets[]={
 {L"custom",L"Other / custom headset",0,0},
 {L"quest1",L"Oculus Quest 1",1440,1600},
 {L"quest2",L"Meta Quest 2",1832,1920},
 {L"quest3",L"Meta Quest 3",2064,2208},
 {L"quest3s",L"Meta Quest 3S",1832,1920},
 {L"questpro",L"Meta Quest Pro",1800,1920},
 {L"psvr2",L"Sony PlayStation VR2",2000,2040},
 {L"steamframe",L"Valve Steam Frame",2160,2160}
};
static constexpr int headsetControl=210,resolutionControl=211,upscalerControl=212,qualityControl=213,graphicsControl=214,developerControl=215;
static constexpr int layoutControl=216,layoutInfoControl=217,iniStatusControl=218,createIniControl=219; // R118
static int HeadsetIndex(const wchar_t* id){for(int i=0;i<int(std::size(headsets));++i)if(std::wcscmp(id,headsets[i].id)==0)return i;return 0;}
static int ReadHeadset(const fs::path& ini){wchar_t id[64]{};GetPrivateProfileStringW(L"Settings",L"StartupHeadset",L"custom",id,64,ini.c_str());return HeadsetIndex(id);}
static bool PresetSize(int headset,int row,int& width,int& height){
 if(headset<=0||headset>=int(std::size(headsets))||row<1||row>8)return false;
 const int tenths=row+5;width=(headsets[headset].width*tenths+5)/10;height=(headsets[headset].height*tenths+5)/10;return true;
}
static std::wstring PresetLabel(int headset,int row){int width=0,height=0;if(!PresetSize(headset,row,width,height))return L"Custom (use width / height below)";
 wchar_t label[128]{};swprintf_s(label,L"%.1fx - %d x %d%s",double(row+5)/10,width,height,row==5?L" (native per eye)":L" per eye");return label;
}
struct State {fs::path ini,root;int layout=-1; // R118: VrControllerLayoutR118 chosen here, -1 = not part of this commit
 bool launcher=false,done=false,start=false,balanced=true,updatingPreset=false;
 bool developer=false,hasLaunchOptions=false;int upscaler=-1,quality=0,graphics=-1,productionMode=3;
 int headset=0,scrollY=0,wheelDelta=0;HFONT font=nullptr;std::vector<std::wstring> initial,values;};
static constexpr int contentHeight=916; // R118: + controller layout row, its info lines and the default INI row
static constexpr DWORD setupStyle=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VSCROLL;
// Use the caller's existing DPI coordinate space; never change process-wide
// awareness in an injected DLL. Short work areas get the same scrollable form.
static RECT FitWindow(RECT work){
 RECT frame{0,0,600,contentHeight};AdjustWindowRect(&frame,setupStyle,FALSE);
 const int width=std::min(int(frame.right-frame.left),std::max(1,int(work.right-work.left)-16));
 const int height=std::min(int(frame.bottom-frame.top),std::max(1,int(work.bottom-work.top)-16));
 const int x=work.left+(work.right-work.left-width)/2,y=work.top+(work.bottom-work.top-height)/2;
 return {x,y,x+width,y+height};
}
static void ScrollTo(HWND w,State& s,int requested){
 SCROLLINFO info{sizeof(info),SIF_POS};info.nPos=requested;SetScrollInfo(w,SB_VERT,&info,TRUE);
 const int next=GetScrollPos(w,SB_VERT),delta=s.scrollY-next;s.scrollY=next;
 if(delta)ScrollWindowEx(w,0,delta,nullptr,nullptr,nullptr,nullptr,SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE);
}
static void ConfigureScroll(HWND w,State& s){
 RECT client{};GetClientRect(w,&client);SCROLLINFO info{sizeof(info),SIF_RANGE|SIF_PAGE};
 info.nMin=0;info.nMax=contentHeight-1;info.nPage=std::max(1L,client.bottom-client.top);
 SetScrollInfo(w,SB_VERT,&info,TRUE);ScrollTo(w,s,s.scrollY);
}
static void RevealControl(HWND w,State& s,HWND control){
 if(!control||!IsChild(w,control))return;
 RECT child{},client{};GetWindowRect(control,&child);MapWindowPoints(nullptr,w,reinterpret_cast<POINT*>(&child),2);GetClientRect(w,&client);
 if(child.top<8)ScrollTo(w,s,s.scrollY+child.top-8);
 else if(child.bottom>client.bottom-8)ScrollTo(w,s,s.scrollY+child.bottom-client.bottom+8);
}
static fs::path ModulePath(HMODULE module){wchar_t b[32768]{};auto n=GetModuleFileNameW(module,b,32768);return n&&n<32768?fs::path(b):fs::path{};}
static fs::path GameRoot(fs::path bin){auto p=bin;for(int i=0;i<5&&!p.empty();++i,p=p.parent_path())if(fs::is_directory(p/L"Data")&&fs::is_directory(p/L"Bin"))return p;return {};}
static bool Parse(const std::wstring& text,const Field& f,double& value){wchar_t* end=nullptr;value=std::wcstod(text.c_str(),&end);while(end&&iswspace(*end))++end;return end!=text.c_str()&&end&&!*end&&std::isfinite(value)&&value>=f.low&&value<=f.high&&(!f.integer||std::floor(value)==value);}
static std::wstring Read(const fs::path& ini,const Field& f){wchar_t fallback[64]{},b[256]{};swprintf_s(fallback,L"%.6g",view_defaults_b2::Get(f.defaultKey,f.fallback));GetPrivateProfileStringW(L"Settings",f.key,fallback,b,256,ini.c_str());return b;}
static bool ReadBool(const fs::path& ini,const wchar_t* key,bool fallback){
 wchar_t value[16]{};GetPrivateProfileStringW(L"Settings",key,fallback?L"true":L"false",value,16,ini.c_str());
 return _wcsicmp(value,L"true")==0||wcscmp(value,L"1")==0;
}
static int ProductionMode(const fs::path& ini){
 // Missing/invalid values use the same production-stereo default as the mod.
 wchar_t text[32]{};GetPrivateProfileStringW(L"Settings",L"VrStereoExperiment",L"3",text,32,ini.c_str());
 wchar_t* end=nullptr;const auto value=std::wcstol(text,&end,10);while(end&&iswspace(*end))++end;
 return end!=text&&end&&!*end&&value==0?0:3;
}
static bool ReadSource(const State& s,std::vector<std::string>& lines){
 lines.clear();if(s.root.empty())return false;
 const auto cfg=s.root/L"user.cfg";std::ifstream in(cfg,std::ios::binary);
 if(fs::exists(cfg)&&!in.is_open())return false;
 std::string line;while(std::getline(in,line)){if(!line.empty()&&line.back()=='\r')line.pop_back();lines.push_back(line);}
 return !in.bad();
}
static bool LaunchCompatible(const State& s,bool apply){
 if(!stereo_upscaler_d81r2::stereoPolicy(s.productionMode))return true;
 const int tech=apply&&s.hasLaunchOptions?s.upscaler:-1;
 if(tech!=-1)return stereo_upscaler_d81r2::canSelect(s.productionMode,tech);
 std::vector<std::string> lines;
 return ReadSource(s,lines)&&startup_graphics_d80::canKeepConfig(s.productionMode,lines);
}
static void LoadLaunchOptions(State& s,const fs::path& profile,bool pending){
 namespace sg=startup_graphics_d80;
 s.productionMode=ProductionMode(s.ini);
 s.developer=ReadBool(s.ini,L"VrDeveloperMode",false);s.upscaler=-1;s.quality=0;s.graphics=-1;
 s.hasLaunchOptions=pending&&GetPrivateProfileIntW(L"Settings",L"StartupSetupHasLaunchOptions",0,profile.c_str())==1;
 if(s.hasLaunchOptions)s.developer=ReadBool(profile,L"VrDeveloperMode",s.developer);
 // Config is authoritative for graphics, including changes saved by the VR
 // menu. Never show a stale INI mirror as if it were the current renderer.
 std::vector<std::string> lines;const bool sourceRead=ReadSource(s,lines);
 if(stereo_upscaler_d81r2::stereoPolicy(s.productionMode)&&(!sourceRead||!sg::canKeepConfig(s.productionMode,lines)))s.upscaler=0;
 if(!sourceRead)return;
 int tech=-1,quality=0,preset=-1;
 if(sg::readInt(lines,"r_SuperResolution_Mode",tech)){
  quality=sg::defaultQuality(tech);const auto* key=sg::qualityKey(tech);
  if((!key||sg::readInt(lines,key,quality))&&sg::valid(tech,quality,-1)&&stereo_upscaler_d81r2::canSelect(s.productionMode,tech)){s.upscaler=tech;s.quality=quality;}
 }
 if(sg::readInt(lines,"sys_spec_Full",preset)&&sg::valid(-1,0,preset)){
  std::vector<std::pair<std::string,int>> group;sg::settings(-1,0,preset,group);
  bool custom=false;for(const auto& entry:group){int value=0;
   bool assigned=false;for(const auto& text:lines)assigned|=graphics_owner31::userCfgKeyR27(text,entry.first);
   if(assigned&&(!sg::readInt(lines,entry.first.c_str(),value)||value!=preset))custom=true;
  }
  if(!custom)s.graphics=preset;
 }
}
// Preserve all unrelated INI keys/comments via the Win32 profile writer on a
// private copy, then replace once. Completion flag is committed with settings.
static bool CommitIni(State& s,bool apply,const wchar_t* serial=nullptr,bool pending=false){
 if(!LaunchCompatible(s,apply))return false;
 auto tmp=s.ini;tmp+=L".setup.tmp";DeleteFileW(tmp.c_str());
 const bool existed=fs::exists(s.ini);
 if(existed&&!CopyFileW(s.ini.c_str(),tmp.c_str(),FALSE))return false;
 if(!existed){HANDLE f=CreateFileW(tmp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);if(f==INVALID_HANDLE_VALUE)return false;CloseHandle(f);}
 bool ok=true;
 if(apply){s.values[4]=s.values[3];for(size_t i=0;i<std::size(fields);++i)ok=WritePrivateProfileStringW(L"Settings",fields[i].key,s.values[i].c_str(),tmp.c_str())&&ok;
  ok=WritePrivateProfileStringW(L"Settings",L"VrCorrectedCenterScaleFovDegrees",s.values[3].c_str(),tmp.c_str())&&ok;
  ok=WritePrivateProfileStringW(L"Settings",L"EnableVrBalancedGraphics",s.balanced?L"true":L"false",tmp.c_str())&&ok;
  if(s.hasLaunchOptions){
   if(!startup_graphics_d80::valid(s.upscaler,s.quality,s.graphics))ok=false;
   else {
    ok=WritePrivateProfileStringW(L"Settings",L"VrDeveloperMode",s.developer?L"true":L"false",tmp.c_str())&&ok;
    ok=WritePrivateProfileStringW(L"Settings",L"StartupUpscaler",std::to_wstring(s.upscaler).c_str(),tmp.c_str())&&ok;
    ok=WritePrivateProfileStringW(L"Settings",L"StartupUpscalerQuality",std::to_wstring(s.quality).c_str(),tmp.c_str())&&ok;
    ok=WritePrivateProfileStringW(L"Settings",L"StartupGraphicsPreset",std::to_wstring(s.graphics).c_str(),tmp.c_str())&&ok;
   }
  }
  if(s.headset<0||s.headset>=int(std::size(headsets)))ok=false;
  else {
   // D33 (Miki): switching the start setup to PSVR2 switches the button glyphs to PlayStation, any other named headset
   // to Xbox. Only when the headset actually changes, so a glyph picked later in the VR menu is not undone by Keep/Apply.
   wchar_t previous[64]{};GetPrivateProfileStringW(L"Settings",L"StartupHeadset",L"",previous,64,tmp.c_str());
   if(s.headset>0&&std::wcscmp(previous,headsets[s.headset].id)!=0){
    wchar_t glyph[8]{};swprintf_s(glyph,L"%d",kcd2vr::gesture_talk_policy_d33::glyphForHeadset(headsets[s.headset].id));
    ok=WritePrivateProfileStringW(L"Settings",L"VrPadGlyphStyle",glyph,tmp.c_str())&&ok;}
   ok=WritePrivateProfileStringW(L"Settings",L"StartupHeadset",headsets[s.headset].id,tmp.c_str())&&ok;}}
 // R118: the controller layout is written by Save AND Keep. Its modifier preset (PS VR2 = FACE TOUCH, others =
 // THUMBREST) is written only when the layout actually changes, so a later VR menu MODIFIER choice is not undone.
 if(s.layout>=0){
  const int layout=controller_layout_r118::Clamp(s.layout);
  // An INI from before R118 has no key = layout 0, so Keep on an old INI never resets a saved FACE TOUCH choice.
  const int previous=int(GetPrivateProfileIntW(L"Settings",L"VrControllerLayoutR118",0,tmp.c_str()));
  if(previous!=layout)ok=WritePrivateProfileStringW(L"Settings",L"VrQuickAccessFaceTouchR112",controller_layout_r118::FaceTouchFor(layout)?L"true":L"false",tmp.c_str())&&ok;
  ok=WritePrivateProfileStringW(L"Settings",L"VrControllerLayoutR118",std::to_wstring(layout).c_str(),tmp.c_str())&&ok;
 }
 // A later Keep settings supersedes an earlier pending graphics selection.
 // Copying the pending file must not retain its old apply flag.
 if(pending){
  ok=WritePrivateProfileStringW(L"Settings",L"StartupSetupHasGraphics",apply?L"1":L"0",tmp.c_str())&&ok;
  ok=WritePrivateProfileStringW(L"Settings",L"StartupSetupHasLaunchOptions",apply&&s.hasLaunchOptions?L"1":L"0",tmp.c_str())&&ok;
 }
 if(serial)ok=WritePrivateProfileStringW(L"Settings",pending?L"StartupSetupPendingSerial":L"StartupSetupAppliedSerial",serial,tmp.c_str())&&ok;
 ok=WritePrivateProfileStringW(L"Settings",L"first_start",L"0",tmp.c_str())&&ok;
 WritePrivateProfileStringW(nullptr,nullptr,nullptr,tmp.c_str());
 // R96: no KCD2VR.ini.pre-setup.bak in the game folder (one log, no extra files). The temp file is
 // merged from the current INI and replaces it in one move; the D62 lease keeps the old bytes in memory.
 if(ok)ok=MoveFileExW(tmp.c_str(),s.ini.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=FALSE;
 if(!ok)DeleteFileW(tmp.c_str());return ok;
}
// H21: the running game owns its loaded INI/atomics. The desktop dialog writes
// only a next-start profile; a later SaveMenuSettings cannot erase these choices.
static fs::path PendingPath(const fs::path& ini){return ini.parent_path()/L"KCD2VR.next-start.ini";}
static std::wstring Canonical(double value,const Field& f){wchar_t b[64]{};if(f.integer)swprintf_s(b,L"%d",int(value));else swprintf_s(b,L"%.6g",value);return b;}
static bool SaveIni(State& s,bool apply){
 s.productionMode=ProductionMode(s.ini);if(!LaunchCompatible(s,apply))return false;
 State pending=s;pending.ini=PendingPath(s.ini);
 if(apply){if(pending.values.size()!=std::size(fields))return false;pending.values[4]=pending.values[3];
  if(pending.hasLaunchOptions&&!startup_graphics_d80::valid(pending.upscaler,pending.quality,pending.graphics))return false;
  for(size_t i=0;i<std::size(fields);++i){double value=0;if(!Parse(pending.values[i],fields[i],value))return false;pending.values[i]=Canonical(value,fields[i]);}}
 FILETIME ft{};GetSystemTimeAsFileTime(&ft);wchar_t serial[48]{};swprintf_s(serial,L"%08lx%08lx-%lu",ft.dwHighDateTime,ft.dwLowDateTime,GetCurrentProcessId());
 return CommitIni(pending,apply,serial,true);
}
static bool ApplyPending(const fs::path& ini){
 const auto pending=PendingPath(ini);if(!fs::exists(pending))return true;
 wchar_t serial[64]{},applied[64]{};
 GetPrivateProfileStringW(L"Settings",L"StartupSetupPendingSerial",L"",serial,64,pending.c_str());
 if(!*serial)return false;
 // D62: a next-start profile contains only selected setup fields. It must
 // never create a sparse main INI before LoadConfig can check the full file.
 // Refusal retains pending choices for a retry after the full INI is restored.
 // This startup lease ends before LoadConfig acquires its own writer lease.
 kcd2vr::core::config_file_guard_d62::Lease currentIniD62(ini);
 if(!currentIniD62.valid())return false;
 GetPrivateProfileStringW(L"Settings",L"StartupSetupAppliedSerial",L"",applied,64,ini.c_str());
 if(std::wcscmp(serial,applied)==0){DeleteFileW(pending.c_str());return true;} // cleanup refusal must not replay settings
 State target{};target.ini=ini;target.root=GameRoot(ini.parent_path());target.productionMode=ProductionMode(ini);const bool apply=GetPrivateProfileIntW(L"Settings",L"StartupSetupHasGraphics",0,pending.c_str())==1;
 {const int layout=int(GetPrivateProfileIntW(L"Settings",L"VrControllerLayoutR118",-1,pending.c_str()));target.layout=layout>=0&&layout<controller_layout_r118::Count?layout:-1;} // R118
 if(apply){for(const auto& f:fields){wchar_t b[256]{};double value=0;
   GetPrivateProfileStringW(L"Settings",f.key,L"",b,256,pending.c_str());
   if(&f==&fields[4]){target.values.push_back(target.values[3]);continue;} // migrate older pending profiles
   if(!Parse(b,f,value))return false;target.values.push_back(Canonical(value,f));}
  target.headset=ReadHeadset(pending);
  wchar_t balanced[16]{};GetPrivateProfileStringW(L"Settings",L"EnableVrBalancedGraphics",L"",balanced,16,pending.c_str());
  if(_wcsicmp(balanced,L"true")==0||wcscmp(balanced,L"1")==0)target.balanced=true;
  else if(_wcsicmp(balanced,L"false")==0||wcscmp(balanced,L"0")==0)target.balanced=false;else return false;
  target.hasLaunchOptions=GetPrivateProfileIntW(L"Settings",L"StartupSetupHasLaunchOptions",0,pending.c_str())==1;
  if(target.hasLaunchOptions){
   target.upscaler=GetPrivateProfileIntW(L"Settings",L"StartupUpscaler",-99,pending.c_str());
   target.quality=GetPrivateProfileIntW(L"Settings",L"StartupUpscalerQuality",-99,pending.c_str());
   target.graphics=GetPrivateProfileIntW(L"Settings",L"StartupGraphicsPreset",-99,pending.c_str());
   if(!startup_graphics_d80::valid(target.upscaler,target.quality,target.graphics))return false;
   wchar_t developer[16]{};GetPrivateProfileStringW(L"Settings",L"VrDeveloperMode",L"",developer,16,pending.c_str());
   if(_wcsicmp(developer,L"true")==0||wcscmp(developer,L"1")==0)target.developer=true;
   else if(_wcsicmp(developer,L"false")==0||wcscmp(developer,L"0")==0)target.developer=false;else return false;
  }
 }
 // Called BEFORE LoadConfig and all runtime menu/settings writers. Merge into
 // the CURRENT main INI, retaining unrelated settings/calibration written meanwhile.
 if(!CommitIni(target,apply,serial,false))return false;
 DeleteFileW(pending.c_str());return true;
}
int EnsureDefaultIniR118() noexcept {
 HMODULE self=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&EnsureDefaultIniR118),&self))return int(core::default_ini_r118::Result::Failed);
 try{const auto dir=ModulePath(self).parent_path();if(dir.empty())return int(core::default_ini_r118::Result::Failed);
  return int(core::default_ini_r118::EnsureBesideModule(dir/L"KCD2VR.ini"));}catch(...){return int(core::default_ini_r118::Result::Failed);}
}
static const wchar_t* IniStatusR118(const fs::path& ini){std::error_code e;return fs::exists(ini,e)?L"KCD2VR.ini found beside the mod (kept as it is).":L"No KCD2VR.ini yet: the game writes the default at start.";}
bool ApplyPendingBeforeConfig() noexcept {
 HMODULE self=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&ApplyPendingBeforeConfig),&self))return false;
 try{return ApplyPending(ModulePath(self).parent_path()/L"KCD2VR.ini");}catch(...){return false;}
}
// Next-launch resolution/FOV and explicit graphics selections. No live writes.
static bool SaveSource(State& s,bool launchOptions=false){
 if(s.root.empty()||s.values.size()!=std::size(fields))return false;
 double width=0,height=0,fov=0;if(!Parse(s.values[1],fields[1],width)||!Parse(s.values[2],fields[2],height)||!Parse(s.values[3],fields[3],fov))return false;
 const auto cfg=s.root/L"user.cfg";auto tmp=cfg;tmp+=L".setup.tmp";
 std::vector<std::string> lines;if(!ReadSource(s,lines))return false;
 s.productionMode=ProductionMode(s.ini);
 auto key=[](const std::string& x){auto a=x.find_first_not_of(" \t");if(a==std::string::npos||x[a]=='#'||x[a]==';')return std::string{};auto b=x.find_first_of(" \t=",a);auto k=x.substr(a,b-a);std::transform(k.begin(),k.end(),k.begin(),[](unsigned char c){return char(std::tolower(c));});return k;};
 std::vector<std::pair<std::string,int>> selected;
 if(launchOptions){if(!startup_graphics_d80::settingsForProduction(s.productionMode,s.upscaler,s.quality,s.graphics,lines,selected))return false;}
 else if(!startup_graphics_d80::canKeepConfig(s.productionMode,lines))return false;
 if(launchOptions)lines=graphics_owner31::updateUserCfgR27(lines,selected);
 std::ofstream out(tmp,std::ios::binary|std::ios::trunc);if(!out)return false;
 for(const auto& l:lines){auto k=key(l);if(k!="r_width"&&k!="r_height"&&k!="cl_fov"&&k!="cl_hfov")out<<l<<"\r\n";}
 out<<"r_Width = "<<int(width)<<"\r\nr_Height = "<<int(height)<<"\r\ncl_fov = "<<fov<<"\r\n";out.flush();bool ok=out.good();out.close();ok=ok&&!out.fail();
 // Native is an explicit mode 0 write, never the no-write Keep (-1) choice.
 // Verify both the closed staging file and the installed file before reporting
 // success. A successful rename alone does not confirm the requested setting.
 const auto selectionSaved=[&](const fs::path& file){
  std::ifstream check(file,std::ios::binary);if(!check.is_open())return false;
  std::vector<std::string> saved;std::string line;
  while(std::getline(check,line)){if(!line.empty()&&line.back()=='\r')line.pop_back();saved.push_back(line);}
  if(check.bad())return false;
  for(const auto& entry:selected){int actual=0;
   if(!startup_graphics_d80::readInt(saved,entry.first.c_str(),actual)||actual!=entry.second)return false;}
  // Check the explicit UI request too, independently of the generated list.
  if(launchOptions&&s.upscaler>=0){int actual=-1;
   if(!startup_graphics_d80::readInt(saved,"r_SuperResolution_Mode",actual)||actual!=s.upscaler)return false;}
  return true;
 };
 if(ok)ok=selectionSaved(tmp);
 // R96: no user.cfg.pre-setup.bak either; the verified temp file replaces user.cfg in one move.
 if(ok)ok=MoveFileExW(tmp.c_str(),cfg.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=FALSE;
 if(ok)ok=selectionSaved(cfg);
 if(!ok)DeleteFileW(tmp.c_str());return ok;
}
static HWND Child(HWND w,const wchar_t* cls,const wchar_t* label,DWORD style,int x,int y,int width,int height,int id=0){auto c=CreateWindowExW(0,cls,label,WS_CHILD|WS_VISIBLE|style,x,y,width,height,w,reinterpret_cast<HMENU>(INT_PTR(id)),nullptr,nullptr);auto s=reinterpret_cast<State*>(GetWindowLongPtrW(w,GWLP_USERDATA));if(c&&s)SendMessageW(c,WM_SETFONT,reinterpret_cast<WPARAM>(s->font),TRUE);return c;}
static void FillResolutions(HWND w,State& s,int selectedRow,bool apply){
 s.updatingPreset=true;auto combo=GetDlgItem(w,resolutionControl);SendMessageW(combo,CB_RESETCONTENT,0,0);
 SendMessageW(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Custom (use width / height below)"));
 if(s.headset>0)for(int row=1;row<=8;++row){auto label=PresetLabel(s.headset,row);SendMessageW(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(label.c_str()));}
 if(s.headset==0||selectedRow<0||selectedRow>8)selectedRow=0;
 SendMessageW(combo,CB_SETCURSEL,selectedRow,0);
 int width=0,height=0;if(apply&&PresetSize(s.headset,selectedRow,width,height)){
  SetDlgItemTextW(w,101,std::to_wstring(width).c_str());SetDlgItemTextW(w,102,std::to_wstring(height).c_str());}
 s.updatingPreset=false;
}
static void FillChoices(HWND w,int id,const startup_graphics_d80::Choice* choices,std::size_t count,int selected){
 auto combo=GetDlgItem(w,id);SendMessageW(combo,CB_RESETCONTENT,0,0);int row=0;
 for(std::size_t i=0;i<count;++i){const auto added=SendMessageW(combo,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(choices[i].label));
  if(added>=0){SendMessageW(combo,CB_SETITEMDATA,added,choices[i].value);if(choices[i].value==selected)row=int(added);}}
 SendMessageW(combo,CB_SETCURSEL,row,0);
}
static int SelectedChoice(HWND w,int id){const auto row=SendDlgItemMessageW(w,id,CB_GETCURSEL,0,0);
 return row==CB_ERR?-99:int(SendDlgItemMessageW(w,id,CB_GETITEMDATA,row,0));}
static void FillQuality(HWND w,State& s){const auto q=startup_graphics_d80::qualities(s.upscaler);
 FillChoices(w,qualityControl,q.data,q.size,s.quality);EnableWindow(GetDlgItem(w,qualityControl),s.upscaler>0);}
static LRESULT ProcBody(HWND w,UINT msg,WPARAM wp,LPARAM lp){
 auto s=reinterpret_cast<State*>(GetWindowLongPtrW(w,GWLP_USERDATA));
 if(msg==WM_NCCREATE){s=static_cast<State*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);SetWindowLongPtrW(w,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(s));}
 if(msg==WM_CREATE){
  Child(w,L"STATIC",L"KCD2VR setup",0,24,16,570,28);
  Child(w,L"STATIC",L"Choose a headset and a source-resolution preset, or keep your custom values.\nNative = panel pixels per eye. OpenXR render scale is a separate setting.",0,24,48,570,46);
  Child(w,L"STATIC",L"Headset",0,24,108,175,26);
  auto headset=Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,104,360,280,headsetControl);
  for(const auto& h:headsets)SendMessageW(headset,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(h.label));
  SendMessageW(headset,CB_SETCURSEL,s->headset,0);
  Child(w,L"STATIC",L"Source resolution",0,24,148,175,26);
  Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,144,360,300,resolutionControl);
  Child(w,L"STATIC",L"Upscaling / antialiasing",0,24,188,180,26);
  Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,184,360,260,upscalerControl);
  s->productionMode=ProductionMode(s->ini);
  const auto technologies=startup_graphics_d80::technologiesForProduction(s->productionMode,LaunchCompatible(*s,false));
  FillChoices(w,upscalerControl,technologies.data,technologies.size,s->upscaler);s->upscaler=SelectedChoice(w,upscalerControl);
  if(s->upscaler<=0)s->quality=0;
  Child(w,L"STATIC",L"Upscaling quality",0,24,228,180,26);
  Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,224,360,260,qualityControl);FillQuality(w,*s);
  Child(w,L"STATIC",L"Game graphics preset",0,24,268,180,26);
  Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,264,360,260,graphicsControl);
  FillChoices(w,graphicsControl,startup_graphics_d80::presets,std::size(startup_graphics_d80::presets),s->graphics);
  for(size_t i=0;i<std::size(fields);++i){Child(w,L"STATIC",fields[i].label,0,24,324+int(i)*36,385,26);Child(w,L"EDIT",s->initial[i].c_str(),WS_BORDER|WS_TABSTOP|ES_AUTOHSCROLL,420,320+int(i)*36,150,27,100+int(i));}
  SetDlgItemTextW(w,104,s->initial[3].c_str());EnableWindow(GetDlgItem(w,104),FALSE);
  int initialRow=0;double width=0,height=0;
  if(Parse(s->initial[1],fields[1],width)&&Parse(s->initial[2],fields[2],height))
   for(int row=1;row<=8;++row){int pw=0,ph=0;if(PresetSize(s->headset,row,pw,ph)&&width==pw&&height==ph){initialRow=row;break;}}
  FillResolutions(w,*s,initialRow,false);
  Child(w,L"BUTTON",L"VR performance overrides (distance, blur, sharpening)",BS_AUTOCHECKBOX|WS_TABSTOP,24,580,550,26,200);SendDlgItemMessageW(w,200,BM_SETCHECK,s->balanced?BST_CHECKED:BST_UNCHECKED,0);
  Child(w,L"BUTTON",L"Developer mode - detailed VR menu and F-key diagnostics",BS_AUTOCHECKBOX|WS_TABSTOP,24,616,550,26,developerControl);SendDlgItemMessageW(w,developerControl,BM_SETCHECK,s->developer?BST_CHECKED:BST_UNCHECKED,0);
  // R118: controller layout + one info line block that says what changes. Saved by Save AND Keep.
  Child(w,L"STATIC",L"Controller layout",0,24,656,175,26);
  auto layout=Child(w,L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_TABSTOP|WS_VSCROLL,210,652,360,200,layoutControl);
  for(int i=0;i<controller_layout_r118::Count;++i)SendMessageW(layout,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(controller_layout_r118::SetupLabel(i)));
  s->layout=controller_layout_r118::Clamp(s->layout);SendMessageW(layout,CB_SETCURSEL,s->layout,0);
  Child(w,L"STATIC",controller_layout_r118::SetupInfo(s->layout),0,24,690,550,44,layoutInfoControl);
  Child(w,L"STATIC",stereo_upscaler_d81r2::stereoPolicy(s->productionMode)
   ?L"DLSS, FSR and XeSS are stereo candidates; image quality still needs testing.\nNative remains available. Upscaling and graphics also appear in the VR menu.\nExisting calibration stays as it is."
   :L"Graphics changes apply on the next game start. DLSS needs a supported NVIDIA GPU.\nSame Developer mode as the VR menu; takes effect next launch.\nKeep settings preserves your current choices and calibration.",0,24,742,550,66);
  // R118: default INI. Written only when missing; an existing KCD2VR.ini is never replaced.
  Child(w,L"STATIC",IniStatusR118(s->ini),0,24,822,360,26,iniStatusControl);
  Child(w,L"BUTTON",L"Create default KCD2VR.ini",WS_TABSTOP,390,818,180,30,createIniControl);
  Child(w,L"BUTTON",s->launcher?L"Save & start":L"Save for next start",WS_TABSTOP|BS_DEFPUSHBUTTON,24,862,170,34,IDOK);
  Child(w,L"BUTTON",s->launcher?L"Keep settings & start":L"Keep settings",WS_TABSTOP,206,862,190,34,201);
  Child(w,L"BUTTON",s->launcher?L"Cancel":L"Skip this time",WS_TABSTOP,410,862,160,34,IDCANCEL);ConfigureScroll(w,*s);return 0;
 }
 if(msg==WM_SIZE&&s){ConfigureScroll(w,*s);return 0;}
 if(msg==WM_VSCROLL&&s){SCROLLINFO info{sizeof(info),SIF_ALL};GetScrollInfo(w,SB_VERT,&info);int next=s->scrollY;
  switch(LOWORD(wp)){case SB_LINEUP:next-=32;break;case SB_LINEDOWN:next+=32;break;
   case SB_PAGEUP:next-=std::max(32,int(info.nPage)-32);break;case SB_PAGEDOWN:next+=std::max(32,int(info.nPage)-32);break;
   case SB_TOP:next=0;break;case SB_BOTTOM:next=contentHeight;break;case SB_THUMBTRACK:case SB_THUMBPOSITION:next=info.nTrackPos;break;}
  ScrollTo(w,*s,next);return 0;
 }
 if(msg==WM_MOUSEWHEEL&&s){s->wheelDelta+=GET_WHEEL_DELTA_WPARAM(wp);const int steps=s->wheelDelta/WHEEL_DELTA;
  s->wheelDelta-=steps*WHEEL_DELTA;if(steps)ScrollTo(w,*s,s->scrollY-steps*96);return 0;}
 if(msg==WM_COMMAND&&s){const auto id=LOWORD(wp);
  if(id==headsetControl&&HIWORD(wp)==CBN_SELCHANGE){
   const int chosen=int(SendDlgItemMessageW(w,headsetControl,CB_GETCURSEL,0,0));if(chosen<0||chosen>=int(std::size(headsets)))return 0;
   const int row=int(SendDlgItemMessageW(w,resolutionControl,CB_GETCURSEL,0,0));s->headset=chosen;
   // R118: picking PS VR2 as headset also picks the PS VR2 controller layout (visible, can be changed before saving).
   if(std::wcscmp(headsets[chosen].id,L"psvr2")==0&&s->layout!=controller_layout_r118::Psvr2){
    s->layout=controller_layout_r118::Psvr2;SendDlgItemMessageW(w,layoutControl,CB_SETCURSEL,s->layout,0);
    SetDlgItemTextW(w,layoutInfoControl,controller_layout_r118::SetupInfo(s->layout));}
   // Naming a headset does not opt a tuned custom resolution into its native
   // panel resolution. Only an explicitly selected preset changes those pixels.
   FillResolutions(w,*s,row>0?row:0,row>0);return 0;
  }
  if(id==resolutionControl&&HIWORD(wp)==CBN_SELCHANGE){const int row=int(SendDlgItemMessageW(w,resolutionControl,CB_GETCURSEL,0,0));FillResolutions(w,*s,row,true);return 0;}
  if(id==upscalerControl&&HIWORD(wp)==CBN_SELCHANGE){
   s->upscaler=SelectedChoice(w,upscalerControl);s->quality=startup_graphics_d80::defaultQuality(s->upscaler);FillQuality(w,*s);return 0;}
  if(id==qualityControl&&HIWORD(wp)==CBN_SELCHANGE){s->quality=SelectedChoice(w,qualityControl);return 0;}
  if(id==layoutControl&&HIWORD(wp)==CBN_SELCHANGE){ // R118
   const int chosen=int(SendDlgItemMessageW(w,layoutControl,CB_GETCURSEL,0,0));if(chosen<0||chosen>=controller_layout_r118::Count)return 0;
   s->layout=chosen;SetDlgItemTextW(w,layoutInfoControl,controller_layout_r118::SetupInfo(chosen));return 0;}
  if(id==createIniControl&&HIWORD(wp)==BN_CLICKED){ // R118: never replaces an existing KCD2VR.ini
   const auto result=core::default_ini_r118::EnsureBesideModule(s->ini);
   MessageBoxW(w,result==core::default_ini_r118::Result::Created?L"Default KCD2VR.ini written beside the mod. Save / Keep puts your choices here into it at the next game start.":
    result==core::default_ini_r118::Result::Exists?L"KCD2VR.ini already exists beside the mod and was not changed. To start from the defaults, rename or delete it first, then press this button again.":
    result==core::default_ini_r118::Result::NoDefault?L"This build has no built-in default KCD2VR.ini. Copy KCD2VR.ini from the mod package.":
    L"Could not write KCD2VR.ini beside the mod. Check that the game folder is writable.",L"KCD2VR setup",MB_OK|(result==core::default_ini_r118::Result::Created||result==core::default_ini_r118::Result::Exists?MB_ICONINFORMATION:MB_ICONERROR));
   SetDlgItemTextW(w,iniStatusControl,IniStatusR118(s->ini));return 0;}
  if(id==graphicsControl&&HIWORD(wp)==CBN_SELCHANGE){s->graphics=SelectedChoice(w,graphicsControl);
   // A chosen native preset should not be silently capped by the old VR preset.
   // The user can explicitly enable the supplemental overrides afterwards.
   if(s->graphics>=1)SendDlgItemMessageW(w,200,BM_SETCHECK,BST_UNCHECKED,0);return 0;}
  if(id==103&&HIWORD(wp)==EN_CHANGE){wchar_t value[128]{};GetDlgItemTextW(w,103,value,128);SetDlgItemTextW(w,104,value);return 0;}
  if((id==101||id==102)&&HIWORD(wp)==EN_CHANGE&&!s->updatingPreset){SendDlgItemMessageW(w,resolutionControl,CB_SETCURSEL,0,0);return 0;}
  if(id==IDOK||id==201){const bool apply=id==IDOK;s->values=s->initial;
  s->productionMode=ProductionMode(s->ini);
  if(!apply&&!LaunchCompatible(*s,false)){MessageBoxW(w,L"The current game setting is unknown or unsupported for stereo. Choose Native, DLSS, FSR or XeSS and use Save & start (or Save for next start). No settings were changed.",L"Choose a supported stereo setting",MB_OK|MB_ICONWARNING);return 0;}
  if(!apply){s->values.clear();for(const auto& f:fields)s->values.push_back(Read(s->ini,f));}
  if(apply){for(size_t i=0;i<std::size(fields);++i){wchar_t b[128]{};GetDlgItemTextW(w,100+int(i),b,128);double v=0;if(!Parse(b,fields[i],v)){MessageBoxW(w,L"Enter a number within the range shown. Source dimensions must be 640-4096; FOV 70-130; zoom 0.5-2; theater scale 0.3-2.",L"Check setup value",MB_OK|MB_ICONWARNING);SetFocus(GetDlgItem(w,100+int(i)));return 0;}s->values[i]=Canonical(v,fields[i]);}
   s->balanced=SendDlgItemMessageW(w,200,BM_GETCHECK,0,0)==BST_CHECKED;
   s->developer=SendDlgItemMessageW(w,developerControl,BM_GETCHECK,0,0)==BST_CHECKED;s->hasLaunchOptions=true;
   s->upscaler=SelectedChoice(w,upscalerControl);s->quality=SelectedChoice(w,qualityControl);s->graphics=SelectedChoice(w,graphicsControl);
   if(!startup_graphics_d80::valid(s->upscaler,s->quality,s->graphics)){MessageBoxW(w,L"Select an upscaler, its supported quality mode, and a graphics preset.",L"Check graphics settings",MB_OK|MB_ICONWARNING);return 0;}
   if(!LaunchCompatible(*s,true)){MessageBoxW(w,L"Choose Native, DLSS, FSR or XeSS for stereo. Keep is available only when user.cfg confirms a supported choice.",L"Choose a supported stereo setting",MB_OK|MB_ICONWARNING);return 0;}
   if(!SaveSource(*s,true)){MessageBoxW(w,L"Could not write and verify game-root user.cfg. No game was started. Check that the launcher is beside the installed dinput8.dll and files are writable.",L"Setup was not completed",MB_OK|MB_ICONERROR);return 0;}}
  if(!apply&&s->launcher&&!SaveSource(*s)){MessageBoxW(w,L"Could not synchronize the saved resolution/FOV to game-root user.cfg. No game was started.",L"Setup was not completed",MB_OK|MB_ICONERROR);return 0;}
  if(!SaveIni(*s,apply)){MessageBoxW(w,L"Could not save the next-start settings profile. Existing INI was not replaced; setup will be offered again.",L"Setup was not completed",MB_OK|MB_ICONERROR);return 0;}
  s->start=true;DestroyWindow(w);return 0;
 }if(id==IDCANCEL){DestroyWindow(w);return 0;}}
 if(msg==WM_CLOSE){DestroyWindow(w);return 0;}if(msg==WM_DESTROY){if(s)s->done=true;return 0;}return DefWindowProcW(w,msg,wp,lp);
}
static LRESULT CALLBACK Proc(HWND w,UINT msg,WPARAM wp,LPARAM lp) noexcept {
 try{return ProcBody(w,msg,wp,lp);}catch(...){
  MessageBoxW(w,L"Setup could not complete. Check file permissions and try again. No game was launched by this dialog.",L"KCD2VR setup",MB_OK|MB_ICONERROR);
  return msg==WM_CREATE?-1:0;
 }
}
static void SetupLogR131(const char* line) noexcept {if(auto f=setupLogSinkR131)f(line);} // R131: in-game path only, KCD2VR.log
static bool Show(HMODULE module,bool launcher){
 auto path=ModulePath(module);if(path.empty())return false;State s{};s.ini=path.parent_path()/L"KCD2VR.ini";s.root=GameRoot(path.parent_path());s.launcher=launcher;
 HANDLE mutex=CreateMutexW(nullptr,TRUE,L"Local\\KCD2VR_SETUP_S15");
 struct Guard {HANDLE h;~Guard(){if(h){ReleaseMutex(h);CloseHandle(h);}}} guard{mutex};
 if(!mutex||GetLastError()==ERROR_ALREADY_EXISTS){if(!launcher)SetupLogR131("R131 SETUP_WINDOW skipped reason=ALREADY_OPEN (another setup window runs)");return false;}
 if(!launcher&&GetPrivateProfileIntW(L"Settings",L"first_start",1,PendingPath(s.ini).c_str())==0){SetupLogR131("R131 SETUP_WINDOW skipped reason=PENDING_SAVED (first_start 0 in the pending file)");return false;}
 if(!launcher&&GetPrivateProfileIntW(L"Settings",L"first_start",1,s.ini.c_str())==0){SetupLogR131("R131 SETUP_WINDOW skipped reason=FIRST_START_0");return false;}
 // A setup window is never opened from the loader lock or a render callback.
 const auto pending=PendingPath(s.ini);const auto uiIni=GetPrivateProfileIntW(L"Settings",L"StartupSetupHasGraphics",0,pending.c_str())==1?pending:s.ini;
 s.headset=ReadHeadset(uiIni);for(const auto& f:fields)s.initial.push_back(Read(uiIni,f));
 {const int pendingLayout=int(GetPrivateProfileIntW(L"Settings",L"VrControllerLayoutR118",-1,pending.c_str())); // R118
  s.layout=controller_layout_r118::Clamp(pendingLayout>=0?pendingLayout:int(GetPrivateProfileIntW(L"Settings",L"VrControllerLayoutR118",0,s.ini.c_str())));}wchar_t balanced[16]{};GetPrivateProfileStringW(L"Settings",L"EnableVrBalancedGraphics",L"true",balanced,16,uiIni.c_str());s.balanced=_wcsicmp(balanced,L"false")&&wcscmp(balanced,L"0");
 LoadLaunchOptions(s,uiIni,uiIni==pending);
 s.font=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
 WNDCLASSW cls{};cls.lpfnWndProc=Proc;cls.hInstance=module;cls.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));cls.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);cls.lpszClassName=L"KCD2VRStartupS15";
 if(!RegisterClassW(&cls)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS){if(!launcher)SetupLogR131("R131 SETUP_WINDOW failed reason=REGISTER_CLASS");DeleteObject(s.font);return false;}
 MONITORINFO monitor{sizeof(monitor)};RECT work{};
 if(GetMonitorInfoW(MonitorFromWindow(GetForegroundWindow(),MONITOR_DEFAULTTOPRIMARY),&monitor))work=monitor.rcWork;
 else if(!SystemParametersInfoW(SPI_GETWORKAREA,0,&work,0))work={0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};
 const RECT r=FitWindow(work);
 // R131: at game start the game window covers the desktop and Windows refuses SetForegroundWindow from our thread, so
 // the window opened BEHIND the game (Miki 7 Nov: "not working"). in-game it is now topmost and flashes in the taskbar.
 auto w=CreateWindowExW(WS_EX_APPWINDOW|(launcher?0:WS_EX_TOPMOST),cls.lpszClassName,L"KCD2VR - Setup",setupStyle,r.left,r.top,r.right-r.left,r.bottom-r.top,nullptr,nullptr,module,&s);
 if(!w&&!launcher)SetupLogR131("R131 SETUP_WINDOW failed reason=CREATE_WINDOW");
 if(w&&!launcher){SetWindowPos(w,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);FLASHWINFO fl{sizeof(fl),w,FLASHW_ALL|FLASHW_TIMERNOFG,0,0};FlashWindowEx(&fl);
  SetupLogR131("R131 SETUP_WINDOW shown on the desktop (topmost; KCD2VR - Setup). Save = used from the next start, close = offered again next start");}
 if(w){ShowWindow(w,SW_SHOWNORMAL);SetForegroundWindow(w);MSG m{};HWND lastFocus=nullptr;while(!s.done){auto got=GetMessageW(&m,nullptr,0,0);if(got<=0)break;if(!IsDialogMessageW(w,&m)){TranslateMessage(&m);DispatchMessageW(&m);}
   if(!s.done){const auto focus=GetFocus();if(focus!=lastFocus){RevealControl(w,s,focus);lastFocus=focus;}}}if(IsWindow(w))DestroyWindow(w);}
 if(w&&!launcher)SetupLogR131(s.start?"R131 SETUP_WINDOW saved (first_start 0, settings used from the next start)":"R131 SETUP_WINDOW closed without saving (offered again next start)");
 DeleteObject(s.font);return s.start;
}
static DWORD WINAPI SetupWorker(void* module) {try{(void)Show(static_cast<HMODULE>(module),false);}catch(...){}FreeLibraryAndExitThread(static_cast<HMODULE>(module),0);return 0;}
void FirstRun() noexcept {
 // Called after LoadConfig; never block renderer hook installation on its
 // bootstrap thread. Own a separate GUI thread; do not stall native startup.
 HMODULE held=nullptr;
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(&FirstRun),&held))return;
 try{auto p=ModulePath(held).parent_path()/L"KCD2VR.ini";if(GetPrivateProfileIntW(L"Settings",L"first_start",1,p.c_str())==0||GetPrivateProfileIntW(L"Settings",L"first_start",1,PendingPath(p).c_str())==0){FreeLibrary(held);return;}
  SetupLogR131("R131 SETUP_WINDOW offered: no first_start 0 in KCD2VR.ini, opening the setup window");}
 catch(...){FreeLibrary(held);return;}
 HANDLE worker=CreateThread(nullptr,0,SetupWorker,held,0,nullptr);if(worker)CloseHandle(worker);else{SetupLogR131("R131 SETUP_WINDOW failed reason=THREAD");FreeLibrary(held);}
}
}
#if defined(KCD2VR_PROXY_DINPUT8)
#define KCD2VR_SETUP_EXPORT_S15 // proxy .def owns ordinal7
#else
#define KCD2VR_SETUP_EXPORT_S15 __declspec(dllexport)
#endif
extern "C" KCD2VR_SETUP_EXPORT_S15 void CALLBACK KCD2VRSetupW(HWND,HINSTANCE,LPWSTR,int){
 HMODULE self=nullptr;GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&KCD2VRSetupW),&self);
 try{if(kcd2vr::startup_s15::Show(self,true)){auto result=ShellExecuteW(nullptr,L"open",L"steam://rungameid/1771300",nullptr,nullptr,SW_SHOWNORMAL);if(reinterpret_cast<INT_PTR>(result)<=32)MessageBoxW(nullptr,L"Settings saved, but Steam could not be started. Start the game from Steam.",L"KCD2VR setup",MB_OK|MB_ICONERROR);}}
 catch(...){MessageBoxW(nullptr,L"Setup could not complete. No game was launched.",L"KCD2VR setup",MB_OK|MB_ICONERROR);}
}

#undef KCD2VR_SETUP_EXPORT_S15
