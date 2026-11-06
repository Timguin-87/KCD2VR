// Included after weapon_tracking.inl. Observers remain read-only. R59.6C2 adds
// one separately qualified F5=2 pickup deferral before the native prologue.
static bool PhysEmpty6698(std::uint64_t key) noexcept;
static void EndInstantPickupRequestD72() noexcept;
namespace il165 = kcd2vr::diagnostics::lifecycle165;
static il165::Queue g_lifecycle_queue165;
static std::atomic<std::uint64_t> g_lifecycle_session165{0},g_lifecycle_until165{0},g_lifecycle_call165{0};
static il165::WatchIndex g_lifecycle_watch165;
static std::atomic<std::uint64_t> g_lifecycle_scope_lost36{0},g_lifecycle_budget36{0},g_lifecycle_budget_lost36{0};
static std::array<unsigned,6> g_lifecycle_failure36{}; // 0 ready; 1 fingerprint; 2 pin; 3 overlap; 4 install
static std::array<std::uintptr_t,6> g_lifecycle_failure_siteH17{};
static std::array<const char*,6> g_lifecycle_failure_stageH17{{"NONE","NONE","NONE","NONE","NONE","NONE"}};
static void LifecycleScope36(il165::Scopes<>& scope,std::uintptr_t sp,const il165::Event& e) noexcept {
    if(!scope.put(sp,e))++g_lifecycle_scope_lost36;
}
static std::atomic<bool> g_lifecycle_ready165{false};
static std::atomic<unsigned> g_lifecycle_coverage175{0};
static std::uintptr_t g_lifecycle_attempted175=0;
static std::atomic<std::uint64_t> g_lifecycle_lab165{0};
static std::atomic<int> g_lifecycle_census165{0},g_lifecycle_weapon168{0};
static std::atomic<std::uint64_t> g_lifecycle_pickup_candidates165{0},g_lifecycle_pickup_henry165{0},g_lifecycle_alchemy_candidates165{0},g_lifecycle_alchemy_henry165{0};
static thread_local il165::Scopes<> g_pickup_scope165,g_alchemy_scope165,g_constraint_scope165,g_physic_param_scope168;
static std::array<std::optional<DetourModKit::hook::Hook>,23> g_lifecycle_hooks165;
static bool LifecycleActive165() noexcept {
    const auto until=g_lifecycle_until165.load(std::memory_order_acquire);
    return until&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64())&&GetTickCount64()<until&&retail_build112::Ready(WhBase());
}
static bool LifecycleHenry165(std::uint32_t id) noexcept {
    auto* entity=g_last_player_entity.load(std::memory_order_acquire);
    std::uintptr_t vf=0;std::uint32_t actual=0;
    return id&&entity&&Read(entity,vf)&&vf==WhBase()+kCEntityVftRva&&
        Read(static_cast<unsigned char*>(entity)+0xC,actual)&&actual==id;
}
static il165::Event LifecycleEvent165(il165::Kind kind,std::uint32_t rva) noexcept {
    il165::Event e{};e.kind=kind;e.rva=rva;e.session=g_lifecycle_session165.load();e.tick=GetTickCount64();return e;
}
static bool LifecycleEntity165(std::uintptr_t entity,std::uint32_t& id) noexcept {
    std::uintptr_t vf=0;return entity&&Read(reinterpret_cast<void*>(entity),vf)&&vf==WhBase()+kCEntityVftRva&&
        Read(reinterpret_cast<void*>(entity+0xC),id)&&id;
}
static void LifecycleWatch165(il165::Event e) noexcept { (void)g_lifecycle_watch165.access(e,true); }
static bool LifecycleWatched165(il165::Event& e) noexcept { return g_lifecycle_watch165.access(e,false); }
static void LifecycleEmit165(il165::Event e) noexcept {
    if(!LifecycleActive165()||e.session!=g_lifecycle_session165.load())return;
    {namespace h4=diagnostics::interaction_cycle_h4;h4::Event out{};out.stage=h4::Stage::Lifecycle;out.tick=e.tick;out.sourceTick=e.sourceTick;out.target=e.target;out.side=e.side;out.entity=e.entity;out.otherTarget=e.actor;out.call=e.call;out.key=e.key;out.code=e.result;out.detail=int(e.kind);out.flags=e.valid;out.value=e.grip;out.point[0]=e.point[0];out.point[1]=e.point[1];out.point[2]=e.point[2];out.action=std::uintptr_t(e.returnBits);
     // related/call are independent lifecycle IDs, not inferred action ownership.
     h4::copy(out.reason,sizeof(out.reason),il165::name(e.kind));h4::copy(out.label,unsigned(std::min(sizeof(out.label),sizeof(e.label))),e.label);h4::observe(out,GetTickCount64());}
    if(g_lifecycle_budget36.fetch_add(1)>=4096){++g_lifecycle_budget_lost36;return;}
    g_lifecycle_queue165.push(e);
}
static void LifecycleItem165(il165::Event& e,void* item) noexcept {
    std::uintptr_t vf=0;
    if(item&&Read(item,vf)&&vf==WhBase()+0x3A2DA88ull){
        e.item=reinterpret_cast<std::uintptr_t>(item);
        (void)Read(static_cast<unsigned char*>(item)+0x30,e.key);
        (void)Read(static_cast<unsigned char*>(item)+0x38,e.meta);
    }
}
using LifecycleContext165=DetourModKit::hook::MidContext;
using LifecycleRegister165=DetourModKit::hook::Gpr;
static std::uintptr_t LReg165(LifecycleContext165& c,LifecycleRegister165 r) noexcept {return DetourModKit::hook::gpr(c,r);}
// Observe the exact retail virtual implementation on entry and the native
// boolean at its own common epilogue, before script marshalling. Never patch the six-byte call/return gap.
static void PickupBegin165(LifecycleContext165& c,bool steal) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);std::uintptr_t ret=0;
    const auto frame=sp+8;g_pickup_scope165.erase(frame);
    if(!Read(reinterpret_cast<void*>(sp),ret)||ret!=WhBase()+(steal?0x2AC0004:0x2AC0087))return;
    ++g_lifecycle_pickup_candidates165;
    auto e=LifecycleEvent165(il165::Kind::PickupRequest,steal?0x2A8A7EC:0x2A8A838);
    e.actor=static_cast<std::uint32_t>(LReg165(c,LifecycleRegister165::Rdx));
    if(!LifecycleHenry165(e.actor))return;
    e.controller=LReg165(c,LifecycleRegister165::Rcx);std::uintptr_t vf=0;void* item=nullptr;
    if(!e.controller||!Read(reinterpret_cast<void*>(e.controller),vf)||vf!=WhBase()+0x3EC85E0||
       !Read(reinterpret_cast<void*>(e.controller+0x38),e.entity)||!LifecycleEntity165(e.entity,e.target))return;
    if(Read(reinterpret_cast<void*>(e.controller+0x58),item))LifecycleItem165(e,item);
    ++g_lifecycle_pickup_henry165;e.action=steal?1:0;e.call=++g_lifecycle_call165;
    LifecycleScope36(g_pickup_scope165,frame,e);LifecycleWatch165(e);LifecycleEmit165(e);
}
static void PickupRequest165(LifecycleContext165& c) noexcept {
    ReadScope101R2 pages;const auto sp=DetourModKit::hook::stack_pointer(c);std::uintptr_t caller=0;
    int instantD72=3;const auto controller=reinterpret_cast<void*>(LReg165(c,LifecycleRegister165::Rcx));
    const auto actor=std::uint32_t(LReg165(c,LifecycleRegister165::Rdx));
    const bool normalCaller=Read(reinterpret_cast<void*>(sp),caller)&&caller==WhBase()+0x2AC0087;
    if(normalCaller)instantD72=InstantPickupRequestD72(controller,actor);
    if(normalCaller&&(instantD72==1||(instantD72==0&&TryStagedPickup596(controller,actor)))){
        // Same-signature return, before the native prologue. False means no
        // inventory pickup occurred; stow later calls the original native path.
        DetourModKit::hook::instruction_pointer(c)=reinterpret_cast<std::uintptr_t>(&DeferredPickup596);return;
    }
    PickupBegin165(c,false);
}
static void StealRequest165(LifecycleContext165& c) noexcept {PickupBegin165(c,true);}
static void PickupEnd165(LifecycleContext165& c,bool steal) noexcept {
    if(!steal)EndInstantPickupRequestD72(); // callback scope ends even with diagnostics disabled
    if(!LifecycleActive165())return;
    // Normal native function pushes RDI and reserves 110h, plus call return.
    // Steal observation is back in its binding with the original caller RSP.
    const auto sp=DetourModKit::hook::stack_pointer(c);const auto frame=steal?sp:sp+0x120;
    if(auto* p=g_pickup_scope165.find(frame,g_lifecycle_session165.load());p&&p->action==(steal?1:0)){
        auto e=*p;e.kind=il165::Kind::PickupReturn;e.rva=steal?0x2AC0004:0x2A8A92C;e.tick=GetTickCount64();
        e.result=steal?-1:static_cast<int>(LReg165(c,LifecycleRegister165::Rax)&255);
        LifecycleEmit165(e); // pre-call identity; no item dereference on return
    }
    g_pickup_scope165.erase(frame);
}
static void PickupReturn165(LifecycleContext165& c) noexcept {PickupEnd165(c,false);}
static void StealReturn165(LifecycleContext165& c) noexcept {PickupEnd165(c,true);}
static il165::Event LifecycleAttachBegin36(void* owner,void* item,int side,std::uint32_t hash) noexcept {
    il165::Event e{};
    if(!LifecycleActive165()||!owner||owner!=g_henry_item_attach_manager.load())return e;
    ReadScope101R2 pages;std::uint64_t live=0;void* player=nullptr;void* entity=nullptr;
    if(!NativeHandKey6697(owner,side==1?1:0,live)||!Read(static_cast<unsigned char*>(owner)+0x38,player)||!player||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||entity!=g_last_player_entity.load())return e;
    e=LifecycleEvent165(il165::Kind::AttachRequest,0x8D00C4);e.side=side;e.eventHash=hash;
    e.call=++g_lifecycle_call165;e.related=live;e.controller=reinterpret_cast<std::uintptr_t>(owner);
    LifecycleItem165(e,item);e.reason=item?0:1;
    if(!item&&side>=0&&side<2&&!PhysEmpty6698(live))e.key=live;
    LifecycleEmit165(e);return e;
}
static void LifecycleAttach165(il165::Event e,int resolvedSide,bool ok) noexcept {
    if(!e.session)return;
    e.tick=GetTickCount64();e.kind=il165::Kind::AttachResult;e.side=resolvedSide;e.result=ok?1:0;
    LifecycleEmit165(e); // copied pre-call item, no post-call item dereference
}
static void HandChange165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const int side=static_cast<int>(LReg165(c,LifecycleRegister165::Rdx));
    auto* manager=g_henry_item_attach_manager.load();std::uint64_t live=0;void* player=nullptr;void* slots=nullptr;void* entity=nullptr;
    if(!NativeHandKey6697(manager,side,live)||live!=LReg165(c,LifecycleRegister165::R9)||
       !Read(static_cast<unsigned char*>(manager)+0x38,player)||!player||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||!entity||entity!=g_last_player_entity.load()||
       !Read(static_cast<unsigned char*>(player)+0x990,slots)||!slots||
       reinterpret_cast<std::uintptr_t>(slots)+0x258!=LReg165(c,LifecycleRegister165::Rsi))return;
    auto e=LifecycleEvent165(il165::Kind::HandLeave,0x8D07B0);e.side=side;e.call=++g_lifecycle_call165;
    e.key=LReg165(c,LifecycleRegister165::R8);if(!PhysEmpty6698(e.key))LifecycleEmit165(e);
    e.kind=il165::Kind::HandEnter;e.key=live;if(!PhysEmpty6698(e.key))LifecycleEmit165(e);
}
static void AlchemyBegin165(LifecycleContext165& c,std::uint32_t rva) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    ++g_lifecycle_alchemy_candidates165;
    auto e=LifecycleEvent165(il165::Kind::AlchemyState,rva);
    e.actor=static_cast<std::uint32_t>(LReg165(c,LifecycleRegister165::R8));
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(!LifecycleHenry165(e.actor)){g_alchemy_scope165.erase(sp);return;}
    ++g_lifecycle_alchemy_henry165;
    e.action=static_cast<int>(LReg165(c,LifecycleRegister165::R9));
    e.call=++g_lifecycle_call165;LifecycleScope36(g_alchemy_scope165,sp,e);
}
static void AlchemyStartBegin165(LifecycleContext165& c) noexcept {AlchemyBegin165(c,0x896C6E);}
static void AlchemyUseBegin165(LifecycleContext165& c) noexcept {AlchemyBegin165(c,0xD5AE0A);}
static void AlchemyQueryBegin165(LifecycleContext165& c) noexcept {AlchemyBegin165(c,0x11EDAD2);}
static void AlchemyRead165(il165::Event& e) noexcept {
    if(!e.controller)return;
    (void)Read(reinterpret_cast<void*>(e.controller+0x2A0),e.state);
    (void)Read(reinterpret_cast<void*>(e.controller+0x2A4),e.previousState);
    // These are raw native enums consumed by +11EDB2C, not invented VR stages.
}
static void AlchemyStart165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(auto* p=g_alchemy_scope165.find(sp,g_lifecycle_session165.load());p&&p->rva==0x896C6E){
        auto e=*p;e.tick=GetTickCount64();e.controller=LReg165(c,LifecycleRegister165::Rcx);e.kind=il165::Kind::AlchemyStart;e.rva=0x896C92;
        // action is the explicit table EntityId, not an item transfer.
        AlchemyRead165(e);LifecycleEmit165(e);
    }
    g_alchemy_scope165.erase(sp);
}
static void AlchemyController165(LifecycleContext165& c,std::uint32_t entry) noexcept {
    if(!LifecycleActive165())return;
    if(auto* p=g_alchemy_scope165.find(DetourModKit::hook::stack_pointer(c),g_lifecycle_session165.load());p&&p->rva==entry)
        p->controller=LReg165(c,LifecycleRegister165::Rax);
}
static void AlchemyUseController165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;
    const auto sp=DetourModKit::hook::stack_pointer(c);std::uintptr_t ret=0;
    if(!Read(reinterpret_cast<void*>(sp),ret)||ret!=WhBase()+0xD5AE33)return;
    if(auto* p=g_alchemy_scope165.find(sp+8,g_lifecycle_session165.load());p&&p->rva==0xD5AE0A)
        p->controller=LReg165(c,LifecycleRegister165::Rcx);
}
static void AlchemyQueryController165(LifecycleContext165& c) noexcept {AlchemyController165(c,0x11EDAD2);}
static il165::Event* AlchemyUseScope165(LifecycleContext165& c) noexcept {
    // D5AD24 pushes RDI and reserves 30h, plus the binding's return address.
    auto* e=g_alchemy_scope165.find(DetourModKit::hook::stack_pointer(c)+0x40,g_lifecycle_session165.load());
    return e&&e->rva==0xD5AE0A&&e->controller==LReg165(c,LifecycleRegister165::Rdi)?e:nullptr;
}
static void AlchemyTarget165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;
    ReadScope101R2 pages;
    if(auto* e=AlchemyUseScope165(c)){
        e->entity=LReg165(c,LifecycleRegister165::Rax);
        if(!LifecycleEntity165(e->entity,e->target)){e->entity=0;e->target=0;return;}
        auto event=*e;event.tick=GetTickCount64();event.kind=il165::Kind::AlchemyTarget;event.rva=0xD5AD55;
        LifecycleWatch165(event);LifecycleEmit165(event);
    }
}
static void AlchemyUse165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    if(auto* p=AlchemyUseScope165(c)){
        auto e=*p;e.tick=GetTickCount64();e.kind=il165::Kind::AlchemyUse;e.rva=0xD5ADAD;
        e.action=static_cast<int>(LReg165(c,LifecycleRegister165::Rbx));e.result=static_cast<int>(LReg165(c,LifecycleRegister165::Rax)&255);
        AlchemyRead165(e);LifecycleEmit165(e);
    }
    g_alchemy_scope165.erase(DetourModKit::hook::stack_pointer(c)+0x40);
}
static void AlchemyState165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(auto* p=g_alchemy_scope165.find(sp,g_lifecycle_session165.load());p&&p->rva==0x11EDAD2&&p->controller){
        auto e=*p;e.rva=0x11EDB09;e.tick=GetTickCount64();(void)Read(reinterpret_cast<void*>(sp+0x40),e.result);AlchemyRead165(e);
        // Native UI queries can run every frame. One per type on change/1s heartbeat.
        static thread_local std::array<il165::Event,21> previous{};
        if(e.action>=0&&e.action<21){auto& old=previous[e.action];
            if(old.session!=e.session||old.controller!=e.controller||old.state!=e.state||old.previousState!=e.previousState||old.result!=e.result||e.tick-old.tick>=1000){
                old=e;LifecycleEmit165(e);
            }
        }
    }
    g_alchemy_scope165.erase(sp);
}
static void Remove165(LifecycleContext165& c,bool accepted) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    auto e=LifecycleEvent165(accepted?il165::Kind::RemoveAcceptedPath:il165::Kind::RemoveRequest,accepted?0x47EEBB:0x47ECBF);
    e.entity=LReg165(c,LifecycleRegister165::Rbx);
    if(!LifecycleEntity165(e.entity,e.target)||!LifecycleWatched165(e))return;
    // Accepted removal path is after the veto callbacks, before deferred cleanup.
    // Neither this event nor a missing entity claims constraint destruction.
    LifecycleEmit165(e);
}
static void RemoveRequest165(LifecycleContext165& c) noexcept {Remove165(c,false);}
static void RemoveAccepted165(LifecycleContext165& c) noexcept {Remove165(c,true);}
static void ConstraintRequest165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    auto e=LifecycleEvent165(il165::Kind::ConstraintRequest,0x37CFD2B);e.entity=LReg165(c,LifecycleRegister165::R15);
    if(!LifecycleEntity165(e.entity,e.target)||!LifecycleWatched165(e)){g_constraint_scope165.erase(sp);return;}
    e.body=LReg165(c,LifecycleRegister165::Rax);e.call=++g_lifecycle_call165;
    std::uintptr_t vf=0;if(e.body&&Read(reinterpret_cast<void*>(e.body),vf)&&vf>=WhBase()&&vf-WhBase()<0x6000000)e.bodyVft=vf-WhBase();
    LifecycleScope36(g_constraint_scope165,sp,e);LifecycleEmit165(e);
}
static void ConstraintReturn165(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(auto* p=g_constraint_scope165.find(sp,g_lifecycle_session165.load())){
        auto e=*p;e.tick=GetTickCount64();e.kind=il165::Kind::ConstraintReturn;e.rva=0x37CFD3C;e.result=static_cast<int>(LReg165(c,LifecycleRegister165::Rax));LifecycleEmit165(e);
    }
    g_constraint_scope165.erase(sp);
}
// V168: separate retail SetPhysicParams wrapper. Types 10 and 11 have a
// different script schema than AddConstraint. We observe native calls only;
// never call a physics vtable, inspect a script table through guessed methods,
// or equate capture end / removal request with actual physics destruction.
static void PhysicParamRequest168(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    const int type=static_cast<int>(LReg165(c,LifecycleRegister165::R9));
    auto e=LifecycleEvent165(il165::Kind::PhysicParamRequest168,0x712094);
    e.entity=LReg165(c,LifecycleRegister165::Rsi);e.action=type;
    if((type!=10&&type!=11)||!LifecycleEntity165(e.entity,e.target)||!LifecycleWatched165(e)){
        g_physic_param_scope168.erase(sp);return;}
    e.body=LReg165(c,LifecycleRegister165::R8);e.controller=LReg165(c,LifecycleRegister165::Rdi);
    std::uintptr_t vf=0;if(!e.body||!Read(reinterpret_cast<void*>(e.body),vf)||vf<WhBase()||vf-WhBase()>=0x6000000){
        g_physic_param_scope168.erase(sp);return;}
    e.bodyVft=vf-WhBase();e.call=++g_lifecycle_call165;
    LifecycleScope36(g_physic_param_scope168,sp,e);LifecycleEmit165(e);
}
static void PhysicParamReturn168(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;
    const auto sp=DetourModKit::hook::stack_pointer(c);
    if(auto* p=g_physic_param_scope168.find(sp,g_lifecycle_session165.load())){
        auto e=*p;e.tick=GetTickCount64();e.kind=il165::Kind::PhysicParamReturn168;e.rva=0x7120AD;
        // The wrapper returns a native int; no entity dereference after original.
        e.result=static_cast<int>(LReg165(c,LifecycleRegister165::Rax));LifecycleEmit165(e);
    }
    g_physic_param_scope168.erase(sp);
}
static il165::Event* PhysicScope36(LifecycleContext165& c) noexcept {
    const auto rbp=LReg165(c,LifecycleRegister165::Rbp);
    auto* e=g_physic_param_scope168.find(rbp+0xAD0,g_lifecycle_session165.load());
    return e&&e->body==LReg165(c,LifecycleRegister165::R14)?e:nullptr;
}
static void PhysicAddSubmit36(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    if(auto* p=PhysicScope36(c);p&&p->action==10){
        auto& e=*p;const auto rbp=LReg165(c,LifecycleRegister165::Rbp);
        e.valid=0;e.reason=10;
        if(Read(reinterpret_cast<void*>(rbp+0x5F8),e.buddy)&&
           Read(reinterpret_cast<void*>(rbp+0x600),e.point)&&
           Read(reinterpret_cast<void*>(rbp+0x618),e.part0)&&
           Read(reinterpret_cast<void*>(rbp+0x61C),e.part1))e.valid=1;
        auto out=e;out.tick=GetTickCount64();out.kind=il165::Kind::PhysicSubmit;out.rva=0xB369E3;LifecycleEmit165(out);
    }
}
static void PhysicRemoveSubmit36(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    if(auto* p=PhysicScope36(c);p&&p->action==11){
        const auto rbp=LReg165(c,LifecycleRegister165::Rbp);
        // RDX is the parsed native action selected by type 11; other branches
        // share this callsite. Require exact local, never an SDK structure.
        if(LReg165(c,LifecycleRegister165::Rdx)!=rbp+0x850)return;
        p->reason=11;p->valid=Read(reinterpret_cast<void*>(rbp+0x854),p->constraintId)?1u:0u;
        auto out=*p;out.tick=GetTickCount64();out.kind=il165::Kind::PhysicSubmit;out.rva=0xB3784D;LifecycleEmit165(out);
    }
}
static void PhysicActionReturn36(LifecycleContext165& c) noexcept {
    if(!LifecycleActive165())return;
    if(auto* p=PhysicScope36(c);p&&p->reason==p->action&&(p->action==10||p->action==11)){
        auto out=*p;out.tick=GetTickCount64();out.kind=il165::Kind::PhysicActionReturn;out.rva=0xB37D81;
        out.result=static_cast<int>(LReg165(c,LifecycleRegister165::Rax));
        if(out.action==10)out.constraintId=static_cast<int>(LReg165(c,LifecycleRegister165::R15));
        LifecycleEmit165(out);p->reason=0; // one observed native submission return
    }
}
// Copies from existing event/snapshot producers. No extra native polling.
static void LifecycleHandSample36(int side,std::uint64_t key,void* item,void* entity,
    std::uint32_t id,const char* name,const float* position) noexcept {
    if(!LifecycleActive165())return;
    auto e=LifecycleEvent165(il165::Kind::HandSample,0);e.side=side;e.key=PhysEmpty6698(key)?0:key;
    if(e.key){LifecycleItem165(e,item);std::uint32_t actual=0;
        if(LifecycleEntity165(reinterpret_cast<std::uintptr_t>(entity),actual)&&actual==id){
            e.entity=reinterpret_cast<std::uintptr_t>(entity);e.target=id;LifecycleWatch165(e);}}
    if(name)std::snprintf(e.label,sizeof(e.label),"%s",name);
    if(position){std::copy_n(position,3,e.point);e.valid=1;}
    LifecycleEmit165(e);
}
static void LifecycleBody36(void* entity,void* body,std::uintptr_t caller) noexcept {
    if(!LifecycleActive165())return;ReadScope101R2 pages;
    if(!g_lifecycle_watch165.possible(reinterpret_cast<std::uintptr_t>(entity)))return;
    auto e=LifecycleEvent165(il165::Kind::BodySample,0x8A49FC);
    e.entity=reinterpret_cast<std::uintptr_t>(entity);
    if(!LifecycleEntity165(e.entity,e.target)||!LifecycleWatched165(e))return;
    e.body=reinterpret_cast<std::uintptr_t>(body);std::uintptr_t vf=0;
    if(body&&Read(body,vf)&&vf>=WhBase()&&vf-WhBase()<0x6000000){e.bodyVft=vf-WhBase();e.valid=1;}
    e.related=caller>=WhBase()&&caller-WhBase()<0x6000000?caller-WhBase():0;
    static thread_local std::array<il165::Event,8> last{};
    auto& old=last[e.target%last.size()];
    if(old.session==e.session&&old.entity==e.entity&&old.body==e.body&&e.tick>=old.tick&&e.tick-old.tick<500)return;
    old=e;LifecycleEmit165(e);
}
static void LifecycleTracking36(const TrackingSnapshot& t,bool gameplay) noexcept {
    if(!LifecycleActive165())return;
    static thread_local std::array<il165::Event,2> last{};
    const auto session=g_lifecycle_session165.load();
    for(int side=0;side<2;++side){auto& old=last[side];
        const float grip=side?t.left_grip:t.right_grip;
        const bool valid=t.head_valid&&(side?t.left_valid:t.right_valid)&&std::isfinite(grip);
        const bool prior=old.session==session&&old.result==1;
        const bool held=valid&&gameplay&&(prior?grip>.42f:grip>=.58f);
        const bool changed=old.session!=session||held!=prior||old.valid!=(unsigned(valid)|(unsigned(gameplay)<<1));
        if(!changed&&(!held||t.tick<old.tick||t.tick-old.tick<500))continue;
        auto e=LifecycleEvent165(il165::Kind::GripSample,0);e.side=side;e.grip=grip;e.sourceTick=t.tick;
        e.related=t.sequence;e.result=held?1:0;e.valid=unsigned(valid)|(unsigned(gameplay)<<1);
        e.reason=old.session!=session?0:(!valid?3:(!gameplay?4:(held&&!prior?1:(!held&&prior?2:5))));
        const auto& pos=side?t.left:t.right;const auto& q=side?t.lq:t.rq;
        e.point[0]=pos.x;e.point[1]=pos.y;e.point[2]=pos.z;
        e.orientation[0]=q.x;e.orientation[1]=q.y;e.orientation[2]=q.z;e.orientation[3]=q.w;
        old=e;LifecycleEmit165(e); // XR coordinates; never labelled an object's world pose
    }
}
struct LifecycleSite165 {std::uintptr_t rva;unsigned char bytes[16];void(*callback)(LifecycleContext165&) noexcept;};
#include "interaction_lifecycle_sites165.inl"
#include "carry_observer59c7g.inl"
static unsigned g_lifecycle_attemptedGroupsD84=0;
static void ShutdownLifecycle165() noexcept {
    g_lifecycle_attemptedGroupsD84=0;
    ShutdownCarryObserver59c7g();
    g_lifecycle_until165=0;g_lifecycle_failure36={};g_lifecycle_failure_siteH17={};g_lifecycle_failure_stageH17.fill("NONE");
    g_lifecycle_ready165=false;g_lifecycle_coverage175=0;g_lifecycle_attempted175=0;
    for(auto& h:g_lifecycle_hooks165)if(h){(void)h->disable();h.reset();}
}
static bool ArmLifecycle165(unsigned requestedD84=0x3fu) noexcept {
    const auto b=WhBase();
    if(!b||!retail_build112::Verify(b)){g_lifecycle_failure36.fill(1);g_lifecycle_failure_stageH17.fill("FINGERPRINT");return false;}
    if(g_lifecycle_attempted175==b&&(g_lifecycle_attemptedGroupsD84&requestedD84)==requestedD84)
        return (g_lifecycle_coverage175.load()&requestedD84)!=0;
    g_lifecycle_attempted175=b; // Each group attempts once; F5 may add observers later.
    using namespace DetourModKit::hook;
    namespace optional=diagnostics::optional175;
    const unsigned mask=optional::arm([&](optional::Group group) noexcept {
        unsigned gi=0;while(optional::lifecycle[gi].first!=group.first)++gi;
        const unsigned bitD84=1u<<gi;
        if(!(requestedD84&bitD84)||(g_lifecycle_attemptedGroupsD84&bitD84))return false;
        g_lifecycle_attemptedGroupsD84|=bitD84;
        for(std::size_t i=group.first;i<group.last;++i){
            const auto& s=kLifecycleSites165[i];unsigned char bytes[16]{};
            for(unsigned j=0;j<16;++j)if(!Read(reinterpret_cast<unsigned char*>(b+s.rva+j),bytes[j])){g_lifecycle_failure36[gi]=2;g_lifecycle_failure_siteH17[gi]=s.rva;g_lifecycle_failure_stageH17[gi]="PIN_READ";return false;}
            if(std::memcmp(bytes,s.bytes,sizeof(bytes))){g_lifecycle_failure36[gi]=2;g_lifecycle_failure_siteH17[gi]=s.rva;g_lifecycle_failure_stageH17[gi]="PIN_BYTES";return false;}
            // Existing mid-hook backend needs room for its absolute patch.
            // Closely spaced pre/post sites must not overwrite one another.
            for(std::size_t j=0;j<i;++j){const auto other=kLifecycleSites165[j].rva;
                if((s.rva>other?s.rva-other:other-s.rva)<16){g_lifecycle_failure36[gi]=3;g_lifecycle_failure_siteH17[gi]=s.rva;g_lifecycle_failure_stageH17[gi]="OVERLAP";return false;}}
        }
        return true;
    },[&](std::size_t i) noexcept {
        const auto& s=kLifecycleSites165[i];Options o{};o.fail_if_already_hooked=true;
        auto h=mid_at(MidRequest{"kcd2vr_lifecycle175_"+std::to_string(i),Target{DetourModKit::Address{b+s.rva}},o},s.callback);
        unsigned gi=0;while(i>=optional::lifecycle[gi].last)++gi;
        if(!h){g_lifecycle_failure36[gi]=4;g_lifecycle_failure_siteH17[gi]=s.rva;g_lifecycle_failure_stageH17[gi]="CREATE";return false;}g_lifecycle_hooks165[i]=std::move(*h);
        const bool enabled=bool(g_lifecycle_hooks165[i]->enable());if(!enabled){g_lifecycle_failure36[gi]=4;g_lifecycle_failure_siteH17[gi]=s.rva;g_lifecycle_failure_stageH17[gi]="ENABLE";}
        return enabled;
    },[&](std::size_t i) noexcept {
        if(g_lifecycle_hooks165[i]){(void)g_lifecycle_hooks165[i]->disable();g_lifecycle_hooks165[i].reset();}
    });
    // Independent observer failure must not disable existing F5 acquisition.
    if(requestedD84!=1u){
        const bool carryReady=ArmCarryObserver59c7g();
        diagnostic_trace::Write(carryReady?"C7G F5_CARRY observer=READY physicsGrab=UNIMPLEMENTED":"C7G F5_CARRY observer=UNAVAILABLE existingF5=UNCHANGED");
    }
    const unsigned combinedD84=g_lifecycle_coverage175.fetch_or(mask)|mask;
    g_lifecycle_ready165=combinedD84!=0;
    return (combinedD84&requestedD84)!=0;
}

