// Generated exact production functions/call; native surface leaves are fixture providers.
static void PublishHandCandidate178r16(std::uint64_t now,const float* c,const RaySample& ray,bool object,std::uint32_t entity,const char* cls,std::uint64_t generation) noexcept {
    if(!c)return;
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    // The offered candidate carries the EXACT admitted palm ray, rather than
    // optional door-only provenance that generic tool admission could never see.
    // Source revocation during the projection cannot repopulate an old squeeze.
    if(object&&(!generation||generation!=g_door_generation38||!g_enabled.load(std::memory_order_acquire)||
       !g_split_ready57c1.load(std::memory_order_acquire)||!HandRayActive()||
       g_visual_fallback_enabled.load(std::memory_order_acquire)||!ray.hand||ray.owner<0||ray.owner>1||
       !interaction112::Fresh(ray,now)))return;
    g_hand_candidate178r14={now,c[0],c[1],c[2],c[3],ray.owner,object,entity};
    // Preserve ordinary native candidate/hover behavior when a committed palm
    // has no optional player annotation. It remains UNKNOWN for Guided input.
    if(object&&ray.sourceEpoch&&ray.sourcePlayer){g_hand_candidate178r14.sourceGeneration=generation;g_hand_candidate178r14.sourceEpoch=ray.sourceEpoch;
        g_hand_candidate178r14.sourcePlayer=ray.sourcePlayer;g_hand_candidate178r14.sourceTick=ray.tick;}
    if(cls){unsigned k=0;for(;k<31&&cls[k];++k)g_hand_candidate178r14.cls[k]=cls[k];g_hand_candidate178r14.cls[k]=0;}
}

bool LastHandCandidate(HandCandidate& out) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);out=g_hand_candidate178r14;
    if(out.sourceGeneration){
        const auto now=std::uint64_t(GetTickCount64());palm59c9::Pair current{};
        if(out.sourceGeneration!=g_door_generation38||!g_enabled.load(std::memory_order_acquire)||
           !g_split_ready57c1.load(std::memory_order_acquire)||!HandRayActive()||g_visual_fallback_enabled.load(std::memory_order_acquire)||
           out.owner<0||out.owner>1||!out.tick||now<out.tick||now-out.tick>80||!g_palm_packets59c9.read(current)||
           !current.eligible[out.owner]||current.ray[out.owner].sourceEpoch!=out.sourceEpoch||
           current.ray[out.owner].sourcePlayer!=out.sourcePlayer||!interaction112::Fresh(current.ray[out.owner],now)){
            out={};return false;
        }
    }
    return out.tick!=0;
}

bool HandRayActive() noexcept {
    RaySample ray{};return g_hand_ray_active.load(std::memory_order_acquire)&&
        g_hand_packets112.read(ray)&&ray.hand&&interaction112::Fresh(ray,GetTickCount64());
}

static void ProductionOffer55(bool offered,std::uint64_t now,const float* point,const RaySample& ray, bool split,bool known,bool head,const Candidate57c1& candidate,std::uint64_t doorGeneration38) noexcept {
    if(offered)PublishHandCandidate178r16(now,point,ray,split&&known&&!head,candidate.id,candidate.cls,doorGeneration38);
}
