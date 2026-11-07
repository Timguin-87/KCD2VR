// Generated verbatim production decision/detour; do not edit.
struct BoundsNote63 {rb63::Box native{},allowed{};std::uint64_t tick=0,sample=0,poseTick=0;};
static std::atomic<bool> g_henry_bounds_enabled63{false},g_henry_bounds_retired63{false};
static kcd2vr::core::SnapshotMailbox<BoundsNote63> g_henry_bounds_note63;
static std::atomic<std::uint64_t> g_henry_bounds_calls63{0},g_henry_bounds_applied63{0},g_henry_bounds_note_tick63{0};
// H17: one relaxed counter on an exact Henry/caller refusal. No formatting,
// native readback, retry or longer pose lease on the renderer thread.
enum class BoundsReject63 : unsigned {
    Context,EnvelopeMissing,EnvelopeFreshness,EnvelopeIdentity,EnvelopePublication,EnvelopeGeometry,
    PacketMissing,PacketIdentity,PacketWorld,NativeReadOrScale,Union,OwnerChanged,Count
};
static std::atomic<std::uint64_t> g_henry_bounds_reject63[unsigned(BoundsReject63::Count)]{};
static const float* SelectHenryBounds63(void* character,const float* native,
        std::uintptr_t caller,std::uint64_t now) noexcept {
    const auto base=WhBase();
    if(!g_henry_bounds_enabled63.load(std::memory_order_acquire)||caller!=base+rb63::kCaller||!native||!character||
       reinterpret_cast<std::uintptr_t>(native)!=reinterpret_cast<std::uintptr_t>(character)+0x9E0||
       g_committed_character6689.load(std::memory_order_acquire)!=character)return native;
    g_henry_bounds_calls63.fetch_add(1,std::memory_order_relaxed);
    rb63::Envelope e{}; // reused copied envelope; diagnostics never reread native memory
    const auto reject63=[&](BoundsReject63 why){
        g_henry_bounds_reject63[unsigned(why)].fetch_add(1,std::memory_order_relaxed);
        if(kcd2vr::diagnostics::vrik_h29::active(now)){namespace vd=kcd2vr::diagnostics::vrik_h29;vd::Record r{};
            r.kind=vd::Kind::Render;r.tick=now;r.character=reinterpret_cast<std::uintptr_t>(character);r.reason=int(why);
            r.sample=e.sample;r.epoch=e.epoch;r.sourceTick=e.tick;r.publication=e.publication;r.policy=e.policy;r.control=e.control;
            std::strcpy(r.stage,"RENDER_BOUNDS_REFUSED");std::strcpy(r.end,"NATIVE_BOUNDS_FALLBACK");vd::push(r);}return native;};
    if(!g_henry_bounds_enabled63.load(std::memory_order_acquire)||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
       g_census_mode.load(std::memory_order_acquire)<2||g_weapon_control_mode.load(std::memory_order_acquire)==2||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||diagnostics::lab::NativeReference())return reject63(BoundsReject63::Context);
    const auto publication=g_frame_publication51.load(std::memory_order_acquire);
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto control=g_frame_control51.load(std::memory_order_acquire);
    CommittedFrame6695 packet{};
    if(!g_render_bounds63.read(e))return reject63(BoundsReject63::EnvelopeMissing);
    if(!rb63::current(e,reinterpret_cast<std::uintptr_t>(character),epoch,publication,policy,control,now)){
        // Classify only the already-refused copied payload. current() remains
        // the unchanged authority; this cannot turn any refusal into a union.
        const auto why=!e.freshTracked||!e.tick||now<e.tick||now-e.tick>80?BoundsReject63::EnvelopeFreshness:
            !e.character||e.character!=reinterpret_cast<std::uintptr_t>(character)||!epoch||e.epoch!=epoch||!e.sample?BoundsReject63::EnvelopeIdentity:
            !publication||(publication&1)||e.publication!=publication||(policy&1)||e.policy!=policy||e.control!=control?BoundsReject63::EnvelopePublication:
            BoundsReject63::EnvelopeGeometry;
        return reject63(why);
    }
    if(!g_committed_frame6695.read(packet))return reject63(BoundsReject63::PacketMissing);
    if(packet.stamp.actor!=e.character||packet.stamp.epoch!=e.epoch||
       packet.stamp.sample!=e.sample||packet.stamp.tick!=e.tick)return reject63(BoundsReject63::PacketIdentity);
    for(int k=0;k<8;++k)if(packet.world[k]!=e.world[k])return reject63(BoundsReject63::PacketWorld);
    void* vft=nullptr;float currentScale=0;
    rb63::Box original{},allowed{};
    if(!Read(character,vft)||vft!=reinterpret_cast<void*>(base+rb63::kCharacterVft)||
       !Read(static_cast<unsigned char*>(character)+0xA24,currentScale)||!std::isfinite(currentScale)||
       currentScale<.1f||currentScale>10.f||std::fabs(currentScale-e.world[7])>=1e-6f||
       !Read(native,original))return reject63(BoundsReject63::NativeReadOrScale);
    // The committed points are model-local. Match the pose publisher's scale
    // tolerance instead of discarding their render bounds for float roundoff.
    // Use the current scale for the existing world-space margin/expansion cap.
    auto renderEnvelope=e;renderEnvelope.world[7]=currentScale;
    if(!rb63::unite(original,renderEnvelope,allowed))return reject63(BoundsReject63::Union);
    // A same-call owner/publication change invalidates the borrowed envelope.
    if(!g_henry_bounds_enabled63.load(std::memory_order_acquire)||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
       g_committed_character6689.load(std::memory_order_acquire)!=character||
       g_frame_publication51.load(std::memory_order_acquire)!=publication||
       g_pose_epoch6689.load(std::memory_order_acquire)!=epoch||
       g_pose_policy_generation57.load(std::memory_order_acquire)!=policy||
       g_frame_control51.load(std::memory_order_acquire)!=control||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||diagnostics::lab::NativeReference())return reject63(BoundsReject63::OwnerChanged);
    // Retail63F90E immediately copies all six floats; it never retains this
    // pointer. Its normal value cache/invalidation remains native-owned.
    static thread_local rb63::Box tls;
    tls=allowed;g_henry_bounds_applied63.fetch_add(1,std::memory_order_relaxed);
    if(kcd2vr::diagnostics::vrik_h29::active(now)){namespace vd=kcd2vr::diagnostics::vrik_h29;vd::Record r{};
        r.kind=vd::Kind::Render;r.tick=now;r.character=reinterpret_cast<std::uintptr_t>(character);r.reason=-1;
        r.sample=e.sample;r.epoch=e.epoch;r.sourceTick=e.tick;r.publication=e.publication;r.policy=e.policy;r.control=e.control;
        r.renderData=true;r.nativeMin={original.v[0],original.v[1],original.v[2]};r.nativeMax={original.v[3],original.v[4],original.v[5]};
        r.allowedMin={allowed.v[0],allowed.v[1],allowed.v[2]};r.allowedMax={allowed.v[3],allowed.v[4],allowed.v[5]};
        std::strcpy(r.stage,"RENDER_BOUNDS_APPLIED");std::strcpy(r.end,"ALLOWED_BOUNDS_RETURNED");vd::push(r);}
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
        auto previous=g_henry_bounds_note_tick63.load(std::memory_order_relaxed);
        if(now>=previous&&now-previous>=500&&g_henry_bounds_note_tick63.compare_exchange_strong(previous,now,std::memory_order_acq_rel))
            (void)g_henry_bounds_note63.publish({original,allowed,now,e.sample,e.tick});
    }
    return tls.v;
}
static const float* __fastcall HenryBoundsDetour63(void* character) {
    const float* native=g_henry_bounds_original63?g_henry_bounds_original63(character):nullptr;
    return SelectHenryBounds63(character,native,reinterpret_cast<std::uintptr_t>(_ReturnAddress()),GetTickCount64());
}
