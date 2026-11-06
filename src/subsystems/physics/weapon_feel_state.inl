// R59.16C10R14 weapon weight / fatigue (Miki: "increasing weapon drag/weight", "halberd = 10-pole stick one-handed")
static std::atomic<float> g_fatigue_ramp59c14{.6f},g_pole_one_hand59c14{.40f};
static std::atomic<bool> g_strike_min_scale_gate59c14{true},g_live_attributes_all_rungs59c14{true};
