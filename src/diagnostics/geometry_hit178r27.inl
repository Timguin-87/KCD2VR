// V66.178R27 GEOMETRY HITS (hybrid stage 2). Included inside nhp134 (native_hit_probe134.inl).
// R59.10C1: the opt-in VR backend uses direct native RPG adjudication plus
// verified game-rule notification. Historical notes below describe earlier
// comparisons; they are not a claim of complete runtime equivalence. See
// review/V66.178R59.10C1/HANDOFF.md for current scope and unresolved contracts.
//
// Native services reference: D568B0 evaluates/applies an initialized melee RPG
// request; D49AB4 selects a recipient reaction; 90C850 dispatches it; D542E0
// sends hit notifications. None alone constitutes an ordinary VR attack driver.
// Historical R27..R39 direct prototypes omitted parts of this transaction.
// Their failure does NOT prove that a normal Henry action is indispensable.
// C5235C remains the comparison transaction, not the replacement's scheduler.
// Current code below retains the comparison implementation for regression work;
// see review/V66.178R59C4/NATIVE_SERVICES.md for separately verified seams.
namespace geom27 {
namespace handoff59c3=kcd2vr::contact59c3;
constexpr std::uintptr_t kHitInfoCtor=0xB2454Cull,kDetailsCtor=0xD508C0ull;
constexpr unsigned char kHitInfoCtorEntry[]{0x48,0x83,0xEC,0x18,0x45,0x33,0xC0,0xC6,0x41,0x24,0x02};
constexpr unsigned char kDetailsCtorEntry[]{0x33,0xC0,0x48,0xC7,0x41,0x64,0xFF,0xFF,0xFF,0xFF};
std::atomic<bool> g_enabled{false};
std::atomic<bool> g_wanted96{false}; // R96: the user's VR-hits setting; a fault clears g_enabled until the next save/level load, not until restart
std::atomic<int> g_type_cut{1},g_type_thrust{1}; // E_CombatAttackType seen for Henry's ordinary hits: 1 (15/17 logged hits)
std::atomic<unsigned> g_min_interval_ms{250};
std::atomic<bool> g_apply178r30{true}; // V66.178R30: BuildAndSubmitMeleeHit apply flag (see header)
// Historical direct-result continuation, DISABLED by the current backend.
// D49AB4 uses the target reaction manager (six arguments), not a fake action.
// Creation is distinct from dispatch. D542E0 notifies actor listeners and an
// interface returned by entity+668 -> virtual+1C8; it is not entity+668 -> +148.
// R59.10C1 qualifies the game-rule notification below; provider-owned action
// completion still cannot be called without an ordinary action.
std::atomic<bool> g_outer178r33{true};std::atomic<bool> g_outer_fault178r33{false};
#include "../subsystems/hybrid/route_state.inl" // M2_OWNED_SOURCE
// R46C1: contact-time phase spoofing is retired. Only a real native strike
// window may reach C5235C; no setter or initialization contract is proven for
// adapting an in-flight action to an earlier physical contact.
// R47C2: D4D34C takes the 0x50-byte attack helper, NOT the 0x20-byte
// C5235C callback receiver. No direct entry/phase bypass is retained. The native
// coordinator must advance both its deadlines and the +420 energy curve first.
// R50C2: retired the standalone D4D34C call. The admitted action clock
// advances upstream; its native helper registers the receiver and enters strike.
#include "../subsystems/hybrid/sweep_owner.inl" // M2_OWNED_SOURCE
#include "native_hit_result50c1.inl"
constexpr std::uintptr_t kLiveHandler=0xC5235Cull;
constexpr unsigned char kLiveHandlerEntry[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0xB0};
// The live ordinary-attack action of this actor, or nullptr: the {context, C5235C} pair in the sweep component's hit list.
const void* LiveAttackAction178r35(const void* actor) noexcept {
    const void* comp=Rd<const void*>(actor,0x218,nullptr);if(!comp)return nullptr;
    // V66.178R37 (ChatGPT R36 review F01): comp+0x28 is a count-prefixed array (CryEngine DynArray): the data pointer,
    // int32 count at data-4 (capacity at data-8), 16-byte {context, fn} entries - exactly how D4F3B8 walks it
    // (movsxd rbx,[rcx-4]; shl 4; add). R36 read it as a begin/end pointer pair and found nothing.
    const auto begin=Rd<std::uintptr_t>(comp,0x28,0);if(!begin)return nullptr;
    const auto count=Rd<std::int32_t>(reinterpret_cast<const void*>(begin-4),0,-1);if(count<=0||count>32)return nullptr;
    const auto end=begin+std::uintptr_t(count)*16;
    for(std::uintptr_t e=begin;e<end;e+=16){
        if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(e),8,0)!=g_base+kLiveHandler)continue;
        const void* action=Rd<const void*>(reinterpret_cast<const void*>(e),0,nullptr);
        if(!action||Rd<std::uintptr_t>(action,0,0)!=g_base+0x3A60FF0||Rd<const void*>(action,8,nullptr)!=actor)continue;
        const auto iface=Rd<std::uintptr_t>(action,0x10,0);if(iface<0x10)continue;
        const void* helper=reinterpret_cast<const void*>(iface-0x10);
        if(Rd<std::uintptr_t>(helper,0,0)!=g_base+0x3A61040||
           Rd<std::uintptr_t>(reinterpret_cast<const void*>(iface),0,0)!=g_base+0x3A61028||
           Rd<const void*>(helper,8,nullptr)!=actor||Rd<const void*>(helper,0x18,nullptr)!=action)continue;
        const auto provider=Rd<std::uintptr_t>(helper,0x20,0);if(provider<0x88)continue;
        const void* main=reinterpret_cast<const void*>(provider-0x88);
        if(Rd<std::uintptr_t>(main,0,0)!=g_base+0x3A61C58||
           Rd<std::uintptr_t>(reinterpret_cast<const void*>(provider),0,0)!=g_base+0x3A61440||
           Rd<const void*>(main,0x70,nullptr)!=actor||Rd<const void*>(main,0xA0,nullptr)!=helper||
           Rd<std::uint8_t>(action,0x18,0)!=1)continue;
        return action;}
    return nullptr;}
// V66.178R48: the attack HELPER that owns this receiver (the object D4D34C/D4D130 take), or nullptr unless every link agrees.
// C38 ATTACK_STEP (F3 combat recorder only, at the same Henry phase edges as NATIVE_CLOCK, read-only): how far Henry's
// ENTITY moved from the attack request (phase 2/4) to each later phase edge, along his forward axis at the request. Tells the
// next log whether "Henry steps forward" is entity motion (alignment / clip root motion) or only the body pose leaning.
// Entity = C_CombatPlayer +1F8 -> C_Player +38 (CEntity vtable 4072C78), cached world Matrix34 at +58 (getter 1A74140).
static void TraceAttackStep38(const void* actor,int phase) noexcept {
    static bool armed=false;static float sx=0,sy=0,sz=0,fx=0,fy=0;static std::uint64_t t0=0;static unsigned lines=0;static std::uint64_t seen=0;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    if(session!=seen){seen=session;armed=false;lines=0;}
    const void* player=Rd<const void*>(actor,0x1F8,nullptr);const void* entity=player?Rd<const void*>(player,0x38,nullptr):nullptr;
    if(!entity||Rd<std::uintptr_t>(entity,0,0)!=g_base+0x4072C78){armed=false;return;}
    const float x=Rd<float>(entity,0x58+12,NAN),y=Rd<float>(entity,0x58+28,NAN),z=Rd<float>(entity,0x58+44,NAN);
    const float ax=Rd<float>(entity,0x58+4,NAN),ay=Rd<float>(entity,0x58+20,NAN);
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)){armed=false;return;}
    const auto now=std::uint64_t(GetTickCount64());
    const bool attack=phase==2||phase==4||phase==8||phase==16||phase==32;
    if(!armed){
        if(phase!=2&&phase!=4)return;
        const float n=std::sqrt(ax*ax+ay*ay);if(!std::isfinite(n)||n<.1f)return;
        armed=true;sx=x;sy=y;sz=z;fx=ax/n;fy=ay/n;t0=now;return;
    }
    const float dx=x-sx,dy=y-sy;
    if(lines<96){++lines;char m[260]{};std::snprintf(m,sizeof(m),
        "C38 ATTACK_STEP phase=%d ms=%llu moveCm=%.1f forwardCm=%.1f sideCm=%.1f dzCm=%.1f end=%d writes=NONE",
        phase,(unsigned long long)(now-t0),double(100.f*std::sqrt(dx*dx+dy*dy)),double(100.f*(dx*fx+dy*fy)),
        double(100.f*(dx*fy-dy*fx)),double(100.f*(z-sz)),attack?0:1);diagnostic_trace::Write(m);}
    if(!attack||now-t0>4000)armed=false;
}
void TraceClock47C2(const void* actor,const void* state) noexcept {
    if(!Active())return;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    static std::uint64_t seenSession=0;static const void* lastActor=nullptr;static int lastPhase=-999;static unsigned count=0;
    if(session!=seenSession){seenSession=session;lastPhase=-999;count=0;lastActor=nullptr;}
    const int phase=I(state,kActionPhaseProp39);
    if((actor==lastActor&&phase==lastPhase)||count>=128)return;
    lastActor=actor;lastPhase=phase;++count;
    const void* clock=Rd<const void*>(actor,0x3C0,nullptr);
    if(!clock||Rd<const void*>(clock,8,nullptr)!=actor)return;
    // These values are outputs/inputs of 9E3B80, never a clock correction.
    char line[640]{};std::snprintf(line,sizeof(line),
        "V66.178R47C2 NATIVE_CLOCK phase=%d actor=%p coordinator=%p attackMode=%u rate=%.4f scaledElapsed=%.4f elapsed=%.4f prepareRemaining=%.4f hitRemaining=%.4f totalRemaining=%.4f strength=%.4f receiver=%p writes=NONE",
        phase,actor,clock,unsigned(Rd<std::uint8_t>(clock,0x14,255)),double(Rd<float>(clock,0x10,-1.f)),
        double(Rd<float>(clock,0x50,-1.f)),double(Rd<float>(clock,0x54,-1.f)),double(F(state,0x3F0)),
        double(F(state,0x450)),double(F(state,0x420)),double(F(state,0x1E0)),LiveAttackAction178r35(actor));
    diagnostic_trace::Write(line);
    TraceAttackStep38(actor,phase);
}
// Copy at the actual contact, before native adjudication can replace the
// victim's action. These are the coordinator's raw values, not an invented
// block-start timestamp (its elapsed counters belong to the last clock reset).
struct BlockClockReceipt19 {
    bool known=false;unsigned mode=255;
    float rate=-1.f,scaledElapsed=-1.f,elapsed=-1.f;
};
static BlockClockReceipt19 CopyBlockClockReceipt19(const void* actor,int phase) noexcept {
    BlockClockReceipt19 out{};
    if(!actor||(phase!=128&&phase!=256))return out;
    const void* clock=Rd<const void*>(actor,0x3C0,nullptr);
    if(!clock||Rd<const void*>(clock,8,nullptr)!=actor)return out;
    const float missing=std::numeric_limits<float>::quiet_NaN();
    const float rate=Rd<float>(clock,0x10,missing);
    const float scaled=Rd<float>(clock,0x50,missing),elapsed=Rd<float>(clock,0x54,missing);
    if(!std::isfinite(rate)||rate<=0.f||!std::isfinite(scaled)||scaled<0.f||!std::isfinite(elapsed)||elapsed<0.f)return out;
    if(Rd<const void*>(actor,0x3C0,nullptr)!=clock||Rd<const void*>(clock,8,nullptr)!=actor)return out;
    out.known=true;out.mode=Rd<std::uint8_t>(clock,0x14,255);
    out.rate=rate;out.scaledElapsed=scaled;out.elapsed=elapsed;return out;
}
static bool CallLiveHandler35(const void* action,void* details) noexcept {
    const auto callableH12=FistReceiverCallableH12();if(!callableH12)return false;
    __try{callableH12(action,details);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
constexpr std::uintptr_t kOuterReact=0xD49AB4ull,kOuterOnHit=0xD542E0ull,kOuterDown=0xD53EECull,kOuterAlive=0x9D3884ull,kOuterKind=0xC52DF4ull,kOuterStrong=0xC52E0Cull,kOuterRelease=0x4A760Cull;
// V66.178R36 (ChatGPT R34 review F01): the created reaction must be DISPATCHED to the victim's action system - C5235C does
// 90C850(victim,&reaction) at C52614 when the reaction exists, word[victim+0x384]==word[attacker+0x384] and
// (B249FC(hitInfo) || 1E44410()); R33/R34 released the reaction without it ("CREATED" but never played).
constexpr std::uintptr_t kOuterDispatch=0x90C850ull,kOuterDeathPred=0xB249FCull,kOuterGlobalPred=0x1E44410ull;
constexpr unsigned char kOuterDispatchEntry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x99,0x00,0x02,0x00,0x00};
constexpr unsigned char kOuterDeathPredEntry[]{0x32,0xC0,0x38,0x41,0x40,0x75,0x06,0x38,0x41,0x41,0x75,0x01,0xC3,0xB0,0x01,0xC3};
constexpr unsigned char kOuterReactEntry[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0x80};
constexpr unsigned char kOuterOnHitEntry[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18};
constexpr unsigned char kOuterDownEntry[]{0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x89,0x00,0x02,0x00,0x00};
constexpr unsigned char kOuterKindEntry[]{0x45,0x84,0xC0,0x74,0x0C,0x8B,0x05};
constexpr unsigned char kOuterReleaseEntry[]{0x48,0x8B,0x09,0x48,0x85,0xC9,0x75,0x01,0xC3};
struct Pending {std::uint64_t gesture59c3=0;std::uint32_t entityId=0;float pos[3]{};float energy=1.f;int kind=0;int part=-1;int surface=-1;int bodyMaterial=-1;std::uint64_t tick=0;std::uint32_t serial=0; // R59.16C7E: +bodyMaterial
    float powerRatio59c8=1.f;bool twoHanded59c8=false; // R59.16C8: strike energy / reference (unclamped) and support-hand grip at impact
    const void* action=nullptr; // V66.178R40 (direction review 4): the native action this contact was bound to on first sight; another action may not deliver it
    kcd2vr::contact47c2::Lease lease47c2{};
    kcd2vr::contact176::Ticket release51{}; // copied identity only, never a native pointer lease
    std::uint64_t recovery55c1=0;
    bool weaponItem=false; // V66.178R43: the struck entity is the opponent's WEAPON/SHIELD item (a block), not a body
    bool targetWait19c3=false; // bookkeeping only: same original contact waiting for the observed actor map
    unsigned targetWaits65h=0; // bounded missing-actor retries; does not refresh original impact
    std::uint64_t key=0,generation=0,epoch=0,request=0,sample=0,queued=0;
    kcd2vr::contact59c7g::Metadata metadata59c7g{};}; // C7G: immutable metadata belongs to this exact queued contact
// R59.16C7E: the impact's body material travels beside the unchanged submit signature, keyed by the impact sample id.
struct BodyMaterialNote {std::atomic<std::uint64_t> sample{0};std::atomic<int> material{-1};};
inline std::array<BodyMaterialNote,8> g_body_material59c7e{};
inline void NoteBodyMaterial59c7e(std::uint64_t sample,int material) noexcept {
    if(!sample)return;static std::atomic<unsigned> next{0};auto& n=g_body_material59c7e[next.fetch_add(1,std::memory_order_relaxed)%8];
    n.material.store(material,std::memory_order_relaxed);n.sample.store(sample,std::memory_order_release);}
std::mutex g_mutex;Pending g_pending{};bool g_has=false,g_draining59c3=false;std::uint64_t g_last_submit=0;std::uint32_t g_serial=0;
kcd2vr::contact_backlog59c6::Queue<Pending,8> g_backlog59c6;
std::atomic<std::uint64_t> g_backlogQueued59c6{0},g_backlogFull59c6{0},g_backlogExpired59c6{0};
// C56 review (Claude): per-site drop counters for the exits that used to return without a line
// (the 12-log analysis: 10-16 % of committed swings ended in "queued=1, no receipt"). Counters only
// here; the bounded per-site lines are written at the exits themselves. Never reset.
std::atomic<std::uint64_t> g_drop_recovery56{0},        // Drain entry / before the live handler: handling-recovery epoch changed
    g_drop_waiting_expired56{0},                            // contact expired (160 ms) while it was waiting for the strike window
    g_drop_flag220_56{0},                                   // phase 4/8 but [actor+0x220]+0x28 != 0 (C5235C precondition)
    g_drop_serial_claimed56{0},                             // release serial already claimed (earlier contact / native answer / admission)
    g_drop_duplicate56{0},                                  // hybrid attempt already consumed (same swing already answered) - expected
    g_drop_weapon_item_live56{0},                           // weapon/shield item contact on the live route (physical stop only)
    g_drop_nonordinary_phase56{0},                          // NO_RELEASE route refused by a protected native phase
    g_drop_outer_unavailable56{0};                          // direct transport preflight: reaction/notification transport missing
#include "../subsystems/hybrid/target_wait_state.inl" // M2_OWNED_SOURCE
static void RecoveryRetired65h(const Pending& p,std::uint64_t epoch,std::uint64_t now,kcd2vr::recovery65h::Site site) noexcept {
 kcd2vr::recovery65h::retired(p,epoch,now,site,diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now));
}
struct ActorSeen {const void* actor=nullptr;std::uint32_t entityId=0;std::uint64_t seen=0,resolved=0;};
#include "../subsystems/gore/contact_declarations.inl" // M2_OWNED_SOURCE
#include "body_combo59c7g.inl"
std::array<ActorSeen,32> g_actors{};std::mutex g_actors_mutex;
std::atomic<std::uint64_t> g_stats[8]{}; // requests, submitted, returnedTrue, targetMiss, fault, dropped, opponentFallback, ctorMismatch
// R59.16C8 STRIKE POWER (Miki, 13 Oct): "two-handed grip = stronger hits like the game's charged ones; normal strike = my
// usual full strike +-10 %; weakest stab = 30 % of normal health and stamina cost; every hit counts but the same hits inside
// 2 s count progressively less". The native hit calculation receives ONE number for the attack strength (the `strength`
// argument of BuildAndSubmitMeleeHit and S_CombatActorState AttackStrength, 1.0 = a full uncharged native swing; native
// NPC swings read 0.55..1.0 in the 13 Oct log). Everything else (weapon stats, armour, skills, perks) stays native.
//   base   = 0.30 + 0.70 * E/E_ref                 (E_ref = 21.6 J, a 1.6 kg sword at 7 m/s), clamped 0.30 .. 1.10
//   x 1.25 when the support hand holds the weapon (two-handed grip), capped 1.35
//   x 1 / 0.5 / 0.25 for the 1st / 2nd / 3rd+ hit on the same target inside 2 s (window restarts after 2 s of nothing)
//   (C26) NO bonus after Henry's perfect block: the riposte is native; riposte59c8 now only marks the receipt
struct PowerNote {std::atomic<std::uint64_t> sample{0};std::atomic<float> ratio{1.f};std::atomic<bool> twoHanded{false};};
inline std::array<PowerNote,8> g_power_note59c8{};
inline void NotePower59c8(std::uint64_t sample,float ratio,bool twoHanded) noexcept {
    if(!sample)return;static std::atomic<unsigned> next{0};auto& n=g_power_note59c8[next.fetch_add(1,std::memory_order_relaxed)%8];
    n.ratio.store(std::isfinite(ratio)?ratio:1.f,std::memory_order_relaxed);n.twoHanded.store(twoHanded,std::memory_order_relaxed);n.sample.store(sample,std::memory_order_release);}
