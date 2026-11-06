// Included in interaction_alignment's existing private implementation after the
// checked SelectionRead68 helpers. No hook, native result or projection changes.
namespace dogD59R1=kcd2vr::dog_candidate_d59r1;
static std::atomic<bool> g_dog_observe_d59r1{false};
static std::uint64_t g_dog_generation_d59r1=1; // existing candidate mutex
static dogD59R1::Store g_dog_candidates_d59r1;
static dogD59R1::Source DogSourceLockedD59R1() noexcept {
    return g_dog_observe_d59r1.load(std::memory_order_acquire)&&g_enabled.load(std::memory_order_acquire)&&
        g_split_ready57c1.load(std::memory_order_acquire)&&!g_visual_fallback_enabled.load(std::memory_order_acquire)?
        dogD59R1::Source{g_dog_generation_d59r1,g_door_acquisition_generation65}:dogD59R1::Source{};
}
static void SetDogObservationImplD59R1(bool enabled) noexcept {
    std::lock_guard<std::mutex> lock(g_hand_candidate_mutex178r14);
    if(g_dog_observe_d59r1.exchange(enabled,std::memory_order_acq_rel)!=enabled){
        ++g_dog_generation_d59r1;g_dog_candidates_d59r1.clear();
    }
}
static void DogCandidateObservationD59R1(const Candidate57c1& native,std::uintptr_t worldPoint,std::uint64_t now) noexcept {
    if(!g_dog_observe_d59r1.load(std::memory_order_acquire)||!native.id||std::strcmp(native.cls,"Dog"))return;
    dogD59R1::Source source{};
    {std::lock_guard<std::mutex> lock(g_hand_candidate_mutex178r14);source=DogSourceLockedD59R1();}
    if(!dogD59R1::same(source,source))return;
    std::uintptr_t playerEntity=0,again=0;float point[3]{};palm59c9::Pair palms{};
    // This exact CPlayer -> CEntity mapping and checked read already qualify
    // native door/selection owners. No guessed Dog bone or physics call.
    const bool valid=dogD59R1::fresh(native.tick,now)&&SelectionVft68(native.player,0x3AA4B98)&&
        SelectionRead68(native.player+0x38,playerEntity)&&SelectionVft68(playerEntity,0x4072C78)&&
        SelectionCopy68(worldPoint,point,sizeof(point))&&g_palm_packets59c9.read(palms)&&
        SelectionRead68(native.player+0x38,again)&&again==playerEntity&&SelectionVft68(native.player,0x3AA4B98);
    std::lock_guard<std::mutex> lock(g_hand_candidate_mutex178r14);
    if(!dogD59R1::same(source,DogSourceLockedD59R1()))return;
    g_dog_candidates_d59r1.observe(native.id,native.cls,valid?playerEntity:0,point,palms,source,native.tick,now);
}
static bool ReadDogCandidateImplD59R1(int side,std::uint64_t now,dogD59R1::Candidate& out) noexcept {
    out=dogD59R1::Candidate{};
    if(!g_dog_observe_d59r1.load(std::memory_order_acquire))return false;
    std::lock_guard<std::mutex> lock(g_hand_candidate_mutex178r14);
    const auto source=DogSourceLockedD59R1();palm59c9::Pair palms{};
    return dogD59R1::same(source,source)&&g_palm_packets59c9.read(palms)&&
        g_dog_candidates_d59r1.read(side,palms,source,now,out);
}
