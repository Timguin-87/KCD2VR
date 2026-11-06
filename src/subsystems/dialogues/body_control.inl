void SetDialogueBody178r9(bool active) noexcept {g_dialogue_body178r9.store(active,std::memory_order_relaxed);}
void PublishDialogueHandPresenceH24(const dialogue_state59c10r2::State& native,bool presentation) noexcept {
    // A revoked presentation wins even if a mailbox slot is temporarily busy.
    // Do not reuse the composite dialogue/workstation/held-prop visual flag.
    const auto witness=kcd2vr::physics::dialogue_hand_h24::capture(native,presentation,
        g_pose_policy_generation57.load(std::memory_order_acquire),GetTickCount64());
    if(!witness.presentation)g_dialogue_hand_presence_enabled_h24.store(false,std::memory_order_release);
    if(g_dialogue_hand_presence_h24.publish(witness))
        g_dialogue_hand_presence_enabled_h24.store(witness.presentation,std::memory_order_release);
}
