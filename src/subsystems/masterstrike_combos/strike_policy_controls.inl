void SetMasterStrikeAllWeapons59(bool on) noexcept {g_master_all_weapons59.store(on,std::memory_order_relaxed);} // V66.178R59
void SetMasterStrikeVeto59c14(bool veto) noexcept {g_master_veto59c14.store(veto,std::memory_order_relaxed);
    diagnostic_trace::Write(veto?"R59.16C10R14 MASTER_STRIKE veto=ON (only an opposite-side thrust within 150 ms may be a master strike)":"R59.16C10R14 MASTER_STRIKE veto=OFF (the game's own factory decides for every VR stroke)");}
