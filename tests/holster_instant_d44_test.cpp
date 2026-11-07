#include "core/native_grasp13.h"
#include "vrik/physical_weapon.h"
#include "vrik/holster_diag_r8d.h"
#include "vrik/bow_arm_aim_r9.h"
#include "diagnostics/capture168.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "vrik/quick_talk_owner_h4.h"
#include "vrik/weapon_release_t1.h"
#include <cstdio>
#include <array>
#include <atomic>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;namespace equipment114=kcd2vr::equipment114;
static std::uint64_t tickD44=1000;static std::uint64_t GetTickCount64(){return tickD44;}
struct PhysEquipmentItem6698{ph::Profile profile{};bool held=false,dropUnitT1=false;int set=0,slot126=0;};
struct PhysEquipment6698{std::array<PhysEquipmentItem6698,8>items{};int count=0,selected=0;bool valid=true,handKnown[2]{true,true};std::uint64_t handKey[2]{};void*player=(void*)123;};
struct GripD44{std::uint64_t key=0,generation=0;bool mountReady=false,ready[2]{};struct{std::uint64_t tick=0,epoch=0;}mountStamp126;ph::Pose mount{},hand[2]{};};
struct PhysGripBank6698{std::array<GripD44,8>grip{};};
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698>g_phys_grips6698;
static bool PhysSupplyHeldT2(const PhysEquipment6698&,const PhysEquipmentItem6698& item,int side){return side==1&&item.held&&item.profile.classId==0&&item.profile.key==777;}
static bool nativeBusyD48=false;static bool NativeHeldBusyT4(){return nativeBusyD48;}static bool PhysLeftHolsterRetiredH36(const ph::Input&,const PhysEquipment6698&){return true;}
static std::atomic<bool>g_native_held_release_inhibit_t4{false};static std::atomic<void*>g_committed_character6689{nullptr};
static bool claimD44[2]{};static bool HandClaimBlockedT24(int s,std::uintptr_t,std::uint64_t,std::uint64_t){return claimD44[s];}
namespace heldT4{struct Owner{std::uint64_t key=0,generation=0,epoch=0,request=0;int side=0;unsigned entityId=0;};static Owner ownerD48{};static bool ReadRetainedOwner(Owner& o){o=ownerD48;return nativeBusyD48;}}
namespace kcd2vr::haptics101{inline std::atomic<bool>enabled{false},allowed{false};inline std::atomic<unsigned>epoch{1};struct Pulse{std::uint64_t epoch,sample,tick,key;float strength;unsigned ms;};struct Queue{bool publish(Pulse){return true;}};inline Queue queue[2];}
namespace kcd2vr::visible_failure{inline constexpr unsigned kMaxText=128;static bool publish(const char*,std::uint64_t){return true;}}
namespace kcd2vr::diagnostic_trace{static void Write(const char*)noexcept{}}
namespace kcd2vr::hand_only_door_h40{static unsigned reservedD44=0,enteredD48=0;static unsigned Reserved(){return reservedD44;}static unsigned Entered(){return reservedD44&enteredD48;}static unsigned Acquired(){return reservedD44&enteredD48;}}
namespace kcd2vr::ammo_service_t22{inline bool integrated=true,leftChestKnivesT26=true;}
namespace kcd2vr::action_time_owner59c11{static bool ThrowingSupplyT2(){return false;}}
namespace kcd2vr::inventory_lifecycle_t8{inline std::atomic<int>daggers{0},axes{0};}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;namespace melee118=kcd2vr::melee118;
#include "subsystems/throw_drop/holster_gestures.inl"
#include "subsystems/throw_drop/holster_commit_d48.inl"
static unsigned total=0,failed=0,badDrawClaims=0;static void check(bool b,const char*s){++total;if(!b){++failed;std::printf("FAIL %s\n",s);}}
struct Case{PhysEquipment6698 gear{};ph::Input in{};ph::Output out{};std::array<ph::HolsterState,8>state{};unsigned draws=0;Case(){gear.count=1;auto&w=gear.items[0];w.profile.key=1234;w.profile.generation=2;w.profile.classId=9;w.profile.side=1;w.set=1;in.bodyEpoch=in.epoch=1;in.tick=tickD44;in.requirePrimaryGrip114=in.allowed=true;in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};ph::holster_mount126(in,w.profile,in.hands[1].p);in.grip[1]=1;}
void seed(){auto&st=state[0].comparison114;st.initialized=st.latched=true;st.key=gear.items[0].profile.key;st.generation=gear.items[0].profile.generation;st.epoch=in.epoch;st.tick=st.started=in.tick;st.origin={};kcd2vr::corpse_grip_cg1::claims.capture(3,state[0].acquisitionCg1);}
void step(unsigned coord=3,bool full=false){tickD44+=16;in.tick=tickD44;++in.sample;out=ph::Output{};if(full)out.commandCount=out.commands.size();kcd2vr::quick_talk_owner_h4::mailbox.publish({in.tick,in.bodyEpoch,in.epoch,(std::uintptr_t)gear.player,0});PhysHolsterGestures6698(in,gear,out,state,3,coord);for(unsigned i=0;i<out.commandCount;++i)if(out.commands[i].kind==ph::CommandKind::Draw){++draws;if(out.commands[i].corpseClaimCg1.mask!=3)++badDrawClaims;}}
};
int main(){
{Case c;c.step();check(c.draws==1,"already closed Grip at valid bow zone draws immediately without80ms or12cm");for(int i=0;i<20;++i)c.step();check(c.draws==1&&c.state[0].comparison114.pending,"pending native ACK prevents repeated draws");}
{Case c;c.in.trigger[1]=1;c.step();check(c.draws==0,"current Trigger+Grip is the fist exception even in bow zone");c.in.trigger[1]=0;c.step();check(c.draws==1,"releasing Trigger leaves ordinary Grip acquisition, no remembered fist blocker");}
{Case c;c.in.hands[1].p={0,.6,1.5};c.in.trigger[1]=1;c.step();check(c.draws==0,"forward combat fist cannot acquire bow");}
{Case c;c.in.intentTracked[1]=false;c.step();check(!c.draws,"lost intent tracking never draws");}
{Case c;c.in.allowed=false;c.step();check(!c.draws,"gameplay disallowed never draws");}
{Case c;c.gear.handKey[1]=999;c.step();check(!c.draws,"occupied grabbing hand never draws");}
{Case c;for(int i=0;i<30;++i)c.step(0);check(!c.draws&&!c.state[0].comparison114.pending,"busy dispatch does not consume or queue a request");c.step(3);check(c.draws==1,"still current held Grip draws once after admission returns");}
{Case c;claimD44[1]=true;c.step();claimD44[1]=false;c.step();check(!c.draws,"real competing object owner requires fresh Grip");c.in.grip[1]=0;c.step();c.in.grip[1]=1;c.step();check(c.draws==1,"release and new Grip after object retirement draws");}
{Case c;c.gear.count=2;auto&w=c.gear.items[1];w.profile.key=555;w.profile.generation=1;w.profile.classId=4;w.profile.side=0;w.set=0;w.held=true;c.gear.handKey[0]=555;c.step();check(!c.draws&&c.out.commandCount==0&&!c.state[0].switchHS2.active,"another held main weapon prevents acquisition without automatic stow/swap");}
{Case c;auto&st=c.state[0].comparison114;st.initialized=st.latched=true;st.key=1234;st.generation=2;st.epoch=1;st.tick=st.started=c.in.tick;st.origin={};kcd2vr::corpse_grip_cg1::claims.capture(3,c.state[0].acquisitionCg1);
 for(int i=0;i<30;++i)c.step(0);check(!c.draws&&st.latched&&!st.pending&&!st.requireReleaseR15,"accepted Grip survives 480ms dispatch contention without fake consumption");c.step(3);check(c.draws==1,"deferred accepted Grip draws once with current admission");}
{Case c;for(int i=0;i<20;++i)c.step(0);c.in.grip[1]=0;c.step(0);c.step(3);check(!c.draws,"released Grip cannot turn dispatch recovery into delayed draw");}
{Case c;kcd2vr::hand_only_door_h40::reservedD44=kcd2vr::hand_only_door_h40::enteredD48=2;c.step();check(!c.draws,"entered door owner prevents bow draw");kcd2vr::hand_only_door_h40::reservedD44=kcd2vr::hand_only_door_h40::enteredD48=0;c.in.grip[1]=0;c.step();c.in.grip[1]=1;c.step();check(c.draws==1,"new Grip after door release draws");}
{Case c;c.gear.items[0].held=true;c.gear.handKey[1]=1234;c.in.trigger[1]=1;c.step();check(!c.draws&&c.out.commandCount==0,"Trigger+Grip does not steal an already held bow for fists");}
{Case c;c.gear.items[0].profile.classId=3;c.gear.items[0].profile.side=0;c.gear.items[0].set=0;c.in.grip[0]=1;c.in.grip[1]=1;ph::holster_mount126(c.in,c.gear.items[0].profile,c.in.hands[0].p);c.gear.count=2;auto&w=c.gear.items[1];w.profile.key=888;w.profile.generation=1;w.profile.classId=12;w.profile.side=1;w.set=0;w.held=true;c.gear.handKey[1]=888;c.step();check(c.draws==1,"torch in left hand does not block selected right main weapon");check(c.out.commandCount==1,"drawing right main weapon does not stow left torch");}
{Case c;c.gear.items[0].profile.classId=3;c.gear.items[0].profile.side=0;c.gear.items[0].set=0;c.in.grip[0]=1;c.in.grip[1]=1;ph::holster_mount126(c.in,c.gear.items[0].profile,c.in.hands[0].p);c.gear.count=2;auto&w=c.gear.items[1];w.profile.key=777;w.profile.generation=1;w.profile.classId=0;w.profile.side=1;w.set=-1;w.held=true;c.gear.handKey[1]=777;c.step();check(c.draws==1,"dedicated knife in left hand does not block selected right main weapon");check(c.out.commandCount==1,"drawing right main weapon does not stow left knife");}
{Case c;c.in.trigger[1]=1;c.step();for(int n=0;n<5;++n)c.step();c.in.hands[1].p.x+=.16;c.step();check(c.draws==0,"actual fist chord cannot draw after legacy80ms/12cm pull either");}
{Case c;c.seed();c.step(0);c.in.intentTracked[1]=false;c.step(0);c.in.intentTracked[1]=true;c.step();check(!c.draws&&!c.state[0].comparison114.latched,"tracking loss retires accepted wait; recovery cannot replay stale draw");}
{Case c;c.seed();c.step(0);++c.gear.items[0].profile.generation;c.step();check(!c.draws&&!c.state[0].comparison114.latched,"item generation change retires accepted wait");}
{Case c;c.seed();c.step(0);++c.in.epoch;c.step();check(!c.draws&&!c.state[0].comparison114.latched,"input context epoch change retires accepted wait");}
{Case c;c.seed();c.step(0);tickD44+=150;c.step();check(!c.draws&&!c.state[0].comparison114.latched,"stale publication cannot replay accepted wait");}
{equipment114::State st;equipment114::Input in;in.key=1;in.generation=1;in.epoch=1;in.eligible=in.mountKnown=true;in.tick=1000;check(equipment114::update(st,in)==equipment114::Request::None,"legacy comparison release does not draw");in.grip=1;in.tick=1016;check(equipment114::update(st,in)==equipment114::Request::None,"legacy comparison still latches");in.hand.x=.14;in.tick=1064;check(equipment114::update(st,in)==equipment114::Request::None,"legacy comparison still requires80ms");in.tick=1096;check(equipment114::update(st,in)==equipment114::Request::Draw,"legacy comparison retains80ms/12cm outward pull");}
{Case c;kcd2vr::hand_only_door_h40::reservedD44=2;c.step();
 check(!c.draws&&c.state[0].comparison114.latched&&!c.state[0].comparison114.requireReleaseR15,"unentered door candidate defers but preserves current holster Grip");
 kcd2vr::hand_only_door_h40::reservedD44=0;c.step();check(c.draws==1,"candidate rejection permits same continuously held Grip");}
{Case c;c.step(3,true);check(!c.state[0].comparison114.pending,"full output cannot leave a phantom pending draw");c.step();check(c.draws==1,"unentered capacity refusal permits next current admission");}
{Case c;const auto before=g_holster_requests136.load();c.step();check(g_holster_requests136.load()==before,"staging a draw is not a committed request/confirmation");
 const auto command=c.out.commands[0];PhysHolsterEnqueuedD48(c.in,c.gear,c.state,command);check(g_holster_requests136.load()==before+1,"real enqueue alone commits request confirmation");
 c.in.grip[1]=0;check(!PhysCurrentDrawIntentD48(c.in,command),"consumer refuses a released unentered draw");
 check(PhysRejectHolsterLocalD48(command,c.in,c.gear,c.state)&&!c.state[0].comparison114.pending&&!c.state[0].comparison114.latched,"exact unentered rejection ends released intent");
 c.step();check(c.draws==1,"open hand cannot retry rejected draw");c.in.grip[1]=1;c.step();check(c.draws==2,"new explicit grip can acquire after exact refusal");}
{Case c;c.step();auto command=c.out.commands[0];++command.sample;++command.tick;check(!PhysRejectHolsterLocalD48(command,c.in,c.gear,c.state)&&c.state[0].comparison114.pending,"receipt for different original tick cannot clear pending draw");}
{Case c;c.step();c.in.grip[1]=0;c.step();check(c.state[0].comparison114.releasedDuringDraw,"release disposition survives pending native draw");
 c.gear.items[0].held=true;c.gear.handKey[1]=1234;c.step();check(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::Holster,"native ACK while hand is already open disposes once without resqueeze");
 c.step();check(c.out.commandCount==0,"pending disposition cannot emit duplicate native stow");}
{Case c;c.step();c.in.grip[1]=0;c.step();c.gear.items[0].held=true;c.gear.handKey[1]=1234;++c.gear.items[0].profile.generation;c.step();
 check(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::Holster,"initial native instance generation change preserves same-key released draw disposition");}
{Case c;c.in.hands[1].p.x+=.26;c.step();check(c.out.cueKey[1]==1234&&c.out.cueRadiusD48[1]>.27,"outer shoulder acquisition shell publishes the identical haptic radius");}
{Case c;auto& item=c.gear.items[0];item.profile.side=0;item.profile.classId=4;item.profile.entity=100;item.held=item.dropUnitT1=true;
 c.gear.handKey[0]=1234;c.in.grip[0]=1;nativeBusyD48=true;heldT4::ownerD48={1234,2,1,1,0,100};kcd2vr::weapon_release_t1::SetMode(1);
 c.step();check(c.out.commandCount==0,"retained native collider observes primary hold before release");
 c.in.grip[0]=0;c.step(3,true);check(!c.state[0].comparison114.pending,"T4 release with full output preserves unentered disposition");
 c.step();check(c.out.commandCount==1&&c.out.commands[0].kind==ph::CommandKind::DropT1,"T4 unknown holster mount uses same outside-holster drop rule");
 c.step();check(c.out.commandCount==0,"retained collider release emits only one native Drop intent");nativeBusyD48=false;kcd2vr::weapon_release_t1::SetMode(0);}
check(!badDrawClaims,"every emitted draw retains real occurrence qualification");
std::printf("D44 actual holster producer %u/%u checks; native calls NONE\n",total-failed,total);return failed?1:0;}
