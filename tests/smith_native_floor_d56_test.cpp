// Executes the complete D56 native-reference adapter and nested Assetslot read.
// Native reads, root transform and local bounds are explicit host leaves; no
// learned hit, ray hit, native scoring, engine execution or headset result.
#include "vrik/smith_tool36.h"
#include "vrik/minigame_hands45.h"
#include "vrik/station_ws_j27.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <limits>
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
#include "generated_adapter_types.h"
namespace st=kcd2vr::smith_tool36;
struct ToolSocket59c10 {
 st::Binding smith36{};void* attachment=nullptr;void* character=nullptr;void* entity=nullptr;
 std::uint32_t id=0;std::uint64_t epoch=0,tick=0,request=0,modeEpoch67=0;
 int side=-1,joint=-1;bool nativeGrasp13=false,gripReady67=false;QuatT28 relative{},grip67{};
};
#include "vrik/smith_contact_history_r38.h"
template<class T>struct MailboxD56 {T value{};bool available=true;bool read(T& v)const{v=value;return available;}};
static MailboxD56<ToolSocket59c10> g_tool_socket59c10[2];
namespace kcd2vr::workstation178r20c1 {static std::atomic<std::uint64_t> smithHoldEpoch36{7};}
namespace kcd2vr::activity_input163 {
enum class Kind175 {Smithing};
struct W{std::atomic<std::uint64_t> smithEpochC1{11},lifetime175{5};std::atomic<bool> ready{true};
 std::atomic<unsigned> transitionDepth175{0};std::atomic<std::uintptr_t> active[1]{{9}};int phase=11;
 int SmithPhaseC1()const{return phase;}};static W witness175;}
static std::uint64_t now=1000,sample=1;
static std::uint64_t GetTickCount64(){return now;}
static unsigned traces=0;
namespace diagnostic_trace {static void Write(const char*){++traces;}}
static constexpr std::uintptr_t base=0x10000000,character=0x1000,model=0x2000,holder=0x3000;
static constexpr std::uintptr_t attachment=0x4000,object=0x5000,tongsCharacter=0x6000,asset=0x7000,assetObject=0x8000;
static std::uintptr_t WhBase(){return base;}
static std::map<std::uintptr_t,unsigned char> memory;
template<class T>static void put(std::uintptr_t p,const T& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(std::size_t i=0;i<sizeof(v);++i)memory[p+i]=b[i];}
template<class T>static bool Read(const void* p,T& v){auto at=reinterpret_cast<std::uintptr_t>(p);auto* b=reinterpret_cast<unsigned char*>(&v);
 for(std::size_t i=0;i<sizeof(v);++i){auto p=memory.find(at+i);if(p==memory.end())return false;b[i]=p->second;}return true;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return Read(p,v)&&v==base+rva;}
static bool pins=true,live=true,boundsKnown=true;
static bool SmithToolPins36(){return pins;}
static const char* SmithAutoHoldReason36(std::uint64_t){return live?nullptr:"INACTIVE";}
static std::uint32_t replacedEntity=0;
static void* EntityById6694(std::uint32_t id){return reinterpret_cast<void*>(0xA000+32*id+(id==replacedEntity?0x10000:0));}
static bool PhysEntityIdentity135(void* p,std::uint32_t id){return p==EntityById6694(id);}
static void* CharacterForEntity6689(void* p){return p==EntityById6694(43)?reinterpret_cast<void*>(tongsCharacter):nullptr;}
static void* CharAttachment59c15(void* p,const char* name){return p==reinterpret_cast<void*>(tongsCharacter)&&!std::strcmp(name,"Assetslot")?reinterpret_cast<void*>(asset):nullptr;}
static st::Binding current{};
static bool ReadSmithTool36(void* att,std::uint32_t id,std::uintptr_t ch,st::Binding& out){
 out={};if(!live||current.attachment!=reinterpret_cast<std::uintptr_t>(att)||current.entity!=id||current.character!=ch)return false;out=current;return true;}
static bool ValidPose6689(const QuatT28& p){for(float v:p.q)if(!std::isfinite(v))return false;for(float v:p.t)if(!std::isfinite(v))return false;
 double n=0;for(float v:p.q)n+=double(v)*v;return n>.9&&n<1.1;}
