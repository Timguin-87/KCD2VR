// Actual native-result observer + actual Mod workstation producer with native
// memory/return-address/call leaves doubled. No retail code is executed.
#include "../src/vrik/alchemy_input19c4.h"
#include "../src/vrik/alchemy_palm_lock27.h"
#include "../src/vrik/workstation178r20c1.h"
#include "../src/activity_input163.h"
#include "../src/diagnostics/interaction_receipt59c10.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
static std::uint64_t clock19=1000;
static void* caller19=nullptr;
static std::vector<std::pair<std::uintptr_t,std::size_t>> memory19;
static unsigned nativeReads38=0;
static void (*readAfter38)(std::uintptr_t)=nullptr;
static std::uint64_t GetTickCount64(){return clock19;}
static void* Return19(){return caller19;}
#define _ReturnAddress Return19
namespace kcd2vr::activity_input163 {
static std::atomic<std::uintptr_t> base175{0};
template<class T>bool AlchemyInputRead19c4(std::uintptr_t at,T& v){++nativeReads38;for(auto s:memory19)if(at>=s.first&&at+sizeof(T)<=s.first+s.second){std::memcpy(&v,reinterpret_cast<void*>(at),sizeof(T));if(readAfter38)readAfter38(at);return true;}return false;}
#ifdef C38_OLD_HINT_OBSERVER
#include C38_OLD_HINT_OBSERVER
#else
#include "../src/vrik/alchemy_input_observer19c4.inl"
#endif
#include "../src/vrik/alchemy_selection38.inl"
}
#undef _ReturnAddress
namespace kcd2vr::interaction_alignment {
static HandCandidate candidate19;static int lockedHand19=1;
int LockedPalmOwner19c4(std::uint64_t) noexcept {return lockedHand19;}
bool LastHandCandidate(HandCandidate& out) noexcept {out=candidate19;return true;}
}
namespace kcd2vr::diagnostic_trace {static unsigned writes19=0;static void Write(const char*){++writes19;}}
static std::uint32_t* changedSelection19=nullptr;
static unsigned originalCalls19=0;static const char* nativeString19="";static bool restart19=false;
static void* Original19(void* self,void* out,unsigned type){assert(self&&type<=21);++originalCalls19;
    if(changedSelection19)*changedSelection19=99;
    if(restart19)kcd2vr::activity_input163::witness175.lifetime175.fetch_add(1);
    std::memcpy(out,&nativeString19,sizeof(nativeString19));return out;}
