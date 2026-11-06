// Deny first, in one atomic word. Never combine an old activity allow bit with
// a newly published real-menu/dialogue/cutscene state. No gameplay authority changes.
    const std::uint64_t toolFlags19c4r2=(state.menu?1ull:0ull)|(state.dialogue?2ull:0ull)|
        (state.cutscene?4ull:0ull)|(state.workstation_controls_only?8ull:0ull)|(state.tool_pose_ui_blocked?16ull:0ull);
    g_tool_context19c4r2.store((std::uint64_t(GetTickCount64())<<8)|toolFlags19c4r2,std::memory_order_release);
