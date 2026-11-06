// Execute the complete production paired-combat adapter and production native
// container membership walk. Native reads/writes and unrelated engine leaves
// are controlled here. Each scenario seeds a previously admitted action; it
// tests cleanup ownership, not retail pair admission, animation or damage.
#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include "diagnostics/capture168.h"
#include "subsystems/masterstrike_combos/paired_signals_d62c.h"
#include "subsystems/masterstrike_combos/paired_hands_d72.h"
#include "vrik/semi_smith_native46.h"
#include <atomic>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>

namespace ms=kcd2vr::ms_reaction_d62c;
namespace instant=kcd2vr::instant_d72;
static unsigned checks=0,failures=0,reads=0,writeAttempts=0,writes=0;
static std::uint64_t tick=1000;
static std::vector<std::pair<std::uintptr_t,std::size_t>> regions;
static std::uintptr_t unreadable=0,writeAddress=0,observedAction=0;
static unsigned actionReads=0,failSpeedReads=0,failReadback=0;
static bool failWrite=false,writeThenFail=false;
static std::function<void()> afterSpeedRead;
static std::uint64_t GetTickCount64() noexcept {return tick;}
namespace diagnostics=kcd2vr::diagnostics;
namespace diagnostic_trace {static void Write(const char*) noexcept {}}
namespace retail_build112 {static bool Ready(std::uintptr_t) noexcept {return false;}}
static bool mapped(const void* p,std::size_t n) noexcept {
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    for(const auto& r:regions)if(a>=r.first&&n<=r.second&&a-r.first<=r.second-n)return true;
    return false;
}
static bool SehCopy(const void* from,void* to,std::size_t n) noexcept {
    ++reads;const auto a=reinterpret_cast<std::uintptr_t>(from);
    if(observedAction&&a>=observedAction&&a-observedAction<0x180)++actionReads;
    if(a==unreadable||(a==writeAddress&&failSpeedReads&&(--failSpeedReads,true))||!mapped(from,n))return false;
    std::memcpy(to,from,n);
    if(a==writeAddress&&afterSpeedRead){auto f=std::move(afterSpeedRead);afterSpeedRead={};f();}
    return true;
}
template<class T>static T Rd(const void* p,std::ptrdiff_t offset,T fallback) noexcept {
    T out{};return p&&SehCopy(static_cast<const unsigned char*>(p)+offset,&out,sizeof(out))?out:fallback;
}
template<class T>static bool Wr(void* p,std::ptrdiff_t offset,T value) noexcept {
    ++writeAttempts;auto* to=static_cast<unsigned char*>(p)+offset;
    if(failWrite||!mapped(to,sizeof(value)))return false;
    ++writes;std::memcpy(to,&value,sizeof(value));
    if(failReadback){--failReadback;++failSpeedReads;}
    return !writeThenFail;
}
constexpr std::uintptr_t kCtrlVft=0x40519E8ull;
constexpr std::ptrdiff_t kCtrlContext=0x20,kCtrlEntityId=0x30,kCtrlScopeContexts=0x58,
    kActRootScope=0x30,kActFragId=0x38,kActTags=0x3C,kActOption=0x50,kActToken=0x54,
    kActSpeedBias=0x5C,kScopeContextChar=8,kScopeLayer=0xE8,kScopeNumLayers=0xEC,
    kScopeAction=0x148,kCharSkelAnim=0xA80;
static std::atomic<std::uintptr_t> g_base{0x10000000};
static std::atomic<void*> g_henry_ctrl{nullptr};
static bool IsHenryController(void*) noexcept {return false;}
static unsigned HenryEntityId18() noexcept {return 1;}
namespace grasp13 {
struct AcquisitionInput55 {
    std::uintptr_t character=0;std::uint64_t epoch=0,trackingEpoch=0,sample=0,tick=0;
    bool headTracked=false,tracked[2]{};
};
}
static kcd2vr::core::SnapshotMailbox<grasp13::AcquisitionInput55> g_acquisition_input55;
static bool FreshAcquisitionInput55(const grasp13::AcquisitionInput55&,std::uint64_t) noexcept {return false;}
namespace animation_filter {static bool AdmittedNativeClipOnlyD72(void*,std::uintptr_t,int) noexcept {return false;}}
#ifndef KCD2VR_MS_ADAPTER_D76R1
#define KCD2VR_MS_ADAPTER_D76R1 "../../../src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl"
#endif
#include KCD2VR_MS_ADAPTER_D76R1

