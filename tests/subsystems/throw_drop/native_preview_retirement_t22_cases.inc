#ifdef KCD2VR_T22_MODEL_FIXTURE_LEAVES
// Real allocated native-layout objects; no fake address100 is declared a CPlayer.
struct alignas(16) RetireActorMemoryT22 {unsigned char b[0xA00]{};};
static RetireActorMemoryT22 retirePlayerT22{};
static Memory retireManagerT22{},retireSlotsT22{},retireCharacterT22{},retireModelT22{},retireOtherModelT22{};
static Memory retireActorEntityT22{},retireOtherEntityT22{};
static Memory retireBoneT22[2]{},retireObjectT22[2]{};
static const char* retireNamesT22[]{"right-native-ammo","left-native-ammo"};
static bool retireActorActiveT22=false,retireCharacterMissingT22=false,retireBindingDelayedT22=false;
static bool retireModelFaultBeforeT22=false,retireModelFaultAfterT22=false,retireBindingUnknownT22=false;
static bool retireDenyActorsT22=false;
static bool retireActorBoundaryRevokedT22=false;
static int retireActorLookupT22=0,retireActorResolveMutationT22=0;
static int retireFindMutationT22=0;
static unsigned retireModelOnlyCallsT22=0,retireModelEffectsT22=0;
static bool ModelReadDeniedT22(const void* value,std::size_t size){
    const bool all=worldChanged||liveContext!=&ctx||retireActorBoundaryRevokedT22||retireActorLookupT22==1||retireActorLookupT22==2;
    if(!retireDenyActorsT22||(!all&&retireActorLookupT22!=3))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(value);
    const auto within=[&](const void* p,std::size_t n){const auto b=reinterpret_cast<std::uintptr_t>(p);return a>=b&&a+size<=b+n;};
    if(all&&within(&retirePlayerT22,sizeof(retirePlayerT22)))return true;
    if(all)for(const auto* p:{&retireManagerT22,&retireSlotsT22,&retireCharacterT22,&retireActorEntityT22})if(within(p,sizeof(*p)))return true;
    for(const auto* p:{&retireModelT22,&retireOtherModelT22,
                      &retireBoneT22[0],&retireBoneT22[1],&retireObjectT22[0],&retireObjectT22[1]})if(within(p,sizeof(*p)))return true;
    return false;
}
static AmmoLookupT18 ModelActorResolveT22(unsigned id,void*& entity){
    assert(id==7);entity=nullptr;
    if(retireActorResolveMutationT22){const auto change=retireActorResolveMutationT22;retireActorResolveMutationT22=0;
        if(change==1)worldChanged=true;if(change==2){++ctx.generation;retireActorBoundaryRevokedT22=true;}
    }
    if(retireActorLookupT22==1)return AmmoLookupT18::Unknown;
    if(retireActorLookupT22==2)return AmmoLookupT18::Absent;
    entity=retireActorLookupT22==3?&retireOtherEntityT22:&retireActorEntityT22;
    return AmmoLookupT18::Present;
}
static void* HenryCharacter18() noexcept {return retireActorActiveT22&&!retireCharacterMissingT22?&retireCharacterT22:nullptr;}
static void* NativeFindAttachmentT22(void* value,const char* name){
    if(retireFindMutationT22){const auto change=retireFindMutationT22;retireFindMutationT22=0;
        if(change==1)worldChanged=true;
        if(change==2){++ctx.generation;retireActorBoundaryRevokedT22=true;}
        if(change==3)retireCharacterMissingT22=true;
        if(change==4)Put(&retirePlayerT22,0x1B0,static_cast<void*>(&retireOtherModelT22));
        if(change==5)++retireProbe->identity.serial;
    }
    if(retireBindingUnknownT22||value!=&retireModelT22)return nullptr;
    for(int side=0;side<2;++side)if(name==retireNamesT22[side])return &retireBoneT22[side];
    return nullptr;
}
static void ModelEffectT22(void* player,int side){
    if(!player)return;
    assert(retireActorActiveT22&&player==&retirePlayerT22&&side>=0&&side<2&&retireProbe->modelEntered);
    if(retireModelFaultBeforeT22)NativeFault();
    ++retireModelEffectsT22;
    const auto offset=std::size_t(0x24*side);const auto count=Get<int>(&retireModelT22,0x28+offset);
    // Substitute only exact retail leaf effects, not admission/receipt policy.
    if(count>0){Put(&retireModelT22,0x28+offset,count-1);if(count==1){
        Put(&retireModelT22,0x2C+offset,0u);
        if(!retireBindingDelayedT22)Put(&retireBoneT22[side],0x20,static_cast<void*>(nullptr));
    }}
    if(retireModelFaultAfterT22)NativeFault();
}
static void NativeModelOnlyT22(void* value,int side,bool policy){
    ++retireModelOnlyCallsT22;
    assert(value==&retireModelT22&&!policy&&retireProbe->modelEntered&&!heldPresent);
    if(retireReenter)ReenterRetireT22();
    ModelEffectT22(&retirePlayerT22,side);
}
static void ResetModelFixtureT22(bool enabled=false){
    retireActorActiveT22=enabled;retireCharacterMissingT22=retireBindingDelayedT22=false;
    retireModelFaultBeforeT22=retireModelFaultAfterT22=retireBindingUnknownT22=false;
    retireDenyActorsT22=false;
    retireActorBoundaryRevokedT22=false;
    retireActorLookupT22=retireActorResolveMutationT22=0;
    retireFindMutationT22=0;retireModelOnlyCallsT22=retireModelEffectsT22=0;
    for(auto* value:{&retireManagerT22,&retireSlotsT22,&retireCharacterT22,&retireModelT22,&retireOtherModelT22,
                    &retireBoneT22[0],&retireBoneT22[1],&retireObjectT22[0],&retireObjectT22[1],&retireActorEntityT22,&retireOtherEntityT22})*value={};
    retirePlayerT22={};if(!enabled)return;
    struct Pin {std::uintptr_t rva;unsigned size;unsigned char bytes[40];};static const Pin pins[]{
#include "model_pins_t22.inc"
    };for(const auto& pin:pins)std::memcpy(image+pin.rva,pin.bytes,pin.size);
    Put(image,0x3AA4B98+0x610,WhBase()+0x1A81DD0);
    Put(&retireManagerT22,0,WhBase()+0x3EC9958);Put(&retireManagerT22,0x38,static_cast<void*>(&retirePlayerT22));
    Put(&retirePlayerT22,0,WhBase()+0x3AA4B98);Put(&retirePlayerT22,0x9D0,static_cast<void*>(&retireManagerT22));
    preview.owner.entity=7;Put(&retireActorEntityT22,0,WhBase()+0x4072C78);Put(&retireActorEntityT22,0xC,7u);
    Put(&retireOtherEntityT22,0,WhBase()+0x4072C78);Put(&retireOtherEntityT22,0xC,7u);
    Put(&retirePlayerT22,0x38,static_cast<void*>(&retireActorEntityT22));
    Put(&retirePlayerT22,0x990,static_cast<void*>(&retireSlotsT22));Put(&retirePlayerT22,0x30,preview.owner.entity);
    Put(&retirePlayerT22,0x1B0,static_cast<void*>(&retireModelT22));Put(&retireModelT22,0x20,static_cast<void*>(&retirePlayerT22));
    preview.owner.player=reinterpret_cast<std::uintptr_t>(&retirePlayerT22);
    preview.owner.manager=reinterpret_cast<std::uintptr_t>(&retireManagerT22);
    preview.owner.character=reinterpret_cast<std::uintptr_t>(&retireCharacterT22);
    for(int side=0;side<2;++side){Put(image,0x40AC298+8*side,retireNamesT22[side]);
        Put(&retireModelT22,0x28+0x24*side,1);Put(&retireModelT22,0x2C+0x24*side,9u);
        Put(&retireBoneT22[side],0,WhBase()+0x3A657C0);Put(&retireBoneT22[side],0x20,static_cast<void*>(&retireObjectT22[side]));
        Put(&retireObjectT22[side],0,WhBase()+0x3A68F68);Put(&retireObjectT22[side],8,9u);
    }
}
#else
// Appended to the actual paid adapter fixture at its native-leaf boundary.
// No policy/retirement implementation is reproduced in these cases.
static void ReenterRetireT22(){
    CHECK(retireProbe&&g_preview_retire_active_t22&&g_transient_active20==&preview);
    CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,*retireProbe));
    p20::Receipt nested{};const float velocity[]{0,3,0};
    CHECK(AmmoPaidReleaseT20(ctx,prepared,velocity,nested)==p20::Status::Refused&&!nested.serial);
}
static int Main(){
    image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x5500000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(image);
    pr22::Receipt retired{};
    const auto reset=[&](){Reset();ResetModelFixtureT22();retired={};retireProbe=&retired;retireReenter=retireDenyOld=retireDenyFlight=false;retireDeniedReads=0;lastCleanupPlayer=reinterpret_cast<void*>(-1);};
    reset();CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired));
    CHECK(retired.acknowledged&&retired.entered&&retired.nativeReturned&&cleanupCalls==1&&costCalls==0&&shootCalls==0);
    CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();++ctx.generation;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired));
    CHECK(preview.vmGeneration==3&&ctx.generation==4&&cleanupCalls==1);
    reset();++inventory_lifecycle_t8::loadEpisode18;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired));
    CHECK(preview.load==4&&inventory_lifecycle_t8::loadEpisode18==5&&cleanupCalls==1);
    reset();native_script_t2::Context next=ctx;++next.generation;liveContext=&next;
    CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    CHECK(AmmoRetirePreviewT22(next,preview,nullptr,retired)&&preview.vmGeneration==3);
    reset();auto copied=ctx;CHECK(!AmmoRetirePreviewT22(copied,preview,nullptr,retired)&&cleanupCalls==0);
    reset();inventory_lifecycle_t8::state=1;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    inventory_lifecycle_t8::state=0;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired));
    reset();ownerThread=false;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&retired.acknowledged&&cleanupCalls==1&&costCalls==0&&shootCalls==0);
    reset();transaction=false;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();preview.classGuid[8]^=1;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();worldChanged=retireDenyOld=true;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0&&retireDeniedReads==0);
    reset();Put(image,0x5487A78,static_cast<void*>(&foreignManager));
    CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();Put(&heldEntity,0x18,0u);flagsReadFault=true;g_ammo_category_hook_t20.reset();g_ammo_raw_hook_t20.reset();
    g_create_ammo_hook_t17.reset();maskReady=false;g_native_fault13=true;
    CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();heldPresent=false;retireDenyOld=true;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0&&!retired.entered&&retireDeniedReads==0);
    reset();sourceReplaced=retireDenyOld=true;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0&&retireDeniedReads==0);
    reset();cleanupUnknown=true;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0&&!retired.entered);
    cleanupUnknown=false;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();Put(&payload,0x50,2);CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    for(unsigned flag:{1u,2u}){reset();Put(&payload,0x60,flag);CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);}
    reset();Put(&payload,0x90,static_cast<void*>(&world));CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();Put(&payload,0x98,static_cast<void*>(&inventory));CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();definition.b[8]^=1;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();lookupFault=true;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();Put(&held,0x120,static_cast<void*>(nullptr));CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==0);
    reset();CHECK(Run()==p20::Status::Launched);CHECK(AmmoRetirePreviewT22(ctx,preview,&receipt,retired)&&cleanupCalls==1&&costCalls==1);
    reset();CHECK(Run()==p20::Status::Launched);++ctx.generation;++inventory_lifecycle_t8::loadEpisode18;payloadPresent=false;retireDenyFlight=true;
    CHECK(AmmoRetirePreviewT22(ctx,preview,&receipt,retired)&&cleanupCalls==1&&costCalls==1&&retireDeniedReads==0);
    reset();CHECK(Run()==p20::Status::Launched);receipt.factoryTransferAcknowledged=false;
    CHECK(!AmmoRetirePreviewT22(ctx,preview,&receipt,retired)&&cleanupCalls==0);
    reset();CHECK(Run()==p20::Status::Launched);receipt.originalScopeLost=true;
    CHECK(!AmmoRetirePreviewT22(ctx,preview,&receipt,retired)&&cleanupCalls==0);
    reset();owner=false;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&lastCleanupPlayer==nullptr);
    reset();cleanupFaultBefore=true;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&retired.entered);
    cleanupFaultBefore=false;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    heldPresent=false;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();cleanupFaultAfter=true;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&!retired.nativeReturned);
    reset();cleanupNoop=true;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    cleanupNoop=false;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();cleanupMutation=1;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    for(int change:{2,3,4,5,6,7}){
        reset();cleanupMutation=change;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&!retired.acknowledged);
    }
    // A new current capability may OBSERVE completed cleanup after a VM change;
    // it cannot replay the native call. Original stored identities are unchanged.
    reset();cleanupMutation=2;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&!retired.scopeLost);
    cleanupMutation=0;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&preview.vmGeneration==3);
    reset();cleanupMutation=8;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&retired.scopeLost);
    cleanupMutation=0;CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1);
    reset();retireReenter=true;CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,retired)&&cleanupCalls==1&&costCalls==0);
    for(int change:{1,2,3,4,5,6,7}){
        reset();resolveMutation=change;
        const bool ok=AmmoRetirePreviewT22(ctx,preview,nullptr,retired);
        CHECK(change==1?ok:!ok);CHECK(cleanupCalls==(change==1?1u:0u));
    }
    const auto modelReset=[&](int side=0){reset();preview.side=side;ResetModelFixtureT22(true);};
    const pr22::ModelObligation modelObligation{&retireModelT22,true};
    const auto retireModel=[&](){return AmmoRetirePreviewT22(ctx,preview,nullptr,retired,modelObligation);};
    for(int side:{0,1}){
        modelReset(side);latestPoseAvailable22=false;owner=false;preview.owner.tick=GetTickCount64()-1000;
        const bool completed=retireModel();
        if(!completed)std::fprintf(stderr,"MODEL_RETIRE_FAILURE entityACK=%u modelACK=%u count=%d id=%u cleanup=%u playerNULL=%u\n",
            unsigned(retired.entityAcknowledged),unsigned(retired.modelAcknowledged),Get<int>(&retireModelT22,0x28+0x24*side),
            Get<unsigned>(&retireModelT22,0x2C+0x24*side),cleanupCalls,unsigned(lastCleanupPlayer==nullptr));
        CHECK(completed&&cleanupCalls==1&&lastCleanupPlayer==&retirePlayerT22&&retireModelOnlyCallsT22==0);
        CHECK(retired.entityAcknowledged&&retired.modelAcknowledged&&retired.modelEntered&&retired.modelNativeReturned);
        CHECK(Get<int>(&retireModelT22,0x28+0x24*side)==0&&Get<unsigned>(&retireModelT22,0x2C+0x24*side)==0);
        CHECK(Get<int>(&retireModelT22,0x28+0x24*(1-side))==1&&Get<unsigned>(&retireModelT22,0x2C+0x24*(1-side))==9);
        CHECK(retireModel()&&cleanupCalls==1&&retireModelEffectsT22==1);
    }
    // Causal old entity-only ACK leaves the original native model occupied.
    modelReset();heldPresent=false;pr22::Receipt entityOnly{};
    CHECK(AmmoRetirePreviewT22(ctx,preview,nullptr,entityOnly)&&entityOnly.entityAcknowledged&&
        Get<int>(&retireModelT22,0x28)==1&&Get<unsigned>(&retireModelT22,0x2C)==9&&retireModelOnlyCallsT22==0);
    CHECK(retireModel()&&retireModelOnlyCallsT22==1&&cleanupCalls==0&&retired.entityAcknowledged&&retired.modelAcknowledged);
    CHECK(retireModel()&&retireModelOnlyCallsT22==1);
    modelReset();retireBindingDelayedT22=true;
    CHECK(!retireModel()&&retired.entityAcknowledged&&!retired.modelAcknowledged&&retired.modelEntered&&cleanupCalls==1);
    CHECK(!retireModel()&&cleanupCalls==1&&retireModelOnlyCallsT22==0);
    Put(&retireBoneT22[0],0x20,static_cast<void*>(nullptr));
    CHECK(retireModel()&&cleanupCalls==1&&retireModelOnlyCallsT22==0);
    modelReset();heldPresent=false;retireBindingDelayedT22=true;
    CHECK(!retireModel()&&retired.entityAcknowledged&&!retired.modelAcknowledged&&retireModelOnlyCallsT22==1);
    CHECK(!retireModel()&&retireModelOnlyCallsT22==1);Put(&retireObjectT22[0],8,0u);
    CHECK(retireModel()&&retireModelOnlyCallsT22==1); // semantic bound0, not an invented object write
    for(int count:{-1,0,2,3}){modelReset();Put(&retireModelT22,0x28,count);
        CHECK(!retireModel()&&cleanupCalls==0&&retireModelOnlyCallsT22==0&&!retired.entered&&!retired.modelEntered);}
    modelReset();Put(&retireModelT22,0x2C,10u);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();Put(&retireObjectT22[0],8,10u);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();retireBindingUnknownT22=true;CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();Put(&retireObjectT22[0],0,WhBase()+0x3A657C0);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();Put(&retireBoneT22[0],0,WhBase()+0x3A68F68);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();Put(&retireModelT22,0x28,0);Put(&retireModelT22,0x2C,0u);Put(&retireBoneT22[0],0x20,static_cast<void*>(nullptr));
    CHECK(retireModel()&&cleanupCalls==1&&lastCleanupPlayer==nullptr&&!retired.modelEntered&&retired.modelAcknowledged);
    modelReset();heldPresent=false;Put(&retireModelT22,0x28,0);Put(&retireModelT22,0x2C,0u);Put(&retireBoneT22[0],0x20,static_cast<void*>(nullptr));
    CHECK(retireModel()&&cleanupCalls==0&&retireModelOnlyCallsT22==0);
    for(int bad:{1,2,3,4,5,6,7,8,9,10}){modelReset();
        if(bad==1)Put(&retireManagerT22,0,WhBase()+0x3EC1850);
        if(bad==2)Put(&retirePlayerT22,0,WhBase()+0x3EC9958);
        if(bad==3)Put(&retirePlayerT22,0x9D0,static_cast<void*>(&retireSlotsT22));
        if(bad==4)Put(&retireManagerT22,0x38,static_cast<void*>(&retireModelT22));
        if(bad==5)Put(&retirePlayerT22,0x990,static_cast<void*>(nullptr));
        if(bad==6)Put(&retirePlayerT22,0x30,preview.owner.entity+1);
        if(bad==7)retireCharacterMissingT22=true;
        if(bad==8)Put(&retirePlayerT22,0x1B0,static_cast<void*>(&retireOtherModelT22));
        if(bad==9)Put(&retireModelT22,0x20,static_cast<void*>(&retireManagerT22));
        if(bad==10)Put(&retirePlayerT22,0x38,static_cast<void*>(&retireOtherEntityT22));
        CHECK(!retireModel()&&cleanupCalls==0&&retireModelOnlyCallsT22==0&&!retired.modelEntered);
    }
    modelReset();sourceReplaced=retireDenyOld=true;CHECK(!retireModel()&&retired.entityAcknowledged&&!retired.modelAcknowledged&&cleanupCalls==0&&retireModelOnlyCallsT22==0&&retireDeniedReads==0);
    modelReset();cleanupUnknown=true;CHECK(!retireModel()&&!retired.entityAcknowledged&&cleanupCalls==0&&retireModelOnlyCallsT22==0);
    modelReset();worldChanged=retireDenyOld=true;CHECK(!retireModel()&&cleanupCalls==0&&retireDeniedReads==0);
    modelReset();heldPresent=false;retireModelFaultBeforeT22=true;
    CHECK(!retireModel()&&retired.modelEntered&&!retired.modelNativeReturned&&retireModelOnlyCallsT22==1);
    retireModelFaultBeforeT22=false;CHECK(!retireModel()&&retireModelOnlyCallsT22==1);
    modelReset();heldPresent=false;retireModelFaultAfterT22=true;
    CHECK(retireModel()&&retired.modelEntered&&!retired.modelNativeReturned&&retireModelOnlyCallsT22==1);
    modelReset();retireModelFaultBeforeT22=true;CHECK(!retireModel()&&retired.entered&&retired.modelEntered&&cleanupCalls==1);
    retireModelFaultBeforeT22=false;CHECK(!retireModel()&&cleanupCalls==1&&retireModelOnlyCallsT22==0);
    modelReset();retireModelFaultAfterT22=true;
    CHECK(!retireModel()&&retired.modelEntered&&retired.modelAcknowledged&&!retired.entityAcknowledged&&cleanupCalls==1);
    retireModelFaultAfterT22=false;CHECK(!retireModel()&&cleanupCalls==1&&retireModelOnlyCallsT22==0);
    heldPresent=false;CHECK(retireModel()&&cleanupCalls==1&&retireModelOnlyCallsT22==0);
    modelReset();retireModelFaultAfterT22=true;CHECK(!retireModel()&&retired.modelAcknowledged&&!retired.entityAcknowledged);
    sourceReplaced=true;retireDenyOld=true;
    CHECK(retireModel()&&retired.entityAcknowledged&&retired.modelAcknowledged&&cleanupCalls==1&&retireModelOnlyCallsT22==0&&retireDeniedReads==0);
    for(int change:{1,2,3,4,5}){modelReset();retireFindMutationT22=change;
        CHECK(!retireModel()&&cleanupCalls==0&&retireModelOnlyCallsT22==0&&!retired.entered&&!retired.modelEntered);}
    modelReset();worldChanged=retireDenyActorsT22=true;CHECK(!retireModel()&&retireDeniedReads==0);
    for(int lookup:{1,2,3}){modelReset();retireActorLookupT22=lookup;retireDenyActorsT22=true;
        CHECK(!retireModel()&&retireDeniedReads==0&&cleanupCalls==0&&retireModelOnlyCallsT22==0);}
    for(int change:{1,2}){modelReset();retireActorResolveMutationT22=change;retireDenyActorsT22=true;
        CHECK(!retireModel()&&retireDeniedReads==0&&cleanupCalls==0&&retireModelOnlyCallsT22==0);}
    modelReset();Put(&retireActorEntityT22,0,WhBase()+0x3AA4B98);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();Put(&retireActorEntityT22,0xC,8u);CHECK(!retireModel()&&cleanupCalls==0);
    modelReset();cleanupMutation=4;retireDenyActorsT22=true;CHECK(!retireModel()&&retireDeniedReads==0&&cleanupCalls==1);
    modelReset();retireReenter=true;CHECK(retireModel()&&cleanupCalls==1&&retireModelEffectsT22==1);
    modelReset();heldPresent=false;retireReenter=true;CHECK(retireModel()&&retireModelOnlyCallsT22==1&&retireModelEffectsT22==1);
    modelReset();retireBindingDelayedT22=true;CHECK(!retireModel());
    CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired,{&retireOtherModelT22,true})&&retired.scopeLost);
    modelReset();CHECK(!AmmoRetirePreviewT22(ctx,preview,nullptr,retired,{nullptr,true})&&cleanupCalls==0);
    modelReset();image[0x23D0DCC]^=1;CHECK(!retireModel()&&cleanupCalls==0&&retireModelOnlyCallsT22==0);
    std::printf("{\"checks\":%u,\"passed\":true,\"actual_retirement_inl\":true,\"native_execution\":false}\n",checks);
    VirtualFree(image,0,MEM_RELEASE);return 0;
}
}
int main(){return fixture::Main();}
#endif