struct ModelWorld6692 {Q4 q{};V3 t{};float scale=1;};static ModelWorld6692 modelWorld{};
static bool ReadModelWorld6692(void* c,ModelWorld6692& out){out=modelWorld;return c==reinterpret_cast<void*>(character);}
static V3 ModelToWorld6692(const ModelWorld6692& w,V3 p){return Add(w.t,Mul(QRotateV(w.q,p),w.scale));}
static V3 WorldToModel6692(const ModelWorld6692& w,V3 p){return Mul(QRotateV(QConj(w.q),Sub(p,w.t)),1.f/w.scale);}
static unsigned boundsReads=0;
static void (*onBoundsD62)()=nullptr;
static bool PhysBounds6698(void* e,float (&out)[6]){++boundsReads;if(!boundsKnown||e!=EntityById6694(44))return false;
 const float b[]{-.3f,-.02f,-.01f,.3f,.02f,.01f};std::copy_n(b,6,out);
 if(onBoundsD62){const auto callback=onBoundsD62;onBoundsD62=nullptr;callback();}return true;}
#include "vrik/smith_native_floor_d56.inl"
struct PoseD56 {unsigned count=2;RigMap rig{};QuatT28 original[2]{};} pose;
static unsigned checks=0;
static void check(bool v,const char* why){++checks;if(!v){std::fprintf(stderr,"FAIL D56 native floor: %s\n",why);std::exit(1);}}
static bool near(float a,float b){return std::fabs(a-b)<1.e-5f;}
static void refresh(){++sample;now+=10;g_tool_socket59c10[1].value.tick=now;g_tool_socket59c10[1].value.smith36=current;
 kcd2vr::activity_input163::witness175.phase=current.phase;}
