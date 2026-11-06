// Read-only receipts on the same synchronous tracked-contact stack. Arguments,
// return values, reference counts and action-manager decisions remain native.
constexpr std::uintptr_t kReactionBuild50=0xD499D8ull,kReactionDispatch50=0x90C850ull;
constexpr unsigned char kReactionBuildEntry50[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x41,0x56};
constexpr unsigned char kReactionDispatchEntry50[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x99,0x00,0x02,0x00,0x00};
using ReactionBuildFn50=void*(__fastcall*)(const void*,void*,const void*,int,const void*);
using ReactionDispatchFn50=bool(__fastcall*)(const void*,void*);
ReactionBuildFn50 g_reaction_build_original50=nullptr;
ReactionDispatchFn50 g_reaction_dispatch_original50=nullptr;
void* __fastcall ReactionBuildDetour50(const void* receiver,void* out,const void* target,int kind,const void* hitInfo) noexcept {
    const auto* scope=kcd2vr::geometry40c1::Scope::current;
    const bool qualified=scope&&scope->target==target&&scope->actor==Rd<const void*>(receiver,8,nullptr);
    // NR1: a landed VR contact (damage already applied) reacts to where the blade hit, not to our published zone.
    const void* nr1Hs=nullptr;int nr1Restore=-1,nr1Written=-1;
    // I10: landed = health or (VrNpcReactsStaminaHits) stamina damage; blocked flags of OUR details refuse it.
    const bool nr2Landed=qualified&&scope->reactionZone>=0&&kcd2vr::bodycombo59c7g::landedForReactionNR2(Rd<float>(hitInfo,0x30,0.f),Rd<float>(hitInfo,0x2C,0.f),
        scope->details&&Rd<std::uint8_t>(scope->details,1,0)!=0,scope->details&&Rd<std::uint8_t>(scope->details,3,0)!=0,
        kcd2vr::bodycombo59c7g::staminaHits.load(std::memory_order_relaxed));
    if(nr2Landed&&!(Rd<float>(hitInfo,0x30,0.f)>0.f))kcd2vr::bodycombo59c7g::nr2Stamina.fetch_add(1,std::memory_order_relaxed);
    const bool nr1=nr2Landed&&
        geom27::vrstate596::ReactionZoneBeginNR1(scope->actor,scope->reactionZone,nr1Hs,nr1Restore,nr1Written,target);
    void* result=nullptr;
    // R1: restore on every exit of the original, including a native exception (it still propagates).
    __try{result=g_reaction_build_original50?g_reaction_build_original50(receiver,out,target,kind,hitInfo):nullptr;}
    __finally{if(nr1)geom27::vrstate596::ReactionZoneEndNR1(scope->actor,nr1Hs,nr1Restore,nr1Written);}
    if(qualified)geom27::NoteInjectedReaction50(out,kind);
    return result;
}
bool __fastcall ReactionDispatchDetour50(const void* target,void* reaction) noexcept {
    const auto* scope=kcd2vr::geometry40c1::Scope::current;
    const bool qualified=scope&&scope->target==target;
    const bool result=g_reaction_dispatch_original50?g_reaction_dispatch_original50(target,reaction):false;
    if(qualified)geom27::NoteInjectedDispatch50(result);
    return result;
}
