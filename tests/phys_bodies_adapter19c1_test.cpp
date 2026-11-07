#include "vrik/phys_bodies59c10r8.h"
#include "vrik/physical_weapon.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <string>
#ifdef _MSC_VER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
namespace pb=kcd2vr::phys_bodies59c10r8;
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
#include "phys_bodies_adapter19c1_types.inc"
// Explicit copied-publication leaves; all target transforms, owner policy,
// creation order, Drive and complete Tick below are production extraction.
template<class T>struct Mailbox19 {T value{};bool available=true;void(*afterRead)()=nullptr;
    bool read(T& out){if(!available)return false;out=value;if(afterRead){auto fn=afterRead;afterRead=nullptr;fn();}return true;}};
static Mailbox19<PhysFrame6698> g_phys_frame6698;
static Mailbox19<PhysEquipment6698> g_phys_equipment6698;
static std::atomic<std::uint64_t> g_phys_ready_sample6698{0},g_weapon_request_epoch6694{1},g_pose_epoch6689{1};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_phys_fault6698{false},g_weapon_fault6694{false};
static std::atomic<void*> g_henry_item_attach_manager{nullptr},g_committed_character6689{nullptr},g_last_player_entity{nullptr};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace pb59c10r8 {} // Tick's production using-directive; adapter definitions are extracted in this test namespace.
static int cookie,body,geometry,worldA,worldB;
static int& g_cookie=cookie;
static void* currentWorld=&worldA;
static void* registered=&body;
static bool binding=true,slots=true,foreign=true,magic=true;
static unsigned destroys=0,queries=0,slotReads=0,foreignReads=0,flagWrites=0,createCalls=0;
static bool readable=true,actionFault=false;static int destroyResult19=1;
static unsigned poseWrites=0,actions=0,latches=0;
static pb::ActionSetVelocity velocity{};
static pb::ParamsFlags flags{};
static pb::ParamsCollisionClass collisionClass{};
static pb::ParamsPos position{};
static std::string receipt;
static std::uintptr_t WhBase(){return 0x180000000ull;}
static bool Bind(std::uintptr_t,bool cleanup=false);
static bool RigidSlots(void*,std::uintptr_t){++slotReads;return slots;}
static bool RigidVtable(void*,std::uintptr_t){return slots;}
static void* CallCreate(void*,void* p){++createCalls;position=*static_cast<pb::ParamsPos*>(p);return &body;}
static int CallDestroy(void*,void*){++destroys;return destroyResult19;}
static void* CallGetById(void*,int){++queries;return registered;}
static void* CallPrim(void*,int,const void*){return &geometry;}
static void* CallReg(void*,void*){return &geometry;}
static int CallGeomRelease(void*){return 1;}
static int CallAddGeom(void*,void*,void*){return 0;}
static int CallUnreg(void*,void*){return 1;}
static int CallSetParams(void*,void* p,int ts){assert(ts==0);if(*static_cast<int*>(p)==15){flags=*static_cast<pb::ParamsFlags*>(p);++flagWrites;}
    if(*static_cast<int*>(p)==26)collisionClass=*static_cast<pb::ParamsCollisionClass*>(p);
    if(*static_cast<int*>(p)==0){position=*static_cast<pb::ParamsPos*>(p);++poseWrites;}return 1;}
