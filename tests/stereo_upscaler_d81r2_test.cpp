// Actual startup policy, config parser and SaveSource. Only Win32 file/profile
// leaves are replaced by host filesystem adapters; no renderer/game is loaded.
#include "subsystems/base/startup_graphics_d80.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cwctype>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
using HFONT=void*;
constexpr int TRUE=1,FALSE=0,MOVEFILE_REPLACE_EXISTING=1,MOVEFILE_WRITE_THROUGH=2;
static int moves=0,copies=0;
static unsigned GetPrivateProfileStringW(const wchar_t*,const wchar_t* key,const wchar_t* fallback,wchar_t* out,unsigned capacity,const char* file){
 std::ifstream in(file);std::string line;std::wstring value(fallback),wideKey(key);const std::string narrow(wideKey.begin(),wideKey.end());
 while(std::getline(in,line))if(kcd2vr::graphics_owner31::userCfgKeyR27(line,narrow)){
  const auto raw=line.substr(line.find('=')+1);value.assign(raw.begin(),raw.end());
 }
 const auto count=std::min<std::size_t>(value.size(),capacity-1);std::copy_n(value.c_str(),count,out);out[count]=0;return static_cast<unsigned>(count);
}
static bool CopyFileW(const char* from,const char* to,bool failExists){++copies;std::error_code error;
 return std::filesystem::copy_file(from,to,failExists?std::filesystem::copy_options::none:std::filesystem::copy_options::overwrite_existing,error);}
