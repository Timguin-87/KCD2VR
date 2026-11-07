// Reuse the real typed phase and original explicit native-memory providers.
// The retained provider fixture main has only an explicit return0 for MSVC; it is not run in these cases.
#define main retained_native_phase_main_h5
#include "native_physics_phase_t4_test.cpp"
#undef main
#include <string>
namespace {
pp::LivingLeaseH24 obligationH5;
std::string scenarioH5;
int referenceCallsH5=0,stageH5=0;bool admittedH5=false;
int retainH5(void* p){assert(p==buddy.data());++referenceCallsH5;
    if(scenarioH5=="native-zero")return 0;
    if(scenarioH5=="native-negative")return -1;
    ++nativeRefs;
    if(scenarioH5=="positive-fault"){pp::faults.fetch_add(1);pp::nativeFaulted=true;}
    if(scenarioH5=="positive-context-loss")set(worldA.data(),0x5FDC,0u);
    if(scenarioH5=="positive-world-loss")selectedWorld=worldB.data();
    if(scenarioH5=="positive-entity-loss")set(buddy.data(),0x20,static_cast<void*>(&privateCookie));
    return nativeRefs;
}
int releaseOriginalH5(void* p){assert(p==buddy.data());return release(p);}
void acquireH5(pp::Context& c,void*)noexcept {
    if(stageH5==0){
        admittedH5=pp::AcquireLivingH24(c,{18,buddy.data(),&entityCookie},obligationH5);return;
    }
    assert(obligationH5.Held());assert(pp::CurrentLivingH24(c,obligationH5,{18,buddy.data(),&entityCookie}));
    assert(pp::ReleaseLivingH24(c,obligationH5));
}
void resetH5(pp::Context&,void*)noexcept {}
}
int main(int argc,char**argv){
    if(argc!=2)return 2;scenarioH5=argv[1];
    pp::base=fakeBase;pp::testRead=&readMemory;pp::api={&lookup,&idOf,&retainH5,&releaseOriginalH5,&status,&action};
    for(auto [table,slot,target]:std::initializer_list<std::array<std::uintptr_t,3>>{
        {0x3A82570,0x10,0x5311C4},{0x3A82570,0x18,0x531F84},
        {0x407FC80,0xB0,0x667418},{0x407FC80,0xA8,0x52950C}})
        slots[fakeBase+table+slot]=fakeBase+target;
    prepareWorld(worldA.data());prepareWorld(worldB.data());selectedWorld=worldA.data();prepareBody();
    std::memcpy(buddy.data(),body.data(),body.size());set(buddy.data(),0,fakeBase+0x3A82570);
    set(buddy.data(),0x20,static_cast<void*>(&entityCookie));set(buddy.data(),0x28,std::int16_t(2));
    set(buddy.data(),0x47,std::int8_t(3));set(buddy.data(),0xEC,4u);
    if(scenarioH5=="independent-owner"){set(worldA.data(),0x10D0,pp::testThread);set(worldA.data(),0x5FD4,0x10000);}
    assert(pp::RegisterClient({&acquireH5,&resetH5,nullptr}));pp::installed=true;
    pp::Access::Dispatch(worldA.data(),pp::Phase::Step);
    const bool noReference=scenarioH5=="native-zero"||scenarioH5=="native-negative"||scenarioH5=="independent-owner";
    assert(admittedH5==(scenarioH5=="ordinary"));
    assert(nativeActions==0&&nativeStatus==0); // retention never grants pair/motor or a status call
    if(noReference){assert(!obligationH5.Held()&&nativeRefs==0&&nativeReleases==0);
        assert(referenceCallsH5==(scenarioH5=="independent-owner"?0:1));}
    else {
        // Positive native return is an obligation even if admission closes.
        assert(obligationH5.Held()&&nativeRefs==1&&referenceCallsH5==1&&nativeReleases==0);
        if(scenarioH5=="positive-fault"){
            // Existing native fault latch prohibits pretending to release it.
            pp::Access::Dispatch(worldA.data(),pp::Phase::Step);
            assert(obligationH5.Held()&&nativeRefs==1&&nativeReleases==0&&!admittedH5);
        }else{
            // A later legitimate callback must release the ORIGINAL world/gen,
            // not null identity obtained through already-closed Context getters.
            selectedWorld=worldA.data();set(worldA.data(),0x5FDC,0x10000u);
            set(buddy.data(),0x20,static_cast<void*>(&entityCookie));stageH5=1;
            pp::Access::Dispatch(worldA.data(),pp::Phase::Step);
            assert(!obligationH5.Held()&&nativeRefs==0&&nativeReleases==1);
        }
    }
    std::printf("living reference obligation %s PASS; native instructions not executed\n",scenarioH5.c_str());return 0;
}
