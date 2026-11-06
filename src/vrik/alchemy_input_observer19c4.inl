// Included by the existing exact minigame lifecycle module. This hook only
// copies the result of the game's already-executed IsUsableByHold query.
using AlchemyHold19c4=void*(__fastcall*)(void*,void*,unsigned);
AlchemyHold19c4 originalAlchemyHold19c4=nullptr;
std::atomic<bool> alchemyHoldReady19c4{false};
static std::uintptr_t AlchemyInputOwner19c4() noexcept {
    const auto b=base175.load(std::memory_order_acquire);
    if(!b||!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire))return 0;
    const auto instance=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire);
    std::uintptr_t vt=0,player=0,entity=0;unsigned char active=0;
    if(!instance||!AlchemyInputRead19c4(instance,vt)||vt!=b+0x3F60B08||
       !AlchemyInputRead19c4(instance+0x2C,active)||active!=1||!AlchemyInputRead19c4(instance+0x18,player)||
       !player||!AlchemyInputRead19c4(player,vt)||vt!=b+0x3AA4B98||!AlchemyInputRead19c4(player+0x38,entity)||!entity)return 0;
    return instance;
}
// D5AD24 (native Alchemy.OnUse) obtains its entity from Henry's current
// C_PlayerInteractor+F4, then reads that entity's alchemy_type. +45C on the
// alchemy object is only a resolver cache; it is not this selection.
struct AlchemySelection19c4r2 {std::uintptr_t player=0,interactor=0;std::uint32_t entity=0;};
static bool ReadAlchemySelection19c4r2(std::uintptr_t instance,AlchemySelection19c4r2& out) noexcept {
    out={};const auto b=base175.load(std::memory_order_acquire);std::uintptr_t vt=0,back=0,again=0;std::uint32_t id=0;
    if(!b||AlchemyInputOwner19c4()!=instance||!AlchemyInputRead19c4(instance+0x18,out.player)||
       !AlchemyInputRead19c4(out.player+0xB08,out.interactor)||!out.interactor||
       !AlchemyInputRead19c4(out.interactor,vt)||vt!=b+0x3A6A0C8||
       !AlchemyInputRead19c4(out.interactor+8,back)||back!=out.player||
       !AlchemyInputRead19c4(out.interactor+0xF4,out.entity)||
       !AlchemyInputRead19c4(instance+0x18,again)||again!=out.player||
       !AlchemyInputRead19c4(out.player+0xB08,again)||again!=out.interactor||
       !AlchemyInputRead19c4(out.interactor+0xF4,id)||id!=out.entity||
       !AlchemyInputRead19c4(out.interactor,vt)||vt!=b+0x3A6A0C8||
       !AlchemyInputRead19c4(out.interactor+8,back)||back!=out.player)return false;
    return true;
}
void* __fastcall NativeAlchemyHold19c4(void*self,void*out,unsigned type){
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto b=base175.load(std::memory_order_acquire);
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const bool candidate=alchemyHoldReady19c4.load(std::memory_order_acquire)&&caller==b+0x8D1E92&&type<=20&&
        AlchemyInputOwner19c4()==reinterpret_cast<std::uintptr_t>(self);
#ifdef KCD2VR_ALCHEMY_STAGE37
    AlchemySelection19c4r2 selectionBefore{},selectionAfter{};
    const bool selectionKnown=candidate&&kcd2vr::diagnostics::capture168::active(
        kcd2vr::diagnostics::capture168::Domain::Interaction,std::uint64_t(GetTickCount64()))&&
        ReadAlchemySelection19c4r2(reinterpret_cast<std::uintptr_t>(self),selectionBefore);
#endif
    auto* result=originalAlchemyHold19c4(self,out,type); // original exactly once, sret ownership unchanged
    if(!candidate||!alchemyHoldReady19c4.load(std::memory_order_acquire)||result!=out||
       lifetime!=witness175.lifetime175.load(std::memory_order_acquire)||
       AlchemyInputOwner19c4()!=reinterpret_cast<std::uintptr_t>(self))return result;
    // C38: a hint query runs INSIDE native candidate enumeration. Its stable
    // before/after selection may still be an intermediate zero or another
    // candidate, so it is diagnostic only. The completed native-frame pump is
    // the sole producer of the selection used by Grip/palm acquisition.
#ifdef KCD2VR_ALCHEMY_STAGE37
    const bool selectedStable=selectionKnown&&ReadAlchemySelection19c4r2(reinterpret_cast<std::uintptr_t>(self),selectionAfter)&&
        selectionBefore.player==selectionAfter.player&&selectionBefore.interactor==selectionAfter.interactor&&selectionBefore.entity==selectionAfter.entity;
#endif
    std::uintptr_t text=0;char first=0;std::uint32_t entity=0,again=0;
    if(out&&AlchemyInputRead19c4(reinterpret_cast<std::uintptr_t>(out),text)&&text&&AlchemyInputRead19c4(text,first)&&
       AlchemyInputRead19c4(reinterpret_cast<std::uintptr_t>(self)+0x150+4*type,entity)&&entity&&
       AlchemyInputRead19c4(reinterpret_cast<std::uintptr_t>(self)+0x150+4*type,again)&&again==entity&&
       lifetime==witness175.lifetime175.load(std::memory_order_acquire)&&
       AlchemyInputOwner19c4()==reinterpret_cast<std::uintptr_t>(self)){
        kcd2vr::alchemy_input19c4::observed[type].publish({reinterpret_cast<std::uintptr_t>(self),lifetime,
            std::uint64_t(GetTickCount64()),entity,type,first!=0});
#ifdef KCD2VR_ALCHEMY_STAGE37
        AlchemyStageReceipt37(reinterpret_cast<std::uintptr_t>(self),lifetime,type,entity,text,first!=0,selectedStable?selectionAfter.entity:0); // C37: F5 rung 1 edges only
#endif
    }
    return result;
}
