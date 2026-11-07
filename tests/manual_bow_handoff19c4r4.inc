struct ManualReady596 {std::uint64_t key=0,request=0,epoch=0,tick=0;bool loaded=false;std::uint64_t generation=0;ph::BowNockAck19c4r4 nockAck{};};
static kcd2vr::core::SnapshotMailbox<ManualReady596> g_manual_bow_ready596;
static bool ManualReadyFor596(std::uint64_t key,std::uint64_t epoch,std::uint64_t now,std::uint64_t generation) noexcept {
    ManualReady596 r{};return g_manual_bow_ready596.read(r)&&r.loaded&&r.key==key&&r.epoch==epoch&&r.generation==generation&&
        r.request==g_weapon_request_epoch6694.load()&&r.tick&&now>=r.tick&&now-r.tick<=80;
}
static std::atomic<std::uint64_t> g_manual_nock_serial19c4r4{0},g_manual_nock_claim19c4r4{0};
static void ManualNockInput19c4r4(ph::Input& in,const ph::Profile& bow,std::uint64_t processedSample) noexcept {
    in.manualNockAck19c4r4={};
    if(!StagedBowEnabled59()||!in.sample||in.sample<=processedSample)return;
    ManualReady596 r{};
    if(!g_manual_bow_ready596.read(r)||!r.loaded||r.key!=bow.key||r.generation!=bow.generation||
       r.request!=in.epoch||r.request!=g_weapon_request_epoch6694.load()||r.epoch!=in.bodyEpoch||
       !r.tick||in.tick<r.tick||in.tick-r.tick>80)return;
    const auto& a=r.nockAck;
    if(!a.serial||a.key!=r.key||a.generation!=r.generation||a.request!=r.request||a.body!=r.epoch||
       !a.tick||in.tick<a.tick||in.tick-a.tick>80||!a.sample||a.sample>in.sample||
       !a.releaseToNock56||!a.handEntity56||!a.handIdentity56||!a.releaseSample56||!a.releaseTick56||
       a.sample<a.releaseSample56||a.tick<a.releaseTick56||in.sample<=a.releaseSample56||
       in.tick<=a.releaseTick56||in.tick-a.releaseTick56>80)return;
    auto claimed=g_manual_nock_claim19c4r4.load();
    // Burn each offered edge once, even if the solve is currently disallowed.
    // A later tracking/menu recovery must use the ordinary neutral rearm rule.
    if(claimed<a.serial){
        if(g_manual_nock_claim19c4r4.compare_exchange_strong(claimed,a.serial)){
            if(r.request==g_weapon_request_epoch6694.load()&&in.allowed&&in.tracked[0]&&in.tracked[1]&&in.intentTracked[0]&&in.intentTracked[1]&&
               ph::physical_bow175(in)&&in.bowGripDraw47c2&&in.bowActions&&in.bowGeometryReady175)
                in.manualNockAck19c4r4=a;
        }
    }
}
