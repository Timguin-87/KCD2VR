#include "../src/diagnostics/xr_timing165.h"
#include <cassert>
#include <string>
#include <vector>
static std::vector<std::string> lines;
namespace kcd2vr::diagnostic_trace {void Write(const char* p) noexcept {lines.emplace_back(p);}}
int main(){
 namespace c=kcd2vr::diagnostics::capture168;namespace x=kcd2vr::diagnostics::xr165;
 assert(c::essential("C23 MS7D8_EDGE edge=OPEN serial=1"));
 assert(c::essential("H3 WINDOW name=wh_cs_ComboSlotDuration event=APPLIED"));
 assert(c::essential("F1 COMBO_NATIVE_SLOT result=EXTENDED"));
 assert(!c::essential("V66.91-F3-COMBO_CANDIDATE row=1"));
 hostTickCount64=10000;c::gate.mask=0;
 assert(c::accept("M1 UI_LASER_INPUT shop=1 gate=FOCUS",hostTickCount64));
 assert(c::accept("M1 UI_LASER_XR owner=INVENTORY panel=0",hostTickCount64));
 assert(c::accept("M1 INPUT B_HOLD_ESCAPE fired=1",hostTickCount64));
 {x::Timer t(x::Frame);assert(!t.enabled);}assert(x::metrics[x::Frame].calls==0);
 c::gate.select(c::Domain::Combat,true,hostTickCount64);
 {x::Timer t(x::Frame);assert(!t.enabled);}assert(x::metrics[x::Frame].calls==0);
 c::gate.select(c::Domain::Hud,true,hostTickCount64);
 {x::Timer t(x::Frame);assert(t.enabled);}assert(x::metrics[x::Frame].calls==1);
 hostTickCount64+=180000;{x::Timer t(x::Frame);assert(!t.enabled);}assert(x::metrics[x::Frame].calls==1);
 x::Report(hostTickCount64-6000);lines.clear();x::Report(hostTickCount64);assert(lines.size()==1&&lines[0].find("xrFramesPerSec=")!=std::string::npos); // V66.178R29: the frame-rate line is essential (like HOOK_COST); per-boundary timing stays recorder-only
 lines.clear();
 assert(c::essential("V66.165 F8_LIFECYCLE_END reason=USER_OFF"));
 assert(c::essential("T4 F9 rung=2 bodyState=7 reason=NATIVE_UNAVAILABLE"));
 assert(c::essential("T5 MAP_READINESS stage=HELPER_LOST actor=42 epoch=7"));
 assert(c::essential("T5 SUPPLY_STATUS status=MISSING_ASSETS actor=42 body=7"));
 assert(c::accept("T5 SUPPLY_STATUS status=EMPTY_STOCK actor=42 body=7",hostTickCount64));
 assert(!c::essential("C7FM1 HELD_MAP requested=1 placed=123")); // periodic counters still require capture
 assert(!c::essential("T2 MAP_FEED valid=1 player=1")); // no new periodic quiet-mode output
 assert(c::accept("T4 F9 retired=ACK legacyProxyPreference=RESTORED",100));
 assert(!c::accept("V166 HUD_COST",hostTickCount64));
 c::gate.mask=0;
 assert(!c::accept("C27 HUD_DIALOGUE_PRESENTATION",hostTickCount64));
 c::gate.select(c::Domain::Combat,true,hostTickCount64);
 assert(!c::accept("C27 HUD_DIALOGUE_PRESENTATION",hostTickCount64));
 c::gate.select(c::Domain::Interaction,true,hostTickCount64);
 assert(c::accept("C27 HUD_DIALOGUE_PRESENTATION",hostTickCount64));
 assert(c::accept("C19C2 HUD_DIALOGUE_STATE",hostTickCount64));
  assert(c::accept("C28 HUD_DIALOGUE_BODY",hostTickCount64));
 c::gate.select(c::Domain::Interaction,false,hostTickCount64);
 c::gate.select(c::Domain::Hud,true,hostTickCount64);
 assert(c::accept("C27 HUD_DIALOGUE_PRESENTATION",hostTickCount64));
 assert(!c::accept("C27 HUD_DIALOGUE_PRESENTATION",hostTickCount64+c::Duration));

 c::gate.mask=0;
 assert(!c::essential("V66.90-PLAYBACK SNAP role=HENRY"));
 assert(!c::accept("V66.90-PLAYBACK SNAP role=HENRY",hostTickCount64));
 c::gate.select(c::Domain::Interaction,true,hostTickCount64);
 assert(c::accept("V66.90-PLAYBACK SNAP role=HENRY",hostTickCount64));
 assert(c::accept("V66.90-PLAYBACK CLIP role=REQUEST_TARGET",hostTickCount64));
 assert(c::accept("C70 F5_HELD_POSE item=2",hostTickCount64));
 assert(!c::accept("V66.90-PLAYBACK SNAP role=HENRY",hostTickCount64+c::Duration));
 c::gate.mask=0;c::gate.select(c::Domain::Reload,true,hostTickCount64);
 assert(c::accept("V66.90-PLAYBACK CLIP role=HENRY",hostTickCount64));
 c::gate.mask=0;
 assert(!c::essential("C63 DIALOGUE_HMD_LOOK call=1"));
 assert(!c::accept("C63 DIALOGUE_HMD_LOOK call=1",hostTickCount64));
 c::gate.select(c::Domain::Hud,true,hostTickCount64);
 assert(c::accept("C63 DIALOGUE_HMD_LOOK call=1",hostTickCount64));
 assert(!c::accept("C63 DIALOGUE_HMD_LOOK call=1",hostTickCount64+c::Duration));
 c::gate.select(c::Domain::Hud,false,hostTickCount64);
 assert(!c::accept("C51 CAMERA_BODY sample=1",hostTickCount64));
 c::gate.select(c::Domain::Combat,true,hostTickCount64);
 assert(c::accept("C51 CAMERA_BODY sample=1",hostTickCount64));
 c::gate.select(c::Domain::Combat,false,hostTickCount64);
 c::gate.select(c::Domain::Interaction,true,hostTickCount64);
 assert(!c::accept("C51 CAMERA_BODY sample=1",hostTickCount64));
 c::gate.select(c::Domain::Hud,true,hostTickCount64);
 assert(c::accept("C51 CAMERA_BODY sample=1",hostTickCount64));
 assert(!c::accept("C51 CAMERA_BODY sample=1",hostTickCount64+c::Duration));
 c::gate.mask=0;
 assert(c::domain("C43 GRAB_SOCKET side=0")==c::Domain::Interaction);
 assert(!c::accept("C43 GRAB_SOCKET side=0",hostTickCount64));
 c::gate.select(c::Domain::Combat,true,hostTickCount64);
 assert(!c::accept("C43 GRAB_SOCKET side=0",hostTickCount64));
 c::gate.select(c::Domain::Interaction,true,hostTickCount64);
 assert(c::accept("C43 GRAB_SOCKET side=0",hostTickCount64));
 assert(!c::accept("C43 GRAB_SOCKET side=0",hostTickCount64+c::Duration));
 c::gate.select(c::Domain::Vrik,false,1000);
 assert(c::gate.vrikRung(1000)==0&&c::gate.cycleVrik(1000)==1);
 const auto f7Until=c::gate.until[unsigned(c::Domain::Vrik)].load(),f7Session=c::gate.session[unsigned(c::Domain::Vrik)].load();
 assert(c::gate.cycleVrik(1001)==2&&c::gate.vrikRung(1001)==2);
 assert(c::gate.until[unsigned(c::Domain::Vrik)].load()==f7Until&&c::gate.session[unsigned(c::Domain::Vrik)].load()==f7Session);
 assert(c::gate.vrikRung(f7Until)==0&&c::gate.vrikRung(999)==0);
 assert(c::gate.cycleVrik(1002)==3&&c::gate.vrikRung(1002)==3);
 assert(c::gate.cycleVrik(1002)==0&&c::gate.vrikRung(1002)==0);
 assert(c::gate.cycleVrik(1003)==1&&c::gate.vrikFistSession.load()==0);
 c::gate.select(c::Domain::Vrik,true,1004);assert(c::gate.vrikRung(1004)==1);
 c::gate.cycleVrik(1005);assert(c::gate.vrikRung(1005)==2);
 c::gate.select(c::Domain::Vrik,false,1006);assert(c::gate.vrikRung(1006)==0);
 return 0;
}
