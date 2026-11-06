#ifndef KCD2VR_OWN_SHIELD_CLANG58_IMPL
#define KCD2VR_OWN_SHIELD_CLANG58_IMPL
// Included after StageShieldPat54, in the existing private pose namespace.
// Both proxy/pose owners remain untouched; this only publishes a cosmetic cue.
static kcd2vr::own_shield_contact58::Event PublishOwnShieldClang58(
    CoherentTransaction6689& tx,const CommittedFrame6695& packet,bool physicalPublished,std::uint64_t now) noexcept {
    namespace clang=kcd2vr::own_shield_contact58;
    static clang::State state{};const auto& x=tx.shieldContact54;
    const bool current=physicalPublished&&x.eligible&&packet.valid[0]&&packet.valid[1]&&
        x.owner.character==packet.stamp.actor&&x.owner.epoch==packet.stamp.epoch&&
        x.sample==packet.stamp.sample&&x.tick==packet.stamp.tick&&
        tx.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        x.owner.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
        tx.tracking.allowed&&tx.tracking.head_valid&&!tx.tracking.head_held&&
        tx.tracking.left_valid&&tx.tracking.right_valid&&!tx.tracking.left_held&&!tx.tracking.right_held&&
        g_gameplay_allowed.load(std::memory_order_acquire)&&!g_ctx_menu.load(std::memory_order_acquire)&&
        !g_ctx_dialogue.load(std::memory_order_acquire)&&!g_ctx_cutscene.load(std::memory_order_acquire)&&
        !g_ctx_reading.load(std::memory_order_acquire)&&WorkstationContextRefusal19c4r2(now,true)==nullptr&&
        !animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&!animation_filter::NativeKickActive175()&&
        !tx.physicalOutput.support[0]&&!tx.physicalOutput.support[1];
    const auto edge=clang::step(state,x,{now,tx.tracking.source_epoch,
        kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire),current});
    if(!edge.tick)return edge;
    // Publish_event enforces the user's current haptics enable/allow/epoch.
    const float amp=float(std::clamp(.35+edge.speed*.08,.40,.80));
    (void)kcd2vr::haptics101::publish_event(edge.weapon,x.owner.key,amp,45,now);
    (void)kcd2vr::haptics101::publish_event(edge.shield,x.shieldKey,amp*.8f,40,now);
    if(cc::active(now)){
        static kcd2vr::receipt56::Site log;
        if(kcd2vr::receipt56::allow(log,4,now)){char m[256]{};std::snprintf(m,sizeof(m),
            "C58 OWN_SHIELD_CONTACT sample=%llu relativeSpeed=%.2f inward=%.2f geometry=NATIVE_LOCAL_BOUNDS_PROXY nativeBlock=NONE damage=NONE",
            (unsigned long long)edge.sample,edge.speed,edge.approach);diagnostic_trace::Write(m);}
    }
    return edge;
}
#endif
