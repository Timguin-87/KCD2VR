// R103: held prop / dragged corpse do not collide with Henry while held (self_collision_r103.h).
// Native write = the retail pe_params_collision_class (type 26) through the body's own
// SetParams slot +0x20, the same 28-byte packet the hand proxies (native_hand_proxy_owner60)
// and weapon bodies (native_weapon_body_t4) already send. Pinned before every use:
//  CRigidEntity vtable 3A48DC0 slot +0x20 = 6F1BE4, which calls CPhysicalEntity::SetParams
//   84E3EC at 6F1CD4; CArticulatedEntity vtable 3A48B30 slot +0x20 = 6F2FE8, which calls
//   6F1BE4 at 6F319B; the type-26 branch at 850AA2 (cmp [r14],1Ah ... and [rdi+0ECh],eax).
// Read back: +0xEC type, +0xF0 ignore (same offsets the hand proxy verifies).
// bThreadSafe=0 like the velocity action: the engine queues the write if the physics
// step is running, so the read back can lag one frame; the lease re-applies at 250 ms.
// Game thread only (static grab tick, corpse services). No on-screen text.
namespace self_collision_r103n {
namespace sc=kcd2vr::self_collision_r103;
static sc::Table g_table;
static std::atomic_flag g_busy=ATOMIC_FLAG_INIT;
struct Lock {bool owned=false;Lock() noexcept {for(int i=0;i<64&&!owned;++i)owned=!g_busy.test_and_set(std::memory_order_acquire);}
    ~Lock(){if(owned)g_busy.clear(std::memory_order_release);}};
struct ClassPacket {int type=26;std::uint32_t orType=0,orIgnore=0,orThird=0,andType=~0u,andIgnore=~0u,andThird=~0u;};
static_assert(sizeof(ClassPacket)==28);
static bool Pins() noexcept {
    static constexpr StageLeafPin596 pins[]{
        {0x850AA2,{0x41,0x83,0x3E,0x1A,0x75,0x53,0x41,0x8B}},{0x850AAA,{0x46,0x10,0x21,0x87,0xEC,0x00,0x00,0x00}},
        {0x6F1CD4,{0xE8,0x13,0xC7,0x15,0x00,0x4C,0x63,0xF8}},{0x6F319B,{0xE8,0x44,0xEA,0xFF,0xFF,0x85,0xC0,0x0F}},
        {0x6F1BE4,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x55}},{0x6F2FE8,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x55}}};
    return StageLeafPins596(pins);
}
// exact rigid or articulated body with its pinned SetParams slot
static bool Setter(std::uintptr_t body,sc::Kind kind,std::uintptr_t& fn) noexcept {
    fn=0;std::uintptr_t vf=0,slot=0;const auto b=WhBase();
    if(!body||!Pins()||!Read(reinterpret_cast<void*>(body),vf))return false;
    const std::uintptr_t want=kind==sc::Kind::Corpse?b+0x3A48B30:b+0x3A48DC0;
    const std::uintptr_t setter=kind==sc::Kind::Corpse?b+0x6F2FE8:b+0x6F1BE4;
    if(vf!=want||!Read(reinterpret_cast<void*>(vf+0x20),slot)||slot!=setter)return false;
    fn=slot;return true;
}
static bool ReadClass(std::uintptr_t body,std::uint32_t& type,std::uint32_t& ignore) noexcept {
    return Read(reinterpret_cast<void*>(body+0xEC),type)&&Read(reinterpret_cast<void*>(body+0xF0),ignore);
}
static bool Send(std::uintptr_t fn,std::uintptr_t body,const ClassPacket& p) noexcept {
    __try {return reinterpret_cast<int(__fastcall*)(void*,const void*,int)>(fn)(reinterpret_cast<void*>(body),&p,0)>0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void Row(const char* phase,const sc::Lease& l,const char* why,std::uint32_t type,std::uint32_t ignore) noexcept {
    char row[320]{};std::snprintf(row,sizeof(row),
        "R103 SELF_COLLISION phase=%s kind=%s id=%u body=%p part=%d owners=0x%X addedType=0x%X addedIgnore=0x%X classType=0x%X classIgnore=0x%X reason=%s",
        phase,l.kind==sc::Kind::Corpse?"CORPSE":"PROP",l.id,reinterpret_cast<void*>(l.body),l.part,l.owners,l.added.type,l.added.ignore,type,ignore,why);
    diagnostic_trace::Write(row);
}
// The body is still exactly this entity's current body (no stale pointer is written).
static bool Current(const sc::Lease& l,ph::Vec3* position) noexcept {
    if(l.kind==sc::Kind::Prop){
        void* e=EntityById6694(l.id);
        if(!e||reinterpret_cast<std::uintptr_t>(e)!=l.entity||reinterpret_cast<std::uintptr_t>(PropBody596(e))!=l.body)return false;
        if(position){Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(e,l.id,tm))return false;*position={tm.m[3],tm.m[7],tm.m[11]};}
        return true;
    }
    if(l.kind==sc::Kind::Corpse){
        // same proof ImpaleRoot47 uses: the articulated root's foreign data is this entity (+20, type 1 or 2 at +28)
        void* e=FlinchEntityById178r11(l.id);std::uintptr_t vf=0,foreign=0;std::int16_t foreignType=0;
        if(!e||reinterpret_cast<std::uintptr_t>(e)!=l.entity||!Read(reinterpret_cast<void*>(l.body),vf)||vf!=WhBase()+0x3A48B30||
           !Read(reinterpret_cast<void*>(l.body+0x20),foreign)||!Read(reinterpret_cast<void*>(l.body+0x28),foreignType)||
           !kcd2vr::impale47::foreignIsEntity(foreignType,foreign,l.entity,true))return false;
        if(position){Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(e,l.id,tm))return false;*position={tm.m[3],tm.m[7],tm.m[11]};}
        return true;
    }
    return false;
}
// Hold start / every drive frame (idempotent). Adds only the missing bits.
static void Exclude(unsigned owner,sc::Kind kind,std::uint32_t id,std::uintptr_t entity,std::uintptr_t body,int part,std::uint64_t now) noexcept {
    if(!id||!entity||!body)return;
    Lock lock;if(!lock.owned)return; // next drive frame asks again
    auto* found=g_table.byBody(body);
    if(found&&(found->owners&owner)&&!found->released&&now>=found->applyTick&&now-found->applyTick<sc::kReapplyMs)return;
    std::uintptr_t fn=0;std::uint32_t type=0,ignore=0;
    sc::Lease probe{};probe.kind=kind;probe.id=id;probe.body=body;probe.part=part;probe.owners=owner;
    if(!Setter(body,kind,fn)||!ReadClass(body,type,ignore)){
        if(!found){static std::uint64_t told=0;if(!told||now<told||now-told>=2000){told=now;Row("REFUSED",probe,"BODY_OR_SETPARAMS_PIN_MISMATCH",type,ignore);}}
        return;
    }
    auto* l=g_table.take(kind,id,entity,body,part,owner);
    if(!l){Row("REFUSED",probe,"TABLE_FULL_HOLD_RUNS_WITHOUT_EXCLUSION",type,ignore);return;}
    l->applyTick=now?now:1;
    const auto add=sc::missing(kind,type,ignore);
    if(!add.any()){if(!found)Row("EXCLUDE",*l,"ALREADY_EXCLUDED",type,ignore);return;}
    ClassPacket p{};p.orType=add.type;p.orIgnore=add.ignore;
    const bool sent=Send(fn,body,p);
    if(sent){l->added.type|=add.type;l->added.ignore|=add.ignore;}
    std::uint32_t t2=0,i2=0;(void)ReadClass(body,t2,i2);
    if(!found||!sent)Row(sent?"EXCLUDE":"REFUSED",*l,sent?((i2&add.ignore)==add.ignore?"APPLIED":"QUEUED_BY_PHYSICS_STEP"):"SETPARAMS_REFUSED",t2,i2);
    if(!sent&&!l->added.any()){l->used=false;} // nothing of ours on the body: no lease
}
static void Release(unsigned owner,std::uint64_t now) noexcept {
    Lock lock;if(!lock.owned){
        // never lose a release: a busy table is only possible on another thread; take it the hard way
        while(g_busy.test_and_set(std::memory_order_acquire)){} lock.owned=true;}
    (void)g_table.release(owner,now);
}
static void Restore(sc::Lease& l,const char* why) noexcept {
    std::uintptr_t fn=0;std::uint32_t type=0,ignore=0;
    if(l.added.any()&&Current(l,nullptr)&&Setter(l.body,l.kind,fn)){
        ClassPacket p{};p.andType=~l.added.type;p.andIgnore=~l.added.ignore;
        const bool sent=Send(fn,l.body,p);(void)ReadClass(l.body,type,ignore);
        Row(sent?"RESTORE":"RESTORE_REFUSED",l,why,type,ignore);
    }else Row("RESTORE_SKIPPED",l,l.added.any()?"BODY_GONE_OR_REPLACED":"NOTHING_ADDED",0,0);
    l={};
}
// Game-thread tick: restore released leases when the object is clear of Henry, at the latest after kMaxMs.
static void Tick(std::uint64_t now) noexcept {
    Lock lock;if(!lock.owned)return;
    if(!g_table.count())return;
    CommittedFrame6695 f{};const bool frame=ReadCommittedPacket6695(f);
    const ph::Vec3 henry{f.world[4],f.world[5],f.world[6]};
    for(auto& l:g_table.e){
        if(!l.used||l.owners)continue;
        ph::Vec3 at{};const bool live=Current(l,&at);
        if(!live){Restore(l,"BODY_GONE_OR_REPLACED");continue;}
        const double dx=at.x-henry.x,dy=at.y-henry.y,horizontal=std::sqrt(dx*dx+dy*dy);
        if(sc::due(l,now,frame&&std::isfinite(henry.x),horizontal)==sc::Due::Restore)
            Restore(l,now-l.released>=sc::kMaxMs?"TIME_LIMIT":"CLEAR_OF_HENRY");
    }
}
} // namespace self_collision_r103n