struct Memory {
    std::vector<unsigned char> bytes;
    explicit Memory(std::size_t n):bytes(n){regions.emplace_back(address(),n);}
    unsigned char* data() noexcept {return bytes.data();}
    std::uintptr_t address() const noexcept {return reinterpret_cast<std::uintptr_t>(bytes.data());}
    template<class T>void put(std::size_t offset,T value) noexcept {std::memcpy(data()+offset,&value,sizeof(value));}
    template<class T>T get(std::size_t offset) const noexcept {T value{};std::memcpy(&value,bytes.data()+offset,sizeof(value));return value;}
};
struct Actor {
    Memory controller{0xB0},scope{0x168},action{0x180},queue{8};
    unsigned entity;ms::Owner* row=nullptr;
    explicit Actor(unsigned id=11):entity(id){
        controller.put(0,g_base.load()+kCtrlVft);controller.put(kCtrlEntityId,entity);
        action.put(0,g_base.load()+kcd2vr::native_pair_d72::fragmentVt);
        action.put(kActToken,123u);action.put(kActFragId,922);action.put(kActSpeedBias,1.f);
        root();
    }
    void root() noexcept {
        controller.put(0x34,1u);controller.put(0x38,scope.data());
        controller.put(0x48,std::uintptr_t(0));controller.put(0x50,std::uintptr_t(0));
        scope.put(0x148,action.data());scope.put(0x150,std::uintptr_t(0));action.put(0x30,scope.data());
    }
    void exiting() noexcept {root();scope.put(0x148,std::uintptr_t(0));scope.put(0x150,action.data());}
    void absent() noexcept {controller.put(0x34,0u);controller.put(0x48,std::uintptr_t(0));controller.put(0x50,std::uintptr_t(0));}
    void queued() noexcept {absent();queue.put(0,action.data());controller.put(0x48,queue.data());controller.put(0x50,queue.data()+8);}
    void enroll() noexcept {
        row=g_ms_table_d62c.offer(controller.address(),action.address(),entity,123,0xBF91494Fu,922,0,nullptr,true,tick);
        if(!row)std::abort();row->instantD72=true;row->state=ms::State::Playing;row->playing=tick;
        g_ms_live_d62c=true;writeAddress=action.address()+kActSpeedBias;observedAction=action.address();
    }
    void lease() noexcept {enroll();MsApplyLeaseD62C(*row,64.f,tick);}
    void update(unsigned elapsed=10) noexcept {
        tick+=elapsed;writeAddress=action.address()+kActSpeedBias;observedAction=action.address();
        MsReactionUpdateD62C(controller.data());
    }
    float speed() const noexcept {return action.get<float>(kActSpeedBias);}
};
static void check(bool ok,const char* message){++checks;if(!ok){++failures;std::printf("FAIL %s\n",message);}}
static void reset() {
    g_ms_table_d62c={};g_ms_live_d62c=false;g_ms_lock_d62c.clear();g_ms_ids_next_d62c=0;
    for(auto& x:g_ms_ids_d62c)x={};regions.clear();tick=1000;
    unreadable=writeAddress=observedAction=0;reads=writeAttempts=writes=actionReads=0;
    failSpeedReads=failReadback=0;failWrite=writeThenFail=false;afterSpeedRead={};
    instant::activities=true;instant::combat=true;
}
static void ready(const Actor& a){check(a.row&&a.row->leased&&a.row->saved==1.f&&a.row->ours==64.f&&a.speed()==64.f&&writes==1,"seeded admitted root acquires one exact speed lease");}
static void restored(const Actor& a){check(a.speed()==1.f&&!a.row->leased&&a.row->restores==1&&writes==2,"known original owner restores exact saved speed once");}

