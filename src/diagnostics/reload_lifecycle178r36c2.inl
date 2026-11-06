// Exact-retail lifecycle observers plus the R59.6C2 F11=2 bow stage gates.
// The two gates redirect qualified native helpers; recording remains separate.
namespace rl36 = kcd2vr::diagnostics::reload36c2;
using ReloadContext36=DetourModKit::hook::MidContext;
using ReloadReg36=DetourModKit::hook::Gpr;
static rl36::Queue<> g_reload_queue36;
static std::atomic<bool> g_reload_ready36{false},g_reload_enabled36{false};
static std::atomic<std::uint64_t> g_reload_generation36{0},g_reload_call36{0},g_reload_events36{0},g_reload_budget36{0},g_reload_scope_loss36{0};
static std::uintptr_t g_reload_attempted36=0;
static thread_local rl36::Scopes<> g_reload_hand36,g_reload_transfer36,g_reload_bind36,g_reload_nock36;
static bool ReloadActive36() noexcept {
    return g_reload_enabled36.load(std::memory_order_acquire)&&g_reload_ready36.load()&&
        ReloadDiagnostics175()&&retail_build112::Ready(WhBase());
}
static std::uintptr_t ReloadRegValue36(ReloadContext36& c,ReloadReg36 r) noexcept {return DetourModKit::hook::gpr(c,r);}
static bool ReloadVft36(std::uintptr_t object,std::uintptr_t expected) noexcept {
    std::uintptr_t vf=0;return object&&Read(reinterpret_cast<void*>(object),vf)&&vf==WhBase()+expected;
}
static bool ReloadAction36(std::uintptr_t action) noexcept {return ReloadVft36(action,0x3E78CF8);}
static bool ReloadHenry36(std::uintptr_t actor,rl36::Event& e) noexcept {
    std::uintptr_t entity=0;std::uint32_t id=0;
    if(!ReloadVft36(actor,0x3AA4B98)||!Read(reinterpret_cast<void*>(actor+0x38),entity)||
       entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))||
       !ReloadVft36(entity,0x4072C78)||!Read(reinterpret_cast<void*>(entity+0xC),id)||!id)return false;
    e.actor=actor;e.henry=id;return true;
}
static rl36::Event ReloadEvent36(rl36::Kind kind,std::uint32_t rva,std::uintptr_t action) noexcept {
    rl36::Event e{};e.kind=kind;e.rva=rva;e.action=action;
    e.session=g_reload_generation36.load();e.tick=GetTickCount64();e.call=++g_reload_call36;return e;
}
static void ReloadHandFields36(rl36::Event& e) noexcept {
    // Snapshot while the native call owns this action. Unreadable != empty.
    e.handRead=Read(reinterpret_cast<void*>(e.action+0x88),e.handId);
    (void)Read(reinterpret_cast<void*>(e.action+0x8C),e.side);
    // B3 retail2893C08: legacy field name; +80 is the shooting action, not its auxiliary entity.
    (void)Read(reinterpret_cast<void*>(e.action+0x80),e.lever);
}
static void ReloadItem36(std::uintptr_t item,std::uintptr_t& pointer,std::uint64_t& key,bool& ok) noexcept {
    pointer=item;ok=ReloadVft36(item,0x3A2DA88)&&Read(reinterpret_cast<void*>(item+0x30),key);
}
static void ReloadNockFields36(rl36::Event& e) noexcept {
    e.nockRead=e.receiver&&Read(reinterpret_cast<void*>(e.receiver+0xDC),e.nockId);
    std::uintptr_t vf=0;
    if(e.receiver&&Read(reinterpret_cast<void*>(e.receiver),vf)&&vf>=WhBase()&&vf-WhBase()<0x6000000)e.receiverVft=vf-WhBase();
}
static void ReloadEmit36(rl36::Event e) noexcept {
    if(!ReloadActive36()||e.session!=g_reload_generation36.load())return;
    if(g_reload_events36.fetch_add(1)>=4096){++g_reload_budget36;return;}
    e.tick=GetTickCount64();g_reload_queue36.push(e);
}
static void ReloadScope36(rl36::Scopes<>& scopes,std::uintptr_t sp,const rl36::Event& e) noexcept {
    if(!scopes.put(sp,e))++g_reload_scope_loss36;
}
static bool ReloadCaller36(std::uintptr_t sp,std::uintptr_t rva) noexcept {
    std::uintptr_t caller=0;return Read(reinterpret_cast<void*>(sp),caller)&&caller==WhBase()+rva;
}
static bool ReloadHenryActor37(std::uintptr_t actor) noexcept {rl36::Event owner{};return ReloadHenry36(actor,owner);}
// C37 automatic-request caller set. 18468BC is a mid hook at function ENTRY:
// rcx/dl/r8b are the request arguments, [rsp] is the return address and
// rbx/rdi are still the caller's callee-saved values (18468BC first stores
// them). 2AAA4C0 restores rbx and rsp before its tail jmp 2AAA505, so the
// lambda/cold frame that called 2AAA4C0 is what this hook sees. Every relation
// below is pinned by ReloadCallerPins37 before g_reload_callers37 is set.
// 2AAA012 keeps its C32 admission (return address only). Crossbow/gun
// dispatcher (1F245F0 tail jmp) and every direct one-shot/result/counter
// caller stay outside this set and therefore native.
enum class ReloadAutoCaller37 : unsigned char {Unknown,UpdateRetry,BowPrimary,BowSecondary,TickRetry,EquipmentPending40};
static std::atomic<bool> g_reload_callers37{false};
static ReloadAutoCaller37 ReloadAutoCallerKind37(std::uintptr_t rva) noexcept {
    switch(rva){case 0x2AAA012:return ReloadAutoCaller37::UpdateRetry;case 0x1F24836:return ReloadAutoCaller37::BowPrimary;
        case 0x1F2469E:return ReloadAutoCaller37::BowSecondary;case 0x224A38C:return ReloadAutoCaller37::TickRetry;case 0x222CB40:return ReloadAutoCaller37::EquipmentPending40;default:return ReloadAutoCaller37::Unknown;}
}
static const char* ReloadAutoCallerName37(ReloadAutoCaller37 k) noexcept {
    switch(k){case ReloadAutoCaller37::UpdateRetry:return "UPDATE_RETRY_2AAA012";case ReloadAutoCaller37::BowPrimary:return "BOW_PRIMARY_1F24836";
        case ReloadAutoCaller37::BowSecondary:return "BOW_SECONDARY_1F2469E";case ReloadAutoCaller37::TickRetry:return "TICK_RETRY_224A38C";case ReloadAutoCaller37::EquipmentPending40:return "EQUIPMENT_PENDING_222CB40";default:return "UNKNOWN";}
}
// Only the new callers read memory here, and only in manual mode. Returns the
// controller whose native +20 pending latch (set by 2AAA4C9, never by us) owns
// a controller request; C40 equipment-pending requests have no controller.
// No write, no retry, no native call.
static bool ReloadAutoContext37(ReloadContext36& c,ReloadAutoCaller37 k,std::uintptr_t equipment,std::uintptr_t& ctl,int& latch,const char** reason) noexcept {
    auto refused=[&](const char* why) noexcept {if(reason)*reason=why;return false;};
    ctl=0;latch=-1;
    if(k==ReloadAutoCaller37::UpdateRetry){ctl=ReloadRegValue36(c,ReloadReg36::Rdi);return true;} // 2AA9E25 mov rdi,rcx; unchanged C32 admission
    if(k==ReloadAutoCaller37::Unknown)return refused("CALLER_NOT_MATCHED");
    if(!g_reload_callers37.load())return refused("CALLER_PINS");
    // C40: 222CB16 clears the equipment's own +5C pending byte, then
    // 222CB33..222CB3B requests (equipment=rbx,dl=0,r8b=0). Its return at
    // 222CB40 is unused. This is NOT the controller +20 latch path. Keep the
    // existing exact current-Henry/bow/fresh-frame admission below; no latch write.
    if(k==ReloadAutoCaller37::EquipmentPending40){
        if((ReloadRegValue36(c,ReloadReg36::Rdx)&0xFF)!=0||(ReloadRegValue36(c,ReloadReg36::R8)&0xFF)!=0)return refused("CALLER_ARGS");
        if(ReloadRegValue36(c,ReloadReg36::Rbx)!=equipment)return refused("CALLER_CONTEXT");
        return true;
    }
    // 2AAA4FB xor r8d,r8d / 2AAA4FE mov dl,1: the automatic-shot request only.
    if((ReloadRegValue36(c,ReloadReg36::Rdx)&0xFF)!=1||(ReloadRegValue36(c,ReloadReg36::R8)&0xFF)!=0)return refused("CALLER_ARGS");
    if(k==ReloadAutoCaller37::TickRetry){
        // 82F746 mov rdi,rcx (controller); 82F759 mov rbx,rax (equipment); 224A381 rdx=rbx.
        if(ReloadRegValue36(c,ReloadReg36::Rbx)!=equipment)return refused("CALLER_CONTEXT");
        ctl=ReloadRegValue36(c,ReloadReg36::Rdi);
    } else {
        // 1F2480A/1F2467A mov rbx,rcx: std::function impl {vft,[+8] controller,[+10] equipment} built at 1786081.
        const auto closure=ReloadRegValue36(c,ReloadReg36::Rbx);std::uintptr_t vft=0,captured=0;
        if(!closure||!Read(reinterpret_cast<void*>(closure),vft)||
           vft!=WhBase()+(k==ReloadAutoCaller37::BowPrimary?0x3EC9610:0x3EC97D0)||
           !Read(reinterpret_cast<void*>(closure+8),ctl)||!Read(reinterpret_cast<void*>(closure+0x10),captured)||
           captured!=equipment)return refused("CALLER_CONTEXT");
    }
    // 2AA9E28/82F749 [controller+18] is the actor whose +280 yields this equipment.
    std::uintptr_t actor=0;
    if(!ctl||!Read(reinterpret_cast<void*>(ctl+0x18),actor)||!ReloadHenryActor37(actor))return refused("CONTROLLER_OWNER");
    std::uint8_t pending=0;if(!Read(reinterpret_cast<void*>(ctl+0x20),pending))return refused("CONTROLLER_OWNER");
    latch=pending;if(pending!=1)return refused("TRIGGER_RELEASED");
    return true;
}
static bool ManualAutomaticFault32() noexcept; // atomic state only, defined with manual owner
static bool ManualAutomaticReload19c3(void*,const char**) noexcept;
static bool CrossbowRifleDeferH21(void*,unsigned char,unsigned char) noexcept;
static void ReloadAutomaticRequest19c3(ReloadContext36& c) noexcept {
    // Bow keeps its exact automatic-controller caller policy. F11=2 ranged
    // staging may park a naturally invoked, separately typed current-Henry
    // request; default/rung1 and other actors stay native.
    const bool ready=g_reload_ready36.load();
    const bool callerChecked=ready;
    // C37: one read of the entry return address serves admission and receipt.
    const auto entryStack35=DetourModKit::hook::stack_pointer(c);
    std::uintptr_t callerRaw35=0;const bool callerRead35=callerChecked&&Read(reinterpret_cast<void*>(entryStack35),callerRaw35);
    const auto base35=WhBase();const auto callerMinusWH35=callerRead35&&callerRaw35>=base35?callerRaw35-base35:~std::uintptr_t(0);
    const auto kind37=callerRead35?ReloadAutoCallerKind37(callerMinusWH35):ReloadAutoCaller37::Unknown;
    const bool callerMatch=kind37!=ReloadAutoCaller37::Unknown;
    const char* reason=ready?(callerMatch?"NOT_CHECKED":"CALLER_NOT_MATCHED"):"NOT_READY";
    const auto equipment=ReloadRegValue36(c,ReloadReg36::Rcx);
    // Manual mode OFF never reads caller context: native auto reload is the default.
    const bool manual37=callerMatch&&(kind37==ReloadAutoCaller37::UpdateRetry||StagedBowEnabled59());
    if(callerMatch&&!manual37)reason="MANUAL_MODE_OFF";
    std::uintptr_t controller37=0;int latch37=-1;
    const bool context37=manual37&&ReloadAutoContext37(c,kind37,equipment,controller37,latch37,&reason);
    const bool rangedH21=ready&&CrossbowRifleDeferH21(reinterpret_cast<void*>(equipment),
        static_cast<unsigned char>(ReloadRegValue36(c,ReloadReg36::Rdx)),static_cast<unsigned char>(ReloadRegValue36(c,ReloadReg36::R8)));
    const bool deferred=rangedH21||(ready&&context37&&ManualAutomaticReload19c3(reinterpret_cast<void*>(equipment),&reason));
    if(rangedH21)reason="H21_RANGED_ORIGINAL_REQUEST_PARKED";
    if(deferred)DetourModKit::hook::instruction_pointer(c)=reinterpret_cast<std::uintptr_t>(&DeferredReload596);
    if(!ReloadActive36())return; // OFF: no formatting, extra native query or admission retry
    // Refused factory calls can include foreign/reentrant producers. This try
    // gate protects only the finite receipt budget, AFTER gameplay admission.
    // Never wait on another native callback or serialize the native decision.
    static std::atomic_flag receiptBusy32=ATOMIC_FLAG_INIT;
    if(receiptBusy32.test_and_set(std::memory_order_acquire))return;
    struct ReceiptExit32 {std::atomic_flag& gate;~ReceiptExit32(){gate.clear(std::memory_order_release);}} receiptExit32{receiptBusy32};
    static std::uint64_t session=0;static unsigned count=0,repeats=0;
    struct Edge37 {std::uintptr_t caller=0;bool deferred=false;const char* reason=nullptr;std::uintptr_t equipment=0,controller=0;
        bool operator==(const Edge37& o) const noexcept {return caller==o.caller&&deferred==o.deferred&&reason==o.reason&&equipment==o.equipment&&controller==o.controller;}};
    static std::array<Edge37,4> recent37{};static unsigned next37=0;
    const auto current=g_reload_generation36.load();if(session!=current){session=current;count=0;repeats=0;recent37={};next37=0;}
    // C37 edge-only: a repeat of one of the last four distinct outcomes (a held
    // trigger retried every tick by 2AAA012 and/or 224A38C) is counted, not
    // printed, so later press/caller/reason changes still get receipts.
    const Edge37 edge37{callerMinusWH35,deferred,reason,equipment,controller37};
    for(std::size_t i=0;i<recent37.size()&&i<count;++i)if(recent37[i]==edge37){++repeats;return;}
    if(count>=16)return;++count;recent37[next37++%recent37.size()]=edge37;const unsigned repeated37=repeats;repeats=0;
    // Evidence only, AFTER the original admission/OFF/budget gates. Reuses the
    // admission read of the entry return address; no further native query.
    char line[768]{};std::snprintf(line,sizeof(line),
        "C32 F11_STAGE stage=%s reason=%s equipment=%p callerExpected=2AAA012|1F24836|1F2469E|224A38C|222CB40 callerChecked=%d callerMatch=%d manualMode=%d ready=%d fault=%d nativeActionAllocated=%s ammoWrites=NONE receipt=%u/16 hookRva=18468BC entryStack=%p callerReadable=%d callerRaw=%p callerMinusWH=%llX callerKind=%s callerPins=%d context=%d controller=%p pendingLatch=%d latchWrites=NONE repeatsBefore=%u",
        deferred?"AUTO_RELOAD_DEFERRED":"AUTO_RELOAD_REFUSED",reason,reinterpret_cast<void*>(equipment),
        callerChecked?1:0,callerMatch?1:0,StagedBowEnabled59()?1:0,ready?1:0,ManualAutomaticFault32()?1:0,
        deferred?"0":"NATIVE_PATH",count,reinterpret_cast<void*>(entryStack35),callerRead35?1:0,
        reinterpret_cast<void*>(callerRaw35),(unsigned long long)callerMinusWH35,ReloadAutoCallerName37(kind37),
        g_reload_callers37.load()?1:0,context37?1:0,reinterpret_cast<void*>(controller37),latch37,repeated37);diagnostic_trace::Write(line);
}
#include "diagnostics/reload_action_admission64.inl"
static void ReloadDispatch36(ReloadContext36& c) noexcept {
    kcd2vr::crossbow_native_xr34::BeforeNativeEvent(reinterpret_cast<void*>(ReloadRegValue36(c,ReloadReg36::Rcx)),reinterpret_cast<const void*>(ReloadRegValue36(c,ReloadReg36::Rdx)));
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto action=ReloadRegValue36(c,ReloadReg36::Rcx),event=ReloadRegValue36(c,ReloadReg36::Rdx);
    if(!ReloadAction36(action)||!event)return;
    auto e=ReloadEvent36(rl36::Kind::Dispatch,0x289CCD0,action);
    if(!Read(reinterpret_cast<void*>(event+0x1C),e.hash))return;
    ReloadHandFields36(e);ReloadEmit36(e);
    // No owner getter is called here. Candidate dispatches are explicitly
    // UNQUALIFIED; only the following game-resolved owner sites prove Henry.
}
// C37 F11 join: 2886BCE runs at 2886B24 entry-28h (push rdi; sub rsp,20h).
// 289618D calls 2886B24 (ret 2896192) at 2896174 entry-30h; 1846A4F calls
// 2896174 (ret 1846A54) at 18468BC entry-30h. Both live return slots must
// match, so the 18468BC entry stack is sp+88h and [entry] is the ORIGINAL
// caller that asked for this action. Not a timer, thread-local or guess.
static void ReloadOrigin37(std::uintptr_t sp,rl36::Event& e) noexcept {
    std::uintptr_t raw=0;const auto entry=sp+0x88,base=WhBase();
    if(!g_reload_callers37.load()||!ReloadCaller36(sp+0x28,0x2896192)||!ReloadCaller36(sp+0x58,0x1846A54)||
       !Read(reinterpret_cast<void*>(entry),raw)||raw<base||raw-base>0x6000000)return;
    e.origin=std::uint32_t(raw-base);e.originStack=entry;
}
static void ReloadConstruct36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;const auto a=ReloadRegValue36(c,ReloadReg36::Rbx);
    if(ReloadAction36(a)){auto e=ReloadEvent36(rl36::Kind::Construct,0x2886BCE,a);ReloadHandFields36(e);
        ReloadOrigin37(DetourModKit::hook::stack_pointer(c),e);ReloadEmit36(e);}
}
static void ReloadDestroy36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;const auto a=ReloadRegValue36(c,ReloadReg36::Rcx);
    if(ReloadAction36(a)){auto e=ReloadEvent36(rl36::Kind::DestroyBegin,0x28952CC,a);ReloadHandFields36(e);ReloadEmit36(e);}
    // Destructor entry is not proof of completed cleanup or ammunition loss.
}
static void ReloadHandBegin36(ReloadContext36& c) noexcept {
    if(g_reload_ready36.load()&&(ReloadRegValue36(c,ReloadReg36::Rdx)&255)&&
       ManualAction596(ReloadRegValue36(c,ReloadReg36::Rcx))){
        DetourModKit::hook::instruction_pointer(c)=reinterpret_cast<std::uintptr_t>(&DeferredReload596);return;
    }
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c),a=ReloadRegValue36(c,ReloadReg36::Rcx);
    g_reload_hand36.erase(sp);
    if(ReloadCaller36(sp,0x289C940))g_reload_transfer36.erase(sp+0x30);
    if(!ReloadAction36(a))return;
    auto e=ReloadEvent36(rl36::Kind::HandBegin,0x289595C,a);
    e.create=(ReloadRegValue36(c,ReloadReg36::Rdx)&255)!=0;
    ReloadHandFields36(e);ReloadScope36(g_reload_hand36,sp,e);
    // Held in TLS until the GAME supplies an exact Henry actor below.
}
static void ReloadHandOwner36(ReloadContext36& c,bool create) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(!ReloadCaller36(sp,create?0x289599D:0x28959C7))return;
    auto* e=g_reload_hand36.find(sp+0x30,g_reload_generation36.load());
    if(!e||e->create!=int(create)||e->action+0x88!=ReloadRegValue36(c,ReloadReg36::R8)||
       !ReloadAction36(e->action)||!ReloadHenry36(ReloadRegValue36(c,ReloadReg36::Rdx),*e))return;
    ReloadEmit36(*e);
}
static void ReloadHandCreate36(ReloadContext36& c) noexcept {ReloadHandOwner36(c,true);}
static void ReloadHandRemove36(ReloadContext36& c) noexcept {ReloadHandOwner36(c,false);}
static void ReloadHandItems36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    // Create helper entry RSP = current RSP+48h; caller's entry = +78h.
    auto* p=g_reload_hand36.find(sp+0x78,g_reload_generation36.load());
    if(!p||!p->henry||p->action+0x88!=ReloadRegValue36(c,ReloadReg36::Rsi)||
       p->actor!=ReloadRegValue36(c,ReloadReg36::Rdi))return;
    ReloadItem36(ReloadRegValue36(c,ReloadReg36::Rbp),p->weapon,p->weaponKey,p->weaponRead);
    ReloadItem36(ReloadRegValue36(c,ReloadReg36::R15),p->ammo,p->ammoKey,p->ammoRead);
    auto e=*p;e.kind=rl36::Kind::HandItems;e.rva=0x2895A18;ReloadEmit36(e);
}
static void ReloadHandEnd36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c),entry=sp+0x28;
    if(auto* p=g_reload_hand36.find(entry,g_reload_generation36.load());p&&p->henry&&
       p->action==ReloadRegValue36(c,ReloadReg36::Rdi)&&ReloadAction36(p->action)){
        auto e=*p;e.kind=rl36::Kind::HandEnd;e.rva=0x28959C7;ReloadHandFields36(e);ReloadEmit36(e);
        // Transfer handler +289C930 ALWAYS calls this helper before +289C988.
        // Exact return site and parent stack join the two calls; never a timer.
        if(ReloadCaller36(entry,0x289C940))ReloadScope36(g_reload_transfer36,entry+0x30,e);
    }
    g_reload_hand36.erase(entry);
}
static void ReloadTransfer36(ReloadContext36& c) noexcept {
    if(g_reload_ready36.load()&&ReloadCaller36(DetourModKit::hook::stack_pointer(c),0x289C957)&&
       ManualTransfer596(reinterpret_cast<void*>(ReloadRegValue36(c,ReloadReg36::Rdx)))){
        DetourModKit::hook::instruction_pointer(c)=reinterpret_cast<std::uintptr_t>(&DeferredReload596);return;
    }
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c),a=ReloadRegValue36(c,ReloadReg36::Rbx);
    g_reload_bind36.erase(sp);
    auto e=ReloadEvent36(rl36::Kind::Transfer,0x289C988,a);
    if(!ReloadCaller36(sp,0x289C957)||!ReloadAction36(a)||!ReloadHenry36(ReloadRegValue36(c,ReloadReg36::Rdx),e))return;
    if(auto* p=g_reload_transfer36.find(sp+0x30,e.session);p&&p->action==a&&p->actor==e.actor)e.call=p->call;
    g_reload_transfer36.erase(sp+0x30);
    ReloadHandFields36(e);ReloadEmit36(e);ReloadScope36(g_reload_bind36,sp,e);
}
static void ReloadAmmoBind36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    g_reload_nock36.erase(sp);
    if(!ReloadCaller36(sp,0x289C9E0))return;
    if(auto* p=g_reload_bind36.find(sp+0x30,g_reload_generation36.load());p&&p->henry){
        auto e=*p;e.kind=rl36::Kind::AmmoBind;e.rva=0x2A8A0C0;e.receiver=ReloadRegValue36(c,ReloadReg36::Rcx);
        ReloadItem36(ReloadRegValue36(c,ReloadReg36::Rdi),e.weapon,e.weaponKey,e.weaponRead);
        ReloadItem36(ReloadRegValue36(c,ReloadReg36::Rdx),e.ammo,e.ammoKey,e.ammoRead);
        ReloadNockFields36(e);ReloadEmit36(e);ReloadScope36(g_reload_nock36,sp,e);
    }
    g_reload_bind36.erase(sp+0x30);
}
static void ReloadNockBegin36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    // This map uses separate exact frames for the parent bind and child create.
    g_reload_nock36.erase(sp);
    if(!ReloadCaller36(sp,0x2A8A101))return;
    if(auto* p=g_reload_nock36.find(sp+0x30,g_reload_generation36.load());p&&p->henry&&p->ammo==ReloadRegValue36(c,ReloadReg36::Rdx)){
        auto e=*p;e.kind=rl36::Kind::NockBegin;e.rva=0x2A8A030;e.receiver=ReloadRegValue36(c,ReloadReg36::Rcx);
        e.receiverVft=0;ReloadNockFields36(e);ReloadEmit36(e);ReloadScope36(g_reload_nock36,sp,e);
    }
    g_reload_nock36.erase(sp+0x30);
}
static void ReloadNockEnd36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto entry=DetourModKit::hook::stack_pointer(c)+0x58;
    if(auto* p=g_reload_nock36.find(entry,g_reload_generation36.load());p&&p->receiver==ReloadRegValue36(c,ReloadReg36::Rbx)){
        auto e=*p;e.kind=rl36::Kind::NockEnd;e.rva=0x2A8A0AD;ReloadNockFields36(e);ReloadEmit36(e);
    }
    g_reload_nock36.erase(entry);
}
static void ReloadCocking36(ReloadContext36& c) noexcept {
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto a=ReloadRegValue36(c,ReloadReg36::Rbx);
    auto e=ReloadEvent36(rl36::Kind::Cocking,0x2895F47,a);
    if(ReloadAction36(a)&&ReloadHenry36(ReloadRegValue36(c,ReloadReg36::Rax),e)){ReloadHandFields36(e);ReloadEmit36(e);}
}
static void ReloadFinalize47C2(ReloadContext36& c) noexcept {
    kcd2vr::crossbow_native_xr34::BeforeNativeComplete(reinterpret_cast<void*>(ReloadRegValue36(c,ReloadReg36::Rcx)),reinterpret_cast<void*>(ReloadRegValue36(c,ReloadReg36::Rdx)));
    if(!ReloadActive36())return;ReadScope101R2 pages;
    const auto a=ReloadRegValue36(c,ReloadReg36::Rcx);if(!ReloadAction36(a))return;
    auto e=ReloadEvent36(rl36::Kind::Finalize,0x289D0B4,a);ReloadHandFields36(e);
    (void)Read(reinterpret_cast<void*>(a+0x98),e.abort);
    (void)Read(reinterpret_cast<void*>(a+0x99),e.finishNock);
    (void)Read(reinterpret_cast<void*>(a+0x9A),e.finishCock);
    ReloadEmit36(e); // candidate only; subsequent native calls qualify the owner
}
struct ReloadSite36 {std::uintptr_t rva;std::array<unsigned char,16> bytes;void(*callback)(ReloadContext36&) noexcept;};
#include "reload_lifecycle_sites178r36c2.inl"
// C37 caller-context pins (exact retail bytes, WHGame 6A350E20). They prove the
// register/stack relations ReloadAutoContext37 and ReloadOrigin37 read. A
// mismatch disables ONLY the C37 expansion/origin join; 2AAA012 stays as C32.
struct ReloadCallerPin37 {std::uint32_t rva;const char* hex;};
static constexpr ReloadCallerPin37 kReloadCallerPins37[]{
    {0x222CB16,"488d4b18c6435c00e8d59264fe488bcbe8512fa3fe84c00f841daf50fe4533c033d2488bcbe87c9d61ff90e90aaf50fe"}, // C40 equipment pending retry, unused return

    {0x2AAA4C0,"40534883ec20488bdac6412001"}, // push rbx; mov rbx,rdx; mov byte[rcx+20],1 (native latch)
    {0x2AAA4FB,"4533c0b2014883c4205be9b2c3d9fe"}, // r8d=0; dl=1; restore rsp/rbx; jmp 18468BC
    {0x1F24800,"40534883ec20488b4108488bd9807820007536e8c86558fe83b8c4000000017428488b4308c6402101488b5310488b4b08e88a5cb800488b4b084533c033d24883c4205be9"}, // bow_primary
    {0x1F24670,"40534883ec20488b4108488bd980782000752ee8586758fe83b8c4000000007420488b5310488b4b08e8225eb800488b4b0841b00133d24883c4205be9"}, // bow_secondary
    {0x224A367,"807b59000f84ff535efe488bcbe80357a1fe84c00f84ef535efe488bd3488bcfe83401860090e9de535efe"}, // tick retry cold
    {0x82F73C,"48895c2408574883ec20488bf9488b4918488b8980020000e8b73bfcff488bd8807859000f85aaaba101807f20000f85f7aba101488b5c24304883c4205fc3"}, // rdi=ctl rbx=equipment
    {0x2AA9E25,"488bf9488b4918488b8980020000e8d894d4fd"}, // Update: rdi=ctl, [ctl+18]+280 -> 7F3310
    {0x2AA9FE7,"807f20007438488bcfe883deffff84c0752c488bcee87b5a1bfe488bce84c0740c4533c0b201e8aac8d9feeb11"}, // Update retry 2AAA00D
    {0x1786055,"488b4e18488b9028010000488b89800200004c8b7260e8a0d206ff498b0e488d15ce354802488bf8488975ff488d058835740248897d07"}, // closure {3EC9610,ctl,equipment}
    {0x1F245F0,"488b5110488b4908e9c35eb800"}, // crossbow/gun tail jmp keeps dispatcher return: EXCLUDED
    {0x1846A46,"488b4d504885c97407e820f70401"}, // 18468BC -> 2896174 (ret 1846A54)
    {0x2896174,"48895c2418574883ec20488b5158488bd94c8bc1488d4c2430e89209ffff"}, // -> 2886B24 (ret 2896192)
    {0x2886B24,"48895c241048896c24184889742420574883ec20"}, // construct frame entry-28h
};
struct ReloadSlotPin37 {std::uint32_t slot,target;};
static constexpr ReloadSlotPin37 kReloadSlotPins37[]{{0x3EC9620,0x1F24800},{0x3EC97E0,0x1F24670}}; // _Do_call slots
static bool ReloadCallerPins37(std::uintptr_t base) noexcept {
    auto nib=[](char ch) noexcept {return ch<='9'?ch-'0':ch-'a'+10;};
    for(const auto& p:kReloadCallerPins37)for(std::size_t j=0;p.hex[2*j];++j){unsigned char b=0;
        if(!Read(reinterpret_cast<void*>(base+p.rva+j),b)||b!=((nib(p.hex[2*j])<<4)|nib(p.hex[2*j+1])))return false;}
    for(const auto& s:kReloadSlotPins37){std::uintptr_t v=0;
        if(!Read(reinterpret_cast<void*>(base+s.slot),v)||v!=base+s.target)return false;}
    return true;
}
static std::array<std::optional<DetourModKit::hook::Hook>,kReloadSites36.size()> g_reload_hooks36;
static rl36::HandRemoveEntryT30 g_reload_hand_entry_t30;
static bool CopyReloadHandEntryT30(std::uintptr_t at,void* out,std::size_t n) noexcept {
    if(n!=16)return false;auto* bytes=static_cast<unsigned char*>(out);
    for(std::size_t j=0;j<n;++j)if(!Read(reinterpret_cast<void*>(at+j),bytes[j]))return false;
    return true;
}

