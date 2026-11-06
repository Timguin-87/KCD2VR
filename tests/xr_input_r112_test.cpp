// Host fixture: production XrInput declarations and all method bodies are
// extracted unchanged by tools/check_xr_input_r112.py. OpenXR/Win32/log leaves
// below are explicit controlled doubles; this is not an SDK ABI or HMD test.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <utility>
#include "quick_access_r112.h"
#include "core/haptics.h"
#include "vrik/claude_motion_d69.h"

// Explicit Win32 output leaves for complete downstream TouchBindings methods.
// No real OS injection occurs; API ABI/layout is not under test.
using UINT=unsigned;using WORD=unsigned short;using DWORD=unsigned long;
constexpr UINT INPUT_KEYBOARD=1,INPUT_MOUSE=0,MAPVK_VK_TO_VSC=0;
constexpr DWORD KEYEVENTF_SCANCODE=8,KEYEVENTF_KEYUP=2,MOUSEEVENTF_LEFTDOWN=2,
    MOUSEEVENTF_LEFTUP=4,MOUSEEVENTF_RIGHTDOWN=8,MOUSEEVENTF_RIGHTUP=16,
    MOUSEEVENTF_MOVE=1,MOUSEEVENTF_WHEEL=2048;
constexpr int WHEEL_DELTA=120,VK_ESCAPE=27,VK_TAB=9,VK_LEFT=37,VK_UP=38,VK_RIGHT=39,VK_DOWN=40;
struct INPUT {UINT type=0;struct {WORD wVk=0,wScan=0;DWORD dwFlags=0;}ki;
    struct {long dx=0,dy=0;DWORD dwFlags=0,mouseData=0;}mi;};
static UINT MapVirtualKeyW(UINT key,UINT){return key;}
static DWORD GetLastError(){return 0;}
using XrInstance=std::uint64_t; using XrSession=std::uint64_t;
using XrActionSet=std::uint64_t; using XrAction=std::uint64_t;
using XrPath=std::uint64_t; using XrSpace=std::uint64_t;
using XrTime=std::int64_t; using XrDuration=std::int64_t;
using XrSpaceLocationFlags=std::uint64_t;
using XrResult=int; using XrActionType=int; using XrBool32=std::uint32_t;
constexpr std::uint64_t XR_NULL_HANDLE=0, XR_NULL_PATH=0;
constexpr XrResult XR_SUCCESS=0, XR_SESSION_NOT_FOCUSED=8,
    XR_ERROR_RUNTIME_FAILURE=-2, XR_ERROR_PATH_INVALID=-19,
    XR_ERROR_PATH_UNSUPPORTED=-22;
constexpr XrBool32 XR_TRUE=1, XR_FALSE=0;
constexpr std::size_t XR_MAX_ACTION_SET_NAME_SIZE=64,
    XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE=128, XR_MAX_ACTION_NAME_SIZE=64,
    XR_MAX_LOCALIZED_ACTION_NAME_SIZE=128, XR_MAX_PATH_LENGTH=256;
constexpr XrSpaceLocationFlags XR_SPACE_LOCATION_ORIENTATION_VALID_BIT=1,
    XR_SPACE_LOCATION_POSITION_VALID_BIT=2,
    XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT=4,
    XR_SPACE_LOCATION_POSITION_TRACKED_BIT=8;
constexpr int XR_SPACE_VELOCITY_LINEAR_VALID_BIT=1, XR_SPACE_VELOCITY_ANGULAR_VALID_BIT=2;
constexpr float XR_FREQUENCY_UNSPECIFIED=0;
enum {XR_ACTION_TYPE_BOOLEAN_INPUT=1, XR_ACTION_TYPE_FLOAT_INPUT,
    XR_ACTION_TYPE_VECTOR2F_INPUT, XR_ACTION_TYPE_POSE_INPUT,
    XR_ACTION_TYPE_VIBRATION_OUTPUT};
enum {XR_TYPE_SPACE_VELOCITY=1, XR_TYPE_ACTION_SET_CREATE_INFO,
    XR_TYPE_ACTION_CREATE_INFO, XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
    XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING, XR_TYPE_ACTION_SPACE_CREATE_INFO,
    XR_TYPE_SPACE_LOCATION, XR_TYPE_ACTION_STATE_GET_INFO, XR_TYPE_ACTION_STATE_FLOAT,
    XR_TYPE_ACTION_STATE_VECTOR2F, XR_TYPE_ACTION_STATE_BOOLEAN, XR_TYPE_ACTIONS_SYNC_INFO,
    XR_TYPE_INTERACTION_PROFILE_STATE, XR_TYPE_HAPTIC_ACTION_INFO, XR_TYPE_HAPTIC_VIBRATION};
