// R20C1: prefer the qualified native offhand wrist relation. These distances
// select a bounded synthetic hilt fallback only while native capture is absent;
// they are not an engine-provided second grip slot. Bow capture is unchanged.
static std::atomic<float> g_support_offset_sword178r9{.10f},g_support_offset_pole178r9{.32f};
