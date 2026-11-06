#define KCD2VR_TEST_HOOK_SPY115 1
#define KCD2VR_H36R3_HOOK_TEST 1
#include "../src/core/initial_draw_completion_h36r3.h"
#define H36_MAIN_NAME ExistingH36Main
#include "holster_completion_h36_bridge.cpp"
#undef H36_MAIN_NAME
static unsigned initialCallsH36r3=0;static bool throwInitialH36r3=false,skipInitialH36r3=false,failedSpawnH36r3=false;
static bool pendingInitialH36r3=false,omitSubmitH36r3=false,foreignSubmitH36r3=false,syncJobH36r3=false,midUnchangedH36r3=false;
static void* incomingSelfH36r3=nullptr;static void* worldManagerH36r3=nullptr;
static std::array<unsigned char,0xE8> envH36r3{};static std::array<unsigned char,0xD0> worldH36r3{};
static std::array<unsigned char,0x30> headH36r3{},nodeH36r3{};static std::array<unsigned char,0x80> functorH36r3{};
static std::uint64_t nextJobH36r3=0;
static void createInitialH36r3(CompletionRigH36&r){putH36(leftItemH36,0x88,static_cast<void*>(linkH36.data()));r.gear.items[1].worldLink54=linkH36.data();r.gear.items[1].entity=entityH36.data();r.gear.items[1].profile.entity=80;}
static void __fastcall ActualHostInitialH36r3(void*self){
 ++initialCallsH36r3;incomingSelfH36r3=self;if(throwInitialH36r3)throw std::runtime_error("original outer callback throw");
 if(!self)return;void*wm=nullptr;std::uint64_t token=0;std::memcpy(&wm,self,8);std::memcpy(&token,static_cast<unsigned char*>(self)+8,8);
 if(wm!=worldManagerH36r3||token!=nextJobH36r3)return;void*root=nullptr;std::memcpy(&root,headH36r3.data()+8,8);if(root!=nodeH36r3.data())return;
 // Exact native order: remove original token, then create link, then invoke
 // original inner native draw callback. Those native leaves are explicit here.
 putH36(headH36r3,8,static_cast<void*>(headH36r3.data()));
 if(failedSpawnH36r3)return;auto&r=*static_cast<CompletionRigH36*>(liveH36);createInitialH36r3(r);if(!skipInitialH36r3)bindHandH36();
 PhysLeftEquipmentRefreshH36(r.gear,hostTickCount64);insideAckH36=!g_native_held_equipment_pending_h36;
}
static bool __fastcall HostPendingInitialH36r3(void*){return pendingInitialH36r3;}
static void DispatchInitialH36r3(void*self){
#if defined(H36R3_BASELINE_HOST)
 ActualHostInitialH36r3(self); // actual baseline has no native completion hook
#else
 InitialDrawCallbackDetourH36r3(self);
#endif
}
static void HostSubmitInitialH36r3(void*,void*item){
 ++nextJobH36r3;putH36(nodeH36r3,0,static_cast<void*>(headH36r3.data()));putH36(nodeH36r3,0x10,static_cast<void*>(headH36r3.data()));putH36(nodeH36r3,0x19,static_cast<unsigned char>(0));
 putH36(nodeH36r3,0x20,nextJobH36r3);putH36(nodeH36r3,0x28,item);putH36(headH36r3,8,static_cast<void*>(nodeH36r3.data()));
 putH36(functorH36r3,8,worldManagerH36r3);putH36(functorH36r3,0x10,nextJobH36r3);
#if !defined(H36R3_BASELINE_HOST)
 if(!omitSubmitH36r3){DetourModKit::hook::MidContext c{};using namespace DetourModKit::hook;
  gpr(c,Gpr::Rcx)=reinterpret_cast<std::uintptr_t>(worldManagerH36r3);gpr(c,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(item);gpr(c,Gpr::Rbx)=nextJobH36r3;
  const auto before=c;auto saved=kcd2vr::initial_draw_h36r3::invoking;if(foreignSubmitH36r3)kcd2vr::initial_draw_h36r3::invoking={};
  InitialDrawSubmittedMidH36r3(c);kcd2vr::initial_draw_h36r3::invoking=saved;midUnchangedH36r3=std::memcmp(&before,&c,sizeof(c))==0;
 }
#endif
 if(syncJobH36r3)DispatchInitialH36r3(functorH36r3.data()+8);
}
static void setupInitialH36r3(CompletionRigH36&r){
 namespace initial=kcd2vr::initial_draw_h36r3;for(auto&s:initial::slots)s.active=0;initial::invoking={};initial::callbackReady=false;
 initialCallsH36r3=0;throwInitialH36r3=skipInitialH36r3=failedSpawnH36r3=pendingInitialH36r3=omitSubmitH36r3=foreignSubmitH36r3=syncJobH36r3=midUnchangedH36r3=false;incomingSelfH36r3=nullptr;nextJobH36r3=0;
 clearHandH36();putH36(storedH36,0x20,static_cast<void*>(nullptr));putH36(leftItemH36,0x88,static_cast<void*>(nullptr));r.gear.items[1].worldLink54=nullptr;r.gear.items[1].entity=nullptr;r.gear.items[1].profile.entity=0;
 auto b=reinterpret_cast<std::uintptr_t>(fakeImageH36);
 unsigned char nativePending[]{0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x33,0xDB,0x48,0x8B,0xF9,0x48,0x83,0xC4,0x20,0x5F,0x48,0x8B,0x5C,0x24,0x10,0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0};
 auto pendingFn=reinterpret_cast<std::uintptr_t>(&HostPendingInitialH36r3);std::memcpy(nativePending+27,&pendingFn,8);std::memcpy(reinterpret_cast<void*>(b+0xABEE24),nativePending,sizeof(nativePending));
 if(!worldManagerH36r3){worldManagerH36r3=VirtualAlloc(nullptr,0x5B9060,0x3000,0x40);if(!worldManagerH36r3)std::abort();}
 envH36r3={};worldH36r3={};headH36r3={};nodeH36r3={};functorH36r3={};
 auto env=static_cast<void*>(envH36r3.data());std::memcpy(reinterpret_cast<void*>(b+0x549D388),&env,8);putH36(envH36r3,0xE0,static_cast<void*>(worldH36r3.data()));putH36(worldH36r3,0xC8,worldManagerH36r3);
 auto head=static_cast<void*>(headH36r3.data());std::memcpy(static_cast<unsigned char*>(worldManagerH36r3)+0x5B9030,&head,8);putH36(headH36r3,8,head);putH36(headH36r3,0x19,static_cast<unsigned char>(1));
 putH36(entityH36,0,b+kCEntityVftRva);putH36(entityH36,0x0C,std::uint32_t(80));putH36(functorH36r3,0,b+0x3EBC600);
 hostInitialSubmitServiceH36=&HostSubmitInitialH36r3;
#if !defined(H36R3_BASELINE_HOST)
 namespace hooks=DetourModKit::hook;hooks::hostOriginal115=+[](const hooks::InlineRequest&r)->std::uintptr_t{return r.name=="kcd2vr_h36r3_original_left_draw_completion"?reinterpret_cast<std::uintptr_t>(&ActualHostInitialH36r3):0;};
 if(!g_initial_draw_hook_h36r3){unsigned char entry[]{0x40,0x57,0x48,0x83,0xEC,0x20,0x4C,0x8B,0x19,0x48,0x8B,0xF9,0x4D,0x8B,0x93,0x30,0x90,0x5B,0x00};unsigned char thunk[]{0x48,0x83,0xC1,0x08,0xE9,0x37,0xC7,0xFF,0xFF};
  std::memcpy(reinterpret_cast<void*>(b+0x1F0D230),entry,sizeof(entry));std::memcpy(reinterpret_cast<void*>(b+0x1F10AF0),thunk,sizeof(thunk));auto target=b+0x1F10AF0;std::memcpy(reinterpret_cast<void*>(b+0x3EBC600+0x10),&target,8);}
 if(!g_initial_draw_mid_hook_h36r3){unsigned char before[]{0x48,0xFF,0x81,0x40,0x90,0x5B,0x00,0x49,0x8B,0xF8,0x48,0x8B,0x99,0x40,0x90,0x5B,0x00};unsigned char at[]{0x4C,0x8D,0x45,0x9F,0x4C,0x8B,0xF1,0x48,0x89,0x55,0xA7,0x48,0x81,0xC1,0x30,0x90,0x5B,0x00,0x48,0x89,0x5D,0x9F};std::memcpy(reinterpret_cast<void*>(b+0x8D08F0),before,sizeof(before));std::memcpy(reinterpret_cast<void*>(b+0x8D0901),at,sizeof(at));}
 if(!InstallInitialDrawCallbackH36r3(b))std::abort();
#else
 initial::callbackReady=true;
#endif
}
int main(){tests=failed=0;
 {CompletionRigH36 r;setupInitialH36r3(r);r.draw();ck(g_native_held_equipment_pending_h36,"initial draw keeps pending after native wrapper return");
#if !defined(H36R3_BASELINE_HOST)
 ck(midUnchangedH36r3,"actual submission MID reads without altering registers flags IP or stack");
#endif
 r.advance();createInitialH36r3(r);bindHandH36();r.refresh();ck(g_native_held_equipment_pending_h36,"born link plus plausible current hand alone never ACKs original");putH36(leftItemH36,0x88,static_cast<void*>(nullptr));r.gear.items[1].worldLink54=nullptr;r.gear.items[1].entity=nullptr;r.gear.items[1].profile.entity=0;clearHandH36();
 DispatchInitialH36r3(functorH36r3.data()+8);ck(initialCallsH36r3==1&&incomingSelfH36r3==functorH36r3.data()+8,"actual outer callback forwards its one original argument exactly once");ck(!insideAckH36,"completion callback cannot ACK during its original invocation");r.refresh();ck(!g_native_held_equipment_pending_h36,"original submission token and outer return plus real hand binding close first draw");
 r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"first draw then original STOW whole cycle completes");}
 for(unsigned bad=0;bad<14;++bad){CompletionRigH36 r;setupInitialH36r3(r);if(bad==0)omitSubmitH36r3=true;if(bad==1)foreignSubmitH36r3=true;if(bad==2)pendingInitialH36r3=true;r.draw();r.advance();
 if(bad==3)putH36(functorH36r3,0x10,std::uint64_t(99));if(bad==4)putH36(functorH36r3,8,static_cast<void*>(nullptr));if(bad==5)++g_player_char37;if(bad==6)++g_weapon_request_epoch6694;if(bad==7)++g_pose_epoch6689;if(bad==8)++g_left_native_membership_h36;if(bad==9)putH36(nodeH36r3,0x28,static_cast<void*>(leftItemH36.data()+8));if(bad==10)putH36(functorH36r3,0,std::uintptr_t(0));if(bad==11)throwInitialH36r3=true;if(bad==12)failedSpawnH36r3=true;if(bad==13)putH36(worldH36r3,0xC8,static_cast<void*>(nullptr));
 try{DispatchInitialH36r3(functorH36r3.data()+8);}catch(const std::runtime_error&){}
 if(bad==3||bad==4){createInitialH36r3(r);bindHandH36();} // plausible replacement cannot substitute for wrong actual token
 r.refresh();ck(initialCallsH36r3==1,"all mismatches preserve exactly one original native invocation");ck(g_native_held_equipment_pending_h36,"foreign token/source/tuple/exception/replacement cannot complete original draw");}
 {CompletionRigH36 r;setupInitialH36r3(r);r.draw();r.advance();skipInitialH36r3=true;DispatchInitialH36r3(functorH36r3.data()+8);r.refresh();ck(g_native_held_equipment_pending_h36,"returned original world job is not an invented physical attachment");bindHandH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"later same-original real attachment completes retained returned job");}
 {CompletionRigH36 r;setupInitialH36r3(r);syncJobH36r3=true;r.draw();ck(!insideAckH36&&g_native_held_equipment_pending_h36,"inline world completion cannot ACK before original DRAW wrapper normal return");r.refresh();ck(!g_native_held_equipment_pending_h36,"inline original world job and later wrapper normal return compose correctly");}
