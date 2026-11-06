// Included after geometry_hit178r27.inl and the common hook installer.
// No general AI override. The lead substitution is scoped to our synchronous
// phase4 announcement and exact native offer callsite. Native RPG min/angle/
// eligibility and native trigger activation remain unchanged.
namespace defense_offer19c4 {
using LeadFn=float(__fastcall*)(const void*,const void*);
constexpr std::uintptr_t kLead=0xA73A04,kLeadReturn=0x9142E4,kOfferPost=0x91432E;
constexpr unsigned char kLeadEntry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xCA,0x48,0x8B,0xDA,0xE8,0x63,0x05,0x00};
constexpr unsigned char kLeadCall[]{0xE8,0x20,0xF7,0x15,0x00,0xF3,0x0F,0x5C,0xF0,0x48,0x8D,0x55,0x30,0x48,0x8D,0x4D,0x40,0xF3,0x0F,0x11,0x75,0x40};
constexpr unsigned char kOfferPostEntry[]{0xBA,0x08,0x00,0x00,0x00,0x48,0x8B,0xCF,0xE8,0x21,0x02,0x00,0x00};
constexpr unsigned char kIntentProducer[]{0x48,0x8B,0x47,0x70,0x8A,0x57,0x60,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0xA0,0x05,0x00,0x00,0xE8,0xD4,0x85,0xC0,0xFF,0x48,0x8B,0x47,0x70,0xBE,0x04,0x00,0x00,0x00,0x8B,0xD6,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x83,0xC1,0x30,0xE8,0x4D,0x2C,0xE1,0xFF};
constexpr unsigned char kIntentDefault[]{0xC7,0x47,0x60,0x01,0x01,0x00,0x00};
constexpr unsigned char kIntentReader[]{0x48,0x8D,0x88,0x90,0x03,0x00,0x00,0x48,0x8B,0x11,0xFF,0x52,0x08,0x84,0xC0,0x75,0x18,0x48,0x8B,0x8F,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0xA0,0x05,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x08,0x84,0xC0,0x74,0x0A,0x33,0xD2,0x48,0x8B,0xCB,0xE8,0x2D,0x02,0x00,0x00};
constexpr unsigned char kReachCtor[]{0x48,0x89,0xAF,0xB0,0x04,0x00,0x00};
constexpr unsigned char kExtendedCtor[]{0x48,0x89,0xAF,0xE0,0x04,0x00,0x00};
constexpr unsigned char kIntentCtor[]{0x48,0x89,0xAF,0xA0,0x05,0x00,0x00};
constexpr unsigned char kZoneSchema[]{0x48,0x8D,0x0D,0x00,0xE5,0xD9,0x03,0x0F,0x57,0xC0,0x0F,0x11,0x40,0x28,0x48,0x89,0x38,0x89,0x58,0x08,0xC7,0x40,0x0C,0xD4,0x00,0x00,0x00,0xC7,0x40,0x10,0x04,0x00,0x00,0x00,0xC7,0x40,0x14,0x04,0x00,0x00,0x00,0x48,0x89,0x48,0x18,0xC6,0x40,0x24};
constexpr unsigned char kZoneParse[]{0x41,0x8B,0x8E,0xD4,0x00,0x00,0x00,0xE8,0x43,0x19,0x00,0x00,0x49,0x89,0x87,0x38,0x01,0x00,0x00};
constexpr unsigned char kZoneNativePublish[]{0x48,0x8B,0x83,0x38,0x01,0x00,0x00,0x48,0x8D,0x8E,0x50,0x01,0x00,0x00,0x8B,0x68,0x08,0x8B,0xD5,0xE8,0x97,0x38,0xFD,0xFF};
constexpr unsigned char kOfferRpgClamp[]{0x05,0x0F,0x28,0xCE,0xEB,0x6F,0x49,0x8B,0xD6,0x48,0x8B,0xCF,0xE8,0x9F,0x36,0x21,0x00,0xF3,0x0F,0x11,0x45,0x48,0x49,0x8B,0x8E,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0x50,0x04,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x08,0x49,0x8B,0xD6,0xC7,0x45,0x30,0x00,0x00,0x00,0x00,0x0F,0x28,0xF0,0xE8,0x20,0xF7,0x15,0x00,0xF3,0x0F,0x5C,0xF0,0x48,0x8D,0x55,0x30,0x48,0x8D,0x4D,0x40,0xF3,0x0F,0x11,0x75,0x40,0xE8,0xD6,0x9C,0xC2,0xFF,0x48,0x8D,0x55,0x48,0x48,0x8D,0x4D,0xD0,0xF3,0x0F,0x10,0x08,0xF3,0x0F,0x11,0x4D,0xD0,0xE8,0xD0,0x9C,0xC2,0xFF,0xF3,0x0F,0x10,0x08,0x0F,0x2F,0x0D,0x31,0x5C,0x78,0x03,0x72,0x08,0x48,0x8B,0xCB,0xE8,0x2B,0xB1,0xAB,0x01,0x48,0x8B,0x03,0x48,0x8B,0xCB,0xFF,0x50,0x60};
static std::optional<DetourModKit::hook::Hook> g_lead_hook,g_post_hook;
static LeadFn g_lead_original=nullptr;
static std::atomic<bool> g_pending{false};
static bool g_lead_retired19c4=false; // control plane only; retired trampoline stays process-live
namespace vs=geom27::vrstate596;
static bool Current(vs::DefenseIntentScope19c4* scope,const void* attacker) noexcept {
    if(!scope||!scope->qualified||g_pending.load()||!vs::g_defense_intent_hooks19c4.load()||
       !kcd2vr::motion59c4::selected.load()||vs::g_fault.load()||vs::g_reset.load()||
       attacker!=reinterpret_cast<const void*>(scope->c.actor)||attacker!=vs::g_henry_actor.load()||
       vs::g_owner_state!=reinterpret_cast<const void*>(scope->c.state)||
       vs::t_own_phase_write19.state!=vs::g_owner_state||
       !kcd2vr::motion59c4::same(scope->c.key,vs::g_key5912)||
       !kcd2vr::motion59c4::same(scope->c.key,vs::g_prediction19c3.key)||
       scope->c.key.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       geom27::LiveAttackAction178r35(attacker)||frik_pose_probe::GeometryOwnerStage49()==6||
       !frik_pose_probe::GeometryOwnerValid178r30(scope->c.key.weapon,scope->c.key.generation,scope->c.key.body,scope->c.key.request))return false;
    const auto now=std::uint64_t(GetTickCount64());
    return now>=scope->c.now&&now<scope->c.deadline&&
       I(vs::g_owner_state,geom27::kActionPhaseProp39)==4&&
       I(vs::g_owner_state,kDefenseZone)==scope->c.defense&&
       F(vs::g_owner_state,vs::kHitRemaining)==scope->c.remaining&&
       vs::di19c4::SameThreat(vs::DefenseIntentRead19c4,g_base,scope->c)&&
       vs::di19c4::Bool(vs::DefenseIntentRead19c4,g_base,scope->c.state,0x5A0)==1;
}
static float LeadResult(vs::DefenseIntentScope19c4* scope,const void* attacker,std::uintptr_t caller,float native) noexcept {
    if(caller!=g_base+kLeadReturn||!std::isfinite(native)||native<0.f||!Current(scope,attacker))return native;
    // The VR prediction already means time remaining to tracked contact.
    // Subtracting the authored automatic-action lead (.15s) from a .14s VR
    // prediction gives an immediately expired offer. Keep native min(RPG,
    // remaining) below this call and replace only that obsolete motion lead.
    scope->authoredLead=native;scope->offsetAdapted=true;return 0.f;
}
static float __fastcall LeadDetour(const void* unused,const void* attacker) noexcept {
    const auto original=g_lead_original;if(!original)return 0.f;
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const float native=original(unused,attacker); // exactly once, including all rejected/foreign calls
    return LeadResult(vs::t_defense_intent19c4,attacker,caller,native);
}
static bool CaptureOffer(vs::DefenseIntentScope19c4* scope,const void* attacker,const void* trigger,const void* handler,float allowance) noexcept {
    if(!Current(scope,attacker)||Rd<std::uintptr_t>(trigger,0,0)!=g_base+0x3B813F8||
       Rd<const void*>(trigger,0x38,nullptr)!=attacker||Rd<const void*>(trigger,8,nullptr)!=reinterpret_cast<const void*>(scope->c.target)||
       Rd<const void*>(handler,8,nullptr)!=reinterpret_cast<const void*>(scope->c.target))return false;
    scope->trigger=trigger;scope->offerObserved=true;
    scope->active=Rd<std::uint8_t>(trigger,0x19,255);scope->queued=Rd<std::uint8_t>(trigger,0x18,255);
    scope->timed=Rd<std::uint8_t>(trigger,0x1B,255);scope->offerDuration=Rd<float>(trigger,0x1C,-1.f);
    // [rbp+48] is initialized only on the normal A73A04 path. The positive
    // payload-duration fast path skips that store; never report its stack junk.
    scope->allowanceKnown=scope->offsetAdapted&&std::isfinite(allowance)&&allowance>=0.f;
    scope->nativeAllowance=scope->allowanceKnown?allowance:-1.f;return true;
}
static float ReadAllowance19c4(const vs::DefenseIntentScope19c4* scope,const void* nativeFrame) noexcept {
    return scope&&scope->offsetAdapted?Rd<float>(nativeFrame,0x48,-1.f):-1.f;
}
static void PostOffer(DetourModKit::hook::MidContext& ctx) noexcept {
    auto* scope=vs::t_defense_intent19c4;if(!scope||!scope->qualified)return;
    using namespace DetourModKit::hook;
    (void)CaptureOffer(scope,reinterpret_cast<const void*>(gpr(ctx,Gpr::R14)),reinterpret_cast<const void*>(gpr(ctx,Gpr::Rbx)),
        reinterpret_cast<const void*>(gpr(ctx,Gpr::Rdi)),ReadAllowance19c4(scope,reinterpret_cast<const void*>(gpr(ctx,Gpr::Rbp))));
}

