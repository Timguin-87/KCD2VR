// Full production scheduler + packet adapter. The native Update, C API and
// memory are explicit host leaves. Native game/SEH/runtime are not executed.
#define main retained_native_script_tests_r96
#include "native_script_t2_test.cpp"
#undef main
static unsigned checksR96=0;
static void needR96(bool v,const char* name){++checksR96;if(!v){std::fprintf(stderr,"FAIL R96 script recovery: %s\n",name);std::abort();}}
static void nativeReadyR96(void* state){
 // The engine has restored/replaced its own stack at the next Update boundary.
 // Never change transport flags, fault state or client capabilities here.
 put(state,0x40,reinterpret_cast<std::uintptr_t>(stack.data()));
 put(state,0x18,reinterpret_cast<std::uintptr_t>(stack.data()));
 put(state,0x10,reinterpret_cast<std::uintptr_t>(stack.data()+32));
 put(state,0x38,reinterpret_cast<std::uintptr_t>(stack.data()+240));
}
int main(){
 retained_native_script_tests_r96();
 using namespace kcd2vr::native_script_t2;namespace life=kcd2vr::inventory_lifecycle_t8;
 reset();Register(&OwnClient);raiseFault=true;expectedPacket=false;Detour(scriptSystem.data());
 needR96(enabled&&pcalls==1&&restores==0&&transportFaultR96.kind==FaultKindR96::Native,"fault is scoped, original retained, no uncertain SetTop");
 raiseFault=false;expectedPacket=true;requestR96="DRAW|2";nativeReadyR96(vm.data());
 Detour(scriptSystem.data());needR96(pcalls==1&&clientCalls==1&&originalCalls==2,"same failed VM has no automatic retry after a clean-looking sample");
 life::Publish(false,false);Detour(scriptSystem.data());
 needR96(pcalls==1&&life::loadEpisode18==1,"unknown UI does not re-enable failed native calls");
 life::Publish(true,false);life::Recheck();life::RequestRecovery();Detour(scriptSystem.data());
 needR96(pcalls==1,"menu/recheck/manual inventory request does not invent a load episode");
 life::Publish(true,true);const auto visit=life::loadEpisode18.load();Detour(scriptSystem.data());
 needR96(pcalls==1&&visit==2,"loading entry cannot run a suspended client early");
 life::Publish(false,false);Detour(scriptSystem.data());needR96(pcalls==1,"unknown loading observation does not complete it");
 life::Publish(true,false);Detour(scriptSystem.data());
 needR96(pcalls==2&&restores==1&&clientCalls==2&&lastInput=="DRAW|2"&&transportFaultR96.kind==FaultKindR96::None,
         "completed positive load admits the fresh real packet and native stack restore");
 needR96(generation==1,"load admission does not pretend VM identity changed or erase client debts");
 requestR96="DRAW|3";Detour(scriptSystem.data());needR96(pcalls==3&&restores==2&&lastInput=="DRAW|3","subsequent native actions remain live");
 // A second fault/load cycle must recover too, with no restart and no direct
 // reset of the production fault or enabled state.
 raiseFault=true;expectedPacket=false;requestR96="DRAW|4";Detour(scriptSystem.data());
 needR96(pcalls==4&&transportFaultR96.kind==FaultKindR96::Native,"second fault is independently scoped");
 raiseFault=false;expectedPacket=true;nativeReadyR96(vm.data());life::Publish(true,true);Detour(scriptSystem.data());
 needR96(pcalls==4,"second loading interval stays suspended");
 life::Publish(true,false);requestR96="DRAW|5";Detour(scriptSystem.data());
 needR96(pcalls==5&&restores==3&&lastInput=="DRAW|5","second completed save transition restores actual action liveness");
 // A fault that occurs during a known loading episode may recover after that
 // same episode ends. Requiring another load would poison the loaded game.
 reset();Register(&OwnClient);life::Publish(true,true);raiseFault=true;expectedPacket=false;Detour(scriptSystem.data());
 raiseFault=false;expectedPacket=true;nativeReadyR96(vm.data());Detour(scriptSystem.data());needR96(pcalls==1,"active load is never recovery evidence");
 life::Publish(true,false);requestR96="DRAW|2";Detour(scriptSystem.data());needR96(pcalls==2&&restores==1,"fault inside the load recovers at its qualified end");
 // Actual VM replacement requires full native cursor qualification. An
 // unreadable replacement does not become authority just because its address changed.
 reset();Register(&OwnClient);replace=true;Detour(scriptSystem.data());
 needR96(enabled&&pcalls==1&&restores==0,"replacement refuses old-frame restoration without process disable");
 replace=false;requestR96="DRAW|2";Detour(scriptSystem.data());needR96(pcalls==1,"unqualified replacement cannot invoke clients");
 nativeReadyR96(otherVm.data());Detour(scriptSystem.data());
 needR96(pcalls==2&&restores==1&&generation==2&&lastInput=="DRAW|2","qualified new VM resumes actual packets once");
 // A nonexceptional uncertain frame can recover when native Update restores
 // its own original frame/height; no old top pointer is written in between.
 reset();Register(&OwnClient);changeFrame=true;expectedPacket=false;Detour(scriptSystem.data());
 needR96(pcalls==1&&restores==0&&transportFaultR96.kind==FaultKindR96::Observation,"uncertain frame records observation suspension");
 changeFrame=false;expectedPacket=true;Detour(scriptSystem.data());needR96(pcalls==1&&restores==0,"still-wrong native frame cannot be retried");
 nativeReadyR96(vm.data());requestR96="DRAW|2";Detour(scriptSystem.data());
 needR96(pcalls==2&&restores==1&&generation==1,"naturally restored same VM resumes without fake replacement or cancellation");
 Shutdown();requestR96="DRAW|3";Detour(scriptSystem.data());needR96(pcalls==2&&!enabled,"administrative shutdown remains explicit and final");
 std::printf("R96 actual script recovery: %u checks PASS; native C API/memory are host leaves; no game/Windows SEH executed\n",checksR96);
}
