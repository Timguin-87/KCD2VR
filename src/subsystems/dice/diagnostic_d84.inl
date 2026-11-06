// Included inside activity_input163's existing owner. F5-only, read-only dice
// observation at completed native-frame end; no new hook, control or cup owner.
// Exact retail evidence: native strcmp at 2E92D60 consumes C_Dice+30;
// 2E8B8D6 reads and 2E951F1/2E95208 write the native cursor index at +4F0.
// `dice_hold` toggles the selected DIE via 2E8FA84 -> 2E97F64. It is not cup grip.
struct DiceSnapshotD84 {
    std::uintptr_t instance=0,player=0,gameState=0,mapPointer=0;
    std::uint64_t lifetime=0,entered=0;
    std::uint32_t mapHash=2166136261u;
    int cursor=-1;
    char map[48]{};
};
static bool DiceSnapshotPinsD84(std::uintptr_t b) noexcept {
    if(!retail_build112::Ready(b))return false;
    static std::uintptr_t checked=0;static bool good=false;
    if(checked==b)return good;
    constexpr unsigned char mapRead[]{0x48,0x8B,0x49,0x30};
    constexpr unsigned char indexRead[]{0x83,0xB9,0xF0,0x04,0x00,0x00,0x00};
    constexpr unsigned char indexClear[]{0x89,0x99,0xF0,0x04,0x00,0x00};
    constexpr unsigned char stateRead[]{0x48,0x8B,0x83,0xA0,0x04,0x00,0x00};
    __try{
        good=!std::memcmp(reinterpret_cast<const void*>(b+0x2E92D60),mapRead,sizeof(mapRead))&&
            !std::memcmp(reinterpret_cast<const void*>(b+0x2E8B8D6),indexRead,sizeof(indexRead))&&
            !std::memcmp(reinterpret_cast<const void*>(b+0x2E951F1),indexClear,sizeof(indexClear))&&
            !std::memcmp(reinterpret_cast<const void*>(b+0x2E8B06C),stateRead,sizeof(stateRead));
    }__except(EXCEPTION_EXECUTE_HANDLER){good=false;}
    checked=b;return good;
}
static bool ReadDiceSnapshotD84(DiceSnapshotD84& sample) noexcept {
    sample={};const auto& w=witness175;
    if(!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire))return false;
    sample.lifetime=w.lifetime175.load(std::memory_order_acquire);
    sample.entered=w.diceStart.load(std::memory_order_acquire);
    sample.instance=w.dice.load(std::memory_order_acquire);
    const auto b=base175.load(std::memory_order_acquire);
    if(!sample.instance||!b||!DiceSnapshotPinsD84(b)||ActiveKind175(reinterpret_cast<void*>(sample.instance))!=Kind175::Dice)return false;
    const auto a=sample.instance;
    if(!AlchemyInputRead19c4(a+0x18,sample.player)||
       !AlchemyInputRead19c4(a+0x30,sample.mapPointer)||!AlchemyInputRead19c4(a+0x4A0,sample.gameState)||
       !AlchemyInputRead19c4(a+0x4F0,sample.cursor)||!sample.mapPointer)return false;
    // Bounded printable C string. Unknown/truncated pointers do not become a
    // guessed phase. No game-state/selected-die/cup pointer is dereferenced here.
    bool ended=false;
    for(unsigned i=0;i+1<sizeof(sample.map);++i){
        char ch=0;if(!AlchemyInputRead19c4(sample.mapPointer+i,ch))return false;
        if(!ch){ended=true;break;}if(ch<32||ch>126)return false;
        sample.map[i]=ch;sample.mapHash=(sample.mapHash^static_cast<unsigned char>(ch))*16777619u;
    }
    if(!ended)return false;
    std::uintptr_t mapAgain=0,playerAgain=0,stateAgain=0;int cursorAgain=-1;
    return AlchemyInputRead19c4(a+0x30,mapAgain)&&mapAgain==sample.mapPointer&&
        AlchemyInputRead19c4(a+0x18,playerAgain)&&playerAgain==sample.player&&
        AlchemyInputRead19c4(a+0x4A0,stateAgain)&&stateAgain==sample.gameState&&
        AlchemyInputRead19c4(a+0x4F0,cursorAgain)&&cursorAgain==sample.cursor&&
        w.ready.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire)&&
        w.dice.load(std::memory_order_acquire)==a&&w.lifetime175.load(std::memory_order_acquire)==sample.lifetime&&
        w.diceStart.load(std::memory_order_acquire)==sample.entered&&
        ActiveKind175(reinterpret_cast<void*>(a))==Kind175::Dice;
}
static void DiceNativeFrameEndD84() noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    constexpr unsigned bit=1u<<unsigned(cap::Domain::Interaction);
    if(!(cap::gate.mask.load(std::memory_order_relaxed)&bit))return;
    const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(cap::Domain::Interaction,now)||!witness175.ready.load(std::memory_order_acquire)||
       !witness175.dice.load(std::memory_order_acquire))return;
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;
    if(busy.test_and_set(std::memory_order_acquire))return;
    struct Release {~Release(){busy.clear(std::memory_order_release);}} release;
    static std::uint64_t previousSession=0,previousLife=0,lastSample=0;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate notes;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    const auto life=witness175.lifetime175.load(std::memory_order_acquire);
    if(session!=previousSession||life!=previousLife){notes={};lastSample=0;previousSession=session;previousLife=life;}
    if(lastSample&&now>=lastSample&&now-lastSample<50)return;
    lastSample=now;
    DiceSnapshotD84 sample{};
    if(!ReadDiceSnapshotD84(sample)){
        if(notes.observe(session,now,{witness175.dice.load(),0,-1,-1},100,4096,500))
            diagnostic_trace::Write("D84 F5_DICE_NATIVE qualified=0 reason=CURRENT_NATIVE_SNAPSHOT_UNAVAILABLE writes=NONE cupOwner=NONE diceOutcome=NOT_OBSERVED");
        return;
    }
    if(!cap::active(cap::Domain::Interaction,now)||
       !notes.observe(session,now,{sample.instance,sample.mapHash,sample.cursor,1},100,4096,500))return;
    char line[880]{};
    std::snprintf(line,sizeof(line),
        "D84 F5_DICE_NATIVE session=%llu tick=%llu qualified=1 instance=%llX life=%llu entered=%llu player=%llX nativeMap=%s nativeCursorIndex=%d nativeStatePointer=%llX cursorIdentity=SEE_F5_DICE_MARKER nativeKeepState=UNJOINED vrDieTarget=SEE_F5_DICE_MARKER cupOwner=NONE handCupControl=NOT_IMPLEMENTED cupGate=NATIVE_CUP_CONTRACT_UNQUALIFIED cupAttachment=UNJOINED cupManipulationInterval=UNJOINED cupThrowCompletion=UNJOINED writes=NONE diceOutcome=NOT_OBSERVED",
        (unsigned long long)session,(unsigned long long)now,(unsigned long long)sample.instance,(unsigned long long)sample.lifetime,
        (unsigned long long)sample.entered,(unsigned long long)sample.player,sample.map,sample.cursor,(unsigned long long)sample.gameState);
    diagnostic_trace::Write(line);
}
