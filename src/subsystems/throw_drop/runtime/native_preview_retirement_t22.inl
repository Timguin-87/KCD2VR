// Cleanup-only contribution in the existing native script-owner scope.
// No startup registration, second executor, VM rebasing or native save hook.
namespace pr22=kcd2vr::preview_retirement_t22;
static bool g_preview_retire_active_t22=false;
static bool PreviewRetireIdentityT22(const ta20::Receipt& a,const ta20::Receipt& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.side==b.side&&a.legacyKey==b.legacyKey&&a.launcherKey==b.launcherKey&&
        a.itemManager==b.itemManager&&a.world==b.world&&a.definition==b.definition&&
        a.scriptSystem==b.scriptSystem&&a.scriptState==b.scriptState&&a.vmGeneration==b.vmGeneration&&a.load==b.load&&
        !std::memcmp(a.classGuid,b.classGuid,sizeof(a.classGuid))&&
        a.owner.player==b.owner.player&&a.owner.manager==b.owner.manager&&a.owner.character==b.owner.character&&
        a.owner.entity==b.owner.entity&&a.owner.body==b.owner.body&&a.owner.request==b.owner.request&&
        a.preview.arrow==b.preview.arrow&&a.preview.entity==b.preview.entity&&a.preview.entityId==b.preview.entityId&&
        a.preview.payloadKey==b.preview.payloadKey;
}
static bool PreviewRetirePairT22(const native_script_t2::Context& ctx,const ta20::Receipt& p) noexcept {
    // Epochs revoke admission, not native existence. The NEW current same-stack
    // capability may only inspect/retire this original real manager/world pair.
    // K37: exact current native script callback owns these inventory operations.
    // Henry's historical animation-worker ID is a different scheduling domain.
    // Current() still proves this stack, VM, generation and actual script thread.
    return ctx.Current()&&g_native_ready13.load()&&
        !(inventory_lifecycle_t8::state.load()&1)&&AmmoPaidNativePinsT20()&&
        AmmoTransientItemManagerT20()==p.itemManager&&WorldInventoryT1()==p.world&&
        Vft13(p.world,0x3A83DC0)&&ctx.Current();
}
static bool PreviewRetireModelPinsT22() noexcept {
    const auto b=WhBase();if(!retail_build112::Ready(b))return false;
    struct Pin {std::uintptr_t rva;unsigned size;unsigned char bytes[40];};
    static constexpr Pin pins[]{
        {0x23D0DCC,36,{0x48,0x83,0xec,0x38,0x45,0x8a,0xc8,0xc6,0x44,0x24,0x20,1,0x4c,0x63,0xc2,0x48,0x8d,5,0xb6,0xb4,0xcd,1,0x4e,0x8b,4,0xc0,0xe8,0xe1,0x45,0xcb,0xfe,0x48,0x83,0xc4,0x38,0xc3}},
        {0x1085433,16,{0x48,0x63,0xc2,0x45,0x8a,0xf9,0x4d,0x8b,0xf0,0x48,0x8b,0xd9,0x48,0x8d,0x34,0xc0}},
        {0x1085443,8,{0x8b,0x44,0xb1,0x28,0x85,0xc0,0x7f,0x5b}},
        {0x10854A6,9,{0x83,0xe8,1,0x89,0x44,0xb1,0x28,0x75,0x9c}},
        {0x10854AF,8,{0x49,0x8b,0xd6,0xe8,0x7d,0xb2,0x85,0xff}},
        {0x10854C8,8,{0xc7,0x44,0xb3,0x2c,0,0,0,0}},
        {0x10854D0,12,{0x48,0x8b,0xf8,0x48,0x85,0xc0,0x0f,0x84,0x6f,0xff,0xff,0xff}},
        {0x1A81DD0,8,{0x48,0x8b,0x81,0xb0,1,0,0,0xc3}}
    };
    for(const auto& pin:pins)if(!MatchBytes(b+pin.rva,pin.bytes,pin.size))return false;
    std::uintptr_t target=0;
    return SehCopy(reinterpret_cast<void*>(b+0x3AA4B98+0x610),&target,sizeof(target))&&target==b+0x1A81DD0;
}
// Native ownership only: a new pose, grip/UI epoch or wrist is not required to
// finish an old native model obligation. Never nominate another actor/model.
static bool PreviewRetireActorT22(const native_script_t2::Context& ctx,const ta20::Receipt& p,
                                  const pr22::ModelObligation& model,void*& player) noexcept {
    player=nullptr;if(!model.required||!model.model||!p.owner.player||!p.owner.manager||
       !p.owner.character||!p.owner.entity||!PreviewRetirePairT22(ctx,p))return false;
    // Resolve current Henry entity BEFORE following saved actor/model pointers.
    // Registry absence/unknown grants no old-pointer access or cleanup command.
    const auto capability=ctx;
    void* actorEntity=nullptr;
    const auto actorLookup=AmmoResolveT18(static_cast<unsigned>(p.owner.entity),actorEntity);
    if(ctx.system!=capability.system||ctx.state!=capability.state||ctx.generation!=capability.generation||
       !PreviewRetirePairT22(ctx,p)||actorLookup!=AmmoLookupT18::Present||!Vft13(actorEntity,0x4072C78)||
       Rd<unsigned>(actorEntity,0xC,0)!=p.owner.entity||
       !Vft13(reinterpret_cast<void*>(p.owner.player),0x3AA4B98)||
       Rd<void*>(reinterpret_cast<void*>(p.owner.player),0x38,nullptr)!=actorEntity)return false;
    std::uint64_t hand=0;void* currentPlayer=nullptr;
    if(!HandKey13(reinterpret_cast<void*>(p.owner.manager),p.side,hand,&currentPlayer)||
       reinterpret_cast<std::uintptr_t>(currentPlayer)!=p.owner.player||
       Rd<unsigned>(currentPlayer,0x30,0)!=p.owner.entity||
       reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=p.owner.character||
       AmmoModelT18(p.owner)!=model.model||Rd<void*>(model.model,0x20,nullptr)!=currentPlayer||
       !PreviewRetirePairT22(ctx,p))return false;
    player=currentPlayer;return true;
}
enum class PreviewModelStateT22 : unsigned char { Unknown,Owned,Retired };
static PreviewModelStateT22 PreviewRetireModelT22(const native_script_t2::Context& ctx,const ta20::Receipt& p,
                                                const pr22::ModelObligation& model,void*& player) noexcept {
    const auto capability=ctx;
    const auto current=[&]() noexcept {return ctx.system==capability.system&&ctx.state==capability.state&&
        ctx.generation==capability.generation&&PreviewRetirePairT22(ctx,p);};
    player=nullptr;if(!PreviewRetireModelPinsT22()||!PreviewRetireActorT22(ctx,p,model,player))return PreviewModelStateT22::Unknown;
    if(!current())return PreviewModelStateT22::Unknown;
    const auto offset=std::size_t(0x24*p.side);int count=0;unsigned id=~0u,bound=~0u;
    if(!AmmoTransientReadT20(model.model,0x28+offset,count)||!AmmoTransientReadT20(model.model,0x2C+offset,id)||
       !AmmoAttachmentT18(model.model,p.side,bound)||!current())return PreviewModelStateT22::Unknown;
    void* samePlayer=nullptr;int afterCount=0;unsigned afterId=~0u;
    if(!PreviewRetireActorT22(ctx,p,model,samePlayer)||!current()||samePlayer!=player||
       !AmmoTransientReadT20(model.model,0x28+offset,afterCount)||afterCount!=count||
       !AmmoTransientReadT20(model.model,0x2C+offset,afterId)||afterId!=id)return PreviewModelStateT22::Unknown;
    if(count==0&&id==0&&bound==0)return PreviewModelStateT22::Retired;
    return count==1&&id==p.preview.entityId&&(bound==0||bound==p.preview.entityId)?
        PreviewModelStateT22::Owned:PreviewModelStateT22::Unknown;
}
static bool PreviewRetireLookupT22(void* manager,std::uint64_t key,void*& item) noexcept {
    item=nullptr;
    __try{item=Native13<void*(__fastcall*)(void*,std::uint64_t)>(0x23D09E0)(manager,key);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void PreviewRetireCallT22(pr22::Receipt& receipt,void* player,int side) noexcept {
    __try{
        Native13<void(__fastcall*)(void*,void*,unsigned*,int)>(0x40B730)(nullptr,player,&receipt.nativeId,side);
        receipt.nativeReturned=true;
    }__except(EXCEPTION_EXECUTE_HANDLER){} // observe only; never replay
}
static void PreviewRetireModelCallT22(pr22::Receipt& receipt,int side) noexcept {
    __try{
        Native13<void(__fastcall*)(void*,int,bool)>(0x23D0DCC)(receipt.model.model,side,false);
        receipt.modelNativeReturned=true;
    }__except(EXCEPTION_EXECUTE_HANDLER){} // a partially applied decrement is spent
}
static bool AmmoRetirePreviewT22(const native_script_t2::Context& ctx,ta20::Receipt& p,
                                  const paid20::Receipt* paid,pr22::Receipt& receipt,
                                  pr22::ModelObligation model={}) noexcept {
    kcd2vr::ammo_eligibility_t20::Guid family{};
    std::memcpy(family.data(),p.classGuid,family.size());
    if(g_preview_retire_active_t22||g_paid_active20||g_transient_active20||g_ammo_capture_t17||
       kcd2vr::ammo_eligibility_t20::family(family)==kcd2vr::ammo_eligibility_t20::Family::None||
       !p.serial||p.side<0||p.side>1||!p.previewReturned||!p.cloneAcknowledged||
       !p.preview.arrow||!p.preview.entity||!p.preview.entityId||!p.preview.payloadKey||(model.required&&!model.model)||
       !PreviewRetirePairT22(ctx,p))return false;
    OwnerTransaction19c4 transaction;if(!transaction)return false;
    struct Guard {
        explicit Guard(ta20::Receipt& original){g_preview_retire_active_t22=true;g_transient_active20=&original;}
        ~Guard(){g_transient_active20=nullptr;g_preview_retire_active_t22=false;}
    } guard(p);
    if(!receipt.pinned){receipt.identity=p;receipt.model=model;receipt.modelAcknowledged=!model.required;receipt.pinned=true;}
    const auto original=receipt.identity; // immutable across synchronous callbacks
    const auto capability=ctx;
    const auto current=[&]() noexcept {
        if(receipt.scopeLost||!receipt.pinned||!PreviewRetireIdentityT22(original,p)||
           !PreviewRetireIdentityT22(original,receipt.identity)||receipt.model.model!=model.model||
           receipt.model.required!=model.required){receipt.scopeLost=true;return false;}
        return ctx.system==capability.system&&ctx.state==capability.state&&ctx.generation==capability.generation&&
            PreviewRetirePairT22(ctx,original);
    };
    if(!current())return false;
    if(receipt.acknowledged)return true;
    const auto finish=[&]() noexcept {
        receipt.acknowledged=receipt.entityAcknowledged&&receipt.modelAcknowledged;
        return receipt.acknowledged;
    };
    void* entity=nullptr;auto lookup=AmmoResolveT18(original.preview.entityId,entity);
    if(!current())return false;
    const bool absent=lookup==AmmoLookupT18::Absent;
    const bool replaced=lookup==AmmoLookupT18::Present&&entity!=original.preview.entity;
    if(absent||replaced)receipt.entityAcknowledged=true;
    if(lookup==AmmoLookupT18::Unknown)return false;
    // An ID reused for another entity must never be passed to model-only native
    // detach: that leaf resolves the ID before touching the entity's physics.
    if(replaced)return (!model.required||receipt.modelAcknowledged)?finish():false;
    void* player=nullptr;auto modelState=PreviewModelStateT22::Retired;
    if(model.required&&!receipt.modelAcknowledged){
        modelState=PreviewRetireModelT22(ctx,original,model,player);
        if(!current())return false;
        if(modelState==PreviewModelStateT22::Retired)receipt.modelAcknowledged=true;
        else if(modelState!=PreviewModelStateT22::Owned)return false;
    }
    if(absent){
        if(model.required&&!receipt.modelAcknowledged&&!receipt.modelEntered){
            if(!current())return false;
            receipt.modelEntered=true;PreviewRetireModelCallT22(receipt,original.side);
            if(!current())return false;
            void* observedPlayer=nullptr;
            if(PreviewRetireModelT22(ctx,original,model,observedPlayer)==PreviewModelStateT22::Retired){
                if(!current())return false;receipt.modelAcknowledged=true;
            }
        }
        return current()&&finish();
    }
    // Reuse the positively resolved entity. AmmoEntityT18 would repeat a native
    // registry lookup before these same fields; it adds no identity evidence.
    unsigned entityId=0,arrowId=0;void* arrowEntity=nullptr;
    if(receipt.entered||lookup!=AmmoLookupT18::Present||!Vft13(entity,0x4072C78)||
       !AmmoTransientReadT20(entity,0xC,entityId)||entityId!=original.preview.entityId||
       !Vft13(original.preview.arrow,0x3ECE6F0)||!AmmoTransientReadT20(original.preview.arrow,0x30,arrowId)||
       arrowId!=original.preview.entityId||!AmmoTransientReadT20(original.preview.arrow,0x38,arrowEntity)||arrowEntity!=entity)return false;
    if(!current())return false;
    void* payload=nullptr;
    if(!AmmoTransientReadT20(original.preview.arrow,0x120,payload))return false;
    if(payload){
        if((original.preview.payloadKey>>56)!=2)return false;
        void* found=nullptr;
        if(!PreviewRetireLookupT22(original.itemManager,original.preview.payloadKey,found))return false;
        if(!current())return false;
        void* definition=nullptr;void* owner=nullptr;void* secondary=nullptr;
        std::uint64_t key=0;int count=0;unsigned flags=0;unsigned char guid[16]{};
        if(found!=payload||!Vft13(found,0x3A2DA88)||!AmmoTransientReadT20(found,0x30,key)||key!=original.preview.payloadKey||
           !AmmoTransientReadT20(found,0x48,definition)||definition!=original.definition||!Vft13(definition,0x3BA6EE8)||
           !SehCopy(static_cast<unsigned char*>(definition)+8,guid,sizeof(guid))||std::memcmp(guid,original.classGuid,sizeof(guid))||
           !AmmoTransientReadT20(found,0x50,count)||count!=1||
           !AmmoTransientReadT20(found,0x60,flags)||(flags&3u)||
           !AmmoTransientReadT20(found,0x90,owner)||owner!=static_cast<unsigned char*>(original.preview.arrow)+0x118||
           !AmmoTransientReadT20(found,0x98,secondary)||secondary)return false;
    }else{
        if(!paid||paid->originalScopeLost||!paid->factoryTransferAcknowledged||!paid->transferAcknowledged||
           !AmmoPaidReceiptJoinT20(original,*paid))return false;
    }
    if(!current())return false;
    // Requalify the captured native model after payload lookup. No pose/UI
    // freshness is involved, and an already-spent model decrement never replays.
    player=nullptr;
    if(model.required&&!receipt.modelAcknowledged){
        if(receipt.modelEntered)return false;
        modelState=PreviewRetireModelT22(ctx,original,model,player);
        if(!current())return false;
        if(modelState==PreviewModelStateT22::Retired){receipt.modelAcknowledged=true;player=nullptr;}
        else if(modelState!=PreviewModelStateT22::Owned)return false;
    }
    if(!current())return false;
    const bool entersModel=player!=nullptr;
    if(entersModel)receipt.modelEntered=true;
    receipt.nativeId=original.preview.entityId;receipt.entered=true;
    PreviewRetireCallT22(receipt,player,original.side);
    if(!current())return false;
    if(entersModel&&receipt.nativeReturned)receipt.modelNativeReturned=true;
    entity=nullptr;lookup=AmmoResolveT18(original.preview.entityId,entity);
    if(!current())return false;
    if(lookup==AmmoLookupT18::Absent||(lookup==AmmoLookupT18::Present&&entity!=original.preview.entity))receipt.entityAcknowledged=true;
    if(model.required&&!receipt.modelAcknowledged){
        void* observedPlayer=nullptr;
        if(PreviewRetireModelT22(ctx,original,model,observedPlayer)==PreviewModelStateT22::Retired){
            if(!current())return false;receipt.modelAcknowledged=true;
        }
    }
    return current()&&finish();
}