inline bool TakePower59c8(std::uint64_t sample,float& ratio,bool& twoHanded) noexcept {
    if(!sample)return false;for(auto& n:g_power_note59c8)if(n.sample.load(std::memory_order_acquire)==sample){ratio=n.ratio.load(std::memory_order_relaxed);twoHanded=n.twoHanded.load(std::memory_order_relaxed);return true;}return false;}
namespace power59c8 {
inline std::atomic<bool> g_enabled{true};inline std::atomic<float> g_min{.30f},g_max{1.10f},g_two_handed{1.25f},g_riposte{1.f}; // C26: riposte bonus retired (Miki 16 Oct: game rules stay native)
inline std::atomic<float> g_final_max{1.20f}; // R59.16C10R6: 120 % ceiling of the user's damage rule
inline std::atomic<unsigned> g_repeat_ms{2000};
inline std::atomic<float> g_repeat_full26{.85f}; // C26: strokes at/above this base power are exempt from the repeat penalty
struct Repeat {std::uint32_t target=0;std::uint64_t tick=0;int count=0;};
inline Repeat g_repeat[8]{}; // under geom27 g_mutex (Deliver runs on the game thread)
struct Result {float power=1.f,base=1.f;int repeat=0;bool twoHanded=false,riposte=false;};
inline Result Compute(const Pending& p,std::uint64_t now,bool riposteWindow) noexcept {
    Result r{};if(!g_enabled.load(std::memory_order_relaxed))return r;
    const float ratio=std::isfinite(p.powerRatio59c8)?std::max(0.f,p.powerRatio59c8):1.f;
    float base=std::clamp(.30f+.70f*ratio,g_min.load(),g_max.load());
    if(p.twoHanded59c8){base=std::min(base*g_two_handed.load(),g_max.load()*1.25f);r.twoHanded=true;}
    r.base=base;
    // Preview only: constructor/owner/transport refusal must not count as a hit.
    const unsigned window=g_repeat_ms.load();const Repeat* slot=nullptr;
    for(const auto& prior:g_repeat)if(prior.target==p.entityId){slot=&prior;break;}
    r.repeat=slot&&slot->tick&&now>=slot->tick&&now-slot->tick<window?slot->count+1:0;
    // C26 (independent review 16 Oct: the repeat rule also cut a correctly timed full-stroke combo to 1.0/0.5/0.3).
    // Miki's rule is "no winning by 1000 small cuts": the repeat penalty now applies only to WEAK strokes. A proper
    // full stroke (base >= 0.85 = about 80 % of your reference stroke energy) is never
    // reduced by the repeat rule; native combos/RPG decide the rest.
    const bool fullStroke26=base>=g_repeat_full26.load(std::memory_order_relaxed);
    float mult=r.repeat==0||fullStroke26?1.f:r.repeat==1?.5f:.25f;
    if(riposteWindow){mult*=g_riposte.load();r.riposte=true;}
    // R59.16C10R6 (Miki's damage rule, 13 Oct, final): the mod only SCALES the native normal swing, within 30..120 %.
    // C10R5 let the product reach 0.05..2.0 (two-handed 1.375, riposte x1.5, 3rd repeat 0.075). Every bonus/penalty
    // still applies, but the final strength stays inside [VrStrikeMinPower, g_final_max] so weapons/armour keep mattering.
    r.power=kcd2vr::strike_power59c10r6::Final(base*mult,g_min.load(),g_final_max.load());return r;}
inline void RecordDelivered(const Pending& p,std::uint64_t now) noexcept {
    if(!g_enabled.load(std::memory_order_relaxed))return;
    Repeat* slot=nullptr;Repeat* oldest=&g_repeat[0];
    for(auto& prior:g_repeat){if(prior.target==p.entityId){slot=&prior;break;}if(prior.tick<oldest->tick)oldest=&prior;}
    if(!slot){slot=oldest;*slot={};slot->target=p.entityId;}
    slot->count=slot->tick&&now>=slot->tick&&now-slot->tick<g_repeat_ms.load()?slot->count+1:0;
    slot->tick=now;
}
}
#include "../subsystems/masterstrike_combos/perfect_block_window.inl" // M2_OWNED_SOURCE
#include "../subsystems/hybrid/answer_state.inl" // M2_OWNED_SOURCE
#include "../subsystems/masterstrike_combos/lineage_owner.inl" // M2_OWNED_SOURCE
std::atomic<std::uint64_t> g_lock_click_until178r29{0},g_lock_click_last178r29{0}; // V66.178R29: one R3 (opponent lock) click when a hit lands out of combat
bool LockClickPending(std::uint64_t now) noexcept {const auto u=g_lock_click_until178r29.load(std::memory_order_relaxed);return u&&now<u;}
// V66.178R31: Henry's own inCombat flag, observed once per C_CombatPlayer update (game thread). The 00:25 log: 15 applied
// hits, 29 damage lines, the enemy died standing and never reacted, Henry's state said inCombat=0 the whole time - the
// R29 R3 click never entered combat either (out of combat R3 is toggle_crouch: "Henry goes in the crouch and loses his
// weapon"). The native attack lifecycle is the game's own way into combat mode, so rung 2 lets it pass while this says 0.
bool NativeOwned178r33(std::uint64_t now) noexcept;void TelegraphTick178r34(const void* hs,std::uint64_t now) noexcept; // defined below
std::atomic<std::uint64_t> g_henry_seen178r31{0};std::atomic<int> g_henry_in_combat178r31{-1};std::atomic<bool> g_engage_native178r31{true};
std::atomic<std::uint64_t> g_henry_in_combat_edges178r33{0};
namespace vrstate596 {void Tick(const void* hs,std::uint64_t now) noexcept;std::atomic<const void*> g_henry_actor{nullptr};} // R59.6 forward declarations (defined after Enabled())
// =====================================================================================================================
namespace impulse596 {void Drain(std::uint64_t now) noexcept;}
#include "live_attributes5912.inl"
#include "native_victim_watch5916c3.inl"
namespace vrstate596 {void StaminaFallback59c17(const void* actor,const void* hs,std::uint64_t now) noexcept;} // R59.16C10R17 (defined after EffortNative5916)
void ObserveHenry178r31(const void* actor) noexcept {
    victim5916c3::Expire(std::uint64_t(GetTickCount64()));
    const void *he=nullptr,*hs=nullptr;if(!CombatActor(actor,he,hs,false)||!hs)return;
    vrstate596::StaminaFallback59c17(actor,hs,std::uint64_t(GetTickCount64()));
    CollectAttributes5912(actor,he,std::uint64_t(GetTickCount64()));
    TelegraphTick178r34(hs,std::uint64_t(GetTickCount64())); // V66.178R34
    vrstate596::g_henry_actor.store(actor); // R59.16
    vrstate596::Tick(hs,std::uint64_t(GetTickCount64()));impulse596::Drain(std::uint64_t(GetTickCount64())); // R59.6: VR state driver + contact impulses (game thread)
}
#include "henry_combat_context_h20.inl"

int HenryInCombat178r31(std::uint64_t now) noexcept {
    const auto s=g_henry_seen178r31.load(std::memory_order_acquire);if(!s||now<s||now-s>1000)return -1;return g_henry_in_combat178r31.load(std::memory_order_relaxed);}
