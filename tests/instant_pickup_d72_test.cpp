// Production D72 native adapter, native ABI leaves replaced explicitly. This
// executes neither the game nor Windows SEH. Parent scheduling/transfer/notice
// mocks make ownership, identity and one-shot failures observable.
#include <array>
#include <atomic>
#include <barrier>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#define __fastcall
#define __try try
#define __except(...) catch(...)
#define __finally catch(...) {} if(true)
#define EXCEPTION_EXECUTE_HANDLER 1
#define KCD2VR_INSTANT_PICKUP_HOST_TEST
#include "../src/core/snapshot_mailbox.h"
#include "../src/core/instant_pickup_d72.h"
#include "../src/diagnostics/capture168.h"
#include "../src/vrik/instant_activities_d72.h"
#include "../src/vrik/physical_weapon.h"
#include "../src/subsystems/throw_drop/hand_claim_t24.h"
#include "../src/subsystems/physics/corpse_grip_cg1.h"
namespace kcd2vr::reload_throw_claim_t25 {
enum class Admission {Unknown,Allowed,Denied};
static Admission observeReloadDebt(int,std::uintptr_t){return Admission::Allowed;}
}
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace DetourModKit {
struct Address {std::uintptr_t p;};
namespace hook {
struct Target {Address a;};struct Options {};
struct InlineRequest {const char* name;Target target;Options options;};
struct Hook {bool live=false;template<class T>T original()const{return nullptr;}
    bool enable(){live=true;return true;}bool disable(){live=false;return true;}bool is_enabled()const{return live;}};
template<class T>std::optional<Hook> inline_at(InlineRequest&&,T){return Hook{};}
}}
namespace diagnostic_trace {
static std::vector<std::string> writesD84;
static void Write(const char* line){writesD84.emplace_back(line);}
}
namespace action_time_owner59c11 {
static bool Installed(){return true;}static bool NativePickupOutstandingD72(){return false;}
// Native side-transport leaf added after the original D72 fixture. The adapter
// still executes its production recipient selection; only this input is mocked.
static int requestedSide=-1;
static bool RequestedPickupSideD85(std::uint32_t entity,int& side){
    side=requestedSide;return entity==3&&side>=0&&side<2;
}
}
namespace retail_build112 {static bool Ready(std::uintptr_t){return true;}}
namespace kcd2vr::workstation178r20c1 {
inline std::atomic<std::uint64_t> experimentEpoch36{9};
static bool ownsWrist(std::uint64_t){return false;} // F5 OFF must not gate the explicit instant owner
}
struct StagedProp596 {
    void* entity=nullptr;void* controller=nullptr;void* item=nullptr;
    std::uint32_t id=0,actor=0;std::uint64_t key=0,epoch=0,sample=0;
    int side=-1;void* body=nullptr;std::uintptr_t character=0;std::uint64_t bodyEpoch=0;
    bool physics=true,instantD72=false,noticeD72=false;std::uint64_t instantTokenD72=0;
    bool kinematicR96=false; // R153: firm grip = carried by entity transform from the first frame
    void* managerD72=nullptr;void* playerD72=nullptr;void* containerD72=nullptr;void* ownerD72=nullptr;void* inventoryD72=nullptr;
    std::uint32_t countD72=0;alignas(8) unsigned char acceptedD72[0xC8]{};
    kcd2vr::instant_pickup_d72::Transfer transferD72=nullptr;
};
static StagedProp596 g_prop596;
static thread_local bool g_prop_replay596=false;
struct PropLock596 {bool owned=true;};
struct CommittedFrame6695 {
    float pickupTriggerD80[2]{};
    struct Stamp {std::uint64_t tick=1000,actor=11,epoch=12;} stamp;
    bool valid[2]{true,true},mapFoldTrackedM39=true,headEvidence122=true,propRequestedValid5916c1[2]{true,true};
    ph::Pose requested[2]{},visible[2]{};
    float mapGripM39[2]{0,1};
};
static std::atomic<void*> g_henry_item_attach_manager{nullptr},g_last_player_entity{nullptr};
struct Fixture;
static Fixture* fixture=nullptr;
static std::uintptr_t WhBase();
static std::uint64_t GetTickCount64();
template<class T>static bool Read(const void* p,T& out){if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
static bool Safe(const void* p,std::size_t){return p!=nullptr;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t value=0;return Read(p,value)&&value==WhBase()+rva;}
static bool PropIdentity596(const StagedProp596&);
static bool PropHandFree596(int);
static bool selectedFistsD80=false;
static bool FistInputOwned101(){return selectedFistsD80;}
#include "../src/vrik/pickup_input_d80.h"
static bool PropInstantContextD78(std::uint64_t){return true;} // actual context/build/hold adapter tested in D78 fixture
static bool ReadCommittedPacket6695(CommittedFrame6695&);
static bool PropObjectPose5916C1(const StagedProp596&,ph::Pose& p){p={};return true;}
static ph::Pose PropWorldHand596(const CommittedFrame6695&,int){return {};}
static bool PropPickupHandD81(const CommittedFrame6695& f,int side,bool physics,ph::Pose& out){
    if(side<0||side>1||!f.valid[side]||(physics&&!f.propRequestedValid5916c1[side]))return false;
    out=physics?f.requested[side]:f.visible[side];return ph::finite(out);
}
static bool BuildStagedPickup596(void*,std::uint32_t,bool,StagedProp596*);
namespace kcd2vr::prop5916c1 {inline std::atomic<bool> firmHoldR116{true};} // R153: VrPropFirmHoldR116
namespace prop5916=kcd2vr::prop5916c1;
static bool PropVelocity596(void*){return true;}
static void PropNote596(const char*,const StagedProp596&,bool,const char*){}
static bool LifecycleHenry165(std::uint32_t);
static void PropRetireInstantD72(const StagedProp596&) noexcept;
// R155: the hold asks the R103 self-collision lease (no collision with Henry) at acceptance and releases it when it
// retires. Here the two calls are recorded; the pure lease table (self_collision_r103.h) is checked at the end of main.
#include "../src/vrik/self_collision_r103.h"
namespace self_collision_r103n {
static unsigned excludedOwners=0,releasedOwners=0,excludeCalls=0;
static void Release(unsigned owner,std::uint64_t) noexcept {releasedOwners|=owner;}
}
static void PropSelfCollisionR155(const StagedProp596& p,std::uint64_t) noexcept { // same guard as staged_pickup596c2.inl
    if(!p.instantD72||!p.id||!p.entity||!p.body||p.side<0||p.side>1)return;
    self_collision_r103n::excludedOwners|=kcd2vr::self_collision_r103::instantOwner(p.side);++self_collision_r103n::excludeCalls;
}
struct StageLeafPin596 {std::uintptr_t rva;std::array<unsigned char,8> bytes;};
template<std::size_t N>static bool StageLeafPins596(const StageLeafPin596(&)[N]){return true;}
static std::uintptr_t HostInstantPickupLeafD72(std::uintptr_t);
static std::uintptr_t HostInstantPickupCallerD72();
#include "../src/vrik/instant_pickup_native_d72.inl"

struct Fixture {
    std::vector<unsigned char> image=std::vector<unsigned char>(0x5500000);
    std::array<unsigned char,0x300> handler{};
    std::array<unsigned char,0xA00> player{};
    std::array<unsigned char,0x100> manager{},item{},entity{},controller{},actor{},expansion{},inventory{},world{},body{};
    std::array<unsigned char,0x1D8> allocation{};
    CommittedFrame6695 frame{};StagedProp596 seed{};
    std::uint64_t now=1000,key=0x12345678,readAdvanceR96=0;
    bool worldAlive=true,free[2]{true,true},unavailable=false,special=false,noticeFault=false,transferFault=false,duplicate=false;
    bool deferred=true,requestAccepted=true,partial=false,wrongCaller=false;
    int handlers=0,factories=0,choices=0,executions=0,animations=0,completions=0,notices=0,transfers=0,destroys=0,chosenHand=-1;
    std::vector<std::string> events;
    template<class T>void put(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
    void vf(void* p,std::uintptr_t rva){put(p,0,reinterpret_cast<std::uintptr_t>(image.data())+rva);}
    void* action(){return allocation.data()+0x10;}
    Fixture(){
        fixture=this;action_time_owner59c11::requestedSide=-1;g_prop596={};EndInstantPickupRequestD72();g_instant_pickup_lease_d72.publish({});
        self_collision_r103n::excludedOwners=self_collision_r103n::releasedOwners=self_collision_r103n::excludeCalls=0;
        g_instant_pickup_live_d72=0;g_instant_pickup_consumed_d72=0;g_instant_pickup_hold_d72=0;g_instant_pickup_terminal_d72=0;
        g_instant_pickup_ready_d72=true;ipd72::stopping=false;ipd72::active=0;ipd72::noticeScope={};
        for(auto& claim:kcd2vr::mutual_admission_t25::pickupClaimsD72)claim=0;
        for(int side=0;side<2;++side){auto& c=kcd2vr::corpse_grip_cg1::claims;
            c.consumed[side]=false;c.nativeHeld[side]=false;c.fault[side]=false;}
        kcd2vr::instant_d72::activities=true;kcd2vr::instant_d72::interactions=true;
        put(image.data(),0x54961D0,static_cast<void*>(handler.data()));put(image.data(),0x5487A78,static_cast<void*>(world.data()));
        vf(handler.data(),0x4001F10);vf(manager.data(),0x3EC9958);vf(player.data(),0x3AA4B98);vf(item.data(),0x3A2DA88);vf(expansion.data(),0x3B55E30);
        put(handler.data(),0x248,static_cast<void*>(actor.data()));
        put(manager.data(),0x38,static_cast<void*>(player.data()));put(player.data(),0x9D0,static_cast<void*>(manager.data()));
        put(player.data(),0x38,static_cast<void*>(entity.data()));put(item.data(),0x30,key);put(item.data(),0x50,std::uint32_t(3));
        put(item.data(),0x90,static_cast<void*>(world.data()));put(item.data(),0x88,static_cast<void*>(controller.data()));
        put(controller.data(),0x30,std::uint32_t(3));
        g_henry_item_attach_manager=manager.data();g_last_player_entity=entity.data();
        seed.entity=entity.data();seed.controller=controller.data();seed.item=item.data();seed.id=3;seed.actor=42;
        seed.key=key;seed.epoch=12;seed.side=1;seed.body=body.data();seed.character=11;seed.bodyEpoch=12;
    }
};
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(fixture->image.data());}
static std::uint64_t GetTickCount64(){return fixture->now;}
static std::uintptr_t HostInstantPickupCallerD72(){return WhBase()+(fixture->wrongCaller?0x2AC0004:0x2A8A8F4);}
static bool PropIdentity596(const StagedProp596& p){return fixture->worldAlive&&p.entity==fixture->entity.data()&&p.item==fixture->item.data()&&p.key==fixture->key;}
static bool PropHandFree596(int side){return side>=0&&side<2&&fixture->free[side];}
static bool ReadCommittedPacket6695(CommittedFrame6695& f){if(fixture->readAdvanceR96)fixture->now=fixture->readAdvanceR96;f=fixture->frame;return true;}
static bool BuildStagedPickup596(void* controller,std::uint32_t actor,bool,StagedProp596* prepared){
    if(g_prop596.id||!fixture->worldAlive||controller!=fixture->controller.data()||actor!=42)return false;
    *prepared=fixture->seed;return true;
}
static bool LifecycleHenry165(std::uint32_t id){return id==42;}
static void* InventoryLeaf(void*){return fixture->inventory.data();}
static bool FlagLeaf(void*,unsigned){return fixture->unavailable;}
static bool SpecialLeaf(void*,void*,bool){return fixture->special;}
static void* ExpansionLeaf(void*){return fixture->expansion.data();}
static void* ManagerLeaf(void*){return fixture->manager.data();}
static void* ItemLeaf(void*,std::uint64_t key){return key==fixture->key?fixture->item.data():nullptr;}
static std::uintptr_t HostInstantPickupLeafD72(std::uintptr_t rva){
    switch(rva){case 0x8D285C:return reinterpret_cast<std::uintptr_t>(&InventoryLeaf);
        case 0x469590:return reinterpret_cast<std::uintptr_t>(&FlagLeaf);case 0x8CE1F8:return reinterpret_cast<std::uintptr_t>(&SpecialLeaf);
        case 0x8C7758:return reinterpret_cast<std::uintptr_t>(&ExpansionLeaf);case 0x8CBED8:return reinterpret_cast<std::uintptr_t>(&ManagerLeaf);
        case 0x23D09E0:return reinterpret_cast<std::uintptr_t>(&ItemLeaf);default:assert(false);return 0;}
}
static void NoticeNative(void*,void*,void*){++fixture->notices;fixture->events.emplace_back("notice");if(fixture->noticeFault)throw 1;}
static bool TransferNative(void*,const void*,bool animate){
    assert(!animate);++fixture->transfers;fixture->events.emplace_back("transfer");
    if(!ipd72::suppressNotice(fixture->item.data(),fixture->player.data()))NoticeNative(nullptr,fixture->item.data(),fixture->player.data());
    fixture->put(fixture->item.data(),0x98,static_cast<void*>(fixture->inventory.data()));fixture->worldAlive=false;
    if(fixture->transferFault)throw 1;return true;
}
static bool ExecuteNative(void* action){
    ++fixture->executions;fixture->events.emplace_back("execute");
    std::array<unsigned char,0xC8> request{};static const char helper[]=""; // native8CFDEC + immediate8CD484(false)
    fixture->put(request.data(),0,InstantPickupReadD72<std::uint64_t>(action,0x1A8));
    fixture->put(request.data(),8,InstantPickupReadD72<int>(action,0x1A0));
    fixture->put(request.data(),0xD,std::uint8_t(1));fixture->put(request.data(),0x10,std::uint8_t(1));
    fixture->put(request.data(),0x18,static_cast<const char*>(helper));fixture->put(request.data(),0xB8,std::uint32_t(fixture->partial?1:0));
    bool handled=false;bool result=InstantPickupTransferD72(fixture->manager.data(),request.data(),false,&TransferNative,&NoticeNative,handled);
    if(!handled)result=TransferNative(fixture->manager.data(),request.data(),false);
    if(fixture->duplicate){bool second=false;const bool again=InstantPickupTransferD72(fixture->manager.data(),request.data(),false,&TransferNative,&NoticeNative,second);assert(second&&again==result);}
    return result;
}
static void ChoiceNative(void* action,bool immediate){
    ++fixture->choices;fixture->events.emplace_back(immediate?"immediate":"animation");
    if(immediate){if(InstantPickupExecuteD72(action))++fixture->completions;}else ++fixture->animations;
}
static void* FactoryNative(void*,void* result,const std::uint64_t* key,int hand){
    ++fixture->factories;fixture->events.emplace_back("factory");fixture->vf(fixture->allocation.data(),0x3FCF7E8);
    fixture->put(fixture->allocation.data(),8,1);fixture->vf(fixture->action(),0x3B5BE20);
    fixture->put(fixture->action(),0x1A8,*key);fixture->put(fixture->action(),0x1A0,hand);
    fixture->put(fixture->action(),0x1B8,std::uint8_t(1));fixture->put(result,0,fixture->action());
    fixture->put(result,8,static_cast<void*>(fixture->allocation.data()));return result;
}
static bool HandlerNative(void* handler,std::uint64_t key,int hand,bool,bool){
    ++fixture->handlers;fixture->chosenHand=hand;if(!fixture->requestAccepted)return false;
    void* result[2]{};InstantPickupFactoryD72(nullptr,result,&key,hand);
    fixture->put(handler,0x280,result[0]);fixture->put(handler,0x288,result[1]);
    fixture->put(fixture->action(),0x98,static_cast<void*>(fixture->actor.data()));fixture->put(fixture->action(),0xBD,std::uint8_t(1));
    // Exact retail180932C clears +280 after native planner handoff. It is not
    // the action's lifetime or execution identity; the test reproduces that.
    fixture->put(handler,0x280,static_cast<void*>(nullptr));fixture->put(handler,0x288,static_cast<void*>(nullptr));
    if(!fixture->deferred)InstantPickupChoiceD72(fixture->action(),false);return true;
}
static void* DestroyNative(void* action,unsigned){++fixture->destroys;return action;}
static void setup(Fixture& f){
    g_instant_pickup_handler_d72=&HandlerNative;g_instant_pickup_factory_d72=&FactoryNative;
    g_instant_pickup_choice_d72=&ChoiceNative;g_instant_pickup_execute_d72=&ExecuteNative;g_instant_pickup_destroy_d72=&DestroyNative;
    ipd72::intercept=&InstantPickupTransferD72;
    assert(InstantPickupRequestD72(f.controller.data(),42)==2);
}
static void admit(Fixture& f){assert(InstantPickupHandlerD72(f.handler.data(),f.key,0,true,false));}
static int checks=0;
int main(){
    // Execute the production log call, not a copy of its condition. Other
    // diagnostics can run together; only F5 owns these interaction receipts.
    {Fixture f;namespace cap=kcd2vr::diagnostics::capture168;
        cap::gate.select(cap::Domain::Interaction,false,f.now);
        diagnostic_trace::writesD84.clear();InstantPickupNoteD72("TEST",{},true);
        assert(diagnostic_trace::writesD84.empty());++checks;
        cap::gate.select(cap::Domain::Vrik,true,f.now);
        InstantPickupNoteD72("TEST",{},true);assert(diagnostic_trace::writesD84.empty());++checks;
        cap::gate.select(cap::Domain::Interaction,true,f.now);
        InstantPickupNoteD72("TEST",{},true);
        assert(diagnostic_trace::writesD84.size()==1&&diagnostic_trace::writesD84[0].find("D72 INSTANT_PICKUP")!=std::string::npos);++checks;
        cap::gate.select(cap::Domain::Interaction,false,f.now);
        InstantPickupNoteD72("TEST",{},true);assert(diagnostic_trace::writesD84.size()==1);++checks;
        cap::gate.select(cap::Domain::Vrik,false,f.now);}
    // A refused instant route keeps the requesting free hand in native fallback.
    for(int side=0;side<2;++side){Fixture f;setup(f);EndInstantPickupRequestD72();
        action_time_owner59c11::requestedSide=side;
        assert(InstantPickupHandlerD72(f.handler.data(),f.key,1-side,true,false));
        assert(f.chosenHand==side);++checks;
    }
    {Fixture f;setup(f);EndInstantPickupRequestD72();action_time_owner59c11::requestedSide=1;f.free[1]=false;
        assert(InstantPickupHandlerD72(f.handler.data(),f.key,0,true,false));
        assert(f.chosenHand==0);++checks;}
    {Fixture f;setup(f);admit(f);assert(f.chosenHand==1&&f.factories==1);++checks;
        InstantPickupChoiceD72(f.action(),false);assert(f.executions==1&&f.animations==0&&f.completions==1&&f.notices==1&&f.transfers==0&&g_prop596.instantD72);++checks;
        assert(f.worldAlive&&g_prop596.entity==f.entity.data()&&g_prop596.key==f.key);++checks;
        assert(!g_prop596.physics&&g_prop596.kinematicR96);++checks; // R153: firm grip ON = in the hand from the first frame
        assert(InstantPickupExecuteD72(f.action())&&f.executions==1&&f.notices==1&&f.transfers==0);++checks;
        // R155: the accepted hold (left hand here) asked the R103 lease once at acceptance, nothing released yet
        {namespace sc=kcd2vr::self_collision_r103;
         assert(self_collision_r103n::excludedOwners==sc::InstantL&&self_collision_r103n::excludeCalls==1&&self_collision_r103n::releasedOwners==0);++checks;}
        const auto held=g_prop596;g_prop596={};assert(PropCommitInstantD72(held));assert(f.transfers==1&&f.notices==1&&!f.worldAlive);++checks;
        assert(!PropCommitInstantD72(held)&&f.transfers==1);PropRetireInstantD72(held);assert(ipd72::active==0);++checks;
        // R155: retiring the hold releases exactly its own owner bit (the scenery-prop bits stay untouched)
        assert(self_collision_r103n::releasedOwners==kcd2vr::self_collision_r103::InstantL);++checks;
        PropRetireInstantD72(held);assert(self_collision_r103n::releasedOwners==kcd2vr::self_collision_r103::InstantL);++checks;}
    {Fixture f;setup(f);prop5916::firmHoldR116=false;admit(f); // R153: firm grip OFF = the old servo start, unchanged
        InstantPickupChoiceD72(f.action(),false);assert(g_prop596.instantD72&&g_prop596.physics&&!g_prop596.kinematicR96);++checks;
        const auto held=g_prop596;g_prop596={};PropRetireInstantD72(held);prop5916::firmHoldR116=true;}
    {Fixture f;setup(f);admit(f);kcd2vr::instant_d72::interactions=false;
        InstantPickupChoiceD72(f.action(),false);assert(f.animations==0&&g_prop596.instantD72);++checks;
        const auto held=g_prop596;g_prop596={};PropRetireInstantD72(held);assert(f.worldAlive&&f.transfers==0&&f.notices==1);++checks;}
    {Fixture f;setup(f);f.duplicate=true;admit(f);InstantPickupChoiceD72(f.action(),false);
        assert(f.notices==1&&f.transfers==0&&f.completions==1);++checks;
        assert(InstantPickupRequestD72(f.controller.data(),42)==1);assert(InstantPickupRequestD72(f.controller.data(),99)==3);++checks;}
    {Fixture f;setup(f);admit(f);InstantPickupChoiceD72(f.action(),false);
        namespace mutual=kcd2vr::mutual_admission_t25;
        assert(!mutual::TryBegin(1,mutual::Kind::Throw).value&&!mutual::TryBegin(1,mutual::Kind::Reload).value&&!mutual::TryBegin(1,mutual::Kind::Door).value);
        assert(kcd2vr::hand_claim_t24::competing(1,{}));++checks;
        // R155: what PropHoldOwnsHandR155 (staged_pickup596c2.inl) reads to let the fingers curl on Grip for THIS hold:
        // the hand's pickup claim is the hold token while the hold is active; the other hand has no claim
        assert(ipd72::active==1&&g_instant_pickup_hold_d72==g_prop596.instantTokenD72&&
               mutual::pickupClaimsD72[1]==g_instant_pickup_hold_d72&&mutual::pickupClaimsD72[0]==0);++checks;
        const auto held=g_prop596;g_prop596={};PropRetireInstantD72(held);assert(!mutual::pickupClaimsD72[1]);++checks;
        assert(ipd72::active==0&&g_instant_pickup_hold_d72==0);++checks;} // retired: the predicate is false again, native fingers
    {Fixture f;setup(f);admit(f);f.now+=81;f.frame.stamp.tick=f.now;InstantPickupChoiceD72(f.action(),false);
        assert(f.animations==1&&f.executions==0&&f.notices==0&&f.transfers==0);++checks;}
    {Fixture f;setup(f);selectedFistsD80=true;f.frame.pickupTriggerD80[0]=1;admit(f);
        InstantPickupChoiceD72(f.action(),false);assert(f.animations==0&&g_prop596.instantD72);++checks;selectedFistsD80=false;}
    {Fixture f;setup(f);selectedFistsD80=true;admit(f);f.frame.pickupTriggerD80[1]=1;
        InstantPickupChoiceD72(f.action(),false);assert(f.animations==1&&!g_prop596.id&&f.notices==0);++checks;selectedFistsD80=false;}
    {Fixture f;setup(f);admit(f);f.frame.mapGripM39[1]=0;InstantPickupChoiceD72(f.action(),false);
        assert(f.animations==1&&f.executions==0);++checks;}
    for(int debt=0;debt<3;++debt){Fixture f;setup(f);auto& c=kcd2vr::corpse_grip_cg1::claims;
        if(debt==0)c.consumed[1]=true;else if(debt==1)c.nativeHeld[1]=true;else c.fault[1]=true;
        admit(f);InstantPickupChoiceD72(f.action(),false);
        assert(f.animations==1&&f.executions==0&&!g_prop596.instantD72);++checks;}
    {Fixture f;setup(f);kcd2vr::corpse_grip_cg1::claims.consumed[0]=true;admit(f);InstantPickupChoiceD72(f.action(),false);
        assert(f.animations==0&&f.executions==1&&g_prop596.instantD72);++checks;
        const auto held=g_prop596;g_prop596={};PropRetireInstantD72(held);}
    {Fixture f;setup(f);admit(f);InstantPickupDestroyD72(f.action(),0);InstantPickupChoiceD72(f.action(),false);
        assert(f.destroys==1&&f.animations==1&&f.notices==0);++checks;}
    {Fixture f;setup(f);admit(f);f.put(f.action(),0x98,static_cast<void*>(f.world.data()));InstantPickupChoiceD72(f.action(),false);
        assert(f.animations==1&&f.executions==0);++checks;}
    {Fixture f;setup(f);admit(f);f.special=true;InstantPickupChoiceD72(f.action(),false);assert(f.animations==1&&f.executions==0);++checks;}
    {Fixture f;setup(f);admit(f);f.noticeFault=true;InstantPickupChoiceD72(f.action(),false);
        assert(f.notices==1&&f.transfers==0&&!g_prop596.id&&ipd72::active==0&&f.worldAlive&&f.completions==0);++checks;
        // R155: a hold that failed right after the notice never asked the lease, and still released its owner bit (idempotent, harmless)
        assert(self_collision_r103n::excludeCalls==0&&self_collision_r103n::releasedOwners==kcd2vr::self_collision_r103::InstantL);++checks;}
    {Fixture f;setup(f);admit(f);InstantPickupChoiceD72(f.action(),false);const auto held=g_prop596;g_prop596={};f.transferFault=true;
        assert(!PropCommitInstantD72(held));assert(!PropCommitInstantD72(held)&&f.transfers==1&&f.notices==1);PropRetireInstantD72(held);++checks;}
    {Fixture f;setup(f);admit(f);InstantPickupChoiceD72(f.action(),false);const auto held=g_prop596;g_prop596={};f.put(f.item.data(),0x50,std::uint32_t(2));
        assert(!PropCommitInstantD72(held)&&f.transfers==0);PropRetireInstantD72(held);++checks;}
    {Fixture f;kcd2vr::instant_d72::interactions=false;assert(InstantPickupRequestD72(f.controller.data(),42)==0&&f.handlers==0);++checks;}
    {Fixture f;setup(f);f.requestAccepted=false;assert(!InstantPickupHandlerD72(f.handler.data(),f.key,0,true,false));assert(f.factories==0&&g_instant_pickup_live_d72==0);++checks;}
    {Fixture f;setup(f);f.wrongCaller=true;admit(f);InstantPickupChoiceD72(f.action(),false);assert(f.chosenHand==0&&f.animations==1&&f.notices==0);++checks;}
    {Fixture f;setup(f);EndInstantPickupRequestD72();admit(f);InstantPickupChoiceD72(f.action(),false);assert(f.chosenHand==0&&f.animations==1);++checks;}
    {Fixture f;setup(f);admit(f);InstantPickupChoiceD72(f.action(),false);assert(!ShutdownInstantPickupD72()&&ipd72::active&&ipd72::stopping);++checks;
        const auto held=g_prop596;g_prop596={};PropRetireInstantD72(held);assert(ShutdownInstantPickupD72()&&!ipd72::active&&f.transfers==0);++checks;}
    {namespace mutual=kcd2vr::mutual_admission_t25;
        constexpr unsigned rounds=1000;std::barrier start(3),done(3),release(3);
        bool pickup=false;mutual::Token peer{};
        std::thread first([&]{for(unsigned n=0;n<rounds;++n){start.arrive_and_wait();pickup=mutual::RetainPickupD72(1,7000+n);done.arrive_and_wait();release.arrive_and_wait();}});
        std::thread second([&]{for(unsigned n=0;n<rounds;++n){start.arrive_and_wait();peer=mutual::TryBegin(1,mutual::Kind::Throw);done.arrive_and_wait();release.arrive_and_wait();}});
        for(unsigned n=0;n<rounds;++n){start.arrive_and_wait();done.arrive_and_wait();assert(!(pickup&&peer.value));
            if(pickup)assert(mutual::RetirePickupD72(1,7000+n));
            if(peer.value)assert(mutual::Finish(peer,+[](void*)noexcept{},nullptr));release.arrive_and_wait();}
        first.join();second.join();++checks;}
    {Fixture f;setup(f);f.frame.stamp.tick=f.now+1;f.readAdvanceR96=f.now+1;admit(f);InstantPickupChoiceD72(f.action(),false);
     assert(f.executions==1&&f.notices==1&&f.animations==0&&g_prop596.instantD72);++checks;}
    {   // R155: the R103 lease table with the new instant owner bits - its own bits, independent of the scenery-prop bits
        namespace sc=kcd2vr::self_collision_r103;sc::Table t{};
        assert(sc::instantOwner(0)==sc::InstantR&&sc::instantOwner(1)==sc::InstantL&&!(sc::InstantR&sc::PropR)&&!(sc::InstantL&sc::PropL));
        assert((sc::InstantR|sc::InstantL)&~(sc::PropR|sc::PropL|sc::CorpseLabR|sc::CorpseLabL|sc::CorpseSavedR|sc::CorpseSavedL));++checks;
        auto* a=t.take(sc::Kind::Prop,3,0x1000,0x2000,-1,sc::instantOwner(1));assert(a&&a->owners==sc::InstantL);
        auto* b=t.take(sc::Kind::Prop,7,0x3000,0x4000,-1,sc::propOwner(1));assert(b&&b!=a&&t.count()==2);++checks;
        // both hands on one body: the lease waits until the last owner lets go
        assert(t.take(sc::Kind::Prop,3,0x1000,0x2000,-1,sc::propOwner(0))==a&&a->owners==(sc::InstantL|sc::PropR));
        assert(t.release(sc::instantOwner(1),5000)==a&&a->owners==sc::PropR&&a->released==0&&t.byOwner(sc::InstantL)==nullptr);++checks;
        assert(t.release(sc::propOwner(0),6000)==a&&a->owners==0&&a->released==6000&&b->owners==sc::PropL);++checks;
        // the added bits for a prop: ignore |= living and type |= living, only what the body does not have yet
        const auto add=sc::missing(sc::Kind::Prop,0x8000,0);assert(add.type==sc::kLiving&&add.ignore==sc::kLiving);
        assert(!sc::missing(sc::Kind::Prop,0x8004,0x4).any());++checks;
        // restore timing: not before 400 ms, when clear of Henry (0.6 m), at the latest after 4 s
        assert(sc::due(*a,6100,true,5.0)==sc::Due::Wait&&sc::due(*a,6500,true,5.0)==sc::Due::Restore&&
               sc::due(*a,6500,true,.3)==sc::Due::Wait&&sc::due(*a,6500,false,0)==sc::Due::Wait&&sc::due(*a,10000,false,0)==sc::Due::Restore);++checks;}
    std::cout<<"D72 actual native pickup adapter PASS checks="<<checks<<"; native leaves mocked; no headset/game/SEH runtime claim\n";
}