#define XR_FAILED(result) ((result)<0)
#define XR_SUCCEEDED(result) ((result)>=0)
struct XrVector2f {float x=0,y=0;};
struct XrVector3f {float x=0,y=0,z=0;};
struct XrQuaternionf {float x=0,y=0,z=0,w=0;};
struct XrPosef {XrQuaternionf orientation{};XrVector3f position{};};
struct XrSpaceVelocity {int type;void* next=nullptr;std::uint64_t velocityFlags=0;XrVector3f linearVelocity{},angularVelocity{};};
struct XrSpaceLocation {int type;void* next=nullptr;XrSpaceLocationFlags locationFlags=0;XrPosef pose{};};
struct XrActionSetCreateInfo {int type;const void* next=nullptr;char actionSetName[64]{};char localizedActionSetName[128]{};std::uint32_t priority=0;};
struct XrActionCreateInfo {int type;const void* next=nullptr;char actionName[64]{};XrActionType actionType=0;std::uint32_t countSubactionPaths=0;const XrPath* subactionPaths=nullptr;char localizedActionName[128]{};};
struct XrSessionActionSetsAttachInfo {int type;const void* next=nullptr;std::uint32_t countActionSets=0;const XrActionSet* actionSets=nullptr;};
struct XrActionSuggestedBinding {XrAction action;XrPath binding;};
struct XrInteractionProfileSuggestedBinding {int type;const void* next=nullptr;XrPath interactionProfile=0;std::uint32_t countSuggestedBindings=0;const XrActionSuggestedBinding* suggestedBindings=nullptr;};
struct XrActionSpaceCreateInfo {int type;const void* next=nullptr;XrAction action=0;XrPath subactionPath=0;XrPosef poseInActionSpace{};};
struct XrActionStateGetInfo {int type;const void* next=nullptr;XrAction action=0;XrPath subactionPath=0;};
struct XrActionStateFloat {int type;void* next=nullptr;float currentState=0;XrBool32 changedSinceLastSync=0;XrTime lastChangeTime=0;XrBool32 isActive=0;};
struct XrActionStateVector2f {int type;void* next=nullptr;XrVector2f currentState{};XrBool32 changedSinceLastSync=0;XrTime lastChangeTime=0;XrBool32 isActive=0;};
struct XrActionStateBoolean {int type;void* next=nullptr;XrBool32 currentState=0,changedSinceLastSync=0;XrTime lastChangeTime=0;XrBool32 isActive=0;};
struct XrActiveActionSet {XrActionSet actionSet;XrPath subactionPath;};
struct XrActionsSyncInfo {int type;const void* next=nullptr;std::uint32_t countActiveActionSets=0;const XrActiveActionSet* activeActionSets=nullptr;};
struct XrInteractionProfileState {int type;void* next=nullptr;XrPath interactionProfile=0;};
struct XrHapticActionInfo {int type;const void* next=nullptr;XrAction action=0;XrPath subactionPath=0;};
struct XrHapticBaseHeader {int type;const void* next=nullptr;};
struct XrHapticVibration {int type;const void* next=nullptr;XrDuration duration=0;float frequency=0,amplitude=0;};

namespace fixture {
struct ActionState {XrResult result=XR_SUCCESS;bool active=true;float value=.75f;XrVector2f vec{.35f,-.45f};bool down=true,changed=true;};
struct SpaceState {XrResult result=XR_SUCCESS;XrSpaceLocationFlags flags=15;XrPosef pose{{0,0,0,1},{1,2,3}};};
struct Runtime {
    std::uint64_t clock=1000,nextHandle=100;
    unsigned apiCalls=0,attaches=0,createdSets=0,destroyedSets=0,createdSpaces=0,destroyedSpaces=0,hapticApplies=0,hapticStops=0;
    XrResult syncResult=XR_SUCCESS;
    bool rejectOptionalTouch=false,rejectAll=false,profileNone=false;
    bool failFaceCreate=false,rejectFaceBinding=false,rejectThumbrest=false,nameFailure=false;
    bool nullProfile[2]{false,false};
    XrResult profileResult[2]{XR_SUCCESS,XR_SUCCESS};
    std::string selectedProfile="/interaction_profiles/oculus/touch_controller";
    unsigned availabilityInsideSync=0,touchBindingLimit=0,actionLimit=0,actionAttempts=0;
    std::map<std::pair<std::string,int>,unsigned> reads;
    std::vector<std::pair<std::string,XrResult>> suggestions;
    std::map<std::string,XrPath> pathIds;
    std::map<XrPath,std::string> pathNames;
    std::map<XrAction,std::string> actionNames;
    std::map<XrSpace,std::pair<std::string,int>> spaces;
    std::map<std::pair<std::string,int>,ActionState> states;
    std::map<std::pair<std::string,int>,SpaceState> locations;
    std::map<std::string,std::vector<std::string>> acceptedBindings;
    std::vector<std::string> status;
    std::vector<INPUT> osInputs;
    XrPath path(const std::string& name){auto& p=pathIds[name];if(!p){p=++nextHandle;pathNames[p]=name;}return p;}
    int side(XrPath p){return pathNames.at(p)=="/user/hand/left"?0:1;}
    ActionState& state(const char* name,int side){return states[{name,side}];}
    SpaceState& location(const char* name,int side){return locations[{name,side}];}
} rt;
}
static std::uint64_t GetTickCount64(){return fixture::rt.clock;}
static UINT SendInput(UINT n,INPUT* inputs,int){fixture::rt.osInputs.insert(fixture::rt.osInputs.end(),inputs,inputs+n);return n;}
static XrResult xrStringToPath(XrInstance,const char* name,XrPath* path){++fixture::rt.apiCalls;*path=fixture::rt.path(name);return XR_SUCCESS;}
static XrResult xrCreateActionSet(XrInstance,const XrActionSetCreateInfo*,XrActionSet* out){auto& r=fixture::rt;++r.apiCalls;++r.createdSets;*out=++r.nextHandle;return XR_SUCCESS;}
static XrResult xrCreateAction(XrActionSet,const XrActionCreateInfo* info,XrAction* out){auto& r=fixture::rt;++r.apiCalls;
    ++r.actionAttempts;
    if((r.actionLimit&&r.actionAttempts>r.actionLimit)||(r.failFaceCreate&&std::string(info->actionName)=="quick_access_face_touch")){*out=0;return XR_ERROR_RUNTIME_FAILURE;}
    *out=++r.nextHandle;r.actionNames[*out]=info->actionName;return XR_SUCCESS;}