#include "../subsystems/hybrid/legacy_telegraph.inl" // M2_OWNED_SOURCE
// V66.178R33 (R31 review 1/2): ONE owner per gesture. While a native engagement lifecycle is latched (prepare passed
// through in rung 2) the geometry route does not submit, and the native clip is not hidden; the latch ends at the
// commit/cancel plus a short tail so a delayed contact of the same swing cannot land twice.
std::atomic<std::uint64_t> g_native_owned_until178r33{0},g_native_owned_key178r33{0};
void NativeOwnerLatch178r33(std::uint64_t key,std::uint64_t until) noexcept {g_native_owned_key178r33.store(key,std::memory_order_relaxed);g_native_owned_until178r33.store(until,std::memory_order_release);}
bool NativeOwned178r33(std::uint64_t now) noexcept {const auto u=g_native_owned_until178r33.load(std::memory_order_acquire);return u&&now<u;}
bool Enabled() noexcept {return g_enabled.load(std::memory_order_relaxed);}
// =====================================================================================================================
#include "../subsystems/direct/native_state_context.inl" // M2_OWNED_SOURCE
// =====================================================================================================================
// C4R4 F9 contact experiment: existing articulated bodies only. This does not
// make living actors ragdoll, change RPG outcomes, or install a grab constraint.
// The exact native no-point path omits a contact moment arm AND the second
// skeleton impulse pass. Original contact identity/age survives queueing.
namespace impulse596 {
constexpr std::uintptr_t kEntitySystemGlobal=0x492D8A0ull,kEntitySystemVft=0x4077048ull,kEntityLookup=0x4B4F98ull;
std::atomic<bool> g_enabled{false},g_observe{false},g_fault{false};
std::atomic<float> g_scale{1.f},g_cap{12.f},g_mass{1.6f};
std::atomic<std::uint64_t> g_sent{0},g_dropped{0},g_session{1};
struct Req {std::uint32_t entity=0;int part=-1;int body=0;float pos[3]{},dir[3]{},speed=0;
    std::uint64_t tick=0,recovery=0,session=0,key=0,generation=0,epoch=0,request=0,physicsEpochH21=0;std::uintptr_t collider=0;};
// A living animated NPC can expose articulated limb shapes. Physics type is
// never death proof. Only a completed native lethal contact in this exact
// body/recovery lifetime may admit this optional corpse impulse experiment.
constexpr std::uint64_t kNativeDeathAge56=3600000;
struct NativeDeath56 {std::uint32_t entity=0;std::uintptr_t identity=0;std::uint64_t epoch=0,recovery=0,tick=0,serial=0;};
std::mutex g_death_mutex56;std::array<NativeDeath56,16> g_deaths56{};unsigned g_death_next56=0;
static void NoteNativeDeath56(const kcd2vr::contact58c1::Receipt& r,std::uint64_t epoch,std::uint64_t recovery,std::uint64_t now) noexcept {
    if(!r.serial||!r.target||!r.targetIdentity||!r.seen||!r.accepted||!r.applied||!r.fields||!r.bodyContact||r.death!=1||
       !epoch||r.epoch!=epoch||recovery!=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)||
       !kcd2vr::contact176::fresh(r.tick,now,80))return;
    std::lock_guard<std::mutex> lock(g_death_mutex56);
    for(const auto& d:g_deaths56)if(d.entity==r.target&&d.identity==r.targetIdentity&&d.epoch==epoch&&d.recovery==recovery)return; // no lease refresh
    g_deaths56[g_death_next56++%g_deaths56.size()]={r.target,r.targetIdentity,epoch,recovery,r.tick,r.serial};
}
static bool NativeDeathCurrent56(const Req& r,const void* entity,std::uint64_t now,const char*& why) noexcept {
    std::lock_guard<std::mutex> lock(g_death_mutex56);
    for(const auto& d:g_deaths56)if(d.entity==r.entity&&d.identity==reinterpret_cast<std::uintptr_t>(entity)&&
       d.epoch==r.epoch&&d.recovery==r.recovery&&d.recovery==kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)&&
       d.serial&&kcd2vr::contact176::fresh(d.tick,now,kNativeDeathAge56))return true;
    why="NATIVE_DEATH_NOT_OBSERVED_IN_CURRENT_LIFETIME";return false;
}
std::mutex g_mutex;std::array<Req,8> g_queue{};unsigned g_count=0;
static bool OwnerCurrent19c4r4(const Req& r,std::uint64_t now) noexcept {
    return r.physicsEpochH21==kcd2vr::collab_h21::epoch.load(std::memory_order_acquire)&&
        r.session==g_session.load()&&r.recovery==kcd2vr::contact176::recoveryEpoch55c1.load()&&
        kcd2vr::contact176::fresh(r.tick,now,160)&&r.key&&r.generation&&r.epoch&&r.request&&
        frik_pose_probe::GeometryOwnerValid178r30(r.key,r.generation,r.epoch,r.request);
}
void Request(std::uint32_t entity,int part,int body,const float pos[3],const float normal[3],float speed,
    std::uint64_t sourceTick,std::uintptr_t collider,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if((!g_enabled.load()&&!g_observe.load())||g_fault.load()||!kcd2vr::contact176::fresh(sourceTick,now,160)||
       !entity||!collider||!key||!generation||!epoch||!request||!std::isfinite(speed)||speed<1.f||speed>40.f)return;
    for(int i=0;i<3;++i)if(!std::isfinite(pos[i])||!std::isfinite(normal[i]))return;
    const float nl=std::sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);if(!(nl>.5f&&nl<1.5f))return;
    std::lock_guard<std::mutex> lock(g_mutex);if(g_count>=g_queue.size()){++g_dropped;return;}
    Req r{};r.entity=entity;r.part=part;r.body=body;r.speed=speed;r.tick=sourceTick;r.collider=collider;
    r.recovery=kcd2vr::contact176::recoveryEpoch55c1.load();r.session=g_session.load();r.key=key;r.generation=generation;r.epoch=epoch;r.request=request;
    r.physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    for(int i=0;i<3;++i){r.pos[i]=pos[i];r.dir[i]=-normal[i]/nl;}g_queue[g_count++]=r;
}
// Per-target native-frame budget, not an entity/body lifetime cache. Addresses
// are compared only. A failed/refused native call is not immediately retried.
struct Budget19c4r4 {std::uint32_t entity=0;std::uintptr_t body=0;std::uint64_t tick=0,source=0,recovery=0;};
std::array<Budget19c4r4,32> g_budget19c4r4{};
static bool Reserve19c4r4(const Req& r,std::uint64_t now,const char*& why) noexcept {
    Budget19c4r4* empty=nullptr;
    for(auto& x:g_budget19c4r4){
        if(x.entity==r.entity&&x.recovery==r.recovery){
            if(now<x.tick||now-x.tick<500){why="BODY_COOLDOWN";return false;}
            if(x.body==r.collider&&r.tick<=x.source){why="DUPLICATE_CONTACT";return false;}
            x={r.entity,r.collider,now,r.tick,r.recovery};return true;
        }
        if(!empty&&(!x.entity||x.recovery!=r.recovery||(now>=x.tick&&now-x.tick>=2000)))empty=&x;
    }
    if(!empty){why="BODY_BUDGET_FULL";return false;}
    *empty={r.entity,r.collider,now,r.tick,r.recovery};return true;
}
static void* EntityById(std::uint32_t id) noexcept {
    std::uintptr_t sys=0,vf=0,fn=0;
    if(!SehCopy146(reinterpret_cast<const void*>(g_base+kEntitySystemGlobal),&sys,8)||!sys||!SehCopy146(reinterpret_cast<const void*>(sys),&vf,8)||vf!=g_base+kEntitySystemVft||
       !SehCopy146(reinterpret_cast<const void*>(vf+0x70),&fn,8)||fn!=g_base+kEntityLookup)return nullptr;
    void* ent=nullptr;__try{ent=reinterpret_cast<void*(__fastcall*)(std::uintptr_t,std::uint32_t)>(fn)(sys,id);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return (ent&&Rd<std::uintptr_t>(ent,0,0)==g_base+kEntityVft&&Rd<std::uint32_t>(ent,0x0C,0)==id)?ent:nullptr;
}

constexpr std::uintptr_t kPhysicalProxyVft5916c3=0x3AC3478,kProxyImpulse5916c3=0x1466710,kArticulatedVft19c4r4=0x3A48B30;
constexpr unsigned char kProxyImpulseEntry5916c3[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x89,0x50,0x10,0x55,0x57,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D};
constexpr unsigned char kParentEntry5916c3[]{0x48,0x8B,0x91,0x98,0x00,0x00,0x00,0x33,0xC0,0x48,0x85,0xD2,0x75,0x01,0xC3,0x48,0x8B,0x42,0x18,0xC3};
constexpr unsigned char kArticulatedType19c4r4[]{0xB8,0x06,0x00,0x00,0x00,0xC3};
constexpr unsigned char kArticulatedAction19c4r4[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x50,0x10,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
constexpr unsigned char kNoPoint19c4r4[]{0x80,0xBD,0x80,0x00,0x00,0x00,0x00,0xB8,0xFF,0xFF,0xBF,0xFF,0x8B,0x75,0x68,0x89,0x44,0x24,0x7C,0x89,0x44,0x24,0x70};
constexpr unsigned char kQueuedAction19c4r4[]{0x45,0x33,0xC0,0x89,0x44,0x24,0x6C,0x48,0x8B,0xCB,0x48,0x8B,0x03,0xF2,0x0F,0x11,0x44,0x24,0x64,0xFF,0x50,0x38};
constexpr unsigned char kNoSkeleton19c4r4[]{0x80,0xBD,0x80,0x00,0x00,0x00,0x00,0x0F,0x85,0xA2,0x06,0xF0,0x00};
// Both native dispatch routes require the same current entity/parent owner.
// A proxy-body mismatch is not permission to bypass native parent forwarding.
static bool ImpulseEntityParent57(const Req& r,const void* entity,const char*& why) noexcept {
    if(!entity||Rd<std::uintptr_t>(entity,0,0)!=g_base+kEntityVft||Rd<std::uint32_t>(entity,0x0C,0)!=r.entity){
        why="ENTITY_IDENTITY_CHANGED";return false;}
    const auto attachment=Rd<std::uintptr_t>(entity,0x98,~std::uintptr_t(0));
    if(attachment==~std::uintptr_t(0)||(attachment&&Rd<std::uintptr_t>(reinterpret_cast<const void*>(attachment),0x18,~std::uintptr_t(0))!=0)){
        why="PARENT_BODY_TRANSFER_UNQUALIFIED";return false;}
    return true;
}
static bool ImpulseBody5916c3(const Req& r,void* entity,std::uintptr_t& proxy,const char*& why) noexcept {
    auto read=[](std::uintptr_t a,auto& v){return SehCopy146(reinterpret_cast<const void*>(a),&v,sizeof(v));};
    const auto e=reinterpret_cast<std::uintptr_t>(entity);
    if(!ImpulseEntityParent57(r,entity,why))return false;
    if(r.body!=int(kcd2vr::npc_contact172::Body::Articulated)){why="NOT_ALREADY_ARTICULATED";return false;}
    if(!kcd2vr::npc_contact172::proxy(e,1,read,proxy)||!proxy){why="NO_QUALIFIED_PHYSICS_PROXY";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0,0)!=g_base+kPhysicalProxyVft5916c3||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kPhysicalProxyVft5916c3),0xD0,0)!=g_base+kProxyImpulse5916c3||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0x30,0)!=e){why="PROXY_TYPE_OR_OWNER_MISMATCH";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(proxy),0x38,0)!=r.collider){why="CONTACT_BODY_REPLACED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.collider),0,0)!=g_base+kArticulatedVft19c4r4){why="BODY_NO_LONGER_ARTICULATED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),8,0)!=g_base+0x1A72E50||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x30,0)!=g_base+0x8A9348||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x38,0)!=g_base+0xA99DF0){why="ARTICULATED_ABI_MISMATCH";return false;}
    return true;
}
// C37C4: the struck body of a CORPSE is usually a limb of the character's own articulated physics, not the body the
// entity proxy points to (+0x38). In Miki's 17 Oct run 102 of 105 pushes were refused as CONTACT_BODY_REPLACED for that
// reason. When the collider is that articulated entity AND it belongs to the same game entity (foreign data type 1 =
// entity, data = the entity pointer; GetForeignData +0x50 = 5D6F44 reads exactly [+0x28]==type ? [+0x20] : 0), the
// impulse is given to the struck body itself through its own pinned Action (+0x38 = A99DF0) as pe_action_impulse on
// the struck part - the same call DeadBody.lua's AddImpulse(hit.ipart, ...) ends in. Never a different body.
static bool NativeColliderImpulse37c4(std::uintptr_t collider,void* action,const char*& why) noexcept;
struct ActionImpulse37c4 {int type=2;float impulse[3]{};float angImpulse[3]{};float point[3]{};int partid=0;int ipart=0;int iApplyTime=2;int iSource=0;};
static_assert(sizeof(ActionImpulse37c4)==56,"pe_action_impulse layout");
static_assert(offsetof(ActionImpulse37c4,partid)==40&&offsetof(ActionImpulse37c4,iApplyTime)==48,"retail impulse offsets");
constexpr unsigned char kForeignDataEntry40[]{0x0F,0xBF,0x41,0x28,0x3B,0xC2,0x75,0x05,0x48,0x8B,0x41,0x20,0xC3,0x33,0xC0,0xC3};
static bool ColliderIsThisEntity37c4(std::uintptr_t collider,const void* entity) noexcept {
    std::int16_t ft=0;std::uintptr_t fd=0;
    return SehCopy146(reinterpret_cast<const void*>(collider+0x28),&ft,sizeof(ft))&&ft==1&&
        SehCopy146(reinterpret_cast<const void*>(collider+0x20),&fd,sizeof(fd))&&fd==reinterpret_cast<std::uintptr_t>(entity);
}
static bool ColliderPush37c4(const Req& r,void* entity,const float impulse[3],const char*& why) noexcept {
    const auto c=r.collider;
    if(!ImpulseEntityParent57(r,entity,why))return false;
    const bool nativeDeathH7=NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why);
    if(!nativeDeathH7)return false;
    // Resolve the current skeleton owner BEFORE dereferencing the saved collider
    // on the new route. A different current body is never substituted.
    const bool skeletonOwnerH7=frik_pose_probe::GeometryCorpseBodyH7(r.entity,reinterpret_cast<std::uintptr_t>(entity),c,r.part,nativeDeathH7);
    if(frik_pose_probe::GeometryCorpseContactFaultH7()){why="CORPSE_STATUS_EXCEPTION";return false;}
    const bool legacyEntityOwnerH7=!skeletonOwnerH7&&ColliderIsThisEntity37c4(c,entity);
    if(!skeletonOwnerH7&&!legacyEntityOwnerH7){why="COLLIDER_NOT_THIS_ENTITY_OR_CURRENT_RELINQUISHED_MAIN";return false;}
    if(!c||Rd<std::uintptr_t>(reinterpret_cast<const void*>(c),0,0)!=g_base+kArticulatedVft19c4r4){why="COLLIDER_NOT_ARTICULATED";return false;}
    if(Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x38,0)!=g_base+0xA99DF0||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+kArticulatedVft19c4r4),0x50,0)!=g_base+0x5D6F44){why="ARTICULATED_ABI_MISMATCH";return false;}
    ActionImpulse37c4 a{};a.impulse[0]=impulse[0];a.impulse[1]=impulse[1];a.impulse[2]=impulse[2];
    const std::uint32_t unusedF=0xFFBFFFFFu;std::memcpy(&a.angImpulse[0],&unusedF,4);std::memcpy(&a.point[0],&unusedF,4);
    a.partid=r.part>=0?r.part:int(0x80000000u);a.ipart=int(0x80000000u);a.iApplyTime=2;a.iSource=0;
    const bool deathBeforeOwnerH7=NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why);
    if(!deathBeforeOwnerH7)return false;
    if(skeletonOwnerH7&&!frik_pose_probe::GeometryCorpseBodyH7(r.entity,reinterpret_cast<std::uintptr_t>(entity),c,r.part,deathBeforeOwnerH7)){
        why=frik_pose_probe::GeometryCorpseContactFaultH7()?"CORPSE_STATUS_EXCEPTION":"CURRENT_RELINQUISHED_MAIN_CHANGED";return false;}
    if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,std::uint64_t(GetTickCount64()))){why="OWNER_CHANGED_BEFORE_CALL";return false;}
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    if(!ImpulseEntityParent57(r,entity,why))return false;
    const bool sentH7=NativeColliderImpulse37c4(c,&a,why);
    if(sentH7)why=skeletonOwnerH7?"CURRENT_RELINQUISHED_MAIN_ACTION":"COLLIDER_DIRECT_ACTION";
    else if(!why||std::strcmp(why,"COLLIDER_ACTION_EXCEPTION")!=0)why=skeletonOwnerH7?"CURRENT_RELINQUISHED_MAIN_ACTION_REFUSED":"COLLIDER_DIRECT_ACTION_REFUSED";
    return sentH7;
}
static bool Contract19c4r4() noexcept {
    static int verified=-1;
    if(verified<0)verified=(retail_build112::Ready(g_base)&&
        EntryIs596(kProxyImpulse5916c3,kProxyImpulseEntry5916c3,sizeof(kProxyImpulseEntry5916c3))&&
        EntryIs596(0x83A8F4,kParentEntry5916c3,sizeof(kParentEntry5916c3))&&
        EntryIs596(0x1A72E50,kArticulatedType19c4r4,sizeof(kArticulatedType19c4r4))&&
        EntryIs596(0xA99DF0,kArticulatedAction19c4r4,sizeof(kArticulatedAction19c4r4))&&
        EntryIs596(0x5D6F44,kForeignDataEntry40,sizeof(kForeignDataEntry40))&&
        EntryIs596(0x146681A,kNoPoint19c4r4,sizeof(kNoPoint19c4r4))&&
        EntryIs596(0x146687C,kQueuedAction19c4r4,sizeof(kQueuedAction19c4r4))&&
        EntryIs596(0x1466892,kNoSkeleton19c4r4,sizeof(kNoSkeleton19c4r4)))?1:0;
    return verified==1;
}
static bool NativeImpulse19c4r4(std::uintptr_t proxy,const Req& r,const float impulse[3],bool atPoint,const char*& why) noexcept {
    using Fn=void(__fastcall*)(void*,int,const float*,const float*,bool,float,float);
    // bPos=false is verified at both native branches: no point moment arm, no
    // extra skeleton pass. Native Action's own scheduling flag is zero.
    __try{reinterpret_cast<Fn>(g_base+kProxyImpulse5916c3)(reinterpret_cast<void*>(proxy),r.part,r.pos,impulse,atPoint,1.f,1.f);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){why="ADDIMPULSE_EXCEPTION";return false;}
}
static bool NativeColliderImpulse37c4(std::uintptr_t collider,void* action,const char*& why) noexcept { // C37C4
    using ActionFn=int(__fastcall*)(void*,void*,int);
    __try{return reinterpret_cast<ActionFn>(g_base+0xA99DF0)(reinterpret_cast<void*>(collider),action,0)!=0;}
    __except(EXCEPTION_EXECUTE_HANDLER){why="COLLIDER_ACTION_EXCEPTION";return false;}
}
static bool Deliver(const Req& r,float J,const char*& why) noexcept {
    if(!Contract19c4r4()){why="IMPULSE_CONTRACT_MISMATCH";return false;}
    void* entity=EntityById(r.entity);if(!entity){why="ENTITY_NOT_FOUND";return false;}
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    std::uintptr_t proxy=0;
    if(!ImpulseBody5916c3(r,entity,proxy,why)){
        if(std::strcmp(why,"CONTACT_BODY_REPLACED")!=0)return false; // every other refusal stands
        const auto now2=std::uint64_t(GetTickCount64());
        if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,now2)){why="OWNER_CHANGED_BEFORE_CALL";return false;}
        if(!Reserve19c4r4(r,now2,why))return false;
        const float impulse2[3]{r.dir[0]*J,r.dir[1]*J,r.dir[2]*J};
        if(!ColliderPush37c4(r,entity,impulse2,why))return false;
        return true; // preserve exact current-main versus legacy route receipt
    }
    const auto now=std::uint64_t(GetTickCount64());
    if(!g_enabled.load()||!kcd2vr::collab_h21::npcPhysics()||!OwnerCurrent19c4r4(r,now)){why="OWNER_CHANGED_BEFORE_CALL";return false;}
    // Fresh owner reads can run concurrently with equipment/tracking updates;
    // recheck the original proxy/body immediately before this game-thread call.
    std::uintptr_t checked=0;if(!ImpulseBody5916c3(r,entity,checked,why)||checked!=proxy)return false;
    if(!Reserve19c4r4(r,now,why))return false;
    const float impulse[3]{r.dir[0]*J,r.dir[1]*J,r.dir[2]*J};
    if(!NativeDeathCurrent56(r,entity,std::uint64_t(GetTickCount64()),why))return false;
    return NativeImpulse19c4r4(proxy,r,impulse,false,why);
}
static void Receipt19c4r4(const Req& r,std::uint64_t now,float J,const char* why,bool ok) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    static std::uint64_t last=0;if(last&&now>=last&&now-last<100)return;last=now;
    char m[430]{};std::snprintf(m,sizeof(m),"C4R4 PHYS_IMPULSE entity=%u body=%d part=%d speed=%.2f requestedJ=%.2f result=%s sent=%llu dropped=%llu nativeCallReturned=%d bodyResponse=UNOBSERVED originalAgeMs=%llu pointMode=NONE extraSkeletonPass=0 livingTransition=NONE",
        r.entity,r.body,r.part,r.speed,J,why,(unsigned long long)g_sent.load(),(unsigned long long)g_dropped.load(),ok?1:0,(unsigned long long)(now>=r.tick?now-r.tick:0));
    diagnostic_trace::Write(m);
}
static bool NativeFault57(const char* why) noexcept {
    return why&&(std::strcmp(why,"ADDIMPULSE_EXCEPTION")==0||std::strcmp(why,"COLLIDER_ACTION_EXCEPTION")==0||std::strcmp(why,"CORPSE_STATUS_EXCEPTION")==0);
}
static bool StopFaultedBatch57(unsigned remaining) noexcept {
    if(!g_fault.load(std::memory_order_acquire))return false;
    std::lock_guard<std::mutex> lock(g_mutex);
    g_dropped.fetch_add(std::uint64_t(remaining)+g_count,std::memory_order_relaxed);g_count=0;return true;
}
void Drain(std::uint64_t now) noexcept {
    if((!g_enabled.load()&&!g_observe.load())||g_fault.load()){std::lock_guard<std::mutex> lock(g_mutex);g_count=0;return;}
    std::array<Req,8> batch{};unsigned n=0;{std::lock_guard<std::mutex> lock(g_mutex);n=g_count;batch=g_queue;g_count=0;}
    for(unsigned i=0;i<n;++i){const auto& r=batch[i];const char* why="QUEUED_NATIVE";
        if(StopFaultedBatch57(n-i))break;
        if(!OwnerCurrent19c4r4(r,now)){++g_dropped;Receipt19c4r4(r,now,0,"CONTACT_EXPIRED_OR_OWNER_CHANGED",false);continue;}
        if(!g_enabled.load()){Receipt19c4r4(r,now,0,"OBSERVE_ONLY",false);continue;}
        const float cap=g_cap.load(),mass=g_mass.load(),scale=g_scale.load();
        if(!std::isfinite(cap)||!std::isfinite(mass)||!std::isfinite(scale)||cap<=0||mass<=0||scale<=0){++g_dropped;continue;}
        // C33 (Miki: "any leads to make the ragdolls more visible?"): the old 12 N*s hard cap on 1.6 kg was ~0.15 m/s on a body,
        // invisible. The push goes to the struck PART (native AddImpulse with its part id), so a limb-sized mass is right.
        // Menu RAGDOLL PUSH: low / normal / strong (scale x cap), capped at 80 N*s.
        const float J=std::min(std::min(cap,80.f),std::min(mass,8.f)*r.speed*std::min(scale,3.f));
        const bool ok=Deliver(r,J,why);if(ok)++g_sent;else ++g_dropped;
        if(!ok&&NativeFault57(why))g_fault.store(true,std::memory_order_release);
        Receipt19c4r4(r,now,J,why,ok);
        if(StopFaultedBatch57(n-i-1))break; // no more native calls or retained callback work after either native fault
    }
}
// C58 LAB: corpse qualification for the lab's corpse limb drag (frik_pose_probe lab58.inl asks per native frame while
// the left Grip is held). the same three proofs Deliver uses: current entity identity, the scoped native death receipt
// (NativeDeathCurrent56, same lifetime rules), and the parent-transfer check (ImpulseEntityParent57). read-only.
unsigned DeathCandidates58(std::uint32_t* out,unsigned cap,std::uint64_t now) noexcept {
    unsigned n=0;std::lock_guard<std::mutex> lock(g_death_mutex56);
    const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    for(const auto& d:g_deaths56){if(n>=cap)break;
        if(!d.entity||!d.serial||d.recovery!=recovery||!kcd2vr::contact176::fresh(d.tick,now,kNativeDeathAge56))continue;
        bool dup=false;for(unsigned i=0;i<n;++i)if(out[i]==d.entity)dup=true;if(!dup)out[n++]=d.entity;}
    return n;
}
bool CorpseQualified58(std::uint32_t entityId,std::uintptr_t identity,std::uint64_t epoch,std::uint64_t now,const char*& why) noexcept {
    why="";if(!entityId||!epoch){why="NO_ENTITY";return false;}
    void* entity=EntityById(entityId);if(!entity){why="ENTITY_NOT_FOUND";return false;}
    if(identity&&reinterpret_cast<std::uintptr_t>(entity)!=identity){why="ENTITY_IDENTITY_CHANGED";return false;}
    Req r{};r.entity=entityId;r.epoch=epoch;r.recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(!NativeDeathCurrent56(r,entity,now,why))return false;
    return ImpulseEntityParent57(r,entity,why);
}
// C58 LAB: one record per geometry hit the game ADMITTED (ok&&apply) and ANSWERED through the outer continuation
// (CallOuter rc>=0) on a LIVING, standing target. frik_pose_probe matches it to its own Living impact record by the
// impact sample id and gives the struck part a visual AddImpact. never written before/instead of the native answer.
struct AnsweredLiving58 {std::uint32_t entity=0;int part=-1;std::uint64_t sample=0,tick=0;std::uint32_t serial=0;int reaction=-1;};
std::mutex g_answered_mutex58;std::array<AnsweredLiving58,8> g_answered58{};unsigned g_answered_count58=0;
static void NoteAnsweredLiving58(std::uint32_t entity,int part,std::uint64_t sample,std::uint32_t serial,int reaction,std::uint64_t now) noexcept {
    if(!entity||!sample)return;std::lock_guard<std::mutex> lock(g_answered_mutex58);
    if(g_answered_count58>=g_answered58.size())g_answered_count58=0; // stale (nobody drained for 8 hits) -> drop, never grow
    g_answered58[g_answered_count58++]={entity,part,sample,now,serial,reaction};
}
unsigned TakeAnsweredLiving58(AnsweredLiving58* out,unsigned cap) noexcept {
    std::lock_guard<std::mutex> lock(g_answered_mutex58);const unsigned n=std::min(cap,g_answered_count58);
    for(unsigned i=0;i<n;++i)out[i]=g_answered58[i];g_answered_count58=0;return n;
}
}

