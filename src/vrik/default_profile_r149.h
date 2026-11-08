#pragma once
// R149 (8 Nov, many players: "VRIK is not on by default"): R111 switched VRIK on at game start / save load ONLY with a
// saved F8 T-pose ([VRIK.ArmCalibration]). a new player has none, so VRIK stayed off until F8 or the menu VRIK button.
// now, with VrVrikOnStartR111 ON and NO saved calibration, the start uses a DEFAULT arm profile built from the eye
// height (headset above the floor when it reads like a standing player, else 1.65 m). it is only kept in memory: it is
// never written to KCD2VR.ini, the F8 T-pose replaces it (and is saved as before). players WITH a saved calibration
// take exactly the old path (source()==Saved, the default is never built).
// what the profile is (calibration_profile.h Arms, ARMS2 in the INI): span = controller to controller in the T-pose,
// leftReach/rightReach = head to each controller, headToHandPlane = how far the hands are below the eyes, offsets =
// head -> controller in the model frame (x = right, y = forward, z = up). note: the live arm solver does not read these
// numbers (it maps the tracked controllers 1:1 relative to the head); the profile is the "calibrated" state that lets
// the session re-anchor run (body under the headset, Henry's head anchor, yaw frame). so a default works for everybody.
#include "calibration_profile.h"
#include <cmath>
namespace kcd2vr::default_profile_r149 {
namespace cp=calibration_profile;
constexpr float kDefaultEyeM=1.65f;   // eye height used until a standing height is measured
constexpr float kMinStandEyeM=1.35f;  // a measured eye height below this is a seated/kneeling player or a wrong floor
constexpr float kMaxStandEyeM=2.05f;
// ratios to the eye height, from his real T-pose (ARMS2 1.575 span, hands 0.15 m below and ~0.17 m behind the eyes,
// for a ~1.65 m eye height; his saved StandingEyeHeightM 1.11 is a wrong floor, not his height) and standard body
// ratios (eye ~0.936 x stature, controller-to-controller span ~ stature - 2 x palm-to-fingertip ~0.95 x eye).
constexpr double kSpanPerEye=.95,kBelowPerEye=.09,kBehindPerEye=.10;

enum class Source : unsigned {Saved,Default,None};
// saved F8 calibration -> Saved (old path, nothing changes); none -> Default (built now); an F8 capture running -> None
inline Source source(bool savedCalibration,bool captureRunning) noexcept {
    if(captureRunning)return Source::None;
    return savedCalibration?Source::Saved:Source::Default;
}
inline bool measured(bool valid,float eye) noexcept {return valid&&std::isfinite(eye)&&eye>=kMinStandEyeM&&eye<=kMaxStandEyeM;}
inline float eyeHeight(bool valid,float eye) noexcept {return measured(valid,eye)?eye:kDefaultEyeM;}
inline cp::Arms build(float eye) noexcept {
    if(!std::isfinite(eye)||eye<kMinStandEyeM||eye>kMaxStandEyeM)eye=kDefaultEyeM;
    const double e=eye,half=kSpanPerEye*e*.5,fwd=-kBehindPerEye*e,up=-kBelowPerEye*e;
    cp::Arms a{};
    a.leftOffset={-half,fwd,up};a.rightOffset={half,fwd,up};a.measuredOffsets=true;
    a.span=pose_core::length(a.rightOffset-a.leftOffset);
    a.leftReach=pose_core::length(a.leftOffset);a.rightReach=pose_core::length(a.rightOffset);
    a.headToHandPlane=-(a.leftOffset.z+a.rightOffset.z)*.5;
    return a;
}
}
