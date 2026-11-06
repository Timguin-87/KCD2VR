// R59.16C10R17: last native answer to Henry's attack (1 block, 2 parry-in-place, 3 perfect block) - see SubmitHitDetour.
inline std::atomic<std::uint64_t> g_answered_skips59c17{0},g_answered_seen59c17{0};
inline kcd2vr::answer59c4r2::Ledger g_answers19c4r2;
void NoteHenryAttackAnswered59c17(const kcd2vr::answer59c4r2::Record& answer) noexcept {
    g_answers19c4r2.note(answer);g_answered_seen59c17.fetch_add(1,std::memory_order_relaxed);}
// R59.16C10R17 F4 press 1 (VR INTENT): an admitted stroke whose contact arrives while the hidden attack is still in phase 4 waits
// this long for the contact catch-up (contactPlan59c3 -> phase 8, hit sync next frame) so the hit goes through the LIVE native chain
// (C5235C: NPC block / parry / weapon clash decided natively, combo hit flag, energy curve ~1). Then the direct route, as in R16.
inline std::atomic<unsigned> g_intent_live_ms59c17{60};inline std::atomic<bool> g_intent_live59c17{true};
inline std::atomic<std::uint64_t> g_intent_live_hits59c17{0},g_intent_direct_hits59c17{0},g_strength_leases59c17{0},g_strength_consumed59c17{0};
