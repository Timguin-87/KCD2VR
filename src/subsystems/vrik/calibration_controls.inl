void SetTrackingOriginProbeH46(kcd2vr::tracking125::OriginReaderH46 probe) noexcept {
    g_tracking_origin_probe_h46.store(probe,std::memory_order_release);
    if(probe&&g_saved_arm_ready66100.load(std::memory_order_acquire))g_cal_reanchor66100.store(true,std::memory_order_release);
}
void SetEyeHeightAboveFloor(bool valid,float meters) noexcept {
    g_eye_floor_valid107.store(valid&&std::isfinite(meters)&&meters>.3f&&meters<2.5f,std::memory_order_release);
    if(valid&&std::isfinite(meters))g_eye_floor_m107.store(meters,std::memory_order_release);
}
// C43 calibration restore (vrik/calibration_restore43.h)
void SetCalibrationAutoRestore43(bool on) noexcept {g_cal_auto_restore43.store(on,std::memory_order_release);}
bool CalibrationAutoRestore43() noexcept {return g_cal_auto_restore43.load(std::memory_order_acquire);}
void ForceCalibrationRestore43() noexcept {g_cal_restore_force43.store(true,std::memory_order_release);}
void SetViewPositionProbe43(bool(*probe)(float&,float&,float&,std::uint64_t)) noexcept {g_view_position_probe43.store(probe,std::memory_order_release);}
// R149: true while the arms come from the in-memory DEFAULT profile (no saved F8 T-pose). never saved; F8 clears it.
static std::atomic<bool> g_arm_default_r149{false};
// R149: a default profile does not count as a saved calibration here, so the menu VRIK button still guides a T-pose
// and RESTORE still says "do a T-pose once" (old behaviour for players without a saved F8). saved players: unchanged.
bool CalibrationStored43() noexcept {return !g_arm_capture_active66100.load()&&g_saved_arm_ready66100.load(std::memory_order_acquire)&&!g_arm_default_r149.load(std::memory_order_acquire);}
bool CalibrationDefaultR149() noexcept {return g_arm_default_r149.load(std::memory_order_acquire);}
// R149 (default_profile_r149.h): no saved F8 profile -> publish a default one built from the eye height, in memory only
// (g_arm_save_generation66100 is NOT bumped, so ServiceCalibrationIo66100 never writes it). the session re-anchor then
// runs like after an INI load (body under the headset, Henry's live head as anchor). a saved profile is never touched.
bool InstallDefaultCalibrationR149() noexcept {
    namespace dp=kcd2vr::default_profile_r149;
    if(g_arm_capture_active66100.load())return false;            // F8 running: its result decides
    if(g_saved_arm_ready66100.load(std::memory_order_acquire))return true; // saved F8 profile or the default already in place
    const bool valid=g_eye_floor_valid107.load(std::memory_order_acquire);const float live=g_eye_floor_m107.load(std::memory_order_acquire);
    const float eye=dp::eyeHeight(valid,live);const auto profile=dp::build(eye);
    if(!kcd2vr::calibration_profile::valid(profile)||!g_arm_profile66100.publish(profile))return false; // retried by the caller (R111 every 2 s)
    g_arm_default_r149.store(true,std::memory_order_release);g_saved_arm_ready66100.store(true,std::memory_order_release);g_cal_reanchor66100.store(true,std::memory_order_release);
    char m[320]{};std::snprintf(m,sizeof(m),"R149 VRIK_DEFAULT_PROFILE eyeHeight=%.3fm source=%s span=%.3fm reach=%.3fm handsBelowEyes=%.3fm notSaved=1 (press F8 for exact arms)",
        double(eye),dp::measured(valid,live)?"HEADSET_STANDING":"DEFAULT_1.65",profile.span,profile.leftReach,profile.headToHandPlane);
    diagnostic_trace::WriteStatus(m); // KCD2VR.log only
    return true;
}
float StandingEyeShift43() noexcept {
    // R111: VrStartAtHenryEyesR111 ON = every recenter takes the headset where it is now as Henry's eyes (no standing
    // correction; seated start = Henry's eye height, head moves relative from there). OFF = the C43 standing correction.
    return kcd2vr::start_r111::recenterShift(kcd2vr::start_r111::henryEyes.load(std::memory_order_acquire),kcd2vr::calibration_restore43::verticalShift(g_cal_auto_restore43.load(std::memory_order_acquire)||g_cal_restore_force43.load(std::memory_order_acquire),
        g_eye_floor_valid107.load(std::memory_order_acquire),g_eye_floor_m107.load(std::memory_order_acquire),g_standing_eye_floor_m107.load(std::memory_order_acquire)));
}
bool RestoreCalibration43(const char* owner) noexcept {
    // Edge action only (owner exits, mode on, menu). The calibrated head anchor is re-asserted; a live anchor that
    // differs (the re-seat/lazy latch took a stance, knockdown or activity pose) is reported as drift.
    static kcd2vr::calibration_restore43::Debounce debounce{};
    const auto now=std::uint64_t(GetTickCount64());
    if(!kcd2vr::calibration_restore43::admit(debounce,now))return false;
    const bool auto43=g_cal_auto_restore43.load(std::memory_order_acquire)||g_cal_restore_force43.load(std::memory_order_acquire);
    if(!auto43||!g_cal_anchor_saved43.load(std::memory_order_acquire)||g_arm_capture_active66100.load()||g_cal_state.load(std::memory_order_acquire)!=2)return false;
    const float cal[3]{g_cal_anchor_x43.load(),g_cal_anchor_y43.load(),g_cal_anchor_z43.load()};
    const float eff[3]{g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
    const float live[3]{g_latest_native_head_x.load(),g_latest_native_head_y.load(),g_latest_native_head_z.load()};
    const float drift=kcd2vr::calibration_restore43::anchorDriftM(cal,eff);
    const float liveDrift=g_latest_native_head_valid.load(std::memory_order_acquire)?kcd2vr::calibration_restore43::anchorDriftM(cal,live):0.f;
    // The effective anchor is what the hands/goals use; the live native head is Henry's current pose (stance/knockdown).
    if(drift>=kcd2vr::calibration_restore43::kDriftReportM){g_cal_drift_count43.fetch_add(1,std::memory_order_relaxed);char dm[256]{};
        std::snprintf(dm,sizeof(dm),"C43 CALIBRATION drift detected owner=%s effectiveHead=(%.3f,%.3f,%.3f) calibrated=(%.3f,%.3f,%.3f) driftM=%.3f liveHeadZ=%.3f",owner?owner:"?",double(eff[0]),double(eff[1]),double(eff[2]),double(cal[0]),double(cal[1]),double(cal[2]),double(drift),double(live[2]));diagnostic_trace::Write(dm);}
    // Rejoin the existing camera reference after an authored owner returns.
    // No view recenter and no saved anthropometry/finger changes.
    g_cal_reanchor66100.store(true,std::memory_order_release);
    g_cal_native_head_x.store(cal[0]);g_cal_native_head_y.store(cal[1]);g_cal_native_head_z.store(cal[2]);g_cal_native_anchor_pending.store(false,std::memory_order_release);
    g_cal_restore_count43.fetch_add(1,std::memory_order_relaxed);
    float camX=0,camY=0,camZ=0;const auto probe=g_view_position_probe43.load(std::memory_order_acquire);const bool cam=probe&&probe(camX,camY,camZ,100);
    kcd2vr::dialogue_state59c10r2::NativeHead head{};const bool haveHead=NativeDialogueHead59c10r2(head,now);
    const float viewAbove=cam&&haveHead?camZ-head.world.z:0.f;
    char rm[320]{};std::snprintf(rm,sizeof(rm),"C43 CALIBRATION restored by=%s eyeHeight=%.3f headOffset=(%.3f,%.3f,%.3f) driftM=%.3f liveStanceDriftM=%.3f viewAboveHeadM=%s%.3f restores=%llu drifts=%llu",
        owner?owner:"?",double(g_standing_eye_floor_m107.load()),double(cal[0]),double(cal[1]),double(cal[2]),double(drift),double(liveDrift),cam&&haveHead?"":"n/a ",double(viewAbove),
        (unsigned long long)g_cal_restore_count43.load(std::memory_order_relaxed),(unsigned long long)g_cal_drift_count43.load(std::memory_order_relaxed));diagnostic_trace::Write(rm);
    return true;
}
void RequestTrackingReanchor() noexcept {
    g_pose_epoch6689.fetch_add(1,std::memory_order_acq_rel);g_committed_pose_valid6689=false;
    kcd2vr::kick34::ledger.retire(kcd2vr::kick34::Why::EpochChanged); // C37
    g_cal_center_reanchor125=true;
    if(g_saved_arm_ready66100.load())g_cal_reanchor66100=true;
    else { // R59.16C10: the camera recentered but the VRIK body cannot re-seat without a saved arm profile - say so once per request
        diagnostic_trace::Write("R59.16C10 REANCHOR_SKIPPED cameraRecenter=1 bodyReanchor=0 reason=NO_SAVED_ARM_PROFILE (F8 once; hands/head keep the old tracking base)");
    }
}
void BeginArmLengthCalibration() noexcept {
    SetWeaponControlMode(0);SetCombatCensusMode(0);ResetCalibration();
    g_arm_capture_active66100=true;g_cal_reanchor66100=false;
    // R59.10C1: prepare the bounded alias catalog during the five-second T-pose
    // workflow. First activation must not start a second, invisible warm-up.
    // The worker remains its sole writer; unknown/protected clips still yield.
    while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){}
    g_arm_window66100={};g_guided_cal_lock.clear(std::memory_order_release);
    g_cal_samples_ok_r132.store(0);g_cal_samples_blocked_r132.store(0);g_cal_samples_lost_r132.store(0); // R132
}
void CancelArmLengthCalibration() noexcept {
    g_guided_cal_pose=0;g_vrik_calibration_requested=false;g_arm_capture_active66100=false;
    g_cal_state=0;g_cal_reanchor66100=g_saved_arm_ready66100.load();
    diagnostic_trace::Write("V66.100-CAL CANCELLED previousSavedReachAndFingersRetained=1");
}
bool CompleteArmLengthCalibration() noexcept {
    // The menu/worker owns finalization; measurements use the current 2s mean.
    g_guided_cal_pose=0;
    GuidedCalAccum a{};kcd2vr::calibration_profile::Window w{};
    while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){}
    a=g_guided_cal_accum;w=g_arm_window66100;g_guided_pose_result[1]=a;
    g_guided_cal_lock.clear(std::memory_order_release);
    kcd2vr::calibration_profile::Arms profile{};
    if(!w.measure(profile,kcd2vr::tracking125::yaw_only({a.hqSum.x,a.hqSum.y,a.hqSum.z,a.hqSum.w}))||a.n!=w.count||!a.haveQ){
        { // R132 (7 Nov second log: three F8 captures ended ALL_VRIK_OFF with no reason in KCD2VR.log): say why, once per capture.
          // the old V66.100-CAL REJECTED row below only reaches the log while a recorder runs.
          const unsigned ok=g_cal_samples_ok_r132.load(),blocked=g_cal_samples_blocked_r132.load(),lost=g_cal_samples_lost_r132.load();
          const char* why=blocked>ok?"the game had control (tracking not allowed: scripted scene, menu, dialogue or cutscene)":
              lost>ok?"headset or a controller not tracked":
              !w.count?"no tracked T-pose sample (arms not straight out to the sides at shoulder height)":
              "you moved or the arms were not a T-pose for the full 2 s (hold still, arms straight out)";
          char m[384]{};std::snprintf(m,sizeof(m),"R132 F8_CALIBRATION failed: %s - samples ok=%u blocked=%u lost=%u tposeSamples=%u; old calibration kept, press F8 again",
              why,ok,blocked,lost,w.count);
          diagnostic_trace::WriteStatus(m);} // KCD2VR.log only, no headset text
        CancelArmLengthCalibration();diagnostic_trace::Write("V66.100-CAL REJECTED needTrackedStableTPose=1 oldProfileRetained=1 autoRetry=0");return false;}
    const V3 nativeHead{g_latest_native_head_x.load(),g_latest_native_head_y.load(),g_latest_native_head_z.load()};
    if(!CommitGuidedCalibrationV6562(a,nativeHead)||!g_arm_profile66100.publish(profile)){diagnostic_trace::WriteStatus("R132 F8_CALIBRATION failed: T-pose measured but Henry's pose could not be matched (commit refused); old calibration kept, press F8 again");CancelArmLengthCalibration();return false;}
    g_cal_native_anchor_pending=!g_latest_native_head_valid.load();g_saved_arm_ready66100=true;g_arm_capture_active66100=false;g_cal_reanchor66100=false;
    if(g_arm_default_r149.exchange(false,std::memory_order_acq_rel))diagnostic_trace::WriteStatus("R149 VRIK_DEFAULT_PROFILE replaced by your F8 T-pose (saved to KCD2VR.ini)"); // R149
    g_arm_save_generation66100.fetch_add(1,std::memory_order_release);
    // C43: Henry's head at the T-pose is part of the calibration (persisted; re-seats keep it instead of the live stance head).
    {const float a43[3]{nativeHead.x,nativeHead.y,nativeHead.z};
     if(g_latest_native_head_valid.load()&&kcd2vr::calibration_restore43::anchorPlausible(a43)){g_cal_anchor_x43=a43[0];g_cal_anchor_y43=a43[1];g_cal_anchor_z43=a43[2];g_cal_anchor_saved43.store(true,std::memory_order_release);g_cal_anchor_save43.store(true,std::memory_order_release);}
     else g_cal_anchor_saved43.store(false,std::memory_order_release);} // the lazy latch (first arm write) fills it
    // V66.107: the F8 T-pose is done standing straight -> remember the standing eye height.
    if(g_eye_floor_valid107.load(std::memory_order_acquire)){g_standing_eye_floor_m107.store(g_eye_floor_m107.load(),std::memory_order_release);g_standing_eye_save107.store(true,std::memory_order_release);}
    else diagnostic_trace::Write("V66.107-HEAD STANDING_REFERENCE_NOT_CAPTURED floorTracking=UNAVAILABLE (STAGE space missing)");
    diagnostic_trace::Write("V66.100-CAL COMPLETE armReachMeasured=1 nativeBoneLengths=UNCHANGED fingerReferences=UNCHANGED destination=INI");return true;
}
void BeginFingerCapture() noexcept {
    SetWeaponControlMode(0);SetCombatCensusMode(2);
    g_capture_session66100.fetch_add(1,std::memory_order_acq_rel);
    g_finger_candidate_ready66100=false;g_finger_capture_status66100=1;g_finger_capture66100.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.100-F10 rung=1.5 TRACKED_ARMS_NATIVE_FINGERS emptyHandsRequired=1 waitRelaxedThenBothTriggers=1 noFingerPoseWrites=1");
}
bool FingerCaptureActive() noexcept {return g_finger_capture66100.load(std::memory_order_acquire);}
int FingerCaptureStatus() noexcept {return g_finger_capture_status66100.load(std::memory_order_acquire);}
int CalibrationPersistenceStatus() noexcept {return g_persistence_status66100.load(std::memory_order_acquire);}
const char* FingerCaptureStatusName(int s) noexcept {
    switch(s){case 1:return "RELEASE GRIPS AND TRIGGERS";case 2:return "WAIT FOR RELAXED HANDS; THEN PRESS BOTH TRIGGERS";case 3:return "PRESS BOTH TRIGGERS TOGETHER";case 4:return "HOLD BOTH TRIGGERS; WAIT FOR NATIVE FISTS";case 8:return "NATIVE FISTS QUALIFIED; RELEASE OR PRESS F10";case 5:return "PAIR CAPTURED; F10 PREVIEWS SAVED FINGERS";case 6:return "PAUSED: EMPTY HANDS/TRACKING REQUIRED; RELEASE TO REARM";case 7:return "WAIT FOR NATIVE FISTS (BUTTONS ALONE ARE NOT ENOUGH)";default:return "OFF";}
}
const char* CombatCensusRungLabel(int mode) noexcept {
    if(mode==2&&FingerCaptureActive())return "1.5";
    switch(mode){case 0:return "0";case 1:return "1";case 2:return "2";case 3:return "3";case 4:return "4";case 5:return "5";default:return "?";}
}
// Retired unused implementation archived: research_archive/retired_pose_implementations