bool CtorsVerified() noexcept {
    if(!g_base)return false;unsigned char a[sizeof(kHitInfoCtorEntry)]{},b[sizeof(kDetailsCtorEntry)]{};
    return SehCopy146(reinterpret_cast<const void*>(g_base+kHitInfoCtor),a,sizeof(a))&&std::memcmp(a,kHitInfoCtorEntry,sizeof(a))==0&&
           SehCopy146(reinterpret_cast<const void*>(g_base+kDetailsCtor),b,sizeof(b))&&std::memcmp(b,kDetailsCtorEntry,sizeof(b))==0;
}
static bool EntryIs(std::uintptr_t rva,const unsigned char* want,std::size_t n) noexcept {
    unsigned char a[32]{};return n<=sizeof(a)&&SehCopy146(reinterpret_cast<const void*>(g_base+rva),a,n)&&std::memcmp(a,want,n)==0;}
// R59.6 fix (21:30 log: GEOM_OUTER rejected=PROLOGUE_MISMATCH on every VR hit -> no reaction, no OnHit, no flinch):
// 90C850 carries OUR OWN read-only receipt detour (native_hit_receipt50c1.inl), so its first bytes are a jump. All other
// entries matched the retail file byte for byte. Calling 90C850 through that detour reaches the original unchanged.
// R59.8 (review): accept ONLY our own live receipt hook - it is installed from pristine bytes (Hook() verifies
// kReactionDispatchEntry50 first) and publishes its original trampoline. A foreign patch, a missing trampoline or a
// non-jump entry still rejects.
static bool DispatchHookedByUs596() noexcept {
    if(!g_reaction_dispatch_original50||!g_reaction_dispatch50)return false;
    unsigned char a[6]{};if(!SehCopy146(reinterpret_cast<const void*>(g_base+kOuterDispatch),a,sizeof(a)))return false;
    return a[0]==0xE9||(a[0]==0xFF&&a[1]==0x25)||(a[0]==0x48&&a[1]==0xB8);}
