// Compose the production host slots: UI apply -> INI save -> reload -> UI read.
// This checks the actual saved keys/units, not a duplicate settings model.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <string>
#include "subsystems/direct/npc_tuning.h"
#include "subsystems/direct/npc_defense.h"
#include "subsystems/direct/combat_router_h3.h"
namespace kcd2vr::npc_difficulty {bool Ready() noexcept{return true;}}
namespace kcd2vr::npc_defense {bool Ready() noexcept{return true;}}
namespace kcd2vr::npc_unarmed_spacing {bool Ready() noexcept{return true;}const char* Failure() noexcept{return "HOST_READY";}}
namespace kcd2vr {
struct State {
 bool touch_attack_direction=true;
#define KCD2VR_M2_HOST_SLOT 285
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
};
std::atomic<bool> enable_touch_attack_direction_{true};
std::atomic<int> g_npc_block39{0};
std::map<std::string,float> ini;
void Apply(const State& state){
#define KCD2VR_M2_HOST_SLOT 73
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
State Read(){State out;
#define KCD2VR_M2_HOST_SLOT 26
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 return out;}
void Save(){bool ok=true;
 auto wf=[](const char* key,float value){ini[key]=value;return true;};
 auto wi=wf;auto wb=wf;
#define KCD2VR_M2_HOST_SLOT 118
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 if(!ok)std::abort();}
float ReadIniFloatExplicit(const char*,const char*,const char* key,float fallback){
 const auto found=ini.find(key);return found==ini.end()?fallback:found->second;}
int ReadIniIntExplicit(const char* p,const char* section,const char* key,int fallback){return int(ReadIniFloatExplicit(p,section,key,float(fallback)));}
void Load(){const char* explicit_ini="fixture";
#define KCD2VR_M2_HOST_SLOT 155
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
}
int main(){using namespace kcd2vr;unsigned checks=0;
 const auto check=[&](bool yes){++checks;if(!yes){std::fprintf(stderr,"FAIL check%u\n",checks);std::exit(1);}};
 const auto near=[](float a,float b){return std::fabs(a-b)<.0001f;};
 ini.clear();Load();auto state=Read();
 check(near(state.npc_difficulty_percent,0));check(near(state.npc_defense_speed,1));
 for(const float percent:{0.f,1.f,37.f,50.f,100.f})for(const float speed:{1.f,1.5f,2.f,3.f}){
  state.npc_difficulty_percent=percent;state.npc_defense_speed=speed;Apply(state);Save();
  check(near(ini.at("VrNpcDifficulty"),percent));check(near(ini.at("VrNpcDefenseSpeed"),speed));
  npc_difficulty::difficulty=0; npc_defense::speed=1;Load();const auto roundtrip=Read();
  check(near(roundtrip.npc_difficulty_percent,percent));check(near(roundtrip.npc_defense_speed,speed));
  check(roundtrip.npc_difficulty_ready&&roundtrip.npc_defense_ready);
 }
 // Each control changes independently, including native0/restoration.
 state.npc_difficulty_percent=100;state.npc_defense_speed=1;Apply(state);check(near(npc_difficulty::difficulty,1)&&near(npc_defense::speed,1));
 state.npc_difficulty_percent=0;state.npc_defense_speed=1.5f;Apply(state);check(near(npc_difficulty::difficulty,0)&&near(npc_defense::speed,1.5f));
 for(const float invalid:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
  state.npc_difficulty_percent=invalid;state.npc_defense_speed=invalid;Apply(state);
  check(near(npc_difficulty::difficulty,0)&&near(npc_defense::speed,1));
  ini["VrNpcDifficulty"]=invalid;ini["VrNpcDefenseSpeed"]=invalid;Load();
  check(near(npc_difficulty::difficulty,0)&&near(npc_defense::speed,1));
 }
 ini["VrNpcDifficulty"]=-20;ini["VrNpcDefenseSpeed"]=-5;Load();check(near(npc_difficulty::difficulty,0)&&near(npc_defense::speed,1));
 ini["VrNpcDifficulty"]=200;ini["VrNpcDefenseSpeed"]=20;Load();check(near(npc_difficulty::difficulty,1)&&near(npc_defense::speed,3));
 // D34 per-type keys: defaults, round trip, clamp, invalid -> native 1.
 ini.clear();Load();check(near(npc_defense::blockSpeed,1.4f)&&near(npc_defense::parrySpeed,1.33f)&&near(npc_defense::dodgeSpeed,1.25f)&&!npc_defense::raiseExperiment);
 check(npc_defense::Wanted()&&near(npc_defense::DodgeFactor(),1.25f)&&near(npc_defense::RaiseFactor(false),1)&&near(npc_defense::RaiseFactor(true),1));
 Save();check(near(ini.at("VrNpcBlockAnimSpeed"),1.4f)&&near(ini.at("VrNpcParryAnimSpeed"),1.33f)&&near(ini.at("VrNpcDodgeAnimSpeed"),1.25f)&&near(ini.at("VrNpcDefenseRaiseExperiment"),0));
 ini["VrNpcBlockAnimSpeed"]=2;ini["VrNpcParryAnimSpeed"]=20;ini["VrNpcDodgeAnimSpeed"]=std::numeric_limits<float>::quiet_NaN();ini["VrNpcDefenseRaiseExperiment"]=1;Load();
 check(near(npc_defense::blockSpeed,2)&&near(npc_defense::parrySpeed,3)&&near(npc_defense::dodgeSpeed,1)&&npc_defense::raiseExperiment);
 Save();npc_defense::blockSpeed=1;npc_defense::raiseExperiment=false;Load();check(near(npc_defense::blockSpeed,2)&&npc_defense::raiseExperiment);
 ini["VrNpcDodgeAnimSpeed"]=-3;ini["VrNpcDefenseRaiseExperiment"]=0;Load();check(near(npc_defense::dodgeSpeed,1)&&!npc_defense::Wanted());
 check(near(npc_defense::speed,1)); // legacy key untouched by per-type keys
 std::printf("PASS %u production NPC settings assertions\n",checks);
}
