#include "native_script_t2.h"
#include "core/snapshot_mailbox.h"
#include "vrik/gore_eligibility_d56.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
std::atomic<std::uint64_t> g_pose_epoch6689{3};
std::atomic<void*> g_last_player_entity{nullptr};
static void* entityD56=reinterpret_cast<void*>(0x420);
static void* characterD56=reinterpret_cast<void*>(0x700);
static bool maleD56=true,currentD56=true,replaceD56=false;
static void* FlinchEntityById178r11(std::uint32_t id) {return id==42?entityD56:nullptr;}
static void* CharacterForEntity6689(void* e) {return e==entityD56?characterD56:nullptr;}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* s){lines.emplace_back(s);}}
namespace kcd2vr::native_script_t2 {
static Client registered=nullptr;static Wanted wanted=nullptr;
bool Register(Client f,Wanted w) noexcept {registered=f;wanted=w;return true;}
bool Install() noexcept{return true;}
bool Context::Current() const noexcept {return currentD56;}
bool Context::CallPacket(const char* fn,const char* in,char* out,std::size_t n) const noexcept {
 assert(std::strcmp(fn,"kcd2vr_gore_eligibility_d56")==0);
 if(replaceD56)characterD56=reinterpret_cast<void*>(0x701);
 std::snprintf(out,n,"%s|%s",in,maleD56?"MALE_HUMAN":"REFUSED");return true;
}
}
namespace production {
#include "subsystems/gore/native_eligibility_d56.inl"
}
int main(){
 using R=kcd2vr::gore_eligibility_d56::Result;using namespace production;
 assert(InstallEligibilityD56());
 assert(EligibilityD56(42,0x420,characterD56,1000)==R::Pending);
 assert(kcd2vr::native_script_t2::wanted());
 kcd2vr::native_script_t2::Context c{0,0,1001,1};
 kcd2vr::native_script_t2::registered(c);
 assert(EligibilityD56(42,0x420,characterD56,1002)==R::MaleHuman);
 assert(EligibilityD56(42,0x421,characterD56,1002)==R::Refused);
 g_last_player_entity=entityD56;assert(EligibilityD56(42,0x420,characterD56,1002)==R::Refused);g_last_player_entity=nullptr;
 maleD56=false;assert(EligibilityD56(42,0x420,characterD56,1082)==R::Pending);c.now=1083;kcd2vr::native_script_t2::registered(c);
 assert(EligibilityD56(42,0x420,characterD56,1084)==R::Refused);
 maleD56=true;replaceD56=true;assert(EligibilityD56(42,0x420,characterD56,1164)==R::Pending);c.now=1165;kcd2vr::native_script_t2::registered(c);
 assert(EligibilityD56(42,0x420,reinterpret_cast<void*>(0x700),1166)==R::Refused); // old native character cannot inherit reply
 replaceD56=false;assert(EligibilityD56(42,0x420,characterD56,1167)==R::Pending);c.now=1168;kcd2vr::native_script_t2::registered(c);
 assert(EligibilityD56(42,0x420,characterD56,1169)==R::MaleHuman);
 ++g_pose_epoch6689;assert(EligibilityD56(42,0x420,characterD56,1170)==R::Pending);
 currentD56=false;c.now=1171;kcd2vr::native_script_t2::registered(c);assert(EligibilityD56(42,0x420,characterD56,1172)==R::Refused);
 c.now=2000;kcd2vr::native_script_t2::registered(c);assert(!kcd2vr::native_script_t2::wanted());
 std::puts("gore_eligibility_d56: actual request/service/receipt owner and freshness transaction PASS");
}
