// Host fixture: production XrInput declarations and all method bodies are
// extracted unchanged by tools/check_xr_input_r105.py. OpenXR/Win32/log leaves
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
#include "core/haptics.h"
#include "vrik/claude_motion_d69.h"

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
    std::map<std::string,XrPath> pathIds;
    std::map<XrPath,std::string> pathNames;
    std::map<XrAction,std::string> actionNames;
    std::map<XrSpace,std::pair<std::string,int>> spaces;
    std::map<std::pair<std::string,int>,ActionState> states;
    std::map<std::pair<std::string,int>,SpaceState> locations;
    std::map<std::string,std::vector<std::string>> acceptedBindings;
    std::vector<std::string> status;
    XrPath path(const std::string& name){auto& p=pathIds[name];if(!p){p=++nextHandle;pathNames[p]=name;}return p;}
    int side(XrPath p){return pathNames.at(p)=="/user/hand/left"?0:1;}
    ActionState& state(const char* name,int side){return states[{name,side}];}
    SpaceState& location(const char* name,int side){return locations[{name,side}];}
} rt;
}
static std::uint64_t GetTickCount64(){return fixture::rt.clock;}
static XrResult xrStringToPath(XrInstance,const char* name,XrPath* path){++fixture::rt.apiCalls;*path=fixture::rt.path(name);return XR_SUCCESS;}
static XrResult xrCreateActionSet(XrInstance,const XrActionSetCreateInfo*,XrActionSet* out){auto& r=fixture::rt;++r.apiCalls;++r.createdSets;*out=++r.nextHandle;return XR_SUCCESS;}
static XrResult xrCreateAction(XrActionSet,const XrActionCreateInfo* info,XrAction* out){auto& r=fixture::rt;++r.apiCalls;*out=++r.nextHandle;r.actionNames[*out]=info->actionName;return XR_SUCCESS;}
static XrResult xrSuggestInteractionProfileBindings(XrInstance,const XrInteractionProfileSuggestedBinding* info){
    auto& r=fixture::rt;++r.apiCalls;const auto name=r.pathNames.at(info->interactionProfile);
    std::vector<std::string> bindings;
    for(unsigned i=0;i<info->countSuggestedBindings;++i){const auto& b=info->suggestedBindings[i];const auto& path=r.pathNames.at(b.binding);
        if(r.rejectOptionalTouch&&name=="/interaction_profiles/oculus/touch_controller"&&path.find("/touch")!=std::string::npos)return XR_ERROR_PATH_UNSUPPORTED;
        bindings.push_back(r.actionNames.at(b.action)+"="+path);}
    if(r.rejectAll)return XR_ERROR_PATH_UNSUPPORTED;
    r.acceptedBindings[name]=bindings;return XR_SUCCESS;
}
static XrResult xrCreateActionSpace(XrSession,const XrActionSpaceCreateInfo* info,XrSpace* out){auto& r=fixture::rt;++r.apiCalls;++r.createdSpaces;*out=++r.nextHandle;r.spaces[*out]={r.actionNames.at(info->action),r.side(info->subactionPath)};return XR_SUCCESS;}
static XrResult xrAttachSessionActionSets(XrSession,const XrSessionActionSetsAttachInfo*){++fixture::rt.apiCalls;++fixture::rt.attaches;return XR_SUCCESS;}
static XrResult xrSyncActions(XrSession,const XrActionsSyncInfo*){++fixture::rt.apiCalls;return fixture::rt.syncResult;}
static XrResult xrGetCurrentInteractionProfile(XrSession,XrPath,XrInteractionProfileState* out){auto& r=fixture::rt;++r.apiCalls;out->interactionProfile=r.profileNone?0:r.path("/interaction_profiles/oculus/touch_controller");return XR_SUCCESS;}
static XrResult xrPathToString(XrInstance,XrPath path,std::uint32_t cap,std::uint32_t* count,char* out){auto& r=fixture::rt;++r.apiCalls;const auto& name=r.pathNames.at(path);*count=static_cast<std::uint32_t>(name.size()+1);std::snprintf(out,cap,"%s",name.c_str());return XR_SUCCESS;}
static XrResult xrGetActionStateFloat(XrSession,const XrActionStateGetInfo* info,XrActionStateFloat* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.states[{r.actionNames.at(info->action),r.side(info->subactionPath)}];if(XR_FAILED(s.result))return s.result;out->currentState=s.value;out->isActive=s.active;return s.result;}
static XrResult xrGetActionStateVector2f(XrSession,const XrActionStateGetInfo* info,XrActionStateVector2f* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.states[{r.actionNames.at(info->action),r.side(info->subactionPath)}];if(XR_FAILED(s.result))return s.result;out->currentState=s.vec;out->isActive=s.active;return s.result;}
static XrResult xrGetActionStateBoolean(XrSession,const XrActionStateGetInfo* info,XrActionStateBoolean* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.states[{r.actionNames.at(info->action),r.side(info->subactionPath)}];if(XR_FAILED(s.result))return s.result;out->currentState=s.down;out->changedSinceLastSync=s.changed;out->isActive=s.active;return s.result;}
static XrResult xrLocateSpace(XrSpace space,XrSpace,XrTime,XrSpaceLocation* out){auto& r=fixture::rt;++r.apiCalls;auto& s=r.locations[r.spaces.at(space)];if(XR_FAILED(s.result))return s.result;out->locationFlags=s.flags;out->pose=s.pose;if(out->next){auto& v=*static_cast<XrSpaceVelocity*>(out->next);v.velocityFlags=3;v.linearVelocity={1,2,3};v.angularVelocity={4,5,6};}return s.result;}
static XrResult xrStopHapticFeedback(XrSession,const XrHapticActionInfo*){++fixture::rt.apiCalls;++fixture::rt.hapticStops;return XR_SUCCESS;}
static XrResult xrApplyHapticFeedback(XrSession,const XrHapticActionInfo*,const XrHapticBaseHeader*){++fixture::rt.apiCalls;++fixture::rt.hapticApplies;return XR_SUCCESS;}
static XrResult xrDestroySpace(XrSpace){++fixture::rt.apiCalls;++fixture::rt.destroyedSpaces;return XR_SUCCESS;}
static XrResult xrDestroyActionSet(XrActionSet){++fixture::rt.apiCalls;++fixture::rt.destroyedSets;return XR_SUCCESS;}

