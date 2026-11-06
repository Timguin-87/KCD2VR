// Actual Mod producer. A retained native activity or a hidden HUD is not enough.
// Native Reading input remains native; no wrist/tool/F5 owner is manufactured.
            const bool readingBody32=reading_pose32::publish(GetTickCount64(),
                visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode()),
                safe_ui_fit,matrix_active,mis_menu_after_frame,dialogue_active,cutscene_flat_ui,
                diagnostics::lab::NativeReference(),ui89_policy);
            static diagnostics::ReadingReceipts32 readingReceipts32;
            readingReceipts32.Observe(GetTickCount64(),[&]{
                const auto& w32=activity_input163::witness175;using K32=activity_input163::Kind175;
                const auto frame32=frik_pose_probe::DialogueVisualSnapshot28(0,GetTickCount64());
                diagnostics::ReadingRecord32 r{};
                r.reading=w32.active[unsigned(K32::Reading)].load(std::memory_order_acquire);
                r.alchemy=w32.active[unsigned(K32::Alchemy)].load(std::memory_order_acquire);
                r.lifetime=w32.lifetime175.load(std::memory_order_acquire);
                r.flags=(readingBody32?1u:0u)|(safe_ui_fit?2u:0u)|(matrix_active?4u:0u)|
                    (mis_menu_after_frame?8u:0u)|(dialogue_active?16u:0u)|(cutscene_flat_ui?32u:0u)|
                    (diagnostics::lab::NativeReference()?64u:0u)|(ui89_policy.uncertain?128u:0u)|
                    (w32.ready.load(std::memory_order_acquire)?256u:0u)|
                    (w32.transitionDepth175.load(std::memory_order_acquire)?512u:0u)|
                    (frame32.poseValid?1024u:0u)|(frame32.nativeBody?2048u:0u)|
                    (frame32.right?4096u:0u)|(frame32.left?8192u:0u);
                r.uiOwner=unsigned(ui89_policy.owner);r.character=frame32.frameCharacter;
                r.frameTick=frame32.frameTick;r.sample=frame32.sample;return r;
            },[](const char* line){diagnostic_trace::Write(line);});
