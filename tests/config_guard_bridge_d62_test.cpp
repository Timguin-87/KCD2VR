// Executes actual extracted LoadConfig/SaveMenuSettings guard prefixes and the
// complete calibration writer functions. Native profile/parser APIs are leaves;
// current-file reading, checkpoint I/O and serialization use production code.
#include "../src/core/config_file_guard_d62.h"
#include "../src/vrik/calibration_profile.h"
#include "temp_ini_fixture_d62.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <map>
#include <vector>
#ifndef _WIN32
using DWORD=unsigned long;
constexpr unsigned MAX_PATH=260,MB_OK=0,MB_ICONERROR=16;
constexpr int FALSE=0;
#endif
namespace fs=std::filesystem;
namespace guard=kcd2vr::core::config_file_guard_d62;
static std::string chosenPath,parserPath,calibrationPath;
static bool moduleResolved=true,failNativeWrites=false;
static unsigned checks=0,parserCalls=0,calibrationCalls=0,profileWrites=0,messageBoxes=0,bindingPhaseEnteredD62=0;
static std::map<std::string,std::string> nativeProfile;
static std::vector<std::string> traces;
static void check(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL D62 config bridge %u: %s\n",checks,why);std::abort();}}
static std::string read(const fs::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};}
static fs::path backup(const fs::path& path){auto b=path;b+=".last-good.d62.bak";return b;}
static std::string key(const char* section,const char* name,const char* path){return std::string(path)+"|"+section+"|"+name;}
static bool GetLoadedModuleSiblingPath(const char*,char* out,DWORD capacity){if(!moduleResolved||chosenPath.size()+1>capacity)return false;std::memcpy(out,chosenPath.c_str(),chosenPath.size()+1);return true;}
static int FixtureMessageBoxD62(void*,const char* message,const char*,unsigned){++messageBoxes;traces.emplace_back(message);return 1;}
static int FixtureWriteProfileD62(const char* section,const char* name,const char* value,const char* path){
 if(!section&&!name&&!value)return 0; // actual documented flush result is zero
 ++profileWrites;if(failNativeWrites)return 0;
 nativeProfile[key(section,name,path)]=value;
 std::ofstream out(path,std::ios::binary|std::ios::app);out<<"\n["<<section<<"]\n"<<name<<'='<<value<<'\n';out.close();return out?1:0;
}
static bool FixtureMigrationD62(const char* path){return FixtureWriteProfileD62("Settings","FixtureAdoptionMarker","1",path)!=0;}
namespace DetourModKit {struct Logger{static void configure(const char*,const char*){}};}
namespace config {struct Ini {void load(const char* path){++parserCalls;parserPath=path;}};}
namespace kcd2vr {
namespace debug_trace {static void Write(const char* value){traces.emplace_back(value);}}
namespace diagnostic_trace {static void Write(const char* value){traces.emplace_back(value);}}
namespace frik_pose_probe {static void ConfigureCalibrationPersistence(const char* path){++calibrationCalls;calibrationPath=path;}}
namespace cp100=calibration_profile;
static char g_calibration_ini66100[MAX_PATH*2]{};
static std::string ReadProfileKey66100(const char* section,const char* name){const auto found=nativeProfile.find(key(section,name,g_calibration_ini66100));return found==nativeProfile.end()?std::string{}:found->second;}
#define MessageBoxA FixtureMessageBoxD62
#define WritePrivateProfileStringA FixtureWriteProfileD62
#include "generated_config_guard_d62.inc"
#undef MessageBoxA
#undef WritePrivateProfileStringA
}
static void resetCounts(){parserCalls=calibrationCalls=profileWrites=messageBoxes=bindingPhaseEnteredD62=0;traces.clear();guard::loadFailureShown=false;guard::checkpointFailureShown=false;}
int main(){
 using namespace kcd2vr;
 const std::string original="; owner comment\r\n[Settings]\r\nEnabled=true\r\n[OtherMod]\r\nRetain=personal\r\n[VRIK.Fingers.123]\r\nBank0=unchanged\r\n";
 TempIniFixtureD62 file(original);chosenPath=file.name;std::strcpy(g_calibration_ini66100,file.name.c_str());
 const cp100::Arms arms{1.44,.76,.76,.22};
 fs::remove(file.path);resetCounts();
 check(!GuardedLoadD62(),"actual startup guard refuses missing current INI");
 check(parserCalls==0&&calibrationCalls==0&&profileWrites==0&&bindingPhaseEnteredD62==0,"refusal precedes parser, calibration reads, bindings and migration phase");
 check(messageBoxes==1&&!fs::exists(file.path),"missing startup reports one actionable error without generating defaults");
 check(!GuardedLoadD62()&&messageBoxes==1,"repeated missing startup does not repeat the modal error");
 check(!GuardedMenuSaveD62()&&profileWrites==0&&!fs::exists(file.path),"actual menu guard cannot reconstruct a missing profile from current settings");
 check(!SaveProfile66100("VRIK.ArmCalibration",arms)&&profileWrites==0,"actual calibration bank writer cannot create a calibration-only INI");
 check(!SaveHeadKeyD62("StandingEyeHeightM","1.70")&&profileWrites==0,"actual head calibration writer also refuses a missing complete profile");
 {
  std::ofstream empty(file.path);empty.close();resetCounts();
  check(!GuardedLoadD62()&&!GuardedMenuSaveD62()&&!SaveHeadKeyD62("StandingEyeHeightM","1.70"),"all actual write entry points refuse an empty current profile");
  check(profileWrites==0&&parserCalls==0&&fs::file_size(file.path)==0,"empty-file refusal retains the original and does no native writes");
 }
 fs::remove(file.path);fs::create_directory(file.path);resetCounts();
 check(!GuardedLoadD62()&&!GuardedMenuSaveD62()&&!SaveProfile66100("VRIK.ArmCalibration",arms),"directory path cannot enter any guarded writer");
 check(profileWrites==0&&parserCalls==0,"directory refusal performs no parser or write leaf calls");
 fs::remove(file.path);{std::ofstream out(file.path,std::ios::binary);out<<original;}
 resetCounts();check(GuardedLoadD62(),"actual startup prefix admits a complete current profile");
 check(parserCalls==1&&calibrationCalls==1&&bindingPhaseEnteredD62==1&&profileWrites==1,"normal startup reaches existing consumers after the read proof");
 check(parserPath==fs::absolute(file.path).string()&&calibrationPath==parserPath,"parser and calibration use the same verified absolute path");
 check(read(backup(file.path))==original&&read(file.path).find(original)==0,"startup checkpoint is pre-migration bytes and unknown data remains");
 auto beforeMenu=read(file.path);resetCounts();
 check(GuardedMenuSaveD62()&&profileWrites==3,"actual menu per-key bool/int/float writers remain operational");
 check(read(backup(file.path))==beforeMenu&&read(file.path).find(original)==0,"menu checkpoints the current file and retains unrelated sections");
 const auto beforeBank=read(file.path);resetCounts();
 check(SaveProfile66100("VRIK.ArmCalibration",arms),"actual bank writer retains checksum/readback behavior under shared lease");
 check(profileWrites>0&&read(backup(file.path))==beforeBank,"bank checkpoint captures the latest menu edits");
 const auto beforeHead=read(file.path);resetCounts();
 check(SaveHeadKeyD62("StandingEyeHeightM","1.7312")&&profileWrites==1,"actual head writer saves under the same lease");
 check(read(backup(file.path))==beforeHead&&read(file.path).find(original)==0,"head checkpoint includes latest calibration and preserves unrelated bytes");
 failNativeWrites=true;const auto beforeFailure=read(file.path);resetCounts();
 check(!GuardedMenuSaveD62()&&!SaveHeadKeyD62("StandingEyeHeightM","1.8"),"native per-key failures still propagate failure");
 check(read(file.path)==beforeFailure,"failing native writer leaf does not trigger a reset or restore");failNativeWrites=false;
 fs::remove(backup(file.path));fs::create_directory(backup(file.path));resetCounts();
 check(GuardedLoadD62()&&GuardedMenuSaveD62()&&SaveHeadKeyD62("StandingEyeHeightM","1.73"),"checkpoint failure never disables valid startup/menu/calibration");
 check(messageBoxes==0&&guard::checkpointFailureShown,"best-effort backup failure is diagnostic, without a startup-blocking modal");
 fs::remove_all(backup(file.path));
 // DetourModKit resolves relative input differently from Win32's explicit INI
 // API. The fallback must feed both consumers the checked absolute file.
 const auto previousDirectory=fs::current_path();fs::current_path(file.directory);moduleResolved=false;resetCounts();
 check(GuardedLoadD62()&&parserPath==fs::absolute(file.path).string(),"unresolved-module fallback is converted to the verified absolute path before parser load");
 check(read(file.path).find(original)==0,"fallback migration still uses the exact checked file");
 moduleResolved=true;fs::current_path(previousDirectory);
 std::printf("config_guard_bridge_d62: %u checks PASS; actual guard prefixes/calibration writers; native profile/parser leaves explicit\n",checks);
}