static XrResult xrSuggestInteractionProfileBindings(XrInstance,const XrInteractionProfileSuggestedBinding* info){
    auto& r=fixture::rt;++r.apiCalls;const auto name=r.pathNames.at(info->interactionProfile);
    std::vector<std::string> bindings;XrResult result=XR_SUCCESS;
    for(unsigned i=0;i<info->countSuggestedBindings;++i){const auto& b=info->suggestedBindings[i];const auto& path=r.pathNames.at(b.binding);
        const auto& action=r.actionNames.at(b.action);
        if((r.rejectFaceBinding&&action=="quick_access_face_touch")||
           (r.rejectThumbrest&&action=="quick_access_touch")||
           (r.rejectOptionalTouch&&name=="/interaction_profiles/oculus/touch_controller"&&path.find("/touch")!=std::string::npos))
            result=XR_ERROR_PATH_UNSUPPORTED;
        bindings.push_back(action+"="+path);}
    if(r.rejectAll||(r.touchBindingLimit&&name=="/interaction_profiles/oculus/touch_controller"&&info->countSuggestedBindings>r.touchBindingLimit))
        result=XR_ERROR_PATH_UNSUPPORTED;
    r.suggestions.push_back({name,result});
    if(XR_SUCCEEDED(result))r.acceptedBindings[name]=bindings;
    return result;
}
static XrResult xrCreateActionSpace(XrSession,const XrActionSpaceCreateInfo* info,XrSpace* out){auto& r=fixture::rt;++r.apiCalls;++r.createdSpaces;*out=++r.nextHandle;r.spaces[*out]={r.actionNames.at(info->action),r.side(info->subactionPath)};return XR_SUCCESS;}
static XrResult xrAttachSessionActionSets(XrSession,const XrSessionActionSetsAttachInfo*){++fixture::rt.apiCalls;++fixture::rt.attaches;return XR_SUCCESS;}
static XrResult xrSyncActions(XrSession,const XrActionsSyncInfo*){++fixture::rt.apiCalls;
    fixture::rt.availabilityInsideSync=kcd2vr::quick_access_r112::AvailableHands();return fixture::rt.syncResult;}
static XrResult xrGetCurrentInteractionProfile(XrSession,XrPath side,XrInteractionProfileState* out){auto& r=fixture::rt;++r.apiCalls;
    const int i=r.side(side);out->interactionProfile=r.nullProfile[i]?0:r.path(r.selectedProfile);return r.profileResult[i];}
static XrResult xrPathToString(XrInstance,XrPath path,std::uint32_t cap,std::uint32_t* count,char* out){auto& r=fixture::rt;++r.apiCalls;
    if(r.nameFailure)return XR_ERROR_RUNTIME_FAILURE;
    const auto& name=r.pathNames.at(path);*count=static_cast<std::uint32_t>(name.size()+1);std::snprintf(out,cap,"%s",name.c_str());return XR_SUCCESS;}
static XrResult xrGetActionStateFloat(XrSession,const XrActionStateGetInfo* info,XrActionStateFloat* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.states[{r.actionNames.at(info->action),r.side(info->subactionPath)}];if(XR_FAILED(s.result))return s.result;out->currentState=s.value;out->isActive=s.active;return s.result;}
static XrResult xrGetActionStateVector2f(XrSession,const XrActionStateGetInfo* info,XrActionStateVector2f* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.states[{r.actionNames.at(info->action),r.side(info->subactionPath)}];if(XR_FAILED(s.result))return s.result;out->currentState=s.vec;out->isActive=s.active;return s.result;}
static XrResult xrGetActionStateBoolean(XrSession,const XrActionStateGetInfo* info,XrActionStateBoolean* out){
    auto& r=fixture::rt;++r.apiCalls;const auto name=r.actionNames.at(info->action);const int side=r.side(info->subactionPath);
    ++r.reads[{name,side}];auto& s=r.states[{name,side}];if(XR_FAILED(s.result))return s.result;
    bool bound=true;
    if(name=="quick_access_face_touch"||name=="quick_access_touch"){
        bound=false;const auto prefix=name+"="+r.pathNames.at(info->subactionPath)+"/";
        for(const auto& b:r.acceptedBindings[r.selectedProfile])if(b.find(prefix)==0)bound=true;
    }
    out->currentState=s.down;out->changedSinceLastSync=s.changed;out->isActive=s.active&&bound;return s.result;
}
static XrResult xrLocateSpace(XrSpace space,XrSpace,XrTime,XrSpaceLocation* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.locations[r.spaces.at(space)];if(XR_FAILED(s.result))return s.result;out->locationFlags=s.flags;out->pose=s.pose;if(out->next){auto& v=*static_cast<XrSpaceVelocity*>(out->next);v.velocityFlags=3;v.linearVelocity={1,2,3};v.angularVelocity={4,5,6};}return s.result;}
static XrResult xrStopHapticFeedback(XrSession,const XrHapticActionInfo*){++fixture::rt.apiCalls;++fixture::rt.hapticStops;return XR_SUCCESS;}
static XrResult xrApplyHapticFeedback(XrSession,const XrHapticActionInfo*,const XrHapticBaseHeader*){++fixture::rt.apiCalls;++fixture::rt.hapticApplies;return XR_SUCCESS;}
static XrResult xrDestroySpace(XrSpace){++fixture::rt.apiCalls;++fixture::rt.destroyedSpaces;return XR_SUCCESS;}
static XrResult xrDestroyActionSet(XrActionSet){++fixture::rt.apiCalls;++fixture::rt.destroyedSets;return XR_SUCCESS;}