template<std::size_t N> static bool Pin(std::uintptr_t rva,const unsigned char(&want)[N]) noexcept {
    unsigned char bytes[N]{};return SehCopy146(reinterpret_cast<const void*>(g_base+rva),bytes,N)&&!std::memcmp(bytes,want,N);
}
static bool Drop(std::optional<DetourModKit::hook::Hook>& h) noexcept {
    if(!h)return true;if(h->is_enabled()&&!h->disable())return false;
    if(h->is_enabled())return false;h.reset();return true;
}
template<class Holder> static bool RetireInline19c4(Holder& h,bool& retired) noexcept {
    if(!h)return true;if(h->is_enabled()&&!h->disable())return false;
    if(h->is_enabled())return false;
    // DMK documents no INLINE rundown for original<Fn>(). Disable restores the
    // prologue, but an entered detour may still call its saved trampoline.
    // release() retains backend/ledger for process lifetime, even disabled;
    // only the disengaged handle is reset. Never clear the original pointer.
    h->release();h.reset();retired=true;return true;
}
static void Uninstall() noexcept {
    g_pending.store(true);vs::g_defense_intent_hooks19c4.store(false,std::memory_order_release);
    const bool wasRetired=g_lead_retired19c4;
    const bool post=Drop(g_post_hook),lead=RetireInline19c4(g_lead_hook,g_lead_retired19c4);
    g_pending.store(!(post&&lead));
    if(!wasRetired&&g_lead_retired19c4)diagnostic_trace::Write("C19C4 DEFENSE_INTENT ready=0 reason=INLINE_RETIRED trampoline=PROCESS_LIFETIME_RETAINED reinstall=DISABLED nativeOriginal=RETAINED");
}
static bool Install() noexcept {
    if(g_pending.load()||g_lead_retired19c4)return false;if(vs::g_defense_intent_hooks19c4.load())return true;
    if(g_lead_hook||g_post_hook){Uninstall();return false;}
    if(!retail_build112::Ready(g_base)||!Pin(kLead,kLeadEntry)||!Pin(0x9142DF,kLeadCall)||!Pin(kOfferPost,kOfferPostEntry)||
       !Pin(0xB27446,kIntentProducer)||!Pin(0x915867,kIntentDefault)||!Pin(0x909C08,kIntentReader)||
       !Pin(0x10F15AB,kReachCtor)||!Pin(0x10F15D7,kExtendedCtor)||!Pin(0x10F168E,kIntentCtor)||
       !Pin(0xB0BF1,kZoneSchema)||!Pin(0x932641,kZoneParse)||!Pin(0x939E21,kZoneNativePublish)||!Pin(0x9142A8,kOfferRpgClamp)||
       !Pin(vs::kDefenseBoolGetter,vs::kDefenseBoolGetterEntry)||
       Rd<std::uintptr_t>(reinterpret_cast<const void*>(g_base+vs::kDefenseBoolVft),8,0)!=g_base+vs::kDefenseBoolGetter)return false;
    if(!Hook(g_lead_hook,g_lead_original,kLead,kLeadEntry,&LeadDetour,"kcd2vr_c19c4_vr_defense_lead")){Uninstall();return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_c19c4_defense_offer_receipt",Target{DetourModKit::Address{g_base+kOfferPost}},options},PostOffer);
    if(!h){Uninstall();return false;}g_post_hook=std::move(*h);
    if(!g_post_hook->enable()){Uninstall();return false;}
    vs::g_defense_intent_hooks19c4.store(true,std::memory_order_release);return true;
}
}
