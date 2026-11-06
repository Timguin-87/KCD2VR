    namespace se54=kcd2vr::swing_effort59c14;se54::Action54 costAction54{};bool costBound54=false;
    if(const auto* released54=se54::releasedInput54;released54&&g_base&&se54::actionContractBase54.load()==g_base&&
       caller==g_base+0xD4B0D2&&reinterpret_cast<std::uintptr_t>(target)==released54->actor){
        const auto action54=Rd<std::uintptr_t>(reaction,0,0),helper54=Rd<std::uintptr_t>(reinterpret_cast<void*>(action54),0xA0,0),
            row54=Rd<std::uintptr_t>(reinterpret_cast<void*>(action54),0x58,0);
        const void* entity54=nullptr;const void* state54=nullptr;const auto type54=Rd<std::uintptr_t>(reinterpret_cast<void*>(row54),0x148,0);
        const int nativeType54=Rd<int>(reinterpret_cast<void*>(type54),8,-1);
        if(CombatActor(target,entity54,state54,false)&&IsPlayer(entity54)&&reinterpret_cast<std::uintptr_t>(state54)==released54->state&&
           Rd<std::uintptr_t>(target,0x1F8,0)==released54->key.player&&
           Rd<std::uintptr_t>(reinterpret_cast<void*>(action54),0,0)==g_base+0x3A61C58&&
           Rd<int>(reinterpret_cast<void*>(action54),0x3C,-1)==0&&Rd<std::uintptr_t>(reinterpret_cast<void*>(action54),0x70,0)==released54->actor&&
           Rd<std::uintptr_t>(reinterpret_cast<void*>(helper54),0,0)==g_base+0x3A61040&&
           Rd<std::uintptr_t>(reinterpret_cast<void*>(helper54),8,0)==released54->actor&&
           Rd<std::uintptr_t>(reinterpret_cast<void*>(helper54),0x20,0)==action54+0x88&&
           Rd<std::uintptr_t>(reinterpret_cast<void*>(action54),0x88,0)==g_base+0x3A61440&&
           (nativeType54==0||nativeType54==1)&&Rd<int>(reinterpret_cast<void*>(row54),0x224,-1)==0)
            costBound54=se54::actionRegistry54.bind(*released54,action54,helper54,row54,GetTickCount64(),costAction54);
    }