static int CallAction(void*,void* p,int threadSafe){assert(threadSafe==0);velocity=*static_cast<pb::ActionSetVelocity*>(p);++actions;return actionFault?-999:1;}
static bool ReadPos(void*,pb::V3f& p,pb::Qf& q){p=position.pos;q=position.q;return readable;}
static void* CallForeign(void*){++foreignReads;return foreign?&g_cookie:nullptr;}
static int CallIForeign(void*){return magic?pb::kForeignMagic:0;}
static int CallGetId(void*,void*){return 71;}
static void Latch(const char* reason){++latches;receipt=reason;}
static std::uint64_t now19=100,palmTick19=100;
static bool palm19[2]{false,false};static pb::V3f palmPosition19[2]{};static bool changeHandOwner19=false,changeHandWorld19=false;
static bool headUsable19=true,handUsable19[2]{true,true};
static bool TrackingHeadUsable111(std::uint64_t=0){return headUsable19;}
static bool TrackingHandUsable111(int side,std::uint64_t=0){return handUsable19[side];}
static std::uint64_t GetTickCount64(){return now19;}
static int CallGetStatus(void*,void*){return 0;}
static bool ReadInteractionPalm59c9(int side,float& x,float& y,float& z,float& dx,float& dy,float& dz,std::uint64_t& tick){
    if(!palm19[side]||!handUsable19[side]||!headUsable19)return false;auto p=palmPosition19[side];x=p.x;y=p.y;z=p.z;dx=0;dy=1;dz=0;tick=palmTick19;
    if(changeHandOwner19)g_pose_epoch6689.fetch_add(1);if(changeHandWorld19)currentWorld=&worldB;return true;
}
static void Log(const char* m){receipt=m;}
namespace diagnostic_trace {static void Write(const char* m){Log(m);}}
static pb::ParamsPos PosParams(pb::V3f p,pb::Qf q){pb::ParamsPos x{};x.pos=p;x.q=q;return x;}
#include "phys_bodies_adapter19c1.inc"
static bool Bind(std::uintptr_t,bool cleanup){
    (void)cleanup; // Complete production Tick also binds before any creation.
    if(!binding)return false;
    g_state.world=currentWorld;return true;
}
static void reset(){
    g_state={};g_state.world=&worldA;g_state.geoman=&geometry;
    currentWorld=&worldA;registered=&body;binding=slots=foreign=magic=true;
    destroys=queries=slotReads=foreignReads=flagWrites=0;receipt.clear();
    readable=true;actionFault=false;destroyResult19=1;poseWrites=actions=latches=0;position={};velocity={};
    createCalls=0;collisionClass={};now19=palmTick19=100;palm19[0]=palm19[1]=false;changeHandOwner19=changeHandWorld19=false;
    headUsable19=handUsable19[0]=handUsable19[1]=true;
    g_phys_frame6698={};g_phys_equipment6698={};g_phys_ready_sample6698=0;g_weapon_request_epoch6694=1;g_pose_epoch6689=1;
    g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;g_phys_fault6698=g_weapon_fault6694=false;
    animation_filter::nativeBodyOwned112=false;g_henry_item_attach_manager=nullptr;g_committed_character6689=nullptr;g_last_player_entity=nullptr;g_mode59c10r8=0;
}
static int player19,manager19,character19,entity19;
static void blade19(){
    now19=1000;g_mode59c10r8=1;g_phys_ready_sample6698=31;g_henry_item_attach_manager=&manager19;
    g_committed_character6689=&character19;g_last_player_entity=&entity19;
    auto& f=g_phys_frame6698.value;f={};f.input.tick=1000;f.input.sample=31;f.input.epoch=f.request=1;f.input.bodyEpoch=1;
    f.input.allowed=f.input.tracked[0]=f.input.haveWorld=true;f.input.worldScale=1;
    f.output.active[0]=f.output.owned[0]=true;f.output.weapons[0].p={1,2,3};
    f.player=&player19;f.manager=&manager19;auto& p=f.profiles[0];p.key=7;p.generation=8;p.entity=9;p.classId=1;p.primary=true;p.side=0;p.tip={0,1,0};
    auto& e=g_phys_equipment6698.value;e={};e.valid=true;e.tick=1000;e.request=e.epoch=1;e.player=f.player;e.manager=f.manager;
    e.handKnown[0]=true;e.handKey[0]=7;e.count=1;e.heldIndex[0]=0;e.items[0].held=true;e.items[0].profile=p;
    // Native equipment metadata has no captured grip. Only the completed frame
    // has profile.primary; requiring it on metadata would reject every blade.
    e.items[0].profile.primary=false;
}
#ifdef _MSC_VER
namespace queueLeaves19c4r3 {
using ActionFn=int(*)(void*,void*,int);using SetParamsFn=int(*)(void*,void*,int);using DestroyFn=int(*)(void*,void*,int,int);
struct Api {DestroyFn destroy=nullptr;} g_api;
#include "phys_bodies_adapter19c4r3_leaves.inc"
static int calls=0;static pb::ActionSetVelocity queuedAction{};static pb::ParamsPos queuedPos{};
static int action(void*,void* a,int ts){assert(ts==0);queuedAction=*static_cast<pb::ActionSetVelocity*>(a);++calls;return 1;}
static int params(void*,void* p,int ts){assert(ts==0);queuedPos=*static_cast<pb::ParamsPos*>(p);++calls;return 1;}
static int destroy(void*,void*,int mode,int ts){assert(mode==0&&ts==0);++calls;return 1;}
static void run(){std::uintptr_t vt[8]{};vt[4]=reinterpret_cast<std::uintptr_t>(&params);vt[7]=reinterpret_cast<std::uintptr_t>(&action);void* entity=vt;
    pb::ActionSetVelocity a{};a.v={1,2,3};assert(CallAction(&entity,&a,0)==1);a.v={};assert(queuedAction.v.y==2);
    pb::ParamsPos p{};p.pos={4,5,6};assert(CallSetParams(&entity,&p,0)==1);p.pos={};assert(queuedPos.pos.z==6);
    g_api.destroy=&destroy;assert(CallDestroy(nullptr,&entity)==1&&calls==3);
}
}
#endif
int main(){
#ifdef _MSC_VER
    queueLeaves19c4r3::run();
#endif
    for(int mode=1;mode<=1;++mode){
        reset();Body b{};
        assert(CreateBody(b,WhBase(),mode,{1,2,3},{},.5f,.025f,1.6f));
        assert(b.world==&worldA&&flagWrites==1&&flags.type==15);
        assert(flags.flags==0x80000000u&&flags.flagsAND==0xffffffffu);
        const std::uint32_t prior=pb::kPefTraceable|0x12u;
        const auto after=(prior&flags.flagsAND)|flags.flagsOR;
        assert((after&prior)==prior);
        assert(bool(after&pb::kPefLogCollisions)==(mode>=2));
        assert((collisionClass.orBits.ignore&pb::kCollClassLiving)!=0);
        DestroyBody(b,"MODE_OFF");
        assert(destroys==1&&g_state.destroys==1&&b.ent&&b.destroyRequested);
        DestroyBody(b,"STILL_PENDING");assert(destroys==1&&b.ent);
        registered=nullptr;DestroyBody(b,"ACK_REGISTRY_GONE");assert(!b.ent&&!b.world&&destroys==1);
        assert(receipt.find("REQUEST_RETIRED_FROM_REGISTRY")!=std::string::npos);
    }
    for(unsigned refusal=0;refusal<7;++refusal){
        reset();Body b{};b.ent=&body;b.world=&worldA;b.id=71;
        if(refusal==0)binding=false;
        if(refusal==1)currentWorld=&worldB;
        if(refusal==2)registered=nullptr;
        if(refusal==3)slots=false;
        if(refusal==4)foreign=false;
        if(refusal==5)magic=false;
        if(refusal==6)b.id=-1;
        DestroyBody(b,"BINDING_LOST");
        assert(destroys==0&&g_state.lost==1&&!b.ent);
        assert(receipt.find("IDENTITY_LOST_NOT_TOUCHED")!=std::string::npos);
        if(refusal<=1||refusal==6)assert(queries==0&&slotReads==0&&foreignReads==0);
        if(refusal==2)assert(slotReads==0&&foreignReads==0);
        if(refusal==3)assert(foreignReads==0);
    }
    reset();Body b{};b.ent=&body;b.world=&worldA;b.id=71;
    g_state.latched=true;DestroyBody(b,"LATCHED");assert(destroys==1);
    reset();b={};b.ent=&body;b.world=&worldA;b.id=71;g_state.world=&worldB;
    assert(!StillOurs(b,WhBase())&&queries==0&&destroys==0&&!b.ent);
    for(int result:{0,-999,1}){reset();b={};b.ent=&body;b.world=&worldA;b.id=71;b.mode=1;destroyResult19=result;
        DestroyBody(b,"ASYNC");assert(b.destroyRequested&&b.ent&&destroys==1);
        pb::Target pending{true,{.1f,0,0},{},100};Drive(b,pending,110);assert(actions==0&&poseWrites==0);
        const auto reads=slotReads;DestroyBody(b,"NO_RETRY");assert(destroys==1&&slotReads==reads);
        currentWorld=&worldB;DestroyBody(b,"WORLD_CHANGED");assert(!b.ent&&destroys==1);
    }
    // Exercise the extracted production Drive, not a mirrored policy helper.
    // Existing contact bodies may park or velocity-drive, never reseed blindly.
    for(int mode=1;mode<=2;++mode)for(int missing=0;missing<2;++missing){
        reset();b={};b.ent=&body;b.mode=mode;b.kind=pb::Kind::Blade;
        pb::Target t{true,{1,0,0},{},100};readable=!missing;
        Drive(b,t,110);assert(actions==1&&latches==0);
        assert(velocity.type==10&&velocity.ipart==pb::kUnusedI&&velocity.partid==pb::kUnusedI);
        assert(pb::len(velocity.v)==0&&pb::len(velocity.w)==0);
        if(mode==2){assert(poseWrites==0&&b.teleports==0&&b.parks==1&&position.pos.x==0);}
        else{assert(poseWrites==1&&b.teleports==1&&b.parks==0&&position.pos.x==1);}
    }
    reset();b={};b.ent=&body;b.mode=2;b.kind=pb::Kind::Blade;
    pb::Target t{true,{.1f,0,0},{0,0,.70710678f,.70710678f},100};
    Drive(b,t,110);assert(poseWrites==0&&b.velocities==1&&actions==1&&velocity.v.x>0&&velocity.w.z>0);
    t.tick=1;Drive(b,t,110);assert(poseWrites==0&&b.parks==1&&pb::len(velocity.v)==0&&pb::len(velocity.w)==0);
    reset();b={};b.ent=&body;b.mode=2;readable=false;actionFault=true;
    t.tick=100;Drive(b,t,110);assert(poseWrites==0&&actions==1&&b.parks==0&&latches==1&&receipt=="PARK_FAULT");
    // Complete actual BladeTarget + Tick: original stale/future publications
    // and invalidated owners must cause ZERO native body creations.
    for(unsigned refusal=0;refusal<37;++refusal){reset();blade19();auto& f=g_phys_frame6698.value;auto& e=g_phys_equipment6698.value;
        switch(refusal){
        case 0:f.input.tick=1;break;case 1:f.input.tick=1100;break;
        case 2:g_phys_ready_sample6698=0;break;case 3:g_phys_ready_sample6698=32;break;
        case 4:f.request=2;break;case 5:g_pose_epoch6689=2;break;case 6:f.input.allowed=false;break;
        case 7:f.input.tracked[0]=false;break;case 8:f.output.owned[0]=false;break;case 9:f.output.active[0]=false;break;
        case 10:g_gameplay_allowed=false;break;case 11:animation_filter::nativeBodyOwned112=true;break;
        case 12:g_henry_item_attach_manager=nullptr;break;case 13:e.player=nullptr;break;
        case 14:e.handKey[0]=11;break;case 15:e.items[0].profile.generation=10;break;
        case 16:e.tick=1;break;case 17:e.items[0].held=false;break;case 18:e.heldIndex[0]=8;break;
        case 19:f.input.worldScale=std::numeric_limits<double>::infinity();break;
        case 20:f.output.weapons[0].p.x=std::numeric_limits<double>::quiet_NaN();break;
        case 21:f.profiles[0].tip.y=std::numeric_limits<double>::quiet_NaN();break;
        case 22:f.input.world.q.w=0;break;case 23:f.profiles[0].primary=false;break;
        case 24:g_ctx_dialogue=true;break;case 25:g_ctx_cutscene=true;break;
        case 26:g_phys_equipment6698.afterRead=[]{g_phys_ready_sample6698=32;};break;
        case 27:g_phys_equipment6698.afterRead=[]{g_weapon_request_epoch6694=2;};break;
        case 28:headUsable19=false;break;case 29:handUsable19[0]=false;break;
        case 30:g_phys_fault6698=true;break;case 31:g_phys_equipment6698.afterRead=[]{g_mode59c10r8=0;};break;
        case 32:e.tick=1100;break;case 33:g_phys_equipment6698.afterRead=[]{handUsable19[0]=false;};break;
        case 34:g_phys_equipment6698.afterRead=[]{g_henry_item_attach_manager=nullptr;};break;
        case 35:g_phys_equipment6698.afterRead=[]{animation_filter::nativeBodyOwned112=true;};break;
        case 36:g_phys_equipment6698.afterRead=[]{currentWorld=&worldB;};break;
        }
        TickPhysBodies59c10r8();assert(createCalls==0&&g_state.creates==0&&!g_state.bodies[0].ent);
    }
    reset();blade19();pb::Target target{};float hh=0;BladeOwner19c2 owner{};
    assert(BladeTarget(target,hh,owner,now19)&&target.tick==1000&&std::fabs(hh-.5f)<1e-6f);
    TickPhysBodies59c10r8();assert(createCalls==1&&g_state.creates==1&&g_state.bodies[0].ent&&g_state.bodies[0].velocities==1);
    assert(position.pos.x==1&&position.pos.y==2.5f&&position.pos.z==3);
    // Complete Tick must retain a queued destroy without driving, touching its
    // body fields, recreating it, or submitting destroy a second time.
    g_mode59c10r8=0;TickPhysBodies59c10r8();assert(destroys==1&&g_state.bodies[0].destroyRequested&&!g_retiredT4.load());
    const auto actionCount=actions,readCount=slotReads;
    TickPhysBodies59c10r8();assert(destroys==1&&actions==actionCount&&slotReads==readCount);
    g_mode59c10r8=1;TickPhysBodies59c10r8();assert(createCalls==1&&destroys==1&&actions==actionCount&&slotReads==readCount);
    registered=reinterpret_cast<void*>(-1);TickPhysBodies59c10r8();
    assert(createCalls==1&&g_state.bodies[0].ent&&g_state.bodies[0].destroyRequested&&destroys==1&&actions==actionCount&&slotReads==readCount);
    binding=false;TickPhysBodies59c10r8();assert(g_state.bodies[0].destroyRequested&&createCalls==1&&destroys==1);
    binding=true;g_mode59c10r8=0;registered=nullptr;TickPhysBodies59c10r8();assert(!g_state.bodies[0].ent&&destroys==1&&g_retiredT4.load());
    // No verified capsule/sphere overlap-clearance backend exists. This is
    // specifically the optional proxy; normal VR weapon queries are untouched.
    reset();blade19();g_mode59c10r8=2;TickPhysBodies59c10r8();
    assert(createCalls==0&&g_state.creates==0&&receipt.find("CLEARANCE_UNAVAILABLE")!=std::string::npos);
    // Actual hand producer preserves its original time and refuses stale/future
    // or changed body generation before any native creation.
    for(unsigned refusal=0;refusal<5;++refusal){reset();now19=1000;g_mode59c10r8=1;palm19[0]=true;
        g_committed_character6689=&character19;g_last_player_entity=&entity19;
        palmTick19=refusal==0?1:refusal==1?1100:1000;
        if(refusal==2)palmPosition19[0].x=std::numeric_limits<float>::quiet_NaN();
        if(refusal==3)changeHandOwner19=true;
        if(refusal==4)changeHandWorld19=true;
        TickPhysBodies59c10r8();assert(createCalls==0&&g_state.creates==0);
    }
    reset();now19=palmTick19=1000;g_mode59c10r8=1;palm19[0]=true;palmPosition19[0]={3,4,5};
    g_committed_character6689=&character19;g_last_player_entity=&entity19;
    TickPhysBodies59c10r8();assert(createCalls==1&&g_state.bodies[1].ent&&g_state.bodies[1].velocities==1);
    std::puts("Current production proxy flags/cleanup/Drive and complete BladeTarget/HandTarget/Tick precreation eligibility PASS");
}
