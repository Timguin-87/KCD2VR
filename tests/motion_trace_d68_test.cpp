// Actual bounded queue and actual native-observer include. Native game leaves
// are explicit POD mocks; this tests observation isolation, not engine/pixels.
#include <cstdint>
static std::uint64_t testNs=10000000000ull;
static unsigned clockCalls=0;
static std::uint64_t clockD68() noexcept {++clockCalls;return testNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK clockD68
#include "diagnostics/motion_trace_d68.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <vector>
namespace md=kcd2vr::diagnostics::motion_d68;
namespace pd=kcd2vr::diagnostics::perf_d61;
static unsigned checks=0;
#define CHECK(x) do {++checks;if(!(x)){std::printf("FAIL line=%d %s\n",__LINE__,#x);std::exit(1);}} while(0)
static void empty(){md::Event e{};while(md::pop(e)){};}
static std::uint64_t start(){empty();pd::gate.select(true,testNs/1000000);return pd::gate.serial.load();}
static void queueTests(){
    pd::gate.select(false,testNs/1000000);const auto before=clockCalls;md::Event e{};
    CHECK(!md::begin(e,md::CoreBefore));CHECK(!md::current(1));CHECK(clockCalls==before);
    const auto session=start();CHECK(md::begin(e,md::Mapping));CHECK(e.session==session&&md::inWindow(e));
    const auto beginNs=e.ns;
    for(unsigned stage=1;stage<=md::HandPrediction;++stage){
        e.stage=stage;e.ns=beginNs+249999999;CHECK(md::inWindow(e));
        const auto was=md::outsideWindow[stage].load();e.ns=beginNs+250000000;CHECK(!md::inWindow(e));CHECK(md::outsideWindow[stage].load()==was+1);
        e.ns=beginNs+1999999999;CHECK(!md::inWindow(e));e.ns=beginNs+2000000000;CHECK(md::inWindow(e));
    }
    e.ns=beginNs-1;CHECK(!md::inWindow(e));e.ns=beginNs;e.session=session-1;CHECK(!md::inWindow(e));
    md::Cadence cadence;CHECK(cadence.due(session,beginNs));CHECK(!cadence.due(session,beginNs));
    CHECK(!cadence.due(session,beginNs+2999999));CHECK(cadence.due(session,beginNs+3000000));
    CHECK(cadence.due(session+1,beginNs));CHECK(cadence.skipped==0);CHECK(!cadence.due(0,beginNs));
    CHECK(md::begin(e,md::CoreBefore));e.sample=987;e.sourceNs=111;e.endNs=222;e.inputTick=333;e.priorSample=444;e.priorSourceNs=555;e.request=666;
    e.thread=777;e.flags=md::StateKnown|md::ContactRelease|md::FingerContact;e.cycle=999;e.count=2;e.v[0]=1.25f;e.v[1]=-2.5f;
    CHECK(md::push(e));md::Event out{};CHECK(md::pop(out));CHECK(!md::pop(out));
    CHECK(out.sample==987&&out.endNs==222&&out.priorSample==444&&out.request==666&&out.cycle==999&&out.v[1]==-2.5f);
    CHECK(out.order!=0&&out.flags==e.flags);
    const auto blocked=md::dropped.load();md::busy.test_and_set();CHECK(!md::push(e));md::busy.clear();CHECK(md::dropped.load()==blocked+1);
    for(unsigned n=0;n<511;++n){e.sample=n;CHECK(md::push(e));}
    CHECK(!md::push(e));for(unsigned n=0;n<511;++n){CHECK(md::pop(out));CHECK(out.sample==n);}CHECK(!md::pop(out));
    e.count=49;CHECK(!md::push(e));e.count=0;e.stage=unsigned(md::HandPrediction)+1;CHECK(!md::push(e));e.stage=md::Mapping;
    e.count=48;for(unsigned i=0;i<48;++i)e.v[i]=(i&1)?-std::numeric_limits<float>::max():std::numeric_limits<float>::denorm_min();
    e.ns=e.endNs=e.sourceNs=e.sourceTick=e.sample=e.trackingEpoch=e.poseEpoch=e.publication=e.frame=e.cycle=e.order=e.inputTick=e.priorSample=e.priorSourceNs=e.request=UINT64_MAX;
    e.object=e.actor=e.descriptor=e.pose=e.output=UINTPTR_MAX;e.targetXrNs=INT64_MIN;e.thread=UINT32_MAX;
    char line[2048]{};CHECK(md::format(e,line,sizeof(line)));CHECK(std::strlen(line)<2048);
    CHECK(std::strstr(line,"endNs=18446744073709551615")&&std::strstr(line,"meshSubmission=UNOBSERVED"));
    char small[20]{};CHECK(!md::format(e,small,sizeof(small)));CHECK(small[19]==0);
    CHECK(md::accept(line,testNs/1000000));CHECK(!md::accept("D68_MOTION session=-1",testNs/1000000));
    CHECK(!md::accept("D68_MOTION session=1wrong",testNs/1000000));
    CHECK(md::begin(e,md::Mapping));e.count=1;e.v[0]=.1f;CHECK(md::push(e));
    pd::gate.select(false,testNs/1000000);CHECK(!md::push(e));std::vector<std::string> lines;
    md::drain(testNs/1000000+5000,[&](const char* s){lines.emplace_back(s);});CHECK(lines.size()==1);
    CHECK(md::accept(lines[0].c_str(),testNs/1000000+5000));CHECK(!md::accept(lines[0].c_str(),testNs/1000000+5001));
    const auto expired=md::expired.load();start();CHECK(md::begin(e,md::Mapping));CHECK(md::push(e));
    pd::gate.select(true,testNs/1000000);md::drain(testNs/1000000,[&](const char*){CHECK(false);});CHECK(md::expired.load()==expired+1);
    CHECK(md::begin(e,md::Mapping));testNs+=pd::DurationMs*1000000ull;CHECK(!md::push(e));CHECK(!md::begin(e,md::Mapping));
}
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
struct Dq32 {float a[4]{};float b[4]{};};
struct RigMap {int hand_r=1,hand_l=2;};
struct TrackingSnapshot {std::uint64_t timeNs=0,tick=0,sequence=0,source_epoch=0,input_tick=0;std::int64_t poseTime=0;};
constexpr unsigned kPoseCapacity6689=4;
struct CoherentTransaction6689 {
    std::uint64_t motionSessionD68=0,motionCycleD68=0,motionSourceNsD68=0,motionSourceTickD68=0,motionSampleD68=0,motionTrackingEpochD68=0,motionInputTickD68=0;
    std::int64_t motionTargetXrNsD68=0;std::uintptr_t motionDescriptorD68=0;
    unsigned motionHandFlagsD68[2]{};bool motionCommittedD68=false,traceNativeReturnedH29=false;
    bool selected[4]{},attempted[4]{};
};
struct State {bool ready=false;void* owner=nullptr;void* character=nullptr;unsigned count=0;QuatT28 work[4]{},original[4]{};} g_pose6689;
static unsigned readCount=0,getCount=0;
template<class T> static bool Read(const void* p,T& out) noexcept {if(!p)return false;++readCount;std::memcpy(&out,p,sizeof(T));return true;}
using PoseGetFn=bool(*)(void*,int,QuatT28&) noexcept;
static bool ReadQuat(PoseGetFn fn,void* p,int i,QuatT28& q) noexcept {return fn&&fn(p,i,q);}
static bool Getter(void* p,int i,QuatT28& out) noexcept {++getCount;out=static_cast<QuatT28*>(p)[i];return true;}
// Only the enclosing transaction/event transport is mocked. The checked native
// descriptor, two-joint reads, raw palette observations and guards below are the
// production include, not a copied implementation.
static bool MotionEventD68(const CoherentTransaction6689& tx,md::Event& e,md::Stage stage) noexcept {
    if(!tx.motionSessionD68||!md::begin(e,stage,false)||e.session!=tx.motionSessionD68)return false;
    e.cycle=tx.motionCycleD68;e.sample=tx.motionSampleD68;e.sourceNs=tx.motionSourceNsD68;return true;
}
#include "vrik/motion_skin_d68.inl"
static void adapterTests(){
    testNs=500000000000ull;const auto session=start();
    QuatT28 bones[4]{};Dq32 palette[4]{};unsigned char character[32]{};int owner=1;
    void* desc[5]{bones,&owner,character+8,nullptr,palette};
    CoherentTransaction6689 tx{};RigMap rig{};TrackingSnapshot tracking{44,55,66,77,88,99};
    tx.motionDescriptorD68=reinterpret_cast<std::uintptr_t>(desc);tx.motionCycleD68=100;
    g_pose6689.ready=true;g_pose6689.owner=&owner;g_pose6689.character=character;g_pose6689.count=4;
    bones[1].t[0]=.2f;bones[2].t[1]=.4f;g_pose6689.work[1].t[0]=.1f;g_pose6689.work[2].t[1]=.3f;
    palette[1].a[0]=12.5f;palette[1].b[3]=-3.5f;palette[2].a[2]=5.5f;
    tx.selected[1]=tx.selected[2]=tx.attempted[1]=tx.attempted[2]=true;
    tx.motionHandFlagsD68[0]=md::StateKnown|md::FreeContactEligible|md::ContactFresh;
    tx.motionHandFlagsD68[1]=md::StateKnown|md::PhysicalOwner;
    const auto bcopy=bones[1];const auto pcopy=palette[1];const auto dcopy=desc[4];
    const auto r=readCount,g=getCount;MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,true);
    MotionCoreAfterD68(tx,desc,bones,&owner,character,palette,rig,4,true);CHECK(readCount==r&&getCount==g);
    tx.motionSessionD68=session;MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,true);
    CHECK(getCount==g+2);CHECK(tx.motionCommittedD68&&tx.motionSampleD68==66&&tx.motionInputTickD68==88);
    md::Event right{},left{};CHECK(md::pop(right)&&md::pop(left));CHECK(!md::pop(left));
    CHECK(right.stage==md::CoreBefore&&right.side==0&&right.cycle==100&&right.sample==66);
    CHECK((right.flags&127)==127&&(right.flags&md::StateKnown)&&(right.flags&md::FreeContactEligible));
    CHECK(right.count==22&&right.v[4]==.2f&&right.v[11]==.1f&&right.v[21]==1);
    CHECK(left.side==1&&(left.flags&md::PhysicalOwner)&&left.v[5]==.4f&&left.v[21]==2);
    MotionCoreAfterD68(tx,desc,bones,&owner,character,palette,rig,4,true);CHECK(md::pop(right)&&md::pop(left));CHECK(!(right.flags&1));
    tx.traceNativeReturnedH29=true;MotionCoreAfterD68(tx,desc,bones,&owner,character,palette,rig,4,true);CHECK(md::pop(right)&&md::pop(left));
    CHECK((right.flags&31)==31&&right.count==16&&right.v[0]==12.5f&&right.v[7]==-3.5f&&right.v[15]==1);
    CHECK(left.v[2]==5.5f&&left.v[15]==2);
    CHECK(!std::memcmp(&bcopy,&bones[1],sizeof(bcopy))&&!std::memcmp(&pcopy,&palette[1],sizeof(pcopy))&&desc[4]==dcopy);
    const auto got=getCount;desc[2]=character;
    MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,false);CHECK(md::pop(right)&&md::pop(left));CHECK(getCount==got&&!(right.flags&1)&&!(right.flags&64));
    MotionCoreAfterD68(tx,desc,bones,&owner,character,palette,rig,4,false);CHECK(md::pop(right)&&md::pop(left));CHECK(!(right.flags&1));
    desc[2]=character+8;rig.hand_l=9;MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,true);
    CHECK(md::pop(right)&&md::pop(left));CHECK((right.flags&1)&&!(left.flags&1));
    const auto got2=getCount;MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,5,Getter,tracking,true);
    CHECK(md::pop(right)&&md::pop(left));CHECK(getCount==got2&&!(right.flags&1));
    g_pose6689.owner=nullptr;MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,true);
    CHECK(md::pop(right)&&md::pop(left));CHECK((right.flags&1)&&!(right.flags&2));
    pd::gate.select(true,testNs/1000000);const auto rr=readCount;
    MotionCoreBeforeD68(tx,desc,bones,&owner,character,palette,rig,4,Getter,tracking,true);MotionCoreAfterD68(tx,desc,bones,&owner,character,palette,rig,4,true);
    CHECK(readCount==rr&&!md::pop(right));
}
int main(){queueTests();adapterTests();std::printf("PASS motion trace + actual native observer %u checks\n",checks);return 0;}