void RequestCalibrationFromRecenter() noexcept {
    // V65.43: recenter and avatar calibration are one workflow. Camera recenter
    // re-seats Henry under the HMD; this arms a stable body T-pose capture.
    // Preserve a previously successful hand calibration until the replacement
    // T-pose is actually validated and committed; an accidental recenter must
    // not immediately destroy good controller->hand tracking.
    const bool hadCalibration=g_cal_state.load(std::memory_order_acquire)==2;
    if(!hadCalibration) g_cal_state.store(0,std::memory_order_release);
    g_vrik_calibration_requested.store(true,std::memory_order_release);
    g_vrik_calibration_requests.fetch_add(1,std::memory_order_relaxed);
    if(!g_cal_stability_lock.test_and_set(std::memory_order_acquire)){
        g_cal_stable_have_sample=false; g_cal_stable_since_ms=0;
        g_cal_stability_lock.clear(std::memory_order_release);
    }
    char cm[320]{}; std::snprintf(cm,sizeof(cm),
        "V65.43-VRIK-CAL ARMED: recenter complete; previousHandCalibration=%s; stand straight + arms out naturally and hold still ~0.4s",
        hadCalibration?"PRESERVED-UNTIL-REPLACED":"NONE");
    diagnostic_trace::Write(cm);
}
bool CalibrationReady() noexcept { return !g_arm_capture_active66100.load() && (g_cal_state.load(std::memory_order_acquire)==2 || g_saved_arm_ready66100.load(std::memory_order_acquire)); }
void BeginGuidedCalibrationPose(int pose) noexcept {
    pose=1;
    while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){}
    g_guided_cal_accum=GuidedCalAccum{};g_guided_cal_pose.store(pose,std::memory_order_release);
    g_guided_cal_lock.clear(std::memory_order_release);
    char m[192]{};std::snprintf(m,sizeof(m),"V65.54-CAL-GUIDED CAPTURE-BEGIN pose=%d",pose);diagnostic_trace::Write(m);
}
void EndGuidedCalibrationPose(int pose) noexcept {
    g_guided_cal_pose.store(0,std::memory_order_release);
    while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){}
    const auto a=g_guided_cal_accum;
    g_guided_pose_result[1]=a;
    g_guided_cal_lock.clear(std::memory_order_release);
    const double d=a.n?1.0/double(a.n):0.0;char m[640]{};std::snprintf(m,sizeof(m),
        "V65.54-CAL-GUIDED CAPTURE-END pose=%d samples=%llu avgH=(%+.3f,%+.3f,%+.3f) avgL=(%+.3f,%+.3f,%+.3f) avgR=(%+.3f,%+.3f,%+.3f)",
        pose,(unsigned long long)a.n,a.h.x*d,a.h.y*d,a.h.z*d,a.l.x*d,a.l.y*d,a.l.z*d,a.r.x*d,a.r.y*d,a.r.z*d);diagnostic_trace::Write(m);
    if(pose==1 && !CalibrationReady()) (void)TryCommitCompletedGuidedCalibrationV6562();
}


