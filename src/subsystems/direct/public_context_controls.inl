void SetSwingStamina5916(bool on) noexcept {nhp134::geom27::vrstate596::g_swing_stamina.store(on);}
void SetTelegraphStrike59c10(bool on) noexcept {nhp134::geom27::vrstate596::g_telegraph_strike59c10.store(on);}
void SetDirectBlockTiming39(bool on,float atSkill0,float atSkill30) noexcept { // C39 VR DIRECT: skill-scaled announced contact ETA
    const float lo=kcd2vr::defense_timing39::clampFactor(atSkill0),hi=kcd2vr::defense_timing39::clampFactor(atSkill30);
    nhp134::geom27::vrstate596::g_block_speed_lo39.store(lo,std::memory_order_relaxed);nhp134::geom27::vrstate596::g_block_speed_hi39.store(hi,std::memory_order_relaxed);
    nhp134::geom27::vrstate596::g_block_timing39.store(on,std::memory_order_release);
    char m[260]{};std::snprintf(m,sizeof(m),"C39 DEFENSE_TIMING_CONFIG enabled=%d speedAtSkill0=%.2f speedAtSkill30=%.2f scope=RECIPROCAL_WAITING_OPPONENT_ONLY input=HENRY_450 native=ALIGNER_CLAMP_0.5_10",on?1:0,double(lo),double(hi));diagnostic_trace::Write(m);}
void SetNpcBlockEta59c10(float prepareSeconds,float strikeSeconds) noexcept {nhp134::geom27::vrstate596::g_eta_prepare59c10.store(prepareSeconds);nhp134::geom27::vrstate596::g_eta_strike59c10.store(strikeSeconds);}
static std::atomic<bool> g_ini_vr596{false};
void SelectVrBackendRung596(bool rung3) noexcept { // F4 rung 3 forces the VR backend; other rungs follow VrAttackBackend
    const bool on=rung3||g_ini_vr596.load();nhp134::geom27::vrstate596::g_consequence.store(on);
    if(rung3)diagnostic_trace::Write("V66.178R59.6 F4_RUNG3 backend=VR (blade drives Henry's attack phase/zone through the native setters; hits=DIRECT_NATIVE; ordinaryAction=NONE; F9=impulses)");}
void SetVrStateConsequence596(bool on) noexcept {g_ini_vr596.store(on);nhp134::geom27::vrstate596::g_consequence.store(on);
    diagnostic_trace::Write(on?"V66.178R59.6 VR_STATE_DRIVER=1 (VrAttackBackend=vr: blade publishes Henry's attack phase/zone natively; hits via the direct route; no ordinary action)":"V66.178R59.6 VR_STATE_DRIVER=0");}
bool VrStateConsequence596() noexcept {return nhp134::geom27::vrstate596::g_consequence.load();}
bool VrStateEvent5912(const kcd2vr::motion59c4::Key& key,std::uint64_t sourceTick,int event,int zone,int technique) noexcept {return nhp134::geom27::vrstate596::Event(key,sourceTick,event,zone,technique);}
kcd2vr::motion59c4::AttackOwnership VrStateAttackOwnership59c10(const kcd2vr::motion59c4::Key& context,std::uint64_t now) noexcept {return nhp134::geom27::vrstate596::QueryAttackOwnership(context,now);}