static bool ArmReloadNative36() noexcept {
    const auto base=WhBase();if(!base||!retail_build112::Verify(base))return false;
    if(g_reload_attempted36==base)return g_reload_ready36.load();g_reload_attempted36=base;
    // Verify EVERY pin before any mutation. The full DLL SHA also covers the
    // remainder of relocated instructions. The adjacent hand epilogue/create
    // entries are 17 bytes apart; the epilogue's decoded 16-byte window fits.
    for(const auto& s:kReloadSites36){std::array<unsigned char,16> bytes{};
        for(std::size_t j=0;j<bytes.size();++j)if(!Read(reinterpret_cast<void*>(base+s.rva+j),bytes[j]))return false;
        if(bytes!=s.bytes)return false;
    }
    g_reload_callers37=ReloadCallerPins37(base); // read-only; expansion off on mismatch
    using namespace DetourModKit::hook;
    for(std::size_t i=0;i<kReloadSites36.size();++i){const auto& s=kReloadSites36[i];Options o{};o.fail_if_already_hooked=true;
        auto h=mid_at(MidRequest{"kcd2vr_reload36c2_"+std::to_string(i),Target{DetourModKit::Address{base+s.rva}},o},s.callback);
        if(h){g_reload_hooks36[i]=std::move(*h);if(g_reload_hooks36[i]->enable()){
            if(s.rva==rl36::kHandRemoveEntryT30&&s.callback==&ReloadHandRemove36){
                (void)g_reload_hand_entry_t30.publish(base,reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),true,
                    CopyReloadHandEntryT30,[&]() noexcept {return bool(g_reload_hooks36[i]->enable());});
            }
            continue;
        }}
        g_reload_hand_entry_t30.clear(); // revoke BEFORE any rollback/restore
        for(auto& installed:g_reload_hooks36)if(installed){(void)installed->disable();installed.reset();}
        return false;
    }
    g_reload_ready36=true;return true;
}
static rl36::Ledger59c1 g_reload_ledger59c1; // control thread; no pointer dereferences
static rl36::ActionCoverage59c10r3 g_reload_actions59c10r3;
static void ServiceReloadNative36() noexcept {
    rl36::Event e{};
    for(int i=0;i<16&&g_reload_queue36.pop(e);++i){
        if(e.session!=g_reload_generation36.load())continue;
        g_reload_ledger59c1.accept(e);
        g_reload_actions59c10r3.accept(e);
        char m[1400]{};std::snprintf(m,sizeof(m),
            "V66.178R36C2 RELOAD_NATIVE session=%llu call=%llu tick=%llu stage=%s rva=%X owner=%s henry=%u action=%p actor=%p event=%s hash=%08X create=%d handRead=%d handId=%u side=%d weaponRead=%d weapon=%p weaponKey=%016llX ammoRead=%d ammo=%p ammoKey=%016llX receiver=%p receiverVft=%llX nockRead=%d nockId=%u lever=%p leverMeaning=SHOOTING_ACTION_NOT_TOOL finalizeAbort=%u finalizeNock=%u finalizeCock=%u origin=%X originKind=%s originStack=%p inventoryDelta=UNOBSERVED physicalConstraint=NONE writes=NONE",
            (unsigned long long)e.session,(unsigned long long)e.call,(unsigned long long)e.tick,rl36::name(e.kind),e.rva,e.henry?"HENRY":"UNQUALIFIED",e.henry,
            reinterpret_cast<void*>(e.action),reinterpret_cast<void*>(e.actor),rl36::event_name(e.hash),e.hash,e.create,e.handRead,e.handId,e.side,e.weaponRead,
            reinterpret_cast<void*>(e.weapon),(unsigned long long)e.weaponKey,e.ammoRead,reinterpret_cast<void*>(e.ammo),(unsigned long long)e.ammoKey,
            reinterpret_cast<void*>(e.receiver),(unsigned long long)e.receiverVft,e.nockRead,e.nockId,reinterpret_cast<void*>(e.lever),unsigned(e.abort),unsigned(e.finishNock),unsigned(e.finishCock),
            e.origin,rl36::origin_name(e.origin),reinterpret_cast<void*>(e.originStack));
        diagnostic_trace::Write(m);
        if(e.kind==rl36::Kind::HandCensus){
            std::snprintf(m,sizeof(m),"R59C1 F11-RELOAD HAND_WORLD session=%llu tick=%llu side=%d heldKey=%016llX entity=%u handRead=%d poseRead=%d matrix=(%.5f,%.5f,%.5f,%.5f;%.5f,%.5f,%.5f,%.5f;%.5f,%.5f,%.5f,%.5f) itemRole=UNCLASSIFIED link=EXACT_KEY_REQUIRED writes=NONE",
                (unsigned long long)e.session,(unsigned long long)e.tick,e.side,(unsigned long long)e.ammoKey,e.handId,e.handRead,e.poseRead59c1,
                double(e.matrix59c1[0]),double(e.matrix59c1[1]),double(e.matrix59c1[2]),double(e.matrix59c1[3]),
                double(e.matrix59c1[4]),double(e.matrix59c1[5]),double(e.matrix59c1[6]),double(e.matrix59c1[7]),
                double(e.matrix59c1[8]),double(e.matrix59c1[9]),double(e.matrix59c1[10]),double(e.matrix59c1[11]));diagnostic_trace::Write(m);
        }
    }
}
static void EndReloadNative36(const char* reason) noexcept {
    if(!g_reload_enabled36.exchange(false))return;
    for(int i=0;i<16;++i)ServiceReloadNative36();
    for(const auto& a:g_reload_actions59c10r3.rows)if(a.session&&a.henry){
        char m[1100]{};std::snprintf(m,sizeof(m),
            "C10R3 F11-RELOAD ACTION_COVERAGE session=%llu lifetime=%llu action=%p origin=%X originKind=%s originStack=%p henry=%u weaponKey=%016llX ammoKey=%016llX constructSeen=%d destroyEntrySeen=%d stages=%llX dispatches=%X qualifiedCalls=%u createRequests=%u removeRequests=%u handRead=%d lastHandEntity=%u nockRead=%d lastNockEntity=%u shootingAction=%p toolEntity=NOT_CAPTURED identityConflict=%d elapsedMs=%llu dropped=%llu budgetRejected=%llu actionOverflow=%llu joins=CALL_LEDGER_REQUIRED loaded=UNPROVEN fired=UNOBSERVED cleanup=UNPROVEN toolType=UNCLASSIFIED writes=NONE",
            (unsigned long long)a.session,(unsigned long long)a.serial,reinterpret_cast<void*>(a.action),a.origin,rl36::origin_name(a.origin),reinterpret_cast<void*>(a.originStack),a.henry,(unsigned long long)a.weapon,(unsigned long long)a.ammo,
            a.constructed,bool(a.mask&(1ull<<unsigned(rl36::Kind::DestroyBegin))),(unsigned long long)a.mask,a.dispatchMask,a.qualifiedCalls,a.createRequests,a.removeRequests,
            a.handRead,a.hand,a.nockRead,a.nock,reinterpret_cast<void*>(a.lever),a.identityConflict,(unsigned long long)(a.last>=a.first?a.last-a.first:0),
            (unsigned long long)g_reload_queue36.dropped.load(),(unsigned long long)g_reload_budget36.load(),(unsigned long long)g_reload_actions59c10r3.overflow);
        diagnostic_trace::Write(m);
    }
    for(const auto& c:g_reload_ledger59c1.calls)if(c.session){char m[800]{};
        std::snprintf(m,sizeof(m),"R59C1 RELOAD_NATIVE_CALL session=%llu call=%llu action=%p henry=%u weaponKey=%016llX ammoKey=%016llX stageMask=%llX handBefore=%u handAfter=%u createRequested=%d removeRequested=%d nockBefore=%u nockAfter=%u nockRead=%d identityConflict=%d elapsedMs=%llu inventoryDelta=UNOBSERVED firedProjectile=UNOBSERVED cleanup=UNPROVEN join=EXACT_NATIVE_CALL",
            (unsigned long long)c.session,(unsigned long long)c.call,reinterpret_cast<void*>(c.action),c.henry,
            (unsigned long long)c.weapon,(unsigned long long)c.ammo,(unsigned long long)c.mask,c.handBefore,c.handAfter,c.created,c.removed,
            c.nockBefore,c.nockAfter,c.nockRead,c.identityConflict,(unsigned long long)(c.last>=c.first?c.last-c.first:0));diagnostic_trace::Write(m);
    }
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.178R36C2 RELOAD_NATIVE_END session=%llu reason=%s queuedAttempts=%llu dropped=%llu budgetRejected=%llu scopeLost=%llu ledgerOverflow=%llu projectileCleanup=UNOBSERVED inventoryDelta=UNOBSERVED",
        (unsigned long long)g_reload_generation36.load(),reason,(unsigned long long)g_reload_events36.load(),
        (unsigned long long)g_reload_queue36.dropped.load(),(unsigned long long)g_reload_budget36.load(),(unsigned long long)g_reload_scope_loss36.load(),(unsigned long long)g_reload_ledger59c1.overflow);diagnostic_trace::Write(m);
}
static bool BeginReloadNative36(std::uint64_t session) noexcept {
    const bool ready=ArmReloadNative36();g_reload_generation36=session;g_reload_ledger59c1={};g_reload_actions59c10r3={};
    g_reload_events36=0;g_reload_budget36=0;g_reload_scope_loss36=0;g_reload_queue36.dropped=0;
    g_reload_enabled36=ready;
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.178R36C2 RELOAD_NATIVE_COVERAGE session=%llu ready=%d sites=%u/%u owner=GAME_RESOLVED_CALLS_ONLY families=SHARED_MISSILE_ACTION dispatcherOwner=UNQUALIFIED ammoCounters=UNOBSERVED projectileCleanup=UNOBSERVED handWorld=BOUNDED_NATIVE_FRAME callLedger=EXACT_STACK_JOIN actionCoverage=32_LIFETIMES toolStages=SPAWN_DESPAWN_COCKING captureMs=180000 budget=4096 writes=NONE",
        (unsigned long long)session,ready,ready?unsigned(kReloadSites36.size()):0,unsigned(kReloadSites36.size()));diagnostic_trace::Write(m);return ready;
}
static void ShutdownReloadNative36() noexcept {
    g_reload_hand_entry_t30.clear(); // readers never follow a retired observer witness
    EndReloadNative36("SHUTDOWN");g_reload_ready36=false;g_reload_callers37=false;
    for(auto& h:g_reload_hooks36)if(h){(void)h->disable();h.reset();}
    g_reload_attempted36=0;
}
