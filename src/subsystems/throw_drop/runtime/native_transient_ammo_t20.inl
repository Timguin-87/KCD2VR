// Integrated T22 transient-preview provider after existing T17/T18 adapters,
// in the SAME action-owner scope. No standalone executor, hook or registration.
// Header native_transient_ammo_t20.h must already be included at TU top level.
namespace ta20=kcd2vr::transient_ammo_t20;
static thread_local ta20::Receipt* g_transient_active20=nullptr;

static bool AmmoTransientPinsT20(std::uintptr_t* failedRvaK39=nullptr) noexcept {
    if(failedRvaK39)*failedRvaK39=0;
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
      {0x4533E4,{0x40,0x53,0x48,0x83,0xec,0x20,0x0f,0x10,0x05,0x87,0xb2,0xc4,0x03,0x48,0x8b,0x05}},
      {0x465FC0,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x41}},
      {0x1E3DDE0,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x48,0x83,0xc1,0x18,0xe8,0x32,0xe8}},
      {0x452EB4,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x41}},
      {0x1AB5490,{0x48,0x89,0x54,0x24,0x10,0x48,0x89,0x4c,0x24,8,0x48,0x83,0xec,0x28,0x48,0x81}},
      {0x479758,{0x48,0x8b,0xc4,0x48,0x89,0x58,8,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48}},
      {0x23D09E0,{0x48,0x89,0x54,0x24,0x10,0x48,0x83,0xec,0x28,0x48,0x81,0xc1,0x68,1,0,0}},
      {0xBEAB7E,{0x48,0x8d,0x05,0xcb,0x6c,0x2d,0x03,0x83,0xcf,0xff,0x48,0x89,0x01,0x48,0x8b,0xd9}},
      {0xBE8A63,{0x48,0x8b,0xc8,0xe8,0xfd,0x20,0x00,0x00,0xeb,0x03,0x48,0x8b,0xc3,0xb9,0x78,0x02}},
      {0xBE8A75,{0x48,0x89,0x87,0xc8,0x00,0x00,0x00,0x48,0x89,0x9f,0xd0,0x00,0x00,0x00,0xe8,0x38}},
      {0xBE8E6C,{0x48,0x8b,0x87,0xc8,0x00,0x00,0x00,0x48,0x8b,0x6c,0x24,0x38,0x48,0x8b,0x74,0x24}},
      {0xBE8E84,{0x48,0x89,0x05,0xed,0xeb,0x89,0x04,0x48,0x8b,0xc7,0x48,0x8b,0x7c,0x24,0x48,0x48}},
      {0x189F3C0,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0x1d,0xc3,0x55,0xbe,0x03,0x90,0x48,0x85}},
      {0x469590,{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0xe8,0x17,0,0,0,0x8b,0xd3,0x48}},
      {0x838AE0,{0x32,0xc0,0xc3,0xcc,0x48,0x83,0xec,0x28,0xe8,0xbb,0xb9,0xe0,0xff,0x48,0x85,0xc0}},
      {0x289F4BA,{0xe8,0x09,0x61,0x07,0xfe,0x49,0x8b,0xcc,0x4c,0x8b,0x90,0xe0,0,0,0,0x49}},
      {0x289F5A6,{0x44,0x8a,0x4d,0x58,0x4d,0x8d,0x87,0x18,1,0,0,0x8a,0x45,0x50,0x41,0x80}}
    };
    for(const auto& p:pins)if(!MatchBytes(b+p.rva,p.bytes,sizeof(p.bytes))){
        if(failedRvaK39)*failedRvaK39=p.rva;return false;
    }
    // World insertion cannot merge the newly created source with existing stock.
    // Qualify the actual live vtable leaf as well as the object vft identity.
    std::uintptr_t policy=0;
    const bool ok=SehCopy(reinterpret_cast<void*>(b+0x3A83DC0+0x80),&policy,sizeof(policy))&&policy==b+0x838AE0;
    if(!ok&&failedRvaK39)*failedRvaK39=0x3A83E40;return ok;
}
// These pins qualify only NEW preview eligibility, never retained cleanup.
// Native Init sets bit15; GetFlags reads DWORD CEntity+18; global SaveEntities
// calls CanSerialize, whose first gate refuses this actual native bit.
static bool AmmoTransientNoSavePinsT20() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
      {0x1A72850,{0x8b,0x41,0x18,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc}},
      {0x2AA7B57,{0x48,0x8b,0x5f,0x38,0x48,0x8b,0xcb,0x48,0x8b,0x03,0xff,0x50,0x30,0x8b,0xf0,0x48}},
      {0x2AA7C5D,{0x0f,0xba,0xee,0x0f,0x48,0x8b,0xcb,0x48,0x8b,0x10,0x48,0x89,0x57,0x60,0x48,0x8b}},
      {0xD59E04,{0xf7,0x41,0x18,0x00,0x80,0x00,0x00,0x74,0x03,0x32,0xc0,0xc3,0x8b,0x41,0x08,0x8b}},
      {0x37148FF,{0x49,0x8b,0x06,0x49,0x8b,0xce,0xff,0x90,0x48,0x04,0x00,0x00,0x84,0xc0,0x0f,0x84}},
      {0x1451AC7,{0x48,0x8d,0x15,0x0a,0xe6,0xc1,0x02,0xc7,0x45,0x30,0x00,0x80,0x00,0x00,0xe8,0x82}}
    };
    for(const auto& p:pins)if(!MatchBytes(b+p.rva,p.bytes,16))return false;
    std::uintptr_t target=0;
    return SehCopy(reinterpret_cast<void*>(b+0x3ECE6F0+0x38),&target,8)&&target==b+0x2AA7B20&&
        SehCopy(reinterpret_cast<void*>(b+0x4072C78+0x30),&target,8)&&target==b+0x1A72850&&
        SehCopy(reinterpret_cast<void*>(b+0x4072C78+0x448),&target,8)&&target==b+0xD59E04;
}
template<class T>static bool AmmoTransientReadT20(const void* p,std::size_t off,T& out) noexcept {
    return p&&SehCopy(static_cast<const unsigned char*>(p)+off,&out,sizeof(out));
}
// Exact C_ItemManager: ctorBEAB68 sets3EC1850, EntityModule stores it+C8
// (BE8A75), then publishes that field to global5487A78 (BE8E6C/BE8E84).
// EntityModule primary3EAA180 is a DIFFERENT object and must be refused.
static void* AmmoTransientItemManagerT20() noexcept {
    void* p=nullptr;
    return AmmoTransientReadT20(reinterpret_cast<void*>(WhBase()),0x5487A78,p)&&Vft13(p,0x3EC1850)?p:nullptr;
}
static bool AmmoTransientScopeT20(const native_script_t2::Context& ctx) noexcept {
    // K37: exact current native script callback owns these inventory operations.
    // Henry's historical animation-worker ID is a different scheduling domain.
    // Current() still proves this stack, VM, generation and actual script thread.
    return ctx.Current()&&g_native_ready13.load()&&
        !(inventory_lifecycle_t8::state.load()&1)&&AmmoTransientPinsT20();
}
static bool AmmoTransientWorldT20(const native_script_t2::Context& ctx,const ta20::Receipt& r) noexcept {
    return AmmoTransientScopeT20(ctx)&&ctx.system==r.scriptSystem&&ctx.state==r.scriptState&&
        ctx.generation==r.vmGeneration&&inventory_lifecycle_t8::loadEpisode18.load()==r.load&&
        AmmoTransientItemManagerT20()==r.itemManager&&WorldInventoryT1()==r.world&&Vft13(r.world,0x3A83DC0);
}
static bool AmmoTransientCurrentOwnerT22(const native_script_t2::Context& ctx,const ta20::Request& q) noexcept {
    if(!q.currentOwner)return AmmoStillSourceT18(q.owner,q.side,GetTickCount64());
    static thread_local bool active=false;
    if(active)return false;
    active=true;bool accepted=false;
    __try{accepted=q.currentOwner(ctx,q);}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    active=false;return accepted;
}
static bool AmmoTransientOwnerT20(const native_script_t2::Context& ctx,const ta20::Request& q) noexcept {
    return AmmoTransientScopeT20(ctx)&&g_context13.load()&&q.serial&&q.legacyKey&&q.launcherKey&&q.legacyKey!=q.launcherKey&&
        (q.side==0||q.side==1)&&AmmoTransientCurrentOwnerT22(ctx,q);
}
static ta20::Lookup AmmoTransientLookupT20(const native_script_t2::Context& ctx,const ta20::Receipt& r,
                                         std::uint64_t key,void*& item) noexcept {
    item=nullptr;if((key>>56)!=2||!AmmoTransientWorldT20(ctx,r))return ta20::Lookup::Unknown;
    __try{item=Native13<void*(__fastcall*)(void*,std::uint64_t)>(0x23D09E0)(r.itemManager,key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return ta20::Lookup::Unknown;}
    if(!AmmoTransientWorldT20(ctx,r))return ta20::Lookup::Unknown;
    if(!item)return ta20::Lookup::Absent; // positive native call, same manager/world
    std::uint64_t actual=0;
    return Vft13(item,0x3A2DA88)&&AmmoTransientReadT20(item,0x30,actual)&&actual==key?
        ta20::Lookup::Present:ta20::Lookup::Unknown;
}
static bool AmmoTransientUnitT20(const native_script_t2::Context& ctx,const ta20::Receipt& r,
                                std::uint64_t key,void* expected,void* owner,void*& item) noexcept {
    void* def=nullptr;void* inventory=nullptr;void* secondary=nullptr;int count=0;
    return AmmoTransientLookupT20(ctx,r,key,item)==ta20::Lookup::Present&&item==expected&&
        AmmoTransientReadT20(item,0x48,def)&&def==r.definition&&
        AmmoTransientReadT20(item,0x50,count)&&count==1&&
        AmmoTransientReadT20(item,0x90,inventory)&&inventory==owner&&
        AmmoTransientReadT20(item,0x98,secondary)&&!secondary;
}
static bool AmmoTransientDefinitionT20(const ta20::Receipt& r) noexcept {
    unsigned char guid[16]{};
    return Vft13(r.definition,0x3BA6EE8)&&SehCopy(static_cast<unsigned char*>(r.definition)+8,guid,sizeof(guid))&&
        !std::memcmp(guid,r.classGuid,sizeof(guid));
}
// Qualified unfired provenance is the original full-factory(false,false)
// preview receipt. No guessed CArrow fired-state field is read or written.
static bool AmmoTransientPreviewNoSaveT20(const native_script_t2::Context& ctx,const ta20::Receipt& r,
                                         ta20::PreviewSaveRead* evidence=nullptr) noexcept {
    ta20::PreviewSaveRead observed{};const auto preview=r.preview;const auto serial=r.serial;
    observed.preview=preview;
    const auto finish=[&](bool ok) noexcept {if(evidence)*evidence=observed;return ok;};
    if(!r.previewEntered||!r.previewReturned||!r.cloneAcknowledged||!serial||
       !AmmoTransientWorldT20(ctx,r)||!AmmoTransientNoSavePinsT20()||
       !AmmoEntityT18(preview.arrow,preview.entity,preview.entityId)||!Vft13(preview.entity,0x4072C78))return finish(false);
    unsigned id=0;void* payload=nullptr;void* observedItem=nullptr;
    if(!AmmoTransientReadT20(preview.entity,0xC,id)||id!=preview.entityId||
       !AmmoTransientReadT20(preview.entity,0x18,observed.entityFlags))return finish(false);
    observed.flagsRead=true;
    auto* wrapper=static_cast<unsigned char*>(preview.arrow)+0x118;
    if(!Vft13(wrapper,0x3A5C838)||!AmmoTransientReadT20(preview.arrow,0x120,payload)||
       !AmmoTransientUnitT20(ctx,r,preview.payloadKey,payload,wrapper,observedItem)||
       !AmmoTransientReadT20(payload,0x90,observed.payloadOwner)||
       !AmmoTransientReadT20(payload,0x98,observed.secondaryOwner))return finish(false);
    observed.privateWrapperOwned=observed.payloadOwner==wrapper&&observed.payloadOwner!=r.world&&!observed.secondaryOwner;
    // Lookup/native callbacks may have changed the entity or public receipt.
    // Rejoin the exact original preview and reread the flag after those calls.
    if(!observed.privateWrapperOwned||r.serial!=serial||r.preview.arrow!=preview.arrow||r.preview.entity!=preview.entity||
       r.preview.entityId!=preview.entityId||r.preview.payloadKey!=preview.payloadKey||
       !AmmoTransientWorldT20(ctx,r)||!AmmoEntityT18(preview.arrow,preview.entity,preview.entityId)||
       !Vft13(preview.entity,0x4072C78)||!AmmoTransientReadT20(preview.entity,0xC,id)||id!=preview.entityId)return finish(false);
    observed.flagsRead=AmmoTransientReadT20(preview.entity,0x18,observed.entityFlags);
    observed.nativeNoSave=observed.flagsRead&&(observed.entityFlags&0x8000u)!=0;
    return finish(observed.nativeNoSave);
}
static bool AmmoTransientConstructT20(ta20::Receipt& r) noexcept {
    r.ctorEntered=true; // spent before native allocation/constructor callbacks
    void* out=nullptr;
    __try{out=Native13<void*(__fastcall*)(void*,const void*,int,float)>(0x4533E4)(r.record.bytes,r.classGuid,1,1.f);r.ctorReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    r.constructed=out==r.record.bytes;return r.constructed;
}
static bool AmmoTransientCreateT20(ta20::Receipt& r) noexcept {
    r.createEntered=true;
    __try{r.source=Native13<void*(__fastcall*)(void*,const void*,int,bool)>(0x465FC0)(r.world,r.record.bytes,10,false);r.createReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // NULL is an unknown creation outcome, never a NO_CREATE/retry permission.
    return r.source!=nullptr;
}
static bool AmmoTransientRecordValidT20(const ta20::Receipt& r) noexcept {
    unsigned char guid[16]{};unsigned flags=0;int count=0;float health=0;void* head=nullptr;
    return r.constructed&&SehCopy(r.record.bytes+0x50,guid,sizeof(guid))&&!std::memcmp(guid,r.classGuid,sizeof(guid))&&
        AmmoTransientReadT20(r.record.bytes,0x60,flags)&&flags==0x25&&
        AmmoTransientReadT20(r.record.bytes,0x64,count)&&count==1&&
        AmmoTransientReadT20(r.record.bytes,0x68,health)&&health==1.f&&
        AmmoTransientReadT20(r.record.bytes,0x98,head)&&head;
}
static bool AmmoTransientRecordCleanupT20(const native_script_t2::Context& ctx,ta20::Receipt& r) noexcept {
    if(r.recordClean)return true;
    if(!r.constructed||r.stringsEntered||!AmmoTransientWorldT20(ctx,r))return false;
    r.stringsEntered=true;
    __try{Native13<void(__fastcall*)(void*)>(0x1E3DDE0)(r.record.bytes+0xD8);r.stringsReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    void* head=nullptr;void* root=nullptr;
    if(!AmmoTransientWorldT20(ctx,r)||!AmmoTransientReadT20(r.record.bytes,0x98,head)||
       !head||!AmmoTransientReadT20(head,8,root))return false;
    r.treeEntered=true;
    __try{Native13<void(__fastcall*)(void*,void*,void*)>(0x452EB4)(r.record.bytes+0x98,r.record.bytes+0x98,root);r.treeReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // Native caller reloads record+98 AFTER tree cleanup. Never free a retained
    // stale head if that cleanup changed it or faulted.
    void* after=nullptr;
    if(!AmmoTransientWorldT20(ctx,r)||!AmmoTransientReadT20(r.record.bytes,0x98,after)||after!=head)return false;
    r.freeEntered=true;
    __try{Native13<void(__fastcall*)(void*,std::size_t)>(0x1AB5490)(after,0x30);r.freeReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    r.recordClean=true;return true;
}
static void AmmoTransientRetireT20(const native_script_t2::Context& ctx,ta20::Receipt& r) noexcept {
    if(!r.sourceOwned||!r.sourceKey||r.sourceKey==r.legacyKey||r.sourceKey==r.launcherKey||r.sourceAbsent)return;
    void* item=nullptr;const auto lookup=AmmoTransientLookupT20(ctx,r,r.sourceKey,item);
    if(lookup==ta20::Lookup::Absent){r.sourceAbsent=true;return;}
    if(lookup==ta20::Lookup::Unknown){r.sourceObservationUnknown=true;return;}
    if(r.retireEntered)return; // observation only after any native entry/fault
    if(!AmmoTransientUnitT20(ctx,r,r.sourceKey,r.source,r.world,item))return;
    r.retireEntered=true;
    __try{Native13<void(__fastcall*)(void*,void*,int,int)>(0x479758)(r.world,item,6,1);r.retireReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    // Do not touch item/r.source after native removal. Re-resolve incarnation.
    item=nullptr;const auto after=AmmoTransientLookupT20(ctx,r,r.sourceKey,item);
    if(after==ta20::Lookup::Absent)r.sourceAbsent=true;
    else if(after==ta20::Lookup::Unknown)r.sourceObservationUnknown=true;
}
static bool AmmoTransientCloneT20(const native_script_t2::Context& ctx,const ta20::Request& q,ta20::Receipt& r) noexcept {
    r.boundaryD66="CLONE_CURRENT_OWNER";
    if(g_ammo_capture_t17||!AmmoTransientOwnerT20(ctx,q)||!AmmoTransientWorldT20(ctx,r)||!AmmoTransientDefinitionT20(r))return false;
    r.boundaryD66="CLONE_SOURCE_UNIT";
    void* source=nullptr;
    if(!AmmoTransientUnitT20(ctx,r,r.sourceKey,r.source,r.world,source))return false;
    r.boundaryD66="CLONE_NATIVE_KINDS";
    void* launcher=Item13(q.launcherKey);if(!AmmoKindsT17(launcher,source))return false;
    r.boundaryD66="CLONE_ARROW_MANAGER";
    void* manager=nullptr;
    __try{manager=Native13<void*(__fastcall*)()>(0x189F3C0)();}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!manager||!AmmoTransientOwnerT20(ctx,q))return false;
    // Reuse the current owned T17 original if installed. Otherwise admit only
    // the unmodified retail entry. Never install another hook in this callback.
    r.boundaryD66="CLONE_FACTORY_ROUTE";
    CreateAmmoFnT17 factory=nullptr;
    if(g_create_ammo_hook_t17){if(!g_create_ammo_original_t17||!g_create_ammo_hook_t17->is_enabled())return false;
        factory=g_create_ammo_original_t17;
    }else{
        constexpr unsigned char entry[]{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x55};
        if(!MatchBytes(WhBase()+0x289F48C,entry,sizeof(entry)))return false;
        factory=Native13<CreateAmmoFnT17>(0x289F48C);
    }
    r.boundaryD66="CLONE_NATIVE_CALL";r.previewEntered=true;
    __try{r.preview.arrow=factory(manager,launcher,source,(q.owner.chestKnivesT26&&q.side==1)?nullptr:reinterpret_cast<void*>(q.owner.player),false,false);r.previewReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    r.boundaryD66="CLONE_RETURNED_ENTITY";
    if(!r.preview.arrow||!AmmoTransientWorldT20(ctx,r)||!Vft13(r.preview.arrow,0x3ECE6F0)||
       !AmmoTransientReadT20(r.preview.arrow,0x30,r.preview.entityId)||
       !AmmoTransientReadT20(r.preview.arrow,0x38,r.preview.entity)||
       !AmmoEntityT18(r.preview.arrow,r.preview.entity,r.preview.entityId))return false;
    r.boundaryD66="CLONE_RETURNED_PAYLOAD";
    void* payload=nullptr;
    if(!AmmoTransientReadT20(r.preview.arrow,0x120,payload)||!Vft13(payload,0x3A2DA88)||
       !AmmoTransientReadT20(payload,0x30,r.preview.payloadKey)||!r.preview.payloadKey||
       r.preview.payloadKey==r.sourceKey||r.preview.payloadKey==r.legacyKey||r.preview.payloadKey==r.launcherKey)return false;
    r.boundaryD66="CLONE_DISTINCT_UNIT_OWNERS";
    void* observed=nullptr;
    r.cloneAcknowledged=AmmoTransientUnitT20(ctx,r,r.preview.payloadKey,payload,
        static_cast<unsigned char*>(r.preview.arrow)+0x118,observed)&&
        AmmoTransientUnitT20(ctx,r,r.sourceKey,r.source,r.world,source);
    if(r.cloneAcknowledged){r.boundaryD66="CLONE_NATIVE_NO_SAVE";(void)AmmoTransientPreviewNoSaveT20(ctx,r,&r.previewSave);}
    return r.cloneAcknowledged;
}
static bool AmmoTransientPreviewBindingT20(const native_script_t2::Context& ctx,const ta20::Receipt& r) noexcept {
    if(!r.cloneAcknowledged||!r.previewReturned||!AmmoTransientWorldT20(ctx,r)||!AmmoTransientDefinitionT20(r)||
       !AmmoEntityT18(r.preview.arrow,r.preview.entity,r.preview.entityId))return false;
    void* payload=nullptr;void* observed=nullptr;
    return AmmoTransientReadT20(r.preview.arrow,0x120,payload)&&
        AmmoTransientUnitT20(ctx,r,r.preview.payloadKey,payload,static_cast<unsigned char*>(r.preview.arrow)+0x118,observed);
}
static bool AmmoTransientPreviewCurrentT20(const native_script_t2::Context& ctx,const ta20::Receipt& r) noexcept {
    return AmmoTransientPreviewBindingT20(ctx,r)&&AmmoTransientPreviewNoSaveT20(ctx,r);
}
static ta20::Status AmmoTransientOutcomeT20(const native_script_t2::Context& ctx,const ta20::Receipt& r) noexcept {
    if(!r.ctorEntered)return ta20::Status::NoSourceCreated;
    if(!r.recordClean)return ta20::Status::Unknown;
    if(!r.createEntered)return ta20::Status::NoSourceCreated;
    if(!r.createReturned||!r.sourceOwned||!r.sourceAbsent)return ta20::Status::Unknown;
    if(!r.previewEntered)return ta20::Status::SourceRetired;
    return AmmoTransientPreviewBindingT20(ctx,r)?ta20::Status::PreviewNeedsCleanup:ta20::Status::Unknown;
}
struct AmmoTransientGuardT20 {
    explicit AmmoTransientGuardT20(ta20::Receipt& r) noexcept{g_transient_active20=&r;}
    ~AmmoTransientGuardT20(){g_transient_active20=nullptr;}
};
static ta20::Status AmmoTransientReconcileT20(const native_script_t2::Context& ctx,ta20::Receipt& r) noexcept {
    if(!r.serial||g_transient_active20)return ta20::Status::Refused;
    OwnerTransaction19c4 tx;if(!tx||!AmmoTransientWorldT20(ctx,r))return ta20::Status::Unknown;
    AmmoTransientGuardT20 guard(r);
    if(!r.recordClean&&!r.stringsEntered)(void)AmmoTransientRecordCleanupT20(ctx,r);
    AmmoTransientRetireT20(ctx,r);
    // Resource cleanup/native creation may be unknown independently of source
    // retirement. Never erase those obligations or manufacture a Ready preview.
    r.status=AmmoTransientOutcomeT20(ctx,r);
    return r.status;
}
static ta20::Status AmmoTransientPreviewT20(const native_script_t2::Context& ctx,const ta20::Request& q,ta20::Receipt& r) noexcept {
    if(r.serial)return ta20::Status::Refused;
    r.boundaryD66="ENTRY_CURRENT_OWNER";
    if(g_transient_active20||g_ammo_capture_t17||!AmmoTransientOwnerT20(ctx,q))return ta20::Status::Refused;
    r.boundaryD66="OWNER_TRANSACTION";
    OwnerTransaction19c4 tx;if(!tx)return ta20::Status::Refused;
    AmmoTransientGuardT20 guard(r);
    r.boundaryD66="WORLD_AND_DEFINITION";
    void* world=WorldInventoryT1();void* sys=AmmoTransientItemManagerT20();
    void* actorInventory=AmmoInventoryT18(q.owner.player);
    if(!sys||!world||world==actorInventory||!Vft13(world,0x3A83DC0)||!Vft13(q.ammoDefinition,0x3BA6EE8))return ta20::Status::Refused;
    alignas(16) unsigned char guid[16]{};
    if(!SehCopy(static_cast<unsigned char*>(q.ammoDefinition)+8,guid,sizeof(guid)))return ta20::Status::Refused;
    unsigned nonzero=0;for(auto b:guid)nonzero|=b;if(!nonzero)return ta20::Status::Refused;
    // Persistent receipt and side-effect budget exist BEFORE any constructor.
    r.owner=q.owner;r.side=q.side;r.serial=q.serial;r.legacyKey=q.legacyKey;r.launcherKey=q.launcherKey;
    r.scriptSystem=ctx.system;r.scriptState=ctx.state;r.vmGeneration=ctx.generation;
    r.load=inventory_lifecycle_t8::loadEpisode18.load();r.itemManager=sys;r.world=world;r.definition=q.ammoDefinition;
    std::memcpy(r.classGuid,guid,sizeof(guid));r.status=ta20::Status::Unknown;
    r.boundaryD66="BEFORE_CONSTRUCTION_OWNER";
    if(!AmmoTransientOwnerT20(ctx,q)||!AmmoTransientWorldT20(ctx,r)||!AmmoTransientDefinitionT20(r)){
        r.status=ta20::Status::NoSourceCreated;return r.status;
    }
    r.boundaryD66="RECORD_CONSTRUCTOR";
    if(!AmmoTransientConstructT20(r))return r.status;
    r.boundaryD66="CONSTRUCTED_RECORD_AND_OWNER";
    if(AmmoTransientOwnerT20(ctx,q)&&AmmoTransientWorldT20(ctx,r)&&AmmoTransientDefinitionT20(r)&&AmmoTransientRecordValidT20(r)){
        r.boundaryD66="SOURCE_NATIVE_CREATE";(void)AmmoTransientCreateT20(r);
        if(r.createReturned&&r.source&&Vft13(r.source,0x3A2DA88)&&AmmoTransientReadT20(r.source,0x30,r.sourceKey)&&
           r.sourceKey!=r.legacyKey&&r.sourceKey!=r.launcherKey){
            r.boundaryD66="SOURCE_RETURNED_UNIT";
            void* unit=nullptr;r.sourceOwned=AmmoTransientUnitT20(ctx,r,r.sourceKey,r.source,r.world,unit);
        }
    }
    const bool clean=AmmoTransientRecordCleanupT20(ctx,r);
    if(!clean)r.boundaryD66="RECORD_CLEANUP";
    if(clean&&r.sourceOwned){
        r.boundaryD66="BEFORE_CLONE_CURRENT_OWNER";
        if(AmmoTransientOwnerT20(ctx,q))(void)AmmoTransientCloneT20(ctx,q,r);
    }
    // Even if the Henry hand/pose changed during native factory callbacks, this
    // same-stack, same-world cleanup can retire our exact unpaid temporary item.
    AmmoTransientRetireT20(ctx,r);
    r.status=AmmoTransientOutcomeT20(ctx,r);
    if(r.status==ta20::Status::PreviewNeedsCleanup){
        r.boundaryD66="FINAL_CURRENT_OWNER_AND_NO_SAVE";
        if(AmmoTransientOwnerT20(ctx,q)&&AmmoTransientPreviewCurrentT20(ctx,r)){
            r.status=ta20::Status::Ready;r.boundaryD66="READY";
        }
    }
    return r.status;
}
