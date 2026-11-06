#include "subsystems/experimental/live_settings_d39.h"
#include "vrik/candidate_defaults_d72.h"
#include <cstdio>
#include <map>
#include <string>
#include <array>
using namespace kcd2vr;
struct Menu {live_settings_d39::Settings live_d39;};
using File=std::map<std::string,std::string>;
static std::map<std::string,File> files;static bool copyFail=false;static std::string failKey;static unsigned writes=0,copies=0;
static constexpr int TRUE=1,FALSE=0,INVALID_FILE_ATTRIBUTES=-1;
int GetFileAttributesA(const char* p){return files.contains(p)?0:INVALID_FILE_ATTRIBUTES;}
int CopyFileA(const char* source,const char* target,int){++copies;if(copyFail||files.contains(target)||!files.contains(source))return FALSE;files[target]=files[source];return TRUE;}
int WritePrivateProfileStringA(const char*,const char* key,const char* value,const char* file){++writes;if(failKey==key)return FALSE;files[file][key]=value;return TRUE;}
int ReadIniIntExplicit(const char* file,const char*,const char* key,int fallback){auto p=files.find(file);return p==files.end()||!p->second.contains(key)?fallback:std::stoi(p->second[key]);}
bool ReadIniBoolExplicit(const char* file,const char*,const char* key,bool fallback){auto p=files.find(file);return p==files.end()||!p->second.contains(key)?fallback:p->second[key]=="true";}
static void read(Menu& out){
#include "instant_read_production.inc"
}
static void apply(const Menu& state){
#include "instant_apply_production.inc"
}
static bool save(const char* explicit_ini){bool ok=true;auto wb=[&](const char* k,bool v){return WritePrivateProfileStringA("Settings",k,v?"true":"false",explicit_ini)!=FALSE;};
#include "instant_save_production.inc"
return ok;}
static void load(const char* explicit_ini){
#include "instant_load_production.inc"
}
static void migrate(const std::string& configPathD62){const char* explicit_ini=configPathD62.c_str();
#include "instant_defaults_production.inc"
}
static unsigned checks=0,failures=0;
static void ck(bool yes,const char* why){++checks;if(!yes){++failures;std::fprintf(stderr,"FAIL %s\n",why);}}
static std::array<bool,5> values(const Menu& m){const auto& x=m.live_d39;return {x.instantActivitiesD72,x.instantInteractionsD72,x.instantCraftingD72,x.instantReloadsD72,x.instantCombatD72};}
int main(){
 for(unsigned mask=0;mask<32;++mask){Menu in;auto& v=in.live_d39;v.instantActivitiesD72=mask&1;v.instantInteractionsD72=mask&2;v.instantCraftingD72=mask&4;v.instantReloadsD72=mask&8;v.instantCombatD72=mask&16;apply(in);
  Menu got;read(got);ck(values(got)==values(in),"actual live apply/read preserve every switch combination");
  for(unsigned f=0;f<4;++f)ck(instant_d72::Requested(instant_d72::Family(f))==bool((mask&1)&&(mask&(2<<f))),"master gates each independent preference");
  ck(save("profile"),"actual persistent writes succeed");Menu defaults;apply(defaults);load("profile");read(got);ck(values(got)==values(in),"actual save/load retains independent choices even master OFF");
 }
 files.clear();load("empty");Menu got;read(got);ck(values(got)==std::array<bool,5>{true,false,false,false,true},"R124: missing keys use Miki's INI (activities ON, interactions/crafting/reloads OFF, combat ON)");
 for(unsigned f=0;f<4;++f)instant_d72::Publish(instant_d72::Family(f),instant_d72::State::Retiring);
 read(got);for(unsigned f=0;f<4;++f)ck(got.live_d39.instantStateD72[f]==unsigned(instant_d72::State::Retiring),"actual UI reads owner cleanup status");
 files.clear();files["game"]={{"VrHeight","1.735"},{"VrInstantReloadsD72","false"},{"VrTwoHandPrimaryBiasD65","0.7"}};auto before=files["game"];writes=copies=0;migrate("game");
 ck(files["game.pre-D72.bak"]==before&&copies==1,"original user config backed up exactly once");
 ck(files["game"]["VrHeight"]=="1.735","unrelated calibration preserved");
 for(const auto& s:defaults_d72::settings)ck(files["game"][s.key]==s.value,"all requested profile settings migrate");
 ck(files["game"]["VrCandidateDefaultsD72"]=="1","migration committed after all writes");
 files["game"]["VrInstantReloadsD72"]="false";files["game"]["VrTwoHandPrimaryBiasD65"]="0.33";writes=0;migrate("game");
 ck(writes==0&&files["game"]["VrInstantReloadsD72"]=="false"&&files["game"]["VrTwoHandPrimaryBiasD65"]=="0.33","later menu choices never overwritten on startup");
 ck(files["game.pre-D72.bak"]==before,"original backup remains unchanged");
 files.clear();files["game"]=before;copyFail=true;writes=0;migrate("game");ck(!writes&&files["game"]==before,"failed backup prevents migration writes");copyFail=false;
 files["game.pre-D72.bak"]={{"existing","backup"}};migrate("game");ck(files["game.pre-D72.bak"]==File{{"existing","backup"}},"existing backup never overwritten");
 files.clear();failKey="VrInstantCraftingD72";migrate("new");ck(!files["new"].contains("VrCandidateDefaultsD72"),"partial write cannot commit profile marker");failKey.clear();migrate("new");ck(files["new"]["VrCandidateDefaultsD72"]=="1","uncommitted migration retries without losing original backup");
 files.clear();copies=0;migrate("new");ck(copies==0&&files["new"]["VrCandidateDefaultsD72"]=="1","absent config receives profile without nonexistent-file backup");
 std::printf("D72 instant settings/migration %u/%u (actual bindings; explicit Win32 IO leaves)\n",checks-failures,checks);return failures?1:0;
}
