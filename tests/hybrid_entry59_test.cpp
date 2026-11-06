#include "subsystems/hybrid/contact_policy.h"
#include <atomic>
#include <cassert>
#include <cstdio>
namespace hs59=kcd2vr::hybrid_serial59;
std::atomic<unsigned> g_drop_serial_claimed56{0};unsigned nativeCalls=0,attemptEntries=0;bool clearInScope=false;
float currentStrength=1.f;std::atomic<unsigned> g_strength_consumed59c17{0};std::atomic<int> g_tg_phase178r34{0};
bool Allow(int,int){return false;}namespace diagnostic_trace{void Write(const char*){}}
namespace kcd2vr::combo_lineage19 {struct Owner{};inline bool valid(const Owner&){return false;}struct OfferScope{explicit OfferScope(const Owner*){}};}
namespace kcd2vr::geometry40c1 {struct Scope{Scope(void*,void*,void*){if(clearInScope)hs59::releases.clear();}};}
namespace vrstate596 {bool SetFloat(void*,int,float x){currentStrength=x;return true;}}
float F(void*,int){return currentStrength;}constexpr int kAttackStrength=0x1E0;
bool CallLiveHandler35(void*,void*){++nativeCalls;return true;}
bool CallSubmit(void*,void*,float*,bool*,void*,bool,int,void*,bool& ok){++nativeCalls;ok=true;return true;}
void TelegraphWrite34(void*,int phase,int){g_tg_phase178r34=phase;}
struct Attempt {void nativeCallStarted(){++attemptEntries;}};
auto ticket59(std::uint64_t serial){kcd2vr::contact176::Ticket t{1,2,serial,100,3,4,5,6,7,8,9,10};return t;}
void live59(hs59::RoutedReservation& serialReservationC2){
 bool survived=false,serialEntered59=false;void* henryActor=nullptr;void* details=nullptr;void* target=nullptr;void* action=nullptr;void* hs=nullptr;
 const kcd2vr::combo_lineage19::Owner comboOwner19{};Attempt hybridReservation54;
 float leased17=.8f,prior17=1.f;currentStrength=leased17;
 #include "hybrid_order59_live.inc"
 assert(serialEntered59&&survived);
}
void direct59(hs59::RoutedReservation& serialReservationC2){
 Attempt claimReservationC2,hybridReservation54;bool vrDirect596=true,telegraph34=true,ok=false,post=false;
 void* rpg=nullptr;void* hitInfo=nullptr;void* request=nullptr;void* meleeDetails=nullptr;void* hs=nullptr;float strength=1.f;bool apply=true;int type=1;
 g_tg_phase178r34=2;
 #include "hybrid_order59_direct.inc"
 assert(serialEntered59&&survived&&ok);
}
int main(){
 kcd2vr::contact176::policy=1;kcd2vr::contact176::recoveryEpoch55c1=1;
 hs59::RoutedReservation live;assert(live.reserve(ticket59(2000),100,true));clearInScope=true;live59(live);
 assert(nativeCalls==0&&attemptEntries==0&&currentStrength==1.f&&g_drop_serial_claimed56==1);
 hs59::RoutedReservation direct;assert(direct.reserve(ticket59(2001),100,true));hs59::releases.clear();direct59(direct);
 assert(nativeCalls==0&&attemptEntries==0&&g_tg_phase178r34==0&&g_drop_serial_claimed56==2);
 clearInScope=false;hs59::RoutedReservation good;assert(good.reserve(ticket59(2002),100,true));live59(good);
 assert(nativeCalls==1&&attemptEntries==1&&currentStrength==1.f);
 hs59::RoutedReservation physicalNoRelease;direct59(physicalNoRelease);
 assert(nativeCalls==2&&attemptEntries==3&&g_tg_phase178r34==0);
 hs59::RoutedReservation failed;auto bad=ticket59(2003);bad.key=0;assert(!failed.reserve(bad,100,true));direct59(failed);
 assert(nativeCalls==2&&attemptEntries==3&&g_tg_phase178r34==0);
 std::puts("hybrid_entry59: actual final LIVE/DIRECT entry branches refuse Clear, preserve strength/telegraph cleanup and attempt rollback; explicit no-release physical transport retained PASS");
}
