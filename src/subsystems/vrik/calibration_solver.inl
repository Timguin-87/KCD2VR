static bool LooksLikeTpose(const TrackingSnapshot& t) noexcept {
    if(!t.head_valid||!t.left_valid||!t.right_valid||!t.allowed) return false;
    const V3 l=Sub(t.left,t.head), r=Sub(t.right,t.head);
    const float sep=Len(Sub(t.right,t.left));
    const float dl=Len(l), dr=Len(r);
    // Coordinate-independent human-scale gate. Palms-up is the instructed calibration
    // pose; controller orientation is captured as the wrist-rotation baseline below.
    // We intentionally do not assume a Quest/OpenXR local controller axis here.
    if(!(sep > 0.65f && sep < 2.20f && dl>0.30f && dl<1.30f && dr>0.30f && dr<1.30f)) return false;
    // Both hands should have broadly similar reach from the HMD in a T pose. This
    // rejects many accidental draw/sheath/interact poses without assuming a tracking-space axis.
    const float radialMismatch=std::fabs(dl-dr);
    return radialMismatch < 0.28f;
}

static bool StableTposeWindow(const TrackingSnapshot& t) noexcept {
    if(!LooksLikeTpose(t)) {
        if(!g_cal_stability_lock.test_and_set(std::memory_order_acquire)){
            g_cal_stable_have_sample=false; g_cal_stable_since_ms=0;
            g_cal_stability_lock.clear(std::memory_order_release);
        }
        return false;
    }
    if(g_cal_stability_lock.test_and_set(std::memory_order_acquire)) return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!g_cal_stable_have_sample){
        g_cal_stable_head=t.head; g_cal_stable_left=t.left; g_cal_stable_right=t.right;
        g_cal_stable_since_ms=now; g_cal_stable_have_sample=true;
        g_cal_stability_lock.clear(std::memory_order_release); return false;
    }
    const float motion=std::max(Len(Sub(t.head,g_cal_stable_head)),
        std::max(Len(Sub(t.left,g_cal_stable_left)),Len(Sub(t.right,g_cal_stable_right))));
    if(motion>0.025f){
        g_cal_stable_head=t.head; g_cal_stable_left=t.left; g_cal_stable_right=t.right; g_cal_stable_since_ms=now;
        g_cal_stability_lock.clear(std::memory_order_release); return false;
    }
    const bool ready=now>=g_cal_stable_since_ms && (now-g_cal_stable_since_ms)>=400ull;
    g_cal_stability_lock.clear(std::memory_order_release);
    return ready;
}

static bool CommitGuidedCalibrationV6562(const GuidedCalAccum& a,const V3& nativeHeadPos) noexcept {
    if(a.n<20 || !a.haveQ) return false;
    const float d=1.0f/float(a.n);
    const V3 head=Mul(a.h,d), left=Mul(a.l,d), right=Mul(a.r,d);
    const Q4 hq=QNormalize(a.hqSum), lq=QNormalize(a.lqSum), rq=QNormalize(a.rqSum);
    g_cal_head_x.store(head.x);g_cal_head_y.store(head.y);g_cal_head_z.store(head.z);
    const auto heading=kcd2vr::tracking125::yaw_only({hq.x,hq.y,hq.z,hq.w});
    const Q4 chq{0,0,float(heading.y),float(heading.w)},clq=XrToCryQ(lq),crq=XrToCryQ(rq);
    g_cal_hq_x=0;g_cal_hq_y=0;g_cal_hq_z=chq.z;g_cal_hq_w=chq.w;
    g_cal_lq_x=clq.x;g_cal_lq_y=clq.y;g_cal_lq_z=clq.z;g_cal_lq_w=clq.w;
    g_cal_rq_x=crq.x;g_cal_rq_y=crq.y;g_cal_rq_z=crq.z;g_cal_rq_w=crq.w;
    // Camera and body use horizontal HMD heading. Looking down during F8 must
    // not make the later upright headset pitch Henry's head and chest upward.
    const Q4 trackingToModel=QConj(chq);
    const float yaw=2.0f*std::atan2(trackingToModel.z,trackingToModel.w);
    const bool spanUsable=Len(Sub(right,left))>.65f;
    g_cal_frame_qx=0;g_cal_frame_qy=0;g_cal_frame_qz=trackingToModel.z;g_cal_frame_qw=trackingToModel.w;
    g_cal_native_head_x.store(nativeHeadPos.x);g_cal_native_head_y.store(nativeHeadPos.y);g_cal_native_head_z.store(nativeHeadPos.z);
    g_cal_pos_l_x.store(0);g_cal_pos_l_y.store(0);g_cal_pos_l_z.store(0);g_cal_pos_r_x.store(0);g_cal_pos_r_y.store(0);g_cal_pos_r_z.store(0);
    g_cal_rot_l_x.store(0);g_cal_rot_l_y.store(0);g_cal_rot_l_z.store(0);g_cal_rot_l_w.store(1);g_cal_rot_r_x.store(0);g_cal_rot_r_y.store(0);g_cal_rot_r_z.store(0);g_cal_rot_r_w.store(1);
    g_cal_state.store(2,std::memory_order_release);g_vrik_calibration_requested.store(false,std::memory_order_release);
    char m[560]{};std::snprintf(m,sizeof(m),"V65.66-VRIK-CAL COMMITTED XR ARM/SHOULDER samples=%llu span=%.3fm yawMap=%+.2fdeg spanQuality=%s no-repeat=1",(unsigned long long)a.n,double(Len(Sub(right,left))),double(yaw*57.29577951308232f),spanUsable?"GOOD":"APPROX");diagnostic_trace::Write(m);debug_trace::Write(m);
    return true;
}

