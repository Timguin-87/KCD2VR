#include <cstdint>
static std::uint64_t testNs=1000000000ull;
static unsigned clockCalls=0;
static std::uint64_t testClock() noexcept {++clockCalls;return testNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK testClock
#include "diagnostics/native_palette_d67r1.h"
#include "diagnostics/capture168.h"
#include <algorithm>
#include <cstring>
#include "diagnostics/capture168.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace diagnostics=kcd2vr::diagnostics;
namespace d=kcd2vr::diagnostics::native_palette_d67r1;
namespace cap=kcd2vr::diagnostics::capture168;
namespace perf=kcd2vr::diagnostics::perf_d61;
namespace md=kcd2vr::diagnostics::motion_d68;
static bool fingerprintReady=true;
namespace kcd2vr::retail_build112 { bool Ready(std::uintptr_t) noexcept {return fingerprintReady;} }
namespace {
unsigned checks=0,failed=0;std::uint64_t tick=1000,reads=0;
void check(bool ok,const char* what){++checks;if(!ok){++failed;std::cerr<<"FAIL "<<what<<'\n';}}
struct Region {std::uintptr_t begin=0;std::size_t size=0;};std::vector<Region> regions;
std::uintptr_t imageBase=0,characterAddress=0,poseAddress=0,skeletonAddress=0;
bool supported=true,identity=true;std::uint64_t completed=70;
std::atomic<std::uint64_t> g_pose_epoch6689{7};std::atomic<bool> g_committed_pose_valid6689{true};
std::uint64_t GetTickCount64(){return tick;}unsigned GetCurrentThreadId(){return 17;}
std::uintptr_t WhBase(){return imageBase;}bool SupportedWhBuild(std::uintptr_t b){return supported&&b==imageBase;}
bool Safe(const void* p,std::size_t n){const auto a=reinterpret_cast<std::uintptr_t>(p);
    for(auto r:regions)if(a>=r.begin&&n<=r.size&&a-r.begin<=r.size-n)return true;
    return false;}
template<class T>bool Read(const void* p,T& out){++reads;if(!Safe(p,sizeof(out)))return false;std::memcpy(&out,p,sizeof(out));return true;}
bool PlayerIdentityAccepts37(const void* p,std::uint64_t){return identity&&reinterpret_cast<std::uintptr_t>(p)==characterAddress;}
bool ValidateCharInstance(void* p,void*& a){a=reinterpret_cast<void*>(characterAddress+8);return identity&&reinterpret_cast<std::uintptr_t>(p)==characterAddress;}
bool ReadValidatedDefaultSkeletonFromPose(void* p,void*& s){s=reinterpret_cast<void*>(skeletonAddress);return reinterpret_cast<std::uintptr_t>(p)==poseAddress;}
std::uint64_t ReadCompletedPublicationT23(){return completed;}
struct RigMap {int hand_r=-1,hand_l=-1;};
struct TrackingSnapshot {std::uint64_t sequence=0,source_epoch=0,timeNs=0;std::int64_t poseTime=0;};
struct CoherentTransaction6689 {std::uint64_t epoch=7,publicationD49=70,motionSessionD68=0,motionCycleD68=0;bool pairedPublishedD49=true;};
#include "vrik/native_palette_d67r1.inl"
template<class T>void put(std::vector<unsigned char>& memory,std::size_t offset,const T& value){std::memcpy(memory.data()+offset,&value,sizeof(value));}
void admit(std::vector<unsigned char>& memory){regions.push_back({reinterpret_cast<std::uintptr_t>(memory.data()),memory.size()});}
}
int main(int argc,char** argv){
    if(argc!=2){std::cerr<<"usage: native_palette_d67r1_test <native_palette_d67r1_abi.txt>\n";return 2;}
    std::array<d::Slot,3> slots{{{100,200,300,42,true},{300,400,500,41,true},{500,600,500,40,true}}};
    const auto match=d::identify(slots,200,400);
    check(match.match==d::Match::Unique&&match.pool==42&&match.previousPool==41,"exact current/previous native slot lineage");
    auto ambiguous=slots;ambiguous[2].output=200;
    check(d::identify(ambiguous,200,400).match==d::Match::Ambiguous,"shared output pointer cannot borrow newest counter");
    auto self=slots;self[0].previousData=self[0].data;
    check(d::identify(self,200,200).match==d::Match::SelfPrevious,"native self-previous fallback is explicit");
    auto gap=slots;gap[1].pool=39;
    check(d::identify(gap,200,400).match==d::Match::PreviousOutsideRing,"counter gap does not become previous-frame proof");
    check(d::identify(slots,0,400).match==d::Match::Unknown,"zero descriptor output fails closed");

    // Run the actual production reader against bounded native-shaped storage.
    // Offset signatures are 96 bytes extracted from the supplied game binary.
    std::vector<unsigned char> image(0x600000),ci(0xB00),pose(64),skeleton(64),data0(128),data1(128),data2(128),out0(512,1),out1(512,2),out2(512,3);
    for(auto* m:{&image,&ci,&pose,&skeleton,&data0,&data1,&data2,&out0,&out1,&out2})admit(*m);
    imageBase=reinterpret_cast<std::uintptr_t>(image.data());characterAddress=reinterpret_cast<std::uintptr_t>(ci.data());
    poseAddress=reinterpret_cast<std::uintptr_t>(pose.data());skeletonAddress=reinterpret_cast<std::uintptr_t>(skeleton.data());
    std::ifstream fixture(argv[1]);std::string rva,bytes;unsigned chunks=0;
    while(fixture>>rva>>bytes){const auto offset=std::stoul(rva,nullptr,16);if(bytes.size()!=32||offset+16>image.size())return 3;
        for(unsigned i=0;i<16;++i)image[offset+i]=static_cast<unsigned char>(std::stoul(bytes.substr(i*2,2),nullptr,16));
        ++chunks;}
    check(chunks==6,"exact supplied-binary instruction fixture loaded");
    const auto dataPtr0=reinterpret_cast<std::uintptr_t>(data0.data()),dataPtr1=reinterpret_cast<std::uintptr_t>(data1.data()),dataPtr2=reinterpret_cast<std::uintptr_t>(data2.data());
    const auto outPtr0=reinterpret_cast<std::uintptr_t>(out0.data()),outPtr1=reinterpret_cast<std::uintptr_t>(out1.data()),outPtr2=reinterpret_cast<std::uintptr_t>(out2.data());
    put(ci,0xA40,dataPtr0);put(ci,0xA48,std::int32_t(42));put(ci,0xA50,dataPtr1);put(ci,0xA58,std::int32_t(41));put(ci,0xA60,dataPtr2);put(ci,0xA68,std::int32_t(40));
    put(data0,8,outPtr0);put(data0,0x30,dataPtr1);put(data1,8,outPtr1);put(data1,0x30,dataPtr2);put(data2,8,outPtr2);put(data2,0x30,dataPtr2);
    put(data0,0x40,std::uintptr_t(0x123456789ABCDEF0ull)); // copied, never dereferenced by the reader
    std::vector<unsigned char> descriptor(48);admit(descriptor);
    put(descriptor,0,poseAddress);put(descriptor,8,skeletonAddress);put(descriptor,0x10,characterAddress+8);put(descriptor,0x18,outPtr1);put(descriptor,0x20,outPtr0);
    RigMap rig{5,6};TrackingSnapshot tracking{0x123456789ABCDEFull,9,900,1200};CoherentTransaction6689 tx{};
    const auto begin=[&]{return BeginNativePaletteD67R1(descriptor.data(),pose.data(),skeleton.data(),ci.data(),out0.data(),rig,16,tracking,tx,true);};
    const auto offReads=reads;const auto off=begin();
    check(!off&&reads==offReads&&d::ordinal.load()==0,"Shift+F2 off performs no native read or receipt work");
    cap::gate.select(cap::Domain::Bug,true,tick);
    const auto f2Clock=clockCalls;check(!begin()&&reads==offReads&&clockCalls==f2Clock,"F2 Bug alone cannot arm palette observer or sample clock");
    perf::gate.select(true,perf::nowMs());
    check(!begin()&&reads==offReads,"unselected core cycle does not read native metadata");
    tx.motionSessionD68=perf::gate.serial.load();tx.motionCycleD68=123;
    fingerprintReady=false;const auto fullOffReads=reads;auto refused=begin();
    check(refused&&!(refused->flags&d::Abi)&&reads==fullOffReads,"full fingerprint lease required before any native signature/metadata read");fingerprintReady=true;
    auto record=begin();check(bool(record),"Shift+F2 selected Henry cycle admits native hook receipt");if(!record)return 4;
    check((record->flags&(d::Abi|d::IdentityPre|d::DescriptorPre|d::RingPreStable))==(d::Abi|d::IdentityPre|d::DescriptorPre|d::RingPreStable),"real reader qualifies native byte signatures and stable descriptor/ring");
    check(record->before.match==d::Match::Unique&&record->before.pool==42&&record->before.previousPool==41&&record->masks[0]==15,"real reader joins current/previous palette and both wrist byte reads");
    check(record->before.transfer==0x123456789ABCDEF0ull,"native transfer-entry identity copied without dereferencing it");
    out0[5*32]=19;out0[6*32]=27; // stand-in for native producer, not mod output
    record->flags|=d::NativeReturned;record->beforeNativeNs=d::clockNs();record->afterNativeNs=d::clockNs();
    ReadNativePaletteD67R1(*record,1,16);
    check(record->masks[1]==15&&(record->flags&d::RingUnchanged),"post-native bytes retain exact ring identity");
    check(record->hashes[0][0][0]!=record->hashes[1][0][0]&&record->hashes[0][1][0]==record->hashes[1][1][0],"actual current-byte change is separate from unchanged previous palette");
    const auto finishReads=reads;FinishNativePaletteD67R1(*record,tx);
    check(reads==finishReads,"finish after restore never reads native pointers");
    check(record->publication==70&&(record->flags&d::PairedPublication)&&(record->flags&d::PublicationCurrent),"completed body publication joins the exact producer receipt");
    std::vector<std::string> lines;d::drain(tick,[&](const char* p){lines.emplace_back(p);});
    check(!lines.empty()&&lines[0].find("sample=81985529216486895")!=std::string::npos&&lines[0].find("meshSubmission=UNOBSERVED")!=std::string::npos,"worker formats full-width sample and explicit unproven consumer boundary");
    check(lines[0].find("pools=42/41")!=std::string::npos&&lines[0].find("previousR=")!=std::string::npos,"single bounded receipt retains native lineage and byte hashes");
    check(md::accept(lines[0].c_str(),tick),"finite performance sink accepts exact receipt family");

    check(record->cycle==123&&lines[0].find("cycle=123")!=std::string::npos,"exact captured D68 core cycle retained");
    auto changed=begin();put(ci,0xA48,std::int32_t(43));ReadNativePaletteD67R1(*changed,1,16);
    check(changed->after.pool==43&&!(changed->flags&d::RingUnchanged),"native slot counter replacement is not relabeled as unchanged");put(ci,0xA48,std::int32_t(42));
    auto moved=begin();put(descriptor,0x20,outPtr1);ReadNativePaletteD67R1(*moved,1,16);
    check(moved->masks[1]==0&&!(moved->flags&d::DescriptorPost),"descriptor replacement rejects post-native palette claim");put(descriptor,0x20,outPtr0);
    auto retired=begin();identity=false;ReadNativePaletteD67R1(*retired,1,16);
    check(retired->masks[1]==0&&!(retired->flags&d::IdentityPost),"retired Henry identity prevents native reads from authorizing output");identity=true;
    supported=false;const auto unsupportedReads=reads;auto unsupported=begin();
    check(unsupported&&!(unsupported->flags&d::Abi)&&reads==unsupportedReads,"unqualified image emits an empty receipt without native metadata reads");supported=true;

    d::Budget budget;bool admitted=true;for(unsigned i=0;i<240;++i)admitted=budget.take(1,1000)&&admitted;
    check(admitted&&!budget.take(1,1000)&&budget.take(1,2000)&&budget.take(2,2000),"capture budget is bounded and renews only its second/session");
    d::Record queued=*record;const auto oldDrops=d::dropped.load();unsigned accepted=0;
    for(unsigned i=0;i<256;++i){queued.serial=i+1;if(d::push(queued))++accepted;}
    check(accepted==255&&d::dropped.load()==oldDrops+1,"bounded queue refuses overflow without overwriting old evidence");
    d::Record copied{};check(d::pop(copied)&&copied.serial==1&&copied.hashes[1][0][0]==record->hashes[1][0][0],"queue preserves an immutable copy and FIFO ownership");while(d::pop(copied)){}
    testNs+=1000000;tick=perf::nowMs();perf::gate.select(false,tick);check(!d::push(queued),"no producer admission after capture closes");
    check(md::accept(lines[0].c_str(),tick)&&!md::accept(lines[0].c_str(),tick+5001),"only already captured records drain within existing finite receipt grace");
    testNs+=1000000;tick=perf::nowMs();perf::gate.select(true,tick);check(!d::push(queued),"new capture rejects a stale session receipt");
    std::cout<<"D67R1 native palette "<<(checks-failed)<<'/'<<checks<<" nativeReader=ACTUAL_INL nativeMemory=BOUNDED_FIXTURE renderer=NOT_EXECUTED\n";
    return failed?1:0;
}
