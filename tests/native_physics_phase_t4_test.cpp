// Compile the actual production owner dispatch and leaves in this TU. Only OS,
// PE memory and native functions are substituted; no copied lifecycle model.
#define KCD2VR_PHYSICS_PHASE_T4_TEST 1
#include "../src/native_physics_phase_t4.cpp"
#include <cassert>
#include <map>
#include <vector>
#include <iostream>

namespace pp=kcd2vr::native_physics_phase_t4;
namespace {
std::array<unsigned char,0x6100> worldA{},worldB{};
std::array<unsigned char,0x500> body{},buddy{};
std::map<std::uintptr_t,std::uintptr_t> slots;
void* selectedWorld=worldA.data();
void* registeredBody=body.data();
void* registeredBuddy=buddy.data();
bool pairStatusUnknown=false,pairStatusAbsent=false,revokeOnLivingAddRef=false;
int nativeRefs=0,nativeLookup=0,nativeStatus=0,nativeActions=0,nativeReleases=0;
bool refRefused=false,lookupFault=false,statusFault=false,releaseFault=false;
int privateCookie=0,entityCookie=0;
int addConstraintReturn=77,removeConstraintReturn=1;
constexpr std::uintptr_t fakeBase=0x10000000;
template<class T> void set(void* p,std::size_t n,T v){std::memcpy(static_cast<unsigned char*>(p)+n,&v,sizeof(v));}
template<class T> T get(const void* p,std::size_t n){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+n,sizeof(v));return v;}
bool readMemory(const void* p,void* out,std::size_t n){
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    if(a==fakeBase+0x492D830&&n==sizeof(void*)){std::memcpy(out,&selectedWorld,n);return true;}
    if(auto i=slots.find(a);i!=slots.end()&&n==sizeof(i->second)){std::memcpy(out,&i->second,n);return true;}
    for(auto r:{std::pair{worldA.data(),worldA.size()},std::pair{worldB.data(),worldB.size()},std::pair{body.data(),body.size()},std::pair{buddy.data(),buddy.size()}}){
        auto begin=reinterpret_cast<std::uintptr_t>(r.first);
        if(a>=begin&&a-begin<=r.second&&n<=r.second-(a-begin)){std::memcpy(out,p,n);return true;}
    }
    return false;
}
void* lookup(void* w,int id){
    ++nativeLookup;assert(w==selectedWorld);
    if(lookupFault){pp::faults.fetch_add(1);pp::nativeFaulted.store(true);return nullptr;}
    return id==17?registeredBody:id==18?registeredBuddy:nullptr;
}
int idOf(void* w,void* p){assert(w==selectedWorld&&p==body.data());return 17;}
int addRef(void* p){assert(p==body.data()||p==buddy.data());if(refRefused)return 0;if(p==buddy.data()&&revokeOnLivingAddRef)set(p,0x20,static_cast<void*>(&privateCookie));return ++nativeRefs;}
int release(void* p){assert(p==body.data()||p==buddy.data());++nativeReleases;
    if(releaseFault){pp::faults.fetch_add(1);pp::nativeFaulted.store(true);return 0;}
    assert(nativeRefs>0);return --nativeRefs;
}
int status(void* p,void* s){
    assert(p==body.data());++nativeStatus;
    if(statusFault){pp::faults.fetch_add(1);pp::nativeFaulted.store(true);return 0;}
    int kind=get<int>(s,0);
    if(kind==1){set(s,0x18,pp::V3{1,2,3});set(s,0x3C,pp::Q{});set(s,0x4C,1.f);set(s,0x50,2);return 1;}
    if(kind==8){set(s,0xC,pp::V3{.1f,.2f,.3f});set(s,0x18,pp::V3{.4f,.5f,.6f});set(s,0x3C,pp::V3{4,5,6});set(s,0x4C,3.5f);return 1;}
    assert(kind==26&&get<int>(s,8)==-1);
    if(get<int>(s,4)==77){if(pairStatusUnknown)return -1;if(pairStatusAbsent)return 0;
        set(s,0xC,0x300u);set(s,0x38,static_cast<void*>(buddy.data()));return 1;}
    if(get<int>(s,4)!=23)return 0;
    set(s,0xC,0x42u);set(s,0x38,static_cast<void*>(buddy.data()));set(s,0x40,reinterpret_cast<void*>(0x12345678));return 1;
}
int action(void* p,void* a,int safe){
    assert(p==body.data()&&safe==1);++nativeActions;
    if(get<int>(a,0)==5){assert(get<std::uintptr_t>(a,8)==reinterpret_cast<std::uintptr_t>(buddy.data()));
        if(get<unsigned>(a,0x60)==0x300u){assert(get<float>(a,0x10)==0&&get<float>(a,0x1C)==0&&get<float>(a,0x3C)==1&&get<float>(a,0x4C)==1);}
        return addConstraintReturn;}
    if(get<int>(a,0)==6){assert(get<unsigned>(a,4)==77&&get<int>(a,0x10)==1);return removeConstraintReturn;}
    assert(get<int>(a,0)==10&&get<unsigned>(a,4)==0x80000000u);
    assert(get<unsigned>(a,8)==0x80000000u);return 1;
}
void prepareWorld(void* w){set(w,0,fakeBase+0x407FC80);set(w,0x5FDC,0x10000u);set(w,0x5FF4,0x10000u);}
void prepareBody(){
    set(body.data(),0,fakeBase+0x3A48DC0);set(body.data(),0x68,selectedWorld);set(body.data(),0x50,0);
    set(body.data(),0x47,std::int8_t(2));set(body.data(),0x84,0u);set(body.data(),0x194,std::uint8_t(1));
    set(body.data(),0x20,static_cast<void*>(&privateCookie));set(body.data(),0x28,std::int16_t(12345));
}
struct State {pp::Lease lease;int stage=0,ticks=0,resetCalls=0;bool resetRelease=true;};
void tick(pp::Context& c,void* user) noexcept {
    auto& s=*static_cast<State*>(user);++s.ticks;
    assert(c.Current()&&c.World()==selectedWorld&&c.ModuleBase()==fakeBase&&c.Generation()>0&&c.Serial()>0&&c.Kind()==pp::Phase::Step);
    switch(s.stage){
    case 0: {
        set(c.World(),0x10D0,1u);set(c.World(),0x5FD4,0x10000);
        assert(!pp::AcquireRigid(c,17,body.data(),s.lease)&&nativeRefs==0);
        set(c.World(),0x10D0,0u);set(c.World(),0x5FD4,0);
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));assert(nativeRefs==1);
        set(c.World(),0x10D0,1u);set(c.World(),0x5FD4,0x10000);int released=nativeReleases;
        assert(!pp::Release(c,s.lease)&&s.lease.Held()&&nativeReleases==released);
        set(c.World(),0x10D0,0u);set(c.World(),0x5FD4,0);
        assert(!pp::AcquireRigid(c,17,body.data(),s.lease));
        pp::RigidSnapshot p{};assert(pp::ReadRigid(c,s.lease,p));assert(p.mass==3.5f&&p.position.z==3&&p.centerOfMass.y==5&&p.active);
        assert(pp::SetVelocity(c,s.lease,{1,2,3},{0,1,0}));const int actions=nativeActions;
        assert(!pp::SetVelocity(c,s.lease,{100,0,0},{}));assert(!pp::SetVelocity(c,s.lease,{NAN,0,0},{}));assert(nativeActions==actions);
        // Actual production constraint leaves: both refs owned in this locked callback.
        std::memcpy(buddy.data(),body.data(),body.size());pp::Lease other;
        assert(pp::AcquireRigid(c,18,buddy.data(),other)&&nativeRefs==2);
        auto packet=kcd2vr::constraint_packet60::defaults();
        kcd2vr::constraint_packet60::setBuddy(packet,reinterpret_cast<std::uintptr_t>(buddy.data()));
        assert(kcd2vr::constraint_packet60::setFlags(packet,0x200));
        auto added=pp::AddConstraint(c,s.lease,other,packet);assert(added.kind==pp::ConstraintResult::Kind::Submitted&&added.id==77);
        addConstraintReturn=0;auto refused=pp::AddConstraint(c,s.lease,other,packet);assert(refused.kind==pp::ConstraintResult::Kind::Refused&&!refused.id);
        addConstraintReturn=-1;assert(pp::AddConstraint(c,s.lease,other,packet).kind==pp::ConstraintResult::Kind::Refused);addConstraintReturn=77;
        const int submitted=nativeActions;
        pp::testThread=2;assert(pp::AddConstraint(c,s.lease,other,packet).kind==pp::ConstraintResult::Kind::Refused&&nativeActions==submitted);pp::testThread=1;
        kcd2vr::constraint_packet60::setBuddy(packet,0x1234);assert(pp::AddConstraint(c,s.lease,other,packet).kind==pp::ConstraintResult::Kind::Refused&&nativeActions==submitted);
        kcd2vr::constraint_packet60::setBuddy(packet,reinterpret_cast<std::uintptr_t>(buddy.data()));
        assert(pp::AddConstraint(c,s.lease,s.lease,packet).kind==pp::ConstraintResult::Kind::Refused);
        kcd2vr::constraint_packet60::setIgnorePairOnly(packet);
        assert(pp::AddConstraint(c,s.lease,other,packet).kind==pp::ConstraintResult::Kind::Refused&&nativeActions==submitted);
        assert(pp::RemoveConstraint(c,s.lease,77).kind==pp::ConstraintResult::Kind::Submitted);
        removeConstraintReturn=0;assert(pp::RemoveConstraint(c,s.lease,77).kind==pp::ConstraintResult::Kind::Refused);removeConstraintReturn=1;
        assert(pp::RemoveConstraint(c,s.lease,0).kind==pp::ConstraintResult::Kind::Refused);
        assert(pp::Release(c,other)&&nativeRefs==1);
        // Production typed living acquisition and explicit inactive pair-only leaf.
        std::memcpy(buddy.data(),body.data(),body.size());set(buddy.data(),0,fakeBase+0x3A82570);
        set(buddy.data(),0x20,static_cast<void*>(&entityCookie));set(buddy.data(),0x28,std::int16_t(2));
        set(buddy.data(),0x47,std::int8_t(3));set(buddy.data(),0xEC,4u);
        pp::LivingSourceH24 livingSource{18,buddy.data(),&entityCookie};pp::LivingLeaseH24 living;
        assert(pp::AcquireLivingH24(c,livingSource,living)&&living.Held()&&nativeRefs==2);
        assert(pp::CurrentLivingH24(c,living,livingSource));
        auto paired=pp::AddSelfIgnorePairH24(c,s.lease,living);
        assert(paired.kind==pp::ConstraintResult::Kind::Submitted&&paired.id==77);
        assert(pp::SelfIgnorePairPresentH24(c,s.lease,living,77));
        pairStatusUnknown=true;assert(!pp::SelfIgnorePairPresentH24(c,s.lease,living,77));pairStatusUnknown=false;
        pairStatusAbsent=true;assert(!pp::SelfIgnorePairPresentH24(c,s.lease,living,77));pairStatusAbsent=false;
        addConstraintReturn=0;assert(pp::AddSelfIgnorePairH24(c,s.lease,living).kind==pp::ConstraintResult::Kind::Refused);addConstraintReturn=77;
        auto wrongLiving=livingSource;wrongLiving.entity=&privateCookie;assert(!pp::CurrentLivingH24(c,living,wrongLiving));
        set(buddy.data(),0x20,static_cast<void*>(&privateCookie));assert(!pp::CurrentLivingH24(c,living,livingSource));
        assert(!pp::SelfIgnorePairPresentH24(c,s.lease,living,77));
        assert(pp::AddSelfIgnorePairH24(c,s.lease,living).kind==pp::ConstraintResult::Kind::Refused);
        set(buddy.data(),0x20,static_cast<void*>(&entityCookie));registeredBuddy=body.data();
        assert(!pp::CurrentLivingH24(c,living,livingSource));registeredBuddy=buddy.data();
        assert(pp::CurrentLivingH24(c,living,livingSource));
        set(buddy.data(),0x68,static_cast<void*>(worldB.data()));assert(!pp::CurrentLivingH24(c,living,livingSource));set(buddy.data(),0x68,selectedWorld);
        set(buddy.data(),0x50,1);assert(!pp::CurrentLivingH24(c,living,livingSource));set(buddy.data(),0x50,0);
        // Typed living reference cannot be fed to SetVelocity or a grasp API.
        assert(pp::RemoveSelfIgnorePairH24(c,s.lease,77).kind==pp::ConstraintResult::Kind::Submitted);
        assert(pp::ReleaseLivingH24(c,living)&&!living.Held()&&nativeRefs==1);
        revokeOnLivingAddRef=true;assert(!pp::AcquireLivingH24(c,livingSource,living)&&living.Held());
        revokeOnLivingAddRef=false;assert(pp::ReleaseLivingH24(c,living)&&nativeRefs==1);set(buddy.data(),0x20,static_cast<void*>(&entityCookie));
        refRefused=true;assert(!pp::AcquireLivingH24(c,livingSource,living)&&!living.Held());refRefused=false;
        set(buddy.data(),0x47,std::int8_t(4));assert(!pp::AcquireLivingH24(c,livingSource,living));set(buddy.data(),0x47,std::int8_t(3));
        const auto savedLivingRef=slots[fakeBase+0x3A82570+0x10];slots[fakeBase+0x3A82570+0x10]=0;
        assert(!pp::AcquireLivingH24(c,livingSource,living));slots[fakeBase+0x3A82570+0x10]=savedLivingRef;
        std::cout<<"Typed living self-only inactive pair: native refusal, Present/unknown/Absent, current identity/body/world/revocation/ref ownership PASS\n";
        pp::ConstraintSnapshot k{};assert(pp::ReadConstraint(c,s.lease,23,k)&&k.exists&&k.buddy==buddy.data()&&k.flags==0x42);
        assert(pp::ReadConstraint(c,s.lease,24,k)&&!k.exists&&k.buddy==nullptr);assert(!pp::ReadConstraint(c,s.lease,-1,k));
        const auto reads=nativeLookup;pp::testThread=2;assert(!c.Current()&&!pp::NativeHandle(c,s.lease));assert(nativeLookup==reads);pp::testThread=1;
        const int ticks=s.ticks;pp::Access::Dispatch(selectedWorld,pp::Phase::Step);assert(s.ticks==ticks); // no recursive admission
        break;
    }
    case 1: {
        set(body.data(),0x50,1);const auto calls=nativeStatus;pp::RigidSnapshot p{};
        assert(!pp::ReadRigid(c,s.lease,p)&&nativeStatus==calls);assert(pp::Release(c,s.lease)); // real ref still owns memory
        set(body.data(),0x50,0);set(body.data(),0x84,0x80000000u);
        assert(pp::AdoptCreatedRigid(c,body.data(),&privateCookie,12345,s.lease));assert(s.lease.InsertionPending());
        assert(!pp::ReadyRigid(c,s.lease));assert(!pp::NativeHandle(c,s.lease));assert(pp::NativeHandle(c,s.lease,true)==body.data());
        assert(!pp::ReadRigid(c,s.lease,p));break;
    }
    case 2: {
        set(body.data(),0x84,0u);assert(pp::ReadyRigid(c,s.lease)&&!s.lease.InsertionPending());
        registeredBody=buddy.data();assert(pp::ObserveRegistry(c,s.lease)==pp::Registry::Retired);assert(!pp::NativeHandle(c,s.lease));
        assert(pp::Release(c,s.lease));registeredBody=body.data();
        set(body.data(),0,fakeBase+0x3A578C0);set(body.data(),0x20,static_cast<void*>(&entityCookie));set(body.data(),0x28,std::int16_t(2));
        set(body.data(),0x47,std::int8_t(-1));set(body.data(),0x7C,0x102u);
        assert(pp::ObserveSuspended(c,17,body.data(),&entityCookie));assert(!pp::ObserveSuspended(c,17,body.data(),&privateCookie));
        set(body.data(),0x7C,2u);assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));
        // C63 profile0 is only an observation candidate. It does not bypass
        // the exact physics-phase suspension/registry/identity postcondition.
        set(body.data(),0x7C,0x102u);set(body.data(),0x47,std::int8_t(2));
        assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));
        set(body.data(),0x47,std::int8_t(-1));set(body.data(),0x28,std::int16_t(1));
        assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));
        set(body.data(),0x28,std::int16_t(2));set(body.data(),0x50,1);
        assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));
        set(body.data(),0x50,0);registeredBody=buddy.data();
        assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));registeredBody=body.data();
        set(body.data(),0x68,static_cast<void*>(worldB.data()));
        assert(!pp::ObserveSuspended(c,17,body.data(),&entityCookie));set(body.data(),0x68,selectedWorld);
        assert(pp::ObserveSuspended(c,17,body.data(),&entityCookie));
        set(body.data(),0,fakeBase+0x3A82570);set(body.data(),0x47,std::int8_t(3));set(body.data(),0xEC,4u);
        assert(pp::ObserveLivingExcluded(c,17,body.data(),&entityCookie));assert(!pp::ObserveLivingExcluded(c,17,body.data(),&privateCookie));
        set(body.data(),0xEC,0u);assert(!pp::ObserveLivingExcluded(c,17,body.data(),&entityCookie));prepareBody();
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));break;
    }
    case 3: // new world without reset must never call old body
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));break;
    case 4:
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));break;
    case 5:
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));
        statusFault=true;{pp::RigidSnapshot p{};assert(!pp::ReadRigid(c,s.lease,p));}
        assert(pp::nativeFaulted.load());assert(!pp::Release(c,s.lease));break;
    case 6: {
        // Only the old generation is abandoned. No old native release is
        // invented; the next acquisition must rejoin this fresh world.
        const auto releasesBefore=nativeReleases;
        assert(pp::AbandonInvalidated(c,s.lease)&&nativeReleases==releasesBefore);
        assert(pp::AcquireRigid(c,17,body.data(),s.lease));
        pp::RigidSnapshot p{};assert(pp::ReadRigid(c,s.lease,p));
        assert(pp::Release(c,s.lease));break;
    }
    default: assert(false);
    }
}
void reset(pp::Context& c,void* user) noexcept {
    auto& s=*static_cast<State*>(user);++s.resetCalls;assert(c.Current());
    pp::Lease denied;auto lookupCount=nativeLookup;
    assert(!pp::AcquireRigid(c,17,body.data(),denied)&&nativeLookup==lookupCount);
    if(c.Kind()==pp::Phase::WorldReplaced){
        int calls=nativeReleases;assert(!pp::Release(c,s.lease));assert(pp::AbandonInvalidated(c,s.lease));assert(nativeReleases==calls);return;
    }
    assert(c.Kind()==pp::Phase::WorldReset||c.Kind()==pp::Phase::Shutdown);
    assert(!pp::SetVelocity(c,s.lease,{},{}));if(s.lease.Held()&&s.resetRelease)assert(pp::Release(c,s.lease));
}
}
int main(){
    pp::base=fakeBase;pp::testRead=&readMemory;pp::api={&lookup,&idOf,&addRef,&release,&status,&action};
    for(auto [table,slot,target]:std::initializer_list<std::array<std::uintptr_t,3>>{
        {0x3A82570,0x10,0x5311C4},{0x3A82570,0x18,0x531F84},{0x3A48DC0,0x150,0xBCC44C},
        {0x407FC80,0xB0,0x667418},{0x407FC80,0xA8,0x52950C},
        {0x3A48DC0,0x10,0x5311C4},{0x3A48DC0,0x18,0x531F84},{0x3A48DC0,0x30,0x8A6C90},{0x3A48DC0,0x38,0xAC0D18}})
        slots[fakeBase+table+slot]=fakeBase+target;
    prepareWorld(worldA.data());prepareWorld(worldB.data());prepareBody();State state;
    assert(pp::RegisterClient({&tick,&reset,&state}));assert(pp::RegisterClient({&tick,&reset,&state})); // idempotent
    pp::installed.store(true);
    set(worldA.data(),0x5FF4,0u);pp::Access::Dispatch(worldA.data(),pp::Phase::Step);assert(state.ticks==0&&nativeLookup==0);
    set(worldA.data(),0x5FF4,0x10000u);
    auto saved=slots[fakeBase+0x407FC80+0xB0];slots[fakeBase+0x407FC80+0xB0]=0;
    pp::Access::Dispatch(worldA.data(),pp::Phase::Step);assert(state.ticks==0);slots[fakeBase+0x407FC80+0xB0]=saved;
    pp::Access::Dispatch(worldA.data(),pp::Phase::Step);assert(state.ticks==1&&nativeRefs==1);
    state.stage=1;pp::Access::Dispatch(worldA.data(),pp::Phase::Step);
    state.stage=2;pp::Access::Dispatch(worldA.data(),pp::Phase::Step);assert(nativeRefs==1);
    // Reset is a distinct native owner: only5FDC is held, not5FF4.
    set(worldA.data(),0x5FF4,0u);auto oldGeneration=pp::generation;
    selectedWorld=nullptr; // native old-world reset remains qualified after global clear
    pp::Access::Dispatch(worldA.data(),pp::Phase::WorldReset);assert(state.resetCalls==1&&!state.lease.Held()&&nativeRefs==0&&pp::generation>oldGeneration);
    selectedWorld=worldA.data();
    set(worldA.data(),0x5FF4,0x10000u);state.stage=3;pp::Access::Dispatch(worldA.data(),pp::Phase::Step);assert(state.lease.Held());
    selectedWorld=worldB.data();prepareBody();state.stage=4;int priorRelease=nativeReleases;
    pp::Access::Dispatch(worldB.data(),pp::Phase::Step);assert(state.resetCalls==2&&pp::ReadReceipt().abandonedReferences==1&&nativeReleases==priorRelease);
    // Native old reference is deliberately unconfirmed, never decremented via
    // a replacement world. Model now owns that separate old-world count.
    assert(nativeRefs==2);nativeRefs=1;
    pp::RequestShutdown();auto priorTicks=state.ticks;pp::Access::Dispatch(worldB.data(),pp::Phase::Step);
    assert(!state.lease.Held()&&nativeRefs==0&&state.ticks==priorTicks);
    pp::Access::Dispatch(worldB.data(),pp::Phase::Step);assert(nativeRefs==0); // repeat cleanup never repeats Release
    pp::shutdown.store(false);state.stage=5;state.resetRelease=false;pp::Access::Dispatch(worldB.data(),pp::Phase::Step);
    assert(state.lease.Held()&&pp::ReadReceipt().faults==1);int calls=nativeStatus;
    pp::Access::Dispatch(worldB.data(),pp::Phase::WorldReset);assert(nativeStatus==calls); // reset never replays the faulted call
    statusFault=false;nativeRefs=0;prepareBody();state.stage=6;
    pp::Access::Dispatch(worldB.data(),pp::Phase::Step);
    assert(!state.lease.Held()&&nativeRefs==0&&nativeStatus==calls+2);
    std::cout<<"Actual production native phase dispatch/reference/rigid/status/reset/fault fixture PASS\n";
    // A successful later preflight cannot erase the first post-mutation failure.
    pp::RecordInstallFailureH7("READ_ONLY_PIN");assert(std::string(pp::InstallFailureStage())=="READ_ONLY_PIN");
    pp::installMutationStartedH7=true;pp::RecordInstallFailureH7("FIRST_MUTATION_STAGE");
    pp::RecordInstallFailureH7("LATER_STAGE");pp::installFailureStageH7="NONE";
    assert(std::string(pp::InstallFailureStage())=="FIRST_MUTATION_STAGE");

    // Dependency regression: hand proxy/weapon owners register before the
    // grab/impale joints that pin them. Reset must notify the joint first.
    for(auto& slot:pp::clients)slot.published.store(false);
    pp::shutdown=false;pp::nativeFaulted=false;pp::currentWorld=nullptr;
    selectedWorld=worldA.data();prepareWorld(worldA.data());prepareWorld(worldB.data());
    struct Dependency {int number=0;bool* parent=nullptr;bool alive=true;std::vector<int>* seen=nullptr;};
    std::vector<int> order;std::array<Dependency,4> dependencies{};
    const auto stepDependency=+[](pp::Context&,void*) noexcept {};
    const auto resetDependency=+[](pp::Context&,void* p) noexcept {
        auto& d=*static_cast<Dependency*>(p);if(d.parent)assert(*d.parent);
        d.seen->push_back(d.number);d.alive=false;
    };
    for(unsigned i=0;i<dependencies.size();++i){auto& d=dependencies[i];d.number=int(i+1);d.seen=&order;
        if(i)d.parent=&dependencies[i-1].alive;assert(pp::RegisterClient({stepDependency,resetDependency,&d}));}
    pp::Access::Dispatch(worldA.data(),pp::Phase::Step);
    pp::Access::Dispatch(worldA.data(),pp::Phase::WorldReset);assert((order==std::vector<int>{4,3,2,1}));
    for(auto& d:dependencies)d.alive=true;order.clear();
    pp::Access::Dispatch(worldA.data(),pp::Phase::Step);selectedWorld=worldB.data();
    pp::Access::Dispatch(worldB.data(),pp::Phase::Step);assert((order==std::vector<int>{4,3,2,1}));
    for(auto& d:dependencies)d.alive=true;order.clear();pp::RequestShutdown();
    pp::Access::Dispatch(worldB.data(),pp::Phase::Step);assert((order==std::vector<int>{4,3,2,1}));
    std::cout<<"Actual four-client reset/replacement/shutdown dependency order PASS\n";

    return 0;
}