template<class T,std::size_t N>static void Put19(std::array<unsigned char,N>& a,unsigned off,T value){assert(off+sizeof(value)<=N);std::memcpy(a.data()+off,&value,sizeof(value));}
static unsigned checks19=0;static void ck19(bool v){++checks19;if(!v)std::fprintf(stderr,"FAIL alchemy check %u\n",checks19);assert(v);}
int main(){
    kcd2vr::workstation178r20c1::alchemyPassenger42=false; // test explicit grip-stage option

    using namespace kcd2vr;namespace ai=activity_input163;namespace al=alchemy_input19c4;using namespace workstation178r20c1;
    alignas(16)std::array<unsigned char,0xC00> player{};alignas(16)std::array<unsigned char,0x700> instanceMemory{};alignas(16)std::array<unsigned char,0x200> interactor{};
    constexpr std::uintptr_t base=0x50000000;const auto instance=reinterpret_cast<std::uintptr_t>(instanceMemory.data());
    const char empty[]="",hold[]="alch_crush";void* out=nullptr;
    memory19={{instance,instanceMemory.size()},{reinterpret_cast<std::uintptr_t>(player.data()),player.size()},
        {reinterpret_cast<std::uintptr_t>(interactor.data()),interactor.size()},{reinterpret_cast<std::uintptr_t>(&out),sizeof(out)},{reinterpret_cast<std::uintptr_t>(empty),sizeof(empty)},
        {reinterpret_cast<std::uintptr_t>(hold),sizeof(hold)}};
    Put19(instanceMemory,0,base+0x3F60B08);Put19(instanceMemory,0x18,reinterpret_cast<std::uintptr_t>(player.data()));
    Put19(instanceMemory,0x2C,std::uint8_t(1));Put19(instanceMemory,0x150+4*2,std::uint32_t(42));
    Put19(player,0,base+0x3AA4B98);Put19(player,0x38,std::uintptr_t(123));
    Put19(player,0xB08,reinterpret_cast<std::uintptr_t>(interactor.data()));
    Put19(interactor,0,base+0x3A6A0C8);Put19(interactor,8,reinterpret_cast<std::uintptr_t>(player.data()));Put19(interactor,0xF4,std::uint32_t(42));
    ai::base175=base;ai::witness175.ready=true;ai::witness175.Begin(ai::Kind175::Alchemy,instance);
    ai::alchemyHoldReady19c4=true;ai::originalAlchemyHold19c4=Original19;caller19=reinterpret_cast<void*>(base+0x8D1E92);
    nativeString19=empty;ck19(ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2)==&out&&originalCalls19==1);
    const auto life=ai::witness175.lifetime175.load();al::Observation value{};
    ck19(al::lookup(instance,life,42,clock19,value)&&!value.hold&&value.type==2);
    // A getter may run before native candidate enumeration has finished. Only
    // the existing outer native-frame completion publishes acquisition target.
    ai::AlchemySelectionNativeFrameEndImpl38();
    ck19(al::lookupSelected(instance,life,clock19,value)&&value.entity==42&&!value.hold);
    nativeString19=hold;ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
    ck19(al::lookup(instance,life,42,clock19,value)&&value.hold&&originalCalls19==2);
    for(unsigned fault=0;fault<7;++fault){al::clear();nativeString19=empty;caller19=reinterpret_cast<void*>(base+0x8D1E92);
        ai::alchemyHoldReady19c4=true;ai::witness175.transitionDepth175=0;
        Put19(instanceMemory,0x18,reinterpret_cast<std::uintptr_t>(player.data()));Put19(instanceMemory,0x2C,std::uint8_t(1));
        if(fault==0)caller19=nullptr;if(fault==1)ai::alchemyHoldReady19c4=false;if(fault==2)ai::witness175.transitionDepth175=1;
        if(fault==3)Put19(instanceMemory,0x18,std::uintptr_t(0));if(fault==4)Put19(instanceMemory,0x2C,std::uint8_t(0));
        if(fault==5)restart19=true;
        const auto before=originalCalls19;ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,fault==6?21:2);
        ck19(originalCalls19==before+1&&!al::lookup(instance,ai::witness175.lifetime175.load(),42,clock19,value));restart19=false;
    }
    ai::alchemyHoldReady19c4=true;ai::witness175.transitionDepth175=0;Put19(instanceMemory,0x2C,std::uint8_t(1));
    caller19=reinterpret_cast<void*>(base+0x8D1E92);nativeString19=empty;ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
    const auto currentLife=ai::witness175.lifetime175.load();
    ck19(!al::lookup(instance,currentLife,42,clock19-1,value)&&!al::lookup(instance,currentLife,42,clock19+81,value));
    al::observed[3].publish({instance,currentLife,clock19,42,3,true});
    ck19(!al::lookup(instance,currentLife,42,clock19,value)); // same entity, conflicting verbs is unknown
    al::observed[3].publish({});
    // Current native selected entity is a separate authority from queried type.
    // C38 selection is read at completed-frame cadence, independently of hints.
    for(unsigned fault=0;fault<6;++fault){
        al::clear();Put19(interactor,0,base+0x3A6A0C8);Put19(interactor,8,reinterpret_cast<std::uintptr_t>(player.data()));
        Put19(player,0xB08,reinterpret_cast<std::uintptr_t>(interactor.data()));Put19(interactor,0xF4,std::uint32_t(42));
        if(fault==0)Put19(interactor,0,std::uintptr_t(123));
        if(fault==1)Put19(interactor,8,std::uintptr_t(123));
        if(fault==2)Put19(player,0xB08,std::uintptr_t(0));
        if(fault==3)Put19(interactor,0xF4,std::uint32_t(43)); // queried42 is not selected43
        if(fault==4)changedSelection19=reinterpret_cast<std::uint32_t*>(interactor.data()+0xF4);
        if(fault==5)Put19(interactor,0xF4,std::uint32_t(0));
        const auto calls=originalCalls19;ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
        ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(originalCalls19==calls+1&&!al::lookupSelected(instance,currentLife,clock19,value));changedSelection19=nullptr;
    }
    Put19(interactor,0xF4,std::uint32_t(42));ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
    ai::AlchemySelectionNativeFrameEndImpl38();
    ck19(al::lookupSelected(instance,currentLife,clock19,value));
    ck19(!al::lookupSelected(instance,currentLife,clock19+81,value)&&!al::lookupSelected(instance,currentLife+1,clock19,value));
    {   // C38 log-derived regression: one native hint batch transiently clears
        // +F4 then offers candidates. It must not erase the last completed
        // frame's fresh selection at the physical Grip edge.
        al::Selection selected38{};al::PalmLock27 lock38;
        ck19(lock38.update(true,instance,currentLife,7,-1,{})==0);
        const auto calls38=originalCalls19;
        Put19(interactor,0xF4,std::uint32_t(0));
        ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
        ck19(al::selectionSnapshot(instance,currentLife,clock19,selected38)&&selected38.entity==42);
        ck19(lock38.update(true,instance,currentLife,7,1,selected38)==42);
        Put19(interactor,0xF4,std::uint32_t(43));
        ai::NativeAlchemyHold19c4(instanceMemory.data(),&out,2);
        ck19(al::selectionSnapshot(instance,currentLife,clock19,selected38)&&selected38.entity==42);
        ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(al::selectionSnapshot(instance,currentLife,clock19,selected38)&&selected38.entity==43);
        ck19(originalCalls19==calls38+2); // frame publisher calls no native getter/action
        ck19(lock38.update(true,instance,currentLife,7,1,selected38)==42); // cannot retarget closed Grip
        ck19(lock38.update(true,instance,currentLife,7,-1,selected38)==0);
        ck19(lock38.update(true,instance,currentLife,7,1,selected38)==43);
        // A REAL empty completed frame clears authority; unknown never retains
        // the previous nonzero target and the original 80ms expiry is unchanged.
        Put19(interactor,0xF4,std::uint32_t(0));ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(!al::selectionSnapshot(instance,currentLife,clock19,selected38));
        al::Selection raw38{};ck19(al::selected.read(raw38)&&raw38.instance==instance&&raw38.entity==0);
        Put19(interactor,0xF4,std::uint32_t(42));ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(!al::selectionSnapshot(instance,currentLife,clock19+81,selected38));
        // If selection/lifetime changes during the final coherent reads, refuse
        // the copy instead of retaining or guessing an acquisition target.
        static std::uintptr_t selectedAddress38=0;
        selectedAddress38=reinterpret_cast<std::uintptr_t>(interactor.data()+0xF4);
        readAfter38=[](std::uintptr_t at){if(at==selectedAddress38){const std::uint32_t other=99;std::memcpy(reinterpret_cast<void*>(at),&other,sizeof(other));readAfter38=nullptr;}};
        ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(!al::selectionSnapshot(instance,currentLife,clock19,selected38));
        Put19(interactor,0xF4,std::uint32_t(42));
        readAfter38=[](std::uintptr_t at){if(at==selectedAddress38){kcd2vr::activity_input163::witness175.transitionDepth175=1;readAfter38=nullptr;}};
        ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(!al::selectionSnapshot(instance,currentLife,clock19,selected38));ai::witness175.transitionDepth175=0;
        // Unavailable/opted-out/non-alchemy frames use only copied state.
        for(unsigned disabled38=0;disabled38<5;++disabled38){
            ai::AlchemySelectionNativeFrameEndImpl38();const auto reads38=nativeReads38;
            if(disabled38==0)ai::alchemyHoldReady19c4=false;
            if(disabled38==1)ai::witness175.ready=false;
            if(disabled38==2)ai::witness175.transitionDepth175=1;
            if(disabled38==3)ai::trackedHands59c7f[0]=false;
            if(disabled38==4)ai::witness175.active[0]=0;
            ai::AlchemySelectionNativeFrameEndImpl38();
            ck19(nativeReads38==reads38&&!al::selectionSnapshot(instance,currentLife,clock19,selected38));
            ai::alchemyHoldReady19c4=true;ai::witness175.ready=true;ai::witness175.transitionDepth175=0;
            ai::trackedHands59c7f[0]=true;ai::witness175.active[0]=instance;
        }
        ai::AlchemySelectionNativeFrameEndImpl38();
    }
    {   // C37I1: the Mod loop starts before native frame completion. Its old
        // clock must not reject the newly completed selection at a Grip edge.
        const bool alchemyPointer59c2=true;const unsigned f5Mode=2;
        const auto ladder_now_ms=clock19;clock19+=16;
        al::PalmLock27 alchemyPalmLock27;
        struct {int locked=-1;std::uint32_t lockedEntity=0;} palms59c9;
        interaction_alignment::HandCandidate candidate59c7f{};candidate59c7f.tick=clock19;candidate59c7f.owner=1;candidate59c7f.object=true;candidate59c7f.entity=777;
        auto acquire27=[&](int side){
            palms59c9.locked=side;
#ifdef C37I1_OLD_PALM_CLOCK
#include C37I1_OLD_PALM_CLOCK
#else
#include "../src/vrik/interaction_palm_lock27.inl"
#endif
            return palms59c9.lockedEntity;
        };
        Put19(interactor,0xF4,std::uint32_t(42));ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(acquire27(-1)==0);
        ck19(acquire27(1)==42); // old ladder_now_ms rejects tick+16 and consumes this edge
        ck19(ladder_now_ms+16==clock19); // resampling did not retimestamp the worker frame
        Put19(interactor,0xF4,std::uint32_t(43));ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(acquire27(1)==42); // a closed Grip still cannot retarget
        ck19(acquire27(-1)==0&&acquire27(1)==43);
        // Real age/future violations remain rejected with the unchanged 80ms limit.
        ck19(acquire27(-1)==0);clock19+=81;
        ck19(acquire27(1)==0);
        ck19(acquire27(-1)==0);
        al::selected.publish({instance,currentLife,clock19+1,42});
        ck19(acquire27(1)==0);
        ck19(acquire27(-1)==0);
        Put19(interactor,0xF4,std::uint32_t(0));ai::AlchemySelectionNativeFrameEndImpl38();
        ck19(acquire27(1)==0); // completed empty selection cannot borrow offered777
    }
    // Actual Mod producer plus actual Controller.apply: a release-only action
    // pulses A while the physical left Grip remains closed and wrist-owned.
    const Kind kind=Kind::Alchemy;const auto& witness=ai::witness175;Controller workstationC1;
    combat_rules178r10::f5Rung=2;setExperiment36(true);
    std::array<Sample,2> hands{};auto wsNow=clock19;
    auto run=[&](float grip,bool known,bool holdHint,bool available=true){
        clock19+=20;wsNow=clock19;for(auto& h:hands){h.valid=true;h.tick=wsNow;h.grip=0;++h.sequence;}hands[1].grip=grip;
        if(known){al::observed[2].publish({instance,witness.lifetime175.load(),wsNow,42,2,holdHint});
            al::selected.publish({instance,witness.lifetime175.load(),wsNow,42});}else al::clear();
        interaction_alignment::candidate19={};interaction_alignment::candidate19.tick=wsNow;interaction_alignment::candidate19.owner=0;interaction_alignment::candidate19.object=true;interaction_alignment::candidate19.entity=777; // later offer differs from final entity AND owner

        if(!available)interaction_alignment::candidate19.entity=43;
        auto result=workstationC1.update(kind,instance,-1,hands,wsNow);
        #include "../src/vrik/alchemy_input_producer19c4.inl"
        bool a=false,lb=true,rb=true;float rt=.25f;workstationC1.apply(wsNow,a,lb,rb,rt);
        ck19(a==result.use&&lb&&rb&&rt==.25f);return a;
    };
    // C46 deliberately retired this legacy Controller A/height pulse. It must
    // stay inert even when its old explicit experiment flag and native hints
    // are present; the connected semi_workstations46/Guided fixtures own input.
    for(unsigned i=0;i<20;++i)ck19(!run(i?1.f:0.f,true,(i&1)!=0));
    ck19(!workstationC1.held[1]&&!workstationC1.ownsGrip(1,wsNow));
    ai::witness175.lifetime175.fetch_add(1);ck19(!run(1,true,false));
    setExperiment36(false);ck19(!run(1,true,false));
    ck19(diagnostic_trace::writes19==0); // recorder-off observer and producer stay quiet
    {   // C37 F5 rung 1 alchemy stage receipts: edge-only, per lifetime, never a verb table.
        al::Edge37 edge{};al::Stage37 before{};al::Stage37 idle{};idle.selectedType=21;
        const al::Hint37 pour{42,al::textHash37("alch_pour",9),false},crush{42,al::textHash37("alch_crush",10),true};
        ck19(edge.update(instance,7,14,pour,idle,before)==3);                 // first hint + first stage
        ck19(edge.update(instance,7,14,pour,idle,before)==0);                 // same sample: no line
        for(unsigned i=0;i<4000;++i){idle.selectedEntity=(i&1)?42:0;
            ck19(edge.update(instance,7,14,pour,idle,before)==0);}             // C37C2 transient hover flood is not a stage change
        ck19(edge.update(instance,7,2,crush,idle,before)==1);                 // another type's hint only
        al::Stage37 running=idle;running.selectedType=14;running.action=0x1000;running.actionVft=0x3A5C670;running.started=1;
        running.child=0x2000;running.childStatus=2;running.childToken=55;
        ck19(edge.update(instance,7,14,pour,running,before)==2&&std::strcmp(al::stageEvent37(before,running),"START")==0);
        al::Stage37 paused=running;paused.childStatus=1;
        ck19(edge.update(instance,7,14,pour,paused,before)==2&&std::strcmp(al::stageEvent37(before,paused),"STATE")==0);
        al::Stage37 other=paused;other.action=0x3000;other.actionVft=0x3F60770;other.child=0;other.childStatus=-1;
        ck19(edge.update(instance,7,14,pour,other,before)==2&&std::strcmp(al::stageEvent37(before,other),"SWITCH")==0);
        ck19(edge.update(instance,7,14,pour,idle,before)==2&&std::strcmp(al::stageEvent37(before,idle),"END")==0);
        ck19(edge.update(instance,8,14,pour,idle,before)==3);                 // new minigame lifetime: fresh edges
        ck19(edge.update(instance,8,14,{42,pour.text,true},idle,before)==1);  // hold flag change is its own edge
        ck19(al::textHash37("alch_pour",9)!=al::textHash37("alch_crush",10));
        // Genuine unusable/unknown hint changes remain visible.
        ck19(edge.update(instance,8,14,{42,al::textHash37("",0),false},idle,before)==1);
        al::ReceiptBudget38 budget;
        for(unsigned i=0;i<256;++i)ck19(budget.admit(3)==3);
        ck19(budget.admit(3)==6); // stage plus one finite-budget warning
        for(unsigned i=257;i<512;++i)ck19(budget.admit(3)==2);
        ck19(budget.admit(3)==0&&budget.exhausted());
        budget={};ck19(budget.admit(3)==3); // new capture re-arms exact edges/budget
    }
    std::printf("PASS %u actual alchemy native-hint observer/Grip producer checks; native leaves mocked\n",checks19);
}
