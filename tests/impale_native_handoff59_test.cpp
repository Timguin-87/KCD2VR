#include "diagnostics/lab_state.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // C29: coordinator stand-down
#if defined(KCD2VR_LIVING_IMPALE_C40)
// Production physical step + native coordinator + exact new evaluated-anchor
// body/namespace capture. Engine reads/getters and OS clocks are explicit leaves.
#include "vrik/weapon_sweep112.h"
#include "vrik/part_anchor59c7g.h"
#include "vrik/part_namespace62.h"
#include "vrik/impale_hold60.h"
#include "subsystems/gore/impale_constraint_h21.h"
#include "vrik/visible_failure.h"
#include "vrik/blood49.h"
#include "vrik/kick_style26.h"
#include "vrik/contact_contract176.h"
#include "vrik/contact_authority176.h"
#include "subsystems/gore/impale_observe_h7.h"
#include "vrik/user_modes175.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;namespace ns=kcd2vr::part_namespace62;
using A=std::uintptr_t;
constexpr A base=0x10000000,entity=0x420,proxy=0x12000,ci=0x22000,model=0x42000,joints=0x52000,body=0x440,root=0x444;
static std::unordered_map<A,unsigned char> memory;
template<class T>void put(A p,const T& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(T);++i)memory[p+i]=b[i];}
template<class T>bool Read(void* p,T& v)noexcept{auto* out=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(T);++i){auto it=memory.find(A(p)+i);if(it==memory.end())return false;out[i]=it->second;}return true;}
struct Q4{float x{},y{},z{},w{1};};struct V3{float x{},y{},z{};};struct QuatT28{float q[4]{0,0,0,1};float t[3]{};};
using PoseGetFn=const QuatT28*(__fastcall*)(void*,int);struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;};
constexpr std::ptrdiff_t kPoseInCharInstanceOffset=0x8e0;
static ModelWorld6692 nativeWorld{};static QuatT28 evaluated[8]{};static bool retail=true,entityOK=true,poseOK=true;static void(*afterWorld)()=nullptr;
static A currentCharacter=ci;static unsigned quatReads=0,lastJoint=9999;static int mutateRead=-1;static void(*afterQuat)()=nullptr;
static A WhBase(){return base;}static std::atomic<void*>g_last_player_entity{reinterpret_cast<void*>(0x111)};
namespace retail_build112{static bool Ready(A v){return retail&&v==base;}}
namespace kcd2vr::combat_diag_probe{static bool NativeCharacterEntryQualified63(A){return false;}}
static void* FlinchEntityById178r11(unsigned id){return entityOK&&id==42?reinterpret_cast<void*>(entity):nullptr;}
static void* CharacterForEntity6689(void* e){return A(e)==entity?reinterpret_cast<void*>(currentCharacter):nullptr;}
static bool ValidateCharInstance(void* p,void*& a){a=p;return A(p)==currentCharacter;}
static bool ReadValidatedDefaultSkeletonFromPose(void* p,void*& s){A v=0;if(!Read(reinterpret_cast<void*>(A(p)-0x8e0+0xa70),v))return false;s=reinterpret_cast<void*>(v);return true;}
static bool ReadJointCount(void* s,std::uint32_t& n){A j=0;return Read(reinterpret_cast<void*>(A(s)+8),j)&&Read(reinterpret_cast<void*>(j-4),n);}
static const QuatT28* __fastcall getter(void*,int i){return i>=0&&i<8?&evaluated[i]:nullptr;}
static bool ValidateRawPoseApi(void*,PoseGetFn& r,PoseGetFn& c){r=c=getter;return poseOK;}
static bool ReadQuat(PoseGetFn f,void* p,int i,QuatT28& v){++quatReads;lastJoint=i;if(i==mutateRead)put(ci+0xa70,A(0xBAD));auto* q=f(p,i);if(!q)return false;v=*q;if(afterQuat){auto callback=afterQuat;afterQuat=nullptr;callback();}return true;}
static bool ValidPose6689(const QuatT28& q){return std::isfinite(q.t[0])&&std::isfinite(q.t[1])&&std::isfinite(q.t[2]);}
static bool ReadModelWorld6692(void*,ModelWorld6692& w){w=nativeWorld;if(afterWorld){auto callback=afterWorld;afterWorld=nullptr;callback();}return true;}
static Q4 QFromQuatT(const QuatT28& q){return {q.q[0],q.q[1],q.q[2],q.q[3]};}
static Q4 QMul(Q4 a,Q4 b){return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};}
static V3 ModelToWorld6692(const ModelWorld6692& w,V3 v){const auto p=ph::point(ph::Pose{{w.q.x,w.q.y,w.q.z,w.q.w},{w.t.x,w.t.y,w.t.z}},ph::Vec3{v.x,v.y,v.z}*w.scale);return {float(p.x),float(p.y),float(p.z)};}
#include "actual_living_anchor.inc"
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::atomic<std::uint64_t>g_weapon_request_epoch6694{3},g_pose_epoch6689{2},g_pose_policy_generation57{2},g_phys_ready_sample6698{1};
static std::atomic<bool>g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
namespace kcd2vr::tracking59c6{static std::atomic<std::uint64_t>epoch{1};}
namespace animation_filter{static std::atomic<bool>nativeBodyOwned112{false};} // C29: native kill stand-down + corpse stab entity lookup reached from the coordinator
namespace animation_filter{inline void* PlayerSkeletonAnim() noexcept{return nullptr;}}
static void* FlinchEntityById178r11(unsigned) noexcept{return nullptr;}

