// Complete current ApplyPendingBeforeConfig -> ApplyPending -> CommitIni flow.
// Native file/profile/module APIs are explicit leaves using real temp files.
#include "../src/core/config_file_guard_d62.h"
#include "../src/vrik/gesture_talk_policy_d33.h"
#include "temp_ini_fixture_d62.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <iterator>
#include <sstream>
#include <vector>
#ifndef _WIN32
using DWORD=unsigned long;using HANDLE=void*;using HMODULE=void*;using HFONT=void*;using LPCWSTR=const wchar_t*;
constexpr bool FALSE=false,TRUE=true;constexpr unsigned GENERIC_WRITE=1,CREATE_NEW=1,FILE_ATTRIBUTE_NORMAL=1;
constexpr unsigned MOVEFILE_REPLACE_EXISTING=1,MOVEFILE_WRITE_THROUGH=2;
constexpr unsigned GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS=1,GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT=2;
#define INVALID_HANDLE_VALUE reinterpret_cast<HANDLE>(~std::uintptr_t(0))
template<std::size_t N,class...Args>static int swprintf_s(wchar_t(&out)[N],const wchar_t* format,Args...args){return std::swprintf(out,N,format,args...);}
static int _wcsicmp(const wchar_t* a,const wchar_t* b){while(*a&&*b&&std::towlower(*a)==std::towlower(*b)){++a;++b;}return int(std::towlower(*a))-int(std::towlower(*b));}
#endif
namespace fs=std::filesystem;
namespace guard=kcd2vr::core::config_file_guard_d62;
static unsigned checks=0,profileWrites=0,creates=0,copies=0,replaces=0,deletes=0,mainReads=0;
static bool failWrites=false,failCopy=false,failReplace=false;
static fs::path chosenDllPath,mainPath;
static void check(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL D62 pending setup %u: %s\n",checks,why);std::abort();}}
static std::string bytes(const fs::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};}
static void put(const fs::path& path,const std::string& value){std::ofstream out(path,std::ios::binary|std::ios::trunc);out.write(value.data(),std::streamsize(value.size()));out.close();if(!out)throw std::runtime_error("fixture write failed");}
static std::string narrow(const wchar_t* value){std::string out;for(;value&&*value;++value)out.push_back(char(*value));return out;}
static std::string trim(std::string value){const auto a=value.find_first_not_of(" \t\r");if(a==std::string::npos)return {};const auto b=value.find_last_not_of(" \t\r");return value.substr(a,b-a+1);}
// This fixture implements only ASCII test-profile transport. It is not a Win32
// parser emulation or evidence about engine/config parser behavior.
static std::wstring value(const fs::path& path,const wchar_t* section,const wchar_t* key,const wchar_t* fallback){
 std::istringstream input(bytes(path));std::string line,current,found=narrow(fallback);
 while(std::getline(input,line)){line=trim(line);if(line.empty()||line[0]==';')continue;
  if(line.front()=='['&&line.back()==']'){current=line.substr(1,line.size()-2);continue;}
  const auto at=line.find('=');if(at!=std::string::npos&&current==narrow(section)&&trim(line.substr(0,at))==narrow(key))found=trim(line.substr(at+1));
 }
 return {found.begin(),found.end()};
}
static DWORD FixtureReadProfileD62(const wchar_t* section,const wchar_t* key,const wchar_t* fallback,wchar_t* out,DWORD capacity,const fs::path::value_type* path){
 if(fs::path(path)==mainPath)++mainReads;
 const auto result=value(path,section,key,fallback);if(!capacity)return 0;
 const auto n=std::min<std::size_t>(result.size(),capacity-1);std::copy_n(result.data(),n,out);out[n]=0;return DWORD(n);
}
static unsigned FixtureReadIntD62(const wchar_t* section,const wchar_t* key,int fallback,const fs::path::value_type* path){const auto v=value(path,section,key,L"");return v.empty()?unsigned(fallback):unsigned(std::wcstol(v.c_str(),nullptr,10));}
static int FixtureWriteProfileD62(const wchar_t* section,const wchar_t* key,const wchar_t* text,const fs::path::value_type* path){
 if(!section&&!key&&!text)return 0;
 ++profileWrites;if(failWrites)return 0;
 std::ofstream out(path,std::ios::binary|std::ios::app);out<<"\n["<<narrow(section)<<"]\n"<<narrow(key)<<'='<<narrow(text)<<'\n';out.close();return out?1:0;
}
static int FixtureDeleteD62(const fs::path::value_type* path){++deletes;std::error_code error;return fs::remove(path,error)&&!error;}
static int FixtureCopyD62(const fs::path::value_type* from,const fs::path::value_type* to,int failIfExists){++copies;if(failCopy)return 0;std::error_code error;return fs::copy_file(from,to,failIfExists?fs::copy_options::none:fs::copy_options::overwrite_existing,error)&&!error;}
struct CreatedFileD62 {std::ofstream stream;explicit CreatedFileD62(const fs::path& path):stream(path,std::ios::binary){}};
static HANDLE FixtureCreateD62(const fs::path::value_type* path,unsigned,unsigned,void*,unsigned,unsigned,void*){
 ++creates;if(fs::exists(path))return INVALID_HANDLE_VALUE;auto* handle=new CreatedFileD62(path);if(!handle->stream){delete handle;return INVALID_HANDLE_VALUE;}return handle;
}
static int FixtureCloseD62(HANDLE handle){auto* file=static_cast<CreatedFileD62*>(handle);file->stream.close();const bool ok=bool(file->stream);delete file;return ok;}
static int FixtureMoveD62(const fs::path::value_type* from,const fs::path::value_type* to,unsigned){++replaces;if(failReplace)return 0;std::error_code error;
#ifdef _WIN32
 return MoveFileExW(from,to,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
#else
 fs::rename(from,to,error);return !error;
#endif
}
static int FixtureModuleD62(unsigned,LPCWSTR,HMODULE* module){*module=reinterpret_cast<HMODULE>(std::uintptr_t(1));return 1;}
namespace kcd2vr::startup_s15 {
namespace fs=std::filesystem;
static fs::path ModulePath(HMODULE){return chosenDllPath;}
#define GetPrivateProfileStringW FixtureReadProfileD62
#define GetPrivateProfileIntW FixtureReadIntD62
#define WritePrivateProfileStringW FixtureWriteProfileD62
#define DeleteFileW FixtureDeleteD62
#define CopyFileW FixtureCopyD62
#define CreateFileW FixtureCreateD62
#define CloseHandle FixtureCloseD62
#define MoveFileExW FixtureMoveD62
#define GetModuleHandleExW FixtureModuleD62
#include "generated_pending_setup_d62.inc"
#undef GetPrivateProfileStringW
#undef GetPrivateProfileIntW
#undef WritePrivateProfileStringW
#undef DeleteFileW
#undef CopyFileW
#undef CreateFileW
#undef CloseHandle
#undef MoveFileExW
#undef GetModuleHandleExW
}
static void select(const TempIniFixtureD62& file){mainPath=file.path;chosenDllPath=file.directory/"dinput8.dll";profileWrites=creates=copies=replaces=deletes=mainReads=0;failWrites=failCopy=failReplace=false;}
static fs::path checkpoint(const fs::path& path){auto p=path;p+=".last-good.d62.bak";return p;}
int main(){
 using namespace kcd2vr::startup_s15;
 const std::string original="; full existing profile\r\n[Settings]\r\nEnabled=true\r\nVrRenderScale=0.9\r\n[OtherMod]\r\nUnknown=retain\r\n[VRIK.Fingers.123]\r\nPersonal=keep calibration\r\n";
 const std::string keep="[Settings]\nStartupSetupPendingSerial=choice-1\nStartupSetupHasGraphics=0\n";
 const std::string graphics="[Settings]\nStartupSetupPendingSerial=choice-2\nStartupSetupHasGraphics=1\nVrRenderScale=1.1\nVrSourceWidth=1800\nVrSourceHeight=1700\nVrGameFov=100\nVrSourceVerticalFovDegrees=99\nVrPresentationZoom=1\nVrFlatScreenScale=1.2\nStartupHeadset=psvr2\nEnableVrBalancedGraphics=true\n";
 // The old pending merge creates a sparse main file in this exact case.
 {
  TempIniFixtureD62 file(original);select(file);put(PendingPath(file.path),keep);fs::remove(file.path);
  const bool admitted=ApplyPendingBeforeConfig();
  if(admitted)std::fprintf(stderr,"PENDING_CREATE_OBSERVATION mainExists=%d mainBytes=%llu creates=%u writes=%u replaces=%u pendingExists=%d\n",fs::exists(file.path),(unsigned long long)(fs::exists(file.path)?fs::file_size(file.path):0),creates,profileWrites,replaces,fs::exists(PendingPath(file.path)));
  check(!admitted,"actual pending setup refuses to manufacture a missing main INI");
  check(!fs::exists(file.path)&&bytes(PendingPath(file.path))==keep,"missing main and pending choices are both preserved");
  check(!profileWrites&&!creates&&!copies&&!replaces&&!deletes&&!mainReads,"missing refusal precedes all main-profile native leaves");
  put(file.path,original);check(ApplyPendingBeforeConfig(),"restoring the complete INI allows the same pending choice to retry");
  check(bytes(file.path).find(original)==0&&!fs::exists(PendingPath(file.path)),"successful retry merges existing data before retiring pending choice");
 }
 for(int fault=0;fault<3;++fault){
  TempIniFixtureD62 file(original);select(file);put(PendingPath(file.path),keep);
  if(fault==0)put(file.path,"");
  else if(fault==1){fs::remove(file.path);fs::create_directory(file.path);}
  else fs::resize_file(file.path,16u*1024u*1024u+1u);
  check(!ApplyPendingBeforeConfig(),"actual pending merge refuses empty/nonregular/oversized main profile");
  check(bytes(PendingPath(file.path))==keep&&!profileWrites&&!creates&&!copies&&!replaces&&!deletes&&!mainReads,"read-proof refusal preserves pending bytes and performs no native mutation");
  check(fault==0?fs::file_size(file.path)==0:fault==1?fs::is_directory(file.path):fs::file_size(file.path)==16u*1024u*1024u+1u,"refused original remains unchanged");
 }
 {
  TempIniFixtureD62 file(original);select(file);
  check(ApplyPendingBeforeConfig()&&bytes(file.path)==original&&!profileWrites,"no pending profile leaves main file unchanged");
  put(PendingPath(file.path),"[Settings]\nStartupSetupHasGraphics=0\n");
  check(!ApplyPendingBeforeConfig()&&bytes(file.path)==original&&fs::exists(PendingPath(file.path)),"missing pending serial is retained without a main mutation");
 }
 {
  TempIniFixtureD62 file(original);select(file);put(PendingPath(file.path),graphics);
  check(ApplyPendingBeforeConfig(),"complete actual graphics-validation and commit path succeeds");
  check(bytes(file.path).find(original)==0&&bytes(checkpoint(file.path))==original,"setup checkpoint is exact pre-merge bytes, with unknown keys and calibration retained");
  check(value(file.path,L"Settings",L"VrRenderScale",L"")==L"1.1"&&value(file.path,L"Settings",L"VrSourceWidth",L"")==L"1800","existing selected settings are applied without new defaults");
  check(value(file.path,L"Settings",L"VrSourceVerticalFovDegrees",L"")==L"100"&&value(file.path,L"Settings",L"VrCorrectedCenterScaleFovDegrees",L"")==L"100","existing linked FOV migration remains intact");
  check(value(file.path,L"Settings",L"StartupHeadset",L"")==L"psvr2"&&value(file.path,L"Settings",L"VrPadGlyphStyle",L"")==L"4","existing explicit headset glyph choice remains intact");
  check(!fs::exists(PendingPath(file.path))&&creates==0,"valid current profile is copied, not created from defaults");
  {guard::Lease next(file.path);check(next.valid(),"pending lease is released before following LoadConfig/file-writer lease");}
  const auto applied=bytes(file.path);put(PendingPath(file.path),graphics);select(file);
  check(ApplyPendingBeforeConfig()&&bytes(file.path)==applied&&!profileWrites&&!replaces,"replayed serial only retires pending choice, preserving later current settings");
 }
 {
  TempIniFixtureD62 file(original);select(file);const auto invalid="[Settings]\nStartupSetupPendingSerial=invalid\nStartupSetupHasGraphics=1\nVrRenderScale=99\n";put(PendingPath(file.path),invalid);
  check(!ApplyPendingBeforeConfig()&&bytes(file.path)==original&&bytes(PendingPath(file.path))==invalid&&!profileWrites,"existing graphics validator still refuses invalid choice without losing either file");
 }
 for(int fault=0;fault<3;++fault){
  TempIniFixtureD62 file(original);select(file);put(PendingPath(file.path),keep);
  failCopy=fault==0;failWrites=fault==1;failReplace=fault==2;
  check(!ApplyPendingBeforeConfig(),"native copy/write/replace failure remains a failed pending apply");
  check(bytes(file.path)==original&&bytes(PendingPath(file.path))==keep,"native merge failure retains original and pending choices");
 }
 {
  TempIniFixtureD62 file(original);select(file);put(PendingPath(file.path),keep);fs::create_directory(checkpoint(file.path));put(checkpoint(file.path)/"retain","old");
  check(ApplyPendingBeforeConfig()&&bytes(file.path).find(original)==0,"best-effort checkpoint failure does not disable valid pending merge");
  check(bytes(checkpoint(file.path)/"retain")=="old","failed checkpoint leaves existing backup destination untouched");
 }
 std::printf("pending_setup_guard_d62: %u checks PASS; complete production pending/commit flow; native profile/file leaves explicit\n",checks);
}
