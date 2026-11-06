// R59.16C8 COUNTER WINDOW (Miki, 13 Oct, final wording): "I don't want any native riposte or master strikes - I want the game to
// recognize them and let ME attack; the enemy stays clever and blocks; only the rules (impalement by a stab, decapitation by a neck
// cut) are the result when the game says it is a kill". So: nothing presses the native attack, ever. Henry's own perfect block
// (seen on the native submit: collision producer, victim = player, details[3]) opens VrRiposteWindowMs = 700 ms; a VR blade contact
// inside it is still OUR direct hit, adjudicated by the game (the enemy may still block), at the SAME power as any hit (C26: no bonus) and marked
// riposte=1 in the VR_HIT receipt. A lethal result is then the kill move: thrust -> impale latch (Codex C7G), neck cut -> decap59c7e.
namespace riposte59c8 {
inline std::atomic<bool> g_native{false};inline std::atomic<std::uint64_t> g_until{0},g_press_until{0},g_last_block{0}; // g_native: retired (never presses the native attack)
inline std::atomic<unsigned> g_window_ms{700};inline std::atomic<const void*> g_blocked_attacker{nullptr};
inline std::atomic<std::uint64_t> g_blocks{0},g_presses{0};
inline void NotePerfectBlock(std::uint64_t now,const void* attacker) noexcept {
    g_until.store(now+g_window_ms.load(),std::memory_order_relaxed);g_last_block.store(now,std::memory_order_relaxed);
    g_blocked_attacker.store(attacker,std::memory_order_relaxed);g_blocks.fetch_add(1,std::memory_order_relaxed);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return; // F3 material (quiet otherwise)
    char m[160]{};std::snprintf(m,sizeof(m),"R59.16C8 RIPOSTE_WINDOW open=%u ms (Henry perfect block #%llu) native=%d",g_window_ms.load(),(unsigned long long)g_blocks.load(),g_native.load()?1:0);diagnostic_trace::Write(m);}
inline bool Open(std::uint64_t now) noexcept {const auto u=g_until.load(std::memory_order_relaxed);return u&&now<u;}
// R59.16C9B (Codex review): the bonus belongs to the ONE attacker whose blow Henry perfect-blocked and to ONE contact: identity-bound
// and consumed on first use. A contact on another enemy, or a second contact in the same window, gets no bonus.
inline bool TakeFor(std::uint64_t now,const void* targetActor) noexcept {
    if(!Open(now)||!targetActor||g_blocked_attacker.load(std::memory_order_relaxed)!=targetActor)return false;
    g_until.store(0,std::memory_order_relaxed);return true;}
inline bool PeekFor(std::uint64_t now,const void* targetActor) noexcept { // R59.16C10R17: TakeFor without consuming
    return Open(now)&&targetActor&&g_blocked_attacker.load(std::memory_order_relaxed)==targetActor;}
// C2: a native callback may open a NEW PB window while this hit is being adjudicated.
// Consume only the previewed generation, never the replacement window.
inline std::uint64_t TokenForC2(std::uint64_t now,const void* targetActor) noexcept {
    return PeekFor(now,targetActor)?g_blocks.load(std::memory_order_relaxed):0;
}
inline bool TakeTokenC2(std::uint64_t now,const void* targetActor,std::uint64_t token) noexcept {
    return token&&g_blocks.load(std::memory_order_relaxed)==token&&TakeFor(now,targetActor);
}
inline void Consume(std::uint64_t now) noexcept {g_until.store(0,std::memory_order_relaxed);g_press_until.store(now+90,std::memory_order_relaxed);g_presses.fetch_add(1,std::memory_order_relaxed);}
}
void NotePerfectBlock59c8(std::uint64_t now,const void* attacker) noexcept {riposte59c8::NotePerfectBlock(now,attacker);} // R59.16C8
