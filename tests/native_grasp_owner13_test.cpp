// Complete production action-owner TU; native leaves and memory are explicitly
// substituted. Proves connected ordering/identity, NOT retail execution or pixels.
#define KCD2VR_GRASP_HOST_TEST 1
#include "../src/diagnostics/interaction_cycle_h4.h"
#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/action_time_owner59c11.cpp"
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
namespace kcd2vr::physics::grab_producer60 {bool ReadStatus(int,Status&) noexcept{return false;}}
namespace kcd2vr::frik_pose_probe {
bool ReadGrabHandWorld60(int,float[3],float[4],std::uint64_t&,std::uint64_t&,std::uintptr_t&,std::uint64_t&) noexcept{return false;}
void PublishVisibleFailure(const char*) noexcept{}
}
namespace kcd2vr::native_script_t2 {
static bool supplyCallbackT5=false,supplyCallbackAvailableT5=true,supplyMalformedT5=false;
static const char* supplyStatusT5="MISSING_ASSETS";
static int supplyDaggersT5=0,supplyAxesT5=0,supplyCallsT5=0;
static bool supplyDrawT13=false;static std::string supplyPacketT13;
bool Register(Client,Wanted) noexcept {return true;}
bool Install() noexcept {return true;}
bool Context::CallPacket(const char* name,const char* input,char* output,std::size_t cap) const noexcept {
 if(!supplyCallbackT5)return false;
 assert(!std::strcmp(name,"kcd2vr_supply_native_t2"));++supplyCallsT5;supplyPacketT13=input;
 if(supplyDrawT13&&kcd2vr::action_time_owner59c11::g_supply_draw_scope_t2){auto& draw=*kcd2vr::action_time_owner59c11::g_supply_draw_scope_t2;draw.calls=1;draw.key=0x0200000000019a5bull;draw.submitted=true;}
 if(!supplyCallbackAvailableT5)return false;
 unsigned long long serial=0;assert(std::sscanf(input,"%llu|",&serial)==1);
 std::snprintf(output,cap,"%s|%llu|%d|%d",supplyStatusT5,serial+(supplyMalformedT5?1:0),supplyDaggersT5,supplyAxesT5);
 return true;
}
}
#include "../src/vrik/pickup_use19c3.h"
#include "../src/vrik/held_interaction114.h"
#include "../src/vrik/hands_mode59c7f.h"
#include "../src/interaction_alignment.h"
#include "../src/tracking_sample59c6.h"
#include "../src/vrik/workstation_sample46.h"
static kcd2vr::workstation_sample46::Frame testWorkstationFrame46{};
// Opt-in native-clock race fixture: an XR publication can supersede input
// while the real consumer is between its two admission checks.
#ifdef KCD2VR_ALCHEMY_INPUT_OBSERVER_D50
static void (*testWorkstationFrameObserverD50)() noexcept=nullptr;
#endif
namespace kcd2vr::frik_pose_probe {
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {
#ifdef KCD2VR_ALCHEMY_INPUT_OBSERVER_D50
    if(testWorkstationFrameObserverD50)testWorkstationFrameObserverD50();
#endif
    out=testWorkstationFrame46;
    return workstation_sample46::fresh(out,GetTickCount64());
}
}
static void* testSkel=nullptr;
namespace kcd2vr::animation_filter {void* PlayerSkeletonAnim() noexcept{return testSkel;}}
#ifdef KCD2VR_RESOURCE_D69_RETAIL_GATE
static bool testRetailReadyD69=true;
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t b) noexcept{return b&&testRetailReadyD69;}bool Verify(std::uintptr_t) noexcept{return testRetailReadyD69;}}
#else
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t b) noexcept{return b!=0;}bool Verify(std::uintptr_t) noexcept{return true;}}
#endif
static void* testAttachManager=nullptr; // C10R18: Henry identity by entity id (R15 path)
namespace kcd2vr::frik_pose_probe {void* HenryItemAttachManager59c15() noexcept{return testAttachManager;}}
namespace kcd2vr::frik_pose_probe {void TickNativeDoor19c4() noexcept{}} // door module has its own native-owner fixture
namespace kcd2vr::frik_pose_probe {void StartNativeDoor19c4() noexcept{} bool StopNativeDoor19c4() noexcept{return true;}}
namespace kcd2vr::frik_pose_probe {void SetInteractionHandContext158(int,bool,float,float,float,float) noexcept{}}
namespace kcd2vr::frik_pose_probe {void NoteVrPickupUse43(int,std::uint32_t,bool,std::uint64_t) noexcept{}} // C43 leaf
namespace kcd2vr::interaction_alignment {
static HandCandidate testCandidate{};
bool LastHandCandidate(HandCandidate& out) noexcept {out=testCandidate;return out.entity!=0;}
bool NativeDirectDoorCandidate54r1(int,std::uint64_t,HandCandidate&) noexcept {return false;} // independently exercised by the door adapter fixture
void SetHandRayWorld(bool,float,float,float,float,float,float,float,float,int) noexcept{}
void SetPalmRays59c9(const palm59c9::Pair&) noexcept{}
std::uint64_t DoorSourceGeneration38() noexcept{return 1;}
bool DoorSourceCurrent38(std::uint64_t generation) noexcept{return generation==1;}
std::uint64_t DoorAcquisitionGeneration65() noexcept{return 1;}
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept{return generation==1;}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept {
 out=testCandidate;return out.entity==target&&out.owner==side&&out.tick&&now>=out.tick&&now-out.tick<=80;
}
}
static std::vector<std::string> lines;
static bool traceEnabled=false;
namespace kcd2vr::diagnostic_trace {bool Enabled() noexcept{return traceEnabled;}void Write(const char*s) noexcept{lines.emplace_back(s);}}
namespace kcd2vr::action_time_owner59c11 { namespace {
static thread_local DWORD pickupThread19c4=1;
DWORD HostPickupThread19c4() noexcept {return pickupThread19c4;}
static std::vector<unsigned char> image(0x5600000);
static std::array<unsigned char,0x1100> character{},player{},manager{},slots{},item{},link{},entity{},logic{},controller{},contexts{},scope{},action{},attachment{},attachmentManager{},attachmentObject{},actionContext{},controllerDefinition{},fragmentDefinition{},fragmentEntries{};
static int worldContainer=1,inventory=2,actorModel=3,itemSystem=4,entitySystem=5;
static constexpr std::uint64_t key=0x0200000000011234ull;
static int attaches=0,transfers=0,notices=0,drops=0,cleanup=0,finishes=0,eventCalls=0;
static bool deferAttachment=false,finishCleanupJob19c4=false,dropCleanupJob19c4=false;
static bool finishUnjoined19c4=false,finishNoCleanup19c4=false,forbidRetiredRead19c4=false;
static bool finishLeavesRoot55=false;
static std::thread unjoinedCleanup19c4;
struct FixtureGate19c4 {
 std::atomic<bool> value{false};std::mutex mutex;std::condition_variable cv;
 bool load(std::memory_order order=std::memory_order_seq_cst)const{return value.load(order);}
 void store(bool v,std::memory_order order=std::memory_order_seq_cst){{std::lock_guard<std::mutex> lock(mutex);value.store(v,order);}cv.notify_all();}
 void wait(bool old,std::memory_order order=std::memory_order_seq_cst){std::unique_lock<std::mutex> lock(mutex);cv.wait(lock,[&]{return value.load(order)!=old;});}
 void notify_all(){cv.notify_all();}
 void operator=(bool v){store(v);}
};
static FixtureGate19c4 cleanupBlocked19c4{},cleanupEntered19c4{},cleanupRelease19c4{};
static unsigned retiredReads19c4=0;
static FixtureGate19c4 outerBlocked19c4{},outerEntered19c4{},outerRelease19c4{},secondOuterEntered19c4{},secondOuterRelease19c4{};
static const char* queuedHelper19c4=nullptr;
bool HostReadAllowed19c4(const void* p,std::size_t n) noexcept {
 if(!forbidRetiredRead19c4)return true;
 const auto a=reinterpret_cast<std::uintptr_t>(p);
 for(const auto* old:{logic.data(),action.data()}){
  const auto first=reinterpret_cast<std::uintptr_t>(old);
  if(a<first+0x1100&&a+n>first){++retiredReads19c4;return false;}
 }
 return true;
}
static bool attachThrows=false,dropAck=true,dropObserved=true,transferAck=true,mutateNotice=false,mergeOnStow=false,itemRetired=false;
static bool dropFixtureT1=false,inventoryAckT1=true,equipmentAckT1=true,bodyReadyT1=false;
static std::array<unsigned char,0x200> frameworkT1{},moduleT1{},definitionT1{},weaponClassT1{};
static std::array<unsigned char,0x100300> inventoryManagerT1{};
static std::array<unsigned char,0x500> rigidT1{};
static int spinCallsT1=0,spinThreadFlagT1=-1;static DropVelocityT1 lastSpinT1{};
static ng13::Pose nativePose{},releasedPose{};static float releasedVelocity[3]{};
template<class T>void put(void* p,std::size_t n,T x){std::memcpy(static_cast<unsigned char*>(p)+n,&x,sizeof(x));}
static void* ItemLeaf(void*,std::uint64_t k){return k==key&&!itemRetired?item.data():nullptr;}
static void* EntityLeaf(void*,std::uint32_t id){return id==72?entity.data():nullptr;}
static void* currentInventory19c4=&inventory;
static void* InvLeaf(void*){return currentInventory19c4;}
static bool FlagLeaf(void*,unsigned){return false;}
static void* ModelLeaf(void*){return &actorModel;}
static void* AttachmentLeaf(void*,const char*){return attachment.data();}
static void (*worldReadChangeH5)()=nullptr;
static void WorldLeaf(void*,void* out){std::memcpy(out,&nativePose,sizeof(nativePose));
 if(worldReadChangeH5){auto change=worldReadChangeH5;worldReadChangeH5=nullptr;change();}}
// C10R18: the getter-entry hook. Default caller = some other native reader (never substituted).
static std::uintptr_t hostCaller=0;static std::array<unsigned char,0x400> flagSource{};
static const void* WorldGetterLeaf(void*,void* out){std::memcpy(out,&nativePose,sizeof(nativePose));return out;}
static void* FlagSourceLeaf(){return flagSource.data();}
static bool lastAttachFlag=false;
static void AttachLeaf(void*,std::uint64_t k,int side,bool,const char* helper,const float*,bool flag){
 queuedHelper19c4=helper;++attaches;lastAttachFlag=flag;put(slots.data(),0x260+8*side,k);if(!deferAttachment)put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));if(attachThrows)throw std::runtime_error("partial native attach");
}
static void DetachLeaf(void*,int side){put(slots.data(),0x260+8*side,std::uint64_t(0));put(attachment.data(),0x20,static_cast<void*>(nullptr));}
static bool QueueLeaf(void*,const void*,void*,bool,bool){return dropAck;}
static void NoticeLeaf(void*,void*,void*){++notices;if(mutateNotice)put(item.data(),0x90,static_cast<void*>(&inventory));}
static bool stowWorldRetired32=false;
static bool TransferLeaf(void* mgr,const void* req,bool keepWorld){
 ++transfers;if(!transferAck)return false;
 assert(Rd<std::uint64_t>(req,0,0)==key);const char* helper=Rd<const char*>(req,0x18,nullptr);assert(helper&&std::strcmp(helper,"slt_0")==0);
 const int side=Rd<int>(req,8,-1);if(Rd<std::uint64_t>(slots.data(),0x260+8*side,0)!=0)return false;
 // The separately retail-pinned8CF798..8CF899 branch is modeled here:
 // a request that animated + keepWorld=false retires its world link BEFORE
 // the native notice/storage, then skips attach. No native deletion is run.
 const bool retireWorld=Rd<unsigned char>(req,0x10,0)&&!keepWorld;
 if(retireWorld){stowWorldRetired32=true;put(item.data(),0x88,static_cast<void*>(nullptr));}
 PickupNoticeDetour13(nullptr,item.data(),player.data());put(item.data(),0x90,static_cast<void*>(&inventory));
 if(!retireWorld)AttachLeaf(mgr,key,side,false,helper,nullptr,false);
 if(mergeOnStow)itemRetired=true;return true;
}
static unsigned corruptRootAtTransfer57=0;
static void EventLeaf(void* l,const void* e){++eventCalls;auto crc=Rd<unsigned>(e,0x1c,0);
 if(crc==101&&corruptRootAtTransfer57){
  if(corruptRootAtTransfer57==1)put(action.data(),0,WhBase()+0x3B13F38);
  else put(action.data(),kActContext,static_cast<void*>(nullptr));
 }
 if(crc==101)PickupTransferDetour13(manager.data(),static_cast<unsigned char*>(l)+0x28,true);
 if(crc==102)PickupCleanupDetour13(l);
}
static void CleanupLeaf(void* l){if(Rd<unsigned char>(l,0xFE,0))return;++cleanup;
 if(cleanupBlocked19c4.load()){cleanupEntered19c4.store(true,std::memory_order_release);cleanupEntered19c4.notify_all();cleanupRelease19c4.wait(false,std::memory_order_acquire);}
 if(Container13(item.data())==&inventory)DetachLeaf(manager.data(),0);
 put(l,0xfe,std::uint8_t(1));
}
static void EndLeaf19c4(void* l,unsigned char a,unsigned char b){assert(a==0&&b==0);PickupCleanupDetour13(l);
 if(outerBlocked19c4.load()){if(pickupThread19c4==30000){secondOuterEntered19c4.store(true);secondOuterRelease19c4.wait(false);}else{outerEntered19c4.store(true,std::memory_order_release);outerEntered19c4.notify_all();outerRelease19c4.wait(false,std::memory_order_acquire);}}}
static void FinishLeaf(void* a){++finishes;if(finishLeavesRoot55)return;put(a,kActStatus,3);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));
 if(finishNoCleanup19c4)return;
 if(finishUnjoined19c4){unjoinedCleanup19c4=std::thread([]{pickupThread19c4=19728;PickupEndDetour19c4(logic.data(),0,0);});return;}
 if(finishCleanupJob19c4){std::thread cleanupJob([]{pickupThread19c4=19728;PickupEndDetour19c4(logic.data(),0,0);});cleanupJob.join();}
 else PickupEndDetour19c4(logic.data(),0,0);}
static void DropLeaf(void* mgr,int side,const float* vel,float){
 ++drops;ng13::Pose scratch{};
 if(dropCleanupJob19c4){std::thread cleanupJob([]{pickupThread19c4=29472;PickupEndDetour19c4(logic.data(),0,0);});cleanupJob.join();}
 // An unrelated reader of the same attachment inside the drop call is never substituted (C10R18 caller filter).
 hostCaller=reinterpret_cast<std::uintptr_t>(image.data())+0x1085F00;assert(AttachmentWorldDetour13(attachment.data(),&scratch)==&scratch);
 hostCaller=reinterpret_cast<std::uintptr_t>(image.data())+np13::kDropPoseReturn;
 releasedPose=*static_cast<const ng13::Pose*>(AttachmentWorldDetour13(attachment.data(),&scratch));
 assert(AttachmentWorldDetour13(attachment.data(),&scratch)==&scratch); // once per drop call
 hostCaller=0;
 for(int i=0;i<3;++i)releasedVelocity[i]=vel[i];
 DetachLeaf(mgr,side);put(item.data(),0x90,static_cast<void*>(&worldContainer));
 if(dropFixtureT1){
  put(item.data(),0x90,inventoryAckT1?static_cast<void*>(inventoryManagerT1.data()+0x100070):static_cast<void*>(&inventory));
  if(equipmentAckT1){put(item.data(),0x60,unsigned(0));std::memset(manager.data()+0x48,0,7*8);}
 }
 std::array<unsigned char,0x50> record{};put(record.data(),0,reinterpret_cast<std::uintptr_t>(mgr));put(record.data(),8,key);
 for(int k=0;k<3;++k)put(record.data(),0x40+k*4,vel[k]);
 if(dropObserved)DropQueueDetour13(nullptr,record.data(),nullptr,false,false);
}
static bool unknownPickingDefinition=false; // No validated Picking CRC: every ownership path must fail closed.
static int fragmentLookups=0,eventOriginalCalls=0;
static const void* refusedWrite19=nullptr;
bool HostWriteAllowed19(const void* p,std::size_t) noexcept{return p!=refusedWrite19;}
static int EventOriginal(void*,void*){++eventOriginalCalls;return 17;}
void HostFragmentRead19c4() noexcept{++fragmentLookups;}
static bool stopAtAlchemyClaim19c4=false;
void HostAlchemyClaim19c4() noexcept {
 if(stopAtAlchemyClaim19c4){g_owner_thread13=99;assert(!StopAlchemyStage19c4()&&g_alchemy_pending19c4);g_owner_thread13=1;}
}
static int queueOriginalCalls=0,updateOriginalCalls=0;
static void QueueActionLeaf(void*,void* a,float queued){++queueOriginalCalls;put(a,kActQueueTime,queued);put(a,kActStatus,int(1));}
static void UpdateActionLeaf(void*,float dt){
 ++updateOriginalCalls;
 // Explicit native-consumer double for the RETAIL-VERIFIED reads at4B3F15 /
 // 4B3A76..4B3B52: the production writes action speed, the native update reads
 // it, caches it on the scope and updates only that scope's animation layers.
 if(Rd<void*>(scope.data(),kScopeAction,nullptr)==action.data()&&Rd<void*>(action.data(),kActRootScope,nullptr)==scope.data()){
  const float speed=Rd<float>(action.data(),kActSpeedBias,-1);
  put(scope.data(),kScopeSpeedBias,speed);
  put(action.data(),kActActiveTime,Rd<float>(action.data(),kActActiveTime,0)+dt*speed);
 }
}
static int numAnimationsH23=1;
static int NumLeaf(void*,int){return numAnimationsH23;}
static void LayerScaleLeafH23(void* skel,int layer,float speed){put(skel,0x50+0x40*layer,speed);}
static float TimeLeaf(void*,int,int){return .50f;}
static void* BodyLeafT1(void* e){assert(e==entity.data());return bodyReadyT1?rigidT1.data():nullptr;}
static int DynamicsLeafT1(void* b,void* out){assert(b==rigidT1.data());auto& d=*static_cast<DropDynamicsT1*>(out);
 assert(d.type==8);d.mass=1.5f;d.v[0]=2;d.v[1]=.4f;d.v[2]=-.2f;d.centre[0]=30.8f;return 1;}
static int VelocityLeafT1(void* b,const void* command,int flag){assert(b==rigidT1.data());++spinCallsT1;
 spinThreadFlagT1=flag;lastSpinT1=*static_cast<const DropVelocityT1*>(command);return 1;}
