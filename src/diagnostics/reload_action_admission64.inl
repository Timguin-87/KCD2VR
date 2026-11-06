// Exact native authored-action allocation boundary; immediate cock/bind and
// no-action pending paths in18468BC never enter this function.
static bool ManualActionAllocation64(void*,const char**) noexcept;
static void ReloadActionAdmission64(ReloadContext36& c) noexcept {
    if(!g_reload_ready36.load()||!g_reload_callers37.load()||!StagedBowEnabled59())return;
    std::uintptr_t caller=0;
    if(!Read(reinterpret_cast<void*>(DetourModKit::hook::stack_pointer(c)),caller)||caller!=WhBase()+0x1846A54)return;
    const auto action=ReloadRegValue36(c,ReloadReg36::Rcx);const char* reason="NOT_CURRENT";
    const bool owned=ManualActionAllocation64(reinterpret_cast<void*>(action),&reason);
    if(owned)DetourModKit::hook::instruction_pointer(c)=reinterpret_cast<std::uintptr_t>(&DeferredReload596);
    if(!ReloadActive36())return;
    static thread_local std::uint64_t generation=0,lastAction=0;
    static thread_local unsigned receipts=0;
    const auto current=g_reload_generation36.load();
    if(generation!=current){generation=current;receipts=0;lastAction=0;}
    if(receipts>=16||(!owned&&lastAction==action))return;
    ++receipts;lastAction=action;char line[320]{};
    std::snprintf(line,sizeof(line),"C63R2 F11_STAGE ACTION_ADMISSION action=%p owned=%d reason=%s caller=1846A54 allocation=%s nativeImmediate=UNCHANGED receipt=%u/16",
        reinterpret_cast<void*>(action),owned,reason,owned?"DEFERRED_TO_MANUAL_OWNER":"NATIVE",receipts);
    diagnostic_trace::Write(line);
}
