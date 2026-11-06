// C37: exact-retail pins for the smithing tool reader (vrik/smith_tool36.h).
// Read-only byte/vtable comparisons, once per module base. No hook, no call.
static bool SmithToolPins36() noexcept {
    const auto base=WhBase();if(!retail_build112::Ready(base))return false;
    static std::atomic<std::uintptr_t> checked{0};static std::atomic<bool> result{false};
    if(checked.load(std::memory_order_acquire)==base)return result.load(std::memory_order_relaxed);
    const auto match=[](std::uintptr_t at,const auto& bytes){for(unsigned i=0;i<sizeof(bytes);++i){unsigned char b=0;if(!Read(reinterpret_cast<void*>(at+i),b)||b!=bytes[i])return false;}return true;};
    const auto callTo=[](std::uintptr_t at,std::uintptr_t target){unsigned char op=0;std::int32_t rel=0;
        return Read(reinterpret_cast<void*>(at),op)&&op==0xE8&&Read(reinterpret_cast<void*>(at+1),rel)&&at+5+std::intptr_t(rel)==target;};
    const auto slot=[](std::uintptr_t vft,unsigned index,std::uintptr_t target){std::uintptr_t fn=0;return Read(reinterpret_cast<void*>(vft+8*index),fn)&&fn==target;};
    // B674A4 smithing start: tongs (hand 1) -> model+2E8, hammer (hand 0) -> model+2E0.
    constexpr unsigned char tongsHand[]{0xc7,0x45,0x20,0x01,0x00,0x00,0x00};
    constexpr unsigned char tongsSlot[]{0x48,0x8d,0x88,0xe8,0x02,0x00,0x00};
    constexpr unsigned char hammerHand[]{0xc7,0x45,0x20,0x00,0x00,0x00,0x00};
    constexpr unsigned char hammerSlot[]{0x48,0x8d,0x88,0xe0,0x02,0x00,0x00};
    // 857ECA/857EF9: the spawned workpiece holder is hand 1 and lives at model+2F8.
    constexpr unsigned char workpieceHand[]{0xc7,0x45,0x77,0x01,0x00,0x00,0x00};
    constexpr unsigned char workpieceSlot[]{0x48,0x89,0x90,0xf8,0x02,0x00,0x00};
    // 8589E8 (C_Blacksmithing+70 anim event listener): model slot -> holder vft+8(player=[this-58]).
    constexpr unsigned char attachHammer[]{0x48,0x83,0xc1,0xf8,0x48,0x8b,0x01,0xff,0x50,0x60,0x48,0x8b,0x88,0xe0,0x02,0x00,0x00};
    constexpr unsigned char attachTongs[]{0x48,0x83,0xc1,0xf8,0x48,0x8b,0x01,0xff,0x50,0x60,0x48,0x8b,0x88,0xe8,0x02,0x00,0x00};
    constexpr unsigned char attachWorkpiece[]{0x48,0x83,0xc1,0xf8,0x48,0x8b,0x01,0xff,0x50,0x60,0x48,0x8b,0x88,0xf8,0x02,0x00,0x00};
    constexpr unsigned char attachTail[]{0x48,0x8b,0x01,0x48,0x8b,0x53,0xa8,0x48,0x83,0xc4,0x20,0x5b,0x48,0xff,0x60,0x08};
    // 193F148 slot-tool attach: once (+14), desc WUID=+28, hand=+8, host=player+30,
    // manager=player vft+5F8 (9D3AB0: [player+9D0]), 8CF6FC, then +14=1.
    constexpr unsigned char slotOnce[]{0x80,0x79,0x14,0x00};
    constexpr unsigned char slotKey[]{0x48,0x8b,0x43,0x28};
    constexpr unsigned char slotHand[]{0x8b,0x43,0x08};
    constexpr unsigned char slotHost[]{0x8b,0x47,0x30,0x89,0x43,0x0c};
    constexpr unsigned char slotManager[]{0xff,0x90,0xf8,0x05,0x00,0x00};
    constexpr unsigned char slotAttached[]{0xc6,0x43,0x14,0x01};
    constexpr unsigned char playerManager[]{0x48,0x8b,0x81,0xd0,0x09,0x00,0x00,0xc3};
    // B67CBC slot-tool detach clears +C and +14 (released membership).
    constexpr unsigned char slotDetach[]{0xc7,0x43,0x0c,0x00,0x00,0x00,0x00,0xc6,0x43,0x14,0x00};
    // B67A4C: holder+28 = native item key of the linked tool entity.
    constexpr unsigned char slotKeySource[]{0x48,0x8b,0x40,0x30,0x48,0x89,0x46,0x28};
    // 8CF6FC desc[0] key / desc+8 hand -> AttachOnly 8D0404 -> 8D06F0 -> 8D0758
    // which writes [player+990]+258+8+8*hand: the hand-slot key read by NativeHandKey6697.
    constexpr unsigned char pickupKey[]{0x48,0x8b,0x12};
    constexpr unsigned char pickupHand[]{0x44,0x8b,0x43,0x08};
    constexpr unsigned char handSlots[]{0x48,0x8b,0xb0,0x90,0x09,0x00,0x00,0x48,0x81,0xc6,0x58,0x02,0x00,0x00};
    constexpr unsigned char handWrite[]{0x4a,0x89,0x5c,0xf6,0x08};
    // B66624 workpiece attach: host=player+30 on the direct entity_X route, +20 Assetslot, +14 attached.
    constexpr unsigned char workpieceHost[]{0x8b,0x46,0x30,0x48,0x8b,0xce,0x89,0x43,0x0c};
    constexpr unsigned char workpieceAsset[]{0xc6,0x43,0x20,0x01};
    constexpr unsigned char workpieceAttached[]{0xc6,0x43,0x14,0x01};
    // G1 exact no-call membership read for inventory tool representations.
    // 23D09E0 adds registry+168 and calls 1E3E090; its entire bounded getter
    // tests key type/index/generation and the complete C_Item key. 82D014
    // returns the current linked entity ID. No guessed vtable call is made.
    constexpr unsigned char itemRegistrySource[]{0x48,0x8b,0x0d,0x0d,0x76,0xbb,0x04};
    constexpr unsigned char itemLookupOwner[]{0x48,0x81,0xc1,0x68,0x01,0x00,0x00};
    constexpr unsigned char itemRegistryGetter[]{0x4c,0x8b,0x02,0x4c,0x8b,0xc9,0x49,0x8b,0xc0,0x48,0xc1,0xe8,0x38,0x3c,0x02,0x75,0x38,0x41,0x8b,0xd0,0x41,0x8b,0xc0,0xc1,0xea,0x12,0x25,0xff,0xff,0x03,0x00,0x74,0x28,0x3d,0x90,0xd0,0x03,0x00,0x77,0x21,0x48,0x8d,0x0c,0x40,0x66,0x41,0x39,0x54,0xc9,0x28,0x75,0x15,0x49,0x8b,0x44,0xc9,0x38,0x48,0x85,0xc0,0x74,0x0b,0x33,0xc9,0x4c,0x39,0x40,0x30,0x48,0x0f,0x45,0xc1,0xc3,0x33,0xc0,0xc3};
    constexpr unsigned char itemIdGetter[]{0x48,0x8b,0x91,0x88,0x00,0x00,0x00,0x33,0xc0,0x48,0x85,0xd2,0x75,0x01,0xc3,0x8b,0x42,0x30,0xc3};
    const bool qualified=match(base+0xB675A3,tongsHand)&&callTo(base+0xB675B4,base+0xB67820)&&match(base+0xB675CB,tongsSlot)&&
        match(base+0xB675E4,hammerHand)&&callTo(base+0xB675F2,base+0xB678C8)&&match(base+0xB67609,hammerSlot)&&
        match(base+0x857ECA,workpieceHand)&&callTo(base+0x857ED1,base+0xB66818)&&match(base+0x857EF9,workpieceSlot)&&
        slot(base+0x3A6A2C8,1,base+0x8589E8)&&slot(base+0x3A6A320,12,base+0x72DD20)&&
        match(base+0x858AB6,attachHammer)&&match(base+0x858AE6,attachTongs)&&match(base+0x858B1A,attachWorkpiece)&&match(base+0x858AF7,attachTail)&&
        slot(base+0x3A7BA48,1,base+0x193F148)&&slot(base+0x3A7BA48,2,base+0xB67CBC)&&
        slot(base+0x3A7BA18,1,base+0xB66624)&&slot(base+0x3A7BA18,2,base+0xB6654C)&&
        match(base+0x193F167,slotOnce)&&match(base+0x193F17D,slotKey)&&match(base+0x193F193,slotHand)&&match(base+0x193F19A,slotHost)&&
        match(base+0x193F1BA,slotManager)&&callTo(base+0x193F1CB,base+0x8CF6FC)&&match(base+0x193F1D8,slotAttached)&&
        slot(base+0x3AA4B98,0xBF,base+0x9D3AB0)&&match(base+0x9D3AB0,playerManager)&&
        match(base+0xB67D7E,slotDetach)&&match(base+0xB67AAC,slotKeySource)&&
        match(base+0x8CF714,pickupKey)&&match(base+0x8CF747,pickupHand)&&callTo(base+0x8CF899,base+0x8D0404)&&
        callTo(base+0x8D0488,base+0x8D06F0)&&callTo(base+0x8D0729,base+0x8D0758)&&match(base+0x8D077B,handSlots)&&match(base+0x8D07A5,handWrite)&&
        match(base+0xB6675C,workpieceHost)&&match(base+0xB66756,workpieceAsset)&&match(base+0xB66792,workpieceAttached)&&
        match(base+0x8D0464,itemRegistrySource)&&match(base+0x23D09E9,itemLookupOwner)&&callTo(base+0x23D09F5,base+0x1E3E090)&&
        match(base+0x1E3E090,itemRegistryGetter)&&match(base+0x82D014,itemIdGetter);
    result.store(qualified,std::memory_order_relaxed);checked.store(base,std::memory_order_release);return qualified;
}
// Called only from the existing native attachment / pose transaction, like
// ReadAlchemyTool18. Phase comes from the exact smith listener observer for the
// current minigame epoch; an unknown phase never binds (fail closed).
static bool ReadSmithTool36(void* attachment,std::uint32_t entity,std::uintptr_t character,
                            kcd2vr::smith_tool36::Binding& out,
                            kcd2vr::smith_tool36::ReadEvidenceI1* evidence=nullptr) noexcept {
    using namespace kcd2vr::activity_input163;
    out={};if(evidence){*evidence={};evidence->boundEntity=entity;}
    const auto refuse=[&](const char* reason){if(evidence)evidence->refusal=reason;return false;};
    if(!SmithToolPins36())return refuse("SMITH_RETAIL_PINS_UNAVAILABLE");
    if(!witness175.ready.load(std::memory_order_acquire))return refuse("SMITH_LIFECYCLE_UNAVAILABLE");
    if(witness175.transitionDepth175.load(std::memory_order_acquire))return refuse("SMITH_LIFECYCLE_TRANSITION");
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    const int phase=witness175.SmithPhaseC1();
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);void* player=nullptr;
    if(!instance)return refuse("SMITH_MINIGAME_INACTIVE");
    if(phase<0)return refuse("UNKNOWN_NATIVE_PHASE");
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !PhysVft6698(player,0x3AA4B98))return refuse("SMITH_ITEM_MANAGER_OR_PLAYER");
    void* back=nullptr;if(!Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager)return refuse("SMITH_ITEM_MANAGER_OWNER_MISMATCH");
    const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
    if(!kcd2vr::smith_tool36::read(rd,WhBase(),instance,reinterpret_cast<std::uintptr_t>(player),
        character,reinterpret_cast<std::uintptr_t>(attachment),entity,phase,out,evidence))return false;
    if(!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
       lifetime!=witness175.lifetime175.load(std::memory_order_acquire)||
       instance!=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)||
       phase!=witness175.SmithPhaseC1()||manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)){
        out={};return refuse("SMITH_LIFECYCLE_OR_MANAGER_CHANGED_DURING_READ");}
    out.lifetime=lifetime;return true;
}
