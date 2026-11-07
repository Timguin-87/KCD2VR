    // Latch the engine head anchor only from an untouched native snapshot.
    V3 unused{};(void)ControllerHeadAnchoredArmGoal(ts,s.original[rig.head],false,unused);
    const Q4 frame=QNormalize(CalFrameR133());
    const Q4 baseH{g_cal_hq_x.load(),g_cal_hq_y.load(),g_cal_hq_z.load(),g_cal_hq_w.load()};
    const Q4 delta=QNormalize(QMul(QMul(frame,QMul(XrToCryQ(ts.hq),QConj(baseH))),QConj(frame)));
    const V3 baseHead{g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
    if(!kcd2vr::native_roomscale_d76::ReadCharacterOrigin(reinterpret_cast<std::uintptr_t>(character),tx.roomOriginD76))
        return RejectCoherent6693(tx,__LINE__);
    tx.roomOriginCapturedD76=true;
    if(kcd2vr::roomscale_d76::recenterPending.load(std::memory_order_acquire)||
       (tx.roomOriginD76.owner.epoch&&tx.roomOriginD76.owner.epoch!=g_cal_room_origin_d76.load(std::memory_order_acquire)))
        return RejectCoherent6693(tx,__LINE__);
    const auto consumedD76=tx.roomOriginD76.consumedTracking;
    const V3 trackedBase{g_cal_head_x.load()+consumedD76.x,g_cal_head_y.load()+consumedD76.y,g_cal_head_z.load()+consumedD76.z};
    // V66.123 HEAD ANCHOR: the VR camera is the CURRENT native camera plus the headset
    // offset (V66.109 camera authority). The VRIK head used the native head latched ONCE at
    // calibration instead, so whenever the game's own animation moved Henry's head (sword
    // stance, crouch, walking, a calibration taken in a bad posture) camera and rendered
    // head separated and the camera ended up inside the neck. Anchor the head to the same
    // current native head the camera uses.
    const V3 liveNativeHead123=FromT(s.original[rig.head]);
    const bool liveHeadOk123=kcd2vr::riding_vrik_d76::LiveHeadUsableD80(tx.ridingGaitD76,liveNativeHead123,baseHead);
    // An invalid current mounted anchor must withdraw, never teleport to a
    // standing calibration from another native posture/model basis.
    if(tx.ridingGaitD76&&!liveHeadOk123)return RejectCoherent6693(tx,__LINE__);
    V3 headAnchor123=liveHeadOk123?liveNativeHead123:baseHead; // C49 restore live stance/crouch anchor with the original camera
    if(tx.diceBodyD81){const auto& h=tx.diceLeaseD81.headLocal;headAnchor123={h.x,h.y,h.z};}
    // Begin with the current native owner for visual-only activities. Ordinary
    // full-body play selects one shared frame below, after the current equipment
    // witness is available. No camera state is published before a pose commits.
    tx.control51=g_frame_control51.load(std::memory_order_acquire);
    const bool smoothBody51=(tx.control51&1)!=0;
    V3 headTarget=Add(headAnchor123,QRotateV(frame,XrToCryV(Sub(ts.head,trackedBase))));
    // V66.107: standing straight maps to Henry's natural head height; a taller stance or
    // tiptoes may raise it at most 5 cm (no stretched spine). Crouching still lowers it.
    // R59.16C9C (Miki 13 Oct: "head too low again"): the 19:16-19:28 log shows the headset 12 cm (median; up to 30 cm) ABOVE the
    // calibrated eye height - he calibrated slouched/seated and then sat up - and the old +5 cm cap pushed the view 7 cm below his
    // real head. The cap is now VrHeadRaiseMaxCm (default 18). Crouching/lowering stays unlimited.
    bool raiseCapped38=false;
    {const float cap=g_head_raise_max59c9.load(std::memory_order_relaxed);if(headTarget.z>headAnchor123.z+cap){headTarget.z=headAnchor123.z+cap;raiseCapped38=true;}}
    // R59.16C7F (13 Oct: "the sword sits in the wrist, not the palm"): the IK goal is the native hand/wrist bone, but the
    // OpenXR grip pose origin is the centre of the palm. VrWristFromGripCm moves the goal down the controller handle
    // (grip +Z) by that much, so the native socket (wrist) lands at the user's wrist and the handle in the palm. Default 0.
    // H50R2c optional free-hand visual filtering (default OFF): a hand that was FREE in the last committed
    // frame and is not acquiring on THIS transaction gets its tracked controller pose smoothed HERE, before the goal,
    // auto palm, free-hand contact and wrist solve consume it. ts stays raw. a parallel RAW requested goal is produced by
    // the same producer below and carried through the same reach mapping for intent (H30) and propRequested (H40/grab).
    // Staged until native restoration and final paired publication (visual_selection59c7f.h).
    const bool smoothEnabledH50=kcd2vr::hand_smooth_h50::enabled.load(std::memory_order_relaxed);
    const bool adaptiveFastH50=smoothEnabledH50&&kcd2vr::smooth_lab::AdaptiveFilterSelected(); // J26: retired from the F2 ladder
    V3 visPH50[2]{ts.right,ts.left};Q4 visQH50[2]{ts.rq,ts.lq};
    // D62: the ordinary semantic/contact solve is independent of the lead
    // experiment. Optional presentation is applied only after that solve.
    const bool visualPolicyH50=smoothEnabledH50;
    kcd2vr::hand_smooth_h50::State stagedH50[2]{s.handSmoothH50[0],s.handSmoothH50[1]};
    for(int side=0;side<2;++side){
        const bool acquiringH50=(tx.acquisitionRequired57&&tx.acquisition54.side==side)||
            (s.pendingUse55.serial&&s.pendingUse55.side==side);
        if(!smoothEnabledH50||!(side?ts.left_valid:ts.right_valid)||!s.handFreeH50[side]||acquiringH50||readingVisual32||tx.reloadWorkingHandXR34||nativeFingerCapture){stagedH50[side].ready=false;continue;}
        const V3 p=side?ts.left:ts.right;const Q4 q=side?ts.lq:ts.rq;
        const float rp[3]{p.x,p.y,p.z},rq[4]{q.x,q.y,q.z,q.w};float op[3]{},oq[4]{0,0,0,1};
        if(kcd2vr::hand_smooth_h50::Step(stagedH50[side],tx.epoch,ts.timeNs?ts.timeNs:ts.tick*1000000ull,rp,rq,op,oq,adaptiveFastH50)){
            visPH50[side]=V3{op[0],op[1],op[2]};visQH50[side]=QNormalize(Q4{oq[0],oq[1],oq[2],oq[3]});}
    }
    V3 palmOffsetD68[2]{},rawPalmOffsetD68[2]{}; // local copies only for the selected Shift+F2 burst
    const float wristBack59c7f=g_wrist_from_grip59c7f.load(std::memory_order_relaxed);
    const V3 rightXr=wristBack59c7f!=0.f?Add(visPH50[0],QRotateV(visQH50[0],V3{0,0,wristBack59c7f})):visPH50[0];
    const V3 leftXr=wristBack59c7f!=0.f?Add(visPH50[1],QRotateV(visQH50[1],V3{0,0,wristBack59c7f})):visPH50[1];
    const V3 handAnchor51=headAnchor123; // C51R1: controller endpoints belong to the shared head frame in both modes.
    V3 requestedGoals[2]{Add(handAnchor51,QRotateV(frame,XrToCryV(Sub(rightXr,trackedBase)))),Add(handAnchor51,QRotateV(frame,XrToCryV(Sub(leftXr,trackedBase))))};
    // R59.16C8 AUTO PALM: wrist goal = grip pose - half of (wrist -> knuckle bar) in the desired hand orientation (Henry's own
    // finger skeleton; PalmGeometry103 out[0] is the knuckle-bar centre relative to the wrist). The grip pose is the palm centre,
    // so the palm lands on the controller and the native weapon socket (at the wrist bone) sits one palm-length behind it.
    if(g_auto_palm59c8.load(std::memory_order_relaxed)){
        for(int side=0;side<2;++side){
            if(!(side?ts.left_valid:ts.right_valid))continue;
            V3 six[6]{};FingerContactGeometry103 g{};
            const Q4 o=DesiredHand6689(s,side,frame,visQH50[side]);
            if(!PalmGeometry103(s,side,o,0.f,0.f,0,six,g))continue;
            const V3 back=Mul(six[0],-.5f);const float cm=Len(back)*100.f;
            if(!std::isfinite(cm)||cm>12.f)continue;
            requestedGoals[side]=Add(requestedGoals[side],back);
            if(tx.motionSessionD68)palmOffsetD68[side]=rawPalmOffsetD68[side]=back;
            if(tx.traceH29&&std::fabs(g_auto_palm_last_cm59c8[side].load(std::memory_order_relaxed)-cm)>.5f){
                g_auto_palm_last_cm59c8[side].store(cm,std::memory_order_relaxed);
                char m[160]{};std::snprintf(m,sizeof(m),"R59.16C8 PALM_AUTO side=%c wristBehindPalmCm=%.1f manualCm=%+.1f (goal = grip pose - half wrist->knuckle bar)",side?'L':'R',cm,wristBack59c7f*100.f);diagnostic_trace::Write(m);}
        }
    }
    // H50R2c: the same producer on the RAW controller pose (identical values when nothing was smoothed). no log.
    V3 requestedGoalsRawH50[2]{requestedGoals[0],requestedGoals[1]};
    for(int side=0;side<2;++side){
        const V3 p=side?ts.left:ts.right;const Q4 q=side?ts.lq:ts.rq;
        if(p.x==visPH50[side].x&&p.y==visPH50[side].y&&p.z==visPH50[side].z&&q.x==visQH50[side].x&&q.y==visQH50[side].y&&
           q.z==visQH50[side].z&&q.w==visQH50[side].w)continue; // not smoothed: raw == visual already
        const V3 xr=wristBack59c7f!=0.f?Add(p,QRotateV(q,V3{0,0,wristBack59c7f})):p;
        requestedGoalsRawH50[side]=Add(handAnchor51,QRotateV(frame,XrToCryV(Sub(xr,trackedBase))));
        if(tx.motionSessionD68)rawPalmOffsetD68[side]={}; // the raw rebuild discarded the visual palm term
        if(g_auto_palm59c8.load(std::memory_order_relaxed)&&(side?ts.left_valid:ts.right_valid)){
            V3 six[6]{};FingerContactGeometry103 g{};
            if(PalmGeometry103(s,side,DesiredHand6689(s,side,frame,q),0.f,0.f,0,six,g)){
                const V3 back=Mul(six[0],-.5f);const float cm=Len(back)*100.f;
                if(std::isfinite(cm)&&cm<=12.f){requestedGoalsRawH50[side]=Add(requestedGoalsRawH50[side],back);
                    if(tx.motionSessionD68)rawPalmOffsetD68[side]=back;}
            }
        }
    }
    V3 goals[2]{requestedGoals[0],requestedGoals[1]};
    const bool valid[2]{ts.right_valid&&(!tx.reloadWorkingHandXR34||reloadHandPolicyR95.tracked[0]),
        ts.left_valid&&(!tx.reloadWorkingHandXR34||reloadHandPolicyR95.tracked[1])};