namespace diagnostics::capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace{static std::vector<std::string>lines;static void Write(const char* s){lines.emplace_back(s);}}
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2>profiles{};std::uint64_t request=3;float world[8]{0,0,0,1,0,0,0,1};double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698>g_phys_frame6698;
struct ImpalePin178r15{bool valid=false;std::uint64_t serial=0,tick=0;float x=0,y=0,z=0;int mode40=0;float depth40=0;};
struct ImpaleSkip40{A entity=0,body=0;std::uint64_t tick=0;std::uint32_t target=0;bool hangR128=false;};
static kcd2vr::core::SnapshotMailbox<ImpalePin178r15>g_impale_pin178r15;static kcd2vr::core::SnapshotMailbox<ImpaleSkip40>g_impale_skip40;
static unsigned corpseReads=0,drives=0,constraints=0;static bool nativeWeapon=true;
static std::atomic<bool>g_combat_experiment175{true},g_combat_integrated178r22c2{false},g_physical_combat177{true},g_combat_full178r10{true},g_impale_enabled178r15{true};
static std::atomic<int>g_combat_rung178r15{7};
static bool CombatExperiment175Enabled(){return g_combat_experiment175.load();}
static bool PhysicalCombat177Enabled(){return CombatExperiment175Enabled()&&g_physical_combat177.load();}
static bool CombatFull178r10Enabled(){return PhysicalCombat177Enabled()&&g_combat_full178r10.load();}
#include "actual_impale_options.inc"
static void options(ph::Input& pi){
#define KCD2VR_IMPALE_OPTIONS_BODY_H41
#include "actual_impale_options.inc"
#undef KCD2VR_IMPALE_OPTIONS_BODY_H41
}
static void acceptedMode(kcd2vr::user175::Mode mode){
 // Mode admission is controlled. Configuration and rung getter are production code.
 using namespace kcd2vr::user175;const auto core=mode==Mode::CombatSynchronized?Mode::CombatIntegrated:mode;const auto c=configuration(core);
 g_combat_experiment175=c.experiment;g_combat_integrated178r22c2=core==Mode::CombatIntegrated;
 g_physical_combat177=c.physical177;g_combat_full178r10=c.full178r10;g_combat_rung178r15=c.rung178r15;
}
static ph::Pose PhysWorld6698(const float* w){return {{w[0],w[1],w[2],w[3]},{w[4],w[5],w[6]}};}
static bool NativeImpaleWeaponCurrentH18(const PhysFrame6698&,int side){return nativeWeapon&&side==0;}
static kcd2vr::impale47::Poll ImpaleRoot47(const kcd2vr::partanchor59c7g::Identity&,A& r,const char*& why){++corpseReads;r=body;why="MODEL_CORPSE_LEAF";return kcd2vr::impale47::Poll::Ready;}
static bool ReadImpalePart59c7g(const kcd2vr::partanchor59c7g::Identity&,ph::Pose& p,double& s){++corpseReads;p={};s=1;return true;}
static bool ReadImpaleCurrentCorpseH18(const kcd2vr::partanchor59c7g::Identity& id,ph::Pose& p,double& s,const char*&){return ReadImpalePart59c7g(id,p,s);}
static bool ReadImpaleSuccessorH18(const kcd2vr::partanchor59c7g::Identity&,const ph::Contact103&,bool,kcd2vr::partanchor59c7g::Identity&,ph::Pose&,double&,const char*&){return false;}
static void ObserveImpaleOwnersH7(const kcd2vr::impale_observe_h7::Identity&,std::uint64_t,unsigned){}
namespace gore32n{static void BloodV(kcd2vr::gore32::V3,kcd2vr::gore32::V3,unsigned,const char*,float){}static bool PartVelocity(A,int,kcd2vr::gore32::V3){++drives;return true;}}
namespace kcd2vr::impale_constraint_h21{void Cancel(std::uint64_t)noexcept{}bool MotorBlocked()noexcept{return false;}const char* Failure()noexcept{return "MODEL_ONLY";}}
static bool StageImpaleConstraintH21(const PhysFrame6698&,const kcd2vr::partanchor59c7g::Identity&,int,ph::Vec3,ph::Vec3,ph::Vec3,std::uint64_t)noexcept{++constraints;return false;}
#ifndef TARGET_COORDINATOR
#define TARGET_COORDINATOR "../src/subsystems/gore/native_impale_part.inl"
#endif
#include TARGET_COORDINATOR
static unsigned checks=0,failed=0;
static void ck(bool b,const char* s){++checks;if(!b)++failed;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
static void seed(){
 acceptedMode(kcd2vr::user175::Mode::CombatContactLab);g_impale_enabled178r15=true;
 memory.clear();currentCharacter=ci;retail=entityOK=poseOK=true;mutateRead=-1;nativeWorld={};quatReads=0;nativeWeapon=true;afterQuat=nullptr;afterWorld=nullptr;
 g_weapon_request_epoch6694=3;g_pose_epoch6689=2;g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=false;animation_filter::nativeBodyOwned112=false;
 put(base+0x6411ac,ns::characterEntry);put(base+0x1aa6bc0,ns::poseGetter);put(base+0x1a77fd0,ns::physicsGetter);put(base+0x1aaf020,ns::countGetter);
 put(entity,base+0x4072c78);put(entity+0xc,unsigned(42));put(base+0x4072c78+0x328,base+0x6411ac);
 put(entity+0xa8,A(0x2000));put(0x2008,A(0x2100));put(0x2119,(unsigned char)0);put(0x2120,unsigned(1));put(0x2128,proxy);
 put(proxy,base+0x3ac3478);put(proxy+0x30,entity);put(proxy+0x38,root);
 put(ci,base+0x40602f8);put(base+0x40602f8+0x38,base+0x1aa6bc0);put(ci+0x698,base+0x3a71028);put(base+0x3a71028+0x30,base+0x1a77fd0);
 put(ci+0x8a8,ci);put(ci+0x7a0,body);put(ci+0x8e0,base+0x3a71510);put(ci+0x940,unsigned(8));put(ci+0xa70,model);
 put(model,base+0x406b930);put(base+0x406b930+8,base+0x1aaf020);put(model+8,joints);put(joints-4,unsigned(8));
 for(auto& p:evaluated){p={};p.t[0]=5;}
}
struct Replay{
 ph::Input in{};ph::State state{};ph::Output out{};std::array<ph::Profile,2>profiles{};double time=1;unsigned rays=0;
 Replay(){in.epoch=3;in.bodyEpoch=2;in.sample=1;in.tick=1000;in.timeNs=1000000000;in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;in.dynamics=false;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;in.coherentMelee119=in.controllerAnchor119=true;in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];in.hands[1].p={-1,0,0};in.impaleEnabled178r15=true;in.haveWorld=true;in.worldScale=1;auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.9,0};sample();for(int i=0;i<12;++i)move({});}
 void sample(){out=ph::step(state,in,profiles,[&](const ph::Profile&,ph::Pose,ph::Pose,bool){++rays;ph::Contact103 c{};c.known=true;c.fraction=1;return c;});}
 void move(ph::Vec3 d){time+=.01;in.timeNs=std::uint64_t(std::llround(time*1e9));in.tick=in.timeNs/1000000;++in.sample;in.hands[0].p=in.hands[0].p+d;in.intentHands[0].p=in.intentHands[0].p+d;sample();}
 ph::Vec3 tip(){return ph::point(out.weapons[0],profiles[0].tip);}
 kcd2vr::contact58c1::Receipt receipt(unsigned serial){kcd2vr::contact58c1::Receipt h{};h.serial=serial;h.attempt=77;h.tick=h.impactTick=in.tick;h.key=h.generation=1;h.epoch=2;h.request=3;h.sample=in.sample;h.target=42;h.targetIdentity=entity;h.collider=body;h.rawPart=457;h.kind=1;h.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);h.weaponClass=4;h.death=0;h.damage=20;h.energy=1;h.seen=h.accepted=h.applied=h.fields=h.bodyContact=h.continuation=true;h.blocked=h.perfect=h.parry=0;auto p=tip();h.point[0]=float(p.x);h.point[1]=float(p.y);h.point[2]=float(p.z);return h;}
 void native(){now=in.tick;PhysFrame6698 f{};f.input=in;f.output=out;f.profiles=profiles;g_phys_frame6698.publish(f);g_phys_ready_sample6698=in.sample;ImpalePartTick59c7g();}
 void consume(const ImpalePin178r15& p){in.impalePinValid178r15=p.valid;in.impalePinSerial178r19=p.serial;in.impalePinWorld178r15={p.x,p.y,p.z};in.impalePinMode40=p.mode40;in.impalePinDepth40=p.depth40;}
 void currentOptions(){in.physicalCombat177=in.coherentMelee119&&PhysicalCombat177Enabled();options(in);}
};
static void verdict(const kcd2vr::contact58c1::Receipt& h){kcd2vr::impale40::Verdict v{};v.serial=h.serial;v.target=h.target;v.identity=h.targetIdentity;v.energy=h.energy;v.damage=h.damage;v.death=h.death;v.kind=h.kind;v.armour=kcd2vr::impale40::ArmourStatus::Read;for(auto& x:v.stab)x=15;kcd2vr::impale40::verdict.publish(v);}
static void optionIngressChecks(){
 using kcd2vr::user175::Mode;
 for(auto mode:{Mode::CombatSynchronized,Mode::CombatContactLab,Mode::Weapons})for(bool coherent:{false,true})for(bool hang:{false,true})for(int c40:{0,1,2})for(bool latch:{false,true}){
  acceptedMode(mode);kcd2vr::gore32::impaleHang=hang;kcd2vr::impale40::mode=c40;g_impale_enabled178r15=latch;
  ph::Input pi{};pi.coherentMelee119=coherent;pi.physicalCombat177=coherent&&PhysicalCombat177Enabled();options(pi);
  const bool experiment=mode!=Mode::Weapons;const int rung=mode==Mode::CombatSynchronized?1:mode==Mode::CombatContactLab?3:0;
  const bool wantHang=coherent&&hang,want40=coherent&&experiment&&latch&&c40>0;
  ck(F4Rung178r10()==rung,"actual F4 getter distinguishes HYBRID1, DIRECT3 and combat OFF");
  ck(pi.impaleHang32==wantHang&&pi.impale40==want40,"literal input producer keeps C40 separate from HANG, Latch and coherent combat constraints");
  ck(pi.impaleEnabled178r15==(wantHang||(latch&&((coherent&&experiment&&rung==3)||want40))),"literal input producer preserves legacy branches and independently enables opted-in C40");
 }
 seed();Replay live;acceptedMode(Mode::CombatSynchronized);kcd2vr::gore32::impaleHang=false;kcd2vr::impale40::mode=1;live.currentOptions();
 ck(F4Rung178r10()==1&&live.in.impale40&&live.in.impaleEnabled178r15&&!live.in.impaleHang32,"HYBRID C40 ON with corpse HANG OFF reaches actual producer");
 auto h=live.receipt(500);live.in.receipt58c1=h;verdict(h);evaluated[5].t[0]=h.point[0];evaluated[5].t[1]=h.point[1];evaluated[5].t[2]=h.point[2];
 const unsigned oldDrive=drives,oldConstraint=constraints,oldCorpse=corpseReads;live.move({});live.native();ImpalePin178r15 pin{};g_impale_pin178r15.read(pin);
 ck(live.out.impaleLatchRequest&&pin.valid&&pin.mode40==1,"HYBRID HANG OFF: actual producer -> physical step -> native coordinator publishes living STUCK");
 ck(drives==oldDrive&&constraints==oldConstraint&&corpseReads==oldCorpse,"independent C40 ingress performs no corpse query/body drive/constraint");
 live.consume(pin);live.move({});ck(live.out.displayLatched[0],"HYBRID HANG OFF: physical display consumes current matching pin");
 kcd2vr::impale40::mode=0;live.currentOptions();live.move({});live.native();g_impale_pin178r15.read(pin);
 ck(!live.in.impaleEnabled178r15&&!pin.valid&&!live.out.displayLatched[0],"turning C40 OFF with HANG OFF retires display at both production consumers");
 kcd2vr::impale40::mode=1;live.currentOptions();live.move({});live.native();g_impale_pin178r15.read(pin);
 ck(!pin.valid,"C40 reenable cannot reacquire an already consumed original receipt");
 for(int loss=0;loss<3;++loss){
  seed();Replay r;acceptedMode(Mode::CombatSynchronized);kcd2vr::gore32::impaleHang=false;kcd2vr::impale40::mode=2;r.currentOptions();
  auto hit=r.receipt(510+loss);r.in.receipt58c1=hit;verdict(hit);evaluated[5].t[0]=hit.point[0];evaluated[5].t[1]=hit.point[1];evaluated[5].t[2]=hit.point[2];r.move({});r.native();g_impale_pin178r15.read(pin);ck(pin.valid,"mode2 independent C40 starts from current original wound");r.consume(pin);
  if(loss==0)acceptedMode(Mode::Weapons);if(loss==1)g_impale_enabled178r15=false;if(loss==2)r.in.coherentMelee119=false;
  r.currentOptions();r.move({});r.native();g_impale_pin178r15.read(pin);ck(!pin.valid&&!r.out.displayLatched[0],"combat OFF, Latch OFF or coherent-frame loss retires independent C40");
 }
 for(bool c40:{false,true}){
  seed();Replay r;acceptedMode(Mode::CombatSynchronized);kcd2vr::gore32::impaleHang=true;kcd2vr::impale40::mode=c40?1:0;r.currentOptions();
  auto hit=r.receipt(520+c40);hit.death=1;r.in.receipt58c1=hit;r.move({});
  ck(r.in.impaleHang32&&r.out.impaleLatchRequest&&r.out.impaleHit47.death==1,"existing default HYBRID HANG lethal receipt route remains available with C40 OFF or ON");
 }
 seed();Replay kill;acceptedMode(Mode::CombatSynchronized);kcd2vr::gore32::impaleHang=false;kcd2vr::impale40::mode=1;kill.currentOptions();
 auto lethal=kill.receipt(530);lethal.death=1;kill.in.receipt58c1=lethal;verdict(lethal);const unsigned beforeDrive=drives,beforeConstraint=constraints;kill.move({});kill.native();
 ck(kill.out.impaleLatchRequest&&kill.out.impaleHit47.death==1,"new C40 ingress preserves existing lethal precedence, not a fabricated living receipt");
 ck(drives==beforeDrive&&constraints==beforeConstraint,"C40 ON with HANG OFF never enables corpse motor or constraint for a lethal receipt");
 acceptedMode(Mode::CombatContactLab);g_impale_enabled178r15=true;kcd2vr::gore32::impaleHang=false;
}
int main(){
 seed();const ph::Vec3 contact{.2,.4,1.1};evaluated[5].t[0]=.25f;evaluated[5].t[1]=.4f;evaluated[5].t[2]=1.1f;
 kcd2vr::partanchor59c7g::Identity id{1,2,3,42,entity,body,457};LivingImpaleC40 selected{};ph::Pose p{};double scale=0;
 ck(AcquireLivingImpaleC40(id,contact,selected,p,scale)&&selected.joint==5&&selected.receipt.part==457,"nearest evaluated joint5 stored SEPARATELY from raw physics part457");
 kcd2vr::partanchor59c7g::Anchor anchor{};ck(anchor.acquire(id,p,scale,contact),"actual Anchor captures accepted contact local offset");
 auto bad=id;bad.body=0xBAD;LivingImpaleC40 rejected{};ck(!AcquireLivingImpaleC40(bad,contact,rejected,p,scale),"unrelated collider is not a current actor body");
 bad=id;bad.part=99999;ck(AcquireLivingImpaleC40(bad,contact,rejected,p,scale)&&rejected.joint==5,"rawPart outside skeleton count is still metadata, not a joint index");
 const unsigned before=quatReads;evaluated[1]=evaluated[5];evaluated[1].t[0]=.2f;evaluated[5].t[0]+=.1f;ph::Vec3 moved{};
 ck(ReadLivingImpaleC40(id,selected,p,scale)&&anchor.update(id,p,scale,moved)&&std::abs(moved.x-.3)<1e-6,"moving retained joint preserves actual wound local offset");
 ck(lastJoint==5&&quatReads==before+1,"later closer joint does not trigger nearest selection again");
 afterWorld=[](){nativeWorld.scale=2;};
 ck(ReadLivingImpaleC40(id,selected,p,scale)&&scale==1,"returned scale belongs to same evaluated-pose world snapshot, not later validation capture");nativeWorld.scale=1;
 evaluated[5].q[2]=evaluated[5].q[3]=float(std::sqrt(.5));
 ck(ReadLivingImpaleC40(id,selected,p,scale)&&anchor.update(id,p,scale,moved)&&std::abs(moved.x-.35)<1e-6&&std::abs(moved.y-.35)<1e-6,"current joint rotation carries original contact-local offset");
 put(ci+0xa70,A(0xBAD));ck(!ReadLivingImpaleC40(id,selected,p,scale),"replaced default skeleton retires display");put(ci+0xa70,model);
 put(ci+0x7a0,A(0xBAD));ck(!ReadLivingImpaleC40(id,selected,p,scale),"replaced current main body retires display");put(ci+0x7a0,body);
 bad=id;bad.serial++;ck(!ReadLivingImpaleC40(bad,selected,p,scale),"another receipt serial cannot reuse chosen joint");
 seed();mutateRead=3;ck(!AcquireLivingImpaleC40(id,contact,selected,p,scale),"skeleton replacement during evaluated scan publishes no partial anchor");
 seed();nativeWorld.scale=2;evaluated[5].t[0]=.1f;evaluated[5].t[1]=.2f;evaluated[5].t[2]=.55f;
 ck(AcquireLivingImpaleC40(id,contact,selected,p,scale)&&selected.joint==5&&scale==2&&pm::length(p.p-contact)<1e-6,"non-unit native model scale converted once");
 seed();bad=id;bad.body=root;evaluated[5].t[0]=float(contact.x);evaluated[5].t[1]=float(contact.y);evaluated[5].t[2]=float(contact.z);
 ck(AcquireLivingImpaleC40(bad,contact,selected,p,scale),"same current entity capsule root also permits geometric display without joint-part inference");
 seed();Replay off;off.in.receipt58c1=off.receipt(100);off.move({});off.native();ImpalePin178r15 pin{};g_impale_pin178r15.read(pin);ck(!off.out.impaleLatchRequest&&!pin.valid&&quatReads==0,"C40 OFF: no extra skeleton acquisition or native pin");
 kcd2vr::impale40::mode=1;
 seed();Replay r;r.in.impale40=true;auto hit=r.receipt(101);r.in.receipt58c1=hit;verdict(hit);const auto wound=ph::Vec3{hit.point[0],hit.point[1],hit.point[2]};
 evaluated[5].t[0]=float(wound.x+.05);evaluated[5].t[1]=float(wound.y);evaluated[5].t[2]=float(wound.z);r.move({});
 ck(r.out.impaleLatchRequest&&r.out.impaleSerial==101,"actual physical step admits existing eligible C40 living receipt");
 const auto raw=r.in.hands[0];r.native();g_impale_pin178r15.read(pin);ck(pin.valid&&pin.serial==101&&pin.mode40==1,"actual native coordinator publishes STUCK nearest-joint display pin");
 ck(drives==0&&corpseReads==0&&constraints==0,"living display uses no corpse query or NPC/body drive");
 r.consume(pin);r.move({});ck(r.out.impaleActive&&r.out.displayLatched[0],"actual physical step consumes matching display pin");
 ck(pm::length(r.in.hands[0].p-raw.p)<1e-12,"display never rewrites raw requested controller pose");
 evaluated[5].t[0]+=.05f;r.native();g_impale_pin178r15.read(pin);ck(pin.valid&&std::abs(pin.x-wound.x-.05)<1e-5,"actual coordinator follows retained joint/contact offset");
 auto dead=hit;dead.serial=102;dead.death=1;kcd2vr::contact58c1::latest.publish(dead);r.in.impaleHang32=true;r.move({});r.native();g_impale_pin178r15.read(pin);
 ck(pin.valid&&pin.mode40==1&&drives==0&&constraints==0,"later native death stays STUCK without HANG handover");
 put(ci+0xa70,A(0xBAD));r.native();g_impale_pin178r15.read(pin);ck(!pin.valid,"actual coordinator releases on current skeleton replacement");
 put(ci+0xa70,model);r.native();g_impale_pin178r15.read(pin);ck(!pin.valid,"same retired original request cannot reselect/reacquire a display joint");
 seed();Replay toggle;toggle.in.impale40=true;auto ht=toggle.receipt(150);toggle.in.receipt58c1=ht;verdict(ht);
 evaluated[5].t[0]=ht.point[0];evaluated[5].t[1]=ht.point[1];evaluated[5].t[2]=ht.point[2];toggle.move({});toggle.native();g_impale_pin178r15.read(pin);
 ck(pin.valid&&pin.mode40==1,"toggle schedule starts from actual C40 display pin");toggle.consume(pin);toggle.in.impale40=false;toggle.in.impaleHang32=true;toggle.move({});toggle.native();g_impale_pin178r15.read(pin);
 ck(!pin.valid&&drives==0,"C40 OFF retires its display even while existing base HANG option stays ON");
 toggle.in.impale40=true;toggle.move({});toggle.native();g_impale_pin178r15.read(pin);ck(!pin.valid,"reenabling option cannot renew same retired original display request");
 seed();Replay unknown;unknown.in.impale40=true;auto hu=unknown.receipt(160);hu.death=-1;unknown.in.receipt58c1=hu;verdict(hu);
 evaluated[5].t[0]=hu.point[0];evaluated[5].t[1]=hu.point[1];evaluated[5].t[2]=hu.point[2];unknown.move({});unknown.native();g_impale_pin178r15.read(pin);
 ck(unknown.out.impaleLatchRequest&&pin.valid&&pin.mode40==1,"unknown native death remains existing C40 eligible display, not a claim actor is known living");
 ck(drives==0&&constraints==0,"unknown native death never authorizes corpse/HANG drive");
 for(int kind=0;kind<5;++kind){seed();Replay no;no.in.impale40=true;auto h=no.receipt(200+kind);if(kind==0)h.damage=0;if(kind==1)h.blocked=1;if(kind==2)h.accepted=false;if(kind==3)h.epoch=9;if(kind==4)h.energy=.1f;no.in.receipt58c1=h;no.move({});ck(!no.out.impaleLatchRequest,"original damage/defense/acceptance/epoch/energy refusal preserved");}
 for(int change=0;change<4;++change){seed();Replay c;c.in.impale40=true;auto h=c.receipt(300+change);c.in.receipt58c1=h;verdict(h);
  evaluated[5].t[0]=h.point[0];evaluated[5].t[1]=h.point[1];evaluated[5].t[2]=h.point[2];c.move({});
  if(change==0)afterQuat=[](){g_weapon_request_epoch6694=99;};
  if(change==1)afterQuat=[](){g_pose_epoch6689=99;};
  if(change==2)afterQuat=[](){now+=81;};
  if(change==3)afterQuat=[](){g_ctx_menu=true;};
  c.native();g_impale_pin178r15.read(pin);ck(!pin.valid,"native-read source/epoch/80ms/context replacement publishes no stale display pin");
 }
 optionIngressChecks();
 std::printf("LIVING_C40_COMPOSED %u/%u failures=%u; native leaves modeled, no engine/headset proof\n",checks-failed,checks,failed);return failed?1:0;
}

