// D30-CMB combo carrier keep-alive (combo_carrier_cb.h). Included inside nhp134 after native_combo_grace.inl.
// hook: retail 90CD98 (C_CombatAnimAction end/once). only Henry's armed ordinary attack fragment on its NATURAL
// duration-crossing route is deferred; everything else calls the original at once. release: Henry's ActorUpdate
// (72F644) BEFORE the native update runs, so the delegate never retires an attack in the middle of its own update.
namespace carrier_cb {
namespace cc=kcd2vr::combo_carrier_cb;
using EndFn=void(__fastcall*)(void*);
std::optional<DetourModKit::hook::Hook> g_hook;
EndFn g_original=nullptr;
std::atomic<bool> g_ready{false};
template<class T> bool ReadCB(std::uintptr_t p,T& out) noexcept {
    if(!p)return false;
    __try{std::memcpy(&out,reinterpret_cast<const void*>(p),sizeof(T));return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct LockCB {LockCB() noexcept {while(cc::tokenLock.test_and_set(std::memory_order_acquire)){}}~LockCB(){cc::tokenLock.clear(std::memory_order_release);}};
void LogCB(const char* what,const char* why,const cc::Token& t) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return; // F3 only
    char m[300]{};std::snprintf(m,sizeof(m),"CB COMBO_CARRIER %s why=%s fragment=%llX attack=%llX stroke=%llu ageMs=%llu deferred=%llu released=%llu dropped=%llu stranded=%llu",
        what,why,(unsigned long long)t.fragment,(unsigned long long)t.attack,(unsigned long long)t.key,
        (unsigned long long)(t.deferredAt&&now>=t.deferredAt?now-t.deferredAt:0),(unsigned long long)cc::deferred.load(),
        (unsigned long long)cc::released.load(),(unsigned long long)cc::dropped.load(),(unsigned long long)cc::stranded.load());
    diagnostic_trace::Write(m);
}
void __fastcall EndDetour(void* fragment) noexcept {
    const auto original=g_original;if(!original)return; // cannot happen once enabled
    // NPC/other fast path: no token, experiment off, or not the natural notification return
    const auto ret=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const bool tokenLive=cc::tokenLive.load(std::memory_order_acquire);
    if(!tokenLive&&(cc::retired.load(std::memory_order_relaxed)||!cc::enabled.load(std::memory_order_relaxed)||ret!=g_base+cc::kNotifyCallRet)){original(fragment);return;}
    cc::Arrival a{};a.fragment=reinterpret_cast<std::uintptr_t>(fragment);a.ret=ret;a.thread=GetCurrentThreadId();
    const auto frame=reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress());
    (void)ReadCB(frame+0x30,a.outerRet);           // 90CCEC's own return (push rdi + sub rsp,20 + call)
    (void)ReadCB(a.fragment,a.fragmentVt);(void)ReadCB(a.fragment+0x98,a.once);(void)ReadCB(a.fragment+0x158,a.functor);
    if(a.functor){(void)ReadCB(a.functor,a.functorVt);(void)ReadCB(a.functor+0x08,a.functorFn);(void)ReadCB(a.functor+0x20,a.functorTarget);}
    cc::Window w{};const auto now=std::uint64_t(GetTickCount64());
    const bool armed=cc::window.read(w)&&cc::armed(w,now);
    if(armed)(void)ReadCB(w.attack+0x80,a.attackOfWindowFragmentSlot);
    cc::Decision d{};cc::Token logged{};
    {LockCB lock;
     d=cc::decide(cc::enabled.load(std::memory_order_relaxed)&&!cc::retired.load(std::memory_order_relaxed),w,armed,a,cc::token,g_base);
     if(d.verdict==cc::Verdict::Defer&&d.why==cc::Why::Defer){
         cc::token=cc::make(w,a,now,cc::windowMs(kcd2vr::combo_hold_r14::maxHoldMs.load(std::memory_order_relaxed)));
         cc::tokenLive.store(true,std::memory_order_release);cc::deferred.fetch_add(1,std::memory_order_relaxed);logged=cc::token;}
     else if(d.verdict==cc::Verdict::DropNative){logged=cc::token;cc::token=cc::Token{};cc::tokenLive.store(false,std::memory_order_release);
         cc::dropped.fetch_add(1,std::memory_order_relaxed);}}
    if(d.verdict==cc::Verdict::Defer){if(d.why==cc::Why::Defer)LogCB("DEFER",cc::name(d.why),logged);return;} // +98 stays 0, delegate not called
    if(d.verdict==cc::Verdict::DropNative)LogCB("DROP",cc::name(d.why),logged);
    else cc::natives.fetch_add(1,std::memory_order_relaxed);
    original(fragment);
}
bool CallOriginalCB(std::uintptr_t fragment) noexcept { // no unwinding objects in this frame (SEH)
    __try{g_original(reinterpret_cast<void*>(fragment));return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Henry's ActorUpdate, before the native update: release / drop / keep the deferred natural end
void Service(void* actor) noexcept {
    if(!cc::tokenLive.load(std::memory_order_acquire)||!g_original)return;
    const auto now=std::uint64_t(GetTickCount64());
    cc::Token t{};{LockCB lock;t=cc::token;}
    if(!t.live||reinterpret_cast<std::uintptr_t>(actor)!=t.actor)return;
    cc::Window w{};const bool armed=cc::window.read(w)&&cc::armed(w,now)&&w.attack==t.attack&&w.fragment==t.fragment&&
        cc::enabled.load(std::memory_order_relaxed)&&!cc::retired.load(std::memory_order_relaxed);
    cc::Live l{};l.thread=GetCurrentThreadId();
    // the manager main slot first: if this attack is not the current main action any more, nothing else is read
    l.readOk=cc::readManagerMain([](std::uintptr_t a,std::uintptr_t& v)noexcept{return ReadCB(a,v);},t.manager,l.managerMain); // D31: [[manager+0x38]+0]
    if(l.readOk&&l.managerMain==t.attack){
        l.readOk=ReadCB(t.attack,l.attackVt)&&ReadCB(t.attack+0x80,l.attackFragment)&&ReadCB(t.fragment,l.fragmentVt)&&
            ReadCB(t.fragment+0x98,l.once)&&ReadCB(t.fragment+0x158,l.functor)&&l.functor&&ReadCB(l.functor+0x20,l.functorTarget);}
    const auto r=cc::release(t,armed,w.key,l,now,g_base);
    if(r.what==cc::Release::Keep)return;
    {LockCB lock;if(!cc::token.live||cc::token.fragment!=t.fragment||cc::token.deferredAt!=t.deferredAt)return; // a native route already took it
     cc::token=cc::Token{};cc::tokenLive.store(false,std::memory_order_release);}
    if(r.what==cc::Release::Call){
        cc::released.fetch_add(1,std::memory_order_relaxed);LogCB("RELEASE",cc::name(r.why),t);
        if(!CallOriginalCB(t.fragment)){cc::faults.fetch_add(1,std::memory_order_relaxed);cc::retired.store(true,std::memory_order_release);
            diagnostic_trace::Write("CB COMBO_CARRIER FAULT on release - carrier keep-alive retired for this session");}
        return;}
    if(r.what==cc::Release::Strand){cc::stranded.fetch_add(1,std::memory_order_relaxed);LogCB("STRANDED",cc::name(r.why),t);return;}
    cc::dropped.fetch_add(1,std::memory_order_relaxed);LogCB("DROP",cc::name(r.why),t);
}
bool Install() noexcept {
    if(g_ready.load())return true;
    if(!Readable(reinterpret_cast<void*>(g_base+0x90CD25),sizeof(cc::kNotifyCall))||std::memcmp(reinterpret_cast<void*>(g_base+0x90CD25),cc::kNotifyCall,sizeof(cc::kNotifyCall))||
       !Readable(reinterpret_cast<void*>(g_base+0x4B3FBF),sizeof(cc::kControllerCall))||std::memcmp(reinterpret_cast<void*>(g_base+0x4B3FBF),cc::kControllerCall,sizeof(cc::kControllerCall))){
        diagnostic_trace::Write("CB COMBO_CARRIER install refused: call-site pins mismatch (combo hold stays clock-only)");return false;}
    if(!Hook(g_hook,g_original,cc::kEnd,cc::kEndEntry,&EndDetour,"kcd2vr_cb_combo_carrier_end")){
        diagnostic_trace::Write("CB COMBO_CARRIER install failed (combo hold stays clock-only)");return false;}
    g_ready.store(true,std::memory_order_release);
    diagnostic_trace::Write("CB COMBO_CARRIER ready: 90CD98 natural-end defer for Henry's armed attack only (COMBO HELP: HOLD), release in Henry's ActorUpdate");
    return true;
}
void Uninstall() noexcept {
    cc::enabled.store(false,std::memory_order_release);
    if(g_hook){(void)g_hook->disable();} // keep handle/original alive: a deferred token may still need the original (dropped below)
    {LockCB lock;cc::token=cc::Token{};cc::tokenLive.store(false,std::memory_order_release);}
    g_ready.store(false,std::memory_order_release);
}
}