namespace DetourModKit {inline void log(){}}
#define KCD2VR_DIAG_LOG(...) ((void)0)
namespace kcd2vr::diagnostics::xr165 {enum Kind {GripLocate,ActionSync};struct Timer {explicit Timer(Kind){}};}
namespace kcd2vr::diagnostic_trace {
inline void Write(const char*){}
inline void WriteStatus(const char* text){fixture::rt.status.emplace_back(text);}
inline void WriteReceiptFor(diagnostics::capture168::Domain,std::uint64_t,const char*){}
}
#include "production_input_r112.inc"

namespace {
unsigned checks=0;std::vector<std::string> failures;
void check(bool ok,const char* name){++checks;if(!ok){failures.emplace_back(name);std::fprintf(stderr,"FAIL: %s\n",name);}}
bool contains(const std::vector<std::string>& v,const std::string& text){return std::find(v.begin(),v.end(),text)!=v.end();}
bool status(const std::string& text){for(const auto& s:fixture::rt.status)if(s.find(text)!=std::string::npos)return true;return false;}
bool valid(const kcd2vr::ControllerState& s){
#if HAS_FACE_TOUCH_R112
    return s.quick_access_touch_valid;
#else
    (void)s;return false; // Baseline has no selected-sensor validity publication.
#endif
}
void reset(){fixture::rt={};fixture::rt.state("button_b",1).down=false;fixture::rt.state("button_y",0).down=false;kcd2vr::quick_access_r112::SetFaceTouch(false);kcd2vr::quick_access_r112::ClearAvailable();}
struct Instance {
    kcd2vr::XrInput input;
    Instance(){check(input.Initialize(1,2,3),"core attached actions initialize");}
    ~Instance(){input.Shutdown();}
};
unsigned reads(const char* name){return fixture::rt.reads[{name,0}]+fixture::rt.reads[{name,1}];}
bool core(const kcd2vr::XrInput& in){return in.Left().active&&in.Right().active&&in.Left().aim_active&&in.Right().aim_active&&
    in.Left().trigger==.75f&&in.Right().grip==.75f&&in.Left().thumb_x==.35f&&in.Right().thumb_y==-.45f&&
    in.Right().a_down&&(in.Right().b_down==fixture::rt.state("button_b",1).down)&&in.Right().b_valid&&in.Left().x_down&&(in.Left().y_down==fixture::rt.state("button_y",0).down)&&in.Left().menu_down&&
    in.Right().thumbstick_down&&in.Right().thumbstick_pressed;}
}
int main(){
    using fixture::rt;namespace qa=kcd2vr::quick_access_r112;
    const std::string touch="/interaction_profiles/oculus/touch_controller",index="/interaction_profiles/valve/index_controller";
    reset();
    check(!qa::FaceTouch()&&qa::AvailableHands()==0,"default remains explicit thumbrest without availability claim");
    {
        Instance f;auto& in=f.input;
        check(contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/left/input/y/touch")&&
              contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/right/input/b/touch"),"Touch secondary-face sensor pair is bound");
        check(contains(rt.acceptedBindings[index],"quick_access_face_touch=/user/hand/left/input/b/touch")&&
              contains(rt.acceptedBindings[index],"quick_access_face_touch=/user/hand/right/input/b/touch"),"Index secondary-face sensor pair is bound");
        check(contains(rt.acceptedBindings[touch],"button_b=/user/hand/right/input/b/click")&&
              contains(rt.acceptedBindings[touch],"button_y=/user/hand/left/input/y/click")&&
              contains(rt.acceptedBindings[touch],"thumbstick_click=/user/hand/right/input/thumbstick/click")&&
              contains(rt.acceptedBindings[touch],"menu=/user/hand/left/input/menu/click"),"existing B Y menu and kick click paths remain separate");
        check(in.Sync(100)&&core(in),"default preserves current poses analogs clicks and R3 kick input");
        check(in.Left().quick_access_touch&&in.Right().quick_access_touch&&reads("quick_access_touch")==2&&reads("quick_access_face_touch")==0,
              "default queries only thumbrest sensor");
        check(valid(in.Left())&&valid(in.Right())&&qa::AvailableHands()==3,"successful selected sensor publishes per-hand availability");
        qa::SetFaceTouch(false);check(qa::AvailableHands()==3,"repeated menu Apply preserves completed availability");
        rt.reads.clear();check(in.Sync(101)&&rt.availabilityInsideSync==3&&qa::AvailableHands()==3,
              "healthy in-progress Sync does not flicker UI availability");
        rt.state("quick_access_touch",0).down=false;rt.state("quick_access_touch",1).down=false;
        rt.state("quick_access_face_touch",0).down=true;rt.state("quick_access_face_touch",1).down=true;
        qa::SetFaceTouch(true);check(qa::AvailableHands()==0,"actual mode change clears old source availability");
        rt.reads.clear();check(in.Sync(102),"live source selection needs no action reattach");
        check(in.Left().quick_access_touch&&in.Right().quick_access_touch,"face-touch mode drives existing modifier while thumbrest is released");
        check(reads("quick_access_face_touch")==2&&reads("quick_access_touch")==0,"face mode queries only its selected optional action");
        check(core(in)&&!in.RecenterRequested()&&rt.attaches==1&&rt.createdSets==1&&rt.createdSpaces==4,
              "sensor selection preserves native controls and attached action lifetime");
        rt.state("quick_access_face_touch",0).down=false;rt.state("quick_access_face_touch",1).down=false;
        rt.state("quick_access_touch",0).down=true;rt.state("quick_access_touch",1).down=true;
        check(in.Sync(103)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch,
              "released selected face sensors do not fall back to thumbrest");
        check(valid(in.Left())&&valid(in.Right())&&qa::AvailableHands()==3,"released active sensors are available without modifier credit");
        rt.state("quick_access_face_touch",0).down=true;rt.state("quick_access_face_touch",1).down=true;
        rt.state("quick_access_face_touch",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("quick_access_face_touch",1).active=false;
        check(in.Sync(104)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&!valid(in.Left())&&!valid(in.Right())&&qa::AvailableHands()==0,
              "failed and inactive face reads clear each modifier and availability");
        check(core(in),"optional touch failure leaves pose grip trigger stick B and R3 current");
        rt.state("quick_access_face_touch",1).active=true;
        check(in.Sync(105)&&!in.Left().quick_access_touch&&in.Right().quick_access_touch&&qa::AvailableHands()==2,
              "right modifier recovers independently while left read still fails");
        rt.state("quick_access_face_touch",0).result=XR_SUCCESS;
        rt.state("button_b",1).changed=false;rt.state("thumbstick_click",1).changed=false;
        check(in.Sync(106)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&qa::AvailableHands()==3,
              "left modifier recovers next read without restart or latch");
        check(!in.Right().b_down&&in.Right().b_valid&&!in.Right().b_pressed&&in.Right().thumbstick_down&&!in.Right().thumbstick_pressed,
              "touch recovery does not fabricate Back or R3 press edges");
        qa::SetFaceTouch(false);rt.reads.clear();
        check(in.Sync(107)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&reads("quick_access_touch")==2&&reads("quick_access_face_touch")==0,
              "live return to default reads original thumbrest only");
        qa::SetFaceTouch(true);qa::PublishAvailable(false,3);
        check(qa::AvailableHands()==0,"outdated other-mode availability publication is refused");
        rt.selectedProfile=index;check(in.Sync(108)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&qa::AvailableHands()==3,
              "Index uses accepted secondary-face touch without profile guessing");
    }
    reset();rt.failFaceCreate=true;
    {
        Instance f;auto& in=f.input;
        check(in.Sync(200)&&core(in)&&in.Left().quick_access_touch&&in.Right().quick_access_touch,
              "optional face-action creation failure preserves default and core input");
        qa::SetFaceTouch(true);rt.reads.clear();
        check(in.Sync(201)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&qa::AvailableHands()==0,
              "selected absent action is unavailable without fallback or held modifier");
        check(core(in)&&reads("quick_access_face_touch")==0&&rt.attaches==1,
              "absent optional action never queries a null handle or rebuilds actions");
        qa::SetFaceTouch(false);check(in.Sync(202)&&in.Right().quick_access_touch,
              "return from unavailable option to thumbrest recovers immediately");
    }
    reset();rt.rejectFaceBinding=true;
    {
        Instance f;auto& in=f.input;
        check(!contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/right/input/b/touch")&&
              contains(rt.acceptedBindings[touch],"button_b=/user/hand/right/input/b/click")&&
              contains(rt.acceptedBindings[index],"button_b=/user/hand/right/input/b/click"),
              "optional face binding refusal preserves both profiles core suggestions");
        bool touchFinal=false,indexFinal=false,hadRefusal=false;
        for(const auto& s:rt.suggestions){if(s.first==touch)touchFinal=s.second==XR_SUCCESS;if(s.first==index)indexFinal=s.second==XR_SUCCESS;if(s.second==XR_ERROR_PATH_UNSUPPORTED)hadRefusal=true;}
        check(hadRefusal&&touchFinal&&indexFinal,"optional refusal is followed by accepted final suggestions");
        qa::SetFaceTouch(true);
        check(in.Sync(300)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&qa::AvailableHands()==0&&core(in),
              "unbound optional sensors stay neutral while normal controls survive");
        rt.selectedProfile=index;
        check(in.Sync(301)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&core(in),
              "Index optional refusal retains current core control input");
        rt.selectedProfile=touch;qa::SetFaceTouch(false);check(in.Sync(302)&&in.Right().quick_access_touch,
              "optional pair refusal does not remove the original thumbrest action");
    }
    reset();rt.touchBindingLimit=23;
    {
        Instance f;
        check(contains(rt.acceptedBindings[touch],"quick_access_touch=/user/hand/right/input/thumbrest/touch")&&
              contains(rt.acceptedBindings[touch],"trigger_touch=/user/hand/right/input/trigger/touch")&&
              contains(rt.acceptedBindings[touch],"feedback=/user/hand/right/output/haptic")&&
              !contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/right/input/b/touch"),
              "new optional pair cannot displace established thumbrest trigger-touch or haptic bindings");
        check(f.input.Sync(350)&&core(f.input)&&f.input.Right().quick_access_touch&&f.input.Right().trigger_touch_valid,
              "binding-limited runtime retains established default controls");
        qa::SetFaceTouch(true);
        check(f.input.Sync(351)&&!f.input.Right().quick_access_touch&&qa::AvailableHands()==0&&core(f.input),
              "binding-limited runtime reports unavailable explicit face mode without blocking core input");
    }
    reset();rt.actionLimit=15;
    {
        Instance f;
        check(contains(rt.acceptedBindings[touch],"trigger_touch=/user/hand/right/input/trigger/touch")&&
              contains(rt.acceptedBindings[touch],"feedback=/user/hand/right/output/haptic")&&
              !contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/right/input/b/touch"),
              "new optional action cannot displace established trigger-touch or haptics under action limit");
        check(f.input.Sync(360)&&core(f.input)&&f.input.Right().quick_access_touch&&f.input.Right().trigger_touch_valid,
              "action-limited runtime preserves existing default input");
        qa::SetFaceTouch(true);
        check(f.input.Sync(361)&&!f.input.Right().quick_access_touch&&qa::AvailableHands()==0&&core(f.input),
              "action-limited face mode remains unavailable without rebuilding or blocking controls");
    }
    reset();rt.rejectThumbrest=true;
    {
        Instance f;qa::SetFaceTouch(true);
        check(f.input.Sync(400)&&f.input.Left().quick_access_touch&&f.input.Right().quick_access_touch&&core(f.input),
              "face-touch mode works when runtime refuses thumbrest suggestions");
        check(contains(rt.acceptedBindings[touch],"quick_access_face_touch=/user/hand/right/input/b/touch")&&
              !contains(rt.acceptedBindings[touch],"quick_access_touch=/user/hand/right/input/thumbrest/touch"),
              "thumbrest refusal does not remove a separately accepted face pair");
    }
    reset();
    {
        kcd2vr::XrInput cold;qa::PublishAvailable(false,3);
        const auto calls=rt.apiCalls;
        check(!cold.Sync(0)&&qa::AvailableHands()==0&&rt.apiCalls==calls,"uninitialized sync clears presentation evidence without XR calls");
    }
    reset();
    {
        Instance f;auto& in=f.input;qa::SetFaceTouch(true);in.Sync(500);
        rt.syncResult=XR_SESSION_NOT_FOCUSED;
        check(!in.Sync(501)&&!in.Left().active&&!in.Right().active&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&qa::AvailableHands()==0,
              "focus loss neutralizes actual input and completed availability");
        rt.syncResult=XR_SUCCESS;
        check(in.Sync(502)&&qa::AvailableHands()==3&&in.Left().quick_access_touch&&rt.attaches==1,
              "focus return recovers selected sensors without reattach");
        rt.syncResult=XR_ERROR_RUNTIME_FAILURE;
        check(!in.Sync(503)&&qa::AvailableHands()==0&&!in.Right().b_down&&!in.Right().thumbstick_down,
              "failed sync clears modifier status and native button levels");
        rt.syncResult=XR_SUCCESS;in.Sync(504);const auto calls=rt.apiCalls;
        in.InvalidateFrameR105();
        check(qa::AvailableHands()==0&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&rt.apiCalls==calls,
              "manager invalidation clears evidence without lifetime mutation");
        check(in.Sync(505)&&qa::AvailableHands()==3,"manager invalidation recovers on next current sync");
        in.OnSessionStoppedR105();
        check(qa::AvailableHands()==0&&!in.Left().active&&!in.Right().quick_access_touch,
              "ended session clears sensor availability");
        check(in.Sync(506)&&qa::AvailableHands()==3&&rt.attaches==1,"same attached session actions resume without persistent input block");
        in.Shutdown();
        check(qa::AvailableHands()==0&&!in.Right().quick_access_touch&&rt.destroyedSpaces==4&&rt.destroyedSets==1&&qa::FaceTouch(),
              "shutdown clears evidence and action resources but preserves user preference");
        check(in.Initialize(1,4,5)&&in.Sync(507)&&qa::AvailableHands()==3&&rt.attaches==2,
              "new session recreates optional action and restores selected source");
    }
    reset();
    {
        Instance f;auto& in=f.input;in.Sync(600);rt.status.clear();rt.clock+=1100;
        rt.profileResult[0]=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(601)&&core(in),"profile query failure cannot suppress healthy input or tracking");
        check(status("left=QUERY_FAILED")&&status("resultL=-2")&&status("hardwarePresence=UNPROVEN")&&!status("asleep")&&!status("battery"),
              "failed profile query is unknown rather than claimed hardware loss");
        rt.status.clear();rt.clock+=1100;rt.profileResult[0]=XR_SUCCESS;rt.nullProfile[0]=true;
        check(in.Sync(602)&&core(in),"successful null profile is independent from valid hand poses");
        check(status("left=NONE")&&status("resultL=0")&&status("poseValidity=INDEPENDENT")&&!status("asleep")&&!status("battery"),
              "successful null profile is distinguished without hardware diagnosis");
        rt.status.clear();rt.clock+=1100;rt.nullProfile[0]=false;
        check(in.Sync(603)&&core(in)&&rt.attaches==1,"profile and left controller recovery need no action-set restart");
        check(status("left=/interaction_profiles/oculus/touch_controller")&&status("resultL=0"),
              "profile recovery is logged with successful query result");
        rt.status.clear();rt.clock+=1100;rt.selectedProfile=index;rt.nameFailure=true;
        check(in.Sync(604)&&core(in)&&status("NAME_UNAVAILABLE"),"path-name failure is diagnostic only and does not invent profile text");
    }
    reset();
    {
        Instance f;auto& in=f.input;qa::SetFaceTouch(true);in.Sync(700);
        rt.location("hand_pose",0).result=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(701)&&!in.Left().active&&in.Right().active&&in.Left().aim_active&&in.Right().b_valid,
              "lost left grip pose does not disable independently current right controls");
        check(in.Left().trigger_valid&&in.Left().grip_valid&&in.Left().quick_access_touch&&valid(in.Left()),
              "optional touch availability never grants missing left pose authority");
        XrPosef poses[2]{};XrSpaceLocationFlags flags[2]{15,15};
        check(in.TryLocateGripPair(702,poses,flags)&&flags[0]==0&&flags[1]==15,
              "pose-only publication keeps genuine left failure invalid");
        rt.location("hand_pose",0).result=XR_SUCCESS;rt.location("hand_pose",0).flags=3;
        check(in.Sync(703)&&in.Left().active&&in.Right().active&&in.Left().pose_flags==3&&rt.attaches==1,
              "left valid pose recovers without persistent ownership latch or tracked-bit requirement");
        rt.state("button_b",1).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("thumbstick_click",1).result=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(704)&&!in.Right().b_down&&!in.Right().b_pressed&&!in.Right().b_valid&&!in.Right().thumbstick_down&&!in.Right().thumbstick_pressed,
              "Back and kick read failures stay neutral even while face modifier is touched");
        check(!in.Right().quick_access_touch&&valid(in.Right())&&qa::AvailableHands()==3&&in.Right().active&&in.Left().active,
              "unknown B click releases face modifier without poisoning sensor availability or hands");
    }
    reset();
    {
        Instance f;auto& in=f.input;qa::SetFaceTouch(true);in.Sync(800);
        rt.reads.clear();rt.state("button_b",1).down=true;rt.state("button_y",0).down=true;
        check(in.Sync(801)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&valid(in.Left())&&valid(in.Right())&&qa::AvailableHands()==3,
              "face touch plus B or Y press suppresses only modifier level while sensor stays available");
        check(in.Right().b_down&&in.Right().b_pressed&&in.Right().b_valid&&in.Left().y_down&&in.Left().y_pressed&&
              reads("button_b")==1&&reads("button_y")==1,
              "press arbitration preserves existing click payloads and reuses one read per click");
        namespace pad=kcd2vr::native_vr_pad_premap;
        pad::Reset();pad::SetBQuickTalk45(true);pad::Context gameplay{};
        auto native=pad::Build(in.Left(),in.Right(),gameplay);
        check(native.rb&&native.y&&!native.b,"actual downstream BQuickTalk and Y admission survive simultaneous face sensor contact");
        pad::Reset();pad::Context ui{};ui.contextual_ui=true;
        native=pad::Build(in.Left(),in.Right(),ui);
        check(native.b&&native.y,"actual downstream contextual B and Y clicks retain native UI meanings");
        rt.state("button_b",1).down=false;rt.state("button_y",0).down=false;
        check(in.Sync(802)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&!in.Right().b_pressed&&!in.Left().y_pressed,
              "releasing B and Y restores face modifier on the next read without synthetic click");
        rt.state("button_y",0).result=XR_ERROR_RUNTIME_FAILURE;rt.state("button_b",1).active=false;
        check(in.Sync(803)&&!in.Left().quick_access_touch&&!in.Right().quick_access_touch&&qa::AvailableHands()==3,
              "unknown or inactive corresponding click refuses face modifier without hiding valid sensor");
        rt.state("button_y",0).result=XR_SUCCESS;rt.state("button_b",1).active=true;
        check(in.Sync(804)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&rt.attaches==1,
              "click-read recovery restores touch-only modifier without restart");
        qa::SetFaceTouch(false);rt.state("button_b",1).down=true;rt.state("button_y",0).down=true;
        check(in.Sync(805)&&in.Left().quick_access_touch&&in.Right().quick_access_touch&&in.Right().b_down&&in.Left().y_down,
              "default thumbrest keeps original simultaneous-click modifier semantics");
        pad::SetBQuickTalk45(false);
    }
    reset();
    {
        Instance f;auto& in=f.input;qa::SetFaceTouch(true);
        rt.state("button_x",0).down=false;rt.state("menu",0).down=false;
        rt.state("thumbstick",0).vec={0,0};rt.state("thumbstick",1).vec={0,0};
        rt.state("thumbstick_click",0).down=false;rt.state("thumbstick_click",0).changed=false;
        rt.state("thumbstick_click",1).down=false;rt.state("thumbstick_click",1).changed=false;
        rt.state("quick_access_face_touch",1).down=false;
        kcd2vr::TouchBindings touch;touch.SetEnabled(true);touch.SetNativeGamepadActions(true);
        touch.SetNativeGamepadAxes(true,true);touch.SetNativeSpecialRoute168(true);touch.SetRightGripFreeLookEnabled(false);
        auto step=[&](unsigned n){in.Sync(n);return touch.Update(in.Left(),in.Right(),false,false,false,false);};
        check(!step(900).special_attack_requested,"actual TouchBindings begins from neutral R3 without request");
        rt.state("thumbstick_click",1).down=true;rt.state("thumbstick_click",1).changed=true;
        check(step(901).special_attack_requested,"actual ordinary R3 requests special attack when face modifier is released");
        rt.state("thumbstick_click",1).changed=false;
        check(!step(902).special_attack_requested,"actual held R3 cannot repeat special request");
        rt.state("thumbstick_click",1).down=false;rt.state("quick_access_face_touch",1).down=true;rt.state("thumbstick",0).vec={-.8f,0};step(903);
        rt.state("thumbstick_click",1).down=true;
        check(!step(904).special_attack_requested,"actual R3 keeps existing active touch-plus-stick chord exclusion");
        rt.state("quick_access_face_touch",1).down=false;
        check(!step(905).special_attack_requested,"lifting modifier does not replay R3 held across exclusion");
        rt.state("thumbstick_click",1).down=false;step(906);rt.state("thumbstick_click",1).down=true;
        check(step(907).special_attack_requested,"fresh R3 after modifier release immediately recovers special request");
        rt.state("thumbstick_click",1).down=false;rt.state("thumbstick",0).vec={0,0};step(908);
        rt.state("quick_access_face_touch",1).down=true;rt.state("thumbstick_click",1).down=true;
        check(step(909).special_attack_requested,"actual R3 still works with touch alone and centered left stick");
        rt.state("thumbstick_click",1).down=false;step(910);
        rt.state("quick_access_face_touch",1).down=true;rt.state("button_b",1).down=false;
        rt.clock=10000;in.Sync(909);touch.UpdateBackEscapeM1(in.Right(),true,1);
        rt.clock=10100;rt.state("button_b",1).down=true;in.Sync(910);
        bool early=touch.UpdateBackEscapeM1(in.Right(),true,1),fired=false;
        for(unsigned tick=10200;tick<=12100;tick+=100){rt.clock=tick;in.Sync(911+tick);
            const bool result=touch.UpdateBackEscapeM1(in.Right(),true,1);if(tick<12100)early|=result;else fired=result;}
        check(!early&&fired&&touch.BackEscapeConsumedM1()&&!in.Right().quick_access_touch&&in.Right().b_down,
              "actual B recovery Escape keeps its two-second owned hold with face sensor touched");
        unsigned escapes=0;for(const auto& event:rt.osInputs)if(event.type==INPUT_KEYBOARD&&event.ki.wScan==27&&!(event.ki.dwFlags&KEYEVENTF_KEYUP))++escapes;
        check(escapes==1,"actual recovery emits one Escape through controlled Win32 output leaf");
    }
    // The full keyboard fallback owns a press/release transaction separately
    // from the native optional BQuickTalk path above. No timers/latches are added.
    for(unsigned scenario=0;scenario<6;++scenario){
        reset();Instance f;auto& in=f.input;
        rt.state("button_x",0).down=false;rt.state("menu",0).down=false;
        rt.state("thumbstick",0).vec={0,0};rt.state("thumbstick",1).vec={0,0};
        rt.state("thumbstick_click",0).down=rt.state("thumbstick_click",1).down=false;
        rt.state("thumbstick_click",0).changed=rt.state("thumbstick_click",1).changed=false;
        qa::SetFaceTouch(scenario!=2&&scenario!=4);
        // Scenario2 starts as a plain default-thumbrest B click; touching the
        // thumbrest at release must retain the existing consumption behavior.
        if(scenario==2)rt.state("quick_access_touch",1).down=false;
        kcd2vr::TouchBindings touch;touch.SetEnabled(true);touch.SetNativeGamepadAxes(true,true);
        if(scenario==5)touch.SetNativeGamepadActions(true);
        bool backContext=scenario==3;
        auto step=[&](unsigned n){in.Sync(n);return touch.Update(in.Left(),in.Right(),false,false,false,backContext);};
        step(1000);rt.state("button_b",1).down=true;step(1001);
        // Original press owner is authoritative: releasing another owner/mode
        // never manufactures eligibility in the branch under test.
        if(scenario==1)rt.state("thumbstick",0).vec={-.8f,0};
        if(scenario==2)rt.state("quick_access_touch",1).down=true;
        if(scenario==3)backContext=false;
        if(scenario==4)qa::SetFaceTouch(true);
        rt.state("button_b",1).down=false;step(1002);
        unsigned quickSpeak=0;for(const auto& event:rt.osInputs)if(event.type==INPUT_KEYBOARD&&event.ki.wScan=='Q'&&!(event.ki.dwFlags&KEYEVENTF_KEYUP))++quickSpeak;
        static const char* names[]={
            "actual fallback B release preserves eligible Quick Speak while face touch remains",
            "actual fallback active utility chord still consumes eligible B release",
            "actual fallback default thumbrest exclusion remains exact",
            "actual fallback prior back-context press cannot become gameplay Quick Speak",
            "actual fallback mode change cannot requalify an ineligible thumbrest-owned press",
            "actual fallback native XI owner cannot emit keyboard Quick Speak"};
        check(quickSpeak==(scenario==0?1u:0u),names[scenario]);
        step(1003);unsigned after=0;for(const auto& event:rt.osInputs)if(event.type==INPUT_KEYBOARD&&event.ki.wScan=='Q'&&!(event.ki.dwFlags&KEYEVENTF_KEYUP))++after;
        check(after==quickSpeak,"actual fallback release is consumed once without a repeat pulse");
    }
    reset();
    {
        Instance f;auto& in=f.input;qa::SetFaceTouch(true);
        rt.state("button_x",0).down=false;rt.state("menu",0).down=false;
        rt.state("thumbstick",0).vec={0,0};rt.state("thumbstick",1).vec={0,0};
        rt.state("thumbstick_click",0).down=rt.state("thumbstick_click",1).down=false;
        rt.state("thumbstick_click",0).changed=rt.state("thumbstick_click",1).changed=false;
        kcd2vr::TouchBindings touch;touch.SetEnabled(true);touch.SetNativeGamepadAxes(true,true);
        auto step=[&](unsigned tick){rt.clock=tick;in.Sync(tick);const bool recovery=touch.UpdateBackEscapeM1(in.Right(),true,1);
            touch.Update(in.Left(),in.Right(),false,false,false,false);return recovery;};
        step(20000);rt.state("button_b",1).down=true;step(20100);bool recovered=false;
        for(unsigned tick=20200;tick<=22100;tick+=100)recovered=step(tick)||recovered;
        rt.state("button_b",1).down=false;step(22200);
        unsigned quickSpeak=0,escapes=0;for(const auto& event:rt.osInputs)if(event.type==INPUT_KEYBOARD&&!(event.ki.dwFlags&KEYEVENTF_KEYUP)){
            quickSpeak+=event.ki.wScan=='Q';escapes+=event.ki.wScan==27;}
        check(recovered&&escapes==1&&quickSpeak==0,"actual fallback consumed two-second Escape cannot become Quick Speak on touch-retaining release");
    }
    qa::SetFaceTouch(false);qa::ClearAvailable();
    std::printf("{\"checks\":%u,\"failures\":%zu,\"failed_checks\":[",checks,failures.size());
    for(std::size_t i=0;i<failures.size();++i)std::printf("%s\"%s\"",i?",":"",failures[i].c_str());
    std::printf("]}\n");return failures.empty()?0:1;
}
