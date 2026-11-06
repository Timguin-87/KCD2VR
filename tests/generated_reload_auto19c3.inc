static bool ManualEquipment596(void* player,PhysFrame6698& f) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!StagedBowEnabled59()||!g_reload_ready36.load()||g_manual_bow_fault596.load()||!ManualPlayer596(player)||!ready||
       !g_phys_frame6698.read(f)||f.player!=player||f.input.sample!=ready||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       !f.input.tick||now<f.input.tick||now-f.input.tick>80||!f.input.allowed||!f.profiles[1].bow())return false;
    std::uint64_t live=0;
    return NativeHandKey6697(f.manager,1,live)&&live==f.profiles[1].key;
}

static bool ManualAutomaticReload19c3(void* equipment,const char** reason) noexcept {
    // Reasons describe only checks already required for admission. No diagnostic
    // native read, second admission attempt, or source-time refresh is added.
    auto refused=[&](const char* why) noexcept {if(reason)*reason=why;return false;};
    if(!equipment)return refused("NULL_EQUIPMENT");
    if(!StagedBowEnabled59())return refused("MANUAL_MODE_OFF");
    if(!g_reload_ready36.load())return refused("NOT_READY");
    if(g_manual_bow_fault596.load())return refused("MANUAL_FAULT");
    PhysFrame6698 seed{},f{},after{};ManualItems596 items{};
    if(!g_phys_frame6698.read(seed)||!seed.player)return refused("FIRST_FRAME");
    if(!ManualEquipment596(seed.player,f))return refused("BEFORE_NOT_CURRENT");
    if(!ManualItemsRead596(seed.player,items))return refused("ITEMS_READ");
    if(items.equipment!=equipment)return refused("EQUIPMENT_OWNER");
    if(!items.component)return refused("COMPONENT_MISSING");
    if(items.weaponKey!=f.profiles[1].key)return refused("WEAPON_KEY");
    // Native item/component access can run helpers. Revalidate the same ready
    // publication and live hand owner afterwards; never turn an old frame into
    // a current exclusion, or suppress another equipped weapon's reload.
    // C32: a newer fresh publication of the SAME owner is not lost ownership.
    // Both snapshots still pass their own80ms/current-ready checks; never restamp.
    if(!ManualEquipment596(seed.player,after))return refused("AFTER_NOT_CURRENT");
    if(after.player!=f.player||after.manager!=f.manager||after.request!=f.request||
       after.input.bodyEpoch!=f.input.bodyEpoch||after.profiles[1].key!=f.profiles[1].key||
       after.profiles[1].generation!=f.profiles[1].generation)return refused("OWNER_CHANGED");
    if(after.input.sample<f.input.sample||after.input.tick<f.input.tick||after.input.tick-f.input.tick>80)
        return refused("PUBLICATION_COHERENCE");
    if(reason)*reason="ACCEPT";return true;
}
// The allocator's receiver must be THIS current equipment's shooting action,
// not a nearby cached C_Item, reload action or another actor. Existing fresh
// manual-owner qualification is repeated after all potentially reentrant reads.
static bool ManualActionAllocation64(void* action,const char** reason) noexcept {
    const auto refuse=[&](const char* why){if(reason)*reason=why;return false;};
    if(!action||!StagedBowEnabled59())return refuse("MANUAL_OWNER_OFF");
    PhysFrame6698 seed{},before{},after{};ManualItems596 items{};void* current=nullptr;
    if(!g_phys_frame6698.read(seed)||!seed.player||!ManualEquipment596(seed.player,before)||
       !ManualItemsRead596(seed.player,items)||!items.equipment||items.weaponKey!=before.profiles[1].key)
        return refuse("CURRENT_EQUIPMENT_UNAVAILABLE");
    if(!Read(static_cast<unsigned char*>(items.equipment)+0x50,current)||current!=action)return refuse("OTHER_SHOOTING_ACTION");
    if(!ManualAutomaticReload19c3(items.equipment,reason)||!ManualEquipment596(seed.player,after)||
       after.player!=before.player||after.manager!=before.manager||after.request!=before.request||
       after.input.bodyEpoch!=before.input.bodyEpoch||after.profiles[1].key!=before.profiles[1].key||
       after.profiles[1].generation!=before.profiles[1].generation||
       after.input.sample<before.input.sample||after.input.tick<before.input.tick||after.input.tick-before.input.tick>80)
        return refuse("ACTION_OWNER_CHANGED");
    if(!Read(static_cast<unsigned char*>(items.equipment)+0x50,current)||current!=action)return refuse("SHOOTING_ACTION_CHANGED");
    if(reason)*reason="CURRENT_MANUAL_BOW_OWNER";return true;
}

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