bool OuterVerified178r33() noexcept {
    static int cached=-1;if(cached>=0)return cached==1;
    const bool ok=g_base&&EntryIs(kOuterReact,kOuterReactEntry,sizeof(kOuterReactEntry))&&EntryIs(kOuterOnHit,kOuterOnHitEntry,sizeof(kOuterOnHitEntry))&&
        EntryIs(kOuterDown,kOuterDownEntry,sizeof(kOuterDownEntry))&&EntryIs(kOuterKind,kOuterKindEntry,sizeof(kOuterKindEntry))&&EntryIs(kOuterRelease,kOuterReleaseEntry,sizeof(kOuterReleaseEntry))&&
        (EntryIs(kOuterDispatch,kOuterDispatchEntry,sizeof(kOuterDispatchEntry))||DispatchHookedByUs596())&&EntryIs(kOuterDeathPred,kOuterDeathPredEntry,sizeof(kOuterDeathPredEntry));
    cached=ok?1:0;if(!ok)diagnostic_trace::Write("V66.178R33 GEOM_OUTER rejected=PROLOGUE_MISMATCH (D49AB4/D542E0/D53EEC/C52DF4/4A760C/90C850/B249FC: one differs from WHGame 6A350E20 and is not our own receipt hook; outer continuation disabled)");
    return ok;}
#include "game_hit_event5910c1.inl"
// C37 (F3 only): read-only bracket around the 90C850 call below. The reaction, its arguments, the native
// decision and reference counting are unchanged; this records which manager slot the reaction landed in.
static kcd2vr::defense37::Dispatch g_dispatch37{};
static bool Copy37(std::uintptr_t p,void* out,std::size_t n) noexcept {return p&&out&&n&&SehCopy146(reinterpret_cast<const void*>(p),out,n);}
static void DispatchBefore37(const void* attacker,const void* target,const void* reaction) noexcept {
    namespace d37=kcd2vr::defense37;auto& d=g_dispatch37;d=d37::Dispatch();
    const auto a=reinterpret_cast<std::uintptr_t>(attacker),t=reinterpret_cast<std::uintptr_t>(target);
    d.henryActor=a;d.target=t;d.henry=d37::ReadHenry(Copy37,g_base,reinterpret_cast<std::uintptr_t>(Rd<const void*>(attacker,0x210,nullptr)));
    d.before=d37::ReadNpc(Copy37,g_base,t,reinterpret_cast<std::uintptr_t>(Rd<const void*>(target,0x210,nullptr)),a);
    d.reaction=d37::ReadAction(Copy37,g_base,reinterpret_cast<std::uintptr_t>(reaction));d.captured=true;
}
static void DispatchAfter37(const void* target,int result) noexcept {
    auto& d=g_dispatch37;if(!d.captured||d.target!=reinterpret_cast<std::uintptr_t>(target))return;
    d.result=result;d.after=kcd2vr::defense37::ReadNpc(Copy37,g_base,d.target,reinterpret_cast<std::uintptr_t>(Rd<const void*>(target,0x210,nullptr)),d.henryActor);
}
// Returns: 0 reaction eligible, 1 native reaction gate closed, -1 continuation fault.
// Game-rule hit notification is required in BOTH non-fault paths.
static int CallOuter(const void* attacker,const void* target,void* hitInfo,void* details,const void* gameRules,bool& queued,bool& reaction,int& kindOut,bool& alive,bool& down,int& dispatched,bool counterMajor=false,int reactionZoneNR1=-1) noexcept {
    using BoolFn=bool(__fastcall*)(const void*);using KindFn=int(__fastcall*)(const void*,int,bool);
    using ReactFn=void*(__fastcall*)(const void*,void**,const void*,int,const void*,int);using RelFn=void(__fastcall*)(void**);
    using OnHitFn=void(__fastcall*)(const void*,void*,const void*,void*);
    // D499D8's real callee takes the victim reaction manager, not an action.
    // D49AB4 returns one retained action reference; 4A760C releases it once.
    const void* manager=Rd<const void*>(target,0x3A8,nullptr);void* out=nullptr;g_dispatch37=kcd2vr::defense37::Dispatch();

    __try{
        alive=reinterpret_cast<BoolFn>(g_base+kOuterAlive)(target);
        down=alive?reinterpret_cast<BoolFn>(g_base+kOuterDown)(target):true;
        if(alive&&!down){
            if(!manager||Rd<const void*>(manager,8,nullptr)!=target)return -1;
            const bool nativeFlagA=reinterpret_cast<BoolFn>(g_base+kOuterStrong)(attacker);
            const bool strong=kcd2vr::counter_stab40::reactionSelectorFlag(nativeFlagA,counterMajor);
            kindOut=reinterpret_cast<KindFn>(g_base+kOuterKind)(attacker,Rd<std::int32_t>(hitInfo,0x44,0),strong);
            const bool death=reinterpret_cast<BoolFn>(g_base+kOuterDeathPred)(hitInfo);
            // NR1 R1: the borrowed zone covers ONLY the two reaction builds (D49AB4), restored before dispatch/notification.
            const void* nr1Hs=nullptr;int nr1Restore=-1,nr1Written=-1;
            const bool nr1=reactionZoneNR1>=0&&vrstate596::ReactionZoneBeginNR1(attacker,reactionZoneNR1,nr1Hs,nr1Restore,nr1Written,target);
            __try{
            if(death)reinterpret_cast<ReactFn>(g_base+kOuterReact)(manager,&out,hitInfo,kindOut,attacker,
                Rd<int>(reinterpret_cast<const void*>(g_base+0x5320D24),0,-1));
            if(!out)reinterpret_cast<ReactFn>(g_base+kOuterReact)(manager,&out,hitInfo,kindOut,attacker,-1);
            }__finally{if(nr1)vrstate596::ReactionZoneEndNR1(attacker,nr1Hs,nr1Restore,nr1Written);}
            reaction=out!=nullptr;
            if(reaction){ // V66.178R36 (F01): the native dispatch with the native gates
                using DispatchFn=bool(__fastcall*)(const void*,void**);using PredFn=bool(__fastcall*)(const void*);using VictimPredFn=bool(__fastcall*)(const void*,const void*);
                const bool sameGroup=Rd<std::uint16_t>(target,0x384,0)==Rd<std::uint16_t>(attacker,0x384,1);
                const bool deathPred=reinterpret_cast<PredFn>(g_base+kOuterDeathPred)(hitInfo);
                // V66.178R37 (R36 review F03): C5235C dispatches when deathPred is TRUE (C525F8) or when 1E44410(attacker, victim)
                // is FALSE (C52601); 1E44410 reads the victim from RDX. R36 had the predicate inverted and argument-less.
                const bool gate=sameGroup&&(deathPred||!reinterpret_cast<VictimPredFn>(g_base+kOuterGlobalPred)(attacker,target));
                const bool watch37=gate&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,std::uint64_t(GetTickCount64()));
                if(watch37)DispatchBefore37(attacker,target,out);
                dispatched=gate?(reinterpret_cast<DispatchFn>(g_base+kOuterDispatch)(target,&out)?1:2):(sameGroup?3:4); // 1 accepted, 2 rejected by the victim's action system, 3 predicate gate, 4 group gate
                if(watch37)DispatchAfter37(target,dispatched);
            }
            reinterpret_cast<RelFn>(g_base+kOuterRelease)(&out);
        }
        // C5269F/C526B8: native flags mark an accepted reaction and a combat-actor victim.
        // C526BC..C526D5 queues HitInfo even when 9D3884 is false. Without it the
        // RPG result alone never reaches the game-rule hit consumer.
        auto flags=Rd<std::uint32_t>(hitInfo,0x78,0);
        if(dispatched==1){flags&=~2u;static_cast<unsigned char*>(hitInfo)[0x75]=1;}
        flags&=~0x20u; // this route has an exact combat-actor target
        std::memcpy(static_cast<unsigned char*>(hitInfo)+0x78,&flags,sizeof(flags));
        if(!QueueHitEvent5910c1(gameRules,hitInfo))return -1;
        queued=true;
        reinterpret_cast<OnHitFn>(g_base+kOuterOnHit)(attacker,hitInfo,target,details);
        return (alive&&!down)?0:1;
    }__except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
