static void ApplyRoomscalePad58(native_vr_pad_premap::State& pad,const native_vr_pad_premap::Context& padCtx,bool trackedGameplay) noexcept
{
    static kcd2vr::hmd_dodge_h24::Request dodgeRequestH24{};
    // R59.16C5: FOLLOW and DODGE are separate. Before, the dodge needed FOLLOW + 6DOF BODY, the F4 experiment, a
    // guard/parry-resolved opponent, AND an untouched left stick - so moving with the stick in a fight disabled it.
    const bool context=!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&!padCtx.semantic_minigame&&
        !padCtx.apse_ui&&!padCtx.back_context&&trackedGameplay&&!animation_filter::nativeBodyOwned112.load();
    const bool followOn=g_roomscale_follow58.load(std::memory_order_relaxed)&&frik_pose_probe::BodyFollow128();
    const bool dodgeOn=g_roomscale_dodge58.load(std::memory_order_relaxed); // C54: explicit Immersion switch owns dodge; F4 cannot override OFF
    const bool stickFree=std::abs(pad.lx)<.01f&&std::abs(pad.ly)<.01f; // the stick wins over FOLLOW, never over a dodge
    if(!context||(!followOn&&!dodgeOn)){dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();(void)CameraHook::RoomscaleDrive(GetTickCount64(),false,false,false);return;}
    const auto rsNow=std::uint64_t(GetTickCount64());std::uint64_t rsTick=0;float rx=0,ry=0;
    static std::uint64_t dodgeEpochH24=0;static void* dodgeEntityH24=nullptr;
    const auto currentEpochH24=frik_pose_probe::PoseEpochH24();void* currentEntityH24=frik_pose_probe::LastPlayerEntity();
    if(dodgeEpochH24!=currentEpochH24||dodgeEntityH24!=currentEntityH24){
        dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();
        (void)CameraHook::RoomscaleDrive(rsNow,false,false,false);
        dodgeEpochH24=currentEpochH24;dodgeEntityH24=currentEntityH24;
    }
    if(CameraHook::RoomscaleRemaining(rx,ry,rsTick)&&rsTick&&rsNow>=rsTick&&rsNow-rsTick<=60){
        // H24: do not spend the gesture/cooldown on an idle combat lean.
        // Original-first publication proves the current reciprocal opponent is
        // attacking now; the transient inCombat byte alone cannot do that.
        const bool combatLocked=combat_diag_probe::IncomingHmdDodgeH24(rsNow);
        const auto d=CameraHook::RoomscaleDrive(rsNow,followOn&&stickFree,combatLocked,dodgeOn);
        if(d.dodge){
            namespace hd=kcd2vr::hmd_dodge_h24;
            hd::Request live{};
            if(!dodgeRequestH24.serial){
                if(!hd::beginRequest(reinterpret_cast<std::uintptr_t>(currentEntityH24),currentEpochH24,rsNow,rsTick,dodgeRequestH24))return;
            }else if(!hd::currentRequest(reinterpret_cast<std::uintptr_t>(currentEntityH24),
                dodgeRequestH24.owner.actor,dodgeRequestH24.owner.state,currentEpochH24,rsNow,live)){
                hd::cancelRequest();return; // never renew a failed/expired gesture into a new held request
            }else{dodgeRequestH24.poseTick=rsTick;(void)hd::requestPublished.publish(dodgeRequestH24);}
            if(!hd::releaseReady(dodgeRequestH24)){
                if(rsNow-dodgeRequestH24.started>60)hd::cancelRequest();
                return; // our native LT owner must acknowledge actual release first
            }
            pad.b=true;pad.lx=d.dodgeLx;pad.ly=d.dodgeLy;
            static std::uint64_t lastDodgeLog58=0;if(rsNow-lastDodgeLog58>=500){lastDodgeLog58=rsNow;char m[200]{};std::snprintf(m,sizeof(m),"R59.16C5 ROOMSCALE_DODGE dir=(%.2f,%.2f) transport=XI_B_PLUS_STICK follow=%d acceptedDodge=UNPROVEN (kcd.log decides perfect dodge)",double(d.dodgeLx),double(d.dodgeLy),followOn?1:0);diagnostic_trace::Write(m);}}
        else {dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();}
        if(!d.dodge&&d.following){pad.lx=std::clamp(pad.lx+d.lx,-1.f,1.f);pad.ly=std::clamp(pad.ly+d.ly,-1.f,1.f);}
    }
    else {dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();(void)CameraHook::RoomscaleDrive(rsNow,false,false,false);}
    static std::uint64_t lastRoomLog58=0;
    if(rsNow-lastRoomLog58>=5000&&diagnostics::capture168::any(rsNow)){lastRoomLog58=rsNow;std::uint64_t consumed=0,dodges=0,injected=0;float ax=0,ay=0;
        CameraHook::RoomscaleCounters(consumed,dodges,injected,ax,ay);char m[220]{};
        std::snprintf(m,sizeof(m),"V66.178R58 ROOMSCALE remaining=(%.2f,%.2f) anchor=(%.2f,%.2f) consumedFrames=%llu injectedFrames=%llu dodges=%llu",double(rx),double(ry),double(ax),double(ay),(unsigned long long)consumed,(unsigned long long)injected,(unsigned long long)dodges);diagnostic_trace::Write(m);}
}

