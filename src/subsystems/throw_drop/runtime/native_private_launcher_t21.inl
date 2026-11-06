// Integrated finite-stock launcher provider. Included after transient T20 in
// the SAME action-owner scope and invoked by the qualified T22 service.
// It owns no second hook/startup/frame callback or inventory supply policy.
namespace pl21=kcd2vr::private_launcher_t21;
struct PrivateLauncherRecordT21 {
    std::uint64_t token=0,key=0;
    void* itemManager=nullptr;void* world=nullptr;void* definition=nullptr;
    void* wrapper=nullptr;void* item=nullptr;void* treeHeadK37=nullptr;
    alignas(16) unsigned char guid[16]{};
    ta20::CreationRecord record{}; // retained if construction/cleanup is uncertain
    pl21::Status status=pl21::Status::Unknown;
    bool wrapperEntered=false,wrapperReturned=false,ctorEntered=false,ctorReturned=false,constructed=false;
    bool createEntered=false,createReturned=false,stringsEntered=false,stringsReturned=false;
    bool treeEntered=false,treeReturned=false,freeEntered=false,freeReturned=false,recordClean=false;
};
static PrivateLauncherRecordT21 g_private_launchers_t21[pl21::capacity]{};
static bool g_private_launcher_active_t21=false; // qualified native owner only