#else
#include "vrik/impale_hold60.h"
#include "subsystems/gore/impale_constraint_h21.h"
#include "vrik/visible_failure.h"
// Complete production native impale coordinator. Only engine leaves are doubled;
// this fixture does NOT establish a retail corpse-body map or invoke game code.
#include "vrik/physical_weapon.h"
#include "vrik/part_anchor59c7g.h"
#include "vrik/blood49.h"
#include "vrik/kick_style26.h"
#include "vrik/contact_contract176.h"
#include "vrik/contact_authority176.h"
#include "subsystems/gore/impale_observe_h7.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
static std::uint64_t now59=1000;static std::uint64_t GetTickCount64(){return now59;}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{3},g_pose_epoch6689{2},g_pose_policy_generation57{2},g_phys_ready_sample6698{1};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
namespace kcd2vr::tracking59c6{static std::atomic<std::uint64_t> epoch{1};}
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};} // C29: native kill stand-down + corpse stab entity lookup reached from the coordinator
namespace animation_filter{inline void* PlayerSkeletonAnim() noexcept{return nullptr;}}
static void* FlinchEntityById178r11(unsigned id) noexcept{return id==42?reinterpret_cast<void*>(0x420):nullptr;}

namespace diagnostics::capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return true;}}
namespace diagnostic_trace{static std::vector<std::string> lines;static void Write(const char* s){lines.push_back(s);}}
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};std::uint64_t request=0;float world[8]{};double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
struct ImpalePin178r15{bool valid=false;std::uint64_t serial=0,tick=0;float x=0,y=0,z=0;int mode40=0;float depth40=0;};
struct ImpaleSkip40{std::uintptr_t entity=0,body=0;std::uint64_t tick=0;std::uint32_t target=0;bool hangR128=false;};
static kcd2vr::core::SnapshotMailbox<ImpalePin178r15> g_impale_pin178r15;
static kcd2vr::core::SnapshotMailbox<ImpaleSkip40> g_impale_skip40;
static int F4Rung178r10(){return 3;}
static ph::Pose PhysWorld6698(const float* w){return {{w[0],w[1],w[2],w[3]},{w[4],w[5],w[6]}};}
static kcd2vr::impale47::Poll poll59=kcd2vr::impale47::Poll::NotYet;
static std::uintptr_t root59=0x440;static unsigned roots59=0,status59=0,drive59=0;
static ph::Vec3 partPoint59{0,.7,0};static bool integratePart59=false;
static kcd2vr::impale47::Poll ImpaleRoot47(const kcd2vr::partanchor59c7g::Identity& id,std::uintptr_t& root,const char*& why){
 ++roots59;assert(id.entity==0x420&&id.body==0x440&&id.part==12);
 root=root59;why=poll59==kcd2vr::impale47::Poll::NotYet?"ROOT_STILL_LIVING":"EXACT_ENGINE_LEAF_FIXTURE";return poll59;
}
static bool ReadImpalePart59c7g(const kcd2vr::partanchor59c7g::Identity& id,ph::Pose& pose,double& scale){
 ++status59;assert(id.entity==0x420&&id.body==0x440&&id.part==12);pose={};pose.p=partPoint59;scale=1;return true;
}
static void ObserveImpaleOwnersH7(const kcd2vr::impale_observe_h7::Identity&,std::uint64_t,unsigned){}
static bool NativeImpaleWeaponCurrentH18(const PhysFrame6698&,int side){return side==0;}
namespace kcd2vr::impale_constraint_h21 {
void Cancel(std::uint64_t)noexcept{}
bool MotorBlocked()noexcept{return false;}
const char* Failure()noexcept{return "NO_CURRENT_ACHIEVED_RIGID_BUDDY_PHYSICAL_IMPALE_ENTRY_NOT_CONNECTED";}
}
static bool StageImpaleConstraintH21(const PhysFrame6698&,const kcd2vr::partanchor59c7g::Identity&,int,ph::Vec3,ph::Vec3,ph::Vec3,std::uint64_t)noexcept{return false;}
static bool ReadImpaleCurrentCorpseH18(const kcd2vr::partanchor59c7g::Identity& id,ph::Pose& pose,double& scale,const char*&){return ReadImpalePart59c7g(id,pose,scale);}
static bool ReadImpaleSuccessorH18(const kcd2vr::partanchor59c7g::Identity&,const ph::Contact103&,bool,
 kcd2vr::partanchor59c7g::Identity&,ph::Pose&,double&,const char*&){return false;}