static void setup(bool nested=false){
 memory.clear();now=1000;sample=1;live=pins=boundsKnown=true;replacedEntity=0;boundsReads=0;onBoundsD62=nullptr;modelWorld={};modelWorld.t={10,20,30};
 g_smith_contact_history_r38={};current={};current.instance=9;current.player=10;current.character=character;current.model=model;
 current.holder=nested?holder+0x100:holder;current.attachment=attachment;current.object=object;current.sourceEntity=current.entity=nested?43:44;
 current.host=77;current.side=1;current.tool=int(nested?st::Tool::Tongs:st::Tool::Workpiece);current.phase=11;current.lifetime=5;
 put(model,base+st::kModelVft);put(model+st::kWorkpieceSlot,holder);put(holder,base+st::kWorkpieceVft);put(holder+st::kHand,int(1));
 put(holder+st::kEntity,std::uint32_t(44));put(holder+st::kHost,nested?std::uint32_t(43):current.host);
 put(holder+st::kAttached,std::uint8_t(1));put(holder+st::kAssetSlot,std::uint8_t(nested));
 put(attachment,base+st::kAttachmentVft);put(attachment+0x20,object);put(object,base+st::kObjectVft);put(object+8,current.entity);
 for(unsigned j=0;j<3;++j)put(object+0x0C+4*j,1.f);
 put(tongsCharacter,base+0x40602F8);put(asset,base+st::kAttachmentVft);put(asset+0x20,assetObject);
 put(assetObject,base+st::kObjectVft);put(assetObject+8,std::uint32_t(44));for(unsigned j=0;j<3;++j)put(assetObject+0x0C+4*j,1.f);
 QuatT28 local{};local.t[2]=.1f;put(asset+0x30,local);
 namespace mh=kcd2vr::minigame_hands45;mh::smithingMode67=mh::SixDofVrik;mh::modeEpoch67=6;
 kcd2vr::station_ws_j27::toolCollision=true;kcd2vr::workstation178r20c1::smithHoldEpoch36=7;
 kcd2vr::activity_input163::witness175.smithEpochC1=11;
 auto& witness=kcd2vr::activity_input163::witness175;witness.ready=true;witness.transitionDepth175=0;
 witness.lifetime175=5;witness.active[0]=9;witness.phase=11;
 auto& sock=g_tool_socket59c10[1].value;sock={};sock.smith36=current;sock.attachment=reinterpret_cast<void*>(attachment);sock.character=reinterpret_cast<void*>(character);
 sock.entity=EntityById6694(current.entity);sock.id=current.entity;sock.side=1;sock.joint=0;sock.epoch=4;sock.request=7;sock.modeEpoch67=6;sock.tick=now;
 sock.gripReady67=true;sock.grip67={};g_tool_socket59c10[1].available=true;
 pose={};pose.rig.hand_l=1;pose.original[0].t[2]=1;pose.original[1].t[2]=1;
}
static bool solve(SmithContactHistoryR38& tx,V3& goal,Q4 q={},int side=1){return SmithNativeFloorD56(reinterpret_cast<void*>(character),4,side,pose,sample,5,now,goal,q,tx);}
#ifndef KCD2VR_SMITH_FLOOR_FIXTURE_ONLY_D62
int main(){
 for(bool nested:{false,true}){
  setup(nested);auto tx=ReadSmithContactHistoryR38();V3 goal{.2f,.3f,.5f};
  check(solve(tx,goal)&&near(goal.z,1)&&near(goal.x,.2f)&&near(goal.y,.3f),"first untouched native idle clamps below item before any learned hit");
  check(tx.nativeFloorD56.have&&!g_smith_contact_history_r38.nativeFloorD56.have,"candidate floor is staged, not published by solving");
  CommitSmithContactHistoryR38(tx);const auto floor=tx.nativeFloorD56.worldBottom;
  current.phase=14;refresh();pose.original[0].t[2]=4;goal={0,0,.5f};tx=ReadSmithContactHistoryR38();
  check(solve(tx,goal)&&near(goal.z,1)&&tx.nativeFloorD56.worldBottom==floor,"authored raised pose cannot move a captured anvil floor");
  refresh();goal={0,0,2};check(solve(tx,goal)&&near(goal.z,2),"upward retreat remains free");
  refresh();goal={0,0,.5f};const Q4 rotate{0,.70710678f,0,.70710678f};
  check(solve(tx,goal,rotate)&&goal.z>1.28f,"rotated workpiece uses all eight native extent corners");
  for(int phase:{8,9,10,12,13,18}){current.phase=phase;refresh();goal={0,0,.5f};
   check(!solve(tx,goal)&&near(goal.z,.5f)&&!tx.nativeFloorD56.have,"transfer/heating/flip retires the reference without constraining hand");}
 }
 setup(true);SmithWorkpieceD56 item{};check(SmithWorkpieceD56Read(current,item)&&near(item.inTool.t[2],.1f),"nested relation uses native Assetslot LOCAL pose");
 for(int fault=0;fault<8;++fault){setup(true);switch(fault){
  case 0:put(holder+st::kAttached,std::uint8_t(0));break;
  case 1:put(holder+st::kHost,std::uint32_t(99));break;
  case 2:put(assetObject+8,std::uint32_t(45));break;
  case 3:put(asset+0x20,std::uintptr_t(0));break;
  case 4:put(assetObject+0x10,2.f);break;
  case 5:put(object+0x0C,2.f);break;
  case 6:put(holder,base+st::kSlotToolVft);break;
  case 7:pins=false;break;}
  check(!SmithWorkpieceD56Read(current,item),"missing/wrong nested owner, class or unsupported scale cannot authorize floor");}
 setup();auto tx=ReadSmithContactHistoryR38();V3 goal{0,0,.5f};check(solve(tx,goal),"fresh direct item acquires reference");
 current.phase=16;refresh();++kcd2vr::activity_input163::witness175.smithEpochC1;goal.z=.4f;
 check(!solve(tx,goal)&&near(goal.z,.4f)&&!tx.nativeFloorD56.have,"new station epoch cannot borrow old floor during fall");
 setup();tx={};goal={0,0,.5f};current.phase=14;refresh();check(!solve(tx,goal),"fall/raise never acquires a floor without native idle");
 setup();tx={};goal={0,0,.5f};g_tool_socket59c10[1].value.tick=now-81;check(!solve(tx,goal),"stale socket has no floor ownership");
 setup();tx={};goal={0,0,.5f};boundsKnown=false;check(!solve(tx,goal),"missing native bounds never invent an item size or plane");
 setup();tx={};goal={0,0,.5f};check(!solve(tx,goal,{},0)&&near(goal.z,.5f),"right hammer remains outside the left item floor");
 setup();tx={};goal={0,0,.5f};kcd2vr::station_ws_j27::toolCollision=false;check(!solve(tx,goal),"existing dedicated crafting contact setting remains authoritative");
 setup();tx={};goal={0,0,.5f};modelWorld.scale=2;check(solve(tx,goal)&&near(goal.z,1),"actual character scale participates in native and desired bounds");
 setup();tx={};goal={0,0,1.006f};kcd2vr::crafting_d67::setSmithSemiAnchors(true);
 check(solve(tx,goal)&&near(goal.z,1.006f),"option without held LT never applies the retired8mm resting snap");
 refresh();goal={.2f,.3f,1.01f};check(solve(tx,goal)&&near(goal.z,1.01f)&&near(goal.x,.2f)&&near(goal.y,.3f),"no LT keeps all above-floor controller movement");
 refresh();goal={.2f,.3f,1.02f};check(solve(tx,goal)&&near(goal.z,1.02f),"lifting for inspection releases support immediately without a blend tail");
 refresh();goal={.2f,.3f,1.006f};kcd2vr::crafting_d67::setSmithSemiAnchors(false);
 check(solve(tx,goal)&&near(goal.z,1.006f),"free mode has collision support but no semi resting snap");
 std::printf("D56 native floor: %u checks PASS; native geometry leaves explicit, no headset/scoring claim\n",checks);
}
#endif