#if !defined(H36R3_BASELINE_HOST)
 {CompletionRigH36 r;setupInitialH36r3(r);namespace h=DetourModKit::hook;StopInitialDrawCallbackH36r3();ck(!kcd2vr::initial_draw_h36r3::callbackReady&&!g_initial_draw_hook_h36r3->is_enabled()&&!g_initial_draw_mid_hook_h36r3->is_enabled(),"both actual hook capabilities must disarm without freeing trampolines");
 h::hostFailEnableH36r3=true;h::hostFailDisableH36r3=true;ck(!InstallInitialDrawCallbackH36r3(reinterpret_cast<std::uintptr_t>(fakeImageH36))&&!kcd2vr::initial_draw_h36r3::callbackReady&&g_initial_draw_original_h36r3,"partial enable/disable failure is not actual completion readiness");auto n=initialCallsH36r3;InitialDrawCallbackDetourH36r3(nullptr);ck(initialCallsH36r3==n+1&&incomingSelfH36r3==nullptr,"retained partial hook still forwards its original incoming pointer");h::hostFailEnableH36r3=h::hostFailDisableH36r3=false;StopInitialDrawCallbackH36r3();
 h::hostFailEnableMidH36r3=true;h::hostFailDisableMidH36r3=true;
 ck(!InstallInitialDrawCallbackH36r3(reinterpret_cast<std::uintptr_t>(fakeImageH36))&&!kcd2vr::initial_draw_h36r3::callbackReady&&!g_initial_draw_hook_h36r3->is_enabled()&&g_initial_draw_mid_hook_h36r3->is_enabled(),"second MID enable/disable failure cleans outer hook and retains disabled capability");
 DetourModKit::hook::MidContext inactive{};const auto unchanged=inactive;InitialDrawSubmittedMidH36r3(inactive);ck(std::memcmp(&inactive,&unchanged,sizeof(inactive))==0,"retained partial MID never alters native registers");
 h::hostFailEnableMidH36r3=h::hostFailDisableMidH36r3=false;StopInitialDrawCallbackH36r3();ck(InstallInitialDrawCallbackH36r3(reinterpret_cast<std::uintptr_t>(fakeImageH36)),"same pinned retained hooks reactivate only at explicit install");}
#endif
 std::printf("H36R3 token initial DRAW %u/%u PASS\n",tests-failed,tests);return failed?1:0;
}
