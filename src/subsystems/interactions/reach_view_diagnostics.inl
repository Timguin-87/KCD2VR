static void ReachView38Tick(bool poseAllowed,bool headValid,int census) noexcept {
    namespace rv=kcd2vr::reach_view38;namespace cap=kcd2vr::diagnostics::capture168;
    const auto now=std::uint64_t(GetTickCount64());
    static rv::Gap gap{};static rv::Flag eyeOut{},overReach[2]{},behind[2]{},wsErr{};
    static std::uint64_t lastCalls=0,frames=0,calls=0;static int lastWs=-1,lastAnchor=-1,lastCap=-1;
    if(!cap::active(cap::Domain::Interaction,now)){gap={};eyeOut={};overReach[0]=overReach[1]={};behind[0]=behind[1]={};wsErr={};
        lastCalls=frames=calls=0;lastWs=lastAnchor=lastCap=-1;return;}
    TraceHenryBounds63(now);
    rv::Evidence e{};const bool have=g_reach_view38.read(e)&&e.tick&&now>=e.tick&&now-e.tick<=1500;
    const auto c=g_correlated_calls.load(std::memory_order_relaxed);calls+=lastCalls&&c>=lastCalls?c-lastCalls:0;lastCalls=c;++frames;
    const double perFrame=frames?double(calls)/double(frames):0.;
    char geo[400]{};std::snprintf(geo,sizeof(geo),
        "eyeOutsideNativeBonesM=%.3f nativeBodyBehindEye=%d handAheadOfEyeM R/L=%.3f/%.3f handOutsideNativeM R/L=%.3f/%.3f reachAsk R/L=%.2f/%.2f reachLimited R/L=%d/%d full=%d workstation=%d poseOnly=%d sample=%llu",
        e.eyeOutside,e.nativeBehindEye?1:0,e.handAhead[0],e.handAhead[1],e.handOutside[0],e.handOutside[1],e.reachAsk[0],e.reachAsk[1],
        e.reachLimited[0]?1:0,e.reachLimited[1]?1:0,e.full?1:0,e.workstation?1:0,e.dialogueBody?1:0,(unsigned long long)e.sample);
    char m[720]{};
    const bool eligible=have&&poseAllowed&&headValid&&census>=2&&g_cal_state.load(std::memory_order_acquire)==2;
    const int g=rv::gap(gap,now,g_henry_skinning_tick.load(std::memory_order_acquire),eligible,120);
    if(g){std::snprintf(m,sizeof(m),"C38 F5_REACH_VIEW edge=%s %s=%llu henrySkinCallsPerTrackFrame=%.2f %s meaning=%s",
        g>0?"HENRY_SKIN_GAP_START":"HENRY_SKIN_GAP_END",g>0?"lastSkinAgeMs":"gapMs",
        (unsigned long long)(g>0?(now>=gap.since?now-gap.since:0):(gap.lastSkin>=gap.since?gap.lastSkin-gap.since:0)),perFrame,geo,
        g>0?"HENRY_NOT_SKINNED_WHILE_TRACKING(culled_or_not_rendered;VRIK_write_skipped)":eligible?"HENRY_SKINNED_AGAIN":"VRIK_NOT_ELIGIBLE(menu/tracking/calibration)");diagnostic_trace::Write(m);}
    if(!have)return;
    const int o=rv::edge(eyeOut,e.eyeOutside,.10,.06,now);
    if(o){std::snprintf(m,sizeof(m),"C38 F5_REACH_VIEW edge=%s henrySkinCallsPerTrackFrame=%.2f(since previous edge) %s note=game_visibility_uses_native_pose(VRIK_writes_at_skinning)",
        o>0?"EYE_LEFT_NATIVE_BODY":"EYE_BACK_IN_NATIVE_BODY",perFrame,geo);diagnostic_trace::Write(m);frames=calls=0;}
    for(int side=0;side<2;++side){
        if(!e.valid[side]){overReach[side]={};behind[side]={};continue;}
        const int r=rv::edge(overReach[side],e.reachAsk[side],1.0,.97,now);
        if(r){std::snprintf(m,sizeof(m),"C38 F5_REACH_VIEW edge=%s side=%c clamp=FULL_EXTENSION_ALONG_CONTROLLER %s",r>0?"OVER_REACH_START":"OVER_REACH_END",side?'L':'R',geo);diagnostic_trace::Write(m);}
        const int b=rv::edge(behind[side],-e.handAhead[side],.02,-.02,now);
        if(b){std::snprintf(m,sizeof(m),"C38 F5_REACH_VIEW edge=%s side=%c %s",b>0?"HAND_BEHIND_EYE_PLANE_START":"HAND_BEHIND_EYE_PLANE_END",side?'L':'R',geo);diagnostic_trace::Write(m);}
    }
    // What anchors the VRIK head at a workstation (alchemy/smithing): entry/exit, anchor or raise-cap change, head error edge.
    const int ws=e.workstation?1:0,anchor=e.liveAnchor?1:0,capped=e.raiseCapped?1:0;
    const int he=ws?rv::edge(wsErr,e.headErr,.05,.03,now):0;if(!ws)wsErr={};
    if(ws!=lastWs||(ws&&(anchor!=lastAnchor||capped!=lastCap||he))){
        std::snprintf(m,sizeof(m),"C38 F5_WORKSTATION_HEAD edge=%s anchor=%s raiseCapped=%d torso=%s nativeHeadFromCalM=(%.3f,%.3f,%.3f) hmdDeltaM=(%.3f,%.3f,%.3f) headTarget=(%.3f,%.3f,%.3f) solvedHead=(%.3f,%.3f,%.3f) headErrM=%.3f chest=(%.3f,%.3f,%.3f) camera=NATIVE_CAMERA+HMD(unless_C10R19_ACTIVITY_EYE_result=APPLIED) sample=%llu",
            ws!=lastWs?(ws?"ENTER":"EXIT"):he>0?"HEAD_ERROR_OVER_5CM":he<0?"HEAD_ERROR_BACK":"ANCHOR_OR_CAP_CHANGED",
            anchor?"LIVE_NATIVE_HEAD+HMD":"CALIBRATION_HEAD+HMD",capped,e.full?"SPINE_SOLVED_TO_HEAD(F10_rung3)":"NATIVE(F10<3)",
            e.nativeHeadFromCal.x,e.nativeHeadFromCal.y,e.nativeHeadFromCal.z,e.hmdDelta.x,e.hmdDelta.y,e.hmdDelta.z,
            e.headTarget.x,e.headTarget.y,e.headTarget.z,e.solvedHead.x,e.solvedHead.y,e.solvedHead.z,e.headErr,e.chest.x,e.chest.y,e.chest.z,
            (unsigned long long)e.sample);diagnostic_trace::Write(m);
        lastWs=ws;lastAnchor=anchor;lastCap=capped;
    }
}