void SetFinalSkinTestMode(int mode) noexcept {
    RestorePendingIfPossible(nullptr);
    mode=std::clamp(mode,0,6);
    if(mode>0) g_census_mode.store(0,std::memory_order_release);
    g_mode.store(mode,std::memory_order_release);
    // V65.03 F9 rung 5 reuses the existing bounded read-only Mannequin/
    // Action/Context RTTI census. It makes no calls and performs no writes.
    g_f11_theory_phase.store(mode>=5?99:0,std::memory_order_release);
    animation_filter::SetMode(animation_filter::SuppressionMode::Off);
    animation_filter::SetStartDiagnosticsLevel(mode);
    ResetCalibration();g_correlated_calls.store(0);g_rejects.store(0);g_prebuild_writes.store(0);g_setik_upstream_writes.store(0);g_setik_hold_writes.store(0);g_rig_valid.store(false);
    g_cached_count.store(0);g_cached_hand_r.store(-1);g_cached_hand_l.store(-1);g_cached_head.store(-1);
    PublishGripTargets(nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr);
    char m[520]{}; std::snprintf(m,sizeof(m),
        "V65.05-VRIK F9 level=%d %s animationWrites=NONE poseWrites=NONE setIKWrites=NONE nativeIKTargetWrites=NONE playerAnimValidated=%d",
        mode,ModeName(mode),animation_filter::PlayerAnimationObjectValidated()?1:0); diagnostic_trace::Write(m);
}
int FinalSkinTestMode() noexcept{return g_mode.load(std::memory_order_acquire);}
const char* ModeName(int mode) noexcept {
    switch(mode){
    case 0:return "OFF";
    case 1:return "HENRY ANIMATION STARTS";
    case 2:return "ANIMATION + WEAPONSOLVER SETIK";
    case 3:return "+ FINAL HAND/HELPER + XR";
    case 4:return "+ ATTACHMENT OWNERSHIP";
    case 5:return "FULL + INTERACTION IKTARGET RUNTIME + HAND/FINGER CENSUS";
    default:return "F9 UNKNOWN";
    }
}
const char* CombatCensusModeName(int mode) noexcept {
    switch(mode){case 0:return "OFF / NATIVE";case 1:return "BOUNDED REFERENCE DIAGNOSTIC";
    case 2:return FingerCaptureActive()?"RUNG 1.5 TRACKED ARMS + NATIVE FINGERS / CAPTURE":"TRACKED ARMS + SAVED/NATIVE-CURVE FINGERS / QUIET";
    case 3:return "CONNECTED UPPER BODY + TRACKED HANDS / QUIET";case 4:return "UPPER BODY + STATIC CONTACT / OPT-IN";case 5:return "PHYSICAL FISTS / STATIC CONTACT / MOTION-ONLY NATIVE PUNCH TEST";default:return "INVALID";}
}
