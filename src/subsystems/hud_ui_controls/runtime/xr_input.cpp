#include "../../../diagnostics/log_sink_d48.h"
#include "xr_input.h"
#include "quick_access_r112.h"
#include "vrik/claude_motion_d69.h"
#include "diagnostics/xr_timing165.h"
#include "diagnostic_trace.h"
#include <cstdio>
#include <DetourModKit/logger.hpp>
#include <array>
#include <vector>
#include <utility>
#include <cstdio>
#include <windows.h>
#include <cstdarg>
#include "core/haptics.h"

namespace kcd2vr {
using DetourModKit::log;
namespace {
// R102: input setup results always go to KCD2VR.log (not only while F2 records): without them a runtime that
// refuses our bindings (SteamVR vs VDXR) leaves no trace and the user just has no controls.
void InputStatusR102(const char* fmt,...) noexcept {
    char line[512]{};va_list a;va_start(a,fmt);std::vsnprintf(line,sizeof(line),fmt,a);va_end(a);
    diagnostic_trace::WriteStatus(line);
}
void TraceHapticR95(int side,const haptics101::Pulse& p,const char* stage,const char* reason,std::uint64_t now,int result) noexcept {
    namespace cap=diagnostics::capture168;
    if(!cap::gate.receipt(cap::Domain::Interaction,p.captureSessionR95,now))return;
    char line[480]{};
    std::snprintf(line,sizeof(line),"R95 HAPTIC_CUE session=%llu side=%c key=%llu epoch=%llu sequence=%llu cueTick=%llu observedTick=%llu phase=%s reason=%s amp=%.2f ms=%d xrResult=%d physicalDelivery=UNVERIFIED queueModel=LATEST_ONLY",
        (unsigned long long)p.captureSessionR95,side?'L':'R',(unsigned long long)p.key,(unsigned long long)p.epoch,
        (unsigned long long)p.sequence,(unsigned long long)p.tick,(unsigned long long)now,stage,reason,double(p.amplitude),p.durationMs,result);
    diagnostic_trace::WriteReceiptFor(cap::Domain::Interaction,p.captureSessionR95,line);
}
}


XrPath XrInput::Path(const char* value) const
{
    XrPath path = XR_NULL_PATH;
    if (XR_FAILED(xrStringToPath(instance_, value, &path))) return XR_NULL_PATH;
    return path;
}

void XrInput::InvalidateFrameR105()
{
    std::lock_guard<std::mutex> lock(action_space_mutex_);
    left_ = {}; right_ = {};
    quick_access_r112::ClearAvailable();
    recenter_requested_ = false;
    focused_ = false;
}

void XrInput::OnSessionStoppedR105()
{
    std::lock_guard<std::mutex> lock(action_space_mutex_);
    left_ = {}; right_ = {};
    quick_access_r112::ClearAvailable();
    recenter_requested_ = false;
    focused_ = false;
    // The session lifetime ends these outputs. Shutdown must not send haptic
    // commands after xrEndSession, while a later READY keeps attached actions.
    haptic_active_[0]=haptic_active_[1]=false;
}

bool XrInput::Initialize(XrInstance instance, XrSession session, XrSpace reference_space)
{
    std::lock_guard<std::mutex> lock(action_space_mutex_);
    instance_ = instance;
    session_ = session;
    reference_space_ = reference_space;
    left_path_ = Path("/user/hand/left");
    right_path_ = Path("/user/hand/right");
    if (left_path_ == XR_NULL_PATH || right_path_ == XR_NULL_PATH) return false;
    if (!CreateActions()) {InputStatusR102("R102 XR_INPUT create actions FAILED: no controller input");return false;}
    if (!SuggestBindings()) {InputStatusR102("R102 XR_INPUT no interaction profile accepted any binding: no controller input");return false;}
    if (!CreateSpaces()) {InputStatusR102("R102 XR_INPUT create hand spaces FAILED: no controller input");return false;}

    XrSessionActionSetsAttachInfo attach{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attach.countActionSets = 1;
    attach.actionSets = &action_set_;
    XrResult r = xrAttachSessionActionSets(session_, &attach);
    if (XR_FAILED(r)) {
        KCD2VR_DIAG_LOG(Bug,error,"OpenXR: xrAttachSessionActionSets failed, code={}", static_cast<int>(r));
        InputStatusR102("R102 XR_INPUT attach FAILED code=%d: no controller input this session",static_cast<int>(r));
        return false;
    }
    haptics101::traceR95.store(TraceHapticR95,std::memory_order_release);
    initialized_ = true;
    KCD2VR_DIAG_LOG(Bug,info,"OpenXR input initialized: grip + aim poses, capacitive quick-access touch, and left/right tracked controller actions are ready");
    return true;
}

bool XrInput::CreateActions()
{
    XrActionSetCreateInfo set{XR_TYPE_ACTION_SET_CREATE_INFO};
    std::snprintf(set.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "kcd2vr");
    std::snprintf(set.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "KCD2VR");
    set.priority = 0;
    if (XR_FAILED(xrCreateActionSet(instance_, &set, &action_set_))) return false;

    const XrPath subpaths[] = {left_path_, right_path_};
    auto create = [&](const char* name, const char* localized, XrActionType type, XrAction* out) {
        XrActionCreateInfo info{XR_TYPE_ACTION_CREATE_INFO};
        std::snprintf(info.actionName, XR_MAX_ACTION_NAME_SIZE, "%s", name);
        std::snprintf(info.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, "%s", localized);
        info.actionType = type;
        info.countSubactionPaths = 2;
        info.subactionPaths = subpaths;
        return xrCreateAction(action_set_, &info, out) == XR_SUCCESS;
    };

    const bool inputs=create("hand_pose", "Hand Pose", XR_ACTION_TYPE_POSE_INPUT, &pose_action_) &&
           create("aim_pose", "Aim Pose", XR_ACTION_TYPE_POSE_INPUT, &aim_pose_action_) &&
           create("trigger", "Trigger", XR_ACTION_TYPE_FLOAT_INPUT, &trigger_action_) &&
           create("grip", "Grip", XR_ACTION_TYPE_FLOAT_INPUT, &grip_action_) &&
           create("thumbstick", "Thumbstick", XR_ACTION_TYPE_VECTOR2F_INPUT, &thumbstick_action_) &&
           create("recenter", "Recenter", XR_ACTION_TYPE_BOOLEAN_INPUT, &recenter_action_) &&
           create("button_a", "A Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &a_action_) &&
           create("button_b", "B Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &b_action_) &&
           create("button_x", "X Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &x_action_) &&
           create("button_y", "Y Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &y_action_) &&
           create("menu", "Menu Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &menu_action_) &&
           create("thumbstick_click", "Thumbstick Click", XR_ACTION_TYPE_BOOLEAN_INPUT, &thumbstick_click_action_) &&
           create("quick_access_touch", "Quick Access Touch", XR_ACTION_TYPE_BOOLEAN_INPUT, &quick_access_touch_action_);
    // R59.16C5: optional capacitive trigger touch (pointing gesture). Its absence never disables other input.
    if(inputs&&!create("trigger_touch","Trigger Touch",XR_ACTION_TYPE_BOOLEAN_INPUT,&trigger_touch_action_))trigger_touch_action_=XR_NULL_HANDLE;
    if(inputs&&!create("feedback","Hand feedback",XR_ACTION_TYPE_VIBRATION_OUTPUT,&haptic_action_)){
        haptic_action_=XR_NULL_HANDLE;KCD2VR_DIAG_LOG(Bug,warning,"OpenXR haptics unavailable; tracking remains enabled");}
    // R112: add after established optional actions so runtime action limits
    // cannot displace trigger touch/haptics. Created before attach for live selection.
    if(inputs&&!create("quick_access_face_touch","Quick Access Face Touch",XR_ACTION_TYPE_BOOLEAN_INPUT,&quick_access_face_touch_action_r112_))
        quick_access_face_touch_action_r112_=XR_NULL_HANDLE;
    return inputs;
}

bool XrInput::SuggestBindings()
{
    // R102 (SteamVR: "no laser, no moving, no controls"): one xrSuggestInteractionProfileBindings call is all or
    // nothing - a single path the runtime does not know (SteamVR and Touch thumbrest/touch, trigger/touch, ...)
    // rejects the whole Touch profile and no action is ever bound. Now:
    //  1. Oculus Touch: the core set first; each optional pair is added only if the runtime accepts it; the final
    //     call always re-suggests exactly the accepted set (a failed call must not be the last word).
    //  2. Index, Vive, WMR and the KHR simple controller get their own suggestions, so a runtime that presents the
    //     controllers under another profile still binds poses, trigger, grip, stick and buttons.
    // Every result goes to KCD2VR.log once (not F2-gated).
    using B=XrActionSuggestedBinding;
    auto suggest=[&](XrPath profile,const std::vector<B>& b)->XrResult{
        if(profile==XR_NULL_PATH||b.empty())return XR_ERROR_PATH_INVALID;
        for(const auto& x:b)if(x.binding==XR_NULL_PATH||x.action==XR_NULL_HANDLE)return XR_ERROR_PATH_INVALID;
        XrInteractionProfileSuggestedBinding info{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        info.interactionProfile=profile;info.suggestedBindings=b.data();info.countSuggestedBindings=static_cast<uint32_t>(b.size());
        return xrSuggestInteractionProfileBindings(instance_,&info);
    };
    auto both=[&](XrAction act,const char* leaf)->std::vector<B>{
        char l[128]{},r[128]{};std::snprintf(l,sizeof(l),"/user/hand/left%s",leaf);std::snprintf(r,sizeof(r),"/user/hand/right%s",leaf);
        return {{act,Path(l)},{act,Path(r)}};
    };
    auto add=[](std::vector<B>& to,const std::vector<B>& more){to.insert(to.end(),more.begin(),more.end());};
    int accepted=0;

    // 1. Oculus Touch (Quest under VDXR, Oculus, SteamVR Link/Virtual Desktop/ALVR).
    {
        const XrPath touch=Path("/interaction_profiles/oculus/touch_controller");
        std::vector<B> minimal;add(minimal,both(pose_action_,"/input/grip/pose"));add(minimal,both(aim_pose_action_,"/input/aim/pose"));
        add(minimal,both(trigger_action_,"/input/trigger/value"));add(minimal,both(grip_action_,"/input/squeeze/value"));
        add(minimal,both(thumbstick_action_,"/input/thumbstick"));
        std::vector<B> core=minimal;add(core,both(thumbstick_click_action_,"/input/thumbstick/click"));
        core.push_back({x_action_,Path("/user/hand/left/input/x/click")});core.push_back({y_action_,Path("/user/hand/left/input/y/click")});
        core.push_back({menu_action_,Path("/user/hand/left/input/menu/click")});
        core.push_back({a_action_,Path("/user/hand/right/input/a/click")});core.push_back({b_action_,Path("/user/hand/right/input/b/click")});
        std::vector<B> set;const char* level="NONE";
        XrResult r=suggest(touch,core);
        if(XR_SUCCEEDED(r)){set=core;level="CORE";}
        else{const XrResult rm=suggest(touch,minimal);InputStatusR102("R102 XR_INPUT touch core refused code=%d, minimal set code=%d",static_cast<int>(r),static_cast<int>(rm));
            if(XR_SUCCEEDED(rm)){set=minimal;level="MINIMAL";}}
        int optionalOk=0,optionalRefused=0;
        if(!set.empty()){
            std::vector<std::vector<B>> optional{both(quick_access_touch_action_,"/input/thumbrest/touch")};
            if(trigger_touch_action_)optional.push_back(both(trigger_touch_action_,"/input/trigger/touch"));
            if(haptic_action_)optional.push_back(both(haptic_action_,"/output/haptic"));
            // Standard Touch positions; an emulating runtime chooses hardware.
            // Append after established options so this new optional pair cannot
            // displace trigger touch or haptics on a binding-limited runtime.
            if(quick_access_face_touch_action_r112_)optional.push_back({
                {quick_access_face_touch_action_r112_,Path("/user/hand/left/input/y/touch")},
                {quick_access_face_touch_action_r112_,Path("/user/hand/right/input/b/touch")}});
            for(const auto& o:optional){std::vector<B> t=set;add(t,o);if(XR_SUCCEEDED(suggest(touch,t))){set=t;++optionalOk;}else ++optionalRefused;}
            r=suggest(touch,set); // the last call for this profile is always the accepted set
            if(XR_SUCCEEDED(r))++accepted;else level="FINAL_REFUSED";
        }
        InputStatusR102("R102 XR_INPUT profile=oculus/touch_controller result=%s bindings=%u optionalAccepted=%d optionalRefused=%d code=%d",
            level,static_cast<unsigned>(set.size()),optionalOk,optionalRefused,static_cast<int>(r));
    }
    // 2. Other controller families (each independent; a refusal only means that family is not supported).
    auto other=[&](const char* name,std::vector<B> b,const std::vector<B>& optional=std::vector<B>{}){
        char p[128]{};std::snprintf(p,sizeof(p),"/interaction_profiles/%s",name);
        const XrPath profile=Path(p);XrResult r=suggest(profile,b);
        if(XR_SUCCEEDED(r)&&!optional.empty()){
            auto candidate=b;add(candidate,optional);
            if(XR_SUCCEEDED(suggest(profile,candidate)))b=std::move(candidate);
            r=suggest(profile,b); // optional refusal cannot erase accepted core controls
        }
        if(XR_SUCCEEDED(r))++accepted;
        InputStatusR102("R102 XR_INPUT profile=%s result=%s bindings=%u code=%d",name,XR_SUCCEEDED(r)?"OK":"REFUSED",static_cast<unsigned>(b.size()),static_cast<int>(r));
    };
    {   // Valve Index: left A/B act as X/Y; no app menu button.
        std::vector<B> b;add(b,both(pose_action_,"/input/grip/pose"));add(b,both(aim_pose_action_,"/input/aim/pose"));
        add(b,both(trigger_action_,"/input/trigger/value"));add(b,both(grip_action_,"/input/squeeze/value"));
        add(b,both(thumbstick_action_,"/input/thumbstick"));add(b,both(thumbstick_click_action_,"/input/thumbstick/click"));
        b.push_back({a_action_,Path("/user/hand/right/input/a/click")});b.push_back({b_action_,Path("/user/hand/right/input/b/click")});
        b.push_back({x_action_,Path("/user/hand/left/input/a/click")});b.push_back({y_action_,Path("/user/hand/left/input/b/click")});
        if(haptic_action_)add(b,both(haptic_action_,"/output/haptic"));
        other("valve/index_controller",b,quick_access_face_touch_action_r112_?both(quick_access_face_touch_action_r112_,"/input/b/touch"):std::vector<B>{});
    }
    {   // HTC Vive wand: trackpad as stick, menu left = menu, menu right = B.
        std::vector<B> b;add(b,both(pose_action_,"/input/grip/pose"));add(b,both(aim_pose_action_,"/input/aim/pose"));
        add(b,both(trigger_action_,"/input/trigger/value"));add(b,both(grip_action_,"/input/squeeze/click"));
        add(b,both(thumbstick_action_,"/input/trackpad"));add(b,both(thumbstick_click_action_,"/input/trackpad/click"));
        b.push_back({menu_action_,Path("/user/hand/left/input/menu/click")});b.push_back({b_action_,Path("/user/hand/right/input/menu/click")});
        if(haptic_action_)add(b,both(haptic_action_,"/output/haptic"));
        other("htc/vive_controller",b);
    }
    {   // Windows Mixed Reality: trackpad click = A / X.
        std::vector<B> b;add(b,both(pose_action_,"/input/grip/pose"));add(b,both(aim_pose_action_,"/input/aim/pose"));
        add(b,both(trigger_action_,"/input/trigger/value"));add(b,both(grip_action_,"/input/squeeze/click"));
        add(b,both(thumbstick_action_,"/input/thumbstick"));add(b,both(thumbstick_click_action_,"/input/thumbstick/click"));
        b.push_back({menu_action_,Path("/user/hand/left/input/menu/click")});b.push_back({b_action_,Path("/user/hand/right/input/menu/click")});
        b.push_back({a_action_,Path("/user/hand/right/input/trackpad/click")});b.push_back({x_action_,Path("/user/hand/left/input/trackpad/click")});
        if(haptic_action_)add(b,both(haptic_action_,"/output/haptic"));
        other("microsoft/motion_controller",b);
    }
    {   // KHR simple controller: poses, select = trigger, menu.
        std::vector<B> b;add(b,both(pose_action_,"/input/grip/pose"));add(b,both(aim_pose_action_,"/input/aim/pose"));
        add(b,both(trigger_action_,"/input/select/click"));
        b.push_back({menu_action_,Path("/user/hand/left/input/menu/click")});b.push_back({b_action_,Path("/user/hand/right/input/menu/click")});
        if(haptic_action_)add(b,both(haptic_action_,"/output/haptic"));
        other("khr/simple_controller",b);
    }
    return accepted>0;
}

bool XrInput::CreateActionSpace(XrAction action, XrPath subaction_path, XrSpace* out_space)
{
    XrActionSpaceCreateInfo info{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    info.action = action;
    info.subactionPath = subaction_path;
    info.poseInActionSpace.orientation.w = 1.0f;
    return xrCreateActionSpace(session_, &info, out_space) == XR_SUCCESS;
}

bool XrInput::CreateSpaces()
{
    return CreateActionSpace(pose_action_, left_path_, &left_space_) &&
           CreateActionSpace(pose_action_, right_path_, &right_space_) &&
           CreateActionSpace(aim_pose_action_, left_path_, &left_aim_space_) &&
           CreateActionSpace(aim_pose_action_, right_path_, &right_aim_space_);
}

bool XrInput::GetPose(XrSpace space, XrTime display_time, XrPose& out_pose, bool& active, XrSpaceLocationFlags* flags, XrSpaceVelocity* velocityD69)
{
    active = false; // V66.178R16: a failed locate must never leave the previous hand's flag in place
    if(flags)*flags=0;
    XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
    if(velocityD69){*velocityD69={XR_TYPE_SPACE_VELOCITY};location.next=velocityD69;}
    XrResult r = xrLocateSpace(space, reference_space_, display_time, &location);
    if (XR_FAILED(r)) return false;
    if(flags)*flags=location.locationFlags;
    active = (location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) &&
             (location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT);
    if (!active) return true;
    out_pose.pos_x = location.pose.position.x;
    out_pose.pos_y = location.pose.position.y;
    out_pose.pos_z = location.pose.position.z;
    out_pose.qx = location.pose.orientation.x;
    out_pose.qy = location.pose.orientation.y;
    out_pose.qz = location.pose.orientation.z;
    out_pose.qw = location.pose.orientation.w;
    return true;
}

bool XrInput::GetFloat(XrAction action, XrPath subaction_path, float& value, bool* active_out)
{
    value=0.0f; // A failed read cannot retain an older trigger/squeeze level.
    if(active_out)*active_out=false;
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateFloat state{XR_TYPE_ACTION_STATE_FLOAT};
    if (XR_FAILED(xrGetActionStateFloat(session_, &info, &state))) return false;
    value = state.isActive ? state.currentState : 0.0f;
    if(active_out)*active_out=state.isActive==XR_TRUE;
    return true;
}

bool XrInput::GetVec2(XrAction action, XrPath subaction_path, float& x, float& y)
{
    x=y=0.0f; // Failed/currently unavailable actions are neutral, not held input.
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateVector2f state{XR_TYPE_ACTION_STATE_VECTOR2F};
    if (XR_FAILED(xrGetActionStateVector2f(session_, &info, &state))) return false;
    x = state.isActive ? state.currentState.x : 0.0f;
    y = state.isActive ? state.currentState.y : 0.0f;
    return true;
}


bool XrInput::GetBoolState(XrAction action, XrPath subaction_path, bool& down, bool& pressed, bool* active_out)
{
    down=pressed=false; // Do not replay the preceding physical edge on read failure.
    if(active_out)*active_out=false;
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateBoolean state{XR_TYPE_ACTION_STATE_BOOLEAN};
    if (XR_FAILED(xrGetActionStateBoolean(session_, &info, &state))) return false;
    down = state.isActive && state.currentState;
    pressed = down && state.changedSinceLastSync;
    if(active_out)*active_out=state.isActive==XR_TRUE;
    return true;
}

bool XrInput::TryLocateGripPair(XrTime time, XrPosef (&poses)[2], XrSpaceLocationFlags (&flags)[2], XrSpaceVelocity* velocitiesD69)
{
    diagnostics::xr165::Timer timing(diagnostics::xr165::GripLocate);
    flags[0]=flags[1]=0;
    poses[0]=poses[1]={{0,0,0,1},{0,0,0}};
    if(velocitiesD69)velocitiesD69[0]=velocitiesD69[1]={XR_TYPE_SPACE_VELOCITY};
    std::unique_lock<std::mutex> lock(action_space_mutex_,std::try_to_lock);
    if(!lock.owns_lock()||!initialized_||time<=0)return false;
    if(!focused_)return true;
    const XrSpace spaces[2]{left_space_,right_space_};
    for(int side=0;side<2;++side){
        XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
        if(velocitiesD69){velocitiesD69[side]={XR_TYPE_SPACE_VELOCITY};location.next=&velocitiesD69[side];}
        if(spaces[side]!=XR_NULL_HANDLE&&XR_SUCCEEDED(xrLocateSpace(spaces[side],reference_space_,time,&location))){
            poses[side]=location.pose;flags[side]=location.locationFlags;
        }
    }
    return true;
}

bool XrInput::Sync(XrTime display_time)
{
    std::lock_guard<std::mutex> lock(action_space_mutex_);
    // Every publication belongs to this sync. A failed action read, loss of
    // focus or unavailable session must not inherit levels/edges from a prior
    // frame; healthy actions and hands below still populate independently.
    left_ = {}; right_ = {};
    // Keep the last completed UI-only availability during a healthy in-progress
    // sync. Publishing a transient zero here would flicker the menu status.
    recenter_requested_ = false;
    focused_ = false;
    if (!initialized_) {quick_access_r112::ClearAvailable();return false;}
    XrActiveActionSet active{action_set_, XR_NULL_PATH};
    XrActionsSyncInfo sync{XR_TYPE_ACTIONS_SYNC_INFO};
    sync.countActiveActionSets = 1;
    sync.activeActionSets = &active;
    XrResult syncResult;{diagnostics::xr165::Timer timing(diagnostics::xr165::ActionSync);syncResult=xrSyncActions(session_, &sync);}
    focused_=syncResult==XR_SUCCESS;
    if(!focused_){quick_access_r112::ClearAvailable();left_.active=right_.active=left_.aim_active=right_.aim_active=false;ServiceHaptics(false);return false;}
    {   // R112: profile selection is not hardware presence or pose validity.
        // A failed query is unknown, distinct from a successful XR_NULL_PATH.
        // Keep the existing once-per-second/change-only diagnostic cadence.
        const unsigned long long nowTick=GetTickCount64();
        if(nowTick>=profile_check_tick_r102_){
            profile_check_tick_r102_=nowTick+1000;
            const XrPath hands[2]{left_path_,right_path_};XrPath got[2]{XR_NULL_PATH,XR_NULL_PATH};
            XrResult result[2]{XR_SUCCESS,XR_SUCCESS};
            for(int i=0;i<2;++i){XrInteractionProfileState st{XR_TYPE_INTERACTION_PROFILE_STATE};
                result[i]=xrGetCurrentInteractionProfile(session_,hands[i],&st);
                if(XR_SUCCEEDED(result[i]))got[i]=st.interactionProfile;}
            if(got[0]!=bound_profile_r102_[0]||got[1]!=bound_profile_r102_[1]||
               result[0]!=profile_result_r112_[0]||result[1]!=profile_result_r112_[1]){
                bound_profile_r102_[0]=got[0];bound_profile_r102_[1]=got[1];++profile_reports_r102_;
                profile_result_r112_[0]=result[0];profile_result_r112_[1]=result[1];
                char name[2][XR_MAX_PATH_LENGTH]{};
                for(int i=0;i<2;++i){uint32_t n=0;
                    if(XR_FAILED(result[i]))std::snprintf(name[i],XR_MAX_PATH_LENGTH,"QUERY_FAILED");
                    else if(got[i]==XR_NULL_PATH)std::snprintf(name[i],XR_MAX_PATH_LENGTH,"NONE");
                    else if(XR_FAILED(xrPathToString(instance_,got[i],XR_MAX_PATH_LENGTH,&n,name[i])))
                        std::snprintf(name[i],XR_MAX_PATH_LENGTH,"NAME_UNAVAILABLE");}
                InputStatusR102("R102 XR_INPUT bound profile left=%s right=%s resultL=%d resultR=%d hardwarePresence=UNPROVEN poseValidity=INDEPENDENT",
                    name[0],name[1],static_cast<int>(result[0]),static_cast<int>(result[1]));
            }
        }
    }

    bool is_active = false;
    GetPose(left_space_, display_time, left_.pose, is_active,&left_.pose_flags,claude_motion_d69::ExtraActive()?&left_.velocityD69:nullptr); left_.active = is_active;
    GetPose(right_space_, display_time, right_.pose, is_active,&right_.pose_flags,claude_motion_d69::ExtraActive()?&right_.velocityD69:nullptr); right_.active = is_active;
    GetPose(left_aim_space_, display_time, left_.aim_pose, is_active); left_.aim_active = is_active;
    GetPose(right_aim_space_, display_time, right_.aim_pose, is_active); right_.aim_active = is_active;
    GetFloat(trigger_action_, left_path_, left_.trigger,&left_.trigger_valid);
    GetFloat(trigger_action_, right_path_, right_.trigger,&right_.trigger_valid);
    GetFloat(grip_action_, left_path_, left_.grip,&left_.grip_valid);
    GetFloat(grip_action_, right_path_, right_.grip,&right_.grip_valid);
    GetVec2(thumbstick_action_, left_path_, left_.thumb_x, left_.thumb_y);
    GetVec2(thumbstick_action_, right_path_, right_.thumb_x, right_.thumb_y);
    // V58: X/Y/Menu and stick-clicks are gameplay inputs now. Recenter is no
    // longer bound to X/Y; leave the explicit action available for a future
    // chord without stealing user buttons from the gameplay profile.
    recenter_requested_ = false;

    GetBoolState(a_action_, right_path_, right_.a_down, right_.a_pressed);
    { // Recovery cannot use an inactive action's false level as a neutral witness.
        bool bActive=false;
        right_.b_valid=GetBoolState(b_action_, right_path_, right_.b_down, right_.b_pressed,&bActive)&&bActive;
    }
    GetBoolState(x_action_, left_path_, left_.x_down, left_.x_pressed);
    bool yActiveR112=false;
    const bool yValidR112=GetBoolState(y_action_,left_path_,left_.y_down,left_.y_pressed,&yActiveR112)&&yActiveR112;
    GetBoolState(menu_action_, left_path_, left_.menu_down, left_.menu_pressed);
    GetBoolState(thumbstick_click_action_, left_path_, left_.thumbstick_down, left_.thumbstick_pressed);
    GetBoolState(thumbstick_click_action_, right_path_, right_.thumbstick_down, right_.thumbstick_pressed);
    // R112: explicit sensor selection, never guessed from a zero/released value.
    // Read only the selected optional action. Failure/inactivity releases the
    // modifier this frame; current reads recover directly without a latch.
    const int modifierModeR143=quick_access_r112::Mode();
    const bool faceTouchR112=modifierModeR143==quick_access_r112::FaceTouchMode;
    const bool leftThumbrestR143=modifierModeR143==quick_access_r112::LeftThumbrestMode;
    const XrAction modifierR112=faceTouchR112?quick_access_face_touch_action_r112_:quick_access_touch_action_;
    bool leftModifierActiveR143=false,leftModifierValidR143=false;
    if(modifierR112){bool ignoredPressed=false,active=false;
        left_.quick_access_touch_valid=GetBoolState(modifierR112,left_path_,left_.quick_access_touch,ignoredPressed,&active)&&active;
        right_.quick_access_touch_valid=GetBoolState(modifierR112,right_path_,right_.quick_access_touch,ignoredPressed,&active)&&active;
    }
    // Touch without press: the corresponding physical click retains its own
    // normal route, including optional B Quick Talk. Unknown click state cannot
    // authorize this modifier. Reuse the reads above; no added XR query or delay.
    if(faceTouchR112){
        left_.quick_access_touch=left_.quick_access_touch&&yValidR112&&!left_.y_down;
        right_.quick_access_touch=right_.quick_access_touch&&right_.b_valid&&!right_.b_down;
    }
    leftModifierActiveR143=left_.quick_access_touch;
    leftModifierValidR143=left_.quick_access_touch_valid;
    // The selected left sensor drives the common modifier state consumed by
    // the existing utility chord. Keep the physical left touch published too:
    // X-hold's independent visor shortcut still sees its actual hand sensor.
    if(leftThumbrestR143){
        right_.quick_access_touch=leftModifierActiveR143;
        right_.quick_access_touch_valid=leftModifierValidR143;
    }
    const unsigned availableR143=leftThumbrestR143?(leftModifierValidR143?1u:0u):
        faceTouchR112?((left_.quick_access_touch_valid?1u:0u)|(right_.quick_access_touch_valid?2u:0u)):
        (right_.quick_access_touch_valid?2u:0u);
    quick_access_r112::PublishAvailable(modifierModeR143,availableR143);
    // R59.16C5: trigger touch fails closed to 'finger on trigger' (no pointing) when unavailable/erroring.
    left_.trigger_touch=true; right_.trigger_touch=true;
    if(trigger_touch_action_){
        auto touch=[&](XrPath sub,bool& out,bool& valid){XrActionStateGetInfo gi{XR_TYPE_ACTION_STATE_GET_INFO};gi.action=trigger_touch_action_;gi.subactionPath=sub;
            XrActionStateBoolean st{XR_TYPE_ACTION_STATE_BOOLEAN};
            // An INACTIVE binding (runtime/controller without trigger touch) counts as 'finger on trigger'.
            if(XR_SUCCEEDED(xrGetActionStateBoolean(session_,&gi,&st))&&st.isActive){out=st.currentState!=XR_FALSE;valid=true;}};
        touch(left_path_,left_.trigger_touch,left_.trigger_touch_valid);
        touch(right_path_,right_.trigger_touch,right_.trigger_touch_valid);}
    ServiceHaptics(true);return true;
}

void XrInput::ServiceHaptics(bool focused)
{
    const auto now=std::uint64_t(GetTickCount64());
    for(int side=0;side<2;++side){
        const bool valid=focused&&(side?left_.active:right_.active)&&haptics101::enabled.load()&&haptics101::allowed.load();
        XrHapticActionInfo action{XR_TYPE_HAPTIC_ACTION_INFO};action.action=haptic_action_;action.subactionPath=side?left_path_:right_path_;
        if(!valid&&haptic_active_[side]&&haptic_action_&&session_){(void)xrStopHapticFeedback(session_,&action);haptic_active_[side]=false;}
        haptics101::Pulse p{};if(!haptics101::queue[side].read(p)||!p.sequence)continue;
        if(haptic_seen_[side]==p.sequence&&haptic_epoch_[side]==p.epoch)continue;
        haptic_seen_[side]=p.sequence;haptic_epoch_[side]=p.epoch;
        // Consume invalid/old requests too; focus/tracking recovery never replays them.
        const char* refusal=!focused?"FOCUS_LOST":!(side?left_.active:right_.active)?"HAND_UNTRACKED":
            !haptics101::enabled.load()?"HAPTICS_OFF":!haptics101::allowed.load()?"GAMEPLAY_POLICY":
            !valid?"INITIAL_AUTHORITY_REFUSED":
            !haptic_action_?"NO_HAPTIC_ACTION":!session_?"NO_XR_SESSION":
            p.epoch!=haptics101::epoch.load()?"MODE_EPOCH_CHANGED":now<p.tick?"FUTURE_CUE":
            now-p.tick>150?"CUE_EXPIRED":!haptics101::eligible(p,now)?"INVALID_CUE":nullptr;
        if(refusal){haptics101::tracePulseR95(side,p,now>=p.tick&&now-p.tick>150?"EXPIRED":"REJECTED",refusal,now);continue;}
        XrHapticVibration v{XR_TYPE_HAPTIC_VIBRATION};v.amplitude=p.amplitude;v.frequency=XR_FREQUENCY_UNSPECIFIED;v.duration=XrDuration(p.durationMs)*1000000;
        const XrResult hr=xrApplyHapticFeedback(session_,&action,reinterpret_cast<const XrHapticBaseHeader*>(&v));
        haptic_active_[side]=hr==XR_SUCCESS;
        haptics101::submitted.fetch_add(1,std::memory_order_relaxed);
        (hr==XR_SUCCESS?haptics101::succeeded:haptics101::rejected).fetch_add(1,std::memory_order_relaxed);
        haptics101::tracePulseR95(side,p,hr==XR_SUCCESS?"XR_SUBMITTED":"XR_REJECTED",
            hr==XR_SUCCESS?"XR_API_ACCEPTED":"XR_API_RESULT",now,int(hr));
    }
}
void XrInput::Shutdown()
{
    std::lock_guard<std::mutex> lock(action_space_mutex_);
    focused_=false;
    ServiceHaptics(false);
    initialized_ = false;
    if (left_space_) { xrDestroySpace(left_space_); left_space_ = XR_NULL_HANDLE; }
    if (right_space_) { xrDestroySpace(right_space_); right_space_ = XR_NULL_HANDLE; }
    if (left_aim_space_) { xrDestroySpace(left_aim_space_); left_aim_space_ = XR_NULL_HANDLE; }
    if (right_aim_space_) { xrDestroySpace(right_aim_space_); right_aim_space_ = XR_NULL_HANDLE; }
    if (action_set_) { xrDestroyActionSet(action_set_); action_set_ = XR_NULL_HANDLE; }
    haptic_action_=XR_NULL_HANDLE;haptic_seen_[0]=haptic_seen_[1]=haptic_epoch_[0]=haptic_epoch_[1]=0;
    pose_action_ = aim_pose_action_ = trigger_action_ = grip_action_ = thumbstick_action_ = recenter_action_ = a_action_ = b_action_ = XR_NULL_HANDLE;
    x_action_ = y_action_ = menu_action_ = thumbstick_click_action_ = quick_access_touch_action_ = quick_access_face_touch_action_r112_ = trigger_touch_action_ = XR_NULL_HANDLE;
    instance_ = XR_NULL_HANDLE;
    session_ = XR_NULL_HANDLE;
    reference_space_ = XR_NULL_HANDLE;
    left_ = {}; right_ = {};
    quick_access_r112::ClearAvailable();
    recenter_requested_ = false;
    bound_profile_r102_[0]=bound_profile_r102_[1]=XR_NULL_PATH;
    profile_result_r112_[0]=profile_result_r112_[1]=XR_SUCCESS;
    profile_check_tick_r102_=0;profile_reports_r102_=0;
}

} // namespace kcd2vr
