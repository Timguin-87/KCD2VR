// Integrated finite-stock payment/launch provider after T17/T18 + transient
// T20. All native mutations stay on the existing qualified script callback.
namespace paid20=kcd2vr::paid_ammo_t20;
static thread_local paid20::Receipt* g_paid_active20=nullptr;

static bool AmmoPaidNativePinsT20(std::uintptr_t* failedRvaK39=nullptr) noexcept {
    if(!AmmoTransientPinsT20(failedRvaK39))return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    static constexpr Pin pins[]{
      {0x479790,{0x41,0xf7,0xd9,0x45,0x8b,0xc1,0xe8,0xb5,0x37,0x32,1,0xe9,0xd9,0,0,0}},
      {0x179CF50,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x41}},
      {0x179CF7A,{0xe8,0x3d,0,0,0,0x8b,0x53,0x50,0x48,0x8b,0xcb,3,0xd7,0xe8,0xa8,0xbb}},
      {0x179D086,{0x45,0x85,0xc0,0x7e,0x7c,0x4c,0x8b,0xb2,0x98,0,0,0,0x4d,0x85,0xf6,0x74}},
      {0x47982F,{0xe8,0x2c,0xfe,0xff,0xff,0x48,0x89,0x5c,0x24,0x20,0x44,0x8b,0xcd,0x4d,0x85,0xf6}},
      {0x4796A1,{0xe8,0x2a,0x4e,0xa9,1,0x48,0x8b,0x5c,0x24,0x30,0x90,0xc7,0x86,0x60,0x90,0x5b}},
      {0x289F7A0,{0x48,0x8b,0xc4,0x48,0x89,0x58,8,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x57}},
      {0x289F81C,{0xe8,0x6b,0xfc,0xff,0xff,0x48,0x8b,0xf8,0x48,0x85,0xc0,0x0f,0x84,0xb3,1,0}},
      {0x40B730,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57}}
    };
    for(const auto& p:pins)if(!MatchBytes(WhBase()+p.rva,p.bytes,16)){
        if(p.rva==0x289F7A0&&ShootPrologueMatchesD65C(p.bytes,16))continue;
        if(p.rva!=0x40B730||!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase())){
            if(failedRvaK39)*failedRvaK39=p.rva;return false;
        }
    }
    return true;
}
static bool AmmoPaidPinsT20(const char** refusalK39=nullptr,std::uintptr_t* failedRvaK39=nullptr) noexcept {
    if(refusalK39)*refusalK39="READY";
    if(failedRvaK39)*failedRvaK39=0;
    const auto refuse=[&](const char* why) noexcept {if(refusalK39)*refusalK39=why;return false;};
    if(g_native_fault13.load())return refuse("NATIVE_GRASP_FAULT");
    if(!AmmoPaidNativePinsT20(failedRvaK39))return refuse("PAID_NATIVE_PIN");
    if(!g_create_ammo_hook_t17||!g_create_ammo_original_t17||!g_create_ammo_hook_t17->is_enabled())return refuse("FACTORY_HOOK");
    if(!AmmoMaskLeaseT20())return refuse("AMMO_MASK_LEASE");
    if(!g_ammo_category_hook_t20||!g_ammo_category_original_t20||!g_ammo_category_hook_t20->is_enabled())return refuse("CATEGORY_HOOK");
    if(!g_ammo_raw_hook_t20||!g_ammo_raw_original_t20||!g_ammo_raw_hook_t20->is_enabled())return refuse("RAW_CATEGORY_HOOK");
    return true;
}
static bool AmmoPaidFamilyT20(const paid20::Prepared& q) noexcept {
    using namespace kcd2vr::ammo_eligibility_t20;
    constexpr Guid legacy[]{
      {{0x47,0x6e,0x28,0x43,0xe0,0xcd,0xbd,0x50,0xba,0x4c,0x97,0x47,0xb7,0x77,0xb1,0x6d}},
      {{0x88,0x06,0x76,0xc0,0x26,0xf4,0x41,0x53,0x95,0xb9,0x48,0x2b,0x87,0xdb,0xcb,0x9b}}
    };
    if(q.family<0||q.family>1||!q.preview)return false;
    Guid actual{};void* cls=nullptr;int kind=-1;
    const auto expected=q.family==0?dagger:axe;
    if(std::memcmp(q.preview->classGuid,expected.data(),expected.size()))return false;
    if(q.stockRole==paid20::StockRole::RecoveredOwnAmmo){
        // Recovered projectile payload is a native Ammo item, not a melee
        // conversion. It pays one REAL owned unit using the same finite census.
        return Vft13(q.stockDefinition,0x3BA6EE8)&&q.stockDefinition==q.preview->definition&&
            SehCopy(static_cast<unsigned char*>(q.stockDefinition)+8,actual.data(),actual.size())&&actual==expected&&
            AmmoTransientReadT20(q.stockDefinition,0xF0,kind)&&kind==1;
    }
    if(q.stockRole!=paid20::StockRole::LegacyMelee||
       !(Vft13(q.stockDefinition,0x3A4E390)||Vft13(q.stockDefinition,0x3A4D890)))return false;
    return SehCopy(static_cast<unsigned char*>(q.stockDefinition)+8,actual.data(),actual.size())&&(actual==legacy[q.family]||(q.family==0&&actual==Guid{{0x5d,0x0e,0x64,0x3a,0xbd,0xd8,0x8b,0x4e,0xb6,0x1d,0x8c,0xd5,0x18,0x0e,0x79,0xe7}}))&&
        AmmoTransientReadT20(q.stockDefinition,0xE8,cls)&&
        AmmoTransientReadT20(cls,8,kind)&&kind==(q.family==0?0:3);
}
static bool AmmoPaidCurrentSourceT22(const native_script_t2::Context& ctx,const paid20::Prepared& q) noexcept {
    if(!q.currentSource)return AmmoStillSourceT18(q.preview->owner,q.preview->side,GetTickCount64());
    static thread_local bool active=false;
    if(active)return false;
    active=true;bool accepted=false;
    __try{accepted=q.currentSource(ctx,q);}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    active=false;return accepted;
}
static bool AmmoPaidOwnerT20(const native_script_t2::Context& ctx,const paid20::Prepared& q) noexcept {
    const auto* p=q.preview;const auto& r=q.release;const auto now=GetTickCount64();
    return p&&q.currentHand&&p->serial&&p->recordClean&&p->sourceOwned&&p->sourceAbsent&&p->cloneAcknowledged&&p->previewReturned&&
        p->preview.payloadKey&&p->preview.payloadKey!=p->legacyKey&&p->preview.payloadKey!=p->launcherKey&&
        r.generation==p->serial&&r.key==p->legacyKey&&
        r.side==p->side&&r.entity==p->preview.entityId&&r.player==p->owner.player&&r.manager==p->owner.manager&&
        r.character==p->owner.character&&r.bodyEpoch==p->owner.body&&r.requestEpoch==p->owner.request&&
        r.sample&&((q.presentation==paid20::PresentationT26::NativeAttachment&&r.attachment)||
            (q.presentation==paid20::PresentationT26::WorldEntity&&p->owner.chestKnivesT26&&r.side==1&&q.family==0&&!r.attachment))&&ng13::fresh(now,r.tick,80)&&ng13::fresh(now,r.motionTick,80)&&ng13::valid(r.lastAllowed)&&
        ((q.presentation==paid20::PresentationT26::WorldEntity&&kcd2vr::supply_t2::enabled.load()&&
          r.throwEpochH21&&r.throwEpochH21==kcd2vr::collab_h21::throwEpoch.load()&&
          r.physicsEpochH21&&r.physicsEpochH21==kcd2vr::collab_h21::epoch.load())||
         (q.presentation==paid20::PresentationT26::NativeAttachment&&wrt1::CurrentTestH21(r)))&&
        AmmoTransientWorldT20(ctx,*p)&&AmmoPaidPinsT20()&&
        AmmoPaidCurrentSourceT22(ctx,q)&&q.currentHand(q);
}
#include "stock_equipment_d65.inl"
static bool AmmoPaidStockT20(const native_script_t2::Context& ctx,const paid20::Prepared& q,
                               void* inventory,void*& item,int& count) noexcept {
    if(!inventory||!AmmoPaidFamilyT20(q)||
       AmmoTransientLookupT20(ctx,*q.preview,q.release.key,item)!=ta20::Lookup::Present)return false;
    const auto equipment=AmmoEquipmentStateD65C(reinterpret_cast<void*>(q.release.manager),q.release.key);
    void* def=nullptr;void* owner=nullptr;void* secondary=nullptr;void* link=nullptr;unsigned flags=0,id=0;
    return AmmoTransientReadT20(item,0x48,def)&&def==q.stockDefinition&&
        AmmoTransientReadT20(item,0x90,owner)&&owner==inventory&&
        AmmoTransientReadT20(item,0x98,secondary)&&!secondary&&
        AmmoTransientReadT20(item,0x60,flags)&&
        AmmoTransientReadT20(item,0x88,link)&&(!link||(AmmoTransientReadT20(link,0x30,id)&&!id))&&
        AmmoTransientReadT20(item,0x50,count)&&
        AmmoStockPermittedD65C(equipment,flags,count,q.family,q.stockRole);
}
static bool AmmoPaidReceiptJoinT20(const ta20::Receipt& p,const paid20::Receipt& r) noexcept {
    return p.serial==r.serial&&p.side==r.side&&p.legacyKey==r.legacyKey&&p.launcherKey==r.launcherKey&&
        p.owner.manager==r.owner.manager&&p.owner.player==r.owner.player&&p.owner.character==r.owner.character&&
        p.owner.entity==r.owner.entity&&p.owner.body==r.owner.body&&p.owner.request==r.owner.request&&
        p.preview.arrow==r.held.arrow&&p.preview.entity==r.held.entity&&p.preview.entityId==r.held.entityId&&
        p.preview.payloadKey==r.payloadKey&&p.itemManager==r.itemManager&&p.world==r.world&&p.definition==r.ammoDefinition&&
        p.scriptSystem==r.scriptSystem&&p.scriptState==r.scriptState&&p.vmGeneration==r.vmGeneration&&p.load==r.load;
}
static bool AmmoPaidPaymentAckT20(const native_script_t2::Context& ctx,const paid20::Prepared& q,
                                 const paid20::Receipt& r) noexcept {
    // A fault after mutation never authorizes launch from an ambient decrement.
    if(!r.costReturned||!AmmoPaidReceiptJoinT20(*q.preview,r))return false;
    void* after=nullptr;const auto found=AmmoTransientLookupT20(ctx,*q.preview,r.legacyKey,after);
    if(r.beforeCount==1)return found==ta20::Lookup::Absent;
    void* def=nullptr;void* owner=nullptr;void* secondary=nullptr;int count=0;
    return found==ta20::Lookup::Present&&after==r.legacy&&
        AmmoTransientReadT20(after,0x48,def)&&def==r.stockDefinition&&
        AmmoTransientReadT20(after,0x90,owner)&&owner==r.inventory&&
        AmmoTransientReadT20(after,0x98,secondary)&&!secondary&&
        AmmoTransientReadT20(after,0x50,count)&&count==r.beforeCount-1;
}
struct AmmoPaidGuardT20 {
    explicit AmmoPaidGuardT20(paid20::Receipt& r,ta20::Receipt& p) noexcept{g_paid_active20=&r;g_transient_active20=&p;}
    ~AmmoPaidGuardT20(){g_transient_active20=nullptr;g_paid_active20=nullptr;}
};
static void* AmmoPaidManagerT20() noexcept {
    __try{return Native13<void*(__fastcall*)()>(0x189F3C0)();}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
static void AmmoPaidCostT20(paid20::Receipt& r) noexcept {
    r.costEntered=true;
    __try{Native13<void(__fastcall*)(void*,void*,int,int)>(0x479758)(r.inventory,r.legacy,7,1);r.costReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){}
}
struct AmmoPaidFactoryScopeT20 {
    const native_script_t2::Context* ctx=nullptr;
    const paid20::Prepared* prepared=nullptr;
    paid20::Receipt* receipt=nullptr;
    const AmmoLaunchCaptureT17* capture=nullptr;
    void* launcherIdentity=nullptr;
    // Both public receipt objects can be replaced by synchronous callbacks.
    // Pin the identity admitted before original Shoot, not their agreement
    // with each other after a callback has changed both.
    ta20::Receipt previewIdentity{};
    paid20::Receipt paidIdentity{};
};
static bool AmmoPaidFactoryCurrentT20(const AmmoPaidFactoryScopeT20& scope) noexcept {
    if(!scope.ctx||!scope.prepared||!scope.receipt||!scope.capture)return false;
    const auto& ctx=*scope.ctx;const auto& q=*scope.prepared;const auto& r=*scope.receipt;
    const auto& before=scope.paidIdentity;const auto& capture=*scope.capture;
    return q.preview&&g_paid_active20==&r&&g_transient_active20==q.preview&&
        g_ammo_capture_t17==scope.capture&&
        capture.manager==before.shootingManager&&capture.player==reinterpret_cast<void*>(before.owner.player)&&
        capture.launcher==scope.launcherIdentity&&capture.ammo==before.payload&&
        r.payload==before.payload&&r.shootingManager==before.shootingManager&&r.legacy==before.legacy&&
        r.stockDefinition==before.stockDefinition&&r.inventory==before.inventory&&
        r.stockRole==before.stockRole&&r.family==before.family&&r.beforeCount==before.beforeCount&&
        r.costAcknowledged&&r.shootEntered&&
        AmmoPaidReceiptJoinT20(scope.previewIdentity,r)&&AmmoPaidReceiptJoinT20(*q.preview,before)&&
        AmmoTransientWorldT20(ctx,scope.previewIdentity)&&AmmoTransientWorldT20(ctx,*q.preview);
}
static void AmmoPaidFactoryReturnedT20(void* opaque,void* arrow) noexcept {
    auto* scope=static_cast<AmmoPaidFactoryScopeT20*>(opaque);
    if(!scope||!scope->ctx||!scope->prepared||!scope->receipt||!scope->capture)return;
    const auto& ctx=*scope->ctx;const auto& q=*scope->prepared;auto& r=*scope->receipt;
    const auto current=[&]() noexcept {return scope->capture->calls==1&&AmmoPaidFactoryCurrentT20(*scope);};
    if(!current())return;
    r.factoryReturned=true;
    if(!arrow||arrow==r.held.arrow||!Vft13(arrow,0x3ECE6F0))return;
    void* entity=nullptr;unsigned id=0;void* payload=nullptr;void* observed=nullptr;void* oldPayload=nullptr;
    if(!AmmoTransientReadT20(arrow,0x30,id)||!AmmoTransientReadT20(arrow,0x38,entity)||
       !AmmoEntityT18(arrow,entity,id)||!current())return;
    if(!AmmoTransientReadT20(arrow,0x120,payload)||payload!=r.payload||
       !AmmoTransientUnitT20(ctx,*q.preview,r.payloadKey,payload,static_cast<unsigned char*>(arrow)+0x118,observed)||
       !current())return;
    if(!AmmoEntityT18(r.held.arrow,r.held.entity,r.held.entityId)||!current()||
       !AmmoTransientReadT20(r.held.arrow,0x120,oldPayload)||oldPayload||!current())return;
    // A fact about this admitted native transfer, not ongoing hand/pose
    // permission. A menu/mode change cannot erase an irreversible movement.
    // Later contact may change or delete this payload; retain these IDs only
    // as historical identities and re-resolve before any later operation.
    r.flight={arrow,entity,id,r.payloadKey};
    r.factoryTransferAcknowledged=true;r.transferAcknowledged=true;
}
static bool AmmoPaidShootT20(const native_script_t2::Context& ctx,const paid20::Prepared& q,paid20::Receipt& r,void* launcher,
                             const float* direction,const void* offset,float speed) noexcept {
    AmmoLaunchCaptureT17 capture{r.shootingManager,reinterpret_cast<void*>(q.release.player),launcher,r.payload};
    AmmoPaidFactoryScopeT20 scope{&ctx,&q,&r,&capture,launcher,*q.preview,r};
    capture.returnedState=&scope;capture.returnedObserver=&AmmoPaidFactoryReturnedT20;
    r.status=paid20::Status::LaunchUnknown;r.shootEntered=true;g_ammo_capture_t17=&capture;
    __try{
        using Shoot=void(__fastcall*)(void*,void*,unsigned,const float*,const void*,const float*,void*,void*,float,bool);
        Native13<Shoot>(0x289F7A0)(r.shootingManager,reinterpret_cast<void*>(q.release.player),0,direction,offset,
                                  q.release.lastAllowed.p,launcher,r.payload,speed,true);
        r.shootReturned=true;
    }__except(EXCEPTION_EXECUTE_HANDLER){}
    const bool sameOriginal=AmmoPaidFactoryCurrentT20(scope)&&capture.launcher==launcher;
    if(!sameOriginal)r.originalScopeLost=true;
    g_ammo_capture_t17=nullptr;r.factoryCalls=capture.calls;
    // Never overwrite an occurrence-time receipt after contact changed its
    // payload. A successful native return still has a separate status below.
    return sameOriginal;
}
static void AmmoPaidCleanupT20(paid20::Receipt& r,void* player) noexcept {
    r.cleanupId=r.held.entityId;r.cleanupEntered=true;
    __try{Native13<void(__fastcall*)(void*,void*,unsigned*,int)>(0x40B730)(nullptr,player,&r.cleanupId,r.side);r.cleanupReturned=true;}
    __except(EXCEPTION_EXECUTE_HANDLER){}
}
static paid20::Status AmmoPaidReleaseT20(const native_script_t2::Context& ctx,const paid20::Prepared& offered,
                                        const float velocity[3],paid20::Receipt& r) noexcept {
    // Native callbacks may replace the caller's request storage. Keep this
    // invocation's POD immutable; preview storage is separately rejoined below.
    const auto q=offered;
    if(r.serial||g_paid_active20||g_transient_active20||g_ammo_capture_t17||!q.preview||
       q.preview->status!=ta20::Status::Ready||!AmmoPaidOwnerT20(ctx,q))return paid20::Status::Refused;
    OwnerTransaction19c4 transaction;if(!transaction)return paid20::Status::Refused;
    AmmoPaidGuardT20 guard(r,*q.preview);
    double speed2=0;if(!velocity)return paid20::Status::Refused;
    for(int j=0;j<3;++j){if(!std::isfinite(velocity[j]))return paid20::Status::Refused;speed2+=double(velocity[j])*velocity[j];}
    if(speed2>576.01||!AmmoTransientPreviewCurrentT20(ctx,*q.preview)||!AmmoPaidFamilyT20(q))return paid20::Status::Refused;
    void* launcher=nullptr;void* payload=nullptr;void* manager=nullptr;
    if(AmmoTransientLookupT20(ctx,*q.preview,q.preview->launcherKey,launcher)!=ta20::Lookup::Present||
       !AmmoTransientReadT20(q.preview->preview.arrow,0x120,payload)||!AmmoKindsT17(launcher,payload))return paid20::Status::Refused;
    manager=AmmoPaidManagerT20();
    void* inventory=AmmoInventoryT18(q.release.player);
    if(!manager||!inventory||inventory==q.preview->world||!AmmoPaidOwnerT20(ctx,q)||
       !AmmoTransientPreviewCurrentT20(ctx,*q.preview))return paid20::Status::Refused;
    // Native ownership callbacks are finished before the final positive census.
    void* legacy=nullptr;int count=0;
    if(!AmmoPaidStockT20(ctx,q,inventory,legacy,count))return paid20::Status::Refused;
    r.serial=q.preview->serial;r.legacyKey=q.release.key;r.launcherKey=q.preview->launcherKey;
    r.payloadKey=q.preview->preview.payloadKey;r.held=q.preview->preview;r.legacy=legacy;
    r.owner=q.preview->owner;r.side=q.preview->side;
    r.stockDefinition=q.stockDefinition;r.stockRole=q.stockRole;r.inventory=inventory;r.beforeCount=count;r.family=q.family;
    r.payload=payload;r.shootingManager=manager;r.status=paid20::Status::PaymentUnknown;
    r.itemManager=q.preview->itemManager;r.world=q.preview->world;r.ammoDefinition=q.preview->definition;
    r.scriptSystem=q.preview->scriptSystem;r.scriptState=q.preview->scriptState;
    r.vmGeneration=q.preview->vmGeneration;r.load=q.preview->load;
    // Shared preview token is spent BEFORE entry; even another empty Receipt
    // cannot replay this preview after an exception or a synchronous callback.
    q.preview->status=ta20::Status::PreviewNeedsCleanup;AmmoPaidCostT20(r);
    // Never read legacy after entry. Resolve its full WUID, including absence.
    r.costAcknowledged=AmmoPaidPaymentAckT20(ctx,q,r);
    if(!r.costAcknowledged)return r.status;
    r.status=paid20::Status::PaidNoLaunch;
    // Cost callbacks can replace the native shooting singleton or class data.
    // Rejoin those inputs too; a stale paid attempt is never refunded/retried.
    if(AmmoPaidManagerT20()!=manager||!AmmoPaidReceiptJoinT20(*q.preview,r)||!AmmoPaidOwnerT20(ctx,q)||
       !AmmoPaidFamilyT20(q)||!AmmoTransientPreviewCurrentT20(ctx,*q.preview))return r.status;
    void* currentLauncher=nullptr;
    if(AmmoTransientLookupT20(ctx,*q.preview,r.launcherKey,currentLauncher)!=ta20::Lookup::Present||currentLauncher!=launcher)return r.status;
    const float speed=float(std::sqrt(speed2));float direction[3]{0,1,0};
    if(speed>.0001f)for(int j=0;j<3;++j)direction[j]=velocity[j]/speed;
    struct ShotOffset {float value[3]{};unsigned char valid=0,pad[3]{};} offset;
    static_assert(sizeof(ShotOffset)==16);
    const bool sameOriginal=AmmoPaidShootT20(ctx,q,r,launcher,direction,&offset,speed);
    // Contact is allowed to consume/replace the real payload before Shoot
    // returns. Only the earlier authenticated factory-return receipt proves
    // transfer; current emptiness alone never does. No damage ACK is invented.
    r.status=paid20::Status::LaunchUnknown;
    if(sameOriginal&&r.shootReturned&&r.factoryCalls==1&&r.factoryTransferAcknowledged&&
       AmmoPaidReceiptJoinT20(*q.preview,r)&&AmmoTransientWorldT20(ctx,*q.preview))r.status=paid20::Status::Launched;
    return r.status; // No refund/retry on any paid or uncertain result.
}

static bool AmmoPaidRetirePreviewT20(const native_script_t2::Context& ctx,ta20::Receipt& p,
                                    paid20::Receipt& r) noexcept {
    if(p.side<0||p.side>1||!r.serial||r.originalScopeLost||!AmmoPaidReceiptJoinT20(p,r)||
       g_paid_active20||g_transient_active20||g_ammo_capture_t17)return false;
    if(r.cleanupAcknowledged)return true;
    // Cleanup is a retained native obligation, independent of selection/mask
    // readiness; disarming ordinary-bow protection must never strand a preview.
    OwnerTransaction19c4 transaction;if(!transaction||!AmmoTransientWorldT20(ctx,p)||!AmmoPaidNativePinsT20())return false;
    AmmoPaidGuardT20 guard(r,p);
    void* entity=nullptr;auto lookup=AmmoResolveT18(r.held.entityId,entity);
    if(!AmmoPaidReceiptJoinT20(p,r)||!AmmoTransientWorldT20(ctx,p))return false;
    if(lookup==AmmoLookupT18::Absent||(lookup==AmmoLookupT18::Present&&entity!=r.held.entity)){
        r.cleanupAcknowledged=true;return true;
    }
    if(r.cleanupEntered||lookup!=AmmoLookupT18::Present||!AmmoEntityT18(r.held.arrow,r.held.entity,r.held.entityId))return false;
    void* payload=nullptr;if(!AmmoTransientReadT20(r.held.arrow,0x120,payload))return false;
    if(payload){void* observed=nullptr;
        if(!AmmoTransientUnitT20(ctx,p,r.payloadKey,payload,static_cast<unsigned char*>(r.held.arrow)+0x118,observed))return false;
    }else if(!r.transferAcknowledged)return false;
    // Empty source is admitted only by the prior exact native transfer receipt.
    // Flight may since hit/break/disappear; cleanup never dereferences it.
    unsigned bound=~0u;void* player=nullptr;void* model=AmmoModelT18(r.owner);
    if(!AmmoPaidReceiptJoinT20(p,r)||!AmmoTransientWorldT20(ctx,p))return false;
    if(model&&Rd<unsigned>(model,0x2C+0x24*r.side,0)==r.held.entityId&&
       AmmoAttachmentT18(model,r.side,bound)&&(bound==0||bound==r.held.entityId))player=reinterpret_cast<void*>(r.owner.player);
    if(!AmmoPaidReceiptJoinT20(p,r)||!AmmoTransientWorldT20(ctx,p))return false;
    AmmoPaidCleanupT20(r,player);
    if(!AmmoPaidReceiptJoinT20(p,r)||!AmmoTransientWorldT20(ctx,p))return false;
    entity=nullptr;lookup=AmmoResolveT18(r.held.entityId,entity);
    if(!AmmoPaidReceiptJoinT20(p,r)||!AmmoTransientWorldT20(ctx,p))return false;
    r.cleanupAcknowledged=lookup==AmmoLookupT18::Absent||(lookup==AmmoLookupT18::Present&&entity!=r.held.entity);
    return r.cleanupAcknowledged;
}
