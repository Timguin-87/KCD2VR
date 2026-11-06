
void __fastcall SetIkDetour(void* self,void* pose){
    const auto diagnosticMaskD47=diagnostics::capture168::gate.mask.load(std::memory_order_relaxed);
    const bool diagnosticSelectedD47=(diagnosticMaskD47&((1u<<unsigned(diagnostics::capture168::Domain::Vrik))|
        (1u<<unsigned(diagnostics::capture168::Domain::Reload))))!=0;
    const auto diagnosticNowD47=diagnosticSelectedD47?std::uint64_t(GetTickCount64()):0;
    const bool nativeDiagnosticD47=diagnosticSelectedD47&&
        (diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,diagnosticNowD47)||
         diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,diagnosticNowD47));
    char rt[96]{};Rtti(pose,rt,sizeof(rt));
    const bool playerPose=pose&&std::strstr(rt,"CPoseData");
    if(playerPose&&nativeDiagnosticD47){
        g_setik_calls.fetch_add(1,std::memory_order_relaxed);g_last_setik_tick.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_relaxed);
        // C37: V64.53 PLAYER-CHAR-LEARNED logged EVERY CPoseData SetIK caller here (NPCs included) before any
        // ownership check: 13,442 lines in the C34 run, 11 characters, 98% NPC. Generic counters above/below stay
        // any-character diagnostics; ownership is decided only by the C37 ledger after ResolveExactPlayerSolverOwner.
        std::uint16_t jc=0xFFFF,je=0xFFFF;
        if(self&&Read(static_cast<unsigned char*>(self)+0x0C,jc)&&jc!=0xFFFFu)g_solver_joint_c.store(static_cast<int>(jc),std::memory_order_relaxed);
        if(self&&Read(static_cast<unsigned char*>(self)+0x0E,je)&&je!=0xFFFFu)g_solver_joint_e.store(static_cast<int>(je),std::memory_order_relaxed);
    }

    const int writeMode=g_mode.load(std::memory_order_acquire);
    const int census=g_census_mode.load(std::memory_order_acquire);
    void* exactPlayerModule=nullptr; void* exactPlayerEntity=nullptr;
    const bool f11Focused=g_f11_theory_phase.load(std::memory_order_acquire)>0;
    const bool needPlayerIdentity=(census>0)||(writeMode>0)||f11Focused||g_vrik_calibration_requested.load(std::memory_order_acquire)||g_player_identity_observe.load(std::memory_order_acquire);
    bool exactPlayer=false;
    if(playerPose&&self&&needPlayerIdentity){
        const bool exactOwner=ResolveExactPlayerSolverOwner(self,exactPlayerModule,exactPlayerEntity);
        void* poseChar=nullptr;void* poseAnim=nullptr;
        const bool poseCharOk=exactOwner&&ReadValidatedCharInstanceFromPlayerPose(pose,poseChar,poseAnim);
        // Candidates are keyed by raw pose-0x8E0 (no dereference); only the exact owner is validated.
        void* offered=poseCharOk?poseChar:reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(pose)-static_cast<std::uintptr_t>(kPoseInCharInstanceOffset));
        const auto v=ObservePlayerIdentity37(pid37::Source::Solver,exactOwner&&poseCharOk,exactOwner?exactPlayerEntity:nullptr,offered,pose,static_cast<std::uint64_t>(GetTickCount64()));
        exactPlayer=v==pid37::Verdict::Refreshed||v==pid37::Verdict::Qualified;
    }
    if(exactPlayer && (census>0 || f11Focused || g_weapon_control_mode.load(std::memory_order_acquire)>0)) TraceWeaponAttachmentOwnerV6528(exactPlayerModule,exactPlayerEntity);
    // V66.69: bounded F12 entity proof is native-callback-owned. Never call entity setters/getters from Mod::Run worker timing.
    if(exactPlayer && g_weapon_control_mode.load(std::memory_order_acquire)>0) WeaponControlTickImpl();
    // V65.45: native-IK runtime trace is not tied to the cumulative weapon ladder.
    if(nativeDiagnosticD47&&playerPose&&self&&writeMode>0) {
        MaybeTraceDiscoveryObject("WeaponSolver",self,writeMode,0);
        if(writeMode==1||writeMode==3||writeMode==5) MaybeTraceDiscoveryObject("CPoseData",pose,writeMode,1);
    }
    // V66.89 SetIK is correlation-only. Retired pre/post native pose writers are archived.
    // V65.09 F10 diagnostics are deliberately narrow: no repeated object-tree
    // scans. We log only the exact C_Player owner gate and timing/write state.
    if(nativeDiagnosticD47&&playerPose&&self&&census==1){
        static std::atomic<std::uint64_t> lastF10Order{0};
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastF10Order.load(std::memory_order_relaxed);
        if(now-prev>=1500ull && lastF10Order.compare_exchange_strong(prev,now,std::memory_order_acq_rel)){
            TrackingSnapshot ts{}; const bool tr=ReadTrackingSnapshot(ts);
            char m[768]{};
            std::snprintf(m,sizeof(m),
                "V65.09-F10-ORDER mode=%d(%s) exactPlayer=%d module=%p entity=%p pose=%p preThisCall=%d pending=%d "
                "lateWrites=%llu setIkWrites=%llu holds=%llu rejects=%llu xrR=%d(%.3f,%.3f,%.3f)",
                census,CombatCensusModeName(census),exactPlayer?1:0,exactPlayerModule,exactPlayerEntity,pose,0,0,
                (unsigned long long)g_prebuild_writes.load(),(unsigned long long)g_setik_upstream_writes.load(),
                (unsigned long long)g_setik_hold_writes.load(),(unsigned long long)g_rejects.load(),
                (tr&&ts.right_valid)?1:0,ts.right.x,ts.right.y,ts.right.z);
            diagnostic_trace::Write(m);
        }
    }

    QuatT28 diagPreC{},diagPreE{}; bool diagPreCOk=false,diagPreEOk=false;
    std::uint32_t diagGate414=0,diagDual410=0;
    const bool ownershipDiag=nativeDiagnosticD47 && playerPose && self && writeMode>=2;
    PoseGetFn diagB0=nullptr,diagB1=nullptr;
    if(ownershipDiag) {
        (void)Read(static_cast<unsigned char*>(self)+0x414,diagGate414);
        (void)Read(static_cast<unsigned char*>(self)+0x410,diagDual410);
        if(ValidateRawPoseApi(pose,diagB0,diagB1)) {
            const int c=g_solver_joint_c.load(std::memory_order_relaxed),e=g_solver_joint_e.load(std::memory_order_relaxed);
            if(c>=0) diagPreCOk=ReadQuat(diagB1,pose,c,diagPreC);
            if(e>=0) diagPreEOk=ReadQuat(diagB1,pose,e,diagPreE);
        }
    }

    if(g_setik_original)g_setik_original(self,pose);

    if(ownershipDiag) {
        if(writeMode>=4) {
            static std::atomic<std::uint64_t> lastOwnerSetIk{0}; const auto onow=static_cast<std::uint64_t>(GetTickCount64()); auto oprev=lastOwnerSetIk.load(std::memory_order_relaxed);
            if(onow-oprev>=1200ull && lastOwnerSetIk.compare_exchange_strong(oprev,onow,std::memory_order_acq_rel)) TracePlayerOwnershipChain(self,writeMode);
        }
        static std::atomic<std::uint64_t> lastOwnSetIk{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastOwnSetIk.load(std::memory_order_relaxed);
        if(now-prev>=1500ull && lastOwnSetIk.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) {
            QuatT28 postC{},postE{}; bool postCOk=false,postEOk=false;
            const int c=g_solver_joint_c.load(std::memory_order_relaxed),e=g_solver_joint_e.load(std::memory_order_relaxed);
            if(diagB1&&c>=0)postCOk=ReadQuat(diagB1,pose,c,postC); if(diagB1&&e>=0)postEOk=ReadQuat(diagB1,pose,e,postE);
            TrackingSnapshot ts{}; const bool tr=ReadTrackingSnapshot(ts);
            char line[1280]{}; std::snprintf(line,sizeof(line),
                "V65.02-SETIK level=%d solver=%p pose=%p gate414=%u dual410=%u jointC/E=%d/%d "
                "preC=%d(%.4f,%.4f,%.4f) postC=%d(%.4f,%.4f,%.4f) preE=%d(%.4f,%.4f,%.4f) postE=%d(%.4f,%.4f,%.4f) "
                "xrR=%d(%.4f,%.4f,%.4f) calls=%llu READ-ONLY",
                writeMode,self,pose,diagGate414,diagDual410,c,e,
                diagPreCOk?1:0,diagPreC.t[0],diagPreC.t[1],diagPreC.t[2],postCOk?1:0,postC.t[0],postC.t[1],postC.t[2],
                diagPreEOk?1:0,diagPreE.t[0],diagPreE.t[1],diagPreE.t[2],postEOk?1:0,postE.t[0],postE.t[1],postE.t[2],
                (tr&&ts.right_valid)?1:0,ts.right.x,ts.right.y,ts.right.z,(unsigned long long)g_setik_calls.load(std::memory_order_relaxed));
            diagnostic_trace::Write(line);
        }
    }


}