std::uintptr_t HostCaller13() noexcept {return hostCaller;}
std::uintptr_t HostNative13(std::uintptr_t rva) noexcept {
#define LEAF(r,f) if(rva==r)return reinterpret_cast<std::uintptr_t>(&f)
 LEAF(np13::kItemLookup,ItemLeaf);LEAF(np13::kEntityLookup,EntityLeaf);LEAF(np13::kActorInventory,InvLeaf);
 LEAF(np13::kItemFlag,FlagLeaf);LEAF(np13::kActorModel,ModelLeaf);LEAF(np13::kFindAttachment,AttachmentLeaf);
 LEAF(np13::kAttachmentWorld,WorldLeaf);LEAF(np13::kAttachOnly,AttachLeaf);LEAF(np13::kDetachHand,DetachLeaf);
 LEAF(np13::kDropHand,DropLeaf);LEAF(np13::kFinishAction,FinishLeaf);
 LEAF(np13::kAttachFlagSource,FlagSourceLeaf);LEAF(kSkelNumAnims,NumLeaf);LEAF(kSkelAnimTime,TimeLeaf);
 LEAF(kLayerScaleH23,LayerScaleLeafH23);LEAF(np13::kEntityPhysics,BodyLeafT1);LEAF(0x8A6C90,DynamicsLeafT1);LEAF(0xAC0D18,VelocityLeafT1);
 std::fprintf(stderr,"unexpected native leaf %zx\n",std::size_t(rva));std::abort();
#undef LEAF
}
static std::uint64_t rawSampleScaleH5=1; // new replay uses real raw-ns versus committed-sequence domains
static void rawFrame55(std::uint64_t tick,float grip=1){
 hostTickCount64=tick;ng13::AcquisitionInput55 input{};
 input.character=reinterpret_cast<std::uintptr_t>(character.data());input.epoch=9;input.trackingEpoch=19;
 input.sample=tick*rawSampleScaleH5;input.tick=tick;input.headTracked=true;input.tracked[0]=true;input.grip[0]=grip;
 PublishAcquisitionInput55(input);
}
static void frame(std::uint64_t tick,float grip=1,float distance=.6f){
 rawFrame55(tick,grip);
 hostTickCount64=tick;ng13::HandFrame f{};f.character=reinterpret_cast<std::uintptr_t>(character.data());f.epoch=9;
 f.sample=tick;f.tick=tick;f.tracked[0]=true;f.grip[0]=grip;f.chestDistance[0]=distance;PublishHands(f);
}
static void reset(){
 rawSampleScaleH5=1;
 worldReadChangeH5=nullptr;
 corruptRootAtTransfer57=0;finishLeavesRoot55=false;numAnimationsH23=1;g_root_before_use55.publish(RootObservation55{});
 assert(!unjoinedCleanup19c4.joinable());finishUnjoined19c4=finishNoCleanup19c4=forbidRetiredRead19c4=false;
 currentInventory19c4=&inventory;retiredReads19c4=0;cleanupBlocked19c4=false;cleanupEntered19c4=false;cleanupRelease19c4=false;queuedHelper19c4=nullptr;
 character.fill(0);player.fill(0);manager.fill(0);slots.fill(0);item.fill(0);link.fill(0);entity.fill(0);logic.fill(0);
 controller.fill(0);contexts.fill(0);scope.fill(0);action.fill(0);attachment.fill(0);attachmentManager.fill(0);attachmentObject.fill(0);actionContext.fill(0);controllerDefinition.fill(0);fragmentDefinition.fill(0);fragmentEntries.fill(0);
 auto b=reinterpret_cast<std::uintptr_t>(image.data());g_base=b;hostModuleHandleW=image.data();hostTickCount64=1000;
 pickupThread19c4=1;g_pickup_job_logic19c4=nullptr;g_owner={};g_pick13={};ClearPickupClockH23();g_native_fault13=false;g_native_ready13=true;g_mode=Own;g_context13=true;g_owner_thread13=1;
 g_pickup_hold62=true;g_pickup_experiment62=false; // each legacy case explicitly selects staged native hold
 g_consumed_use13=0;for(auto& t:g_retired13)t={};g_retired_next13=0;g_foreign_cancel13=false;g_cleanup_claim19c4=0;g_cleanup_done19c4=0;g_cleanup_original19c4=0;g_cleanup_requested19c4=0;g_free_hold19c4r2=0;g_outer_callbacks19c4=0;g_outer_unobserved19c4=0;secondOuterEntered19c4=false;secondOuterRelease19c4=false;outerBlocked19c4=false;outerEntered19c4=false;outerRelease19c4=false;finishCleanupJob19c4=dropCleanupJob19c4=false;g_stow13=g_inside_terminal13=g_in_cleanup13=false;g_event_logic13=nullptr;g_drop_call13=nullptr;
 g_henry_ctrl=controller.data();testSkel=character.data()+0x110;put(testSkel,0x4e8,static_cast<void*>(character.data()));
 auto vft=[b](void*p,std::uintptr_t v){put(p,0,b+v);};
 vft(player.data(),0x3AA4B98);vft(manager.data(),0x3EC9958);vft(item.data(),0x3A2DA88);vft(entity.data(),0x4072C78);
 vft(logic.data(),0x3A5B670);vft(attachment.data(),0x3A657C0);vft(controller.data(),kCtrlVft);vft(attachmentObject.data(),0x3A68F68);put(attachmentObject.data(),8,unsigned(72));
 put(image.data(),0x3A68F68+0x10,b+0x4238B8);put(image.data(),0x3AA4B98+0x610,b+np13::kActorModel);put(image.data(),0x3A657C0+0x70,b+np13::kAttachmentWorld);
 put(image.data(),0x4072C78+0x100,b+np13::kEntityWorld);
 // The entity-system object's leading word is a native vtable pointer, not the integer fixture variable.
 static std::array<unsigned char,64> es{};vft(es.data(),0x4077048);put(image.data(),0x492D8A0,static_cast<void*>(es.data()));
 put(image.data(),0x4077048+0x70,b+np13::kEntityLookup);put(image.data(),0x5487A78,static_cast<void*>(&itemSystem));
 put(image.data(),0x5325B60,std::uint64_t(0));put(image.data(),0x548A788,unsigned(101));put(image.data(),0x548A78C,unsigned(102));
 put(image.data(),0x40AC2A8,static_cast<const char*>("hand_right"));
 put(manager.data(),0x38,static_cast<void*>(player.data()));put(player.data(),0x9D0,static_cast<void*>(manager.data()));
 put(player.data(),0x990,static_cast<void*>(slots.data()));put(player.data(),0x30,unsigned(42));
 put(item.data(),0x30,key);put(item.data(),0x50,unsigned(1));put(item.data(),0x90,static_cast<void*>(&worldContainer));put(item.data(),0x88,static_cast<void*>(link.data()));
 put(link.data(),0x30,unsigned(72));put(link.data(),0x38,static_cast<void*>(entity.data()));put(entity.data(),0xC,unsigned(72));
 put(logic.data(),8,unsigned(42));put(logic.data(),0x28,key);put(logic.data(),0x30,int(0));put(logic.data(),0x38,std::uint8_t(1));
 put(logic.data(),0x40,static_cast<const char*>("slt_0"));float q[4]{0,0,0,1};std::memcpy(logic.data()+0x48,q,16);
 put(controller.data(),kCtrlContext,static_cast<void*>(actionContext.data()));put(actionContext.data(),0,static_cast<void*>(controllerDefinition.data()));
 put(controllerDefinition.data(),0x18,static_cast<void*>(fragmentDefinition.data()));put(fragmentDefinition.data(),8,static_cast<void*>(fragmentEntries.data()+8));
 put(fragmentEntries.data(),4,int(2));put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(unknownPickingDefinition?0x12345678u:0x03E46E25u));
 put(controller.data(),kCtrlScopeContexts,static_cast<void*>(contexts.data()));put(contexts.data(),kScopeContextChar,static_cast<void*>(character.data()));
 put(controller.data(),kCtrlScopeCount,unsigned(1));put(controller.data(),kCtrlScopes,static_cast<void*>(scope.data()));put(controller.data(),kCtrlEntityId,unsigned(42));
 put(scope.data(),kScopeAction,static_cast<void*>(action.data()));put(action.data(),kActRootScope,static_cast<void*>(scope.data()));
 put(action.data(),kActContext,static_cast<void*>(actionContext.data()));put(action.data(),0,b+0x3A450B0);put(action.data(),kActFragId,int(1));put(action.data(),kActStatus,int(2));put(action.data(),kActToken,unsigned(55));put(action.data(),kActSpeedBias,float(1));
 put(image.data(),0x3A657C0+0x60,b+0x1A72860);put(image.data(),0x3A657C0+0xC8,b+0x1AAF3C0);
 const unsigned char relativeGetter55[]{0x48,0x8D,0x81,0x9C,0,0,0,0xC3},jointGetter55[]{0x0F,0xB7,0x41,0x78,0xC3};
 std::memcpy(image.data()+0x1A72860,relativeGetter55,sizeof(relativeGetter55));std::memcpy(image.data()+0x1AAF3C0,jointGetter55,sizeof(jointGetter55));
 put(attachment.data(),0x78,std::int16_t(0));put(attachment.data(),0x9C+12,float(1));
 put(attachment.data(),0x28,static_cast<void*>(attachmentManager.data()));put(attachmentManager.data(),0x70,static_cast<void*>(character.data()));
 float m[12]{1,0,0,30,0,1,0,40,0,0,1,50};std::memcpy(entity.data()+0x58,m,sizeof(m));nativePose={};nativePose.p[0]=-99;
 g_pick_event13=&EventLeaf;g_pick_transfer13=&TransferLeaf;g_pick_cleanup13=&CleanupLeaf;g_pick_end19c4=&EndLeaf19c4;g_notice13=&NoticeLeaf;g_drop_queue13=&QueueLeaf;
 g_attachment_world13=&WorldGetterLeaf;hostCaller=0;flagSource.fill(0);lastAttachFlag=false;testAttachManager=nullptr;
 g_queue_original=nullptr;g_update_original=nullptr;attaches=transfers=notices=drops=cleanup=finishes=eventCalls=0;
 deferAttachment=attachThrows=mutateNotice=mergeOnStow=itemRetired=stowWorldRetired32=false;dropAck=transferAck=dropObserved=true;lines.clear();PublishBinding13();Publish();frame(1000);
 NoteUseSent(0,1000);QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),1000);
 assert(g_owner.state==(unknownPickingDefinition?Idle:Playing));
}
static void poseProof55(){
 ng13::Binding b{};if(!ReadGraspBinding(b)||(!b.attached&&!b.socketOnly28))return;
 ng13::SocketProof28 proof{};proof.binding=b;proof.self=reinterpret_cast<std::uintptr_t>(attachmentObject.data());
 proof.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());proof.sample=hostTickCount64;proof.tick=hostTickCount64;
 proof.joint=Rd<std::int16_t>(attachment.data(),0x78,-1);std::memcpy(proof.relative,attachment.data()+0x9C,sizeof(proof.relative));
 proof.gripReady55=true;proof.grip55.p[0]=.07f;
 // Explicit native-joint leaf: packet and original attachment grip share this
 // actual acquisition/resource and source transaction. No idle-pose learning.
 ng13::Acquisition54 acquisition{};
 if(ReadAcquisition54(acquisition)&&ng13::acquisitionOwns55(b,acquisition)){
  proof.fingers55.acquisition=acquisition;proof.fingers55.sample=proof.sample;proof.fingers55.tick=proof.tick;
  proof.fingers55.count=15;for(auto& q:proof.fingers55.rotations)q={.1f,0,0,std::sqrt(.99f)};
  auto& m=proof.fingers55.mapping;m.skeleton=0x1234;m.generation=1;m.joints=500;m.wrist=408;m.count=15;
  for(unsigned i=0;i<m.count;++i){m.ids[i]=int(i)+1;m.parents[i]=408;}
 }
 PublishGraspSocket28(proof);
}
static void attach(bool withOriginalPose55=true){std::array<unsigned char,40> e{};put(e.data(),0x1c,unsigned(101));PickupEventDetour13(logic.data(),e.data());
 if(withOriginalPose55&&!deferAttachment)poseProof55();}
