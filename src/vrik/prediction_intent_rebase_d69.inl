// A deliberate live prediction policy/target-basis change is one non-motion
// source sample. Preserve the checked weapon/support and all native in-flight
// state; update local source origins without forging movement or an Abort.
inline void normal_intent_prediction_d69(State& state,const Input& in,const Profile& p,
        WeaponState& s,Output& out,int side,double dt,Pose rawIntentWeapon104,
        Pose intentWeapon104,Pose relativeWeapon63,Pose final,bool stablePair63,
        Vec3 supportHandDelta63,Vec3 supportRelativeDelta63,bool rebase,bool softH30=false){
    const bool current=!in.contactFaultH30&&model_intent_current_h30(in,side)&&
        (!normal_intent_enabled_h30(in)||(in.attacks&&in.intentTracked[side]&&finite(in.intentHands[side])));
    if(!rebase||!current){normal_intent_phase_h30(state,in,p,s,out,side,dt,rawIntentWeapon104,intentWeapon104,
        relativeWeapon63,final,stablePair63,supportHandDelta63,supportRelativeDelta63,softH30);return;}
    const auto model=p.melee()&&!p.shield176()?model_intent_hand_h30(in,side):in.hands[side];
    s.intentTip=point(intentWeapon104,p.tip);s.intentHand=model.p;
    s.relativeIntentTip=point(relativeWeapon63,p.tip);s.relativeIntentHand=in.intentHands[side].p;
    s.intentReady=finite(model)&&finite(intentWeapon104);
    s.relativeIntentReady=in.intentTracked[side]&&finite(in.intentHands[side])&&finite(relativeWeapon63);
    s.lastTrackingRotationH29=in.intentHands[side].q;
    s.sourceModelH30=model;s.sourceTrackingH30=in.intentHands[side];
    s.sourceOtherModelH30=model_intent_hand_h30(in,1-side);s.sourceOtherTrackingH30=in.intentHands[1-side];
    s.sourceGripH30=p.grip[side];s.sourceTipH30=p.tip;s.sourcePoseH30=s.intentReady&&s.relativeIntentReady;
    s.sourceModelFrameH8=in.modelIntentFrameH8;s.sourceModelCalibrationH8=in.modelIntentCalibrationH8;
    // No new impact identity/energy is minted for this basis-only increment.
    out.impactRecord178r30[side]={};
}
