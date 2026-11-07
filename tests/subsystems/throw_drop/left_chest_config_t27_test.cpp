#include <cassert>
#include <map>
#include <string>
#include "vrik/weapon_release_t1.h"
namespace action_time_owner59c11 {static bool value=false;bool ThrowingSupplyT2(){return value;}void SetThrowingSupplyT2(bool x){value=x;}}
std::map<std::string,bool> values;int rightMode=0;
struct Ini{} ini;Ini* explicit_ini=&ini;
bool ReadIniBoolExplicit(Ini*,const char*,const char* key,bool fallback){auto i=values.find(key);return i==values.end()?fallback:i->second;}
int ReadIniIntExplicit(Ini*,const char*,const char*,int){return rightMode;}
bool wb(const char* key,bool v){values[key]=v;return true;}bool wi(const char*,int v){rightMode=v;return true;}
using namespace kcd2vr;
void load(){
#define KCD2VR_CONFIG_LOAD_THROW_DROP0
#define KCD2VR_CONFIG_LOAD_THROW_DROP1
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_LOAD_THROW_DROP0
#undef KCD2VR_CONFIG_LOAD_THROW_DROP1
}
void save(){bool ok=true;
#define KCD2VR_CONFIG_SAVE_THROW_DROP0
#define KCD2VR_CONFIG_SAVE_THROW_DROP1
#include "subsystems/throw_drop/config.inl"
#undef KCD2VR_CONFIG_SAVE_THROW_DROP0
#undef KCD2VR_CONFIG_SAVE_THROW_DROP1
assert(ok);}
int main(){using namespace kcd2vr;
values["VrThrowingSupply"]=false;load();assert(!action_time_owner59c11::value&&weapon_release_t1::Mode()==0); // R124: missing key = OFF (Miki's INI)
action_time_owner59c11::SetThrowingSupplyT2(false);save();action_time_owner59c11::value=true;load();assert(!action_time_owner59c11::value&&weapon_release_t1::Mode()==0);
action_time_owner59c11::SetThrowingSupplyT2(true);save();load();assert(action_time_owner59c11::value&&weapon_release_t1::Mode()==0);
rightMode=1;values["VrLeftChestKnives"]=false;load();assert(!action_time_owner59c11::value&&weapon_release_t1::Mode()==1);
values["VrLeftChestKnives"]=true;rightMode=0;load();assert(action_time_owner59c11::value&&weapon_release_t1::Mode()==0);
assert(values["VrThrowingSupply"]==false);puts("PASS actual config: R124 default-off, persisted OFF, legacy false independent, RIGHT AUTO unchanged");}
