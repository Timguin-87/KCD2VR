struct GameStateContext {
    bool dialogue=false;
    bool cutscene=false;
    bool menu=false;
    // Native activity controls block gameplay, but are not a real UI menu.
    bool workstation_controls_only=false;
    bool tool_pose_ui_blocked=false;
    bool native_reading=false; // Exact native Reading lifecycle, never HUD visibility.
    bool hud_hidden=false; // Diagnostic presentation state only.
    bool agency_known=false;
    int agency_count=-1;
    bool scripted=false;
};
void ProduceContext57(GameStateContext& gs,bool hud_hidden57){
            gs.native_reading=activity_input163::witness175.ready.load(std::memory_order_acquire)&&
                activity_input163::witness175.active[unsigned(activity_input163::Kind175::Reading)].load(std::memory_order_acquire)!=0;
            gs.hud_hidden=hud_hidden57; // Presentation only, never a native Reading owner.
}
void PublishGameStateContext(const GameStateContext& state) noexcept {
    // C57R1: detect UI/context ABA during a solve without a frame counter or scan.
    // Hidden HUD is intentionally absent: it is not an ownership transition.
    const std::uint64_t policy57=(state.dialogue?1ull:0ull)|(state.cutscene?2ull:0ull)|
        (state.menu?4ull:0ull)|(state.native_reading?8ull:0ull)|
        (state.tool_pose_ui_blocked?16ull:0ull)|(state.workstation_controls_only?32ull:0ull)|
        (state.agency_known?64ull:0ull)|((state.agency_known&&state.scripted)?128ull:0ull);
    const bool policyChanged57=g_pose_policy_flags57.load(std::memory_order_acquire)!=policy57;
    if(policyChanged57){
        g_pose_policy_generation57.fetch_add(1,std::memory_order_acq_rel); // odd: in progress
        // Atomic cancellation also catches menu/cutscene OFF/ON between physics
        // callbacks; the old squeeze cannot be re-labelled as a new request.
        kcd2vr::physics::grab_producer60::Cancel(0);kcd2vr::physics::grab_producer60::Cancel(1);
    }
// Deny first, in one atomic word. Never combine an old activity allow bit with
// a newly published real-menu/dialogue/cutscene state. No gameplay authority changes.
    const std::uint64_t toolFlags19c4r2=(state.menu?1ull:0ull)|(state.dialogue?2ull:0ull)|
        (state.cutscene?4ull:0ull)|(state.workstation_controls_only?8ull:0ull)|(state.tool_pose_ui_blocked?16ull:0ull);
    g_tool_context19c4r2.store((std::uint64_t(GetTickCount64())<<8)|toolFlags19c4r2,std::memory_order_release);

    g_ctx_dialogue.store(state.dialogue,std::memory_order_release);
    g_ctx_cutscene.store(state.cutscene,std::memory_order_release);
    g_ctx_menu.store(state.menu,std::memory_order_release);
    g_ctx_reading.store(state.native_reading,std::memory_order_release);
    g_ctx_agency_known.store(state.agency_known,std::memory_order_release);
    g_ctx_agency_count.store(state.agency_known?state.agency_count:-1,std::memory_order_release);
    g_ctx_scripted.store(state.agency_known&&state.scripted,std::memory_order_release);
    if(policyChanged57){g_pose_policy_flags57.store(policy57,std::memory_order_release);
        g_pose_policy_generation57.fetch_add(1,std::memory_order_release);} // even: published

    // Transition-only correlation while F11 is active. The agency field explicitly
    // reports unresolved instead of fabricating a native ActionMapManager ABI.
    if(g_f11_theory_phase.load(std::memory_order_acquire)>0){
        const std::uint64_t packed=(state.dialogue?1ull:0ull)|(state.cutscene?2ull:0ull)|
            (state.menu?4ull:0ull)|(state.native_reading?8ull:0ull)|
            (state.agency_known?16ull:0ull)|((state.agency_known&&state.scripted)?32ull:0ull)|
            (static_cast<std::uint64_t>(state.agency_known?std::clamp(state.agency_count,0,255):255)<<8);
        auto prev=g_ctx_packed_last_logged.load(std::memory_order_relaxed);
        if(prev!=packed && g_ctx_packed_last_logged.compare_exchange_strong(prev,packed,std::memory_order_acq_rel)){
            char m[384]{};
            std::snprintf(m,sizeof(m),
                "V64.19C-F11-STATE dialogue=%d cutscene=%d menu=%d nativeReading=%d agencyKnown=%d agencyCount=%d scripted=%d source=native-proven agencyABI=%s",
                state.dialogue?1:0,state.cutscene?1:0,state.menu?1:0,state.native_reading?1:0,
                state.agency_known?1:0,state.agency_known?state.agency_count:-1,(state.agency_known&&state.scripted)?1:0,
                state.agency_known?"PROVEN-CCRYACTION":"UNAVAILABLE-FAIL-CLOSED");
            diagnostic_trace::Write(m);
        }
    }
}
