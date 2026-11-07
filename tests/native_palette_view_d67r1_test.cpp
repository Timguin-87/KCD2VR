#include <cstdint>
static std::uint64_t testNs=1000000000ull;
static unsigned clockCalls=0;
static std::uint64_t testClock() noexcept {++clockCalls;return testNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK testClock
#include "diagnostics/native_palette_view_d67r1.h"
#include "diagnostics/capture168.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace d=kcd2vr::diagnostics::native_palette_d67r1;
namespace v=d::view;
namespace cap=kcd2vr::diagnostics::capture168;
namespace perf=kcd2vr::diagnostics::perf_d61;
namespace md=kcd2vr::diagnostics::motion_d68;
static bool fingerprintReady=true;
namespace kcd2vr::retail_build112 { bool Ready(std::uintptr_t) noexcept {return fingerprintReady;} }
namespace {
unsigned checks=0,failed=0;std::uint64_t reads=0;
void check(bool ok,const char* message){++checks;if(!ok){++failed;std::cerr<<"FAIL "<<message<<'\n';}}
struct Region {std::uintptr_t base;std::size_t size;};std::vector<Region> regions;
std::uintptr_t changingAddress=0;unsigned changedReads=0;
bool readFixture(std::uintptr_t p,void* out,std::size_t size){
    ++reads;
    for(auto r:regions)if(p>=r.base&&size<=r.size&&p-r.base<=r.size-size){
        std::memcpy(out,reinterpret_cast<void*>(p),size);
        if(p==changingAddress&&++changedReads==2){auto value=std::int32_t(1001);std::memcpy(out,&value,sizeof(value));}
        return true;
    }
    return false;
}
template<class T> void put(std::vector<unsigned char>& storage,std::size_t offset,T value){std::memcpy(storage.data()+offset,&value,sizeof(value));}
std::uintptr_t admit(std::vector<unsigned char>& storage){const auto p=reinterpret_cast<std::uintptr_t>(storage.data());regions.push_back({p,storage.size()});return p;}
}
int main(int argc,char** argv){
    if(argc!=2){std::cerr<<"usage: native_palette_view_d67r1_test <native_palette_view_d67r1_abi.txt>\n";return 2;}
    std::vector<unsigned char> image(0x1DF8000),renderer(0x300),view(0x9000),pipeline(0x2000);
    const auto wh=admit(image),ren=admit(renderer),crv=admit(view),pipe=admit(pipeline);
    put(image,0,std::uint16_t(0x5A4D));put(image,0x3C,std::uint32_t(0x80));put(image,0x80,std::uint32_t(0x4550));
    put(image,0x88,std::uint32_t(0x6A350E20));put(image,0x98,std::uint16_t(0x20B));put(image,0xD0,std::uint32_t(0x5B2D000));
    std::ifstream fixture(argv[1]);std::string address,bytes;unsigned chunks=0;
    while(fixture>>address>>bytes){const auto off=std::stoul(address,nullptr,16);if(bytes.size()%2||off+bytes.size()/2>image.size())return 3;
        for(unsigned i=0;i<bytes.size()/2;++i)image[off+i]=static_cast<unsigned char>(std::stoul(bytes.substr(i*2,2),nullptr,16));
        ++chunks;
    }
    check(chunks==6,"exact supplied-binary fixture loaded");
    auto reader=&readFixture;
    check(v::abi(reader,wh),"actual image contract and six native call/offset byte pins match fixture");
    image[0x505E61]^=1;check(!v::abi(reader,wh),"changed native pool instruction fails closed");image[0x505E61]^=1;
    put(image,0x88,std::uint32_t(0x12345678));check(!v::abi(reader,wh),"unsupported native image refused");put(image,0x88,std::uint32_t(0x6A350E20));
    put(renderer,0,wh+0x3DFC008);put(renderer,0x24C,std::int32_t(1000));
    put(view,0,wh+0x3DF25C0);put(view,0x14,std::uint32_t(0));put(view,0x2C,std::uint32_t(9000));
    put(view,0x8D40,std::int32_t(999));put(view,0x8EE0,std::uint32_t(0));put(view,0x8D60,pipe);
    put(pipeline,0,wh+0x3DFEBD0);put(pipeline,0x1978,crv);put(pipeline,0x58,std::uintptr_t(0xFEDCBA9876543210ull));
    const auto observe=[&](unsigned eye,std::uint64_t tick=1000){v::observe(reader,wh,ren,crv,pipe,9000,eye,77,tick,21);};
    const auto before=reads;observe(0);v::Record r{};
    check(reads==before&&!v::pop(r),"Shift+F2 off does no native read and emits no receipt");
    cap::gate.select(cap::Domain::Bug,true,1000);const auto offClock=clockCalls;observe(0);
    check(reads==before&&!v::pop(r)&&clockCalls==offClock,"F2 alone does not arm observer or sample clock");
    perf::gate.select(true,perf::nowMs());
    fingerprintReady=false;const auto readyReads=reads;observe(0);
    check(v::pop(r)&&r.status==v::Status::AbiRefused&&reads==readyReads,"full fingerprint lease required before any native pin or field read");fingerprintReady=true;
    testNs+=250000000;const auto burstReads=reads,mainOutside=md::outsideWindow[md::RenderRequest].load();observe(0);
    check(reads==burstReads&&!v::pop(r)&&v::outsideBurst.load()==1,"outside shared burst no native reads or receipt");
    check(md::outsideWindow[md::RenderRequest].load()==mainOutside,"separate observer cannot contaminate main render stage health");
    testNs=3000000000ull;observe(0);
    check(v::pop(r)&&r.status==v::Status::Stable,"current qualified left Standard entry is admitted");
    check(r.live.pool==999&&r.live.frame==9000&&r.live.rendererPool==1000,"upload pool, view frame and renderer pool remain separate actual fields");
    check(r.live.bulkOwner==0xFEDCBA9876543210ull,"bulk owner is copied without dereferencing it");
    put(view,0x8EE0,std::uint32_t(1));observe(1);check(v::pop(r)&&r.status==v::Status::Stable&&r.eye==1,"existing extra eye captures the same native upload pool");
    observe(0);check(v::pop(r)&&r.status==v::Status::IdentityRefused,"wrong selector cannot become a left-eye receipt");
    put(view,0x8D60,std::uintptr_t(123));observe(1);check(v::pop(r)&&r.status==v::Status::IdentityRefused,"current view owner must match current pipeline argument");put(view,0x8D60,pipe);
    changingAddress=crv+0x8D40;changedReads=0;observe(1);check(v::pop(r)&&r.status==v::Status::Changed,"pool change between repeated reads is not a stable receipt");changingAddress=0;
    regions.pop_back();observe(1);check(v::pop(r)&&r.status==v::Status::Unreadable,"unreadable current pipeline fails closed");admit(pipeline);
    observe(1);std::vector<std::string> lines;v::drain(1000,[&](const char* s){lines.emplace_back(s);});
    check(!lines.empty()&&lines[0].find("uploadPool=999")!=std::string::npos&&lines[0].find("henryDraw=UNOBSERVED")!=std::string::npos,"drained receipt reports upload identity without claiming selected Henry draw");
    check(md::accept(lines[0].c_str(),1000),"finite diagnostic sink accepts exact view-pool receipt family");
    const auto suppressed=v::suppressed.load();for(unsigned i=0;i<250;++i)observe(1);
    check(v::suppressed.load()>suppressed,"observer is bounded to 240 admissions per capture second");
    check(v::dropped.load()>0,"full bounded queue counts loss instead of silently overwriting receipts");
    while(v::pop(r)){}
    testNs=3001000000ull;perf::gate.select(false,perf::nowMs());const auto closedReads=reads;observe(1,3001);
    check(reads==closedReads&&!v::pop(r),"closed capture prevents native reads and producer records");
    check(md::accept(lines[0].c_str(),3001)&&!md::accept(lines[0].c_str(),8002),"receipt drain uses existing five-second grace only");
    testNs=3002000000ull;perf::gate.select(true,perf::nowMs());const auto readsBeforeLock=reads;v::observerLock.test_and_set();observe(1,3002);v::observerLock.clear();
    check(reads==readsBeforeLock&&!v::pop(r),"observer contention drops without a wait or native reads");
    std::cout<<"D67R1 native palette view "<<(checks-failed)<<'/'<<checks<<" reader=PRODUCTION_HEADER nativeMemory=BOUNDED_FIXTURE finalDraw=UNOBSERVED\n";
    return failed?1:0;
}