static bool CommitGuidedCalibrationImmediateV6566() noexcept {
    if(g_cal_state.load(std::memory_order_acquire)==2) return true;
    GuidedCalAccum a{}; while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){} a=g_guided_pose_result[1]; g_guided_cal_lock.clear(std::memory_order_release);
    if(a.n<60 || !a.haveQ) return false;
    // Arm/shoulder calibration is an XR-space measurement. Do not block readiness
    // on a lucky Henry skeleton callback. If a validated native head is already
    // cached use it; otherwise commit with a temporary zero anchor and latch the
    // first real Henry head pose when VRIK actually writes an arm.
    V3 nativeHead{};
    const bool haveNative=g_latest_native_head_valid.load(std::memory_order_acquire);
    if(haveNative) nativeHead={g_latest_native_head_x.load(),g_latest_native_head_y.load(),g_latest_native_head_z.load()};
    if(!CommitGuidedCalibrationV6562(a,nativeHead)) return false;
    g_cal_native_anchor_pending.store(!haveNative,std::memory_order_release);
    diagnostic_trace::Write(haveNative
        ? "V65.66-VRIK-CAL READY immediately from XR T-pose; Henry anchor already cached"
        : "V65.66-VRIK-CAL READY immediately from XR T-pose; Henry anchor will latch lazily on first arm write");
    return true;
}

static bool TryCommitCompletedGuidedCalibrationV6562() noexcept {
    return CommitGuidedCalibrationImmediateV6566();
}

