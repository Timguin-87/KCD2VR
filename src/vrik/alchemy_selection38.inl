// C38 completed-frame alchemy selection. Included beside the existing exact
// native hint observer; the frame-end pump calls this once after the original
// outer FrameNexus returns, never for a second eye. No new native hook/call.
static void AlchemySelectionNativeFrameEndImpl38() noexcept {
    namespace al=kcd2vr::alchemy_input19c4;
    // All these gates are copied state. Inactive/unavailable/opted-out alchemy
    // performs no native-memory reads and cannot retain acquisition authority.
    if(!alchemyHoldReady19c4.load(std::memory_order_acquire)||
       !witness175.ready.load(std::memory_order_acquire)||
       witness175.transitionDepth175.load(std::memory_order_acquire)||
       !trackedHands59c7f[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)){
        (void)al::selected.publish({});return;
    }
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire);
    if(!instance){(void)al::selected.publish({});return;}
    AlchemySelection19c4r2 selection{},again{};
    const bool known=ReadAlchemySelection19c4r2(instance,selection)&&
        ReadAlchemySelection19c4r2(instance,again)&&
        selection.player==again.player&&selection.interactor==again.interactor&&selection.entity==again.entity&&
        alchemyHoldReady19c4.load(std::memory_order_acquire)&&
        witness175.ready.load(std::memory_order_acquire)&&
        !witness175.transitionDepth175.load(std::memory_order_acquire)&&
        trackedHands59c7f[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)&&
        lifetime==witness175.lifetime175.load(std::memory_order_acquire)&&
        instance==witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire);
    // Zero is a real current "nothing selected" result. Do not keep the last
    // nonzero entity or extend its original freshness when this frame is empty.
    // R8b: also the native selected TYPE (instance+0x304, the C37 selectedType read) - the mounted bellows is selected
    // by type 18 with entity 0. read twice, 21 when unreadable/changed.
    std::uint32_t type38=21,typeAgain38=21;
    if(!known||!AlchemyInputRead19c4(instance+0x304,type38)||!AlchemyInputRead19c4(instance+0x304,typeAgain38)||
       type38!=typeAgain38||type38>21)type38=21;
    (void)al::selected.publish(known?al::Selection{instance,lifetime,
        std::uint64_t(GetTickCount64()),selection.entity,type38}:al::Selection{});
    // G1: bounded copies of the current table's registered entities. Geometry
    // comes only from the native candidate callbacks, never a scene scan.
    namespace craft=kcd2vr::subsystems::alchemy::bridge67;
    if(craft::enabled.load(std::memory_order_acquire)){
        craft::NativeTable t{};static std::uint64_t sequence67=0;
        t.instance=instance;t.lifetime=lifetime;t.tick=GetTickCount64();t.sequence=++sequence67;
        // A page-turn may temporarily disable the selector. That revokes new
        // admissions, not the lifetime of the existing native book/action.
        bool valid67=AlchemyInputOwner19c4()==instance&&AlchemyInputRead19c4(instance+0x18,t.player)&&t.player&&
            AlchemyInputRead19c4(t.player+0x38,t.playerEntity)&&t.playerEntity;
        for(unsigned i=0;i<craft::prox67::Capacity&&valid67;++i){
            t.refs[i].slot=int(i);valid67=AlchemyInputRead19c4(instance+0x150+4*i,t.refs[i].entity);
        }
        std::uintptr_t director67=0,first67=0,last67=0,back67=0;unsigned char started67=0;
        t.actionKnown=valid67&&AlchemyInputRead19c4(instance+0x78,director67)&&director67==base175.load()+0x3BFA5B0&&
            AlchemyInputRead19c4(instance+0xB0,first67)&&AlchemyInputRead19c4(instance+0xB8,last67)&&first67&&
            last67>=first67+16&&last67-first67<=256&&!((last67-first67)%8)&&AlchemyInputRead19c4(first67+8,t.action);
        if(t.actionKnown&&t.action)t.actionKnown=AlchemyInputRead19c4(t.action+0x58,back67)&&back67==instance&&
            AlchemyInputRead19c4(t.action+0x28,started67)&&started67==1&&AlchemyInputRead19c4(first67+8,back67)&&back67==t.action;
        // Exact native parent class, not a generic nonzero pointer/animation.
        // Needed to distinguish an accepted PlaceMortar from unrelated A work.
        std::uintptr_t class69=0,again69=0;
        t.placeMortar69=t.actionKnown&&t.action&&AlchemyInputRead19c4(t.action,class69)&&class69==base175.load()+0x3A5BB90&&
            AlchemyInputRead19c4(first67+8,again69)&&again69==t.action&&
            AlchemyInputRead19c4(t.action,again69)&&again69==class69;
        // D56: retain the retail Hold-A distillation path and authored fixture
        // animation. Same synchronous class/slot recheck as PlaceMortar above.
        t.nativeRetortD56=t.actionKnown&&t.action&&AlchemyInputRead19c4(t.action,class69)&&class69==base175.load()+0x3F60410&&
            AlchemyInputRead19c4(first67+8,again69)&&again69==t.action&&
            AlchemyInputRead19c4(t.action,again69)&&again69==class69;
        // G1R1: bellows remains mounted. Its real native LEFT membership18 is
        // published by6C4B86 and cleared to21 by17BAE8B; no fake attachment.
        t.handTypesKnown68=valid67;
        for(unsigned side=0;side<2&&t.handTypesKnown68;++side)
            t.handTypesKnown68=AlchemyInputRead19c4(instance+0x2A0+4*side,t.handType68[side])&&t.handType68[side]<=21;
        for(unsigned side=0;side<2&&t.handTypesKnown68;++side){unsigned againType68=21;
            t.handTypesKnown68=AlchemyInputRead19c4(instance+0x2A0+4*side,againType68)&&againType68==t.handType68[side];}
        valid67=valid67&&AlchemyInputOwner19c4()==instance&&lifetime==witness175.lifetime175.load(std::memory_order_acquire);
        craft::finish(valid67?t:craft::NativeTable{});
    }
}
