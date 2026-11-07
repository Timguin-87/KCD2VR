// Complete production projection/publication functions; supplies current ray/candidate metadata
// and an original-native projection spy. No game or engine function is invoked.
#include "generated_interactor5916c3.inc"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::interaction_alignment;
static unsigned nativeCalls=0;
static char nativeAnswer=1;
static char NativeProjection(std::uintptr_t,std::uintptr_t,float*,std::uintptr_t) noexcept {++nativeCalls;return nativeAnswer;}
int main(){
 hostTickCount64=10000;g_wh_base=0x10000000;g_split_ready57c1=true;
 hostReturnAddressValue=reinterpret_cast<void*>(g_wh_base+kScreenReturn);
 g_onscreen_original=&NativeProjection;
 alignas(8) std::uintptr_t objectPolicy[2]={g_wh_base+kObjectPolicy,0x55};
 float point[3]={0,1,0},screen[2]={};
 NativeRay112 native{};native.tick=10000;native.reach=2;native.ray.y=1;native.ray.tick=10000;
 g_native_packets112.publish(native);
 auto query=[&](){g_candidate57c1={0x55,10000,kcd2vr::interaction57c1::Kind::Object};return OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy));};
 SetVisualFallbackEnabled(false); // mod.cpp head_interaction_aim is false in a native minigame.
 SetEnabled(false);SetHandRayWorld(false,0,0,0,0,1,0);
 nativeCalls=0;const int old=query();std::printf("native_comparison_disabled result=%d originalCalls=%u\n",old,nativeCalls);assert(old==1&&nativeCalls==1);
 SetNativeActivityFallback5916c3(true);SetEnabled(true);SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);
 nativeCalls=0;const int raised=query();std::printf("C3_pointing_hand result=%d originalCalls=%u\n",raised,nativeCalls);assert(raised==1);
 SetHandRayWorld(false,0,0,0,0,1,0);
 nativeCalls=0;const int lowered=query();std::printf("C3_lowered_hand result=%d originalCalls=%u\n",lowered,nativeCalls);assert(lowered==1&&nativeCalls==1);
 SetEnabled(false);nativeCalls=0;const int disabled=query();std::printf("C3_explicit_backend_off result=%d originalCalls=%u\n",disabled,nativeCalls);assert(disabled==1&&nativeCalls==1);
 // Stale or lost ray also falls back. Ordinary VRIK objects do not regain gaze by this patch.
 SetEnabled(true);SetNativeActivityFallback5916c3(true);SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);
 hostTickCount64=10200;native.tick=10200;g_native_packets112.publish(native);nativeCalls=0;
 assert(query()==1&&nativeCalls==1);
 SetNativeActivityFallback5916c3(false);SetHandRayWorld(false,0,0,0,0,1,0);nativeCalls=0;
 g_candidate57c1={0x55,10200,kcd2vr::interaction57c1::Kind::Object};
 assert(OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy))==1&&nativeCalls==1);
 nativeCalls=0;nativeAnswer=0;g_candidate57c1={0x55,10200,kcd2vr::interaction57c1::Kind::Object};
 assert(OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy))==0&&nativeCalls==1);
 nativeAnswer=1;
 // Two real production projection paths: nearer LEFT palm beats RIGHT,
 // candidate enumeration order cannot overwrite the nearer choice, grip locks
 // the object ID, while actor policy retains the native head direction.
 hostTickCount64=11000;native.tick=11000;native.ray.tick=11000;native.reach=10;g_native_packets112.publish(native);
 SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);
 kcd2vr::palm59c9::Pair palms{};
 for(int side=0;side<2;++side){auto& r=palms.ray[side];r.hand=true;r.owner=side;r.tick=11000;r.y=1;r.limit=2;r.radius=.2;palms.eligible[side]=true;}
 palms.ray[1].oy=.9f;SetPalmRays59c9(palms);
 auto object=[&](unsigned id,float y,const char* cls=nullptr){point[1]=y;g_candidate57c1={0x55,hostTickCount64,kcd2vr::interaction57c1::Kind::Object,id};if(cls)std::strcpy(g_candidate57c1.cls,cls);return OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy));};
 ++g_interaction_calls;assert(object(17,1.f)==1);const float close=screen[0];assert(g_hand_candidate178r14.owner==1&&g_hand_candidate178r14.entity==17);
 assert(object(18,1.4f)==1&&screen[0]>close);assert(g_hand_candidate178r14.entity==17);
 palms.locked=1;palms.lockedEntity=17;SetPalmRays59c9(palms);
 assert(object(18,1.0f)==0);assert(object(17,1.0f)==1);
 // Hand held directly above a small object is still inside the near volume.
 palms.ray[1].y=0;palms.ray[1].x=1;SetPalmRays59c9(palms);assert(object(17,1.f)==1);
 hostTickCount64=11081;native.tick=11081;g_native_packets112.publish(native);
 assert(object(17,1.f)==0); // NO 300 ms frozen ray
 // Diagnostic observations must leave projection results untouched. The
 // off recorder did no formatting/output; a new capture receives the actual
 // grip-lock rejection, not a fabricated successful Use.
 assert(kcd2vr::diagnostic_trace::receipts.empty());
 hostTickCount64=11000;native.tick=11000;g_native_packets112.publish(native);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,11000);
 assert(object(18,1.f)==0);
 assert(kcd2vr::diagnostic_trace::receipts.size()==1&&kcd2vr::diagnostic_trace::receipts.back().find("GRIP_TARGET_LOCK")!=std::string::npos);
 assert(object(17,1.f)==1); // logging never alters the native candidate answer
 // C38 exercises the real production observation, lookup and invalidation
 // functions: a nearer other-hand pickup cannot replace exact door evidence.
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,12000);
 hostTickCount64=12000;native.tick=native.ray.tick=12000;g_native_packets112.publish(native);
 SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);
 palms=kcd2vr::palm59c9::Pair{};for(int side=0;side<2;++side){auto& ray=palms.ray[side];ray.hand=true;ray.owner=side;ray.tick=12000;ray.y=1;ray.limit=2;ray.radius=.2;ray.sourceEpoch=7;ray.sourcePlayer=0x1234;palms.eligible[side]=true;}
 palms.ray[1].ox=1;SetPalmRays59c9(palms);++g_interaction_calls;
 point[0]=0;assert(object(42,.2f,"AnimDoor")==1);HandCandidate exact38{};
 assert(ExactDoorCandidate38(42,0,12000,exact38)&&exact38.owner==0&&exact38.entity==42);
 const auto atEntry38=exact38;
 point[0]=1;assert(object(263342,.05f,"PickableItem")==1);
 assert(g_hand_candidate178r14.entity==263342&&g_hand_candidate178r14.owner==1);
 assert(ExactDoorCandidate38(42,0,12000,exact38)&&exact38.entity==42&&!ExactDoorCandidate38(42,1,12000,exact38));
 point[0]=3;assert(object(42,.2f,"AnimDoor")==0);
 assert(!ExactDoorCandidate38(42,0,12000,exact38)&&atEntry38.entity==42&&atEntry38.owner==0);
 point[0]=0;assert(object(42,.2f,"AnimDoor")==1&&ExactDoorCandidate38(42,0,12000,exact38));
 SetHandRayWorld(false,0,0,0,0,1,0);assert(!ExactDoorCandidate38(42,0,12000,exact38));
 SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);assert(!ExactDoorCandidate38(42,0,12000,exact38));
 assert(object(42,.2f,"AnimDoor")==1);SetEnabled(false);SetEnabled(true);
 SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);assert(!ExactDoorCandidate38(42,0,12000,exact38));
 assert(object(42,.2f,"AnimDoor")==1);SetVisualFallbackEnabled(true);SetVisualFallbackEnabled(false);
 assert(!ExactDoorCandidate38(42,0,12000,exact38));
 assert(object(42,.2f,"AnimDoor")==1);hostTickCount64=12081;
 assert(!ExactDoorCandidate38(42,0,12081,exact38));
 // Deterministic in-flight callback: capture its token, revoke/re-enable the
 // source before the real publish helper runs, and reject the old token/copy.
 hostTickCount64=12000;Candidate57c1 doorNative38{0x55,12000,kcd2vr::interaction57c1::Kind::Object,42};
 std::strcpy(doorNative38.cls,"AnimDoor");const float doorPoint38[4]{0,.2f,0,.2f};
 for(int revoke38=0;revoke38<3;++revoke38){
     SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);assert(object(42,.2f,"AnimDoor")==1);
     assert(ExactDoorCandidate38(42,0,12000,exact38));const auto before38=exact38.sourceGeneration;
     if(revoke38==0){SetEnabled(false);SetEnabled(true);}
     if(revoke38==1){SetVisualFallbackEnabled(true);SetVisualFallbackEnabled(false);}
     if(revoke38==2)SetHandRayWorld(false,0,0,0,0,1,0);
     SetHandRayWorld(true,0,0,0,0,1,0,2,.2,0);
     DoorCandidateObservation38(doorNative38,doorPoint38,palms.ray[0],12000,true,before38);
     assert(!ExactDoorCandidate38(42,0,12000,exact38)&&!DoorSourceCurrent38(before38));
 }
 // Missing source identity never upgrades a fallback ray into a door witness.
 auto unknown38=palms.ray[0];unknown38.sourceEpoch=0;
 DoorCandidateObservation38(doorNative38,doorPoint38,unknown38,12000,true,DoorSourceGeneration38());
 assert(!ExactDoorCandidate38(42,0,12000,exact38));
 // B3: real native forge trigger survives a lowered/absent hand, beyond the
 // pickup cap. Return the original answer unchanged, once, including refusal.
 hostTickCount64=13000;native.tick=native.ray.tick=13000;native.reach=10;
 g_native_packets112.publish(native);SetNativeActivityFallback5916c3(false);
 SetVisualFallbackEnabled(false);SetEnabled(true);SetHandRayWorld(false,0,0,0,0,1,0);
 point[0]=0;point[1]=3;
 auto forge=[&](){g_candidate57c1={0x55,13000,kcd2vr::interaction57c1::Classify("ForgeBuilderTrigger"),73};
   std::strcpy(g_candidate57c1.cls,"ForgeBuilderTrigger");
   return OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy));};
 nativeCalls=0;nativeAnswer=1;assert(forge()==1&&nativeCalls==1);
 nativeCalls=0;nativeAnswer=0;assert(forge()==0&&nativeCalls==1);
 nativeAnswer=1;nativeCalls=0;
 g_candidate57c1={0x55,13000,kcd2vr::interaction57c1::Kind::Object,74};
 assert(OnScreenDetour(0,reinterpret_cast<std::uintptr_t>(point),screen,reinterpret_cast<std::uintptr_t>(objectPolicy))==1&&nativeCalls==1);
 std::puts("PASS actual projection: native fallback and palm selection preserved; exact C38 door witness survives other-hand overwrite and revokes on loss/fallback/staleness");
 return 0;
}
