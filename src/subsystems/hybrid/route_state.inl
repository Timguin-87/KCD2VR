// LEGACY COMPARISON: a real native helper registers its collision receiver
// through D4CB30 (D4CB40 is an interior instruction, not a callable entry).
// The C3 repair matches physical contacts to that admitted action. It improves
// transport but still depends on native action state and is NOT VR-owned combat.
// The new backend must supply the semantic transaction without this dependency.
std::atomic<bool> g_live178r35{true};std::atomic<bool> g_live_fault178r35{false};std::atomic<bool> g_direct_fallback178r37{false}; // V66.178R37: direct route only on request
// V66.178R39 HOLD FOR THE STRIKE PHASE. The 12:23 log (R38): the live action WAS found (6 contacts) and C5235C rejected
// every one before the builder. Its first precondition is 72EFDC(actor,8): the attacker state's property +0x30 (the
// action phase, read through its getter) must equal 8 - the strike window of the native attack. A VR blade reaches the
// enemy 100-250 ms after the stroke commits, while the native action is still in its wind-up (phase != 8); and 120 of
// 128 contacts came before any action existed at all (the stroke commits at/after the contact). So a contact is now
// HELD: it stays queued, re-checked every player update, and is handed to the live action the moment the phase is 8,
// up to VrGeometryHoldMs (default 600). Haptics/visuals happened at the touch; the native hit lands when the game's own
// strike window opens. Log: GEOM_LIVE_WAIT (phase/flags while waiting), GEOM_SKIP reason=STRIKE_WINDOW_MISSED.
std::atomic<unsigned> g_hold_ms178r39{600};std::atomic<int> g_strike_phase178r39{8};
// R59.16C10R9 F4 rung 3 SYNCHRONIZED (Miki: "rung 3 = 1+2"). Same native admission/defense/combos/clock as rung 1. The only
// difference is here: rung 1 HOLDS a VR contact until the native strike window (phase 8) opens and drops it after
// VrGeometryHoldMs - the 'contacts expiring while waiting for an action window' of the C10R4 logs. In SYNCHRONIZED the
// VR contact has authority: when the window is not open g_sync_grace_ms after the ORIGINAL contact time (or the native
// attack never produced an action although Henry's release was admitted), the same contact is delivered ONCE through
// the direct native hit (BuildAndSubmitMeleeHit: native damage/armour/reaction), claimed with the same serial so the
// live route can never deliver it again, and Henry's canned sweep stays rejected (one producer per stroke, R40).
inline std::atomic<bool> g_sync59c10r9{false};inline std::atomic<unsigned> g_sync_grace_ms59c10r9{70};
// R59.16C10R16 F4 press 1 VR INTENT (Miki 15 Oct: "the damage and everything is scaled according to the energy"): with the release
// of THIS stroke admitted (the join below proves the native attack was announced, so the NPC already reacts to it), the blade
// contact is delivered at once through the direct native hit (energy-scaled 30..120 %, riposte bonus, the game adjudicates block/
// parry/armour), claimed once per release serial. No wait for the hidden native strike window, no 70 ms grace, no expiry.
inline std::atomic<bool> g_intent59c16{false};inline std::atomic<std::uint64_t> g_intent_hits59c16{0};
// R59.16C10R12 (19:43 log): three switches for what the log showed dying. SYNC: 11 contacts expired at 172 ms because the stroke had no
// admitted native release (withheld / an older action alive) and only an admitted release could take the overdue direct route.
// RECOVERY: 16 rung-4 attempts yielded on Henry's phase 32 (the tail of OUR previous strike) and 1 on 16. ROW: 97 of 152 rung-4
// attempts died on DEFENSE_ROW_UNAVAILABLE (12EDEF4 null) - R3B never needed the row; the victim's own row adjudicates the hit.
inline std::atomic<bool> g_sync_overdue_any59c12{true}; // RECOVERY/ROW switches live in vrstate596 below (host-extracted state driver)
inline std::atomic<std::uint64_t> g_sync_live59c10r9{0},g_sync_direct59c10r9{0};
