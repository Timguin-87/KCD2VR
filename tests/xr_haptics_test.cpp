// Executes the actual xr_input.cpp, against Khronos headers retained in the
// supplied Crysis VR source; runtime calls below are mocks, not headset proof.
#include <windows.h>
#include "../src/xr_input.cpp"
#include <map>
#include <string>
#include <vector>
static std::vector<std::string> tracesR95;
static unsigned nextHandle=10;template<class T>T handle(){return reinterpret_cast<T>(std::uintptr_t(++nextHandle));}
static std::map<std::string,XrPath> paths;static std::map<XrSpace,XrPath> spaces;
static XrResult syncResult=XR_SUCCESS,applyResult=XR_SUCCESS;static bool failHaptic=false,leftTracked=true,rightTracked=true;
static unsigned calls=0,stops=0,bindCount=0;static XrPath lastHand=0;static XrHapticVibration lastV{};
extern "C" {
XRAPI_ATTR XrResult XRAPI_CALL xrStringToPath(XrInstance,const char*s,XrPath*p){auto&v=paths[s];if(!v)v=paths.size()+1;*p=v;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateActionSet(XrInstance,const XrActionSetCreateInfo*,XrActionSet*p){*p=handle<XrActionSet>();return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateAction(XrActionSet,const XrActionCreateInfo*i,XrAction*p){if(i->actionType==XR_ACTION_TYPE_VIBRATION_OUTPUT&&failHaptic)return XR_ERROR_ACTION_TYPE_MISMATCH;*p=handle<XrAction>();return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrSuggestInteractionProfileBindings(XrInstance,const XrInteractionProfileSuggestedBinding*i){bindCount=i->countSuggestedBindings;for(unsigned j=0;j<bindCount;++j)if(!i->suggestedBindings[j].action)return XR_ERROR_HANDLE_INVALID;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateActionSpace(XrSession,const XrActionSpaceCreateInfo*i,XrSpace*p){*p=handle<XrSpace>();spaces[*p]=i->subactionPath;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrAttachSessionActionSets(XrSession,const XrSessionActionSetsAttachInfo*){return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrSyncActions(XrSession,const XrActionsSyncInfo*){return syncResult;}
XRAPI_ATTR XrResult XRAPI_CALL xrLocateSpace(XrSpace space,XrSpace,XrTime,XrSpaceLocation*out){bool tracked=spaces[space]==paths["/user/hand/left"]?leftTracked:rightTracked;out->locationFlags=tracked?XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT|XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT:0;out->pose.orientation.w=1;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateFloat(XrSession,const XrActionStateGetInfo*,XrActionStateFloat*out){out->isActive=XR_TRUE;out->currentState=0;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateVector2f(XrSession,const XrActionStateGetInfo*,XrActionStateVector2f*out){out->isActive=XR_TRUE;out->currentState={};return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateBoolean(XrSession,const XrActionStateGetInfo*,XrActionStateBoolean*out){out->isActive=XR_TRUE;out->currentState=0;out->changedSinceLastSync=0;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrApplyHapticFeedback(XrSession,const XrHapticActionInfo*i,const XrHapticBaseHeader*h){++calls;lastHand=i->subactionPath;lastV=*reinterpret_cast<const XrHapticVibration*>(h);return applyResult;}
XRAPI_ATTR XrResult XRAPI_CALL xrStopHapticFeedback(XrSession,const XrHapticActionInfo*){++stops;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrDestroySpace(XrSpace){return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrDestroyActionSet(XrActionSet){return XR_SUCCESS;}
}
static unsigned tests=0,failures=0;static void check(bool v,const char*n){++tests;failures+=!v;std::printf("%s %s\n",v?"PASS":"FAIL",n);}

static void correlationR95(){
 using namespace kcd2vr;namespace h=haptics101;namespace cap=diagnostics::capture168;
 hostTickCount64=5000;failHaptic=false;applyResult=syncResult=XR_SUCCESS;leftTracked=rightTracked=true;
 XrInput input;check(input.Initialize(handle<XrInstance>(),handle<XrSession>(),handle<XrSpace>()),"R95 correlation uses the actual XR input adapter");
 h::select(true);h::allowed=true;tracesR95.clear();const auto initialCalls=calls;
 cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);
 check(h::publish_event(0,9000,.4f,30,hostTickCount64),"R95 producer reports actual mailbox acceptance");input.Sync(500);
 check(tracesR95.empty()&&calls==initialCalls+1,"R95 F5 off permits feedback and emits no cue diagnostic");
 cap::gate.select(cap::Domain::Interaction,true,hostTickCount64);
 const auto serial=cap::gate.session[unsigned(cap::Domain::Interaction)].load();
 check(h::publish_event(0,9001,.4f,30,hostTickCount64)&&tracesR95.size()==1&&tracesR95.back().find("phase=QUEUE_ACCEPTED")!=std::string::npos,
       "R95 producer receipt follows successful mailbox publish, before XR call");
 h::Pulse pulse{};h::queue[0].read(pulse);const auto identity="key=9001 epoch="+std::to_string(pulse.epoch)+" sequence="+std::to_string(pulse.sequence);
 check(pulse.captureSessionR95==serial&&tracesR95.back().find(identity)!=std::string::npos&&calls==initialCalls+1,"R95 queue receipt contains exact cue key/epoch/sequence and original F5 lease");
 input.Sync(501);check(tracesR95.size()==2&&tracesR95.back().find(identity)!=std::string::npos&&tracesR95.back().find("phase=XR_SUBMITTED")!=std::string::npos&&tracesR95.back().find("xrResult=0")!=std::string::npos&&calls==initialCalls+2,"R95 actual successful XR API result joins the same cue");
 const auto count=tracesR95.size();input.Sync(502);check(tracesR95.size()==count,"R95 seen cue emits no duplicate submit receipt");
 h::publish_event(0,9002,.4f,30,hostTickCount64);rightTracked=false;input.Sync(503);rightTracked=true;
 check(tracesR95.back().find("phase=REJECTED reason=HAND_UNTRACKED")!=std::string::npos,"R95 actual consumer logs tracking refusal");
 const auto before=calls;input.Sync(504);check(calls==before,"R95 recovery cannot replay a rejected cue");
 h::publish_event(0,9003,.4f,30,hostTickCount64);hostTickCount64+=151;input.Sync(505);
 check(tracesR95.back().find("phase=EXPIRED reason=CUE_EXPIRED")!=std::string::npos&&calls==before,"R95 observed stale cue is expired without calling XR");
 h::publish_event(0,9004,.4f,30,hostTickCount64);applyResult=XR_SESSION_NOT_FOCUSED;input.Sync(506);applyResult=XR_SUCCESS;
 check(tracesR95.back().find("phase=XR_REJECTED")!=std::string::npos&&tracesR95.back().find("xrResult=8")!=std::string::npos,"R95 non-success XR result is not reported delivered");
 h::publish_event(0,9005,.4f,30,hostTickCount64);syncResult=XR_SESSION_NOT_FOCUSED;input.Sync(507);syncResult=XR_SUCCESS;
 check(tracesR95.back().find("phase=REJECTED reason=FOCUS_LOST")!=std::string::npos,"R95 focus rejection records same queued request");
 h::publish_event(0,9006,.4f,30,hostTickCount64);h::epoch.fetch_add(1);input.Sync(508);
 check(tracesR95.back().find("phase=REJECTED reason=MODE_EPOCH_CHANGED")!=std::string::npos,"R95 changed mode epoch records rejection");
 h::publish_event(0,9007,.4f,30,hostTickCount64+1);input.Sync(509);
 check(tracesR95.back().find("phase=REJECTED reason=FUTURE_CUE")!=std::string::npos,"R95 future cue never submitted");
 h::publish_event(0,9008,.4f,30,hostTickCount64);cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);input.Sync(510);
 check(tracesR95.back().find("key=9008")!=std::string::npos&&tracesR95.back().find("phase=XR_SUBMITTED")!=std::string::npos,"R95 already-captured receipt uses only original finite drain grace");
 const auto stopped=tracesR95.size();h::publish_event(0,9009,.4f,30,hostTickCount64);input.Sync(511);
 check(tracesR95.size()==stopped,"R95 off recorder cannot acquire new queued receipts");
 cap::gate.select(cap::Domain::Interaction,true,hostTickCount64);h::publish_event(0,9010,.4f,30,hostTickCount64);const auto oldSession=tracesR95.size();
 cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);cap::gate.select(cap::Domain::Interaction,true,hostTickCount64);input.Sync(512);
 check(tracesR95.size()==oldSession,"R95 queued receipt never borrows a replacement capture session");
 const auto invalid=tracesR95.size();check(!h::publish_event(2,9011,.4f,30,hostTickCount64)&&tracesR95.size()==invalid,"R95 rejected producer cannot log queue acceptance");
 for(const char* phase:{"phase=QUEUE_ACCEPTED ","phase=XR_SUBMITTED ","phase=REJECTED ","phase=EXPIRED ","phase=XR_REJECTED "})
  for(const auto& line:tracesR95)if(line.find(phase)!=std::string::npos){std::printf("HOST_RECEIPT %s\n",line.c_str());break;}
 cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);input.Shutdown();
}
int main(){using namespace kcd2vr;namespace h=haptics101;
 XrInput input;check(input.Initialize(handle<XrInstance>(),handle<XrSession>(),handle<XrSpace>())&&bindCount==23&&paths.count("/user/hand/right/input/thumbrest/touch")&&paths.count("/user/hand/right/input/trigger/touch")/*R59.16C5 optional trigger touch*/,"actual input creates haptics plus per-hand Touch/thumbrest bindings");
 h::select(true);h::allowed=true;h::Cue cue{};hostTickCount64=1000;h::observe(cue,0,77,.1,true,1,1000);input.Sync(1);check(calls==0,"proximity must settle before readiness pulse");
 hostTickCount64=1070;h::observe(cue,0,77,.1,true,2,1070);input.Sync(2);check(calls==1&&lastHand==paths["/user/hand/right"]&&lastV.duration==70000000&&lastV.amplitude==.55f,"native-ready right mount queues configured 70ms proximity pulse through actual consumer");
 for(int i=0;i<100;++i){hostTickCount64++;h::observe(cue,0,77,.1,true,3+i,hostTickCount64);input.Sync(3+i);}check(calls==1,"standing near mount does not vibrate every frame");
 h::allowed=false;input.Sync(200);const auto stop=stops;check(stops==1,"menu/gameplay loss stops prior haptic");input.Sync(201);check(stops==stop,"stopped feedback is not repeatedly stopped");
 h::allowed=true;hostTickCount64=2000;h::queue[1].publish({h::epoch.load(),1000,2000,78,.12f,35});input.Sync(202);check(calls==2&&lastHand==paths["/user/hand/left"],"left hand uses distinct subaction path");
 leftTracked=false;hostTickCount64=2010;h::queue[1].publish({h::epoch.load(),1001,2010,78,.12f,35});input.Sync(203);const auto before=calls;leftTracked=true;input.Sync(204);check(calls==before,"tracking recovery does not replay discarded haptic");
 hostTickCount64=2300;h::queue[0].publish({h::epoch.load(),1002,2000,77,.12f,35});input.Sync(205);check(calls==before,"expired cue is never played");
 h::queue[0].publish({h::epoch.load(),1003,2300,77,.12f,35});syncResult=XR_SESSION_NOT_FOCUSED;check(!input.Sync(206),"positive NOT_FOCUSED result is not treated as usable action sync");syncResult=XR_SUCCESS;input.Sync(207);check(calls==before,"focus recovery does not replay old cue");
 h::queue[0].publish({h::epoch.load(),1004,2300,77,.12f,35});h::select(false);h::select(true);input.Sync(208);check(calls==before,"mode epoch rejects old readiness target");
 h::queue[0].publish({h::epoch.load(),1005,2300,77,.12f,35});applyResult=XR_SESSION_NOT_FOCUSED;input.Sync(209);const auto failed=calls;input.Sync(210);check(calls==failed,"failed haptic is not retried each frame");input.Shutdown();
 failHaptic=true;applyResult=XR_SUCCESS;XrInput other;check(other.Initialize(handle<XrInstance>(),handle<XrSession>(),handle<XrSpace>())&&bindCount==21/*R59.16C5: +2 trigger touch*/&&other.Sync(300),"unsupported haptic action does not disable tracking or thumbrest input");other.Shutdown();
 correlationR95();
 std::printf("xr_haptics=%u/%u runtime=MOCKED\n",tests-failures,tests);return failures?1:0;
}

namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {}
bool EnabledFor(diagnostics::capture168::Domain domain) noexcept {return diagnostics::capture168::active(domain,hostTickCount64);}
void WriteReceiptFor(diagnostics::capture168::Domain domain,std::uint64_t session,const char* line) noexcept {
 if(diagnostics::capture168::gate.receipt(domain,session,hostTickCount64))tracesR95.emplace_back(line);
}
}
