// Source-connected desktop setup test; real temporary cfg files, host OS leaves.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "subsystems/base/startup_graphics_d80.h"
#include "subsystems/base/view_defaults_b2.h"

using HWND=int;using HFONT=void*;using WPARAM=std::intptr_t;using LPARAM=std::intptr_t;
constexpr int CB_RESETCONTENT=1,CB_ADDSTRING=2,CB_SETITEMDATA=3,CB_SETCURSEL=4,CB_GETCURSEL=5,CB_GETITEMDATA=6,BM_GETCHECK=7,BST_CHECKED=1,CB_ERR=-1;
constexpr int IDOK=1,MB_OK=1,MB_ICONWARNING=2,MB_ICONERROR=4,TRUE=1,FALSE=0;
constexpr int MOVEFILE_REPLACE_EXISTING=1,MOVEFILE_WRITE_THROUGH=8;
constexpr int upscalerControl=212,qualityControl=213,graphicsControl=214,developerControl=215;
struct Combo {std::vector<std::wstring> labels;std::vector<int> data;int selected=-1;bool enabled=true;};
std::map<int,Combo> controls;
std::map<int,std::wstring> texts;
std::vector<std::wstring> messages;
std::map<std::pair<std::string,std::wstring>,std::wstring> profiles;
enum class MoveResult {Normal,Fail,SuccessWithoutMove,CorruptMode,CorruptPreset};
MoveResult moveResult=MoveResult::Normal;
int iniCalls=0,savedIniTech=-99,renameCalls=0,copyCalls=0;
bool iniOk=true,destroyed=false;
std::filesystem::path evidenceRoot;

HWND GetDlgItem(HWND,int id){return id;}
std::intptr_t SendMessageW(HWND id,int msg,std::intptr_t a,std::intptr_t b){auto& c=controls[id];
    switch(msg){case CB_RESETCONTENT:c={};return 0;
    case CB_ADDSTRING:c.labels.emplace_back(reinterpret_cast<const wchar_t*>(b));c.data.push_back(0);return c.labels.size()-1;
    case CB_SETITEMDATA:c.data.at(a)=int(b);return 0;
    case CB_SETCURSEL:c.selected=int(a);return a;
    case CB_GETCURSEL:return c.selected;
    case CB_GETITEMDATA:return c.selected<0?CB_ERR:c.data.at(a);
    case BM_GETCHECK:return 0;default:return 0;}}
std::intptr_t SendDlgItemMessageW(HWND w,int id,int msg,std::intptr_t a,std::intptr_t b){return SendMessageW(GetDlgItem(w,id),msg,a,b);}
void EnableWindow(HWND id,bool enabled){controls[id].enabled=enabled;}
void GetDlgItemTextW(HWND,int id,wchar_t* out,int count){std::wcsncpy(out,texts[id].c_str(),count-1);out[count-1]=0;}
void MessageBoxW(HWND,const wchar_t* message,const wchar_t*,int){messages.emplace_back(message);}
void SetFocus(HWND){}
void DestroyWindow(HWND){destroyed=true;}
template<std::size_t N,class... A> int swprintf_s(wchar_t (&out)[N],const wchar_t* fmt,A... args){return std::swprintf(out,N,fmt,args...);}
int _wcsicmp(const wchar_t* a,const wchar_t* b){while(*a&&std::towlower(*a)==std::towlower(*b)){++a;++b;}return int(std::towlower(*a))-int(std::towlower(*b));}
unsigned GetPrivateProfileStringW(const wchar_t*,const wchar_t* key,const wchar_t* fallback,wchar_t* out,unsigned capacity,const std::filesystem::path::value_type* path){
    const auto found=profiles.find({std::filesystem::path(path).string(),key});const auto value=found==profiles.end()?std::wstring(fallback):found->second;
    if(capacity){std::wcsncpy(out,value.c_str(),capacity-1);out[capacity-1]=0;}return capacity?std::wcslen(out):0;}
int GetPrivateProfileIntW(const wchar_t*,const wchar_t* key,int fallback,const std::filesystem::path::value_type* path){
    const auto found=profiles.find({std::filesystem::path(path).string(),key});if(found==profiles.end())return fallback;try{return std::stoi(found->second);}catch(...){return fallback;}}
int CopyFileW(const std::filesystem::path::value_type* from,const std::filesystem::path::value_type* to,int failIfExists){++copyCalls;std::error_code error;
    return std::filesystem::copy_file(from,to,failIfExists?std::filesystem::copy_options::none:std::filesystem::copy_options::overwrite_existing,error)&&!error;}