static bool CaptureTposeCalibration(void* pose,PoseGetFn getter,const RigMap& rig,const TrackingSnapshot& t) noexcept {
    if(!g_vrik_calibration_requested.load(std::memory_order_acquire) || !pose || !getter) return false;
    static std::atomic<std::uint64_t> lastWaitLog{0};
    // V65.55 guided calibration: the two-second sample window is the stability filter.
    // Do not additionally require one lucky 400 ms interval with <2.5 cm motion.
    // Once we have >=120 valid guided samples, solve from their mean. Legacy/manual
    // calibration still uses StableTposeWindow().
    bool guidedMeanReady=false;
    GuidedCalAccum guidedMean{};
    if(!g_guided_cal_lock.test_and_set(std::memory_order_acquire)) {
        // V65.57a: accept either the live guided T-pose accumulator OR the most
        // recently completed guided T-pose. EndGuidedCalibrationPose() clears the
        // live pose flag before the next Henry SetIK callback can arrive, so the old
        // code could throw away a perfectly good ~180-sample capture and fall back
        // to the legacy 400 ms / 2.5 cm stillness gate. Preserve the captured mean
        // across that boundary. 60 valid samples is intentionally forgiving; normal
        // two-second captures are ~175-180 samples in the user's runtime.
        const bool live=(g_guided_cal_pose.load(std::memory_order_acquire)==1);
        const auto a=live?g_guided_cal_accum:g_guided_pose_result[1];
        guidedMeanReady=(a.n>=60 && a.haveQ);
        if(guidedMeanReady) guidedMean=a;
        g_guided_cal_lock.clear(std::memory_order_release);
    }
    if(!guidedMeanReady && !StableTposeWindow(t)) {
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastWaitLog.load(std::memory_order_relaxed);
        if(now-prev>=1000ull && lastWaitLog.compare_exchange_strong(prev,now,std::memory_order_acq_rel))
            diagnostic_trace::Write("V65.55-VRIK-CAL waiting: need guided mean >=120 samples or legacy stable T-pose window");
        return false;
    }
    QuatT28 nh{},lh{},rh{};
    if(!ReadQuat(getter,pose,rig.head,nh)||!ReadQuat(getter,pose,rig.hand_l,lh)||!ReadQuat(getter,pose,rig.hand_r,rh)) return false;
    TrackingSnapshot ct=t;
    // V65.54: while the guided T-pose capture is active, solve against the mean
    // of the capture samples accumulated so far. This removes single-frame jitter
    // from both avatar yaw and controller->palm orientation calibration.
    // V65.57a: when a completed guided T-pose is available, always solve from
    // that averaged capture rather than from whichever controller frame happens to
    // coincide with the later Henry pose callback.
    if(guidedMeanReady) {
        const auto& a=guidedMean;
        const float d=1.0f/float(a.n);
        ct.head=Mul(a.h,d);ct.left=Mul(a.l,d);ct.right=Mul(a.r,d);
        ct.hq=QNormalize(a.hqSum);ct.lq=QNormalize(a.lqSum);ct.rq=QNormalize(a.rqSum);
    } else if(g_guided_cal_pose.load(std::memory_order_acquire)==1 && !g_guided_cal_lock.test_and_set(std::memory_order_acquire)) {
        const auto a=g_guided_cal_accum;
        g_guided_cal_lock.clear(std::memory_order_release);
        if(a.n>=12 && a.haveQ) {
            const float d=1.0f/float(a.n);
            ct.head=Mul(a.h,d);ct.left=Mul(a.l,d);ct.right=Mul(a.r,d);
            ct.hq=QNormalize(a.hqSum);ct.lq=QNormalize(a.lqSum);ct.rq=QNormalize(a.rqSum);
        }
    }
    g_cal_head_x.store(ct.head.x);g_cal_head_y.store(ct.head.y);g_cal_head_z.store(ct.head.z);
    const auto yaw125=kcd2vr::tracking125::yaw_only({ct.hq.x,ct.hq.y,ct.hq.z,ct.hq.w});
    const Q4 hq=XrToCryQ({float(yaw125.x),float(yaw125.y),float(yaw125.z),float(yaw125.w)}), lq=XrToCryQ(ct.lq), rq=XrToCryQ(ct.rq);
    g_cal_hq_x.store(hq.x);g_cal_hq_y.store(hq.y);g_cal_hq_z.store(hq.z);g_cal_hq_w.store(hq.w);
    g_cal_lq_x.store(lq.x);g_cal_lq_y.store(lq.y);g_cal_lq_z.store(lq.z);g_cal_lq_w.store(lq.w);
    g_cal_rq_x.store(rq.x);g_cal_rq_y.store(rq.y);g_cal_rq_z.store(rq.z);g_cal_rq_w.store(rq.w);
    // Camera and every body-calibration path share horizontal HMD heading.
    // A tilted or asymmetric arm span must not tilt/rotate the tracking frame.
    const Q4 trackingToModel=QConj(hq);
    const float yaw=2*std::atan2(trackingToModel.z,trackingToModel.w);
    g_cal_frame_qx.store(trackingToModel.x);g_cal_frame_qy.store(trackingToModel.y);
    g_cal_frame_qz.store(trackingToModel.z);g_cal_frame_qw.store(trackingToModel.w);
    const V3 nativeHeadPos=FromT(nh);
    g_cal_native_head_x.store(nativeHeadPos.x);g_cal_native_head_y.store(nativeHeadPos.y);g_cal_native_head_z.store(nativeHeadPos.z);
    {const float a43[3]{nativeHeadPos.x,nativeHeadPos.y,nativeHeadPos.z}; // C43: the T-pose head is the calibrated anchor (persisted)
     if(kcd2vr::calibration_restore43::anchorPlausible(a43)){g_cal_anchor_x43=a43[0];g_cal_anchor_y43=a43[1];g_cal_anchor_z43=a43[2];g_cal_anchor_saved43.store(true,std::memory_order_release);g_cal_anchor_save43.store(true,std::memory_order_release);}}
    // V65.56: F8 no longer discovers wrist rotation from a user pose. The OpenXR
    // grip pose is the controller authority; Henry's fixed bone->anatomical-palm
    // relation is derived once from his finger-root geometry by the live arm solver.
    // Calibration therefore owns BODY/YAW/SCALE only. Keep legacy rotation offsets
    // identity so older diagnostic paths cannot inherit an arbitrary animated wrist.
    V3 pl{0,0,0},pr{0,0,0}; Q4 ql{},qr{};
    g_cal_pos_l_x.store(pl.x);g_cal_pos_l_y.store(pl.y);g_cal_pos_l_z.store(pl.z);
    g_cal_pos_r_x.store(pr.x);g_cal_pos_r_y.store(pr.y);g_cal_pos_r_z.store(pr.z);
    g_cal_rot_l_x.store(ql.x);g_cal_rot_l_y.store(ql.y);g_cal_rot_l_z.store(ql.z);g_cal_rot_l_w.store(ql.w);
    g_cal_rot_r_x.store(qr.x);g_cal_rot_r_y.store(qr.y);g_cal_rot_r_z.store(qr.z);g_cal_rot_r_w.store(qr.w);

    // V65.80: the current guided workflow captures only one T-pose, so the retired
    // three-pose body-fit block can never set g_body_cal_ready. Derive the anatomical
    // shoulder frame directly from Henry's validated native T-pose. This is the frame
    // consumed by the hard upper-body writer (head-relative shoulder center + width).
    QuatT28 clavL{},clavR{};
    const bool haveClavL=rig.clav_l>=0&&ReadQuat(getter,pose,rig.clav_l,clavL);
    const bool haveClavR=rig.clav_r>=0&&ReadQuat(getter,pose,rig.clav_r,clavR);
    if(haveClavL&&haveClavR) {
        const V3 shoulderL=FromT(clavL), shoulderR=FromT(clavR);
        const V3 shoulderC=Mul(Add(shoulderL,shoulderR),0.5f);
        const V3 shoulderRel=Sub(shoulderC,nativeHeadPos);
        const float shoulderW=Len(Sub(shoulderR,shoulderL));
        const float nativeHandSpan=Len(Sub(FromT(rh),FromT(lh)));
        if(std::isfinite(shoulderW)&&shoulderW>0.18f&&shoulderW<0.90f&&
           std::isfinite(nativeHandSpan)&&nativeHandSpan>0.45f&&nativeHandSpan<2.20f&&
           std::isfinite(shoulderRel.x)&&std::isfinite(shoulderRel.y)&&std::isfinite(shoulderRel.z)) {
            g_body_shoulder_rel_x.store(shoulderRel.x); g_body_shoulder_rel_y.store(shoulderRel.y); g_body_shoulder_rel_z.store(shoulderRel.z);
            g_body_shoulder_width.store(shoulderW); g_body_arm_span.store(nativeHandSpan);
            g_body_guard_forward_x.store(0.0f); g_body_guard_forward_y.store(1.0f);
            g_body_cal_ready.store(true,std::memory_order_release);
            char bm[512]{}; std::snprintf(bm,sizeof(bm),
                "V65.80-CAL-BODY-FIT source=SINGLE-NATIVE-TPOSE shoulderCenterRelHead=(%+.3f,%+.3f,%+.3f) shoulderWidth=%.3f nativeHandSpan=%.3f",
                double(shoulderRel.x),double(shoulderRel.y),double(shoulderRel.z),double(shoulderW),double(nativeHandSpan));
            diagnostic_trace::Write(bm);
        } else {
            diagnostic_trace::Write("V65.80-CAL-BODY-FIT rejected native T-pose geometry; hard body remains fail-soft bodyFit=0");
        }
    } else {
        diagnostic_trace::Write("V65.80-CAL-BODY-FIT unavailable: native clavicle transforms missing");
    }

    g_cal_state.store(2,std::memory_order_release);
    g_vrik_calibration_requested.store(false,std::memory_order_release);
    char m[640]{}; std::snprintf(m,sizeof(m),
        "V65.56-VRIK-CAL CAPTURED BODY TPOSE span=%.3fm yawMap=%+.2fdeg LposLocal=(%+.3f,%+.3f,%+.3f) RposLocal=(%+.3f,%+.3f,%+.3f) positionMode=CONTROLLER-HARD frame=TPOSE-SPAN-YAW handRotation=FIXED-OPENXR-GRIP+HENRY-ANATOMY",
        double(Len(Sub(t.right,t.left))),double(yaw*57.29577951308232f),double(pl.x),double(pl.y),double(pl.z),double(pr.x),double(pr.y),double(pr.z));
    diagnostic_trace::Write(m); debug_trace::Write(m);
    return true;
}

