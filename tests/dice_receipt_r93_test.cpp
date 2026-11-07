// Actual diagnostic producer; native decisions are supplied, no game-memory fixture.
#include "../src/subsystems/dice/hud_depth_r92r3.h"
#include "../src/diagnostics/capture168.h"
#include "../src/diagnostics/interaction_receipt59c10.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
namespace kcd2vr {
namespace diagnostic_trace {
static std::vector<std::string> lines;
static void Write(const char* text){lines.emplace_back(text);}
}
#ifndef KCD2VR_DICE_RECEIPT_R93_EXTRACT
#error Extract the actual DiceMarkerReceiptR93 function.
#endif
#include KCD2VR_DICE_RECEIPT_R93_EXTRACT
}
using namespace kcd2vr;
namespace dm=kcd2vr::dice_marker_r92;
namespace dh=kcd2vr::dice_hands_d85;
namespace cap=kcd2vr::diagnostics::capture168;
static unsigned checks=0;
static void check(bool yes,const char* what){++checks;if(!yes){std::fprintf(stderr,"dice receipt: %s\n",what);std::abort();}}
static bool has(const char* text){return !diagnostic_trace::lines.empty()&&diagnostic_trace::lines.back().find(text)!=std::string::npos;}
int main(){
    dh::Witness w{11,22,3,4,true,true,true,true};
    dm::Selection s{{1,2,3},11,4,3,1000,2,true};
    dh::enabled=false;
    DiceMarkerReceiptR93(w,true,true,s,1000);
    check(diagnostic_trace::lines.empty(),"F5 off produces no diagnostic row");
    cap::gate.select(cap::Domain::Interaction,true,1000);
    DiceMarkerReceiptR93(w,true,true,s,1000);
    check(has("XR_POSE_OR_ATLAS_NOT_PREPARED")&&has("nativeSelected=1")&&has("handPointing=0"),"gamepad selection is diagnosed while hand pointing is off");
    DiceMarkerReceiptR93(w,true,true,s,1001);
    check(diagnostic_trace::lines.size()==1,"identical per-frame receipt is coalesced");
    const auto publish=[&](std::uint64_t now,bool prep,bool hide,bool end){s.tick=now;const dm::Presented p{s.instance,s.visit,s.lifetime,now,s.index,true};dm::prepared.publish(prep?p:dm::Presented{});dm::hidden.publish(hide?p:dm::Presented{});dm::presented.publish(end?p:dm::Presented{});dm::flatOwnerReady=hide;};
    publish(1100,true,false,false);DiceMarkerReceiptR93(w,true,true,s,1100);
    check(has("NATIVE_CURSOR_HIDE_NOT_JOINED")&&has("prepared=1"),"prepared ring is not a native hide receipt");
    publish(1200,true,true,false);DiceMarkerReceiptR93(w,true,true,s,1200);
    check(has("XR_END_NOT_YET_JOINED"),"hidden native ring is not a submitted ring");
    dice_hud_depth_r92r3::applied.publish({s.instance,s.visit,s.lifetime,1300,.7f,2.f,1.f,true});
    publish(1300,true,true,true);DiceMarkerReceiptR93(w,true,true,s,1300);
    check(has("stage=XR_END_JOINED")&&has("pixelScanout=NOT_OBSERVED")&&has("nativeReads=NONE"),"XR receipt never claims pixels or scanout");
    check(has("hudDepthJoined=1")&&has("hudDepth=0.70000"),"broad HUD depth has its own matching XR end receipt");
    publish(1400,true,true,true);dm::Presented old{s.instance,s.visit+1,s.lifetime,1400,s.index,true};dm::presented.publish(old);DiceMarkerReceiptR93(w,true,true,s,1400);
    check(has("XR_END_NOT_YET_JOINED"),"another dice visit cannot count as current presentation");
    check(has("hudDepthJoined=0"),"expired depth receipt is not reported as currently applied");
    publish(1500,true,true,true);s.index=1;DiceMarkerReceiptR93(w,true,true,s,1500);
    check(has("XR_POSE_OR_ATLAS_NOT_PREPARED"),"old selected index cannot count as current preparation");
    publish(1600,false,false,false);DiceMarkerReceiptR93(w,true,false,s,1600);
    check(has("NATIVE_CHOOSING_TURN_UNAVAILABLE"),"unavailable native turn is distinct from unavailable XR layer");
    s={};DiceMarkerReceiptR93(w,true,true,s,1700);
    check(has("NATIVE_SELECTED_DIE_UNAVAILABLE"),"invalid selected die is separately identified");
    DiceMarkerReceiptR93(w,false,false,s,1800);
    check(has("SELECTOR_PINS_UNQUALIFIED"),"retail leaf pin qualification is a distinct reason");
    w.retail=false;DiceMarkerReceiptR93(w,false,false,s,1900);
    check(has("RETAIL_UNQUALIFIED"),"retail fingerprint gate is distinguished from selector leaves");
    cap::gate.select(cap::Domain::Interaction,false,2000);const auto before=diagnostic_trace::lines.size();
    DiceMarkerReceiptR93(w,false,false,s,2100);check(diagnostic_trace::lines.size()==before,"ending F5 disables producer immediately");
    cap::gate.select(cap::Domain::Interaction,true,2200);w.instance=0;DiceMarkerReceiptR93(w,false,false,s,2200);
    check(diagnostic_trace::lines.size()==before,"no active dice instance produces no background diagnostic row");
    w.instance=11;DiceMarkerReceiptR93(w,false,false,s,2200);check(diagnostic_trace::lines.size()==before+1,"new capture starts a fresh bounded receipt session");
    const auto next=diagnostic_trace::lines.size();DiceMarkerReceiptR93(w,false,false,s,2200+cap::Duration);
    check(diagnostic_trace::lines.size()==next,"lease expiry cannot be renewed by diagnostic producer");
    std::printf("R93 actual dice marker receipt: %u checks PASS (no native reads or gameplay writes)\n",checks);
}