static il165::Ledger g_lifecycle_ledger165; // worker/control-thread only
static void ServiceLifecycle165() noexcept {
    il165::Event e{};
    for(int i=0;i<64&&g_lifecycle_queue165.pop(e);++i){
        if(e.session!=g_lifecycle_session165.load())continue;
        const auto* p=g_lifecycle_ledger165.accept(e);
        char m[1800]{};std::snprintf(m,sizeof(m),
            "V66.165 F8_LIFECYCLE session=%llu tick=%llu call=%llu episode=%llu event=%s rva=%X actor=%u target=%u entity=%p item=%p wuid=%016llX meta=%016llX side=%d resultRaw=%d controller=%p state2A0=%d state2A4=%d actionRaw=%d eventHash=%08X body=%p bodyVftRva=%llX related=%llu sourceTick=%llu returnBits=%llX constraintId=%d part0=%d part1=%d buddy=%p valid=%u reason=%d grip=%.3f point=(%.5f,%.5f,%.5f) quat=(%.5f,%.5f,%.5f,%.5f) label=%s episodeTarget=%u episodeWuid=%016llX identityJoin=EXACT_LEDGER writes=NONE",
            (unsigned long long)e.session,(unsigned long long)e.tick,(unsigned long long)e.call,(unsigned long long)(p?p->id:0),il165::name(e.kind),e.rva,e.actor,e.target,
            reinterpret_cast<void*>(e.entity),reinterpret_cast<void*>(e.item),(unsigned long long)e.key,(unsigned long long)e.meta,e.side,e.result,
            reinterpret_cast<void*>(e.controller),e.state,e.previousState,e.action,e.eventHash,reinterpret_cast<void*>(e.body),(unsigned long long)e.bodyVft,(unsigned long long)e.related,(unsigned long long)e.sourceTick,
            (unsigned long long)e.returnBits,e.constraintId,e.part0,e.part1,reinterpret_cast<void*>(e.buddy),e.valid,e.reason,double(e.grip),
            double(e.point[0]),double(e.point[1]),double(e.point[2]),double(e.orientation[0]),double(e.orientation[1]),double(e.orientation[2]),double(e.orientation[3]),e.label[0]?e.label:"NONE",p?p->target:0,(unsigned long long)(p?p->key:0));
        diagnostic_trace::Write(m);
        if(e.kind==il165::Kind::CarrySnapshot59c7g){
            std::snprintf(m,sizeof(m),"C7G F5_CARRY session=%llu controller=%p actor=%u target=%u entity=%p state=%d phase=%s nativeCarrying=%d predicateAgrees=%d source=NATIVE_QUERY_CALLBACK attachment=UNOBSERVED constraint=NONE writes=NONE targetKey=%llu targetKeyDomain=NATIVE_ACTOR_NOT_ENTITY",
                (unsigned long long)e.session,reinterpret_cast<void*>(e.controller),e.actor,e.target,reinterpret_cast<void*>(e.entity),e.state,
                kcd2vr::carry59c7g::phase(e.state),e.result,e.valid?1:0,(unsigned long long)e.key);
            diagnostic_trace::Write(m);
        }
        if(e.kind==il165::Kind::ObjectPose){
            std::snprintf(m,sizeof(m),"R59C1 F5_OBJECT_WORLD session=%llu episode=%llu target=%u wuid=%016llX present=%d validMask=%u activityMask=%d smithPhase=%d matrix=(%.5f,%.5f,%.5f,%.5f;%.5f,%.5f,%.5f,%.5f;%.5f,%.5f,%.5f,%.5f) identity=FRESH_ID_AND_POINTER_MATCH absent=NOT_DESTRUCTION_PROOF writes=NONE",
                (unsigned long long)e.session,(unsigned long long)(p?p->id:0),e.target,(unsigned long long)e.key,e.result,e.valid,e.state,e.action,
                double(e.worldMatrix59c1[0]),double(e.worldMatrix59c1[1]),double(e.worldMatrix59c1[2]),double(e.worldMatrix59c1[3]),
                double(e.worldMatrix59c1[4]),double(e.worldMatrix59c1[5]),double(e.worldMatrix59c1[6]),double(e.worldMatrix59c1[7]),
                double(e.worldMatrix59c1[8]),double(e.worldMatrix59c1[9]),double(e.worldMatrix59c1[10]),double(e.worldMatrix59c1[11]));
            diagnostic_trace::Write(m);
        }
    }
}
static void EndLifecycle165(const char* reason) noexcept {
    diagnostics::combat_capture::interactionOnlyUntil168r1=0;
    if(!g_lifecycle_until165.exchange(0))return;
    // Producers already in flight retain their session; drain bounded snapshots.
    for(int i=0;i<8;++i)ServiceLifecycle165();
    for(const auto& p:g_lifecycle_ledger165.episodes)if(p.session){char m[880]{};
        std::snprintf(m,sizeof(m),"V66.165 F8_LIFECYCLE_SUMMARY session=%llu episode=%llu target=%u wuid=%016llX pickupRequest=%d pickupReturn=%d requestAccepted=%d attachAccepted=%d handPresent=%d handLeave=%d removeRequest=%d removeAcceptedPath=%d constraintRequest=%d constraintReturn=%d paramAddRequest=%d paramAddReturn=%d paramRemoveRequest=%d paramRemoveReturn=%d heldMsObserved=%llu handVisits=%llu physicsRelease=UNPROVEN destruction=UNPROVEN end=%s",
            (unsigned long long)p.session,(unsigned long long)p.id,p.target,(unsigned long long)p.key,p.requested,p.returned,p.accepted,p.attached,p.held,p.released,p.removeRequested,p.removeAccepted,p.constraintRequest,p.constraintReturn,p.paramAddRequest168,p.paramAddReturn168,p.paramRemoveRequest168,p.paramRemoveReturn168,(unsigned long long)p.heldMs,(unsigned long long)p.handVisits,reason);
        diagnostic_trace::Write(m);
        std::snprintf(m,sizeof(m),"R58C2 F5_OBJECT_COVERAGE session=%llu episode=%llu target=%u wuid=%016llX eventMask=%llX handSamples=%llu bodySamples=%llu body=%p constraintResultRaw=%d constraintIdObserved=%d pickup=%s attach=%s hold=%s release=%s bodyEvidence=%s constraintEvidence=%s objectWorldPoseSamples=%llu objectAbsentSamples=%llu dropInventoryDelta=UNOBSERVED cleanup=UNPROVEN",
            (unsigned long long)p.session,(unsigned long long)p.id,p.target,(unsigned long long)p.key,(unsigned long long)p.eventMask58c2,
            (unsigned long long)p.handSamples58c2,(unsigned long long)p.bodySamples58c2,reinterpret_cast<void*>(p.observedBody58c2),p.lastConstraintResult58c2,p.lastConstraintId58c2,
            p.accepted?"ACCEPTED":"NOT_OBSERVED",p.attached?"ACCEPTED":"NOT_OBSERVED",p.handVisits?"OBSERVED":"NOT_OBSERVED",p.released?"OBSERVED":"NOT_OBSERVED",
            p.observedBody58c2?"POINTER_ONLY":"NOT_OBSERVED",p.constraintReturn?"RAW_RETURN_ONLY":"NOT_OBSERVED",(unsigned long long)p.objectPoses59c1,(unsigned long long)p.objectAbsent59c1);
        diagnostic_trace::Write(m);
    }
    char m[880]{};std::snprintf(m,sizeof(m),"V66.178R36C3 F8_LIFECYCLE_END session=%llu reason=%s dropped=%llu ledgerOverflow=%llu identityConflicts=%llu pickupCandidates=%llu pickupHenry=%llu alchemyCandidates=%llu alchemyHenry=%llu scopeLost=%llu watchLost=%llu budgetLost=%llu constraintRemovalCoverage=PARSED_ID_AND_NATIVE_ACTION_RETURN nativeCleanup=NOT_IMPLIED_BY_CAPTURE_END",
        (unsigned long long)g_lifecycle_session165.load(),reason,(unsigned long long)g_lifecycle_queue165.dropped.exchange(0),(unsigned long long)g_lifecycle_ledger165.overflow,(unsigned long long)g_lifecycle_ledger165.conflicts,
        (unsigned long long)g_lifecycle_pickup_candidates165.exchange(0),(unsigned long long)g_lifecycle_pickup_henry165.exchange(0),
        (unsigned long long)g_lifecycle_alchemy_candidates165.exchange(0),(unsigned long long)g_lifecycle_alchemy_henry165.exchange(0),(unsigned long long)g_lifecycle_scope_lost36.exchange(0),(unsigned long long)g_lifecycle_watch165.lost.exchange(0),(unsigned long long)g_lifecycle_budget_lost36.exchange(0));diagnostic_trace::Write(m);
}