static void placed(std::uint64_t tick,float x){
 frame(tick);ng13::Placement p{};p.binding=g_pick13.bind;p.self=80;p.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());p.sample=tick;p.tick=tick;
 p.world.p[0]=x;p.world.p[1]=40;p.world.p[2]=50;PublishGraspPlacement(p);TickOwner(controller.data(),tick);
 float m[12]{1,0,0,x,0,1,0,40,0,0,1,50};std::memcpy(entity.data()+0x58,m,sizeof(m));
}
struct PickupProducer19 {
 pickup_use19c3::State pickupUse19c3{};
 door_use46::Controller doorUse46{};
 struct XRHand46 {float grip=0,trigger=0;struct {float pos_x=0,pos_y=0,pos_z=0;}pose;};
 struct XR46 {XRHand46 hands[2];std::uint64_t tick=0;
  const XRHand46& RightController(){return hands[0];}const XRHand46& LeftController(){return hands[1];}
  bool CopyInputSampleStamp59c6(tracking59c6::Stamp& s){s.tick=tick;s.epoch=1;return true;}
 } xr46;
 XR46* xr_manager_=&xr46;
 held_interaction114::State heldUse114{};
 physical_interaction106::Input input{};
 unsigned f5Mode=2,grabMask59c7f=0,reserved114=0,gripMask116=0;
 bool comparison114=true,rayPublished116=true,alchemyPointer59c2=false,native_xi_full=true,f5OwnedRay=false;
 struct Transport { // explicit native transport double; dispatch is the actual Mod .inl
  bool held=false,accept=true;unsigned pulses=0;
  bool SetPhysicalInteractionHeld114(bool value){held=value;return held;}
  std::uint64_t queuedPress=0;
  bool RequestPhysicalInteractionUse(std::uint64_t originalPress=0){++pulses;if(accept)queuedPress=originalPress;return accept;}
  void ValidatePhysicalInteractionPressD48(std::uint64_t originalPress,bool eligible){if(!eligible||queuedPress!=originalPress)queuedPress=0;}
 } touch_bindings_;
 void tick(std::uint64_t ladder_now_ms,float grip){
  hostTickCount64=ladder_now_ms;rawFrame55(ladder_now_ms,grip); // actual source publication precedes Use transport
  input.enabled=true;input.now_ms=ladder_now_ms;input.hand[0]={true,true,true,grip,false};
  const auto held114=held_interaction114::update(heldUse114,input);
  const auto& pi=input;xr46.tick=ladder_now_ms;xr46.hands[0].grip=grip;
  palm59c9::Pair palms59c9{};palms59c9.eligible[0]=true;palms59c9.ray[0].tick=ladder_now_ms;
  palms59c9.ray[0].sourceEpoch=7;palms59c9.ray[0].sourcePlayer=0x1234;
  palms59c9.locked=held114.owner;palms59c9.ray[0].hand=true;
  palms59c9.lockedEntity=palm59c9::ordinaryTransportLock64.observe(palms59c9,held114.owner,heldUse114.pressed,
   interaction_alignment::DoorSourceGeneration38(),1,door_use46::settingsEpoch.load(),
   workstation178r20c1::experimentEpoch36.load(),f5Mode,ladder_now_ms);
  physical_interaction106::Output po{};po.ray_owner=held114.owner;po.ray_active=held114.ray;
  door_hold65::Frame hold65{};auto& hand65=hold65.hand[0];hand65.available=true;hand65.side=0;hand65.grip=grip;
  hand65.tick=hand65.poseTick=ladder_now_ms;hand65.press=held114.owner==0?heldUse114.pressed:0;
  hand65.poseEpoch=7;hand65.trackingEpoch=1;hand65.player=0x1234;hand65.sourceGeneration=interaction_alignment::DoorSourceGeneration38();
  hand65.settingsEpoch=door_use46::settingsEpoch.load();hand65.experimentEpoch=workstation178r20c1::experimentEpoch36.load();
  (void)door_hold65::frame.publish(hold65); // explicit copied upstream ordinary-empty-hand source
  (void)door_surface65::withdraw(0,ladder_now_ms,ladder_now_ms);
  const auto& candidate65=interaction_alignment::testCandidate;
  if(candidate65.entity&&candidate65.owner==0&&!std::strcmp(candidate65.cls,"AnimDoor")&&door_use46::fresh(candidate65.tick,ladder_now_ms)){
    door_surface65::Witness witness65{};witness65.source=door_surface65::request(candidate65.entity,hand65,candidate65.tick,{candidate65.x,candidate65.y,candidate65.z});
    witness65.queryTick=witness65.sample=ladder_now_ms;witness65.point={0,.08f,0};witness65.normal={0,-1,0};
    witness65.entityId=candidate65.entity;witness65.opaqueOwner=0x5678;witness65.opaqueCollider=0x6789;witness65.distance=.08f;witness65.valid=true;
    (void)door_surface65::offer(witness65.source,ladder_now_ms);(void)door_surface65::publish(witness65); // native physical-hit leaf only
  }
  bool heldNow114=false,useSent116=false;
#include "../src/vrik/pickup_use_producer19c3.inl"
 }
};
int scenario(){
 // Actual installed-entry receipt publisher and public equipment validator.
 // Synthetic patch bytes are not executed; the native draw detour still passes
 // an unrelated ordinary call through exactly once, with identical arguments.
 {
  reset();const auto b=WhBase();unsigned char saved[16]{};std::memcpy(saved,image.data()+0x8CEA48,16);
  const unsigned char original[16]{0x48,0x83,0xec,0x28,0x4c,0x8b,0xd9,0x48,0x89,0x54,0x24,0x38,0x48,0x8b,0x0d,0x1d};
  std::memcpy(image.data()+0x8CEA48,original,16);g_supply_draw_entry62.publish({});
  assert(!NativeDrawEntryQualified62(b));g_supply_draw_hook_t2.emplace();
  static unsigned calls62=0;calls62=0;
  g_supply_draw_original_t2=+[](void* m,std::uint64_t k,int side)->bool{++calls62;assert(m==manager.data()&&k==key&&side==2);return true;};
  assert(!PublishSupplyDrawEntry62(b)); // enabled handle without actual changed entry is insufficient
  image[0x8CEA48]=0xE9;assert(PublishSupplyDrawEntry62(b));assert(NativeDrawEntryQualified62(b));
  assert(!NativeDrawEntryQualified62(b+1));
  assert(SupplyDrawDetourT2(manager.data(),key,2)&&calls62==1);
  image[0x8CEA48+9]^=1;assert(!NativeDrawEntryQualified62(b));image[0x8CEA48+9]^=1;
  SupplyDrawEntry62 receipt{};assert(g_supply_draw_entry62.read(receipt));receipt.trampoline=0;g_supply_draw_entry62.publish(receipt);
  assert(!NativeDrawEntryQualified62(b));assert(PublishSupplyDrawEntry62(b));
  g_supply_draw_hook_t2->live=false;assert(!PublishSupplyDrawEntry62(b));
  g_supply_draw_hook_t2.reset();g_supply_draw_original_t2=nullptr;g_supply_draw_entry62.publish({});
  std::memcpy(image.data()+0x8CEA48,saved,16);
 }

 // Actual script-owner supply service, not a duplicate readiness policy.
 // Native/VM leaves are substituted; refresh receipts remain diagnostic only.
 {
  reset();namespace script=kcd2vr::native_script_t2;namespace supply=kcd2vr::supply_t2;
  script::supplyCallbackT5=true;script::supplyCallbackAvailableT5=true;script::supplyMalformedT5=false;
  script::supplyStatusT5="MISSING_ASSETS";script::supplyDaggersT5=script::supplyAxesT5=script::supplyCallsT5=0;
  supply::enabled=true;supply::Input in{};in.manager=reinterpret_cast<std::uintptr_t>(manager.data());
  in.player=reinterpret_cast<std::uintptr_t>(player.data());in.character=reinterpret_cast<std::uintptr_t>(character.data());
  in.entity=42;in.body=17;in.request=18;in.allowed=in.tracked=in.bone=true;
  const auto refresh=[&](std::uint64_t now,std::uint64_t generation=600){
   hostTickCount64=now;in.tick=in.sample=now;g_supply_input_t2.publish(in);
   script::Context ctx{};ctx.system=1;ctx.state=2;ctx.now=now;ctx.generation=generation;SupplyScriptServiceT2(ctx);
  };
  const auto readiness=[](){std::size_t n=0;for(const auto& line:lines)if(line.find("T5 SUPPLY_STATUS stage=READINESS")!=std::string::npos)++n;return n;};
  refresh(2000);assert(script::supplyCallsT5==1&&readiness()==1);
  assert(lines.back().find("current=1 actor=42")!=std::string::npos&&lines.back().find("body=17 request=18 vm=600 status=MISSING_ASSETS daggers=0 axes=0")!=std::string::npos);
  for(std::uint64_t now=2010;now<3000;now+=10)refresh(now);
  assert(script::supplyCallsT5==1&&readiness()==1); // no per-frame call or log
  refresh(3000);assert(script::supplyCallsT5==2&&readiness()==1); // serial is not a state change
  script::supplyStatusT5="READY";script::supplyDaggersT5=5;script::supplyAxesT5=3;
  refresh(4000);assert(readiness()==2&&lines.back().find("status=READY daggers=5 axes=3")!=std::string::npos);
  script::supplyDaggersT5=4;refresh(5000);assert(readiness()==3&&lines.back().find("daggers=4 axes=3")!=std::string::npos);
  in.body=19;refresh(6000);assert(readiness()==4&&lines.back().find("body=19")!=std::string::npos);
  refresh(6010,601);assert(readiness()==4); // VM edge cannot spam inside1s
  refresh(7010,601);assert(readiness()==5&&lines.back().find("vm=601")!=std::string::npos);
  script::supplyCallbackAvailableT5=false;refresh(8010,601);assert(readiness()==6&&lines.back().find("status=CALLBACK_UNAVAILABLE daggers=-1 axes=-1 callback=0")!=std::string::npos);
  script::supplyCallbackAvailableT5=true;script::supplyMalformedT5=true;refresh(9010,601);
  assert(readiness()==7&&lines.back().find("status=BAD_RECEIPT daggers=-1 axes=-1")!=std::string::npos);
  script::supplyMalformedT5=false;script::supplyDaggersT5=6;refresh(10010,601);
  assert(readiness()==7); // impossible cap remains the same malformed receipt
  script::supplyDaggersT5=0;script::supplyAxesT5=0;script::supplyStatusT5="EMPTY_STOCK";refresh(11010,601);
  assert(readiness()==8&&lines.back().find("status=EMPTY_STOCK")!=std::string::npos);
  in.allowed=false;script::supplyStatusT5="CLEARED";refresh(12010,601);
  assert(readiness()==9&&lines.back().find("current=0 actor=0 player=0 body=0 request=0")!=std::string::npos);
  assert(attaches==0&&transfers==0&&drops==0&&notices==0); // no inventory/native hand mutation
  const auto called=script::supplyCallsT5;refresh(13010,601);assert(script::supplyCallsT5==called&&readiness()==9);
  SupplyReadinessReceiptT5 receipt{};const char extra[]="READY|1|0|0|extra",overflow[]="READY|18446744073709551616|0|0";
  assert(!SupplyReadinessParseT5(extra,sizeof(extra),1,receipt));
  assert(!SupplyReadinessParseT5(overflow,sizeof(overflow),1,receipt));
  // T13: the actual script-owner service retires an uncertain draw only for
  // this actual unit/actor/body transfer, independently of spin readiness.
  in.allowed=true;in.body=21;in.request=22;in.grip=0;in.pouchNear[1]=true;
  in.offset[1][0]=0;
  put(slots.data(),0x260,std::uint64_t(0));refresh(14000,602);
  script::supplyDrawT13=true;in.grip=1;refresh(14020,602);
  // T15 stock holsters deliberately need an outward12cm/80ms pull. Merely
  // squeezing (the retired T13 fixture setup) must not request native draw.
  assert(script::supplyPacketT13.find("|draw|")==std::string::npos);
  in.offset[1][0]=.14f;refresh(14099,602);
  assert(script::supplyPacketT13.find("|draw|")==std::string::npos); //79ms still cannot draw
  refresh(14100,602);assert(script::supplyPacketT13.find("|draw|1|")!=std::string::npos);
  refresh(14110,602);assert(supply::nativeLease.load());
  script::supplyDrawT13=false;refresh(15700,602);assert(supply::nativeLease.load());
  assert(std::any_of(lines.begin(),lines.end(),[](const std::string& l){return l.find("T2 SUPPLY stage=UNCERTAIN")!=std::string::npos;}));
  DropTransferT13 wrongTransferT13{};wrongTransferT13.source.key=0x0200000000019a5bull;
  wrongTransferT13.source.manager=in.manager;wrongTransferT13.source.player=in.player;
  wrongTransferT13.source.character=in.character;wrongTransferT13.source.bodyEpoch=99;
  wrongTransferT13.serial=g_drop_transfer_serial_t13.fetch_add(1)+1;
  g_drop_transfer_t13[0].publish(wrongTransferT13);refresh(15710,602);assert(supply::nativeLease.load());
  wrongTransferT13.source.bodyEpoch=in.body;wrongTransferT13.serial=g_drop_transfer_serial_t13.fetch_add(1)+1;
  g_drop_transfer_t13[0].publish(wrongTransferT13);refresh(15720,602);
  assert(!supply::nativeLease.load()&&script::supplyPacketT13.find("|release|")!=std::string::npos);
  in.offset[1][0]=0;in.grip=0;refresh(15740,602);script::supplyDrawT13=true;in.grip=1;refresh(15760,602);
  assert(script::supplyPacketT13.find("|draw|")==std::string::npos);
  in.offset[1][0]=.14f;refresh(15839,602);assert(script::supplyPacketT13.find("|draw|")==std::string::npos);
  refresh(15840,602);assert(script::supplyPacketT13.find("|draw|1|")!=std::string::npos);
  refresh(15850,602);assert(supply::nativeLease.load());
  script::supplyDrawT13=false;
  script::supplyCallbackT5=false;supply::enabled=false;g_supply_input_t2.publish({});g_supply_cleanup_t2=false;
  std::puts("PASS T5 actual supply script-owner refresh: honest state/count/identity/VM receipts, bounded edge logging, malformed reply/refused cleanup and no native inventory mutation");
 }

 // Actual production controller/Use publication/installed-root chain. A native
 // Picking root is first observed before Attach even when the native Queue was
 // on another thread or did not traverse the observed Queue entry at all.
 const auto beginUse55=[](){
  reset();ResetOwner();g_consumed_use13=0;
  put(action.data(),0,WhBase()+0x3B13F38); // native locomotion IAction subclass
  put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
  put(action.data(),kActFragId,int(0));frame(980);UpdateDetour(controller.data(),.02f);
  frame(1000);NoteUseSent(0,1000);
  ng13::PendingUse55 pending{};assert(ReadPendingUse55(pending)&&pending.requestedAt==1000&&pending.epoch==9);
  ng13::Acquisition54 acquiring{};assert(!ReadAcquisition54(acquiring));
  assert(attaches==0&&transfers==0&&finishes==0&&g_owner.state==Idle);
  return pending;
 };
 const auto installPicking55=[](){
  put(action.data(),0,WhBase()+0x3A450B0);put(action.data(),kActFragId,int(1));
  put(action.data(),kActToken,unsigned(56));frame(1020);
 };
 // H23: a retiring/queued locomotion root is a valid read-only BEFORE
 // witness, but never authorizes active Picking, speed or inventory changes.
 // Exercise the actual UpdateDetour -> NoteUseSent -> installed-root route.
 for(int status:{0,1,3}){
  beginUse55();put(action.data(),kActStatus,status);frame(1001);
  UpdateDetour(controller.data(),.02f);NoteUseSent(0,1001);
  ng13::PendingUse55 pending{};assert(ReadPendingUse55(pending));
  assert(g_owner.state==Idle&&attaches==0&&transfers==0&&finishes==0);
  CurrentRoot57 observation{};
  assert(!ReadCurrentRootIdentity57(controller.data(),true,observation));
  installPicking55();put(action.data(),kActStatus,int(2));
  UpdateDetour(controller.data(),.02f);
  ng13::Acquisition54 active{};assert(ReadAcquisition54(active)&&active.active&&g_owner.state==Playing);
  assert(attaches==0&&transfers==0&&finishes==0&&Rd<float>(action.data(),kActSpeedBias,0)==1.f);
  attach();assert(attaches==1&&transfers==0);frame(1040,0);TickOwner(controller.data(),1040);
  assert(drops==1&&transfers==0&&g_owner.state==Idle);
 }
 for(int status:{-1,4,255}){
  beginUse55();put(action.data(),kActStatus,status);frame(1001);
  UpdateDetour(controller.data(),.02f);NoteUseSent(0,1001);
  ng13::PendingUse55 pending{};assert(!ReadPendingUse55(pending));
  installPicking55();put(action.data(),kActStatus,int(2));UpdateDetour(controller.data(),.02f);
  assert(g_owner.state==Idle&&attaches==0&&transfers==0&&finishes==0);
 }
 std::puts("PASS H23 pre-Use native transition observation; only a new Playing Picking root grants early visual ownership, real Attach remains original");
 // H23 exact clip setter through production UpdateDetour; native leaves are
 // substituted. The action clock stays 1x, and every replacement sees native
 // layer speed before original Queue/Update executes.
 const auto prepareClockH23=[&](){
  beginUse55();installPicking55();put(action.data(),kActStatus,int(2));
  put(testSkel,0,WhBase()+kSkelVft);put(image.data(),kSkelVft+0xB8,WhBase()+kLayerScaleH23);
  std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
  put(scope.data(),kScopeNumLayers,unsigned(3));put(scope.data(),kScopeSpeedBias,1.f);put(testSkel,0x50,1.f);
  g_update_original=&UpdateActionLeaf;UpdateDetour(controller.data(),.02f);
  assert(g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==8.f);
  assert(Rd<float>(action.data(),kActSpeedBias,0)==1.f&&Rd<float>(action.data(),kActActiveTime,0)==.02f);
 };
 prepareClockH23();
 // Explicit native clip-consumer double: layer scale advances clip events;
 // IAction Update still receives only ordinary dt. The original Attach event
 // path executes once on crossing, with no production event synthesized.
 float clipH23=0;unsigned clipFramesH23=0;
 while(attaches==0&&clipFramesH23<60){
  frame(1040+20*clipFramesH23);UpdateDetour(controller.data(),.02f);
  clipH23+=.02f*Rd<float>(testSkel,0x50,0);++clipFramesH23;
  if(clipH23>=.5f)attach();
 }
 assert(attaches==1&&transfers==0&&clipFramesH23==4&&Rd<float>(action.data(),kActActiveTime,0)<.11f);
 frame(1140,0);TickOwner(controller.data(),1140);assert(drops==1&&g_owner.state==Idle);
 prepareClockH23();
 g_update_original=+[](void*,float){assert(Rd<float>(testSkel,0x50,0)==1.f);UpdateActionLeaf(controller.data(),.02f);};
 frame(1040);UpdateDetour(controller.data(),.02f);assert(g_pickup_clock_h23.active);
 attach();assert(!g_pickup_clock_h23.active&&attaches==1&&transfers==0&&Rd<float>(testSkel,0x50,-1)==1.f);
 frame(1060,0);TickOwner(controller.data(),1060);assert(drops==1&&g_owner.state==Idle);
 prepareClockH23();
 g_queue_original=+[](void*,void*,float){assert(!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);};
 QueueDetour(controller.data(),action.data(),0);assert(!g_pickup_clock_h23.active);
 prepareClockH23();put(action.data(),0,WhBase()+0x3B13F38);put(action.data(),kActFragId,int(0));
 g_update_original=+[](void*,float){assert(!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);};
 frame(1040);UpdateDetour(controller.data(),.02f);assert(!g_pickup_clock_h23.active); // changed root inherits no scale
 prepareClockH23();put(testSkel,0x50,.75f);assert(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,0)==.75f);
 prepareClockH23();put(scope.data(),kScopeSpeedBias,8.f);assert(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,0)==8.f);
 prepareClockH23();put(scope.data(),kScopeSpeedBias,.5f);assert(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,0)==.5f);
 // ACTIVE receipt failures: unknown evidence retains a retry; controller/
 // scope observer loss restores captured1 under exact current skeleton proof.
 prepareClockH23();put(image.data(),kLayerScaleH23,std::uint8_t(0));
 assert(!RestorePickupClipH23()&&g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==8.f);
 std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
 assert(RestorePickupClipH23()&&!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();void* liveSkelH23=testSkel;testSkel=nullptr;
 assert(!RestorePickupClipH23()&&g_pickup_clock_h23.active);testSkel=liveSkelH23;
 assert(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();g_henry_ctrl=nullptr;
 assert(RestorePickupClipH23()&&!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();g_henry_ctrl=nullptr;
 g_update_original=+[](void*,float){assert(!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);};
 frame(1040);UpdateDetour(controller.data(),.02f);assert(RestorePickupClipH23());
 prepareClockH23();SetMode(Off);assert(!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();SetContextAvailable(false);assert(!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();g_owner_thread13=99;Uninstall();
 assert(g_pickup_clock_live_h23.load()&&g_mode==Off);g_owner_thread13=1;
 assert(RestorePickupClipH23()&&!g_pickup_clock_live_h23.load());
 prepareClockH23();put(controller.data(),kCtrlScopes,static_cast<void*>(nullptr));
 assert(RestorePickupClipH23()&&!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 prepareClockH23();put(scope.data(),kScopeSpeedBias,std::numeric_limits<float>::quiet_NaN());
 assert(RestorePickupClipH23()&&!g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,0)==1.f);
 // Busy native owner-state transaction never prevents clip restoration before
 // original Update. Lease storage is exclusively mutated by this owner thread.
 prepareClockH23();g_owner_transaction19c4.test_and_set();
 g_update_original=+[](void*,float){assert(Rd<float>(testSkel,0x50,0)==1.f);};
 frame(1040);UpdateDetour(controller.data(),.02f);g_owner_transaction19c4.clear();
 assert(!g_pickup_clock_h23.active);
 for(unsigned bad=0;bad<9;++bad){
  prepareClockH23();assert(RestorePickupClipH23());
  if(bad==0)put(scope.data(),kScopeNumLayers,unsigned(0));
  if(bad==1)put(action.data(),kActSpeedBias,.5f);
  if(bad==2)put(scope.data(),kScopeSpeedBias,.5f);
  if(bad==3)put(testSkel,0x50,.5f);
  if(bad==4)put(image.data(),kLayerScaleH23,std::uint8_t(0));
  if(bad==5)put(image.data(),kSkelVft+0xB8,WhBase()+kLayerScaleH23+1);
  if(bad==6)frame(1040,0);
  if(bad==7)numAnimationsH23=2;
  if(bad==8)put(action.data(),kActStatus,int(3));
  TickPickupClipH23(GetTickCount64());assert(!g_pickup_clock_h23.active);
 }
 std::puts("PASS H23 actual clip-only speed lease: action1x, Update/Queue entry restores, actual Attach restores, changed root inherits no8x, independent/native writers win");

 for(bool foreignQueue:{false,true}){
  const auto pending=beginUse55();installPicking55();
  if(foreignQueue){pickupThread19c4=19728;QueueDetour(controller.data(),action.data(),0);pickupThread19c4=1;}
  assert(g_owner.state==Idle);UpdateDetour(controller.data(),.02f);
  ng13::Acquisition54 acquiring{};
  assert(ReadAcquisition54(acquiring)&&acquiring.active&&acquiring.serial==pending.serial&&
      acquiring.startedAt==pending.requestedAt&&acquiring.action==reinterpret_cast<std::uintptr_t>(action.data()));
  assert(!acquiring.graspToken&&!acquiring.graspKey&&!acquiring.graspItem&&!acquiring.graspEntity);
  assert(attaches==0&&transfers==0&&finishes==0&&Rd<float>(action.data(),kActSpeedBias,0)==1);
  // The genuine native Attach alone creates the binding and stronger copied
  // held-item presentation join. Early body permission never fabricates it.
  attach();ng13::Binding real{};assert(ReadGraspBinding(real)&&ReadAcquisition54(acquiring));
  assert(acquiring.graspToken==real.token&&acquiring.graspKey==real.key&&
      acquiring.graspItem==real.item&&acquiring.graspEntity==real.entity&&attaches==1&&transfers==0);
  // Model the observed native Finish tail: original request returned while the
  // SAME root is still installed. Only actual cleanup, not the request alone,
  // revokes visual placement; cleanup/outer-completion quarantine is unchanged.
  finishLeavesRoot55=true;frame(1040);TickOwner(controller.data(),1040);
  assert(finishes==1&&g_pick13.finishing&&ReadGraspBinding(real)&&ReadAcquisition54(acquiring)&&acquiring.active);
  PickupEndDetour19c4(logic.data(),0,0);
  assert(!ReadGraspBinding(real)&&!ReadAcquisition54(acquiring));
  frame(1060);TickOwner(controller.data(),1060);
  assert(ReadGraspBinding(real)&&real.logic==0&&!ReadAcquisition54(acquiring)&&cleanup==1&&transfers==0);
  frame(1080,0);TickOwner(controller.data(),1080);assert(drops==1&&transfers==0&&g_owner.state==Idle);
 }
 // Every guard uses the real publication and callback path; no fake permit.
 for(unsigned refusal=0;refusal<12;++refusal){
  beginUse55();installPicking55();
  switch(refusal){
   case 0:put(action.data(),kActToken,unsigned(55));break; // unchanged native generation
   case 1:put(action.data(),0,WhBase()+0x3B13F38);break; // wrong Picking class
   case 2:put(action.data(),kActFragId,int(0));break; // another native fragment
   case 3:put(action.data(),kActRootScope,static_cast<void*>(nullptr));break;
   case 4:put(action.data(),kActStatus,int(1));break;
   case 5:put(action.data(),kActContext,static_cast<void*>(nullptr));break;
   case 6:frame(1020,0);break;
   case 7:hostTickCount64=1101;break; // genuine stale hand 81ms
   case 8:{ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);++input.epoch;PublishAcquisitionInput55(input);break;}
   case 9:g_context13=false;break;
   case 10:g_mode=Observe;break;
   case 11:g_native_ready13=false;break;
  }
  UpdateDetour(controller.data(),.02f);ng13::Acquisition54 acquiring{};
  assert(g_owner.state==Idle&&!ReadAcquisition54(acquiring)&&attaches==0&&transfers==0&&finishes==0);
 }
 // Missing, future or stale pre-Use root cannot arm this early route. An
 // already-playing Picking action cannot borrow a subsequent squeeze either.
 for(unsigned refusal=0;refusal<4;++refusal){
  beginUse55();RootObservation55 before{};g_root_before_use55.read(before);
  if(refusal==0)before={};else if(refusal==1)before.tick=1001;else if(refusal==2)before.tick=919;else before.picking=true;
  g_root_before_use55.publish(before);NoteUseSent(0,1000);ng13::PendingUse55 pending{};assert(!ReadPendingUse55(pending));
  installPicking55();UpdateDetour(controller.data(),.02f);assert(g_owner.state==Idle&&attaches==0&&transfers==0);
 }
 beginUse55();installPicking55();UpdateDetour(controller.data(),.02f);assert(g_owner.state==Playing);
 ResetOwner();put(action.data(),kActToken,unsigned(57));UpdateDetour(controller.data(),.02f);
 assert(g_owner.state==Idle&&attaches==0&&transfers==0); // consumed Use is never replayed
 for(unsigned retirement=0;retirement<8;++retirement){
  const auto original=beginUse55();
  if(retirement==0){SetMode(Off);SetMode(Own);}
  else if(retirement==1){SetContextAvailable(false);SetContextAvailable(true);}
  else if(retirement==2){frame(1001,0);frame(1002,1);}
  else {ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);input.tick=1001;
   if(retirement==3)input.tracked[0]=false;
   else if(retirement==4)++input.epoch;
   else if(retirement==5)++input.trackingEpoch;
   else if(retirement==6)input.headTracked=false;
   else ++input.character;
   PublishAcquisitionInput55(input);frame(1002,1);}
  ng13::PendingUse55 pending{};assert(!ReadPendingUse55(pending));
  installPicking55();UpdateDetour(controller.data(),.02f);assert(g_owner.state==Idle&&attaches==0&&transfers==0);
  Use13 stillOriginal{};g_use13.read(stillOriginal);assert(stillOriginal.serial==original.serial&&!UseLive55(stillOriginal));
  // The old Attach fallback cannot lend this retired request a new owner.
  attach();assert(g_owner.state==Idle&&!g_pick13.attachStarted&&transfers==1);
 }
 // Raw source inputs continue before native Picking is allowed to commit a
 // body pose. A stale/gated completed HandFrame cannot deadlock acquisition or
 // falsely retire a real held Grip; it still cannot grant chest/held-item use.
 for(bool gatedBody:{false,true}){
  const auto pending=beginUse55();installPicking55();
  ng13::HandFrame body{};g_hands13.read(body);body.tick=1000;body.sample=1000;
  body.tracked[0]=!gatedBody;PublishHands(body);
  for(const auto tick:{1040u,1080u,1120u,1140u})rawFrame55(tick);
  ng13::PendingUse55 current{};assert(ReadPendingUse55(current)&&current.serial==pending.serial);
  UpdateDetour(controller.data(),.02f);ng13::Acquisition54 acquired{};
  assert(ReadAcquisition54(acquired)&&acquired.active&&acquired.serial==pending.serial&&
      acquired.generation55&&acquired.trackingEpoch55==19&&attaches==0&&transfers==0);
  ng13::Binding absent{};assert(!ReadGraspBinding(absent));
  // Only a later completed actual body frame and the real native Attach make
  // the independently retained placement/terminal HandFrame path available.
  frame(1160);attach();ng13::Binding held{};
  assert(ReadGraspBinding(held)&&attaches==1&&transfers==0);
 }
 // An already-published active acquisition must not resurrect for the reader
 // before the next controller publication after OFF/ON, context or raw loss.
 for(unsigned retirement=0;retirement<8;++retirement){
  beginUse55();installPicking55();UpdateDetour(controller.data(),.02f);
  ng13::Acquisition54 old{};assert(ReadAcquisition54(old)&&old.active);
  if(retirement==0){SetMode(Off);SetMode(Own);}
  else if(retirement==1){SetContextAvailable(false);SetContextAvailable(true);}
  else if(retirement==2){rawFrame55(1021,0);rawFrame55(1022,1);}
  else {ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);input.tick=1021;
   if(retirement==3)input.tracked[0]=false;
   else if(retirement==4)++input.epoch;
   else if(retirement==5)++input.trackingEpoch;
   else if(retirement==6)input.headTracked=false;
   else ++input.character;
   PublishAcquisitionInput55(input);rawFrame55(1022,1);}
  ng13::Acquisition54 refused{};assert(!ReadAcquisition54(refused));
  // Native Attach can arrive before another controller tick; it also cannot
  // adopt this retired pre-Attach lease, even after a fresh completed regrip.
  frame(1024);attach();assert(!g_pick13.attachStarted&&transfers==1);
 }
 // Physical source time and transport consumption time differ. A delayed
 // newer release source can predate Use's transport timestamp and must still
 // retire the original squeeze; a later regrip cannot revive the old serial.
 for(bool alreadyInstalled:{false,true}){
  beginUse55();hostTickCount64=1010;NoteUseSent(0,1010); // original raw source1000
  ng13::PendingUse55 pending{};assert(ReadPendingUse55(pending)&&pending.requestedAt==1010);
  if(alreadyInstalled){installPicking55();UpdateDetour(controller.data(),.02f);
   ng13::Acquisition54 active{};assert(ReadAcquisition54(active)&&active.active);}
  ng13::AcquisitionInput55 release{};g_acquisition_input55.read(release);
  release.tick=1005;release.sample=1005;release.grip[0]=0;PublishAcquisitionInput55(release);
  rawFrame55(1022,1);assert(!ReadPendingUse55(pending));
  ng13::Acquisition54 refused{};assert(!ReadAcquisition54(refused));
  if(!alreadyInstalled){installPicking55();UpdateDetour(controller.data(),.02f);assert(g_owner.state==Idle);}
  frame(1024);attach();assert(!g_pick13.attachStarted&&transfers==1);
 }
 std::puts("PASS C55 actual pre-Use root -> native installed Picking, no Queue/foreign Queue, immutable request baseline, exact Attach-only item proof, Finish-tail/cleanup quarantine, freshness/context/identity/no-replay guards");
 // Real action-owner publication: queued can save a baseline, but only an
 // installed exact root action grants active pickup presentation authority.
 reset();
 {ng13::Acquisition54 p{};assert(ReadAcquisition54(p)&&p.active&&p.side==0&&p.serial&&p.epoch==9);
  const auto original=p;frame(1081);hostTickCount64=1081;assert(!ReadAcquisition54(p)); // publication does not self-renew
  TickOwner(controller.data(),1081);assert(ReadAcquisition54(p)&&p.active&&p.serial==original.serial);
  frame(1090,0);TickOwner(controller.data(),1090);assert(!ReadAcquisition54(p));
 }
 reset();
 {ng13::Acquisition54 p{};g_owner.state=Armed;g_owner.scope=nullptr;Publish();
  assert(ReadAcquisition54(p)&&!p.active&&!p.scope);const auto serial=p.serial;
  TickOwner(controller.data(),1000);assert(ReadAcquisition54(p)&&p.active&&p.scope&&p.serial==serial);
  g_context13=false;assert(!ReadAcquisition54(p));Publish();assert(!ReadAcquisition54(p));g_context13=true;
  g_owner.action=nullptr;Publish();assert(!ReadAcquisition54(p));
 }
 reset();
 {ng13::Acquisition54 p{};put(scope.data(),kScopeAction,static_cast<void*>(nullptr));Publish();assert(!ReadAcquisition54(p));
  put(scope.data(),kScopeAction,static_cast<void*>(action.data()));put(action.data(),kActToken,unsigned(56));Publish();assert(!ReadAcquisition54(p));
 }
 std::puts("PASS C54 exact native Picking acquisition witness: queued/installed distinction, immutable Use serial, stale/current identity and release/context retirement");
 // Canonical native fingers survive the REAL production native Finish/cleanup,
 // including retiring its unique logic pointer. A changed item never inherits it.
 reset();attach();
 {ng13::Binding held{};ng13::Acquisition54 acquisition{};assert(ReadGraspBinding(held)&&ReadAcquisition54(acquisition));
  ng13::FingerRotations54 original{};for(auto& q:original)q={.1f,0,0,std::sqrt(.99f)};
  ng13::NativeFinger54 fingers;assert(fingers.capture(held,acquisition,1000,original,15));
  frame(1020);TickOwner(controller.data(),1020); // actual FinishLeaf -> native End/cleanup -> retained world item
  ng13::Binding after{};assert(ReadGraspBinding(after)&&after.logic==0&&held.logic!=0&&!ReadAcquisition54(acquisition));
  ng13::NativeFinger54 canonical;assert(canonical.captureCanonical55(after,15)&&
      after.fingers55.sample==1000&&std::fabs(canonical.rotations[0][0]-.1f)<1e-6f);
  assert(fingers.current(after,15)&&std::fabs(fingers.rotations[0][0]-.1f)<1e-6f);
  auto changed=after;++changed.key;assert(!fingers.current(changed,15)&&!fingers.ready);
  assert(!fingers.capture(after,acquisition,1020,original,15)); // no learning idle fingers after native Finish
  frame(1040,0);TickOwner(controller.data(),1040);ng13::Binding dropped{};assert(!ReadGraspBinding(dropped));
  assert(!fingers.current(dropped,15));
 }
 std::puts("PASS C54 native finger pose retained after real Picking finish, cleared on changed resource/drop; no idle-pose relearn");
 // Actual input publication API: both tap and held transport start one native
 // request. A sustained hold cannot repeatedly renew its identity/deadline.
 NoteUseInput(false,false,-1,1);Use13 u0{},u1{};g_use13.read(u0);
 NoteUseInput(false,true,0,100);g_use13.read(u1);assert(u1.serial!=u0.serial&&u1.side==0&&u1.tick==100);
 const auto first=u1.serial;for(unsigned t=101;t<500;++t)NoteUseInput(false,true,0,t);
 g_use13.read(u1);assert(u1.serial==first&&u1.tick==100);
 NoteUseInput(false,false,-1,501);NoteUseInput(false,true,1,502);g_use13.read(u1);assert(u1.serial!=first&&u1.side==1&&u1.tick==502);
 NoteUseInput(false,false,-1,503);const auto held=u1.serial;NoteUseInput(true,false,0,504);g_use13.read(u1);assert(u1.serial!=held&&u1.side==0);
 const auto pulse=u1.serial;NoteUseInput(false,true,0,400);g_use13.read(u1);assert(u1.serial==pulse); // reversed source time cannot arm

 // Actual Mod dispatch + actual native owner: release the native short Use
 // while the physical Grip stays down through the delayed native Attach.
 const bool savedReachD40=pickup_use19c3::nearOnlyD40.load();pickup_use19c3::setNearOnlyD40(false);
 unknownPickingDefinition=true;reset();NoteUseInput(false,false,-1,900);
 auto candidate=[](std::uint64_t t,const char* cls="PickableItem",float d=.15f,int side=0){
  auto& c=interaction_alignment::testCandidate;c={};c.tick=t;c.entity=72;c.owner=side;c.object=true;c.distance=d;c.sourceTick=t;c.sourceGeneration=1;c.sourceEpoch=7;c.sourcePlayer=0x1234;std::snprintf(c.cls,sizeof(c.cls),"%s",cls);
 };
 PickupProducer19 producer;candidate(1000);producer.tick(1000,0);producer.tick(1020,1);
 for(unsigned t=1040;t<=1180;t+=20){candidate(t);producer.tick(t,1);if(t<1180)assert(producer.touch_bindings_.pulses==0&&!producer.touch_bindings_.held);}
 assert(producer.touch_bindings_.pulses==1&&!producer.touch_bindings_.held);
 Use13 startUse{};g_use13.read(startUse);assert(startUse.tick==1180&&startUse.side==0&&startUse.physicalPickup63);
 for(unsigned t=1200;t<=2000;t+=20){candidate(t);if(t>1200)interaction_alignment::testCandidate.entity=73;producer.tick(t,1);frame(t,1);}
 Use13 endUse{};g_use13.read(endUse);assert(endUse.serial==startUse.serial&&endUse.tick==1180&&producer.touch_bindings_.pulses==1);
 // Model the genuine current Picking definition becoming available before
 // Attach; the event must not invent Picking from an unknown definition.
 put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(0x03E46E25u));
 attach();assert(g_owner.state==Held&&attaches==1&&transfers==0); // native eligibility/identity, not the pulse, admits the hold
 producer.tick(2020,0);frame(2020,0);TickOwner(controller.data(),2020);assert(drops==1&&producer.touch_bindings_.pulses==1);
 candidate(2040);producer.tick(2040,1);for(unsigned t=2060;t<=2200;t+=20){candidate(t);producer.tick(t,1);}assert(producer.touch_bindings_.pulses==2);
 // Qualified contextual non-Picking actions retain native held Use. Unknown,
 // stale, wrong-hand or too-far Picking selection waits without transporting
 // Use, with doors either enabled or disabled; a later fresh target can qualify
 // during the same physical squeeze without inheriting the earlier hover time.
 unsigned doorProducerChecks46=0;
 for(unsigned doorMode46=0;doorMode46<2;++doorMode46){
  const bool previousDoors46=door_use46::enabled.load();door_use46::SetEnabled(doorMode46!=0);
  for(unsigned reason=0;reason<9;++reason){
   reset();PickupProducer19 p;candidate(1000);
   if(reason==0)p.f5Mode=1;if(reason==1)candidate(1000,"Furniture");
   if(reason==2)candidate(1000,"AlchemyTool");if(reason==3)candidate(1000,"");
   if(reason==4)candidate(900);if(reason==5)candidate(1000,"PickableItem",.26f);
   if(reason==6)candidate(1000,"PickableItem",.15f,1);
   if(reason==7)p.alchemyPointer59c2=true;if(reason==8)interaction_alignment::testCandidate.entity=0;
   p.tick(1000,0);Use13 before46{};g_use13.read(before46);
   p.tick(1020,1);p.tick(1040,1);
   if(doorMode46&&(reason==4||reason==6||reason==8)){
    Use13 deferred46{};g_use13.read(deferred46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==0&&!p.touch_bindings_.held&&p.doorUse46.deferred&&deferred46.serial==before46.serial);
    candidate(1060,"AnimDoor",1.4f);p.tick(1060,1);
    Use13 resumed46{};g_use13.read(resumed46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held&&resumed46.serial!=before46.serial&&resumed46.tick==1060);
    for(unsigned t=1080;t<=1200;t+=20){candidate(t,"AnimDoor",1.4f);p.tick(t,1);}
    Use13 final46{};g_use13.read(final46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held&&final46.serial==resumed46.serial&&final46.tick==1060);
   }else if(reason==3||reason==5||(!doorMode46&&(reason==4||reason==6||reason==8))){
    Use13 waiting46{};g_use13.read(waiting46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==0&&!p.touch_bindings_.held&&p.pickupUse19c3.waitingH12&&waiting46.serial==before46.serial);
    candidate(1060);p.tick(1060,1);
    for(unsigned t=1080;t<=1160;t+=20){candidate(t);p.tick(t,1);
     assert(p.touch_bindings_.pulses==0&&!p.touch_bindings_.held);}
    candidate(1180);p.tick(1180,1);Use13 picked46{};g_use13.read(picked46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held&&picked46.serial!=before46.serial&&picked46.tick==1180&&picked46.physicalPickup63);
    candidate(1200);interaction_alignment::testCandidate.entity=73;p.tick(1200,1);
    Use13 frozen46{};g_use13.read(frozen46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held&&frozen46.serial==picked46.serial&&p.pickupUse19c3.target63==72);
    p.tick(1220,0);candidate(1240);p.tick(1240,1);
    for(unsigned t=1260;t<=1400;t+=20){candidate(t);p.tick(t,1);}
    Use13 regripped46{};g_use13.read(regripped46);++doorProducerChecks46;
    assert(p.touch_bindings_.pulses==2&&!p.touch_bindings_.held&&regripped46.serial!=picked46.serial&&regripped46.tick==1400);
   }else{
    assert(p.touch_bindings_.pulses==0&&p.touch_bindings_.held);
    if(doorMode46)++doorProducerChecks46;
   }
  }
  door_use46::SetEnabled(previousDoors46);
 }
 {
  const bool previousDoors46=door_use46::enabled.load();door_use46::SetEnabled(true);
  for(unsigned missing46=0;missing46<3;++missing46){
   reset();PickupProducer19 p;candidate(missing46==0?900:1000,"PickableItem",.15f,missing46==1?1:0);
   if(missing46==2)interaction_alignment::testCandidate.entity=0;
   p.tick(1000,0);Use13 before46{};g_use13.read(before46);p.tick(1020,1);
   for(unsigned t=1040;t<=1300;t+=20)p.tick(t,1);
   Use13 expired46{};g_use13.read(expired46);++doorProducerChecks46;
   assert(p.touch_bindings_.pulses==0&&!p.touch_bindings_.held&&p.doorUse46.deferred&&!p.doorUse46.cancelled&&expired46.serial==before46.serial);
   candidate(1320,"AnimDoor",1.4f);p.tick(1320,1);++doorProducerChecks46;
   assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held); // C65 first eligible near surface can arrive after250ms
   p.tick(1340,0);candidate(1360,"AnimDoor",1.4f);p.tick(1360,1);candidate(1380,"AnimDoor",1.4f);p.tick(1380,1);
   Use13 renewed46{};g_use13.read(renewed46);++doorProducerChecks46;
   assert(p.touch_bindings_.pulses==2&&!p.touch_bindings_.held&&renewed46.serial!=before46.serial&&renewed46.tick==1380);
  }
  door_use46::SetEnabled(previousDoors46);
 }
 std::printf("C46/H12 native-grasp producer: qualified contextual Held preserved; %u current selection/wait/recovery/timeout checks PASS\n",doorProducerChecks46);
 // C19C4 normal AnimDoor is also a native short-use route. Holding Grip
 // retains that hand intent without generating repeated Use or arming Picking.
 reset();ResetOwner();NoteUseInput(false,false,-1,900);PickupProducer19 doorProducer;
 candidate(1000,"AnimDoor");doorProducer.tick(1000,0);doorProducer.tick(1020,1);
 candidate(1040,"AnimDoor");doorProducer.tick(1040,1);
 for(unsigned t=1060;t<=2000;t+=20){candidate(t,"AnimDoor");doorProducer.tick(t,1);}
 assert(doorProducer.touch_bindings_.pulses==1&&!doorProducer.touch_bindings_.held&&g_owner.state==Idle);
 doorProducer.tick(2020,0);assert(doorProducer.touch_bindings_.pulses==1&&g_owner.state==Idle);
 // Context loss releases the transport and cannot create a delayed new Use
 // when the same physical squeeze remains held.
 reset();PickupProducer19 p;candidate(1000,"AnimDoor");p.tick(1000,0);p.tick(1020,1);candidate(1040,"AnimDoor");p.tick(1040,1);
 p.rayPublished116=false;p.tick(1060,1);assert(!p.touch_bindings_.held&&p.touch_bindings_.pulses==1);
 p.rayPublished116=true;candidate(1080,"AnimDoor");p.tick(1080,1);assert(p.touch_bindings_.pulses==1&&!p.touch_bindings_.held);
 p.tick(1100,0);candidate(1120,"AnimDoor");p.tick(1120,1);candidate(1140,"AnimDoor");p.tick(1140,1);assert(p.touch_bindings_.pulses==2);
 reset();PickupProducer19 failed;failed.touch_bindings_.accept=false;candidate(1000);failed.tick(1000,0);
 g_use13.read(startUse);failed.tick(1020,1);candidate(1040);failed.tick(1040,1);
 for(unsigned t=1060;t<=1500;t+=20){candidate(t);failed.tick(t,1);}
 failed.tick(1520,0);g_use13.read(endUse);
 assert(failed.touch_bindings_.pulses==1&&!failed.touch_bindings_.held&&endUse.serial==startUse.serial);
 unknownPickingDefinition=false;pickup_use19c3::setNearOnlyD40(savedReachD40);

 // C19C4: actual checked definition reader. The old getter's sub-tag
 // pointer can contain arbitrary binary bytes; it is not consulted as text.
 reset();std::uint32_t actualCrc=0;
 assert(FragmentCrc19c4(controller.data(),1,actualCrc)&&actualCrc==0x03E46E25u);
 assert(FragName(controller.data(),1)&&!FragName(controller.data(),0));
 put(fragmentEntries.data()+8,0x20+8,static_cast<void*>(logic.data()));
 assert(FragName(controller.data(),1)); // sibling sub-definition pointer is irrelevant
 assert(!FragmentCrc19c4(controller.data(),-1,actualCrc)&&!FragmentCrc19c4(controller.data(),2,actualCrc));
 put(fragmentEntries.data(),4,int(65537));assert(!FragmentCrc19c4(controller.data(),1,actualCrc));
 put(fragmentEntries.data(),4,int(2));put(controller.data(),kCtrlContext,static_cast<void*>(nullptr));assert(!FragName(controller.data(),1));
 reset();put(controller.data(),0,std::uintptr_t(0));assert(!FragName(controller.data(),1));
 reset();put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(0x12345678));assert(!FragName(controller.data(),1));
 // C19C4: actual Henry Attach runs on an animation job, then its synchronous
 // native transfer is intercepted on THAT stack. No pointer/event replay and no
 // job-thread Mannequin speed write; the controller adopts the pending attach.
 reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
 const auto controllerThread=g_owner_thread13.load();
 std::thread job19([]{pickupThread19c4=19728;assert(!OnOwnerThread13());attach();assert(!g_pickup_job_logic19c4);});job19.join();
 assert(g_owner_thread13==controllerThread&&attaches==1&&notices==1&&transfers==0&&g_owner.state==PendingAttach);
 assert(g_pick13.controllerPausePending19c4&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 assert(!g_pick13.attached&&!WristOwnedByController(1));
 std::thread duplicateJob19([]{pickupThread19c4=29472;attach();std::array<unsigned char,40> ev{};put(ev.data(),0x1c,unsigned(102));PickupEventDetour13(logic.data(),ev.data());});duplicateJob19.join();
 assert(attaches==1&&transfers==0&&cleanup==0&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 g_update_original=UpdateActionLeaf;updateOriginalCalls=0;frame(1020);UpdateDetour(controller.data(),.02f);
 assert(g_owner.state==Held&&!g_pick13.controllerPausePending19c4&&finishes==0);
 poseProof55();frame(1040);UpdateDetour(controller.data(),.02f);
 assert(g_owner.state==Held&&!g_pick13.controllerPausePending19c4&&g_pick13.cleanupSeen&&finishes==1&&g_owner.action==nullptr&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 frame(1060,0);UpdateDetour(controller.data(),.02f);UpdateDetour(controller.data(),.02f);
 assert(drops==1&&cleanup==1&&transfers==0&&g_owner.state==Idle);
 // The same exact native proof can fill the queue-observation gap at Attach.
 reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));ResetOwner();g_consumed_use13=0;
 std::thread fallbackJob19([]{pickupThread19c4=12204;attach();});fallbackJob19.join();
 assert(attaches==1&&transfers==0&&g_owner.state==PendingAttach);frame(1020);UpdateDetour(controller.data(),.02f);assert(g_owner.state==Held);
 // A queued action installed by native Update before the next owner tick uses
 // the current proven native token, not the pre-install token.
 reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));g_owner.state=Armed;
 put(action.data(),kActToken,unsigned(56));std::thread queuedJob19([]{pickupThread19c4=30764;attach();});queuedJob19.join();
 assert(attaches==1&&g_owner.nativeToken==56&&g_owner.state==PendingAttach);frame(1020);UpdateDetour(controller.data(),.02f);assert(g_owner.state==Held);
 // C57: ALL admission paths share one exact current FullBody Picking proof.
 // Exercise the real early observer, owner/job event + native nested transfer,
 // and queued installation; never repair wrong class/fragment/context by name.
 unsigned rootCases57=0;
 reset();const char* rootReasonH17=nullptr;CurrentRoot57 rootProofH17{};
 assert(ReadCurrentRootIdentity57(controller.data(),true,rootProofH17,0,&rootReasonH17)&&!std::strcmp(rootReasonH17,"READY"));
 put(action.data(),kActStatus,int(1));
 assert(!ReadCurrentRootIdentity57(controller.data(),true,rootProofH17,0,&rootReasonH17)&&!std::strcmp(rootReasonH17,"ACTION_NOT_PLAYING"));
 reset();put(action.data(),kActContext,static_cast<void*>(logic.data()));
 assert(!ReadCurrentRootIdentity57(controller.data(),true,rootProofH17,0,&rootReasonH17)&&!std::strcmp(rootReasonH17,"ACTION_CONTEXT_MISMATCH"));
 reset();put(action.data(),0,WhBase()+0x3B13F38);
 assert(!ReadCurrentRootIdentity57(controller.data(),true,rootProofH17,0,&rootReasonH17)&&!std::strcmp(rootReasonH17,"PICKING_ACTION_CLASS_MISMATCH"));
 for(bool onJob57:{false,true})for(unsigned refusal57=0;refusal57<6;++refusal57){
  reset();ResetOwner();g_consumed_use13=0;testAttachManager=manager.data();
  switch(refusal57){
   case 0:put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(0x12345678));break;
   case 1:put(action.data(),0,WhBase()+0x3B13F38);break;
   case 2:put(action.data(),kActContext,static_cast<void*>(logic.data()));break;
   case 3:put(controller.data(),kCtrlContext,static_cast<void*>(nullptr));break;
   case 4:put(controller.data(),kCtrlScopeCount,unsigned(0));break;
   case 5:put(action.data(),kActFragId,int(0));break;
  }
  const auto beforeArm57=g_armed_at_attach18.load();const auto root57=ReadCurrentRoot55(controller.data(),1000);
  assert(!root57.valid||!root57.picking);
  if(onJob57){std::thread rejected57([]{pickupThread19c4=37852;attach();});rejected57.join();}else attach();
  ng13::Acquisition54 absent57{};
  assert(g_armed_at_attach18.load()==beforeArm57&&transfers==1&&!g_pick13.attachStarted&&
    g_owner.state==Idle&&finishes==0&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f&&!ReadAcquisition54(absent57));
  ++rootCases57;
 }
 for(unsigned refusal57=0;refusal57<3;++refusal57){
  reset();g_owner.state=Armed;g_owner.scope=nullptr;
  if(refusal57==0)put(action.data(),0,WhBase()+0x3B13F38);
  if(refusal57==1)put(action.data(),kActFragId,int(0));
  if(refusal57==2)put(action.data(),kActContext,static_cast<void*>(nullptr));
  TickOwner(controller.data(),1000);assert(g_owner.state==Idle&&finishes==0&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
  ++rootCases57;
 }
 for(unsigned corrupt57=1;corrupt57<=2;++corrupt57){
  reset();corruptRootAtTransfer57=corrupt57;attach();
  assert(transfers==1&&!g_pick13.attachStarted&&g_owner.state==Idle&&finishes==0&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
  ++rootCases57;
 }
 reset();ResetOwner();g_consumed_use13=0;attach();assert(attaches==1&&transfers==0&&g_owner.state==Held);++rootCases57;
 std::printf("C57 exact current Picking: %u actual early/owner/job/transfer/queued root cases PASS; foreign actions stay native\n",rootCases57);
 // Unqualified jobs retain the original native pickup, not an invented hold.
 for(unsigned why=0;why<7;++why){reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
  if(why==0)put(logic.data(),8,unsigned(99));if(why==1)put(action.data(),kActContext,static_cast<void*>(nullptr));
  if(why==2)put(fragmentEntries.data()+8,0x20+0x10,unsigned(0x12345678));if(why==3)put(action.data(),kActRootScope,static_cast<void*>(nullptr));
  if(why==4)put(action.data(),kActStatus,int(3));if(why==5)testAttachManager=nullptr;if(why==6)g_context13=false;
  std::thread badJob19([]{pickupThread19c4=37852;attach();});badJob19.join();assert(transfers==1&&!g_pick13.attachStarted&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 }
 // No blocking/spinning: if the controller's own small transaction is busy,
 // an unowned callback falls back native; an owned one cannot pocket the item.
 reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
 {OwnerTransaction19c4 held;assert(held);std::thread busyJob19([]{pickupThread19c4=19728;attach();});busyJob19.join();}
 assert(transfers==1&&!g_pick13.attachStarted);
 reset();attach();{OwnerTransaction19c4 held;assert(held);std::thread busyOwned19([]{pickupThread19c4=19728;attach();});busyOwned19.join();}
 assert(transfers==0&&attaches==1&&g_foreign_cancel13);frame(1020);UpdateDetour(controller.data(),.02f);assert(drops==1);
 // Crucially the original Update runs outside our transaction and can join an
 // event job. This actual producer test would fall back native if it retained it.
 reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
 g_update_original=+[](void*,float){std::thread nested19([]{pickupThread19c4=19728;attach();});nested19.join();};
 UpdateDetour(controller.data(),.02f);
 assert(attaches==1&&transfers==0&&g_owner.state==Held&&!g_pick13.controllerPausePending19c4&&
        g_pick13.attachedAt==1000&&g_owner.useInput55.tick==1000&&
        Rd<float>(action.data(),kActSpeedBias,-1)==0.f); // H5: controller adopts its joined job on return
 // Native terminal jobs are joined without waiting on our lock. Their one
 // cleanup executes ON its native stack; only completion is adopted later.
 for(unsigned which=0;which<3;++which){reset();attach();finishCleanupJob19c4=which!=1;dropCleanupJob19c4=which!=0;
  frame(1020,0);UpdateDetour(controller.data(),.02f);UpdateDetour(controller.data(),.02f);
  assert(drops==1&&transfers==0&&cleanup==1&&g_cleanup_claim19c4==g_cleanup_done19c4&&g_owner.state==Idle&&!g_foreign_cancel13);}
 // Cleanup may retire native objects immediately after the job returns. Only
 // the completed token is adopted: poisoned logic/action cannot be dereferenced
 // or replayed by the next controller transaction.
 reset();attach();auto oldToken19=g_pick13.bind.token;
 {OwnerTransaction19c4 held;std::thread nativeCancel19([]{pickupThread19c4=19728;PickupEndDetour19c4(logic.data(),0,0);});nativeCancel19.join();
  ng13::Binding cancelled{};assert(cleanup==1&&!ReadGraspBinding(cancelled));}
 logic.fill(0xdd);action.fill(0xdd);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));
 TickOwner(controller.data(),1020);assert(cleanup==1&&drops==0&&transfers==0&&g_owner.state==Idle&&g_cleanup_done19c4==oldToken19);
 std::puts("PASS exact Henry animation-job Attach -> native nested transfer -> controller pause/hold/drop, foreign-job negatives and nonblocking Update join");
 // C4R1 actual unjoined native cleanup: neither held Drop nor Stow may
 // mutate an item while the native callback is still running.
 for(int intent=0;intent<3;++intent){
  reset();char temporaryHelper[96]="slt_0";put(logic.data(),0x40,static_cast<const char*>(temporaryHelper));attach();
  const char* retainedHelper=queuedHelper19c4;std::memset(temporaryHelper,'?',sizeof(temporaryHelper));
  assert(retainedHelper!=temporaryHelper&&std::strcmp(retainedHelper,"slt_0")==0);
  cleanupBlocked19c4=true;
  if(intent==0){ // native cancellation starts before any VR release intention
   unjoinedCleanup19c4=std::thread([]{pickupThread19c4=29472;PickupEndDetour19c4(logic.data(),0,0);});
   cleanupEntered19c4.wait(false,std::memory_order_acquire);
   frame(1020,0);TickOwner(controller.data(),1020);assert(finishes==0);
  }else{
   finishUnjoined19c4=true;
   Terminal13(intent==1?ng13::Terminal::Drop:ng13::Terminal::Stow,false,"UNJOINED_FIXTURE");
   cleanupEntered19c4.wait(false,std::memory_order_acquire);
   assert(finishes==1&&g_pick13.pendingTerminal==(intent==1?ng13::Terminal::Drop:ng13::Terminal::Stow));
  }
  for(unsigned t=1040;t<1200;t+=20){frame(t,0);TickOwner(controller.data(),t);}
  assert(drops==0&&transfers==0&&Rd<std::uint64_t>(slots.data(),0x260,0)==key&&cleanup==1);
  cleanupRelease19c4.store(true,std::memory_order_release);cleanupRelease19c4.notify_all();unjoinedCleanup19c4.join();
  logic.fill(0xdd);action.fill(0xdd);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));forbidRetiredRead19c4=true;
  TickOwner(controller.data(),1220);
  assert(retiredReads19c4==0&&g_owner.state==Idle&&cleanup==1&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
  assert(drops==(intent==1?1:0)&&transfers==(intent==2?1:0));
  assert(std::strcmp(retainedHelper,"slt_0")==0); // queued char* still valid after request+lease retirement
 }
 // Cleanup inner return is insufficient: the outer End continuation can
 // still update native manager/player state. Hold all item writes until RET.
 reset();attach();outerBlocked19c4=true;finishUnjoined19c4=true;
 Terminal13(ng13::Terminal::Drop,false,"OUTER_END_BARRIER");outerEntered19c4.wait(false,std::memory_order_acquire);
 assert(cleanup==1&&g_cleanup_original19c4==g_pick13.bind.token&&g_cleanup_done19c4!=g_pick13.bind.token);
 TickOwner(controller.data(),1020);assert(drops==0&&transfers==0);
 outerRelease19c4.store(true,std::memory_order_release);outerRelease19c4.notify_all();unjoinedCleanup19c4.join();
 TickOwner(controller.data(),1040);assert(drops==1&&cleanup==1&&g_owner.state==Idle);
 // Two unjoined outer callbacks for the same native logic: the first RET
 // cannot publish completion while the other continuation remains active.
 reset();attach();finishNoCleanup19c4=true;Terminal13(ng13::Terminal::Drop,false,"TWO_OUTERS");outerBlocked19c4=true;
 unjoinedCleanup19c4=std::thread([]{pickupThread19c4=29472;PickupEndDetour19c4(logic.data(),0,0);});outerEntered19c4.wait(false);
 std::thread secondOuter([]{pickupThread19c4=30000;PickupEndDetour19c4(logic.data(),0,0);});secondOuterEntered19c4.wait(false);
 assert((g_outer_callbacks19c4.load()&0xFFFF)==2&&cleanup==1);
 outerRelease19c4.store(true);unjoinedCleanup19c4.join();TickOwner(controller.data(),1020);
 assert((g_outer_callbacks19c4.load()&0xFFFF)==1&&g_cleanup_done19c4!=g_pick13.bind.token&&drops==0&&transfers==0);
 secondOuterRelease19c4.store(true);secondOuter.join();TickOwner(controller.data(),1040);assert(drops==1&&g_owner.state==Idle&&cleanup==1);
 // Exact pending native Detach is allowed even while ForceFinish's controller
 // transaction is occupied; the original event/cleanup executes once, no Attach.
 reset();attach();finishNoCleanup19c4=true;Terminal13(ng13::Terminal::Drop,false,"DETACH_COMPLETION");
 {OwnerTransaction19c4 busy;std::thread detachJob([]{pickupThread19c4=19728;std::array<unsigned char,40> event{};put(event.data(),0x1c,unsigned(102));PickupEventDetour13(logic.data(),event.data());});detachJob.join();}
 assert(cleanup==1&&g_cleanup_done19c4==g_pick13.bind.token&&transfers==0);TickOwner(controller.data(),1020);assert(drops==1&&g_owner.state==Idle);
 // An unobserved foreign direct caller cannot invent an outer lifetime receipt.
 reset();attach();PickupCleanupDetour13(logic.data());TickOwner(controller.data(),1020);
 assert(cleanup==1&&drops==0&&transfers==0&&g_owner.state!=Idle&&g_cleanup_done19c4!=g_pick13.bind.token);
 // Missing native completion is not inferred from ForceFinish return/status.
 reset();attach();finishNoCleanup19c4=true;Terminal13(ng13::Terminal::Drop,false,"NO_CALLBACK");
 for(unsigned t=1020;t<6000;t+=20){frame(t,0);TickOwner(controller.data(),t);}
 assert(finishes==1&&drops==0&&transfers==0&&cleanup==0&&g_pick13.pendingTerminal==ng13::Terminal::Drop);
 PickupEndDetour19c4(logic.data(),0,0);TickOwner(controller.data(),6000);assert(drops==1&&g_owner.state==Idle);
 // Replacement item/actor cannot be touched after asynchronous cancellation.
 reset();attach();PickupEndDetour19c4(logic.data(),0,0);put(slots.data(),0x260,std::uint64_t(999));TickOwner(controller.data(),1020);
 assert(drops==0&&transfers==0&&Rd<std::uint64_t>(slots.data(),0x260,0)==999&&g_owner.state==Idle);
 reset();attach();PickupEndDetour19c4(logic.data(),0,0);put(player.data(),0x30,unsigned(43));TickOwner(controller.data(),1020);
 assert(drops==0&&transfers==0&&Rd<std::uint64_t>(slots.data(),0x260,0)==key&&g_owner.state!=Idle);
 // C4R2: releasing the native full-body action does not release the actual
 // world item. Finish/cleanup runs once; hold and later Drop/Stow survive the
 // original action+logic being destroyed. No motion/body flags are written.
 for(int terminal=0;terminal<3;++terminal){
  reset();attach();const auto token=g_pick13.bind.token;finishUnjoined19c4=true;outerBlocked19c4=true;
  frame(1020);TickOwner(controller.data(),1020);outerEntered19c4.wait(false);
  ng13::Binding held{};assert(finishes==1&&cleanup==1&&drops==0&&transfers==0&&!ReadGraspBinding(held));
  frame(1040);TickOwner(controller.data(),1040);assert(!g_pick13.cleanupSeen&&g_owner.action==action.data());
  outerRelease19c4.store(true);unjoinedCleanup19c4.join();
  logic.fill(0xdd);action.fill(0xdd);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));forbidRetiredRead19c4=true;
  frame(1060);TickOwner(controller.data(),1060);
  assert(g_owner.state==Held&&g_pick13.cleanupSeen&&g_owner.action==nullptr&&g_owner.scope==nullptr&&g_free_hold19c4r2==token);
  assert(ReadGraspBinding(held)&&held.place&&held.attached&&retiredReads19c4==0&&finishes==1&&cleanup==1);
  placed(1080,30.f);placed(1100,30.10f);assert(g_pick13.motion.have&&retiredReads19c4==0&&ReadGraspBinding(held));
  if(terminal==0){dropAck=false;frame(1120,0);}
  else if(terminal==1){frame(1120,1,.15f);TickOwner(controller.data(),1120);frame(1320,1,.15f);}
  else{g_context13=false;frame(1120,1);}
  TickOwner(controller.data(),hostTickCount64);
  assert(g_owner.state==Idle&&!g_native_fault13&&finishes==1&&cleanup==1&&retiredReads19c4==0);
  assert(drops==(terminal==1?0:1)&&transfers==(terminal==1?1:0)&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
 }
 // No invented held attachment after native End removes it. A remaining hand
 // membership is natively detached, with no forced reattach or physics writes.
 reset();attach();finishUnjoined19c4=true;outerBlocked19c4=true;frame(1020);TickOwner(controller.data(),1020);outerEntered19c4.wait(false);
 put(attachment.data(),0x20,static_cast<void*>(nullptr));outerRelease19c4.store(true);unjoinedCleanup19c4.join();frame(1040);TickOwner(controller.data(),1040);
 assert(g_owner.state==Idle&&attaches==1&&drops==0&&transfers==0&&cleanup==1&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
 // A pickup after the successful inline drop can arm again (no global-fault
 // latch). The game has reused native memory for a new live request/token.
 reset();attach();dropAck=false;frame(1020,0);TickOwner(controller.data(),1020);assert(g_owner.state==Idle&&!g_native_fault13);
 const auto firstToken=g_next_token13;put(logic.data(),0xFE,std::uint8_t(0));put(scope.data(),kScopeAction,static_cast<void*>(action.data()));
 put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));put(action.data(),kActToken,unsigned(56));
 frame(1200,1);NoteUseSent(0,1200);QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),1200);attach();
 assert(attaches==2&&g_pick13.bind.token>firstToken&&g_owner.state==Held&&!g_native_fault13);
 frame(1220,0);TickOwner(controller.data(),1220);assert(drops==2&&finishes==2&&cleanup==2&&g_owner.state==Idle);
 std::puts("PASS C4R2 inline-vs-queued drop, second pickup, native full-body completion -> current world hold, poison-safe movement placement and once-only Drop/Stow/cancel");
 // Retained destination/quantity can only use the CURRENT exact inventory.
 reset();put(logic.data(),0x28+0xC0,static_cast<void*>(&inventory));attach();currentInventory19c4=&worldContainer;
 Terminal13(ng13::Terminal::Stow,false,"INVENTORY_REPLACED");
 assert(cleanup==1&&transfers==0&&g_owner.state==Idle&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
 reset();put(logic.data(),0x28+0xB8,unsigned(1));attach();put(item.data(),0x50,unsigned(2));
 Terminal13(ng13::Terminal::Stow,false,"STACK_CHANGED");assert(cleanup==1&&transfers==0&&g_owner.state==Idle);
 // Overlong helper or exhausted bounded intern table leaves native pickup unchanged.
 reset();char longHelper[98];std::memset(longHelper,'a',97);longHelper[97]=0;put(logic.data(),0x40,static_cast<const char*>(longHelper));
 transferAck=false;attach();assert(transfers==1&&!g_pick13.attachStarted);
 const auto savedHelperCount=g_grasp_helper_count19c4;reset();char uniqueHelper[]="new_unseen_helper";put(logic.data(),0x40,static_cast<const char*>(uniqueHelper));
 g_grasp_helper_count19c4=32;transferAck=false;attach();assert(transfers==1&&!g_pick13.attachStarted);g_grasp_helper_count19c4=savedHelperCount;
 // Non-Henry native callbacks execute once without acquiring the transaction.
 reset();updateOriginalCalls=queueOriginalCalls=0;std::array<unsigned char,0x1100> npcController{},npcLogic{};put(npcLogic.data(),8,unsigned(99));
 g_update_original=+[](void*,float){assert(g_owner_transaction_depth19c4==0);++updateOriginalCalls;};
 g_queue_original=+[](void*,void*,float){assert(g_owner_transaction_depth19c4==0);++queueOriginalCalls;};
 g_pick_cleanup13=+[](void*){assert(g_owner_transaction_depth19c4==0);++cleanup;};
 UpdateDetour(npcController.data(),.02f);QueueDetour(npcController.data(),action.data(),0);PickupCleanupDetour13(npcLogic.data());
 assert(updateOriginalCalls==1&&queueOriginalCalls==1&&cleanup==1);
 std::puts("PASS C4R1 unjoined cleanup-first/terminal-first Drop+Stow, immutable helper/request lifetime, no retired reads, missing callback and NPC fast paths");
 // Native accepted request -> real QueueDetour -> native-playing root ->
 // Attach -> native Finish/cleanup -> indefinite independent hold -> one drop.
 // No guessed text leaf, normalized-time seek or native action bypass.
 reset();ResetOwner();g_consumed_use13=0;NoteUseSent(0,1000);
 put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(action.data(),kActRootScope,static_cast<void*>(nullptr));
 put(action.data(),kActStatus,int(0));put(action.data(),kActSpeedBias,float(1));g_queue_original=&QueueActionLeaf;queueOriginalCalls=0;
 QueueDetour(controller.data(),action.data(),.125f);
 assert(queueOriginalCalls==1&&g_owner.state==Armed&&g_owner.action==action.data());
 assert(Rd<float>(action.data(),kActQueueTime,0)==.125f&&Rd<float>(action.data(),kActSpeedBias,0)==1);
 put(scope.data(),kScopeAction,static_cast<void*>(action.data()));put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));
 g_update_original=&UpdateActionLeaf;updateOriginalCalls=0;frame(1020);UpdateDetour(controller.data(),.02f);
 // Native time remains 1x until real Attach; do not restore the unproven 8x
 // approach merely to satisfy C55's former clock expectation.
 assert(g_owner.state==Playing&&updateOriginalCalls==1&&Rd<float>(action.data(),kActActiveTime,0)==.02f);
 attach();assert(g_owner.state==Held&&attaches==1&&transfers==0&&Rd<float>(action.data(),kActSpeedBias,1)==0);
 for(unsigned t=1040;t<=2040;t+=20){frame(t);UpdateDetour(controller.data(),.02f);}
 assert(g_owner.state==Held&&g_pick13.cleanupSeen&&g_owner.action==nullptr&&g_owner.scope==nullptr&&finishes==1&&cleanup==1&&Rd<float>(action.data(),kActSpeedBias,0)==1);
 assert(attaches==1&&transfers==0&&drops==0);
 frame(2060,0);UpdateDetour(controller.data(),.02f);assert(drops==1&&transfers==0&&(g_owner.state==Idle));
 UpdateDetour(controller.data(),.02f);assert(drops==1&&cleanup==1&&queueOriginalCalls==1&&g_owner.state==Idle);
 reset();attach();assert(attaches==1&&transfers==0&&notices==1&&g_owner.state==Held);assert(Container13(item.data())==&worldContainer);assert(WristOwnedByController(0));
 attach();assert(attaches==1&&transfers==0); // repeated native Attach cannot store
 for(unsigned t=1020;t<12000;t+=20){frame(t);TickOwner(controller.data(),t);}assert(transfers==0&&drops==0&&g_owner.state==Held);
 placed(12000,30);placed(12020,30.10f);frame(12021,0,.10f);TickOwner(controller.data(),12021);
 assert(drops==1&&transfers==0&&(g_owner.state==Idle)&&cleanup==1);assert(std::abs(releasedPose.p[0]-30.1f)<.0001);assert(std::abs(releasedVelocity[0]-5)<.01);assert(releasedPose.p[0]!=nativePose.p[0]);
 PickupEndDetour19c4(logic.data(),0,0);assert(drops==1&&cleanup==1);attach();assert(attaches==1);TickOwner(controller.data(),12022);assert(g_owner.state==Idle);
 const int callsBeforeLate=eventCalls;attach();assert(eventCalls==callsBeforeLate&&transfers==0); // retired duplicate stays harmless
 put(logic.data(),0xFE,std::uint8_t(0));SetMode(Observe);attach();assert(eventCalls==callsBeforeLate+1&&transfers==1); // native reused instance is allowed

 reset();attach();frame(1020,1,.15);TickOwner(controller.data(),1020);frame(1201,1,.15);TickOwner(controller.data(),1201);
 assert(transfers==1&&notices==1&&drops==0&&(g_owner.state==Idle)&&Container13(item.data())==&inventory);
 assert(finishes==1&&cleanup==1); // native cleanup precedes the deferred transfer
 assert(stowWorldRetired32&&Rd<void*>(item.data(),0x88,reinterpret_cast<void*>(1))==nullptr&&attaches==1); // no kept-world reattach/floating prop
 PickupEndDetour19c4(logic.data(),0,0);PickupEndDetour19c4(logic.data(),0,0);assert(cleanup==1&&transfers==1&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
 // C35 actual owner uses the configured 28cm stow radius. Native cleanup
 // ordering still wins; acquisition near the torso is not an auto-stow.
 reset();SetPropPresentation35(.02f,.28f);attach();frame(1020,1,.27f);TickOwner(controller.data(),1020);
 assert(transfers==0&&drops==0);frame(1201,1,.27f);TickOwner(controller.data(),1201);
 assert(transfers==1&&drops==0&&cleanup==1&&g_owner.state==Idle);
 reset();SetPropPresentation35(.02f,.24f);attach();frame(1020,1,.27f);TickOwner(controller.data(),1020);
 frame(1201,1,.27f);TickOwner(controller.data(),1201);assert(transfers==0&&g_owner.state==Held);
 frame(1220,0,.27f);TickOwner(controller.data(),1220);assert(transfers==0&&drops==1);
 SetPropPresentation35(.02f,.28f);
 // C59: actual native Attach -> canonical grip -> Finish/cleanup -> retained
 // hold -> fresh chest release commits the ORIGINAL transfer exactly once.
 reset();attach();frame(1020,1,.27f);TickOwner(controller.data(),1020);
 assert(transfers==0&&drops==0&&g_pick13.cleanupSeen);
 frame(1040,0,.27f);TickOwner(controller.data(),1040);TickOwner(controller.data(),1041);
 assert(transfers==1&&drops==0&&cleanup==1&&g_owner.state==Idle&&stowWorldRetired32);
 assert(Rd<std::uint64_t>(slots.data(),0x260,key)==0&&Container13(item.data())==&inventory);
 PickupEndDetour19c4(logic.data(),0,0);assert(transfers==1&&cleanup==1);
 // Releasing outside the actual current chest radius still drops, even after
 // a prior near witness. Context loss never becomes an inventory operation.
 for(unsigned refusal59=0;refusal59<3;++refusal59){
  reset();attach();frame(1020,1,.27f);TickOwner(controller.data(),1020);
  frame(1040,0,refusal59==0?.40f:.27f);
  if(refusal59==1)SetContextAvailable(false);
  if(refusal59==2){ng13::HandFrame h{};g_hands13.read(h);h.tracked[0]=false;PublishHands(h);}
  TickOwner(controller.data(),1040);assert(drops==1&&transfers==0&&g_owner.state==Idle);
 }
 reset();attach();mergeOnStow=true;frame(1020,1,.15);TickOwner(controller.data(),1020);frame(1201,1,.15);TickOwner(controller.data(),1201);
 assert(transfers==1&&itemRetired&&(g_owner.state==Idle)&&!g_native_fault13);PickupEndDetour19c4(logic.data(),0,0);assert(transfers==1&&cleanup==1);
 reset();put(item.data(),0x50,unsigned(5));put(logic.data(),0x28+0xB8,unsigned(1));attach();assert(transfers==1&&!g_pick13.attachStarted); // partial stack stays native
 reset();attach();frame(1020,1,.15);TickOwner(controller.data(),1020);for(int i=0;i<20;++i)TickOwner(controller.data(),1020);assert(transfers==0);
 frame(1201,0,.15);TickOwner(controller.data(),1201);assert(drops==1&&transfers==0); // previous held-near frame stale181ms
 reset();attach();placed(1020,30);placed(1040,30.1f);SetContextAvailable(false);TickOwner(controller.data(),1041);assert(drops==1&&releasedVelocity[0]==0);
 reset();attach();placed(1020,30);placed(1040,30.1f);put(entity.data(),0x64,float(31.f));frame(1041,0);TickOwner(controller.data(),1041);assert(drops==1&&releasedPose.p[0]==31&&releasedVelocity[0]==0);
 reset();attach();SetMode(Observe);TickOwner(controller.data(),1001);assert(drops==1&&transfers==0);
 reset();attach();put(slots.data(),0x260,std::uint64_t(999));frame(1020,0);TickOwner(controller.data(),1020);assert(drops==0&&g_native_fault13);
 // The exact retail false return is inline execution, not a failed drop.
 reset();attach();dropAck=false;frame(1020,0);TickOwner(controller.data(),1020);assert(drops==1&&g_owner.state==Idle&&!g_native_fault13);
 TickOwner(controller.data(),1021);assert(drops==1&&cleanup==1&&transfers==0);
 bool inlineReceipt=false;for(const auto& line:lines)if(line.find("dispatch=INLINE_NATIVE_RETURN")!=std::string::npos)inlineReceipt=true;assert(inlineReceipt);
 // An unobserved dispatcher remains ambiguous; never repeat irreversible Drop.
 reset();attach();dropObserved=false;frame(1020,0);TickOwner(controller.data(),1020);assert(drops==1&&g_owner.state!=Idle&&g_native_fault13);
 TickOwner(controller.data(),1021);PickupEndDetour19c4(logic.data(),0,0);assert(drops==1&&cleanup==1&&transfers==0);
 reset();attachThrows=true;attach();assert(attaches==1&&g_native_fault13&&transfers==0);attach();assert(attaches==1);
 reset();attach();transferAck=false;frame(1020,1,.15);TickOwner(controller.data(),1020);frame(1201,1,.15);TickOwner(controller.data(),1201);assert(transfers==1&&g_native_fault13);TickOwner(controller.data(),1202);assert(transfers==1&&drops==0);
 reset();attach();g_owner_thread13=2;PickupEndDetour19c4(logic.data(),0,0);
 assert(cleanup==1&&drops==0);{ng13::Binding cancelled{};assert(!ReadGraspBinding(cancelled));}
 g_owner_thread13=1;TickOwner(controller.data(),1001);assert(drops==0&&g_owner.state==Idle); // native cancellation wins; no replay
 reset();attach();put(action.data(),kActSpeedBias,float(.75));frame(1020,0);TickOwner(controller.data(),1020);assert(Rd<float>(action.data(),kActSpeedBias,0)==.75f); // do not restore over another speed writer
 reset();attach();put(entity.data(),0x58,std::numeric_limits<float>::quiet_NaN());frame(1020,0);TickOwner(controller.data(),1020);assert(drops==0&&g_native_fault13);
 reset();SetMode(Observe);attach();assert(transfers==1&&Container13(item.data())==&inventory&&!g_pick13.attachStarted); // comparison preserves native path
 // An Attach event does not establish the identity of the FullBody action.
 // Unknown fragment definitions stay fully native on EVERY callback thread.
 unknownPickingDefinition=true;reset();assert(g_owner.state==Idle);const auto beforeArms57=g_armed_at_attach18.load();attach();
 assert(g_armed_at_attach18.load()==beforeArms57&&attaches==1&&transfers==1&&g_owner.state==Idle&&!g_pick13.attachStarted);
 unknownPickingDefinition=false;
 // no recent VR hand Use (> 4 s) -> the native pickup is untouched
 reset();ResetOwner();g_consumed_use13=0;hostTickCount64=6000;attach();assert(g_owner.state==Idle&&transfers==1&&attaches==1);
 // a different actor's pickup logic never arms Henry's owner
 reset();ResetOwner();g_consumed_use13=0;put(logic.data(),8,unsigned(43));attach();assert(g_owner.state==Idle);
 unknownPickingDefinition=false;
 // C10R18 (review): an Attach the owner armed but the transfer did not take over releases the owner at once (native unchanged).
 unknownPickingDefinition=true;reset();frame(1000,.2f);attach();assert(transfers==1&&!g_pick13.attachStarted&&g_owner.state==Idle&&!WristOwnedByController(0));
 unknownPickingDefinition=false;
 // C10R18 (review): a transient null skinning pointer is not an identity change - the controller's FullBody character stands in.
 reset();attach();assert(g_owner.state==Held);testSkel=nullptr;frame(1020,0);TickOwner(controller.data(),1020);
 assert(drops==1&&(g_owner.state==Idle)&&!g_native_fault13);
 // C10R18: Henry's controller by entity id (R15) when the skinning path is null
 reset();testSkel=nullptr;testAttachManager=manager.data();g_henry_ctrl=nullptr;assert(IsHenryController(controller.data()));
 put(controller.data(),kCtrlEntityId,unsigned(43));assert(!IsHenryController(controller.data()));put(controller.data(),kCtrlEntityId,unsigned(42));
 unknownPickingDefinition=false;
 // C10R18: the attach-only last argument mirrors the native byte when the request animates
 reset();put(logic.data(),0x38,std::uint8_t(1));put(flagSource.data(),0x380,std::uint8_t(1));attach();assert(attaches==1&&lastAttachFlag);
 reset();put(logic.data(),0x38,std::uint8_t(0));put(flagSource.data(),0x380,std::uint8_t(1));attach();assert(attaches==1&&!lastAttachFlag);
 // C19C1: WUID membership is immediate; character attachment may arrive later.
 reset();deferAttachment=true;attach();assert(attaches==1&&g_owner.state==PendingAttach&&!g_pick13.attached&&!g_native_fault13);
 {ng13::Binding b{};assert(!ReadGraspBinding(b)&&!b.place);}
 attach();assert(attaches==1&&transfers==0); // duplicated event cannot enqueue a second attach
 frame(1020,0);TickOwner(controller.data(),1020);
 assert(drops==0&&cleanup==1&&!g_native_fault13&&g_owner.state==Idle&&Rd<std::uint64_t>(slots.data(),0x260,key)==0);
 // Cleanup completed before the queued attachment; native hand identity is
 // removed, so the deferred native callback cannot attach after Grip release.
 frame(1040,1);TickOwner(controller.data(),1040);assert(attaches==1&&transfers==0&&drops==0);
 reset();deferAttachment=true;attach();frame(1020,1);TickOwner(controller.data(),1020);
 assert(!g_pick13.attached&&g_owner.state==PendingAttach);
 put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));TickOwner(controller.data(),1021);
 assert(g_pick13.attached&&g_owner.state==Held&&!g_native_fault13); // native completion, not a second acquire
 // C10R20: +0x7A is a transform-dirty flag. It delays the FIRST observation only; a held item whose transform is dirty
 // (our own placement every frame) still releases normally - never a terminal fault.
 reset();deferAttachment=true;attach();put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));put(attachment.data(),0x7A,std::uint8_t(1));
 assert(!ObserveAttached19(1020)&&g_owner.state==PendingAttach);put(attachment.data(),0x7A,std::uint8_t(0));assert(ObserveAttached19(1021)&&g_owner.state==Held);
 put(attachment.data(),0x7A,std::uint8_t(1));frame(1040,0);TickOwner(controller.data(),1040);
 assert(drops==1&&!g_native_fault13&&(g_owner.state==Idle));
 // C28: real attachment present but dirty forever. Native hand membership
 // alone cannot grant HELD or any placement; a next-original-pose POD must
 // be revalidated on the controller before the ordinary finish-for-hold path.
 auto pendingSocket28=[&](){
  reset();deferAttachment=true;attach();put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));
  put(attachment.data(),0x7A,std::uint8_t(1));put(attachment.data(),0x78,std::int16_t(0));
  const float local[7]{0,0,0,1,0,0,0};std::memcpy(attachment.data()+0x9C,local,sizeof(local));
  put(image.data(),0x3A657C0+0x60,reinterpret_cast<std::uintptr_t>(image.data())+0x1A72860);
  put(image.data(),0x3A657C0+0xC8,reinterpret_cast<std::uintptr_t>(image.data())+0x1AAF3C0);
  const unsigned char relativeGetter28[]{0x48,0x8D,0x81,0x9C,0,0,0,0xC3};
  const unsigned char jointGetter28[]{0x0F,0xB7,0x41,0x78,0xC3};
  std::memcpy(image.data()+0x1A72860,relativeGetter28,sizeof(relativeGetter28));
  std::memcpy(image.data()+0x1AAF3C0,jointGetter28,sizeof(jointGetter28));
  frame(1020);TickOwner(controller.data(),1020);
  ng13::Binding pending{};assert(ReadGraspBinding(pending)&&pending.socketOnly28&&!pending.attached&&!pending.place);
  assert(g_owner.state==PendingAttach&&finishes==0&&drops==0&&transfers==0);
  ng13::SocketProof28 proof{};proof.binding=pending;proof.self=reinterpret_cast<std::uintptr_t>(attachmentObject.data());
  proof.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());proof.sample=1020;proof.tick=1020;proof.joint=0;
  std::memcpy(proof.relative,local,sizeof(local));proof.gripReady55=true;return proof;
 };
 {auto proof=pendingSocket28();ng13::Placement forbidden{};forbidden.binding=proof.binding;forbidden.sample=1020;forbidden.tick=1020;
  PublishGraspPlacement(forbidden);assert(!g_pick13.motion.have); // discovery is not a write capability
  PublishGraspSocket28(proof);frame(1040);TickOwner(controller.data(),1040);
  assert(g_pick13.attached&&g_pick13.cleanupSeen&&g_owner.state==Held&&finishes==1&&cleanup==1);
  assert(g_owner.action==nullptr&&g_owner.scope==nullptr); // FullBody authoring really released
  ng13::Binding live{};assert(ReadGraspBinding(live)&&live.attached&&live.place&&!live.socketOnly28);
  frame(1060,0);TickOwner(controller.data(),1060);assert(drops==1&&g_owner.state==Idle&&!g_native_fault13);
  PublishGraspSocket28(proof);frame(1080);TickOwner(controller.data(),1080);assert(drops==1&&!ReadGraspBinding(live));
 }
 for(unsigned fault=0;fault<12;++fault){auto proof=pendingSocket28();
  if(fault==0)++proof.binding.token;if(fault==1)++proof.binding.key;if(fault==2)++proof.binding.epoch;
  if(fault==3)proof.attachment=0;if(fault==4)++proof.self;if(fault==5)proof.joint=1;
  if(fault==6)proof.tick=939;if(fault==7)proof.tick=1021;if(fault==8)proof.sample=1021;
  if(fault==9)proof.relative[4]=.1f;if(fault==10)proof.relative[3]=0;
  if(fault==11)SetContextAvailable(false);
  PublishGraspSocket28(proof);
  if(fault==11){TickOwner(controller.data(),1020);assert(g_owner.state==Idle&&cleanup==1&&drops==0);}
  else {TickOwner(controller.data(),1020);assert(!g_pick13.attached&&finishes==0&&drops==0&&transfers==0);}
 }
 {auto proof=pendingSocket28();PublishGraspSocket28(proof);g_owner_thread13=2;
  assert(!ObserveAttached19(1020)&&!g_pick13.attached&&finishes==0);g_owner_thread13=1;
  put(attachment.data(),0x9C,float(.1f));assert(!ObserveAttached19(1020));
  put(attachment.data(),0x9C,float(0));assert(ObserveAttached19(1020));frame(1040,0);TickOwner(controller.data(),1040);assert(drops==1);
 }
 for(unsigned value:{2u,255u}){(void)pendingSocket28();put(attachment.data(),0x7A,std::uint8_t(value));
  assert(!ObserveAttached19(1020));ng13::Binding b{};assert(!ReadGraspBinding(b));
  frame(1040,0);TickOwner(controller.data(),1040);assert(!g_pick13.attached);
 }
 {auto old=pendingSocket28();PickupEndDetour19c4(logic.data(),0,0);TickOwner(controller.data(),1020);
  PublishGraspSocket28(old);assert(g_owner.state==Idle&&cleanup==1);
  auto current=pendingSocket28();PublishGraspSocket28(old);TickOwner(controller.data(),1020);
  assert(!g_pick13.attached&&finishes==0);PublishGraspSocket28(current);TickOwner(controller.data(),1020);
  assert(g_pick13.attached&&finishes==1);frame(1040,0);TickOwner(controller.data(),1040);assert(drops==1&&cleanup==1);
 }
 std::puts("PASS dirty native pickup -> observation-only socket -> original-pose proof -> controller Held -> native FullBody End -> free hold -> Drop;12 negatives and old-token regrab");
 reset();attach();put(attachment.data(),0x7A,std::uint8_t(1));put(attachmentObject.data(),8,unsigned(73));
 ng13::Pose refusedPose{};void* refusedAttachment=nullptr;assert(!NativeAttachment13(refusedAttachment,refusedPose));
 // A different bound entity and an uncompleted attachment never become Held.
 reset();deferAttachment=true;attach();put(attachmentObject.data(),8,unsigned(73));put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));
 frame(1020);TickOwner(controller.data(),1020);assert(!g_pick13.attached);
 frame(2020);TickOwner(controller.data(),2020);
 assert(drops==0&&transfers==0&&cleanup==1&&(g_owner.state==Idle)&&!g_native_fault13);
 assert(Rd<std::uint64_t>(slots.data(),0x260,key)==0&&Container13(item.data())==&worldContainer);
 // Native cleanup must still run exactly once for an action ending before attach.
 reset();deferAttachment=true;attach();PickupEndDetour19c4(logic.data(),0,0);TickOwner(controller.data(),1001);
 assert(cleanup==1&&drops==0&&transfers==0&&(g_owner.state==Idle)&&!g_native_fault13);
 PickupEndDetour19c4(logic.data(),0,0);assert(cleanup==1);
 reset();deferAttachment=true;attach();SetMode(Observe);TickOwner(controller.data(),1001);
 assert(cleanup==1&&drops==0&&g_owner.state==Idle&&!g_native_fault13);
 // Queue acknowledgement explicitly does not claim the later profile callback ran.
 reset();attach();frame(1020,0);TickOwner(controller.data(),1020);
 bool queued=false;for(const auto& line:lines)if(line.find("phase=DROP_QUEUED")!=std::string::npos&&line.find("COMPLETION_UNOBSERVED")!=std::string::npos)queued=true;
 assert(queued);
 // C19C1 connected capture/quiet-work checks: F5 diagnoses but never grants
 // ownership; quiet comparison/NPC paths do not resolve fragment names.
 reset();ResetOwner();g_mode=Observe;traceEnabled=true;
 namespace cap=kcd2vr::diagnostics::capture168;
 for(unsigned i=0;i<cap::DomainCount;++i)cap::gate.select(cap::Domain(i),false,1000);
 fragmentLookups=0;lines.clear();ScopeReceipts(controller.data(),1000);
 QueueDetour(controller.data(),action.data(),0);QueueDetour(player.data(),action.data(),0);
 UpdateDetour(controller.data(),.016f);assert(fragmentLookups==0&&lines.empty());
 cap::gate.select(cap::Domain::Combat,true,1000);assert(!DiagOn());
 assert(!cap::accept("R59.16C10R11 MANNEQUIN_QUEUE",1000));
 assert(!cap::accept("R59.16C10R11 ANIM_EVENT",1000));
 assert(cap::accept("C10R13 NATIVE_GRASP phase=DROP_QUEUED",1000));
 cap::gate.select(cap::Domain::Interaction,true,1000);assert(DiagOn());
 assert(cap::accept("R59.16C10R11 ACTION_OWNER state=HELD",1000));
 assert(cap::accept("R59.16C10R11 MANNEQUIN_SCOPE",1000));
 // F5-only refusal survives routing; quiet attempts consume no session budget.
 cap::gate.select(cap::Domain::Combat,false,1000);
 assert(cap::accept("C10R18 NATIVE_GRASP ATTACH_NOT_ARMED reason=NO_FRESH_VR_HAND_USE",1000));
 assert(cap::accept("C19C3 NATIVE_GRASP ATTACH_NOT_ARMED reason=NO_FULLBODY_ROOT_ACTION",1000));
 assert(cap::accept("C10 HAND_HOLD_INPUT held=1",1000));
 lines.clear();ArmRefused18("NO_FULLBODY_ROOT_ACTION",logic.data(),1000);
 assert(lines.size()==1&&lines.back().find("fullBodyReady=1")!=std::string::npos&&lines.back().find("useSide=0")!=std::string::npos);
 g_arm_notes19.total=40;lines.clear();ArmRefused18("BUDGET",logic.data(),1000);assert(lines.empty());
 cap::gate.select(cap::Domain::Interaction,false,1000);ArmRefused18("QUIET",logic.data(),1000);assert(lines.empty()&&g_arm_notes19.total==40);
 cap::gate.select(cap::Domain::Interaction,true,1000);ArmRefused18("NEW_SESSION",logic.data(),1000);
 assert(lines.size()==1&&g_arm_notes19.total==1);
 g_mode=Off;QueueDetour(controller.data(),action.data(),0);
 assert(fragmentLookups==1&&g_owner.state==Idle&&!NativeGraspEnabled());
 assert(!lines.empty()&&lines.back().find("MANNEQUIN_QUEUE")!=std::string::npos);
 ScopeReceipts(controller.data(),1000);assert(g_t_scope.total==1);
 g_t_scope.total=12000;ScopeReceipts(controller.data(),1000);assert(g_t_scope.total==12000);
 cap::gate.select(cap::Domain::Interaction,true,1000);ScopeReceipts(controller.data(),1000);
 assert(g_t_scope.total==1); // same scope/action is rediscovered in the new session
 QueueDetour(controller.data(),action.data(),0);g_t_queue.total=8000;
 QueueDetour(controller.data(),action.data(),0);assert(g_t_queue.total==8000);
 cap::gate.select(cap::Domain::Interaction,true,1000);QueueDetour(controller.data(),action.data(),0);
 assert(g_t_queue.total==1&&g_owner.state==Idle);
 g_orig_cb=&EventOriginal;g_tapped_skel=testSkel;eventOriginalCalls=0;
 assert(EventTap(character.data(),nullptr)==17&&eventOriginalCalls==1);
 const auto recordedEvents=g_events.load();cap::gate.select(cap::Domain::Interaction,false,1000);
 assert(EventTap(character.data(),nullptr)==17&&eventOriginalCalls==2&&g_events==recordedEvents);
 put(testSkel,kSkelCb,EventCb(&EventTap));refusedWrite19=static_cast<unsigned char*>(testSkel)+kSkelCb;
 assert(!RemoveTap()&&g_tapped_skel==testSkel&&g_orig_cb==&EventOriginal);
 assert(EventTap(character.data(),nullptr)==17&&eventOriginalCalls==3);
 // Quiet expiry follows the same retained-original path; no native event is lost.
 UpdateDetour(controller.data(),.016f);assert(g_tapped_skel==testSkel&&g_orig_cb==&EventOriginal);
 assert(EventTap(character.data(),nullptr)==17&&eventOriginalCalls==4);
 refusedWrite19=nullptr;assert(RemoveTap()&&!g_tapped_skel&&!g_orig_cb);
 assert(Rd<EventCb>(testSkel,kSkelCb,nullptr)==&EventOriginal);
 cap::gate.select(cap::Domain::Interaction,true,1000);
 hostTickCount64=1000+cap::Duration;assert(!DiagOn());fragmentLookups=0;
 QueueDetour(controller.data(),action.data(),0);ScopeReceipts(controller.data(),hostTickCount64);
 assert(fragmentLookups==0&&g_owner.state==Idle);
 for(unsigned i=0;i<cap::DomainCount;++i)cap::gate.select(cap::Domain(i),false,hostTickCount64);
 traceEnabled=false;
 // Actual rejected Attach events, not just a formatter test: F5 with F3 off
 // diagnoses the owner-thread/root gate and preserves exactly one native call.
 unknownPickingDefinition=true;reset();testAttachManager=manager.data();traceEnabled=true;
 cap::gate.select(cap::Domain::Interaction,true,1000);g_owner_thread13=2;lines.clear();attach();
 assert(g_owner.state==Idle&&transfers==1&&eventCalls==1);
 assert(lines.size()==1&&lines.back().find("threadMatch=0")!=std::string::npos&&lines.back().find("logicActor=42")!=std::string::npos);
 assert(lines.back().find("jobQualifier=FRAGMENT_NOT_PICKING")!=std::string::npos); // exact failed identity, not a guessed thread fix
 assert(cap::accept(lines.back().c_str(),1000));
 reset();cap::gate.select(cap::Domain::Interaction,true,1000);put(action.data(),kActStatus,int(1));lines.clear();attach();
 assert(g_owner.state==Idle&&transfers==1&&eventCalls==1);
 assert(lines.size()==1&&lines.back().find("reason=NO_EXACT_PICKING_ROOT")!=std::string::npos&&lines.back().find("fullBodyReady=0")!=std::string::npos);
 cap::gate.select(cap::Domain::Interaction,false,1000);traceEnabled=false;unknownPickingDefinition=false;
 // C19C5 lifecycle: actual production callbacks/owner; native leaves substituted.
 {
  static int secondTransfers19c5=0;secondTransfers19c5=0;
  auto count19c5=[](const char* needle){int n=0;for(const auto& l:lines)if(l.find(needle)!=std::string::npos)++n;return n;};
  auto has19c5=[](const char* a,const char* b){for(const auto& l:lines)if(l.find(a)!=std::string::npos&&l.find(b)!=std::string::npos)return true;return false;};
  // A second native C_PickUpItemLogic allocated at the retired address (new key/side, live +FE=0).
  auto reuseLogic19c5=[]{logic.fill(0);put(logic.data(),0,reinterpret_cast<std::uintptr_t>(image.data())+0x3A5B670);
   put(logic.data(),8,unsigned(42));put(logic.data(),0x28,key^0x10);put(logic.data(),0x30,int(1));put(logic.data(),0x38,std::uint8_t(1));
   put(logic.data(),0x40,static_cast<const char*>("slt_0"));float q[4]{0,0,0,1};std::memcpy(logic.data()+0x48,q,16);
   g_pick_transfer13=+[](void*,const void* r,bool){++secondTransfers19c5;assert(Rd<std::uint64_t>(r,0,0)==(key^0x10));return true;};};
  auto untracked19c5=[](std::uint64_t t){hostTickCount64=t;ng13::HandFrame f{};f.character=reinterpret_cast<std::uintptr_t>(character.data());f.epoch=9;f.sample=t;f.tick=t;PublishHands(f);};
  auto rearm19c5=[](std::uint64_t t){put(logic.data(),0xFE,std::uint8_t(0));put(scope.data(),kScopeAction,static_cast<void*>(action.data()));
   put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));put(action.data(),kActToken,unsigned(56));
   frame(t,1);NoteUseSent(0,t);QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),t);attach();};
  // (1a) Retired allocator reuse AFTER free-hold requalification: logic+frag retired, transfer NOT swallowed.
  reset();attach();frame(1020);TickOwner(controller.data(),1020);
  const auto heldToken19c5=g_pick13.bind.token;
  assert(g_owner.state==Held&&g_pick13.cleanupSeen&&g_pick13.bind.logic==0&&g_owner.frag[0]==0&&g_free_hold19c4r2==heldToken19c5);
  {ng13::Binding b{};assert(ReadGraspBinding(b)&&b.place&&b.logic==0&&b.token==heldToken19c5);}
  reuseLogic19c5();int events19c5=eventCalls;attach();
  assert(eventCalls==events19c5+1&&secondTransfers19c5==1&&attaches==1&&g_pick13.bind.token==heldToken19c5&&g_owner.state==Held);
  assert((g_outer_callbacks19c4.load()&0xFFFF)==0); // the reused address never joins the retired token
  {std::array<unsigned char,40> d{};put(d.data(),0x1c,unsigned(102));PickupEventDetour13(logic.data(),d.data());}
  assert(cleanup==2&&g_cleanup_claim19c4==heldToken19c5); // its cleanup is native-owned, unclaimed
  g_pick_transfer13=&TransferLeaf;dropAck=false;frame(1040,0);TickOwner(controller.data(),1040); // inline 1F1EB80 false = success
  assert(drops==1&&g_owner.state==Idle&&!g_native_fault13&&transfers==0&&has19c5("dispatch=INLINE_NATIVE_RETURN","released=1"));
  // (1b) Window between native outer completion and controller adoption: still not swallowed.
  reset();attach();finishUnjoined19c4=true;outerBlocked19c4=true;frame(1020);TickOwner(controller.data(),1020);outerEntered19c4.wait(false);
  const auto racingToken19c5=g_pick13.bind.token;outerRelease19c4.store(true);unjoinedCleanup19c4.join();
  assert(g_cleanup_done19c4==racingToken19c5&&g_pick13.bind.logic==reinterpret_cast<std::uintptr_t>(logic.data()));
  reuseLogic19c5();secondTransfers19c5=0;events19c5=eventCalls;attach();
  assert(eventCalls==events19c5+1&&secondTransfers19c5==1&&attaches==1&&g_pick13.bind.token==racingToken19c5);
  g_pick_transfer13=&TransferLeaf;frame(1040);TickOwner(controller.data(),1040);
  assert(g_owner.state==Held&&g_pick13.bind.logic==0&&g_free_hold19c4r2==racingToken19c5);
  frame(1060,0);TickOwner(controller.data(),1060);assert(drops==1&&g_owner.state==Idle&&!g_native_fault13);
  // (1c) Menu / tracking loss during a retired-logic hold: one native Drop, no fault.
  for(int why=0;why<2;++why){reset();attach();frame(1020);TickOwner(controller.data(),1020);assert(g_pick13.bind.logic==0);
   if(why==0){SetContextAvailable(false);frame(1040);}else untracked19c5(1040);
   TickOwner(controller.data(),1040);assert(drops==1&&g_owner.state==Idle&&!g_native_fault13&&transfers==0);}
  std::puts("PASS C19C5 retired Picking logic/frag after outer completion+requalification; reused address second pickup transfer not swallowed; inline drop; menu/tracking");
 }
 // Reviewed C25 safety closure: elapsed time and failed reads never complete native cleanup.
 {
  auto count=[](const char* needle){int n=0;for(const auto& l:lines)if(l.find(needle)!=std::string::npos)++n;return n;};
  reset();attach();finishNoCleanup19c4=true;frame(1020);TickOwner(controller.data(),1020);
  const auto token=g_pick13.bind.token;assert(g_pick13.finishing&&!g_pick13.cleanupSeen&&finishes==1);
  forbidRetiredRead19c4=true;lines.clear();
  for(unsigned t=1040;t<=6000;t+=20){frame(t);TickOwner(controller.data(),t);}
  assert(g_pick13.bind.token==token&&g_pick13.bind.logic==reinterpret_cast<std::uintptr_t>(logic.data()));
  assert(!g_pick13.cleanupSeen&&g_free_hold19c4r2!=token&&drops==0&&transfers==0&&retiredReads19c4==0);
  assert(count("FINISH_CLEANUP_BOUND")==1&&count("WAIT_UNMATCHED_NATIVE_CLEANUP")==1);
  // A release after the diagnostic bound does not bypass unknown continuation.
  frame(6020,0);TickOwner(controller.data(),6020);
  assert(drops==0&&transfers==0&&retiredReads19c4==0&&!g_pick13.cleanupSeen);
  forbidRetiredRead19c4=false;PickupEndDetour19c4(logic.data(),0,0);TickOwner(controller.data(),6040);
  assert(drops==1&&g_owner.state==Idle&&!g_native_fault13);
  // Quarantine remains while exactly held. A known empty current hand settles
  // bookkeeping only; no Drop/transfer/cleanup is retried.
  reset();attach();put(entity.data(),0x58,std::numeric_limits<float>::quiet_NaN());lines.clear();
  frame(1020,0);TickOwner(controller.data(),1020);assert(g_native_fault13&&drops==0);
  for(unsigned t=1040;t<=5000;t+=20){frame(t);TickOwner(controller.data(),t);}
  assert(g_native_fault13&&count("QUARANTINE state=HELD")==1&&drops==0);
  DetachLeaf(manager.data(),0);frame(5020);TickOwner(controller.data(),5020);
  assert(!g_native_fault13&&g_owner.state==Idle&&drops==0&&count("QUARANTINE state=SETTLED")==1);
  // Failed old-hand reads plus a new character are UNKNOWN, not a world boundary.
  reset();attach();GraspFault13("UNKNOWN_WORLD_FIXTURE");
  static std::array<unsigned char,0x1100> otherCharacter{};otherCharacter.fill(0);
  testSkel=otherCharacter.data()+0x110;put(testSkel,0x4e8,static_cast<void*>(otherCharacter.data()));
  put(contexts.data(),kScopeContextChar,static_cast<void*>(otherCharacter.data()));
  put(manager.data(),0,std::uintptr_t(0));
  frame(4000);TickOwner(controller.data(),4000);
  assert(g_native_fault13&&g_owner.state!=Idle&&drops==0&&transfers==0);
  // Active or unobserved outer callbacks still own cleanup even with empty hand.
  for(int uncertain=0;uncertain<3;++uncertain){reset();attach();GraspFault13("OUTER_FIXTURE");DetachLeaf(manager.data(),0);
   const auto t=g_pick13.bind.token;
   if(uncertain==0)g_outer_callbacks19c4.store((t<<16)|1);
   if(uncertain==1)g_outer_unobserved19c4.store(t);
   if(uncertain==2)g_outer_callbacks19c4.store((t+1)<<16);
   TickOwner(controller.data(),1020);assert(g_native_fault13&&g_owner.state!=Idle&&drops==0);}
  // Current exact live speed lease must restore successfully before settlement.
  reset();attach();GraspFault13("RESTORE_FIXTURE");DetachLeaf(manager.data(),0);
  refusedWrite19=action.data()+kActSpeedBias;TickOwner(controller.data(),1020);
  assert(g_native_fault13&&g_owner.state!=Idle&&Rd<float>(action.data(),kActSpeedBias,-1)==0);
  refusedWrite19=nullptr;TickOwner(controller.data(),1040);
  assert(!g_native_fault13&&g_owner.state==Idle&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f&&drops==0);
  // Another speed writer is preserved, and an unknown/replaced root is not read
  // through merely because the old pointer is still accessible.
  reset();attach();GraspFault13("FOREIGN_SPEED_FIXTURE");DetachLeaf(manager.data(),0);
  put(action.data(),kActSpeedBias,.75f);TickOwner(controller.data(),1020);
  assert(!g_native_fault13&&g_owner.state==Idle&&Rd<float>(action.data(),kActSpeedBias,-1)==.75f);
  reset();attach();GraspFault13("ROOT_LOST_FIXTURE");DetachLeaf(manager.data(),0);
  put(scope.data(),kScopeAction,static_cast<void*>(nullptr));forbidRetiredRead19c4=true;
  TickOwner(controller.data(),1020);assert(g_native_fault13&&g_owner.state!=Idle&&retiredReads19c4==0);
  forbidRetiredRead19c4=false;
  std::puts("PASS C4R4C1 reviewed C25: timeout is diagnostic only; exact quarantine settlement; unknown world/outer/root retained; speed lease compare-owned");
 }
 // C62 real preference/owner graph: CSV is not an authority switch; native
 // mode forwards the original event, staged hold still waits for real Attach.
 {
  ng13::Binding bound{};
  beginUse55();SetPickupHold62(false);installPicking55();UpdateDetour(controller.data(),.02f);
  assert(!NativeGraspEnabled()&&g_owner.state==Idle&&!ReadGraspBinding(bound));
  attach();assert(eventCalls==1&&transfers==1&&!g_pick13.attachStarted&&notices==1);
  beginUse55();SetOwnedFragments("");installPicking55();UpdateDetour(controller.data(),.02f);
  assert(NativeGraspEnabled()&&g_owner.state==Playing&&attaches==0&&transfers==0);
  attach();assert(attaches==1&&transfers==0&&ReadGraspBinding(bound));
  // Preference-off revokes render immediately; exact native cleanup/drop runs
  // once on its owner callback. Late callbacks cannot revive the old Use.
  SetPickupHold62(false);assert(!ReadGraspBinding(bound)&&!WristOwnedByController(0));
  frame(1040);TickOwner(controller.data(),1040);TickOwner(controller.data(),1040);
  assert(drops==1&&cleanup==1&&transfers==0&&g_owner.state==Idle);
  const int afterDrop=attaches;SetPickupHold62(true);attach();TickOwner(controller.data(),1040);
  assert(attaches==afterDrop&&transfers==0&&drops==1);
  // Pending native attachment cancels by existing End/current-hand cleanup,
  // not a fabricated Drop or inventory insertion.
  reset();deferAttachment=true;attach();SetPickupHold62(false);TickOwner(controller.data(),1001);TickOwner(controller.data(),1001);
  assert(cleanup==1&&drops==0&&transfers==0&&g_owner.state==Idle&&!g_native_fault13);
  // OFF->ON cannot adopt the previous request even if its old root is installed.
  beginUse55();SetPickupHold62(false);SetPickupHold62(true);installPicking55();UpdateDetour(controller.data(),.02f);
  assert(g_owner.state==Idle&&attaches==0&&transfers==0);
  // Explicit armed F5 rungs2/3 independently request the same proven hold.
  for(int rung:{1,2,3})for(bool armed:{false,true}){
   beginUse55();SetPickupHold62(false);
   kcd2vr::combat_rules178r10::f5Rung=rung;kcd2vr::workstation178r20c1::experimentArmed36=armed;
   SetPickupExperiment62(armed);frame(1001);UpdateDetour(controller.data(),.02f);frame(1002);NoteUseSent(0,1002);
   installPicking55();UpdateDetour(controller.data(),.02f);
   const bool effective=armed&&(rung==2||rung==3);
   assert(NativeGraspEnabled()==effective&&g_owner.state==(effective?Playing:Idle));
   if(effective){
    attach();assert(attaches==1&&transfers==0&&ReadGraspBinding(bound));
    SetPickupExperiment62(false);assert(!ReadGraspBinding(bound));frame(1040);TickOwner(controller.data(),1040);TickOwner(controller.data(),1040);
    assert(drops==1&&cleanup==1&&transfers==0&&g_owner.state==Idle);
   }else assert(attaches==0&&transfers==0);
  }
  // H17: the old `mode=2 ready=1` refusal was not proof of an enabled
  // pickup request. Reproduce that exact disabled experiment on a real native
  // animation-job callback, then enable the same public request and exercise
  // actual original-stack Attach -> controller adoption -> native Drop.
  reset();ResetOwner();SetPickupHold62(false);SetPickupExperiment62(false);testAttachManager=manager.data();
  kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::workstation178r20c1::experimentArmed36=false;
  traceEnabled=true;cap::gate.select(cap::Domain::Interaction,true,1000);lines.clear();
  std::thread unarmedF5H17([]{pickupThread19c4=17817;attach();});unarmedF5H17.join();
  assert(g_mode==Own&&g_native_ready13&&!NativeGraspEnabled()&&transfers==1&&!g_pick13.attachStarted);
  assert(lines.size()==1&&lines.back().find("jobQualifier=NATIVE_GRASP_DISABLED_OR_FAULTED")!=std::string::npos);
  traceEnabled=false;cap::gate.select(cap::Domain::Interaction,false,1000);
  reset();ResetOwner();SetPickupHold62(false);testAttachManager=manager.data();
  kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::workstation178r20c1::experimentArmed36=true;
  SetPickupExperiment62(true);frame(1001);NoteUseSent(0,1001);
  assert(NativeGraspEnabled());
  std::thread armedF5H17([]{pickupThread19c4=27817;attach();});armedF5H17.join();
  assert(attaches==1&&transfers==0&&g_owner.state==PendingAttach);
  g_update_original=UpdateActionLeaf;
  frame(1020);UpdateDetour(controller.data(),.02f);assert(g_owner.state==Held);
  frame(1040,0);UpdateDetour(controller.data(),.02f);UpdateDetour(controller.data(),.02f);
  assert(drops==1&&cleanup==1&&transfers==0&&g_owner.state==Idle);
  SetPickupExperiment62(false);
  kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::experimentArmed36=false;
  reset();SetMode(Observe);SetPickupExperiment62(true);kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::workstation178r20c1::experimentArmed36=true;
  assert(!NativeGraspEnabled()); // explicit ActionOwner comparison always wins
  SetPickupExperiment62(false);kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::experimentArmed36=false;
  std::puts("PASS C62 NATIVE/STAGED HOLD real adoption+Attach graph, once-only pending/held cleanup, stale Use retirement and explicit armed F5 override");
 }
 // C63: physical Pickable use owns THIS request even with old VrGrabMode=0;
 // normal native A still has no transported VR token and remains inventory pickup.
 {
  beginUse55();SetPickupHold62(false);SetPickupExperiment62(false);frame(1001,1);
  PreparePhysicalPickup63(true,0,1001);UpdateDetour(controller.data(),.02f);frame(1002,1);
  assert(NativeGraspAvailable63()&&!NativeGraspEnabled());
  NotePhysicalPickupUse63(0,1002,72);Use13 forced{};g_use13.read(forced);assert(forced.physicalPickup63&&NativeGraspEnabled());
  installPicking55();UpdateDetour(controller.data(),.02f);assert(g_owner.state==Playing);
  attach();assert(attaches==1&&transfers==0);frame(1040,0);TickOwner(controller.data(),1040);
  assert(drops==1&&g_owner.state==Idle&&!NativeGraspEnabled());
  beginUse55();SetPickupHold62(false);SetPickupExperiment62(false);frame(1001,1);
  PreparePhysicalPickup63(true,0,1001);UpdateDetour(controller.data(),.02f);assert(g_owner.state==Idle&&!NativeGraspEnabled());
  frame(1002,1);NotePhysicalPickupUse63(0,1002,72);installPicking55();UpdateDetour(controller.data(),.02f);
  assert(g_owner.state==Playing&&attaches==0&&transfers==0);
  frame(1040,0);TickOwner(controller.data(),1040);assert(g_owner.state==Idle&&attaches==0&&transfers==0&&finishes==1);
  beginUse55();SetPickupHold62(false);SetPickupExperiment62(false);frame(1002,1);
  // A native pickup after the absent VR route is unchanged.
  installPicking55();UpdateDetour(controller.data(),.02f);attach();assert(transfers==1&&!g_pick13.attachStarted&&g_owner.state==Idle);
  std::puts("PASS C63 per-request deliberate VR Pickable hold survives VrGrabMode0; original native A untouched; actual Attach then once-only Drop");
 }
 #include "alchemy_stage19c4_cases.inl"
 #include "weapon_drop_t1_cases.inl"
 // Failed disable retains ALL callbacks' native originals and prevents a
 // second Install from replacing the remaining reachable inline hook.
 reset();ResetOwner();g_installed=true;
 g_update_hook.emplace();g_queue_hook.emplace();
 for(auto& h:g_native_hooks13)h.emplace();
 g_native_hooks13[5]->failDisable=true;
 Uninstall();assert(Installed()&&!NativeGraspEnabled()&&Mode()==Off);
 assert(g_update_hook&&g_queue_hook&&g_native_hooks13[0]&&g_native_hooks13[5]);
 assert(g_attachment_world13==&WorldGetterLeaf&&g_pick_event13==&EventLeaf&&g_pick_transfer13==&TransferLeaf);
 ng13::Pose stillNative{};assert(AttachmentWorldDetour13(attachment.data(),&stillNative)==&stillNative);
 assert(stillNative.p[0]==nativePose.p[0]);
 const int beforeRetainedEvent=eventCalls;attach();assert(eventCalls==beforeRetainedEvent+1&&transfers==1);
 assert(!Install()&&g_native_hooks13[5]&&g_attachment_world13==&WorldGetterLeaf&&g_pick_transfer13==&TransferLeaf);
 // Once the retained hook really disarms, a repeated cleanup may clear the set.
 g_native_hooks13[5]->failDisable=false;Uninstall();assert(!Installed()&&!g_update_hook&&!g_queue_hook);
 for(const auto& h:g_native_hooks13)assert(!h);
 assert(!g_attachment_world13&&!g_pick_event13&&!g_pick_transfer13);
 std::puts("PASS complete production native-grasp owner: attach without storage, indefinite hold, current entity pose drop, native transfer once, original cleanup, duplicate/foreign/failure guards; native leaves substituted");return 0;
}
}}
int main(){kcd2vr::collab_h21::setThrowing(true);return kcd2vr::action_time_owner59c11::scenario();}

// Retained whole-owner fixture: this combat-only observer is an unexercised
// native animation leaf. Refuse it; do not synthesize an admitted paired clip.
namespace kcd2vr::animation_filter {bool AdmittedNativeClipOnlyD72(void*,std::uint64_t,int) noexcept{return false;}}
