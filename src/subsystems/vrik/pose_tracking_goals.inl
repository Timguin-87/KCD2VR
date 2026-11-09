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
    const bool directHandsR135=DirectControllerHandsR135();
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
    // R142: direct controller goals must share the camera's actual placed head
    // origin (visual center between eyes in world space, AER invariant).
    // Use the latest published camera world center and convert it into this
    // character's model space. When direct hands are active, fall back to baseHead
    // (calibrated head) rather than live animated head so hands never inherit head bob.
    const V3 directFallbackAnchor=Add(baseHead,QRotateV(frame,XrToCryV(Sub(ts.head,trackedBase))));
    V3 directHandAnchorR142=directFallbackAnchor;float directHandModelScaleR142=1.f;
    bool directCameraAnchorR142=false;
    Q4 directHandFrameR142=frame;
    if(directHandsR135){
        float wx=0.f,wy=0.f,wz=0.f;const auto probe=g_view_position_probe43.load(std::memory_order_acquire);
        ModelWorld6692 world{};
        if(probe&&probe(wx,wy,wz,250)&&std::isfinite(wx)&&std::isfinite(wy)&&std::isfinite(wz)&&
           ReadModelWorld6692(character,world)&&world.scale>.01f&&std::isfinite(world.scale)){
            directHandAnchorR142=WorldToModel6692(world,V3{wx,wy,wz});
            directHandModelScaleR142=world.scale;
            directCameraAnchorR142=std::isfinite(directHandAnchorR142.x)&&std::isfinite(directHandAnchorR142.y)&&std::isfinite(directHandAnchorR142.z);
            if(!directCameraAnchorR142)directHandAnchorR142=directFallbackAnchor;

            // Align direct 1:1 hand coordinate space to visual camera forward in model space
            // (e.g. on horseback or when turning the camera independent of character/horse root)
            float fx=0.f,fy=0.f,fz=0.f;const auto fwdProbe=g_view_forward_probe43.load(std::memory_order_acquire);
            if(fwdProbe&&fwdProbe(fx,fy,fz,250)){
                const V3 fwdModel=QRotateV(QConj(world.q),V3{fx,fy,fz});
                const float h2=fwdModel.x*fwdModel.x+fwdModel.y*fwdModel.y;
                if(std::isfinite(h2)&&h2>1e-4f){
                    const float cyaw=std::atan2(-fwdModel.x,fwdModel.y);
                    const Q4 camModelYaw{0.f,0.f,std::sin(cyaw*0.5f),std::cos(cyaw*0.5f)};
                    const Q4 hmdYaw=XrToCryQ({0.f,ts.hq.y,0.f,ts.hq.w});
                    directHandFrameR142=QNormalize(QMul(camModelYaw,QConj(hmdYaw)));
                }
            }
        }
    }
    // R59.16C7F (13 Oct: "the sword sits in the wrist, not the palm"): the IK goal is the native hand/wrist bone, but the
    // OpenXR grip pose origin is the centre of the palm. VrWristFromGripCm moves the goal down the controller handle
    // (grip +Z) by that much, so the native socket (wrist) lands at the user's wrist and the handle in the palm. Default 0.
    // R141 direct mode keeps controller translation 1:1, but still applies the skeleton-derived palm-to-wrist correction
    // below. The manual offset stays legacy-only because it is a user-authored displacement, not hand anatomy.
    // H50R2c optional free-hand visual filtering (default OFF): a hand that was FREE in the last committed
    // frame and is not acquiring on THIS transaction gets its tracked controller pose smoothed HERE, before the goal,
    // auto palm, free-hand contact and wrist solve consume it. ts stays raw. a parallel RAW requested goal is produced by
    // the same producer below and carried through the same reach mapping for intent (H30) and propRequested (H40/grab).
    // Staged until native restoration and final paired publication (visual_selection59c7f.h).
    const bool smoothEnabledH50=!directHandsR135&&kcd2vr::hand_smooth_h50::enabled.load(std::memory_order_relaxed);
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
    // R150 HAND OFFSETS (menu): applied in the controller's own grip space (OpenXR X right, Y up, -Z forward)
    // so the 1:1, legacy, palm and raw-goal paths below all see one consistently offset controller.
    // R153 HAND ALIGN: while aligning, the hand is held at the frozen pose; squeezing grip solves the offsets so the
    // current controller pose + offsets lands exactly on it (offset = controller^-1 * frozen).
    auto handOffsetR150=[&](int side,V3& p,Q4& q,bool primary){
        const V3 rawP=p;const Q4 rawQ=q;
        float o[6];for(int i=0;i<6;++i)o[i]=g_hand_offset_r150[side*6+i].load(std::memory_order_relaxed);
        if(o[0]!=0.f||o[1]!=0.f||o[2]!=0.f||o[3]!=0.f||o[4]!=0.f||o[5]!=0.f){
            const Q4 q0=q;
            auto axis=[](float x,float y,float z,float deg){const float h=deg*(3.14159265f/360.f);const float sn=std::sin(h);return Q4{x*sn,y*sn,z*sn,std::cos(h)};};
            // yaw about up, then pitch about right, then roll about forward (-Z); local post-multiply.
            q=QNormalize(QMul(q0,QMul(axis(0,1,0,o[4]),QMul(axis(1,0,0,o[3]),axis(0,0,-1,o[5])))));
            p=Add(p,QRotateV(q0,V3{o[0]*.01f,o[1]*.01f,-o[2]*.01f}));
        }
        float* fz=g_hand_align_frozen_r153[side];
        if(primary){
            const int req=g_hand_align_req_r153[side].exchange(0,std::memory_order_acq_rel);
            if(req==2)g_hand_align_state_r153[side].store(0,std::memory_order_release);
            else if(req==1&&(side?ts.left_valid:ts.right_valid)){
                fz[0]=p.x;fz[1]=p.y;fz[2]=p.z;fz[3]=q.x;fz[4]=q.y;fz[5]=q.z;fz[6]=q.w;
                g_hand_align_state_r153[side].store(1,std::memory_order_release);
                diagnostic_trace::Write(side?"R153 HAND_ALIGN L frozen; release grip, align controller, squeeze grip":"R153 HAND_ALIGN R frozen; release grip, align controller, squeeze grip");
            }
        }
        const int st=g_hand_align_state_r153[side].load(std::memory_order_acquire);
        if(!st)return;
        const V3 fp{fz[0],fz[1],fz[2]};const Q4 fq=QNormalize(Q4{fz[3],fz[4],fz[5],fz[6]});
        p=fp;q=fq;
        if(!primary||!(side?ts.left_valid:ts.right_valid))return;
        const float grip=side?ts.left_grip:ts.right_grip;
        if(st==1){if(grip<.3f)g_hand_align_state_r153[side].store(2,std::memory_order_release);return;}
        if(grip<.8f)return;
        const Q4 inv=QConj(QNormalize(rawQ));
        const V3 local=QRotateV(inv,Sub(fp,rawP));
        const Q4 r=QNormalize(QMul(inv,fq)); // = Ry(yaw) * Rx(pitch) * Rz(-roll)
        const float m02=2.f*(r.x*r.z+r.w*r.y),m22=1.f-2.f*(r.x*r.x+r.y*r.y),m12=2.f*(r.y*r.z-r.w*r.x);
        const float m10=2.f*(r.x*r.y+r.w*r.z),m11=1.f-2.f*(r.x*r.x+r.z*r.z);
        constexpr float deg=180.f/3.14159265f;
        const float solved[6]{local.x*100.f,local.y*100.f,-local.z*100.f,
            std::asin(std::clamp(-m12,-1.f,1.f))*deg,std::atan2(m02,m22)*deg,-std::atan2(m10,m11)*deg};
        bool clipped=false;
        for(int i=0;i<6;++i){const float lim=i<3?20.f:180.f;const float v=std::isfinite(solved[i])?solved[i]:0.f;
            const float c=std::clamp(std::round(v*(i<3?2.f:1.f))/(i<3?2.f:1.f),-lim,lim);clipped|=std::fabs(c-v)>(i<3?.3f:.6f);
            g_hand_offset_r150[side*6+i].store(c,std::memory_order_relaxed);}
        g_hand_align_state_r153[side].store(0,std::memory_order_release);
        g_hand_align_serial_r153.fetch_add(1,std::memory_order_acq_rel);
        char m[256]{};std::snprintf(m,sizeof(m),"R153 HAND_ALIGN %c solved pos=(%+.1f,%+.1f,%+.1f)cm rot=(p%+.0f,y%+.0f,r%+.0f)deg clipped=%d",
            side?'L':'R',solved[0],solved[1],solved[2],solved[3],solved[4],solved[5],clipped?1:0);diagnostic_trace::Write(m);
    };
    for(int side=0;side<2;++side)handOffsetR150(side,visPH50[side],visQH50[side],true);
    V3 palmOffsetD68[2]{},rawPalmOffsetD68[2]{}; // local copies only for the selected Shift+F2 burst
    const float wristBack59c7f=directHandsR135?0.f:g_wrist_from_grip59c7f.load(std::memory_order_relaxed);
    const V3 rightXr=wristBack59c7f!=0.f?Add(visPH50[0],QRotateV(visQH50[0],V3{0,0,wristBack59c7f})):visPH50[0];
    const V3 leftXr=wristBack59c7f!=0.f?Add(visPH50[1],QRotateV(visQH50[1],V3{0,0,wristBack59c7f})):visPH50[1];
    const V3 handAnchor51=directHandsR135?directHandAnchorR142:headAnchor123; // camera placement is authoritative in direct mode
    const Q4 activeFrame51=directHandsR135?directHandFrameR142:frame;
    auto trackedGoal51=[&](V3 hand){
        V3 offset=QRotateV(activeFrame51,XrToCryV(Sub(hand,directHandsR135?ts.head:trackedBase)));
        // Model-space positions are multiplied by character world scale on the
        // way to the camera/world. Invert it so controller travel remains 1:1 m.
        if(directHandsR135&&directCameraAnchorR142)offset=Mul(offset,1.f/directHandModelScaleR142);
        return Add(handAnchor51,offset);
    };
    V3 requestedGoals[2]{trackedGoal51(rightXr),trackedGoal51(leftXr)};
    // R59.16C8 AUTO PALM: wrist goal = grip pose - half of (wrist -> knuckle bar) in the desired hand orientation (Henry's own
    // finger skeleton; PalmGeometry103 out[0] is the knuckle-bar centre relative to the wrist). The grip pose is the palm centre,
    // so the palm lands on the controller and the native weapon socket (at the wrist bone) sits one palm-length behind it.
    if(g_auto_palm59c8.load(std::memory_order_relaxed)){
        for(int side=0;side<2;++side){
            if(!(side?ts.left_valid:ts.right_valid))continue;
            V3 six[6]{};FingerContactGeometry103 g{};
            const Q4 o=DesiredHand6689(s,side,activeFrame51,visQH50[side]);
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
        V3 p=side?ts.left:ts.right;Q4 q=side?ts.lq:ts.rq;handOffsetR150(side,p,q,false); // R150
        if(p.x==visPH50[side].x&&p.y==visPH50[side].y&&p.z==visPH50[side].z&&q.x==visQH50[side].x&&q.y==visQH50[side].y&&
           q.z==visQH50[side].z&&q.w==visQH50[side].w)continue; // not smoothed: raw == visual already
        const V3 xr=wristBack59c7f!=0.f?Add(p,QRotateV(q,V3{0,0,wristBack59c7f})):p;
        requestedGoalsRawH50[side]=trackedGoal51(xr);
        if(tx.motionSessionD68)rawPalmOffsetD68[side]={}; // the raw rebuild discarded the visual palm term
        if(g_auto_palm59c8.load(std::memory_order_relaxed)&&(side?ts.left_valid:ts.right_valid)){
            V3 six[6]{};FingerContactGeometry103 g{};
            if(PalmGeometry103(s,side,DesiredHand6689(s,side,activeFrame51,q),0.f,0.f,0,six,g)){
                const V3 back=Mul(six[0],-.5f);const float cm=Len(back)*100.f;
                if(std::isfinite(cm)&&cm<=12.f){requestedGoalsRawH50[side]=Add(requestedGoalsRawH50[side],back);
                    if(tx.motionSessionD68)rawPalmOffsetD68[side]=back;}
            }
        }
    }
    V3 goals[2]{requestedGoals[0],requestedGoals[1]};
    const bool valid[2]{ts.right_valid&&(!tx.reloadWorkingHandXR34||reloadHandPolicyR95.tracked[0]),
        ts.left_valid&&(!tx.reloadWorkingHandXR34||reloadHandPolicyR95.tracked[1])};
