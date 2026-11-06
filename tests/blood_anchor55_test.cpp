// Actual production StartGush/TickGushes with only native engine leaves doubled.
#include "vrik/blood49.h"
#include "vrik/gore32.h"
#include "vrik/decap_joint44.h"
#include <atomic>
#include <cassert>
#include <vector>
#include <string>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {static std::vector<std::string> lines;void Write(const char* s)noexcept{lines.push_back(s);}}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;
namespace diagnostics::capture168 {enum class Domain{Combat};inline bool active(Domain,std::uint64_t){return true;}}
struct Q4{float x=0,y=0,z=0,w=1;};struct V3{float x=0,y=0,z=0;};struct ModelWorld6692{Q4 q{};V3 t{};};
static std::atomic<void*> g_last_player_entity{nullptr};static std::atomic<std::uintptr_t> g_material_contract116{1};static std::atomic<bool> g_contact_fx_fault178r12{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{2},g_weapon_request_epoch6694{3};
static std::uintptr_t WhBase(){return 1;}namespace retail_build112 {inline bool Ready(std::uintptr_t){return true;}}
static void* FlinchEntityById178r11(std::uint32_t id){return id==42?reinterpret_cast<void*>(0x42):nullptr;}
static void* CharacterForEntity6689(void* e){return e?reinterpret_cast<void*>(0x1234):nullptr;}
static bool ReadModelWorld6692(void*,ModelWorld6692& out){out={};return true;}
namespace gore32n {
namespace g=kcd2vr::gore32;
using LuaExec43=bool(*)(const char*)noexcept;static bool lua(const char*)noexcept{return true;}
static std::atomic<LuaExec43> g_lua_exec43{lua};
struct NeckFrame55 {void* character=nullptr;void* skeleton=nullptr;int joint=-1;g::V3 point{},up{};};
static bool neckValid=true;static g::V3 neckPoint{1,2,3};
static bool ReadNeckFrame55(void* ci,NeckFrame55& out){if(!neckValid)return false;out={ci,reinterpret_cast<void*>(0x5678),12,neckPoint,{0,0,1}};return true;}
static std::vector<g::V3> points;
static bool Blood(const float* p,const float*,std::uint32_t,const char*,float){points.push_back({p[0],p[1],p[2]});return true;}
}
#include "../src/subsystems/gore/native_blood.inl"
int main(){namespace b=kcd2vr::blood49;
 b::KillEvent k{};k.kill=b::Kill::Decap;k.target=42;k.identity=0x42;k.tick=1000;
 assert(!blood49n::NeckAttachment(42)); // no queued-call == renderer-attachment claim
 blood49n::StartGush(k,1000);assert(blood49n::g_gushes.any());blood49n::TickGushes(1000);
 assert(gore32n::points.size()==1&&gore32n::points.back().z==3);
 // The NPC root did not move; the actual named neck did. No old world/model source.
 gore32n::neckPoint={5,6,.5};blood49n::TickGushes(2000);
 assert(gore32n::points.size()==2&&gore32n::points.back().x==5&&gore32n::points.back().z==.5);
 gore32n::neckValid=false;blood49n::TickGushes(3100);assert(!blood49n::g_gushes.any()&&gore32n::points.size()==2);
 // An arbitrary lethal contact cannot invent an articulated wound anchor.
 k.kill=b::Kill::Lethal;blood49n::StartGush(k,4000);assert(!blood49n::g_gushes.any());
 k.kill=b::Kill::Impale;blood49n::StartGush(k,4000);assert(!blood49n::g_gushes.any());
 b::PartWound55 p{};p.serial=8;p.target=42;p.identity=0x42;p.tick=4000;p.epoch=2;p.request=3;p.point[0]=10;p.point[2]=1;
 b::partWound55.publish(p);blood49n::StartGush(k,4000);blood49n::TickGushes(4000);
 assert(gore32n::points.size()==3&&gore32n::points.back().x==10);
 p.tick=5000;p.point[0]=11;b::partWound55.publish(p);blood49n::TickGushes(5000);
 assert(gore32n::points.size()==4&&gore32n::points.back().x==11);
 // A stale/released or different serial anchor cannot keep a fountain at old point.
 blood49n::TickGushes(6100);assert(!blood49n::g_gushes.any()&&gore32n::points.size()==4);
 p.tick=7000;p.serial=9;b::partWound55.publish(p);blood49n::StartGush(k,7000);
 p.tick=8000;p.serial=10;b::partWound55.publish(p);blood49n::TickGushes(8000);assert(!blood49n::g_gushes.any());
 std::puts("PASS C55 actual gush scheduler follows current named neck/exact original impale part; missing/stale/changed anchors never fallback to stationary world/model root");
}