namespace DetourModKit {inline void log(){}}
#define KCD2VR_DIAG_LOG(...) ((void)0)
namespace kcd2vr::diagnostics::xr165 {enum Kind {GripLocate,ActionSync};struct Timer {explicit Timer(Kind){}};}
namespace kcd2vr::diagnostic_trace {
inline void WriteStatus(const char* text){fixture::rt.status.emplace_back(text);}
inline void WriteReceiptFor(diagnostics::capture168::Domain,std::uint64_t,const char*){}
}
#include "production_input_r105.inc"

namespace {
unsigned checks=0;std::vector<std::string> failures;
void check(bool ok,const char* name){++checks;if(!ok){failures.emplace_back(name);std::fprintf(stderr,"FAIL: %s\n",name);}}
bool zeroButtons(const kcd2vr::ControllerState& s){return !s.a_down&&!s.b_down&&!s.x_down&&!s.y_down&&!s.menu_down&&!s.thumbstick_down&&!s.a_pressed&&!s.b_pressed&&!s.x_pressed&&!s.y_pressed&&!s.menu_pressed&&!s.thumbstick_pressed;}
bool neutral(const kcd2vr::ControllerState& s){return !s.active&&!s.aim_active&&!s.pose_flags&&!s.velocityD69.velocityFlags&&s.trigger==0&&s.grip==0&&s.thumb_x==0&&s.thumb_y==0&&zeroButtons(s)&&!s.quick_access_touch&&s.trigger_touch&&!s.trigger_touch_valid&&!s.b_valid&&!s.grip_valid&&!s.trigger_valid;}
bool identity(const XrPosef& p){return p.orientation.x==0&&p.orientation.y==0&&p.orientation.z==0&&p.orientation.w==1&&p.position.x==0&&p.position.y==0&&p.position.z==0;}
struct Instance {
    kcd2vr::XrInput input;
    explicit Instance(bool optionalRefusal=false,bool none=false){fixture::rt={};fixture::rt.rejectOptionalTouch=optionalRefusal;fixture::rt.profileNone=none;check(input.Initialize(1,2,3),"initialize actual attached action set");}
    ~Instance(){input.Shutdown();}
};
}
int main(){
    using fixture::rt;
    {
        Instance f;auto& in=f.input;
        check(in.Sync(100),"healthy sync");
        check(in.Left().active&&in.Right().active&&in.Left().aim_active&&in.Right().aim_active,"healthy independent grip and aim poses");
        check(in.Left().trigger==.75f&&in.Right().grip==.75f&&in.Left().thumb_x==.35f&&in.Right().thumb_y==-.45f,"healthy analog action values");
        check(in.Right().a_pressed&&in.Right().b_pressed&&in.Left().menu_pressed&&in.Left().x_pressed&&in.Left().y_pressed,"healthy action edges");
        check(rt.attaches==1&&rt.createdSets==1&&rt.createdSpaces==4,"one initial attach and four spaces");
        rt.state("trigger",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("grip",1).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("thumbstick",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("button_a",1).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("button_b",1).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("button_x",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("button_y",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("menu",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("thumbstick_click",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("quick_access_touch",0).result=XR_ERROR_RUNTIME_FAILURE;
        rt.state("trigger_touch",0).result=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(101),"partial read failure retains successful sync");
        check(in.Left().trigger==0&&!in.Left().trigger_valid,"failed trigger clears old level");
        check(in.Right().grip==0&&!in.Right().grip_valid,"failed squeeze clears old level");
        check(in.Left().thumb_x==0&&in.Left().thumb_y==0,"failed stick clears both old axes");
        check(!in.Right().a_down&&!in.Right().a_pressed&&!in.Right().b_down&&!in.Right().b_pressed&&!in.Right().b_valid,"failed right buttons clear old levels and edges");
        check(zeroButtons(in.Left()),"failed left buttons clear old levels and edges");
        check(!in.Left().quick_access_touch&&in.Left().trigger_touch&&!in.Left().trigger_touch_valid,"optional failed touch uses safe defaults");
        check(in.Right().trigger==.75f&&in.Left().grip==.75f&&in.Right().thumb_x==.35f&&in.Right().thumbstick_pressed&&in.Left().active&&in.Right().active,"partial failure preserves healthy actions and hands");
        rt.states.clear();rt.state("button_a",1).changed=false;
        check(in.Sync(102)&&in.Left().trigger==.75f&&in.Right().grip==.75f&&in.Left().x_pressed,"individual action recovery on next sync");
        check(in.Right().a_down&&!in.Right().a_pressed,"producer does not synthesize held-button edge");
        rt.state("trigger",0).active=false;rt.state("thumbstick",0).active=false;rt.state("button_b",1).active=false;
        check(in.Sync(103)&&in.Left().trigger==0&&!in.Left().trigger_valid&&in.Left().thumb_x==0&&in.Left().thumb_y==0&&!in.Right().b_down&&!in.Right().b_pressed&&!in.Right().b_valid,"inactive action payloads are neutral");
        rt.states.clear();in.Sync(104);rt.syncResult=XR_SESSION_NOT_FOCUSED;
        check(!in.Sync(105),"not-focused result refuses current input");
        check(neutral(in.Left())&&neutral(in.Right()),"focus loss neutralizes full current state");
        XrPosef poses[2]{{{1,2,3,4},{5,6,7}},{{1,2,3,4},{5,6,7}}};XrSpaceLocationFlags flags[2]{15,15};
        XrSpaceVelocity v[2]{{XR_TYPE_SPACE_VELOCITY},{XR_TYPE_SPACE_VELOCITY}};v[0].velocityFlags=v[1].velocityFlags=3;
        check(in.TryLocateGripPair(106,poses,flags,v)&&!flags[0]&&!flags[1],"unfocused pose-only read returns no valid hand");
        check(identity(poses[0])&&identity(poses[1])&&!v[0].velocityFlags&&!v[1].velocityFlags,"unfocused pose-only output cannot retain old pose or velocity");
        rt.syncResult=XR_SUCCESS;
        check(in.Sync(107)&&in.Left().active&&in.Right().active&&in.Left().trigger==.75f&&rt.attaches==1&&rt.createdSets==1,"focus recovery uses existing actions without event or reattach");
        rt.syncResult=XR_ERROR_RUNTIME_FAILURE;check(!in.Sync(108)&&neutral(in.Left())&&neutral(in.Right()),"failed sync neutralizes full current state");
        rt.syncResult=XR_SUCCESS;rt.location("hand_pose",0).result=XR_ERROR_RUNTIME_FAILURE;rt.location("aim_pose",1).result=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(109)&&!in.Left().active&&in.Right().active&&in.Left().aim_active&&!in.Right().aim_active,"single grip and aim failures remain independent");
        check(in.Left().trigger==.75f&&in.Right().b_down,"pose failures do not erase real action input");
        rt.location("hand_pose",0).result=XR_SUCCESS;rt.location("hand_pose",0).flags=3;
        check(in.Sync(110)&&in.Left().active&&in.Left().pose_flags==3,"valid inferred pose accepted without tracked bits");
        poses[0]=poses[1]={{1,2,3,4},{5,6,7}};flags[0]=flags[1]=15;v[0].velocityFlags=v[1].velocityFlags=3;
        check(!in.TryLocateGripPair(0,poses,flags,v)&&!flags[0]&&!flags[1],"invalid pose-only time refuses validity");
        check(identity(poses[0])&&identity(poses[1])&&!v[0].velocityFlags&&!v[1].velocityFlags,"invalid pose-only time clears payloads");
#if HAS_INVALIDATE_R105
        rt.locations.clear();in.Sync(111);const auto apiBefore=rt.apiCalls;
        in.InvalidateFrameR105();
        check(neutral(in.Left())&&neutral(in.Right())&&!in.RecenterRequested(),"manager invalidation neutralizes current state");
        check(rt.apiCalls==apiBefore&&rt.attaches==1&&rt.destroyedSets==0&&rt.destroyedSpaces==0,"invalidation makes no XR calls or action lifetime changes");
        rt.state("trigger",0).result=XR_ERROR_RUNTIME_FAILURE;rt.location("hand_pose",1).result=XR_ERROR_RUNTIME_FAILURE;
        check(in.Sync(112)&&in.Left().active&&!in.Right().active&&in.Left().trigger==0&&in.Right().trigger==.75f&&in.Right().b_down&&rt.attaches==1,"invalidation recovers next sync with independent partial failures");
        rt.states.clear();rt.locations.clear();
        check(in.Sync(113)&&in.Left().active&&in.Right().active&&in.Left().trigger==.75f&&rt.attaches==1,"all input recovers without session recreation");
#endif
        rt.location("hand_pose",0).result=XR_ERROR_RUNTIME_FAILURE;
        poses[0]=poses[1]={{1,2,3,4},{5,6,7}};flags[0]=flags[1]=15;
        check(in.TryLocateGripPair(114,poses,flags)&&!flags[0]&&flags[1]==15,"pose-only read preserves independent healthy hand");
        check(identity(poses[0])&&poses[1].position.x==1,"failed pose-only hand has neutral payload");
        const auto logs=rt.status.size();in.Shutdown();
        check(neutral(in.Left())&&neutral(in.Right())&&rt.destroyedSpaces==4&&rt.destroyedSets==1,"shutdown releases existing actions and neutralizes state");
        rt.locations.clear();check(in.Initialize(1,4,5)&&in.Sync(115),"new session can initialize and sync");
        bool rebound=false;for(std::size_t i=logs;i<rt.status.size();++i)rebound|=rt.status[i].find("bound profile left=")!=std::string::npos;
        check(rebound,"new session resets bound-profile diagnostic sampling");
    }
    {
        Instance f(true,true);
        check(rt.acceptedBindings.count("/interaction_profiles/oculus/touch_controller")==1,"optional refusal keeps accepted core Touch profile");
        const auto& touch=rt.acceptedBindings.at("/interaction_profiles/oculus/touch_controller");
        bool core=false,optional=false;for(const auto& path:touch){core|=path=="trigger=/user/hand/right/input/trigger/value";optional|=path.find("/touch")!=std::string::npos;}
        check(core&&!optional,"final Touch suggestion contains only accepted paths");
        check(rt.acceptedBindings.size()==5,"Touch Index Vive WMR simple suggestions retained");
        check(f.input.Sync(120),"initially unbound profile does not prevent later synchronization");
        rt.profileNone=false;rt.clock+=1000;
        check(f.input.Sync(121)&&f.input.Right().active&&rt.attaches==1,"controller wake uses next sync without reattach");
    }
#if HAS_SESSION_STOP_R105
    {
        Instance f;auto& in=f.input;
        kcd2vr::haptics101::enabled.store(true);kcd2vr::haptics101::allowed.store(true);
        check(kcd2vr::haptics101::publish_event(0,1,.5f,50,rt.clock),"real haptic mailbox accepts current cue");
        check(in.Sync(130)&&rt.hapticApplies==1,"current focused pulse reaches runtime callback");
        auto before=rt.apiCalls;in.InvalidateFrameR105();
        check(rt.apiCalls==before&&rt.hapticStops==0,"ordinary invalidation does not issue haptic command");
        rt.syncResult=XR_SESSION_NOT_FOCUSED;
        check(!in.Sync(131)&&rt.hapticStops==1,"ordinary focus loss still stops previously active pulse");
        rt.syncResult=XR_SUCCESS;kcd2vr::haptics101::publish_event(0,2,.5f,50,rt.clock);
        check(in.Sync(132)&&rt.hapticApplies==2,"fresh focused pulse after focus recovery");
        before=rt.apiCalls;in.OnSessionStoppedR105();
        check(neutral(in.Left())&&neutral(in.Right())&&rt.apiCalls==before,"session stop only retires local state");
        in.Shutdown();check(rt.hapticStops==1,"shutdown after ended session issues no haptic stop");
        check(in.Initialize(1,9,10),"new session initializes after stopped shutdown");
        kcd2vr::haptics101::publish_event(0,3,.5f,50,rt.clock);
        check(in.Sync(133)&&rt.hapticApplies==3,"fresh pulse works in subsequent session");
        before=rt.attaches;in.OnSessionStoppedR105();
        kcd2vr::haptics101::publish_event(0,4,.5f,50,rt.clock);
        check(in.Sync(134)&&rt.hapticApplies==4&&rt.attaches==before,"same session READY can submit fresh pulse without reattach");
        in.OnSessionStoppedR105();
        kcd2vr::haptics101::enabled.store(false);kcd2vr::haptics101::allowed.store(false);
    }
#endif
    std::printf("{\"checks\":%u,\"failures\":%zu,\"has_invalidate\":%s,\"failed_checks\":[",checks,failures.size(),HAS_INVALIDATE_R105?"true":"false");
    for(std::size_t i=0;i<failures.size();++i)std::printf("%s\"%s\"",i?",":"",failures[i].c_str());
    std::puts("]}");return failures.empty()?0:1;
}
