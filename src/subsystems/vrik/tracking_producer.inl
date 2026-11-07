void PublishTrackingSnapshot(const TrackingPose& head,bool hv,const TrackingPose& left,bool lv,const TrackingPose& right,bool rv,float leftGrip,float rightGrip,float leftTrigger,float rightTrigger,bool allowed,std::uint64_t sourceTick,std::uint64_t sourceNs,std::uint64_t sourceEpoch,std::int64_t poseTime,bool dialogueFingersD78,float leftVisualTriggerD78,float rightVisualTriggerD78) noexcept {
    const auto poseNow=sourceTick?sourceTick:static_cast<std::uint64_t>(GetTickCount64());
    const auto validPose=[](const TrackingPose& p){
        const double n=double(p.qx)*p.qx+double(p.qy)*p.qy+double(p.qz)*p.qz+double(p.qw)*p.qw;
        return std::isfinite(p.px)&&std::isfinite(p.py)&&std::isfinite(p.pz)&&std::isfinite(n)&&n>.90&&n<1.10;
    };
    hv=hv&&validPose(head);lv=lv&&validPose(left);rv=rv&&validPose(right);
    // C55: acquisition must see ORIGINAL input before Picking's gameplay gate
    // or presentation grace. Waiting for PublishCommitted6689/HandFrame here
    // made early action ownership depend on the pose it was meant to enable.
    // This copied witness has no item/chest/placement or native-call authority.
    const auto inputNow55=std::uint64_t(GetTickCount64());
    kcd2vr::grasp13::AcquisitionInput55 input55{};
    input55.tick=sourceTick;input55.sample=sourceNs;input55.trackingEpoch=sourceEpoch;
    input55.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    input55.character=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
    const auto sameInputBody55=[&](){return input55.character&&input55.epoch&&
        input55.character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
        input55.character+8==reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))&&
        input55.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        sourceEpoch&&sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire);};
    const auto ordinaryInput55=[&](){return !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!activity_input163::witness175.Active()&&
        !WorkstationContextRefusal19c4r2(inputNow55,true)&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        !g_kick_busy176.load()&&!diagnostics::lab::NativeReference();};
    const bool originalInput55=sourceTick&&sourceNs&&sourceNs<=tracking59c6::NowNs()&&
        inputNow55>=sourceTick&&sameInputBody55()&&ordinaryInput55();
    input55.headTracked=originalInput55&&hv&&inputNow55-sourceTick<=60;
    input55.tracked[0]=originalInput55&&rv&&std::isfinite(rightGrip)&&inputNow55-sourceTick<=80;
    input55.tracked[1]=originalInput55&&lv&&std::isfinite(leftGrip)&&inputNow55-sourceTick<=80;
    input55.grip[0]=rightGrip;input55.grip[1]=leftGrip;
    if(!sameInputBody55()){input55.headTracked=false;input55.tracked[0]=input55.tracked[1]=false;}
    // Observe the current raw Grip before asking whether its request can hold.
    // Otherwise one released sample disables the reader that must see re-squeeze.
    if(g_graspInput55)g_graspInput55(input55);
    kcd2vr::grasp13::Acquisition54 picking55{};
    const bool acquisitionPose55=g_graspInput55&&
        kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,picking55)&&picking55.active&&
        kcd2vr::grasp13::freshAcquisition54(picking55,inputNow55)&&
        picking55.character==input55.character&&picking55.epoch==input55.epoch&&
        picking55.trackingEpoch55==sourceEpoch&&input55.headTracked&&input55.tracked[picking55.side]&&
        input55.grip[picking55.side]>.25f&&sameInputBody55()&&ordinaryInput55()&&
        visual_selection59c7f::Selected(g_cal_state.load()==2,g_census_mode.load(),g_mode.load());
    // R95: normal HELD adoption retires the old Picking action. Keep only the
    // exact item's visible body foundation through an independent policy-cache
    // delay. The reader requires fresh raw Grip published above; no old action
    // or cached visual timestamp can grant gameplay/native execution.
    kcd2vr::grasp13::FinishingPresentationR94 adopted55{};
    const bool adoptedPoseR95=action_time_owner59c11::ReadAdoptedAttachmentPresentationR95(adopted55)&&
        adopted55.binding.character==input55.character&&adopted55.binding.epoch==input55.epoch&&
        adopted55.trackingEpoch==sourceEpoch&&input55.headTracked&&
        input55.tracked[adopted55.binding.side]&&input55.grip[adopted55.binding.side]>.25f&&
        sameInputBody55()&&ordinaryInput55()&&
        visual_selection59c7f::Selected(g_cal_state.load()==2,g_census_mode.load(),g_mode.load());
    if(!allowed||!hv||!lv||!rv)animation_filter::combatPoseTick112.store(0,std::memory_order_release);
    // A procedural kick never extends tracking grace or resumes after protected
    // native body ownership. This path also runs when pose work is skipped.
    const bool kickPendingH17=g_kick_busy176.load()||kcd2vr::kick26::requestTick.load()||g_special_pending168.load();
    TrackingSnapshot kickPreviousH17{};
    const auto kickSourceH17=sourceEpoch?sourceEpoch:tracking59c6::epoch.load(std::memory_order_acquire);
    const bool kickSourceChangedH17=kickPendingH17&&g_tracking_frames6695.read(kickPreviousH17)&&
        kickPreviousH17.source_epoch&&kickPreviousH17.source_epoch!=kickSourceH17;
    if(kickPendingH17&&(!allowed||!hv||!lv||!rv||animation_filter::nativeBodyOwned112.load()||kickSourceChangedH17))CancelProceduralKick176();
    else if(g_kick_busy176.load()&&
       kcd2vr::kick26::NativeBlocksVrLeg(animation_filter::NativeKickActive175()))CancelKickVisualH17(); // native GAME kick keeps its original clip service; only its competing VR leg retires
    // Only this exact installed Picking may retain tracking ticks and its
    // completed pose. ReadTrackingSnapshot still returns allowed=false for
    // generic consumers; BeginCoherent6689 joins the native acquisition again.
    const bool poseAllowed=kcd2vr::reading_pose32::poseAllowed(allowed,g_dialogue_body178r9.load(std::memory_order_relaxed),GetTickCount64())||acquisitionPose55||adoptedPoseR95;
    if(!poseAllowed){g_head_valid_tick.store(0);g_left_valid_tick.store(0);g_right_valid_tick.store(0);}
    const bool headUsable=hv||tracking_grace111::KeepLastGood(false,poseNow,g_head_valid_tick.load(std::memory_order_relaxed),60);
    const bool rightUsable=rv||tracking_grace111::KeepLastGood(false,poseNow,g_right_valid_tick.load(std::memory_order_relaxed),80);
    kcd2vr::haptics101::allowed.store(allowed&&headUsable,std::memory_order_release);
    kcd2vr::haptics101::service_test(poseNow);
    // C10R19: the dialogue/workstation body (V66.178R9) keeps its committed pose valid; gameplay consumers still check g_gameplay_allowed.
    if(!poseAllowed||!headUsable||!rightUsable)g_committed_pose_valid6689.store(false,std::memory_order_release);
    const int guidedPose=g_guided_cal_pose.load(std::memory_order_acquire);
    const auto cv=[](const TrackingPose&p){return kcd2vr::pose_core::Vec3{p.px,p.py,p.pz};};
    const auto qvalid=[](const TrackingPose&p){const double n=double(p.qx)*p.qx+double(p.qy)*p.qy+double(p.qz)*p.qz+double(p.qw)*p.qw;return std::isfinite(n)&&n>.90&&n<1.10;};
    const bool calSample=allowed&&hv&&lv&&rv&&qvalid(head)&&qvalid(left)&&qvalid(right);
    if(guidedPose>0&&!g_guided_cal_lock.test_and_set(std::memory_order_acquire)){
      if(!calSample){if(g_arm_capture_active66100.load())g_arm_window66100.moved=true;
        (!allowed?g_cal_samples_blocked_r132:g_cal_samples_lost_r132).fetch_add(1,std::memory_order_relaxed); // R132: failure reason
        g_guided_cal_lock.clear(std::memory_order_release);}
      else {
        g_cal_samples_ok_r132.fetch_add(1,std::memory_order_relaxed);
        if(g_arm_capture_active66100.load())g_arm_window66100.add(cv(head),cv(left),cv(right),GetTickCount64());
        ++g_guided_cal_accum.n;
        g_guided_cal_accum.h.x+=head.px;g_guided_cal_accum.h.y+=head.py;g_guided_cal_accum.h.z+=head.pz;
        g_guided_cal_accum.l.x+=left.px;g_guided_cal_accum.l.y+=left.py;g_guided_cal_accum.l.z+=left.pz;
        g_guided_cal_accum.r.x+=right.px;g_guided_cal_accum.r.y+=right.py;g_guided_cal_accum.r.z+=right.pz;
        auto accQ=[](Q4 q,Q4& ref,Q4& sum,bool have) noexcept {
            q=QNormalize(q); if(!have) ref=q;
            const float d=q.x*ref.x+q.y*ref.y+q.z*ref.z+q.w*ref.w;
            if(d<0.0f) q={-q.x,-q.y,-q.z,-q.w};
            sum.x+=q.x;sum.y+=q.y;sum.z+=q.z;sum.w+=q.w;
        };
        const bool have=g_guided_cal_accum.haveQ;
        accQ(Q4{head.qx,head.qy,head.qz,head.qw},g_guided_cal_accum.hqRef,g_guided_cal_accum.hqSum,have);
        accQ(Q4{left.qx,left.qy,left.qz,left.qw},g_guided_cal_accum.lqRef,g_guided_cal_accum.lqSum,have);
        accQ(Q4{right.qx,right.qy,right.qz,right.qw},g_guided_cal_accum.rqRef,g_guided_cal_accum.rqSum,have);
        g_guided_cal_accum.haveQ=true;
        g_guided_cal_lock.clear(std::memory_order_release);
      }
    }
    g_track_seq.fetch_add(1,std::memory_order_acq_rel);
    if(hv){g_hp_x.store(head.px);g_hp_y.store(head.py);g_hp_z.store(head.pz);g_hq_x.store(head.qx);g_hq_y.store(head.qy);g_hq_z.store(head.qz);g_hq_w.store(head.qw);g_head_valid_tick.store(poseAllowed?poseNow:0,std::memory_order_relaxed);}
    if(lv){g_lp_x.store(left.px);g_lp_y.store(left.py);g_lp_z.store(left.pz);g_lq_x.store(left.qx);g_lq_y.store(left.qy);g_lq_z.store(left.qz);g_lq_w.store(left.qw);g_left_valid_tick.store(poseAllowed?poseNow:0,std::memory_order_relaxed);}
    if(rv){g_rp_x.store(right.px);g_rp_y.store(right.py);g_rp_z.store(right.pz);g_rq_x.store(right.qx);g_rq_y.store(right.qy);g_rq_z.store(right.qz);g_rq_w.store(right.qw);g_right_valid_tick.store(poseAllowed?poseNow:0,std::memory_order_relaxed);}
    g_left_grip.store(std::clamp(leftGrip,0.0f,1.0f)); g_right_grip.store(std::clamp(rightGrip,0.0f,1.0f));
    g_left_trigger.store(std::clamp(leftTrigger,0.0f,1.0f)); g_right_trigger.store(std::clamp(rightTrigger,0.0f,1.0f));
    g_head_valid.store(hv);g_left_valid.store(lv);g_right_valid.store(rv);g_gameplay_allowed.store(allowed);
    g_tracking_tick6689.store(poseNow,std::memory_order_relaxed);
    g_track_seq.fetch_add(1,std::memory_order_release);
    TrackingSnapshot ready{};
    ready.head={head.px,head.py,head.pz};ready.left={left.px,left.py,left.pz};ready.right={right.px,right.py,right.pz};
    ready.hq={head.qx,head.qy,head.qz,head.qw};ready.lq={left.qx,left.qy,left.qz,left.qw};ready.rq={right.qx,right.qy,right.qz,right.qw};
    ready.left_grip=g_left_grip.load();ready.right_grip=g_right_grip.load();
    ready.left_trigger=g_left_trigger.load();ready.right_trigger=g_right_trigger.load();
    ready.dialogueFingersD78=kcd2vr::dialogue_fingers_d78::capture(
        dialogueFingersD78&&sourceTick&&sourceNs&&sourceNs<=tracking59c6::NowNs()&&sourceEpoch&&
        sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&!allowed&&
        g_ctx_dialogue.load(std::memory_order_acquire)&&g_dialogue_body178r9.load(std::memory_order_acquire),
        lv,rv,leftVisualTriggerD78,rightVisualTriggerD78);
    ready.head_valid=hv;ready.left_valid=lv;ready.right_valid=rv;ready.allowed=poseAllowed; // V66.178R9: pose path may run in dialogue
    ready.timeNs=sourceNs?sourceNs:tracking59c6::NowNs();ready.poseTime=poseTime;
    ready.tick=poseNow;ready.sequence=tracking59c6::Next();
    ready.input_tick=poseNow;ready.source_epoch=sourceEpoch?sourceEpoch:tracking59c6::epoch.load(std::memory_order_acquire);
    LifecycleTracking36(ready,allowed); // raw validity/input before presentation grace
    TrackingSnapshot previous{};(void)g_tracking_frames6695.read(previous);
    tracking_grace111::HoldCoherent112(ready,previous);
    const bool publishedD68=g_tracking_frames6695.publish(ready);
    // The permission/button packet is separate from the latest pose family.
    // bit16 distinguishes it; its original input_tick is never refreshed here.
    if(kcd2vr::diagnostics::motion_d68::requested()){
        namespace mt=kcd2vr::diagnostics::motion_d68;mt::Event e{};
        static thread_local mt::Cadence cadence;
        if(mt::begin(e,mt::InputPublished)&&mt::inWindow(e)){
            if(cadence.due(e.session,e.ns)){
                e.flags=0x10000u|unsigned(publishedD68)|(unsigned(ready.allowed)<<17)|
                    (unsigned(ready.head_valid)<<1)|(unsigned(ready.left_valid)<<2)|(unsigned(ready.right_valid)<<3)|
                    (unsigned(ready.head_held)<<10)|(unsigned(ready.left_held)<<11)|(unsigned(ready.right_held)<<12);
                e.sourceNs=ready.timeNs;e.sourceTick=ready.tick;e.inputTick=ready.input_tick;e.sample=ready.sequence;
                e.trackingEpoch=ready.source_epoch;e.targetXrNs=ready.poseTime;e.thread=GetCurrentThreadId();e.endNs=e.ns;
                const float v[]{ready.head.x,ready.head.y,ready.head.z,ready.hq.x,ready.hq.y,ready.hq.z,ready.hq.w,
                    ready.left.x,ready.left.y,ready.left.z,ready.lq.x,ready.lq.y,ready.lq.z,ready.lq.w,
                    ready.right.x,ready.right.y,ready.right.z,ready.rq.x,ready.rq.y,ready.rq.z,ready.rq.w};
                std::copy(std::begin(v),std::end(v),e.v);e.count=21;mt::push(e);
            }else mt::noteCadenceSkip(mt::InputPublished);
        }
    }
    const int mode=g_mode.load(std::memory_order_acquire),census=g_census_mode.load(std::memory_order_acquire);
    ReachView38Tick(poseAllowed,hv,census); // C38: F5 rung 1 edges only; returns at once otherwise
    const bool needRight=mode>=2 || census>=2;
    const bool needLeft=census>=2;
    // V65.12: tracking loss only suppresses writes for that frame. A T-pose
    // calibration is persistent until the next explicit recenter calibration.
    (void)needRight; (void)needLeft;
}
