// Dependencies for the actual calibration_runtime.inl included by pose_adapter.
// No production function body is replaced. INI transport is in-memory.
#include "../src/vrik/calibration_profile.h"
#include <map>
using DWORD=unsigned long;
constexpr int MAX_PATH=260;
static std::map<std::string,std::string> poseIni;
static DWORD GetPrivateProfileSectionNamesA(char*out,DWORD capacity,const char*path){
 std::string joined,last;const std::string prefix=std::string(path?path:"")+"|";
 for(const auto& kv:poseIni){if(kv.first.compare(0,prefix.size(),prefix)!=0)continue;
  const auto end=kv.first.find('|',prefix.size());if(end==std::string::npos)continue;
  const auto name=kv.first.substr(prefix.size(),end-prefix.size());if(name==last)continue;
  joined+=name;joined.push_back('\0');last=name;
 }
 joined.push_back('\0');if(capacity<2)return 0;
 if(joined.size()>capacity){std::memcpy(out,joined.data(),capacity-2);out[capacity-2]=out[capacity-1]=0;return capacity-2;}
 std::memcpy(out,joined.data(),joined.size());return joined.size()-1;
}

static DWORD GetPrivateProfileStringA(const char*s,const char*k,const char*d,char*out,DWORD n,const char*p){
 auto it=poseIni.find(std::string(p)+"|"+s+"|"+k);const std::string value=it==poseIni.end()?d:it->second;const auto len=std::min<std::size_t>(value.size(),n-1);std::memcpy(out,value.data(),len);out[len]=0;return len;
}
static int WritePrivateProfileStringA(const char*s,const char*k,const char*v,const char*p){if(!s&&!k&&!v)return 0;if(!s||!k||!v)return 0;poseIni[std::string(p)+"|"+s+"|"+k]=v;return 1;}
static kcd2vr::core::SnapshotMailbox<kcd2vr::calibration_profile::Arms> g_arm_profile66100;
static std::atomic<bool> g_saved_arm_ready66100{false},g_cal_reanchor66100{false},g_cal_center_reanchor125{false},g_arm_capture_active66100{false},g_finger_capture66100{false};
static std::atomic<std::uint64_t> g_arm_save_generation66100{0},g_capture_session66100{0};
static std::atomic<void*>g_henry_item_attach_manager{nullptr};
static bool mockHandsEmpty=true;
static bool NativeHandKey6697(void*,int,std::uint64_t&key){key=mockHandsEmpty?0:10;return true;}
static std::atomic<float> g_cal_pos_l_x{0},g_cal_pos_l_y{0},g_cal_pos_l_z{0},g_cal_pos_r_x{0},g_cal_pos_r_y{0},g_cal_pos_r_z{0};
static std::atomic<float> g_cal_rot_l_x{0},g_cal_rot_l_y{0},g_cal_rot_l_z{0},g_cal_rot_l_w{1},g_cal_rot_r_x{0},g_cal_rot_r_y{0},g_cal_rot_r_z{0},g_cal_rot_r_w{1};
static std::atomic<bool>g_cal_native_anchor_pending{false},g_vrik_calibration_requested{false};
// V66.107 production state is internal to frik_pose_probe.cpp. The portable pose
// fixture supplies equivalent storage so the unchanged production .inl files compile.
static std::atomic<std::uint64_t> g_native_use_until107[2]{},g_native_use_serial107[2]{};
static std::atomic<std::uint64_t> g_native_wrist_until107[2]{},g_native_wrist_retired107[2]{};
static std::atomic<bool> g_eye_floor_valid107{false};
static std::atomic<float> g_eye_floor_m107{0.f},g_standing_eye_floor_m107{0.f};
static std::atomic<bool> g_standing_eye_save107{false};
// C43 calibration restore storage + the vertical-shift accessor (production: frik_pose_probe.cpp).
#include "../src/vrik/calibration_restore43.h"
static std::atomic<bool> g_cal_auto_restore43{true},g_cal_anchor_saved43{false},g_cal_anchor_save43{false},g_cal_restore_force43{false};
static std::atomic<float> g_cal_anchor_x43{0},g_cal_anchor_y43{0},g_cal_anchor_z43{0};
static std::atomic<std::uint64_t> g_cal_restore_count43{0},g_cal_drift_count43{0};
static float StandingEyeShift43() noexcept {return kcd2vr::calibration_restore43::verticalShift(g_cal_auto_restore43.load()||g_cal_restore_force43.load(),g_eye_floor_valid107.load(),g_eye_floor_m107.load(),g_standing_eye_floor_m107.load());}

#include "../src/diagnostics/lab_state.h"
namespace diagnostics=kcd2vr::diagnostics;
#ifndef KCD2VR_POSE_FIXTURE_CLOCK
#define KCD2VR_POSE_FIXTURE_CLOCK 10000
#endif
static std::uint64_t GetTickCount64(){return KCD2VR_POSE_FIXTURE_CLOCK;}
static bool PhysEmpty6698(std::uint64_t key){return key==0||key==~std::uint64_t(0);}
