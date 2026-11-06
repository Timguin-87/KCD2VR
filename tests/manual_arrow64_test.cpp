// Executes the actual presentation owner/transaction/helper consumer. Only
// engine lookups, current-packet provider and entity/child PRS are explicit leaves.
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <future>
#include <string>
#include <vector>
#include "vrik/physical_weapon.h"
#include "vrik/staged_hold596c2.h"
#include "core/snapshot_mailbox.h"
#include "core/haptics.h"
#include "hand_claim_fixture_t24.h"
namespace pm=kcd2vr::pose_core;namespace ph=kcd2vr::physical;
// Match the native translation unit: V3 is a distinct float-valued engine-side
// type, not an alias of the double-valued physical/pose-core vector.
struct V3 {float x{},y{},z{};};
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static V3 FromT(const QuatT28& a){return {a.t[0],a.t[1],a.t[2]};}
static V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static double Len(V3 a){return std::sqrt(double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z);}
static ph::Pose PhysPose6698(const QuatT28& a){return {{a.q[0],a.q[1],a.q[2],a.q[3]},{a.t[0],a.t[1],a.t[2]}};}
static QuatT28 qt(ph::Pose p){return {{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};}
static QuatT28 AbsoluteFromRelative(const QuatT28& a,const QuatT28& b){return qt(ph::compose(PhysPose6698(a),PhysPose6698(b)));}
static QuatT28 RelativeFromAbsolute(const QuatT28& a,const QuatT28& b){return qt(ph::compose(ph::inverse(PhysPose6698(a)),PhysPose6698(b)));}
static bool ValidPose6689(const QuatT28& q){return ph::finite(PhysPose6698(q));}
#include "vrik/manual_arrow_types64.inl"
struct CommittedFrame6695 {pm::Stamp stamp{};QuatT28 native[2]{},allowed[2]{};bool valid[2]{};ManualArrowPose64 arrow64{};};
struct ManualBow596 {void* player=nullptr;void* entity=nullptr;std::uint32_t hand=0;int side=0;
    std::uint64_t weapon=0,ammo=0,generation=0,epoch=0,request=0,arrowSerial64=0;};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};};
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static std::atomic<std::uint64_t> g_pose_epoch6689{1},g_weapon_request_epoch6694{1};
static std::atomic<bool> g_manual_bow_fault596{false},g_gameplay_allowed{true};
static std::atomic<void*> g_henry_char_attachment_manager{nullptr};
static bool staged=true,tracked=true,contract=true,helperOK=true,bodyOK=true,readbackOK=true,child=false;
static std::array<unsigned char,0x300> player{},model{},entity{},object{},attachment{},manager{},character{},other{};
static constexpr std::uintptr_t base=0x10000000;
static void* entityLookup=nullptr;static const char* ammoName="entity_right";
static unsigned placed=0,childPlaced=0,helperCalls=0,checks=0;static float naturalScale=1;
struct Matrix34V6550 {float m[12]{};};
static Matrix34V6550 entityWorld{};static QuatT28 helper{};
static std::uintptr_t WhBase(){return base;}
template<class T>static void put(auto& a,std::size_t n,const T& value){std::memcpy(a.data()+n,&value,sizeof(value));}
template<class T>static bool Read(const void* p,T& out){
    if(reinterpret_cast<std::uintptr_t>(p)==base+0x40AC298){if constexpr(std::is_same_v<T,const char*>)out=ammoName;else return false;return true;}
    const auto address=reinterpret_cast<std::uintptr_t>(p);
    for(auto* block:{&player,&model,&entity,&object,&attachment,&manager,&character,&other}){
        const auto lo=reinterpret_cast<std::uintptr_t>(block->data());
        if(address>=lo&&address-lo<=block->size()&&sizeof(T)<=block->size()-(address-lo)){
            std::memcpy(&out,p,sizeof(T));return true;}}
    return false;
}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t actual=0;return Read(p,actual)&&actual==base+rva;}
static void* EntityById6694(unsigned id){return id==77?entityLookup:nullptr;}
static bool ManualPlayer596(void* p){return p==player.data();}
static bool StagedBowEnabled59(){return staged;}
static bool TrackingHeadUsable111(std::uint64_t){return tracked;}
static bool TrackingHandUsable111(int,std::uint64_t){return tracked;}
static void* ManualFindHandAttachment19c4r4(void* m,const char* name){return m==model.data()&&name==ammoName?attachment.data():nullptr;}
// Actual native-binding readers generated from the current production source.
#include "manual_arrow_binding64.inc"
static bool NativeHelper(void*,const char*,QuatT28* out,bool){++helperCalls;*out=helper;return helperOK;}
static auto g_attachment_helper_transform_original=&NativeHelper;
static bool PhysSocketContract101(std::uintptr_t){return contract;}
static CommittedFrame6695 body{};
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=body;return bodyOK&&pm::fresh(out.stamp,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(character.data()),now,80);}
struct ModelWorld6692 {pm::Quat q{};V3 t{};float scale=1;};
static ModelWorld6692 world{};
static bool PhysAttachmentWorld164(void* a,std::uintptr_t c,ModelWorld6692& out){out=world;return a==attachment.data()&&c==reinterpret_cast<std::uintptr_t>(character.data());}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692& w){p.p=p.p*w.scale;return ph::compose({w.q,{w.t.x,w.t.y,w.t.z}},p);}
static bool ReadAttachmentQs6689(void*,float (&q)[8]){q[0]=q[1]=q[2]=0;q[3]=1;q[7]=naturalScale;q[4]=q[5]=q[6]=0;return true;}
static bool PlaceBothConsumers6689(void* self,void*,const float (&q)[8],bool& haveChild){
    ++placed;haveChild=child;if(child)++childPlaced;
    const pm::Quat r{q[0],q[1],q[2],q[3]};
    const pm::Vec3 axes[3]{pm::rotate(r,{1,0,0}),pm::rotate(r,{0,1,0}),pm::rotate(r,{0,0,1})};
    for(int c=0;c<3;++c){float scale=0;if(!Read(static_cast<unsigned char*>(self)+0x0C+c*4,scale))return false;
        entityWorld.m[c]=float(axes[c].x*q[7]*scale);entityWorld.m[4+c]=float(axes[c].y*q[7]*scale);entityWorld.m[8+c]=float(axes[c].z*q[7]*scale);}
    entityWorld.m[3]=q[4];entityWorld.m[7]=q[5];entityWorld.m[11]=q[6];return true;
}
static bool PlacementReadback6689(void*,void*,unsigned,const float (&)[8],bool& checked){checked=child;return readbackOK;}
static bool GetEntityWorldTmV6552(void* e,unsigned id,Matrix34V6550& out){out=entityWorld;return e==entity.data()&&id==77;}
namespace diagnostics::capture168 {enum class Domain{Reload};static bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
// Native passenger owner is an explicit external leaf in this retained fixture.
static bool nativePassengerA34=false;
static bool NativePassengerCurrentA34(std::uint64_t) noexcept {return nativePassengerA34;}
#include "vrik/manual_arrow64.inl"
static ManualBow596 bow{};static PhysFrame6698 frame{};
static ph::Profile bowProfile{};static ph::Output bowOutput{};
static QuatT28 original[1]{},reference[1]{};
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static bool near(V3 a,V3 b,double tolerance=1e-5){return Len(Sub(a,b))<tolerance;}
static void reset(){
    ManualArrowWithdraw64();g_manual_arrow_socket64.publish({});now=1000;g_pose_epoch6689=1;g_weapon_request_epoch6694=1;
    g_manual_bow_fault596=false;g_gameplay_allowed=true;staged=tracked=contract=helperOK=bodyOK=readbackOK=true;child=false;naturalScale=1;
    player.fill(0);model.fill(0);entity.fill(0);object.fill(0);attachment.fill(0);manager.fill(0);character.fill(0);
    hand_claim_fixture_t24::Reset(player.data(),reinterpret_cast<std::uintptr_t>(character.data()));
    put(player,0x1B0,static_cast<void*>(model.data()));put(model,0x20,static_cast<void*>(player.data()));put(model,0x2C,std::uint32_t(77));
    put(entity,0,base+0x4072C78);put(object,0,base+0x3A68F68);put(object,8,std::uint32_t(77));
    for(int i=0;i<3;++i)put(object,0x0C+i*4,1.f);
    put(attachment,0,base+0x3A657C0);put(attachment,0x20,static_cast<void*>(object.data()));put(attachment,0x28,static_cast<void*>(manager.data()));
    put(attachment,0x78,std::int16_t(0));QuatT28 relative{};relative.t[0]=.01f;put(attachment,0x9C,relative);
    put(manager,0x70,static_cast<void*>(character.data()));g_henry_char_attachment_manager=manager.data();entityLookup=entity.data();
    bow={};bow.player=player.data();bow.entity=entity.data();bow.hand=77;bow.weapon=20;bow.ammo=30;bow.generation=bow.epoch=bow.request=1;bow.arrowSerial64=ManualArrowBegin64();
    body={};body.stamp={1,10,1000,reinterpret_cast<std::uintptr_t>(character.data())};body.valid[0]=body.valid[1]=true;
    body.allowed[0].t[0]=1;body.allowed[0].t[1]=2;body.allowed[0].t[2]=3;
    frame=PhysFrame6698{};frame.input.allowed=true;frame.input.epoch=frame.input.bodyEpoch=1;frame.input.tick=1000;frame.input.sample=10;
    frame.input.hands[0].p={8,9,10}; // far-away controller intent must not determine achieved nocking
    bowProfile={};bowProfile.classId=9;bowProfile.key=20;bowProfile.generation=1;bowProfile.bowNockQualified40=true;bowProfile.support=true;
    bowOutput=ph::Output{};bowOutput.active[1]=true;bowOutput.weapons[1].p={1,2,3};frame.profiles[1]=bowProfile;frame.output=bowOutput;
    kcd2vr::arrow_hold_d48::nativeTurn.store(false);reference[0]={};
    original[0]={};original[0].t[0]=.1f;helper={};helper.t[0]=-.11f;world={};placed=childPlaced=helperCalls=0;entityWorld={};
    ManualArrowPublish64(bow,frame,1000,10);
}
static void bootstrap(){
    ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==0,"first native socket waits for original joint transaction");
    BuildManualArrowPoses64(body,original,1,frame.input,bowOutput,bowProfile,reference,0);
    ck(body.arrow64.valid&&body.arrow64.bowValid,"same original joints produce exact current hand-arrow pose and bow target");
    ck(ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"native arrow entity receives exactly one placement");
}
int main(){
    { // D48: exercise the actual pose builder and final native attachment owner.
      reset();bootstrap();QuatT28 animation[2]{},anatomy[2]{};
      anatomy[0].t[0]=.1f; // slot is joint0, known hand is joint1
      const auto before=body.arrow64.model;
      const double half=std::sqrt(.5);animation[0].q[2]=float(half);animation[0].q[3]=float(half);
      body.arrow64={};BuildManualArrowPoses64(body,animation,2,frame.input,bowOutput,bowProfile,anatomy,1);
      ck(body.arrow64.valid&&std::abs(body.arrow64.model.q[2]-before.q[2])<1e-6,"native slot turn cannot rotate stable held arrow");
      ck(ManualArrowAttachment64(object.data(),attachment.data()),"stable reference feeds the actual attachment placement");
      ManualArrowAchieved64 achieved{};g_manual_arrow_achieved64.read(achieved);
      ck(achieved.valid&&near(achieved.arrowNock,{1,2,3}),"stable orientation retains slt_0 at committed wrist");
      kcd2vr::arrow_hold_d48::nativeTurn.store(true);body.arrow64={};
      BuildManualArrowPoses64(body,animation,2,frame.input,bowOutput,bowProfile,anatomy,1);
      ck(body.arrow64.valid&&std::abs(body.arrow64.model.q[2])>.7f,"live comparison selects native slot turn without restart");
      kcd2vr::arrow_hold_d48::nativeTurn.store(false);body.arrow64={};
      BuildManualArrowPoses64(body,animation,2,frame.input,bowOutput,bowProfile,nullptr,-1);
      ck(!body.arrow64.valid,"missing reference cannot silently reuse animated grip or a previous placed pose");
      // Distinct anatomy hand index is used even when two current bones match.
      anatomy[1].q[2]=float(half);anatomy[1].q[3]=float(half);animation[0]=animation[1]={};body.arrow64={};
      BuildManualArrowPoses64(body,animation,2,frame.input,bowOutput,bowProfile,anatomy,1);
      ck(body.arrow64.valid&&body.arrow64.model.q[2]<-.7f,"explicit hand joint survives duplicate current bone poses");
    }
    reset();bootstrap();ManualArrowAchieved64 a{};g_manual_arrow_achieved64.read(a);
    nativePassengerA34=true;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,
        "native passenger yields current arrow placement without changing native ammo lifetime");nativePassengerA34=false;
    ck(a.valid&&near(a.arrowNock,{1,2,3})&&a.gap<1e-5,"actual slt_0 nock endpoint aligns at VR wrist, not mesh origin");
    ck(ManualArrowNockGap64(bow,frame)<1e-5,"nocking consumes achieved helper rather than far-away controller intent");
    ck(helperCalls==1,"same native arrow helper is cached, not polled every callback");
    ManualArrowAttachment64(object.data(),attachment.data());ck(placed==2&&helperCalls==1,"repeated attachment callback has one writer and no repeated helper lookup");
    reset();const double q=std::sqrt(.5);world.q={0,0,q,q};world.t={10,20,30};world.scale=2;naturalScale=2;bootstrap();g_manual_arrow_achieved64.read(a);
    ck(a.valid&&near(a.arrowNock,{6,22,36})&&near(a.bowNock,{6,22,36}),"current rotated translated scaled parent and entity scale compose once");
    entityWorld.m[3]=500;BuildManualArrowPoses64(body,original,1,frame.input,bowOutput,bowProfile,reference,0);
    ManualArrowAttachment64(object.data(),attachment.data());g_manual_arrow_achieved64.read(a);
    ck(near(a.arrowNock,{6,22,36}),"already-posed destination is never used as another VR delta source");
    reset();original[0].t[0]=-.4f;helper.t[0]=-.0009f;helper.t[1]=-.645f;helper.t[2]=-.0006f;
    bootstrap();g_manual_arrow_achieved64.read(a);
    ck(a.valid&&near(a.arrowNock,{1,2,3})&&a.gap<1e-5,"retail .645m rear nock and shaft offset grip at committed wrist, not a metre away");
    ck(ManualArrowNockGap64(bow,frame)<=.14,"actual rear nock enters unchanged physical nock gate");
    ck(std::fabs(entityWorld.m[0]-1.f)<1e-5&&std::fabs(entityWorld.m[5]-1.f)<1e-5,"nock anchor preserves qualified native arrow orientation");
    { // Connect the real achieved endpoint consumer to the real loader policy.
      namespace reload=kcd2vr::staged596c2;reload::Loader loader;
      ck(loader.update(1000,true,false,false,0,true,100.,false,false,true)==reload::Command::None,"empty quiver owner observes neutral");
      ck(loader.update(1020,true,false,false,1,true,100.,false,false,true)==reload::Command::Retrieve,"intentional shoulder close requests one native arrow");
      ck(loader.update(1040,true,false,true,1,true,ManualArrowNockGap64(bow,frame),true,false,true)==reload::Command::None,"attached arrow cannot nock inside quiver");
      ck(loader.update(1060,true,false,true,1,false,ManualArrowNockGap64(bow,frame),true,false,true)==reload::Command::None,"held arrow leaves quiver with actual endpoint near string");
      ck(loader.update(1080,true,false,true,0,false,ManualArrowNockGap64(bow,frame),true,false,true)==reload::Command::Nock,"release of actual held rear nock requests native transfer once");
      ck(loader.update(1100,true,false,false,0,false,100.,false,true,true)==reload::Command::None&&loader.state==reload::Reload::AwaitNock,"submitted transfer neither repeats nor grants readiness without attachment");
      ck(loader.update(1120,true,true,false,0,false,0.,false,false,true)==reload::Command::None&&loader.state==reload::Reload::Ready,"only actual native nock attachment completes connected load policy");
    }
    reset();original[0].t[0]=-.4f;helper.t[1]=-.645f;world.q={0,0,q,q};world.t={10,20,30};world.scale=2;naturalScale=2;
    put(object,0x0C,1.2f);put(object,0x10,.8f);put(object,0x14,.9f);bootstrap();g_manual_arrow_achieved64.read(a);
    ck(a.valid&&near(a.arrowNock,{6,22,36})&&a.gap<1e-5,"rear nock anchor applies rotated parent and native per-axis scale exactly once");
    reset();bootstrap();put(object,0x10,0.f);ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"invalid native attachment scale cannot invent a nock placement");
    ck(ManualArrowNockGap64(bow,frame)==100.,"known invalid current scale revokes earlier achieved nock receipt");
    reset();child=true;bootstrap();ck(childPlaced==1,"optional animated child shares same placement owner");
    reset();helperOK=false;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==0,"missing native helper never invents nock geometry");
    reset();bootstrap();readbackOK=false;ck(ManualArrowAttachment64(object.data(),attachment.data()),"failed readback after write still owns callback, no second native placement");
    ck(ManualArrowNockGap64(bow,frame)==100.,"failed achieved readback revokes nocking");
    reset();bootstrap();ManualArrowWithdraw64();ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"withdraw before native removal prevents further writes");
    reset();bootstrap();entityLookup=other.data();ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"reused entity ID cannot move replacement");
    reset();bootstrap();put(model,0x2C,std::uint32_t(88));ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"native hand replacement revokes presentation");
    reset();bootstrap();put(object,8,std::uint32_t(88));ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"foreign attachment object remains native");
    reset();bootstrap();++g_weapon_request_epoch6694;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"new weapon request invalidates old lease");
    reset();bootstrap();++g_pose_epoch6689;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"recenter/body epoch invalidates old lease");
    reset();bootstrap();now=1081;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"original source expires without lease restamping");
    reset();bootstrap();now=999;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"future source is refused");
    reset();bootstrap();tracked=false;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"tracking loss cannot borrow old placed arrow");
    reset();bootstrap();g_gameplay_allowed=false;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"menu/native owner revokes arrow placement");
    reset();bootstrap();staged=false;ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"automatic-loading policy restores native placement");
    reset();bootstrap();bow.arrowSerial64=ManualArrowBegin64();ManualArrowPublish64(bow,frame,1000,10);
    ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"new creation serial cannot borrow earlier committed socket pose");
    reset();bootstrap();QuatT28 changed{};changed.t[0]=.02f;put(attachment,0x9C,changed);put(attachment,0x7A,static_cast<unsigned char>(1));
    ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"current socket change waits for new original-joint correlation");
    body.arrow64={};BuildManualArrowPoses64(body,original,1,frame.input,bowOutput,bowProfile,reference,0);
    ck(body.arrow64.valid&&ManualArrowAttachment64(object.data(),attachment.data()),"pending native flag does not starve already-current socket bootstrap");
    frame.input.sample=9;ck(ManualArrowNockGap64(bow,frame)==100.,"future achieved pose cannot nock an older input");
    reset();bootstrap();
    {std::promise<void> locked,finish;auto done=finish.get_future();
     std::thread other([&]{std::lock_guard<std::recursive_mutex> owner(g_manual_arrow_mutex64);locked.set_value();done.wait();});
     locked.get_future().wait();
     ck(ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"contended current arrow callback cannot add a second native writer");
     put(model,0x2C,std::uint32_t(88));
     ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"contended foreign/replaced hand attachment retains native authority");
     finish.set_value();other.join();}
    reset();namespace hp=kcd2vr::haptics101;hp::Pulse pulse{};
    hp::queue[0].publish({});hp::enabled=false;hp::allowed=true;ManualArrowCue64(frame);hp::queue[0].read(pulse);
    ck(!pulse.key,"nock cue respects disabled haptics");
    hp::enabled=true;hp::allowed=false;ManualArrowCue64(frame);hp::queue[0].read(pulse);
    ck(!pulse.key,"nock cue respects native/menu haptic revocation");
    hp::allowed=true;ManualArrowCue64(frame);hp::queue[0].read(pulse);
    ck(hp::eligible(pulse,frame.input.tick)&&pulse.key==frame.profiles[1].key&&pulse.sequence!=(frame.input.sample)&&pulse.durationMs==25,"nock cue uses unique bounded ordinary haptic event");

    reset();bootstrap();hand_claim_fixture_t24::Publish(0);
    ck(!ManualHandAttached19c4r4(bow),"T24 claimed right hand refuses actual binding owner");
    ck(!ManualArrowAttachment64(object.data(),attachment.data())&&placed==1,"T24 actual arrow consumer cannot place over same-hand preview");
    reset();bootstrap();hand_claim_fixture_t24::Publish(1);
    ck(ManualHandAttached19c4r4(bow),"T24 opposite left preview preserves native right binding");
    ck(ManualArrowAttachment64(object.data(),attachment.data())&&placed==2,"T24 opposite left claim preserves actual right-arrow placement");
    std::printf("manual_arrow64=%u/%u native_execution=NONE\n",checks,checks);
}
