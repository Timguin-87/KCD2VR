// Compiled together with the actual native_upscaler_stereo_d82.cpp.
// Backend leaves are spies; no SDK, GPU or headset is involved.
#include "native_upscaler_stereo_d82.h"
#include "native_dlss_stereo_d82.h"
#include "native_fsr_stereo_d82.h"
#include "diagnostic_trace.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %u line%d: %s\n",checks,__LINE__,#x);std::exit(1);}}while(0)
struct Backend {bool installed=false,enabled=false,ready=false;std::uint64_t receiptEpoch=0;std::uint32_t receiptFrame=0;
 unsigned installs=0,enables=0,readyReads=0,pairReads=0,stops=0,logs=0;};
static Backend dlss,fsr;static std::vector<std::string> events,lines;
static void enabled(Backend& b,bool on,const char* label){++b.enables;events.push_back(label);if(b.enabled!=on){b.receiptEpoch=0;b.receiptFrame=0;}b.enabled=on;}
static bool pair(Backend& b,std::uint64_t e,std::uint32_t f){++b.pairReads;return b.installed&&b.enabled&&b.ready&&b.receiptEpoch==e&&b.receiptFrame==f;}
namespace kcd2vr::native_dlss_stereo_d82 {
bool Install() noexcept {++dlss.installs;events.emplace_back("install_dlss");dlss.installed=true;return true;}
void SetEnabled(bool on) noexcept {enabled(dlss,on,"enable_dlss");}
bool ReadyForStereo() noexcept {++dlss.readyReads;return dlss.installed&&dlss.ready;}
bool PairReady(std::uint64_t e,std::uint32_t f) noexcept {return pair(dlss,e,f);}
void Stop() noexcept {++dlss.stops;enabled(dlss,false,"stop_dlss");}
void LogStatus(const char*) noexcept {++dlss.logs;}
}
namespace kcd2vr::native_fsr_stereo_d82 {
bool Install() noexcept {++fsr.installs;events.emplace_back("install_fsr");fsr.installed=true;return true;}
void SetEnabled(bool on) noexcept {enabled(fsr,on,"enable_fsr");}
bool ReadyForStereo() noexcept {++fsr.readyReads;return fsr.installed&&fsr.ready;}
bool PairReady(std::uint64_t e,std::uint32_t f) noexcept {return pair(fsr,e,f);}
void Stop() noexcept {++fsr.stops;enabled(fsr,false,"stop_fsr");}
void LogStatus(const char*) noexcept {++fsr.logs;}
}
namespace kcd2vr::diagnostic_trace {void Write(const char* s) noexcept {lines.emplace_back(s?s:"");}}
static int liveTechnology=-1;static unsigned liveReads=0;static int readTechnology() noexcept {++liveReads;return liveTechnology;}
int main(){namespace u=kcd2vr::native_upscaler_stereo_d82;
 u::SetTechnologyReader(nullptr);u::SetMode(false,0);u::BeginEpoch(0);
 CHECK(u::CanEnterStereo(0));CHECK(!u::TemporalActive());CHECK(u::PairReady(0,0));CHECK(u::FrameReady(999));
 CHECK(!dlss.installs&&!fsr.installs&&!dlss.pairReads&&!fsr.pairReads&&!dlss.readyReads&&!fsr.readyReads);
 for(int tech:{-1,3,4,99}){u::SetMode(true,tech);CHECK(!u::CanEnterStereo(tech));CHECK(u::TemporalActive());CHECK(!u::PairReady(7,100));CHECK(!u::FrameReady(100));}
 events.clear();u::InstallEarly(true);
 CHECK((events==std::vector<std::string>{"enable_dlss","enable_fsr","install_dlss","install_fsr"}));
 CHECK(dlss.installed&&fsr.installed&&dlss.enabled&&fsr.enabled);
 CHECK(!u::CanEnterStereo(1)&&!u::CanEnterStereo(2));
 dlss.ready=true;fsr.ready=true;u::SetMode(false,2);
 CHECK(u::CanEnterStereo(2)&&u::CanEnterStereo(1));CHECK(!u::CanEnterStereo(3)&&!u::CanEnterStereo(-1));CHECK(!dlss.enabled&&!fsr.enabled);CHECK(!u::FrameReady(100));
 // Readiness describes captured resources even in Mono; enablement controls routing.
 u::SetMode(true,2);u::BeginEpoch(7);dlss.receiptEpoch=7;dlss.receiptFrame=100;
 CHECK(u::PairReady(7,100)&&u::FrameReady(100));CHECK(!u::PairReady(6,100));CHECK(!u::PairReady(7,99));
 const auto beforeInvalid=dlss.pairReads;CHECK(!u::PairReady(0,100)&&!u::PairReady(8,100));CHECK(dlss.pairReads==beforeInvalid);
 u::BeginEpoch(8);CHECK(!u::PairReady(7,100));CHECK(!u::FrameReady(100));
 dlss.receiptEpoch=8;CHECK(u::FrameReady(100));
 // A live CVar switch cannot convert a partial temporal pair into Native.
 liveTechnology=2;u::SetTechnologyReader(readTechnology);dlss.receiptFrame=0;
 CHECK(!u::FrameReady(100));liveTechnology=0;
 CHECK(!u::FrameReady(100)&&"MIDPAIR_TEMPORAL_TO_NATIVE_REFUSED");
 liveTechnology=2;dlss.receiptFrame=100;u::SetTechnologyReader(nullptr);
 // Matching receipts from another backend need that backend's new epoch.
 u::SetMode(true,1);CHECK(!u::FrameReady(100));fsr.receiptEpoch=8;fsr.receiptFrame=100;CHECK(!u::FrameReady(100));
 u::BeginEpoch(9);fsr.receiptEpoch=9;CHECK(u::FrameReady(100));
 u::SetMode(true,2);liveTechnology=1;u::SetTechnologyReader(readTechnology);liveReads=0;
 dlss.receiptEpoch=9;dlss.receiptFrame=101;CHECK(u::FrameReady(100));CHECK(!u::FrameReady(101));CHECK(liveReads==2);
 liveTechnology=2;CHECK(!u::FrameReady(101));CHECK(!u::FrameReady(100));
 u::BeginEpoch(10);dlss.receiptEpoch=10;CHECK(u::FrameReady(101));CHECK(!u::FrameReady(100));
 liveTechnology=-1;CHECK(!u::FrameReady(101));liveTechnology=3;CHECK(!u::FrameReady(101));
 liveTechnology=0;u::Stop();CHECK(!u::TemporalActive());CHECK(!u::FrameReady(101));
 u::BeginEpoch(11);CHECK(u::FrameReady(101));CHECK(!u::PairReady(10,101));
 // Queue/publication-side readiness must only read the selected backend receipt.
 liveTechnology=2;u::SetMode(true,0);u::BeginEpoch(12);dlss.receiptEpoch=12;dlss.receiptFrame=200;
 const auto oldDlss=dlss,oldFsr=fsr;const auto oldEvents=events.size(),oldLines=lines.size();
 for(unsigned i=0;i<100;++i)CHECK(u::FrameReady(200));
 CHECK(dlss.pairReads==oldDlss.pairReads+100&&fsr.pairReads==oldFsr.pairReads);
 CHECK(dlss.installs==oldDlss.installs&&fsr.installs==oldFsr.installs&&dlss.enables==oldDlss.enables&&fsr.enables==oldFsr.enables);
 CHECK(dlss.readyReads==oldDlss.readyReads&&fsr.readyReads==oldFsr.readyReads&&events.size()==oldEvents&&lines.size()==oldLines);
 u::Stop();CHECK(!u::FrameReady(200));CHECK(u::CanEnterStereo(2));CHECK(dlss.stops==2&&fsr.stops==2);
 u::SetMode(true,2);u::BeginEpoch(13);CHECK(!u::FrameReady(200));dlss.receiptEpoch=13;dlss.receiptFrame=201;CHECK(u::FrameReady(201));
 u::SetTechnologyReader(nullptr);u::SetMode(true,1);CHECK(!u::FrameReady(201));u::BeginEpoch(14);fsr.receiptEpoch=14;fsr.receiptFrame=201;CHECK(u::FrameReady(201));
 // Every supported transition must cross the existing safe new-epoch gate.
 std::uint64_t nextEpoch=20;
 for(int before:{0,1,2})for(int after:{0,1,2})if(before!=after){
  u::SetMode(true,before);u::BeginEpoch(nextEpoch);dlss.receiptEpoch=fsr.receiptEpoch=nextEpoch;dlss.receiptFrame=fsr.receiptFrame=300;
  CHECK(u::FrameReady(300));u::SetMode(true,after);CHECK(!u::FrameReady(300));
  u::BeginEpoch(++nextEpoch);dlss.receiptEpoch=fsr.receiptEpoch=nextEpoch;CHECK(u::FrameReady(300));++nextEpoch;
 }
 u::SetMode(true,1);u::BeginEpoch(nextEpoch);
 u::LogStatus("binding-test");CHECK(!lines.empty()&&lines.back().find("technology=1")!=std::string::npos);CHECK(dlss.logs==1&&fsr.logs==1);
 std::printf("D82 actual upscaler aggregate: %u checks; backend read/lifecycle leaves controlled; GPU/headset NOT_RUN\n",checks);
}
