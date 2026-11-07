// Actual production return adapter and actual native binding reader. Native
// memory, original calls and the tracking publication are explicit doubles.
#include "../../../src/subsystems/alchemy/native_return67.h"
#include "../../../src/vrik/minigame_hands45.h"
#include "../../../src/vrik/alchemy_work_clock_d47.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
static unsigned checks=0;
static void ck(bool b,const char* message){++checks;if(!b){std::fprintf(stderr,"FAIL %u %s\n",checks,message);std::exit(1);}}
namespace kcd2vr::activity_input163 {
enum class Kind175 {Alchemy=0};
struct Witness {std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};
 std::atomic<std::uint64_t> lifetime175{7};std::atomic<std::uintptr_t> active[2]{};} witness175;
}
static std::uint64_t nativeClockR96=1000,frameReadClockR96=0;
static std::uint64_t GetTickCount64(){return nativeClockR96;}
namespace kcd2vr::frik_pose_probe {
static workstation_sample46::Frame frame;static bool sourceOk=true;
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {if(frameReadClockR96)nativeClockR96=frameReadClockR96;out=frame;return sourceOk;}
}
namespace kcd2vr::workstation178r20c1 {static bool fullVr=true;bool fullVrAlchemy63(std::uint64_t) noexcept{return fullVr;}}
namespace retail_build112 {static bool good=true;bool Ready(std::uintptr_t) noexcept{return good;}}
static std::atomic<std::uintptr_t> g_base{0x10000000};static std::atomic<void*> g_henry_ctrl{};
enum {Own=2,Idle=0};static std::atomic<int> g_mode{Own};static struct {int state=Idle;} g_owner;
constexpr std::ptrdiff_t kCtrlEntityId=0x30,kCtrlScopeContexts=0x28,kScopeContextChar=0x10,kCtrlContext=0x20;
constexpr std::ptrdiff_t kActStatus=0x28,kActContext=8,kActToken=0x54;
constexpr std::ptrdiff_t kActSpeedBias=0x5C,kActActiveTime=0x58;
constexpr std::ptrdiff_t kCtrlScopeCount=0x34,kCtrlScopes=0x38,kScopeAction=0x148,kScopeExiting=0x150;
constexpr std::size_t kScopeStride=0x168;constexpr unsigned kMaxScopes=64;
static bool ownerThread=true,henry=true,pins=true,restore=true,oldRestore=true;
static bool OnOwnerThread13(){return ownerThread;}static bool OnAlchemyUpdateOwnerR27(){return ownerThread;}static bool IsHenryController(void*){return henry;}
static bool MatchBytes(std::uintptr_t,const unsigned char*,std::size_t){return pins;}
static bool DiagOn(){return true;}static unsigned receipts=0;
namespace diagnostic_trace {void Write(const char*){++receipts;}}
static std::array<unsigned char,0x1000> memory_instance{},memory_player{},memory_attachment{},memory_attachmentManager{},memory_object{},memory_parent{},memory_child{},memory_controller{},memory_itemManager{},memory_scopeContexts{},memory_scope{};
static std::array<std::uintptr_t,2> slots{};static std::array<std::uintptr_t,1> queue{};
static const char leftName[]="entity_left";static std::vector<std::pair<const void*,std::size_t>> regions;
static bool SehCopy(const void* p,void* out,std::size_t n) noexcept {
 if(reinterpret_cast<std::uintptr_t>(p)==g_base.load()+0x40AC2A0&&n==sizeof(const char*)){const char* name=leftName;std::memcpy(out,&name,n);return true;}
 const auto a=reinterpret_cast<std::uintptr_t>(p);for(const auto& r:regions){const auto b=reinterpret_cast<std::uintptr_t>(r.first);
  if(a>=b&&n<=r.second&&a-b<=r.second-n){std::memcpy(out,p,n);return true;}}return false;
}
template<class T>T Rd(const void* p,std::ptrdiff_t off,T fallback){T out{};return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&out,sizeof(out))?out:fallback;}
template<class T>bool Wr(void* p,std::ptrdiff_t off,T value){T old{};if(!SehCopy(static_cast<unsigned char*>(p)+off,&old,sizeof(old)))return false;std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));return true;}
template<class T>static void put(void* p,std::size_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
static void* FindRootScope(void*,void* c){return Rd<std::uintptr_t>(memory_scope.data(),kScopeAction,0)==reinterpret_cast<std::uintptr_t>(c)?memory_scope.data():nullptr;}
namespace semi46 {enum {Resource=3};struct Work {std::uintptr_t instance=0,parent=0,child=0;std::uint64_t lifetime=0;std::uint32_t token=0;unsigned kind=Resource;};
struct Control {bool enabled=true,manualUse=false;std::uintptr_t instance=0,parent=0,child=0;std::uint64_t lifetime=0,tick=0,sequence=0,poseEpoch=0,sourceEpoch=0;std::uint32_t token=0;double totalSeconds=0;};
static kcd2vr::core::SnapshotMailbox<Control> control;}
static struct {bool written=false;semi46::Work owner{};float saved=1.f,lastWritten=0,previousWritten=1.f;bool pendingNoted=false,writeUncertain=false;std::uint64_t consumed=0;double consumedSeconds=0;kcd2vr::alchemy_work_clock_d47::Ledger clock{};} g_semi_alchemy46;
static std::atomic<bool> g_semi_pending46{false};
static struct {bool written=false;struct {kcd2vr::alchemy_tool18::Binding binding{};} proof;} g_alchemy_stage19c4;
static unsigned restores=0,oldRestores=0;
static bool SemiAlchemyRestore46(const char*){++restores;if(!restore)return false;
 if(g_semi_alchemy46.written)Wr<float>(memory_child.data(),kActSpeedBias,g_semi_alchemy46.saved);
 g_semi_alchemy46.written=false;g_semi_pending46=false;return true;}
static bool RestoreAlchemyStage19c4(const char*){++oldRestores;if(!oldRestore)return false;g_alchemy_stage19c4.written=false;return true;}
#define KCD2VR_GESTURE_RETURN67_HOST_TEST
#include "../../../src/subsystems/alchemy/native_return67.inl"
static void (*cancelEffectH21)()=nullptr; // explicit synchronous native Cancel leaf side effect
static unsigned cancels=0,detaches=0,homes=0;static int behavior=0;static bool nested=false;
static std::vector<unsigned> order;
bool HostAlchemyCancel67(void* director) noexcept {
 ++cancels;order.push_back(1);ck(director==memory_instance.data()+0x78,"exact director receiver");
 ck(!g_semi_alchemy46.written&&!g_alchemy_stage19c4.written,"owned clock restored before native cancellation");
 if(nested)TickGestureAlchemyReturn67(memory_controller.data(),1000);
 if(behavior==1)return false;if(behavior==2)return true;
 slots[1]=0;
 if(behavior!=7){put(memory_scope.data(),kScopeAction,std::uintptr_t(0));queue[0]=0;}
 if(behavior==3){put(memory_attachment.data(),0x20,std::uintptr_t(0));put(memory_itemManager.data(),0x4C,0);put(memory_itemManager.data(),0x50,std::uint32_t(0));}
 if(behavior==4)put(memory_attachment.data(),0x20,reinterpret_cast<std::uintptr_t>(memory_child.data()));
 if(behavior==5){slots[1]=reinterpret_cast<std::uintptr_t>(memory_object.data());put(memory_object.data(),0,g_base.load()+0x3A5C670);}
 if(behavior==6)kcd2vr::activity_input163::witness175.lifetime175++;
 if(cancelEffectH21)cancelEffectH21();
 return true;
}
bool HostAlchemyDetach67(void* manager,const char* name) noexcept {
 ++detaches;order.push_back(2);ck(manager==memory_itemManager.data()&&std::strcmp(name,leftName)==0,"exact manager and actual native memory_attachment name");
 if(behavior==8)return false;
 put(memory_attachment.data(),0x20,std::uintptr_t(0));put(memory_itemManager.data(),0x4C,0);put(memory_itemManager.data(),0x50,std::uint32_t(0));return true;
}
bool HostAlchemyHome67(void* target,int type) noexcept {
 ++homes;order.push_back(3);ck(target==memory_instance.data()&&type==14,"exact native home owner/type");
 ck(Rd<std::uintptr_t>(memory_attachment.data(),0x20,1)==0,"home dispatch follows actual native detachment");return behavior!=9;
}
namespace ret=kcd2vr::subsystems::alchemy::return67;
static ret::Request request67;
static void prepare(){
 nativeClockR96=1000;frameReadClockR96=0;
 for(auto* p:{&memory_instance,&memory_player,&memory_attachment,&memory_attachmentManager,&memory_object,&memory_parent,&memory_child,&memory_controller,&memory_itemManager,&memory_scopeContexts,&memory_scope})p->fill(0);
 regions.clear();for(auto* p:{&memory_instance,&memory_player,&memory_attachment,&memory_attachmentManager,&memory_object,&memory_parent,&memory_child,&memory_controller,&memory_itemManager,&memory_scopeContexts,&memory_scope})regions.push_back({p->data(),p->size()});
 regions.push_back({slots.data(),sizeof(slots)});regions.push_back({queue.data(),sizeof(queue)});regions.push_back({leftName,sizeof(leftName)});
 g_base.fetch_add(0x10000000);const auto b=g_base.load();g_henry_ctrl=memory_controller.data();g_mode=Own;g_owner.state=Idle;ownerThread=henry=pins=restore=oldRestore=true;retail_build112::good=true;
 kcd2vr::workstation178r20c1::fullVr=true;kcd2vr::frik_pose_probe::sourceOk=true;
 kcd2vr::minigame_hands45::alchemyMode67=kcd2vr::minigame_hands45::Gesture;kcd2vr::minigame_hands45::modeEpoch67=1;
 auto& f=kcd2vr::frik_pose_probe::frame;f=kcd2vr::workstation_sample46::Frame{};f.allowed=f.headValid=true;f.tick=1000;f.sequence=10;f.timeNs=1000000000;f.poseEpoch=20;f.sourceEpoch=30;f.hand[1].valid=true;
 auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=7;w.active[0]=reinterpret_cast<std::uintptr_t>(memory_instance.data());
 put(memory_instance.data(),0,b+0x3F60B08);put(memory_instance.data(),0x2C,(unsigned char)1);put(memory_instance.data(),0x18,reinterpret_cast<std::uintptr_t>(memory_player.data()));put(memory_instance.data(),0x28,std::uint32_t(100));
 put(memory_instance.data(),0x78,b+0x3BFA5B0);slots={0,reinterpret_cast<std::uintptr_t>(memory_parent.data())};put(memory_instance.data(),0xB0,reinterpret_cast<std::uintptr_t>(slots.data()));put(memory_instance.data(),0xB8,reinterpret_cast<std::uintptr_t>(slots.data()+2));
 put(memory_instance.data(),0x304,std::uint32_t(14));put(memory_instance.data(),0x150+4*14,std::uint32_t(500));put(memory_instance.data(),0x2A4,std::uint32_t(21));
 put(memory_player.data(),0,b+0x3AA4B98);put(memory_player.data(),0x9D0,memory_itemManager.data());put(memory_player.data(),0x30,std::uint32_t(99));
 put(memory_itemManager.data(),0,b+0x3EC9958);put(memory_itemManager.data(),0x38,memory_player.data());put(memory_itemManager.data(),0x4C,1);put(memory_itemManager.data(),0x50,std::uint32_t(500));
 put(memory_attachment.data(),0,b+0x3A657C0);put(memory_attachment.data(),0x10,leftName);put(memory_attachment.data(),0x20,reinterpret_cast<std::uintptr_t>(memory_object.data()));put(memory_attachment.data(),0x28,reinterpret_cast<std::uintptr_t>(memory_attachmentManager.data()));
 put(memory_attachmentManager.data(),0x70,reinterpret_cast<std::uintptr_t>(memory_scopeContexts.data()));
 put(memory_object.data(),0,b+0x3A68F68);put(memory_object.data(),8,std::uint32_t(500));
 put(memory_parent.data(),0,b+0x3A5C670);put(memory_parent.data(),0x58,reinterpret_cast<std::uintptr_t>(memory_instance.data()));put(memory_parent.data(),0x40,reinterpret_cast<std::uintptr_t>(memory_instance.data()+0x78));put(memory_parent.data(),0x28,(unsigned char)1);put(memory_parent.data(),0x29,(unsigned char)0);put(memory_parent.data(),0x3C,1);put(memory_parent.data(),0x78,reinterpret_cast<std::uintptr_t>(memory_child.data()));
 put(memory_child.data(),0,b+0x3A47948);put(memory_child.data(),0x268,reinterpret_cast<std::uintptr_t>(memory_instance.data()));put(memory_child.data(),kActStatus,2);put(memory_child.data(),kActToken,std::uint32_t(77));put(memory_child.data(),kActContext,memory_controller.data()+0x200);
 put(memory_controller.data(),kCtrlEntityId,std::uint32_t(99));put(memory_controller.data(),kCtrlContext,memory_controller.data()+0x200);put(memory_controller.data(),kCtrlScopeContexts,memory_scopeContexts.data());put(memory_scopeContexts.data(),kScopeContextChar,reinterpret_cast<std::uintptr_t>(memory_scopeContexts.data()));
 put(memory_controller.data(),kCtrlScopeCount,unsigned(1));put(memory_controller.data(),kCtrlScopes,memory_scope.data());put(memory_scope.data(),kScopeAction,reinterpret_cast<std::uintptr_t>(memory_child.data()));queue[0]=0;
 put(memory_controller.data(),0x48,reinterpret_cast<std::uintptr_t>(queue.data()));put(memory_controller.data(),0x50,reinterpret_cast<std::uintptr_t>(queue.data()));
 request67={};auto& a=request67.binding;a={reinterpret_cast<std::uintptr_t>(memory_instance.data()),reinterpret_cast<std::uintptr_t>(memory_player.data()),reinterpret_cast<std::uintptr_t>(memory_scopeContexts.data()),reinterpret_cast<std::uintptr_t>(memory_attachment.data()),500,1,14};a.action=reinterpret_cast<std::uintptr_t>(memory_parent.data());a.actionClass=b+0x3A5C670;a.object=reinterpret_cast<std::uintptr_t>(memory_object.data());a.lifetime=7;
 request67.child=reinterpret_cast<std::uintptr_t>(memory_child.data());request67.token=77;request67.serial=100;request67.tick=1000;request67.poseEpoch=20;request67.sourceEpoch=30;request67.modeEpoch=1;
 semi46::Control c{};c.instance=a.instance;c.parent=a.action;c.child=request67.child;c.lifetime=7;c.tick=1000;c.sequence=10;c.poseEpoch=20;c.sourceEpoch=30;c.token=77;c.totalSeconds=.1;semi46::control.publish(c);
 g_alchemy_return67={};g_alchemy_return_retired67={};g_semi_alchemy46={};g_semi_alchemy46.written=true;g_semi_alchemy46.owner={a.instance,a.action,request67.child,7,77,semi46::Resource};g_alchemy_stage19c4={};
 cancels=detaches=homes=restores=oldRestores=receipts=0;behavior=0;nested=false;cancelEffectH21=nullptr;order.clear();
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,1000);
 ret::request.publish(request67);ret::result.publish({});ret::interested=true;
}
static ret::Result result(){ret::Result r{};ret::result.read(r);return r;}
static void tick(std::uint64_t now=1000){TickGestureAlchemyReturn67(memory_controller.data(),now);}
int main(){
 prepare();tick();ck(cancels==1&&detaches==1&&homes==1&&restores==1,"one native cancel/detach/home with restore");ck(result().status==ret::Status::NativeReturnDispatched,"home dispatch receipt");ck(order==std::vector<unsigned>({1,2,3}),"native call order");tick();ck(cancels==1&&homes==1,"retired request cannot replay");
 prepare();behavior=3;tick();ck(cancels==1&&detaches==0&&homes==1,"native cancel already detached, no duplicate detach");
 prepare();behavior=4;tick();ck(detaches==0&&homes==0&&result().status==ret::Status::ForeignAttachment,"foreign memory_attachment after cancel untouched");
 prepare();behavior=5;tick();ck(detaches==0&&homes==0&&result().status==ret::Status::ForeignAttachment,"new resource memory_parent cannot borrow old hold");
 prepare();behavior=6;tick();ck(detaches==0&&homes==0&&result().status==ret::Status::NativeOwnerEnded,"lifetime changed inside native cancellation");
 prepare();behavior=2;tick();ck(cancels==1&&homes==0&&g_semi_alchemy46.written&&g_semi_pending46&&Rd<float>(memory_child.data(),kActSpeedBias,-1.f)==0.f&&result().status==ret::Status::CancelRefusedPausedNativeA,"refused cancellation keeps unconsumed work paused with explicit Native-A recovery");tick();ck(cancels==1,"native cancellation not retried after refusal");
 ck(g_semi_alchemy46.consumed==10&&g_semi_alchemy46.consumedSeconds==.1,"abort drops outstanding ledger without advancing native Use");
 ck(SemiAlchemyRestore46("NATIVE_A_RECOVERY")&&!g_semi_alchemy46.written&&Rd<float>(memory_child.data(),kActSpeedBias,-1.f)==1.f,"same compare-owned pause permits native manual recovery");
 prepare();behavior=1;tick();ck(cancels==1&&homes==0&&result().status==ret::Status::FaultQuarantined,"faulted cancellation quarantined");tick();ck(cancels==1,"fault never automatically retries");
 prepare();behavior=8;tick();ck(detaches==1&&homes==0&&result().status==ret::Status::FaultQuarantined,"detach exception prevents home call");
 prepare();behavior=9;tick();ck(homes==1&&result().status==ret::Status::FaultQuarantined,"home exception recorded without fake completion");
 prepare();restore=false;tick();tick();ck(cancels==0&&g_semi_alchemy46.written&&receipts==1,"restore failure retains existing owner, one sparse receipt");restore=true;tick();ck(cancels==1&&homes==1,"native restore acknowledgement allows local return");
 prepare();g_alchemy_stage19c4.written=true;g_alchemy_stage19c4.proof.binding=request67.binding;tick();ck(oldRestores==1&&homes==1,"matching older stage lease restored too");
 prepare();g_alchemy_stage19c4.written=true;g_alchemy_stage19c4.proof.binding=request67.binding;oldRestore=false;behavior=2;
 tick();ck(!cancels&&!g_semi_alchemy46.written&&g_alchemy_return67.savedSemi.written,"old-stage restore delay retains already-restored semi proof");
 oldRestore=true;tick();ck(g_semi_alchemy46.written&&result().status==ret::Status::CancelRefusedPausedNativeA,"delayed refusal can still recover original compare-owned pause");
 prepare();g_semi_alchemy46.owner.token++;tick();ck(cancels==0&&restores==0&&result().status==ret::Status::Rejected,"foreign speed lease not restored");
 prepare();nested=true;tick();ck(cancels==1&&homes==1,"native callback reentry cannot double execute");
 prepare();behavior=7;tick();ck(homes==0&&result().status==ret::Status::WaitingChildRetired,"memory_parent gone alone does not prove memory_child ended");put(memory_scope.data(),kScopeAction,std::uintptr_t(0));tick(1010);ck(cancels==1&&homes==1,"memory_child removal acknowledgement permits cleanup without repeat cancellation");
 prepare();behavior=7;tick();tick(3101);ck(homes==0&&result().status==ret::Status::CancelNotAcknowledged,"never-retired memory_child gets explicit bounded refusal");
 prepare();request67.tick=919;ret::request.publish(request67);tick();ck(cancels==0,"stale original request rejected");
 prepare();request67.tick=1001;ret::request.publish(request67);tick();ck(cancels==0,"future request rejected");
 prepare();request67.poseEpoch++;ret::request.publish(request67);tick();ck(cancels==0,"recentered frame rejects old request");
 prepare();request67.sourceEpoch++;ret::request.publish(request67);tick();ck(cancels==0,"different tracking source rejected");
 prepare();kcd2vr::frik_pose_probe::frame.allowed=false;tick();ck(cancels==0,"menu/context tracking revocation rejected");
 prepare();kcd2vr::frik_pose_probe::frame.tick=919;tick();ck(cancels==0,"stale current tracking rejected");
 prepare();kcd2vr::minigame_hands45::modeEpoch67++;tick();ck(cancels==0,"changed saved mode generation rejected");
 prepare();ownerThread=false;tick();ck(cancels==0&&restores==0,"worker thread performs no native operations");
 prepare();g_mode=0;tick();ck(cancels==1&&homes==1,"saved Gesture return does not require legacy owner Own mode");
 prepare();g_mode=1;tick();ck(cancels==1&&homes==1,"saved Gesture return also works in legacy Observe mode");
 prepare();g_owner.state=1;tick();ck(cancels==0&&restores==0,"foreign active legacy transaction still blocks G1 native return");
 prepare();pins=false;tick();ck(cancels==0,"retail byte pin refusal");
 prepare();retail_build112::good=false;tick();ck(cancels==0,"retail fingerprint refusal");
 prepare();put(memory_itemManager.data(),0x4C,2);tick();ck(cancels==1&&detaches==0&&homes==0,"foreign memory_attachment reference count not decremented");
 prepare();put(memory_parent.data(),0x29,(unsigned char)1);tick();ck(cancels==0,"native already-finished flag respected");
 prepare();kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,1000);
 restore=false;tick();tick();ck(cancels==0&&g_semi_alchemy46.written&&receipts==0,"F5 off retains native cleanup ownership without diagnostic writes");
 std::printf("native_return67: %u checks PASS\n",checks);
 return 0;
}
