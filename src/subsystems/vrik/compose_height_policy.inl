// V66.137 performance-only A/B switch. Final committed skeleton must remain
// fully composed regardless of this value.
std::atomic<bool> g_compose_defer137{true};
static std::atomic<float> g_follow_lean128{.15f},g_follow_max128{.60f},g_follow_crouch128{.90f},g_step_trigger128{.16f};
// C54: one atomic mode/generation token also owns HMD-height policy. The high
// bit requests legacy native Z; default is HMD-owned. Setters update this token
// atomically, so a completed pose cannot mix a new preference with an old mode.
static constexpr std::uint64_t kNativeBodyHeightBit54=std::uint64_t(1)<<63;
// R98: second policy bit = COMBINED (HMD height, native height while crouched
// or riding). Only meaningful while bit 63 is clear. Same atomic token.
static constexpr std::uint64_t kCombinedBodyHeightBitR98=std::uint64_t(1)<<62;