// Entity -> combat actor map, fed by C_CombatActor::Update (every actor, every frame): the pointer is
// remembered at once; valid ids resolve every 1 s. Failed identity reads retry
// after 50 ms on the next real ActorUpdate, without extending any contact lease.
#include "counter_response36.inl"
#include "../subsystems/direct/counter_stab_native.inl" // M2_OWNED_SOURCE
using ActorLookupScopeH7=kcd2vr::actor_lookup_h7::CallbackScope;
static const void* NativeActorForEntityH7(std::uint32_t id) noexcept {
    namespace lookup=kcd2vr::actor_lookup_h7;
    auto read=[](std::uintptr_t at,void* out,std::size_t size) noexcept {return SehCopy146(reinterpret_cast<const void*>(at),out,size);};
    auto call=[](std::uintptr_t method,std::uintptr_t service,std::uint32_t key,const void*& actor) noexcept {
        actor=nullptr;
        __try {actor=reinterpret_cast<const void*(__fastcall*)(void*,std::uint32_t)>(method)(reinterpret_cast<void*>(service),key);return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    };
    auto validate=[](const void* nativeActor,std::uint32_t key) noexcept -> const void* {
        // +278 comes from the actual native D51C1D enrichment load. Do not
        // allow a different actor/map key to stand in for the struck NPC.
        const void* nativeEntity=Rd<const void*>(nativeActor,0x38,nullptr);
        const void* combat=Rd<const void*>(nativeActor,0x278,nullptr);
        const void *entity=nullptr,*state=nullptr;
        if(!nativeEntity||EntityId(nativeEntity)!=key||!combat||
           !CombatActor(combat,entity,state,false)||!entity||!state||entity!=nativeEntity||EntityId(entity)!=key)return nullptr;
        return combat;
    };
    return lookup::lookup(g_base,id,retail_build112::Ready(g_base),read,call,validate);
}
void ActorSeen27(const void* actor) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
#include "../subsystems/direct/actor_update_observer.inl" // M2_OWNED_SOURCE
    std::unique_lock<std::mutex> lock(g_actors_mutex,std::try_to_lock);if(!lock.owns_lock())return;
    ActorSeen* slot=nullptr;ActorSeen* oldest=&g_actors[0];
    for(auto& a:g_actors){if(a.actor==actor){slot=&a;break;}if(a.seen<oldest->seen)oldest=&a;}
    if(!slot){slot=oldest;*slot={};slot->actor=actor;}
    slot->seen=now;
    if(slot->resolved&&now>=slot->resolved&&now-slot->resolved<(slot->entityId?1000u:50u))return;
    slot->resolved=now;const void *entity=nullptr,*state=nullptr;
    slot->entityId=(CombatActor(actor,entity,state,false)&&entity)?EntityId(entity):0;
}
const void* ActorForEntity(std::uint32_t id,std::uint64_t now) noexcept {
    if(!id)return nullptr;
    if(kcd2vr::actor_lookup_h7::callbackDepth){
        // A valid new contact must not wait for an NPC Update to populate our
        // 32-slot observer cache or for Henry to acquire that NPC as opponent.
        // Current native lookup is authoritative here: a removed/invalid map
        // member cannot fall back to a stale observed actor.
        const auto actor=NativeActorForEntityH7(id);
        if(actor){std::unique_lock<std::mutex> lock(g_actors_mutex,std::try_to_lock);
            if(lock.owns_lock()){
                ActorSeen* slot=nullptr;ActorSeen* oldest=&g_actors[0];
                for(auto& a:g_actors){if(a.actor==actor){slot=&a;break;}if(a.seen<oldest->seen)oldest=&a;}
                if(!slot)slot=oldest;*slot={actor,id,now,now};
            }}
        return actor;
    }
    // Physical-producer callers may only use the existing bounded observation;
    // they never invoke gameplay/native actor services on a physics thread.
    std::lock_guard<std::mutex> lock(g_actors_mutex);
    for(const auto& a:g_actors)if(a.actor&&a.entityId==id&&now>=a.seen&&now-a.seen<500)return a.actor;
    return nullptr;
}
// C19C3: an Update/map ordering gap is not a different impact. Retry only the
// original queued contact, within its unchanged 160 ms lease. An existing map
// candidate with a bad identity is terminal, never redirected to the opponent.
enum class TargetResolution19c3 { Ready, Missing, Invalid };
static bool OriginalContactOwned19c3(const Pending& p,std::uint64_t now) noexcept {
    return p.recovery55c1==kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)&&
        kcd2vr::contact176::fresh(p.tick,now,handoff59c3::maximumContactAge)&&
        kcd2vr::solver_rpg_h29::originalContactOwnedH30(p.metadata59c7g,p.key,p.generation,p.epoch,p.request,
            p.sample,p.gesture59c3,p.tick,now,frik_pose_probe::GeometryOwnerValid178r30,
            frik_pose_probe::OriginalGeometryContactOwnerH30,frik_pose_probe::GeometrySolverOwnerValidH28);
}
static bool RetainedContactBody19c3(const Pending& p,const void* entity) noexcept {
    // Reuse the exact retail read-only proxy map/owner contract already used
    // by the contact impulse adapter. No native method, physics query or write.
    if(!p.metadata59c7g.collider||!entity)return false;
    const auto e=reinterpret_cast<std::uintptr_t>(entity);
    auto read=[](std::uintptr_t a,auto& v){return SehCopy146(reinterpret_cast<const void*>(a),&v,sizeof(v));};
    std::uintptr_t proxy=0,vf=0,owner=0,body=0;
    if(!read(e,vf)||vf!=g_base+kEntityVft||!kcd2vr::npc_contact172::proxy(e,1,read,proxy)||!proxy||
       !read(proxy,vf)||vf!=g_base+impulse596::kPhysicalProxyVft5916c3||
       !read(proxy+0x30,owner)||owner!=e||!read(proxy+0x38,body))return false;
    if(body==p.metadata59c7g.collider)return true; // original capsule path unchanged
    auto copy=[](std::uintptr_t at,void* out,std::size_t size){return SehCopy146(reinterpret_cast<const void*>(at),out,size);};
    // A living NPC's ray-hit skeleton can differ from its locomotion capsule.
    // Validate the CURRENT main as this SAME original collider; do not replace
    // the contact or mint a retained allocation/part/native-action lease.
    return kcd2vr::articulated_target_owner_h8::current(g_base,e,p.entityId,p.metadata59c7g.collider,proxy,
        retail_build112::Ready(g_base),copy);
}
#include "../subsystems/direct/target_wait.inl" // M2_OWNED_SOURCE
#include "../subsystems/hybrid/target_wait.inl" // M2_OWNED_SOURCE
#include "../subsystems/gore/lethal_contact.inl" // M2_OWNED_SOURCE
#include "impale_armour40.inl" // C40 IMPALE BY ARMOUR: native per-zone armour read, once per candidate thrust
bool Submit(const Pending& p) noexcept {
    // Construct before the queue lock: format only after that lock is released.
    struct QueueTraceC54R3 {const Pending& p;const char* why="UNOBSERVED";std::uint32_t serial=0;bool queued=false;
        ~QueueTraceC54R3(){const auto now=std::uint64_t(GetTickCount64());
            if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;char line[540]{};
            std::snprintf(line,sizeof(line),"C54R3 HIT_QUEUE gesture=%llu sample=%llu target=%u tick=%llu ageMs=%llu serial=%u queued=%d why=%s key=%016llX generation=%llu epoch=%llu request=%llu nativeAcceptance=NOT_INFERRED",
                (unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.entityId,(unsigned long long)p.tick,
                (unsigned long long)(now>=p.tick?now-p.tick:UINT64_MAX),serial,int(queued),why,(unsigned long long)p.key,(unsigned long long)p.generation,(unsigned long long)p.epoch,(unsigned long long)p.request);diagnostic_trace::Write(line);}
    } queueTrace{p};
    g_stats[0].fetch_add(1,std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(g_mutex);
    const auto now=std::uint64_t(GetTickCount64());
    const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(!p.gesture59c3||!kcd2vr::contact176::fresh(p.tick,now,handoff59c3::maximumContactAge)){
        queueTrace.why=!p.gesture59c3?"NO_PHYSICAL_GESTURE":"IMPACT_EXPIRED_OR_FUTURE_160MS";
        // C67 review: enrich the existing bounded receipt from the original
        // impact. No extra actor reads, guessed speed, or contact-minted gesture.
        ++g_stats[5];if(Allow(4,16)){char line[480]{};std::snprintf(line,sizeof(line),
            "C54 HYBRID_ENQUEUE gesture=%llu sample=%llu target=%u kind=%d queued=0 reason=%s ageMs=%llu normalizedEnergy=%.3f powerRatio=%.3f part=%d surface=%d weaponItem=%d serial=%u",
            (unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.entityId,p.kind,
            !p.gesture59c3?"NO_PHYSICAL_GESTURE":"IMPACT_EXPIRED_OR_FUTURE",
            (unsigned long long)(now>=p.tick?now-p.tick:UINT64_MAX),double(p.energy),double(p.powerRatio59c8),
            p.part,p.surface,p.weaponItem?1:0,p.serial);diagnostic_trace::Write(line);}return false;
    }
    auto usable=[&](const Pending& x){const bool ok=x.recovery55c1==recovery&&
        kcd2vr::contact176::fresh(x.tick,now,handoff59c3::maximumContactAge);
        if(!ok)RecoveryRetired65h(x,recovery,now,kcd2vr::recovery65h::Site::SubmitPrune);return ok;};
    const auto pruned57=g_backlog59c6.prune(usable);
    g_backlogExpired59c6.fetch_add(pruned57,std::memory_order_relaxed);
    if(!g_draining59c3&&g_has&&!usable(g_pending)){
        g_drop_waiting_expired56.fetch_add(1,std::memory_order_relaxed);
        g_has=false;++g_backlogExpired59c6;}
    // No new formatting/trace queue work while the gameplay queue lock is held.
    if(!g_draining59c3&&!g_has)g_has=g_backlog59c6.pop(g_pending);
    Pending copy=p;copy.action=nullptr;copy.lease47c2={};copy.release51={};
    copy.recovery55c1=recovery;copy.queued=now;
    const auto same=kcd2vr::contact_backlog59c6::sameContact<Pending>;
    if((g_has&&same(g_pending,copy))||g_backlog59c6.contains(copy,same)){
        queueTrace.why="EXACT_CONTACT_ALREADY_QUEUED";
        ++g_coalesced59c3;return false; // duplicate, never refreshed or overwritten
    }
    if(++g_serial==0)++g_serial;copy.serial=g_serial;
    queueTrace.serial=copy.serial;
    if(!g_draining59c3&&!g_has){queueTrace.why="QUEUED_PRIMARY";queueTrace.queued=true;g_pending=copy;g_has=true;return true;}
    if(!g_backlog59c6.push(copy)){queueTrace.why="CONTACT_BACKLOG_FULL";++g_backlogFull59c6;++g_stats[5];return false;}
    queueTrace.why="QUEUED_BACKLOG";queueTrace.queued=true;++g_backlogQueued59c6;return true;
}
void Clear() noexcept {kcd2vr::hybrid_serial59::releases.clear();kcd2vr::motion59c4::hybridAttempts54.clear();kcd2vr::combo_lineage19::grace.clear();victim5916c3::Clear();vrstate596::Reset();std::lock_guard<std::mutex> lock(g_mutex);g_has=false;g_pending={};g_backlog59c6.clear();g_henry_seen178r31.store(0);g_henry_in_combat178r31.store(-1);g_native_owned_until178r33.store(0);g_tg_phase178r34.store(0);} // V66.178R30 (F03): a mode change drops queued work; R33: and the combat cache / owner latch
static bool CallCtor(std::uintptr_t fn,void* obj) noexcept {
    using Ctor=void(__fastcall*)(void*);
    __try{reinterpret_cast<Ctor>(fn)(obj);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool CallSubmit(void* rpg,void* hi,const float* st,bool* post,const void* rq,bool apply,int type,void* md,bool& ok) noexcept {
    __try{ok=g_submit_original(rpg,hi,st,post,rq,apply,type,md);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Called inside Henry's C_CombatActor::Update after the original (game thread).
#include "../subsystems/hybrid/consume_native_answer.inl" // M2_OWNED_SOURCE
void Drain(void* henryActor) noexcept {
    RearmCombatAfterLoadR96(); // R96: re-arm fault latches once after a save/level load
    Pending p{};int counterOrigin36=-1;{std::lock_guard<std::mutex> lock(g_mutex);
        if(g_draining59c3)return;
        if(!g_has)g_has=g_backlog59c6.pop(g_pending);
        if(!g_has)return;p=g_pending;g_draining59c3=true;}
    // Reserve the slot across the native callback. A producer/reentrant drain
    // cannot overwrite or deliver a contact while its first consumer is active.
    struct DrainLease59c3 {
        std::uint32_t serial;bool keep=false;
        ~DrainLease59c3(){
            // C56 review (Claude): a waiting contact that expires here used to vanish without a receipt
            // (12-log analysis rank 3b). Collect under the lock, write after it.
            Pending expired{};bool expiredNow=false;std::size_t pruned=0;std::uint64_t now=0,recovery=0;
            {std::lock_guard<std::mutex> lock(g_mutex);
            if(!keep&&g_has&&g_pending.serial==serial)g_has=false;g_draining59c3=false;
            now=std::uint64_t(GetTickCount64());
            recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
            const auto usable=[&](const Pending& x){return x.recovery55c1==recovery&&kcd2vr::contact176::fresh(x.tick,now,handoff59c3::maximumContactAge);};
            const auto prune65h=[&](const Pending& x){const bool ok=usable(x);if(!ok)RecoveryRetired65h(x,recovery,now,kcd2vr::recovery65h::Site::DrainPrune);return ok;};
            pruned=g_backlog59c6.prune(prune65h);g_backlogExpired59c6.fetch_add(pruned,std::memory_order_relaxed);
            if(g_has&&!usable(g_pending)){RecoveryRetired65h(g_pending,recovery,now,kcd2vr::recovery65h::Site::Waiting);expired=g_pending;expiredNow=true;g_has=false;++g_backlogExpired59c6;}
            if(!g_has)g_has=g_backlog59c6.pop(g_pending);}
            if(expiredNow){g_drop_waiting_expired56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char line[280]{};std::snprintf(line,sizeof(line),"C56 GEOM_HIT serial=%u rejected=%s site=DRAIN_LEASE gesture=%llu release=%llu ageMs=%llu waitedMs=%llu boundAction=%p",
                    expired.serial,expired.recovery55c1!=recovery?"RECOVERY_EPOCH_CHANGED_WHILE_WAITING":"CONTACT_EXPIRED_WHILE_WAITING(160ms since physical impact)",
                    (unsigned long long)expired.gesture59c3,(unsigned long long)expired.release51.serial,
                    (unsigned long long)(now>=expired.tick?now-expired.tick:0),(unsigned long long)(now>=expired.queued?now-expired.queued:0),expired.action);diagnostic_trace::Write(line);}}
            if(pruned&&Allow(4,4)){char line[200]{};std::snprintf(line,sizeof(line),"C56 GEOM_HIT rejected=BACKLOG_EXPIRED_OR_RECOVERED count=%u site=DRAIN_LEASE",unsigned(pruned));diagnostic_trace::Write(line);}}
        void wait(const Pending& p){std::lock_guard<std::mutex> lock(g_mutex);
            if(g_has&&g_pending.serial==serial){g_pending=p;keep=true;}}
        void defer(const Pending& p){std::lock_guard<std::mutex> lock(g_mutex);
            if(!g_has||g_pending.serial!=serial)return;
            Pending next{};
            if(g_backlog59c6.pop(next)){
                // Popping made room: keep this exact old contact at the tail,
                // without renewing tick/queued or blocking another recipient.
                if(!g_backlog59c6.push(p))++g_stats[5];
                g_pending=next;
            }else g_pending=p;
            keep=true;
        }
    } pendingLease{p.serial};
    const auto drainRecovery65h=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(p.recovery55c1!=drainRecovery65h){RecoveryRetired65h(p,drainRecovery65h,GetTickCount64(),kcd2vr::recovery65h::Site::DrainEntry);++g_stats[5];
        // C56 review (Claude): the handling-recovery drop had no receipt (C54 15:56:16.161 serial 94: queued=1, 17.5 m/s,
        // release 81 admitted, phase 8 open, nothing logged; the next contact of the same swing carried gesture 0).
        g_drop_recovery56.fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4)){const auto nowR=std::uint64_t(GetTickCount64());char m[260]{};std::snprintf(m,sizeof(m),"C56 GEOM_HIT serial=%u rejected=RECOVERY_EPOCH_CHANGED site=DRAIN_ENTRY gesture=%llu release=%llu ageMs=%llu contactRecovery=%llu currentRecovery=%llu (handling recovery invalidates queued contacts by design)",
            p.serial,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(nowR>=p.tick?nowR-p.tick:0),(unsigned long long)p.recovery55c1,(unsigned long long)kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire));diagnostic_trace::Write(m);}
        return;}
    const auto now=std::uint64_t(GetTickCount64());
    if(!g_submit_original||!CtorsVerified()){g_stats[7].fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,2))diagnostic_trace::Write("V66.178R27 GEOM_HIT rejected=CTOR_OR_HOOK_MISMATCH (0xB2454C/0xD508C0 prologue or submit hook missing)");return;}
    const void *he=nullptr,*hs=nullptr;
    if(!CombatActor(henryActor,he,hs,false)||!IsPlayer(he)){g_stats[4].fetch_add(1,std::memory_order_relaxed);return;}
    void* rpg=Rd<void*>(henryActor,0x388,nullptr);
    if(!rpg||Rd<const void*>(rpg,0x8,nullptr)!=henryActor){g_stats[4].fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,2)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R27 GEOM_HIT rejected=RPG_LINK actor+0x388=%p rpg+8!=actor",rpg);diagnostic_trace::Write(m);}return;}
    // V66.178R30 (F03): the queued request must still belong to the current weapon/pose/body epoch and be young.
    const bool liveRoute39=g_live178r35.load(std::memory_order_relaxed);const std::uint64_t maxAge39=handoff59c3::maximumContactAge; // bounded contact/admission rendezvous
    const bool originalOwnedH8=OriginalContactOwned19c3(p,now);
    if(!originalOwnedH8&&liveRoute39&&g_sync59c10r9.load(std::memory_order_acquire)&&
       !p.weaponItem&&!p.metadata59c7g.occurrenceSerialH28&&kcd2vr::solver_command_h28::empty(p.metadata59c7g.sourceSolverH28)){
        const kcd2vr::original_contact_h8::Source original{p.gesture59c3,p.key,p.generation,p.epoch,p.request,
            p.recovery55c1,p.sample,p.tick,reinterpret_cast<std::uintptr_t>(Rd<const void*>(henryActor,0x1F8,nullptr)),
            reinterpret_cast<std::uintptr_t>(he),p.metadata59c7g.collider};
        if(frik_pose_probe::WaitOriginalGeometryContactH8(original,p.release51,now)==kcd2vr::original_contact_h8::Decision::Wait){
            p.targetWait19c3=true; // resume through existing exact struck-body validation, never a current ray
            pendingLease.wait(p); // original occurrence/tick/queued/sample remain unchanged; no reservation/native entry
            if(Allow(4,8)){char line[280]{};std::snprintf(line,sizeof(line),
                "H8 GEOM_WAIT serial=%u gesture=%llu reason=ORIGINAL_CHECKED_CONTACT_SOURCE_QUARANTINE ageMs=%llu originalTick=%llu originalSample=%llu nativeEntry=NONE",
                p.serial,(unsigned long long)p.gesture59c3,(unsigned long long)(now-p.tick),
                (unsigned long long)p.tick,(unsigned long long)p.sample);diagnostic_trace::Write(line);}return;
        }
    }
    if(!originalOwnedH8){
        g_stats[5].fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4)){char m[220]{};std::snprintf(m,sizeof(m),"V66.178R30 GEOM_HIT serial=%u rejected=%s ageMs=%llu",p.serial,(p.tick&&now>=p.tick&&now-p.tick>maxAge39&&liveRoute39)?"CONTACT_EXPIRED(160ms since physical impact)":frik_pose_probe::GeometryOwnerStage49()==6?"STALE_OR_OWNER_CHANGED(stage 6: native body owns Henry - hit reaction/clip)":frik_pose_probe::GeometryOwnerStage49()==1?"STALE_OR_OWNER_CHANGED(stage 1: frame not ready)":frik_pose_probe::GeometryOwnerStage49()==2?"STALE_OR_OWNER_CHANGED(stage 2: pose not fresh)":frik_pose_probe::GeometryOwnerStage49()==8?"STALE_OR_OWNER_CHANGED(stage 8: weapon key/generation)":frik_pose_probe::GeometryOwnerStage49()==9?"STALE_OR_OWNER_CHANGED(stage 9: physical weapon not active/owned)":"STALE_OR_OWNER_CHANGED(other stage)",(unsigned long long)(now>=p.tick?now-p.tick:0));diagnostic_trace::Write(m);}return;}
    const void* gameRules5910c1=nullptr;
    kcd2vr::motion59c4::ClaimReservationC2 claimReservationC2;
    kcd2vr::motion59c4::ClaimReservationC2 hybridReservation54;
    kcd2vr::hybrid_serial59::RoutedReservation serialReservationC2;
    bool noRelease54=false;
    vrstate596::ContactScope5912 contactContext5912;
    bool vrDirect596=false; // R59.6: VR-owned attempt delivered through the direct native route (no ordinary action)
    const void* directTarget19c3=nullptr;
