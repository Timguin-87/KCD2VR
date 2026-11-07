static int CallOuter(const void* attacker,const void* target,void* hitInfo,void* details,const void* gameRules,bool& queued,bool& reaction,int& kindOut,bool& alive,bool& down,int& dispatched,bool counterMajor=false) noexcept {
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
            if(death)reinterpret_cast<ReactFn>(g_base+kOuterReact)(manager,&out,hitInfo,kindOut,attacker,
                Rd<int>(reinterpret_cast<const void*>(g_base+0x5320D24),0,-1));
            if(!out)reinterpret_cast<ReactFn>(g_base+kOuterReact)(manager,&out,hitInfo,kindOut,attacker,-1);
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
