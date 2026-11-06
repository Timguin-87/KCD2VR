// V66.128 (b) natural chest twist: smooth yaw saturation (no hard 35 deg kink),
// cumulative spine distribution, neck continuity, bounded bias toward the hands.
std::atomic<bool> g_chest_natural128{true},g_chest_hand_bias128{true};
static std::atomic<float> g_chest_max_deg128{45.f},g_chest_gain128{.55f},g_chest_hand_weight128{.35f};
