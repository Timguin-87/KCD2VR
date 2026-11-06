// Literal production startup bodies are extracted by the D82 runner. Only
// Win32 file/profile leaves are substituted; files are real host fixtures.
#include "subsystems/base/startup_graphics_d80.h"
#include "vrik/gesture_talk_policy_d33.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
using HFONT=void*;
using HANDLE=FILE*;
constexpr int TRUE=1,FALSE=0,MOVEFILE_REPLACE_EXISTING=1,MOVEFILE_WRITE_THROUGH=2;
constexpr int GENERIC_WRITE=1,CREATE_NEW=1,FILE_ATTRIBUTE_NORMAL=1;
constexpr HANDLE INVALID_HANDLE_VALUE=nullptr;
static int moves=0,copies=0,profileWrites=0;
static bool failMove=false,failCopy=false;
static std::string narrow(const wchar_t* value){if(!value)return {};std::wstring text(value);return {text.begin(),text.end()};}
static std::string lower(std::string text){for(auto& c:text)c=char(std::tolower(static_cast<unsigned char>(c)));return text;}
static std::vector<std::string> hostLines(const char* path){std::ifstream in(path);std::vector<std::string> out;std::string line;while(std::getline(in,line)){if(!line.empty()&&line.back()=='\r')line.pop_back();out.push_back(line);}return out;}
static unsigned GetPrivateProfileStringW(const wchar_t* section,const wchar_t* key,const wchar_t* fallback,wchar_t* out,unsigned capacity,const char* file){
    std::wstring value(fallback);const auto wantedSection=lower(narrow(section)),wantedKey=narrow(key);std::string active;
    for(const auto& line:hostLines(file)){
        if(line.size()>1&&line.front()=='['&&line.back()==']'){active=lower(line.substr(1,line.size()-2));continue;}
        if(active==wantedSection&&kcd2vr::graphics_owner31::userCfgKeyR27(line,wantedKey)){
            const auto raw=line.substr(line.find('=')+1);value.assign(raw.begin(),raw.end());
        }
    }
    const auto count=std::min<std::size_t>(value.size(),capacity-1);std::copy_n(value.c_str(),count,out);out[count]=0;return static_cast<unsigned>(count);
}
static int GetPrivateProfileIntW(const wchar_t* section,const wchar_t* key,int fallback,const char* file){
    wchar_t text[128]{};const auto defaultText=std::to_wstring(fallback);GetPrivateProfileStringW(section,key,defaultText.c_str(),text,128,file);return int(std::wcstol(text,nullptr,10));
}
static bool WritePrivateProfileStringW(const wchar_t* section,const wchar_t* key,const wchar_t* value,const char* file){
    if(!section&&!key&&!value)return true; // flush leaf
    ++profileWrites;auto lines=hostLines(file);std::string active;const auto wantedSection=lower(narrow(section)),wantedKey=narrow(key);
    std::size_t insert=lines.size();bool foundSection=false,updated=false;
    for(std::size_t i=0;i<lines.size();++i){
        const auto line=lines[i];
        if(line.size()>1&&line.front()=='['&&line.back()==']'){
            if(foundSection&&active==wantedSection){insert=i;break;}
            active=lower(line.substr(1,line.size()-2));foundSection|=active==wantedSection;continue;
        }
        if(active==wantedSection&&kcd2vr::graphics_owner31::userCfgKeyR27(line,wantedKey)){
            if(value)lines[i]=wantedKey+"="+narrow(value);else lines.erase(lines.begin()+i);
            updated=true;break;
        }
    }
    if(!updated&&value){
        if(!foundSection){lines.push_back("["+narrow(section)+"]");insert=lines.size();}
        lines.insert(lines.begin()+insert,wantedKey+"="+narrow(value));
    }
    std::ofstream out(file,std::ios::binary|std::ios::trunc);for(const auto& line:lines)out<<line<<"\n";out.flush();return out.good();
}
static bool CopyFileW(const char* from,const char* to,bool failExists){++copies;if(failCopy)return false;std::error_code error;return std::filesystem::copy_file(from,to,failExists?std::filesystem::copy_options::none:std::filesystem::copy_options::overwrite_existing,error);}
static bool MoveFileExW(const char* from,const char* to,int){++moves;if(failMove)return false;std::error_code error;std::filesystem::rename(from,to,error);return !error;}
static bool DeleteFileW(const char* file){std::error_code error;return std::filesystem::remove(file,error);}
static HANDLE CreateFileW(const char* file,int,int,void*,int,int,void*){if(std::filesystem::exists(file))return INVALID_HANDLE_VALUE;return std::fopen(file,"wb");}
static void CloseHandle(HANDLE handle){std::fclose(handle);}
static int _wcsicmp(const wchar_t* a,const wchar_t* b){return wcscasecmp(a,b);}
template<std::size_t N,class... T> static int swprintf_s(wchar_t (&out)[N],const wchar_t* format,T... args){return std::swprintf(out,N,format,args...);}
namespace kcd2vr::startup_s15 {
namespace fs=std::filesystem;
#include "startup_d82_actual.inc"
}
using namespace kcd2vr;
using namespace kcd2vr::startup_s15;
static int checks=0,failures=0;
static void check(bool ok,const char* expression,int line){++checks;if(!ok){++failures;std::fprintf(stderr,"FAIL line=%d %s\n",line,expression);}}
#define CHECK(x) check(bool(x),#x,__LINE__)
static void put(const fs::path& path,const std::string& value){std::ofstream out(path,std::ios::binary);out<<value;CHECK(out.good());}
static std::string bytes(const fs::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),{}};}
static std::vector<std::string> cfgLines(const State& state){std::vector<std::string> lines;CHECK(ReadSource(state,lines));return lines;}
static int cfgValue(const State& state,const char* key){int value=-99;CHECK(startup_graphics_d80::readInt(cfgLines(state),key,value));return value;}
static int assignmentCount(const std::vector<std::string>& lines,const char* key){int count=0;for(const auto& line:lines)count+=graphics_owner31::userCfgKeyR27(line,key);return count;}
int main(int argc,char** argv){
    if(argc!=2)return 2;const fs::path root(argv[1]);fs::remove_all(root);fs::create_directories(root);
    namespace p=stereo_upscaler_d81r2;namespace g=startup_graphics_d80;
    for(int tech:{-1,0,1,2,3,4,99}){CHECK(p::canSelect(3,tech)==(tech==0||tech==1||tech==2));CHECK(p::canSelect(0,tech));}
    for(bool known:{false,true})for(int tech:{-1,0,1,2,3,4}){CHECK(p::canKeep(3,known,tech)==(known&&(tech==0||tech==1||tech==2)));CHECK(p::canKeep(0,known,tech));}
    for(bool keep:{false,true}){
        const auto choices=g::technologiesForProduction(3,keep);CHECK(choices.size==std::size_t(keep?4:3));
        const std::vector<int> expected=keep?std::vector<int>{-1,0,2,1}:std::vector<int>{0,2,1};
        for(std::size_t i=0;i<choices.size;++i){CHECK(choices.data[i].value==expected[i]);
            if(choices.data[i].value>0)CHECK(std::wstring(choices.data[i].label).find(L"D82 stereo candidate")!=std::wstring::npos);
        }
    }
    CHECK(g::technologiesForProduction(0,false).size==5&&g::technologiesForProduction(0,true).size==5);
    const std::vector<std::vector<std::string>> rejected{{},{";r_SuperResolution_Mode=0"},{"r_SuperResolution_Mode=4"},{"r_SuperResolution_Mode=3"},
        {"r_SuperResolution_Mode=0","r_SuperResolution_Mode=custom"},{"r_SuperResolution_Mode=1","r_SuperResolution_Mode="},{"r_SuperResolution_Mode=99"}};
    for(const auto& lines:rejected){CHECK(!g::canKeepConfig(3,lines));CHECK(g::canKeepConfig(0,lines));}
    for(int tech:{0,1,2})CHECK(g::canKeepConfig(3,{"r_SuperResolution_Mode=4"," R_SUPERRESOLUTION_MODE = \""+std::to_string(tech)+"\" // supported"}));
    std::vector<std::pair<std::string,int>> selected;
    for(int tech:{0,1,2,4}){const auto q=g::qualities(tech);for(std::size_t i=0;i<q.size;++i){
        const bool valid=tech!=4;CHECK(g::settingsForProduction(3,tech,q.data[i].value,-1,{},selected)==valid);
        if(valid)CHECK(selected.back()==std::make_pair(std::string("r_SuperResolution_Mode"),tech));else CHECK(selected.empty());
        CHECK(g::settingsForProduction(0,tech,q.data[i].value,-1,{},selected));
    }}
    CHECK(!g::settingsForProduction(3,-1,0,-1,{},selected)&&selected.empty());
    CHECK(g::settingsForProduction(3,-1,0,-1,{"r_SuperResolution_Mode=2"},selected)&&selected.empty());
    for(int tech:{-99,3,5})CHECK(!g::settingsForProduction(3,tech,0,-1,{},selected)&&selected.empty());

    State s{};s.root=root;s.ini=root/"KCD2VR.ini";s.values={L"1",L"1920",L"1800",L"95",L"95",L"1",L"1.05"};s.hasLaunchOptions=true;
    const auto cfg=root/"user.cfg",profile=root/"pending.ini";
    const std::string originalIni="; preserve user calibration and preferences\n[Settings]\nVrStereoExperiment=3\nStartupUpscaler=0\nVrDeveloperMode=false\nStartupHeadset=psvr2\nVrPadGlyphStyle=2\nVrManualBowDefault59=true\nUserCollisionSetting=0.73\nfirst_start=1\n[Calibration]\nHeight=1.83\nControllerGrip=0.127\n";
    const std::string unrelatedCfg="; preserve cfg comment\nr_AntialiasingMode=3\nr_DuplicateLastFrame=0\ncustom_key=7\nr_SuperResolution_NVIDIA_DLSS_Preset=6\n";
    put(s.ini,originalIni);for(const auto* value:{"custom","3","2","-1","0bad",""}){put(s.ini,std::string("[Settings]\nVrStereoExperiment=")+value+"\n");CHECK(ProductionMode(s.ini)==3);}
    put(s.ini,"[Settings]\nVrStereoExperiment=0\n");CHECK(ProductionMode(s.ini)==0);put(s.ini,originalIni);
    // Explicit saved technology/quality is exact. AA, custom values and the
    // user's entire INI remain unchanged by the graphics source writer.
    for(int tech:{0,1,2}){const auto qualities=g::qualities(tech);for(std::size_t i=0;i<qualities.size;++i){
        s.upscaler=tech;s.quality=qualities.data[i].value;s.graphics=-1;
        put(cfg,"r_SuperResolution_Mode=4\nR_SUPERRESOLUTION_MODE=0\n"+unrelatedCfg);moves=copies=0;
        CHECK(SaveSource(s,true));CHECK(cfgValue(s,"r_SuperResolution_Mode")==tech);
        CHECK(assignmentCount(cfgLines(s),"r_SuperResolution_Mode")==1);
        if(tech>0)CHECK(cfgValue(s,g::qualityKey(tech))==s.quality);
        const auto after=bytes(cfg);for(const auto* keep:{"preserve cfg comment","r_AntialiasingMode=3","r_DuplicateLastFrame=0","custom_key=7","r_SuperResolution_NVIDIA_DLSS_Preset=6"})CHECK(after.find(keep)!=std::string::npos);
        CHECK(bytes(s.ini)==originalIni&&moves==1);
    }}
    CHECK(bytes(root/"user.cfg.pre-setup.bak")=="r_SuperResolution_Mode=4\nR_SUPERRESOLUTION_MODE=0\n"+unrelatedCfg);
    for(int quality:{0,2,6}){s.upscaler=4;s.quality=quality;s.productionMode=0;put(cfg,"r_SuperResolution_Mode=2\n"+unrelatedCfg);const auto before=bytes(cfg);moves=copies=0;
        CHECK(!SaveSource(s,true)&&bytes(cfg)==before&&moves==0&&copies==0);CHECK(s.productionMode==3);
    }
    // Keep reads actual cfg, not StartupUpscaler=0 or a stale UI field.
    for(int actual:{0,1,2}){
        put(cfg,"r_SuperResolution_Mode="+std::to_string(actual)+"\n"+unrelatedCfg);s.upscaler=-1;s.quality=0;s.productionMode=3;
        CHECK(LaunchCompatible(s,true)&&LaunchCompatible(s,false));CHECK(SaveSource(s,true));CHECK(cfgValue(s,"r_SuperResolution_Mode")==actual);
        s.upscaler=4;s.quality=6;CHECK(!LaunchCompatible(s,true)&&LaunchCompatible(s,false));CHECK(SaveSource(s,false));CHECK(cfgValue(s,"r_SuperResolution_Mode")==actual);
    }
    s.upscaler=-1;s.quality=0;for(const auto& lines:rejected){std::string contents;for(const auto& line:lines)contents+=line+"\n";put(cfg,contents);moves=copies=0;
        CHECK(!LaunchCompatible(s,true)&&!LaunchCompatible(s,false));CHECK(!SaveSource(s,true)&&!SaveSource(s,false)&&bytes(cfg)==contents&&moves==0&&copies==0);
    }
    fs::remove(cfg);CHECK(!SaveSource(s,false)&&!fs::exists(cfg));std::vector<std::string> missing;CHECK(ReadSource(s,missing)&&missing.empty());
    fs::create_directory(cfg);std::vector<std::string> unreadable;CHECK(!ReadSource(s,unreadable));CHECK(!SaveSource(s,true)&&!LaunchCompatible(s,false));fs::remove(cfg);
    // Source read failure changes no file; stale pending metadata cannot claim
    // that the configured live technology has some other identity.
    put(profile,"[Settings]\nStartupSetupHasLaunchOptions=1\nStartupUpscaler=1\nStartupUpscalerQuality=1\nVrDeveloperMode=true\nVrStereoExperiment=0\n");
    put(cfg,"r_SuperResolution_Mode=2\nr_SuperResolution_NVIDIA_DLSS_QualityMode=5\n"+unrelatedCfg);
    State opened=s;LoadLaunchOptions(opened,profile,true);
    CHECK(opened.productionMode==3&&opened.upscaler==2&&opened.quality==5&&opened.developer&&opened.hasLaunchOptions);
    CHECK(bytes(s.ini)==originalIni);LoadLaunchOptions(opened,s.ini,false);CHECK(!opened.developer&&!opened.hasLaunchOptions&&opened.upscaler==2);
    for(const auto* mode:{"4","3","custom"}){put(cfg,std::string("r_SuperResolution_Mode=")+mode+"\n");const auto before=bytes(cfg);LoadLaunchOptions(opened,s.ini,false);CHECK(opened.upscaler==0&&bytes(cfg)==before);}
    fs::remove(cfg);LoadLaunchOptions(opened,s.ini,false);CHECK(opened.upscaler==0&&!fs::exists(cfg));
    fs::create_directory(cfg);LoadLaunchOptions(opened,s.ini,false);CHECK(opened.upscaler==0);fs::remove(cfg);
    put(cfg,"r_SuperResolution_Mode=2\nr_SuperResolution_NVIDIA_DLSS_QualityMode=invalid\n");LoadLaunchOptions(opened,s.ini,false);CHECK(opened.upscaler==-1); // Keep does not fabricate a quality.
    put(cfg,"r_SuperResolution_Mode=0\nsys_spec_Full=3\nsys_spec_Shading=custom\n");LoadLaunchOptions(opened,s.ini,false);CHECK(opened.graphics==-1);
    put(cfg,"r_SuperResolution_Mode=0\nsys_spec_Full=4\nsys_spec_Shading=4\n");LoadLaunchOptions(opened,s.ini,false);CHECK(opened.graphics==4);

    // The exact CommitIni preserves existing non-setup settings and calibration.
    put(s.ini,originalIni);put(cfg,"r_SuperResolution_Mode=2\n"+unrelatedCfg);s.productionMode=3;s.upscaler=2;s.quality=5;s.graphics=-1;s.headset=6;s.developer=true;s.hasLaunchOptions=true;
    CHECK(CommitIni(s,true,L"D82-test",false));const auto committed=bytes(s.ini);
    for(const auto* keep:{"preserve user calibration and preferences","VrStereoExperiment=3","VrManualBowDefault59=true","UserCollisionSetting=0.73","Height=1.83","ControllerGrip=0.127","VrPadGlyphStyle=2"})CHECK(committed.find(keep)!=std::string::npos);
    CHECK(GetPrivateProfileIntW(L"Settings",L"StartupUpscaler",-1,s.ini.c_str())==2&&GetPrivateProfileIntW(L"Settings",L"StartupUpscalerQuality",-1,s.ini.c_str())==5);
    CHECK(ReadBool(s.ini,L"VrDeveloperMode",false));CHECK(bytes(fs::path(s.ini.string()+".pre-setup.bak"))==originalIni);
    s.upscaler=4;s.quality=6;const auto beforeRefusal=bytes(s.ini);moves=copies=profileWrites=0;CHECK(!CommitIni(s,true)&&bytes(s.ini)==beforeRefusal&&moves==0&&copies==0&&profileWrites==0);
    // Keep ignores staged edits and clears obsolete pending apply flags.
    CHECK(CommitIni(s,false,L"D82-keep",true));CHECK(GetPrivateProfileIntW(L"Settings",L"StartupUpscaler",-1,s.ini.c_str())==2);
    CHECK(GetPrivateProfileIntW(L"Settings",L"StartupSetupHasGraphics",-1,s.ini.c_str())==0&&GetPrivateProfileIntW(L"Settings",L"StartupSetupHasLaunchOptions",-1,s.ini.c_str())==0);
    CHECK(bytes(s.ini).find("ControllerGrip=0.127")!=std::string::npos);
    // Failed atomic replacement never destroys the existing config or INI.
    s.upscaler=1;s.quality=0;const auto iniBeforeFailure=bytes(s.ini),cfgBeforeFailure=bytes(cfg);failMove=true;
    CHECK(!SaveSource(s,true)&&bytes(cfg)==cfgBeforeFailure&&!fs::exists(root/"user.cfg.setup.tmp"));
    CHECK(!CommitIni(s,true)&&bytes(s.ini)==iniBeforeFailure&&!fs::exists(root/"KCD2VR.ini.setup.tmp"));failMove=false;

    // Mono retains the complete native selection/quality contract, including
    // XeSS; its Keep behavior does not impose Stereo's verification rule.
    put(s.ini,"[Settings]\nVrStereoExperiment=0\n");s.productionMode=0;
    for(int tech:{0,1,2,4}){const auto qualities=g::qualities(tech);for(std::size_t i=0;i<qualities.size;++i){s.upscaler=tech;s.quality=qualities.data[i].value;CHECK(SaveSource(s,true));CHECK(cfgValue(s,"r_SuperResolution_Mode")==tech);}}
    put(cfg,"r_SuperResolution_Mode=custom\ncustom_key=7\n");s.upscaler=-1;s.quality=0;CHECK(SaveSource(s,true)&&SaveSource(s,false));CHECK(bytes(cfg).find("r_SuperResolution_Mode=custom")!=std::string::npos);
    put(cfg,"r_SuperResolution_Mode=4\nr_SuperResolution_INTEL_XeSS_QualityMode=6\n");LoadLaunchOptions(opened,s.ini,false);CHECK(opened.productionMode==0&&opened.upscaler==4&&opened.quality==6);
    std::printf("D82 startup checks=%d failures=%d; actual policy/parser/load/commit/save, host Win32 leaves, Windows/headset NOT_RUN\n",checks,failures);
    return failures?1:0;
}