#include "../subsystems/direct/drain_admission.inl" // M2_OWNED_SOURCE
#include "../subsystems/hybrid/drain_rendezvous.inl" // M2_OWNED_SOURCE
    // R44C1: weapon/shield item contacts retain physical stop/haptics only.
    // Never substitute the current opponent for an unverified item holder.
    if(p.weaponItem){g_stats[5].fetch_add(1,std::memory_order_relaxed);g_drop_weapon_item_live56.fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,2)){char m[220]{};std::snprintf(m,sizeof(m),"C56 GEOM_SKIP serial=%u reason=CONTACT_ON_WEAPON_OR_SHIELD_ITEM route=LIVE entity=%u gesture=%llu release=%llu ageMs=%llu (physical stop/haptics only; the native clash is the game's)",
            p.serial,p.entityId,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
        return;}
#include "../subsystems/hybrid/drain_target_resolution.inl" // M2_OWNED_SOURCE
    // C65H1 TARGET_STAGE_END
#include "../subsystems/hybrid/drain_cost_claim.inl" // M2_OWNED_SOURCE
    alignas(16) unsigned char hitInfo[0x100]{};alignas(16) unsigned char details[0xE0]{};alignas(16) unsigned char meleeDetails[0x20]{};
    const auto initializeDetails59c3=[&]() noexcept -> bool {
    if(!CallCtor(g_base+kHitInfoCtor,hitInfo)||!CallCtor(g_base+kDetailsCtor,details)){g_stats[4].fetch_add(1,std::memory_order_relaxed);
        diagnostic_trace::Write("V66.178R27 GEOM_HIT_FAULT stage=CTOR (geometry hits disabled for this session)");g_enabled.store(false);return false;}
    std::memcpy(details+0x00,p.pos,sizeof(p.pos));           // contact point (collision producer: details+0 == hitInfo pos)
    if(p.surface>=0){const std::int32_t s=p.surface;std::memcpy(details+0x64,&s,4);} // runtime surface index (material effects row)
    // R59.16C10R4E (S37): the RPG locates the hit by details+0x68 = ORIGINAL material id of the struck body proxy (body_material2subpart:
    // 1 head_up, 3 head_front = the face -> equipment_part 'face', 6.. torso, 12-25 arms, 26-37 legs). When the ray witness carried no
    // idmatOrg (00:30 log: never), the native attack path's own approximation is used instead - combat_zone.approx_orig_mat_id per
    // attack zone (head 1, upper_left 24, upper_right 17, lower 6, lower_left 11, lower_right 10) - so the location is at least the
    // one a native swing into that zone would report, never 'unknown'. A real face hit still needs the witness material 3.
    int locationMaterial=p.bodyMaterial;
    if(locationMaterial<0){const int zone=hs?I(hs,kAttackZone):-1;
        locationMaterial=zone==0?1:zone==1?24:zone==2?17:zone==5?6:zone==3?11:zone==4?10:-1;}
    (void)kcd2vr::contact_location59c10::writeOriginalMaterial(details,locationMaterial);
    if(p.part>=0){const std::int32_t part=p.part;std::memcpy(details+0x70,&part,4);std::memcpy(details+0x80,&part,4);}
    // V66.178R35: the rest of the native collision details block, read from 40 HIT_TEMPLATE dumps of Henry's own
    // ordinary hits: +0x24 attacker entity id, +0x6C second surface index, +0x7C contact coefficient, +0x88 the
    // constant 0x00040012 of every ordinary hit, +0xC4 two set flag bytes, +0xC8 target actor, +0xD0 attacker actor.
    // V66.178R37 (R36 review F02): +0x24 is the STRUCK TARGET's entity id (D53610's repeat-target key; all 11 native
    // templates agree), +0xD0 the target's owner actor (D51C00: looked up by +0x24, stored at D0) and +0xC8 that
    // owner's +0x278 combat-actor link. R36 wrote Henry's id and Henry's actor there.
    {const std::uint32_t targetId=p.entityId;std::memcpy(details+0x24,&targetId,4);
     const void* owner=Rd<const void*>(target,0x1F8,nullptr);const void* ownerLink=owner?Rd<const void*>(owner,0x278,nullptr):nullptr;
     if(owner&&ownerLink==target)std::memcpy(details+0xD0,&owner,sizeof(owner));
     // +6C is the ATTACKING shape's surface (13F58BA..C2), not the
     // struck body's +64. Retain the constructor's unknown value when our
     // tracked contact has no independently qualified attacking surface.
     const float coef=std::clamp(p.energy,0.05f,1.f);std::memcpy(details+0x7C,&coef,4);
     const std::uint32_t k88=0x00040012u;std::memcpy(details+0x88,&k88,4);
     details[0xC4]=1;details[0xC5]=1;std::memcpy(details+0xC8,&target,sizeof(target));}
        return true;
    };
#include "../subsystems/hybrid/drain_native_answer.inl" // M2_OWNED_SOURCE
#include "../subsystems/hybrid/drain_live_delivery.inl" // M2_OWNED_SOURCE
#include "../subsystems/masterstrike_combos/direct_lineage_owner.inl" // M2_OWNED_SOURCE
#include "../subsystems/masterstrike_combos/contact_window_plan.inl" // M2_OWNED_SOURCE
    const power59c8::Result power59c8=power59c8::Compute(p,now,riposteContactC2); // R59.16C9B: bound to the blocked attacker, consumed once (R16: VR INTENT; R17: every F4 press-1/3 direct hit)
    if(!initializeDetails59c3())return;
    struct Sample {std::int32_t part=-1;float coef=1.f;float energy=1.f;std::int32_t idx=-1;} sample{};
    sample.energy=std::clamp(power59c8.power,0.05f,1.f); // R59.16C8: was p.energy (0.05..1 of E_ref); the per-sample energy now follows the strike power
    alignas(16) unsigned char request[0x40]{};
    // C9DC1: no speculative request+1 write; D568B0 does not read it.
    {const std::int32_t hand=2;std::memcpy(request+0x8,&hand,4);std::memcpy(request+0xC,&hand,4);
     std::memcpy(request+0x10,&target,sizeof(target));void* det=details;std::memcpy(request+0x18,&det,sizeof(det));
     const Sample* b=&sample;const Sample* e=&sample+1;std::memcpy(request+0x20,&b,sizeof(b));std::memcpy(request+0x28,&e,sizeof(e));std::memcpy(request+0x30,&e,sizeof(e));}
    float strength=power59c8.power;bool post=false;bool ok=false; // R59.16C8: strike power (was the constant 1.0); C40 may raise it once below
    const int type=p.kind==1?g_type_thrust.load():g_type_cut.load();
    g_stats[1].fetch_add(1,std::memory_order_relaxed);{std::lock_guard<std::mutex> lock(g_mutex);g_last_submit=now;}
    const bool apply=g_apply178r30.load(std::memory_order_relaxed);
#include "../subsystems/direct/contact_counter_plan.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/contact_stagger_strength.inl" // M2_OWNED_SOURCE
    // V66.178R34: the hit is adjudicated against Henry's attack zone; make sure the committed telegraph is in place
    // for this very call (thrust -> zone 5, cut -> the stroke's zone when fresh, else the already written zone).
    // Complete transport preflight BEFORE either native apply or temporary
    // telegraph writes. A rejected receiver must not leave an attacking flag set.
    const void* reactionManager5910c1=Rd<const void*>(target,0x3A8,nullptr);
    if(apply&&(!g_outer178r33.load()||!OuterVerified178r33()||g_outer_fault178r33.load()||
        !reactionManager5910c1||Rd<const void*>(reactionManager5910c1,8,nullptr)!=target||
        !(gameRules5910c1=HitEventReceiver5910c1()))){g_drop_outer_unavailable56.fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,2)){char m[220]{};std::snprintf(m,sizeof(m),"C56 GEOM_SKIP serial=%u reason=OUTER_CONTINUATION_UNAVAILABLE route=%s target=%u outer=%d fault=%d reactionManager=%d (no apply without reaction/notification transport)",
            p.serial,vrDirect596?"DIRECT":"SYNC_DIRECT",p.entityId,g_outer178r33.load()?1:0,g_outer_fault178r33.load()?1:0,reactionManager5910c1?1:0);diagnostic_trace::Write(m);}
        return;}
#include "../subsystems/hybrid/contact_telegraph_plan.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/contact_strength_context.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/contact_counter_before.inl" // M2_OWNED_SOURCE
    const int contactOrigin59c7g=::kcd2vr::zone_d85::executedHenryZone(I(hs,kDefenseZone)); // D85 zone: executed zone, Henry frame (was +0x270 = input class); before afterHit restores the VR context
    // Snapshot the exact inputs D56814/D55790 read BEFORE native submission.
    // Post-hit phase changes alone cannot prove an NPC elected to block.
    const int victimPhaseC10=ts?I(ts,kActionPhaseProp39):-1;
#include "../subsystems/direct/contact_defense_observation.inl" // M2_OWNED_SOURCE
    const bool serialEntered59=serialReservationC2.tryEnter();
    bool survived=false;
    if(serialEntered59){
        frik_pose_probe::ObserveHitPartNamespace62({p.serial,p.sample,p.tick,p.key,p.generation,p.epoch,p.request,
            p.entityId,reinterpret_cast<std::uintptr_t>(target),reinterpret_cast<std::uintptr_t>(te),p.metadata59c7g.collider,p.part});
        if(vrDirect596)claimReservationC2.nativeCallStarted(); // no retry after unknown native side effects
        hybridReservation54.nativeCallStarted();
        survived=CallSubmit(rpg,hitInfo,&strength,&post,request,apply,type,meleeDetails,ok);}
#include "../subsystems/hybrid/contact_telegraph_completed.inl" // M2_OWNED_SOURCE
    if(!serialEntered59){g_drop_serial_claimed56.fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4))diagnostic_trace::Write("C59 GEOM_SKIP site=DIRECT_SUBMIT reason=RELEASE_REVOKED_BEFORE_NATIVE_ENTRY nativeCall=0 attempt=ROLLBACK_ON_EXIT");return;}
    if(!survived){g_stats[4].fetch_add(1,std::memory_order_relaxed);g_enabled.store(false);
        diagnostic_trace::Write("V66.178R27 GEOM_HIT_FAULT stage=SUBMIT (exception inside BuildAndSubmitMeleeHit; geometry hits disabled for this session)");return;}
    if(ok)g_stats[2].fetch_add(1,std::memory_order_relaxed);
