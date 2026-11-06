namespace ac18=kcd2vr::ammo_carry_t18;
namespace flight17=kcd2vr::throw_projectile_t17;
struct AmmoCommandT18 {st2::Input owner{};std::uint64_t serial=0,ammo=0,launcher=0,reserved=0;int side=-1,kind=-1;};
struct AmmoStatusT18 {std::uint64_t serial=0,ended=0,tick=0;bool held=false;};
static core::SnapshotMailbox<AmmoCommandT18> g_ammo_command18[2];
static core::SnapshotMailbox<AmmoStatusT18> g_ammo_status18[2];
struct AmmoHandT18 {
    AmmoCommandT18 command{};void* arrow=nullptr;void* entity=nullptr;std::uint32_t id=0;
    ng13::Pose grip{};ac18::Motion motion{};std::uint64_t load=0,lastSeen=0,ended=0,previewKey=0;
    bool releasing=false,spent=false,fault=false,ready=false,waitingCleanup=false,retireFault=false;std::uint64_t started=0;
};
static AmmoHandT18 g_ammo_hand18[2];

struct AmmoFlightT18 {
    std::uintptr_t arrow=0,entity=0;std::uint32_t id=0;
    std::uint64_t key=0,serial=0,load=0,tick=0,born=0,ageMs=0;int kind=0;
    flight17::Flight policy{};float slot[12]{};bool slotKnown=false;
};
static AmmoFlightT18 g_ammo_flight18[16];
struct AmmoContactIdentityT18 {std::uintptr_t arrow=0;std::uint32_t entity=0;std::uint64_t key=0,serial=0;};
static core::SnapshotMailbox<AmmoContactIdentityT18> g_ammo_contact_id18[16];
static std::atomic<std::uint64_t> g_ammo_contact18[16]{};
// T25 read-only contact-entry cause receipt. Callback never logs or gates native
// contact. Each registration gets one bounded attempt, even on read/mailbox miss.
struct AmmoCauseReceiptT25 {
    std::uintptr_t arrow=0;std::uint32_t entity=0,actor=0;
    std::uint64_t key=0,serial=0,source=0;unsigned known=0;
};
static core::SnapshotMailbox<AmmoCauseReceiptT25> g_ammo_cause_contact25[16];
static std::atomic<std::uint64_t> g_ammo_cause_attempt25[16]{};
static void AmmoDrainCauseContactT25(int,const AmmoFlightT18&) noexcept;
using ArrowContactFnT18=bool(__fastcall*)(void*,void*);
static ArrowContactFnT18 g_arrow_contact_original18=nullptr;
static std::optional<DetourModKit::hook::Hook> g_arrow_contact_hook18;
static bool AmmoNativePinsT18(std::uintptr_t* failedRvaK39=nullptr) noexcept {
    if(failedRvaK39)*failedRvaK39=0;
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
      {0x8D534C,{0x48,0x89,0x5c,0x24,0x20,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57}},
      {0x40B730,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}},
      {0x509810,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x45}},
      {0x421528,{0x48,0x89,0x5c,0x24,8,0x57,0x48,0x83,0xec,0x20,0x8b,0xfa,0x41,0x8a,0xd8,0x33}},
      {0xAB2EA8,{0x48,0x89,0x5c,0x24,8,0x57,0x48,0x83,0xec,0x20,0x8b,0xfa,0x49,0x8b,0xd8,0x33}},
      {0x2AAD50C,{0x48,0x85,0xd2,0x0f,0x84,0xbc,1,0,0,0x48,0x8b,0xc4,0x48,0x89,0x58,8}},
      {0xD85828,{0x48,0x8b,0xc4,0x48,0x89,0x58,8,0x48,0x89,0x70,0x10,0x55,0x57,0x41,0x56,0x48}},
      {0x23D1C90,{0x48,0x83,0xec,0x38,0x8a,0x44,0x24,0x60,0x4c,0x8d,0x1d,0xf9,0xa5,0xcd,1,0x4d}},
      {0x2AA3374,{0x48,0x8b,0x81,0x20,1,0,0,0x48,0x85,0xc0,0x74,6,0x48,0x8b,0x40,0x30}},
      {0x2AA1A9C,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x81,0xec,0xf0,3}}
    };
    for(const auto& p:pins)if(!MatchBytes(b+p.rva,p.bytes,16)){
        if(p.rva!=0x40B730||!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase())){
            if(failedRvaK39)*failedRvaK39=p.rva;return false;
        }
    }
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3B0C698),0x30,0)!=b+0xD85828){
        if(failedRvaK39)*failedRvaK39=0x3B0C6C8;return false;
    }
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3B0C698),8,0)!=b+0x1A724F0){
        if(failedRvaK39)*failedRvaK39=0x3B0C6A0;return false;
    }
    return true;
}
// K33: the dedicated chest route is published from exact native entity slot0
// and the completed-body source. Skinning/Mannequin caches are a different
// observer and may be absent while that CURRENT native source is valid.
static bool AmmoCharacterCurrentK33(const st2::Input& i,std::uint64_t now) noexcept {
    if(i.chestKnivesT26&&kcd2vr::ammo_service_t22::integrated&&kcd2vr::ammo_service_t22::leftChestKnivesT26){
        kcd2vr::throw_admission_t23::Source source{};
        return kcd2vr::throw_admission_t23::CurrentSource(i,now,&source)&&
            source.frame.chestKnivesT26&&source.frame.character==i.character;
    }
    return reinterpret_cast<std::uintptr_t>(HenryCharacter18())==i.character;
}
static bool AmmoInputT18(const st2::Input& i,int side,std::uint64_t now) noexcept {
    if(side<0||side>1||(inventory_lifecycle_t8::state.load()&1)||!i.allowed||!i.manager||!i.player||!i.character||!i.entity||!i.body||!i.request||!i.sample||
       !st2::fresh(now,i.tick,80)||!i.pose18[side]||!std::isfinite(i.grip18[side])||
       !Vft13(reinterpret_cast<void*>(i.manager),0x3EC9958)||!Vft13(reinterpret_cast<void*>(i.player),0x3AA4B98)||
       !AmmoCharacterCurrentK33(i,now))return false;
    // Ordinary native pickup/held-prop acquisition owns the same real hand.
    // Never let an equipment-empty snapshot steal that owner's lifetime.
    ng13::Binding binding{};ng13::Acquisition54 acquisition{};ng13::PendingUse55 use{};
    if((ReadGraspBinding(binding)&&binding.side==side)||
       (ReadAcquisition54(acquisition)&&acquisition.side==side&&ng13::freshAcquisition54(acquisition,now))||
       (ReadPendingUse55(use)&&use.side==side&&use.character==i.character&&use.epoch==i.body&&st2::fresh(now,use.requestedAt,80)))return false;
    void* player=nullptr;std::uint64_t hand=0;
    if(!HandKey13(reinterpret_cast<void*>(i.manager),side,hand,&player)||reinterpret_cast<std::uintptr_t>(player)!=i.player||
       !SupplyEmptyT2(hand)||Rd<unsigned>(player,0x30,0)!=i.entity||Rd<void*>(player,0x9D0,nullptr)!=reinterpret_cast<void*>(i.manager))return false;
    // The existing bow/crossbow/rifle owns its ammunition attachment/reload.
    // Throwables may occupy an empty offhand beside a MELEE weapon, not steal
    // a ranged weapon's string/tool/bolt hand.
    for(int s=0;s<2;++s){std::uint64_t key=0;if(!HandKey13(reinterpret_cast<void*>(i.manager),s,key))return false;
        if(!SupplyEmptyT2(key)){void* item=Item13(key);if(!item)return false;
            __try{if(Native13<bool(__fastcall*)(void*,int)>(0x469590)(item,2))return false;}
            __except(EXCEPTION_EXECUTE_HANDLER){return false;}
        }
    }
    return true;
}
// R11: the owner half of AmmoInputT18 (same current Henry/body/request/fresh packet), no hand conditions.
static bool AmmoOwnerT18R11(const st2::Input& i,std::uint64_t now) noexcept {
    return !(inventory_lifecycle_t8::state.load()&1)&&i.allowed&&i.manager&&i.player&&i.character&&i.entity&&i.body&&i.request&&i.sample&&
        st2::fresh(now,i.tick,80)&&Vft13(reinterpret_cast<void*>(i.manager),0x3EC9958)&&Vft13(reinterpret_cast<void*>(i.player),0x3AA4B98)&&
        AmmoCharacterCurrentK33(i,now)&&Rd<unsigned>(reinterpret_cast<void*>(i.player),0x30,0)==i.entity&&
        Rd<void*>(reinterpret_cast<void*>(i.player),0x9D0,nullptr)==reinterpret_cast<void*>(i.manager);
}
static void* AmmoModelT18(const st2::Input& in) noexcept {
    void* player=reinterpret_cast<void*>(in.player);void* model=Rd<void*>(player,0x1B0,nullptr);
    return Vft13(player,0x3AA4B98)&&model&&Rd<void*>(model,0x20,nullptr)==player?model:nullptr;
}
static bool AmmoSlotOwnedT18(const AmmoHandT18& h) noexcept {
    void* model=AmmoModelT18(h.command.owner);
    return model&&Rd<unsigned>(model,0x2C+0x24*h.command.side,0)==h.id;
}
static bool AmmoAttachmentT18(void* model,int side,unsigned& bound) noexcept {
    bound=~0u;if(!model||side<0||side>1)return false;
    const char* name=Rd<const char*>(reinterpret_cast<void*>(WhBase()+0x40AC298),8*side,nullptr);
    if(!name)return false;void* attachment=nullptr;
    __try{attachment=Native13<void*(__fastcall*)(void*,const char*)>(np13::kFindAttachment)(model,name);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!Vft13(attachment,0x3A657C0))return false;
    void* object=Rd<void*>(attachment,0x20,nullptr);if(!object){bound=0;return true;}
    if(!Vft13(object,0x3A68F68))return false;bound=Rd<unsigned>(object,8,0);return true;
}
static bool AmmoSameOwnerT18(const st2::Input& a,const st2::Input& b) noexcept {
    return a.manager==b.manager&&a.player==b.player&&a.character==b.character&&a.entity==b.entity&&a.body==b.body&&a.request==b.request;
}
static void AmmoNoteT18(const char* stage,int side,std::uint64_t serial,std::uint64_t key,const char* detail) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;const auto now=std::uint64_t(GetTickCount64());
    for(auto domain:{cap::Domain::Interaction,cap::Domain::Combat,cap::Domain::Reload,cap::Domain::Bug})if(cap::active(domain,now)){
        char line[384]{};std::snprintf(line,sizeof(line),"T18 AMMO stage=%s side=%d serial=%llu key=%016llX %s",
            stage,side,(unsigned long long)serial,(unsigned long long)key,detail);diagnostic_trace::WriteFor(domain,line);return;
    }
}
static std::uint64_t AmmoNoteSessionK39() noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;const auto now=std::uint64_t(GetTickCount64());
    for(auto domain:{cap::Domain::Interaction,cap::Domain::Combat,cap::Domain::Reload,cap::Domain::Bug})if(cap::active(domain,now)){
        const auto serial=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
        return serial&&serial<=(UINT64_MAX>>4)?(serial<<4)|(unsigned(domain)+1):0;
    }
    return 0;
}
static void* AmmoInventoryT18(std::uintptr_t player) noexcept {
    __try{return Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(reinterpret_cast<void*>(player));}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static bool AmmoEntityT18(void* arrow,void* entity,std::uint32_t id) noexcept {
    return arrow&&entity&&id&&Entity13(id)==entity&&Vft13(entity,0x4072C78)&&Vft13(arrow,0x3ECE6F0)&&
        Rd<unsigned>(arrow,0x30,0)==id&&Rd<void*>(arrow,0x38,nullptr)==entity;
}
enum class AmmoLookupT18 { Unknown, Absent, Present };
static AmmoLookupT18 AmmoResolveT18(unsigned id,void*& entity) noexcept {
    entity=nullptr;const auto base=WhBase();
    void* sys=Rd<void*>(reinterpret_cast<void*>(base),0x492D8A0,nullptr);
    if(!id||!retail_build112::Ready(base)||!MatchBytes(base+np13::kEntityLookup,np13::pEntityLookup,sizeof(np13::pEntityLookup))||!Vft13(sys,0x4077048)||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(base+0x4077048),0x70,0)!=base+np13::kEntityLookup)return AmmoLookupT18::Unknown;
    __try{entity=Native13<void*(__fastcall*)(void*,unsigned)>(np13::kEntityLookup)(sys,id);}
    __except(EXCEPTION_EXECUTE_HANDLER){return AmmoLookupT18::Unknown;}
    if(Rd<void*>(reinterpret_cast<void*>(base),0x492D8A0,nullptr)!=sys||!Vft13(sys,0x4077048))return AmmoLookupT18::Unknown;
    if(!entity)return AmmoLookupT18::Absent; // native success, explicitly NULL
    return Vft13(entity,0x4072C78)&&Rd<unsigned>(entity,0xC,0)==id?AmmoLookupT18::Present:AmmoLookupT18::Unknown;
}
static bool AmmoPlaceT18(void* entity,const ng13::Pose& p) noexcept {
    if(!ng13::valid(p)||!Vft13(entity,0x4072C78))return false;
    const float scale[3]{1,1,1};
    __try{Native13<void(__fastcall*)(void*,const float*,const float*,const float*,unsigned)>(0x509810)(entity,p.p,p.q,scale,0x10);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool AmmoSlotT18(void* entity,const float* matrix) noexcept {
    __try{Native13<void(__fastcall*)(void*,int,const float*,unsigned)>(0xAB2EA8)(entity,0,matrix,0);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void AmmoCaptureCauseContactT25(int n,void* arrow,const AmmoContactIdentityT18& id) noexcept {
    if(n<0||n>=16||!id.serial||!id.key||!id.entity||id.arrow!=reinterpret_cast<std::uintptr_t>(arrow))return;
    std::uint64_t empty=0;
    if(!g_ammo_cause_attempt25[n].compare_exchange_strong(empty,id.serial,std::memory_order_acq_rel))return;
    // Read only the original native callback receiver and its CURRENT payload.
    // A bad payload never authorizes reading the cause fields or a retry.
    void* payload=nullptr;std::uint64_t key=0;unsigned entity=0;
    if(!SehCopy(static_cast<unsigned char*>(arrow)+0x30,&entity,sizeof(entity))||entity!=id.entity||
       !SehCopy(static_cast<unsigned char*>(arrow)+0x120,&payload,sizeof(payload))||!payload||
       !SehCopy(static_cast<unsigned char*>(payload)+0x30,&key,sizeof(key))||key!=id.key)return;
    AmmoCauseReceiptT25 receipt{};receipt.arrow=id.arrow;receipt.entity=id.entity;receipt.key=id.key;receipt.serial=id.serial;
    if(SehCopy(static_cast<unsigned char*>(arrow)+0x108,&receipt.actor,sizeof(receipt.actor)))receipt.known|=1;
    if(SehCopy(static_cast<unsigned char*>(arrow)+0x110,&receipt.source,sizeof(receipt.source)))receipt.known|=2;
    (void)g_ammo_cause_contact25[n].publish(receipt); // copy only; busy is a lost diagnostic, never a hit refusal
}
static bool __fastcall ArrowContactT18(void* arrow,void* event) noexcept {
    // Occurrence only. No hits, timing changes, native pointer ownership or
    // engine mutations from the physics callback. The original owns the hit.
    for(int n=0;n<16;++n){AmmoContactIdentityT18 id{};
        if(g_ammo_contact_id18[n].read(id)&&id.serial&&id.arrow==reinterpret_cast<std::uintptr_t>(arrow)&&
           Rd<unsigned>(arrow,0x30,0)==id.entity){g_ammo_contact18[n].store(id.serial,std::memory_order_release);AmmoCaptureCauseContactT25(n,arrow,id);break;}}
    return g_arrow_contact_original18?g_arrow_contact_original18(arrow,event):false;
}
static bool InstallAmmoContactT18() noexcept {
    if(g_arrow_contact_hook18)return g_arrow_contact_original18&&g_arrow_contact_hook18->is_enabled();
    const unsigned char pin[]{0x48,0x89,0x5c,0x24,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
    if(!AmmoNativePinsT18()||!MatchBytes(WhBase()+0x2ACBFA8,pin,sizeof(pin)))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_t18_arrow_contact_observation",DetourModKit::hook::Target{DetourModKit::Address{WhBase()+0x2ACBFA8}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(req),&ArrowContactT18);if(!hook)return false;
    g_arrow_contact_hook18=std::move(*hook);g_arrow_contact_original18=g_arrow_contact_hook18->original<ArrowContactFnT18>();
    return g_arrow_contact_original18&&PinOwnCallbackT2(reinterpret_cast<const void*>(&ArrowContactT18))&&g_arrow_contact_hook18->enable();
}
static bool AmmoFlightIdentityT18(const AmmoFlightT18& f,void*& arrow,void*& entity) noexcept {
    arrow=reinterpret_cast<void*>(f.arrow);entity=Entity13(f.id);
    if(reinterpret_cast<std::uintptr_t>(entity)!=f.entity||!AmmoEntityT18(arrow,entity,f.id))return false;
    void* payload=Item13(f.key);
    return payload&&Rd<void*>(arrow,0x120,nullptr)==payload&&Rd<std::uint64_t>(payload,0x30,0)==f.key;
}
static void AmmoTrackFlightT18(const AmmoLaunchResultT17& result,const AmmoHandT18& h,std::uint64_t now) noexcept {
    for(int n=0;n<16;++n)if(!g_ammo_flight18[n].serial){
        auto& f=g_ammo_flight18[n];f={};f.arrow=reinterpret_cast<std::uintptr_t>(result.projectile);
        f.entity=reinterpret_cast<std::uintptr_t>(Entity13(result.entity));f.id=result.entity;f.key=result.payloadKey;
        f.serial=h.command.serial;f.load=h.load;f.tick=now;f.born=now;f.kind=h.command.kind;
        f.policy.begin({h.motion.last.p[0],h.motion.last.p[1],h.motion.last.p[2]});
        __try{const auto* slot=Native13<const float*(__fastcall*)(void*,int,bool)>(0x421528)(reinterpret_cast<void*>(f.entity),0,true);
            f.slotKnown=slot&&SehCopy(slot,f.slot,sizeof(f.slot));}
        __except(EXCEPTION_EXECUTE_HANDLER){f.slotKnown=false;}
        g_ammo_cause_attempt25[n].store(0,std::memory_order_release);(void)g_ammo_cause_contact25[n].publish({});
        g_ammo_contact18[n].store(0);(void)g_ammo_contact_id18[n].publish({f.arrow,f.id,f.key,f.serial});ac18::flights.fetch_add(1);return;
    }
    AmmoNoteT18("NATIVE_FLIGHT_UNTRACKED",h.command.side,h.command.serial,result.payloadKey,"boundedTable=FULL nativeFlightContinues=1");
}
static void AmmoTickFlightsT18(std::uint64_t now,bool allowed) noexcept {
    for(int n=0;n<16;++n){auto& f=g_ammo_flight18[n];if(!f.serial)continue;
        // Wall-clock policy retirement cannot wait for accepted frame deltas.
        // Spend our observer before resolving any historical native pointer.
        if(now<f.born||now-f.born>15000||f.load!=inventory_lifecycle_t8::loadEpisode18.load()){
            (void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);continue;
        }
        void* arrow=nullptr;void* entity=nullptr;
        if(!AmmoFlightIdentityT18(f,arrow,entity)){
            (void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);continue;
        }
        AmmoDrainCauseContactT25(n,f); // after EXISTING current-flight identity; no second lookup
        const bool contacted=g_ammo_contact18[n].load(std::memory_order_acquire)==f.serial;
        if(contacted){
            if(f.slotKnown)(void)AmmoSlotT18(entity,f.slot);
            AmmoNoteT18(contacted?"NATIVE_CONTACT":"FLIGHT_POLICY_RETIRED",-1,f.serial,f.key,"damage=NATIVE_CARROW_ONLY embedding=NATIVE rotation=RESTORED");
            (void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);continue;
        }
        const auto prior=f.tick;f.tick=now;
        if(!allowed||now<=prior||now-prior>50){f.policy.breakInterval();continue;}f.ageMs+=now-prior;
        float tm[12]{};ng13::Pose pose{};
        if(!SehCopy(static_cast<unsigned char*>(entity)+0x58,tm,sizeof(tm))||!ng13::fromMatrix(tm,pose)||
           !f.policy.observe({pose.p[0],pose.p[1],pose.p[2]})){f.policy.breakInterval();continue;}
        // Native render-proxy slot transform only. Particle/collision root and
        // its native trajectory remain untouched by visual tumbling.
        if(f.slotKnown){
            float rotated[12]{};const float angle=f.policy.furthestHorizontal*(f.kind==1?1.5707963f:1.0471976f);
            const float c=std::cos(angle),s=std::sin(angle);
            for(int r=0;r<3;++r){const int j=r*4;rotated[j]=f.slot[j];
                rotated[j+1]=f.slot[j+1]*c+f.slot[j+2]*s;rotated[j+2]=-f.slot[j+1]*s+f.slot[j+2]*c;rotated[j+3]=f.slot[j+3];}
            (void)AmmoSlotT18(entity,rotated);
        }
        void* body=Rd<void*>(arrow,0x70,nullptr);
        if(!Vft13(body,0x3B0C698)){f.policy.breakInterval();continue;} // only the proven native arrow particle
        DropDynamicsT1 d{};int ok=0;
        __try{ok=Native13<int(__fastcall*)(void*,void*)>(0xD85828)(body,&d);}
        __except(EXCEPTION_EXECUTE_HANDLER){ok=0;}
        if(!ok||!flight17::finite({d.v[0],d.v[1],d.v[2]})){f.policy.breakInterval();continue;}
        flight17::Vec3 v{};
        if(f.policy.tailVelocity({d.v[0],d.v[1],d.v[2]},float(now-prior)*.001f,v)){
            if(!AmmoFlightIdentityT18(f,arrow,entity)||g_ammo_contact18[n].load()==f.serial||Rd<void*>(arrow,0x70,nullptr)!=body){f.policy.breakInterval();continue;}
            const float corrected[3]{v.x,v.y,v.z};
            __try{Native13<void(__fastcall*)(void*,void*,const float*,float,void*,int)>(0x2AAD50C)(nullptr,body,corrected,1.f,nullptr,0);}
            __except(EXCEPTION_EXECUTE_HANDLER){if(f.slotKnown)(void)AmmoSlotT18(entity,f.slot);(void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);}
        }
    }
}
struct AmmoReplyT18 {char status[32]{};std::uint64_t key=0,launcher=0;int legacy=0,daggers=0,axes=0;};
static bool AmmoTokenT18(const char* text,std::uint64_t& key) noexcept {
    key=0;if(!std::strcmp(text,"-"))return true;
    constexpr char prefix[]="[Item-";if(std::strncmp(text,prefix,6))return false;
    const char* p=text+6;std::uint64_t value=0;unsigned digits=0;
    while(*p>='0'&&*p<='9'){const auto d=unsigned(*p++-'0');if(value>(0x00FFFFFFFFFFFFFFull-d)/10)return false;value=value*10+d;++digits;}
    if(!digits||p[0]!=']'||p[1]||!value)return false;key=0x0200000000000000ull|value;return true;
}
static bool AmmoStillSourceT18(const st2::Input& before,int side,std::uint64_t now) noexcept {
    st2::Input after{};return g_supply_input_t2.read(after)&&AmmoSameOwnerT18(before,after)&&AmmoInputT18(after,side,now);
}
#include "native_escrow_return_t19.inl"
struct AmmoReservationT18 {std::uint64_t serial=0,key=0,nextClear=0;int kind=0;st2::Input owner{};bool abortComplete=false;};
