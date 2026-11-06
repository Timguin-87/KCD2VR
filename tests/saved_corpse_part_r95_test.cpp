#define NOMINMAX
#ifdef _WIN32
#include <Windows.h>
#else
#include "corpse_native_host_r96.h"
#endif
#include "vrik/part_namespace62.h"
#include "subsystems/physics/load_fault_recovery_r96.h"
#include "vrik/part_anchor59c7g.h"
#include "subsystems/gore/corpse_contact_h7.h"
#include "subsystems/experimental/claude_options_h8.h"
#include <atomic>
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <cstdio>
namespace n=kcd2vr::part_namespace62;
namespace ph=kcd2vr::physical;
static std::unordered_map<std::uintptr_t,unsigned char> mem;
static std::uintptr_t base=0;
constexpr std::uintptr_t ent=0x1000,proxy=0x3000,ci=0x4000,model=0x6000,joints=0x7000,body=0x8000,rootBody=0x9000,oldBody=0xDEAD0000;
static unsigned queries=0,reads=0,oldReads=0;static int mode=0;
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
template<class T> void put(std::uintptr_t p,const T& v){const auto* x=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof v;++i)mem[p+i]=x[i];}
template<class T> bool Read(const void* ptr,T& out)noexcept{auto p=reinterpret_cast<std::uintptr_t>(ptr);++reads;if(p>=oldBody&&p<oldBody+4096){++oldReads;return false;}auto* x=reinterpret_cast<unsigned char*>(&out);for(unsigned i=0;i<sizeof out;++i){auto it=mem.find(p+i);if(it==mem.end())return false;x[i]=it->second;}return true;}
static std::uintptr_t WhBase()noexcept{return base;}
static bool SupportedWhBuild(std::uintptr_t b)noexcept{return b==base;}
static void* FlinchEntityById178r11(std::uint32_t id)noexcept{return id==456?reinterpret_cast<void*>(ent):nullptr;}
static void* CharacterForEntity6689(void* e)noexcept{return e==reinterpret_cast<void*>(ent)?reinterpret_cast<void*>(ci):nullptr;}
static bool PartAnchorPins59c7g()noexcept{return true;}
namespace kcd2vr::combat_diag_probe {bool NativeCharacterEntryQualified63(std::uintptr_t)noexcept{return false;}}
static int __fastcall Status(void* b,void* ptr){assert(b==reinterpret_cast<void*>(body));++queries;auto& s=*static_cast<kcd2vr::partanchor59c7g::Status*>(ptr);if(s.index>=0){if(s.index!=0)return 0;s.part=379;}else if(s.part!=379)return 0;
 if(mode==1)return 0;
 if(mode==4){RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);return 0;}
 s.position[0]=s.position[1]=s.position[2]=0;s.rotation[0]=s.rotation[1]=s.rotation[2]=0;s.rotation[3]=1;s.scale=1;
 if(mode==2)s.part=179;if(mode==3)put(ci+0x7A0,oldBody);if(mode==5)put(ci+0x7B4,std::uint8_t(0));return 1;
}
static std::atomic<void*> g_last_player_entity{nullptr};
namespace retail_build112 {static bool Ready(std::uintptr_t b){return b==base;}}
namespace kcd2vr::frik_pose_probe {
#include <corpse_contact_native_h7.inc>
#include <saved_part_native_r95.inc>
}
namespace f=kcd2vr::frik_pose_probe;
static void setup(){mem.clear();queries=reads=oldReads=0;mode=0;base=reinterpret_cast<std::uintptr_t>(&Status)-0x8A9348;
 put(base+0x6411AC,n::characterEntry);put(base+0x1AA6BC0,n::poseGetter);put(base+0x1A77FD0,n::physicsGetter);put(base+0x1AAF020,n::countGetter);
 put(base+0xC37477,std::array<unsigned char,9>{0xE8,0x10,0x55,0xE8,0xFF,0x80,0x4E,0x1C,0x20});
 put(base+0x3A71028+0x48,base+0xC35E60);
 put(base+0xC35E60,std::array<unsigned char,7>{0x48,0x81,0xC1,0,1,0,0});put(base+0xC35EED,std::array<unsigned char,3>{0x48,0x8B,0xF1});
 put(ent,base+0x4072C78);put(ent+0xC,std::uint32_t(456));put(base+0x4072C78+0x328,base+0x6411AC);
 put(ent+0xA8,std::uintptr_t(0x2000));put(0x2008,std::uintptr_t(0x2100));put(0x2119,std::uint8_t(0));put(0x2120,std::uint32_t(1));put(0x2128,proxy);
 put(proxy,base+0x3AC3478);put(proxy+0x30,ent);put(proxy+0x38,rootBody);
 put(ci,base+0x40602F8);put(base+0x40602F8+0x38,base+0x1AA6BC0);put(ci+0x698,base+0x3A71028);put(base+0x3A71028+0x30,base+0x1A77FD0);
 put(ci+0x8A8,ci);put(ci+0x7A0,body);put(ci+0x7B4,std::uint8_t(0x20));
 put(ci+0x8E0,base+0x3A71510);put(ci+0x940,std::uint32_t(500));put(ci+0xA70,model);
 put(model,base+0x406B930);put(base+0x406B930+8,base+0x1AAF020);put(model+8,joints);put(joints-4,std::uint32_t(500));
 put(body,base+0x3A48B30);put(base+0x3A48B30+0x30,base+0x8A9348);
}
int main(){
 setup();f::SavedCorpsePartR95 p{};
 assert(f::ReadSavedCorpsePartR95(456,ent,0,-1,{},p)&&p.body==body&&p.part==379&&p.scale==1);
 setup();assert(!f::ReadSavedCorpsePartR95(456,ent,rootBody,379,{},p)&&queries==0);
 setup();put(ci+0x7B4,std::uint8_t(0));assert(!f::ReadSavedCorpsePartR95(456,ent,0,-1,{},p)&&queries==0);
 setup();mode=2;assert(!f::ReadSavedCorpsePartR95(456,ent,body,379,{},p));
 setup();mode=3;assert(!f::ReadSavedCorpsePartR95(456,ent,body,379,{},p)&&oldReads==0);
 setup();mode=5;assert(!f::ReadSavedCorpsePartR95(456,ent,body,379,{},p));
 setup();assert(f::ReadSavedCorpsePartR95(456,ent,body,379,{},p));
 std::puts("PASS R95 actual current-body part selection: saved main distinct from root, no bone mapping, changed body/relinquish/status rejection, recovery");
}
