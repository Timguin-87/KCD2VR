// C45: copy of the live alchemy OpenBook action (recipe book). Same exact director slot read as the C37 receipt
// (owner+78 director vft 3BFA5B0, +B0/+B8 live slot vector, slot 1 = current action, action+28 started) plus the
// framework action owner backlink (+58 == owner, alchemy_tool18). C46 requires the
// exact native read-enable generation and mode4, and copies book entity type19.
// Zero when anything differs; never a native call.
static void AlchemyBookNativeFrameEndImpl45() noexcept {
    const auto b=base175.load(std::memory_order_acquire);
    const auto instance=AlchemyInputOwner19c4();
    if(!b||!instance){(void)alchemyBook45.publish({});return;}
    const auto lifetime=witness175.lifetime175.load(std::memory_order_acquire);
    std::uintptr_t director=0,first=0,last=0,action=0,vft=0,owner=0;unsigned char started=0,mode=255;
    std::uint32_t entity=0;kcd2vr::alchemy_semi46::BookEnable enabled{};
    const bool open=kcd2vr::alchemy_semi46::hooksReady.load(std::memory_order_acquire)&&
        kcd2vr::alchemy_semi46::bookEnable.read(enabled)&&enabled.enabled&&enabled.instance==instance&&
        enabled.lifetime==lifetime&&enabled.generation==kcd2vr::alchemy_semi46::bookGeneration.load(std::memory_order_acquire)&&
        AlchemyInputRead19c4(instance+0x78,director)&&director==b+0x3BFA5B0&&
        AlchemyInputRead19c4(instance+0xB0,first)&&AlchemyInputRead19c4(instance+0xB8,last)&&first&&last>=first+16&&
        last-first<=256&&!((last-first)%8)&&AlchemyInputRead19c4(first+8,action)&&action&&
        AlchemyInputRead19c4(action,vft)&&vft==b+0x3A5BD40&&AlchemyInputRead19c4(action+0x58,owner)&&owner==instance&&
        AlchemyInputRead19c4(action+0x28,started)&&started==1&&enabled.action==action&&
        AlchemyInputRead19c4(instance+0x300,mode)&&mode==4&&AlchemyInputRead19c4(instance+0x19C,entity)&&entity&&
        AlchemyInputRead19c4(first+8,owner)&&owner==action&&
        lifetime==witness175.lifetime175.load(std::memory_order_acquire)&&AlchemyInputOwner19c4()==instance;
    (void)alchemyBook45.publish(open?AlchemyBook45{instance,action,lifetime,std::uint64_t(GetTickCount64()),enabled.generation,entity}:AlchemyBook45{});
}
