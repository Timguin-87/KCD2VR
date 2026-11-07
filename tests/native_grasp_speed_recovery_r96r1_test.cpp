// Full production controller/native-grasp adapter. The existing fixture supplies
// native memory/call leaves; this test never calls SettleSpeed/Quarantine itself.
// Host receipts prove callback ordering and retained debt, not engine progress.
#define main retainedNativeGraspMainR96R1
#include "native_grasp_owner13_test.cpp"
#undef main
#include "pickup_target_d40r1_stubs.cpp"
#include <cstdlib>

// Unrelated full-owner module leaves, matching the retained D60 owner adapter.
namespace kcd2vr::frik_pose_probe {int ReloadMode47C2() noexcept{return 0;}}
namespace kcd2vr::diagnostic_trace {
void WriteFor(diagnostics::capture168::Domain,const char* text) noexcept{Write(text);}
}

using namespace kcd2vr::action_time_owner59c11;
static unsigned checksR96R1=0, nativeUpdatesR96R1=0, nativeUnpausedR96R1=0;
static float nativeSpeedR96R1=-1;
static bool recordSpeedR96R1=true;
static void checkR96R1(bool value,const char* why){
    ++checksR96R1;
    if(!value){std::printf("FAIL R96R1 native grasp: %s\n",why);std::exit(1);}
}
static void NativeUpdateR96R1(void* ctrl,float dt){
    checkR96R1(ctrl==controller.data()&&dt>0,"actual original Update receives its native arguments");
    ++nativeUpdatesR96R1;
    // This is an observation at the substituted native Update leaf, not an
    // invented game event or a claim that a retail animation completed.
    if(recordSpeedR96R1&&Rd<void*>(scope.data(),kScopeAction,nullptr)==action.data()){
        nativeSpeedR96R1=Rd<float>(action.data(),kActSpeedBias,-1);
        if(nativeSpeedR96R1>0)++nativeUnpausedR96R1;
    }
}
static void updateR96R1(std::uint64_t now){frame(now);UpdateDetour(controller.data(),.02f);}
static void heldFaultR96R1(){
    refusedWrite19=nullptr;reset();attach(false);
    checkR96R1(attaches==1&&g_owner.state==Held&&g_owner.speedWritten&&
             Rd<float>(action.data(),kActSpeedBias,-1)==0,"real Attach acquires the native zero-speed hold");
    nativeUpdatesR96R1=nativeUnpausedR96R1=0;nativeSpeedR96R1=-1;recordSpeedR96R1=true;
    g_update_original=&NativeUpdateR96R1;
    // A transient missing native world-link observation reaches the real
    // Identity13 -> GraspFault13 path, while the action/root/hand stay exact.
    put(item.data(),0x88,static_cast<void*>(nullptr));updateR96R1(1020);
    checkR96R1(g_native_fault13&&g_pick13.terminal.chosen==ng13::Terminal::Fault&&
             g_pick13.bind.token&&g_owner.speedWritten,"production controller faults after the missing native item link");
    checkR96R1(nativeUpdatesR96R1==1&&nativeSpeedR96R1==0&&nativeUnpausedR96R1==0,
             "faulting Update still observes the previously owned pause");
    put(item.data(),0x88,static_cast<void*>(kcd2vr::action_time_owner59c11::link.data()));
}
static void debtHeldR96R1(){
    checkR96R1(g_native_fault13&&g_pick13.bind.token&&g_owner.state!=Idle&&
             Rd<std::uint64_t>(slots.data(),0x260,0)==key,"speed recovery retains the held-item quarantine");
    checkR96R1(attaches==1&&drops==0&&transfers==0&&finishes==0&&cleanup==0,
             "speed recovery makes no item mutation, cleanup replay or forced finish");
    ng13::Binding out{};
    checkR96R1(!ReadGraspBinding(out)&&!NativeGraspAvailable63(),"quarantine still revokes semantic/presentation admission");
}

