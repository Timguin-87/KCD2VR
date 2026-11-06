// Re-express the preceding requested MODEL sample in the current ordinary
// native anchor. Controllers still supply both motion families. This removes
// only a shared coordinate translation; no rendered goal, body, sweep, impact,
// source clock, accumulated gesture vector or captured origin is changed.
inline void rebase_model_intent_frame_h8(WeaponState& s,const Input& in,const Profile& p,int side) noexcept {
    if(!normal_hybrid_intent_h31(in)||!model_intent_selected_h30(in,side)||!model_intent_current_h30(in,side)||
       !p.melee()||p.shield176()||s.key!=p.key||s.generation!=p.generation||
       !s.intentReady||!s.relativeIntentReady||!in.tracked[side]||!in.intentTracked[side])return;
    const auto& a=in.modelIntentFrameH8;const auto& b=s.sourceModelFrameH8;
    const double sourceDt=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
    // The provenance must describe the very two samples already compared by
    // normal intent. A replaced/calibrated/protected frame cannot donate it.
    if(!a.valid||!b.valid||a.bow||b.bow||a.acquisitionSerial||b.acquisitionSerial||
       a.stamp.actor!=b.stamp.actor||!a.stamp.actor||a.stamp.epoch!=in.bodyEpoch||b.stamp.epoch!=in.bodyEpoch||
       a.stamp.sample!=in.sample||a.stamp.tick!=in.tick||b.stamp.sample!=s.lastSample||b.stamp.tick!=s.lastTick||
       a.control!=b.control||!a.control||a.smooth!=b.smooth||a.hmdHeight!=b.hmdHeight||
       !std::isfinite(a.scale)||!std::isfinite(b.scale)||std::abs(a.scale-b.scale)>=1e-6||
       !m::finite(a.anchor)||!m::finite(b.anchor)||!m::finite(a.calibratedHead)||!m::finite(b.calibratedHead)||
       !m::finite(a.restOffset)||!m::finite(b.restOffset)||
       m::length(a.calibratedHead-b.calibratedHead)>=1e-6||m::length(a.restOffset-b.restOffset)>=1e-6||
       !finite(Pose{in.modelIntentCalibrationH8,{}})||!finite(Pose{s.sourceModelCalibrationH8,{}})||
       m::angle(in.modelIntentCalibrationH8,s.sourceModelCalibrationH8)>=1e-7||
       !(sourceDt>0&&sourceDt<=.080000001))return;
    const Vec3 translation{a.anchor.x-b.anchor.x,a.anchor.y-b.anchor.y,tracked_frame51::shared_frame_delta_r98(a,b).z}; // R98: native height moves the same frame
    s.intentHand=s.intentHand+translation;s.intentTip=s.intentTip+translation;
    s.sourceModelH30.p=s.sourceModelH30.p+translation;
    s.sourceOtherModelH30.p=s.sourceOtherModelH30.p+translation;
    s.supportIntentHand63=s.supportIntentHand63+translation;
    s.sourceModelFrameH8=a;
}
