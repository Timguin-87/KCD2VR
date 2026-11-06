// The caller has already qualified retail/native owner and completed frame.
// F3 OFF returns before copying the optional fields or formatting any receipt.
static void ObserveTorchOffhandD60(const PhysFrame6698& f,const NativeOwner104& owner,
                                  const nc104::Observed& native,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;namespace t=kcd2vr::torch_offhand_d60;
    if(!cap::active(cap::Domain::Combat,now)||!f.profiles[1].torch40())return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire);
    t::Frame p{};p.owner=reinterpret_cast<std::uintptr_t>(owner.player);
    p.key=f.profiles[1].key;p.generation=f.profiles[1].generation;p.body=f.input.bodyEpoch;p.request=f.request;
    p.sample=f.input.sample;p.tick=f.input.tick;p.classId=f.profiles[1].classId;p.side=f.profiles[1].side;
    p.phase=native.phase;p.type=native.type;p.input=native.input;p.zone=native.zone;p.hand=native.hand;
    const auto& guard=g_native_guard112;
    // A retained guard belonging to another actor is not this torch's evidence.
    const bool sameGuard=guard.owner.player==owner.player&&guard.owner.actor==owner.actor&&
        guard.owner.state==owner.state&&guard.owner.epoch==owner.epoch;
    p.guardSide=sameGuard&&(guard.held||guard.pendingPress46)?guard.side:-1;
    p.flags=(f.output.active[1]?t::Active:0u)|(f.output.owned[1]?t::Owned:0u)|
        (f.output.contactKnown[1]?t::LocalContactKnown:0u)|(f.output.blocked[1]?t::LocalBlocked:0u)|
        (f.output.wallBlocked58[1]?t::LocalWallBlocked:0u)|(native.blocking?t::NativeBlocking:0u)|
        (native.requested?t::NativeRequested:0u)|(native.requestCandidate?t::NativeCandidate:0u)|
        (sameGuard&&guard.held?t::GuardHeld:0u)|(sameGuard&&guard.pendingPress46?t::GuardPending:0u);
    static t::State state{};
    if(!state.offer(p,session,true,now)||!cap::active(cap::Domain::Combat,now)||
       session!=cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire))return;
    char line[640]{};std::snprintf(line,sizeof(line),
        "D42_F3 TORCH_OFFHAND_D60 session=%llu receipt=%u/64 owner=%llX key=%016llX generation=%llu body=%llu request=%llu sample=%llu tick=%llu flags=%u native=(%d,%d,%d,%d,%d) guardSide=%d keySource=COMPLETED_PROFILE contact=LOCAL_PRESENTATION_ONLY torchAttack=UNCONNECTED fire=UNPROVEN nativeDamage=UNOBSERVED",
        (unsigned long long)session,state.emitted,(unsigned long long)p.owner,(unsigned long long)p.key,
        (unsigned long long)p.generation,(unsigned long long)p.body,(unsigned long long)p.request,
        (unsigned long long)p.sample,(unsigned long long)p.tick,p.flags,p.phase,p.type,p.input,p.zone,p.hand,p.guardSide);
    diagnostic_trace::Write(line);
}