static bool PrivateLauncherPinsT21() noexcept {
    const auto b=WhBase();if(!AmmoTransientPinsT20())return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
      {0x8D3DC4,{0x48,0x89,0x4c,0x24,8,0x48,0x83,0xec,0x28,0x83,0x3d,0x9c,0x95,0xbc,4,0}},
      {0x8D3E0A,{0x48,0x8d,5,0x27,0x8a,0x18,3,0x48,0xc7,0x41,8,0,0,0,0,0x48}},
      {0x1A78ED0,{0xb0,5,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}},
      {0x19B7D2C,{0x41,0xf6,0xc0,4,0x74,4,0x48,0x89,0x51,8,0x41,0xf6,0xc0,1,0x74,4}},
      {0x19A9F68,{0x41,0xf6,0xc0,1,0x74,4,0xc6,0x41,0x10,0,0x41,0xf6,0xc0,4,0x74,8}},
      {0x468373,{0x48,0x8b,0x83,0x80,0,0,0,0x48,0x8b,0x53,0x78,0x48,0x85,0xc0,0x0f,0x84}}
    };
    for(const auto& p:pins)if(!MatchBytes(b+p.rva,p.bytes,sizeof(p.bytes)))return false;
    std::uintptr_t policy=0,cast=0,type=0;
    return AmmoTransientReadT20(reinterpret_cast<void*>(b+0x3A5C838),0x80,policy)&&policy==b+0x838AE0&&
        AmmoTransientReadT20(reinterpret_cast<void*>(b+0x3A5C838),8,type)&&type==b+0x1A78ED0&&
        AmmoTransientReadT20(reinterpret_cast<void*>(b+0x3A4D890),0x108,cast)&&cast==b+0x5F5DA0;
}
static bool PrivateLauncherScopeT21(const native_script_t2::Context& ctx) noexcept {
    return ctx.Current()&&g_native_ready13.load()&&
        !(inventory_lifecycle_t8::state.load()&1)&&PrivateLauncherPinsT21();
}
static bool PrivateLauncherPairT21(const native_script_t2::Context& ctx,
                                   const PrivateLauncherRecordT21& r) noexcept {
    // loadEpisode18 is POLICY ONLY, never a native allocation incarnation.
    return PrivateLauncherScopeT21(ctx)&&AmmoTransientItemManagerT20()==r.itemManager&&
        WorldInventoryT1()==r.world&&Vft13(r.world,0x3A83DC0)&&PrivateLauncherScopeT21(ctx);
}
static bool PrivateLauncherRowT21(void* row) noexcept {
    unsigned char guid[16]{};
    return Vft13(row,0x3A4D890)&&AmmoTransientReadT20(row,8,guid)&&
        !std::memcmp(guid,pl21::launcherGuid,sizeof(guid));
}
static bool PrivateLauncherRegistryT21(const native_script_t2::Context& ctx,void* row) noexcept {
    if(!PrivateLauncherScopeT21(ctx)||!PrivateLauncherRowT21(row))return false;
    const auto base=WhBase();void* entries=nullptr;std::uint64_t count=0;
    if(!AmmoTransientReadT20(reinterpret_cast<void*>(base+0x5325820),0x78,entries)||
       !AmmoTransientReadT20(reinterpret_cast<void*>(base+0x5325820),0x80,count)||
       !entries||!count||count>65536)return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(entries);
    if(begin>~std::uintptr_t(0)-count*24)return false;
    unsigned found=0;
    for(std::uint64_t i=0;i<count;++i){
        void* current=nullptr;
        if(!AmmoTransientReadT20(entries,static_cast<std::size_t>(i*24+0x10),current))return false;
        if(current==row)++found;
    }
    void* after=nullptr;std::uint64_t afterCount=0;
    return found==1&&AmmoTransientReadT20(reinterpret_cast<void*>(base+0x5325820),0x78,after)&&
        AmmoTransientReadT20(reinterpret_cast<void*>(base+0x5325820),0x80,afterCount)&&
        after==entries&&afterCount==count&&PrivateLauncherScopeT21(ctx)&&PrivateLauncherRowT21(row);
}
static pl21::Lease PrivateLauncherLeaseT21(const PrivateLauncherRecordT21& r) noexcept {
    return {r.token,r.key,reinterpret_cast<std::uintptr_t>(r.itemManager),reinterpret_cast<std::uintptr_t>(r.world),
        reinterpret_cast<std::uintptr_t>(r.wrapper),reinterpret_cast<std::uintptr_t>(r.definition),r.status};
}
static void PrivateLauncherQuarantineOthersT21(void* manager,void* world) noexcept {
    for(auto& r:g_private_launchers_t21)if(r.token&&(r.itemManager!=manager||r.world!=world))
        r.status=pl21::Status::Quarantined; // identity only; NEVER follow old+8
}
static bool PrivateLauncherRecordCurrentT21(const native_script_t2::Context& ctx,
                                           const PrivateLauncherRecordT21& r) noexcept {
    return r.status!=pl21::Status::Quarantined&&PrivateLauncherPairT21(ctx,r)&&
        PrivateLauncherRowT21(r.definition)&&PrivateLauncherPairT21(ctx,r);
}
// K37: exact native lookup qualifies the original item before any retained
// wrapper read. Cleanup may resume only after this same proof, even before the
// independent creation-record memory has finished its known cleanup phases.
static pl21::Status PrivateLauncherItemK37(const native_script_t2::Context& ctx,
                                          PrivateLauncherRecordT21& r,void*& item,bool requireClean) noexcept {
    item=nullptr;
    if(r.status==pl21::Status::Quarantined||r.status==pl21::Status::Absent)return r.status;
    if(!PrivateLauncherPairT21(ctx,r)||!r.createReturned||!r.key||!r.item||!r.wrapper||(requireClean&&!r.recordClean))
        return pl21::Status::Unknown;
    void* found=nullptr;
    __try {found=Native13<void*(__fastcall*)(void*,std::uint64_t)>(0x23D09E0)(r.itemManager,r.key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return pl21::Status::Unknown;}
    if(!PrivateLauncherPairT21(ctx,r))return pl21::Status::Unknown;
    if(!found){r.status=pl21::Status::Absent;return r.status;}
    void* row=nullptr;void* owner=nullptr;void* secondary=nullptr;std::uint64_t key=0;int count=0;unsigned flags=0;
    // Resolve ORIGINAL item first. A changed/absent WUID cannot justify even an
    // old wrapper read, much less destruction or source reconstruction.
    if(found!=r.item||!Vft13(found,0x3A2DA88)||!AmmoTransientReadT20(found,0x30,key)||key!=r.key||
       !AmmoTransientReadT20(found,0x48,row)||row!=r.definition||!PrivateLauncherRowT21(row)||
       !AmmoTransientReadT20(found,0x50,count)||count!=1||
       !AmmoTransientReadT20(found,0x60,flags)||(flags&3u)||
       !AmmoTransientReadT20(found,0x90,owner)||owner!=r.wrapper||
       !AmmoTransientReadT20(found,0x98,secondary)||secondary||!PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
    void* current=nullptr;
    if(!Vft13(r.wrapper,0x3A5C838)||!AmmoTransientReadT20(r.wrapper,8,current)||current!=found||
       !PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
    item=found;if(requireClean)r.status=pl21::Status::Ready;return pl21::Status::Ready;
}
static pl21::Status PrivateLauncherObserveT21(const native_script_t2::Context& ctx,
                                             PrivateLauncherRecordT21& r,void*& item) noexcept {
    return PrivateLauncherItemK37(ctx,r,item,true);
}
// K37: each entered native effect has its own positive-return receipt.
// Resume only a later NEVER-ENTERED phase. Never repeat a faulted/unknown call.
static bool PrivateLauncherCleanRecordT21(const native_script_t2::Context& ctx,
                                          PrivateLauncherRecordT21& r) noexcept {
    if(r.recordClean)return true;
    if(!r.constructed||!PrivateLauncherRecordCurrentT21(ctx,r))return false;
    if(!r.stringsEntered){
        r.stringsEntered=true;
        __try{Native13<void(__fastcall*)(void*)>(0x1E3DDE0)(r.record.bytes+0xD8);r.stringsReturned=true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    if(!r.stringsReturned||!PrivateLauncherRecordCurrentT21(ctx,r))return false;
    if(!r.treeEntered){
        void* head=nullptr;void* root=nullptr;
        if(!AmmoTransientReadT20(r.record.bytes,0x98,head)||!head||
           !AmmoTransientReadT20(head,8,root)||!PrivateLauncherRecordCurrentT21(ctx,r))return false;
        r.treeHeadK37=head;r.treeEntered=true;
        __try{Native13<void(__fastcall*)(void*,void*,void*)>(0x452EB4)(r.record.bytes+0x98,r.record.bytes+0x98,root);r.treeReturned=true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    if(!r.treeReturned||!PrivateLauncherRecordCurrentT21(ctx,r))return false;
    if(!r.freeEntered){
        void* head=nullptr;
        if(!r.treeHeadK37||!AmmoTransientReadT20(r.record.bytes,0x98,head)||head!=r.treeHeadK37||
           !PrivateLauncherRecordCurrentT21(ctx,r))return false;
        r.freeEntered=true;
        __try{Native13<void(__fastcall*)(void*,std::size_t)>(0x1AB5490)(head,0x30);r.freeReturned=true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    // No read through the tree head after native free, including on re-entry.
    r.recordClean=r.freeReturned;return r.recordClean;
}
static bool PrivateLauncherAllocateT21(const native_script_t2::Context& ctx,PrivateLauncherRecordT21& r) noexcept {
    if(!PrivateLauncherRecordCurrentT21(ctx,r))return false;
    r.wrapperEntered=true;
    __try{r.wrapper=Native13<void*(__fastcall*)()>(0x8D3DC4)();r.wrapperReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return r.wrapper!=nullptr;
}
static bool PrivateLauncherConstructT21(const native_script_t2::Context& ctx,PrivateLauncherRecordT21& r) noexcept {
    if(!PrivateLauncherRecordCurrentT21(ctx,r))return false;
    r.ctorEntered=true;void* returned=nullptr;
    __try{returned=Native13<void*(__fastcall*)(void*,const void*,int,float)>(0x4533E4)(r.record.bytes,r.guid,1,1.f);r.ctorReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    r.constructed=returned==r.record.bytes;return r.constructed;
}
static void PrivateLauncherCreateT21(const native_script_t2::Context& ctx,PrivateLauncherRecordT21& r) noexcept {
    void* empty=nullptr;
    if(!PrivateLauncherRecordCurrentT21(ctx,r)||!Vft13(r.wrapper,0x3A5C838)||
       !AmmoTransientReadT20(r.wrapper,8,empty)||empty||!PrivateLauncherRecordCurrentT21(ctx,r))return;
    r.createEntered=true;
    __try{r.item=Native13<void*(__fastcall*)(void*,const void*,int,bool)>(0x465FC0)(r.wrapper,r.record.bytes,0x1C,false);r.createReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){} // spent; never replay an unknown effect
}
// K37: a qualified original pair may advance known-completed phases after a
// temporary observation/context refusal. Existing unknown native effects stay
// spent. The original wrapper/item are never replaced or reconstructed.
static pl21::Status PrivateLauncherContinueK37(const native_script_t2::Context& ctx,
                                               PrivateLauncherRecordT21& r) noexcept {
    if(r.status==pl21::Status::Quarantined||r.status==pl21::Status::Absent)return r.status;
    if(!PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
    // A creation record whose cleanup began can never be reused to create a
    // native item, including a retained record from an older implementation.
    if(!r.createEntered&&(r.stringsEntered||r.treeEntered||r.freeEntered))return pl21::Status::Unknown;
    if(r.recordClean){void* item=nullptr;return PrivateLauncherObserveT21(ctx,r,item);}
    if(!r.wrapperEntered&& !PrivateLauncherAllocateT21(ctx,r))return pl21::Status::Unknown;
    if(!r.wrapperReturned||!r.wrapper)return pl21::Status::Unknown;
    if(!r.createEntered){
        // Until Create is entered this wrapper is exclusively ours, unpublished,
        // and has no destructor in this route. It must still be exactly empty.
        void* empty=nullptr;
        if(!PrivateLauncherRecordCurrentT21(ctx,r)||!Vft13(r.wrapper,0x3A5C838)||
           !AmmoTransientReadT20(r.wrapper,8,empty)||empty||!PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
        if(!r.ctorEntered&& !PrivateLauncherConstructT21(ctx,r))return pl21::Status::Unknown;
        if(!r.ctorReturned||!r.constructed||!PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
        unsigned char guid[16]{};unsigned flags=0;int count=0;float health=0;void* tree=nullptr;
        const bool valid=AmmoTransientReadT20(r.record.bytes,0x50,guid)&&!std::memcmp(guid,r.guid,sizeof(guid))&&
            AmmoTransientReadT20(r.record.bytes,0x60,flags)&&flags==0x25&&AmmoTransientReadT20(r.record.bytes,0x64,count)&&count==1&&
            AmmoTransientReadT20(r.record.bytes,0x68,health)&&health==1.f&&AmmoTransientReadT20(r.record.bytes,0x98,tree)&&tree;
        if(!valid||!PrivateLauncherRecordCurrentT21(ctx,r))return pl21::Status::Unknown;
        PrivateLauncherCreateT21(ctx,r);
        // Capture the native key only on the SAME positively current return.
        // If this observation was lost, do not borrow r.item in a later frame.
        if(PrivateLauncherRecordCurrentT21(ctx,r)&&r.createReturned&&Vft13(r.item,0x3A2DA88)){
            std::uint64_t key=0;
            if(AmmoTransientReadT20(r.item,0x30,key)&&(key>>56)==2)r.key=key;
        }
    }
    if(!r.createReturned||!r.key)return pl21::Status::Unknown;
    void* current=nullptr;
    const auto qualified=PrivateLauncherItemK37(ctx,r,current,false);
    if(qualified!=pl21::Status::Ready)return qualified;
    (void)PrivateLauncherCleanRecordT21(ctx,r);
    return PrivateLauncherObserveT21(ctx,r,current);
}
static pl21::Status AmmoPrivateLauncherAcquireT21(const native_script_t2::Context& ctx,void* definition,
                                                 pl21::Lease& lease) noexcept {
    lease={};if(g_private_launcher_active_t21||!PrivateLauncherScopeT21(ctx))return pl21::Status::Refused;
    OwnerTransaction19c4 tx;if(!tx)return pl21::Status::Refused;
    struct Guard {Guard(){g_private_launcher_active_t21=true;}~Guard(){g_private_launcher_active_t21=false;}} guard;
    void* manager=AmmoTransientItemManagerT20();void* world=WorldInventoryT1();
    if(!manager||!world||!Vft13(world,0x3A83DC0)||!PrivateLauncherScopeT21(ctx))return pl21::Status::Refused;
    PrivateLauncherQuarantineOthersT21(manager,world);
    PrivateLauncherRecordT21* slot=nullptr;
    for(auto& r:g_private_launchers_t21){
        if(r.token&&r.itemManager==manager&&r.world==world){
            if(definition!=r.definition)return pl21::Status::Refused;
            const auto status=PrivateLauncherContinueK37(ctx,r);lease=PrivateLauncherLeaseT21(r);
            lease.status=status;return status; // spent pair, even absent/unknown
        }
        if(!r.token&&!slot)slot=&r;
    }
    if(!slot)return pl21::Status::Capacity;
    if(!PrivateLauncherRegistryT21(ctx,definition)||AmmoTransientItemManagerT20()!=manager||WorldInventoryT1()!=world)
        return pl21::Status::Refused;
    auto& r=*slot;r.token=static_cast<std::uint64_t>(slot-g_private_launchers_t21)+1;
    r.itemManager=manager;r.world=world;r.definition=definition;
    std::memcpy(r.guid,pl21::launcherGuid,sizeof(r.guid)); // spend pair BEFORE ctor
    const auto status=PrivateLauncherContinueK37(ctx,r);lease=PrivateLauncherLeaseT21(r);
    lease.status=status;return status;
}
static pl21::Status AmmoPrivateLauncherLookupT21(const native_script_t2::Context& ctx,const pl21::Lease& lease,
                                                void*& item) noexcept {
    item=nullptr;if(g_private_launcher_active_t21||!PrivateLauncherScopeT21(ctx))return pl21::Status::Refused;
    OwnerTransaction19c4 tx;if(!tx)return pl21::Status::Refused;
    if(!lease.token||lease.token>pl21::capacity)return pl21::Status::Refused;
    auto& r=g_private_launchers_t21[lease.token-1];
    void* manager=AmmoTransientItemManagerT20();void* world=WorldInventoryT1();
    if(!manager||!world||!Vft13(world,0x3A83DC0))return pl21::Status::Refused;
    PrivateLauncherQuarantineOthersT21(manager,world);
    if(r.token!=lease.token||reinterpret_cast<std::uintptr_t>(r.itemManager)!=lease.itemManager||
       reinterpret_cast<std::uintptr_t>(r.world)!=lease.world||reinterpret_cast<std::uintptr_t>(r.wrapper)!=lease.wrapper||
       reinterpret_cast<std::uintptr_t>(r.definition)!=lease.definition||r.key!=lease.key)return pl21::Status::Refused;
    struct Guard {Guard(){g_private_launcher_active_t21=true;}~Guard(){g_private_launcher_active_t21=false;}} guard;
    return PrivateLauncherObserveT21(ctx,r,item);
}
