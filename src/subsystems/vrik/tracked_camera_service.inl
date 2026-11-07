// E1: the original body is the core; TrackedFrameCamera55 (below) holds its last
// accepted base across a short gap instead of handing camera_hook the native eye.
static kcd2vr::core::SnapshotMailbox<kcd2vr::camera_hold_e1::Sample> g_camera_hold_e1;
// R8d2 (Codex P1): the hold may only cover PLAIN gameplay. Evaluated on the CURRENT state, after the current-root
// read, right before a held base is published. picking/workstation never hold across (their identity is not cached).
static bool PlainGameplayCameraE1() noexcept {
    kcd2vr::grasp13::Acquisition54 acquisition{};
    const bool picking=kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,acquisition)&&acquisition.active;
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 workstation{};
    const bool workstationKind=kcd2vr::workstation178r20c1::workstationOwnerH51(GetTickCount64(),workstation);
    return !picking&&!workstationKind&&CombatCensusMode()>=3&&g_cal_state.load()==2&&g_gameplay_allowed.load()&&g_head_valid.load()&&
        !diagnostics::lab::NativeReference()&&!g_ctx_dialogue.load()&&!g_ctx_menu.load()&&!g_ctx_cutscene.load()&&
        !g_ctx_scripted.load()&&!g_ctx_reading.load()&&!animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175();
}
// D55 opt-in: a current exact controller-owned Picking action can borrow a
// plain anatomical base or its OWN accepted base. Stations/native owners cannot.
static kcd2vr::camera_hold_e1::ContextD55 CameraHoldContextD55() noexcept {
    using C=kcd2vr::camera_hold_e1::ContextD55;
    if(g_pose_restore_fault6689.load()||!TrackingHeadUsable111())return {};
    const auto tracking=tracking59c6::epoch.load(std::memory_order_acquire);
    if(PlainGameplayCameraE1())return C{1,tracking};
    const auto now=std::uint64_t(GetTickCount64());kcd2vr::grasp13::Acquisition54 p{};
    if(CombatCensusMode()<3||g_cal_state.load()!=2||diagnostics::lab::NativeReference()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       WorkstationContextRefusal19c4r2(now,true)||!kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,p)||
       !p.active||!kcd2vr::grasp13::freshAcquisition54(p,now)||!p.generation55||
       p.character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
       p.character+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load())||
       p.epoch!=g_pose_epoch6689.load()||!tracking||p.trackingEpoch55!=tracking)return {};
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 station{};
    if(kcd2vr::workstation178r20c1::workstationOwnerH51(now,station))return {};
    return C{2,tracking,p.serial,p.generation55,p.action,p.scope,p.nativeToken,p.side};
}
static void CameraGapReceiptD55(const kcd2vr::camera_hold_e1::Sample& s,
    const kcd2vr::camera_hold_e1::ContextD55& context,kcd2vr::camera_hold_e1::Why why,
    kcd2vr::pose_core::Vec3 eye,std::uint64_t now) noexcept {
    namespace ic=diagnostics::interaction_cycle_h4;
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    ic::Event e{};e.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    e.tick=now;e.stage=ic::Stage::Lifecycle;e.code=int(why);e.actor=s.actor;e.poseEpoch=s.epoch;
    e.sourceTick=s.tick;e.action=context.action;e.token=context.serial;e.generation=context.generation;e.side=context.side;
    e.trackingEpoch=context.tracking;e.entity=context.scope;e.target=context.token;e.collector=s.control;
    e.value=float(context.kind);e.point[0]=float(eye.x);e.point[1]=float(eye.y);e.point[2]=float(eye.z);
    ic::copy(e.reason,sizeof(e.reason),kcd2vr::camera_hold_e1::name(why));
    ic::copy(e.label,sizeof(e.label),"D55_CAMERA_BRIDGE_NOT_FINAL_RENDER");
    static thread_local ic::Event previous{};
    if(previous.session!=e.session||previous.code!=e.code||previous.actor!=e.actor||previous.poseEpoch!=e.poseEpoch||
       previous.token!=e.token||previous.action!=e.action||previous.generation!=e.generation||
       now<previous.tick||now-previous.tick>=250){if(ic::observe(e,now))previous=e;}
}
static bool TrackedFrameCameraCoreE1(float x,float y,float z,float fx,float fy,float fz,
                          float& ox,float& oy,float& oz,float& hx,float& hy,bool& admittedE1,bool pickupBridgeD55=false,bool* retryPublicationD56=nullptr,bool* pickupHandoverD64=nullptr,kcd2vr::native_roomscale_d76::Origin* roomOriginD76=nullptr) noexcept {
    hx=hy=0;admittedE1=false;if(retryPublicationD56)*retryPublicationD56=false;
    if(pickupHandoverD64)*pickupHandoverD64=false;
    struct FailedCamera48 {bool success=false;
        diagnostics::interaction_cycle_h4::Event interaction{};
        const char* reason="TRACKED_CAMERA_BASE_UNAVAILABLE_FLAGS_NATIVE_VALID_MENU_SCRIPT_DIALOGUE";
        ~FailedCamera48(){
        // D53: F5 alone previously missed the camera side of pickup handovers.
        // Copy this EXISTING attempt to its bounded worker queue; no extra native
        // reads, camera writes, interpolation, or ownership changes. The wrapper
        // may still supply its qualified short hold: this is NOT final render.
        namespace ic=diagnostics::interaction_cycle_h4;
        const auto now=std::uint64_t(GetTickCount64());
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
            static thread_local ic::Event previous{};
            const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
            interaction.session=session;interaction.tick=now;interaction.stage=ic::Stage::Lifecycle;interaction.code=success?1:0;
            ic::copy(interaction.reason,sizeof(interaction.reason),success?"ACCEPTED_TRACKED_BODY_CAMERA_BASE":reason);
            ic::copy(interaction.label,sizeof(interaction.label),"D53_CAMERA_CORE_NOT_FINAL_RENDER");
            const bool changed=previous.session!=session||previous.code!=interaction.code||
                previous.actor!=interaction.actor||previous.poseEpoch!=interaction.poseEpoch||
                previous.token!=interaction.token||previous.action!=interaction.action||
                std::strcmp(previous.reason,interaction.reason)!=0;
            if(changed||now<previous.tick||now-previous.tick>=250){
                if(ic::observe(interaction,now))previous=interaction;
            }
        }
        if(!success&&diagnostics::ownership48::active(GetTickCount64())){
            diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::CameraBase;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();e.code=0;e.epoch=g_pose_epoch6689.load();e.publication=g_frame_publication51.load();
            e.flags=(animation_filter::nativeBodyOwned112.load()?1u:0u)|(g_committed_pose_valid6689.load()?2u:0u)|(g_ctx_menu.load()?4u:0u)|(g_ctx_scripted.load()?8u:0u)|(g_ctx_dialogue.load()?16u:0u);diagnostics::ownership48::name(e,reason);(void)diagnostics::ownership48::push(e);
        }
    }} failedCamera48;

    static std::mutex mutex;static kcd2vr::tracked_frame51::EyeState state{};
    // Neutral measurement follows the native lifecycle, not the later control
    // thread's minigame-kind cache. Start can call an authored view before its
    // post-original Begin publication; its existing transition scope covers it.
    const auto nativeActivityEyeH51=[](){const auto& activity=kcd2vr::activity_input163::witness175;
        namespace mh=kcd2vr::minigame_hands45;
        const bool alchemy=mh::freeTrackedMode(mh::modeFor(mh::Alchemy))||kcd2vr::alchemy_own_head_h51::enabled.load(std::memory_order_relaxed);
        const bool smithing=mh::freeTrackedMode(mh::modeFor(mh::Smithing));
        return activity.ready.load(std::memory_order_acquire)&&
            ((alchemy&&activity.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire))||
             (smithing&&activity.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load(std::memory_order_acquire))||
             ((alchemy||smithing)&&activity.transitionDepth175.load(std::memory_order_acquire)));};
    const bool nativeActivityAtReadH51=nativeActivityEyeH51();
    // Temporary unavailable packets cannot apply an offset. Keep only the
    // anatomical measurement; the next valid body/epoch/scale/calibration must
    // qualify it again. No temporal filter survives here outside the pose owner.
    const auto admitted=[](const kcd2vr::tracked_frame51::Frame* frame=nullptr,
        kcd2vr::grasp13::Acquisition54* exactPicking55=nullptr,bool* exactWorkstationH51=nullptr){
        kcd2vr::grasp13::Acquisition54 acquisition{};
        const bool picking=kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,acquisition)&&acquisition.active&&
            kcd2vr::grasp13::freshAcquisition54(acquisition,GetTickCount64())&&
            acquisition.character+8==reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load())&&
            acquisition.epoch==g_pose_epoch6689.load()&&
            (!frame||(frame->acquisitionSerial==acquisition.serial&&frame->acquisitionAction==acquisition.action&&
                frame->acquisitionScope==acquisition.scope&&frame->acquisitionToken==acquisition.nativeToken&&
                frame->stamp.actor==acquisition.character&&frame->stamp.epoch==acquisition.epoch));
        if(exactPicking55)*exactPicking55=picking?acquisition:kcd2vr::grasp13::Acquisition54{};
        // Reuse the native workstation body's copied-pose reader. The switch
        // alone is not an owner: current saved workstation policy and completed frame
        // must agree, including the existing mode lifetime on entry/exit.
        kcd2vr::workstation178r20c1::WorkstationOwnerH51 workstationOwnerH51{};
        const bool workstationKindH51=kcd2vr::workstation178r20c1::workstationOwnerH51(GetTickCount64(),workstationOwnerH51)&&
            (!frame||(frame->workstationKindH51==workstationOwnerH51.kind&&frame->workstationModeEpochH51==workstationOwnerH51.modeEpoch&&
                frame->workstationLifetimeH51==workstationOwnerH51.lifetime&&frame->workstationInstanceH51==workstationOwnerH51.instance));
        if(exactWorkstationH51)*exactWorkstationH51=workstationKindH51;
        return CombatCensusMode()>=3&&g_cal_state.load()==2&&(g_gameplay_allowed.load()||picking||workstationKindH51)&&
            g_head_valid.load()&&!diagnostics::lab::NativeReference()&&!g_ctx_dialogue.load()&&
            (workstationKindH51?WorkstationContextRefusal19c4r2(GetTickCount64(),true)==nullptr:!g_ctx_menu.load())&&
            !g_ctx_cutscene.load()&&(!g_ctx_scripted.load()||picking||workstationKindH51)&&(!g_ctx_reading.load()||(workstationKindH51&&workstationOwnerH51.alchemyBookAS1))&&
            !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
            (!frame||!frame->acquisitionSerial||picking)&&(!frame||!frame->workstationKindH51||workstationKindH51);
    };
    kcd2vr::grasp13::Acquisition54 picking55{};
    bool workstation55=false;
    if(!admitted(nullptr,&picking55,&workstation55))return false;
    const auto entryContextD55=pickupBridgeD55?CameraHoldContextD55():kcd2vr::camera_hold_e1::ContextD55{};
    failedCamera48.interaction.action=picking55.action;
    failedCamera48.interaction.token=picking55.serial;
    failedCamera48.interaction.side=picking55.active?picking55.side:-1;
    admittedE1=true; // native camera ownership (dialogue/menu/hit reaction/...) was ruled out above
    // D37B: a busy reader is a presentation gap, not a change of camera owner.
    // Qualify current ownership first so the wrapper can retain its bounded
    // same-body base; it rechecks plain gameplay around the current-root read.
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);
    if(!lock.owns_lock()){failedCamera48.reason="TRACKED_CAMERA_CORE_BUSY";return false;}
    const auto publication=g_frame_publication51.load(std::memory_order_acquire);
    const auto control=g_frame_control51.load(std::memory_order_acquire);
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto owner=g_henry_char_attachment_manager.load(std::memory_order_acquire);
    failedCamera48.interaction.collector=publication;failedCamera48.interaction.poseEpoch=epoch;
    if(!publication||(publication&1)){failedCamera48.reason="TRACKED_CAMERA_PUBLICATION_EMPTY_OR_WRITING";return false;}
    CommittedFrame6695 packet{};kcd2vr::tracked_frame51::Frame frame{};
    if(!ReadCommittedPacketFor6695(packet,workstation55,picking55.active?&picking55:nullptr)||!packet.headEvidence122||!g_tracked_frame51.read(frame)||
       frame.publication!=publication||packet.stamp.epoch!=epoch||packet.stamp.actor+8!=reinterpret_cast<std::uintptr_t>(owner)||
       kcd2vr::pose_core::length(frame.root-kcd2vr::pose_core::Vec3{packet.world[4],packet.world[5],packet.world[6]})>1e-6||
       kcd2vr::pose_core::angle(frame.rotation,kcd2vr::pose_core::Quat{packet.world[0],packet.world[1],packet.world[2],packet.world[3]})>1e-6||
       std::fabs(frame.scale-packet.world[7])>1e-6){
        failedCamera48.reason="TRACKED_CAMERA_PACKET_JOIN_UNAVAILABLE";
        // D62: a complete newer publication can win DURING the first packet
        // read, before the current-root leaf. It is the same publication race
        // already retried below, not proof that the native camera owns the eye.
        // Retry once from scratch; never pair the old packet with the new frame
        // or cache this failed result. A continuously changing writer remains
        // bounded by the wrapper's single retry.
        const auto afterPublicationD62=g_frame_publication51.load(std::memory_order_acquire);
        if(retryPublicationD56&&afterPublicationD62&&!(afterPublicationD62&1)&&afterPublicationD62!=publication&&
           g_pose_epoch6689.load(std::memory_order_acquire)==epoch&&g_henry_char_attachment_manager.load(std::memory_order_acquire)==owner&&
           g_frame_control51.load(std::memory_order_acquire)==control&&admitted())
            *retryPublicationD56=true;
        return false;
    }
    failedCamera48.interaction.actor=packet.stamp.actor;failedCamera48.interaction.sourceTick=packet.stamp.tick;
    failedCamera48.interaction.generation=packet.stamp.sample;
    // The packet owns a model-local completed anchor, never up to 80 ms of old
    // root travel. Reuse the existing bounded +0xA08 model-world read on the exact
    // joined Henry character, preserving CURRENT translation/rotation like C34.
    ModelWorld6692 current{};
    if(!ReadModelWorld6692(reinterpret_cast<void*>(packet.stamp.actor),current)||
       std::fabs(current.scale-frame.scale)>1e-6){failedCamera48.reason="TRACKED_CAMERA_CURRENT_ROOT_UNAVAILABLE";return false;}
    frame.root={current.t.x,current.t.y,current.t.z};frame.rotation={current.q.x,current.q.y,current.q.z,current.q.w};
    // A native activity may begin after this ordinary body's publication. Its
    // current authored eye is not a neutral measurement for that older frame.
    // Keep established anatomy usable; only the local measurement flag changes.
    if(frame.neutral&&(nativeActivityAtReadH51||nativeActivityEyeH51()))frame.neutral=false;
    auto staged=state;kcd2vr::pose_core::Vec3 eye{},forward{};
    if(!kcd2vr::tracked_frame51::eye(staged,frame,packet.stamp,control,GetTickCount64(),{x,y,z},eye,{fx,fy,fz},&forward)){
        failedCamera48.reason="TRACKED_CAMERA_EYE_RECONSTRUCTION_REFUSED";return false;}
    // Invalidation paths already revoke the completed pose. Re-read that exact
    // gate after calculation and rejoin the publication serial: this covers a
    // same-sample native callback, failed restore, tracking loss or mode handoff
    // during the current-world leaf without authorizing mixed body/camera data.
    CommittedFrame6695 verify{};kcd2vr::grasp13::Acquisition54 verifyPicking55{};bool verifyWorkstationH51=false;
    if(!admitted(&frame,&verifyPicking55,&verifyWorkstationH51)||
       !ReadCommittedPacketFor6695(verify,verifyWorkstationH51,verifyPicking55.active?&verifyPicking55:nullptr)||
       !kcd2vr::tracked_frame51::same_stamp(verify.stamp,packet.stamp)||
       g_pose_epoch6689.load(std::memory_order_acquire)!=epoch||g_henry_char_attachment_manager.load(std::memory_order_acquire)!=owner||
       g_frame_control51.load(std::memory_order_acquire)!=control||g_frame_publication51.load(std::memory_order_acquire)!=publication){
        failedCamera48.reason="TRACKED_CAMERA_FINAL_OWNER_OR_PUBLICATION_CHANGED";
        // D56: a newer completed pose is not a native camera handoff. Ask the
        // wrapper to read and reconstruct one fresh pair, never reuse this eye.
        // All identity/context checks run again in the second attempt.
        const auto afterPublicationD56=g_frame_publication51.load(std::memory_order_acquire);
        if(retryPublicationD56&&afterPublicationD56&&!(afterPublicationD56&1)&&afterPublicationD56!=publication&&
           g_pose_epoch6689.load(std::memory_order_acquire)==epoch&&g_henry_char_attachment_manager.load(std::memory_order_acquire)==owner&&
           g_frame_control51.load(std::memory_order_acquire)==control&&admitted())
            *retryPublicationD56=true;
        // D64: Attach/Finish can retire Picking between the completed pose and
        // this read. An old Picking frame is not native camera ownership. Permit
        // only that exact pickup edge (or its own active action), with the same
        // body/control/tracking and current VR admission. The wrapper rechecks
        // the cached owner and CURRENT root, and caps this edge at 60 ms even
        // when both context reads have already become plain gameplay.
        const auto currentContextD64=pickupBridgeD55?CameraHoldContextD55():kcd2vr::camera_hold_e1::ContextD55{};
        const bool pickupEdgeD64=entryContextD55.kind==2||currentContextD64.kind==2||frame.acquisitionSerial!=0;
        admittedE1=pickupBridgeD55&&pickupEdgeD64&&
            kcd2vr::camera_hold_e1::compatible(entryContextD55,currentContextD64)&&
            g_pose_epoch6689.load(std::memory_order_acquire)==epoch&&
            g_henry_char_attachment_manager.load(std::memory_order_acquire)==owner&&
            g_frame_control51.load(std::memory_order_acquire)==control&&admitted();
        if(pickupHandoverD64)*pickupHandoverD64=admittedE1;
        return false;}
    // Rejoin after the final copied-packet read too. If Begin happened during
    // that read, recompute from the original anatomical state without learning
    // the activity's eye or heading. The frame remains its original owner.
    if(frame.neutral&&nativeActivityEyeH51()){
        frame.neutral=false;staged=state;
        if(!kcd2vr::tracked_frame51::eye(staged,frame,packet.stamp,control,GetTickCount64(),{x,y,z},eye,{fx,fy,fz},&forward)){
            failedCamera48.reason="TRACKED_CAMERA_ENTRY_EYE_RECONSTRUCTION_REFUSED";return false;}
    }
    failedCamera48.success=true;
    diagnostics::ownership48::cameraSample48=packet.stamp.sample;diagnostics::ownership48::cameraPublication48=publication;diagnostics::ownership48::cameraEpoch48=epoch;diagnostics::ownership48::cameraActor48=packet.stamp.actor;
    if(!kcd2vr::native_roomscale_d76::SameSerial(kcd2vr::native_roomscale_d76::Surface::Character,packet.stamp.actor,current.roomOriginD76.serial))return false;
    if(roomOriginD76)*roomOriginD76=current.roomOriginD76;
    state=staged;ox=float(eye.x);oy=float(eye.y);oz=float(eye.z);hx=float(forward.x);hy=float(forward.y);
    {   // R133: Henry's root yaw from this same current-root read, for the FREE PLUS body hold in camera_hook.
        const double qx=current.q.x,qy=current.q.y,qz=current.q.z,qw=current.q.w;
        const double rfx=2*(qx*qy-qw*qz),rfy=1-2*(qx*qx+qz*qz);
        if(std::isfinite(rfx)&&std::isfinite(rfy)&&rfx*rfx+rfy*rfy>1e-6){
            kcd2vr::free_targeting_r133::rootYaw.store(float(kcd2vr::free_targeting_r133::yawOf(rfx,rfy)),std::memory_order_relaxed);
            kcd2vr::free_targeting_r133::rootYawTick.store(GetTickCount64(),std::memory_order_release);
        }
    }
    // value = forward displacement from incoming native eye; value2 = vertical.
    // generation = XR sample; collector = body publication; point = solved base.
    const float horizontal53=std::sqrt(fx*fx+fy*fy);
    failedCamera48.interaction.value=horizontal53>1e-5f?((ox-x)*fx+(oy-y)*fy)/horizontal53:0;
    failedCamera48.interaction.value2=oz-z;
    failedCamera48.interaction.point[0]=ox;failedCamera48.interaction.point[1]=oy;failedCamera48.interaction.point[2]=oz;
    {auto held=kcd2vr::camera_hold_e1::remember(packet.stamp.actor,epoch,control,GetTickCount64(),current.scale,
        {g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()},frame.root,frame.rotation,eye,forward.x,forward.y);
     if(pickupBridgeD55){
         held.contextD55=CameraHoldContextD55();
         const bool sameOwner=kcd2vr::camera_hold_e1::same(entryContextD55,held.contextD55);
         (void)g_camera_hold_e1.publish(held.valid&&held.contextD55.kind&&sameOwner&&!workstation55&&!verifyWorkstationH51?
             held:kcd2vr::camera_hold_e1::Sample{});
     }else{
         // R8d2 default: only plain-gameplay bases are cached.
         (void)g_camera_hold_e1.publish(held.valid&&!picking55.active&&!workstation55&&!verifyPicking55.active&&!verifyWorkstationH51?
             held:kcd2vr::camera_hold_e1::Sample{});
     }}
    if(diagnostics::ownership48::active(GetTickCount64())){
        diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::CameraBase;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();
        e.actor=packet.stamp.actor;e.sample=packet.stamp.sample;e.publication=publication;e.epoch=epoch;e.code=1;
        const float data[]{x,y,z,ox,oy,oz,hx,hy,float(frame.neutral),float(nativeActivityAtReadH51)};std::copy_n(data,10,e.v);
        diagnostics::ownership48::name(e,"ACCEPTED_TRACKED_BODY_CAMERA_BASE");(void)diagnostics::ownership48::push(e);
    }

    // Copied current transaction only: one receipt/500ms under finite F3/F11
    // capture. No extra native query, worker observer or quiet-mode polling.
    const auto cameraNow55=std::uint64_t(GetTickCount64());
    if(frame.bow&&(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Combat,cameraNow55)||
                   kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Reload,cameraNow55))){
        static std::atomic<std::uint64_t> next55{0};auto due55=next55.load(std::memory_order_relaxed);
        if(cameraNow55>=due55&&next55.compare_exchange_strong(due55,cameraNow55+500,std::memory_order_relaxed)){
            char receipt55[520]{};std::snprintf(receipt55,sizeof(receipt55),
                "C55 BOW_CAMERA sample=%llu epoch=%llu publication=%llu ageMs=%llu hmdHeight=%d neutral=%d headingReady=%d nativeEye=(%.4f,%.4f,%.4f) finalBase=(%.4f,%.4f,%.4f) nativeForward=(%.5f,%.5f,%.5f) bowForward=(%.5f,%.5f) rootQ=(%.5f,%.5f,%.5f,%.5f)",
                (unsigned long long)packet.stamp.sample,(unsigned long long)epoch,(unsigned long long)publication,
                (unsigned long long)(cameraNow55-packet.stamp.tick),frame.hmdHeight,frame.neutral,staged.headingReady,
                double(x),double(y),double(z),eye.x,eye.y,eye.z,double(fx),double(fy),double(fz),forward.x,forward.y,
                current.q.x,current.q.y,current.q.z,current.q.w);
            diagnostic_trace::Write(receipt55);
        }
    }
    return true;
}
bool TrackedFrameCamera55(float x,float y,float z,float fx,float fy,float fz,
                          float& ox,float& oy,float& oz,float& hx,float& hy,kcd2vr::native_roomscale_d76::Origin* roomOriginD76) noexcept {
    if(roomOriginD76)*roomOriginD76={};
    namespace ch=kcd2vr::camera_hold_e1;
    static std::atomic<bool> heldE1{false};
    const bool pickupBridgeD55=ch::pickupBridgeD55.load(std::memory_order_acquire);
    static std::atomic<bool> previousBridgeD55{false};
    if(previousBridgeD55.exchange(pickupBridgeD55,std::memory_order_acq_rel)!=pickupBridgeD55){
        (void)g_camera_hold_e1.publish(ch::Sample{});heldE1.store(false,std::memory_order_relaxed);
    }
    bool admittedE1=false,retryPublicationD56=false,pickupHandoverD64=false;
    bool acceptedD56=TrackedFrameCameraCoreE1(x,y,z,fx,fy,fz,ox,oy,oz,hx,hy,admittedE1,pickupBridgeD55,&retryPublicationD56,&pickupHandoverD64,roomOriginD76);
    if(!acceptedD56&&retryPublicationD56){
        const bool admittedBeforeRetryD56=admittedE1;bool retryPickupHandoverD64=false;
        acceptedD56=TrackedFrameCameraCoreE1(x,y,z,fx,fy,fz,ox,oy,oz,hx,hy,admittedE1,pickupBridgeD55,nullptr,&retryPickupHandoverD64,roomOriginD76);
        pickupHandoverD64=pickupHandoverD64||retryPickupHandoverD64;
        // A failed fresh-pair retry cannot widen the original hold permission.
        // In particular, ordinary final-owner loss still clears its old base.
        if(!acceptedD56)admittedE1=admittedE1&&admittedBeforeRetryD56;
    }
    if(acceptedD56){heldE1.store(false,std::memory_order_relaxed);return true;}
    hx=hy=0;
    // Not admitted = the game owns the camera on purpose (menu, dialogue, hit reaction, native body): native eye.
    // R8d2: also retire the cached base, so nothing from before the native owner can come back afterwards.
    const auto beforeD55=pickupBridgeD55?CameraHoldContextD55():ch::ContextD55{};
    if(const auto g=ch::gap(admittedE1,pickupBridgeD55?beforeD55.kind!=0:PlainGameplayCameraE1(),false,ch::Why::None);g.clearCache){
        if(pickupBridgeD55)CameraGapReceiptD55({},beforeD55,ch::Why::Identity,{},GetTickCount64());
        heldE1.store(false,std::memory_order_relaxed);(void)g_camera_hold_e1.publish(ch::Sample{});return false;}
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::native_roomscale_d76::Origin heldOriginD76{};
    ch::Sample s{};ch::Why why=ch::Why::None;kcd2vr::pose_core::Vec3 eye{};double hxd=0,hyd=0;
    if(g_camera_hold_e1.read(s)&&s.valid){
        ModelWorld6692 current{};
        if(pickupBridgeD55&&!ch::compatible(s.contextD55,beforeD55)){
            why=ch::Why::Identity;(void)g_camera_hold_e1.publish(ch::Sample{});
        }else if(s.actor+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))||
           !ReadModelWorld6692(reinterpret_cast<void*>(s.actor),current))why=ch::Why::Identity;
        else why=ch::hold(s,s.actor,g_pose_epoch6689.load(std::memory_order_acquire),g_frame_control51.load(std::memory_order_acquire),
            current.scale,{g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()},
            {current.t.x,current.t.y,current.t.z},{current.q.x,current.q.y,current.q.z,current.q.w},now,
            pickupBridgeD55?ch::bridgeLimitD64(s.contextD55,beforeD55,ch::maxHoldMs.load(std::memory_order_relaxed),pickupHandoverD64):
                ch::maxHoldMs.load(std::memory_order_relaxed),eye,hxd,hyd);
        heldOriginD76=current.roomOriginD76;
    }
    // R8d2 (Codex P1): ownership re-read on the CURRENT state after the world read, before publishing the held base.
    bool stillOwnedD55=false;
    if(pickupBridgeD55){const auto afterD55=CameraHoldContextD55();
        const auto finalLimitD64=ch::bridgeLimitD64(s.contextD55,afterD55,ch::maxHoldMs.load(std::memory_order_relaxed),
            pickupHandoverD64||beforeD55.kind==2);
        stillOwnedD55=ch::pickupBridgeD55.load(std::memory_order_acquire)&&ch::compatible(beforeD55,afterD55)&&
            ch::compatible(s.contextD55,afterD55)&&s.actor+8==reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load())&&
            s.epoch==g_pose_epoch6689.load()&&s.control==g_frame_control51.load();
        // A pickup may begin during the root read. Reapply its shorter limit;
        // a fresh action cannot renew an old anatomical sample.
        if(why==ch::Why::Held&&(!finalLimitD64||now<s.tick||now-s.tick>finalLimitD64))
            why=finalLimitD64?ch::Why::Expired:ch::Why::Disabled;
    }
    if(const auto g=ch::gap(true,true,why==ch::Why::Held&&(pickupBridgeD55?stillOwnedD55:PlainGameplayCameraE1()),why);g.clearCache){why=ch::Why::Identity;(void)g_camera_hold_e1.publish(ch::Sample{});}
    if(pickupBridgeD55)CameraGapReceiptD55(s,beforeD55,why,eye,now);
    if(why==ch::Why::Held&&!kcd2vr::native_roomscale_d76::SameSerial(kcd2vr::native_roomscale_d76::Surface::Character,s.actor,heldOriginD76.serial))why=ch::Why::Identity;
    if(why==ch::Why::Held){
        if(roomOriginD76)*roomOriginD76=heldOriginD76;
        ox=float(eye.x);oy=float(eye.y);oz=float(eye.z);hx=float(hxd);hy=float(hyd);
        ch::heldFrames.fetch_add(1,std::memory_order_relaxed);heldE1.store(true,std::memory_order_relaxed);return true;
    }
    // The hold ran out (or the body changed) while gameplay still owns the VR camera: say so once.
    if(heldE1.exchange(false,std::memory_order_relaxed)){
        ch::expired.fetch_add(1,std::memory_order_relaxed);
        char m[200]{};std::snprintf(m,sizeof(m),"E1 CAMERA_HOLD end=%s holdMs=%u heldFrames=%llu nativeEye=(%.4f,%.4f,%.4f)",
            ch::name(why),unsigned(ch::maxHoldMs.load()),(unsigned long long)ch::heldFrames.load(),double(x),double(y),double(z));
        diagnostic_trace::Write(m);
        if(why==ch::Why::Expired)(void)kcd2vr::visible_failure::publish("VR camera: VR body pose lost too long, game camera used",now);
    }
    return false;
}
bool ReadCameraRoomscaleOriginD76(kcd2vr::native_roomscale_d76::Origin& out) noexcept {
    return kcd2vr::native_roomscale_d76::ReadCharacterOrigin(g_player_char37.load(std::memory_order_acquire),out);
}
bool TrackedFrameCamera51(float x,float y,float z,float& ox,float& oy,float& oz) noexcept {
    float hx=0,hy=0;return TrackedFrameCamera55(x,y,z,0,0,0,ox,oy,oz,hx,hy);
}
bool MotionOwnedCamera47(float x,float y,float z,float fx,float fy,float fz,float& ox,float& oy,float& oz,float& hx,float& hy) noexcept {
    if(CombatCensusMode()<3||g_cal_state.load()!=2||!g_gameplay_allowed.load()||!g_head_valid.load()||
       diagnostics::lab::NativeReference()||g_ctx_dialogue.load()||g_ctx_menu.load()||g_ctx_cutscene.load()||
       g_ctx_scripted.load()||animation_filter::nativeBodyOwned112.load())return false;
    CommittedFrame6695 f{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(f)||!f.headEvidence122||f.stamp.epoch!=g_pose_epoch6689.load()||
       f.stamp.actor+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load()))return false;
    kcd2vr::motion_camera47::Input i{};i.character=f.stamp.actor;i.epoch=f.stamp.epoch;i.tick=f.stamp.tick;
    i.root={f.world[4],f.world[5],f.world[6]};i.rotation={f.world[0],f.world[1],f.world[2],f.world[3]};i.scale=f.world[7];
    i.nativeHead={f.nativeHead122.x,f.nativeHead122.y,f.nativeHead122.z};
    i.calibratedHead={g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
    i.nativeEye={x,y,z};i.nativeForward={fx,fy,fz};i.allowed=true;i.neutral=BowCameraNeutral35(now);
    static std::mutex mutex;static kcd2vr::motion_camera47::State state{};
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock())return false;
    kcd2vr::motion_camera47::Result out{};if(!kcd2vr::motion_camera47::solve(state,i,now,out))return false;
    ox=float(out.eye.x);oy=float(out.eye.y);oz=float(out.eye.z);hx=float(out.forward.x);hy=float(out.forward.y);return true;
}
