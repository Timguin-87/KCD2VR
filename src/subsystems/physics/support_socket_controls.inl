void SetSupportSlot34(bool on) noexcept {g_support_slot34.store(on,std::memory_order_relaxed);
    diagnostic_trace::Write(on?"C37 SUPPORT_SLOT_FALLBACK=ON (order: captured native left frame, then measured slt_1, then synthetic sliders)":"C37 SUPPORT_SLOT_FALLBACK=OFF (order: captured native left frame, then synthetic sliders)");}
bool SupportSlot34() noexcept {return g_support_slot34.load(std::memory_order_relaxed);}
void SetSupportSocket178r9(bool synthetic,float swordCm,float poleCm) noexcept {
    g_support_synthetic178r9.store(synthetic,std::memory_order_relaxed);
    g_support_offset_sword178r9.store(std::clamp(swordCm,6.f,60.f)/100.f,std::memory_order_relaxed);
    g_support_offset_pole178r9.store(std::clamp(poleCm,6.f,60.f)/100.f,std::memory_order_relaxed);
}
// V66.178R7 (public; the atomics live in the anonymous namespace above)
