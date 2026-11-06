bool __fastcall AttachmentHelperTransformDetourV6573(void* owner,const char* helperName,QuatT28* out,bool includeAdditional) {
    const bool nativeOk=g_attachment_helper_transform_original?g_attachment_helper_transform_original(owner,helperName,out,includeAdditional):false;
    const auto whBase136=(std::uintptr_t)GetModuleHandleW(L"WHGame.dll");
    const auto ret136=(std::uintptr_t)_ReturnAddress();
    const std::uintptr_t caller136=(whBase136&&ret136>=whBase136)?(ret136-whBase136):0ull;
    const auto now136=(std::uint64_t)GetTickCount64();int reloadWeapon136=0;
    if(ReloadDiagnostics175()&&
       animation_filter::ReloadDiagnosticWindow136R3(now136,reloadWeapon136)&&
       (caller136==0x1F19E2Cull||caller136==0x2A8ADB4ull)&&
       g_missile_helper_logs136r3.fetch_add(1,std::memory_order_relaxed)<120){
        char helper[96]{};bool helperOk=false;QuatT28 observed{};bool outOk=false;
        if(helperName)__try{std::size_t i=0;for(;i+1<sizeof(helper)&&helperName[i];++i)helper[i]=helperName[i];helper[i]=0;helperOk=true;}__except(EXCEPTION_EXECUTE_HANDLER){helperOk=false;}
        if(out)__try{observed=*out;outOk=true;}__except(EXCEPTION_EXECUTE_HANDLER){outOk=false;}
        char m[640]{};std::snprintf(m,sizeof(m),
            "V66.136R3-F3 MISSILE_HELPER weapon=%s callerRva=0x%llX owner=%p helper=%s nativeOk=%d out=%d t=(%.4f,%.4f,%.4f) q=(%.4f,%.4f,%.4f,%.4f) includeAdditional=%d semantic=OBSERVE_ONLY ownerIdentity=UNPROVEN weaponLabel=TEMPORAL_HINT",
            animation_filter::ReloadDiagnosticWeaponName136R3(reloadWeapon136),(unsigned long long)caller136,owner,
            helperOk?helper:"<UNREADABLE>",nativeOk?1:0,outOk?1:0,observed.t[0],observed.t[1],observed.t[2],
            observed.q[0],observed.q[1],observed.q[2],observed.q[3],includeAdditional?1:0);
        diagnostic_trace::Write(m);
    }
    if(!out||!ReadCStringEqV6573(helperName,"slt_0")) return nativeOk;

    const auto calls=g_slt0_helper_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const int mode=g_weapon_control_mode.load(std::memory_order_acquire);
    {   // V66.128 (c) ARROW ATTACH OBSERVATION, read-only, every F12 mode. The only
        // caller in this RVA range is C_MissileWeaponItem::+0x2A8AD10: it reads the
        // ARROW model's slt_0 (nock) helper and sets the bow's "arrow" attachment
        // offset to its inverse. One call per nocked arrow; capped at 40 lines.
        const auto b=(std::uintptr_t)GetModuleHandleW(L"WHGame.dll");const auto r=(std::uintptr_t)_ReturnAddress();
        const std::uintptr_t rva=(b&&r>=b)?(r-b):0ull;
        if(ReloadDiagnostics175()&&(rva==0x1F19E2Cull||rva==0x2A8ADB4ull)&&g_arrow_client_logs128.fetch_add(1,std::memory_order_relaxed)<40){
            QuatT28 o{};bool ok=false;__try{o=*out;ok=true;}__except(EXCEPTION_EXECUTE_HANDLER){ok=false;}
            const auto now=(std::uint64_t)GetTickCount64(),bt=g_bow_char_tick128.load(),ct=g_bow_charge_tick128.load();
            char m[512]{};std::snprintf(m,sizeof(m),"V66.128-ARROW ATTACH callerRva=0x%llX arrowOwner=%p nativeOk=%d read=%d nockHelper q=(%.4f,%.4f,%.4f,%.4f) t=(%.4f,%.4f,%.4f) attachmentOffset=INVERSE bowChar=%p bowCharAgeMs=%lld charge=%.3f chargeAgeMs=%lld f12=%d writes=NONE ownerIdentity=UNPROVEN",
                (unsigned long long)rva,owner,nativeOk?1:0,ok?1:0,o.q[0],o.q[1],o.q[2],o.q[3],o.t[0],o.t[1],o.t[2],g_bow_char128.load(),
                bt?(long long)(now-bt):-1ll,double(g_bow_charge128.load()),ct?(long long)(now-ct):-1ll,mode);
            diagnostic_trace::Write(m);
        }
    }
    if(mode!=1) return nativeOk;
    const auto whBase=(std::uintptr_t)GetModuleHandleW(L"WHGame.dll");
    const auto ret=(std::uintptr_t)_ReturnAddress();
    const std::uintptr_t callerRva=(whBase&&ret>=whBase)?(ret-whBase):0ull;
    const char* callerClass=(callerRva>=0x2A8AD80ull&&callerRva<=0x2A8AE20ull)?"ARROW-CLIENT":
                            ((callerRva>=0x289F880ull&&callerRva<=0x289F920ull)?"TRANSFORM-CLIENT-289F":
                            ((callerRva>=0x1F19DC0ull&&callerRva<=0x1F19E80ull)?"CLIENT-1F19":"OTHER"));

    const void* exactOwner=g_henry_char_attachment_manager.load(std::memory_order_acquire);
    const DWORD tid=GetCurrentThreadId();
    const DWORD henryTid=g_henry_skinning_thread.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto henryTick=g_henry_skinning_tick.load(std::memory_order_acquire);
    const bool ownerMatch=owner&&exactOwner&&owner==exactOwner;
    const bool sameThreadNear=henryTid!=0&&tid==henryTid&&henryTick!=0&&now>=henryTick&&(now-henryTick)<=50ull;
    const auto heldTick=g_native_side_item_tick[0].load(std::memory_order_acquire);
    const bool heldSide0=g_native_side_item[0].load(std::memory_order_acquire)!=nullptr&&heldTick!=0&&now>=heldTick&&(now-heldTick)<=5000ull;
    if(ownerMatch) g_slt0_helper_owned_calls.fetch_add(1,std::memory_order_relaxed);

    QuatT28 nativeOut{}; bool outOk=false;
    __try { nativeOut=*out; outOk=true; } __except(EXCEPTION_EXECUTE_HANDLER) { outOk=false; }
    if(!outOk) return nativeOk;

    bool writeEligible=false; // V66.85 retired: slt_0 is observation-only, never weapon authority
    const char* gate=ownerMatch?"EXACT-HENRY-ATTACHMENT-OWNER":((heldSide0&&sameThreadNear)?"HELD-SIDE0+HENRY-THREAD":"REJECT");

    QuatT28 nativeHand{},vrHand{}; const bool handDelta=ReadF12HandDeltaV6557(0,nativeHand,vrHand);

    static std::atomic<std::uint64_t> lastLog{0}; auto prev=lastLog.load(std::memory_order_relaxed);
    if(now-prev>=200ull&&lastLog.compare_exchange_strong(prev,now,std::memory_order_acq_rel)){
        QuatT28 finalOut=nativeOut; __try { finalOut=*out; } __except(EXCEPTION_EXECUTE_HANDLER) {}
        char rt[128]{}; if(owner&&Safe(owner,sizeof(void*)))Rtti(owner,rt,sizeof(rt));
        char m[896]{}; std::snprintf(m,sizeof(m),
            "V66.86-F12-RETIRED-SLT0-OBSERVE rung=%d call=%llu callerRva=0x%llX callerClass=%s owner=%p/%s henryAttach=%p ownerMatch=%d heldSide0=%d sameThread50ms=%d handDelta=%d gate=%s write=%d writes=%llu native=(%.3f,%.3f,%.3f) final=(%.3f,%.3f,%.3f)",
            mode,(unsigned long long)calls,(unsigned long long)callerRva,callerClass,owner,rt[0]?rt:"-",exactOwner,ownerMatch?1:0,heldSide0?1:0,sameThreadNear?1:0,handDelta?1:0,gate,(writeEligible&&handDelta)?1:0,
            (unsigned long long)g_slt0_helper_writes.load(std::memory_order_relaxed),
            nativeOut.t[0],nativeOut.t[1],nativeOut.t[2],finalOut.t[0],finalOut.t[1],finalOut.t[2]);
        diagnostic_trace::Write(m);
    }
    return nativeOk;
}

