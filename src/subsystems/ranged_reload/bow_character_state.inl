// V66.128 (c)/(d): exact identity of Henry's HELD bow character, captured only on
// the physical placement route from the equipment-qualified bow entity. The bow
// skeleton carries the native "arrow" attachment (WHGame+0x2A8AD10,
// C_MissileWeaponItem vtable slot 22) and its string is posed by bow_pull_back /
// LayerManualUpdate("ChargeFraction") from wh_bow_database.adb.
std::atomic<void*> g_bow_char128{nullptr};
struct BowCharacterIdentity166 {void* character=nullptr;void* entity=nullptr;std::uint64_t tick=0;};
static kcd2vr::core::SnapshotMailbox<BowCharacterIdentity166> g_bow_identity166;
std::atomic<std::uint64_t> g_bow_char_tick128{0};
std::atomic<float> g_bow_charge128{0.f};
std::atomic<std::uint64_t> g_bow_charge_tick128{0};
std::atomic<bool> g_bow_string_follow128{true};
std::atomic<bool> g_bow_fault128{false};
std::atomic<float> g_bow_finger_bias128{.07f};
std::atomic<bool> g_bow_string_glue178r9{true}; // V66.178R9 INI VrBowStringGlue