static bool MoveFileExW(const char* from,const char* to,int){++moves;std::error_code error;std::filesystem::rename(from,to,error);return !error;}
static bool DeleteFileW(const char* file){std::error_code error;return std::filesystem::remove(file,error);}
namespace kcd2vr::startup_s15 {
namespace fs=std::filesystem;
#include "startup_d81r2_actual.inc"
}
using namespace kcd2vr;
using namespace kcd2vr::startup_s15;
static void put(const fs::path& file,const std::string& value){std::ofstream out(file);out<<value;assert(out.good());}
static std::string bytes(const fs::path& file){std::ifstream in(file,std::ios::binary);return {std::istreambuf_iterator<char>(in),{}};}
int main(int argc,char** argv){
 assert(argc==2);const fs::path root(argv[1]);fs::create_directories(root);
 namespace p=stereo_upscaler_d81r2;namespace g=startup_graphics_d80;
 int checks=0;auto ck=[&](bool value){++checks;if(!value){std::fprintf(stderr,"FAIL check=%d\n",checks);std::abort();}};
 ck(p::nativeRequired(3)&&!p::nativeRequired(0));
 for(int tech:{-1,0,1,2,3,4,99}){ck(p::canSelect(3,tech)==(tech==0));ck(p::canSelect(0,tech));}
 for(bool known:{false,true})for(int tech:{-1,0,1,2,3,4}){
  ck(p::canKeep(3,known,tech)==(known&&tech==0));ck(p::canKeep(0,known,tech));}
 const std::vector<std::vector<std::string>> rejected{{},{";r_SuperResolution_Mode=0"},{"r_SuperResolution_Mode=2"},
  {"r_SuperResolution_Mode=0","r_SuperResolution_Mode=2"},{"r_SuperResolution_Mode=0","r_SuperResolution_Mode=custom"},
  {"r_SuperResolution_Mode=0","r_SuperResolution_Mode="},{"r_SuperResolution_Mode=99"}};
 for(const auto& lines:rejected){ck(!g::canKeepConfig(3,lines));ck(g::canKeepConfig(0,lines));}
 std::vector<std::string> native{"r_SuperResolution_Mode=2"," R_SUPERRESOLUTION_MODE = \"0\" // Native", "custom_key=7"};
 ck(g::canKeepConfig(3,native));
 std::vector<std::pair<std::string,int>> selected;
 for(int tech:{1,2,4}){auto q=g::qualities(tech);for(std::size_t i=0;i<q.size;++i){
  ck(!g::settingsForProduction(3,tech,q.data[i].value,-1,native,selected)&&selected.empty());
  ck(g::settingsForProduction(0,tech,q.data[i].value,-1,native,selected));}}
 ck(g::settingsForProduction(3,-1,0,-1,native,selected)&&selected.empty());
 ck(!g::settingsForProduction(3,-1,0,-1,{},selected));
 ck(g::settingsForProduction(3,0,0,-1,{},selected)&&selected.size()==1&&selected[0].second==0);
 ck(g::technologiesForProduction(3,false).size==1&&g::technologiesForProduction(3,false).data[0].value==0);
 ck(g::technologiesForProduction(3,true).size==2&&g::technologiesForProduction(0,false).size==5);
 State s{};s.root=root;s.ini=root/"KCD2VR.ini";s.values={L"1",L"1920",L"1800",L"95",L"95",L"1",L"1.05"};s.hasLaunchOptions=true;
 const auto cfg=root/"user.cfg";
 put(s.ini,"[Settings]\nVrStereoExperiment=3\nStartupUpscaler=0\n[Calibration]\nHeight=1.83\n");
 const auto originalIni=bytes(s.ini);
 for(const auto* invalid:{"custom","3","2","-1","0bad",""}){put(s.ini,std::string("[Settings]\nVrStereoExperiment=")+invalid+"\n");ck(ProductionMode(s.ini)==3);}
 put(s.ini,originalIni);
 for(int tech:{1,2,4})for(int quality:{0,1,5}){
  s.upscaler=tech;s.quality=quality;
  put(cfg,"r_SuperResolution_Mode=2\ncustom_key=7\n");const auto before=bytes(cfg);moves=copies=0;
  ck(!SaveSource(s,true)&&bytes(cfg)==before&&moves==0&&copies==0);
  ck(!SaveSource(s,false)&&bytes(cfg)==before&&moves==0&&copies==0);
 }
 // Actual file content wins over a stale Native INI mirror and stale UI state.
 s.upscaler=-1;s.quality=0;
 for(const auto& lines:rejected){std::string text;for(const auto& line:lines)text+=line+"\n";put(cfg,text);
  moves=copies=0;ck(!SaveSource(s,true)&&bytes(cfg)==text&&moves==0&&copies==0);ck(!LaunchCompatible(s,true));}
 fs::remove(cfg);ck(!SaveSource(s,false)&&!fs::exists(cfg));
 const std::string old="; r_SuperResolution_Mode=2\nr_SuperResolution_Mode=0\nR_SUPERRESOLUTION_MODE=2\nr_AntialiasingMode=3\ncustom_key=7\n";
 put(cfg,old);s.upscaler=0;ck(SaveSource(s,true));const auto repaired=bytes(cfg);
 std::vector<std::string> actual;ck(ReadSource(s,actual)&&g::canKeepConfig(3,actual));int count=0;for(const auto& l:actual)count+=graphics_owner31::userCfgKeyR27(l,"r_SuperResolution_Mode");
 ck(count==1&&repaired.find("r_AntialiasingMode=3")!=std::string::npos&&repaired.find("custom_key=7")!=std::string::npos);
 ck(bytes(root/"user.cfg.pre-setup.bak")==old&&bytes(s.ini)==originalIni);
 s.upscaler=-1;ck(SaveSource(s,true)&&LaunchCompatible(s,true));ck(SaveSource(s,false));
 put(cfg,"r_SuperResolution_Mode=4\n");ck(!SaveSource(s,true)&&!SaveSource(s,false));
 // Mono retains all existing technology and quality choices, including DLAA.
 put(s.ini,"[Settings]\nVrStereoExperiment=0\n");
 for(int tech:{0,1,2,4}){const auto qualities=g::qualities(tech);for(std::size_t i=0;i<qualities.size;++i){
  s.upscaler=tech;s.quality=qualities.data[i].value;ck(SaveSource(s,true));
  std::vector<std::string> current;int actualTech=-1;ck(ReadSource(s,current)&&g::readInt(current,"r_SuperResolution_Mode",actualTech)&&actualTech==tech);
 }}
 put(cfg,"r_SuperResolution_Mode=custom\n");s.upscaler=-1;s.quality=0;ck(SaveSource(s,true)&&SaveSource(s,false));
 std::printf("D81R2 startup compatibility: %d checks PASS (actual policy/parser/SaveSource; Win32 leaves substituted)\n",checks);
}
