static std::atomic<int> g_hide_stowed_bow59c14{2};
static std::atomic<float> g_hide_bow_pitch_deg59c14{20.f},g_hmd_pitch_deg59c14{0.f};
static std::atomic<void*> g_hidden_bow_entity59c14{nullptr};
static std::atomic<std::uint64_t> g_hidden_bow_seen59c14{0},g_bow_hides59c14{0},g_bow_shows59c14{0};
// Protected by the existing attachment transaction lock, including native-tick restore.
static std::uint32_t g_hidden_bow_id59c14=0;
