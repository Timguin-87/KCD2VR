// One existing committed stroke owns both recognition and fresh-contact binding.
// Its original tick/origin never change. REARM while spent is not a cancellation;
// confirmed quiet/recovery completion or a new candidate ends that stroke.
inline void AdvanceCommittedGestureH8(const Input& in,const Profile& p,WeaponState& s,int side,
    Pose modelWeapon,Pose trackingWeapon,bool sourceCurrent) noexcept {
    if(!normal_hybrid_intent_h31(in)||!s.committedGestureH26.gesture)return;
    auto& c=s.committedGestureH26;
    if(!in.allowed||!in.attacks||!in.tracked[side]||!in.intentTracked[side]||!model_intent_current_h30(in,side)||
       in.contactFaultH30||s.guard117||c.key!=p.key||c.generation!=p.generation||
       c.epoch!=in.bodyEpoch||c.request!=in.epoch||in.tick<c.tick||
       s.gesture59c3!=c.gesture||(!s.stroke118.spent&&!s.stroke118.active)) {c={};return;}
    if(c.forwardSampleH8==in.sample&&c.forwardTickH8==in.tick)return;
    if(c.uncertainSinceH8&&(in.tick<c.uncertainSinceH8||
       motion_time::elapsed(c.uncertainSinceH8,in.tick,c.uncertainNsH8,in.timeNs)>.080000001)){c={};return;}
    const double dt=motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
    if(!(dt>0)||dt>.080000001||!s.intentReady||!s.relativeIntentReady){c={};return;}
    const Vec3 modelTip=point(modelWeapon,p.tip)-s.intentTip,trackingTip=point(trackingWeapon,p.tip)-s.relativeIntentTip;
    const auto hand=model_intent_hand_h30(in,side);
    const Vec3 modelHand=hand.p-s.intentHand,trackingHand=in.intentHands[side].p-s.relativeIntentHand;
    const double modelSpeed=m::length(modelTip)/dt,trackingSpeed=m::length(trackingTip)/dt;
    if(!m::finite(modelTip)||!m::finite(trackingTip)||!m::finite(modelHand)||!m::finite(trackingHand)||
       !std::isfinite(modelSpeed)||!std::isfinite(trackingSpeed)||
       !std::isfinite(m::length(modelHand)/dt)||!std::isfinite(m::length(trackingHand)/dt)){c={};return;}
    // A committed attack retains its original identity through finite fast motion.
    // Do not quarantine it a second time using arbitrary hand/tip speed ceilings.
    if(!sourceCurrent){c={};return;}
    c.uncertainSinceH8=c.uncertainNsH8=c.uncertainSampleH8=c.uncertainAfterSampleH8=0;c.uncertainFailedH8=0;
    const Vec3 forward=body_axes(in).forward;
    const Vec3 relativeForward=m::rotate(m::multiply(in.intentHands[side].q,m::conjugate(hand.q)),forward);
    // Same windup test as the selected recognizer, with both original frames.
    // Do not compare a curved cut to its COMMIT tangent: a valid arc turns.
    // A cut's tip can arc back while its hand continues toward the opponent.
    // That is the SAME committed cut, not a new backward windup. Reclassification
    // must not discard its checked landing. A retreat increment supplies no
    // contact identity; the recognizer's confirmed recovery retires the cut.
    const double modelForwardHand=m::dot(modelHand,forward)/dt;
    const double trackedForwardHand=m::dot(trackingHand,relativeForward)/dt;
    const bool tipRetreat=m::dot(modelTip,forward)/dt<-melee118::kCutWindupTip55||
        m::dot(trackingTip,relativeForward)/dt<-melee118::kCutWindupTip55;
    // CL-F (REPAIR2 headset log: 45 of 57 unlanded NPC contacts came 20-165 ms after a COMMITTED cut): a cut is an
    // arc around the shoulder. Past its most-forward point the hand keeps going sideways/down and its body-forward
    // component turns a little negative - that follow-through is the SAME cut. For a cut, only a DOMINANTLY backward
    // hand (back component >= 60% of the hand speed, and above the windup speed) is a retreat/back-swing. Thrusts keep
    // the strict test. The recognizer's own rearm/new-candidate/quiet rules still end the stroke (checked above).
    const double modelHandSpeedF=m::length(modelHand)/dt,trackedHandSpeedF=m::length(trackingHand)/dt;
    const auto backward=[&](double forwardSpeed,double speed){
        return forwardSpeed<-melee118::kCutWindupInward51&&(c.kind!=0||-forwardSpeed>=.60*speed);};
    const auto backDominant=[&](double forwardSpeed,double speed){return forwardSpeed<=0&&-forwardSpeed>=.60*speed;};
    // Counter-rotation may pull the wrist back while the blade still cuts
    // forward. For a committed CUT, that is not a backward weapon windup.
    const bool thrustForwardD70=kcd2vr::claude_d70::ForwardThrust(in.tick,c.tick,c.kind,
        m::dot(modelTip,c.modelDirection),m::dot(trackingTip,c.trackingDirection),
        m::dot(modelTip,forward),m::dot(trackingTip,relativeForward));
    const bool bladeStillForward=(c.kind==0||thrustForwardD70)&&m::dot(modelTip,forward)>0&&m::dot(trackingTip,relativeForward)>0;
    const bool handRetreat=!bladeStillForward&&(backward(modelForwardHand,modelHandSpeedF)||backward(trackedForwardHand,trackedHandSpeedF));
    if(handRetreat||(tipRetreat&&(c.kind!=0||backDominant(modelForwardHand,modelHandSpeedF)||
       backDominant(trackedForwardHand,trackedHandSpeedF)))){
        // Do not confuse one inward increment with the recognizer's completed
        // recovery. Keep the original CUT identity, but never stamp this
        // backward frame for contact. Existing recovery/quiet/new-candidate
        // handling above and in normal_intent_phase_h30 ends the original cut.
        // STAB and POMMEL keep immediate retreat retirement. No clock is renewed.
        if(c.kind!=0||s.stroke117!=CommandKind::Slash)c={};
        return;
    }
    if(modelSpeed>.25&&trackingSpeed>.25&&
       (!c.kind||(m::dot(m::unit(modelTip).value_or(Vec3{}),c.modelDirection)>.25&&
                  m::dot(m::unit(trackingTip).value_or(Vec3{}),c.trackingDirection)>.25))){
        c.forwardTickH8=in.tick;c.forwardSampleH8=in.sample;
    }
}