#include "../subsystems/direct/contact_context_completed.inl" // M2_OWNED_SOURCE
#include "../subsystems/masterstrike_combos/direct_combo_flag.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/contact_result_observation.inl" // M2_OWNED_SOURCE
    // V66.178R33: the outer continuation (victim reaction action + OnHit listeners) after an APPLIED, accepted hit.
    bool contactContinued59c7g=false,counterMajorRequested=false;int contactDispatch59c7g=-1;int reaction23=-1;int kind40=-1; // C23b: CallOuter's reaction kind (receipt copy); C40: native hit type even without a reaction
    if(ok&&apply&&g_outer178r33.load(std::memory_order_relaxed)&&!g_outer_fault178r33.load(std::memory_order_relaxed)&&OuterVerified178r33()){
        bool reaction=false,alive=false,down=false,queued=false;int kind=-1,dispatched=0;
        if(stab40.why==kcd2vr::counter_stab40::Why::Qualified){
            InjectResult35 counterResult{};DecodeNativeHitResult59c7g(counterResult,hitInfo,meleeDetails,ok,post);
            // Read the actual native result; blocked/parried/dead or foreign-target
            // hits never request this counter reaction. No second hit is submitted.
            counterMajorRequested=kcd2vr::counter_stab40::majorReaction(stab40.why,p.kind,!p.weaponItem,apply,p.entityId,counterResult);
        }
        // NR1: this hit is already adjudicated (CallSubmit above); only a landed, unblocked one reacts to the blade's real spot.
        // R1: the zone is only borrowed inside CallOuter around its two reaction builds.
        // I10: landed = health OR (VrNpcReactsStaminaHits) stamina damage, any blocked flag refuses; the spot mapping is NR1's.
        const bool nr2Landed=!p.weaponItem&&kcd2vr::bodycombo59c7g::landedForReactionNR2(Rd<float>(hitInfo,0x30,0.f),Rd<float>(hitInfo,0x2C,0.f),
            Rd<std::uint8_t>(meleeDetails,1,0)!=0,Rd<std::uint8_t>(meleeDetails,3,0)!=0,kcd2vr::bodycombo59c7g::staminaHits.load(std::memory_order_relaxed));
        const int nr1Zone=!nr2Landed?-1:kcd2vr::bodycombo59c7g::reactionZoneForMaterial(p.bodyMaterial,p.kind==1);
        if(nr1Zone>=0&&!(Rd<float>(hitInfo,0x30,0.f)>0.f))kcd2vr::bodycombo59c7g::nr2Stamina.fetch_add(1,std::memory_order_relaxed);
        const int rc=CallOuter(henryActor,target,hitInfo,meleeDetails,gameRules5910c1,queued,reaction,kind,alive,down,dispatched,counterMajorRequested,nr1Zone);
        contactContinued59c7g=queued&&rc>=0;contactDispatch59c7g=dispatched;if(rc>=0&&reaction)reaction23=kind;kind40=rc>=0?kind:-1;
        if(queued)victim5916c3::Start(target,p.entityId,p.serial,std::uint64_t(GetTickCount64()),alive?1:0,Rd<std::uint8_t>(hitInfo,0x40,0)!=0);
        {char event[240]{};std::snprintf(event,sizeof(event),"R59.10C1 GAME_HIT_EVENT serial=%u target=%u queued=%d reaction=%d dispatch=%d nativeCombat=%d death=%d rc=%d",p.serial,p.entityId,queued?1:0,reaction?1:0,dispatched,alive?1:0,Rd<std::uint8_t>(hitInfo,0x40,0),rc);diagnostic_trace::Write(event);}
        if(rc<0){g_outer_fault178r33.store(true);g_enabled.store(false);diagnostic_trace::Write("R59.10C1 GEOM_OUTER_FAULT (native continuation failed; further direct hits disabled, no retry of applied hit)");}
        else if(Allow(4,20)){static const char* dn[]{"NOT_CREATED","ACCEPTED","REJECTED_BY_VICTIM_ACTIONS","PREDICATE_GATE","GROUP_GATE"};char m[360]{};
            // V66.178R36 (F05): 9D3884 is the victim's first state property getter (the inCombat predicate the native caller uses), not a health query.
            std::snprintf(m,sizeof(m),"V66.178R36 GEOM_OUTER serial=%u target=%u victimStateGate(9D3884)=%d downEvaluated=%d down=%d kind=%d reactionAction=%s dispatch=%s onHit=D542E0 death=%d (direct-route fallback: reaction via target+0x3A8 + 90C850 dispatch as C5235C does)",
            p.serial,p.entityId,alive?1:0,alive?1:0,down?1:0,kind,rc==0?(reaction?"CREATED":"NONE"):"SKIPPED",dn[dispatched>=0&&dispatched<=4?dispatched:0],Rd<std::uint8_t>(hitInfo,0x40,0));diagnostic_trace::Write(m);}
    }
#include "../subsystems/direct/contact_counter_completed.inl" // M2_OWNED_SOURCE
    InjectResult35 directResult59c7g{};
    DecodeNativeHitResult59c7g(directResult59c7g,hitInfo,meleeDetails,ok,post);
    directResult59c7g.dispatch=contactDispatch59c7g;
#include "../subsystems/masterstrike_combos/direct_outcome_observer.inl" // M2_OWNED_SOURCE
    // C61/C58: visual-only living impact follows an ACCEPTED native hit and answered continuation.
    // No additional damage, stamina, or hit request is ever made from this ledger.
    if(ok&&apply&&contactContinued59c7g&&te){
        impulse596::NoteAnsweredLiving58(p.entityId,p.part,p.sample,p.serial,reaction23,std::uint64_t(GetTickCount64()));
    }
    const auto completedNowC2=std::uint64_t(GetTickCount64());
    const auto completedReceiptC2=PublishContact59c7g(p,directResult59c7g,completedNowC2,te,contactOrigin59c7g,true,ok&&apply,contactContinued59c7g,victimPhaseC10,ts?I(ts,kActionPhaseProp39):-1,hs); // D85 zone: F3 ZONE row
    if(completedReceiptC2.accepted&&completedReceiptC2.fields&&completedReceiptC2.applied){
#include "../subsystems/masterstrike_combos/direct_lineage_completed.inl" // M2_OWNED_SOURCE
        power59c8::RecordDelivered(p,completedNowC2);
#include "../subsystems/masterstrike_combos/consume_perfect_block_window.inl" // M2_OWNED_SOURCE
#include "../subsystems/direct/contact_target_completed.inl" // M2_OWNED_SOURCE
    }
    // V66.178R29: the 23:22 log showed 9/9 hits accepted with damage computed while Henry's state said inCombat=0
    // (no native attack = the fight never starts, the NPC never reacts). Rung 2 now clicks the opponent lock once
    // (the game's own way into combat mode) when a hit lands out of combat; the pad transport does the press.
    // V66.178R31: off by default - out of combat R3 is toggle_crouch (00:25 log); engagement now goes through the native attack lifecycle (GEOM_ENGAGE_NATIVE).
    if(hs&&B(hs,kInCombat)==0&&frik_pose_probe::GeometryAutoLock178r29()){
        const auto last=g_lock_click_last178r29.load();
        if(!last||now-last>=2000){g_lock_click_last178r29.store(now);g_lock_click_until178r29.store(now+90);
            diagnostic_trace::Write("V66.178R29 GEOM_ENGAGE reason=HENRY_NOT_IN_COMBAT action=R3_OPPONENT_LOCK_CLICK(90ms via native pad) note=damage lines follow as GEOM_DAMAGE");}}
    const auto hitTarget=Rd<std::uint32_t>(hitInfo,0x28,0u);
    // V66.178R30 (F04): result flags come from S_MeleeHitDetails (the result buffer), not from the collision block
    // (whose first bytes are the contact point); feedback only for an APPLIED hit, never for an evaluation.
    if(apply&&ok)frik_pose_probe::NotifyNativeWeaponImpact136R2(Rd<float>(hitInfo,0x30,0.f),Rd<float>(hitInfo,0x2C,0.f),sample.energy,
        meleeDetails[1]!=0,meleeDetails[3]!=0,meleeDetails[7]!=0,hitTarget?hitTarget:p.entityId);
    if(Allow(4,20)){char h[400]{},d[256]{},s[400]{};HitInfoText(hitInfo,h,sizeof(h));DetailsText(meleeDetails,d,sizeof(d));if(hs)StateText(hs,s,sizeof(s));
        char m[1500]{};std::snprintf(m,sizeof(m),"V66.178R30 GEOM_HIT serial=%u entity=%u target=%u via=%s kind=%s type=%d apply=%d energy=%.2f speedTick=%llu pos=(%.2f,%.2f,%.2f) part=%d surface=%d returned=%d post=%d %s %s henry{%s} producer=MOD_DIRECT(BuildAndSubmitMeleeHit) shape=COLLISION_CALLER(apply=1)|EVALUATION(apply=0) clip=NONE rootMotion=NONE proof=GEOM_DAMAGE",
            p.serial,p.entityId,te?EntityId(te):0u,via,p.kind==1?"THRUST":"CUT",type,apply?1:0,sample.energy,(unsigned long long)p.tick,p.pos[0],p.pos[1],p.pos[2],p.part,p.surface,ok?1:0,post?1:0,d,h,s);
        diagnostic_trace::Write(m);}
}
void Summary() noexcept {
    unsigned queued=0;{std::lock_guard<std::mutex> lock(g_mutex);queued=unsigned(g_backlog59c6.count)+(g_has?1u:0u);}
    char m[640]{};std::snprintf(m,sizeof(m),"R59C6 GEOM_HIT_SUMMARY enabled=%d requests=%llu submitted=%llu returnedTrue=%llu targetMiss=%llu opponentFallback=%llu rejected=%llu faults=%llu ctorMismatch=%llu coalesced=%llu contactAgeCapMs=160 backlogAccepted=%llu queueFull=%llu prunedExpired=%llu pending=%u capacity=9 perAttemptClaim=ONE",
        Enabled()?1:0,(unsigned long long)g_stats[0].load(),(unsigned long long)g_stats[1].load(),(unsigned long long)g_stats[2].load(),(unsigned long long)g_stats[3].load(),
        (unsigned long long)g_stats[6].load(),(unsigned long long)g_stats[5].load(),(unsigned long long)g_stats[4].load(),(unsigned long long)g_stats[7].load(),(unsigned long long)g_coalesced59c3.load(),
        (unsigned long long)g_backlogQueued59c6.load(),(unsigned long long)g_backlogFull59c6.load(),
        (unsigned long long)g_backlogExpired59c6.load(),queued);
    diagnostic_trace::Write(m);
    {char d56[400]{};std::snprintf(d56,sizeof(d56),"C56 GEOM_DROP_SITES recoveryEpoch=%llu waitingExpired=%llu flag220=%llu serialClaimed=%llu hybridDuplicate=%llu weaponItemLive=%llu protectedPhase=%llu outerUnavailable=%llu (per-site drop counters, never reset; lines are per-site rate-capped)",
        (unsigned long long)g_drop_recovery56.load(),(unsigned long long)g_drop_waiting_expired56.load(),(unsigned long long)g_drop_flag220_56.load(),(unsigned long long)g_drop_serial_claimed56.load(),
        (unsigned long long)g_drop_duplicate56.load(),(unsigned long long)g_drop_weapon_item_live56.load(),(unsigned long long)g_drop_nonordinary_phase56.load(),(unsigned long long)g_drop_outer_unavailable56.load());diagnostic_trace::Write(d56);}
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,std::uint64_t(GetTickCount64()))){
      const auto& r=kcd2vr::recovery65h::stats;char m65h[720]{};
      std::snprintf(m65h,sizeof(m65h),"C65H1 RECOVERY_RETIRE_SUMMARY edges=%llu retiredContacts=%llu recoveryOnly=%llu expiredOnly=%llu both=%llu hadRelease=%llu sites{submitPrune=%llu drainPrune=%llu waiting=%llu drainEntry=%llu beforeLive=%llu} edgeSnapshotDrops=NONE nativeAnswer=NOT_SAMPLED counts=F3_CUMULATIVE_CONTACTS",
       (unsigned long long)r.edges.load(),(unsigned long long)r.retired.load(),(unsigned long long)r.recoveryOnly.load(),(unsigned long long)r.expiredOnly.load(),(unsigned long long)r.both.load(),(unsigned long long)r.hadRelease.load(),
       (unsigned long long)r.sites[0].load(),(unsigned long long)r.sites[1].load(),(unsigned long long)r.sites[2].load(),(unsigned long long)r.sites[3].load(),(unsigned long long)r.sites[4].load());diagnostic_trace::Write(m65h);
      std::snprintf(m65h,sizeof(m65h),"C65H1 HYBRID_TARGET_WAIT_SUMMARY enabled=%d waits=%llu resolvedAfterWait=%llu leaseMs=160",g_hybrid_wait65h.load()?1:0,(unsigned long long)g_hybrid_waits65h.load(),(unsigned long long)g_hybrid_resolved65h.load());diagnostic_trace::Write(m65h);
    }
    char r17[420]{};std::snprintf(r17,sizeof(r17),
        "R59.16C10R17 HIT_ROUTES intent=%d intentAdmittedHits=%llu intentLive=%llu intentDirect=%llu liveWaitMs=%u strengthLeases=%llu strengthConsumed=%llu comboHitFlags=%llu npcAnswersSeen=%llu vrHitsSkippedAsAnswered=%llu syncLive=%llu syncDirect=%llu",
        g_intent59c16.load()?1:0,(unsigned long long)g_intent_hits59c16.load(),(unsigned long long)g_intent_live_hits59c17.load(),(unsigned long long)g_intent_direct_hits59c17.load(),
        g_intent_live_ms59c17.load(),(unsigned long long)g_strength_leases59c17.load(),(unsigned long long)g_strength_consumed59c17.load(),(unsigned long long)g_combo_flags59c17.load(),
        (unsigned long long)g_answered_seen59c17.load(),(unsigned long long)g_answered_skips59c17.load(),(unsigned long long)g_sync_live59c10r9.load(),(unsigned long long)g_sync_direct59c10r9.load());
    diagnostic_trace::Write(r17);
}
}
