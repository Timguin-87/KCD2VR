// Actual ordinary producer + native-entry capture + current control predicate;
// only native/OS/memory leaves from the retained fixture are substituted.
#include "diagnostics/interaction_cycle_h4.h"
namespace diagnostics {namespace interaction_cycle_h4=kcd2vr::diagnostics::interaction_cycle_h4;}
#pragma warning(push)
#pragma warning(disable:4716) // retained main is included but never called
#define main retained_native_door_owner_main_h5
#include "native_door_io19c4_test.cpp"
#undef main
#pragma warning(pop)
#include "generated_door_entry_h5.inc"

static int checksH5=0,failuresH5=0,nativeCallsH5=0,scenarioH5=0;
static unsigned currentModeH5=0;
static int currentSideH5=0;
static DoorRequestWitness38 witnessH5{};
static void checkH5(bool pass,const char* text) {
    ++checksH5;if(!pass){++failuresH5;std::fprintf(stderr,"FAIL %s\n",text);}
}
static bool ownedH5(std::uint64_t* serial=nullptr) {
    return kcd2vr::door66::OwnedMotionFor(reinterpret_cast<std::uintptr_t>(player.data()),42,
        reinterpret_cast<std::uintptr_t>(entity.data()),now,serial);
}
static std::uintptr_t __fastcall EmptyNestedH5(void*,const char*,std::uint32_t,bool,float) {
    ++nativeCallsH5;checkH5(!ownedH5(),"nested unqualified native request has no inherited door proposal");return 0x1200;
}
static std::uintptr_t __fastcall NativeH5(void*,const char*,std::uint32_t,bool,float) {
    ++nativeCallsH5;
    const bool expected=door_use46::inputMode(currentModeH5);
    std::uint64_t serial=0;
    checkH5(ownedH5(&serial)==expected,"actual public motion owner reaches nested original only for current input modes");
    if(expected){
        checkH5(serial==(0x100000000ull|witnessH5.hand65.press),"proposal preserves original physical press without minting native accepted request");
        if(scenarioH5==1){
            kcd2vr::combat_rules178r10::f5Rung=currentModeH5==2?0:2;
            checkH5(!ownedH5(),"mode change inside native callback revokes original proposal");
        }else if(scenarioH5==2){
            kcd2vr::workstation178r20c1::experimentEpoch36.fetch_add(1);
            checkH5(!ownedH5(),"experiment/source epoch change inside callback revokes proposal");
        }else if(scenarioH5==3){
            g_ctx_menu=true;checkH5(!ownedH5(),"menu entered inside callback revokes proposal");
        }else if(scenarioH5==4){
            DoorRequestWitness38 none{};
            const auto result=DoorNativeCall66(&EmptyNestedH5,player.data(),"door_l_f_o",42,false,1,none,currentSideH5,now);
            checkH5(result==0x1200&&ownedH5(),"nested original executes once and restores exact outer proposal");
        }
        checkH5(!kcd2vr::door66::OwnedMotionFor(reinterpret_cast<std::uintptr_t>(player.data()),43,
            reinterpret_cast<std::uintptr_t>(entity.data()),now,nullptr),"different target cannot borrow proposal");
    }
    return 0x4400;
}
int main() {
    for(unsigned mode:{0u,1u,2u,3u}) for(int hand:{0,1}) for(int scenario=0;scenario<5;++scenario){
        Setup();currentModeH5=mode;currentSideH5=hand;scenarioH5=scenario;nativeCallsH5=0;
        // Acquire through actual mode0/2/3 producer. Reference1 receives no
        // proposal even if a just-copied mode2 source exists at the transition.
        const auto producerMode=mode==1?2:mode;
        kcd2vr::combat_rules178r10::f5Rung=producerMode;
        Producer producer;producer.f5Mode=producerMode;producer.comparison114=producerMode!=0;
        witnessH5=PhysicalDoorEntry62(producer,true,hand);
        kcd2vr::door_signal19c4r4::ready=true;
        checkH5(witnessH5.handSource65&&witnessH5.acceptedUse62.valid,"actual producer/capture supplies exact source and transported physical input");
        checkH5(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now),"F5 recorder remains OFF");
        kcd2vr::combat_rules178r10::f5Rung=mode;
        const auto result=DoorNativeCall66(&NativeH5,player.data(),"door_l_f_o",42,false,1,witnessH5,hand,now);
        checkH5(result==0x4400,"opaque original native return is forwarded unchanged");
        checkH5(nativeCallsH5==((scenario==4&&mode!=1)?2:1),"original called once per exact outer/nested invocation");
        checkH5(!ownedH5(),"temporary motion proposal ends immediately after original returns");
        checkH5(speedCalls==0&&timeCalls==0,"entry does not seek pause finish or mutate native animation");
    }
    for(int invalid=0;invalid<8;++invalid){
        Setup();kcd2vr::combat_rules178r10::f5Rung=0;
        Producer producer;producer.f5Mode=0;producer.comparison114=false;
        auto witness=PhysicalDoorEntry62(producer,true,0);kcd2vr::door_signal19c4r4::ready=true;
        if(invalid==0)witness.handSource65=false;
        if(invalid==1)witness.acceptedUse62={};
        if(invalid==2)door_surface65::witnessMailbox.publish({});
        if(invalid==3)g_ctx_menu=true;
        if(invalid==4)reservations65=1;
        if(invalid==5)++g_pose_epoch6689;
        if(invalid==6)handTick=now-81;
        if(invalid==7)headTick=now-61;
        checkH5(!DoorEntry66(witness,42,0,now).serial,"ordinary mode retains original missing-source/context/surface/tracking refusal");
    }
    std::printf("DOOR_ENTRY_H5 checks=%d failures=%d\n",checksH5,failuresH5);return failuresH5?1:0;
}