namespace gore32n{
static void BloodV(kcd2vr::gore32::V3,kcd2vr::gore32::V3,std::uint32_t,const char*,float){}
static bool PartVelocity(std::uintptr_t body,int part,kcd2vr::gore32::V3 v){assert(body==0x440&&part==12);++drive59;if(integratePart59)partPoint59=partPoint59+ph::Vec3{v.x,v.y,v.z}*.011;return true;}
}
// Retained lethal-only coordinator fixture: this separate display path is
// exercised by the actual-helper composition mode above, never by these leaves.
struct LivingImpaleC40 {int joint=-1;};
static bool AcquireLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity&,ph::Vec3,LivingImpaleC40&,ph::Pose&,double&){return false;}
static bool ReadLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity&,const LivingImpaleC40&,ph::Pose&,double&){return false;}
#include "../src/vrik/impale_death_speed_r106.h" // R106
#include "../src/subsystems/gore/native_impale_part.inl"
static PhysFrame6698 frame59;
static void tick59(std::uint64_t t){now59=t;frame59.input.tick=t;++frame59.input.sample;g_phys_frame6698.publish(frame59);g_phys_ready_sample6698=frame59.input.sample;ImpalePartTick59c7g();}
static void request59(std::uint64_t serial,std::uint64_t tick){
 frame59={};frame59.request=3;frame59.input.epoch=3;frame59.input.bodyEpoch=2;
 frame59.input.allowed=frame59.input.tracked[0]=frame59.input.contact=frame59.input.haveWorld=true;frame59.input.worldScale=1;frame59.input.grip[0]=1;
 frame59.input.impaleEnabled178r15=frame59.input.impaleHang32=true;frame59.input.hands[1].p={-1,0,0};
 frame59.world[3]=frame59.world[7]=1;
 auto& o=frame59.output;o.impaleLatchRequest=true;o.impaleSerial=serial;o.impaleEntity=42;o.impaleSide32=0;
 o.impaleTipWorld=o.impaleLocal32={0,.7,0};o.active[0]=o.owned[0]=o.contactKnown[0]=true;
 auto& p=frame59.profiles[0];p.key=p.generation=1;p.side=0;p.primary=p.geometry=true;p.classId=4;p.tip={0,.9,0};
 auto& h=o.impaleHit47;h.serial=serial;h.tick=h.impactTick=tick;h.attempt=77;h.key=h.generation=1;h.epoch=2;h.request=3;
 h.target=42;h.targetIdentity=0x420;h.collider=0x440;h.rawPart=12;h.kind=1;h.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);h.weaponClass=4;h.death=1;h.damage=25;h.energy=1;
 h.seen=h.accepted=h.applied=h.fields=h.bodyContact=h.continuation=true;h.blocked=h.perfect=h.parry=0;h.point[1]=.7f;
}
int main(){
 kcd2vr::collab_h21::setPhysicsMode(3);
 ImpalePin178r15 pin{};
 // Native acquired the exact root/part and published its first pin. The XR
 // consumer has not marked its own state pinned when reach recovery arrives.
 request59(1,1000);poll59=kcd2vr::impale47::Poll::NotYet;tick59(1000);assert(!drive59&&!status59);
 poll59=kcd2vr::impale47::Poll::Ready;tick59(1100);assert(drive59&&g_impale_pin178r15.read(pin)&&pin.valid);
 const unsigned statusBefore=status59,driveBefore=drive59;
 frame59.output.handlingOnly55c1[0]=true;frame59.output.contactKnown[0]=false;
 tick59(1110);assert(status59==statusBefore&&drive59==driveBefore&&g_impale_pin178r15.read(pin)&&!pin.valid);
 // An original immutable request may wait through a recovery, but cannot query
 // or acquire a corpse from its unchecked direct-hand presentation.
 request59(2,2000);poll59=kcd2vr::impale47::Poll::NotYet;tick59(2000);
 const unsigned rootBefore=roots59;frame59.output.handlingOnly55c1[0]=true;frame59.output.contactKnown[0]=false;
 poll59=kcd2vr::impale47::Poll::Ready;tick59(2400);assert(roots59==rootBefore&&status59==statusBefore&&drive59==driveBefore);
 frame59.output.handlingOnly55c1[0]=false;frame59.output.contactKnown[0]=true;tick59(2450);
 assert(drive59>driveBefore&&g_impale_pin178r15.read(pin)&&pin.valid&&pin.serial==2);
 // Retire that anchor explicitly through the original request-ended path.
 frame59.output={};tick59(2460);assert(g_impale_pin178r15.read(pin)&&!pin.valid);
 // Even a Ready native body at/after deadline is rejected BEFORE its query.
 request59(3,3000);poll59=kcd2vr::impale47::Poll::NotYet;tick59(3000);
 const unsigned expiredRoots=roots59,expiredStatus=status59,expiredDrive=drive59;
 poll59=kcd2vr::impale47::Poll::Ready;tick59(7500);
 assert(roots59==expiredRoots&&status59==expiredStatus&&drive59==expiredDrive&&g_impale_pin178r15.read(pin)&&!pin.valid);
 // Same NPC foreign owner is not a new body/raw-part map. Never substitute it.
 request59(4,8000);root59=0x999;tick59(8000);
 assert(status59==expiredStatus&&drive59==expiredDrive&&g_impale_pin178r15.read(pin)&&!pin.valid);
 bool mappingReceipt=false;for(const auto& line:diagnostic_trace::lines)
  if(line.find("ROOT_MAPPING_REFUSED")!=std::string::npos&&line.find("entity=420 hitCollider=440 currentRoot=999 rawPart=12 mapping=NONE")!=std::string::npos)mappingReceipt=true;
 assert(mappingReceipt);
 std::puts("PASS actual native impale coordinator: no drive on first-pin/recovery race, bounded read-only wait, checked requalification, pre-query expiry and original collider/raw-part mapping");
 return 0; // Also callable by the combined R93 scheduling replay.
}

#endif