#ifdef KCD2VR_GRASP_RETAINED_R96R1
int main(){kcd2vr::collab_h21::setThrowing(true);return kcd2vr::action_time_owner59c11::scenario();}
#else
int main(){
    kcd2vr::collab_h21::setThrowing(true);
    heldFaultR96R1();const auto firstToken=g_pick13.bind.token;
    updateR96R1(1040);
    checkR96R1(nativeSpeedR96R1==1&&nativeUnpausedR96R1==1&&!g_owner.speedWritten,
             "restore current held speed before original native Update without waiting for hand release");
    debtHeldR96R1();updateR96R1(1060);debtHeldR96R1();
    checkR96R1(nativeSpeedR96R1==1&&nativeUnpausedR96R1==2,"native Update continues receiving the restored speed");
    DetachLeaf(manager.data(),0);updateR96R1(1080);
    checkR96R1(!g_native_fault13&&g_owner.state==Idle&&!g_pick13.bind.token&&drops==0&&transfers==0,
             "only a real current-hand release settles the remaining fault");
    // A fresh native occurrence reuses the addresses, with a new native token
    // and a new explicit Use. No mod reset or process restart between visits.
    put(logic.data(),0xFE,std::uint8_t(0));put(scope.data(),kScopeAction,static_cast<void*>(action.data()));
    put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));
    put(action.data(),kActToken,unsigned(56));frame(1120);NoteUseSent(0,1120);
    QueueDetour(controller.data(),action.data(),0);UpdateDetour(controller.data(),.02f);attach();
    checkR96R1(attaches==2&&g_owner.state==Held&&g_pick13.bind.token>firstToken&&!g_native_fault13,
             "fresh native pickup succeeds after release without resetting the owner");
    frame(1140,0);UpdateDetour(controller.data(),.02f);
    checkR96R1(drops==1&&finishes==1&&cleanup==1&&transfers==0&&g_owner.state==Idle,
             "fresh gesture executes one actual native Drop and cleanup after fault recovery");
    std::puts("PASS R96R1 callback recovery: original Update sees speed 0 -> 1 while item debt remains; actual hand release -> fresh Attach -> one Drop, without reset");

    heldFaultR96R1();put(action.data(),kActSpeedBias,.75f);updateR96R1(1040);debtHeldR96R1();
    checkR96R1(nativeSpeedR96R1==.75f&&!g_owner.speedWritten,"positive foreign speed retires only our speed lease");
    put(action.data(),kActSpeedBias,0.f);updateR96R1(1060);debtHeldR96R1();
    checkR96R1(nativeSpeedR96R1==0&&!g_owner.speedWritten,"a later equal-valued native write is never reclaimed after foreign ownership");
    DetachLeaf(manager.data(),0);updateR96R1(1080);
    checkR96R1(!g_native_fault13&&g_owner.state==Idle&&nativeSpeedR96R1==0,
             "native hand release settles item debt while preserving the foreign zero speed");
    std::puts("PASS R96R1 foreign-writer ABA: 0 -> native .75 -> native 0 remains native-owned");

    heldFaultR96R1();refusedWrite19=action.data()+kActSpeedBias;updateR96R1(1040);debtHeldR96R1();
    checkR96R1(g_owner.speedWritten&&nativeSpeedR96R1==0,"failed restore retains the live speed debt");
    refusedWrite19=nullptr;updateR96R1(1060);debtHeldR96R1();
    checkR96R1(!g_owner.speedWritten&&nativeSpeedR96R1==1,"the next qualified callback can retry speed restitution, with item mutations still blocked");
    heldFaultR96R1();put(action.data(),kActSpeedBias,std::numeric_limits<float>::quiet_NaN());updateR96R1(1040);
    checkR96R1(g_native_fault13&&g_owner.speedWritten&&std::isnan(nativeSpeedR96R1),"nonfinite speed is unavailable proof and retains its debt");
    put(action.data(),kActSpeedBias,0.f);updateR96R1(1060);debtHeldR96R1();
    checkR96R1(!g_owner.speedWritten&&nativeSpeedR96R1==1,"valid compare-owned speed can recover after an unavailable read");
    std::puts("PASS R96R1 failed/nonfinite speed proof retains debt and recovers on the next qualified callback");

    for(int uncertain=0;uncertain<3;++uncertain){
        heldFaultR96R1();const auto token=g_pick13.bind.token;
        if(uncertain==0)g_outer_callbacks19c4.store((token<<16)|1);
        if(uncertain==1)g_outer_unobserved19c4.store(token);
        if(uncertain==2)g_outer_callbacks19c4.store((token+1)<<16);
        updateR96R1(1040);debtHeldR96R1();
        checkR96R1(g_owner.speedWritten&&nativeSpeedR96R1==0,"active, unobserved or foreign outer lifetime blocks all speed restoration");
    }
    heldFaultR96R1();
    {
        NativeLifetime19c4 outer(logic.data());updateR96R1(1040);debtHeldR96R1();
        checkR96R1(g_owner.speedWritten&&nativeSpeedR96R1==0,"a real open native outer scope retains speed debt");
    }
    updateR96R1(1060);debtHeldR96R1();
    checkR96R1(!g_owner.speedWritten&&nativeSpeedR96R1==1,"actual outer return permits exact live speed restoration");
    std::puts("PASS R96R1 outer lifetime: active/unobserved/foreign tokens refuse speed writes; actual outer return permits restitution");

    for(int replacement=0;replacement<6;++replacement){
        heldFaultR96R1();recordSpeedR96R1=false;
        if(replacement==0){put(scope.data(),kScopeAction,static_cast<void*>(nullptr));forbidRetiredRead19c4=true;}
        if(replacement==1)put(action.data(),kActToken,unsigned(56));
        if(replacement==2)put(action.data(),0,WhBase()+0x3B13F38);
        if(replacement==3)put(action.data(),kActContext,static_cast<void*>(nullptr));
        if(replacement==4)put(action.data(),kActStatus,int(3));
        if(replacement==5)put(action.data(),kActFragId,int(0));
        updateR96R1(1040);debtHeldR96R1();
        checkR96R1(g_owner.speedWritten,"missing/replaced root, token, class, context, status or fragment retains speed debt");
        checkR96R1(retiredReads19c4==0,"missing native root never authorizes reading the old action");
        forbidRetiredRead19c4=false;
        checkR96R1(Rd<float>(action.data(),kActSpeedBias,-1)==0,"no replacement proof permits a speed write");
    }
    std::puts("PASS R96R1 native identity controls: lost root, changed token/class/context/status/fragment preserve unresolved speed/item debt");

    heldFaultR96R1();put(manager.data(),0,std::uintptr_t(0));updateR96R1(1040);
    checkR96R1(nativeSpeedR96R1==1&&!g_owner.speedWritten&&g_native_fault13&&g_pick13.bind.token,
             "independent exact action proof restores speed while unreadable hand ownership remains quarantined");
    put(manager.data(),0,WhBase()+0x3EC9958);DetachLeaf(manager.data(),0);updateR96R1(1060);
    checkR96R1(!g_native_fault13&&g_owner.state==Idle,"later exact hand-release proof settles item debt without guessing through the missing manager");
    heldFaultR96R1();g_context13=false;g_mode=Off;updateR96R1(1040);
    checkR96R1(nativeSpeedR96R1==1&&!g_owner.speedWritten&&g_native_fault13&&g_pick13.bind.token,
             "feature/context exit still restitutes its exact live speed and retains item debt");
    std::puts("PASS R96R1 speed and hand proofs remain independent; context/off neither clears item debt nor strands owned speed");

    heldFaultR96R1();const auto cleanupToken=g_pick13.bind.token;
    // Actual native End/Cleanup adapters publish completion. The world item is
    // still in hand: cleanup is not treated as a native detach or destruction.
    put(action.data(),kActStatus,int(3));put(scope.data(),kScopeAction,static_cast<void*>(nullptr));
    {
        NativeLifetime19c4 outer(logic.data());PickupCleanupDetour13(logic.data());
        checkR96R1(cleanup==1&&g_cleanup_original19c4==cleanupToken&&g_cleanup_done19c4!=cleanupToken,
                 "cleanup return inside a live native outer is not final lifetime completion");
        recordSpeedR96R1=false;forbidRetiredRead19c4=true;updateR96R1(1040);
        checkR96R1(g_native_fault13&&g_owner.speedWritten&&retiredReads19c4==0,
                 "incomplete native outer retains speed debt without touching retired action memory");
    }
    updateR96R1(1060);
    checkR96R1(g_cleanup_done19c4==cleanupToken&&!g_owner.speedWritten&&g_native_fault13&&g_pick13.bind.token&&
             Rd<std::uint64_t>(slots.data(),0x260,0)==key&&retiredReads19c4==0,
             "exact completed native lifetime retires only speed debt, leaving the held item fault intact");
    DetachLeaf(manager.data(),0);updateR96R1(1080);
    checkR96R1(!g_native_fault13&&g_owner.state==Idle&&cleanup==1&&finishes==0&&drops==0&&transfers==0&&retiredReads19c4==0,
             "actual native hand release finally settles fault without stale reads or cleanup/drop replay");
    forbidRetiredRead19c4=false;
    std::puts("PASS R96R1 completed cleanup: actual outer return retires speed lease only; exact native hand release still required");
    std::printf("PASS R96R1 native grasp recovery: %u checks; full UpdateDetour/Attach/End/Cleanup/Drop adapters, explicit native leaves; no DLL/game/headset execution\n",checksR96R1);
    return 0;
}
#endif
