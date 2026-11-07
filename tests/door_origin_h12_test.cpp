// Actual MID/leaf wrappers, original-once native leaf and exact reader/policy
// helpers. Native patching/engine calls are leaves, not claimed game execution.
#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <array>
#include <optional>
#include <mutex>
#include "interaction_alignment.h"
#include "core/door_candidates38.h"
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include "core/door_origin_h12.h"
#include "core/door_use46.h"
#include "vrik/workstation178r20c1.h"
#include "tracking_sample59c6.h"
using namespace kcd2vr;
static std::uintptr_t g_wh_base=0x180000000ull;
static std::atomic<bool> g_enabled{true},g_split_ready57c1{true},g_visual_fallback_enabled{false};
static std::uint64_t nowH12=1000,generationH12=3;
static unsigned checks=0,calls=0,creates=0,pins=0,enables=0,disables=0;
static int failCreate=0,failEnable=0;static bool badPin=false,badRetail=false,badModule=false,failDisable=false;
static const float* seenPoint=nullptr;static const float* seenBounds=nullptr;static float* seenDistance=nullptr;
static void ck(bool v,const char* text){++checks;if(!v){std::fprintf(stderr,"FAIL %u %s\n",checks,text);std::exit(2);}}
static std::uint64_t ClockH12(){return nowH12;}
static std::uint64_t DoorAcquisitionGeneration65(){return generationH12;}
static bool MatchEntry(std::uintptr_t,const unsigned char*,std::size_t){return !badPin;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return !badRetail;}}
static BOOL ModuleH12(DWORD flags,LPCWSTR,HMODULE* module){++pins;ck((flags&GET_MODULE_HANDLE_EX_FLAG_PIN)!=0,"pinned module");*module=badModule?nullptr:reinterpret_cast<HMODULE>(1);return !badModule;}
static void (*duringLeaf)()=nullptr;
static char __fastcall OriginalLeaf(std::uintptr_t,const float* point,const float* bounds,float* distance){
    ++calls;seenPoint=point;seenBounds=bounds;seenDistance=distance;
    if(duringLeaf){auto f=duringLeaf;duringLeaf=nullptr;f();}
    for(unsigned n=0;n<3;++n)if(point[n]<bounds[n]||point[n]>bounds[n+3])return 1;
    float d=0;for(unsigned n=0;n<3;++n){const float delta=(bounds[n]+bounds[n+3])*.5f-point[n];d+=delta*delta;}
    *distance=d;return 0;
}
namespace DetourModKit {struct Address{std::uintptr_t value;};namespace hook {
struct Options{bool fail_if_already_hooked=false;};struct Target{Address address;};struct InlineRequest{const char* name;Target target;Options options;};using MidRequest=InlineRequest;
enum class Gpr{Rbp,Rsi,R12,R15,Rdx,R8,R9};
struct XmmView{float lane0=0;template<class T>T lane(std::size_t)const{return T(lane0);}};
struct MidContext{std::uintptr_t r[7]{},stack=0;XmmView x[16]{};};
static std::uintptr_t& gpr(MidContext& c,Gpr r){return c.r[unsigned(r)];}static std::uintptr_t stack_pointer(MidContext& c){return c.stack;}static XmmView xmm(MidContext& c,unsigned n){return c.x[n];}
struct Hook{unsigned id=0;bool live=false;
 template<class T>T original(){return id==1?reinterpret_cast<T>(&OriginalLeaf):nullptr;}
 bool enable(){++enables;ck(pins!=0,"module pin before enable");if(failEnable==int(id))return false;live=true;return true;}
 bool is_enabled()const{return live;}
 bool disable(){++disables;if(failDisable)return false;live=false;return true;}
};
template<class F>static std::optional<Hook> inline_at(InlineRequest q,F){ck(q.options.fail_if_already_hooked,"collision refuses");++creates;if(failCreate==int(creates))return std::nullopt;return Hook{creates,false};}
template<class F>static std::optional<Hook> mid_at(MidRequest q,F f){return inline_at(q,f);}
}}
static std::optional<DetourModKit::hook::Hook> g_direct_door_hook54r1;
namespace kcd2vr::frik_pose_probe {static unsigned reserved=0;static int item=0;
static bool palmReadable=true;static std::uint64_t poseEpoch=9;
bool ReadInteractionPalm59c9(int side,float& x,float& y,float& z,float&,float&,float&,std::uint64_t& tick,std::uint64_t* epoch,std::uintptr_t* player) noexcept {
 door_hold65::Frame f{};if(!palmReadable||!door_hold65::frame.read(f))return false;const auto& h=f.hand[side];x=h.point.x;y=h.point.y;z=h.point.z;tick=h.poseTick;*epoch=poseEpoch;*player=h.player;return true;
}
unsigned PhysicalHandReservations114() noexcept{return reserved;}int ExactHandItemState(int) noexcept{return item;}}
namespace diagnostics=kcd2vr::diagnostics;
namespace kcd2vr::diagnostic_trace {static void Write(const char*);}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;
#include "generated_door_origin_reads_h12.inc"
#define GetTickCount64 ClockH12
#define GetModuleHandleExW ModuleH12
#include "../src/interaction_door_origin_h12.inl"
#undef GetModuleHandleExW
#undef GetTickCount64
static std::array<unsigned char,0xB20> player{};
static std::array<unsigned char,0x120> interactor{};
static std::array<unsigned char,0x100> henry{},door{},klass{},policy{};
static std::array<unsigned char,0x40> policies{};
static std::array<unsigned char,0x240> stack{};
static DetourModKit::hook::MidContext context{};
template<class T>static std::uintptr_t a(T& v){return reinterpret_cast<std::uintptr_t>(v.data());}
template<class T>static void put(std::uintptr_t where,T value){std::memcpy(reinterpret_cast<void*>(where),&value,sizeof(value));}
static float* point(){return reinterpret_cast<float*>(a(stack)+0x48);}static float* box(){return reinterpret_cast<float*>(a(stack)+0xA8);}static float* distance(){return reinterpret_cast<float*>(a(stack)+0x1C0);}
static void reset(int side=0,unsigned mode=2){
 player={};interactor={};henry={};door={};klass={};policy={};policies={};stack={};context={};
 g_enabled=true;g_split_ready57c1=true;g_visual_fallback_enabled=false;badPin=badRetail=badModule=failDisable=false;failCreate=failEnable=0;
 g_direct_door_hook54r1.emplace(DetourModKit::hook::Hook{3,true});
 g_door_point_hookH12.reset();g_door_point_midH12.reset();g_door_point_originalH12=&OriginalLeaf;g_door_point_readyH12=true;g_door_point_retiredH12=false;
 g_door_point_callH12={};g_door_point_runningH12=false;originH12::receipts={};originH12::next=0;
 (void)door_surface65::requestMailbox.publish({});(void)door_surface65::witnessMailbox.publish({});
 calls=creates=pins=enables=disables=0;duringLeaf=nullptr;seenPoint=nullptr;nowH12=1000;generationH12=3;
 frik_pose_probe::reserved=0;frik_pose_probe::item=0;frik_pose_probe::palmReadable=true;frik_pose_probe::poseEpoch=9;combat_rules178r10::f5Rung=mode;workstation178r20c1::setExperiment36(true);
 tracking59c6::epoch=7;door_use46::SetEnabled(true);
 put(a(player),g_wh_base+0x3AA4B98);put(a(player)+0x38,a(henry));put(a(player)+0xB08,a(interactor));
 put(a(henry),g_wh_base+0x4072C78);put(a(interactor),g_wh_base+0x3A6A0C8);put(a(interactor)+8,a(player));
 put(a(interactor)+0xD8,a(policies));put(a(interactor)+0xE0,std::uintptr_t(1));put(a(policies),1u);put(a(policies)+8,a(policy));
 put(a(policy),g_wh_base+0x3A69D90);put(a(policy)+8,a(player));put(a(door),g_wh_base+0x4072C78);put(a(door)+0xC,42u);
 put(a(door)+0x20,a(klass));put(a(klass),g_wh_base+0x3B282A0);put(a(klass)+0x10,reinterpret_cast<std::uintptr_t>("AnimDoor"));
 const auto frame=a(stack)+0x90;put(frame+0x48,42u);
 float inverse[12]{1,0,0,0,0,1,0,0,0,0,1,0};std::memcpy(reinterpret_cast<void*>(frame-0x20),inverse,sizeof(inverse));
 const float nativePoint[3]{10,20,30};std::memcpy(point(),nativePoint,sizeof(nativePoint));
 const float bounds[6]{-.5f,-.5f,-.5f,.5f,.5f,.5f};std::memcpy(box(),bounds,sizeof(bounds));*distance()=12345;
 using namespace DetourModKit::hook;gpr(context,Gpr::Rbp)=frame;gpr(context,Gpr::Rsi)=a(door);gpr(context,Gpr::R12)=a(policy);
 gpr(context,Gpr::R15)=a(interactor);gpr(context,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(point());gpr(context,Gpr::R8)=reinterpret_cast<std::uintptr_t>(box());gpr(context,Gpr::R9)=reinterpret_cast<std::uintptr_t>(distance());
 context.stack=a(stack)+0x180;context.x[15].lane0=10;context.x[14].lane0=20;context.x[13].lane0=30;
 door_hold65::Frame f{};auto& h=f.hand[side];h.available=true;h.side=side;h.grip=1;h.tick=h.poseTick=nowH12;h.press=800;
 h.player=a(henry);h.poseEpoch=9;h.trackingEpoch=7;h.sourceGeneration=3;h.settingsEpoch=door_use46::settingsEpoch.load();
 h.experimentEpoch=workstation178r20c1::experimentEpoch36.load();h.point={.1f,.2f,.3f};door_hold65::frame.publish(f);
}
static char leaf(){return DoorPointLeafH12(0x777,point(),box(),distance());}
static door_surface65::Witness physicalSurface(){
 door_hold65::Frame f{};door_hold65::frame.read(f);std::array<door_surface65::Request,8> requests{};
 const auto& h=f.hand[0];ck(door_surface65::requests(nowH12,h.player,h.poseEpoch,0,requests)==1,"native entry nominates exactly one bounded physical query before leaf");
 ck(requests[0].nativePoint.x==0&&requests[0].nativePoint.y==0&&requests[0].nativePoint.z==0,"copied native inverse supplies world bounds centre hint");
 door_surface65::Witness w{};w.source=requests[0];w.queryTick=nowH12;w.sample=nowH12;w.actualPalm=h.point;
 w.point={.5f,0,0};w.normal={1,0,0};w.entityId=42;w.opaqueOwner=a(door);w.opaqueCollider=0x1234;w.distance=.1f;w.valid=true;return w;
}

// Connect the actual post-leaf receipt to the actual completed descriptor
// observer. Only native entity lookup and physical-hit leaves are supplied.
using kcd2vr::interaction_alignment::HandCandidate;
static constexpr std::uintptr_t kObjectPolicy=0x3A69D90;
static std::mutex g_hand_candidate_mutex178r14;
static std::uint64_t& g_door_acquisition_generation65=generationH12;
static std::uint64_t& g_door_generation38=generationH12;
static door_candidates38::Records<HandCandidate> g_door_candidates38;
static core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
static bool HandRayActive(){return true;}
static std::uint64_t DoorSourceGeneration38(){return generationH12;}
namespace kcd2vr::diagnostic_trace {static void Write(const char*){}}
static void SelectionReceipt59c10(std::uint64_t,std::uint32_t,int,int,float,float,std::uint32_t){}
static interaction57c1::Kind CandidateKind57c1(std::uintptr_t i,std::uint32_t id,std::uintptr_t& p,char* cls,std::uintptr_t* e){
    p=0;*e=0;if(i!=a(interactor)||id!=42)return interaction57c1::Kind::Unknown;
    p=a(player);*e=a(door);std::strcpy(cls,"AnimDoor");return interaction57c1::Kind::Object;
}
#define GetTickCount64 ClockH12
#include "../src/interaction_direct_door54r1.inl"
#undef GetTickCount64
static void completedDescriptor(){
    using namespace DetourModKit::hook;
    alignas(8) unsigned char payload[0x38]{};put(reinterpret_cast<std::uintptr_t>(payload)+0x10,42u);
    MidContext c{};gpr(c,Gpr::R15)=a(interactor);gpr(c,Gpr::R12)=a(policy);gpr(c,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(payload);
    gpr(c,Gpr::Rbp)=a(stack)+0x90;const auto original=c;
    (void)door_surface65::requestMailbox.publish({});(void)door_surface65::witnessMailbox.publish({});
    palm59c9::Pair palms{};g_palm_packets59c9.publish(palms);
    NativeDirectDoor54r1(c);ck(std::memcmp(&c,&original,sizeof(c))==0,"completed descriptor stays native");
    HandCandidate candidate{};ck(!ReadDirectDoor54r1(0,nowH12,candidate),"inside-box alone is not a20cm physical contact");
    door_hold65::Frame f{};door_hold65::frame.read(f);const auto& h=f.hand[0];
    std::array<door_surface65::Request,8> requests{};const auto count=door_surface65::requests(nowH12,h.player,h.poseEpoch,0,requests);
    ck(count==1&&requests[0].target==42,"same native descriptor nominates surface query after real leaf");
    door_surface65::Witness w{};w.source=requests[0];w.queryTick=nowH12;w.sample=nowH12;w.actualPalm=h.point;
    w.point={h.point.x,h.point.y+.08f,h.point.z};w.normal={0,-1,0};w.entityId=42;w.opaqueOwner=a(door);w.opaqueCollider=0x1234;w.distance=.08f;w.valid=true;
    ck(door_surface65::publish(w),"actual physics leaf supplies matching20cm surface");NativeDirectDoor54r1(c);
    ck(ReadDirectDoor54r1(0,nowH12,candidate)&&candidate.entity==42&&candidate.owner==0&&candidate.distance<.081f,
        "actual leaf receipt -> completed native descriptor -> actual current surface -> exact offer");
    ++gpr(c,Gpr::Rbp);NativeDirectDoor54r1(c);ck(!ReadDirectDoor54r1(0,nowH12,candidate),"foreign selector frame cannot inherit adapted origin");
}

static bool sehLeafH12(){bool caught=false;__try{(void)leaf();}__except(EXCEPTION_EXECUTE_HANDLER){caught=true;}return caught;}
int main(){
 for(unsigned mode:{0u,2u,3u})for(int side=0;side<2;++side){reset(side,mode);const auto original=context;DoorPointPrepareH12(context);
  ck(std::memcmp(&original,&context,sizeof(context))==0,"MID changes no registers");ck(g_door_point_callH12.pending,"current empty original held palm qualifies without DoorRequest");
  ck(leaf()==0&&calls==1&&seenPoint!=point()&&seenBounds==box()&&seenDistance==distance(),"original once with only palm point substituted");
  ck(std::fabs(*distance()-.14f)<.00001f,"original squared centre distance forwarded");
  const auto* receipt=originH12::find(42);door_hold65::Frame f{};door_hold65::frame.read(f);
  ck(receipt&&originH12::joins(*receipt,42,a(door),a(policy),a(player),a(stack)+0x90,f.hand[side],nowH12),"typed same target/hand/frame/stamp join");
  auto h=f.hand[side];++h.poseTick;ck(!originH12::joins(*receipt,42,a(door),a(policy),a(player),a(stack)+0x90,h,nowH12),"changed committed sample cannot borrow receipt");
  ck(!originH12::joins(*receipt,99,a(door),a(policy),a(player),a(stack)+0x90,f.hand[side],nowH12),"wrong candidate cannot borrow receipt");
  for(unsigned bad=0;bad<17;++bad){auto r=*receipt;
   if(bad==0)++r.target;if(bad==1)++r.entity;if(bad==2)++r.policy;if(bad==3)++r.nativePlayer;if(bad==4)++r.frame;
   if(bad==5)r.tick=919;if(bad==6)r.hand.side=1-side;if(bad==7)++r.hand.player;if(bad==8)++r.hand.poseEpoch;
   if(bad==9)++r.hand.trackingEpoch;if(bad==10)++r.hand.sourceGeneration;if(bad==11)++r.hand.settingsEpoch;
   if(bad==12)++r.hand.experimentEpoch;if(bad==13)++r.hand.press;if(bad==14)++r.hand.poseTick;
   if(bad==15)r.hand.point.x+=.01f;if(bad==16)r.hand.available=false;
   ck(!originH12::joins(r,42,a(door),a(policy),a(player),a(stack)+0x90,f.hand[side],nowH12),"receipt identity and original source cannot change");
  }
  ck(leaf()==1&&calls==2&&seenPoint==point(),"one-shot source cannot replay");
 }
 reset();{
  const float inverse[12]{0,1,0,-10,-1,0,0,20,0,0,2,-60};
  const float native[3]{10,10,0},bounds[6]{-10,19,-60,-9,20,-59};
  std::memcpy(reinterpret_cast<void*>(a(stack)+0x70),inverse,sizeof(inverse));std::memcpy(point(),native,sizeof(native));std::memcpy(box(),bounds,sizeof(bounds));
  DoorPointPrepareH12(context);ck(leaf()==0&&calls==1,"rotated translated scaled native inverse drives original containment");
  const auto* r=originH12::find(42);ck(r&&std::fabs(r->local[0]+9.8f)<.0001f&&std::fabs(r->local[1]-19.9f)<.0001f&&std::fabs(r->local[2]+59.4f)<.0001f,
    "native row-major Matrix34 point is not an invented wrist basis");
 }
 reset();DoorPointPrepareH12(context);ck(leaf()==0,"connected positive native containment");completedDescriptor();
 reset();combat_rules178r10::f5Rung=1;DoorPointPrepareH12(context);
 ck(g_door_point_callH12.pending&&leaf()==0,"F5 recorder rung1 preserves normal near-door hand point");
 // Every refusal runs the unmodified original leaf once. No fake inside result.
 for(unsigned bad=0;bad<21;++bad){reset();door_hold65::Frame f{};door_hold65::frame.read(f);
  if(bad==0)put(a(klass)+0x10,reinterpret_cast<std::uintptr_t>("PickableItem"));
  if(bad==1)put(a(policy),g_wh_base+0x3A69E48);
  if(bad==2)put(a(policies),4u);
  if(bad==3)put(a(door)+0xC,99u);
  if(bad==4)f.hand[0].player=a(door);
  if(bad==5)f.hand[0].poseTick=919;
  if(bad==6)f.hand[0].trackingEpoch=8;
  if(bad==7)f.hand[0].sourceGeneration=4;
  if(bad==8)++f.hand[0].settingsEpoch;
  if(bad==9)++f.hand[0].experimentEpoch;
  if(bad==10)f.hand[0].grip=0;
  if(bad==11)frik_pose_probe::reserved=1;
  if(bad==12)frik_pose_probe::item=1;
  if(bad==13)point()[0]+=1;
  if(bad==14)box()[0]=1;
  if(bad==15)g_visual_fallback_enabled=true;
  if(bad==16)combat_rules178r10::f5Rung=4; // unknown mode, not recorder rung1
  if(bad==17)frik_pose_probe::palmReadable=false;if(bad==18)++frik_pose_probe::poseEpoch;
  if(bad==19)put(a(stack)+0x70,std::numeric_limits<float>::quiet_NaN());if(bad==20)context.x[15].lane0=std::numeric_limits<float>::quiet_NaN();
  door_hold65::frame.publish(f);DoorPointPrepareH12(context);ck(!g_door_point_callH12.pending,"refused source never modifies native point");
  ck(leaf()==1&&calls==1&&seenPoint==point()&&*distance()==12345&&!originH12::find(42),"refusal preserves native AL1 and untouched distance");
 }
 for(unsigned bad=0;bad<3;++bad){reset();DoorPointPrepareH12(context);
  float otherPoint[3]{10,20,30},otherBounds[6]{-.5f,-.5f,-.5f,.5f,.5f,.5f},otherDistance=12345;
  const float* p=bad==0?otherPoint:point();const float* b=bad==1?otherBounds:box();float* d=bad==2?&otherDistance:distance();
  ck(DoorPointLeafH12(0x777,p,b,d)==1&&calls==1&&seenPoint==p&&seenBounds==b&&seenDistance==d&&*d==12345,
     "mismatched point/bounds/output address forwards exact unmodified original once");
 }
 reset();door_hold65::Frame f{};door_hold65::frame.read(f);f.hand[0].point={2,0,0};door_hold65::frame.publish(f);
 DoorPointPrepareH12(context);ck(!g_door_point_callH12.pending,"remote palm retains original native point instead of hijacking the door");
 ck(leaf()==1&&calls==1&&*distance()==12345&&!originH12::find(42),"outside remains outside, no box inflation or false receipt");
 // Actual production entry -> bounded physical mailbox -> original native leaf.
 // Palm outside the thin box is not itself proof of a door contact.
 for(unsigned bad=0;bad<12;++bad){reset();door_hold65::frame.read(f);f.hand[0].point={.6f,0,0};door_hold65::frame.publish(f);
  DoorPointPrepareH12(context);auto w=physicalSurface();ck(leaf()==1&&calls==1&&!originH12::find(42),"nomination alone never changes native outside result");
  if(bad==1)w.opaqueOwner=a(henry);if(bad==2)++w.source.press;if(bad==3)++w.source.sourceGeneration;
  if(bad==4)++w.source.settingsEpoch;if(bad==5)++w.source.trackingEpoch;
  if(bad==6)w.source.target=w.entityId=99;if(bad==7)w.queryTick=919;
  if(bad==8)w.point.x=.3f; // actual surface over20cm away: no publish
  if(bad==9)w.valid=false; // occlusion/no hit cannot supply a contact
  const bool published=door_surface65::publish(w);
  if(bad==7||bad==8||bad==9)ck(!published,"invalid/stale/too-far physical proof cannot publish");
  DoorPointPrepareH12(context);
  if(bad==10){(void)door_surface65::withdraw(0,w.sample,w.queryTick);}
  if(bad==11){++tracking59c6::epoch;}
  const auto result=leaf();
  if(bad==0){ck(result==0&&calls==2&&g_door_point_callH12.usesSurface&&originH12::find(42),"actual exact-door physical surface reaches ORIGINAL native inside acceptance");
   ck(std::fabs(*distance()-.25f)<.00001f,"distance is original native surface result");
   ck(originH12::find(42)->hand.point.x==.6f&&originH12::find(42)->local[0]==.5f,"receipt retains actual palm source and distinct physical local point");
   alignas(8) unsigned char payload[0x38]{};put(reinterpret_cast<std::uintptr_t>(payload)+0x10,42u);
   using namespace DetourModKit::hook;MidContext c{};gpr(c,Gpr::R15)=a(interactor);gpr(c,Gpr::R12)=a(policy);
   gpr(c,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(payload);gpr(c,Gpr::Rbp)=a(stack)+0x90;
   NativeDirectDoor54r1(c);HandCandidate candidate{};
   ck(ReadDirectDoor54r1(0,nowH12,candidate)&&candidate.entity==42&&candidate.owner==0&&candidate.distance<.101f,
      "outside palm -> real surface -> ORIGINAL AL0 -> actual completed descriptor -> exact current door candidate");}
  else ck(result==1&&calls==2&&!originH12::find(42),"foreign/stale/revoked surface never opens native door gate");
 }
 reset();door_hold65::frame.read(f);f.hand[0].point={2.4548f,-3.2661f,.8304f};door_hold65::frame.publish(f);
 DoorPointPrepareH12(context);ck(leaf()==1&&!originH12::find(42),"actual H25 far local palm remains outside without same-door20cm physical evidence");
 reset(0,3);DoorPointPrepareH12(context);std::array<door_surface65::Request,8> none{};door_hold65::frame.read(f);
 ck(door_surface65::requests(nowH12,f.hand[0].player,f.hand[0].poseEpoch,0,none)==0,"legacy F5rung3 does not acquire spatial surface authority");
 {float out[3]{},identity[12]{1,0,0,0,0,1,0,0,0,0,1,0};const float b[6]{-1,-2,-3,1,2,3};
  ck(originH12::worldCenter(identity,b,out)&&out[0]==0&&out[1]==0&&out[2]==0,"affine centre identity");
  const float m[12]{0,1,0,-10,-1,0,0,20,0,0,2,-60};
  ck(originH12::worldCenter(m,b,out)&&out[0]==20&&out[1]==10&&out[2]==30,"affine centre rotation translation scale");
  identity[0]=0;ck(!originH12::worldCenter(identity,b,out),"singular inverse cannot nominate a guessed world centre");}
 reset();DoorPointPrepareH12(context);++tracking59c6::epoch;ck(leaf()==1&&calls==1&&seenPoint==point(),"epoch revocation immediately before leaf");
 reset();DoorPointPrepareH12(context);duringLeaf=[](){++tracking59c6::epoch;};ck(leaf()==0&&!originH12::find(42),"revocation during original cannot publish authority");
 reset();DoorPointPrepareH12(context);duringLeaf=[](){DoorPointPrepareH12(context);ck(!g_door_point_callH12.pending,"nested prepare is masked");};
 ck(leaf()==0&&calls==1&&!g_door_point_runningH12,"nested work leaves outer source intact");
 reset();DoorPointPrepareH12(context);duringLeaf=[](){RaiseException(0xE1230012,0,0,nullptr);};
 ck(sehLeafH12()&&!g_door_point_runningH12&&!g_door_point_callH12.pending&&!originH12::find(42),"native exception propagates and clears running/pending");
 ck(leaf()==1&&calls==2,"after exception native original remains callable");
 for(unsigned bad=0;bad<6;++bad){reset();g_door_point_readyH12=false;
  if(bad==0)badPin=true;if(bad==1)badRetail=true;if(bad==2)badModule=true;if(bad==3)failCreate=2;if(bad==4)failEnable=2;if(bad==5)g_direct_door_hook54r1.reset();
  ck(!InstallDoorPointH12(g_wh_base)&&!g_door_point_readyH12,"unproven/partial install refuses authority");
  if(bad>=3&&bad<=4){ck(g_door_point_retiredH12&&g_door_point_hookH12.has_value(),"partial install keeps executable handle");const auto n=creates;ck(!InstallDoorPointH12(g_wh_base)&&creates==n,"partial install never retries");}
 }
 reset();g_door_point_readyH12=false;ck(InstallDoorPointH12(g_wh_base)&&creates==2&&enables==2,"both hooks prepared before authority");
 DoorPointPrepareH12(context);failDisable=true;StopDoorPointH12();ck(g_door_point_retiredH12&&!g_door_point_readyH12&&g_door_point_midH12&&g_door_point_hookH12,"retirement retains uncertain executable callbacks");
 ck(leaf()==1&&calls==1&&seenPoint==point(),"retired leaf forwards original even after failed disable");
 std::printf("door_origin_h12: %u checks PASS; actual native point input/source/result/receipt and hook lifecycle\n",checks);
}
