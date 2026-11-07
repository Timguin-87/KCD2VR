// Called only for the already-qualified Henry skinning callback, before the
// wrist consumers. Uses that callback's fresh XR packet, never a worker poll.
static void PublishRoomscaleStepD76(void* character,const TrackingSnapshot& ts) noexcept {
    namespace rs=kcd2vr::roomscale_d76;
    namespace nr=kcd2vr::native_roomscale_d76;
    if(!rs::Enabled()&&!nr::Enrolled())return; // never-enrolled OFF keeps zero native reads
    static rs::TrackedStepState state{};
    static std::uint64_t selection=0,bodyLifetime=0;
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);
    if(!lock.owns_lock())return;
    const auto revision=rs::selectionRevision.load(std::memory_order_acquire);
    if(selection!=revision){state={};selection=revision;}
    const auto now=std::uint64_t(GetTickCount64());
    rs::TrackedStepSample sample{};
    sample.actorLifetime=g_player_lifetime_d44.load(std::memory_order_acquire);
    sample.tracking=ts.source_epoch;sample.origin=rs::originRevision.load(std::memory_order_acquire);
    sample.sample=ts.sequence;sample.tick=ts.tick;sample.now=now;
    sample.x=ts.head.x;sample.y=ts.head.y;sample.z=ts.head.z;
    sample.standingY=g_cal_head_y.load();sample.leanHeight=kcd2vr::roomscale58::LeanHeightD70();
    sample.live=ts.head_valid&&!ts.head_held;
    sample.allowed=rs::Enabled()&&!rs::recenterPending.load(std::memory_order_acquire)&&rs::movementContext.load(std::memory_order_acquire)&&nr::Ready()&&
        ts.allowed&&g_cal_state.load()==2&&g_cal_room_origin_d76.load(std::memory_order_acquire)==sample.origin&&BodyFollow128()&&
        !g_ctx_dialogue.load()&&!g_ctx_menu.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
        !g_ctx_reading.load()&&!animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        !diagnostics::lab::NativeReference()&&PlayerIdentityAccepts37(character,now);
    // Read the same C37 identity publication as the qualified skinning owner.
    // Walking must not depend on weapon/diagnostic manager discovery.
    const auto identityVersion=g_player_publication37.load(std::memory_order_acquire);
    void* entity=g_last_player_entity.load(std::memory_order_acquire);
    void* player=g_player_module_d76.load(std::memory_order_acquire);
    void* actualEntity=nullptr;std::uintptr_t vf=0;std::uint32_t entityId=0;
    if((identityVersion&1)||!player||!entity||
       !Read(player,vf)||vf!=WhBase()+0x3AA4B98ull||
       !Read(static_cast<unsigned char*>(player)+0x38,actualEntity)||actualEntity!=entity||
       !Read(static_cast<unsigned char*>(entity)+0x0C,entityId)||!entityId||
       identityVersion!=g_player_publication37.load(std::memory_order_acquire)||
       sample.actorLifetime!=g_player_lifetime_d44.load(std::memory_order_acquire)||
       !PlayerIdentityAccepts37(character,now)){state={};nr::CancelPending();return;}
    nr::Origin current{};
    (void)nr::ReadTrackingOriginD76(current); // metadata remains available during an old root write/fault
    // A genuine new player/world lifetime starts a new origin. Pose-control,
    // weapon, native animation and feature-toggle epochs are deliberately absent.
    if(!current.serial||bodyLifetime!=sample.actorLifetime||current.owner.player!=reinterpret_cast<std::uintptr_t>(player)||
       current.owner.entity!=reinterpret_cast<std::uintptr_t>(entity)||current.owner.character!=reinterpret_cast<std::uintptr_t>(character)){
        nr::Owner owner{reinterpret_cast<std::uintptr_t>(player),reinterpret_cast<std::uintptr_t>(entity),
            reinterpret_cast<std::uintptr_t>(character),entityId,sample.origin,sample.actorLifetime};
        if(!nr::Rebase(owner)){state={};return;}
        nr::SetAdmission(sample.allowed);bodyLifetime=sample.actorLifetime;state={};
    }
    // Coordinate lifecycle precedes gameplay admission, including feature OFF.
    // Native takeover/late XR may stop movement but cannot resurrect consumed room travel.
    if(!sample.allowed){(void)rs::observe(state,sample);nr::CancelPending();return;}
    const auto delta=rs::observe(state,sample);
    if(delta.retirePending)nr::CancelPending();
    if(!delta.submit)return;
    ModelWorld6692 world{};
    if(!ReadModelWorld6692(character,world)){nr::CancelPending();return;}
    const Q4 frame=QNormalize(CalFrameR133());
    const V3 xr{float(delta.x),0,float(delta.z)};
    V3 forward=QRotateV(world.q,QRotateV(frame,V3{0,1,0}));
    const float horizontal=std::hypot(forward.x,forward.y);
    if(!std::isfinite(horizontal)||horizontal<1e-4f){nr::CancelPending();return;}
    forward={forward.x/horizontal,forward.y/horizontal,0};
    const V3 right{forward.y,-forward.x,0};
    const V3 displacement{right.x*xr.x-forward.x*xr.z,right.y*xr.x-forward.y*xr.z,0};
    nr::StepRequest request{};request.owner=world.roomOriginD76.owner;
    request.sample=sample.sample;request.tickMs=sample.tick;
    request.trackingDelta={xr.x,0,xr.z};request.worldDelta={displacement.x,displacement.y,0};
    // No retry on refusal: a later stationary sample cannot replay a missed step.
    (void)nr::Submit(request);
}
