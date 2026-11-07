// Generated verbatim actual native combo-step detour.
static void __fastcall ComboStepDetour6691(void*self,const std::int32_t*a0,const std::int32_t*a1) noexcept {ClearReadableCache178r23();kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ComboStep};kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    const auto original=g_combo_step_original6691;if(!original)return;
    // Timing proof is independent of F3/recording. All reads are bounded at
    // this actual native callback, never polled from the input/render thread.
    kcd2vr::combo_lineage19::Owner cb2Owner{};
    const auto cb2Generation=nhp134::combo59c17::BeginStepCB2(self,cb2Owner);
    const auto cb2Capture=[&](){auto reader=ComboRead6691;return diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()));};
    const auto cb2Before=cb2Generation?cb2Capture():diagnostics::combo::Snapshot{};
    const auto cb2Finish=[&](){if(cb2Generation)nhp134::combo59c17::RealStepCB2(self,cb2Owner,cb2Generation,cb2Before,cb2Capture());};
    auto q=BeginCombo6691(self);if(!q.record){{kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboStep};original(self,a0,a1);}cb2Finish();return;}
    const auto caller=KCD2VR_COMBO_CALLER(),base=g_combo_base6691.load();q.callerRva=(caller>=base&&caller-base<0x6000000ull)?caller-base:0;
    std::int32_t v0=0,v1=0;
    const bool ok0=q.record&&ReadCopy6689(a0,&v0,4),ok1=q.record&&ReadCopy6689(a1,&v1,4);
    // No mutex is held through the original native call. Nested callbacks pass through.
    ++g_combo_depth6691;
    const auto e=diagnostics::combo::Around(q.record,[&](){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ComboStep};original(self,a0,a1);},[&](){if(!ComboLease176(q))return diagnostics::combo::Snapshot{};auto reader=ComboRead6691;return diagnostics::combo::Capture(reader,diagnostics::combo::Layout{g_combo_base6691.load()},reinterpret_cast<std::uintptr_t>(self),reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()));});
    --g_combo_depth6691;cb2Finish();EmitComboEpisode6691(q,"STEP_FILTER",e,ok0,v0,ok1,v1);
}
