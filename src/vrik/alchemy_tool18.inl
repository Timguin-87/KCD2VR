// Called only from the existing native attachment / pose transaction. No new
// worker polling, native call, action admission, physicalization or inventory use.
static bool AlchemyToolPins18() noexcept {
    const auto base=WhBase();if(!retail_build112::Ready(base))return false;
    static std::atomic<std::uintptr_t> checked{0};static std::atomic<bool> result{false};
    if(checked.load(std::memory_order_acquire)==base)return result.load(std::memory_order_relaxed);
    // 2E1959C: action+58 owner, owner+304 selected type, owner+150[type]
    // entity, action+50 hand; attach via entity_right/left, then 6C677C.
    constexpr unsigned char attach[]{0x48,0x8b,0x51,0x58,0x48,0x8b,0xd9,0x48,0x8b,0x0d};
    constexpr unsigned char select[]{0x48,0x63,0xba,0x04,0x03,0x00,0x00};
    constexpr unsigned char entity[]{0x8b,0xb4,0xba,0x50,0x01,0x00,0x00};
    constexpr unsigned char hand[]{0x4c,0x63,0x43,0x50};
    // 6C677C receives owner+108 and writes +198+4*hand: owner+2A0/+2A4 held type.
    constexpr unsigned char current[]{0x42,0x89,0x84,0x81,0x98,0x01,0x00,0x00};
    // Current action authority + native hand layout, read-only and exact retail.
    constexpr unsigned char actionOwner[]{0x4c,0x89,0x4b,0x58};
    constexpr unsigned char actionDirector[]{0x48,0x89,0x51,0x40};
    constexpr unsigned char actionSlot[]{0xc7,0x43,0x3c,0x01,0x00,0x00,0x00};
    constexpr unsigned char directorOffset[]{0x48,0x8d,0x4b,0x78};
    constexpr unsigned char liveSlots[]{0x4c,0x8b,0x41,0x38,0x48,0x8b,0x41,0x40};
    constexpr unsigned char isStarted[]{0x0f,0xb6,0x41,0x28,0xc3};
    constexpr unsigned char resourceType[]{0x48,0x63,0x81,0x04,0x03,0x00,0x00};
    constexpr unsigned char resourceLeft[]{0x44,0x8d,0x47,0x01};
    constexpr unsigned char mortarLeft[]{0x8b,0x98,0x58,0x01,0x00,0x00};
    constexpr unsigned char mortarRight[]{0x8b,0x98,0x5c,0x01,0x00,0x00};
    constexpr unsigned char mortarHeld69[]{0x8b,0x81,0xa0,0x02,0x00,0x00,0x83,0xf8,0x15};
    constexpr unsigned char mortarPhase69[]{0x80,0xb8,0x00,0x03,0x00,0x00,0x02};
    constexpr unsigned char entityId[]{0x8b,0x57,0x08};
    constexpr unsigned char getObject[]{0x48,0x8b,0x41,0x20,0xc3};
    constexpr unsigned char getName[]{0x48,0x8b,0x41,0x10,0xc3};
    const auto match=[](std::uintptr_t at,const auto& bytes){for(unsigned i=0;i<sizeof(bytes);++i){unsigned char b=0;if(!Read(reinterpret_cast<void*>(at+i),b)||b!=bytes[i])return false;}return true;};
    std::uintptr_t nameFn=0,objectFn=0;
    const bool qualified=match(base+0x2E195AB,attach)&&match(base+0x2E195B9,select)&&match(base+0x2E195C3,entity)&&
        match(base+0x2E195F7,hand)&&match(base+0x73795C,directorOffset)&&match(base+0x9CC2AB,actionOwner)&&
        match(base+0x9CC3CA,actionDirector)&&match(base+0x9CC2D7,actionSlot)&&
        match(base+0x8D2454,liveSlots)&&match(base+0x1A731D0,isStarted)&&
        match(base+0x6C2A15,resourceType)&&match(base+0x6C2A48,resourceLeft)&&
        match(base+0x6C3C9F,mortarLeft)&&match(base+0x6C3CDA,mortarRight)&&match(base+0x8D237C,mortarHeld69)&&match(base+0xA97A9A,mortarPhase69)&&match(base+0x4238D4,entityId)&&match(base+0x6C6790,current)&&match(base+0x638480,getName)&&
        Read(reinterpret_cast<void*>(base+0x3A657C0+0x10),nameFn)&&nameFn==base+0x638480&&match(base+0x19A2A90,getObject)&&
        Read(reinterpret_cast<void*>(base+0x3A657C0+0xE0),objectFn)&&objectFn==base+0x19A2A90;
    result.store(qualified,std::memory_order_relaxed);checked.store(base,std::memory_order_release);return qualified;
}
static bool ReadAlchemyTool18(void* attachment,std::uint32_t entity,std::uintptr_t character,
                              kcd2vr::alchemy_tool18::Binding& out) noexcept {
    using namespace kcd2vr::activity_input163;
    out={};if(!AlchemyToolPins18()||!witness175.ready.load(std::memory_order_acquire)||
        witness175.transitionDepth175.load(std::memory_order_acquire))return false;
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire);
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);void* player=nullptr;
    if(!instance||!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !PhysVft6698(player,0x3AA4B98))return false;
    void* back=nullptr;if(!Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager)return false;
    const auto rd=[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<void*>(at),value);};
    kcd2vr::subsystems::alchemy::bridge67::Control intent70{};
    const auto now70=std::uint64_t(GetTickCount64());
    const bool place70=kcd2vr::workstation178r20c1::fullVrAlchemy63(now70)&&
        kcd2vr::subsystems::alchemy::bridge67::control.read(intent70)&&intent70.enabled&&!intent70.passenger&&intent70.controllerMortar70&&
        intent70.instance==instance&&intent70.lifetime==lifetime&&intent70.tick&&now70>=intent70.tick&&now70-intent70.tick<=80&&
        intent70.modeEpoch==kcd2vr::minigame_hands45::modeEpoch67.load();
    if(!kcd2vr::alchemy_tool18::read(rd,WhBase(),instance,reinterpret_cast<std::uintptr_t>(player),
        character,reinterpret_cast<std::uintptr_t>(attachment),entity,out,place70))return false;
    if(!witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
       lifetime!=witness175.lifetime175.load(std::memory_order_acquire)||
       instance!=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)||
       manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)){out={};return false;}
    out.lifetime=lifetime;return true;
}
