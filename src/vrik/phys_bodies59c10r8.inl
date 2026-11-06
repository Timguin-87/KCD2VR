// R59.16C10R8 PHYSICS BODIES - native adapter (included by frik_pose_probe.cpp inside kcd2vr::frik_pose_probe).
// Mod-thread only: TickPhysBodies59c10r8() runs once per main-loop iteration. Every native leaf is pinned to the exact
// WHGame 6A350E20 slot values below, SEH-guarded, and a fault latches the feature OFF for the session.
// Live changes use the normal native queue (bThreadSafe=0). Value 1 asserts physics-thread ownership and bypasses it.
namespace pb59c10r8 {
namespace pb=kcd2vr::phys_bodies59c10r8;
// IPhysicalWorld (vtable +407FC80; RWI +0x118 = +50A3E8 already pinned by the ray sweeps)
inline constexpr std::uintptr_t kWorldVtable=0x407FC80,kCreateSlot=0x88,kCreateRva=0x8463B8,kDestroySlot=0x98,kDestroyRva=0x846E10,
    kGetIdSlot=0xA8,kGetIdRva=0x52950C,kGetByIdSlot=0xB0,kGetByIdRva=0x667418,kGeomanSlot=0x20,kGeomanRva=0x84BF50;
// IGeomManager (world+0x10, vtable +407FFC0 written by the world ctor at +99AC98)
inline constexpr std::uintptr_t kGeomanVtable=0x407FFC0,kPrimSlot=0x28,kPrimRva=0x8102E0,kRegSlot=0x38,kRegRva=0x8100EC,kUnregSlot=0x48,kUnregRva=0x84A954;
// CRigidEntity (PE_RIGID) vtable +3A48DC0: GetType(+8) returns 2, SetParams +20, GetStatus +30 (8A6C90), Action +38 (AC0D18), AddGeometry +40 (526194)
inline constexpr std::uintptr_t kRigidVtable=0x3A48DC0,kSetParamsRva=0x6F1BE4,kGetStatusRva=0x8A6C90,kActionRva=0xAC0D18,kAddGeomRva=0x526194,kForeignRva=0x5D6F44,kIForeignRva=0x82BED0;

using CreateFn=void*(*)(void*,int,void*,void*,int,int,void*);
using DestroyFn=int(*)(void*,void*,int,int);
using GetIdFn=int(*)(void*,void*);
using GetByIdFn=void*(*)(void*,int);
using PrimFn=void*(*)(void*,int,const void*);
using RegFn=void*(*)(void*,void*,int,int*,int);
using UnregFn=int(*)(void*,void*);
using GeomReleaseFn=int(*)(void*);
using SetParamsFn=int(*)(void*,void*,int);
using GetStatusFn=int(*)(void*,void*);
using ActionFn=int(*)(void*,void*,int);
using AddGeomFn=int(*)(void*,void*,void*,int,int);
using ForeignFn=void*(*)(void*,int);
using IForeignFn=int(*)(void*);

struct Body {void* ent=nullptr;void* world=nullptr;int id=-1;pb::Kind kind=pb::Kind::Blade;float halfHeight=0,radius=0,mass=0;int mode=0;
    bool destroyRequested=false;
    std::uint64_t created=0,lastContactPoll=0;unsigned teleports=0,parks=0,velocities=0;float maxError=0;};
struct State {int binding=0;bool latched=false;void* world=nullptr;void* geoman=nullptr;Body bodies[3]{};
    std::uint64_t lastStatusLog=0,lastClearanceLog=0;unsigned contacts=0,creates=0,destroys=0,lost=0;};
static State g_state;
static std::atomic<int> g_mode59c10r8{0};
static std::atomic<float> g_hand_mass{3.f},g_blade_mass{4.f};
static std::atomic<bool> g_retiredT4{true};
static int g_cookie=0; // address = foreign-data identity of our bodies

template<class F> static bool Slot(std::uintptr_t vt,std::uintptr_t slot,std::uintptr_t base,std::uintptr_t rva,F& fn) noexcept {
    std::uintptr_t p=0;if(!Read(reinterpret_cast<void*>(vt+slot),p)||p!=base+rva)return false;fn=reinterpret_cast<F>(p);return true;}
static void Log(const char* m) noexcept {diagnostic_trace::Write(m);}

struct Api {CreateFn create=nullptr;DestroyFn destroy=nullptr;GetIdFn getId=nullptr;GetByIdFn getById=nullptr;PrimFn prim=nullptr;RegFn reg=nullptr;UnregFn unreg=nullptr;};
static Api g_api{};

// Binding: re-read every tick (cheap) because the world pointer can change on level load; slot contents are pinned.
static bool Bind(std::uintptr_t base,bool cleanup=false) noexcept {
    if((g_state.latched&&!cleanup)||!base||!SupportedWhBuild(base))return false;
    void* world=nullptr;std::uintptr_t vt=0;
    if(!Read(reinterpret_cast<void*>(base+wc::kWorldSlotRva),world)||!world||!Read(world,vt)||vt!=base+kWorldVtable)return false;
    Api a{};std::uintptr_t gvt=0;void* geoman=static_cast<unsigned char*>(world)+0x10;
    std::uintptr_t gm=0;
    const bool ok=Slot(vt,kCreateSlot,base,kCreateRva,a.create)&&Slot(vt,kDestroySlot,base,kDestroyRva,a.destroy)&&
        Slot(vt,kGetIdSlot,base,kGetIdRva,a.getId)&&Slot(vt,kGetByIdSlot,base,kGetByIdRva,a.getById)&&
        Read(reinterpret_cast<void*>(vt+kGeomanSlot),gm)&&gm==base+kGeomanRva&&
        Read(geoman,gvt)&&gvt==base+kGeomanVtable&&Slot(gvt,kPrimSlot,base,kPrimRva,a.prim)&&Slot(gvt,kRegSlot,base,kRegRva,a.reg)&&
        Slot(gvt,kUnregSlot,base,kUnregRva,a.unreg);
    const int state=ok?1:-1;
    if(g_state.binding!=state){g_state.binding=state;char m[300]{};std::snprintf(m,sizeof(m),
        "R59.16C10R8 PHYS_BODIES BINDING %s world=+%llX create=+%llX destroy=+%llX geoman=+%llX prim=+%llX register=+%llX",
        ok?"CONTRACT_MATCH":"BLOCKED_CONTRACT",(unsigned long long)kWorldVtable,(unsigned long long)kCreateRva,(unsigned long long)kDestroyRva,
        (unsigned long long)kGeomanVtable,(unsigned long long)kPrimRva,(unsigned long long)kRegRva);Log(m);}
    if(!ok)return false;
    g_api=a;g_state.world=world;g_state.geoman=geoman;return true;
}
// ---- SEH leaves (no C++ objects with destructors inside __try)
static void* CallCreate(void* world,void* params) noexcept {__try{return g_api.create(world,pb::kPeRigid,params,&g_cookie,pb::kForeignMagic,-1,nullptr);}__except(EXCEPTION_EXECUTE_HANDLER){return reinterpret_cast<void*>(-1);}}
static int CallDestroy(void* world,void* ent) noexcept {__try{return g_api.destroy(world,ent,0,0);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static int CallGetId(void* world,void* ent) noexcept {__try{return g_api.getId(world,ent);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static void* CallGetById(void* world,int id) noexcept {__try{return g_api.getById(world,id);}__except(EXCEPTION_EXECUTE_HANDLER){return reinterpret_cast<void*>(-1);}}
static void* CallPrim(void* geoman,int type,const void* prim) noexcept {__try{return g_api.prim(geoman,type,prim);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}}
static void* CallReg(void* geoman,void* geom) noexcept {__try{return g_api.reg(geoman,geom,0,nullptr,0);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}}
static int CallUnreg(void* geoman,void* pg) noexcept {__try{return g_api.unreg(geoman,pg);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static int CallGeomRelease(void* geom) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(geom);return reinterpret_cast<GeomReleaseFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x18))(geom);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static bool RigidVtable(void* ent,std::uintptr_t base) noexcept {std::uintptr_t vt=0;return ent&&Read(ent,vt)&&vt==base+kRigidVtable;}
static int CallSetParams(void* ent,void* p,int ts) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<SetParamsFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x20))(ent,p,ts);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static int CallGetStatus(void* ent,void* s) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<GetStatusFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x30))(ent,s);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static int CallAction(void* ent,void* a,int ts) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<ActionFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x38))(ent,a,ts);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static int CallAddGeom(void* ent,void* pg,void* gp) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<AddGeomFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x40))(ent,pg,gp,-1,0);}__except(EXCEPTION_EXECUTE_HANDLER){return -999;}}
static void* CallForeign(void* ent) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<ForeignFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x50))(ent,0);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}}
static int CallIForeign(void* ent) noexcept {__try{std::uintptr_t vt=*reinterpret_cast<std::uintptr_t*>(ent);return reinterpret_cast<IForeignFn>(*reinterpret_cast<std::uintptr_t*>(vt+0x58))(ent);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}}
// Full slot check before the first entity call (the per-call leaves read the same slots).
static bool RigidSlots(void* ent,std::uintptr_t base) noexcept {
    std::uintptr_t vt=0,s=0;if(!RigidVtable(ent,base)||!Read(ent,vt))return false;
    const std::pair<std::uintptr_t,std::uintptr_t> want[]{{0x20,kSetParamsRva},{0x30,kGetStatusRva},{0x38,kActionRva},{0x40,kAddGeomRva},{0x50,kForeignRva},{0x58,kIForeignRva}};
    for(const auto& w:want)if(!Read(reinterpret_cast<void*>(vt+w.first),s)||s!=base+w.second)return false;return true;
}
static void Latch(const char* why) noexcept {if(g_state.latched)return;g_state.latched=true;char m[200]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODIES LATCHED_OFF reason=%s (feature off until restart)",why);Log(m);}

static pb::ParamsPos PosParams(pb::V3f p,pb::Qf q) noexcept {
    pb::ParamsPos pp{};pp.type=0;pp.pos=p;pp.q=q;pp.scale=pb::unusedFloat();pp.pMtx3x4=nullptr;pp.pMtx3x3=nullptr;pp.iSimClass=pb::kUnusedI;pp.bRecalcBounds=1;pp.bEntGridUseOBB=false;return pp;}
static bool ReadPos(void* ent,pb::V3f& p,pb::Qf& q) noexcept {pb::StatusPos s{};if(CallGetStatus(ent,&s)<=0)return false;p=s.pos;q=s.q;return pb::finite(p)&&pb::finite(q);}

static void DestroyBody(Body& b,const char* why) noexcept {
    // OFF/binding-loss used to skip StillOurs and call through cached world/body
    // pointers. Cleanup needs the same current retail world and registry identity.
    const auto base=WhBase();
    if(b.ent&&b.destroyRequested){
        // Unknown binding/registry is not observed retirement. Keep the opaque
        // pending slot, blocking a replacement, until a qualified lookup exists.
        if(!Bind(base,true)||b.id<0)return;
        const bool current=b.world==g_state.world;
        // Once native destroy owns the request, only consult the registry.
        // It may delete the body between this lookup and a would-be vft read.
        if(current){const auto live=CallGetById(g_state.world,b.id);
            if(live==reinterpret_cast<void*>(-1)||live==b.ent)return;}
        char m[180]{};std::snprintf(m,sizeof(m),"C19C4R3 PHYS_BODIES cleanup=%s body=%s id=%d reason=%s",
            current?"REQUEST_RETIRED_FROM_REGISTRY":"IDENTITY_LOST_NOT_TOUCHED",pb::Name(b.kind),b.id,why);Log(m);
        const auto kind=b.kind;b=Body{};b.kind=kind;return;
    }
    const bool ours=b.ent&&Bind(base,true)&&b.world==g_state.world&&b.id>=0&&
        CallGetById(g_state.world,b.id)==b.ent&&RigidSlots(b.ent,base)&&
        CallForeign(b.ent)==&g_cookie&&CallIForeign(b.ent)==pb::kForeignMagic;
    if(ours){
        b.destroyRequested=true;
        const int r=CallDestroy(g_state.world,b.ent);
        if(r==-999)Latch("DESTROY_FAULT");
        if(r>0)++g_state.destroys;
        char m[260]{};std::snprintf(m,sizeof(m),"C19C4R3 PHYS_BODIES cleanup=%s body=%s id=%d reason=%s nativeReturn=%d completion=UNOBSERVED",
            r>0?"DESTROY_REQUESTED":r==-999?"DESTROY_FAULT_RETAINED":"DESTROY_REFUSED_RETAINED",pb::Name(b.kind),b.id,why,r);Log(m);
        // Native busy-queue completion is asynchronous. Retain the slot until
        // the CURRENT world registry stops owning this exact body; no readback
        // of the retired pointer and no replacement creation while pending.
        return;
    }
    if(b.ent){++g_state.lost;
        char m[180]{};std::snprintf(m,sizeof(m),"C19C4R3 PHYS_BODIES cleanup=%s body=%s id=%d reason=%s",
            "IDENTITY_LOST_NOT_TOUCHED",pb::Name(b.kind),b.id,why);Log(m);}
    const auto kind=b.kind;b=Body{};b.kind=kind;
}
static bool CreateBody(Body& b,std::uintptr_t base,int mode,pb::V3f p,pb::Qf q,float halfHeight,float radius,float mass) noexcept {
    const float norm=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    if(mode<1||mode>2||!pb::finite(p)||!pb::finite(q)||!std::isfinite(norm)||std::fabs(norm-1.f)>.01f||
       !std::isfinite(halfHeight)||halfHeight<0||halfHeight>1.5f||!std::isfinite(radius)||radius<=0||radius>.5f||
       !std::isfinite(mass)||mass<.2f||mass>50.f)return false;
    // The optional proxy has no verified full-volume spawn-clearance query.
    // A line ray cannot prove that a sphere/capsule is outside solid geometry.
    // Keep FOLLOW useful; refuse NEW COLLIDE bodies instead of creating first
    // and parking an overlapping body afterward. Main VR sweeps are unchanged.
    if(mode>=2){const auto now=std::uint64_t(GetTickCount64());
        if(!g_state.lastClearanceLog||now<g_state.lastClearanceLog||now-g_state.lastClearanceLog>=5000){
            g_state.lastClearanceLog=now;Log("C19C2 PHYS_BODIES spawn=REFUSED reason=CLEARANCE_UNAVAILABLE scope=OPTIONAL_COLLIDE_PROXY mainWeaponQueries=UNCHANGED");}
        return false;
    }
    auto pp=PosParams(p,q);
    void* ent=CallCreate(g_state.world,&pp);
    if(ent==reinterpret_cast<void*>(-1)){Latch("CREATE_FAULT");return false;}
    if(!ent)return false;
    if(!RigidSlots(ent,base)){(void)CallDestroy(g_state.world,ent);Latch("CREATED_BODY_NOT_CRIGIDENTITY");return false;}
    // geometry: one primitive, registered, added as part 0, then our references released (the body keeps its own)
    void* geom=nullptr;
    if(b.kind==pb::Kind::Blade){pb::PrimCapsule c{};c.center={0,0,0};c.axis={0,1,0};c.r=radius;c.hh=halfHeight;geom=CallPrim(g_state.geoman,pb::kPrimCapsule,&c);}
    else{pb::PrimSphere s{};s.center={0,0,0};s.r=radius;geom=CallPrim(g_state.geoman,pb::kPrimSphere,&s);}
    void* pg=geom?CallReg(g_state.geoman,geom):nullptr;
    if(geom)(void)CallGeomRelease(geom);
    if(!pg){(void)CallDestroy(g_state.world,ent);Latch("GEOMETRY_FAULT");return false;}
    pb::GeomParams gp{};gp.type=0;gp.mass=mass;gp.density=0;gp.q={};gp.scale=1;gp.minContactDist=pb::unusedFloat();
    gp.flags=mode>=2?pb::kGeomColltype0:0u;gp.flagsCollider=mode>=2?pb::kGeomColltype0:0u; // FOLLOW: collides with nothing; never geom_colltype_ray
    const int part=CallAddGeom(ent,pg,&gp);(void)CallUnreg(g_state.geoman,pg);
    if(part<0){(void)CallDestroy(g_state.world,ent);Latch(part==-999?"ADDGEOMETRY_FAULT":"ADDGEOMETRY_REJECTED");return false;}
    pb::ParamsSimulation sp{};sp.type=10;sp.iSimClass=pb::kUnusedI;sp.maxTimeStep=pb::unusedFloat();sp.minEnergy=0;sp.damping=0;sp.gravity={0,0,0};
    sp.dampingFreefall=0;sp.gravityFreefall={0,0,0};sp.maxRotVel=pb::unusedFloat();sp.mass=pb::unusedFloat();sp.density=pb::unusedFloat();
    sp.maxLoggedCollisions=mode>=2?4:0;sp.disablePreCG=pb::kUnusedI;sp.maxFriction=pb::unusedFloat();sp.collTypes=pb::kUnusedI;
    pb::ParamsFlags fp{};fp.type=15;fp.flags=1u<<31; // MARK_UNUSED unsigned int
    fp.flagsAND=0xFFFFFFFFu; // preserve native flags; this packet only adds flagsOR
    fp.flagsOR=pb::kPefNeverAffectTriggers|pb::kPefIgnoreAreas|(mode>=2?pb::kPefLogCollisions:0u);
    pb::ParamsCollisionClass cc{};cc.type=26;cc.orBits={pb::kCollClassGame,pb::kCollClassLiving}; // never touch Henry / living NPCs
    if(CallSetParams(ent,&sp,0)<=0||CallSetParams(ent,&fp,0)<=0||CallSetParams(ent,&cc,0)<=0){(void)CallDestroy(g_state.world,ent);Latch("SETPARAMS_REJECTED");return false;}
    // read-back: the body must report the pose we created it at (proves pe_params_pos + identity)
    pb::V3f rp{};pb::Qf rq{};const bool back=ReadPos(ent,rp,rq);
    if(!back||pb::len(pb::sub(rp,p))>.05f||CallForeign(ent)!=&g_cookie||CallIForeign(ent)!=pb::kForeignMagic){
        (void)CallDestroy(g_state.world,ent);Latch("READBACK_MISMATCH");return false;}
    b.ent=ent;b.world=g_state.world;b.id=CallGetId(g_state.world,ent);b.halfHeight=halfHeight;b.radius=radius;b.mass=mass;b.mode=mode;b.created=GetTickCount64();
    ++g_state.creates;char m[260]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODIES CREATED body=%s id=%d mode=%s mass=%.1f radius=%.3f halfHeight=%.3f part=%d readbackErrorM=%.4f",
        pb::Name(b.kind),b.id,mode>=2?"COLLIDE":"FOLLOW",double(mass),double(radius),double(halfHeight),part,double(pb::len(pb::sub(rp,p))));Log(m);
    return true;
}
// The body may have been destroyed by a level change: never call into a pointer the world no longer owns.
static bool StillOurs(Body& b,std::uintptr_t base) noexcept {
    if(!b.ent)return false;void* live=b.world==g_state.world?CallGetById(g_state.world,b.id):nullptr;
    if(live!=b.ent||!RigidVtable(b.ent,base)||CallForeign(b.ent)!=&g_cookie||CallIForeign(b.ent)!=pb::kForeignMagic){
        ++g_state.lost;char m[160]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODIES LOST body=%s id=%d (world reset or replaced; not touched)",pb::Name(b.kind),b.id);Log(m);
        const auto kind=b.kind;b=Body{};b.kind=kind;return false;}
    return true;
}
static void Drive(Body& b,const pb::Target& t,std::uint64_t now) noexcept {
    if(b.destroyRequested)return;
    pb::V3f p{};pb::Qf q{};const bool have=ReadPos(b.ent,p,q);
    auto c=pb::step(t,p,q,have,now,pb::Tuning{},b.kind==pb::Kind::Blade);
    // FOLLOW has no contact geometry and may reseed its diagnostic proxy.
    // A live COLLIDE body must not jump through/into geometry when it falls
    // behind, or when status is unreadable. No clearance/reset contract exists:
    // park it at its achieved location until ordinary bounded drive is possible.
    if(b.mode>=2&&c.op==pb::Command::Op::Teleport)c.op=pb::Command::Op::Park;
    if(c.op==pb::Command::Op::Velocity){pb::ActionSetVelocity a{};a.ipart=pb::kUnusedI;a.partid=pb::kUnusedI;a.v=c.v;a.w=c.w;a.bRotationAroundPivot=1;
        if(CallAction(b.ent,&a,0)==-999){Latch("ACTION_FAULT");return;}++b.velocities;b.maxError=std::max(b.maxError,c.error);}
    else if(c.op==pb::Command::Op::Teleport){auto pp=PosParams(c.p,c.q);if(CallSetParams(b.ent,&pp,0)==-999){Latch("TELEPORT_FAULT");return;}
        pb::ActionSetVelocity a{};a.ipart=pb::kUnusedI;a.partid=pb::kUnusedI;(void)CallAction(b.ent,&a,0);++b.teleports;}
    else if(c.op==pb::Command::Op::Park){pb::ActionSetVelocity a{};a.ipart=pb::kUnusedI;a.partid=pb::kUnusedI;
        if(CallAction(b.ent,&a,0)==-999){Latch("PARK_FAULT");return;}++b.parks;}
}
static void PollContacts(Body& b,std::uint64_t now) noexcept {
    if(b.mode<2||now-b.lastContactPoll<100)return;b.lastContactPoll=now;
    pb::CollHistoryItem items[4]{};pb::StatusCollisions s{};s.pHistory=items;s.len=4;s.age=.25f;s.bClearHistory=1;
    const int n=CallGetStatus(b.ent,&s);if(n<=0)return;
    for(int i=0;i<n&&i<4;++i){++g_state.contacts;const auto& h=items[i];
        const float rel=pb::len(pb::sub(h.v[0],h.v[1]));
        char m[240]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODY_CONTACT body=%s colliderId=%d colliderMass=%.1f relSpeed=%.2f point=(%.2f,%.2f,%.2f) mat=%d",
            pb::Name(b.kind),h.idCollider,double(h.mass[1]),double(rel),double(h.pt.x),double(h.pt.y),double(h.pt.z),h.idmat[1]);Log(m);}
}
// ---- targets from the existing tracked state
struct BladeOwner19c2 {std::uint64_t request=0,epoch=0,sample=0,key=0,generation=0;void* player=nullptr;void* manager=nullptr;std::uint32_t entity=0;};
static bool TargetTime19c2(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
static bool TargetReady19c2(const pb::Target& t,std::uint64_t now) noexcept {
    const float norm=t.q.x*t.q.x+t.q.y*t.q.y+t.q.z*t.q.z+t.q.w*t.q.w;
    return t.valid&&TargetTime19c2(t.tick,now)&&pb::finite(t.p)&&pb::finite(t.q)&&std::isfinite(norm)&&std::fabs(norm-1.f)<=.01f;
}
static bool Gameplay19c2() noexcept {
    return g_gameplay_allowed.load(std::memory_order_acquire)&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&!animation_filter::nativeBodyOwned112.load()&&
        !g_phys_fault6698.load()&&!g_weapon_fault6694.load()&&TrackingHeadUsable111();
}
static bool BladeCurrent19c2(const BladeOwner19c2& o,std::uint64_t now) noexcept {
    if(!Gameplay19c2()||!TrackingHandUsable111(0,now)||!o.player||!o.manager||!o.key||!o.generation||!o.entity||!o.sample||
       o.sample!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       o.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       o.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||o.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire))return false;
    PhysEquipment6698 e{};
    if(!g_phys_equipment6698.read(e)||!e.valid||!TargetTime19c2(e.tick,now)||e.player!=o.player||e.manager!=o.manager||
       e.request!=o.request||e.epoch!=o.epoch||e.count<1||e.count>8||!e.handKnown[0]||e.handKey[0]!=o.key||
       e.heldIndex[0]<0||e.heldIndex[0]>=e.count)return false;
    const auto& held=e.items[e.heldIndex[0]];
    return held.held&&held.profile.key==o.key&&held.profile.generation==o.generation&&held.profile.entity==o.entity&&
        held.profile.side==0&&held.profile.melee()&&
        o.sample==g_phys_ready_sample6698.load(std::memory_order_acquire)&&
        o.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&o.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        o.manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&TrackingHandUsable111(0,now)&&Gameplay19c2();
}
static bool BladeTarget(pb::Target& t,float& halfHeight,BladeOwner19c2& owner,std::uint64_t now) noexcept {
    t={};halfHeight=0;owner={};PhysFrame6698 f{};
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !TargetTime19c2(f.input.tick,now)||!f.input.allowed||!f.input.tracked[0]||!f.output.active[0]||!f.output.owned[0]||
       !f.profiles[0].melee()||!f.profiles[0].primary||f.profiles[0].side!=0||!f.input.haveWorld||
       !ph::finite(f.input.world)||!ph::finite(f.output.weapons[0])||!pm::finite(f.profiles[0].tip)||
       !std::isfinite(f.input.worldScale)||f.input.worldScale<.1||f.input.worldScale>10||f.input.epoch!=f.request)return false;
    owner={f.request,f.input.bodyEpoch,f.input.sample,f.profiles[0].key,f.profiles[0].generation,f.player,f.manager,f.profiles[0].entity};
    if(!BladeCurrent19c2(owner,now))return false;
    const auto& w=f.output.weapons[0];const auto& prof=f.profiles[0];
    const ph::Vec3 a=ph::point(f.input.world,w.p*f.input.worldScale);
    const ph::Vec3 tipLocal=ph::point(w,prof.tip);
    const ph::Vec3 b=ph::point(f.input.world,tipLocal*f.input.worldScale);
    pb::V3f center{};pb::Qf q{};float hh=0;
    if(!pb::capsuleFromSegment({float(a.x),float(a.y),float(a.z)},{float(b.x),float(b.y),float(b.z)},center,q,hh))return false;
    t.valid=true;t.p=center;t.q=q;t.tick=f.input.tick;halfHeight=hh;
    if(!TargetReady19c2(t,now)||!BladeCurrent19c2(owner,now)){t={};owner={};return false;}
    return true;
}
static bool HandTarget(int side,pb::Target& t,std::uint64_t now) noexcept {
    t={};if(!Gameplay19c2())return false;
    float x=0,y=0,z=0,dx=0,dy=0,dz=0;std::uint64_t tick=0;
    if(!ReadInteractionPalm59c9(side,x,y,z,dx,dy,dz,tick))return false;
    t.valid=true;t.p={x,y,z};t.q={};t.tick=tick;
    if(!TargetReady19c2(t,now)||!Gameplay19c2()){t={};return false;}
    return true;
}
} // namespace pb59c10r8