// V65.83: native anatomy readiness is independent from the user's XR T-pose
// request.  A completed guided calibration may clear g_vrik_calibration_requested
// before Henry's pose callback arrives; lazily recover the stable head/clavicle
// relationship from any validated Henry pose instead of asking for another T-pose.
// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations

// V66.88: V66.86 final-camera/HMD pitch compensation retired after headset regression.

// V64.0 arm-handle/grip-chain goal placement.
//
// KCD2 Mannequin interaction data names native procedural arm IK handles RgtArm01 and
// LftArm01. Those are semantic IK GOALS, not skeletal hand joints. The runtime
// IKTargetContext ABI that owns the named handles is not yet proven, so this lab
// deliberately keeps the already-proven V63.4 late-pose write boundary. However,
// the target fed to our two-bone solve is now an absolute controller-derived arm
// goal in Henry model space. hand_r/hand_l are only the final skeletal effectors
// written after solving the chain; they are never used as the target origin.
//
// This removes the old "frozen hand position + controller delta" target error.
bool ControllerHeadAnchoredArmGoal(const TrackingSnapshot& t,const QuatT28& nativeHead,bool left,V3& out) noexcept {
    out={};
    if(g_cal_native_anchor_pending.exchange(false,std::memory_order_acq_rel)) {
        const V3 nh=FromT(nativeHead);
        if(std::isfinite(nh.x)&&std::isfinite(nh.y)&&std::isfinite(nh.z)) {
            g_cal_native_head_x.store(nh.x); g_cal_native_head_y.store(nh.y); g_cal_native_head_z.store(nh.z);
            diagnostic_trace::Write("V65.66-VRIK-CAL lazy Henry head anchor latched; XR arm calibration remains unchanged");
            {const float a43[3]{nh.x,nh.y,nh.z}; // C43: first anchor of a fresh calibration is the calibrated one (persisted)
             if(!g_cal_anchor_saved43.load(std::memory_order_acquire)&&kcd2vr::calibration_restore43::anchorPlausible(a43)){g_cal_anchor_x43=a43[0];g_cal_anchor_y43=a43[1];g_cal_anchor_z43=a43[2];g_cal_anchor_saved43.store(true,std::memory_order_release);g_cal_anchor_save43.store(true,std::memory_order_release);}}
        } else {
            g_cal_native_anchor_pending.store(true,std::memory_order_release);
        }
    }
    const bool controllerValid=left?t.left_valid:t.right_valid;
    if(!t.head_valid||!controllerValid||!t.allowed||g_cal_state.load(std::memory_order_acquire)!=2) return false;
    const V3 controller=left?t.left:t.right;
    const V3 xrRel=Sub(controller,t.head);
    const float xrLen=Len(xrRel);
    if(!std::isfinite(xrLen)||xrLen<0.08f||xrLen>1.50f) return false;
    const Q4 trackingToModel=QNormalize(Q4{g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()});
    const V3 calTrackingHead{g_cal_head_x.load(),g_cal_head_y.load(),g_cal_head_z.load()};
    const V3 nativeHeadAtCal{g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
    // Translate the avatar head by physical HMD motion since calibration, but keep
    // the calibration orientation fixed. Native idle/combat head animation can no
    // longer rotate the controller vector behind the player.
    const V3 headDeltaModel=QRotateV(trackingToModel,XrToCryV(Sub(t.head,calTrackingHead)));
    const V3 avatarHead=Add(nativeHeadAtCal,headDeltaModel);
    // V66.88 recovery: use one stable calibration frame for controller position/orientation.
    // V66.86 tried to subtract final-camera pitch from HMD pitch, but the final camera
    // sample alternated between two frames and injected 20-40 degree jumps into the arms.
    // Free-look is therefore NOT folded into VRIK here; controller->Henry mapping remains
    // the stable V65.81/V65.85 calibration-space path.
    V3 modelRel=QRotateV(trackingToModel,XrToCryV(xrRel));
    const V3 offLocal=left?V3{g_cal_pos_l_x.load(),g_cal_pos_l_y.load(),g_cal_pos_l_z.load()}
                          :V3{g_cal_pos_r_x.load(),g_cal_pos_r_y.load(),g_cal_pos_r_z.load()};
    const Q4 ctrlModel=QNormalize(QMul(trackingToModel,XrToCryQ(left?t.lq:t.rq)));
    const V3 modelOffset=QRotateV(ctrlModel,offLocal);
    out=Add(avatarHead,Add(modelRel,modelOffset));
    return std::isfinite(out.x)&&std::isfinite(out.y)&&std::isfinite(out.z);
}