int DeleteFileW(const std::filesystem::path::value_type* path){std::error_code error;return std::filesystem::remove(path,error)&&!error;}
int MoveFileExW(const std::filesystem::path::value_type* from,const std::filesystem::path::value_type* to,int){++renameCalls;
    if(moveResult==MoveResult::Fail)return FALSE;
    if(moveResult==MoveResult::SuccessWithoutMove)return TRUE;
    std::error_code error;std::filesystem::rename(from,to,error);if(error)return FALSE;
    if(moveResult==MoveResult::CorruptMode){std::ofstream out(to,std::ios::app);out<<"r_SuperResolution_Mode = 2\n";}
    if(moveResult==MoveResult::CorruptPreset){std::ofstream out(to,std::ios::app);out<<"sys_spec_Full = 5\n";}
    return TRUE;}

namespace kcd2vr::startup_s15 {
namespace fs=std::filesystem;
#include "startup_native_save_actual.inc"
// SaveSource is real above. The pending-INI leaf records exactly what the actual
// UI branch passes; no claim is made to emulate Windows profile transactions.
static bool SaveIni(State& s,bool apply){++iniCalls;savedIniTech=apply?s.upscaler:-1;return iniOk;}
#include "startup_native_save_click.inc"
}

using namespace kcd2vr;
using namespace startup_s15;
int failures=0,checks=0;
void check(bool ok,const char* label){++checks;if(!ok){++failures;std::cout<<"FAIL "<<label<<'\n';}}
std::string readFile(const std::filesystem::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),{}};}
bool actualMode(const State& s,int mode){std::vector<std::string> lines;int actual=-99;return ReadSource(s,lines)&&startup_graphics_d80::readInt(lines,"r_SuperResolution_Mode",actual)&&actual==mode;}

State prepare(const std::string& name,int previous,bool existing=true){
    controls.clear();texts.clear();messages.clear();profiles.clear();iniCalls=renameCalls=copyCalls=0;
    iniOk=true;destroyed=false;moveResult=MoveResult::Normal;
    State s{};s.root=evidenceRoot/name;std::filesystem::remove_all(s.root);std::filesystem::create_directories(s.root);
    s.ini=s.root/"KCD2VR.ini";s.launcher=true;s.upscaler=previous;s.quality=startup_graphics_d80::defaultQuality(previous);
    profiles[{s.ini.string(),L"VrStereoExperiment"}]=L"3";
    profiles[{s.ini.string(),L"StartupUpscaler"}]=L"4"; // Deliberately stale mirror.
    if(existing){std::ofstream out(s.root/"user.cfg",std::ios::binary);
        out<<"; r_SuperResolution_Mode = 4 (comment)\r\n\tr_SuperResolution_Mode = "<<previous<<"\r\n R_SUPERRESOLUTION_MODE = "<<previous<<"\n"
           <<"sys_MaxFPS = 0\r\nr_SuperResolution_NVIDIA_DLSS_QualityMode = 1\r\n";}
    for(std::size_t i=0;i<std::size(fields);++i){auto value=Canonical(fields[i].fallback,fields[i]);s.initial.push_back(value);texts[100+int(i)]=value;}
    s.values=s.initial;
    const auto technologies=startup_graphics_d80::technologiesForProduction(3,existing);
    FillChoices(1,upscalerControl,technologies.data,technologies.size,0);
    // Cached state deliberately remains previous: IDOK must re-read UI item data.
    FillChoices(1,qualityControl,startup_graphics_d80::nativeQuality,std::size(startup_graphics_d80::nativeQuality),0);
    EnableWindow(GetDlgItem(1,qualityControl),false);
    FillChoices(1,graphicsControl,startup_graphics_d80::presets,std::size(startup_graphics_d80::presets),-1);
    return s;
}

