#include "../src/diagnostics/physics_stage_d42.h"
#include <cassert>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <cstdio>
namespace d=kcd2vr::diagnostics::physics_d42;
namespace c=kcd2vr::diagnostics::capture168;
static unsigned checks=0;
#define CK(x) do{++checks;assert(x);}while(0)
static std::vector<std::string> lines;
static void drain(std::uint64_t now){d::drain(now,[](const char* s){lines.emplace_back(s);});}
static bool has(const char* s){for(const auto&l:lines)if(l.find(s)!=std::string::npos)return true;return false;}
static d::Receipt receipt(std::uint64_t tick,std::uint64_t sample=1){d::Receipt r{};r.tick=r.inputTick=tick;r.sample=sample;return r;}
int main(){
 unsigned clockReads=0;{d::Scope off(1,1,[&](){++clockReads;return 1ull;});CK(!off.on());}drain(1);CK(clockReads==0&&lines.empty());
 c::gate.select(c::Domain::Bug,true,1000);
 auto l=d::lease(1000);CK(l.serial[0]&&!l.serial[1]);auto r=receipt(1000);d::observe(l,r);drain(1000);
 CK(has("D42_F2 T4_STAGE")&&has("T4_ACCOUNT")&&!has("D42_F7"));
 r.tick=1001;r.sample=2;r.outcome=d::Outcome::Revoked;d::observe(l,r);drain(2000);
 CK(d::storage.accounts[0].throttled==1&&d::storage.accounts[0].suppressedEdges==1);
 CK(has("lastRefusalTick=1001")&&has("lastRefusalOutcome=SOURCE_REVOKED"));
 r.tick=2020;r.inputTick=1100;r.sample=4;d::observe(l,r);r.tick=2045;r.inputTick=1099;r.sample=3;d::observe(l,r);drain(3000);
 CK(d::storage.accounts[0].gap80==1&&d::storage.accounts[0].backwards==1);
 // Already queued data is counted and discarded after OFF. Only final END survives.
 r.tick=3500;d::observe(l,r);c::gate.select(c::Domain::Bug,false,3501);lines.clear();drain(3501);
 CK(has("D42_F2_END T4_ACCOUNT")&&has("discarded=1")&&!has("T4_STAGE"));CK(!d::storage.accounts[0].serial);
 c::gate.select(c::Domain::Bug,true,4000);c::gate.select(c::Domain::Vrik,true,4000);l=d::lease(4000);
 CK(l.serial[0]&&l.serial[1]);r=receipt(4000);d::observe(l,r);lines.clear();drain(4000);
 CK(has("D42_F2 T4_STAGE")&&has("D42_F7 T4_STAGE"));
 c::gate.select(c::Domain::Bug,false,4001);lines.clear();drain(4001);CK(has("D42_F2_END")&&!has("D42_F7_END"));
 // A replaced session cannot overwrite the previous summary or reuse its budget.
 c::gate.select(c::Domain::Vrik,true,4500);auto next=d::lease(4500);r=receipt(4500);d::observe(next,r);
 CK(d::storage.accounts[1].serial!=next.serial[1]);lines.clear();drain(4500);CK(has("D42_F7_END"));
 d::observe(next,r);drain(4500);CK(d::storage.accounts[1].serial==next.serial[1]&&d::storage.accounts[1].observed==1);
 // Budget is finite; accounting continues and no unbounded detail is queued.
 d::storage.accounts[1].queued=d::DetailBudget;r.tick=5000;r.inputTick=5000;r.outcome=d::Outcome::Revoked;d::observe(next,r);drain(5500);
 CK(d::storage.accounts[1].budgetDropped==1&&d::storage.accounts[1].suppressedEdges==1&&has("budgetDropped=1"));
 c::gate.select(c::Domain::Vrik,false,6000);drain(6000);
 c::gate.select(c::Domain::Bug,true,7000);l=d::lease(7000);
 for(unsigned i=0;i<d::QueueSize+2;++i){r=receipt(7000+i*25,i+1);r.outcome=i%2?d::Outcome::Applied:d::Outcome::Revoked;d::observe(l,r);}
 CK(d::storage.size==d::QueueSize&&d::storage.accounts[0].queueFull==2);
 c::gate.select(c::Domain::Bug,false,9000);lines.clear();drain(9000);CK(has("queueFull=2")&&has("discarded=64"));
 CK(d::storage.size==0);
 // Contention never waits; its counter remains observable, including per-key isolation.
 c::gate.select(c::Domain::Bug,true,10000);l=d::lease(10000);auto busy=d::storage.busy[0].load();
 d::storage.lock.test_and_set();d::observe(l,receipt(10000));d::storage.lock.clear();CK(d::storage.busy[0]==busy+1);
 d::observe(l,receipt(10000));lines.clear();drain(10000);CK(has("busyProcessTotal="));
 // Clock rollback/expiry revokes the capture and emits one END.
 lines.clear();drain(1);CK(has("D42_F2_END"));lines.clear();drain(1);CK(lines.empty());
 // Formatter separates observation age from source input and never certifies a blade feature.
 d::Entry e{};e.serial=1;e.receipt=receipt(2000);e.receipt.inputTick=1800;e.receipt.targetValid=e.receipt.achievedValid=true;e.receipt.target[0]=.1;
 char text[1800]{};const int n=d::format(text,sizeof(text),e);CK(n>0&&std::size_t(n)<sizeof(text));
 CK(std::string(text).find("inputAgeMs=200")!=std::string::npos&&std::string(text).find("targetErrorMm=100.000")!=std::string::npos);
 CK(std::string(text).find("WHOLE_CAPSULE_FEATURE_UNKNOWN")!=std::string::npos);
 // Concurrent producers and drain exercise only copied receipts. A separate writer
 // counts lines; no game or native physics provider is invoked by this fixture.
 c::gate.select(c::Domain::Bug,true,20000);l=d::lease(20000);std::atomic<bool> done{false};
 std::thread producer([&](){for(unsigned i=0;i<10000;++i)d::observe(l,receipt(20000+i,i+1));done=true;});
 std::size_t emitted=0;while(!done.load())d::drain(20000,[&](const char*){++emitted;});producer.join();
 c::gate.select(c::Domain::Bug,false,31000);d::drain(31000,[&](const char*){++emitted;});CK(emitted>0);
 c::gate.select(c::Domain::Bug,true,40000);l=d::lease(40000);d::observe(l,receipt(40000));lines.clear();
 d::drain(40001,[](const char*s){lines.emplace_back(s);},true);
 CK(has("D42_F2_END")&&has("discarded=1")&&c::active(c::Domain::Bug,40001));
 std::printf("physics_stage_d42: %u checks, capture lifecycle/finite budgets/no native reads PASS\n",checks);
}
