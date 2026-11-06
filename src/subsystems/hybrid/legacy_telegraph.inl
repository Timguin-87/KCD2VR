// V66.178R34 TELEGRAPH. The enemy AI blocks by reading the attacker's attack telegraph - the pending/committed zone,
// attack type/strength and the attacking flag in his C_CombatActor state, which a native attack action writes as
// it runs (the F3 census: atk{type=1 str=1.00 zone=upperR pending=none committed=upperR attacking=1} during a native
// swing, type=-1 attacking=0 otherwise). A geometry swing never wrote them, so in rung 2 the enemy could be hit but
// never block (blocked=0 on all 44 hits of the 01:14 log). Rung 2 now mirrors the VR stroke into those same fields:
// stroke prepared -> pending zone + type/strength (the AI sees the attack coming), stroke committed -> attack/committed
// zone + attacking=1 (D568B0 adjudicates blocked/perfectBlock against the victim's defense zone from the same fields),
// contact submitted or stroke aborted -> the native idle values (type=-1, strength 0, pending none, attacking 0).
// Game thread only (player C_CombatActor::Update), value bytes only (prop+8, what B()/I()/F() read), never while a
// native engagement swing owns the gesture (the action writes them itself). INI VrGeometryTelegraph (default on).
std::atomic<bool> g_telegraph178r34{true};
std::atomic<int> g_tg_phase178r34{0},g_tg_zone178r34{-1};std::atomic<std::uint64_t> g_tg_tick178r34{0}; // mailbox: 0 none, 1 pending, 2 committed
void TelegraphPublish178r34(int phase,int zone,std::uint64_t now) noexcept {g_tg_zone178r34.store(zone,std::memory_order_relaxed);g_tg_tick178r34.store(now,std::memory_order_relaxed);g_tg_phase178r34.store(phase,std::memory_order_release);}
static bool SehWrite34(void* p,const void* in,std::size_t n) noexcept {if(!p||!in||!n)return false;__try{std::memcpy(p,in,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
template<class T> static bool Wr34(const void* st,std::uint32_t prop,const T& v) noexcept {return SehWrite34(const_cast<unsigned char*>(static_cast<const unsigned char*>(st))+prop+8,&v,sizeof(v));}
static int g_tg_written178r34=0; // game thread: last written phase (0 = idle values restored)
static int g_tg_written_zone178r34=-1;
static void TelegraphWrite34(const void* hs,int phase,int zone) noexcept {
    if(phase>0){Wr34<std::int32_t>(hs,kAttackType,1);Wr34<float>(hs,kAttackStrength,1.f);Wr34<std::int32_t>(hs,kPendingZone,zone);
        if(phase==2){Wr34<std::int32_t>(hs,kAttackZone,zone);Wr34<std::int32_t>(hs,kCommittedZone,zone);Wr34<std::uint8_t>(hs,kIsAttacking,1);Wr34<std::uint8_t>(hs,kAttackRequested,1);}
    }else{Wr34<std::int32_t>(hs,kAttackType,-1);Wr34<float>(hs,kAttackStrength,0.f);Wr34<std::int32_t>(hs,kPendingZone,-1);Wr34<std::uint8_t>(hs,kIsAttacking,0);Wr34<std::uint8_t>(hs,kAttackRequested,0);}
    g_tg_written178r34=phase;g_tg_written_zone178r34=zone;
}
bool TelegraphDirty178r34() noexcept {return g_tg_written178r34!=0;}
void TelegraphRestore178r34(const void* actor) noexcept { // rung left while a telegraph was written: back to the native idle values
    const void *he=nullptr,*hs=nullptr;if(!CombatActor(actor,he,hs,false)||!hs)return;TelegraphWrite34(hs,0,-1);g_tg_phase178r34.store(0);
    diagnostic_trace::Write("V66.178R34 TELEGRAPH phase=IDLE (restored after leaving rung 2)");}
// Called from the player's C_CombatActor::Update detour (game thread, after the original) with Henry's state.
void TelegraphTick178r34(const void* hs,std::uint64_t now) noexcept {
    if(!hs)return;
    const bool on=g_telegraph178r34.load(std::memory_order_relaxed)&&g_enabled.load(std::memory_order_relaxed)&&!NativeOwned178r33(now);
    int phase=on?g_tg_phase178r34.load(std::memory_order_acquire):0;const int zone=g_tg_zone178r34.load(std::memory_order_relaxed);
    const auto t=g_tg_tick178r34.load(std::memory_order_relaxed);if(phase>0&&(!t||now<t||now-t>700))phase=0; // a telegraph lives 700 ms at most
    if(phase==g_tg_written178r34&&(phase==0||zone==g_tg_written_zone178r34))return;
    if(phase==0&&g_tg_written178r34==0)return;
    TelegraphWrite34(hs,phase,zone);
    if(Allow(4,30)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R34 TELEGRAPH phase=%s zone=%d (Henry's attack state mirrored from the VR stroke so the enemy AI can block; native idle values restored at 0)",phase==2?"COMMITTED":phase==1?"PENDING":"IDLE",zone);diagnostic_trace::Write(m);}
}
