#include "vrik/melee_tuning_c54.h"
#include "vrik/finger_tuning40.h"
#include <map>
#include <string>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cstring>
namespace kcd2vr { }
using namespace kcd2vr;
struct State {std::array<melee_tuning_c54::Values,17> melee_types_c54{};float punch_sensitivity_c54=1,block_sensitivity_h29=1,blade_axis_cosine_h41=.80f;
 bool craft_trial_h41=false,bypass_mod_vetoes_h41=false,archer_trial_h41=false;float chest_trial_h41=0;
 float punch_travel_h41=1,guard_width_h41=.5f,guard_height_h41=.2f,pb_window_h41=2,ms_window_h41=1.5f;float finger_middle_lateral_c54[2][5]{};float finger_middle_shift_mm_h31[2]{15,15},finger_middle_distal_medial_h31[2]{10,10};};
static std::map<std::string,std::string> ini;static bool failWrite=false;
static bool wf(const char* key,float value){if(failWrite)return false;char text[64]{};std::snprintf(text,sizeof(text),"%.6g",double(value));ini[key]=text;return true;}
// Explicit host leaves for the additional current fields in the same actual settings block.
namespace kcd2vr::subsystems::alchemy::reach_craft1 {inline std::atomic<bool> enabled{false};}
namespace kcd2vr::frik_pose_probe {inline float chest=0;inline bool archer=false;inline float ChestPitchShareCB(){return chest;}inline bool ArcherStanceCB(){return archer;}inline void SetChestPitchShareCB(float v){chest=v;}inline void SetArcherStanceCB(bool v){archer=v;}}
namespace kcd2vr::player_window_h26 {inline std::atomic<float> perfectBlockMultiplier{2},multiplier{1.5f};}
static bool wb(const char* key,bool v){return wf(key,v?1.f:0.f);}
using DWORD=unsigned long;
namespace kcd2vr::debug_trace {inline void Write(const char*){}}
static DWORD H29GetPrivateProfileStringA(const char*,const char* key,const char* fallback,char* out,DWORD capacity,const char*){
 const auto it=ini.find(key);const std::string v=it==ini.end()?fallback:it->second;const auto n=std::min(std::size_t(capacity-1),v.size());std::memcpy(out,v.data(),n);out[n]=0;return DWORD(n);
}
#define GetPrivateProfileStringA H29GetPrivateProfileStringA
#include "generated/h29_settings.inl"
#undef GetPrivateProfileStringA
static void bladeAxisSettingsChecks(){
 ini.clear();melee_tuning_c54::bladeAxisCosineH41=.72f;H29Load();State s{};H29Fill(s);
 assert(s.blade_axis_cosine_h41==.80f&&melee_tuning_c54::bladeAxisH41()==.80);
 for(float value:{.60f,.72f,.80f,.95f}){
  s.blade_axis_cosine_h41=value;H29Apply(s);assert(melee_tuning_c54::bladeAxisCosineH41==value);
  assert(H29Save());melee_tuning_c54::bladeAxisCosineH41=.81f;H29Load();H29Fill(s);
  assert(s.blade_axis_cosine_h41==value&&ini.count("VrBladeAxisAlignmentH41")==1);
 }
 for(const char* invalid:{"nan","inf","0.59","0.96","-9","garbage"}){
  ini["VrBladeAxisAlignmentH41"]=invalid;H29Load();H29Fill(s);
  assert(s.blade_axis_cosine_h41==.80f&&melee_tuning_c54::bladeAxisH41()==.80);
 }
 for(float invalid:{.59f,.96f,std::numeric_limits<float>::quiet_NaN()}){
  s.blade_axis_cosine_h41=invalid;H29Apply(s);assert(melee_tuning_c54::bladeAxisH41()==.80);
 }
 ini.clear();
}
int main(){bladeAxisSettingsChecks();State s{};H29Load();H29Fill(s);assert(s.finger_middle_shift_mm_h31[0]==15&&s.finger_middle_shift_mm_h31[1]==15&&s.finger_middle_distal_medial_h31[0]==10&&s.finger_middle_distal_medial_h31[1]==10);s.finger_middle_shift_mm_h31[0]=11.5;s.finger_middle_shift_mm_h31[1]=17;s.finger_middle_distal_medial_h31[0]=9;s.finger_middle_distal_medial_h31[1]=-4.5;for(unsigned c=0;c<17;++c){assert(s.melee_types_c54[c].cut==1&&s.melee_types_c54[c].stab==1&&s.melee_types_c54[c].intentH29==1&&s.melee_types_c54[c].sensitivityH29==1);s.melee_types_c54[c].cut=.5f+.05f*c;s.melee_types_c54[c].stab=2.f-.05f*c;s.melee_types_c54[c].intentH29=.6f+.05f*c;s.melee_types_c54[c].sensitivityH29=1.9f-.05f*c;}s.punch_sensitivity_c54=.9f;s.block_sensitivity_h29=1.3f;for(int h=0;h<2;++h)for(int f=0;f<5;++f)s.finger_middle_lateral_c54[h][f]=float(h*5+f)-2;H29Apply(s);assert(H29Save()&&ini.size()==111);for(unsigned c=0;c<17;++c)melee_tuning_c54::set(int(c),{});melee_tuning_c54::punchSensitivity=1;melee_tuning_c54::blockSensitivityH29=1;for(int h=0;h<2;++h)for(int f=0;f<5;++f)finger40::setMiddleLateralC54(h,f,0);finger40::setShiftMmH31(0,0);finger40::setShiftMmH31(1,0);finger40::setDistalMedialDegH31(0,0);finger40::setDistalMedialDegH31(1,0);H29Load();State restored{};H29Fill(restored);for(int h=0;h<2;++h){assert(restored.finger_middle_shift_mm_h31[h]==s.finger_middle_shift_mm_h31[h]);assert(restored.finger_middle_distal_medial_h31[h]==s.finger_middle_distal_medial_h31[h]);}for(unsigned c=0;c<17;++c){assert(std::abs(restored.melee_types_c54[c].cut-s.melee_types_c54[c].cut)<1e-5);assert(std::abs(restored.melee_types_c54[c].stab-s.melee_types_c54[c].stab)<1e-5);assert(std::abs(restored.melee_types_c54[c].intentH29-s.melee_types_c54[c].intentH29)<1e-5);assert(std::abs(restored.melee_types_c54[c].sensitivityH29-s.melee_types_c54[c].sensitivityH29)<1e-5);}assert(restored.punch_sensitivity_c54==s.punch_sensitivity_c54&&restored.block_sensitivity_h29==s.block_sensitivity_h29);for(int h=0;h<2;++h)for(int f=0;f<5;++f)assert(restored.finger_middle_lateral_c54[h][f]==s.finger_middle_lateral_c54[h][f]);ini["VrFingerMiddleBaseShiftMmH31_R"]="nan";ini["VrFingerMiddleBaseShiftMmH31_L"]="99";ini["VrFingerMiddleDistalMedialDegH31_R"]="nan";ini["VrFingerMiddleDistalMedialDegH31_L"]="-99";ini["VrMeleeTypeC54_4_Cut"]="nan";ini["VrFistBlockSensitivityH29"]="99";ini["VrMeleeTypeH29_4_Intent"]="nan";ini["VrMeleeTypeH29_4_Sensitivity"]="99";H29Load();assert(melee_tuning_c54::read(4).cut==1&&melee_tuning_c54::blockH29()==1.5f&&melee_tuning_c54::read(4).intentH29==1.f&&melee_tuning_c54::read(4).sensitivityH29==2.f);assert(finger40::getShiftMmH31(0)==15&&finger40::getShiftMmH31(1)==20&&finger40::getDistalMedialDegH31(0)==10&&finger40::getDistalMedialDegH31(1)==-20);failWrite=true;assert(!H29Save());std::puts("h29_settings: actual menu Apply/Fill and serialized INI Save/Load, 111 current independent keys, bounds, NaN and write failure PASS");return 0;}
