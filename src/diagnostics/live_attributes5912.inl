// Exact retail C_Soul primary vtable / state and base-stat accessors. No SDK
// offsets or guessed script handler. The native MaxState routine includes its
// own derived maximum calculation. All calls here are on Henry's native update.
static bool SoulPins5912() noexcept {
    if(!retail_build112::Ready(g_base))return false;
    constexpr unsigned char current[]{0x8B,0xC2,0xF3,0x0F,0x10,0x84,0x81,0x08,0x07,0x00,0x00,0xC3};
    constexpr unsigned char maximum[]{0x48,0x83,0xEC,0x28,0x85,0xD2,0x75,0x12};
    constexpr unsigned char baseStat[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x83,0xFA,0x07};
    unsigned char a[12]{},b[8]{},c[12]{};
    return SehCopy146(reinterpret_cast<void*>(g_base+0x642B30),a,sizeof(a))&&std::memcmp(a,current,sizeof(a))==0&&
        SehCopy146(reinterpret_cast<void*>(g_base+0x46F1F4),b,sizeof(b))&&std::memcmp(b,maximum,sizeof(b))==0&&
        SehCopy146(reinterpret_cast<void*>(g_base+0x469D7C),c,sizeof(c))&&std::memcmp(c,baseStat,sizeof(c))==0&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(g_base+0x3F44F28),0x270,0)==g_base+0x642B30&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(g_base+0x3F44F28),0x280,0)==g_base+0x46F1F4;
}
// R59.16C10R17: effective stat level (Soul vt+0x2A8 = 469D28(soul, idx, ctx=nullptr, flag=0), the same call the native attack-cost
// path makes at 687183): base + modifier chain (buffs, alcohol, sleep exhaustion), clamped by the game to [0, StatCap].
static bool EffectivePins59c17() noexcept {
    static int pins=-1;if(pins>=0)return pins==1;
    constexpr unsigned char entry[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x30,0x49};
    unsigned char a[sizeof(entry)]{};
    pins=(retail_build112::Ready(g_base)&&SehCopy146(reinterpret_cast<void*>(g_base+0x469D28),a,sizeof(a))&&std::memcmp(a,entry,sizeof(a))==0&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(g_base+0x3F44F28),0x2A8,0)==g_base+0x469D28)?1:0;
    return pins==1;
}
static bool ReadEffective59c17(const void* soul,float& strength,float& agility) noexcept {
    using Fn=float(__fastcall*)(const void*,int,const void*,int);
    if(!EffectivePins59c17())return false;
    __try{
        if(*static_cast<const std::uintptr_t*>(soul)!=g_base+0x3F44F28)return false;
        strength=reinterpret_cast<Fn>(g_base+0x469D28)(soul,0,nullptr,0);agility=reinterpret_cast<Fn>(g_base+0x469D28)(soul,1,nullptr,0);
        return std::isfinite(strength)&&std::isfinite(agility)&&strength>=0&&strength<=100&&agility>=0&&agility<=100;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// C26c: base skill level i = soul+0x5E8+8*i, as read by the native generic stat/skill getter 1123C34 (ids < 0x23 =
// skills; 0x23..0x2C = the base stats via 469D7C). Pinned once; any mismatch leaves skills unread (-1, neutral feel).
static bool SkillPins26() noexcept {
    static int pins=-1;if(pins>=0)return pins==1;
    constexpr unsigned char getter[]{0x83,0xFA,0x23,0x73,0x0B,0x8B,0xC2,0x8B,0x84,0xC1,0xE8,0x05,0x00,0x00};
    unsigned char a[sizeof(getter)]{};
    pins=(retail_build112::Ready(g_base)&&SehCopy146(reinterpret_cast<void*>(g_base+0x1123C4B),a,sizeof(a))&&std::memcmp(a,getter,sizeof(a))==0)?1:0;
    return pins==1;
}
static void ReadSkills26(const void* soul,int (&out)[5]) noexcept {
    static constexpr int ids[5]{16,17,23,22,24};
    if(!SkillPins26())return;
    __try{
        if(*static_cast<const std::uintptr_t*>(soul)!=g_base+0x3F44F28)return;
        for(unsigned i=0;i<5;++i){const int v=Rd<int>(soul,0x5E8+8*ids[i],-1);out[i]=v>=0&&v<=40?v:-1;}
    }__except(EXCEPTION_EXECUTE_HANDLER){for(auto& v:out)v=-1;}
}
static bool ReadSoul5912(const void* soul,float& current,float& maximum,int& strength,int& agility) noexcept {
    using Fn=float(__fastcall*)(const void*,int);
    __try {
        if(*static_cast<const std::uintptr_t*>(soul)!=g_base+0x3F44F28)return false;
        current=reinterpret_cast<Fn>(g_base+0x642B30)(soul,1);
        maximum=reinterpret_cast<Fn>(g_base+0x46F1F4)(soul,1);
        // 469D7C: base stat i at +56C+8*i; stat.xml: 0 strength, 1 agility.
        strength=Rd<int>(soul,0x56C,-1);agility=Rd<int>(soul,0x574,-1);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void CollectAttributes5912(const void* actor,const void* entity,std::uint64_t now) noexcept {
    namespace ls=kcd2vr::live5912;
    static std::uint64_t last=0;static const void* lastPlayer=nullptr;static bool lastOK=false;
    const void* player=Rd<const void*>(actor,0x1F8,nullptr);
    if(!ls::enabled.load()||!(kcd2vr::motion59c4::selected.load()||(ls::allRungs.load(std::memory_order_relaxed)&&frik_pose_probe::CombatExperiment175Enabled()))){
        if(lastOK){ls::latest.publish({});lastOK=false;}return;
    }
    if(player==lastPlayer&&now>=last&&now-last<33)return;
    last=now;lastPlayer=player;ls::Snapshot s{};s.tick=now;
    s.player=reinterpret_cast<std::uintptr_t>(player);s.entity=reinterpret_cast<std::uintptr_t>(entity);
    s.recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
    const void* soul=player?Rd<const void*>(player,0x668,nullptr):nullptr;s.soul=reinterpret_cast<std::uintptr_t>(soul);
    float current=0,maximum=0;int strength=-1,agility=-1;
    if(player&&entity&&Rd<const void*>(player,0x278,nullptr)==actor&&Rd<const void*>(player,0x38,nullptr)==entity&&
       soul&&SoulPins5912()&&ReadSoul5912(soul,current,maximum,strength,agility)&&
       std::isfinite(current)&&std::isfinite(maximum)&&current>=0&&maximum>0&&maximum<=1.e6f&&
       strength>=0&&strength<=1000&&agility>=0&&agility<=1000){
        s.current=current;s.maximum=maximum;s.stamina=std::clamp(double(current)/maximum,0.,1.);
        s.strength=strength;s.agility=agility;s.valid=true;
        float es=-1,ea=-1;if(ReadEffective59c17(soul,es,ea)){s.strengthEffective=es;s.agilityEffective=ea;}
        ReadSkills26(soul,s.skills26);
    }
    ls::latest.publish(s); // failed reads revoke the old value, rather than renew it
    if(s.valid!=lastOK){char line[512]{};std::snprintf(line,sizeof(line),
        "R59.12C1 LIVE_ATTRIBUTES valid=%d stamina=%.4f current=%.3f maximum=%.3f strengthBase=%.1f agilityBase=%.1f sampleTick=%llu strengthEffective=%.1f agilityEffective=%.1f skills(sword/heavy/large/dagger/unarmed)=%d/%d/%d/%d/%d",
        s.valid,s.stamina,s.current,s.maximum,s.strength,s.agility,(unsigned long long)s.tick,s.strengthEffective,s.agilityEffective,s.skills26[0],s.skills26[1],s.skills26[2],s.skills26[3],s.skills26[4]);diagnostic_trace::Write(line);}
    lastOK=s.valid;
}
