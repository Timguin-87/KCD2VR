// Generated from actual production camera adapter; do not edit.
core::SnapshotMailbox<ae19::Neutral> g_activity_neutral59c19;
core::SnapshotMailbox<ae19::Anchor> g_activity_anchor59c19;
std::atomic_flag g_activity_cache_writer43=ATOMIC_FLAG_INIT;
bool ActivityEyeBase59c19(Matrix34f& matrix,bool render,native_roomscale_d76::Origin* roomOriginD76=nullptr,std::uint64_t roomRevisionD76=0) noexcept {
    // Cull and publish may overlap. Serialize the small cache transaction
    // before taking any state/head snapshot; a busy attempt keeps its native
    // base. Never block a camera thread or let an old clear erase a new latch.
    if(g_activity_cache_writer43.test_and_set(std::memory_order_acquire))return false;
    struct Unlock43 {~Unlock43(){g_activity_cache_writer43.clear(std::memory_order_release);}} unlock43;
    const auto now=std::uint64_t(GetTickCount64());
    const auto state=activity_lens59c10r1::EyeState(now);
    // FREE VRIK consumes its exact completed C51 body below. Do not first
    // replace the native eye with the separate passenger anchor and then skip
    // that consumer. A missing/protected C51 frame retains the native fallback.
    workstation178r20c1::WorkstationOwnerH51 workstationOwner{};
    if(workstation178r20c1::workstationOwnerH51(now,workstationOwner)){
        g_activity_anchor59c19.publish({});dice_body_d81::retire();return false;
    }
    if(ae19::nativeAlchemy63(state,now)){
        // Preserve the exact current native eye, including the authored reach
        // to the book. Normal HMD rotation/recenter/IPD still compose below.
        // No old anchor can survive this native-owned alchemy interval.
        g_activity_anchor59c19.publish({});
        const ds::V native{matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
        activity_lens59c10r1::NoteEye59c19({now,state,ae19::Reason::NativeAlchemy,native,native,{},false,false,!render});
        return false;
    }
    ds::NativeHead head{};const bool haveHead=frik_pose_probe::NativeDialogueHead59c10r2(head,now);
    ae19::Neutral neutral{};ae19::Anchor anchor{};
    (void)g_activity_neutral59c19.read(neutral);(void)g_activity_anchor59c19.read(anchor);
    ds::Eye native{};native.position={matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
    for(unsigned i=0;i<3;++i)native.basis[i]={matrix.m[0][i],matrix.m[1][i],matrix.m[2][i]};
    // The ordinary publish route reuses the exact composed culling matrix.
    // Learn/latch from that pre-HMD base too: there may be no later render=true
    // call. Existing native owner, body, freshness and lifecycle gates remain.
    if(haveHead&&ae19::learn(state,head,native,now,neutral))g_activity_neutral59c19.publish(neutral);
    if(ae19::fresh(state,now)&&!state.active){anchor={};g_activity_anchor59c19.publish(anchor);}
    if(haveHead&&ae19::latch(state,head,neutral,now,anchor)==ae19::Reason::Applied)
        g_activity_anchor59c19.publish(anchor);
    // No world-space replay when ownership/body publication is missing. An
    // anatomical model-local anchor can survive recenter, but applying it still
    // requires the CURRENT qualified body; unavailable is not permission.
    ds::Eye eye{};const auto reason=ae19::resolveCurrent(state,head,haveHead,anchor,now,eye);
    if(haveHead&&roomOriginD76&&roomRevisionD76&&!roomscale_d76::compatible(*roomOriginD76,head.roomOriginD76,roomRevisionD76)){
        dice_body_d81::retire();return false;
    }
    const bool applied=reason==ae19::Reason::Applied;
    dice_body_d81::publish(state,head,anchor,now,applied);
    if(applied&&roomOriginD76)*roomOriginD76=head.roomOriginD76;
    if(applied){
        matrix.m[0][3]=eye.position.x;matrix.m[1][3]=eye.position.y;matrix.m[2][3]=eye.position.z;
        for(unsigned i=0;i<3;++i){matrix.m[0][i]=eye.basis[i].x;matrix.m[1][i]=eye.basis[i].y;matrix.m[2][i]=eye.basis[i].z;}
    }
    // Attempt receipt only: a cull can be discarded without being rendered.
    if(state.active)activity_lens59c10r1::NoteEye59c19({now,state,reason,native.position,
        applied?eye.position:native.position,head.world,haveHead,neutral.ready,!render});
    return applied;
}