void SetPhysBodies59c10r8(int mode,float handMass,float bladeMass) noexcept {
    pb59c10r8::g_mode59c10r8.store(std::clamp(mode,0,2));
    pb59c10r8::g_hand_mass.store(std::isfinite(handMass)?std::clamp(handMass,.2f,50.f):3.f);
    pb59c10r8::g_blade_mass.store(std::isfinite(bladeMass)?std::clamp(bladeMass,.2f,50.f):4.f);
    char m[160]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODIES MODE %s handMass=%.1f bladeMass=%.1f",mode<=0?"OFF":mode==1?"FOLLOW":"COLLIDE",
        double(pb59c10r8::g_hand_mass.load()),double(pb59c10r8::g_blade_mass.load()));diagnostic_trace::Write(m);
}
void TickPhysBodies59c10r8() noexcept {
    using namespace pb59c10r8;namespace pb=kcd2vr::phys_bodies59c10r8;
    auto& st=g_state;const int mode=g_mode59c10r8.load();const std::uint64_t now=GetTickCount64();
    g_retiredT4.store(false,std::memory_order_release);
    struct RetiredT4 {~RetiredT4(){bool retired=true;for(const auto& b:g_state.bodies)retired=retired&&!b.ent;
        g_retiredT4.store(retired,std::memory_order_release);}} retiredT4;
    const std::uintptr_t base=WhBase();
    const bool bound=mode>0&&Bind(base);
    for(auto& b:st.bodies){
        if(!b.ent)continue;
        if(b.destroyRequested){DestroyBody(b,"PENDING_NATIVE_DESTROY");continue;}
        if(!bound||st.latched){DestroyBody(b,st.latched?"LATCHED":mode<=0?"MODE_OFF":"BINDING_LOST");continue;}
        if(!StillOurs(b,base))continue;
        if(b.mode!=mode)DestroyBody(b,"MODE_CHANGED");
    }
    if(!bound||st.latched)return;
    // A level/world replacement is not permission to spawn this tick's old
    // pose into the new world. Requalify, then require the original binding.
    void* const targetWorld=st.world;void* const targetGeoman=st.geoman;
    st.bodies[0].kind=pb::Kind::Blade;st.bodies[1].kind=pb::Kind::HandRight;st.bodies[2].kind=pb::Kind::HandLeft;
    // blade
    {auto& b=st.bodies[0];pb::Target t{};float hh=0;BladeOwner19c2 owner{};const bool have=BladeTarget(t,hh,owner,now);
     if(b.ent&&have&&std::fabs(hh-b.halfHeight)>.02f)DestroyBody(b,"WEAPON_LENGTH_CHANGED");
     const auto createNow=std::uint64_t(GetTickCount64());
     if(!b.ent&&have&&g_mode59c10r8.load()==mode&&TargetReady19c2(t,createNow)&&BladeCurrent19c2(owner,createNow)&&
        Bind(base)&&st.world==targetWorld&&st.geoman==targetGeoman)(void)CreateBody(b,base,mode,t.p,t.q,hh,.025f,g_blade_mass.load());
     if(b.ent&&!b.destroyRequested){if(!have&&now-b.created>2000)DestroyBody(b,"NO_MELEE_WEAPON");else Drive(b,t,now);}
     if(b.ent&&!b.destroyRequested)PollContacts(b,now);}
    // hands (side 0 = right, 1 = left)
    for(int side=0;side<2&&!st.latched;++side){auto& b=st.bodies[1+side];pb::Target t{};
        const auto bodyEpoch=g_pose_epoch6689.load(std::memory_order_acquire),request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
        const auto character=g_committed_character6689.load(std::memory_order_acquire);const auto entity=g_last_player_entity.load(std::memory_order_acquire);
        const bool have=HandTarget(side,t,now);
        if(!b.ent&&have&&g_mode59c10r8.load()==mode&&TargetReady19c2(t,std::uint64_t(GetTickCount64()))&&Gameplay19c2()&&character&&entity&&
           bodyEpoch==g_pose_epoch6689.load(std::memory_order_acquire)&&request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
           character==g_committed_character6689.load(std::memory_order_acquire)&&entity==g_last_player_entity.load(std::memory_order_acquire)&&
           Bind(base)&&st.world==targetWorld&&st.geoman==targetGeoman)
            (void)CreateBody(b,base,mode,t.p,t.q,0,.045f,g_hand_mass.load());
        if(b.ent&&!b.destroyRequested){Drive(b,t,now);PollContacts(b,now);}}
    if(now-st.lastStatusLog>=5000&&(st.bodies[0].ent||st.bodies[1].ent||st.bodies[2].ent)){st.lastStatusLog=now;
        char m[260]{};std::snprintf(m,sizeof(m),"R59.16C10R8 PHYS_BODIES STATUS mode=%s blade=%d handR=%d handL=%d creates=%u destroys=%u lost=%u contacts=%u bladeMaxErrorM=%.3f",
            mode==1?"FOLLOW":"COLLIDE",st.bodies[0].ent?1:0,st.bodies[1].ent?1:0,st.bodies[2].ent?1:0,st.creates,st.destroys,st.lost,st.contacts,double(st.bodies[0].maxError));
        diagnostic_trace::Write(m);}
}
bool LegacyPhysBodiesRetiredT4() noexcept {return pb59c10r8::g_retiredT4.load(std::memory_order_acquire);}