int main(){
    for(bool preferenceOff:{false,true}){
        reset();Actor a;a.lease();ready(a);instant::combat=!preferenceOff;
        failSpeedReads=1;a.update();
        check(a.row->leased&&a.speed()==64.f&&writes==1,"temporary speed read failure keeps cleanup debt without a write");
        a.update();restored(a);a.update();check(writes==2,"completed lease never restores twice");
        check(instant::Combat()==!preferenceOff,"cleanup does not change the user's combat preference");
    }
    for(unsigned field:{unsigned(kActToken),unsigned(kActFragId),0u}){
        reset();Actor a;a.lease();ready(a);unreadable=a.action.address()+field;a.update();
        check(a.row->leased&&a.speed()==64.f&&writes==1,"unknown action identity defers cleanup");
        unreadable=0;a.update();restored(a);
    }
    {
        reset();Actor a;a.lease();ready(a);unreadable=a.controller.address()+kCtrlEntityId;a.update();
        check(a.row->leased&&writes==1,"unknown controller entity defers cleanup");
        unreadable=0;a.update();restored(a);
    }
    {
        reset();Actor a;a.lease();ready(a);unreadable=a.controller.address()+0x38;actionReads=0;a.update();
        check(a.row->leased&&writes==1&&actionReads==0,"unknown native containers do not dereference or restore saved action");
        a.update(600);a.update(3000);a.update(3000);
        check(a.row->leased&&a.row->state==ms::State::Lost&&writes==1,"unreadable cleanup survives observer and old two-second timeouts");
        unreadable=0;a.update();restored(a);
    }
    {
        reset();Actor a;a.enroll();failReadback=1;MsApplyLeaseD62C(*a.row,64.f,tick);
        check(a.speed()==64.f&&a.row->leased&&a.row->writes==1&&a.row->restores==0,"installed acquisition with unreadable readback retains cleanup debt");
        a.update();restored(a);
    }
    {
        reset();Actor a;a.lease();ready(a);failWrite=true;a.update();
        check(a.speed()==64.f&&a.row->leased&&a.row->restores==0&&writes==1,"failed restoration write keeps original cleanup debt and no false restore count");
        failWrite=false;a.update();restored(a);
    }
    for(bool afterWriteFault:{false,true}){
        reset();Actor a;a.lease();ready(a);
        if(afterWriteFault)writeThenFail=true;else failReadback=1;
        a.update();
        check(a.speed()==1.f&&a.row->leased&&a.row->restores==0&&writes==2,"uncertain completed restore remains pending until a positive value sample");
        writeThenFail=false;a.update();
        check(a.speed()==1.f&&!a.row->leased&&writes==2,"positive restored value ends uncertain debt without a second write");
    }
    {
        reset();Actor a;a.enroll();failWrite=true;MsApplyLeaseD62C(*a.row,64.f,tick);
        check(a.row->leased&&a.row->writes==1&&a.speed()==1.f&&writes==0,"uncertain acquisition remembers only one attempted lease");
        failWrite=false;a.update();a.update();
        check(!a.row->leased&&a.speed()==1.f&&writeAttempts==1,"positive unchanged native value ends failed acquisition without retrying");
    }
    {
        reset();Actor a;a.lease();ready(a);a.action.put(kActSpeedBias,2.f);a.update();
        check(a.speed()==2.f&&!a.row->leased&&writes==1,"foreign native speed is never overwritten by cleanup");
    }
    for(unsigned replacement=0;replacement<4;++replacement){
        reset();Actor a;a.lease();ready(a);
        if(replacement==0)a.action.put(kActToken,124u);
        if(replacement==1)a.controller.put(kCtrlEntityId,12u);
        if(replacement==2)a.action.put(kActFragId,923);
        if(replacement==3)a.action.put(0,g_base.load()+0x1234);
        a.update();
        check(a.speed()==64.f&&!a.row->leased&&writes==1,"positive action/controller replacement cannot receive former owner's saved speed");
    }
    for(unsigned replacement=0;replacement<4;++replacement){
        reset();Actor a;a.lease();ready(a);
        afterSpeedRead=[&]{
            if(replacement==0)a.action.put(kActToken,124u);
            if(replacement==1)a.controller.put(kCtrlEntityId,12u);
            if(replacement==2)a.absent();
            if(replacement==3)unreadable=a.controller.address()+0x38;
        };
        a.update();
        check(a.speed()==64.f&&writes==1,"ownership change after speed sample is rechecked before write");
        if(replacement==3){check(a.row->leased,"late unknown membership retains debt");unreadable=0;a.update();restored(a);}
        else check(!a.row->leased,"late positive replacement/absence retires former debt");
    }
    for(bool acquiring:{false,true}){
        reset();Actor a;if(acquiring)a.enroll();else{a.lease();ready(a);}
        afterSpeedRead=[&]{a.action.put(kActSpeedBias,2.f);};
        if(acquiring)MsApplyLeaseD62C(*a.row,64.f,tick);else a.update();
        check(a.speed()==2.f&&!a.row->leased&&writes==(acquiring?0u:1u),"speed changed during ownership proof is never overwritten");
    }
    {
        reset();Actor a;a.lease();ready(a);a.queued();a.update();
        check(a.row->leased&&a.speed()==64.f&&writes==1,"queued membership cannot lend scope-write authority");
        a.root();a.update();restored(a);
    }
    {
        reset();Actor a;a.lease();ready(a);a.exiting();a.update();restored(a);
    }
    {
        reset();Actor a;a.lease();ready(a);a.absent();actionReads=0;a.update();
        check(!a.row->leased&&writes==1&&actionReads==0,"positive absence retires without following the removed action pointer");
    }
    {
        reset();Actor a;a.lease();ready(a);Actor unrelated(22);const auto before=reads;
        MsReactionUpdateD62C(unrelated.controller.data());
        check(reads==before&&writes==1,"untracked controller cannot service another action's debt");
        instant::activities=false;a.update();restored(a);
        Actor next(23);MsReactionQueueD62C(next.controller.data(),next.action.data(),true);
        // The completed observer is still live until native retirement; retire
        // it before checking the quiet OFF admission path.
        a.absent();a.update();const auto quiet=reads;
        MsReactionQueueD62C(next.controller.data(),next.action.data(),true);
        check(reads==quiet&&g_ms_table_d62c.next==2,"master OFF admits no new occurrence after existing cleanup retires");
    }
    {
        reset();std::vector<std::unique_ptr<Actor>> actors;
        for(unsigned i=0;i<12;++i){actors.emplace_back(std::make_unique<Actor>(100+i));actors.back()->lease();}
        check(writes==12,"fixed table holds twelve separately owned leases");
        for(auto& a:actors){unreadable=a->controller.address()+0x38;a->update();a->update(600);a->update(3000);}
        unsigned pending=0;for(const auto& row:g_ms_table_d62c.rows)pending+=row.leased;
        Actor extra(200);
        auto* refused=g_ms_table_d62c.offer(extra.controller.address(),extra.action.address(),200,123,0xBF91494Fu,922,0,nullptr,true,tick);
        check(pending==12&&!refused&&g_ms_table_d62c.dropped==1&&writes==12,"persistent unknown cleanup stays bounded at twelve rows and refuses reuse");
        unreadable=0;actors[0]->absent();actors[0]->update();
        auto* reused=g_ms_table_d62c.offer(extra.controller.address(),extra.action.address(),200,123,0xBF91494Fu,922,0,nullptr,true,tick);
        check(reused&&reused->entity==200&&!reused->leased,"only positive retirement releases a pending row for a new occurrence");
    }
    std::printf("D76R1 paired cleanup: %u/%u; complete actual adapter, native memory/leaves controlled; game/SEH/headset NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