void successPaths(){
    for(int previous:{1,2,4}){auto s=prepare("native_from_"+std::to_string(previous),previous);
        const auto original=readFile(s.root/"user.cfg");
        check(SelectedChoice(1,upscalerControl)==0,"Native combo item data remains zero");
        Handle(s,IDOK);
        check(s.hasLaunchOptions&&s.upscaler==0&&s.quality==0&&!controls[qualityControl].enabled,"Save handler re-reads valid Native and disabled quality zero");
        check(actualMode(s,0),"Save & start installs exact Native mode zero");
        check(iniCalls==1&&savedIniTech==0,"pending profile receives exact Native zero");
        check(s.start&&destroyed&&messages.empty(),"successful verified saves allow start");
        std::vector<std::string> lines;ReadSource(s,lines);int assignments=0;
        for(const auto& line:lines)assignments+=graphics_owner31::userCfgKeyR27(line,"r_SuperResolution_Mode");
        check(assignments==1,"case-insensitive duplicate active assignments are removed");
        check(readFile(s.root/"user.cfg").find("; r_SuperResolution_Mode = 4 (comment)")!=std::string::npos,"mode comments remain unchanged");
        int cap=-1;check(startup_graphics_d80::readInt(lines,"sys_MaxFPS",cap)&&cap==0,"unrelated FPS setting is retained");
        check(readFile(s.root/"user.cfg.pre-setup.bak")==original,"backup preserves original cfg exactly");
        check(!std::filesystem::exists(s.root/"user.cfg.setup.tmp"),"successful replacement leaves no staging file");
        State reopened=s;reopened.upscaler=2;LoadLaunchOptions(reopened,s.ini,false);
        check(reopened.upscaler==0,"reopened UI takes cfg zero despite stale StartupUpscaler mirror");
    }
    {auto s=prepare("missing_cfg_native",2,false);Handle(s,IDOK);
        check(s.start&&actualMode(s,0)&&copyCalls==0,"explicit Native creates a missing cfg and permits start");}
    {auto s=prepare("native_and_preset",2);FillChoices(1,graphicsControl,startup_graphics_d80::presets,std::size(startup_graphics_d80::presets),3);Handle(s,IDOK);
        std::vector<std::string> lines;ReadSource(s,lines);int preset=-1;
        check(s.start&&actualMode(s,0)&&startup_graphics_d80::readInt(lines,"sys_spec_Full",preset)&&preset==3,"Native zero survives an explicit graphics preset");}
    {auto s=prepare("keep_dropdown",2);const auto tech=startup_graphics_d80::technologiesForProduction(3,true);
        FillChoices(1,upscalerControl,tech.data,tech.size,-1);Handle(s,IDOK);
        check(s.start&&actualMode(s,2)&&savedIniTech==-1,"Keep minus one preserves current upscaler");}
    {auto s=prepare("keep_button",2);Handle(s,201);
        check(s.start&&actualMode(s,2)&&savedIniTech==-1,"existing Keep button preserves current upscaler");}
}

void failurePaths(){
    for(const auto fault:{MoveResult::Fail,MoveResult::SuccessWithoutMove,MoveResult::CorruptMode}){
        auto s=prepare("rename_fault_"+std::to_string(int(fault)),2);moveResult=fault;Handle(s,IDOK);
        check(!s.start&&!destroyed&&iniCalls==0&&!messages.empty(),"rename failure or wrong installed mode blocks pending save and launch");
        check(!std::filesystem::exists(s.root/"user.cfg.setup.tmp"),"failed verified save cleans remaining staging file");
    }
    {auto s=prepare("preset_corruption",2);moveResult=MoveResult::CorruptPreset;
        FillChoices(1,graphicsControl,startup_graphics_d80::presets,std::size(startup_graphics_d80::presets),3);Handle(s,IDOK);
        check(!s.start&&iniCalls==0&&!messages.empty(),"wrong installed graphics key also blocks launch");}
    {auto s=prepare("pending_failure",2);iniOk=false;Handle(s,IDOK);
        check(!s.start&&!destroyed&&actualMode(s,0)&&iniCalls==1&&!messages.empty(),"pending profile failure blocks launch after cfg save");}
    {auto s=prepare("missing_combo_selection",2);controls[upscalerControl].selected=-1;Handle(s,IDOK);
        check(!s.start&&renameCalls==0&&iniCalls==0&&!messages.empty(),"missing selection blocks cfg and pending writes");}
    {auto s=prepare("missing_cfg_keep",2,false);const auto tech=startup_graphics_d80::technologiesForProduction(0,true);
        FillChoices(1,upscalerControl,tech.data,tech.size,-1);Handle(s,IDOK);
        check(!s.start&&renameCalls==0&&!std::filesystem::exists(s.root/"user.cfg"),"unverified Keep cannot create a misleading cfg");}
}

void omittedMode(){
    auto s=prepare("missing_generated_mode",2);const auto before=readFile(s.root/"user.cfg");Handle(s,IDOK);
    check(!s.start&&iniCalls==0&&!messages.empty(),"independent request guard blocks a generator that omits explicit Native");
    check(renameCalls==0&&copyCalls==0,"invalid staged selection is rejected before backup or replacement");
    check(readFile(s.root/"user.cfg")==before,"rejected stage leaves original cfg intact");
    check(!std::filesystem::exists(s.root/"user.cfg.setup.tmp"),"rejected stage is cleaned");
}

int main(int argc,char** argv){
    if(argc!=3)return 2;evidenceRoot=std::filesystem::absolute(argv[1]);std::filesystem::create_directories(evidenceRoot);
    if(std::string(argv[2])=="missing_generated_mode")omittedMode();else{successPaths();failurePaths();}
    std::cout<<(failures?"FAIL ":"PASS ")<<checks<<" startup selection/real-cfg checks, "<<failures<<" failures; Win32/profile leaves are adapters, no game run\n";
    return failures?1:0;
}
