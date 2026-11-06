// DS-F17: native side of the visible failure line. Included inside
// frik_pose_probe's private namespace after touch_react43.inl (needs
// gore32n::g_lua_exec43). Publish is callable from ANY thread; ONLY the drain
// (called from NativeRhythmTick178, the game-thread Lua seam) executes
// Game.SendInfoText. One native dispatch per 2 s; a queued line older than 5 s
// is dropped (deliberate disposition); a missing Lua route never re-executes.
void PublishVisibleFailure(const char* text) noexcept {
    namespace vf=::kcd2vr::visible_failure;
    namespace cap=::kcd2vr::diagnostics::capture168;
    const auto now=std::uint64_t(GetTickCount64());
    vf::publishLogOnlyD80(text,now);
    // R90: on screen only with F2 (Bug); other captures keep the log row only.
    if(vf::screenAllowedR90(now))(void)vf::publishDiagnosticD74(text,now,cap::Domain::Bug);
}
// DS-F20: constraint op drain. The v3 state machine's actions become bounded
// single-slot ops; ONLY this game-thread drain executes them, one per tick,
// through a registered native executor. While the executor is absent (the
// native path is not proven yet) ops are dropped with an explicit disposition
// and the receipt keeps the evidence.
using ConstraintOpExecutor60=bool(*)(const ::kcd2vr::constraint_op60::Op&) noexcept;
std::atomic<ConstraintOpExecutor60> g_constraint_op_executor60{nullptr};
void DrainConstraintOps60(std::uint64_t now) noexcept {
    namespace co=::kcd2vr::constraint_op60;
    static co::Op pendingSettle{};static bool pendingExecuted=false;
    // DS-F45 (Codex F38): EVERY disposition goes through the retained
    // pending-settle path - a busy settle is retried next tick on all branches.
    const auto settleAlways=[&](const co::Op& op,bool ok) noexcept -> bool {
        if(co::settle(op,ok))return true;
        pendingSettle=op;pendingExecuted=ok;return false;
    };
    if(pendingSettle.kind!=co::Kind::None){
        if(!co::settle(pendingSettle,pendingExecuted))return;
        pendingSettle={};
    }
    co::Op op{};
    if(!co::take(op))return;
    const auto exec=g_constraint_op_executor60.load(std::memory_order_acquire);
    if(!exec){char m[220]{};std::snprintf(m,sizeof(m),"C60 OP_DROP kind=%s id=%u reason=EXECUTOR_ABSENT tick=%llu",
        co::kindName(op.kind),op.key.id,(unsigned long long)op.tick);diagnostic_trace::Write(m);
        (void)settleAlways(op,false);return;}
    if(now<op.tick||now-op.tick>5000){char m[220]{};std::snprintf(m,sizeof(m),"C60 OP_DROP kind=%s id=%u reason=STALE",co::kindName(op.kind),op.key.id);
        diagnostic_trace::Write(m);(void)settleAlways(op,false);return;}
    bool ok=false;__try{ok=exec(op);}__except(EXCEPTION_EXECUTE_HANDLER){ok=false;}
    (void)settleAlways(op,ok);
    char m[240]{};std::snprintf(m,sizeof(m),"C60 OP_EXEC kind=%s id=%u serial=%u result=%s",
        co::kindName(op.kind),op.key.id,op.serial,ok?"EXECUTED":"EXECUTOR_FAILED");diagnostic_trace::Write(m);
}
void DrainVisibleFailure(std::uint64_t now) noexcept {
    namespace vf=::kcd2vr::visible_failure;
    vf::Message m{};
    if(!vf::mailbox.read(m)||!m.serial||!m.tick||!vf::safe(m.text))return;
    const auto drained=vf::lastDrainedSerial.load(std::memory_order_acquire);
    if(m.serial==drained)return;
    vf::lastDrainedSerial.store(m.serial,std::memory_order_release); // disposition once, either way
    if(now<m.tick||now-m.tick>vf::kMaxQueueAgeMs)return; // stale: drop
    if(!vf::diagnosticAllowedD74(m,now))return; // recorder ended/replaced: no delayed diagnostic popup
    if(!vf::due(now))return; // hard cap: at most one native dispatch per 2 s
    char lua[420]{};
    std::snprintf(lua,sizeof(lua),
        "if Game and Game.SendInfoText then Game.SendInfoText(\"KCD2VR %s\") end",m.text);
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);
    if(exec&&exec(lua))vf::lastDispatched.store(now,std::memory_order_release);
    // No Lua route / failed exec: the exact receipt stays in kcd.log.
}
